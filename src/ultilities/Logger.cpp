#include "Logger.h"

void Logger::Log(Level level, const std::string& message, const char* file, int line)
{
	const char* fileName = GetFileName(file);
	std::string logMessage = "[" + GetLevelString(level) + "] "
		+ fileName + ":" + std::to_string(line) + " - " + message;
	SetColor(level);
	std::cout << logMessage << std::endl;
	ResetColor();
}

void Logger::LogInfo(const std::string& message, const char* file, int line)
{
	Log(Level::INFO, message, file, line);
}

void Logger::LogWarning(const std::string& message, const char* file, int line)
{
	Log(Level::WARNING, message, file, line);
}

void Logger::LogError(const std::string& message, const char* file, int line)
{
	Log(Level::ERR, message, file, line);
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

void Logger::SetColor(Level level)
{
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	int color;
	switch (level)
	{
	case Level::INFO:    color = 9; break;   // Blue
	case Level::WARNING: color = 14; break; // Yellow
	case Level::ERR:     color = 12; break;  // Red
	case Level::DEBUG:   color = 10; break;  // Green
	default:             color = 7; break; // Default White
	}
	SetConsoleTextAttribute(hConsole, color);
#endif
}

void Logger:: ResetColor()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	int color;
#ifdef _WIN32
	color = 7;
	SetConsoleTextAttribute(hConsole, color);
	// Windows resets after each SetConsoleTextAttribute
#endif
}