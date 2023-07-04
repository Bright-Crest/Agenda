// define Class CmdProcessor

#pragma once

#include <string>
#include "task_list.h"

using namespace std;

class CmdProcessor {
 public:
  CmdProcessor();
  ~CmdProcessor();
  bool GetArgv(int argc, char* argv[]); // transform argv[1] ~ argv[argc - 1] to cmd_ and check cmd_[0]; return: true -- with cmd, false -- no cmd.
  bool GetCmd(); // In while, get user's input into cmd_; return: true -- with cmd, false -- nothing.
  // neither of the above check if the cmd is correct.

  bool CmdDistributor(TaskList& task_list); // probably using switch case to check cmd and do sth accordingly; return: true -- correct cmd.
  // const string* Cmd() { return cmd_; }

 private:
  string* cmd_;
};

