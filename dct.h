#ifndef _DCT_H_
#define _DCT_H_

// used for the part where H has to be generated dynamically
// add -lm to the Makefile to link against it
#include <math.h>

#define UDP_CLIENT_PORT 8000
#define UDP_SERVER_PORT 8001

#include "H_and_H_inv.h" // defines L
#include "test_signal.h" // defines N

#define M (L / 2) // compression ratio
#define UDP_MSGLEN 64 // bytes

#define COMPRESSION_RATIO = (L / M) // e.g. 8 / 4 = 2
#define COMPRESED_DATA_LENGTH  ((M * N) / L) // e.g. (4 * 512) / 8 = 512 / 2 = 256
#define COMPRESED_DATA_LENGTH_IN_BYTES COMPRESED_DATA_LENGTH * sizeof(float)

typedef float dct_block_t[L];
typedef float compressed_dct_block_t[M];

typedef float signal_t[N];

typedef float dct_comprssed_signal_t[COMPRESED_DATA_LENGTH];

/// Performs the calculation y = Mx
void matmul(const float M[L][L], const float x[L], float y[L]) {
	for (int i = 0; i < L; ++i) {
		float sum = 0.0f;
		for (int j = 0; j < L; ++j) {
			sum += M[i][j] * x[j];
		}
		y[i] = sum;
	}
}

/// Preallocated array filled by the procedure gen_H()
static float H_dynamic[L][L];

/// Generates the DCT H matrix of size (L x L) dynamically
/// The matrix is returned through a pointer (const float **)
/// That points to a static prealloacted array, so there is no need to deallocate on 
/// the caller site.
const float** gen_H() {
	for (int k = 0; k < L; ++k) {
		for (int n =0; n < L; ++n) {
			H_dynamic[k][n] = sqrt(( 2 - (k == 0 ? 1 : 0 )) / L) * cos((PI / L) * (n + 1 / 2) * k);
		}
	}

	return &H_dynamic;
}

#endif // _DCT_H_