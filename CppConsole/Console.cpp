#include <windows.h>

#include "Console.hpp"

#include "Renderer.hpp"

void Console::clear() noexcept {
    auto* logger = Logger::the();
    HANDLE hStdOut;

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // Fetch existing console mode so we correctly add a flag and not turn off others
    DWORD mode = 0;
    if (!GetConsoleMode(hStdOut, &mode))
    {
        auto last_error = ::GetLastError();
        logger->write(LogKind::Error, "In Console::clear(), the function failed at GetConsoleMode(...).");
        logger->write(LogKind::Error, "  GetLastError() returned {}", last_error);
        return;
    }

    // Hold original mode to restore on exit to be cooperative with other command-line apps.
    const DWORD originalMode = mode;
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    // Try to set the mode.
    if (!SetConsoleMode(hStdOut, mode))
    {
        auto last_error = ::GetLastError();
        logger->write(LogKind::Error, "In Console::clear(), the function failed at SetConsoleMode(...).");
        logger->write(LogKind::Error, "  GetLastError() returned {}", last_error);
        return;
    }

    // Write the sequence for clearing the display.
    DWORD written = 0;
    PCWSTR sequence = L"\x1b[2J\x1b[3J";
    if (!WriteConsoleW(hStdOut, sequence, (DWORD)wcslen(sequence), &written, NULL))
    {
        // If we fail, try to restore the mode on the way out.
        SetConsoleMode(hStdOut, originalMode);
        auto last_error = ::GetLastError();
        logger->write(LogKind::Error, "In Console::clear(), the function failed at WriteConsoleW(...).");
        logger->write(LogKind::Error, "  GetLastError() returned {}", last_error);
        return;
    }

    // To also clear the scroll back, emit L"\x1b[3J" as well.
    // 2J only clears the visible window and 3J only clears the scroll back.

    // Restore the mode on the way out to be nice to other command-line applications.
    SetConsoleMode(hStdOut, originalMode);
}

int32_t Console::run(int argc, char** argv) noexcept {
    m_renderer->begin_render_loop(*this, *m_renderer);
    return 0;
}