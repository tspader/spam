#include <dbus/dbus.h>

#include <stdio.h>

int main(void) {
  printf("dbus %s (%d.%d.%d)\n", DBUS_VERSION_STRING, DBUS_MAJOR_VERSION, DBUS_MINOR_VERSION, DBUS_MICRO_VERSION);
  printf("int64 is %zu bytes, void* is %d bytes\n", sizeof(dbus_int64_t), DBUS_SIZEOF_VOID_P);
  printf("max name length: %d\n", DBUS_MAXIMUM_NAME_LENGTH);
  return sizeof(dbus_int64_t) == 8 && sizeof(dbus_int32_t) == 4 && sizeof(dbus_int16_t) == 2 && sizeof(void*) == DBUS_SIZEOF_VOID_P ? 0 : 1;
}
