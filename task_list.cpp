#include "task_list.h"
#include"tools.h"
#include <utility>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include<iomanip>
TaskList::TaskList(const char* filename)
{
	file_ = filename;
	Load(filename);
}

TaskList::~TaskList()
{
}

bool TaskList::Load(const char* filename)
{
	fstream read(filename, ios::in);
	if (!read.is_open())
	{
		std::cout << "open failed" << endl;
	}
	else {
		char buf[1024] = { 0 };
		read.getline(buf, 1024);
		m_header = buf;
		std::cout << m_header << endl;
		char data[1024] = { 0 };
		while (!read.eof())
		{
			while(read.getline(data,1024)){
				if(data[0]!='\0' && data[0] != '\n')
					break;
			}
			Other stu;
			stringstream ss(data);
			long long  id;
			ss >> id;
			std::cout << id << " ";
			ss >> stu.name;
			string bt;
			ss >> bt;
			stu.begin_time = to_time_t(bt);
			ss >> stu.priority >> stu.type;
			string rt;
			ss >> rt;
			stu.remind_time = to_time_t(rt);
			std::cout << stu.name << " " << stu.begin_time << " " <<
				stu.priority << " " << stu.type << " " << stu.remind_time << endl;
			task_list_.push_back(make_pair(id, stu));

		}
		read.close();
		sort(task_list_.begin(), task_list_.end(), GreaterRemind);
		return true;
	}
}
int TaskList::Add(pair<int, Other> task)
{
	int i;
	for (i = 0; i < task_list_.size(); i++)
	{
		if (task_list_[i].second.begin_time == task.second.begin_time)
			return -1;
		if(task_list_[i].second.name == task.second.name)
			return 1;
	}

	for (i = 0; i < task_list_.size(); i++)
	{
		if (task_list_[i].second.remind_time < task.second.remind_time) // 找到task在按remind_time_从大到小排序的vector task_list_中的位置
			break;
	}

	task_list_.insert(task_list_.begin() + i, task); // 完成插入,此时vector是有序的

	ofstream write;
	write.open(file_, ios::app); // 打开文件,文件指针在文件尾部,将新增的信息保存在文件最后一行

	// 将int类型的begin_time和remind_time转化为可写入文件的字符串begin_time_str和remind_time_str
	time_t begin_time_ = static_cast<time_t>(task.second.begin_time);
	time_t remind_time_ = static_cast<time_t>(task.second.remind_time);
	char begin_time_str[80];
	char remind_time_str[80];
	strftime(begin_time_str, sizeof(begin_time_str), "%Y/%m/%d/%H:%M:%S", localtime(&begin_time_));
	strftime(remind_time_str, sizeof(remind_time_str), "%Y/%m/%d/%H:%M:%S", localtime(&remind_time_));

	write << '\n' << task.first << '\t' << task.second.name << '\t' << begin_time_str << '\t' <<
		task.second.priority << '\t' << task.second.type << '\t' << remind_time_str ;

	write.close();

	vector<pair<int, Other>>::iterator it = task_list_.begin() + i;
	std::cout << "Task: " << endl;
	ShowTask(it);
	std::cout << "has been added to the Task List!" << endl;
	return 0;
}
bool TaskList::Erase(int id)
{
	if (FindTask(id) != task_list_.end())
	{
		cout << "Task: " << endl;
		ShowTask(FindTask(id));
		cout << "has been deleted!" << endl;
		task_list_.erase(FindTask(id));
		return true;
	}
	else
		return false;
}
bool TaskList::Erase(string name)
{
	if (FindTask(name) != task_list_.end())
	{
		cout << "Task: " << endl;
		ShowTask(FindTask(name));
		cout << "has been deleted!" << endl;
		task_list_.erase(FindTask(name));
		return true;
	}
	else
		return false;
}
void TaskList::saveFile()
{
	fstream write(file_, ios::out | ios::trunc);
	if (!write.is_open())
	{
		cerr << file_ << "file open failed" << endl;
		return;
	}
	//head
	m_header += '\n';
	write.write(m_header.c_str(), m_header.size());
	//data
	for (auto& task : task_list_)
	{
		time_t begin_time_ = static_cast<time_t>(task.second.begin_time);
		time_t remind_time_ = static_cast<time_t>(task.second.remind_time);
		char begin_time_str[80];
		char remind_time_str[80];
		strftime(begin_time_str, sizeof(begin_time_str), "%Y/%m/%d/%H:%M:%S", localtime(&begin_time_));
		strftime(remind_time_str, sizeof(remind_time_str), "%Y/%m/%d/%H:%M:%S", localtime(&remind_time_));

		write << '\n' << task.first << '\t' << task.second.name << '\t' << begin_time_str << '\t' <<
			task.second.priority << '\t' << task.second.type << '\t' << remind_time_str ;
	}
	write.close();
}
vector<pair<int, Other>>::iterator TaskList::FindTask(int id){
	for (auto it = task_list_.begin(); it != task_list_.end(); it++) {
		if (it->first == id) {
			return it;
		}
	}
	return task_list_.end();
}

