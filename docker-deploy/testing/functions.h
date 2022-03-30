#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>

using namespace std;

void convertStringToFile(string fileName, string s);

/*
  return random integer range from [min,max].
*/
int getRandomINT(int min, int max);

#endif