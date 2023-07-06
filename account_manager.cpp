
#include "account_manager.h"

// Standard C++ includes
#include <conio.h>  // for _getch()
#include <stdlib.h>

#include <algorithm>  // for find()
#include <exception>
#include <iostream>
#include <string>
#include <vector>

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

// Own header
#include "tools.h"

using namespace std;

AccountManager::AccountManager(const string& hostname, const string& username,
                               const string& password, const string& db,
                               const string& table, const string& user_column,
                               const string& encrypt_pw_column)
    : db_(db),
      table_(table),
      user_column_(user_column),
      encrypt_pw_column_(encrypt_pw_column) {
  driver_ = get_driver_instance();
  con_ = driver_->connect(hostname, username, password);
  con_->setSchema(db_);
}

bool AccountManager::Login(const string& user, const string& pw) const {
  sql::PreparedStatement* pstmt;
  // sql::Statement* stmt;
  sql::ResultSet* res;
  string expr = "SELECT " + encrypt_pw_column_ + " FROM " + table_ + " WHERE " +
                user_column_ + " = ?";
  // cout << expr << endl;

  // stmt = con_->createStatement();
  // res = stmt->executeQuery(expr);

  pstmt = con_->prepareStatement(expr);
  pstmt->setString(1, user);
  res = pstmt->executeQuery();

  if (!res->next()) {
    cout << "The username doesn't exist. Create a new account or check your "
            "username.\n";
    return false;
  }
  if (bcrypt::validatePassword(pw, res->getString(encrypt_pw_column_))) {
    return true;
  } else {
    cout << "The password is wrong. Please retry.\n";
    return false;
  }

  delete res;
  // delete stmt;
  delete pstmt;
}

bool AccountManager::ChangePw(const string& user, const string& pw,
                              const string& new_pw) const {
  if (!Login(user, pw)) return false;
  // check new_pw length
  if (pw.length() < 8 || pw.length() > 16) {
    cout << "Password should be within 8 and 16 characters.\n";
    return false;
  } else if (pw[0] == '-') {
    cout << "Password should not begin with '-'.\n";
    return false;
  }

  sql::PreparedStatement* pstmt;
  string expr = "UPDATE " + table_ + " SET " + encrypt_pw_column_ +
                " = ? WHERE " + user_column_ + " = ?";
  cout << expr << endl;

  pstmt = con_->prepareStatement(expr);
  pstmt->setString(1, new_pw);
  pstmt->setString(2, user);
  int count = pstmt->executeUpdate();

  delete pstmt;

  if (count != 1)
    throw exception(
        "In AccountManager::ChangePw, changed count not equal to 1.\n");

  return true;
}

bool AccountManager::Create(const string& user, const string& pw) const {
  // need to check the length of pw and user
  if (user.length() < 1 || user.length() > 32) {
    cout << "User name should be within 1 and 32 characters.\n";
    return false;
  } else if (user[0] == '-') {
    cout << "User name should not begin with '-'.\n";
    return false;
  }
  if (pw.length() < 8 || pw.length() > 16) {
    cout << "Password should be within 8 and 16 characters.\n";
    return false;
  } else if (pw[0] == '-') {
    cout << "Password should not begin with '-'.\n";
    return false;
  }

  sql::PreparedStatement* pstmt;
  sql::ResultSet* res;
  string expr1 = "SELECT " + user_column_ + " FROM " + table_ + " WHERE " +
                 user_column_ + " = ?";
  string expr2 = "INSERT INTO " + table_ + " (" + user_column_ + ", " +
                 encrypt_pw_column_ + ") VALUES (?, ?)";
  // cout << expr1 << endl;

  pstmt = con_->prepareStatement(expr1);
  pstmt->setString(1, user);
  res = pstmt->executeQuery();

  delete pstmt;

  if (res->next()) {
    cout << "The username already exists. Please use another name.\n";
    return false;
  }

  // cout << expr2 << endl;

  pstmt = con_->prepareStatement(expr2);
  pstmt->setString(1, user);
  pstmt->setString(2, bcrypt::generateHash(pw));
  int count = pstmt->executeUpdate();

  delete res;
  delete pstmt;

  if (count != 1)
    throw exception(
        "In AccountManager::Create, created count not equal to 1.\n");

  return true;
}

