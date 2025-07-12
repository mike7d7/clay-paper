#include "SDL3/SDL_dialog.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"
#include <string.h>
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#define CLAY_IMPLEMENTATION
#include "clay.h"

#include <stdio.h>

#include "clay_renderer_SDL3.c"
#include "config.h"
#include "widgets.h"

#define NUMBER_OF_FONTS 1
#define SCROLL_SENSITIVITY 3 // The higher, the more it scrolls

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

  TTF_SetFontSize(font, config->fontSize);
  if (!TTF_GetStringSize(font, text.chars, text.length, &width, &height)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to measure text: %s",
                 SDL_GetError());
  }

  return (Clay_Dimensions){(float)width, (float)height};
}

void HandleClayErrors(Clay_ErrorData errorData) {
  printf("%s", errorData.errorText.chars);
}

Clay_RenderCommandArray ClayImageSample_CreateLayout(SDL_Texture *img) {
  Clay_BeginLayout();

  CLAY({.id = CLAY_ID("outer-container"),
        .layout =
            {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)},
                .padding = CLAY_PADDING_ALL(16),
                .childGap = 16,
            },
        .backgroundColor = COLOR_BACKGROUND}) {
    HeaderBar();
    ImageGrid(img);
    CLAY({
        .id = CLAY_ID("footer"),
        .layout = {.sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(60)}},
        .backgroundColor = COLOR_ELEMENT_BACKGROUND,
    });
  };

  return Clay_EndLayout();
}

SDL_Texture **img;
SDL_Texture *texture_atlas;

void create_texture_atlas(AppState *state, SDL_Texture **imgs) {
  Uint32 height = number_of_images / 3;
  height += (number_of_images % 3) ? 1 : 0;
  height *= 200;
  float image_width;
  float image_height;
  double scale_w;
  double scale_h;
  double scale;

  SDL_Texture *texture_atlas =
      SDL_CreateTexture(state->rendererData.renderer, SDL_PIXELFORMAT_RGBA32,
                        SDL_TEXTUREACCESS_TARGET, 600, height);
  if (!SDL_SetRenderTarget(state->rendererData.renderer, texture_atlas)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Error setting texture atlas as render target: %s",
                 SDL_GetError());
  } else {
    for (int i = 0; i < number_of_images; i++) {
      SDL_GetTextureSize(imgs[i], &image_width, &image_height);
      scale_w = 200 / image_width;
      scale_h = 200 / image_height;
      scale = SDL_min(scale_w, scale_h);

      SDL_FRect position_in_atlas = {
          ((i % 3) * 200) + (200 - image_width * scale) / 2,
          (SDL_floor((double)i / 3) * 200) + (200 - image_height * scale) / 2,
          image_width * scale, image_height * scale};
      SDL_RenderTexture(state->rendererData.renderer, imgs[i], NULL,
                        &position_in_atlas);
    }
    SDL_Surface *atlas_as_surface =
        SDL_RenderReadPixels(state->rendererData.renderer, NULL);
    if (atlas_as_surface) {
      if (IMG_SavePNG(atlas_as_surface, cache_path)) {
        SDL_LogDebug(SDL_LOG_PRIORITY_DEBUG, "Texture atlas (cache) saved.");
      } else {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Error saving texture atlas to file: %s", SDL_GetError());
      }
    } else {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Error locking texture as surface: %s", SDL_GetError());
    }
  }

  SDL_SetRenderTarget(state->rendererData.renderer, NULL);
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

  if (!SDL_CreateWindowAndRenderer("Clay Demo", 800, 600, SDL_WINDOW_RESIZABLE,
                                   &state->window,
                                   &state->rendererData.renderer)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Failed to create window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  state->rendererData.textEngine =
      TTF_CreateRendererTextEngine(state->rendererData.renderer);
  if (!state->rendererData.textEngine) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Failed to create text engine from renderer: %s",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }

  state->rendererData.fonts = SDL_calloc(NUMBER_OF_FONTS, sizeof(TTF_Font *));
  if (!state->rendererData.fonts) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Failed to allocate memory for the font array: %s",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }

  TTF_Font *font = TTF_OpenFont("/nix/var/nix/profiles/system/sw/share/X11/"
                                "fonts/LiberationMono-Regular.ttf",
                                16);
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load font: %s",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }

  state->rendererData.fonts[FONT_ID] = font;

  load_config();
  texture_atlas = IMG_LoadTexture(state->rendererData.renderer, cache_path);

  /* Initialize Clay */
  Uint64 totalMemorySize = Clay_MinMemorySize();
  Clay_Arena clayMemory = (Clay_Arena){.memory = SDL_malloc(totalMemorySize),
                                       .capacity = totalMemorySize};

  int width, height;
  SDL_GetWindowSize(state->window, &width, &height);
  Clay_Initialize(clayMemory, (Clay_Dimensions){(float)width, (float)height},
                  (Clay_ErrorHandler){HandleClayErrors});
  Clay_SetMeasureTextFunction(SDL_MeasureText, state->rendererData.fonts);
  Clay_SetDebugModeEnabled(false);

  InitializeCustomEvents();
  *appstate = state;
  shown_images =
      (config_options & SHOW_HIDDEN) ? number_of_images : non_hidden_imgs;

  return SDL_APP_CONTINUE;
}

