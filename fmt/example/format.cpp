#include "fmt/format.h"
#include "fmt/ranges.h"

#include <string>
#include <vector>

struct rgb {
  unsigned char r, g, b;
};

template <> struct fmt::formatter<rgb> : fmt::formatter<std::string> {
  auto format(const rgb& c, fmt::format_context& ctx) const {
    return fmt::formatter<std::string>::format(
        fmt::format("#{:02X}{:02X}{:02X}", c.r, c.g, c.b), ctx);
  }
};

int main() {
  fmt::print("fmt {}.{}.{}\n", FMT_VERSION / 10000, FMT_VERSION / 100 % 100,
             FMT_VERSION % 100);

  fmt::print("positional: {1} then {0}\n", "second", "first");
  fmt::print("named: {who} scored {score:.2f}%\n", fmt::arg("who", "ada"),
             fmt::arg("score", 99.4567));

  std::vector<std::string> names = {"alpha", "beta", "gamma"};
  std::vector<int> counts = {7, 128, 4096};
  fmt::print("{:-^34}\n", " table ");
  for (std::size_t i = 0; i < names.size(); ++i) {
    fmt::print("|{:<10}|{:>8}|{:^12}|\n", names[i], counts[i],
               fmt::format("{:#08x}", counts[i]));
  }

  fmt::print("joined: {}\n", fmt::join(counts, " + "));
  fmt::print("range: {}\n", names);

  rgb dodger{0x1e, 0x90, 0xff};
  fmt::print("custom: {} padded [{:>12}]\n", dodger, dodger);

  std::string spec = "runtime: {:.3f} and {:.3e}\n";
  double pi = 3.14159265358979;
  double avogadro = 6.02214076e23;
  fmt::print(fmt::runtime(spec), pi, avogadro);

  fmt::print("formatted_size of {{:>20}} = {}\n",
             fmt::formatted_size("{:>20}", "right"));
  return 0;
}
