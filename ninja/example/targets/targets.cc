#include "src/disk_interface.h"
#include "src/graph.h"
#include "src/manifest_parser.h"
#include "src/state.h"

extern "C" bool ninja_load_targets(
  const char* path,
  void (*emit)(void* user, const char* target, const char* rule),
  void* user,
  char* error,
  unsigned capacity
) {
  State state;
  RealDiskInterface disk;
  ManifestParser parser(&state, &disk);
  std::string message;
  if (!parser.Load(path, &message)) {
    unsigned length = message.size() < capacity - 1 ? (unsigned)message.size() : capacity - 1;
    message.copy(error, length);
    error[length] = 0;
    return false;
  }
  for (Edge* edge : state.edges_) {
    for (Node* output : edge->outputs_) {
      emit(user, output->path().c_str(), edge->rule().name().c_str());
    }
  }
  return true;
}
