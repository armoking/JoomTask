#include <vector>
#include <iostream>
#include <random>
#include <cstring>
#include <tuple> 
#include <thread>
#include <atomic>

// CONSTANTS
const size_t MAX_DEPTH_CONSTANT = 25;
const size_t MAX_SIZE = 1e9;

// GLOBAL VALUES
static std::atomic<char> DATA[MAX_SIZE];
static std::atomic<int> thread_counter{0};
static const int MAX_THREADS = 32;


// DATA immitates file, it is all because I use Windows (b.o. my university)
// this OS gives '\r' besides '\n' at ends of lines, so it really cannot be read good from the files without
// any good primitive parser to *NIX-style text (without '\r')

// So, I just realized functions like Get(size_t k) that return k-th character of the file
// and Set(size_t k, char ch) that set k-th character of the file to the "ch"


char Get(size_t index) {
  return DATA[index];
}

void Set(size_t index, char ch) {
  DATA[index] = ch;
}


/*
  In case of a normal OS (*NIX like) this code may look like this
char Get(size_t index, FILE* file) {  
  rewind(file);
  fseek(file, index, SEEK_SET);
  char ch = fgetc(file);
  return ch;
}
void Set(size_t index, char ch, FILE* file) {
  rewind(file);
  fseek(file, index, SEEK_SET);
  fputc(ch, file);
}
 */


/*
 This function calculates the number of lines, the longest line and the length of the file
 */
std::tuple<size_t, size_t, size_t> CalculateFileProperties() {  
  size_t max_length_of_line = 0;
  size_t current_length = 0;
  size_t number_of_lines = 0;
  size_t length = 0;
  
  for (char current = Get(length); current != EOF; current = Get(length)) {
    ++current_length;
    ++length;
    if (current == '\n') { // new line
      max_length_of_line = std::max(current_length, max_length_of_line);
      current_length = 0;
      ++number_of_lines;
    }
  }
  
  if (Get(length - 1) != '\n') {
    Set(length, '\n');
    ++length;
    ++current_length;
    ++number_of_lines;
  }
 
  max_length_of_line = std::max(max_length_of_line, current_length);  
  
  return std::tuple<size_t, size_t, size_t>(number_of_lines, max_length_of_line, length);
}


void Debug() {
  for (size_t i = 0; DATA[i] != EOF; i++) {
    std::cout << DATA[i]; 
  }
  std::cout << std::endl;
}


/*
  This function adds to all of the lines in the file some number of EOF to make
  their lengths equal (for random access in O(1) to each line)
 */
void MakeLinesEqual(size_t number_of_lines, size_t max_length_of_line, size_t length) {
  size_t needed_size_of_file = max_length_of_line * number_of_lines;
  size_t number_of_EOF_chars = needed_size_of_file - length;
  
  for (size_t index = 0; index < number_of_EOF_chars; ++index) { 
    Set(length + index, EOF);
  }
  
  
  size_t last_ptr = needed_size_of_file - 1;
  size_t current_position = length - 1;
  
  for (size_t index_of_line = number_of_lines; index_of_line > 0; --index_of_line) {
    size_t to = current_position;
    size_t current_length = 0;
    std::string current_line;

    
    while (true) {
      char current_char = Get(current_position);
      if (current_char == '\n' && current_length != 0) {
        break;
      }
      
      current_line.push_back(current_char);
      
      ++current_length;
      --current_position; // overflow, but it is ok
      
      if (current_position + 1 == 0) {
        break;
      }
    }
    
    
    size_t current_zeros = max_length_of_line - current_length;
    
    for (size_t index = 0; index < current_zeros; ++index) {
      Set(last_ptr, EOF);
      --last_ptr;
    }
    
    for (size_t index = 0; index < current_length; index++) {
      Set(last_ptr, current_line[index]);
      --last_ptr;
    }
    
  }
}


/*
  Compare two lines that store in positions
  from i to i + len - 1 
  and from j to j + len - 1 
  */
bool CompareLines(size_t i, size_t j, size_t len) {
  if (i == j) return false;
  for (size_t index = 0; index < len; ++index) {
    char first = Get(i + index);
    char second = Get(j + index);
    if (first == second) {
      continue;
    }
    return first < second;
  }
  return false;
}


/*
 Swap two lines
 */
void SwapLines(size_t i, size_t j, size_t len) {
  if (i == j) return;
  for (size_t index = 0; index < len; ++index) {
    char first = Get(i + index);
    char second = Get(j + index);
    Set(j + index, first);
    Set(i + index, second);
  }
}


