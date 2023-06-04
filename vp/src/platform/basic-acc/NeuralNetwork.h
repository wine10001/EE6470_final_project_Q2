#ifndef NEURAL_NETWORK_H_
#define NEURAL_NETWORK_H_
#include <systemc>
#include <cmath>
#include <iomanip>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"
#include "weights.h"

struct NeuralNetwork : public sc_module {
  tlm_utils::simple_target_socket<NeuralNetwork> tsock;

  sc_fifo<unsigned char> i_pixel;
  sc_fifo<unsigned char> i_flag;
  sc_fifo<int> result_prediction;

  SC_HAS_PROCESS(NeuralNetwork);

  NeuralNetwork(sc_module_name n): 
    sc_module(n), 
    tsock("t_skt"), 
    base_offset(0) 
  {
    tsock.register_b_transport(this, &NeuralNetwork::blocking_transport);
    SC_THREAD(do_propagation);
  }

  ~NeuralNetwork() {
    //cout << "NeuralNetwork: " << endl;
	}


  int prediction;
	float image[INPUT_SIZE];
	float hidden[HIDDEN_SIZE];
	float output[OUTPUT_SIZE];
  unsigned int base_offset;

  void do_propagation(){
    { wait(CLOCK_PERIOD, SC_NS); }
    int cnt = 0;
    while (true) {
      unsigned char grey = i_pixel.read();
      unsigned char flag = i_flag.read();
      image[cnt] = (float)grey/255.0;


      if (flag){
        // Forward propagation input -> hidden
        for (unsigned int i=0; i<HIDDEN_SIZE; i++){
          hidden[i] = 0;
          for (unsigned int j=0; j<INPUT_SIZE; j++){
            hidden[i] += image[j]*weights1[j][i];
          }
          hidden[i] += bias1[i];
        }

        // ReLU
        for (unsigned int i=0; i<HIDDEN_SIZE; i++){
          if (hidden[i]<0){
            hidden[i] = 0;
          }
        }

        // Forward propagation hidden -> output
        for (unsigned int i=0; i<OUTPUT_SIZE; i++){
          output[i] = 0;
          for (unsigned int j=0; j<HIDDEN_SIZE; j++){
            output[i] += hidden[j]*weights2[j][i];
          }
          output[i] += bias2[i];
        }

        // Sorted and find the max
        float max = 0;
        int max_index = 0;
        for (unsigned int i=0; i<OUTPUT_SIZE; i++){
          if (output[i]>max){
            max = output[i];
            max_index = i;
          }
        }
        prediction = max_index;
      } else {
        prediction = 10;
      }

      cnt++;
      result_prediction.write(prediction);
    }
  }

  void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay){
    wait(delay);
    // unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    // auto len = payload.get_data_length();
    tlm::tlm_command cmd = payload.get_command();
    sc_dt::uint64 addr = payload.get_address();
    unsigned char *data_ptr = payload.get_data_ptr();

    addr -= base_offset;


    // cout << (int)data_ptr[0] << endl;
    // cout << (int)data_ptr[1] << endl;
    // cout << (int)data_ptr[2] << endl;
    word buffer;

    switch (cmd) {
      case tlm::TLM_READ_COMMAND:
        // cout << "READ" << endl;
        switch (addr) {
          case NEURAL_NETWORK_RESULT_ADDR:
            buffer.uc[0] = 0;
            buffer.uc[1] = 0;
            buffer.uc[2] = 0;
            buffer.uc[3] = result_prediction.read();
            break;
          default:
            std::cerr << "READ Error! NeuralNetwork::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
          }
        data_ptr[0] = buffer.uc[0];
        data_ptr[1] = buffer.uc[1];
        data_ptr[2] = buffer.uc[2];
        data_ptr[3] = buffer.uc[3];
        break;
      case tlm::TLM_WRITE_COMMAND:
        // cout << "WRITE" << endl;
        switch (addr) {
          case NEURAL_NETWORK_R_ADDR:
            i_pixel.write(data_ptr[0]);
            i_flag.write(data_ptr[3]);
            break;
          default:
            std::cerr << "WRITE Error! NeuralNetwork::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
        }
        break;
      case tlm::TLM_IGNORE_COMMAND:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
      default:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
      }
      payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
  }
};
#endif
