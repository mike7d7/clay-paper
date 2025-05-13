#include "clay.h"

typedef struct {
  Clay_String hintText;
  char *textToEdit;
  bool isPassword;
  int maxLength;
  bool disable;
} TextEditData;

void HandleTextEditInteraction(Clay_ElementId elementId,
                               Clay_PointerData pointerInfo, intptr_t userData);
