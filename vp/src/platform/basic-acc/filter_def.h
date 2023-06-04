#ifndef FILTER_DEF_H_
#define FILTER_DEF_H_

#define CLOCK_PERIOD 10


#define MAX_IMAGE_BUFFER_LENTH 1024

// DOUBLE mask parameters
const int INPUT_SIZE = 784;
const int HIDDEN_SIZE = 64;
const int OUTPUT_SIZE = 10;
const int DMA_TRANS = 64;


// DOUBLE Filter inner transport addresses
// Used between blocking_transport() & do_filter()
const int NEURAL_NETWORK_R_ADDR = 0x00000000;
const int NEURAL_NETWORK_RESULT_ADDR = 0x00000004;

const int NEURAL_NETWORK_RS_R_ADDR   = 0x00000000;
const int NEURAL_NETWORK_RS_W_WIDTH  = 0x00000004;
const int NEURAL_NETWORK_RS_W_HEIGHT = 0x00000008;
const int NEURAL_NETWORK_RS_W_DATA   = 0x0000000C;
const int NEURAL_NETWORK_RS_RESULT_ADDR = 0x00800000;


union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};

#endif
