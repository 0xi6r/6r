#include "platform.h"

#ifdef PLATFORM_UNIX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

static struct termios original_termios;
static int terminal_initialized = 0;

void platform_init_terminal() {
    if (!terminal_initialized) {
        tcgetattr(STDIN_FILENO, &original_termios);
        terminal_initialized = 1;
    }
}

void platform_cleanup_terminal() {
    if (terminal_initialized) {
        tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
        terminal_initialized = 0;
    }
}

void platform_clear_screen() {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void platform_set_cursor_position(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);
    fflush(stdout);
}

void platform_get_console_size(int* rows, int* cols) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *rows = w.ws_row;
    *cols = w.ws_col;
}

void platform_set_color(int foreground, int background) {
    int fg = foreground & 7;
    int bg = (background & 7) << 4;
    printf("\033[%d;%dm", fg + 30, bg + 40);
    fflush(stdout);
}

void platform_reset_color() {
    printf("\033[0m");
    fflush(stdout);
}

void platform_hide_cursor() {
    printf("\033[?25l");
    fflush(stdout);
}

void platform_show_cursor() {
    printf("\033[?25h");
    fflush(stdout);
}

int platform_get_key(KeyEvent* event) {
    if (!event) return 0;
    
    // Set raw mode for character input
    struct termios raw = original_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1; // 100ms timeout
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    
    char ch;
    int result = read(STDIN_FILENO, &ch, 1);
    
    // Restore terminal mode
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
    
    if (result <= 0) {
        return 0;
    }
    
    // Initialize event
    event->key = ch;
    event->ctrl = 0;
    event->alt = 0;
    event->shift = 0;
    
    // Handle escape sequences for arrow keys
    if (ch == 27) {
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) == 1 && seq[0] == '[') {
            if (read(STDIN_FILENO, &seq[1], 1) == 1) {
                switch (seq[1]) {
                    case 'A': event->key = KEY_UP; break;
                    case 'B': event->key = KEY_DOWN; break;
                    case 'C': event->key = KEY_RIGHT; break;
                    case 'D': event->key = KEY_LEFT; break;
                    case 'H': event->key = KEY_HOME; break;
                    case 'F': event->key = KEY_END; break;
                    default:
                        if (seq[1] >= '1' && seq[1] <= '8') {
                            if (read(STDIN_FILENO, &seq[2], 1) == 1) {
                                switch (seq[1]) {
                                    case '3': if (seq[2] == '~') event->key = KEY_DELETE; break;
                                    case '5': if (seq[2] == '~') event->key = KEY_PAGE_UP; break;
                                    case '6': if (seq[2] == '~') event->key = KEY_PAGE_DOWN; break;
                                    case '7': if (seq[2] == '~') event->key = KEY_HOME; break;
                                    case '8': if (seq[2] == '~') event->key = KEY_END; break;
                                }
                            }
                        }
                        break;
                }
            }
        }
    }
    
    return 1;
}

void platform_set_raw_mode(int enable) {
    if (enable) {
        struct termios raw = original_termios;
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        raw.c_cflag |= CS8;
        raw.c_oflag &= ~OPOST;
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
    }
}

#endif