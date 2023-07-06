// main

/* Standard C++ includes */
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

using namespace std;

int main(int argc, char* argv[]) {
  // TaskList tl("D:/stephenzhu/Desktop/data.txt");
  // tl.FindShow("eat");
  // tl.FindShow(1);
  // tl.Show(0, 100);
  //  tl.Erase("eat");
  // tl.Erase(1);
  //  tl.Show(0, 100, 2);
  //  tl.Remind();

  try {
    const string kProgramName = "mytask";

    // AccountManager initialize parameters
    string hostname = "tcp://127.0.0.1:3306";
    string username = "root";
    string password = "Agenda2022";
    string db = "test1";
    string table = "AgendaLogin";
    string user_column = "Username";
    string encrypt_pw_column = "Password";
    AccountManager am(hostname, username, password, db, table, user_column,
                      encrypt_pw_column);

    vector<string> login_cmd;
    string user;
    CmdProcessor cpr;

    // deal with command line
    if (argc > 1) {
      string argv1 = argv[1];
      to_lower(argv1);
      if (argv1 != "login" && argv1 != "-u" && argv1 != "-p") {
        cout << "Please log in first.\n";
      } else {
        int i;
        for (i = 1; i < argc && argv[i] != "-c";
             i++)  // TODO : c -- command. Plus, '-' is not allowed in password
          login_cmd.push_back(argv[i]);
        cpr.GetArgv(argc, argv, ++i);
      }
    }
    // keep asking to log in or quit
    while (login_cmd.size() == 0) {
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
      // TODO : Create account and Delete Account
      if (tmp != "login" && tmp != "-u" && tmp != "-p") {
        cout << "Please log in first.\n";
      } else {
        login_cmd.push_back(tmp);
        while (cmd_stream >> tmp) login_cmd.push_back(tmp);
      }
    }
    if (!UserLogin(am, login_cmd, user)) {
      return 0;
    } else {
      string filename = "D:/stephenzhu/Desktop/" + user + "_task.txt";  // TODO : filename pattern and path pattern
      TaskList tl(&filename[0]);  // convert string to char[]
      int flag = cpr.CmdDistributor(tl);
      if (flag == 1 || flag == -1) return 0;

      while (true) {
        cout << "(" << kProgramName << ") ";
        if (!cpr.GetCmd()) continue;
        if (cpr.CmdDistributor(tl) == -1) return 0;
      }
    }

  } catch (sql::SQLException& e) {
    cout << "# ERR: SQLException in " << __FILE__;
    cout << "(" << __FUNCTION__ << ") on line  » " << __LINE__ << endl;
    cout << "# ERR: " << e.what();
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << " )" << endl;
  }

  return 0;
}