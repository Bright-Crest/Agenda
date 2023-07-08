
#include "cmd_processor.h"

#include <iostream>
#include <list>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

#include "task_list.h"
#include "tools.h"

using namespace std;

extern mutex mtx;

bool CmdProcessor::GetArgv(int argc, const char* argv[], int start) {
  cmd_.clear();
  if (start >= argc) {
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

// check cmd and do sth accordingly; return: 1 -- correct cmd, 0 -- wrong cmd,
// -1 -- quit.
int CmdProcessor::CmdDistributor(TaskList& task_list) const {
  const int kSize = cmd_.size();
  if (kSize < 1) return 0;

  bool flag = false;
  string first_cmd = cmd_.front();
  to_lower(first_cmd);

  if (first_cmd == "addtask" || first_cmd == "add") {
    lock_guard<mutex> lck(mtx);
    if (kSize == 1)
      flag = AddTaskNoOp(task_list);
    else
      flag = AddTaskOp(task_list, cmd_);
  } else if (first_cmd == "modifytask" || first_cmd == "modify") {
    lock_guard<mutex> lck(mtx);
    if (kSize == 1)
      flag = ModifyTaskNoOp(task_list);
    else
      flag = ModifyTaskOp(task_list, cmd_);
  } else if (first_cmd == "deletetask" || first_cmd == "delete") {
    lock_guard<mutex> lck(mtx);
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
  } else if (first_cmd == "login" || first_cmd == "li" ||
             first_cmd == "createaccount" || first_cmd == "ca" ||
             first_cmd == "changepassword" || first_cmd == "changepw" ||
             first_cmd == "cp" || first_cmd == "deleteaccount" ||
             first_cmd == "da") {
    cout << "Please quit first and then restart the program to \"" << first_cmd
         << "\".\n";
  } else {
    cout << "\"" << cmd_.front() << "\" is not a valid command.\n";
    return 0;
  }
  return (int)flag;
}

// �û������ʽ:addtask -n name -b begin -p priority -t type -r remind
// (ע��:�û�����Ĳ���������-��ͷ,priorityĬ��Ϊ0,typeĬ��Ϊ"
// ",ѡ���˳����Դ���)
bool AddTaskOp(TaskList& task_list, list<string> cmd) {
  Other task;
  task.priority = 0;
  task.type = " ";
  string begin;  // begin���ڱ���begin_time_������Ψһ��id
  list<string>::iterator it = ++cmd.begin();  // ��cmd�ĵڶ���string��ʼ�ο�

  while (it != cmd.end()) {
    if (*it == "-n") {
      if (++it == cmd.end())  //-name�����һ��,����û�в���
      {
        cout << "No parameters after -n" << endl;
        return false;
      }
      string str = *(it);  // �ҵ�-name����һλ
      if (str[0] == '-')   // ������
      {
        cout << "No parameters after -n" << endl;
        return false;
      }

      task.name = str;
      it++;
    } else if (*it == "-b") {
      if (++it == cmd.end())  //-begin�����һ��,����û�в���
      {
        return false;
        cout << "No parameters after -b" << endl;
      }
      begin = *(it);        // �ҵ�-begin����һλ
      if (begin[0] == '-')  // ������
      {
        cout << "No parameters after -b" << endl;
        return false;
      }
      if (!isValidDate(begin))  // ʱ�����������Ҫ��
      {
        cout << "The parameter does not meet the requirements" << endl;
        return false;
      }

      task.begin_time = to_time_t(begin);

      it++;
    } else if (*it == "-p") {
      if (++it == cmd.end())  //-priority�����һ��,����û�в���,���ǿ��Ե�
        break;
      string str = *(it);  // �ҵ�-priority����һλ
      if (str == "-n" || str == "-b" || str == "-t" ||
          str == "-r")  //-priority�󲻸�����,���ǿ��Ե�
      {
        it++;
        continue;
      }
      int p = stoi(str);
      if (p != 1 && p != 2 && p != 4) {
        cout << "The parameters do not meet the requirements";
        return false;
      }

      task.priority = p;
      it++;
    } else if (*it == "-t") {
      if (++it == cmd.end())  //-type�����һ��,����û�в���,���ǿ��Ե�
        break;
      string str = *(it);  // �ҵ�-type����һλ
      if (str == "-n" || str == "-b" || str == "-p" ||
          str == "-r")  //-type�󲻸�����,���ǿ��Ե�
      {
        it++;
        continue;
      }

      task.type = str;
      it++;
    } else if (*it == "-r") {
      if (++it == cmd.end())  //-remind�����һ��,����û�в���
      {
        cout << "No parameters after -r" << endl;
        return false;
      }
      string str = *(it);  // �ҵ�-remind����һλ
      if (str[0] == '-')   // ������
      {
        cout << "No parameters after -r" << endl;
        return false;
      }
      if (!isValidDate(str))  // ʱ�����������Ҫ��
      {
        cout << "The parameters do not meet the requirements";
        return false;
      }

      task.remind_time = to_time_t(str);
      it++;
    } else {
      cout << "Invalid command" << endl;
      return false;
    }
  }

  int id = (to_time_t(begin)) % 997;  // �õ�id��������add
  vector<pair<int, Other>>::iterator it1 = task_list.FindTask(id);
  while (it1 != task_list.return_end()) {
    id = (id + 1) % 997;
    it1 = task_list.FindTask(id);
  }
  if (task_list.Add(make_pair(id, task))) {
    cout << "Faliure" << endl;
    return false;
  }

  return true;
}

// ��Ҫ�涨:���ڲ��ҵ�ѡ���Ǵ�д,�����޸ĵ�ѡ����Сд!
//  �û������ʽ: modifytask -N name -I id -n new_name -b begin -p priority -t
//  type -r
//  remind(ע��:-N��-I������1��,-p,-t�������û�в���,-n,-b,-r��������в���,id�������޸�)
bool ModifyTaskOp(TaskList& task_list, list<string> cmd) {
  string name = "-";  // ���ڲ��ҵ�name,Ĭ����"-"
  int id = -1;        // ���ڲ��ҵ�id
  Other task;
  task.name =
      "-";  // Ĭ����"-",������û��ͨ�������޸�,��copy���ҵ��������name,����������ͬ��
  task.begin_time = -1;
  task.priority = 0;
  task.type = " ";
  task.remind_time = -1;
  list<string>::iterator it = ++cmd.begin();  // ��cmd�ĵڶ���string��ʼ�ο�

  while (it != cmd.end()) {
    if (*it == "-n")  // �޸����õ�name
    {
      if (++it == cmd.end())  //-name�����һ��,����û�в���
      {
        cout << "No parameters after -n" << endl;
        return false;
      }
      string str = *(it);  // �ҵ�-name����һλ
      if (str[0] == '-')   // ������
      {
        cout << "No parameters after -n" << endl;
        return false;
      }

      task.name = str;
      it++;
    } else if (*it == "-N")  // �������õ�name
    {
      if (++it == cmd.end())  //-name�����һ��,����û�в���
      {
        cout << "No parameters after -N" << endl;
        return false;
      }
      string str = *(it);  // �ҵ�-name����һλ
      if (str[0] == '-')   // ������
      {
        cout << "No parameters after -N" << endl;
        return false;
      }

      name = str;
      it++;
    } else if (*it == "-I")  // �������õ�id
    {
      if (++it == cmd.end())  //-id�����һ��,����û�в���
      {
        cout << "No parameters after -I" << endl;
        return false;
      }
      string str = *(it);  // �ҵ�-name����һλ
      if (str[0] == '-')   // ������
      {
        cout << "No parameters after -I" << endl;
        return false;
      }

      id = stoi(str);  // ������Ҫ�����id,Ϊ֮��Ĳ�����׼��
      it++;
    } else if (*it == "-b") {
      if (++it == cmd.end())  //-begin�����һ��,����û�в���
      {
        return false;
        cout << "No parameters after -b" << endl;
      }
      string str = *(it);  // �ҵ�-begin����һλ
      if (str[0] == '-')   // ������
      {
        cout << "No parameters after -b" << endl;
        return false;
      }
      if (!isValidDate(str))  // ʱ�����������Ҫ��
      {
        cout << "The parameter does not meet the requirements" << endl;
        return false;
      }

      task.begin_time = to_time_t(str);
      it++;
    } else if (*it == "-p") {
      if (++it == cmd.end())  //-priority�����һ��,����û�в���,���ǿ��Ե�
        break;
      string str = *(it);  // �ҵ�-priority����һλ
      if (str == "-n" || str == "-b" || str == "-t" || str == "-r" ||
          str == "-N" || str == "-I")  //-priority�󲻸�����,���ǿ��Ե�
        continue;
      int p = stoi(str);
      if (p != 1 && p != 2 && p != 4) {
        cout << "The parameters do not meet the requirements";
        return false;
      }

      task.priority = p;
      it++;
    } else if (*it == "-t") {
      if (++it == cmd.end())  //-type�����һ��,����û�в���,���ǿ��Ե�
        break;
      string str = *(it);
      if (str == "-n" || str == "-b" || str == "-p" || str == "-r" ||
          str == "-N" || str == "-I")  //-type�󲻸�����,���ǿ��Ե�
        continue;
      task.type = str;
      it++;
    } else if (*it == "-r") {
      if (++it == cmd.end())  //-remind�����һ��,����û�в���
      {
        cout << "No parameters after -r" << endl;
        return false;
      }
      string str = *(it);  // �ҵ�-remind����һλ
      if (str[0] == '-')   // ������
      {
        cout << "No parameters after -r" << endl;
        return false;
      }
      if (!isValidDate(str))  // ʱ�����������Ҫ��
      {
        cout << "The parameters do not meet the requirements";
        return false;
      }

      task.remind_time = to_time_t(str);
      it++;
    } else {
      cout << "Invalid command" << endl;
      return false;
    }
  }

  int id1;  // �����޸������idֵ
  vector<pair<int, Other>>::iterator it1 = task_list.FindTask(id);
  vector<pair<int, Other>>::iterator it2 = task_list.FindTask(name);
  bool flag1 = it1 != task_list.return_end();
  bool flag2 = it2 != task_list.return_end();
  if ((flag1 && flag2 && it1 == it2) || (!flag1 && flag2) ||
      (flag1 && !flag2))  // 3�ַ��ϲ���Ҫ������
  {
    vector<pair<int, Other>>::iterator itt;
    if (flag1)
      itt = it1;
    else
      itt = it2;
    id1 = itt->first;
    if (task.name == "-") task.name = itt->second.name;
    if (task.begin_time == -1) task.begin_time = itt->second.begin_time;
    if (task.type == " ") task.type = itt->second.type;
    if (task.priority == 0) task.priority = itt->second.priority;
    if (task.remind_time == -1) task.remind_time = itt->second.remind_time;

    if (flag1)
      task_list.Erase(id);  // ɾ����Ӧ������
    else
      task_list.Erase(name);

    if (!task_list.Add(make_pair(id1, task)))  // �����Ӧ������
    {
      cout << "Faliure" << endl;
      return false;
    }

    task_list.saveFile();

    return true;
  } else {
    task_list.saveFile();

    return false;
  }
}

// ʹ��˵��: searchtask -N name -I id(-N��-I������һ��)
bool SearchTaskOp(TaskList& task_list, list<string> cmd) {
  string name = "-";  // ������Ҫ���ҵ�name,Ĭ��Ϊ"-"
  int id = -1;
  list<string>::iterator it = ++cmd.begin();  // ��cmd�ĵڶ���string��ʼ�ο�

  while (it != cmd.end()) {
    if (*it == "-N")  // �������õ�name
    {
      if (++it == cmd.end())  //-name�����һ��,����û�в���
      {
        cout << "No parameters after -N" << endl;
        return false;
      }
      string str = *(it);  // �ҵ�-name����һλ
      if (str[0] == '-')   // ������
      {
        cout << "No parameters after -N" << endl;
        return false;
      }

      name = str;
      it++;
    } else if (*it == "-I")  // �������õ�id
    {
      if (++it == cmd.end())  //-id�����һ��,����û�в���
      {
        cout << "No parameters after -I" << endl;
        return false;
      }
      string str = *(it);
      if (str[0] == '-')  // ������
      {
        cout << "No parameters after -I" << endl;
        return false;
      }

      id = stoi(str);  // ������Ҫ�����id,Ϊ֮��Ĳ�����׼��
      it++;
    } else {
      cout << "Invalid command" << endl;
      return false;
    }
  }
  vector<pair<int, Other>>::iterator it1 = task_list.FindTask(id);
  vector<pair<int, Other>>::iterator it2 = task_list.FindTask(name);
  bool flag1 = it1 != task_list.return_end();
  bool flag2 = it2 != task_list.return_end();
  if ((flag1 && flag2 && it1 == it2) || (!flag1 && flag2) ||
      (flag1 && !flag2))  // 3�ַ��ϲ���Ҫ������,���������Ϣ
  {
    if (flag1)
      task_list.FindShow(id);
    else
      task_list.FindShow(name);

    return true;
  } else
    return false;
}

// ʹ��˵��:showtask -S start -E end -P priority -T type -A
// i(id)/b(begin)/p(priority)/r(remind) -D
// (����-S,-E,-P,-T������ȱʡ,-A,-D������1��,��������в���)
bool ShowTaskOp(TaskList& task_list, list<string> cmd) {
  list<string>::iterator it = ++cmd.begin();  // ��cmd�ĵڶ���string��ʼ
  int begin_time = -1;
  int end_time = -1;
  int priority = 0;
  string type = " ";
  static bool (*func)(pair<int, Other> task1, pair<int, Other> task2) =
      LessBegin;

  while (it != cmd.end()) {
    if (*it == "-S") {
      if (++it ==
          cmd.end())  //-S�����һ��,����û�в���,��Ϊbegin��ȱʡֵ,������ǿ��Ե�
        break;
      string str = *(it);  // �ҵ�-S����һλ
      if (str == "-E" || str == "-P" || str == "-T" || str == "-A" ||
          str == "-D")  //-S�󲻸�����,���ǿ��Ե�
        continue;
      if (!isValidDate(str))  // ʱ�����������Ҫ��
      {
        cout << "The parameter does not meet the requirements" << endl;
        return false;
      }

      begin_time = to_time_t(str);
      it++;
    } else if (*it == "-E") {
      if (++it ==
          cmd.end())  //-E�����һ��,����û�в���,��Ϊend��ȱʡֵ,������ǿ��Ե�
        break;
      string str = *(it);  // �ҵ�-S����һλ
      if (str == "-S" || str == "-P" || str == "-T" || str == "-A" ||
          str == "-D")  //-S�󲻸�����,���ǿ��Ե�
        continue;
      if (!isValidDate(str))  // ʱ�����������Ҫ��
      {
        cout << "The parameter does not meet the requirements" << endl;
        return false;
      }

      end_time = to_time_t(str);
      it++;
    } else if (*it == "-P") {
      if (++it == cmd.end())  //-P�����һ��,����û�в���,���ǿ��Ե�
        break;
      string str = *(it);
      if (str == "-S" || str == "-E" || str == "-T" || str == "-A" ||
          str == "-D")  //-P�󲻸�����,���ǿ��Ե�
        continue;
      int p = stoi(str);
      if (p != 1 && p != 2 && p != 3 && p != 4 && p != 5 && p != 6 && p != 7) {
        cout << "The parameters do not meet the requirements";
        return false;
      }

      priority = p;
      it++;
    } else if (*it == "-T") {
      if (++it == cmd.end())  //-T�����һ��,����û�в���,���ǿ��Ե�
        break;
      string str = *(it);
      if (str == "-S" || str == "-E" || str == "-P" || str == "-A" ||
          str == "-D")  //-T�󲻸�����,���ǿ��Ե�
        continue;

      type = str;
      it++;
    } else if (*it == "-A") {
      if (++it == cmd.end()) {
        cout << "No parameters after -A" << endl;
        return false;
      }
      string str = *(it);
      if (str[0] == '-')  // ������
      {
        cout << "No parameters after -A" << endl;
        return false;
      }
      if (str != "b" && str != "begin" && str != "i" && str != "id" &&
          str != "p" && str != "priority" && str != "r" && str != "remind") {
        cout << "The parameter does not meet the requirements" << endl;
        return false;
      }

      if (str == "b" || str == "begin")
        func = GreaterBegin;
      else if (str == "i" || str == "id")
        func = GreaterId;
      else if (str == "p" || str == "priority")
        func = GreaterPriority;
      else
        func = GreaterRemind;

      it++;
    } else if (*it == "-D") {
      if (++it == cmd.end()) {
        cout << "No parameters after -D" << endl;
        return false;
      }
      string str = *(it);
      if (str[0] == '-')  // ������
      {
        cout << "No parameters after -D" << endl;
        return false;
      }
      if (str != "b" && str != "begin" && str != "i" && str != "id" &&
          str != "p" && str != "priority" && str != "r" && str != "remind") {
        cout << "The parameter does not meet the requirements" << endl;
        return false;
      }

      if (str == "b" || str == "begin")
        func = LessBegin;
      else if (str == "i" || str == "id")
        func = LessId;
      else if (str == "p" || str == "priority")
        func = LessPriority;
      else
        func = LessRemind;

      it++;
    } else {
      cout << "Invalid command" << endl;
      return false;
    }
  }

  if (begin_time == -1) begin_time = 0;
  if (end_time == -1) end_time = pow(2, 31) - 1;
  if (priority == 0) priority = 7;
  if (type == " ")
    task_list.Show(begin_time, end_time, priority, func);
  else
    task_list.Show(type, begin_time, end_time, priority, func);

  return true;
}

// ʹ��˵��:deltask -n name -i id -a(-n,-i���Ծ�û��,-a����ɾ��ȫ������)
bool DeleteTaskOp(TaskList& task_list, list<string> cmd) {
  string name = "-";  // ������Ҫ���ҵ�name,Ĭ��Ϊ"-"
  int id = -1;
  list<string>::iterator it = ++cmd.begin();  // ��cmd�ĵڶ���string��ʼ�ο�
  bool flagForAll = false;

  while (it != cmd.end()) {
    if (*it == "-n")  // �������õ�name
    {
      if (++it == cmd.end())  //-n�����һ��,����û�в���
      {
        cout << "No parameters after -n" << endl;
        return false;
      }
      string str = *(it);  // �ҵ�-name����һλ
      if (str[0] == '-')   // ������
      {
        cout << "No parameters after -n" << endl;
        return false;
      }

      name = str;
      it++;
    } else if (*it == "-i")  // �������õ�id
    {
      if (++it == cmd.end())  //-id�����һ��,����û�в���
      {
        cout << "No parameters after -i" << endl;
        return false;
      }
      string str = *(it);  // �ҵ�-name����һλ
      if (str[0] == '-')   // ������
      {
        cout << "No parameters after -i" << endl;
        return false;
      }

      id = stoi(str);  // ������Ҫ�����id,Ϊ֮��Ĳ�����׼��
      it++;
    } else if (*it == "-a") {
      list<string>::iterator it1 = ++it;
      if (it1 == cmd.end() || *it1 == "-n" || *it1 == "-i")
        flagForAll = true;
      else
        return false;

      it = it1;
    } else {
      cout << "Invalid command" << endl;
      return false;
    }
  }

  vector<pair<int, Other>>::iterator it1 = task_list.FindTask(id);
  vector<pair<int, Other>>::iterator it2 = task_list.FindTask(name);
  bool flag1 = it1 != task_list.return_end();
  bool flag2 = it2 != task_list.return_end();

  bool flagForErase = (flag1 && flag2 && it1 == it2) || (!flag1 && flag2) ||
                      (flag1 && !flag2);  // ָ��-n��-i��ȷ

  if (flagForErase && flagForAll)  // ɾ��ȫ������
  {
    task_list.Clear();
    task_list.saveFile();

    return true;
  } else if (flagForErase && !flagForAll) {
    if (flag1)
      task_list.Erase(id);
    else
      task_list.Erase(name);

    task_list.saveFile();

    return true;
  } else {
    task_list.saveFile();

    return false;
  }
}


bool AddTaskNoOp(TaskList& task_list){
    // �����������
    Other newtask;
    std::cout << "Please input the name of the task" << endl;
    std::cin >> newtask.name;
    // �������Ŀ�ʼʱ��
    std::cout << "Please input the begin time of the task" << endl;
    std::cout << "Time format should be 2022/02/02/03:00:00" << endl;
    string begin_t;
    bool valid_input = false;
    while (!valid_input)//�������Ŀ�ʼʱ���ǲ���%Y/%M/%D/%h:%m:%s����ʽ
    {
        std::cin >> begin_t;
        if (isValidDate(begin_t))//������˳�ѭ��
            valid_input = true;
        else
            std::cout << "Invalid format. Please try again." << endl;//����һֱ����ֱ����Ϊֹ
    }
    newtask.begin_time = to_time_t(begin_t);//���û��������ʽת����time_t��ʽ����
    // �����������ȼ�
    std::cout << "Please input the priority of the task" << endl;
    std::cout << "1-low, 2-medium, 4-high, default value or other value-low" << endl;
    std::cin >> newtask.priority;
    // ������ȼ��ǲ��Ƿ���1��2��4
    while(!std::cin){
      std::cin.clear(); // ���������״̬��־
      std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ����ʣ�������

      std::cout << "Invalid input. Please enter a valid priority: " << endl;
      std::cin >> newtask.priority;
    }
    if(newtask.priority != 1 && newtask.priority != 2 && newtask.priority != 4)
      newtask.priority = 1;
    // ������������
    std::cout << "Please input the type of the task, such as: entertainment  sport  study  routine" << endl;
    std::cout << "If you want a default type, please enter -." << endl;
    std::cin >> newtask.type;
    if(newtask.type[0] == '-')
      newtask.type = "default";
    // ������������ʱ��
    std::cout << "Please input the remind time of the task" << endl;
    std::cout << "Time format should be 2022/02/02/03:00:00" << endl;
    string remind_t;
    valid_input = false;
    //�������Ŀ�ʼʱ���ǲ���%Y/%M/%D/%h:%m:%s����ʽ
    while (!valid_input)
    {
        std::cin >> remind_t;
        if (isValidDate(remind_t))
            valid_input = true;
        else
            std::cout << "Invalid format. Please try again." << endl;
    }
    newtask.remind_time = to_time_t(remind_t);//���û��������ʽת����time_t��ʽ����
    //id�Ϳ�ʼʱ��һ��
    int id = to_time_t(begin_t)%1000;
    while(task_list.FindTask(id) != task_list.return_end()){
        id++;
    }
    int res = task_list.Add(make_pair(id, newtask));
    while ( res!= 0){//����û�����Ŀ�ʼʱ��������ظ��ˣ�����ֵ����0
        if(res==-1){
            std::cout << "The begin time repeats! Please enter another begin time: " << endl;
            bool valid_input = false;
            while (!valid_input)//�������Ŀ�ʼʱ���ǲ���%Y/%M/%D/%h:%m:%s����ʽ
            {
                std::cin >> begin_t;
                if (isValidDate(begin_t))//������˳�ѭ��
                    valid_input = true;
                else
                    std::cout << "Invalid format. Please try again." << endl;//����һֱ����ֱ����Ϊֹ
            }
            newtask.begin_time = to_time_t(begin_t);//���û��������ʽת����time_t��ʽ����
            id = to_time_t(begin_t)%1000;
            while(task_list.FindTask(id) == task_list.return_end()){
                id++;
            }
            res = task_list.Add(make_pair(id, newtask));
        }
        else if(res==1){
            std::cout << "The name repeats! Please enter another name: " << endl;
            std::cin >> newtask.name;
            res = task_list.Add(make_pair(id, newtask));
        }
    }
    return true;
}

bool DeleteTaskNoOp(TaskList& task_list){
  std::cout << "Delete tasks based on name or ID" << endl;
  std::cout << "1 represents deletion based on ID\nwhile the remaining represents deletion based on name" << endl;

  int choice;
  std::cin >> choice;
  while (!std::cin) // ��������Ƿ���Ч
  {
    std::cin.clear(); // ���������״̬��־
    std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ����ʣ�������

    std::cout << "Invalid input. Please enter a valid ID: " << endl;
    std::cin >> choice;
  }
  if (choice == 1)
  {
    std::cout << "Please input the ID of the task to be deleted" << endl;
    long long id;
    std::cin >> id;
    while (!std::cin || !task_list.Erase(id)) // ��������Ƿ���Ч�Լ�id�Ƿ����
    {	//��������Ч������²Ż�ִ��Erase���жϷ���ֵ
      if(!std::cin){
        std::cin.clear(); // ���������״̬��־
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ����ʣ�������

        std::cout << "Invalid input. Please enter a valid ID: " << endl;
        std::cin >> id;
      }
      else if(!task_list.Erase(id)){
        std::cin.clear(); // ���������״̬��־
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ����ʣ�������

        std::cout << "Can not find this id. Please enter a valid ID: " << endl;
        std::cin >> id;
      }
    }
  }
  else
  {
    std::cout << "Please input the name of the task to be deleted" << endl; // ��������ɾ������
    string taskName;
    std::cin >> taskName;
    while (!task_list.Erase(taskName))
    {
      std::cin.clear(); // ���������״̬��־
      std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ����ʣ�������

      std::cout << "Can not find this name. Please enter a valid name: " << endl;
      std::cin >> taskName;
    }

  }
  task_list.saveFile();//�����ļ�
  return true;
}

bool ShowTaskNoOp( TaskList& task_list){
  std::cout << "If you want to see all the tasks, input 0" << endl;
			std::cout << "If you need to select according to the importance of the task, input 1" << endl;
			std::cout << "If you need to select according to the time of the task, input 2" << endl;
			std::cout << "If you need to select according to both the time and importance of the task, input 3" << endl;
			std::cout << "default: show all" << endl;

			int option;
			std::cin >> option;
			while (!std::cin) // ��������Ƿ���Ч
			{
				std::cin.clear(); // ���������״̬��־
				std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ����ʣ�������

				std::cout << "Invalid input. Please enter a valid choice: " << endl;
				std::cin >> option;
			}
			int priority = 7;
			int startTime = 0;
			string startTimeStr = "";
			string endTimeStr = "";
			string c;//���û������Ƿ�Ҫ�趨ʱ��
			int endTime = pow(2, 31) - 1;

			switch (option)
			{
			case 0:
				task_list.Show(); // ��ʾ��������
				break;
			case 1:
				std::cout << "1 for low, 2 for medium, 4 for high" << endl;
				std::cout << "3 for low and medium, 6 for medium and high, 5 for low and high" << endl;
				std::cout << "7 for all" << endl;
				std::cin >> priority;
				while (priority > 7 || priority < 1)
				{
					std::cout << "invalid" << endl;
					std::cin >> priority;
				}
				while (!std::cin) // ��������Ƿ���Ч
				{
					std::cin.clear(); // ���������״̬��־
					std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ����ʣ�������

					std::cout << "Invalid input. Please enter a valid choice: " << endl;
					std::cin >> priority;
				}
				task_list.Show(0, pow(2, 31) - 1, priority); // �������ȼ���ʾ����
				break;
			case 2:
				std::cout << "If you want to see all the tasks before the specified date, enter y/n" << endl;
				std::cout << "format: 2022/02/02/10:00:00" << endl;
				std::cin >> c;

				if (c == "y" || c == "Y") {
					std::cout << "Please input the date" << endl;
					std::cin >> startTimeStr;

					while (!isValidDate(startTimeStr))
					{
						std::cout << "Invalid input, please try again" << endl;
						std::cin >> startTimeStr;
					}

				}

				if (startTimeStr != "")
					startTime = to_time_t(startTimeStr);

				std::cout << "If you want to see all the tasks after the specified date, enter y/n" << endl;
				std::cout << "format: 2022/02/02/10:00:00" << endl;
				std::cin >> c;

				if (c == "y" || c == "Y")
				{
					std::cout << "Please input the date" << endl;
					std::cin >> endTimeStr;

					while (!isValidDate(endTimeStr))
					{
						std::cout << "Invalid input, please try again" << endl;
						std::cin >> endTimeStr;
					}

				}

				if (endTimeStr != "")
					endTime = to_time_t(endTimeStr);

				task_list.Show(startTime, endTime); // ����ʱ�䷶Χ��ʾ����
				break;
			case 3:
				std::cout << "1 for low, 2 for medium, 4 for high" << endl;
				std::cout << "3 for low and medium, 6 for medium and high, 5 for low and high" << endl;
				std::cout << "7 for all" << endl;
				std::cin >> priority;
				while (priority > 7 || priority < 1)
				{
					std::cout << "invalid" << endl;
					std::cin >> priority;
				}
				while (!std::cin) // ��������Ƿ���Ч
				{
					std::cin.clear(); // ���������״̬��־
					std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ����ʣ�������

					std::cout << "Invalid input. Please enter a valid choice: " << endl;
					std::cin >> priority;
				}
				std::cout << "If you want to see all the tasks before the specified date, enter y/n" << endl;
				std::cout << "format: 2022/02/02/10:00:00" << endl;
				std::cin >> c;

				if (c == "y" || c == "Y") {
					std::cout << "Please input the date" << endl;
					std::cin >> startTimeStr;

					while (!isValidDate(startTimeStr))
					{
						std::cout << "Invalid input, please try again" << endl;
						std::cin >> startTimeStr;
					}

				}

				if (startTimeStr != "")
					startTime = to_time_t(startTimeStr);

				std::cout << "If you want to see all the tasks after the specified date, enter y/n" << endl;
				std::cout << "format: 2022/02/02/10:00:00" << endl;
				std::cin >> c;

				if (c == "y" || c == "Y") {
					std::cout << "Please input the date" << endl;
					std::cin >> endTimeStr;

					while (!isValidDate(endTimeStr))
					{
						std::cout << "Invalid input, please try again" << endl;
						std::cin >> endTimeStr;
					}

				}

				if (endTimeStr != "")
					endTime = to_time_t(endTimeStr);

				task_list.Show(startTime, endTime, priority); // ����ʱ�䷶Χ�����ȼ�
			}
      return true;
}

bool ModifyTaskNoOp(TaskList& task_list){
  std::cout << "search tasks based on name or ID" << endl;
  std::cout << "1 represents search based on ID\nwhile the remaining represents search based on name" << endl;
  int choice;
  std::cin >> choice;
  while (!std::cin) // ��������Ƿ���Ч
  {
    std::cin.clear(); // ���������״̬��־
    std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ����ʣ�������

    std::cout << "Invalid input. Please enter a valid choice: " << endl;
    std::cin >> choice;
  }
  vector<pair<int, Other>>::iterator it;
  string newstart;
  string newremind;
  switch (choice)
  {
  case 1:
    std::cout << "please input the id of the wanted task" << endl;
    long long id;
    std::cin >> id;
    while (!std::cin || !task_list.FindShow(id)) // ��������Ƿ���Ч
    {
      if(!std::cin){
        std::cin.clear(); // ���������״̬��־
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ����ʣ�������

        std::cout << "Invalid input. Please enter a valid id: " << endl;
        std::cin >> id;
      }
      else if(!task_list.FindShow(id)){
        std::cin.clear(); // ���������״̬��־
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ����ʣ�������

        std::cout << "Can not find this id. Please enter a valid id: " << endl;
        std::cin >> id;
      }
    }
    
    it = task_list.FindTask(id);
    break;
  default:
    std::cout << "please input the name of the wanted task" << endl;
    string idx;
    std::cin >> idx;

    while (!task_list.FindShow(idx)) // ��������Ƿ���Ч
    {
      if(!task_list.FindShow(idx)){
        std::cin.clear(); // ���������״̬��־
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ����ʣ�������

        std::cout << "Can not find this name. Please enter a valid id: " << endl;
        std::cin >> idx;
      }
    }

    it = task_list.FindTask(idx);
  }
  std::cout << "n : change name" << endl;
  std::cout << "s : change start time" << endl;
  std::cout << "r : change remind time" << endl;
  std::cout << "t : change type " << endl;
  std::cout << "p : change priority " << endl;

  string xx;
  std::cin >> xx;
  bool is_invalid=true;
  while(is_invalid){
    if (xx == "n" || xx == "N") {
      std::cout << "change name" << endl;
      std::cin >> it->second.name;
      is_invalid=false;
    }
    else if (xx == "s" || xx == "S") {
      std::cout << "change begin_time" << endl;
      std::cin >> newstart;
      while(!isValidDate(newstart)){
        cout << "invalid date format" << endl;
        cin >> newstart;
      }
      if(it->second.begin_time != to_time_t(newstart))
        it->second.begin_time = to_time_t(newstart);
      is_invalid=false;
    }
    else if (xx == "t" || xx == "T") {
      std::cout << "change type" << endl;
      std::cin >> it->second.type;
      is_invalid=false;
    }
    else if (xx == "r" || xx == "R") {
      std::cout << "change remind_time" << endl;
      std::cin >> newremind;
      while(!isValidDate(newremind)){
        cout << "invalid date format" << endl;
        cin >> newremind;
      }
      if(it->second.remind_time != to_time_t(newremind))
        it->second.remind_time = to_time_t(newremind);
      is_invalid=false;
    }
    else if (xx == "p" || xx == "P") {
      std::cout << "change prority" << endl;
      std::cin >> it->second.priority;
      is_invalid=false;
    }
    else{
      std::cout << "Invalid choice. Please enter a valid choice: " << endl;
      std::cin >> xx;
    }
      
  }
  
  task_list.saveFile();
  return true;
}

bool SearchTaskNoOp(TaskList& task_list){
  std::cout << "search tasks based on name or ID" << endl;
  std::cout << "1 represents search based on ID\nwhile the remaining represents search based on name" << endl;
  int choice;
  std::cin >> choice;
  while (!std::cin) // ��������Ƿ���Ч
  {
    std::cin.clear(); // ���������״̬��־
    std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ����ʣ�������

    std::cout << "Invalid input. Please enter a valid choice: " << endl;
    std::cin >> choice;
  }
  switch (choice)
  {
  case 1:
    std::cout << "please input the id of the wanted task" << endl;
    long long id;
    std::cin >> id;
    while (!std::cin || !task_list.FindShow(id)) // ��������Ƿ���Ч
    {
      if(!std::cin){
        std::cin.clear(); // ���������״̬��־
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ����ʣ�������

        std::cout << "Invalid input. Please enter a valid id: " << endl;
        std::cin >> id;
      }
      else if(!task_list.FindShow(id)){
        std::cin.clear(); // ���������״̬��־
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ����ʣ�������

        std::cout << "Can not find this id. Please enter a valid id: " << endl;
        std::cin >> id;
      }
    }
    break;
  default:
    std::cout << "please input the name of the wanted task" << endl;
    string ix;
    std::cin >> ix;
    while (!task_list.FindShow(ix)) // ��������Ƿ���Ч
    {
        std::cin.clear(); // ���������״̬��־
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ����ʣ�������

        std::cout << "Can not find this name. Please enter a valid id: " << endl;
        std::cin >> ix;
    }
    break;
  }
  return true;
}
