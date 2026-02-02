#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
    #define PLATFORM_WINDOWS
    #include <windows.h>
    #include <conio.h>
#else
    #define PLATFORM_UNIX
    #include <unistd.h>
    #include <termios.h>
    #include <sys/ioctl.h>
    #include <sys/select.h>
    #include <time.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declaration for KeyEvent
typedef struct {
    int key;
    int ctrl;
    int alt;
    int shift;
} KeyEvent;

// Platform-specific types and constants
#ifdef PLATFORM_WINDOWS
    typedef void* PlatformHandle;
    typedef struct {
        struct { short X; short Y; } dwSize;
        struct { short X; short Y; } dwCursorPosition;
        unsigned short wAttributes;
        struct { short Left; short Top; short Right; short Bottom; } srWindow;
        struct { short X; short Y; } dwMaximumWindowSize;
    } ConsoleInfo;
    typedef unsigned long PlatformMode;
    typedef unsigned short PlatformColor;
    
    // Windows constants
    #define STD_INPUT_HANDLE ((unsigned long)-10)
    #define STD_OUTPUT_HANDLE ((unsigned long)-11)
    #define ENABLE_ECHO_INPUT 0x0004
    #define ENABLE_LINE_INPUT 0x0002
    #define ENABLE_PROCESSED_OUTPUT 0x0001
    #define LEFT_CTRL_PRESSED 0x0008
    #define RIGHT_CTRL_PRESSED 0x0004
    #define LEFT_ALT_PRESSED 0x0002
    #define RIGHT_ALT_PRESSED 0x0001
    #define SHIFT_PRESSED 0x0010
    #define TRUE 1
    #define FALSE 0
    
    // Virtual key codes
    #define VK_UP 0x26
    #define VK_DOWN 0x28
    #define VK_LEFT 0x25
    #define VK_RIGHT 0x27
    #define VK_HOME 0x24
    #define VK_END 0x23
    #define VK_PRIOR 0x21
    #define VK_NEXT 0x22
    #define VK_DELETE 0x2E
    #define VK_BACK 0x08
    
    // Event types
    #define KEY_EVENT 0x0001
    
    // Input record structures
    typedef struct {
        unsigned short EventType;
        union {
            struct {
                int bKeyDown;
                unsigned short wRepeatCount;
                unsigned short wVirtualKeyCode;
                unsigned short wVirtualScanCode;
                union {
                    unsigned char AsciiChar;
                    unsigned short UnicodeChar;
                } uChar;
                unsigned long dwControlKeyState;
            } KeyEvent;
        } Event;
    } INPUT_RECORD;
    
    typedef struct {
        unsigned long dwSize;
        struct { short X; short Y; } dwCursorPosition;
        unsigned short wAttributes;
        struct { short Left; short Top; short Right; short Bottom; } srWindow;
        struct { short X; short Y; } dwMaximumWindowSize;
    } CONSOLE_SCREEN_BUFFER_INFO;
    
    typedef struct {
        unsigned long dwSize;
        int bVisible;
        unsigned short dwFillAttribute;
    } CONSOLE_CURSOR_INFO;
    
    typedef struct { short X; short Y; } COORD;
    typedef struct { short Left; short Top; short Right; short Bottom; } SMALL_RECT;
    typedef unsigned short WORD;
    typedef unsigned long DWORD;
    typedef char TCHAR;
    
#else
    typedef int PlatformHandle;
#endif

// Key codes - cross-platform mapping
#define KEY_UP 0x101
#define KEY_DOWN 0x102
#define KEY_LEFT 0x103
#define KEY_RIGHT 0x104
#define KEY_HOME 0x105
#define KEY_END 0x106
#define KEY_PAGE_UP 0x107
#define KEY_PAGE_DOWN 0x108
#define KEY_DELETE 0x109
#define KEY_ESC 27
#define KEY_ENTER 13
#define KEY_BACKSPACE 8
#define KEY_TAB 9
#define KEY_CTRL_S 19
#define KEY_CTRL_O 15
#define KEY_CTRL_Q 17
#define KEY_CTRL_N 14
#define KEY_F1 0x70

// Color definitions
#define COLOR_BLACK 0
#define COLOR_WHITE 7
#define COLOR_BLUE 4
#define COLOR_GREEN 2
#define COLOR_RED 1
#define COLOR_YELLOW 3

// Platform function declarations
void platform_init_terminal();
void platform_cleanup_terminal();
void platform_clear_screen();
void platform_set_cursor_position(int x, int y);
void platform_get_console_size(int* rows, int* cols);
void platform_set_color(int foreground, int background);
void platform_reset_color();
void platform_hide_cursor();
void platform_show_cursor();
int platform_get_key(KeyEvent* event);
void platform_set_raw_mode(int enable);

#endif