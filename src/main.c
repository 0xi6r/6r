#include "editor.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    printf("6r - Simple TUI Editor\n");
    printf("Loading...\n");
    
    Editor editor;
    editor_init(&editor);
    
    // If filename provided as argument
    if (argc > 1) {
        if (!file_open(editor.buffer, argv[1])) {
            printf("Could not open file: %s\n", argv[1]);
            printf("Creating new file instead.\n");
            Sleep(1000);
        }
    }
    
    editor_run(&editor);
    editor_cleanup(&editor);
    
    return 0;
}