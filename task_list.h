#ifndef _TASK_LIST_H_
#define _TASK_LIST_H_
#include <ctime>
#include <string>
#include <utility>
#include <cmath>
#include <vector>

using namespace std;

struct Other {
  string name;
  time_t begin_time;
  int priority; // 4 - high, 2 - medium, 1 - low
  string type;
  time_t remind_time;
};

static bool LessId(pair<int, Other> task1, pair<int, Other> task2) { return task1.first < task2.first; }
static bool GreaterId(pair<int, Other> task1, pair<int, Other> task2) { return task1.first > task2.first; }
static bool LessBegin(pair<int, Other> task1, pair<int, Other> task2) { return task1.second.begin_time < task2.second.begin_time; }
static bool GreaterBegin(pair<int, Other> task1, pair<int, Other> task2) { return task1.second.begin_time > task2.second.begin_time; }
static bool LessRemind(pair<int, Other> task1, pair<int, Other> task2) { return task1.second.remind_time < task2.second.remind_time; }
static bool GreaterRemind(pair<int, Other> task1, pair<int, Other> task2) { return task1.second.remind_time > task2.second.remind_time; }
static bool LessPriority(pair<int, Other> task1, pair<int, Other> task2) { return task1.second.priority < task2.second.priority; }
static bool GreaterPriority(pair<int, Other> task1, pair<int, Other> task2) { return task1.second.priority > task2.second.priority; }

class TaskList {
 public:
  TaskList(const char* filename); // including load
  ~TaskList();
  bool Load(const char* filename);
  bool Add(pair<int, Other> task);
  void Erase(int id) { task_list_.erase(FindTask(id)); } 
  void Erase(string name) { task_list_.erase(FindTask(name)); }
  void Clear() { task_list_.clear(); }
  bool FindShow(int id) { 
    ShowHead();
    ShowTask(FindTask(id)); 
    return (FindTask(id) != task_list_.end()) ? true : false;
  }
  bool FindShow(string name) {
    ShowHead();
    ShowTask(FindTask(name));
    return (FindTask(name) != task_list_.end()) ? true : false;
  }
  void Show(int start = 0, int end = pow(2, 31) - 1, int priority_range = 7, 
            bool (*Compare)(pair<int, Other> task1, pair<int, Other> task2) = LessBegin);
  void Show(string type, int start = 0, int end = pow(2, 31) - 1, int priority_range = 7,
            bool (*Compare)(pair<int, Other> task1, pair<int, Other> task2) = LessBegin);
  void Remind();

 private:
  vector<pair<int, Other>> task_list_;
  const char* file_;

  string m_header;  // 储存表头信息
  vector<pair<int, Other>>::iterator FindTask(int id);
  vector<pair<int, Other>>::iterator FindTask(string name); // if not found, return task_list_.end()
  void ShowTask(vector<pair<int, Other>>::iterator it);   // TODO ShowTask ShowHead
  void ShowHead();  // print the heading of the task list

  void Show(vector<pair<int, Other>> &vec, int start = 0,
                      int end = pow(2, 31) - 1, int priority_range = 7);
	//私有函数show，展示vec中begin_time介于start与end之间，priority在priority_range内的事件
  string get_priority_string(int Priority);//私有函数，根据priority的代码返回对应的优先级字符串
  void Show_with_one_priority(int start, int end, int Priority, vector<pair<int, Other> > &vec);//私有函数，处理展示一个优先级的事件的要求
  void Show_with_two_priority(int start, int end, int Priority1, int Priority2, vector<pair<int, Other> > &vec);//私有函数，处理展示两个优先级的事件的要求
  void Show_with_all_priority(int start, int end, vector< pair<int, Other> > &vec);//私有函数，处理展示全部优先级的事件的要求
  void SaveFile(const std::string& fileName);
};


#endif
