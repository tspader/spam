#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdio.h>

static void on_error(int code, const char* description) {
  fprintf(stderr, "glfw error %d: %s\n", code, description);
}

int main(void) {
  glfwSetErrorCallback(on_error);
  if (!glfwInit()) {
    return 1;
  }

  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  GLFWwindow* window = glfwCreateWindow(320, 240, "spn", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);
  printf("%s\n", glfwGetVersionString());

  for (int frame = 0; frame < 10 && !glfwWindowShouldClose(window); frame++) {
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
