#include "file_ops.h"
#include "display.h"
#include <stdio.h>
#include <string.h>

int file_open(Editor *editor, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        display_show_message("Error: Cannot open file!\n", 1);
        return 0;
    }
    
    editor_new(editor);
    strcpy(editor->filename, filename);
    
    int line_num = 0;
    while (fgets(editor->lines[line_num], MAX_LINE_LENGTH, file) && 
           line_num < MAX_LINES) {
        editor->lines[line_num][strcspn(editor->lines[line_num], "\n")] = '\0';
        line_num++;
    }
    
    editor->line_count = (line_num > 0) ? line_num : 1;
    editor->modified = 0;
    fclose(file);
    
    return 1;
}

int file_save(Editor *editor) {
    FILE *file = fopen(editor->filename, "w");
    if (!file) {
        display_show_message("Error: Cannot save file!\n", 1);
        return 0;
    }
    
    for (int i = 0; i < editor->line_count; i++) {
        fprintf(file, "%s\n", editor->lines[i]);
    }
    
    fclose(file);
    editor->modified = 0;
    
    char message[256];
    snprintf(message, sizeof(message), "File saved: %s\n", editor->filename);
    display_show_message(message, 1);
    
    return 1;
}

int file_save_as(Editor *editor, const char *filename) {
    strcpy(editor->filename, filename);
    return file_save(editor);
}