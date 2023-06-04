#ifndef FILTER_DEF_H_
#define FILTER_DEF_H_

#define CLOCK_PERIOD 10


#define MAX_IMAGE_BUFFER_LENTH 1024
#define THRESHOLD 90

// DOUBLE mask parameters
const int MASK_N = 2;
const int MASK_X = 3;
const int MASK_Y = 3;
const int INPUT_SIZE = 784;
const int HIDDEN_SIZE = 64;
const int OUTPUT_SIZE = 10;
const int DMA_TRANS = 64;


// DOUBLE Filter inner transport addresses
// Used between blocking_transport() & do_filter()
const int DOUBLE_FILTER_R_ADDR = 0x00000000;
const int DOUBLE_FILTER_RESULT_ADDR = 0x00000004;

const int DOUBLE_FILTER_RS_R_ADDR   = 0x00000000;
const int DOUBLE_FILTER_RS_W_WIDTH  = 0x00000004;
const int DOUBLE_FILTER_RS_W_HEIGHT = 0x00000008;
const int DOUBLE_FILTER_RS_W_DATA   = 0x0000000C;
const int DOUBLE_FILTER_RS_RESULT_ADDR = 0x00800000;


union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};

// DOUBLE mask
const int mask[MASK_N][MASK_X][MASK_Y] = {{{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}},

                                          {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}}};
#endif
