#include "platform.h"

#ifdef PLATFORM_UNIX
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <unistd.h>

static struct termios original_termios;
static int raw_mode_enabled = 0;
#endif

void platform_init_terminal() {
#ifdef PLATFORM_WINDOWS
    // Windows terminal initialization is handled by individual functions
#else
    // Save original terminal settings
    tcgetattr(STDIN_FILENO, &original_termios);
#endif
}

void platform_cleanup_terminal() {
#ifdef PLATFORM_UNIX
    if (raw_mode_enabled) {
        platform_set_raw_mode(0);
    }
#endif
}

void platform_clear_screen() {
#ifdef PLATFORM_WINDOWS
    system("cls");
#else
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

void platform_set_cursor_position(int x, int y) {
#ifdef PLATFORM_WINDOWS
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    printf("\033[%d;%dH", y + 1, x + 1);
    fflush(stdout);
#endif
}

void platform_get_console_size(int* rows, int* cols) {
#ifdef PLATFORM_WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    *cols = ws.ws_col;
    *rows = ws.ws_row;
#endif
}

void platform_set_color(int foreground, int background) {
#ifdef PLATFORM_WINDOWS
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                          foreground | (background << 4));
#else
    // ANSI color codes (simplified)
    int color_code = 30 + foreground;  // Default to normal intensity
    if (foreground >= 8) {
        color_code = 90 + (foreground - 8);  // Bright colors
    }
    
    printf("\033[%dm", color_code);
    fflush(stdout);
#endif
}

void platform_reset_color() {
#ifdef PLATFORM_WINDOWS
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                          7 | (0 << 4));  // Default white on black
#else
    printf("\033[0m");
    fflush(stdout);
#endif
}

void platform_hide_cursor() {
#ifdef PLATFORM_WINDOWS
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
#else
    printf("\033[?25l");
    fflush(stdout);
#endif
}

void platform_show_cursor() {
#ifdef PLATFORM_WINDOWS
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
#else
    printf("\033[?25h");
    fflush(stdout);
#endif
}

#ifdef PLATFORM_UNIX
static void set_raw_mode_internal(int enable) {
    struct termios raw = original_termios;
    
    if (enable) {
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        raw.c_cflag |= CS8;
        raw.c_oflag &= ~(OPOST);
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;
    }
    
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
#endif

void platform_set_raw_mode(int enable) {
#ifdef PLATFORM_UNIX
    if (enable && !raw_mode_enabled) {
        set_raw_mode_internal(1);
        raw_mode_enabled = 1;
    } else if (!enable && raw_mode_enabled) {
        set_raw_mode_internal(0);
        raw_mode_enabled = 0;
    }
#else
    // Windows handles raw mode through console mode settings
    if (enable) {
        HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode = 0;
        GetConsoleMode(stdin_handle, &mode);
        SetConsoleMode(stdin_handle, mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));
    } else {
        HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode = 0;
        GetConsoleMode(stdin_handle, &mode);
        SetConsoleMode(stdin_handle, mode | (ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));
    }
#endif
}

int platform_get_key(KeyEvent* event) {
    event->key = 0;
    event->ctrl = 0;
    event->alt = 0;
    event->shift = 0;
    
#ifdef PLATFORM_WINDOWS
    INPUT_RECORD input_record;
    DWORD events_read;
    
    while (1) {
        ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &input_record, 1, &events_read);
        
        if (input_record.EventType == KEY_EVENT && input_record.Event.KeyEvent.bKeyDown) {
            event->key = input_record.Event.KeyEvent.wVirtualKeyCode;
            event->ctrl = (input_record.Event.KeyEvent.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;
            event->alt = (input_record.Event.KeyEvent.dwControlKeyState & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) != 0;
            event->shift = (input_record.Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED) != 0;
            
            // For printable characters, also get the actual character
            if (event->key >= 0x30 && event->key <= 0x5A) {
                event->key = input_record.Event.KeyEvent.uChar.AsciiChar;
                if (event->key == 0) {
                    continue;  // Skip non-character keys
                }
            }
            
            // Map Windows virtual keys to our cross-platform keys
            switch (event->key) {
                case VK_UP: event->key = KEY_UP; break;
                case VK_DOWN: event->key = KEY_DOWN; break;
                case VK_LEFT: event->key = KEY_LEFT; break;
                case VK_RIGHT: event->key = KEY_RIGHT; break;
                case VK_HOME: event->key = KEY_HOME; break;
                case VK_END: event->key = KEY_END; break;
                case VK_PRIOR: event->key = KEY_PAGE_UP; break;
                case VK_NEXT: event->key = KEY_PAGE_DOWN; break;
                case VK_DELETE: event->key = KEY_DELETE; break;
                case VK_BACK: event->key = KEY_BACKSPACE; break;
            }
            
            return 1;
        }
    }
#else
    // Unix/Linux implementation
    char ch;
    if (read(STDIN_FILENO, &ch, 1) <= 0) {
        return 0;
    }
    
    // Check for escape sequences
    if (ch == '\033') {
        // Check if this is an escape sequence
        fd_set readset;
        struct timeval timeout;
        FD_ZERO(&readset);
        FD_SET(STDIN_FILENO, &readset);
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;  // 100ms timeout
        
        if (select(STDIN_FILENO + 1, &readset, NULL, NULL, &timeout) > 0) {
            char seq[3];
            if (read(STDIN_FILENO, seq, 2) == 2) {
                if (seq[0] == '[') {
                    switch (seq[1]) {
                        case 'A': event->key = KEY_UP; return 1;
                        case 'B': event->key = KEY_DOWN; return 1;
                        case 'C': event->key = KEY_RIGHT; return 1;
                        case 'D': event->key = KEY_LEFT; return 1;
                        case 'H': event->key = KEY_HOME; return 1;
                        case 'F': event->key = KEY_END; return 1;
                    }
                    
                    // Handle extended escape sequences
                    if (seq[1] >= '0' && seq[1] <= '9') {
                        if (read(STDIN_FILENO, &seq[2], 1) == 1 && seq[2] == '~') {
                            switch (seq[1]) {
                                case '3': event->key = KEY_DELETE; return 1;
                                case '5': event->key = KEY_PAGE_UP; return 1;
                                case '6': event->key = KEY_PAGE_DOWN; return 1;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Handle backspace and delete keys
    if (ch == 127 || ch == 8) {
        event->key = KEY_BACKSPACE;
        return 1;
    }
    
    // Regular character
    event->key = ch;
    return 1;
#endif
}