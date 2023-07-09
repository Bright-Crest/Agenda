
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

//bool CmdProcessor::GetArgv(int argc, const char* argv[], int start) {
//    cmd_.clear();
//    if (start >= argc) {
//        return false;
//    }
//
//    for (int i = start; i < argc; i++) {
//        cmd_.push_back(argv[i]);
//    }
//    return true;
//}

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
    }
    else if (first_cmd == "modifytask" || first_cmd == "modify") {
        lock_guard<mutex> lck(mtx);
        if (kSize == 1)
            flag = ModifyTaskNoOp(task_list);
        else
            flag = ModifyTaskOp(task_list, cmd_);
    }
    else if (first_cmd == "deletetask" || first_cmd == "delete") {
        lock_guard<mutex> lck(mtx);
        if (kSize == 1)
            flag = DeleteTaskNoOp(task_list);
        else
            flag = DeleteTaskOp(task_list, cmd_);
    }
    else if (first_cmd == "showtask" || first_cmd == "show") {
        if (kSize == 1)
            flag = ShowTaskNoOp(task_list);
        else
            flag = ShowTaskOp(task_list, cmd_);
    }
    else if (first_cmd == "searchtask" || first_cmd == "search") {
        if (kSize == 1)
            flag = SearchTaskNoOp(task_list);
        else
            flag = SearchTaskOp(task_list, cmd_);
    }
    else if (first_cmd == "quit" || first_cmd == "q") {
        cout << "Bye.\n";
        return -1;
    }
    else if (first_cmd == "login" || first_cmd == "li" ||
        first_cmd == "createaccount" || first_cmd == "ca" ||
        first_cmd == "changepassword" || first_cmd == "changepw" ||
        first_cmd == "cp" || first_cmd == "deleteaccount" ||
        first_cmd == "da") {
        cout << "Please quit first and then restart the program to \"" << first_cmd
            << "\".\n";
    }
    else {
        cout << "\"" << cmd_.front() << "\" is not a valid command.\n";
        return 0;
    }
    return (int)flag;
}

// 用户输入格式:addtask -n name -b begin -p priority -t type -r remind (注意:用户输入的参数不能以-开头,priority默认为0,type默认为" ",选项的顺序可以打乱)
bool AddTaskOp(TaskList& task_list, list<string> cmd)
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
                cout << "The begin_time parameter does not meet the requirements" << endl;
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
                continue;
            
            int p = stoi(str);
            if (p != 1 && p != 2 && p != 4)
            {
                cout << "The priority parameters do not meet the requirements";
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
                continue;

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
                cout << "The remind_time parameters do not meet the requirements";
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

    int id = (to_time_t(begin)) % 997; // 得到id后打包进行add
    vector<pair<int, Other>>::iterator it1 = task_list.FindTask(id);
    while (it1 != task_list.return_end())
    {
        id = (id + 1) % 997;
        it1 = task_list.FindTask(id);
    }
    if (task_list.Add(make_pair(id, task)))
    {
        cout << "Faliure,unable to add the new task" << endl;
        return false;
    }

    return true;
}

