#include "SDL3/SDL_render.h"
#include "clay.h"
#include "widget_functions.h"
#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const int FONT_ID_BODY_16 = 0;
const Clay_Color COLOR_WHITE = {255, 255, 255, 255};
const Clay_Color COLOR_BACKGROUND = {53, 53, 53, 255};
const Clay_Color COLOR_ELEMENT_BACKGROUND = {255, 255, 255, 255};
const Clay_Color COLOR_TEXTEDIT_HOVERED = {40, 52, 64, 255};
const Clay_Color COLOR_TEXTEDIT_NORMAL = {30, 41, 53, 255};
const Clay_Color COLOR_TEXTEDIT_ACTIVE = {104, 133, 161, 255};
const Clay_Color COLOR_CATPPUCCIN_GREEN = {64, 160, 43, 255};
const Clay_Color COLOR_TRANSPARENT = {0, 0, 0, 0};
const Clay_BorderElementConfig image_border = {.color = COLOR_TEXTEDIT_ACTIVE,
                                               .width = {8, 8, 8, 8}};
const Clay_BorderElementConfig image_no_border = {.width = {0, 0, 0, 0}};
#define TOP_WIDTH 400

void HeaderButton(Clay_String id, Clay_String text, void *on_click_function) {
  CLAY({.id = CLAY_SID(id),
        .layout = {.padding = {10, 10, 6, 6}},
        .backgroundColor = {140, 140, 140, 255},
        .cornerRadius = CLAY_CORNER_RADIUS(12)}) {
    Clay_OnHover(on_click_function, 0);
    CLAY_TEXT(text, CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_16,
                                      .fontSize = 16,
                                      .textColor = {255, 255, 255, 255}}));
  }
}

void HorizontalSpacer() {
  CLAY({.layout = {.sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}}});
}

void TextEditComponent(Clay_String id, TextEditData *data) {
  Clay_String text = (Clay_String){.length = strlen(data->textToEdit),
                                   .chars = data->textToEdit};

  if (data->isPassword) {
    char *masked = SDL_malloc(text.length);
    if (masked != NULL) {
      for (int i = 0; i < text.length; i++) {
        masked[i] = '*';
      }
      text.chars = masked;
    }
  }
  const Clay_BorderWidth border_text_active = {3, 3, 3, 3};
  const Clay_BorderWidth border_text_inactive = {0, 0, 0, 0};

  CLAY({.id = CLAY_SID(id),
        .layout = {.padding = {10, 10, 6, 6},
                   .childAlignment = {.y = CLAY_ALIGN_Y_CENTER},
                   .sizing = {.width = CLAY_SIZING_FIXED(TOP_WIDTH / 2.0f)}},
        .backgroundColor =
            Clay_Hovered() ? COLOR_TEXTEDIT_HOVERED : COLOR_TEXTEDIT_NORMAL,
        .cornerRadius = CLAY_CORNER_RADIUS(12),
        .border = {.width =
                       editing_text ? border_text_active : border_text_inactive,
                   .color = COLOR_TEXTEDIT_ACTIVE}}) {
    Clay_OnHover(HandleTextEditInteraction, 0);
    CLAY_TEXT(
        text.length > 0 ? text : data->hintText,
        CLAY_TEXT_CONFIG({.textColor = text.length > 0
                                           ? (Clay_Color){241, 243, 244, 255}
                                           : (Clay_Color){117, 138, 161, 255},
                          .fontSize = 16,
                          .fontId = 0}));
  }
}

