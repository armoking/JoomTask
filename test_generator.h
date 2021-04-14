#include <vector>
#include <iostream>
#include <random>
#include <cstring>


using namespace std;

mt19937 rnd(random_device{}());
const string alphabet = "qwertyuiopasdfghjklzxcvbnmQWERTYUIPOLKJHGFDSAZXCVBNM\n";

void Generate(size_t length_of_the_test, size_t max_length) {
  string str;
  int current_length = 0;
  for (int i = 0; i < length_of_the_test; i++) {
    str.push_back(alphabet[rnd() % alphabet.size()]);
    if (str.back() == '\n') {
      current_length = 0; 
    } else if (current_length + 1 == max_length) {
      str.push_back('\n');
      current_length = 0;
    }
  }
  
  FILE* out = freopen("test_data.txt", "w", stdout);
  cout << str;
  
  fflush(out);
  fclose(out);
}