bool AccountManager::Delete(const string& user, const string& pw) const {
  sql::PreparedStatement* pstmt;
  sql::ResultSet* res;
  string expr1 = "SELECT " + encrypt_pw_column_ + " FROM " + table_ +
                 " WHERE " + user_column_ + " = ?";
  string expr2 = "DELETE FROM " + table_ + " WHERE " + user_column_ + " = ?";
  int count = 0;
  // cout << expr1 << endl;

  pstmt = con_->prepareStatement(expr1);
  pstmt->setString(1, user);
  res = pstmt->executeQuery();

  delete pstmt;

  if (!res->next()) {
    cout << "The username doesn't exist. Please check your username.\n";
    return false;
  }
  if (bcrypt::validatePassword(pw, res->getString(encrypt_pw_column_))) {
    // cout << expr2 << endl;

    pstmt = con_->prepareStatement(expr2);
    pstmt->setString(1, user);
    count = pstmt->executeUpdate();

    delete pstmt;

    // TODO : also delete the file belonging to the user
  } else {
    cout << "The password is wrong. Please retry.\n";
    return false;
  }

  delete res;

  if (count != 1)
    throw exception(
        "In AccountManager::Create, created count not equal to 1.\n");

  return true;
}

void AccountManager::PrintTable_Insecure() const {
  sql::Statement* stmt;
  sql::ResultSet* res;
  string expr = "SELECT * FROM " + table_;

  stmt = con_->createStatement();
  res = stmt->executeQuery(expr);

  cout << "Table " << table_ << " :\n";
  cout << user_column_ << '\t' << encrypt_pw_column_ << '\n';
  while (res->next()) {
    cout << res->getString(user_column_) << '\t'
         << res->getString(encrypt_pw_column_) << '\n';
  }

  delete stmt;
  delete res;
}

string TakePwSecure() {
  cout << "[password]: ";

  enum IN { kBackspace = 8, kReturn = 32 };

  string pw = "";
  char ch;

  while (true) {
    ch = _getch();
    if (ch < IN::kReturn && ch != IN::kBackspace) {
      cout << '\n';
      return pw;
    } else if (ch == IN::kBackspace) {
      if (pw.size() > 0) {
        pw.pop_back();
      }
    } else {
      pw.push_back(ch);
    }
  }
}

bool UserLogin(const AccountManager& am, const vector<string>& cmd,
               string& user) {
  string pw = "";

  if (!ExtractUser(cmd, user)) return false;
  if (!ExtractPw(cmd, pw)) return false;

  if (am.Login(user, pw)) {
    cout << user << " logs in.\n";
    return true;
  } else {
    return false;
  }
}

bool UserChangePw(const AccountManager& am, const vector<string>& cmd,
                  string& user) {
  string pw = "";
  string new_pw;
  string confirm_new_pw;

  if (!ExtractUser(cmd, user)) return false;
  if (!ExtractPw(cmd, pw)) return false;
  cout << "Please input your new password.\n";
  new_pw = TakePwSecure();
  cout << "Please confirm your new password.\n";
  confirm_new_pw = TakePwSecure();
  if (new_pw != confirm_new_pw) {
    cout << "You entered two different passwords.\n";
    return false;
  }

  if (!am.ChangePw(user, pw, new_pw)) {
    return false;
  } else {
    cout << user << " changed password.\n";
    return true;
  }
}

bool CreatAccount(const AccountManager& am, const vector<string>& cmd,
                  string& user) {
  string pw;
  string confirm_pw;

  if (!ExtractUser(cmd, user)) return false;
  if (!ExtractPw(cmd, pw)) return false;
  // comfirm pw
  cout << "Please confirm your password.\n";
  confirm_pw = TakePwSecure();
  if (pw != confirm_pw) {
    cout << "You entered two different password. Please retry.\n";
    return false;
  }

  if (!am.Create(user, pw)) {
    return false;
  } else {
    cout << user << " created.\n";
    return true;
  }
}

bool DeleteAccount(const AccountManager& am, const vector<string>& cmd,
                   string& user) {
  string pw;

  if (!ExtractUser(cmd, user)) return false;
  if (!ExtractPw(cmd, pw)) return false;

  if (!am.Delete(user, pw)) {
    return false;
  } else {
    cout << user << " deleted.\n";
    return true;
  }
}

bool ExtractUser(const vector<string>& cmd, string& user) {
  user = "";
  auto it = find(cmd.begin(), cmd.end(), "-u");
  if (it == cmd.end()) {
    cout << "No Option \"-u\".\n";
    return false;
  } else if (++it == cmd.end() || (*it)[0] == '-') {
    cout << "No content behind Option \"-u\".\n";
    return false;
  }
  user = *it;

  return true;
}

bool ExtractPw(const vector<string>& cmd, string& pw) {
  pw = "";
  auto it = find(cmd.begin(), cmd.end(), "-p");
  if (it == cmd.end()) {
    cout << "No Option \"-p\".\n";
    return false;
  } else if (++it != cmd.end() && (*it)[0] != '-') {
    cout << "Recommend not to input your password explicitly in the command "
            "line since it's insecure.\n";
    pw = *it;
  } else {
    pw = TakePwSecure();
  }
  return true;
}