/*
  This function removes EOFs at the end of all lines
 */
size_t RemoveAllZeros(size_t length) {
  size_t index = 0;
  size_t len = 0;
  for (size_t i = 0; i < length; ++i) {
    char cur = Get(i);
    if (cur != EOF) {
      Set(index++, cur);
    }
  }
  for (size_t i = index; i < length; ++i) {
    Set(i, EOF);
  }
  return index;
}


/*
 Function for HeapSort
 */
void RelaxUp(size_t root, size_t v, size_t max_length) {
  v -= root;
  while (v != 0) {
    size_t parent = (v - 1) / 2;
    if (CompareLines((parent + root) * max_length, (v + root) * max_length, max_length)) {
      SwapLines((parent + root) * max_length, (v + root) * max_length, max_length);
      v = parent;
    } else {
      break;
    }
  }
}


/*
 Function for HeapSort
 */
void RelaxDown(size_t v, size_t n, size_t max_length) {
  size_t delta = v;
  n -= v;
  v = 0;
  while (v * 2 + 1 < n) {
    size_t left = v * 2 + 1;
    size_t right = v * 2 + 2;
    if (v * 2 + 2 < n && CompareLines((left + delta) * max_length, (right + delta) * max_length, max_length)) {
      left = right;
    }     
    if (CompareLines((v + delta) * max_length, (left + delta) * max_length, max_length)) {
      SwapLines((v + delta) * max_length, (left + delta) * max_length, max_length);
      v = left;
    } else {
      break;
    }
  }
}


/*
 Main function of HeapSort
 */
void HeapSort(size_t left, size_t right, size_t max_length) {
  for (size_t index = left + 1; index <= right; ++index) {
    RelaxUp(left, index, max_length);
  }

  for (size_t index = right; index > left; --index) {
    SwapLines(left * max_length, index * max_length, max_length);
    RelaxDown(left, index, max_length);
  }
}

/*
 Main function of TimSort
 Timsort is a combination of QuickSort and HeapSort
 (HeapSort in case if the recursion depth is large enough)
 (P.S. This part can be done in multithreads)
 */
void TimSort(size_t left, size_t right, size_t max_length, size_t depth = 0) {
  if (left >= right) {
    return;
  }
  
  if (depth >= MAX_DEPTH_CONSTANT) {
    HeapSort(left, right, max_length);
    return;
  }
  
  size_t mid = left + (right - left) / 2; // Can be selected randomly
  
  size_t lower = left;
  for (size_t index = left; index <= right; index++) {
    if (CompareLines(index * max_length, mid * max_length, max_length)) {
      lower++;
    }
  }
  
  SwapLines(mid * max_length, lower * max_length, max_length);
  mid = lower;
  
  size_t iter = left;
  
  for (size_t index = left; index <= right; index++) {
    if (CompareLines(index * max_length, mid * max_length, max_length)) {
      SwapLines(index * max_length, iter * max_length, max_length);
      ++iter;
    }
  }
  
  #if THREADS
    if (mid > 0) {
      if (thread_counter.load() < MAX_THREADS - 2) {
        thread_counter.fetch_add(2);
        
        std::thread thread1(TimSort, left, mid - 1, max_length, depth + 1);
        std::thread thread2(TimSort, mid + 1, right, max_length, depth + 1);
        
        thread1.join();
        thread2.join();
        
        thread_counter.fetch_add(-2);
      } else if (thread_counter.load() < MAX_THREADS - 1) {
        thread_counter.fetch_add(1);
        
        std::thread thread(TimSort, left, mid - 1, max_length, depth + 1);
        TimSort(mid + 1, right, max_length, depth + 1);
      
        thread.join();
        thread_counter.fetch_add(-1);
      } else {
        TimSort(left, mid - 1, max_length, depth + 1);
        TimSort(mid + 1, right, max_length, depth + 1);
      }
    } else {
      TimSort(mid + 1, right, max_length, depth + 1);
    }
  #else
    if (mid > 0) {
      TimSort(left, mid - 1, max_length, depth + 1);
      TimSort(mid + 1, right, max_length, depth + 1);
    } else {
      TimSort(mid + 1, right, max_length, depth + 1);
    }
  #endif
}


/*
 Function for sorting all lines in the file
 */
size_t SortLinesInFile() {
  auto [number_of_lines, max_length, length] = CalculateFileProperties();

  MakeLinesEqual(number_of_lines, max_length, length);
  
  TimSort(0, number_of_lines - 1, max_length);
  
  return RemoveAllZeros(max_length * number_of_lines);
}
