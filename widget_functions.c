#include "widget_functions.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_stdinc.h"
#include "clay.h"
#include <SDL3/SDL.h>
#include <stdint.h>
#include <unistd.h>

char empty_buffer[1024];
bool text_input_initialized = false;
Uint32 registered_event_type;
SDL_Event start_text_edit;
SDL_Event end_text_edit;
SDL_Event open_folder_dialog;
SDL_Event refresh_button;
bool editing_text = false;
Uint32 config_options = 0;
Uint32 number_of_images = 0;
char **files = NULL;
Uint32 selected_image = 0;
char *folder_path;
char *cache_path;
Uint32 non_hidden_imgs = 0;
Uint32 *rendered_to_list;

TextEditData default_data = (TextEditData){
    .hintText = CLAY_STRING("Search"),
    .textToEdit = empty_buffer,
    .isPassword = false,
    .maxLength = sizeof(empty_buffer),
};

// Copied from jftui with minor modifications made.
// https://github.com/Aanok/jftui/blob/master/src/shared.c
char *jf_concat(size_t n, ...) {
  char *buf;
  char *tmp;
  size_t len = 0;
  size_t i;
  va_list ap;

  va_start(ap, n);
  for (i = 0; i < n; i++) {
    len += SDL_strlen(va_arg(ap, const char *));
  }
  va_end(ap);

  buf = SDL_malloc(len + 1);
  tmp = buf;
  va_start(ap, n);
  for (i = 0; i < n; i++) {
    const char *string_from_arg = va_arg(ap, const char *);
    size_t arg_str_length = SDL_strlen(string_from_arg);
    SDL_memcpy(tmp, string_from_arg, arg_str_length);
    tmp += arg_str_length;
  }
  buf[len] = '\0';
  va_end(ap);

  return buf;
}

void InitializeCustomEvents() {
  registered_event_type = SDL_RegisterEvents(4);

  SDL_zero(start_text_edit);
  start_text_edit.type = registered_event_type;
  start_text_edit.user.code = 1;

  SDL_zero(end_text_edit);
  end_text_edit.type = registered_event_type + 1;
  end_text_edit.user.code = 2;

  SDL_zero(open_folder_dialog);
  open_folder_dialog.type = registered_event_type + 2;
  open_folder_dialog.user.code = 3;

  SDL_zero(refresh_button);
  refresh_button.type = registered_event_type + 3;
  refresh_button.user.code = 4;
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

void updateImg(int rendered_images, int image_list) {
  selected_image = rendered_images;
  char *img_path = jf_concat(3, folder_path, "/", files[image_list]);
  char *argument_list[] = {"swww",   "img",
                           img_path, "--transition-type",
                           "wipe",   "--transition-step",
                           "255",    "--transition-angle",
                           "30",     "--transition-duration",
                           "2",      "--transition-fps",
                           "240",    NULL};

  if (fork() == 0) {
    execvp("swww", argument_list);
  } else {
    free(img_path);
  }
}

void HandleImgClick(Clay_ElementId id, Clay_PointerData pointer_data,
                    intptr_t index) {
  Indexes *indexes = (Indexes *)index;
  if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    updateImg(indexes->rendered_images, indexes->image_list);
  }
}

void HandleFolder(Clay_ElementId id, Clay_PointerData pointer_data) {
  if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    SDL_PushEvent(&open_folder_dialog);
  }
}
