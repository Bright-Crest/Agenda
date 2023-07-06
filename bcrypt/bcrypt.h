#ifndef BCRYPT_H
#define BCRYPT_H

#define _CRT_SECURE_NO_WARNINGS

#include <string>

namespace bcrypt {

    std::string generateHash(const std::string & password , unsigned rounds = 10 );

    bool validatePassword(const std::string & password, const std::string & hash);

}

#endif // BCRYPT_H