vector<pair<int, Other>>::iterator TaskList::FindTask(string name){
	for (auto it = task_list_.begin(); it != task_list_.end(); it++) {
		if (it->second.name.compare(name) == 0) {
			return it;
		}
	}
	return task_list_.end();
}
void TaskList::ShowTask(vector<pair<int, Other>>::iterator it) const {
	// TODO ShowTask
	string showbegintime = to_string_t(it->second.begin_time);
	string showremindtime = to_string_t(it->second.remind_time);
	string sPriority = get_priority_string(it->second.priority);
	std::cout << "----------------------------------------------------------------" << endl;
			std::cout << std::setw(14) << "ID: " << it->first << endl
				<< std::setw(14) << "Name: " << it->second.name << endl
				<< std::setw(14) << "Begin time: " << showbegintime << endl
				<< std::setw(14) << "Priority: " << sPriority << endl
				<< std::setw(14) << "Type: " << it->second.type << endl
				<< std::setw(14) << "Remind time: " << showremindtime << endl;
			std::cout << "----------------------------------------------------------------" << endl;
}
void TaskList::ShowHead() const{
	// TODO ShowHead
	string ID;
	string t_name;
	string t_type;
	string t_priority;
	string t_begin;
	string t_remind;
	stringstream ss(m_header);
	ss >> ID >> t_name >> t_begin >> t_priority >> t_type >> t_remind;
	std::cout << std::setw(10) << ID << '\t'
		<< std::setw(14) << t_name << '\t'
		<< std::setw(14) << t_begin << '\t'
		<< std::setw(14) << t_priority << '\t'
		<< std::setw(10) << t_type << '\t'
		<< std::setw(14) << t_remind << endl;;

}
void TaskList::Show(int start, int end, int priority_range,
	bool (*Compare)(pair<int, Other> task1,
		pair<int, Other> task2)) const{
	// 创建副本并按begin_time排序
	vector<pair<int, Other>> tmp(task_list_);

	sort(tmp.begin(), tmp.end(), Compare);

	// 调用私有函数展示tmp
	Show(tmp, start, end, priority_range);
}

void TaskList::Show(string type, int start, int end,
	int priority_range,
	bool (*Compare)(pair<int, Other> task1,
		pair<int, Other> task2)) const{
	// 创建只含type类型的时间的副本并排序
	vector<pair<int, Other>> tmp;
	for (int i = 0; i < task_list_.size(); i++) {
		if (type.compare(task_list_[i].second.type) == 0)
			tmp.push_back(task_list_[i]);
	}

	sort(tmp.begin(), tmp.end(), Compare);

	// 调用私有函数展示tmp
	Show(tmp, start, end, priority_range);
}

// 私有函数show，展示vec中begin_time介于start与end之间，priority在priority_range内的事件
void TaskList::Show(vector<pair<int, Other>>& vec,
	int start, int end, int priority_range) const{
	// 根据priority进行区分
	switch (priority_range) {
	case 4: {
		Show_with_one_priority(start, end, 4, vec);
		break;
	}
	case 2: {
		Show_with_one_priority(start, end, 2, vec);
		break;
	}
	case 1: {
		Show_with_one_priority(start, end, 1, vec);
		break;
	}
	case 3: {
		Show_with_two_priority(start, end, 1, 2, vec);
		break;
	}
	case 5: {
		Show_with_two_priority(start, end, 1, 4, vec);
		break;
	}
	case 6: {
		Show_with_two_priority(start, end, 2, 4, vec);
		break;
	}
	case 7: {
		Show_with_all_priority(start, end, vec);
		break;
	}
	}
}

// 私有函数，根据priority的代码返回对应的优先级字符串
string TaskList::get_priority_string(int Priority) const{
	switch (Priority) {
	case 4: {
		return "High";
		break;
	}
	case 2: {
		return "Medium";
		break;
	}
	case 1: {
		return "Low";
		break;
	}
	}
}

// 私有函数，处理展示一个优先级的事件的要求
void TaskList::Show_with_one_priority(int start, int end, int Priority,
	vector<pair<int, Other>>& vec) const {
	string sPriority = get_priority_string(Priority);

	int i = 0;
	for (; i < vec.size() && vec[i].second.begin_time < start; i++)
		;
	for (; i < vec.size() && vec[i].second.begin_time <= end; i++) {
		string b_t = to_string_t(vec[i].second.begin_time);
		string r_t = to_string_t(vec[i].second.remind_time);

		if (vec[i].second.priority == Priority)
		{
			std::cout << "----------------------------------------------------------------" << endl;
			std::cout << std::setw(14) << "ID: " << vec[i].first << endl
				<< std::setw(14) << "Name: " << vec[i].second.name << endl
				<< std::setw(14) << "Begin time: " << b_t << endl
				<< std::setw(14) << "Priority: " << sPriority << endl
				<< std::setw(14) << "Type: " << vec[i].second.type << endl
				<< std::setw(14) << "Remind time: " << r_t << endl;
			std::cout << "----------------------------------------------------------------" << endl;
		}
	}

	std::cout << endl;
}

