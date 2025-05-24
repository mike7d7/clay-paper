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

// Size of 'empty_buffer' is the maxLength of the string - 1 (space for null
// terminator).
extern char empty_buffer[];
extern TextEditData default_data;
extern bool editing_text;
extern uint32_t config_options;

void InitializeCustomEvents();
void HandleTextEditInteraction(Clay_ElementId elementId,
                               Clay_PointerData pointer_data, intptr_t userData);
void HandleExitButton(Clay_ElementId id, Clay_PointerData pointer_data);
void HandleClearButton(Clay_ElementId id, Clay_PointerData pointer_data);
void HandleOptionsButton(Clay_ElementId id, Clay_PointerData pointer_data, int config);
