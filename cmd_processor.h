// define Class CmdProcessor
// define some functions to deal with specific cmds

#pragma once

#include <string>
#include <list>
#include "task_list.h"

using namespace std;

class CmdProcessor {
 public:
  CmdProcessor() {}
  ~CmdProcessor() {}
  bool GetArgv(int argc, char* argv[], int start); // transform argv[start] ~ argv[argc - 1] to cmd_ and check cmd_[0]; return: true -- with cmd, false -- no cmd.
  bool GetCmd(); // In while, get user's input into cmd_; return: true -- with cmd, false -- nothing.
  // neither of the above check if the cmd is correct.

  int CmdDistributor(TaskList& task_list) const; // check cmd and do sth accordingly; return: 1 -- correct cmd, 0 -- wrong cmd, -1 -- quit.
  // const string* Cmd() { return cmd_; }

 private:
  list<string> cmd_;
};

bool AddTaskNoOp(TaskList& task_list);
bool ModifyTaskNoOp(TaskList& task_list);
bool DeleteTaskNoOp(TaskList& task_list);
bool ShowTaskNoOp(const TaskList& task_list);
bool SearchTaskNoOp(const TaskList& task_list);
bool should_quit(string choice);

// All ops(options) start with "-" and have only one character.
bool AddTaskOp(TaskList& task_list, list<string> cmd);  // ops: "n:b:r:pt"
bool ModifyTaskOp(TaskList& task_list, list<string> cmd);  // "i:n:C:T" i--id, n--name, C--Change, T--To. id 和 name 有一个即可。C 后接要改变的东西，如nbrpt(name,begin_time...), 不论是简写还是完整的都要支持。T 后接变成的内容，注意要检查。
bool DeleteTaskOp(TaskList& task_list, list<string> cmd);   // "i:n:a" a--all.
bool ShowTaskOp(const TaskList& task_list, list<string> cmd);  // "S:E:p:t:A:D:" S--Start, E--End, A--Ascending, D--Descending. A 和 D 后接inbrp(id, name, begin_time, remind_time, priority), 用于排序。
bool SearchTaskOp(const TaskList& task_list, list<string> cmd);  // "i:n:"
