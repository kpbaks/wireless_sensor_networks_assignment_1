#ifndef _DCT_H_
#define _DCT_H_

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

// used for the part where H has to be generated dynamically
// add -lm to the Makefile to link against it
// #include <math.h>

#include "H_and_H_inv.h" // defines L
#include "test_signal.h" // defines N

#define M 4 // compression ratio

// #define COMPRESSION_RATIO = (L / M) // e.g. 8 / 4 = 2
#define COMPRESSION_RATIO 2 // e.g. 8 / 4 = 2
// #define COMPRESED_DATA_LENGTH  ((M * N) / L) // e.g. (4 * 512) / 8 = 512 / 2
// = 256
#define COMPRESED_DATA_LENGTH 256 // e.g. (4 * 512) / 8 = 512 / 2 = 256
#define COMPRESED_DATA_LENGTH_IN_BYTES COMPRESED_DATA_LENGTH * sizeof(float)

// typedef float dct_block_t[L];
typedef float dct_block_t[8];
// typedef float compressed_dct_block_t[M];
typedef float compressed_dct_block_t[4];

// typedef float signal_t[N];
typedef float signal_t[512];
// typedef float dct_comprssed_signal_t[COMPRESED_DATA_LENGTH];
typedef float dct_comprssed_signal_t[256];

/// Performs the calculation y = Mx
// void matmul{const float M[L][L], const float x[L], float y[L]} {
void matmul(const float arr[8][8], const float x[8], float y[8]) {
	for (int i = 0; i < L; ++i) {
		float sum = 0.0f;
		for (int j = 0; j < L; ++j) {
			sum += arr[i][j] * x[j];
		}
		y[i] = sum;
	}
}

static dct_comprssed_signal_t compressed_signal;

void dct(const signal_t signal) {
	dct_block_t xi;
	dct_block_t yi;
	compressed_dct_block_t yM;

	const uint8_t chunk_size_in_bytes = L * sizeof(float);
	const uint8_t compressed_chunk_size_in_bytes = M * sizeof(float);

	// iterate over each chunck xi of size L of the signal
	// and transform it.
	for (int i = 0; i < SIGNAL_LENGTH; i += L) {
		// xi = signal[i:(i + L)]
		memcpy(xi, signal + i, chunk_size_in_bytes);
		// yi = H * xi
		matmul(H, xi, yi);
		// yM = yi[:M] i.e. select the first M coefficients
		memcpy(yM, yi, compressed_chunk_size_in_bytes);
		// y[M * (i / L):(M * (i / L) + M)] = yM
		int offset = M * (i / L); // 0, M, 2M, 3M ...
		memcpy(compressed_signal + offset, yM, compressed_chunk_size_in_bytes);
	}
}

static signal_t decompressed_signal;

/// Apply Inverse Discrete Cosine Transform to a DCT signal
/// The resulting decompressed signal is returned as a pointer to a static
/// buffer 'decompressed_signal'
void idct(dct_comprssed_signal_t compressed_signal) {
	dct_block_t xi;
	dct_block_t yi;

	const uint8_t chunk_size_in_bytes = L * sizeof(float);

	// iterate over each chunck xi of size M of the compressed_signal
	// and transform it.
	for (int i = 0; i < COMPRESED_DATA_LENGTH; i += M) {
		// yi[:M] = compressed_signal[i:(i + M)]
		memcpy(yi, compressed_signal + i, chunk_size_in_bytes);
		// pad yi[M:] with zeros
		for (int j = M; j < L; ++j) {
			yi[j] = 0.0f;
		}

		// xi = H_inv * yi
		matmul(H_inv, yi, xi);
		printf("xi = ");
		for (int j = 0; j < L; ++j) {
			printf("%f ", xi[j]);

		}
		printf("\n");
		printf("yi = ");
		for (int j = 0; j < L; ++j) {
			printf("%f ", yi[j]);

		}
		printf("\n");
		// copy xi chunk into the decompressed_signal buffer
		int offset = COMPRESSION_RATIO * i;
		printf("offset = %d\n", offset);
		memcpy(decompressed_signal + offset, xi, chunk_size_in_bytes);
	}
}

#endif // _DCT_H_
