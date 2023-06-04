#include "string"
#include "string.h"
#include "cassert"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "patterns.h"
#include "math.h"

#define PROCESSORS 2
#define DMA_BANDWIDTH 4
#define INITIAL_CYCLES 0

union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};


// Neral Network ACC
static char* const NEURALNETWORK_START_ADDR[PROCESSORS] = {
    reinterpret_cast<char* const>(0x73000000),
    reinterpret_cast<char* const>(0x74000000)
};
static char* const NEURALNETWORK_READ_ADDR[PROCESSORS] = {
    reinterpret_cast<char* const>(0x73000004),
    reinterpret_cast<char* const>(0x74000004)
};


// DMA 
static volatile uint32_t * const DMA_SRC_ADDR  = (uint32_t * const)0x70000000;
static volatile uint32_t * const DMA_DST_ADDR  = (uint32_t * const)0x70000004;
static volatile uint32_t * const DMA_LEN_ADDR  = (uint32_t * const)0x70000008;
static volatile uint32_t * const DMA_OP_ADDR   = (uint32_t * const)0x7000000C;
static volatile uint32_t * const DMA_STAT_ADDR = (uint32_t * const)0x70000010;
static const uint32_t DMA_OP_MEMCPY = 1;

#define PROCESSORS 2
#define DMA_BANDWIDTH 4
#define INITIAL_CYCLES 0
bool _is_using_dma = true;
int write_cycles = 0;
int read_cycles = 0;


void write_data_to_ACC(char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){  
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(buffer);
    *DMA_DST_ADDR = (uint32_t)(ADDR);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
    write_cycles += INITIAL_CYCLES + len / DMA_BANDWIDTH;
  }else{
    // Directly Send
    memcpy(ADDR, buffer, sizeof(unsigned char)*len);
  }
}
void read_data_from_ACC(char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(ADDR);
    *DMA_DST_ADDR = (uint32_t)(buffer);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
    read_cycles += INITIAL_CYCLES + len / DMA_BANDWIDTH;
  }else{
    // Directly Read
    memcpy(buffer, ADDR, sizeof(unsigned char)*len);
  }
}


int sem_init (uint32_t *__sem, uint32_t count) __THROW
{
  *__sem=count;
  return 0;
}

int sem_wait (uint32_t *__sem) __THROW
{
  uint32_t value, success; //RV32A
  __asm__ __volatile__("\
L%=:\n\t\
     lr.w %[value],(%[__sem])            # load reserved\n\t\
     beqz %[value],L%=                   # if zero, try again\n\t\
     addi %[value],%[value],-1           # value --\n\t\
     sc.w %[success],%[value],(%[__sem]) # store conditionally\n\t\
     bnez %[success], L%=                # if the store failed, try again\n\t\
"
    : [value] "=r"(value), [success]"=r"(success)
    : [__sem] "r"(__sem)
    : "memory");
  return 0;
}

int sem_post (uint32_t *__sem) __THROW
{
  uint32_t value, success; //RV32A
  __asm__ __volatile__("\
L%=:\n\t\
     lr.w %[value],(%[__sem])            # load reserved\n\t\
     addi %[value],%[value], 1           # value ++\n\t\
     sc.w %[success],%[value],(%[__sem]) # store conditionally\n\t\
     bnez %[success], L%=                # if the store failed, try again\n\t\
"
    : [value] "=r"(value), [success]"=r"(success)
    : [__sem] "r"(__sem)
    : "memory");
  return 0;
}

int barrier(uint32_t *__sem, uint32_t *__lock, uint32_t *counter, uint32_t thread_count) {
	sem_wait(__lock);
	if (*counter == thread_count - 1) { //all finished
		*counter = 0;
		sem_post(__lock);
		for (int j = 0; j < thread_count - 1; ++j) sem_post(__sem);
	} else {
		(*counter)++;
		sem_post(__lock);
		sem_wait(__sem);
	}
	return 0;
}


#define PROCESSORS 2
uint32_t barrier_counter=0; 
uint32_t barrier_lock; 
uint32_t barrier_sem; 
uint32_t lock;  
uint32_t print_sem[PROCESSORS]; 



int main(unsigned hart_id) {
  /////////////////////////////
	// thread and barrier init //
	/////////////////////////////
	
	if (hart_id == 1) {
		// create a barrier object with a count of PROCESSORS
		sem_init(&barrier_lock, 1);
		sem_init(&barrier_sem, 0); //lock all cores initially
		for(int i=0; i< PROCESSORS; ++i){
			sem_init(&print_sem[i], 0); //lock printing initially
		}
		// Create mutex lock
		sem_init(&lock, 1);
	}



  unsigned char  buffer[4] = {0};
  unsigned char flag = 0;
  word data;
  int result;
  int prediction;
  //printf("Start processing...(%d, %d)\n", width, height);

  unsigned char image[INPUT_SIZE] = {0};

  if (hart_id == 0){
    for(int i = 0; i < INPUT_SIZE; i++){
      image[i] = (unsigned char)img_4[i];  // Modify here to change input image
    }
  } else {
    for(int i = 0; i < INPUT_SIZE; i++){
      image[i] = (unsigned char)img_7[i];  // Modify here to change input image
    }
  }

  sem_wait(&lock);
  for(int i = 0; i < INPUT_SIZE; i++){
    flag = (i==INPUT_SIZE-1) ? 1 : 0;
    buffer[0] = image[i];
    buffer[1] = 0;
    buffer[2] = 0;
    buffer[3] = flag;
    write_data_to_ACC(NEURALNETWORK_START_ADDR[hart_id], buffer, 4);
    read_data_from_ACC(NEURALNETWORK_READ_ADDR[hart_id], buffer, 4);
    memcpy(data.uc, buffer, 4);
    prediction = data.uc[3];
  }
  printf("core%d, predicted: = %d\n", hart_id, prediction);
  sem_post(&lock);  // Release the lock
  barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);

  if (hart_id == 0) {
    printf("Total Write Cycles: %d\n", write_cycles);
    printf("Total Read Cycles: %d\n", read_cycles);
    printf("Total DMA Cycles: %d\n", write_cycles + read_cycles);
    printf("Total Cycles (DMA + PE): %d\n", write_cycles + read_cycles + INPUT_SIZE);
  }

	return 0;
}
