#ifndef INPUT_H
#define INPUT_H

#include "editor.h"

// Input handling
void input_handle_key(Editor *editor, int *running);
int input_get_filename(char *buffer, int size);

#endif // INPUT_H