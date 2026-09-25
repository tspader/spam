#include <cstdio>

#include <vulkan/vulkan.hpp>

int main() {
  vk::ApplicationInfo app;
  app.apiVersion = vk::ApiVersion14;
  std::printf("%s\n", vk::to_string(vk::StructureType::eApplicationInfo).c_str());
  return app.apiVersion == vk::makeApiVersion(0, 1, 4, 0) ? 0 : 1;
}
