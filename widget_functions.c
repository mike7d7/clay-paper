#include "widget_functions.h"
#include "clay.h"
#include <raylib.h>
#include <stdint.h>
#include <string.h>

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

