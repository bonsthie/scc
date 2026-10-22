#include "scc/Error/Error.h"

#include <algorithm>
#include <charconv>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sys/ioctl.h>

using namespace scc;

namespace {

struct CodeLine {
    std::string Text;
    size_t      MarkerStart = 0;
    size_t      MarkerWidth = 1;
};

} // namespace

static size_t terminal_columns(std::ostream &O) {
    winsize   Size{};
    const int FD = O.rdbuf() == std::cout.rdbuf() ? STDOUT_FILENO : STDERR_FILENO;
    if (ioctl(FD, TIOCGWINSZ, &Size) == 0 && Size.ws_col > 0)
        return Size.ws_col;

    if (const char *Value = std::getenv("COLUMNS")) {
        const std::string_view Text(Value);
        size_t                 Columns = 0;
        auto [End, Error] = std::from_chars(Text.data(), Text.data() + Text.size(), Columns);
        if (Error == std::errc{} && End == Text.data() + Text.size() && Columns > 0)
            return Columns;
    }
    return 80;
}

static std::optional<std::string> read_source_line(const PosView &Pos) {
    if (!Pos.FID || Pos.P.Line == 0 || Pos.P.Column == 0)
        return std::nullopt;

    std::ifstream Source(Pos.FID->getName());
    if (!Source)
        return std::nullopt;

    std::string Line;
    for (unsigned I = 1; I < Pos.P.Line; ++I) {
        if (!std::getline(Source, Line) || Source.eof())
            return std::nullopt;
    }
    // An empty final line is a valid location for an EOF diagnostic.
    std::getline(Source, Line);
    if (!Line.empty() && Line.back() == '\r')
        Line.pop_back();
    if (Pos.P.Column - 1 > Line.size())
        return std::nullopt;
    return Line;
}

static CodeLine expand_source_line(std::string_view Line, const MemoryViewPos &Begin,
                                   const std::optional<MemoryViewPos> &End) {
    const size_t Start = Begin.Column - 1;
    size_t       Finish = Start;
    if (End) {
        if (End->Line > Begin.Line)
            Finish = Line.size();
        else if (End->Line == Begin.Line && End->Column > Begin.Column)
            Finish = std::min(Line.size(), size_t(End->Column - 1));
    }

    CodeLine Result;
    size_t   MarkerEnd = 0;
    // Expand tabs using eight-column tab stops before calculating display positions.
    for (size_t I = 0; I <= Line.size(); ++I) {
        if (I == Start)
            Result.MarkerStart = Result.Text.size();
        if (I == Finish)
            MarkerEnd = Result.Text.size();
        if (I == Line.size())
            break;
        if (Line[I] == '\t')
            Result.Text.append(8 - Result.Text.size() % 8, ' ');
        else
            Result.Text += Line[I];
    }
    Result.MarkerWidth = std::max(size_t(1), MarkerEnd - Result.MarkerStart);
    return Result;
}

static CodeLine crop_source_line(CodeLine Line, size_t Available) {
    // Reserve the caret's column even when it points just past the source at EOF.
    const size_t Total = std::max(Line.Text.size(), Line.MarkerStart + 1);
    if (Total <= Available)
        return Line;

    // On narrow terminals, give the caret priority over the ellipses.
    const size_t Ellipsis = Available >= 9 ? 3 : 0;
    const size_t Content = Available - 2 * Ellipsis;
    const size_t Context =
        std::min(size_t(16), (Content - std::min(Content, Line.MarkerWidth)) / 2);
    size_t Begin =
        std::min(Line.MarkerStart - std::min(Line.MarkerStart, Context), Total - Content);
    // Keep the start of the line when the complete token fits beside a trailing ellipsis.
    if (Line.MarkerStart < Available - Ellipsis &&
        Line.MarkerWidth <= Available - Ellipsis - Line.MarkerStart)
        Begin = 0;
    size_t End = Begin + Content;
    // Reclaim space for an ellipsis when that end of the line is visible.
    if (Begin == 0)
        End = std::min(Total, End + Ellipsis);
    if (End == Total)
        Begin -= std::min(Begin, Ellipsis);

    const std::string Left = Begin > 0 && Ellipsis ? "..." : "";
    const std::string Right = End < Total && Ellipsis ? "..." : "";
    return {Left + Line.Text.substr(Begin, End - Begin) + Right,
            Left.size() + Line.MarkerStart - Begin,
            std::min(Line.MarkerWidth, End - Line.MarkerStart)};
}

static void print_source_line(std::ostream &O, const CodeLine &Line) {
    const size_t HighlightEnd = std::min(Line.Text.size(), Line.MarkerStart + Line.MarkerWidth);
    O << Line.Text.substr(0, Line.MarkerStart);
    if (HighlightEnd > Line.MarkerStart)
        O << Color::lightBlue()
          << Line.Text.substr(Line.MarkerStart, HighlightEnd - Line.MarkerStart) << Color::reset();
    O << Line.Text.substr(HighlightEnd) << '\n';
}

void Error::printCode(std::ostream &O) const {
    if (!Pos)
        return;
    const auto Source = read_source_line(*Pos);
    if (!Source)
        return;

    const size_t      Columns = terminal_columns(O);
    const std::string LineNumber = std::to_string(Pos->P.Line);
    std::string       SourcePrefix = LineNumber + " | ";
    std::string       MarkerPrefix = std::string(LineNumber.size(), ' ') + " | ";
    // On tiny terminals, give the caret priority over the line-number gutter.
    if (Columns <= SourcePrefix.size()) {
        SourcePrefix.clear();
        MarkerPrefix.clear();
    }

    const CodeLine Excerpt =
        crop_source_line(expand_source_line(*Source, Pos->P, End), Columns - SourcePrefix.size());
    O << SourcePrefix;
    print_source_line(O, Excerpt);
    O << MarkerPrefix << std::string(Excerpt.MarkerStart, ' ') << Color::green() << '^'
      << std::string(Excerpt.MarkerWidth - 1, '~') << Color::reset() << '\n';
}
