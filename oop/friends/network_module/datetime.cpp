#include "datetime.hpp"
#include <sstream>
#include <iomanip>

std::chrono::system_clock::time_point StringToTime(const std::string &timeStr)
{
    std::tm tm = {};
    std::istringstream ss(timeStr);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");

    if (ss.fail())
    {
        throw std::runtime_error("Failed to parse time: " + timeStr);
    }

    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

std::string timeToString(const std::chrono::system_clock::time_point &time)
{
    std::time_t time_t = std::chrono::system_clock::to_time_t(time);
    std::tm tm = *std::localtime(&time_t);

    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return ss.str();
}