void load_images(AppState *state) {
  if (folder_path) {
    SDL_free(files);
    SDL_LogDebug(SDL_LOG_PRIORITY_DEBUG, "Full path to selected file: '%s'",
                 folder_path);
    files =
        SDL_GlobDirectory(folder_path, "*.*", 0,
                          &number_of_images); // currently ignores subfolders
    SDL_free(img);
    SDL_LogDebug(SDL_LOG_PRIORITY_DEBUG, "number of images = %i",
                 number_of_images);
    SDL_free(rendered_to_list);
    img = SDL_malloc(sizeof(SDL_Texture *) * number_of_images);
    rendered_to_list = (Uint32 *)SDL_malloc(sizeof(Uint32) * number_of_images);
    non_hidden_imgs = 0;
    for (int i = 0; i < number_of_images; i++) {
      if (files[i][0] != '.') {
        rendered_to_list[non_hidden_imgs] = i;
        non_hidden_imgs++;
      }
      char *img_path = jf_concat(3, folder_path, "/", files[i]);
      printf("%s\n", img_path);
      img[i] = IMG_LoadTexture(state->rendererData.renderer, img_path);
      SDL_free(img_path);
    }
    selected_image = 0;
    create_texture_atlas(state, img);
    texture_atlas = IMG_LoadTexture(state->rendererData.renderer, cache_path);
    Clay_GetScrollContainerData(CLAY_ID("image_grid")).scrollPosition->y = 0;
  }
}

