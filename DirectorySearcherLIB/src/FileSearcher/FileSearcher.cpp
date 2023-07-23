#include "pch.h"
#include "FileSearcher.h"

void FileSearcher::FileToFind(const std::string& fileName, const std::string startPath)
{
	try
	{
		FillVector(startPath);
		WaitForTasks();
		const size_t maxThread = std::thread::hardware_concurrency();
		const size_t amountOfFiles = Files.size();
		std::string res = "File not found";
		for (size_t i = 0; i < maxThread; i++)
		{
			size_t start = i * amountOfFiles / maxThread;
			size_t end = (i + 1) * amountOfFiles / maxThread;
			threadPool.Post([start, end, this, &res, &fileName]
				{
					std::vector<std::pair <std::string, std::string>> partOfFiles;
					for (size_t i = start; i < end; i++)
					{
						partOfFiles.push_back({ Files.at(i).first,Files.at(i).second });
					}
					std::lock_guard<std::mutex> lock(localMutex);
					Find(fileName, partOfFiles, res);
				});
		}
		WaitForTasks();
		PrintResult(res);
	}

	catch (const std::exception& ex)
	{
		PrintResult(ex.what());
	}
}

FileSearcher::FileSearcher(int PrintOption)
{
	threadPool.AddThread(std::thread::hardware_concurrency() - 1);
	if (PrintOption == 0)
		toFile = false;
	else if (PrintOption == 1)
		toFile = false;
	else toFile = false;
}

void FileSearcher::Find(const std::string& fileName, std::vector <std::pair <std::string, std::string>> files, std::string& res)
{
	try
	{
		std::string currentFileName = "not found";
		for (size_t i = 0; i < files.size(); i++)
		{
			currentFileName = files[i].first;
			if (currentFileName.find(fileName) != std::string::npos)
			{
				res = files[i].second + "\\" + files[i].first;
				threadPool.WorkOff();
				break;
			}
			currentFileName = "not found";
		}
	}
	catch (const std::exception& ex)
	{
		PrintResult(ex.what());
	}
}

void FileSearcher::FillVector(const std::string& directoryPath)
{
	try
	{
		WIN32_FIND_DATAA findFileData;
		HANDLE hFind = FindFirstFileA(((directoryPath)+"\\*").c_str(), &findFileData);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			throw std::exception("Error finding files in the directory.\n");
		}
		do
		{
			if (findFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				std::lock_guard<std::mutex> lock(localMutex);
				Files.push_back({ findFileData.cFileName, directoryPath });
			}
			else
			{
				if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0)
				{
					std::string subDirPath = directoryPath + "\\" + findFileData.cFileName;
					threadPool.Post([subDirPath, this]
						{
							FillVector(subDirPath);
						});
					;
				}
			}
		} while (FindNextFileA(hFind, &findFileData) != 0);

		FindClose(hFind);
	}
	catch (const std::exception& ex)
	{
		PrintResult(ex.what());
	}

}

void FileSearcher::WaitForTasks()
{
	while ((threadPool.GettaskInWorkCount() != 0) || (threadPool.GetTaskCount() != 0))
	{
		std::cout << "Finding..." << std::endl;
		std::system("cls");
	}
}

void FileSearcher::PrintResult(const std::string & res)
{
	if (toFile)
		WriteToFile(res);
	else std::cout << res << std::endl;
}

void FileSearcher::WriteToFile(const std::string &whatToWrite)
{
	try
	{
		std::string out = "out";
		if (!(CreateDirectoryA(out.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
		{
			std::string exMessage = "Error creating directory for out result\n" + whatToWrite;
			throw std::exception(exMessage.c_str());
		}
		else
		{
			std::ofstream writter;
			writter.open(out + "\\" + output, std::ios::app);
			if (!writter)
			{
				throw std::exception("Error open file");
			}
			writter << whatToWrite + "\n";
			writter.close();
		}
	}
	catch (const std::exception& ex)
	{
		throw ex;
	}
}