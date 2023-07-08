#pragma once
#include <cmath>
#include <ctime>
#include <string>
#include <thread>
#include <utility>
#include <vector>
using namespace std;
struct Other {
  std::string name;
  time_t begin_time;
  int priority;
  std::string type;
  time_t remind_time;
};
static bool LessId(pair<int, Other> task1, pair<int, Other> task2) {
  return task1.first < task2.first;
}
static bool GreaterId(pair<int, Other> task1, pair<int, Other> task2) {
  return task1.first > task2.first;
}
static bool LessBegin(pair<int, Other> task1, pair<int, Other> task2) {
  return task1.second.begin_time < task2.second.begin_time;
}
static bool GreaterBegin(pair<int, Other> task1, pair<int, Other> task2) {
  return task1.second.begin_time > task2.second.begin_time;
}
static bool LessRemind(pair<int, Other> task1, pair<int, Other> task2) {
  return task1.second.remind_time < task2.second.remind_time;
}
static bool GreaterRemind(pair<int, Other> task1, pair<int, Other> task2) {
  return task1.second.remind_time > task2.second.remind_time;
}
static bool LessPriority(pair<int, Other> task1, pair<int, Other> task2) {
  return task1.second.priority < task2.second.priority;
}
static bool GreaterPriority(pair<int, Other> task1, pair<int, Other> task2) {
  return task1.second.priority > task2.second.priority;
}
class TaskList {
 public:
  TaskList(const char* filename);  // including load
  ~TaskList();
  vector<pair<int, Other>>::iterator return_end() { return task_list_.end(); }
  bool Load(const char* filename);
  int Add(pair<int, Other> task);
  bool Erase(int id);
  bool Erase(string name);
  void Clear() { task_list_.clear(); }
  bool FindShow(int id) {
    if (FindTask(id) == task_list_.end()) return false;
    ShowHead();
    ShowTask(FindTask(id));
    return true;
  }
  bool FindShow(string name) {
    if (FindTask(name) == task_list_.end()) return false;
    ShowHead();
    ShowTask(FindTask(name));
    return true;
  }
  void Show(int start = 0, int end = pow(2, 31) - 1, int priority_range = 7,
            bool (*Compare)(pair<int, Other> task1,
                            pair<int, Other> task2) = LessBegin) const;
  void Show(string type, int start = 0, int end = pow(2, 31) - 1,
            int priority_range = 7,
            bool (*Compare)(pair<int, Other> task1,
                            pair<int, Other> task2) = LessBegin) const;
  void Remind();
  void saveFile();
  vector<pair<int, Other>>::iterator FindTask(int id);
  vector<pair<int, Other>>::iterator FindTask(
      string name);  // if not found, return task_list_.end()

 private:
  vector<pair<int, Other>> task_list_;
  const char* file_;

  string m_header;  // �����ͷ��Ϣ

  void ShowTask(
      vector<pair<int, Other>>::iterator it) const;  
  void ShowHead() const;  // print the heading of the task list

  void Show(vector<pair<int, Other>>& vec, int start = 0,
            int end = pow(2, 31) - 1, int priority_range = 7) const;
  // ˽�к���show��չʾvec��begin_time����start��end֮�䣬priority��priority_range�ڵ��¼�
  string get_priority_string(int Priority)
      const;  // ˽�к���������priority�Ĵ��뷵�ض�Ӧ�����ȼ��ַ���
  void Show_with_one_priority(int start, int end, int Priority,
                              vector<pair<int, Other>>& vec)
      const;  // ˽�к���������չʾһ�����ȼ����¼���Ҫ��
  void Show_with_two_priority(int start, int end, int Priority1, int Priority2,
                              vector<pair<int, Other>>& vec)
      const;  // ˽�к���������չʾ�������ȼ����¼���Ҫ��
  void Show_with_all_priority(int start, int end, vector<pair<int, Other>>& vec)
      const;  // ˽�к���������չʾȫ�����ȼ����¼���Ҫ��
};