#include "cmd_processor.h"

#include <iostream>
#include <sstream>
#include <string>
#include <list>

#include "task_list.h"
#include "tools.h"

using namespace std;

bool CmdProcessor::GetArgv(int argc, char* argv[], int start) {
  cmd_.clear();
  if (start == argc) {
    return false;
  }

  for (int i = start; i < argc; i++) {
    cmd_.push_back(argv[i]);
  }
  return true;
}

bool CmdProcessor::GetCmd() {
  cmd_.clear();
  string tmp;
  getline(cin, tmp);
  istringstream cmd(tmp);

  cmd >> tmp;
  if (!cmd) return false;
  cmd_.push_back(tmp);
  while (cmd >> tmp) {
    cmd_.push_back(tmp);
  }
  return true;
}

int CmdProcessor::CmdDistributor(TaskList& task_list) const {
  const int kSize = cmd_.size();
  if (kSize < 1) return 0;

  bool flag = false;
  string first_cmd = cmd_.front();
  to_lower(first_cmd);

  if (first_cmd == "addtask" || first_cmd == "add") {
    if (kSize == 1)
      flag = AddTaskNoOp(task_list);
    else
      flag = AddTaskOp(task_list, cmd_);
  } else if (first_cmd == "modifytask" || first_cmd == "modify") {
    if (kSize == 1)
      flag = ModifyTaskNoOp(task_list);
    else
      flag = ModifyTaskOp(task_list, cmd_);
  } else if (first_cmd == "deletetask" || first_cmd == "delete") {
    if (kSize == 1)
      flag = DeleteTaskNoOp(task_list);
    else
      flag = DeleteTaskOp(task_list, cmd_);
  } else if (first_cmd == "showtask" || first_cmd == "show") {
    if (kSize == 1)
      flag = ShowTaskNoOp(task_list);
    else
      flag = ShowTaskOp(task_list, cmd_);
  } else if (first_cmd == "searchtask" || first_cmd == "search") {
    if (kSize == 1)
      flag = SearchTaskNoOp(task_list);
    else
      flag = SearchTaskOp(task_list, cmd_);
  } else if (first_cmd == "quit" || first_cmd == "q") {
    cout << "Bye.\n";
    return -1;
  } else {
    cout << "\"" << cmd_.front() << "\" is not a valid command.\n";
    return 0;
  }
  return (int)flag;
}

bool AddTaskNoOp(TaskList& task_list) { return 1; }

bool ModifyTaskNoOp(TaskList& task_list) { return 1; }

bool DeleteTaskNoOp(TaskList& task_list) { return 1; }

bool ShowTaskNoOp(const TaskList& task_list) { return 1; }

bool SearchTaskNoOp(const TaskList& task_list) { return 1; }

bool AddTaskOp(TaskList& task_list, list<string> cmd) { return 1; }

bool ModifyTaskOp(TaskList& task_list, list<string> cmd) { return 1; }

bool DeleteTaskOp(TaskList& task_list, list<string> cmd) { return 1; }

bool ShowTaskOp(const TaskList& task_list, list<string> cmd) { return 1; }

bool SearchTaskOp(const TaskList& task_list, list<string> cmd) { return 1; }
