#include "widget_functions.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"
#include "clay.h"
#include <SDL3/SDL.h>
#include <stdint.h>
#include <unistd.h>

char empty_buffer[64];
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
Uint32 shown_images;
Uint32 transition_type;
Uint32 fill_type;
TextEditData *current_textbox_buffer;

// swww arguments
char duration_buffer[4];
char angle_buffer[4];
char fps_buffer[4];
char steps_buffer[4];

char *transition_types[] = {"any",  "none",   "simple", "fade",   "wipe",
                            "left", "right",  "top",    "bottom", "wave",
                            "grow", "center", "outer",  "random"};
char *fill_types[] = {"no", "crop", "fit", "stretch"};

TextEditData default_data = (TextEditData){
    .hintText = CLAY_STRING("Search"),
    .textToEdit = empty_buffer,
    .isPassword = false,
    .maxLength = sizeof(empty_buffer),
};
TextEditData duration_data = (TextEditData){
    .hintText = CLAY_STRING("Duration"),
    .textToEdit = duration_buffer,
    .isPassword = false,
    .maxLength = sizeof(duration_buffer),
};
TextEditData angle_data = (TextEditData){
    .hintText = CLAY_STRING("Angle"),
    .textToEdit = angle_buffer,
    .isPassword = false,
    .maxLength = sizeof(angle_buffer),
};
TextEditData fps_data = (TextEditData){
    .hintText = CLAY_STRING("FPS"),
    .textToEdit = fps_buffer,
    .isPassword = false,
    .maxLength = sizeof(fps_buffer),
};
TextEditData steps_data = (TextEditData){
    .hintText = CLAY_STRING("Steps"),
    .textToEdit = steps_buffer,
    .isPassword = false,
    .maxLength = sizeof(steps_buffer),
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
      start_text_edit.user.data2 = (void *)userData;
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
    Clay_ScrollContainerData scroll_data =
        Clay_GetScrollContainerData(CLAY_ID("image_grid"));
    selected_image = 0;
    config_options ^= 1 << config;
    scroll_data.scrollPosition->y = 0;
  }
}

void updateImg(int rendered_images, int image_list) {
  selected_image = rendered_images;
  char *img_path = jf_concat(3, folder_path, "/", files[image_list]);
  char *argument_list[] = {"swww",
                           "img",
                           img_path,
                           "--resize",
                           fill_types[fill_type],
                           "--transition-type",
                           transition_types[transition_type],
                           "--transition-step",
                           SDL_strlen(steps_buffer) ? steps_buffer : "90",
                           "--transition-angle",
                           SDL_strlen(angle_buffer) ? angle_buffer : "30",
                           "--transition-duration",
                           SDL_strlen(duration_buffer) ? duration_buffer : "2",
                           "--transition-fps",
                           SDL_strlen(fps_buffer) ? fps_buffer : "120",
                           NULL};
  if (config_options & USE_MATUGEN) {
    if (fork() == 0) {
      char *matugen_args[] = {"matugen", "image", img_path, NULL};
      execvp("matugen", matugen_args);
    }
  }

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

void HandleRefresh(Clay_ElementId id, Clay_PointerData pointer_data) {
  if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    SDL_PushEvent(&refresh_button);
  }
}

void HandleRandom(Clay_ElementId id, Clay_PointerData pointer_data) {
  if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    if ((config_options & SHOW_HIDDEN)) {
      selected_image = SDL_rand(number_of_images);
      updateImg(selected_image, selected_image);
    } else {
      selected_image = SDL_rand(non_hidden_imgs);
      int img_index = rendered_to_list[selected_image];
      updateImg(selected_image, img_index);
    }
    Clay_ScrollContainerData scroll_data =
        Clay_GetScrollContainerData(CLAY_ID("image_grid"));
    Clay_ElementData element_data =
        Clay_GetElementData(CLAY_IDI("image_row", (int)selected_image / 3));
    float new_scroll_data = (element_data.boundingBox.height + 16) *
                            (SDL_floorf((float)selected_image / 3));
    if (new_scroll_data > scroll_data.scrollContainerDimensions.height + 63 ||
        element_data.boundingBox.y < 0) {
      scroll_data.scrollPosition->y = 0;
      scroll_data.scrollPosition->y -= new_scroll_data;
    }
  }
}

void HandleFillTypes(Clay_ElementId id, Clay_PointerData pointer_data,
                     int selected_index) {

  if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    fill_type = selected_index;
    SDL_Log("selected fill type = %s", fill_types[selected_index]);
  }
}

void HandleTransitionTypes(Clay_ElementId id, Clay_PointerData pointer_data,
                           int selected_index) {

  if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    transition_type = selected_index;
    SDL_Log("selected fill type = %s", transition_types[selected_index]);
  }
}
