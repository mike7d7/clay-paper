#include "clay.h"
#include "widget_functions.h"
#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const int FONT_ID_BODY_16 = 0;
Clay_Color COLOR_WHITE = {255, 255, 255, 255};
Clay_Color COLOR_BACKGROUND = {53, 53, 53, 255};
Clay_Color COLOR_ELEMENT_BACKGROUND = {255, 255, 255, 255};
Clay_Color COLOR_TEXTEDIT_HOVERED = {40, 52, 64, 255};
Clay_Color COLOR_TEXTEDIT_NORMAL = {30, 41, 53, 255};
Clay_Color COLOR_TEXTEDIT_ACTIVE = {104, 133, 161, 255};
Clay_Color COLOR_CATPPUCCIN_GREEN = {64, 160, 43, 255};
Clay_Color COLOR_TRANSPARENT = {0, 0, 0, 0};
#define TOP_WIDTH 400

void HeaderButton(Clay_String id, Clay_String text, void *on_click_function) {
  CLAY({.id = CLAY_SID(id),
        .layout = {.padding = {16, 16, 8, 8}},
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
    char *masked = malloc(text.length);
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
        .layout = {.padding = {16, 16, 8, 8},
                   .childAlignment = {.y = CLAY_ALIGN_Y_CENTER},
                   .sizing = {CLAY_SIZING_FIXED(TOP_WIDTH / 1.6f)}},
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
        .layout = {.padding = {16, 16, 8, 8}},
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
          .layout = {.padding = {16, 16, 8, 8},
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
                .layout = {.padding = {0, 0, 8, 8}}}) {
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
        .layout = {.padding = {16, 16, 8, 8}},
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
          .layout = {.padding = {16, 16, 8, 8},
                     .layoutDirection = CLAY_TOP_TO_BOTTOM},
          .backgroundColor = {40, 40, 40, 255},
          .cornerRadius = CLAY_CORNER_RADIUS(12),
          .floating = {.attachTo = CLAY_ATTACH_TO_PARENT,
                       .attachPoints = {.element = CLAY_ATTACH_POINT_LEFT_TOP,
                                        .parent =
                                            CLAY_ATTACH_POINT_LEFT_BOTTOM}},
      }) {
        for (int i = 0; i < number_of_elements; i++) {
          CLAY({
              .id = CLAY_SID(dropdown_elements[i]),
              .layout = {.padding = {0, 0, 8, 8},
                         .layoutDirection = CLAY_LEFT_TO_RIGHT,
                         .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                            .y = CLAY_ALIGN_Y_CENTER}},
          }) {
            Clay_OnHover(on_click_function, i);
            CLAY({.layout.padding = {0, 8, 0, 4}}) {
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
      .layout = {.sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(60)},
                 .childAlignment = {.y = CLAY_ALIGN_Y_CENTER,
                                    .x = CLAY_ALIGN_X_CENTER}},
      .backgroundColor = COLOR_ELEMENT_BACKGROUND,
  }) {
    HorizontalSpacer();
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
    HorizontalSpacer();
  };
}
