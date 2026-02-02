#include "fileio.h"
#include "platform.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int file_save(TextBuffer* buffer, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        return 0;
    }
    
    for (int i = 0; i < buffer->line_count; i++) {
        char* line = buffer_get_line(buffer, i);
        if (line) {
            fprintf(file, "%s\n", line);
        }
    }
    
    fclose(file);
    strncpy(buffer->filename, filename, sizeof(buffer->filename) - 1);
    buffer->modified = 0;
    
    return 1;
}

int file_save_as(TextBuffer* buffer) {
    char filename[256];
    printf("\nEnter filename to save: ");
    fflush(stdout);
    
    // Simple input - in real implementation, would need custom input handling
    system("cls");
    printf("Save As - Enter filename: ");
    if (scanf("%255s", filename) == 1) {
        if (file_save(buffer, filename)) {
            printf("File saved successfully.\nPress any key to continue...");
            getchar(); getchar();
            return 1;
        }
    }
    
    printf("Error saving file.\nPress any key to continue...");
    getchar(); getchar();
    return 0;
}

int file_open(TextBuffer* buffer, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return 0;
    }
    
    buffer_clear(buffer);
    
    char line[MAX_LINE_LENGTH];
    int line_num = 0;
    
    while (fgets(line, sizeof(line), file) && line_num < MAX_LINES) {
        // Remove newline
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        if (len > 1 && line[len - 2] == '\r') {
            line[len - 2] = '\0';
        }
        
        if (line_num == 0) {
            buffer_set_line(buffer, 0, line);
        } else {
            buffer_insert_line(buffer, line_num, line);
        }
        line_num++;
    }
    
    fclose(file);
    strncpy(buffer->filename, filename, sizeof(buffer->filename) - 1);
    buffer->modified = 0;
    
    return 1;
}

int file_new(TextBuffer* buffer) {
    if (buffer->modified) {
        // Ask to save current file first
        system("cls");
        printf("Save current file? (y/n): ");
        char ch = getchar();
        if (ch == 'y' || ch == 'Y') {
            if (buffer->filename[0]) {
                file_save(buffer, buffer->filename);
            } else {
                file_save_as(buffer);
            }
        }
        while (getchar() != '\n'); // Clear input buffer
    }
    
    buffer_clear(buffer);
    return 1;
}

void file_show_message(const char* message) {
    // Simple message display
    platform_clear_screen();
    printf("%s\nPress any key to continue...", message);
    getchar();
}