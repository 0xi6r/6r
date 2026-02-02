#ifndef INPUT_H
#define INPUT_H

#include "platform.h"
#include "buffer.h"
#include "tui.h"

int input_get_key(KeyEvent* event);
void input_handle_key(TUIState* tui, TextBuffer* buffer, KeyEvent event);
void input_insert_char(TUIState* tui, TextBuffer* buffer, char ch);
void input_delete_char(TUIState* tui, TextBuffer* buffer);
void input_move_cursor(TUIState* tui, TextBuffer* buffer, int dx, int dy);
void input_scroll_to_cursor(TUIState* tui);

#endif