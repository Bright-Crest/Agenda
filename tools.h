#pragma once
#include<string>
void to_lower( std::string& str);
time_t to_time_t(const std::string& str);
bool isLeapYear(int year);
bool checkTimeFormat(const std::string& inputTime);
bool isValidDate(const std::string& inputTime);
std::string to_string_t(time_t s);
bool fileExists(const std::string& filename);
void generate_txtfiles(std::string& username, std::string& password);
