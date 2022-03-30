#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>

using namespace std;

void convertStringToFile(string fileName, string s) {
  fstream out(fileName.c_str(), ios::out);
  if (out.is_open()) {
    out << s;
  }
  out.close();
}

/*
  return random integer range from [min,max].
*/
int getRandomINT(int min, int max) {
  unsigned seed = time(0);
  srand(seed);

  return rand() % (max - min + 1) + min;
}