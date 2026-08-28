// Recursively hashes every regular file under a directory into a single
// digest. Paths feed the hasher along with contents, so a rename changes
// the result. Symlinks and unreadable entries are skipped.
#ifndef _WIN32
  #define _POSIX_C_SOURCE 200809L
#endif

#include <blake3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
  #include <windows.h>
  #define TREE_DEFAULT_ROOT "C:\\Windows\\System32"
#else
  #include <dirent.h>
  #include <sys/stat.h>
  #define TREE_DEFAULT_ROOT "/usr/lib"
#endif

static blake3_hasher hasher;
static unsigned long long num_files = 0;
static unsigned long long num_bytes = 0;

static void hash_file(const char* path) {
  FILE* file = fopen(path, "rb");
  if (!file) {
    return;
  }
  blake3_hasher_update(&hasher, path, strlen(path) + 1);
  unsigned char buffer[65536];
  size_t len;
  while ((len = fread(buffer, 1, sizeof(buffer), file)) > 0) {
    blake3_hasher_update(&hasher, buffer, len);
    num_bytes += len;
  }
  fclose(file);
  num_files++;
}

#ifdef _WIN32
static void walk(const char* dir) {
  char pattern[4096];
  snprintf(pattern, sizeof(pattern), "%s\\*", dir);
  WIN32_FIND_DATAA entry;
  HANDLE find = FindFirstFileA(pattern, &entry);
  if (find == INVALID_HANDLE_VALUE) {
    return;
  }
  do {
    if (!strcmp(entry.cFileName, ".") || !strcmp(entry.cFileName, "..")) {
      continue;
    }
    if (entry.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
      continue;
    }
    char path[4096];
    snprintf(path, sizeof(path), "%s\\%s", dir, entry.cFileName);
    if (entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      walk(path);
    }
    else {
      hash_file(path);
    }
  } while (FindNextFileA(find, &entry));
  FindClose(find);
}
#else
static void walk(const char* dir) {
  struct dirent** entries;
  int count = scandir(dir, &entries, NULL, alphasort);
  if (count < 0) {
    return;
  }
  for (int i = 0; i < count; i++) {
    const char* name = entries[i]->d_name;
    if (strcmp(name, ".") && strcmp(name, "..")) {
      char path[4096];
      snprintf(path, sizeof(path), "%s/%s", dir, name);
      struct stat info;
      if (!lstat(path, &info)) {
        if (S_ISDIR(info.st_mode)) {
          walk(path);
        }
        else if (S_ISREG(info.st_mode)) {
          hash_file(path);
        }
      }
    }
    free(entries[i]);
  }
  free(entries);
}
#endif

int main(int argc, char** argv) {
  const char* root = argc > 1 ? argv[1] : TREE_DEFAULT_ROOT;
  blake3_hasher_init(&hasher);
  walk(root);
  if (!num_files) {
    printf("no files under %s\n", root);
    return 1;
  }

  unsigned char digest[BLAKE3_OUT_LEN];
  blake3_hasher_finalize(&hasher, digest, sizeof(digest));
  printf("%llu files, %llu bytes under %s\n", num_files, num_bytes, root);
  for (size_t i = 0; i < sizeof(digest); i++) {
    printf("%02x", digest[i]);
  }
  printf("\n");
  return 0;
}
