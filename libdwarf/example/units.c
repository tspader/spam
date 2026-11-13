#include <libdwarf.h>
#include <stdio.h>

int main(int argc, char** argv) {
  const char* path = argc > 1 ? argv[1] : argv[0];
  Dwarf_Debug dbg = 0;
  Dwarf_Error error = 0;
  if (dwarf_init_path(path, 0, 0, DW_GROUPNUMBER_ANY, 0, 0, &dbg, &error) != DW_DLV_OK) {
    printf("no debug info in %s\n", path);
    return 1;
  }

  int units = 0;
  Dwarf_Die die = 0;
  Dwarf_Unsigned next = 0;
  Dwarf_Half type = 0;
  Dwarf_Half version = 0;
  Dwarf_Half address_size = 0;
  Dwarf_Half offset_size = 0;
  while (dwarf_next_cu_header_e(dbg, 1, &die, 0, &version, 0, &address_size, &offset_size, 0, 0, 0, &next, &type, &error) == DW_DLV_OK) {
    char* name = 0;
    if (dwarf_diename(die, &name, &error) == DW_DLV_OK) {
      printf("%s\n", name);
    }
    dwarf_dealloc_die(die);
    units++;
  }

  printf("libdwarf %s: %d compile units\n", dwarf_package_version(), units);
  dwarf_finish(dbg);
  return 0;
}
