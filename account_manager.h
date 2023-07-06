// define Class AccountManager
// define UserLogin

#ifndef _ACCOUNT_MANAGER_H_
#define _ACCOUNT_MANAGER_H_

// Standard C++ includes
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <string>

// Include directly the different
// headers from cppconn/ and mysql_driver.h + mysql_util.h
// (and mysql_connection.h). This will reduce your build time!
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "mysql_connection.h"

// Include encryting header
#include "bcrypt.h"

using namespace std;

class AccountManager {
 public:
  AccountManager(const string& hostname, const string& username,
                 const string& password, const string& db, const string& table,
                 const string& user_column, const string& encrypt_pw_column);
  ~AccountManager() { delete con_; }
  bool Login(const string& user, const string& pw) const;
  bool ChangePw(const string& user, const string& pw, const string& new_pw) const;
  bool Create(const string& user, const string& pw) const;
  bool Delete(const string& user, const string& pw) const;
  void PrintTable_Insecure() const;

 private:
  sql::Driver* driver_;
  sql::Connection* con_;
  string db_;  // database
  string table_;
  string user_column_;
  string encrypt_pw_column_;
};

string TakePwSecure();
bool UserLogin(const AccountManager& am, const vector<string>& cmd, string& user);  // deal with cmd and read user name from cmd; return: false -- login failed.
bool UserChangePw(const AccountManager& am, const vector<string>& cmd,
                  string& user);
bool CreatAccount(const AccountManager& am, const vector<string>& cmd,
                  string& user);
bool DeleteAccount(const AccountManager& am, const vector<string>& cmd, string& user);
bool ExtractUser(const vector<string>& cmd, string& user);
bool ExtractPw(const vector<string>& cmd, string& pw);

#endif