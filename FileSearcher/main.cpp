#include <iostream>

#include "FileSearcher/FileSearcher.h"

std::string getCurrentDirectory()
{
	const DWORD bufferLength = MAX_PATH;
	char currentPath[MAX_PATH];

	DWORD result = GetCurrentDirectoryA(bufferLength, currentPath);

	if (result == 0)
		return "";

	if (result > bufferLength)
	{
		return "";
	}
	return std::string(currentPath);
}

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		// If there's only one argument (the program name), show help message and exit.
		std::cout << "Usage: " << argv[0] << " [options]\n"
			<< "Options:\n"
			<< "-help : Show help message.\n"
			<< "<name of the file to be searched> <search path>(optional)\n"
			<< "-f : The results will be output to a file.\n" + getCurrentDirectory() + "\\out\\output.txt\n"
			<< "-s : Display content on the screen.\n";
	}
	else if (argc == 2 && std::string(argv[1]) == "-help")
	{
		// If the first argument is "-help", show the help message and exit.
		std::cout
			<< "First parameter = <name of the file to be searched>\n"
			<< "Second parameter(optional) = <search path>. if you do not set the second parameter, "
			<< "the search will be performed by the default path. (C:\\)\n"
			<< "-f : The results will be output to a file. " + getCurrentDirectory() + "\\out\\output.txt"
			<< "-s : Display content on the screen.\n"
			<<"Example: FindFile.exe -f \"README.md\" \"C:\\Users\\\"\n";
	}
	else if (argc == 3 && std::string(argv[1]) == "-f")
	{
		// If there are two arguments and the first argument is "-file", open the file and display its content.
		FileSearcher filesearcher(1);
		filesearcher.FileToFind(std::string(argv[2]));
	}
	else if (argc == 3 && std::string(argv[1]) == "-s")
	{
		FileSearcher filesearcher(0);

		filesearcher.FileToFind(std::string(argv[2]));
	}
	else if (argc == 4 && std::string(argv[1]) == "-s")
	{
		FileSearcher filesearcher(0);
		filesearcher.FileToFind(std::string(argv[2]), std::string(argv[3]));
	}
	else if (argc == 4 && std::string(argv[1]) == "-f")
	{
		FileSearcher filesearcher(1);
		filesearcher.FileToFind(std::string(argv[2]), std::string(argv[3]));
	}
	else
	{
		std::cout << "Invalid or unsupported arguments.\n";
	}

	system("pause");
	return 0;
}
