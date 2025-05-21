#include "clay.h"
#include "widget_functions.h"
#include <SDL3/SDL.h>
#include <string.h>

const int FONT_ID_BODY_16 = 0;
Clay_Color COLOR_WHITE = {255, 255, 255, 255};
Clay_Color COLOR_BACKGROUND = {53, 53, 53, 255};
Clay_Color COLOR_ELEMENT_BACKGROUND = {255, 255, 255, 255};
Clay_Color COLOR_TEXTEDIT_HOVERED = {40, 52, 64, 255};
Clay_Color COLOR_TEXTEDIT_NORMAL = {30, 41, 53, 255};
#define TOP_WIDTH 400

void HeaderButton(Clay_String id, Clay_String text, void *on_click_function) {
  CLAY({.id = CLAY_SID(id),
        .layout = {.padding = {16, 16, 8, 8}},
        .backgroundColor = {140, 140, 140, 255},
        .cornerRadius = CLAY_CORNER_RADIUS(5)}) {
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

  CLAY({.id = CLAY_SID(id),
        .layout = {.padding = CLAY_PADDING_ALL(10),
                   .childAlignment = {.y = CLAY_ALIGN_Y_CENTER},
                   .sizing = {CLAY_SIZING_FIXED(TOP_WIDTH / 1.6f)}},
        .backgroundColor =
            Clay_Hovered() ? COLOR_TEXTEDIT_HOVERED : COLOR_TEXTEDIT_NORMAL,
        .cornerRadius = {.topLeft = 8}}) {
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
    HeaderButton(CLAY_STRING("Name"), CLAY_STRING("Name"), HandleExitButton);
    HorizontalSpacer();
    HeaderButton(CLAY_STRING("Refresh"), CLAY_STRING("Refresh"),
                 HandleExitButton);
    HorizontalSpacer();
    HeaderButton(CLAY_STRING("Random"), CLAY_STRING("Random"),
                 HandleExitButton);
    HorizontalSpacer();
    HeaderButton(CLAY_STRING("Options"), CLAY_STRING("Options"),
                 HandleExitButton);
    HorizontalSpacer();
    HeaderButton(CLAY_STRING("Exit"), CLAY_STRING("Exit"), HandleExitButton);
    HorizontalSpacer();
  };
}
