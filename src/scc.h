#include "FileManager.h"
#include "PreProcessor.h"

namespace scc {
class Driver {
  public:
    Driver(int argc, char **argv) : _fm(), _cpp(_fm) {};
    int run(void);

  private:
    FileManager  _fm;
    PreProcessor _cpp;
};
}; // namespace scc
