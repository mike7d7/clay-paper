#include "config.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_storage.h"
#include "widget_functions.h"

void load_config() {
  SDL_Storage *config_file = SDL_OpenUserStorage("mike7d7", "clay-paper", 0);
  Uint64 filesize;

  SDL_GetStorageFileSize(config_file, "config", &filesize);
  void *buffer = SDL_malloc(filesize);
  Uint8 *buffer_ptr = buffer;
  Uint8 *buffer_ptr_end = buffer + filesize;
  Uint32 rendered_to_list_size;
  Uint32 folder_path_length;

  if (SDL_ReadStorageFile(config_file, "config", buffer, filesize)) {
    // config_options
    SDL_memcpy(&config_options, buffer_ptr, sizeof(Uint32));
    buffer_ptr += sizeof(Uint32);
    // number_of_images
    SDL_memcpy(&number_of_images, buffer_ptr, sizeof(Uint32));
    buffer_ptr += sizeof(Uint32);
    // non_hidden_imgs
    SDL_memcpy(&non_hidden_imgs, buffer_ptr, sizeof(Uint32));
    buffer_ptr += sizeof(Uint32);

    // rendered_to_list
    rendered_to_list_size = number_of_images * sizeof(Uint32);
    rendered_to_list = SDL_malloc(rendered_to_list_size);

    SDL_memcpy(rendered_to_list, buffer_ptr, rendered_to_list_size);
    buffer_ptr += rendered_to_list_size;
    // files
    files = (char **)SDL_calloc(number_of_images, sizeof(char *));
    for (Uint32 i = 0; i < number_of_images; i++) {
      Uint32 str_len;
      SDL_memcpy(&str_len, buffer_ptr, sizeof(Uint32));
      buffer_ptr += sizeof(Uint32);

      files[i] = (char *)SDL_malloc(str_len);
      SDL_memcpy(files[i], buffer_ptr, str_len);
      buffer_ptr += str_len;
    }
    // folder_path
    folder_path_length = buffer_ptr_end - buffer_ptr;
    folder_path = SDL_malloc(folder_path_length);
    SDL_memcpy(folder_path, buffer_ptr, folder_path_length);
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
  Uint32 ints_size = sizeof(Uint32) * 3;
  Uint32 rendered_to_list_size = number_of_images * sizeof(Uint32);
  Uint32 folder_path_length =
      SDL_strlen(folder_path) + 1; // +1 to include null terminator
  Uint32 files_size =
      0; // files is the variable where all image paths are stored.
  for (int i = 0; i < number_of_images; i++) {
    files_size += sizeof(Uint32);           // Current string length
    files_size += SDL_strlen(files[i]) + 1; // Current string + \0
  }
  Uint32 buffer_size =
      ints_size + folder_path_length + rendered_to_list_size + files_size;
  void *buffer = SDL_malloc(buffer_size);

  Uint8 *buffer_ptr = buffer;
  // config_options
  SDL_memcpy(buffer_ptr, &config_options, sizeof(Uint32));
  buffer_ptr += sizeof(Uint32);
  // number_of_images
  SDL_memcpy(buffer_ptr, &number_of_images, sizeof(Uint32));
  buffer_ptr += sizeof(Uint32);
  // non_hidden_imgs
  SDL_memcpy(buffer_ptr, &non_hidden_imgs, sizeof(Uint32));
  buffer_ptr += sizeof(Uint32);
  // rendered_to_list
  SDL_memcpy(buffer_ptr, rendered_to_list, rendered_to_list_size);
  buffer_ptr += rendered_to_list_size;
  // files
  for (int i = 0; i < number_of_images; i++) {
    Uint32 str_len = SDL_strlen(files[i]) + 1;
    SDL_memcpy(buffer_ptr, &str_len, sizeof(Uint32));
    buffer_ptr += sizeof(Uint32);
    SDL_memcpy(buffer_ptr, files[i], str_len);
    buffer_ptr += str_len;
  }
  // folder_path
  SDL_memcpy(buffer_ptr, folder_path, folder_path_length);

  SDL_WriteStorageFile(config_file, "config", buffer, buffer_size);
  SDL_free(buffer);
  SDL_CloseStorage(config_file);
}
