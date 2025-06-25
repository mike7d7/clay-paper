#include "config.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_storage.h"
#include "widget_functions.h"
#include <stdint.h>

typedef struct {
  int config_int;
  char *imgs_path;
} Config_Format;

void read_config() {
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
    folder_path = "";
  }
  SDL_free(buffer);
}

void write_config() {
  SDL_Storage *config_file = SDL_OpenUserStorage("mike7d7", "clay-paper", 0);

  Config_Format test_conf = {33, "helo from this universe 4"};
  uint32_t strlen = SDL_strlen(test_conf.imgs_path);
  void *buffer = SDL_malloc(sizeof(uint32_t) + strlen);
  SDL_memcpy(buffer, &test_conf.config_int, sizeof(uint32_t));
  SDL_memcpy((Uint8 *)buffer + sizeof(uint32_t), test_conf.imgs_path, strlen);
  SDL_WriteStorageFile(config_file, "config", buffer,
                       sizeof(uint32_t) + strlen);
  SDL_free(buffer);
}

void load_config() {
  write_config();
  read_config();
}
