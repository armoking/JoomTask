#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include <cstring>


using namespace std;

mt19937 rnd(23);
const string alphabet = "qwertyuiopasdfghjklzxcvbnmQWERTYUIPOLKJHGFDSAZXCVBNM\n";

void Generate(size_t length_of_the_test, size_t max_length) {
  string str;
  
  int current_length = 0;
  
  for (int i = 0; i < length_of_the_test; i++) {
    str.push_back(alphabet[rnd() % alphabet.size()]);
    current_length++;
    if (str.back() == '\n') {
      current_length = 0; 
    } else if (current_length >= max_length) {
      str.push_back('\n');
      current_length = 0;
    }
  }
  
  ofstream out;
  
  out.open("test_data.txt");
  out << str;
  out.close();
}
