#include "widget_functions.h"
#include "clay.h"
#include <SDL3/SDL.h>
#include <stdint.h>

char empty_buffer[1024];
bool text_input_initialized = false;
Uint32 myEventType;

TextEditData default_data = (TextEditData){
    .hintText = CLAY_STRING("Search"),
    .textToEdit = empty_buffer,
    .isPassword = false,
    .maxLength = sizeof(empty_buffer),
};

void HandleTextEditInteraction(Clay_ElementId elementId,
                               Clay_PointerData pointer_data,
                               intptr_t userData) {
  if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    if (!text_input_initialized) {
      myEventType = SDL_RegisterEvents(1);
      text_input_initialized = !text_input_initialized;
    }
    Clay_ElementData element_data = Clay_GetElementData(elementId);
    SDL_Rect *element_area = malloc(sizeof(SDL_Rect));
    *element_area = (SDL_Rect){
        .x = element_data.boundingBox.x,
        .y = element_data.boundingBox.y,
        .w = element_data.boundingBox.width,
        .h = element_data.boundingBox.height,
    };
    if (myEventType != 0) {
      SDL_Event start_text_edit;
      SDL_zero(start_text_edit);
      start_text_edit.type = myEventType;
      start_text_edit.user.code = 1;
      start_text_edit.user.data1 = element_area;
      SDL_PushEvent(&start_text_edit);
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
