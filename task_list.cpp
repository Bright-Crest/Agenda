#include "task_list.h"

#include <utility>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

TaskList::TaskList(const char *filename) {
  Load(filename);  // 从文件中加载信息
}

TaskList::~TaskList() {}

bool TaskList::Load(const char *fileName) {
  fstream read(fileName, ios::in);  // 读入文件
  if (!read.is_open()) {
    cout << "open failed" << endl;  // 打开文件失败，返回false
    return false;
  } else
    cout << "open succeed" << endl;
  char buf[1024] = {0};
  read.getline(buf, 1024);
  m_header = buf;  // 读取表头
  cout << m_header << endl;
  while (!read.eof()) {
    char data[1024] = {0};
    read.getline(data, 1024);
    if (strlen(data) == 0)  // 如果读取的数据为空，则退出循环
      break;
    Other stu;
    stringstream ss(data);  // 通过sstream流入信息
    int id;
    ss >> id;
    cout << id << " ";
    ss >> stu.name >> stu.begin_time >> stu.priority >> stu.type >>
        stu.remind_time;
    cout << stu.name << " " << stu.begin_time << " " << stu.priority << " "
         << stu.type << " " << stu.remind_time << endl;
    task_list_.push_back(std::make_pair(id, stu));  // 将任务插入任务列表中
  }
  read.close();
  sort(task_list_.begin(), task_list_.end(),
       GreaterRemind);  // 对提醒顺序列表进行排序
  return true;
}

bool TaskList::Add(pair<int, Other> task)
{
  for (int i = 0; i < task_list_.size(); i++)
  {
    if (task_list_[i].second.begin_time == task.second.begin_time || task_list_[i].second.name == task.second.name)
      return false;
  }

  int i;
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
  strftime(begin_time_str, sizeof(begin_time_str), "%Y-%m-%d %H:%M:%S", localtime(&begin_time_));
  strftime(remind_time_str, sizeof(remind_time_str), "%Y-%m-%d %H:%M:%S", localtime(&remind_time_));

  write << task.first << task.second.name << '\t' << begin_time_str << '\t' << task.second.priority << '\t' << task.second.type << '\t' << remind_time_str << '\n';

  write.close();

  return true;
}

void TaskList::Remind() {
  // TODO Remind
  const time_t kTolerantTime = 15;
  bool flag = false;
  for (auto it = task_list_.begin(); it != task_list_.end(); it++) {
    if (it->second.remind_time <= time(NULL) - kTolerantTime) break;
    if (it->second.remind_time >= time(NULL) + kTolerantTime) continue;
    else {
      if (!flag) {
        cout << "Attention: the following task(s) should be done!\n";
        flag = true;
      }
      ShowTask(it);
    }
  }
}

vector<pair<int, Other>>::iterator TaskList::FindTask(int id) {
  for (auto it = task_list_.begin(); it != task_list_.end(); it++) {
    if (it->first == id) {
      return it;
    }
  }
  return task_list_.end();
}

vector<pair<int, Other>>::iterator TaskList::FindTask(string name) {
  for (auto it = task_list_.begin(); it != task_list_.end(); it++) {
    if (it->second.name.compare(name) == 0) {
      return it;
    }
  }
  return task_list_.end();
}

void TaskList::ShowTask(vector<pair<int, Other>>::iterator it) {
  // TODO ShowTask
  cout << it->first << '\t' << it->second.name << '\t' << it->second.begin_time
       << '\t' << it->second.priority << '\t' << it->second.type << '\t' << it->second.remind_time
       << "\tOnly for test\n";
}
void TaskList::ShowHead() {
  // TODO ShowHead
  cout << m_header << endl;
}

void TaskList::Show(int start, int end,
                    int priority_range,
                    bool (*Compare)(pair<int, Other> task1,
                                    pair<int, Other> task2)) {
  // 创建副本并按begin_time排序
  vector<pair<int, Other>> tmp(task_list_);

  sort(tmp.begin(), tmp.end(), Compare);

  // 调用私有函数展示tmp
  Show(tmp, start, end, priority_range);
}

void TaskList::Show(string type, int start, int end,
                    int priority_range,
                    bool (*Compare)(pair<int, Other> task1,
                                    pair<int, Other> task2)) {
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
void TaskList::Show(vector<pair<int, Other>> &vec, 
                    int start, int end, int priority_range) {
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
string TaskList::get_priority_string(int Priority) {
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
                                      vector<pair<int, Other>> &vec) {
  string sPriority = get_priority_string(Priority);

  int i = 0;
  for (; i < vec.size() && vec[i].second.begin_time < start; i++)
    ;
  for (; i < vec.size() && vec[i].second.begin_time <= end; i++) {
    if (vec[i].second.priority == Priority)
      cout << "Name: " << vec[i].second.name << endl
           << "Begin time: " << vec[i].second.begin_time << endl
           << "Priority: " << sPriority << endl
           << "Type: " << vec[i].second.type << endl
           << "Remind time: " << vec[i].second.remind_time << endl;
  }

  cout << endl;
}

// 私有函数，处理展示两个优先级的事件的要求
void TaskList::Show_with_two_priority(int start, int end, int Priority1,
                                      int Priority2,
                                      vector<pair<int, Other>> &vec) {
  string sPriority1 = get_priority_string(Priority1);
  string sPriority2 = get_priority_string(Priority2);

  int i = 0;
  for (; i < vec.size() && vec[i].second.begin_time < start; i++)
    ;
  for (; i < vec.size() && vec[i].second.begin_time <= end; i++) {
    if (vec[i].second.priority == Priority1)
      cout << "Name: " << vec[i].second.name << endl
           << "Begin time: " << vec[i].second.begin_time << endl
           << "Priority: " << sPriority1 << endl
           << "Type: " << vec[i].second.type << endl
           << "Remind time: " << vec[i].second.remind_time << endl;
    else if (vec[i].second.priority == Priority2)
      cout << "Name: " << vec[i].second.name << endl
           << "Begin time: " << vec[i].second.begin_time << endl
           << "Priority: " << sPriority2 << endl
           << "Type: " << vec[i].second.type << endl
           << "Remind time: " << vec[i].second.remind_time << endl;
  }

  cout << endl;
}

// 私有函数，处理展示全部优先级的事件的要求
void TaskList::Show_with_all_priority(int start, int end,
                                      vector<pair<int, Other>> &vec) {
  int i = 0;
  for (; i < vec.size() && vec[i].second.begin_time < start; i++)
    ;
  for (; i < vec.size() && vec[i].second.begin_time <= end; i++) {
    if (vec[i].second.priority == 4)
      cout << "Name: " << vec[i].second.name << endl
           << "Begin time: " << vec[i].second.begin_time << endl
           << "Priority: "
           << "High" << endl
           << "Type: " << vec[i].second.type << endl
           << "Remind time: " << vec[i].second.remind_time << endl;
    else if (vec[i].second.priority == 2)
      cout << "Name: " << vec[i].second.name << endl
           << "Begin time: " << vec[i].second.begin_time << endl
           << "Priority: "
           << "Medium" << endl
           << "Type: " << vec[i].second.type << endl
           << "Remind time: " << vec[i].second.remind_time << endl;
    else if (vec[i].second.priority == 1)
      cout << "Name: " << vec[i].second.name << endl
           << "Begin time: " << vec[i].second.begin_time << endl
           << "Priority: "
           << "Low" << endl
           << "Type: " << vec[i].second.type << endl
           << "Remind time: " << vec[i].second.remind_time << endl;
  }

  cout << endl;
}
