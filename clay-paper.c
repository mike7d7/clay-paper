#include <stdint.h>
#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay_renderer_raylib.c"
#include <raylib.h>

#include "widgets.h"

// This function is new since the video was published
void HandleClayErrors(Clay_ErrorData errorData) {
  printf("%s", errorData.errorText.chars);
}

int main(void) {
  Clay_Raylib_Initialize(0, 0, "Clay-paper",
                         FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT |
                             FLAG_VSYNC_HINT);

  uint64_t clayRequiredMemory = Clay_MinMemorySize();
  Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(
      clayRequiredMemory, malloc(clayRequiredMemory));
  Clay_Initialize(
      clayMemory,
      (Clay_Dimensions){.width = GetScreenWidth(), .height = GetScreenHeight()},
      (Clay_ErrorHandler){HandleClayErrors});
  Font fonts[1];
  fonts[0] = LoadFontEx(
      "/nix/store/z52y6yr69xa38ynsf4dssm5phvcb9dvp-liberation-fonts-2.1.5/"
      "share/fonts/"
      "truetype/LiberationMono-Regular.ttf",
      48, 0, 400);
  SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_BILINEAR);
  Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

  while (!WindowShouldClose()) {
    // Run once per frame
    Clay_SetLayoutDimensions((Clay_Dimensions){.width = GetScreenWidth(),
                                               .height = GetScreenHeight()});

    Vector2 mousePosition = GetMousePosition();
    Vector2 scrollDelta = GetMouseWheelMoveV();
    Clay_SetPointerState((Clay_Vector2){mousePosition.x, mousePosition.y},
                         IsMouseButtonDown(0));
    Clay_UpdateScrollContainers(
        true, (Clay_Vector2){scrollDelta.x, scrollDelta.y}, GetFrameTime());

    Clay_BeginLayout();

    CLAY({.id = CLAY_ID("outer-container"),
          .layout =
              {
                  .layoutDirection = CLAY_TOP_TO_BOTTOM,
                  .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)},
                  .padding = CLAY_PADDING_ALL(16),
              },
          .backgroundColor = COLOR_BACKGROUND}) {
      HeaderBar();
      CLAY({.layout = {.sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}}});
      CLAY({
          .id = CLAY_ID("footer"),
          .layout = {.sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(60)}},
          .backgroundColor = COLOR_ELEMENT_BACKGROUND,
      });
    };

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();

    BeginDrawing();
    ClearBackground(BLACK);
    Clay_Raylib_Render(renderCommands, fonts);
    EndDrawing();
  }
  // This function is new since the video was published
  Clay_Raylib_Close();
}
