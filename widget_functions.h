#include "SDL3/SDL_stdinc.h"
#include "clay.h"
#include <stdint.h>

#define SHOW_GIFS_ONLY (1 << 0)
#define SHOW_SUBFOLDERS (1 << 1)
#define SHOW_ALL_SUBFOLDERS (1 << 2)
#define SHOW_HIDDEN (1 << 3)
#define SHOW_PATH_IN_TOOLTIP (1 << 4)

typedef struct {
  Clay_String hintText;
  char *textToEdit;
  bool isPassword;
  int maxLength;
  bool disable;
} TextEditData;

typedef struct {
  int image_list;
  int rendered_images;
} Indexes;

// Size of 'empty_buffer' is the maxLength of the string - 1 (space for null
// terminator).
extern char empty_buffer[];
extern TextEditData default_data;
extern bool editing_text;
extern Uint32 config_options;
extern Uint32 number_of_images;
extern char **files;
extern Uint32 selected_image;
extern char *folder_path;
extern Uint32 non_hidden_imgs;
extern char *cache_path;
extern Uint32 *rendered_to_list;

void InitializeCustomEvents();
void HandleTextEditInteraction(Clay_ElementId elementId,
                               Clay_PointerData pointer_data,
                               intptr_t userData);
void HandleExitButton(Clay_ElementId id, Clay_PointerData pointer_data);
void HandleClearButton(Clay_ElementId id, Clay_PointerData pointer_data);
void HandleOptionsButton(Clay_ElementId id, Clay_PointerData pointer_data,
                         int config);
void updateImg(int rendered_images, int image_list);
void HandleImgClick(Clay_ElementId id, Clay_PointerData pointer_data,
                    intptr_t index);
void HandleFolder(Clay_ElementId id, Clay_PointerData pointer_data);
char *jf_concat(size_t n, ...);
