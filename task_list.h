#ifndef _TASK_LIST_H_
#define _TASK_LIST_H_
// #include <map>
#include <ctime>
#include <string>
#include <utility>
#include <cmath>
#include <vector>

using namespace std;

struct Other {
  string name;
  int begin_time;
  int priority; // 4 - high, 2 - medium, 1 - low
  string type;
  int remind_time;
};

bool LessBegin(pair<int, Other> task1, pair<int, Other> task2) { return task1.second.begin_time < task2.second.begin_time; }
bool GreaterBegin(pair<int, Other> task1, pair<int, Other> task2) { return task1.second.begin_time > task2.second.begin_time; }
bool LessRemind(pair<int, Other> task1, pair<int, Other> task2) { return task1.second.remind_time < task2.second.remind_time; }
bool GreaterRemind(pair<int, Other> task1, pair<int, Other> task2) { return task1.second.remind_time > task2.second.remind_time; }

class TaskList {
 public:
  TaskList(char* filename);
  ~TaskList();
  bool Load(); // return false if file is doesn't exist; 
  void Add(pair<int, Other> task);
  bool Erase(int begin_t) { return task_list_.erase(begin_t); }
  bool Erase(string name);
  void Clear() { task_list_.clear(); }
  bool FindShow(int begin_t);
  bool FindShow(string name);
  void Show(int start = 0, int end = pow(2, 31) - 1, int priority_range = 7, 
            bool (*Compare)(pair<int, Other> task1, pair<int, Other> task2) = LessBegin);
  void Show(string* type_range, int start = 0, int end = pow(2, 31) - 1, int priority_range = 7,
            bool (*Compare)(pair<int, Other> task1, pair<int, Other> task2) = LessBegin);
  bool Remind();

 private:
  // map<int, Other> task_list_; 
  vector<pair<int, Other>> task_list_;
  char* file_;
  string m_header;//储存表头信息
  // map<int, Other>::iterator FindTask(string name);
  vector<pair<int, Other>>::iterator FindTask(string name);
  // void ShowTask(map<int, Other>::iterator it);
  void ShowTask(vector<pair<int, Other>>::iterator it);
  void ShowHead();  // print the heading of the task list
  void Show(int start = 0, int end = pow(2, 31) - 1, int priority_range = 7,  
			vector<pair<int, Other> > &vec);//私有函数show，展示vec中begin_time介于start与end之间，priority在priority_range内的事件
  string get_priority_string(int Priority);//私有函数，根据priority的代码返回对应的优先级字符串
  void Show_with_one_priority(int start, int end, int Priority, vector<pair<int, Other> > &vec);//私有函数，处理展示一个优先级的事件的要求
  void Show_with_two_priority(int start, int end, int Priority1, int Priority2, vector<pair<int, Other> > &vec);//私有函数，处理展示两个优先级的事件的要求
  void Show_with_all_priority(int start, int end, vector< pair<int, Other> > &vec);//私有函数，处理展示全部优先级的事件的要求


};


#endif
