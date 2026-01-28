#ifndef FILEIO_H
#define FILEIO_H

#include "buffer.h"

int file_save(TextBuffer* buffer, const char* filename);
int file_save_as(TextBuffer* buffer);
int file_open(TextBuffer* buffer, const char* filename);
int file_new(TextBuffer* buffer);
void file_show_message(const char* message);

#endif