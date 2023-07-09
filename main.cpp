// main

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

/* Standard C++ includes */
#include <stdlib.h>

#include <chrono>
#include <experimental/filesystem>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <cstring>
#include <thread>
#include <vector>
#include <atomic>

/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "mysql_connection.h"

// Include encryting header
#include "account_manager.h"
#include "bcrypt.h"
#include "cmd_processor.h"
#include "file_encrypt.h"
#include "task_list.h"
#include "tools.h"

using namespace std;
namespace fs = std::experimental::filesystem;

mutex mtx;
const string kProgramName = "mytask";
atomic<bool> is_done(false);

void CopyRight();
void Help();

void UserInterface(TaskList& task_list, const string en_filename,
                   const string de_filename, const string password) {
  try {
    CmdProcessor cmd_processor;
    while (true) {
      cout << "(" << kProgramName << ") ";
      cmd_processor.GetCmd();
      int flag = cmd_processor.CmdDistributor(task_list);
      if (flag == -1) {
        EncryptFile(de_filename, en_filename, password);
        remove(de_filename.c_str());
        break;
      }
    }
  is_done = true;

  } catch (const exception& kException) {
    cout << "# ERR: std::Exception in " << __FILE__;
    cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << kException.what() << endl;
  }
}

int main(int argc, const char* argv[]) {
  try {
    // AccountManager initialize parameters
    const string am_hostname = "tcp://127.0.0.1:3306";
    const string am_username = "root";
    const string am_password = "Agenda2022";
    const string db = "test1";
    const string table = "AgendaLogin";
    const string user_column = "Username";
    const string encrypt_pw_column = "Password";
    AccountManager am(am_hostname, am_username, am_password, db, table,
                      user_column, encrypt_pw_column);

    // login parameters and objects
    vector<string> account_cmd;
    string user;
    string password;
    CmdProcessor cmd_processor;

    // deal with command line
    if (argc > 1) {
      string argv1 = argv[1];
      to_lower(argv1);
      if (argv1 == "createaccount" || argv1 == "ca" ||
          argv1 == "changepassword" || argv1 == "changepw" || argv1 == "cp" ||
          argv1 == "deleteaccount" || argv1 == "da") {
        for (int i = 1; i < argc && strcmp(argv[i], "-c") != 0; i++)
          account_cmd.push_back(argv[i]);
      } else if (argv1 != "login" && argv1 != "li" && argv1 != "-u" &&
                 argv1 != "-p") {
        cout << "Please log in first.\n";
      } else {
        int i;
        // c -- command. Plus, '-' is not allowed in password
        for (i = 1; i < argc && strcmp(argv[i], "-c") != 0; i++)
          account_cmd.push_back(argv[i]);
        cmd_processor.GetArgv(argc, argv, ++i);
      }
    }

    // keep asking to log in or quit
    while (account_cmd.size() == 0 ||
           !AccountCmdDistributor(am, account_cmd, user, password)) {
      account_cmd.clear();

      cout << "(" << kProgramName << ") ";
      string tmp;
      getline(cin, tmp);
      istringstream cmd_stream(tmp);

      cmd_stream >> tmp;
      to_lower(tmp);

      if (tmp == "quit" || tmp == "q") {
        cout << "Bye.\n";
        return 0;
      }
      if (tmp == "login" || tmp == "li" || tmp == "createaccount" ||
          tmp == "ca" || tmp == "changepassword" || tmp == "changepw" ||
          tmp == "cp" || tmp == "deleteaccount" || tmp == "da") {
        account_cmd.push_back(tmp);
        while (cmd_stream >> tmp) account_cmd.push_back(tmp);
      } else {
        cout << "Please log in first.\n";
      }
    }

    // TODO : Linux file path and name pattern
     //const string path = "~/Agenda/userfile/";
     //const string tmp_path = "~/Agenda/tmp/";
    // Windows file path and name pattern
    //const string home = getenv("")
    const string path = "D:/Agenda/userfile/";
    const string tmp_path = "D:/Agenda/tmp/";

    // TEST filename
    // const string en_filename = "D:/stephenzhu/Desktop/data.bin";
    // const string de_filename = "D:/stephenzhu/Desktop/data.txt";

    // if path doesn't exist, create path
    fs::create_directories(path);
    fs::create_directories(tmp_path);

    const string en_filename = path + user + ".bin";
    const string de_filename = tmp_path + user + ".txt";

    generate_txtfiles(en_filename, de_filename, password);
    TaskList task_list(de_filename.data());

    int flag = cmd_processor.CmdDistributor(task_list);
    if (flag == 1 || flag == -1) {
      return 0;
    }

    // After loging in, split into 2 threads
    thread user_thread(UserInterface, ref(task_list), en_filename, de_filename,
                       password);

    while (true) {
      this_thread::sleep_for(chrono::seconds(1));
      if (is_done) break;
      lock_guard<mutex> lck(mtx);
      task_list.Remind();
    }

    user_thread.join();

    return 0;

  } catch (sql::SQLException& e) {
    cout << "# ERR: SQLException in " << __FILE__;
    cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << e.what();
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << " )" << endl;
  } catch (const exception& kException) {
    cout << "# ERR: Exception in " << __FILE__;
    cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << kException.what() << endl;
  }

  return 0;
}

void CopyRight() {

}

