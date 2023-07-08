
// avoid errors about strcpy, _snprintf, localtime
#define _CRT_SECURE_NO_WARNINGS

#include "tools.h"
#include"file_encrypt.h"
#include<algorithm>
#include<sstream>
#include<stdio.h>
#include<ctime>
#include<iomanip>
#include<iostream>
#include<regex>
#include<fstream>
bool fileExists(const std::string& filename)
{
	
		std::ifstream file(filename);
		return file.good();
	
}
void generate_txtfiles(std::string en_filename, std::string de_filename, std::string password)
{
	if (!fileExists(en_filename))//��������ڸ��û�������Ķ������ļ�
	{
		// ����txt�ļ���д������
		std::ofstream file(de_filename);
		if (file.is_open()) {
			file << "ID" << "\t" << "task_name" << "\t" << "begin_time" << "\t" << "priority";
			file << "\t" << "type" << "\t" << "remind_time";
			file.close();
			//printf("�ļ�д��ɹ�\n");
			//std::remove(username.c_str());
		}
		else {
			printf("create file failure\n");
		}
	}
	else
	{
		//�Ѿ�����
		DecryptFile(en_filename, de_filename, password);
	}
}
void to_lower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
		return std::tolower(c);
		});
}
time_t to_time_t(const std::string& begin_t)
{
	std::tm time_struct = {};
	std::istringstream iss(begin_t);
	iss >> std::get_time(&time_struct, "%Y/%m/%d/%H:%M:%S");
	if (iss.fail()) {
		std::cout << "��������ʱ��ʧ�ܣ�" << std::endl;
		return -1;
	}
	std::time_t timestamp = std::mktime(&time_struct);
	return timestamp;
}
bool isLeapYear(int year)
{
	if (year % 4 == 0)
	{
		if (year % 100 == 0)
		{
			if (year % 400 == 0)
				return true;
			else
				return false;
		}
		else
			return true;
	}
	else
		return false;
}

bool checkTimeFormat(const std::string& inputTime)
{
	std::regex pattern("^\\d{4}/\\d{2}/\\d{2}/\\d{2}:\\d{2}:\\d{2}$");
	return std::regex_match(inputTime, pattern);
}

bool isValidDate(const std::string& inputTime)
{
	// ��֤ʱ���ʽ�Ƿ���ȷ
	if (!checkTimeFormat(inputTime))
		return false;

	// ����ʱ���ַ���
	std::stringstream ss(inputTime);
	std::string token_year, token_month, token_day, token_hour, token_minute, token_second;
	int year, month, day, hour, minute, second;

	if (std::getline(ss, token_year, '/') &&
		std::getline(ss, token_month, '/') &&
		std::getline(ss, token_day, '/') &&
		std::getline(ss, token_hour, ':') &&
		std::getline(ss, token_minute, ':') &&
		std::getline(ss, token_second, ':'))//��ʽ��ȷΪ2022/02/02/00:00:00�ĸ�ʽ
	{
		year = stoi(token_year);
		month = stoi(token_month);
		day = stoi(token_day);
		hour = stoi(token_hour);
		minute = stoi(token_minute);
		second = stoi(token_second);
		if (year < 0) return false;
		if (month < 1 || month > 12) return false;
		if (day < 1) return false;
		if (hour < 0 || hour > 23) return false;
		if (minute < 0 || minute > 59) return false;
		if (second < 0 || second > 59) return false;
		if (month == 2)
		{
			// ��������꣬���¿�����29��
			if (isLeapYear(year))
			{
				if (day > 29) return false;
			}
			else
			{
				if (day > 28) return false;
			}
		}
		else if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
		{
			return false;
		}
		else if (day > 31)
		{
			return false;
		}

		return true;
	}
	else
		return false;
}

std::string to_string_t(time_t s)
{
	struct tm* timeinfo = localtime(&s);
	char formatted_time[20];
	strftime(formatted_time, sizeof(formatted_time), "%Y/%m/%d/%H:%M:%S", timeinfo);
	return formatted_time;
}
