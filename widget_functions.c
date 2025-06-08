#include "widget_functions.h"
#include "SDL3/SDL_stdinc.h"
#include "clay.h"
#include <SDL3/SDL.h>
#include <stdint.h>

char empty_buffer[1024];
bool text_input_initialized = false;
Uint32 registered_event_type;
SDL_Event start_text_edit;
SDL_Event end_text_edit;
bool editing_text = false;
uint32_t config_options = 1;
int number_of_images = 0;

TextEditData default_data = (TextEditData){
    .hintText = CLAY_STRING("Search"),
    .textToEdit = empty_buffer,
    .isPassword = false,
    .maxLength = sizeof(empty_buffer),
};
void InitializeCustomEvents() {
  registered_event_type = SDL_RegisterEvents(2);

  SDL_zero(start_text_edit);
  start_text_edit.type = registered_event_type;
  start_text_edit.user.code = 1;

  SDL_zero(end_text_edit);
  end_text_edit.type = registered_event_type + 1;
  end_text_edit.user.code = 2;
}

void HandleTextEditInteraction(Clay_ElementId id, Clay_PointerData pointer_data,
                               intptr_t userData) {
  if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    Clay_ElementData element_data = Clay_GetElementData(id);
    SDL_Rect *element_area = SDL_malloc(sizeof(SDL_Rect));
    *element_area = (SDL_Rect){
        .x = element_data.boundingBox.x,
        .y = element_data.boundingBox.y,
        .w = element_data.boundingBox.width,
        .h = element_data.boundingBox.height,
    };
    if (registered_event_type != 0) {
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
    if (registered_event_type != 0) {
      SDL_PushEvent(&end_text_edit);
    }
  }
}

void HandleOptionsButton(Clay_ElementId id, Clay_PointerData pointer_data,
                         int config) {
  if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    config_options ^= 1 << config;
  }
}
