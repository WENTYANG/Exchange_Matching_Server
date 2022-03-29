#include "server.h"

int main() {
  Server s("12345");
  s.run();
  return 0;
}