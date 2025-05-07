#include "clay.h"

const int FONT_ID_BODY_16 = 0;
Clay_Color COLOR_WHITE = {255, 255, 255, 255};
Clay_Color COLOR_BACKGROUND = {53, 53, 53, 255};
Clay_Color COLOR_ELEMENT_BACKGROUND = {255, 255, 255, 255};

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
