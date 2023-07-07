// main

/* Standard C++ includes */
#include <stdlib.h>

#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <chrono>

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
#include "task_list.h"
#include "tools.h"
#include "file_encrypt.h"

using namespace std;

mutex mtx;

void UserInterface(TaskList& task_list) {
  
}

int main(int argc, char* argv[]) {
  try {
    const string kProgramName = "mytask";

    // AccountManager initialize parameters
    string am_hostname = "tcp://127.0.0.1:3306";
    string am_username = "root";
    string am_password = "Agenda2022";
    string db = "test1";
    string table = "AgendaLogin";
    string user_column = "Username";
    string encrypt_pw_column = "Password";
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
        for (int i = 1; i < argc && argv[i] != "-c"; i++)
          account_cmd.push_back(argv[i]);
      } else if (argv1 != "login" && argv1 != "li" && argv1 != "-u" &&
                 argv1 != "-p") {
        cout << "Please log in first.\n";
      } else {
        int i;
        // c -- command. Plus, '-' is not allowed in password
        for (i = 1; i < argc && argv[i] != "-c"; i++)
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
      if (tmp == "login" || tmp == "li" || tmp == "createaccount" || tmp == "ca" ||
          tmp == "changepassword" || tmp == "changepw" || tmp == "cp" ||
          tmp == "deleteaccount" || tmp == "da") {
        account_cmd.push_back(tmp);
        while (cmd_stream >> tmp) account_cmd.push_back(tmp);
      } else {
        cout << "Please log in first.\n";
      }
    }

    // TODO : Create folders?
    // Linux file path and name pattern
    //string filename = "~/Agenda/userfile/" + user + "_task.bin";
    // Windows file path and name pattern
    string en_filename = "~/Agenda/userfile/" + user + "_task.bin";
    // TODO : if en_filename doesn't exist, create a new de_filename;
    string de_filename = "~/Agenda/tmp/" + user + "_task.txt";
    DecryptFile(en_filename, de_filename, password);
    TaskList task_list(de_filename.data());
    
    int flag = cmd_processor.CmdDistributor(task_list); 
    if (flag == 1 || flag == -1) {
      return 0;
    }

    // After loging in, split into 2 threads
    thread user_thread(UserInterface, ref(task_list));
    
    while (true) {
      this_thread::sleep_for(chrono::seconds(1));
      if (!user_thread.joinable()) break;
      lock_guard<mutex> lck(mtx);
      task_list.Remind();
    }

    return 0;

  } catch (sql::SQLException& e) {
    cout << "# ERR: SQLException in " << __FILE__;
    cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << e.what();
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << " )" << endl;
  } catch (const exception& kException) {
    cout << "# ERR: std::Exception in " << __FILE__;
    cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << kException.what() << endl;
  }

  return 0;
}
