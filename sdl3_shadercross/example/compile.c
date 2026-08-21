#include <SDL3_shadercross/SDL_shadercross.h>
#include <stdio.h>

static const char HLSL[] =
    "float4 main(float4 color : COLOR) : SV_Target {\n"
    "  return color * 0.25;\n"
    "}\n";

int main() {
  if (!SDL_ShaderCross_Init()) {
    fprintf(stderr, "SDL_ShaderCross_Init failed: %s\n", SDL_GetError());
    return 1;
  }

  SDL_ShaderCross_HLSL_Info info = {
    .source = HLSL,
    .entrypoint = "main",
    .shader_stage = SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT,
  };
  size_t size = 0;
  void* spirv = SDL_ShaderCross_CompileSPIRVFromHLSL(&info, &size);
  if (!spirv || size < 4) {
    fprintf(stderr, "CompileSPIRVFromHLSL failed: %s\n", SDL_GetError());
    return 1;
  }
  Uint32 magic = 0;
  SDL_memcpy(&magic, spirv, 4);
  if (magic != 0x07230203) {
    fprintf(stderr, "bad spirv magic: 0x%08x\n", magic);
    return 1;
  }
  printf("spirv: %u bytes\n", (unsigned)size);

  SDL_ShaderCross_GraphicsShaderMetadata* metadata = SDL_ShaderCross_ReflectGraphicsSPIRV(spirv, size, 0);
  if (!metadata) {
    fprintf(stderr, "ReflectGraphicsSPIRV failed: %s\n", SDL_GetError());
    return 1;
  }
  printf("samplers: %u uniform_buffers: %u\n", (unsigned)metadata->num_samplers, (unsigned)metadata->num_uniform_buffers);

  SDL_free(metadata);
  SDL_free(spirv);
  SDL_ShaderCross_Quit();
  printf("ok\n");
  return 0;
}
