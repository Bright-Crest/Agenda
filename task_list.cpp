#include "task_list.h"
#include <ctime>
#include <vector>
#include <string>

using namespace std;

void TaskList::Remind() {
  // TODO Remind
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
}
void TaskList::ShowHead() {
  // TODO ShowHead
}