// 私有函数，处理展示两个优先级的事件的要求
void TaskList::Show_with_two_priority(int start, int end, int Priority1,
	int Priority2,
	vector<pair<int, Other>>& vec) const {
	string sPriority1 = get_priority_string(Priority1);
	string sPriority2 = get_priority_string(Priority2);

	int i = 0;
	for (; i < vec.size() && vec[i].second.begin_time < start; i++)
		;
	for (; i < vec.size() && vec[i].second.begin_time <= end; i++) {

		string b_t = to_string_t(vec[i].second.begin_time);
		string r_t = to_string_t(vec[i].second.remind_time);
		if (vec[i].second.priority == Priority1)
		{
			std::cout << "----------------------------------------------------------------" << endl;
			std::cout << std::setw(14) << "ID: " << vec[i].first << endl
				<< std::setw(14) << "Name: " << vec[i].second.name << endl
				<< std::setw(14) << "Begin time: " << b_t << endl
				<< std::setw(14) << "Priority: " << sPriority1 << endl
				<< std::setw(14) << "Type: " << vec[i].second.type << endl
				<< std::setw(14) << "Remind time: " << r_t << endl;
			std::cout << "----------------------------------------------------------------" << endl;
		}
		else if (vec[i].second.priority == Priority2)
		{
			std::cout << "----------------------------------------------------------------" << endl;
			std::cout << std::setw(14) << "ID: " << vec[i].first << endl
				<< std::setw(14) << "Name: " << vec[i].second.name << endl
				<< std::setw(14) << "Begin time: " << b_t << endl
				<< std::setw(14) << "Priority: " << sPriority2 << endl
				<< std::setw(14) << "Type: " << vec[i].second.type << endl
				<< std::setw(14) << "Remind time: " << r_t << endl;
			std::cout << "----------------------------------------------------------------" << endl;
		}

	}

	std::cout << endl;
}

// 私有函数，处理展示全部优先级的事件的要求
void TaskList::Show_with_all_priority(int start, int end,
	vector<pair<int, Other>>& vec) const {
	int i = 0;
	for (; i < vec.size() && vec[i].second.begin_time < start; i++)
		;
	for (; i < vec.size() && vec[i].second.begin_time <= end; i++) {

		string b_t = to_string_t(vec[i].second.begin_time);
		string r_t = to_string_t(vec[i].second.remind_time);
		if (vec[i].second.priority == 4)
		{
			std::cout << "----------------------------------------------------------------" << endl;
			std::cout << std::setw(14) << "ID: " << vec[i].first << endl
				<< std::setw(14) << "Name: " << vec[i].second.name << endl
				<< std::setw(14) << "Begin time: " << b_t << endl
				<< std::setw(14) << "Priority: " << "High" << endl
				<< std::setw(14) << "Type: " << vec[i].second.type << endl
				<< std::setw(14) << "Remind time: " << r_t << endl;
			std::cout << "----------------------------------------------------------------" << endl;
		}

		else if (vec[i].second.priority == 2)
		{
			std::cout << "----------------------------------------------------------------" << endl;
			std::cout << std::setw(14) << "ID: " << vec[i].first << endl
				<< std::setw(14) << "Name: " << vec[i].second.name << endl
				<< std::setw(14) << "Begin time: " << b_t << endl
				<< std::setw(14) << "Priority: " << "Medium" << endl
				<< std::setw(14) << "Type: " << vec[i].second.type << endl
				<< std::setw(14) << "Remind time: " << r_t << endl;
			std::cout << "----------------------------------------------------------------" << endl;
		}

		else if (vec[i].second.priority == 1)
		{
			std::cout << "----------------------------------------------------------------" << endl;
			std::cout << std::setw(14) << "ID: " << vec[i].first << endl
				<< std::setw(14) << "Name: " << vec[i].second.name << endl
				<< std::setw(14) << "Begin time: " << b_t << endl
				<< std::setw(14) << "Priority: " << "Low" << endl
				<< std::setw(14) << "Type: " << vec[i].second.type << endl
				<< std::setw(14) << "Remind time: " << r_t << endl;
			std::cout << "----------------------------------------------------------------" << endl;
		}

	}

	std::cout << endl;
}

void TaskList::Remind() {

	const time_t kTolerantTime = 2;
	for (auto it = task_list_.begin(); it != task_list_.end(); it++) {
		if (it->second.remind_time < time(NULL) - kTolerantTime)
		{
			break;
		}
		if (it->second.remind_time > time(NULL) + kTolerantTime) continue;
		else {
			
			cout << endl;
			cout << "Attention: " << it->second.name << " the following task(s) should be done!\n";
			ShowTask(it);
			

		}
	}

}
