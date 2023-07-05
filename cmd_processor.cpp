#include "cmd_processor.h"

#include <iostream>
#include <sstream>
#include <string>
#include <list>

#include "task_list.h"
#include "tools.h"

using namespace std;

bool CmdProcessor::GetArgv(int argc, char* argv[], int start) {
  if (start == argc) {
    return false;
  }

  for (int i = start; i < argc; i++) {
    cmd_.push_back(argv[i]);
  }
  return true;
}

bool CmdProcessor::GetCmd() {
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
  if (kSize < 1) return false;

  bool flag = false;
  string first_cmd = cmd_.front();
  to_lower(first_cmd);

  if (first_cmd == "login") {
    // TODO login
  } else if (first_cmd == "addtask" || first_cmd == "add") {
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
  return flag;
}

// 用户输入格式:addtask -name n -begin b -priority p -type t -remind r (注意:用户输入的参数不能以-开头,priority默认为0,type默认为" ",选项的顺序可以打乱)
bool AddTaskOp(TaskList &task_list, list<string> &cmd)
{
  Other task;
  task.priority = 0;
  task.type = " ";
  string begin;                              // begin用于保存begin_time_并分配唯一的id
  list<string>::iterator it = ++cmd.begin(); // 从cmd的第二个string开始参考
  while (it != cmd.end())
  {
    if (*it == "-name")
    {
      if (++it == cmd.end()) //-name是最后一个,后面没有参数
          return false;
      string str = *(it); // 找到-name的下一位
      if (str[0] == '-')  // 不符合
          return false;
      task.name = str;
      it++;
    }
    else if (*it == "-begin")
    {
      if (++it == cmd.end()) //-begin是最后一个,后面没有参数
          return false;
      begin = *(it);       // 找到-begin的下一位
      if (begin[0] == '-') // 不符合
          return false;
      if (!isValidDate(begin)) // 时间参数不符合要求
          return false;
      task.begin_time = to_time_t(begin);
      it++;
    }
    else if (*it == "-priority")
    {
      if (++it == cmd.end()) //-priority是最后一个,后面没有参数,这是可以的
          break;
      string str = *(it);                                                          // 找到-priority的下一位
      if (str == "-name" || str == "-begin" || str == "-type" || str == "-remind") //-priority后不跟参数,这是可以的
          continue;
      int p = stoi(str);
      if (p != 1 && p != 2 && p != 4)
          return false;
      task.priority = p;
      it++;
    }
    else if (*it == "-type")
    {
      if (++it == cmd.end()) //-type是最后一个,后面没有参数,这是可以的
          break;
      string str = *(it);                                                              // 找到-type的下一位
      if (str == "-name" || str == "-begin" || str == "-priority" || str == "-remind") //-type后不跟参数,这是可以的
          continue;
      if (str != "entertainment" && str != "sport" && str != "study" && str != "routine")
          return false;
      task.type = str;
      it++;
    }
    else if (*it == "-remind")
    {
      if (++it == cmd.end()) //-remind是最后一个,后面没有参数
          return false;
      string str = *(it); // 找到-remind的下一位
      if (str[0] == '-')  // 不符合
          return false;
      if (!isValidDate(str)) // 时间参数不符合要求
          return false;
      task.begin_time = to_time_t(str);
      it++;
    }
    else
    {
      cout << "Invalid command" << endl;
      return false;
    }
  }

  int id = to_time_t(begin); // 得到id后打包进行add
  if (!task_list.Add(make_pair(id, task)))
  {
    cout << "Faliure" << endl;
    return false;
  }

  return true;
}
