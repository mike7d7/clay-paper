#include "widget_functions.h"
#include "clay.h"
#include <raylib.h>
#include <stdint.h>
#include <string.h>

bool should_exit = false;
char empty_buffer[11];
TextEditData default_data = (TextEditData){.hintText = CLAY_STRING("Search"),
                                           .textToEdit = empty_buffer,
                                           .isPassword = false,
                                           .maxLength = sizeof(empty_buffer),
                                           .disable = false};

void HandleTextEditInteraction(Clay_ElementId elementId,
                               Clay_PointerData pointerInfo,
                               intptr_t userData) {
  TextEditData *data = (TextEditData *)userData;
  if (data->disable) {
    return;
  }

  uint32_t key = GetCharPressed();
  if (key != 0) {
    uint32_t len = strlen(data->textToEdit);
    if (len < data->maxLength - 1) {
      data->textToEdit[len] = (char)key;
      data->textToEdit[len + 1] = '\0';
    }
  }
}

void HandleExitButton(Clay_ElementId id, Clay_PointerData pointer_data) {
  if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    should_exit = true;
  }
}

void HandleClearButton(Clay_ElementId id, Clay_PointerData pointer_data) {

  if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    for (int i = 0; i < sizeof(default_data.textToEdit); i++) {
      default_data.textToEdit[i] = 0;
    }
  }
}
