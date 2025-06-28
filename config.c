#include "config.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_storage.h"
#include "widget_functions.h"
#include <stdint.h>

void load_config() {
  SDL_Storage *config_file = SDL_OpenUserStorage("mike7d7", "clay-paper", 0);
  Uint64 filesize;

  SDL_GetStorageFileSize(config_file, "config", &filesize);
  void *buffer = SDL_malloc(filesize);

  if (SDL_ReadStorageFile(config_file, "config", buffer, filesize)) {
    folder_path = SDL_malloc(filesize - sizeof(uint32_t));
    SDL_memcpy(&config_options, buffer, sizeof(uint32_t));
    SDL_memcpy(folder_path, buffer + sizeof(uint32_t),
               filesize - sizeof(uint32_t));
  } else {
    config_options = 1;
    folder_path = SDL_malloc(0);
  }
  SDL_free(buffer);
}

void write_config() {
  SDL_Storage *config_file = SDL_OpenUserStorage("mike7d7", "clay-paper", 0);
  uint32_t strlen = SDL_strlen(folder_path);
  void *buffer = SDL_malloc(sizeof(uint32_t) + strlen);

  SDL_memcpy(buffer, &config_options, sizeof(uint32_t));
  SDL_memcpy((Uint8 *)buffer + sizeof(uint32_t), folder_path, strlen);
  SDL_WriteStorageFile(config_file, "config", buffer,
                       sizeof(uint32_t) + strlen);
  SDL_free(buffer);
}