void DropDownButton(Clay_String id, Clay_String text,
                    Clay_String *dropdown_elements,
                    uint_fast32_t number_of_elements,
                    uint_fast32_t id_offset /*should be !=0*/) {
  // HeaderButton
  CLAY({.id = CLAY_SID(id),
        .layout = {.padding = {10, 10, 6, 6}},
        .backgroundColor = {140, 140, 140, 255},
        .cornerRadius = CLAY_CORNER_RADIUS(12)}) {
    // Clay_OnHover(on_click_function, 0);
    CLAY_TEXT(text, CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_16,
                                      .fontSize = 16,
                                      .textColor = {255, 255, 255, 255}}));
    // Dropdown Menu
    bool name_dropdown_active = Clay_PointerOver(Clay_GetElementId(id)) ||
                                Clay_PointerOver(CLAY_SIDI(id, id_offset));
    if (name_dropdown_active) {
      CLAY({
          .id = CLAY_SIDI(id, id_offset),
          .layout = {.padding = {10, 10, 6, 6},
                     .layoutDirection = CLAY_TOP_TO_BOTTOM},
          .backgroundColor = {40, 40, 40, 255},
          .cornerRadius = CLAY_CORNER_RADIUS(12),
          .floating = {.attachTo = CLAY_ATTACH_TO_PARENT,
                       .attachPoints = {.element = CLAY_ATTACH_POINT_LEFT_TOP,
                                        .parent =
                                            CLAY_ATTACH_POINT_LEFT_BOTTOM}},
      }) {
        for (int i = 0; i < number_of_elements; i++) {
          CLAY({.id = CLAY_SID(dropdown_elements[i]),
                .layout = {.padding = {0, 0, 6, 6}}}) {
            CLAY_TEXT(dropdown_elements[i],
                      CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_16,
                                        .fontSize = 16,
                                        .textColor = {255, 255, 255, 255}}));
          };
        }
      };
    }
  }
}

void DropDownCheckBoxButton(Clay_String id, Clay_String text,
                            Clay_String *dropdown_elements,
                            uint_fast32_t number_of_elements,
                            void *on_click_function) {
  // HeaderButton
  CLAY({.id = CLAY_SID(id),
        .layout = {.padding = {10, 10, 6, 6}},
        .backgroundColor = {140, 140, 140, 255},
        .cornerRadius = CLAY_CORNER_RADIUS(12)}) {
    // Clay_OnHover(on_click_function, 0);
    CLAY_TEXT(text, CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_16,
                                      .fontSize = 16,
                                      .textColor = {255, 255, 255, 255}}));
    // Dropdown Menu
    bool name_dropdown_active = Clay_PointerOver(Clay_GetElementId(id)) ||
                                Clay_PointerOver(CLAY_SIDI(id, 1));
    if (name_dropdown_active) {
      CLAY({
          .id = CLAY_SIDI(id, 1),
          .layout = {.padding = {10, 10, 6, 6},
                     .layoutDirection = CLAY_TOP_TO_BOTTOM},
          .backgroundColor = {30, 30, 30, 255},
          .cornerRadius = CLAY_CORNER_RADIUS(12),
          .floating = {.attachTo = CLAY_ATTACH_TO_PARENT,
                       .attachPoints = {.element = CLAY_ATTACH_POINT_CENTER_TOP,
                                        .parent =
                                            CLAY_ATTACH_POINT_LEFT_BOTTOM}},
      }) {
        for (int i = 0; i < number_of_elements; i++) {
          CLAY({
              .id = CLAY_SID(dropdown_elements[i]),
              .layout = {.padding = {0, 0, 6, 6},
                         .layoutDirection = CLAY_LEFT_TO_RIGHT,
                         .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                            .y = CLAY_ALIGN_Y_CENTER}},
          }) {
            Clay_OnHover(on_click_function, i);
            CLAY({.layout.padding = {0, 6, 0, 4}}) {
              CLAY({
                  .layout = {.sizing = {.height = CLAY_SIZING_FIXED(16),
                                        .width = CLAY_SIZING_FIXED(16)}},
                  .backgroundColor = (config_options & 1 << i)
                                         ? COLOR_CATPPUCCIN_GREEN
                                         : COLOR_TRANSPARENT,
                  .border = {.width = {2, 2, 2, 2}, .color = COLOR_WHITE},
              });
            };

            CLAY_TEXT(dropdown_elements[i],
                      CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_16,
                                        .fontSize = 16,
                                        .textColor = {255, 255, 255, 255}}));
          };
        }
      };
    }
  }
}

