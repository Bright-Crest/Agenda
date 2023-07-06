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


// 用户输入格式:addtask -n name -b begin -p priority -t type -r remind (注意:用户输入的参数不能以-开头,priority默认为0,type默认为" ",选项的顺序可以打乱)
bool AddTaskOp(TaskList &task_list, list<string> cmd)
{
  Other task;
  task.priority = 0;
  task.type = " ";
  string begin;                              // begin用于保存begin_time_并分配唯一的id
  list<string>::iterator it = ++cmd.begin(); // 从cmd的第二个string开始参考

  while (it != cmd.end())
  {
    if (*it == "-n")
    {
      if (++it == cmd.end()) //-name是最后一个,后面没有参数
      {
        cout << "No parameters after -n" << endl;
        return false;
      }
      string str = *(it); // 找到-name的下一位
      if (str[0] == '-')  // 不符合
      {
        cout << "No parameters after -n" << endl;
        return false;
      }

      task.name = str;
      it++;
    }
    else if (*it == "-b")
    {
      if (++it == cmd.end()) //-begin是最后一个,后面没有参数
      {
        return false;
        cout << "No parameters after -b" << endl;
      }
      begin = *(it);       // 找到-begin的下一位
      if (begin[0] == '-') // 不符合
      {
        cout << "No parameters after -b" << endl;
        return false;
      }
      if (!isValidDate(begin)) // 时间参数不符合要求
      {
        cout << "The parameter does not meet the requirements" << endl;
        return false;
      }

      task.begin_time = to_time_t(begin);
      it++;
    }
    else if (*it == "-p")
    {
      if (++it == cmd.end()) //-priority是最后一个,后面没有参数,这是可以的
        break;
      string str = *(it);                                           // 找到-priority的下一位
      if (str == "-n" || str == "-b" || str == "-t" || str == "-r") //-priority后不跟参数,这是可以的
      {
        it++;
        continue;
      }
      int p = stoi(str);
      if (p != 1 && p != 2 && p != 4)
      {
        cout << "The parameters do not meet the requirements";
        return false;
      }

      task.priority = p;
      it++;
    }
    else if (*it == "-t")
    {
      if (++it == cmd.end()) //-type是最后一个,后面没有参数,这是可以的
        break;
      string str = *(it);                                           // 找到-type的下一位
      if (str == "-n" || str == "-b" || str == "-p" || str == "-r") //-type后不跟参数,这是可以的
      {
        it++;
        continue;
      }

      task.type = str;
      it++;
    }
    else if (*it == "-r")
    {
      if (++it == cmd.end()) //-remind是最后一个,后面没有参数
      {
        cout << "No parameters after -r" << endl;
        return false;
      }
      string str = *(it); // 找到-remind的下一位
      if (str[0] == '-')  // 不符合
      {
        cout << "No parameters after -r" << endl;
        return false;
      }
      if (!isValidDate(str)) // 时间参数不符合要求
      {
        cout << "The parameters do not meet the requirements";
        return false;
      }

      task.remind_time = to_time_t(str);
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

// 重要规定:用于查找的选项是大写,用于修改的选项是小写!
//  用户输入格式: modifytask -N name -I id -n new_name -b begin -p priority -t type -r remind(注意:-N和-I至少有1个,-p,-t后面可以没有参数,-n,-b,-r后面必须有参数,id不可以修改)
bool ModifyTaskOp(TaskList &task_list, list<string> cmd)
{
  string name = "-"; // 用于查找的name,默认是"-"
  int id = -1;       // 用于查找的id
  Other task;
  task.name = "-"; // 默认是"-",若发现没有通过命令修改,则copy查找到的任务的name,其他的属性同理
  task.begin_time = -1;
  task.priority = 0;
  task.type = " ";
  task.remind_time = -1;
  list<string>::iterator it = ++cmd.begin(); // 从cmd的第二个string开始参考

  while (it != cmd.end())
  {
    if (*it == "-n") // 修改所用的name
    {
      if (++it == cmd.end()) //-name是最后一个,后面没有参数
      {
        cout << "No parameters after -n" << endl;
        return false;
      }
      string str = *(it); // 找到-name的下一位
      if (str[0] == '-')  // 不符合
      {
        cout << "No parameters after -n" << endl;
        return false;
      }

      task.name = str;
      it++;
    }
    else if (*it == "-N") // 查找所用的name
    {
      if (++it == cmd.end()) //-name是最后一个,后面没有参数
      {
        cout << "No parameters after -N" << endl;
        return false;
      }
      string str = *(it); // 找到-name的下一位
      if (str[0] == '-')  // 不符合
      {
        cout << "No parameters after -N" << endl;
        return false;
      }

      name = str;
      it++;
    }
    else if (*it == "-I") // 查找所用的id
    {
      if (++it == cmd.end()) //-id是最后一个,后面没有参数
      {
        cout << "No parameters after -I" << endl;
        return false;
      }
      string str = *(it); // 找到-name的下一位
      if (str[0] == '-')  // 不符合
      {
        cout << "No parameters after -I" << endl;
        return false;
      }

      id = stoi(str); // 保存需要处理的id,为之后的查找做准备
      it++;
    }
    else if (*it == "-b")
    {
      if (++it == cmd.end()) //-begin是最后一个,后面没有参数
      {
        return false;
        cout << "No parameters after -b" << endl;
      }
      string str = *(it); // 找到-begin的下一位
      if (str[0] == '-')  // 不符合
      {
        cout << "No parameters after -b" << endl;
        return false;
      }
      if (!isValidDate(str)) // 时间参数不符合要求
      {
        cout << "The parameter does not meet the requirements" << endl;
        return false;
      }

      task.begin_time = to_time_t(str);
      it++;
    }
    else if (*it == "-p")
    {
      if (++it == cmd.end()) //-priority是最后一个,后面没有参数,这是可以的
        break;
      string str = *(it);                                                                         // 找到-priority的下一位
      if (str == "-n" || str == "-b" || str == "-t" || str == "-r" || str == "-N" || str == "-I") //-priority后不跟参数,这是可以的
      {
        it++;
        continue;
      }
      int p = stoi(str);
      if (p != 1 && p != 2 && p != 4)
      {
        cout << "The parameters do not meet the requirements";
        return false;
      }

      task.priority = p;
      it++;
    }
    else if (*it == "-t")
    {
      if (++it == cmd.end()) //-type是最后一个,后面没有参数,这是可以的
        break;
      string str = *(it);                                                                         // 找到-type的下一位
      if (str == "-n" || str == "-b" || str == "-p" || str == "-r" || str == "-N" || str == "-I") //-type后不跟参数,这是可以的
      {
        it++;
        continue;
      }

      task.type = str;
      it++;
    }
    else if (*it == "-r")
    {
      if (++it == cmd.end()) //-remind是最后一个,后面没有参数
      {
        cout << "No parameters after -r" << endl;
        return false;
      }
      string str = *(it); // 找到-remind的下一位
      if (str[0] == '-')  // 不符合
      {
        cout << "No parameters after -r" << endl;
        return false;
      }
      if (!isValidDate(str)) // 时间参数不符合要求
      {
        cout << "The parameters do not meet the requirements";
        return false;
      }

      task.remind_time = to_time_t(str);
      it++;
    }
    else
    {
      cout << "Invalid command" << endl;
      return false;
    }
  }

  vector<pair<int, Other>>::iterator it1 = task_list.FindTask(id);
  vector<pair<int, Other>>::iterator it2 = task_list.FindTask(name);
  bool flag1 = it1 != task_list.return_end();
  bool flag2 = it2 != task_list.return_end();
  if ((flag1 && flag2 && it1 == it2) || (!flag1 && flag2) || (flag1 && !flag2)) // 3种符合查找要求的情况
  {
    task_list.Erase(name); // 删除对应的任务
    vector<pair<int, Other>>::iterator itt;
    if (flag1)
      itt = it1;
    else
      itt = it2;
    if (task.name == "-")
      task.name = itt->second.name;
    if (task.begin_time == -1)
      task.begin_time = itt->second.begin_time;
    if (task.type == " ")
      task.type = itt->second.type;
    if (task.priority == 0)
      task.priority = itt->second.priority;
    if (task.remind_time == -1)
      task.remind_time = itt->second.remind_time;
    if (!task_list.Add(make_pair(id, task))) // 添加相应的任务
    {
      cout << "Faliure" << endl;
      return false;
    }

    return true;
  }
  else
  {
    return false;
  }
}

// 使用说明: searchtask -N name -I id(-N和-I至少有一个)
bool SearchTaskOp(TaskList &task_list, list<string> cmd)
{
  string name = "-"; // 保存需要查找的name,默认为"-"
  int id = -1;
  list<string>::iterator it = ++cmd.begin(); // 从cmd的第二个string开始参考

  while (it != cmd.end())
  {
    if (*it == "-N") // 查找所用的name
    {
      if (++it == cmd.end()) //-name是最后一个,后面没有参数
      {
        cout << "No parameters after -N" << endl;
        return false;
      }
      string str = *(it); // 找到-name的下一位
      if (str[0] == '-')  // 不符合
      {
        cout << "No parameters after -N" << endl;
        return false;
      }

      name = str;
      it++;
    }
    else if (*it == "-I") // 查找所用的id
    {
      if (++it == cmd.end()) //-id是最后一个,后面没有参数
      {
        cout << "No parameters after -I" << endl;
        return false;
      }
      string str = *(it); 
      if (str[0] == '-')  // 不符合
      {
        cout << "No parameters after -I" << endl;
        return false;
      }

      id = stoi(str); // 保存需要处理的id,为之后的查找做准备
      it++;
    }
    else
    {
      cout << "Invalid command" << endl;
      return false;
    }
  }
  vector<pair<int, Other>>::iterator it1 = task_list.FindTask(id);
  vector<pair<int, Other>>::iterator it2 = task_list.FindTask(name);
  bool flag1 = it1 != task_list.return_end();
  bool flag2 = it2 != task_list.return_end();
  if ((flag1 && flag2 && it1 == it2) || (!flag1 && flag2) || (flag1 && !flag2)) // 3种符合查找要求的情况,输出任务信息
  {
    if (flag1)
      task_list.FindShow(id);
    else
      task_list.FindShow(name);

    return true;
  }
  else
    return false;
}

// 使用说明:showtask -S start -E end -P priority -T type -A i(id)/b(begin)/p(priority)/r(remind) -D (其中-S,-E,-P,-T均可以缺省,-A,-D至多有1个,后面必须有参数)
bool ShowTaskOp(TaskList &task_list, list<string> cmd)
{
  list<string>::iterator it = ++cmd.begin(); // 从cmd的第二个string开始
  int begin_time = -1;
  int end_time = -1;
  int priority = 0;
  string type = " ";
  static bool (*func)(pair<int, Other> task1, pair<int, Other> task2)=LessBegin;

  while (it != cmd.end())
  {
    if (*it == "-S")
    {
      if (++it == cmd.end()) //-S是最后一个,后面没有参数,因为begin有缺省值,因此这是可以的
        break;
      string str = *(it);                                                          // 找到-S的下一位
      if (str == "-E" || str == "-P" || str == "-T"||str == "-A"||str == "-D") //-S后不跟参数,这是可以的
      {
        it++;
        continue;
      }
      if (!isValidDate(str)) // 时间参数不符合要求
      {
        cout << "The parameter does not meet the requirements" << endl;
        return false;
      }

      begin_time = to_time_t(str);
      it++;
    }
    else if (*it == "-E")
    {
      if (++it == cmd.end()) //-E是最后一个,后面没有参数,因为end有缺省值,因此这是可以的
        break;
      string str = *(it);                                                          // 找到-S的下一位
      if (str == "-S" || str == "-P" || str == "-T"||str == "-A"||str == "-D") //-S后不跟参数,这是可以的
      {
        it++;
        continue;
      }
      if (!isValidDate(str)) // 时间参数不符合要求
      {
        cout << "The parameter does not meet the requirements" << endl;
        return false;
      }

      end_time = to_time_t(str);
      it++;
    }
    else if (*it == "-P")
    {
      if (++it == cmd.end()) //-P是最后一个,后面没有参数,这是可以的
        break;
      string str = *(it);
      if (str == "-S" || str == "-E" || str == "-T"||str == "-A"||str == "-D") //-P后不跟参数,这是可以的
      {
        it++;
        continue;
      }
      int p = stoi(str);
      if (p != 1 && p != 2 && p != 3 && p != 4 && p != 5 && p != 6 && p != 7)
      {
        cout << "The parameters do not meet the requirements";
        return false;
      }

      priority = p;
      it++;
    }
    else if (*it == "-T")
    {
      if (++it == cmd.end()) //-T是最后一个,后面没有参数,这是可以的
        break;
      string str = *(it);
      if (str == "-S" || str == "-E" || str == "-P"||str == "-A"||str == "-D") //-T后不跟参数,这是可以的
      {
        it++;
        continue;
      }

      type = str;
      it++;
    }
    else if(*it=="-A")
    {
      if(++it==cmd.end())
      {
        cout << "No parameters after -A" << endl;
        return false;
      }
      string str = *(it); 
      if (str[0] == '-')  // 不符合
      {
        cout << "No parameters after -A" << endl;
        return false;
      }
      if(str!="b"&&str!="begin"&&str!="i"&&str!="id"&&str!="p"&&str!="priority"&&str!="r"&&str!="remind")
      {
        cout << "The parameter does not meet the requirements" << endl;
        return false;
      }

      if(str=="b"||str!="begin")
          func=GreaterBegin;
      else if(str=="i"||str!="id")
          func=GreaterId;
      else if(str=="p"||str!="priority")
          func=GreaterPriority;
      else
          func=GreaterRemind;
      
      it++;
    }
    else if(*it=="-D")
    {
      if(++it==cmd.end())
      {
        cout << "No parameters after -D" << endl;
        return false;
      }
      string str = *(it); 
      if (str[0] == '-')  // 不符合
      {
        cout << "No parameters after -D" << endl;
        return false;
      }
      if(str!="b"&&str!="begin"&&str!="i"&&str!="id"&&str!="p"&&str!="priority"&&str!="r"&&str!="remind")
      {
        cout << "The parameter does not meet the requirements" << endl;
        return false;
      }

      if(str=="b"||str!="begin")
          func=LessBegin;
      else if(str=="i"||str!="id")
          func=LessId;
      else if(str=="p"||str!="priority")
          func=LessPriority;
      else
          func=LessRemind;
      
      it++;
    }
    else
    {
      cout << "Invalid command" << endl;
      return false;
    }
  }

  if (begin_time == -1)
    begin_time = 0;
  if (end_time == -1)
    end_time = pow(2, 31) - 1;
  if (priority == 0)
    priority = 7;
  if (type == " ")
    task_list.Show(begin_time, end_time, priority,func);
  else
    task_list.Show(type, begin_time, end_time, priority,func);

  return true;
}

// 使用说明:deltask -n name -i id -a(-n,-i可以均没有,-a代表删除全部任务)
bool DeleteTaskOp(TaskList &task_list, list<string> cmd)
{
  string name = "-"; // 保存需要查找的name,默认为"-"
  int id = -1;
  list<string>::iterator it = ++cmd.begin(); // 从cmd的第二个string开始参考
  bool flagForAll = false;

  while (it != cmd.end())
  {
    if (*it == "-n") // 查找所用的name
    {
      if (++it == cmd.end()) //-n是最后一个,后面没有参数
      {
        cout << "No parameters after -n" << endl;
        return false;
      }
      string str = *(it); // 找到-name的下一位
      if (str[0] == '-')  // 不符合
      {
        cout << "No parameters after -n" << endl;
        return false;
      }

      name = str;
      it++;
    }
    else if (*it == "-i") // 查找所用的id
    {
      if (++it == cmd.end()) //-id是最后一个,后面没有参数
      {
        cout << "No parameters after -i" << endl;
        return false;
      }
      string str = *(it); // 找到-name的下一位
      if (str[0] == '-')  // 不符合
      {
        cout << "No parameters after -i" << endl;
        return false;
      }

      id = stoi(str); // 保存需要处理的id,为之后的查找做准备
      it++;
    }
    else if (*it == "-a")
    {
      list<string>::iterator it1 = ++it;
      if (it1 == cmd.end() || *it1 == "-n" || *it1 == "-i")
        flagForAll = true;
      else
        return false;
        
      it=it1;
    }
    else
    {
      cout << "Invalid command" << endl;
      return false;
    }

    if (flagForAll)
    {
      task_list.Clear(); // 删除全部任务
      return true;
    }

    vector<pair<int, Other>>::iterator it1 = task_list.FindTask(id);
    vector<pair<int, Other>>::iterator it2 = task_list.FindTask(name);
    bool flag1 = it1 != task_list.return_end();
    bool flag2 = it2 != task_list.return_end();
    if ((flag1 && flag2 && it1 == it2) || (!flag1 && flag2) || (flag1 && !flag2)) // 3种符合查找要求的情况,输出任务信息
    {
      if (flag1)
        task_list.Erase(id);
      else
        task_list.Erase(name);

      return true;
    }
    else
      return false;
  }
}
