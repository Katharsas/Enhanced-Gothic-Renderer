#include "stdafx.h"
#include "MapHooks.h"
#include <sstream>
#include <algorithm>
#include <functional>

#define GEN_DAEDALUS

// Windows.h, needed by detours
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "detours.h"

// Prefix to attach to cachefiles
static const char* CACHEFILE_POSTFIX = ".cache";

// Static instance of this object
static MapHooks* s_Instance = nullptr;


MapHooks::MapHooks(void)
{
}


MapHooks::~MapHooks(void)
{
	// Write cache for every map
	for each (auto var in MapFiles)
	{
		WriteCache(var.second);
	}
}

/**
 * Loads the specified mapfile. Tries to use the cache-file if possible.
 */
bool MapHooks::InitializeMap(const char* mapfile, const char* alias, bool tryCache)
{
	// Create instance if not already done
	if(!s_Instance)
		s_Instance = new MapHooks;

	if(tryCache)
	{
		// Try to load cachefile first
		if(!s_Instance->LoadMapFile((std::string(mapfile) + std::string(CACHEFILE_POSTFIX)).c_str(), alias, true))
		{
			// Didn't work, use full file
			if(!s_Instance->LoadMapFile(mapfile, alias, false))
				return false; // Couldn't even find that, abort!
		}
	}
	else
	{
		// Load full file
		if(!s_Instance->LoadMapFile(mapfile, alias, false))
			return false;
	}

	// If we have gotten so far, we have a map-object. Insert the original filename
	s_Instance->MapFiles[alias].m_File = mapfile;
	s_Instance->MapFiles[alias].m_CacheThis = tryCache;

	return true;
}

/**
* Shuts the object down and writes the cachefiles
*/
void MapHooks::Uninitialize()
{
	delete s_Instance;
}

/**
 * Tries to load a mapfile and puts it into our map.
 */
bool MapHooks::LoadMapFile(const char* mapfile, const char* alias, bool isCacheFile)
{
	FILE* f;
	fopen_s(&f, mapfile, "r");

	// Couldn't open the file?
	if(!f)
		return false;

	// Get map-object
	MapFile& mapFileObject = MapFiles[alias];

	// Need to learn std::io, duh!
	const int MAX_LINE = 512;
	char cline[MAX_LINE];
	std::string sline;

	// Start reading data.
	while(!feof(f))
	{
		// Read line
		fgets(cline, MAX_LINE - 1, f);
		sline = cline;

		// Called two times for a cachefile with only one line? TODO
		//OutputDebugStringA(("Line: " + sline + "\n").c_str());

		if(sline.find_first_of(':') == std::string::npos)
			continue; // Skip header and empty lines

		// Get section
		std::string section = sline.substr(0, sline.find_first_of(':'));

		// Strip section
		sline = sline.substr(sline.find_first_of(':') + 1);

		// Find out if this is a section-marker
		if(!section.empty())
		{
			if(section[0] == 'S')
			{
				// Strip the S
				section = section.substr(1);

				// Store the offset of the current section
				unsigned int offset = strtoul(sline.c_str(), NULL, 16);
				mapFileObject.m_ProgramSegmentation[section] = offset;

				// We're done here, continue with next line
				continue;
			}
		}

		// Strip leading spaces
		section = section.substr(section.find_first_not_of(' '));

		// Get address (chars until next space)
		std::string addressStr = sline.substr(0, sline.find_first_of(' '));
		uint32_t address = strtoul(addressStr.c_str(), NULL, 16); // Read hex-number

		// Add segmentation
		address += mapFileObject.m_ProgramSegmentation[section];

		// Strip address
		sline = sline.substr(addressStr.size());

		// Get function (rest after all the spaces)
		std::string function = sline.substr(sline.find_first_not_of(' '));

		// Strip any trailing newlines
		function = function.substr(0, function.find_last_not_of("\r\n") + 1);

		// Store in map
		if(isCacheFile)
			mapFileObject.m_CachedFunctionMap[function] = address;
		else
			mapFileObject.m_FunctionMap[function] = address;
	}

	fclose(f);

	return true;
}

/**
 * Writes the cached functions to a file
 */
bool MapHooks::WriteCache(const MapFile& mapfile)
{
	FILE* f;
	fopen_s(&f, (mapfile.m_File + CACHEFILE_POSTFIX).c_str(), "w");

	if(!f) // No permissions to write?
		return false;

	// Write everything into the cachefile
	for each (auto var in mapfile.m_CachedFunctionMap)
	{
		std::stringstream line;

		// Make a .map-file like line
		// Don't need to undo segmentation, since we will load it as a continous bunch anyways
		line << " 0000:" << std::hex << var.second << "     " << var.first << "\n";

		fputs(line.str().c_str(), f);
	}

	fclose(f);

	return true;
}

