#include "display.h"
#include <stdio.h>
#include <string.h>

static int last_cursor_line = -1;
static int last_cursor_col = -1;

void display_init(void) {
    platform_init_terminal();
    // Hide cursor during rendering
    display_hide_cursor();
}

void display_cleanup(void) {
    display_show_cursor();
    platform_cleanup_terminal();
}

void display_clear_screen(void) {
    platform_clear_screen();
}

void display_set_cursor_position(int x, int y) {
    platform_set_cursor_position(x, y);
}

ScreenSize display_get_screen_size(void) {
    ScreenSize size;
    platform_get_console_size(&size.height, &size.width);
    return size;
}

void display_show_cursor(void) {
    platform_show_cursor();
}

void display_hide_cursor(void) {
    platform_hide_cursor();
}

void display_flush(void) {
    fflush(stdout);
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
    
    // Set background color for status bar
    platform_set_color(COLOR_BLACK, COLOR_WHITE);
    printf("%s", status);
    
    // Pad to screen width
    int padding = screen.width - strlen(status);
    if (padding < 0) padding = 0;
    for (int i = 0; i < padding; i++) printf(" ");
    
    platform_reset_color();
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
    getchar();
}

void display_show_message(const char *message, int wait_for_key) {
    printf("%s", message);
    fflush(stdout);
    if (wait_for_key) {
        getchar();
    }
}

void display_show_save_dialog(void) {
    display_clear_screen();
    printf("File has unsaved changes. Save before exit? (y/n): ");
    fflush(stdout);
}