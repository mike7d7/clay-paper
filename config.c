#include "config.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_log.h"
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
    SDL_memcpy(&config_options, buffer, sizeof(uint32_t));
    SDL_memcpy(&number_of_images, buffer + sizeof(uint32_t), sizeof(uint32_t));
    SDL_memcpy(&non_hidden_imgs, buffer + sizeof(uint32_t) * 2,
               sizeof(uint32_t));
    rendered_to_list = SDL_malloc(sizeof(uint32_t) * number_of_images);
    SDL_memcpy(rendered_to_list, buffer + sizeof(uint32_t) * 3,
               sizeof(uint32_t) * number_of_images);
    folder_path = SDL_malloc(filesize - (sizeof(uint32_t) * 3 +
                                         sizeof(uint32_t) * number_of_images));
    SDL_memcpy(
        folder_path,
        buffer + sizeof(uint32_t) * 3 + sizeof(uint32_t) * number_of_images,
        filesize -
            (sizeof(uint32_t) * 3 + sizeof(uint32_t) * number_of_images));
  } else {
    SDL_LogDebug(SDL_LOG_PRIORITY_DEBUG,
                 "Couldn't load from config file, probably because this is a "
                 "first time run: %s",
                 SDL_GetError());
    config_options = 1;
    number_of_images = 0;
    non_hidden_imgs = 0;
    rendered_to_list = SDL_malloc(0);
    folder_path = SDL_malloc(0);
  }
  cache_path =
      jf_concat(2, SDL_GetPrefPath("mike7d7", "clay-paper"), "cache.png");
  SDL_free(buffer);
  SDL_CloseStorage(config_file);
}

void write_config() {
  SDL_Storage *config_file = SDL_OpenUserStorage("mike7d7", "clay-paper", 0);
  uint32_t strlen = SDL_strlen(folder_path);
  void *buffer = SDL_malloc(sizeof(uint32_t) * 3 +
                            (sizeof(uint32_t) * number_of_images) + strlen);

  SDL_memcpy(buffer, &config_options, sizeof(uint32_t));
  SDL_memcpy(buffer + sizeof(uint32_t), &number_of_images, sizeof(uint32_t));
  SDL_memcpy(buffer + sizeof(uint32_t) * 2, &non_hidden_imgs, sizeof(uint32_t));
  SDL_memcpy(buffer + sizeof(uint32_t) * 3, rendered_to_list,
             (sizeof(uint32_t) * number_of_images));
  SDL_memcpy((Uint8 *)buffer + sizeof(uint32_t) * 3 +
                 (sizeof(uint32_t) * number_of_images),
             folder_path, strlen);
  SDL_WriteStorageFile(config_file, "config", buffer,
                       sizeof(uint32_t) * 3 +
                           (sizeof(uint32_t) * number_of_images) + strlen);
  SDL_free(buffer);
  SDL_CloseStorage(config_file);
}
