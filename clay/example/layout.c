#define CLAY_IMPLEMENTATION
#include <clay.h>

#include <stdio.h>
#include <stdlib.h>

static Clay_Dimensions measure_text(Clay_StringSlice text, Clay_TextElementConfig* config, void* user_data) {
  (void)user_data;
  return (Clay_Dimensions) { .width = (float)text.length * config->fontSize * 0.5f, .height = config->fontSize };
}

int main(void) {
  uint32_t size = Clay_MinMemorySize();
  Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(size, malloc(size));
  Clay_Initialize(arena, (Clay_Dimensions) { 640, 480 }, (Clay_ErrorHandler) { 0 });
  Clay_SetMeasureTextFunction(measure_text, NULL);

  Clay_BeginLayout();
  CLAY(CLAY_ID("root"), { .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(16) }, .backgroundColor = { 32, 32, 32, 255 } }) {
    CLAY(CLAY_ID("header"), { .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(48) } }, .backgroundColor = { 200, 80, 40, 255 } }) {
      CLAY_TEXT(CLAY_STRING("hello, clay"), CLAY_TEXT_CONFIG({ .fontSize = 24, .textColor = { 255, 255, 255, 255 } }));
    }
  }
  Clay_RenderCommandArray commands = Clay_EndLayout();

  static const char* names[] = { "none", "rectangle", "border", "text", "image", "scissor_start", "scissor_end", "custom" };
  for (int32_t i = 0; i < commands.length; i++) {
    Clay_RenderCommand* command = Clay_RenderCommandArray_Get(&commands, i);
    Clay_BoundingBox box = command->boundingBox;
    printf("%-13s x=%g y=%g w=%g h=%g\n", names[command->commandType], box.x, box.y, box.width, box.height);
  }

  free(arena.memory);
  return 0;
}