// 重要规定:用于查找的选项是大写,用于修改的选项是小写!
//  用户输入格式: modifytask -N name -I id -n new_name -b begin -p priority -t type -r remind(注意:-N和-I至少有1个,-p,-t后面可以没有参数,-n,-b,-r后面必须有参数,id不可以修改)
bool ModifyTaskOp(TaskList& task_list, list<string> cmd)
{
    string name = "-"; // 用于查找的name,默认是"-"
    int id = -1;       // 用于查找的id
    Other task;
    task.name = "-"; // 默认是"-",若发现没有通过命令修改,则copy查找到的任务的name,其他的属性同理
    task.begin_time = -1;
    task.priority = 0;
    task.type = "-";
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
                cout << "The begin_time parameter does not meet the requirements" << endl;
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
                continue;
            int p = stoi(str);
            if (p != 1 && p != 2 && p != 4)
            {
                cout << "The priority parameters do not meet the requirements";
                return false;
            }

            task.priority = p;
            it++;
        }
        else if (*it == "-t")
        {
            if (++it == cmd.end()) //-type是最后一个,后面没有参数,这是可以的
                break;
            string str = *(it);
            if (str == "-n" || str == "-b" || str == "-p" || str == "-r" || str == "-N" || str == "-I") //-type后不跟参数,这是可以的
                continue;
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
                cout << "The remind_time parameters do not meet the requirements";
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

    int id1; // 保存修改任务的id值
    vector<pair<int, Other>>::iterator it1 = task_list.FindTask(id);
    vector<pair<int, Other>>::iterator it2 = task_list.FindTask(name);
    bool flag1 = it1 != task_list.return_end();
    bool flag2 = it2 != task_list.return_end();
    if ((flag1 && flag2 && it1 == it2) || (flag1 && name == "-" && !flag2) || (!flag1 && id == -1 && flag2)) // 3种符合查找要求的情况,分别是:id,name均有筛选且匹配;以及id,name仅有一个且找到
    {
        vector<pair<int, Other>>::iterator itt;
        if (flag1)
            itt = it1;
        else
            itt = it2;
        id1 = itt->first;
        if (task.name == "-")
            task.name = itt->second.name;
        if (task.begin_time == -1)
            task.begin_time = itt->second.begin_time;
        if (task.type == "-")
            task.type = itt->second.type;
        if (task.priority == 0)
            task.priority = itt->second.priority;
        if (task.remind_time == -1)
            task.remind_time = itt->second.remind_time;

        if (flag1)
            task_list.Erase(id); // 删除对应的任务
        else
            task_list.Erase(name);

        if (task_list.Add(make_pair(id1, task))) // 添加相应的任务
        {
            cout << "Faliure,unable to add the new task" << endl;
            return false;
        }

        task_list.saveFile();

        return true;
    }
    else // id和match筛选得到的任务不是同一个
    {
        task_list.saveFile();

        cout << "The id and name used for modifying do not match" << endl;

        return false;
    }
}

// 使用说明: searchtask -N name -I id(-N和-I至少有一个)
bool SearchTaskOp(TaskList& task_list, list<string> cmd)
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
            if (str[0] == '-') // 不符合
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
    if ((flag1 && flag2 && it1 == it2) || (!flag1 && id == -1 && flag2) || (flag1 && name == "-" && !flag2)) // 3种符合查找要求的情况,输出任务信息
    {
        if (flag1)
            task_list.FindShow(id);
        else
            task_list.FindShow(name);

        return true;
    }
    else
    {
        cout << "The id and name used for searching do not meet the requirements" << endl;

        return false;
    }
}

