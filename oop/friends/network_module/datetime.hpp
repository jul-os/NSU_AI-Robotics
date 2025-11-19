#pragma once
#include <chrono>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

std::chrono::system_clock::time_point StringToTime(const std::string &timeStr);
std::string timeToString(const std::chrono::system_clock::time_point &time);