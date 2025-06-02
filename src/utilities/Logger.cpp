#include "Logger.h"

void Logger::Log(Level level, const std::string& message, const char* file, int line)
{
	const char* fileName = GetFileName(file);
	std::string logMessage = "[" + GetLevelString(level) + "] "
		+ fileName + ":" + std::to_string(line) + " - " + message;
	std::cout << logMessage << std::endl;

}

void Logger::LogInfo(const std::string& message, const char* file, int line)
{
	Log(Level::INFO, message, file, line);
}

void Logger::LogError(const std::string& message, const char* file, int line)
{
	const char* fileName = GetFileName(file);
	std::string logMessage = "[" + GetLevelString(Level::ERR) + "] "
		+ fileName + ":" + std::to_string(line) + " - " + message;
	std::cerr << logMessage << std::endl;
}

const char* Logger::GetFileName(const char* filePath)
{
	const char* fileName = filePath;
	for (const char* ptr = filePath; *ptr; ++ptr)
	{
		if (*ptr == '/' || *ptr == '\\')
		{
			fileName = ptr + 1;
		}
	}
	return fileName;
}

std::string Logger::GetLevelString(Level level)
{
	switch (level)
	{
	case Level::INFO:     return "INFO";
	case Level::WARNING:  return "WARNING";
	case Level::ERR:      return "ERROR";
	case Level::DEBUG:    return "DEBUG";
	default:              return "UNKNOWN";
	}
}
