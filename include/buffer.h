#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>
#include <string.h>

#define MAX_LINES 10000
#define MAX_LINE_LENGTH 1024

typedef struct {
    char** lines;
    int line_count;
    int max_lines;
    char filename[256];
    int modified;
} TextBuffer;

TextBuffer* buffer_create();
void buffer_destroy(TextBuffer* buffer);
int buffer_insert_line(TextBuffer* buffer, int index, const char* text);
int buffer_delete_line(TextBuffer* buffer, int index);
int buffer_split_line(TextBuffer* buffer, int line_num, int position);
int buffer_merge_line(TextBuffer* buffer, int line_num);
void buffer_clear(TextBuffer* buffer);
char* buffer_get_line(TextBuffer* buffer, int index);
int buffer_set_line(TextBuffer* buffer, int index, const char* text);

#endif