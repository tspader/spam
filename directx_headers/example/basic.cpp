#include <stdio.h>

#ifndef _WIN32
#include <wsl/winadapter.h>
#endif

#include <directx/d3d12.h>

#ifndef _WIN32
#include <dxguids/dxguids.h>
#endif

int main(void) {
  printf("sizeof(D3D12_RESOURCE_DESC) = %u\n", (unsigned)sizeof(D3D12_RESOURCE_DESC));
#ifndef _WIN32
  GUID iid = uuidof<ID3D12Device>();
  printf("IID_ID3D12Device = %08x-%04x-%04x\n", (unsigned)iid.Data1, iid.Data2, iid.Data3);
#endif
  return 0;
}
