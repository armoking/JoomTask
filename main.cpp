/*
  execute with:
  g++ -std=c++17 -Ofast main.cpp -o main && ./main (*NIX)
  g++ -std=c++17 -Ofast main.cpp -o main && main.exe (Windows)
 */

#pragma GCC Optimize("Ofast")

#define THREADS 1

#include "sort_lines_in_file.h"
#include "test_generator.h"


int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);

  Generate(1e7, 5);
  std::cout << "Generated" << std::endl;
  const std::string filename = "test_data.txt";
  memset(DATA, -1, sizeof DATA);
  auto in = freopen(filename.c_str(), "r", stdin);
  
  size_t index = 0;
  char ch;
  
  while ((ch = getchar()) != EOF) {
    Set(index++, ch);
  }
  
  Set(index++, EOF);
  Set(index++, EOF);
  
  fclose(in);
  
  std::cout << "Read" << std::endl;
  
  auto start = clock();
  size_t size = SortLinesInFile();
  std::cout << (THREADS ? "Multithreads mode with " + to_string(MAX_THREADS) + " threads" : "Single thread") << std::endl;
  std::cout << "Used time is: " << ((clock() - start) * 1000.0 / CLOCKS_PER_SEC) << " ms." << std::endl;
  
  std::cout << "Sort finished" << std::endl;
  
  // check correctness of the result
  {
    string pre = "";
    string cur = "";
    for (size_t i = 0; i < size; i++) {
      cur = "";
      while (Get(i) != '\n' && i < size) {
        cur.push_back(Get(i));
        i++;
      }
      if (cur >= pre) {
        pre = cur;
      } else {
        std::cerr << pre << " > " << cur << std::endl;
        std::cerr << "ERROR, BAD SORT" << std::endl;
        exit(-1);
      }
    }
  }
  
  ofstream out;
  out.open(filename);
  for (size_t i = 0; i < size; i++) {
    out << Get(i);
  }
  out.flush();
  out.close();
  
  return 0;
}
