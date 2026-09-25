#include <stdio.h>

#include <vulkan/vulkan.h>

int main(void) {
  VkApplicationInfo app = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .apiVersion = VK_API_VERSION_1_4,
  };
  printf("Vulkan %u.%u.%u\n",
    VK_API_VERSION_MAJOR(VK_HEADER_VERSION_COMPLETE),
    VK_API_VERSION_MINOR(VK_HEADER_VERSION_COMPLETE),
    VK_API_VERSION_PATCH(VK_HEADER_VERSION_COMPLETE));
  return app.apiVersion == VK_MAKE_API_VERSION(0, 1, 4, 0) ? 0 : 1;
}
