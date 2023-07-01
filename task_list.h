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
  TaskList(char* filename); // including load
  ~TaskList(){}
  void Add(pair<int, Other> task);
  void Erase(int id) { task_list_.erase(FindTask(id)); } 
  void Erase(string name) { task_list_.erase(FindTask(name)); }
  void Clear() { task_list_.clear(); }
  bool FindShow(int id) { 
    ShowHead();
    ShowTask(FindTask(id)); 
  }
  bool FindShow(string name) {
    ShowHead();
    ShowTask(FindTask(name));
  }
  void Show(int start = 0, int end = pow(2, 31) - 1, int priority_range = 7, 
            bool (*Compare)(pair<int, Other> task1, pair<int, Other> task2) = LessBegin);
  void Show(string* type_range, int start = 0, int end = pow(2, 31) - 1, int priority_range = 7,
            bool (*Compare)(pair<int, Other> task1, pair<int, Other> task2) = LessBegin);
  bool Remind();  // TODO Remind

 private:
  vector<pair<int, Other>> task_list_;
  char* file_;

  vector<pair<int, Other>>::iterator FindTask(int id);
  vector<pair<int, Other>>::iterator FindTask(string name); // if not found, return task_list_.end()
  void ShowTask(vector<pair<int, Other>>::iterator it);   // TODO ShowTask ShowHead
  void ShowHead();  // print the heading of the task list
};


#endif
