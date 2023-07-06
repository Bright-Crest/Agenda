#pragma once
#include <iostream>
#include <string>

#include "AES.h"

using namespace std;

void EncryptFile(const string& filename, const string& en_filename, string key);
void DecryptFile(const string& filename, const string& de_filename, string key); 
void File2VectorUnCh(const string& filename, vector<unsigned char>& data);
void VectorUnCh2BinFile(const vector<unsigned char>& data, const string& filename);
void BinFile2VectorUnCh(const string& filename, vector<unsigned char>& data);
void VectorUnCh2File(const vector<unsigned char>& data, const string& filename);