void Help() {
  cout << "Help Document:\n";
  cout << " / /\tIndicates that this option can be omitted\n"
          " [ ]\tIndicates that this parameter can be omitted\n"
          " { }\tIndicates that this should be treated as an entity\n"
          "  / \tIndicates that this is a choice.\n";  
  cout << "Commands about accounts:\n";
  cout << "login:\t{login / li} -u USER -p [PASSWORD]\n";
  cout << "changepassword:\t{changepassword / changepw / cp} -u USER -p "
          "[PASSWORD]\nTips:\tYou can only enter your new password in the secure way. There's also a confirmation of your new password\n";
  cout << "createaccount:\t{createaccount / ca} -u USER -p [PASSWORD]\n";
  cout << "deleteaccount:\t{deleteaccount / da} -u USER -p [PASSWORD]\nTips:\tthis command will also delete your file and you cannot restore it.\n";
  cout
      << "Note:\tyou cannot refind your password if you happen to forget it.\n";
  cout << "Options:\n";
  cout << "\t\t-u\tindicates USER name\n";
  cout << "\t\t-p\tindicates PASSWORD\tIf no parameter, you can enter your "
          "password securely. Recommend not to enter password explicitly in "
          "command line since it's insecure.\n";
  cout << "<<Mode 1>>  Complete everything just in Shell Command Line.\n";
  cout << "Syntax:\t{path/to/executable} [login / li] -u USER -p [PASSWORD] /-c/ [COMMAND]\n";
  cout
      << "Options:\t-c\tindicates COMMAND. The COMMAND here can be any "
         "commands to interact with your tasks which will be described below\n";
  cout << "<<Mode 2>>  Log in first and input COMMAND to manage your task.\n";
  cout << "All commands are case insensitive.\n";
  cout << "Copyright:\tthis help document is written by Zhu Shi Zheng and Zhu "
          "Zhou Zheng.\n";
  
  cout<<endl;
  
  stringstream ss;

  ss << "Display help information for commands." << endl
     << "" << endl
     << "<<Mode 1>>" << endl
     << "           Write all instructions for an operation of a task on one line for the program to " << endl
     << "automatically read and determine" << endl
     << "" << endl
     << "/ /  Indicates that this option can be omitted" << endl
     << "[ ]  Indicates that this parameter can be omitted" << endl
     << "" << endl
     << "(1)Function and Usage Description of Function <addtask>:" << endl
     << "" << endl
     << "command format:addtask -n   NAME   -b   BEGIN_TIME   /-p/   [PRIORITY]  /-t/  [TYPE]  -r  REMIND_TIME" << endl
     << "" << endl
     << "" << endl
     << "available options:" << endl
     << "                  -n     Name of the new task" << endl
     << "                  -b     begin_time of new task" << endl
     << "                  /-p/   priority of new task          [PRIORITY]   default to 0" << endl
     << "                  /-t/   type of new task              [TYPE]       default to "<< endl
     << "                  -r     remind_time of new task" << endl
     << "" << endl
     << "Example Usage:" << endl
     << "" << endl
     << "                   addtask -n	Tom  -b  1990/01/01/10:10:10  -p 2 -t sport -r  1995/11/10/10:25:15" << endl
     << "" << endl
     << "Tips:" << endl
     << "" << endl
     << "                   1.The time needs to comply with the specifications of the time format like xxxx/xx/xx/xx:xx:xx and have practical meanings." << endl
     << "" << endl
     << "                   2.User input parameters cannot start with -." << endl
     << "" << endl
     << "" << endl
     << "" << endl
     << "(2)Function and Usage Description of Function <modifytask>:" << endl
     << "" << endl
     << "command format:modifytask /-N/  NAME  /-I/  ID  /-n/  [NEW_NAME]  /-b/   [BEGIN_TIME]  /-p/   [PRIORITY]  /-" << endl
     << "t/   [TYPE]  /-r/  [REMIND_TIME]" << endl
     << "" << endl
     << "available options:" << endl
     << "                  /-N/     Task name that needs to modify information" << endl
     << "                  /-I/     Task id that needs to modify information" << endl
     << "                  /-n/     Name of the new task              [NEW_NAME]  default to the name of the task that " << endl
     << "needs to be modified" << endl
     << "                  /-b/     begin_time of the new task        [BEGIN_TIME]    default to the begin_time of the " << endl
     << "task that needs to be modified" << endl
     << "                  /-p/     priority of new task              [PRIORITY]     default to the priority of the " << endl
     << "task that needs to be modified" << endl
     << "                  /-t/     type of new task                  [TYPE]     default to the type of the task that " << endl
     << "needs to be modified" << endl
     << "                  /-r/     remind_time of new task           [REMIND_TIME]     default to the remind_time of " << endl
     << "the task that needs to be modified" << endl
     << "Example Usage:" << endl
     << "" << endl
     << "                   modifytask -N  Tom  -b  1990/01/01/10:10:10   -t sport" << endl
     << "                   modifytask -N  Tom  -I  123    -r  1990/01/01/10:10:10   -p  1" << endl
     << "" << endl
     << "Tips:" << endl
     << "" << endl
     << "                  1.-I and - N must have at least one." << endl
     << "                  2.If no task needs to be modified is found, an error will be automatically reported." << endl
     << "" << endl;

     cout << ss.str();
}
