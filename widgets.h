#include "clay.h"
#include <raylib.h>
#include <stdint.h>
#include <string.h>

const int FONT_ID_BODY_16 = 0;
Clay_Color COLOR_WHITE = {255, 255, 255, 255};
Clay_Color COLOR_BACKGROUND = {53, 53, 53, 255};
Clay_Color COLOR_ELEMENT_BACKGROUND = {255, 255, 255, 255};
Clay_Color COLOR_TEXTEDIT_HOVERED = {40, 52, 64, 255};
Clay_Color COLOR_TEXTEDIT_NORMAL = {30, 41, 53, 255};
#define TOP_WIDTH 400

typedef struct {
  Clay_String hintText;
  char *textToEdit;
  bool isPassword;
  int maxLength;
  bool disable;
} TextEditData;

void HeaderButton(Clay_String text) {
  CLAY({.layout = {.padding = {16, 16, 8, 8}},
        .backgroundColor = {140, 140, 140, 255},
        .cornerRadius = CLAY_CORNER_RADIUS(5)}) {
    CLAY_TEXT(text, CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_16,
                                      .fontSize = 16,
                                      .textColor = {255, 255, 255, 255}}));
  }
}

void HorizontalSpacer() {
  CLAY({.layout = {.sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}}});
}

// not a widget
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
    Clay_OnHover(HandleTextEditInteraction, (intptr_t)data);
    CLAY_TEXT(
        text.length > 0 ? text : data->hintText,
        CLAY_TEXT_CONFIG({.textColor = text.length > 0
                                           ? (Clay_Color){241, 243, 244, 255}
                                           : (Clay_Color){117, 138, 161, 255},
                          .fontSize = 16,
                          .fontId = 0}));
  }
}

// Size of 'empty_buffer' is the maxLength of the string - 1 (space for null
// terminator).
char empty_buffer[11];
TextEditData default_data = (TextEditData){.hintText = CLAY_STRING("Search"),
                                           .textToEdit = empty_buffer,
                                           .isPassword = false,
                                           .maxLength = sizeof(empty_buffer),
                                           .disable = false};

void HeaderBar() {
  CLAY({
      .id = CLAY_ID("header"),
      .layout = {.sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(60)},
                 .childAlignment = {.y = CLAY_ALIGN_Y_CENTER,
                                    .x = CLAY_ALIGN_X_CENTER}},
      .backgroundColor = COLOR_ELEMENT_BACKGROUND,
  }) {
    HorizontalSpacer();
    HeaderButton(CLAY_STRING("Folder"));
    HorizontalSpacer();
    TextEditComponent(CLAY_STRING("search"), &default_data);
    HorizontalSpacer();
    HeaderButton(CLAY_STRING("Clear"));
    HorizontalSpacer();
    HeaderButton(CLAY_STRING("Name"));
    HorizontalSpacer();
    HeaderButton(CLAY_STRING("Refresh"));
    HorizontalSpacer();
    HeaderButton(CLAY_STRING("Random"));
    HorizontalSpacer();
    HeaderButton(CLAY_STRING("Options"));
    HorizontalSpacer();
    HeaderButton(CLAY_STRING("Exit"));
    HorizontalSpacer();
  };
}
