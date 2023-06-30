
#include "Testing.hpp"
#include "Utils.hpp"

#include <cstdlib>

static bool init(const tp::ModuleManifest* self) {
  tp::gTesting.setRootName(self->getName());
  return true;
}

void testMath();

int main() {

  tp::ModuleManifest* deps[] = { &tp::gModuleUtils, nullptr };
  tp::ModuleManifest testModule("ContainersTest", init, nullptr, deps);

  if (!testModule.initialize()) {
    return 1;
  }

  testMath();

  testModule.deinitialize();
}
