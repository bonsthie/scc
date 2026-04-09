#ifndef SCC_COLORS_COLORS_H
#define SCC_COLORS_COLORS_H

namespace scc {

class Color {
    bool Enabled = false;

    Color() = default;

    static const char *code(const char *Value) { return enabled() ? Value : ""; }

  public:
    static Color &instance() {
        static Color Instance;
        return Instance;
    }

    static void enable() { instance().Enabled = true; }
    static void disable() { instance().Enabled = false; }
    static void setEnabled(bool Enable) { instance().Enabled = Enable; }
    static bool enabled() { return instance().Enabled; }

    static const char *reset() { return code("\x1b[0m"); }
    static const char *bold() { return code("\x1b[1m"); }
    static const char *dim() { return code("\x1b[2m"); }
    static const char *green() { return code("\x1b[32m"); }
    static const char *red() { return code("\x1b[31m"); }
    static const char *yellow() { return code("\x1b[33m"); }
    static const char *blue() { return code("\x1b[34m"); }
    static const char *magenta() { return code("\x1b[35m"); }
    static const char *cyan() { return code("\x1b[36m"); }
};

} // namespace scc

#endif // SCC_COLORS_COLORS_H
