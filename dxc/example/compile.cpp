#include <dxc/dxcapi.h>
#include <stdio.h>
#include <string.h>

static const char HLSL[] =
    "float4 main(float4 color : COLOR) : SV_Target {\n"
    "  return color * 0.5;\n"
    "}\n";

static IDxcBlob* run(IDxcCompiler3* compiler, const wchar_t** args, unsigned count) {
  DxcBuffer buffer = {};
  buffer.Ptr = HLSL;
  buffer.Size = sizeof(HLSL) - 1;
  buffer.Encoding = DXC_CP_UTF8;

  IDxcResult* result = nullptr;
  if (FAILED(compiler->Compile(&buffer, (LPCWSTR*)args, count, nullptr, __uuidof(IDxcResult), (void**)&result))) {
    return nullptr;
  }

  HRESULT status = E_FAIL;
  result->GetStatus(&status);
  if (FAILED(status)) {
    IDxcBlobUtf8* errors = nullptr;
    result->GetOutput(DXC_OUT_ERRORS, __uuidof(IDxcBlobUtf8), (void**)&errors, nullptr);
    if (errors && errors->GetBufferSize()) {
      fprintf(stderr, "%s\n", (const char*)errors->GetBufferPointer());
    }
    return nullptr;
  }

  IDxcBlob* object = nullptr;
  result->GetOutput(DXC_OUT_OBJECT, __uuidof(IDxcBlob), (void**)&object, nullptr);
  return object;
}

int main() {
  IDxcCompiler3* compiler = nullptr;
  if (FAILED(DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler3), (void**)&compiler))) {
    fprintf(stderr, "DxcCreateInstance failed\n");
    return 1;
  }

  const wchar_t* spirv_args[] = { L"-T", L"ps_6_0", L"-E", L"main", L"-spirv" };
  IDxcBlob* spirv = run(compiler, spirv_args, 5);
  if (!spirv || spirv->GetBufferSize() < 4) {
    fprintf(stderr, "spirv compile failed\n");
    return 1;
  }
  unsigned magic = 0;
  memcpy(&magic, spirv->GetBufferPointer(), 4);
  if (magic != 0x07230203) {
    fprintf(stderr, "bad spirv magic: 0x%08x\n", magic);
    return 1;
  }
  printf("spirv: %lu bytes\n", (unsigned long)spirv->GetBufferSize());

  const wchar_t* dxil_args[] = { L"-T", L"ps_6_0", L"-E", L"main" };
  IDxcBlob* dxil = run(compiler, dxil_args, 4);
  if (!dxil || dxil->GetBufferSize() < 4) {
    fprintf(stderr, "dxil compile failed\n");
    return 1;
  }
  if (memcmp(dxil->GetBufferPointer(), "DXBC", 4) != 0) {
    fprintf(stderr, "bad dxil container magic\n");
    return 1;
  }
  printf("dxil: %lu bytes\n", (unsigned long)dxil->GetBufferSize());

  return 0;
}
