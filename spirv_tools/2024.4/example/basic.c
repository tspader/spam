#include <spirv-tools/libspirv.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  const char* text =
      "OpCapability Shader\n"
      "OpMemoryModel Logical GLSL450\n"
      "OpEntryPoint GLCompute %main \"main\"\n"
      "OpExecutionMode %main LocalSize 1 1 1\n"
      "%void = OpTypeVoid\n"
      "%fn = OpTypeFunction %void\n"
      "%main = OpFunction %void None %fn\n"
      "%entry = OpLabel\n"
      "OpReturn\n"
      "OpFunctionEnd\n";

  spv_context context = spvContextCreate(SPV_ENV_VULKAN_1_1);
  if (!context) {
    return 1;
  }

  spv_binary binary = NULL;
  spv_diagnostic diagnostic = NULL;
  if (spvTextToBinary(context, text, strlen(text), &binary, &diagnostic) != SPV_SUCCESS) {
    spvDiagnosticPrint(diagnostic);
    return 1;
  }

  if (spvValidateBinary(context, binary->code, binary->wordCount, &diagnostic) != SPV_SUCCESS) {
    spvDiagnosticPrint(diagnostic);
    return 1;
  }

  printf("%zu bytes\n", binary->wordCount * sizeof(uint32_t));
  printf("valid\n");

  spvBinaryDestroy(binary);
  spvContextDestroy(context);
  return 0;
}
