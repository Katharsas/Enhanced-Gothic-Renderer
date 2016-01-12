#pragma once
#include <unordered_map>
#include <cstdint>
#include <string>

#pragma comment(lib, "MapHooks.lib")

class MapHooks
{
public:
	MapHooks(void);
	~MapHooks(void);

	/**
	 * Loads the specified mapfile. Tries to use the cache-file if possible.
	 * Use the alias to make access to this specific MapFile easier. 
	 */
	static bool InitializeMap(const char* mapfile, const char* alias, bool tryCache = false);

	/**
	 * Shuts the object down and writes the cachefiles
	 */
	static void Uninitialize();

	/** 
	 * Returns the address to the given function/name 
	 */
	static void* GetAddress(const char* mapAlias, const char* fn);

	/**
	 * Writes a C++-header with the found values
	 */
	static bool WriteCppHeader(const char* file);

	/**
	 * Hooks a function of the given name. Returns a pointer to the original function.
	 */
	template<typename T>
	static T Hook(const char* mapAlias, const char* function, void* hook)
	{
		return (T)PerformHook(mapAlias, function, hook);
	}
private:

	struct MapFile
	{
		// Map of all adresses and functionnames.
		// Example: zCRnd_D3D::XD3D_AlphaBlendPoly(zCPolygon *,zCMaterial *) -> 0x0024B960 
		std::unordered_map<std::string, uint32_t> m_FunctionMap;

		// Map of all used hooks in this session. Will be added to the cache-file when the program closes.
		std::unordered_map<std::string, uint32_t> m_CachedFunctionMap;

		// Program segmentation
		std::unordered_map<std::string, unsigned int> m_ProgramSegmentation;

		// Original file this was loaded from
		std::string m_File;

		// Wheather to cache this
		bool m_CacheThis;
	};

	/**
	 * Performs the hook
	 */
	static void* PerformHook(const char* mapAlias, const char* function, void* hook);

	/**
	 * Tries to load the cache for a mapfile.
	 */
	bool LoadMapFile(const char* mapfile, const char* alias, bool isCacheFile);

	/**
	 * Writes the cached functions to a file 
	 */
	bool WriteCache(const MapFile& mapfile);

	// Map of different mapfiles, by alias. In case you want to load more than one.
	std::unordered_map<std::string, MapFile> MapFiles;
};

