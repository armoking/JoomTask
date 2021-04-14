#include "sort_lines_in_file.h"
#include "test_generator.h"


int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);

  Generate(1e7, 20);  
  
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
  
  size_t size = SortLinesInFile();
  
  auto out = freopen(filename.c_str(), "w", stdout);
  
  
  fwrite(DATA, 1, size, out);
  
  fflush(out);
  fclose(out);
  
  return 0;
  
}
