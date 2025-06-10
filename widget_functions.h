#include "clay.h"
#include <stdint.h>

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
extern int number_of_images;
extern char **files;
extern uint_fast32_t selected_image;

void InitializeCustomEvents();
void HandleTextEditInteraction(Clay_ElementId elementId,
                               Clay_PointerData pointer_data, intptr_t userData);
void HandleExitButton(Clay_ElementId id, Clay_PointerData pointer_data);
void HandleClearButton(Clay_ElementId id, Clay_PointerData pointer_data);
void HandleOptionsButton(Clay_ElementId id, Clay_PointerData pointer_data, int config);
void HandleImgClick(Clay_ElementId id, Clay_PointerData pointer_data, intptr_t index);
