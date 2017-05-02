#pragma once
#include "bitvector.h"
typedef MyBitVector BitVector;
typedef std::vector<BitVector> BitSerialVector;
typedef std::vector<BitSerialVector> BitSerialMatrix;
typedef int32_t AccumulateElem;
typedef std::vector<AccumulateElem> AccumulateVector;
typedef std::vector<AccumulateVector> AccumulateMatrix;
typedef uint8_t ResultElem;
typedef std::vector<ResultElem> ResultVector;
typedef std::vector<AccumulateVector> ThresholdMatrix;

/**
* Convert a buffer of unsigned char values into a gemm-bitserial vector
*/
BitSerialVector toBitSerialVector(const uint8_t * vec, const size_t n, const size_t bits);

/**
* Convert a gemm-bitserial vector into a buffer of unsigned char values
*/
void fromBitSerialVector(const BitSerialVector & vec, uint8_t * ret);

/**
* Convert a buffer of unsigned char values into a gemm-bitserial matrix
*/
BitSerialMatrix toBitSerialMatrix(const uint8_t * mat, const size_t rows, const size_t cols, size_t bits);

/**
* Convert a buffer of unsigned char values into a gemm-bitserial matrix
*/
void fromBitSerialMatrix(const BitSerialMatrix & mat, size_t bits, uint8_t * ret);