void HeaderBar() {
  CLAY({
      .id = CLAY_ID("header"),
      .layout = {.sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_FIT(0)},
                 .childAlignment = {.y = CLAY_ALIGN_Y_CENTER,
                                    .x = CLAY_ALIGN_X_CENTER},
                 .padding = {0, 0, 0, 0}},
      .backgroundColor = COLOR_TRANSPARENT,
  }) {
    HeaderButton(CLAY_STRING("Folder"), CLAY_STRING("Folder"),
                 HandleExitButton);
    HorizontalSpacer();
    TextEditComponent(CLAY_STRING("search"), &default_data);
    HorizontalSpacer();
    HeaderButton(CLAY_STRING("Clear"), CLAY_STRING("Clear"), HandleClearButton);
    HorizontalSpacer();
    Clay_String names_arr[] = {CLAY_STRING("Name ↓"), CLAY_STRING("Name ↑"),
                               CLAY_STRING("Date ↓"), CLAY_STRING("Date ↑")};
    DropDownButton(CLAY_STRING("Name"), CLAY_STRING("Name"), names_arr, 4, 1);
    HorizontalSpacer();
    HeaderButton(CLAY_STRING("Refresh"), CLAY_STRING("Refresh"),
                 HandleExitButton);
    HorizontalSpacer();
    HeaderButton(CLAY_STRING("Random"), CLAY_STRING("Random"),
                 HandleExitButton);
    HorizontalSpacer();
    Clay_String options_arr[] = {
        CLAY_STRING("Show GIFs only"), CLAY_STRING("Show subfolders"),
        CLAY_STRING("Show all subfolders"), CLAY_STRING("Show hidden"),
        CLAY_STRING("Show path in tooltip")};
    DropDownCheckBoxButton(CLAY_STRING("Options"), CLAY_STRING("Options"),
                           options_arr, 5, HandleOptionsButton);
    HorizontalSpacer();
    HeaderButton(CLAY_STRING("Exit"), CLAY_STRING("Exit"), HandleExitButton);
  };
}

void ImageGrid(SDL_Texture **img) {
  CLAY({.id = CLAY_ID("image_grid"),
        .clip = {.vertical = true, .childOffset = Clay_GetScrollOffset()},
        .layout = {.sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_GROW()},
                   .layoutDirection = CLAY_TOP_TO_BOTTOM,
                   .childGap = 16,
                   .padding = {16, 16, 0, 0},
                   .childAlignment = {.x = CLAY_ALIGN_X_CENTER}},
        .backgroundColor = COLOR_BACKGROUND}) {
    int index = 0;
    for (int i = 0; i < number_of_images; i += 3) {
      CLAY({.layout = {.sizing = {.height = CLAY_SIZING_FIT(),
                                  .width = CLAY_SIZING_FIT()},
                       .layoutDirection = CLAY_LEFT_TO_RIGHT,
                       .childGap = 16,
                       .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}},
            .backgroundColor = COLOR_BACKGROUND}) {
        for (int j = 0; j < 3 && i + j < number_of_images; j++) {
          if (SDL_strncmp(files[index], ".", 1) == 0 &&
              !(config_options & 1 << 3)) {
            index++;
            continue;
          }
          CLAY({.layout = {.sizing = {.width = CLAY_SIZING_FIXED(200),
                                      .height = CLAY_SIZING_FIXED(200)}},
                .aspectRatio = {(float)img[index]->w / img[index]->h},
                .image = {.imageData = img[index]},
                .border = (i == 0 && j == 0) ? image_border : image_no_border});
          index++;
        }
      };
    }
  };
}
