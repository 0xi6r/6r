#include "buffer.h"

TextBuffer* buffer_create() {
    TextBuffer* buffer = (TextBuffer*)malloc(sizeof(TextBuffer));
    if (!buffer) return NULL;
    
    buffer->max_lines = MAX_LINES;
    buffer->lines = (char**)calloc(buffer->max_lines, sizeof(char*));
    buffer->line_count = 0;
    buffer->filename[0] = '\0';
    buffer->modified = 0;
    
    // Initialize with one empty line
    buffer_insert_line(buffer, 0, "");
    
    return buffer;
}

void buffer_destroy(TextBuffer* buffer) {
    if (!buffer) return;
    
    for (int i = 0; i < buffer->line_count; i++) {
        free(buffer->lines[i]);
    }
    free(buffer->lines);
    free(buffer);
}

int buffer_insert_line(TextBuffer* buffer, int index, const char* text) {
    if (!buffer || index < 0 || index > buffer->line_count || 
        buffer->line_count >= buffer->max_lines) {
        return 0;
    }
    
    // Shift lines down
    for (int i = buffer->line_count; i > index; i--) {
        buffer->lines[i] = buffer->lines[i - 1];
    }
    
    buffer->lines[index] = (char*)malloc(MAX_LINE_LENGTH);
    if (!buffer->lines[index]) return 0;
    
    strncpy(buffer->lines[index], text, MAX_LINE_LENGTH - 1);
    buffer->lines[index][MAX_LINE_LENGTH - 1] = '\0';
    buffer->line_count++;
    buffer->modified = 1;
    
    return 1;
}

int buffer_delete_line(TextBuffer* buffer, int index) {
    if (!buffer || index < 0 || index >= buffer->line_count) {
        return 0;
    }
    
    free(buffer->lines[index]);
    
    // Shift lines up
    for (int i = index; i < buffer->line_count - 1; i++) {
        buffer->lines[i] = buffer->lines[i + 1];
    }
    
    buffer->line_count--;
    buffer->modified = 1;
    return 1;
}

int buffer_split_line(TextBuffer* buffer, int line_num, int position) {
    if (!buffer || line_num < 0 || line_num >= buffer->line_count) {
        return 0;
    }
    
    char* line = buffer->lines[line_num];
    int len = strlen(line);
    
    if (position > len) position = len;
    
    // Create new line with text after position
    char second_part[MAX_LINE_LENGTH];
    strcpy(second_part, line + position);
    
    // Truncate original line
    line[position] = '\0';
    
    // Insert new line
    return buffer_insert_line(buffer, line_num + 1, second_part);
}

int buffer_merge_line(TextBuffer* buffer, int line_num) {
    if (!buffer || line_num < 0 || line_num >= buffer->line_count - 1) {
        return 0;
    }
    
    char* line1 = buffer->lines[line_num];
    char* line2 = buffer->lines[line_num + 1];
    int len1 = strlen(line1);
    int len2 = strlen(line2);
    
    if (len1 + len2 >= MAX_LINE_LENGTH) {
        return 0;
    }
    
    strcat(line1, line2);
    buffer_delete_line(buffer, line_num + 1);
    
    return 1;
}

void buffer_clear(TextBuffer* buffer) {
    if (!buffer) return;
    
    for (int i = 0; i < buffer->line_count; i++) {
        free(buffer->lines[i]);
    }
    buffer->line_count = 0;
    buffer_insert_line(buffer, 0, "");
    buffer->filename[0] = '\0';
    buffer->modified = 0;
}

char* buffer_get_line(TextBuffer* buffer, int index) {
    if (!buffer || index < 0 || index >= buffer->line_count) {
        return NULL;
    }
    return buffer->lines[index];
}

int buffer_set_line(TextBuffer* buffer, int index, const char* text) {
    if (!buffer || index < 0 || index >= buffer->line_count) {
        return 0;
    }
    
    strncpy(buffer->lines[index], text, MAX_LINE_LENGTH - 1);
    buffer->lines[index][MAX_LINE_LENGTH - 1] = '\0';
    buffer->modified = 1;
    
    return 1;
}