#include "scc/FileManager/MemoryBuffer.h"
#include <cstring>
#include <iostream>
#include <vector>

int cc1(int argc, char **argv, char **env);


int cc1(int argc, char** argv, char** env);

// ==== main ===============================================================
int main(int argc, char** argv, char** env) {
  // 1) cc1 fast-path (tool mode)
  if (argc >= 2 && std::strcmp(argv[1], "-cc1") == 0)
    return cc1(argc, argv, env);

  // 2) infra
//  FileManager FM;          // filesystem abstraction
//  Diagnostics Diag;        // owns sinks/printers
//  WarningSystem WS(Diag);  // warning flags, -Werror, etc.
//
//  // 3) parse args (raw, order-preserving)
//  OptionTable Opts = buildOptionTable();          // your table of flags
//  auto parseRes = parseCommandLine(Opts, argc, argv); // -> Args + diagnostics
//  if (!parseRes) {
//    Diag.error("failed to parse command line");
//    return static_cast<int>(DriverStatus::InvalidCommand);
//  }
//  const Args& RawArgs = *parseRes;
//
//  // 4) configure warnings early (before we emit other diags)
//  if (!WS.setup(RawArgs)) {
//    // setup should self-diagnose; treat as invalid command
//    return static_cast<int>(DriverStatus::InvalidCommand);
//  }
//
//  // 5) build typed driver config (defaults, validation, conflicts)
//  auto cfgOrErr = buildDriverConfig(RawArgs, FM, Diag); // returns Expected<DriverConfig>
//  if (!cfgOrErr) {
//    // errors already reported
//    return static_cast<int>(DriverStatus::InvalidCommand);
//  }
//  DriverConfig Cfg = std::move(*cfgOrErr);
//
//  // 6) create the driver and a compilation plan (jobs & temps)
//  Driver TheDriver(FM, Diag, WS);
//  std::unique_ptr<Compilation> C = TheDriver.createCompilation(Cfg);
//  if (!C) {
//    // driver should have reported why
//    return static_cast<int>(DriverStatus::InvalidCommand);
//  }
//
//  // 7) execute the plan
//  DriverStatus status = TheDriver.run(*C);
//
//  // 8) clean-up temps if policy says so
//  TheDriver.cleanup(*C, status);
//
//  return static_cast<int>(status);
}
