#include "gemm-bitserial.h"
#include <iostream>
#include <chrono>
using namespace std;

#include "sfc-data.h"
#include "sfc-w.h"

BitSerialMatrix l0_w, l1_w, l2_w, l3_w;
size_t l0_c = 784, l1_c = 256, l2_c = 256, l3_c = 256;

// naive LinearLayer implementation, int in, float params and out
vector<float> LinearLayer(const AccumulateVector & in, const vector<float> & mul, const vector<float> & add) {
  vector<float> ret;
  size_t scale_param_ind = 0;
  const size_t scale_param_wraparound = mul.size() - 1;
  for(size_t i = 0; i < in.size(); i++) {
    ret.push_back(in[i] * mul[scale_param_ind] + add[scale_param_ind]);
    scale_param_ind = (scale_param_ind == scale_param_wraparound) ? 0 : (scale_param_ind+1);
  }
  return ret;
}

vector<float> pipeline(const AccumulateVector & in) {
  ResultVector in_quantized = threshold(in, in_t);
  BitSerialVector in_bs = toBitSerialVector(in_quantized.data(), in_quantized.size(), 2);
  // layer 0 (first layer)
  ResultVector res0_quantized = bitSerialMatrixVectorThreshold(l0_w, in_bs, l0_t, l0_c, true, false);
  BitSerialVector res0_bs = toBitSerialVector(res0_quantized.data(), res0_quantized.size(), 2);
  // layer 1
  ResultVector res1_quantized = bitSerialMatrixVectorThreshold(l1_w, res0_bs, l1_t, l1_c, true, false);
  BitSerialVector res1_bs = toBitSerialVector(res1_quantized.data(), res1_quantized.size(), 2);
  // layer 2
  ResultVector res2_quantized = bitSerialMatrixVectorThreshold(l2_w, res1_bs, l2_t, l2_c, true, false);
  BitSerialVector res2_bs = toBitSerialVector(res2_quantized.data(), res2_quantized.size(), 2);
  // layer 3 (output layer)
  AccumulateVector res3 = bitSerialMatrixVector(l3_w, res2_bs, l3_c, true, false);
  vector<float> ret = LinearLayer(res3, l3_scale, l3_shift);
  return ret;
}

int main(int argc, char const *argv[]) {
  // prepare network parameters
  l0_w = toBitSerialMatrix((uint8_t*)w0, 256, 784, 2);
  l1_w = toBitSerialMatrix((uint8_t*)w1, 256, 256, 2);
  l2_w = toBitSerialMatrix((uint8_t*)w2, 256, 256, 2);
  l3_w = toBitSerialMatrix((uint8_t*)w3, 10, 256, 2);
  int reps = 100;
  vector<float> out;

  auto start = chrono::high_resolution_clock::now();
  for(unsigned int i = 0; i < reps; i++)
    out = pipeline(indata);
  auto end = chrono::high_resolution_clock::now();
  float uscount = chrono::duration_cast<std::chrono::microseconds>(end-start).count() / (float)reps;
  float fps = 1000000 / uscount;
  //float perf = 1000000 * (d*2 / uscount);
  cout << "Time for SFC inference: " << uscount << " microseconds" << endl;
  cout << "Frames per second: " << fps << endl;
  //cout << "Performance for and_cardinality: " << perf << " binary ops per second" << endl;

  for(size_t i = 0; i < out.size(); i++) {
    cout << i << " : " << out[i] << endl;
  }

  return 0;
}
