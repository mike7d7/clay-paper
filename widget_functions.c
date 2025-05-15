#include "widget_functions.h"
#include "SDL3/SDL_events.h"
#include "clay.h"
#include <SDL3/SDL.h>
#include <stdint.h>
#include <string.h>

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

  uint32_t key = 32;
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
    SDL_Event quit_event;
    quit_event.type = SDL_EVENT_QUIT;
    SDL_PushEvent(&quit_event);
  }
}

void HandleClearButton(Clay_ElementId id, Clay_PointerData pointer_data) {

  if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    for (int i = 0; i < sizeof(default_data.textToEdit); i++) {
      default_data.textToEdit[i] = 0;
    }
  }
}