/** Returns the address to the given function/name */
void* MapHooks::GetAddress(const char* mapAlias, const char* fn)
{
	if(!s_Instance)
		return nullptr; // Not initialized

	auto& map = s_Instance->MapFiles[mapAlias];

	// Try to find it in the cache-map first
	auto it = map.m_CachedFunctionMap.find(std::string(fn));
	if(it != map.m_CachedFunctionMap.end())
	{
		// Got the value!
		return (void*)((*it).second);
	}

	// Not in cache, try to find it in main-map
	// But first, see if it's loaded
	if(map.m_FunctionMap.empty())
	{
		// Try to load it
		if(!s_Instance->LoadMapFile(map.m_File.c_str(), mapAlias, false))
			return false; // Can't load the file, that means we do not have an address
	}

	// We should now have data inside the functionmap
	it = map.m_FunctionMap.find(std::string(fn));
	if(it != map.m_FunctionMap.end())
	{
		// Got the value!
		// Add line to cache for next time
		map.m_CachedFunctionMap.insert((*it));

		return (void*)((*it).second);
	}

	// Couldn't find it
	return 0;
}

/**
 * Performs the hook
 */
void* MapHooks::PerformHook(const char* mapAlias, const char* function, void* hook)
{
	// Find address from maps
	void* addr = GetAddress(mapAlias, function);

	// Failed to find it?
	if(!addr)
		return nullptr;

	// Apply hook
	void* original = DetourFunction((PBYTE)addr, (PBYTE)hook);

	return original;
}

/**
 * Writes a C++-header with the found values
 */
bool MapHooks::WriteCppHeader(const char* file, ECodeType codeType = CT_CPP)
{
	FILE* f;
	fopen_s(&f, file, "w");

	if(!f)
		return false;

	if(codeType == CT_CPP) {
		// Write header
		fputs("#pragma once\n\n", f);
	}

	// Info
	fputs("/**** AUTOGENERATED FILE ****/\n\n", f);

	if(codeType == CT_CPP) {
		// Write main-namespace
		fputs("namespace MemoryLocations\n{\n", f);
	}

	// Write contents
	for each (auto& map in s_Instance->MapFiles)
	{
		// Make sure the whole thing is loaded
		if(map.second.m_FunctionMap.empty())
			s_Instance->LoadMapFile(map.second.m_File.c_str(), map.first.c_str(), false);

		if(codeType == CT_CPP) {
			// Write namespace using map-alias
			fputs(("\tnamespace " + map.first + "\n\t{\n").c_str(), f);
		}

		std::unordered_map<std::string, int> dblNamesMap;

		// Write values of function-map
		for each (auto loc in map.second.m_FunctionMap)
		{
			// Flatten, remove all :: and ( )
			std::string name = loc.first;

			// Remove spaces
			//name.erase(std::remove_if(name.begin(), name.end(), isspace), name.end());

			// Replace some chars with _
			std::replace(name.begin(), name.end(), ' ', '_');
			std::replace(name.begin(), name.end(), ':', '_');
			std::replace(name.begin(), name.end(), '(', '_');
			std::replace(name.begin(), name.end(), ')', '_');
			std::replace(name.begin(), name.end(), ',', '_');
			std::replace(name.begin(), name.end(), '*', 'p');
			std::replace(name.begin(), name.end(), '&', 'r');

			// Make sure operator-overloads don't look the same
			std::replace(name.begin(), name.end(), '+', '1');
			std::replace(name.begin(), name.end(), '-', '2');
			std::replace(name.begin(), name.end(), '/', '3');
			std::replace(name.begin(), name.end(), '=', '4');
			std::replace(name.begin(), name.end(), '!', '5');
			std::replace(name.begin(), name.end(), '~', 'd');

			// lambda that allows _ but no other non-alphas
			auto fn = [](char c)
			{
				if(c != '_' && !IsCharAlphaNumericA(c))
					return !IsCharAlphaA(c);

				return false;
			};

			// Remove some other non-alpha-chars
			name.erase(std::remove_if(name.begin(), name.end(), fn), name.end());

			// Remove trailing _
			name = name.substr(0, name.find_last_not_of("_") + 1);

			if(name.empty())
				continue; // Happens when the name only consists out of some _

			// Append a number to the end of the functionname to avoid double-namings
			auto it = dblNamesMap.find(name);
			if(it != dblNamesMap.end())
			{
				name += "_" + std::to_string((*it).second);
				(*it).second++;
			}
			else
			{
				dblNamesMap[name] = 1;
			}

			std::stringstream ss;
			
			// Construct constant
			if(codeType == CT_CPP) 
				ss << "\t\tstatic const void* " << name << " = (void*)0x" << std::hex << loc.second << ";\n";
			else		
				ss << "\t\const int " << name << " = " << loc.second << ";\n";	
			
			fputs(ss.str().c_str(), f);
		}

		if(codeType == CT_CPP) {
			fputs("\t}\n", f);
		}
	}

	if(codeType == CT_CPP) {
		// close main-namespace
		fputs("}\n", f);
	}

	fclose(f);

	return true;
}