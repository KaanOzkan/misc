#include <immintrin.h>
#include <chrono>
#include <iostream>
#include <xmmintrin.h>

#define ARRAY_SIZE 1024
#define ITERATIONS 100'000

void aligned_square(float *data, int size) {
  for (int i = 0; i < size; i += 4) {
    // load 4 floats at once into SSE register
    __m128 vec = _mm_load_ps(&data[i]); // Requires 16 byte allignment
    __m128 result = _mm_mul_ps(vec, vec); // Multipy each element
    _mm_store_ps(&data[i], result); // Store result back to data
  }
}

float misaligned_data[ARRAY_SIZE] __attribute__((aligned(4)));
void unaligned_square(float *data, int size) {
  for (int i = 0; i < size; i += 4) {
    // load 4 floats at once into SSE register
    __m128 vec = _mm_loadu_ps(&data[i]); // !! Unaligned load
    __m128 result = _mm_mul_ps(vec, vec); // Multipy each element
    _mm_storeu_ps(&data[i], result); // Store result back to data
  }
}

void scalar_square(float *data, int size) {
  for (int i = 0; i < size; ++i) {
    data[i] = data[i] * data[i];
  }
}

int main (int argc, char *argv[]) {
  float aligned_data[ARRAY_SIZE] __attribute__((aligned(16)));

  auto t1 = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < ITERATIONS; ++i) {
    aligned_square(aligned_data, ARRAY_SIZE);
  }
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> ms = (t2 - t1);
  std::cout << "Aligned simd: " << ms.count() << "ms\n";

  auto t3 = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < ITERATIONS; ++i) {
    unaligned_square(misaligned_data, ARRAY_SIZE);
  }
  auto t4 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> ms2 = (t4 - t3);
  std::cout << "unaligned simd: " << ms2.count() << "ms\n";

  auto t5 = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < ITERATIONS; ++i) {
    scalar_square(aligned_data, ARRAY_SIZE);
  }
  auto t6 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> ms3 = (t6 - t5);
  std::cout << "scalar square: " << ms3.count() << "ms\n";
}
