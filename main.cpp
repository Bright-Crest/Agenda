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

  } catch (const exception& kException) {
    cout << "# ERR: std::Exception in " << __FILE__;
    cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << kException.what() << endl;
  }

  is_done = true;
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
    // const string path = "~/Agenda/userfile/";
    // const string tmp_path = "~/Agenda/tmp/";
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

// int main(int argc, char* argv[]) {
//   // TaskList tl("D:/stephenzhu/Desktop/data.txt");
//   // tl.FindShow("eat");
//   // tl.FindShow(1);
//   // tl.Show(0, 100);
//   //  tl.Erase("eat");
//   // tl.Erase(1);
//   //  tl.Show(0, 100, 2);
//   //  tl.Remind();
//
//   try {
//     const string kProgramName = "mytask";
//
//     // AccountManager initialize parameters
//     string hostname = "tcp://127.0.0.1:3306";
//     string username = "root";
//     string password = "Agenda2022";
//     string db = "test1";
//     string table = "AgendaLogin";
//     string user_column = "Username";
//     string encrypt_pw_column = "Password";
//     AccountManager am(hostname, username, password, db, table, user_column,
//                       encrypt_pw_column);
//
//     vector<string> login_cmd;
//     string user;
//     string pw;
//     CmdProcessor cpr;
//
//     // deal with command line
//     if (argc > 1) {
//       string argv1 = argv[1];
//       to_lower(argv1);
//       if (argv1 != "login" && argv1 != "-u" && argv1 != "-p") {
//         cout << "Please log in first.\n";
//       } else {
//         int i;
//         for (i = 1; i < argc && argv[i] != "-c";
//              i++)  // TODO : c -- command. Plus, '-' is not allowed in
//              password
//           login_cmd.push_back(argv[i]);
//         cpr.GetArgv(argc, argv, ++i);
//       }
//     }
//     // keep asking to log in or quit
//     while (login_cmd.size() == 0) {
//       cout << "(" << kProgramName << ") ";
//       string tmp;
//       getline(cin, tmp);
//       istringstream cmd_stream(tmp);
//
//       cmd_stream >> tmp;
//       to_lower(tmp);
//       if (tmp == "quit" || tmp == "q") {
//         cout << "Bye.\n";
//         return 0;
//       }
//
//       if (tmp != "login" && tmp != "-u" && tmp != "-p") {
//         cout << "Please log in first.\n";
//       } else {
//         login_cmd.push_back(tmp);
//         while (cmd_stream >> tmp) login_cmd.push_back(tmp);
//       }
//     }
//     if (!UserLogin(am, login_cmd, user, pw)) {
//       return 0;
//     } else {
//       string filename = "D:/stephenzhu/Desktop/" + user + "_task.txt";  //
//       TaskList tl(&filename[0]);  //
//       convert string to char[] int flag = cpr.CmdDistributor(tl); if (flag ==
//       1 || flag == -1) return 0;
//
//       while (true) {
//         cout << "(" << kProgramName << ") ";
//         if (!cpr.GetCmd()) continue;
//         if (cpr.CmdDistributor(tl) == -1) return 0;
//       }
//     }
//
//   } catch (sql::SQLException& e) {
//     cout << "# ERR: SQLException in " << __FILE__;
//     cout << "(" << __FUNCTION__ << ") on line  » " << __LINE__ << endl;
//     cout << "# ERR: " << e.what();
//     cout << " (MySQL error code: " << e.getErrorCode();
//     cout << ", SQLState: " << e.getSQLState() << " )" << endl;
//   }
//
//   return 0;
// }