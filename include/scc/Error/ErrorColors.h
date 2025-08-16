#ifndef SCC_ERROR_ERRORCOLORS_H
#define SCC_ERROR_ERRORCOLORS_H

#include <cstdlib>

#include <unistd.h>
#define ISATTY isatty
#define FD_STDERR STDERR_FILENO

// --- Raw ANSI codes
#define ESC "\x1b["
#define COL_RESET ESC "0m"
#define COL_BOLD ESC "1m"
#define COL_DIM ESC "2m"
#define COL_RED ESC "31m"
#define COL_YELLOW ESC "33m"
#define COL_BLUE ESC "34m"
#define COL_MAGENTA ESC "35m"
#define COL_CYAN ESC "36m"

// --- Gate coloring behind a runtime flag
inline bool diag_use_color() {
    // Respect NO_COLOR and only color when stderr is a TTY
    const char *no = std::getenv("NO_COLOR");
    return !no && ISATTY(FD_STDERR);
}

// --- Convenient wrappers that auto-disable color
#define _COL_IF_ENABLED(code) code
#define BOLD(s) (_COL_IF_ENABLED(COL_BOLD) << (s) << _COL_IF_ENABLED(COL_RESET))
#define RED(s) (_COL_IF_ENABLED(COL_RED) << (s) << _COL_IF_ENABLED(COL_RESET))
#define YELLOW(s) (_COL_IF_ENABLED(COL_YELLOW) << (s) << _COL_IF_ENABLED(COL_RESET))

// --- High-level macros to print diagnostics
#define DIAG_WARNING(msg)                                                                          \
    (std::cerr << _COL_IF_ENABLED(COL_BOLD) << _COL_IF_ENABLED(COL_YELLOW)                         \
               << "warning: " << _COL_IF_ENABLED(COL_RESET) << msg << '\n')

#define DIAG_ERROR(msg)                                                                            \
    (std::cerr << _COL_IF_ENABLED(COL_BOLD) << _COL_IF_ENABLED(COL_RED)                            \
               << "error: " << _COL_IF_ENABLED(COL_RESET) << msg << '\n')

#endif //  SCC_ERROR_ERRORCOLORS_H