// 使用说明:showtask -S start -E end -P priority -T type -a(show all) -A i(id)/b(begin)/p(priority)/r(remind) -D (其中-S,-E,-P,-T均可以缺省,-A,-D至多有1个,后面必须有参数)
bool ShowTaskOp(TaskList& task_list, list<string> cmd)
{
    list<string>::iterator it = ++cmd.begin(); // 从cmd的第二个string开始
    int begin_time = -1;
    int end_time = -1;
    int priority = 0;
    string type = "-";
    static bool (*func)(pair<int, Other> task1, pair<int, Other> task2) = LessBegin;
    bool flagForAll = false;

    while (it != cmd.end())
    {
        if (*it == "-a")
        {
            list<string>::iterator it1 = ++it;
            if (it1 == cmd.end() || *it1 == "-S" || *it1 == "-E" || *it1 == "-P" || *it1 == "-T" || *it1 == "-A" || *it1 == "-D")
                flagForAll = true;
            else
                return false;

            it = it1;
        }
        else if (*it == "-S")
        {
            if (++it == cmd.end()) //-S是最后一个,后面没有参数,因为begin有缺省值,因此这是可以的
                break;
            string str = *(it);                                                                         // 找到-S的下一位
            if (str == "-E" || str == "-P" || str == "-T" || str == "-A" || str == "-D" || str == "-a") //-S后不跟参数,这是可以的
                continue;
            if (!isValidDate(str)) // 时间参数不符合要求
            {
                cout << "The start_time parameter does not meet the requirements" << endl;
                return false;
            }

            begin_time = to_time_t(str);
            it++;
        }
        else if (*it == "-E")
        {
            if (++it == cmd.end()) //-E是最后一个,后面没有参数,因为end有缺省值,因此这是可以的
                break;
            string str = *(it);                                                                         // 找到-S的下一位
            if (str == "-S" || str == "-P" || str == "-T" || str == "-A" || str == "-D" || str == "-a") //-S后不跟参数,这是可以的
                continue;
            if (!isValidDate(str)) // 时间参数不符合要求
            {
                cout << "The end_time parameter does not meet the requirements" << endl;
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
            if (str == "-S" || str == "-E" || str == "-T" || str == "-A" || str == "-D" || str == "-a") //-P后不跟参数,这是可以的
                continue;
            int p = stoi(str);
            if (p != 1 && p != 2 && p != 3 && p != 4 && p != 5 && p != 6 && p != 7)
            {
                cout << "The priority parameters do not meet the requirements";
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
            if (str == "-S" || str == "-E" || str == "-P" || str == "-A" || str == "-D" || str == "-a") //-T后不跟参数,这是可以的
                continue;

            type = str;
            it++;
        }
        else if (*it == "-D") // 降序输出
        {
            if (++it == cmd.end())
            {
                cout << "No parameters after -D" << endl;
                return false;
            }
            string str = *(it);
            if (str[0] == '-') // 不符合
            {
                cout << "No parameters after -D" << endl;
                return false;
            }
            if (str != "b" && str != "begin" && str != "i" && str != "id" && str != "p" && str != "priority" && str != "r" && str != "remind")
            {
                cout << "The filter parameter does not meet the requirements" << endl;
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
        }
        else if (*it == "-A") // 升序输出
        {
            if (++it == cmd.end())
            {
                cout << "No parameters after -A" << endl;
                return false;
            }
            string str = *(it);
            if (str[0] == '-') // 不符合
            {
                cout << "No parameters after -A" << endl;
                return false;
            }
            if (str != "b" && str != "begin" && str != "i" && str != "id" && str != "p" && str != "priority" && str != "r" && str != "remind")
            {
                cout << "The filter parameter does not meet the requirements" << endl;
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
        }
        else
        {
            cout << "Invalid command" << endl;
            return false;
        }
    }

    if (flagForAll) // show全部的任务并返回
    {
        task_list.Show();
        return true;
    }

    if (begin_time == -1)
        begin_time = 0;
    if (end_time == -1)
        end_time = pow(2, 31) - 1;
    if (priority == 0)
        priority = 7;
    if (type == "-")
        task_list.Show(begin_time, end_time, priority, func);
    else
        task_list.Show(type, begin_time, end_time, priority, func);

    return true;
}

// 使用说明:deltask -n name -i id -a(-n,-i可以均没有,-a代表删除全部任务)
bool DeleteTaskOp(TaskList& task_list, list<string> cmd)
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

            it = it1;
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

    bool flagForErase = (flag1 && flag2 && it1 == it2) || (!flag1 && id == -1 && flag2) || (flag1 && name == "-" && !flag2); // 指令-n和-i正确

    if (flagForErase && flagForAll) // 删除全部任务
    {
        task_list.Clear();
        task_list.saveFile(); // 记得将task_list中保存的任务再次保存至文件里

        return true;
    }
    else if (flagForErase && !flagForAll)
    {
        if (flag1)
            task_list.Erase(id);
        else
            task_list.Erase(name);

        task_list.saveFile();

        return true;
    }
    else
    {
        task_list.saveFile();

        cout << "The id and name used for deletion do not match" << endl;

        return false;
    }
}

bool AddTaskNoOp(TaskList& task_list) {
    // 添加任务名字
    Other newtask;
    std::cout << "Please input the name of the task" << std::endl;
    std::string newtask_name;
    std::cin >> std::ws;  // Skips leading whitespace characters
    std::getline(std::cin, newtask_name);
    std::istringstream newtaskname(newtask_name);
    newtaskname >> newtask.name;

    // 添加任务的开始时间
    std::cout << "Please input the begin time of the task" << endl;
    std::cout << "Time format should be 2022/02/02/03:00:00" << endl;
    string begin_t;
    bool valid_input = false;
    while (!valid_input)//检查任务的开始时间是不是%Y/%M/%D/%h:%m:%s的形式
    {
        std::cin >> begin_t;
        if (isValidDate(begin_t))//如果是退出循环
            valid_input = true;
        else
            std::cout << "Invalid format. Please try again." << endl;//否则一直输入直到是为止
    }
    newtask.begin_time = to_time_t(begin_t);//将用户输入的形式转化成time_t格式保存
    // 添加任务的优先级
    std::cout << "Please input the priority of the task" << endl;
    std::cout << "1-low, 2-medium, 4-high, default value or other value-low" << endl;
    std::cin >> newtask.priority;
    // 检查优先级是不是符合数字
    while (!std::cin) {
        std::cin.clear(); // 清除输入流状态标志
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

        std::cout << "Invalid input. Please enter a valid priority: " << endl;
        std::cin >> newtask.priority;
    }
    if (newtask.priority != 1 && newtask.priority != 2 && newtask.priority != 4)
        newtask.priority = 1;
    // 添加任务的种类
    std::cout << "Please input the type of the task, such as: entertainment  sport  study  routine" << endl;
    std::cout << "If you want a default type, please enter -." << endl;
    string newtask_type;
    std::cin >> std::ws;  // Skips leading whitespace characters
    std::getline(std::cin, newtask_type);
    istringstream  newtasktype(newtask_type);
    newtasktype >> newtask.type;
    if (newtask.type[0] == '-')
        newtask.type = "default";
    // 添加任务的提醒时间
    std::cout << "Please input the remind time of the task" << endl;
    std::cout << "Time format should be 2022/02/02/03:00:00" << endl;
    string remind_t;
    valid_input = false;
    //检查任务的开始时间是不是%Y/%M/%D/%h:%m:%s的形式
    while (!valid_input)
    {
        std::cin >> remind_t;
        if (isValidDate(remind_t))
            valid_input = true;
        else
            std::cout << "Invalid format. Please try again." << endl;
    }
    newtask.remind_time = to_time_t(remind_t);//将用户输入的形式转化成time_t格式保存
    //id和开始时间一样
    int id = to_time_t(begin_t) % 1000;
    while (task_list.FindTask(id) != task_list.return_end()) {
        id++;
    }
    int res = task_list.Add(make_pair(id, newtask));
    while (res != 0) {//如果用户输入的开始时间或名字重复了，返回值不是0
        if (res == -1) {
            std::cout << "The begin time repeats! Please enter another begin time: " << endl;
            bool valid_input = false;
            while (!valid_input)//检查任务的开始时间是不是%Y/%M/%D/%h:%m:%s的形式
            {
                std::cin >> begin_t;
                if (isValidDate(begin_t))//如果是退出循环
                    valid_input = true;
                else
                    std::cout << "Invalid format. Please try again." << endl;//否则一直输入直到是为止
            }
            newtask.begin_time = to_time_t(begin_t);//将用户输入的形式转化成time_t格式保存
            id = to_time_t(begin_t) % 997;
            while (task_list.FindTask(id) == task_list.return_end()) {
                id++;
            }
            res = task_list.Add(make_pair(id, newtask));
        }
        else if (res == 1) {
            std::cout << "The name repeats! Please enter another name: " << endl;
            string newtask_name2;
            std::cin >> std::ws;  // Skips leading whitespace characters
            std::getline(std::cin, newtask_name2);
            istringstream  newtaskname2(newtask_name2);
            newtaskname2>> newtask.name;
            res = task_list.Add(make_pair(id, newtask));
        }
    }
    return true;
}

bool DeleteTaskNoOp(TaskList& task_list) {
    std::cout << "Delete tasks based on name or ID" << endl;
    std::cout << "1 represents deletion based on ID\nwhile the remaining represents deletion based on name" << endl;

    int choice;
    std::cin >> choice;
    while (!std::cin) // 检查输入是否有效
    {
        std::cin.clear(); // 清除输入流状态标志
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

        std::cout << "Invalid input. Please enter a valid ID: " << endl;
        std::cin >> choice;
    }
    if (choice == 1)
    {
        std::cout << "Please input the ID of the task to be deleted" << endl;
        long long id;
        std::cin >> id;
        while (!std::cin || !task_list.Erase(id)) // 检查输入是否有效以及id是否存在
        {	//在输入有效的情况下才会执行Erase并判断返回值
            if (!std::cin) {
                std::cin.clear(); // 清除输入流状态标志
                std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

                std::cout << "Invalid input. Please enter a valid ID: " << endl;
                std::cin >> id;
            }
            else if (!task_list.Erase(id)) {
                std::cin.clear(); // 清除输入流状态标志
                std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

                std::cout << "Can not find this id. Please enter a valid ID: " << endl;
                std::cin >> id;
            }
        }
    }
    else
    {
        std::cout << "Please input the name of the task to be deleted" << endl; // 根据名称删除任务
        string taskName;
        string newtask_name;
        std::cin >> std::ws;  // Skips leading whitespace characters
        std::getline(std::cin, newtask_name);
        istringstream  newtaskname(newtask_name);
        newtaskname >> taskName;
        while (!task_list.Erase(taskName))
        {
            std::cin.clear(); // 清除输入流状态标志
            std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

            std::cout << "Can not find this name. Please enter a valid name: " << endl;

            string newtask_name1;
            std::cin >> std::ws;  // Skips leading whitespace characters
            std::getline(std::cin, newtask_name1);
            istringstream  newtaskname1(newtask_name1);
            newtaskname1 >> taskName;
        }

    }
    task_list.saveFile();//保存文件
    return true;
}

bool ShowTaskNoOp(TaskList& task_list) {
    std::cout << "If you want to see all the tasks, input 0" << endl;
    std::cout << "If you need to select according to the importance of the task, input 1" << endl;
    std::cout << "If you need to select according to the time of the task, input 2" << endl;
    std::cout << "If you need to select according to both the time and importance of the task, input 3" << endl;
    std::cout << "default: show all" << endl;

    int option;
    std::cin >> option;
    while (!std::cin) // 检查输入是否有效
    {
        std::cin.clear(); // 清除输入流状态标志
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

        std::cout << "Invalid input. Please enter a valid choice: " << endl;
        std::cin >> option;
    }
    int priority = 7;
    int startTime = 0;
    string startTimeStr = "";
    string endTimeStr = "";
    string c;//由用户决定是否要设定时间
    int endTime = pow(2, 31) - 1;

    switch (option)
    {
    case 0:
        task_list.Show(); // 显示所有任务
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
        while (!std::cin) // 检查输入是否有效
        {
            std::cin.clear(); // 清除输入流状态标志
            std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

            std::cout << "Invalid input. Please enter a valid choice: " << endl;
            std::cin >> priority;
        }
        task_list.Show(0, pow(2, 31) - 1, priority); // 根据优先级显示任务
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

        task_list.Show(startTime, endTime); // 根据时间范围显示任务
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
        while (!std::cin) // 检查输入是否有效
        {
            std::cin.clear(); // 清除输入流状态标志
            std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

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

        task_list.Show(startTime, endTime, priority); // 根据时间范围和优先级
    }
    return true;
}

bool ModifyTaskNoOp(TaskList& task_list) {
    std::cout << "search tasks based on name or ID" << endl;
    std::cout << "1 represents search based on ID\nwhile the remaining represents search based on name" << endl;
    int choice;
    std::cin >> choice;
    while (!std::cin) // 检查输入是否有效
    {
        std::cin.clear(); // 清除输入流状态标志
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

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
        while (!std::cin || !task_list.FindShow(id)) // 检查输入是否有效
        {
            if (!std::cin) {
                std::cin.clear(); // 清除输入流状态标志
                std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

                std::cout << "Invalid input. Please enter a valid id: " << endl;
                std::cin >> id;
            }
            else if (!task_list.FindShow(id)) {
                std::cin.clear(); // 清除输入流状态标志
                std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

                std::cout << "Can not find this id. Please enter a valid id: " << endl;
                std::cin >> id;
            }
        }

        it = task_list.FindTask(id);
        break;
    default:
        std::cout << "please input the name of the wanted task" << endl;
        string idx;
        string newtask_name;
        std::cin >> std::ws;  // Skips leading whitespace characters
        std::getline(std::cin, newtask_name);
        istringstream  newtaskname(newtask_name);
        newtaskname >> idx;
        while (!task_list.FindShow(idx)) // 检查输入是否有效
        {
            if (!task_list.FindShow(idx)) {
                std::cin.clear(); // 清除输入流状态标志
                std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

                std::cout << "Can not find this name. Please enter a valid id: " << endl;
                string newtask_name1;
                std::cin >> std::ws;  // Skips leading whitespace characters
                std::getline(std::cin, newtask_name1);
                istringstream  newtaskname1(newtask_name1);
                newtaskname1 >> idx;
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
    bool is_invalid = true;
    while (is_invalid) {
        if (xx == "n" || xx == "N") {
            std::cout << "change name" << endl;
            string newtask_namen;
            std::cin >> std::ws;  // Skips leading whitespace characters
            std::getline(std::cin, newtask_namen);
            istringstream  newtasknamen(newtask_namen);
            newtasknamen >> it->second.name;
            is_invalid = false;
        }
        else if (xx == "s" || xx == "S") {
            std::cout << "change begin_time" << endl;
            std::cin >> newstart;
            while (!isValidDate(newstart)) {
                cout << "invalid date format" << endl;
                cin >> newstart;
            }
            if (it->second.begin_time != to_time_t(newstart))
                it->second.begin_time = to_time_t(newstart);
            is_invalid = false;
        }
        else if (xx == "t" || xx == "T") {
            std::cout << "change type" << endl;
            string newtask_namet;
            std::cin >> std::ws;  // Skips leading whitespace characters
            std::getline(std::cin, newtask_namet);
            istringstream  newtasknamet(newtask_namet);
            newtasknamet >> it->second.type;
            is_invalid = false;
        }
        else if (xx == "r" || xx == "R") {
            std::cout << "change remind_time" << endl;
            std::cin >> newremind;
            while (!isValidDate(newremind)) {
                cout << "invalid date format" << endl;
                cin >> newremind;
            }
            if (it->second.remind_time != to_time_t(newremind))
                it->second.remind_time = to_time_t(newremind);
            is_invalid = false;
        }
        else if (xx == "p" || xx == "P") {
            std::cout << "change prority" << endl;
            std::cin >> it->second.priority;
            is_invalid = false;
        }
        else {
            std::cout << "Invalid choice. Please enter a valid choice: " << endl;
            string newtask_namenx;
            std::cin >> std::ws;  // Skips leading whitespace characters
            std::getline(std::cin, newtask_namenx);
            istringstream  newtasknamenx(newtask_namenx);
            newtasknamenx >> xx;
        }

    }

    task_list.saveFile();
    return true;
}

bool SearchTaskNoOp(TaskList& task_list) {
    std::cout << "search tasks based on name or ID" << endl;
    std::cout << "1 represents search based on ID\nwhile the remaining represents search based on name" << endl;
    int choice;
    std::cin >> choice;
    while (!std::cin) // 检查输入是否有效
    {
        std::cin.clear(); // 清除输入流状态标志
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

        std::cout << "Invalid input. Please enter a valid choice: " << endl;
        std::cin >> choice;
    }
    switch (choice)
    {
    case 1:
        std::cout << "please input the id of the wanted task" << endl;
        long long id;
        std::cin >> id;
        while (!std::cin || !task_list.FindShow(id)) // 检查输入是否有效
        {
            if (!std::cin) {
                std::cin.clear(); // 清除输入流状态标志
                std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

                std::cout << "Invalid input. Please enter a valid id: " << endl;
                std::cin >> id;
            }
            else if (!task_list.FindShow(id)) {
                std::cin.clear(); // 清除输入流状态标志
                std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

                std::cout << "Can not find this id. Please enter a valid id: " << endl;
                std::cin >> id;
            }
        }
        break;
    default:
        std::cout << "please input the name of the wanted task" << endl;
        string ix;
        string newtask_namenxx;
        std::cin >> std::ws;  // Skips leading whitespace characters
        std::getline(std::cin, newtask_namenxx);
        istringstream  newtasknamenxx(newtask_namenxx);
        newtasknamenxx >> ix;
        while (!task_list.FindShow(ix)) // 检查输入是否有效
        {
            std::cin.clear(); // 清除输入流状态标志
            std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

            std::cout << "Can not find this name. Please enter a valid name: " << endl;
            string newtask_namenxox;
            std::cin >> std::ws;  // Skips leading whitespace characters
            std::getline(std::cin, newtask_namenxox);
            istringstream  newtasknamenxox(newtask_namenxox);
            newtasknamenxox >> ix;
        }
        break;
    }
    return true;
}
