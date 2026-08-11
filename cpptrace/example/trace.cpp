#include <cpptrace/cpptrace.hpp>

#include <cstdio>

void leaf() {
  cpptrace::generate_trace().print();
}

void branch() {
  leaf();
}

int main() {
  std::printf("stacktrace from main:\n");
  branch();
  return 0;
}
