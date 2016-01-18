#include <MapHooks.h>
#include <string>

const char* INFO_TEXT = "/**	\n"
" * Small tool to generate a C++-Header from a modified .MAP-File from IDA.\n"
" *	\n"
" * Usage: MemHeaderGen <.map-File> <outputfile-name> <namespace>\n"
" *	\n"
" * Notice: You will need to clean the file from IDA.\n"
" * RegEx is your friend.\n"
" *	\n"
" * Also, IDA doesn't properly generate segmentation-info for me. Please put it\n"
" * at the top of the file in the following format:\n"
" *	\n"
" * S000x:########\n"
" *	\n"
" * where x is the segment and # stands for the starting-address from IDAs\n"
" * program-segmentation view.\n"
" *	\n"
" * Copyright (c) 2015 Andre Taulien */\n";

int main(int argc, char** argv)
{
	if(argc == 1)
	{
		// Display help-text
		printf(INFO_TEXT);
		return 0;
	}

	if(argc > 4)
	{
		printf("Too many arguments. Re-run without arguments for help.\n");
	}

	std::string name = argv[1];
	std::string target;
	std::string ns;
	if(argc == 2) // Use second parameter
	{	
		target = argv[1];
	}else // Use same name as map
	{
		target = argv[2];
	}


	// Strip path and ext, get alias from file
	if(name.find_last_of("\\/") != std::string::npos)
		name = name.substr(name.find_last_of("\\/")+1);

	if(name.find_last_of('.') != std::string::npos)
		name = name.substr(0, name.find_last_of('.'));

	// See if the user specified a namespace. If not, use the mapfiles name
	if(argc == 4)
		ns = argv[3];
	else
		ns = name;

	// Initialize MapHooks
	if(!MapHooks::InitializeMap(argv[1], ns.c_str()))
	{
		printf("Failed to load map-file!\n");
		return -1;
	}

	if(!MapHooks::WriteCppHeader(target.c_str()))
	{
		printf("Failed to write header-file!\n");
		return -1;
	}

	MapHooks::Uninitialize();

	printf("Successfully wrote header-file: %s\n", target.c_str());

	return 0;
}