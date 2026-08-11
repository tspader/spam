#include "fmt/os.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

int main() {
  const char* tmp = std::getenv("TMPDIR");
  std::string path = std::string(tmp ? tmp : "/tmp") + "/spn-fmt-example.txt";

  {
    fmt::ostream out = fmt::output_file(path);
    out.print("{:<8}{:>10}{:>12}\n", "step", "value", "ratio");
    double prev = 1.0;
    for (int i = 1; i <= 4; ++i) {
      double value = prev * 1.5 + i;
      out.print("{:<8}{:>10.3f}{:>12.4f}\n", i, value, value / prev);
      prev = value;
    }
  }

  fmt::print("wrote {}\n", path);

  std::ifstream in(path);
  std::string line;
  while (std::getline(in, line)) fmt::print("| {}\n", line);

  std::remove(path.c_str());
  return 0;
}