static void SDLCALL folder_dialog_callback(void *userdata,
                                           const char *const *filelist,
                                           int filter) {
  AppState *state = userdata;
  if (!filelist) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "An error occured: %s",
                 SDL_GetError());
    return;
  } else if (!*filelist) {
    SDL_LogDebug(SDL_LOG_PRIORITY_DEBUG, "The user did not select any file.");
    SDL_LogDebug(SDL_LOG_PRIORITY_DEBUG,
                 "Most likely, the dialog was canceled.");
    return;
  }

  if (*filelist) {
    SDL_free(folder_path);
    folder_path = SDL_malloc(SDL_strlen(*filelist) + 1);
    SDL_strlcpy(folder_path, *filelist, SDL_strlen(*filelist) + 1);
    load_images(state);
  }

  if (filter < 0) {
    SDL_LogDebug(SDL_LOG_PRIORITY_DEBUG,
                 "The current platform does not support fetching "
                 "the selected filter, or the user did not select"
                 " any filter.");
  }
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  AppState *state = appstate;
  SDL_AppResult ret_val = SDL_APP_CONTINUE;
  Clay_ScrollContainerData scroll_data =
      Clay_GetScrollContainerData(CLAY_ID("image_grid"));

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
        true,
        (Clay_Vector2){event->wheel.x, event->wheel.y * SCROLL_SENSITIVITY},
        0.01f);
    break;
  case SDL_EVENT_USER: // start_text_edit
    SDL_StartTextInput(state->window);
    SDL_SetTextInputArea(state->window, event->user.data1, 0);
    editing_text = true;
    SDL_free(event->user.data1);
    break;
  case SDL_EVENT_USER + 1: // end_text_edit
    SDL_StopTextInput(state->window);
    editing_text = false;
    break;
  case SDL_EVENT_USER + 2: // open_folder_dialog
    SDL_ShowOpenFolderDialog(folder_dialog_callback, state, state->window, NULL,
                             false);
    break;
  case SDL_EVENT_USER + 3: // refresh_button
    load_images(state);
    break;
  case SDL_EVENT_TEXT_INPUT:
    strcat(empty_buffer, event->text.text);
    break;
  case SDL_EVENT_KEY_DOWN:
    if (editing_text) {
      selected_image = 0;
      if (event->key.key == SDLK_ESCAPE || event->key.key == SDLK_RETURN) {
        SDL_StopTextInput(state->window);
        editing_text = false;
      }
      if (event->key.key == SDLK_BACKSPACE) {
        int length = strlen(empty_buffer);
        if (length > 0) {
          empty_buffer[length - 1] = '\0';
        }
      }
    } else {
      if (event->key.key == SDLK_ESCAPE ||
          event->key.scancode == SDL_SCANCODE_Q) {
        ret_val = SDL_APP_SUCCESS;
      }
      if (event->key.scancode == SDL_SCANCODE_H ||
          event->key.key == SDLK_LEFT) {
        if (selected_image > 0) {
          selected_image--;
          Clay_ElementData element_data = Clay_GetElementData(
              CLAY_IDI("image_row", (int)selected_image / 3));
          if (element_data.boundingBox.y < 0) {
            scroll_data.scrollPosition->y +=
                element_data.boundingBox.height + 16;
          }
        }
      }
      if (event->key.scancode == SDL_SCANCODE_J ||
          event->key.key == SDLK_DOWN) {
        if (selected_image < shown_images - 4) {
          selected_image += 3;
        } else {
          selected_image = shown_images - 1;
        }
        Clay_ElementData element_data =
            Clay_GetElementData(CLAY_IDI("image_row", (int)selected_image / 3));
        if (element_data.boundingBox.y + element_data.boundingBox.height >
            scroll_data.scrollContainerDimensions.height + 63) {
          scroll_data.scrollPosition->y -= element_data.boundingBox.height + 16;
        }
      }
      if (event->key.scancode == SDL_SCANCODE_K || event->key.key == SDLK_UP) {
        if (selected_image > 2) {
          selected_image -= 3;
        } else {
          selected_image = 0;
        }
        Clay_ElementData element_data =
            Clay_GetElementData(CLAY_IDI("image_row", (int)selected_image / 3));
        if (element_data.boundingBox.y < 0) {
          scroll_data.scrollPosition->y += element_data.boundingBox.height + 16;
        }
      }
      if (event->key.scancode == SDL_SCANCODE_L ||
          event->key.key == SDLK_RIGHT) {
        if (selected_image < shown_images - 1) {
          selected_image++;
          Clay_ElementData element_data = Clay_GetElementData(
              CLAY_IDI("image_row", (int)selected_image / 3));
          if (element_data.boundingBox.y + element_data.boundingBox.height >
              scroll_data.scrollContainerDimensions.height + 63) {
            scroll_data.scrollPosition->y -=
                element_data.boundingBox.height + 16;
          }
        }
      }
      if ((event->key.mod & SDL_KMOD_CTRL) && event->key.key == SDLK_H) {
        selected_image = 0;
        config_options ^= SHOW_HIDDEN;
        scroll_data.scrollPosition->y = 0;
      }
      if (event->key.key == SDLK_RETURN) {
        updateImg(selected_image, rendered_to_list[selected_image]);
      }
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

  Clay_RenderCommandArray render_commands =
      ClayImageSample_CreateLayout(texture_atlas);

  SDL_SetRenderDrawColor(state->rendererData.renderer, 0, 0, 0, 255);
  SDL_RenderClear(state->rendererData.renderer);

  SDL_Clay_RenderClayCommands(&state->rendererData, &render_commands);

  SDL_RenderPresent(state->rendererData.renderer);

  if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("search")))) {
    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT));
  } else {
    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  (void)result;

  if (result != SDL_APP_SUCCESS) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Application failed to run");
  }

  AppState *state = appstate;
  write_config();

  if (state) {
    if (state->rendererData.renderer)
      SDL_DestroyRenderer(state->rendererData.renderer);

    if (state->window)
      SDL_DestroyWindow(state->window);

    if (state->rendererData.fonts) {
      for (size_t i = 0; i < NUMBER_OF_FONTS; i++) {
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
