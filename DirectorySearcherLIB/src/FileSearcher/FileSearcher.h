#pragma once

#include "pch.h"
#include "SimpleThreadPool.h"
#include <fstream>

class FileSearcher
{
public:
	void FileToFind(const std::string& fileName, const std::string startPath = "\\??\\C:\\");
	FileSearcher(int PrintOption);
private:
	void WriteToFile(const std::string& whatToWrite);
	void Find(const std::string& fileName, std::vector<std::pair <std::string, std::string>> files, std::string& res);
	void FillVector(const std::string& directoryPath);
	void WaitForTasks();
	void PrintResult(const std::string& res);

	const std::string output = "directory.txt";

	SimpleThreadPool threadPool;
	std::mutex localMutex;

	std::vector<std::pair <std::string, std::string>> Files;

	bool toFile;
};

