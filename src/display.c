#include "display.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>

static HANDLE hConsoleOutput = NULL;
static CONSOLE_SCREEN_BUFFER_INFO csbi;
static int last_cursor_line = -1;
static int last_cursor_col = -1;

void display_init(void) {
    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    
    // Set console mode for better performance
    DWORD dwMode = 0;
    GetConsoleMode(hConsoleOutput, &dwMode);
    dwMode |= ENABLE_PROCESSED_OUTPUT;
    SetConsoleMode(hConsoleOutput, dwMode);
    
    // Hide cursor during rendering
    display_hide_cursor();
}

void display_cleanup(void) {
    display_show_cursor();
}

void display_clear_screen(void) {
    COORD coordScreen = { 0, 0 };
    DWORD cCharsWritten = 0;
    DWORD dwConSize;
    
    GetConsoleScreenBufferInfo(hConsoleOutput, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    
    FillConsoleOutputCharacter(hConsoleOutput, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten);
    FillConsoleOutputAttribute(hConsoleOutput, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
    
    SetConsoleCursorPosition(hConsoleOutput, coordScreen);
}

void display_set_cursor_position(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(hConsoleOutput, coord);
}

ScreenSize display_get_screen_size(void) {
    GetConsoleScreenBufferInfo(hConsoleOutput, &csbi);
    
    ScreenSize size;
    size.width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    size.height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return size;
}

void display_show_cursor(void) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsoleOutput, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsoleOutput, &cursorInfo);
}

void display_hide_cursor(void) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsoleOutput, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsoleOutput, &cursorInfo);
}

void display_flush(void) {
    FlushConsoleInputBuffer(hConsoleOutput);
}

void display_show_status_bar(const Editor *editor) {
    ScreenSize screen = display_get_screen_size();
    
    display_set_cursor_position(0, screen.height - 1);
    
    // Create status bar string
    char status[512];
    snprintf(status, sizeof(status), " %s | Line: %d | Col: %d | %s ",
             editor_get_filename(editor),
             editor_get_cursor_line(editor) + 1,
             editor_get_cursor_col(editor) + 1,
             editor_is_modified(editor) ? "[MODIFIED]" : "");
    
    // Pad to screen width
    int padding = screen.width - strlen(status);
    if (padding < 0) padding = 0;
    
    // Set background color for status bar
    printf("\033[7m%s", status);
    for (int i = 0; i < padding; i++) printf(" ");
    printf("\033[0m");
}

void display_render_editor(const Editor *editor) {
    ScreenSize screen = display_get_screen_size();
    int display_rows = screen.height - 1;
    int line_count = editor_get_line_count(editor);
    
    // Clear only once at the beginning
    display_clear_screen();
    display_set_cursor_position(0, 0);
    
    // Render all visible lines
    for (int i = 0; i < display_rows && i < line_count; i++) {
        printf("%4d | %s\n", i + 1, editor_get_line(editor, i));
    }
    
    // Fill remaining lines
    for (int i = line_count; i < display_rows; i++) {
        printf("     |\n");
    }
    
    // Render status bar
    display_show_status_bar(editor);
    
    // Position cursor at the end
    int cursor_x = editor_get_cursor_col(editor) + 7;
    int cursor_y = editor_get_cursor_line(editor);
    display_set_cursor_position(cursor_x, cursor_y);
    display_show_cursor();
}

void display_show_help(void) {
    display_clear_screen();
    printf("=== SIMPLE TUI EDITOR - HELP ===\n\n");
    printf("CONTROLS:\n");
    printf("  Arrow Keys    - Move cursor\n");
    printf("  Home/End      - Go to line start/end\n");
    printf("  Page Up/Down  - Scroll up/down\n");
    printf("  Delete        - Delete character\n");
    printf("  Backspace     - Delete previous character\n");
    printf("  Enter         - New line\n");
    printf("  Ctrl+S        - Save file\n");
    printf("  Ctrl+O        - Open file\n");
    printf("  Ctrl+N        - New file\n");
    printf("  Ctrl+H        - Show this help\n");
    printf("  Ctrl+Q        - Quit\n\n");
    printf("Press any key to continue...");
    fflush(stdout);
    _getch();
}

void display_show_message(const char *message, int wait_for_key) {
    printf("%s", message);
    fflush(stdout);
    if (wait_for_key) {
        _getch();
    }
}

void display_show_save_dialog(void) {
    display_clear_screen();
    printf("File has unsaved changes. Save before exit? (y/n): ");
    fflush(stdout);
}