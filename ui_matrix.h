#ifndef ui_matrix_h
#define ui_matrix_h

void UI_Display_Matrix(void);
void UI_Handle_Matrix(void);

typedef struct {
  unsigned char Source;
  unsigned char Destination;
  unsigned char Amount;
} mmodmap_t;

extern const mmodmap_t EB_MmodMap[10];

#endif

