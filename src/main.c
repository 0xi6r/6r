#include "editor.h"
#include "display.h"
#include "input.h"
#include <stdio.h>
#include <conio.h>
#include <windows.h>

int main(void) {
    // Initialize display
    display_init();
    
    // Initialize editor
    Editor *editor = editor_create();
    if (!editor) {
        printf("Error: Failed to create editor!\n");
        display_cleanup();
        return 1;
    }
    
    display_clear_screen();
    printf("Simple TUI Editor v1.0\n");
    printf("Press Ctrl+H for help\n");
    printf("Press any key to start...\n");
    fflush(stdout);
    _getch();
    
    // Initial render
    display_render_editor(editor);
    
    int running = 1;
    int needs_redraw = 1;
    
    // Main loop with optimized rendering
    while (running) {
        // Only redraw when needed
        if (needs_redraw) {
            display_render_editor(editor);
            needs_redraw = 0;
        }
        
        // Check for input (non-blocking)
        if (_kbhit()) {
            input_handle_key(editor, &running);
            needs_redraw = 1;  // Redraw after input
        } else {
            // Small sleep to reduce CPU usage
            Sleep(50);  // 50ms delay
        }
    }
    
    // Cleanup
    if (editor_is_modified(editor)) {
        display_show_save_dialog();
        if (_getch() == 'y') {
            file_save(editor);
        }
    }
    
    editor_destroy(editor);
    display_cleanup();
    display_clear_screen();
    printf("Goodbye!\n");
    
    return 0;
}