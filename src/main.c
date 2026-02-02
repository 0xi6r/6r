#include "editor.h"
#include "platform.h"
#include <stdio.h>
#include <unistd.h>

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
#ifdef PLATFORM_WINDOWS
            Sleep(1000);
#else
            sleep(1);
#endif
        }
    }
    
    editor_run(&editor);
    editor_cleanup(&editor);
    
    return 0;
}