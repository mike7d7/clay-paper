#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#define CLAY_IMPLEMENTATION
#include "clay.h"

#include <stdio.h>

#include "clay_renderer_SDL3.c"
#include "widgets.h"

static const Uint32 FONT_ID = 0;

typedef struct app_state {
  SDL_Window *window;
  Clay_SDL3RendererData rendererData;
} AppState;

static inline Clay_Dimensions SDL_MeasureText(Clay_StringSlice text,
                                              Clay_TextElementConfig *config,
                                              void *userData) {
  TTF_Font **fonts = userData;
  TTF_Font *font = fonts[config->fontId];
  int width, height;

  if (!TTF_GetStringSize(font, text.chars, text.length, &width, &height)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to measure text: %s",
                 SDL_GetError());
  }

  return (Clay_Dimensions){(float)width, (float)height};
}

void HandleClayErrors(Clay_ErrorData errorData) {
  printf("%s", errorData.errorText.chars);
}

Clay_RenderCommandArray ClayImageSample_CreateLayout() {
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

  return Clay_EndLayout();
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  if (!TTF_Init()) {
    return SDL_APP_FAILURE;
  }

  AppState *state = SDL_calloc(1, sizeof(AppState));
  if (!state) {
    return SDL_APP_FAILURE;
  }
  *appstate = state;

  if (!SDL_CreateWindowAndRenderer("Clay Demo", 640, 480, 0, &state->window,
                                   &state->rendererData.renderer)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Failed to create window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_SetWindowResizable(state->window, true);

  state->rendererData.textEngine =
      TTF_CreateRendererTextEngine(state->rendererData.renderer);
  if (!state->rendererData.textEngine) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Failed to create text engine from renderer: %s",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }

  state->rendererData.fonts = SDL_calloc(1, sizeof(TTF_Font *));
  if (!state->rendererData.fonts) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Failed to allocate memory for the font array: %s",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }

  TTF_Font *font = TTF_OpenFont(
      "/nix/store/vl44mgyhq46plr28vfj06zj9lk89jyaw-liberation-fonts-2.1.5/"
      "share/fonts/truetype/LiberationMono-Regular.ttf",
      24);
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load font: %s",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }

  state->rendererData.fonts[FONT_ID] = font;

  /* Initialize Clay */
  uint64_t totalMemorySize = Clay_MinMemorySize();
  Clay_Arena clayMemory = (Clay_Arena){.memory = SDL_malloc(totalMemorySize),
                                       .capacity = totalMemorySize};

  int width, height;
  SDL_GetWindowSize(state->window, &width, &height);
  Clay_Initialize(clayMemory, (Clay_Dimensions){(float)width, (float)height},
                  (Clay_ErrorHandler){HandleClayErrors});
  Clay_SetMeasureTextFunction(SDL_MeasureText, state->rendererData.fonts);

  *appstate = state;
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  AppState *state = appstate;
  SDL_AppResult ret_val = SDL_APP_CONTINUE;

  switch (event->type) {
  case SDL_EVENT_QUIT:
    ret_val = SDL_APP_SUCCESS;
    break;
  case SDL_EVENT_WINDOW_RESIZED:
    Clay_SetLayoutDimensions((Clay_Dimensions){(float)event->window.data1,
                                               (float)event->window.data2});
    break;
  case SDL_EVENT_MOUSE_MOTION:
    Clay_SetPointerState((Clay_Vector2){event->motion.x, event->motion.y},
                         event->motion.state & SDL_BUTTON_LMASK);
    break;
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    Clay_SetPointerState((Clay_Vector2){event->button.x, event->button.y},
                         event->button.button == SDL_BUTTON_LEFT);
    break;
  case SDL_EVENT_MOUSE_BUTTON_UP:
    Clay_SetPointerState((Clay_Vector2){event->button.x, event->button.y},
                         event->button.button == SDL_BUTTON_LEFT);
    break;
  case SDL_EVENT_MOUSE_WHEEL:
    Clay_UpdateScrollContainers(
        true, (Clay_Vector2){event->wheel.x, event->wheel.y}, 0.01f);
    break;
  case SDL_EVENT_USER:
    SDL_StartTextInput(state->window);
    SDL_SetTextInputArea(state->window, event->user.data1, 0);
    free(event->user.data1);
    break;
  case SDL_EVENT_TEXT_INPUT:
    strcat(empty_buffer, event->text.text);
    break;
  case SDL_EVENT_KEY_DOWN:
    if (event->key.key == SDLK_ESCAPE || event->key.key == SDLK_RETURN) {
      SDL_StopTextInput(state->window);
    }
    break;
  case SDL_EVENT_TEXT_EDITING:
    /*
    Update the composition text.
    Update the cursor position.
    Update the selection length (if any).
    */
    // composition = event->edit.text;
    // cursor = event->edit.start;
    // selection_len = event->edit.length;
    break;
  default:
    break;
  };

  return ret_val;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *state = appstate;

  Clay_RenderCommandArray render_commands = ClayImageSample_CreateLayout();

  SDL_SetRenderDrawColor(state->rendererData.renderer, 0, 0, 0, 255);
  SDL_RenderClear(state->rendererData.renderer);

  SDL_Clay_RenderClayCommands(&state->rendererData, &render_commands);

  SDL_RenderPresent(state->rendererData.renderer);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  (void)result;

  if (result != SDL_APP_SUCCESS) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Application failed to run");
  }

  AppState *state = appstate;

  if (state) {
    if (state->rendererData.renderer)
      SDL_DestroyRenderer(state->rendererData.renderer);

    if (state->window)
      SDL_DestroyWindow(state->window);

    if (state->rendererData.fonts) {
      for (size_t i = 0; i < sizeof(state->rendererData.fonts) /
                                 sizeof(*state->rendererData.fonts);
           i++) {
        TTF_CloseFont(state->rendererData.fonts[i]);
      }

      SDL_free(state->rendererData.fonts);
    }

    if (state->rendererData.textEngine)
      TTF_DestroyRendererTextEngine(state->rendererData.textEngine);

    SDL_free(state);
  }
  TTF_Quit();
}
