
// avoid errors about strcpy, _snprintf, localtime
#define _CRT_SECURE_NO_WARNINGS

#include "file_encrypt.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include <fstream>

#include "AES.h"

using namespace std;

void EncryptFile(string filename, string en_filename,
  string key) {
  //cout << "\nIn EncryptFile:\n";

  vector<unsigned char> unch_key;
  vector<unsigned char> plain_data;
  vector<unsigned char> en_data;

  File2VectorUnCh(filename, plain_data);

  for (int i = plain_data.size() % 16; plain_data.size() % 16 != 0; i++) {
    plain_data.push_back((unsigned char)' ');
  }
  for (int i = 0; i < key.length(); i++) {
    unch_key.push_back((unsigned char)key[i]);
  }
  for (int i = unch_key.size() % 16; unch_key.size() % 16 != 0; i++) {
    unch_key.push_back((unsigned char)'*');
  }
  int size = plain_data.size();

  //cout << "plain_data:\n";
  //for (int i = 0; i < size; i++) {
  //  cout << (char)plain_data[i];
  //}
  //cout << endl;

  //cout << "plain_data.size(): " << size << endl;

  AES aes(AESKeyLength::AES_128);
  en_data = aes.EncryptECB(plain_data, unch_key);

  VectorUnCh2BinFile(en_data, en_filename);

  //cout << "End of EncryptFile.\n\n";
}

void DecryptFile(string filename, string de_filename,
  string key) {
  //cout << "\nIn DecryptFile:\n";

  vector<unsigned char> unch_key;
  vector<unsigned char> en_data;
  vector<unsigned char> de_data;

  BinFile2VectorUnCh(filename, en_data);

  for (int i = 0; i < key.length(); i++) {
    unch_key.push_back((unsigned char)key[i]);
  }
  for (int i = unch_key.size() % 16; unch_key.size() % 16 != 0; i++) {
    unch_key.push_back((unsigned char)'*');
  }

  AES aes(AESKeyLength::AES_128);
  de_data = aes.DecryptECB(en_data, unch_key);
  
  //cout << "de_data:\n";
  //for (int i = 0; i < de_data.size(); i++) {
  //  cout << (char)de_data[i];
  //}
  //cout << "\n";

  VectorUnCh2File(de_data, de_filename);

  //cout << "End of DecryptFile.\n\n";
}


void File2VectorUnCh(const string& filename, vector<unsigned char>& data) {
  //cout << "\nIn File2VectorUnCh:\n";

  FILE* file;
  file = fopen(filename.data(), "r");
  if (!file) {
    throw exception("No such file.\n");
  }
  unsigned char ch;
  while ((ch = fgetc(file)) != (unsigned char)EOF) {
    data.push_back(ch);
  }
  fclose(file);

  //cout << "data.size(): " << data.size() << endl;
  //cout << "End of File2VectorUnCh.\n\n";
}

void VectorUnCh2BinFile(const vector<unsigned char>& data,
  const string& filename) {
  //cout << "\nIn VectorUnCh2BinFile:\n";

  ofstream file;
  file.open(filename.data(), ios::binary | ios::out);
  file.write((char*)&data[0], data.size() * sizeof(unsigned char));

  //cout << "file.tellp(): " << file.tellp() << endl;

  file.close();
  
  //cout << "End of VectorUnCh2BinFile.\n\n";
}

void BinFile2VectorUnCh(const string& filename, vector<unsigned char>& data) {
  //cout << "\nIn BinFile2VectorUnCh:\n";

  ifstream file;
  file.open(filename.data(), ios::binary | ios::in);
  if (!file) {
    throw exception("No such file.\n");
  }

  file.seekg(0, ios::end);
  int count = file.tellg();
  file.seekg(0, ios::beg);

  for (int i = 0; i < count; i++) {
    char ch;
    file.read(&ch, sizeof(char));
    data.push_back((unsigned char)ch);
  }
  
  //cout << "count: " << count << endl;
  //cout << "data.size(): " << data.size() << endl;

  file.close();

  //cout << "End of BinFile2VectorUnCh.\n\n";
}

void VectorUnCh2File(const vector<unsigned char>& data,
                     const string& filename) {
  //cout << "\nIn VectorUnCh2File:\n";

  ofstream file;
  file.open(filename.data(), ios::out);
  file.write((char*)&data[0], data.size() * sizeof(unsigned char));
  file.close();

  //cout << "End of VectorUnCh2File.\n\n";
}
