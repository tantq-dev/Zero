#pragma once
#include <iostream>
#include <string>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#endif

class Logger {
public:
	enum class Level { INFO, WARNING, ERR, DEBUG };
	static void Log(Level level, const std::string& message, const char* file, int line);
	static void LogInfo(const std::string& message, const char* file, int line);
	static void LogWarning(const std::string& message, const char* file, int line);
	static void LogError(const std::string& message, const char* file, int line);

private:

	static const char* GetFileName(const char* filePath);
	static std::string GetLevelString(Level level);
	static void SetColor(Level level);
	static void ResetColor();
};

#define LOG(level, message) Logger::Log(level, message, __FILE__, __LINE__)
#define LOG_INFO(message) Logger::LogInfo(message, __FILE__, __LINE__)
#define LOG_WARNING(message) Logger::LogWarning(message, __FILE__, __LINE__)
#define LOG_ERROR(message) Logger::LogError(message, __FILE__, __LINE__)