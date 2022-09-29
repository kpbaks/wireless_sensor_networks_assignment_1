#!/usr/bin/env python

import sys
from pprint import pp

import numpy as np

from gen_H import gen_inverse_dct_matrix
from gen_time_series_signal import gen_test_signal

p = lambda *args, **kvargs: print(*args, **kvargs)

def write_dat_file(filename, xs):
    with open(filename, "w") as f:
        for x in xs:
            f.write(f"{x:.3}\n")

def read_dat_file(filename):
    with open(filename, "r") as f:
        return [float(line) for line in f.readlines()]

def idct(L, M, compressed_data):
    decompressed_data = []

    compressed_data = np.array(compressed_data)

    N_ = len(compressed_data)
    assert N_ % L == 0

    H_inv = gen_inverse_dct_matrix(L)

    for i in range(0,N_, M):
        yi = np.zeros(L)
        yi[:M] = compressed_data[i:(i + M)]
        xi = H_inv @ yi.T
        decompressed_data = decompressed_data + list(xi)

    return decompressed_data

    # # pp({"compressed_data": compressed_data})
    # p("compressed_data length =", len(compressed_data))

    # N = len(compressed_data)

    # zeros = np.zeros((M, N // M))
    # # zeros[:M, :] = compressed_data.reshape(M, N // M)
    # compressed_data = compressed_data.reshape(M, N // M)
    # Y = np.vstack((compressed_data, zeros))
    # # Y = zeros
    # p("Y[:,0]", Y[:, 0])

    # pp({"shape(Y)": np.shape(Y)})
    # X = H_inv @ Y
    # return X.flatten()

    # for i in range((len(compressed_data) // M) + 1):
    #     p(i)
    #     # for block in np.split(compressed_data, M):
    #     block = compressed_data[i * M : (i + 1) * M]
    #     p("len(block) =", len(block))
    #     # print(block)
    #     M_zeros = np.zeros(M)
    #     block_padded_with_zeros = np.array(list(block) + list(M_zeros))
    #     xi = H_inv @ block_padded_with_zeros
    #     # pp({"len(xi)": block_padded_with_zeros, "xi": xi})
    #     decompressed_data = decompressed_data + list(xi)

    # return decompressed_data

if __name__ == "__main__":
    argv = sys.argv
    argc = len(argv)

    if argc != 6:
        print(
            """
usage: ./script.py <N: int> <L: int> <M: int> <compressed_data_dat_file: filepath> <decompressed_data_dat_file: filepath>
        """,
            file=sys.stderr,
        )
        sys.exit(1)

    N = int(sys.argv[1])
    L = int(sys.argv[2])
    M = int(sys.argv[3])
    compressed_data_dat_file = sys.argv[4]
    decompressed_data_dat_file = sys.argv[5]

    compressed_data = read_dat_file(compressed_data_dat_file)
    decompressed_data = idct(L, M, compressed_data)

    p('wrote decompressed data to ' + decompressed_data_dat_file)
    write_dat_file(decompressed_data_dat_file, list(decompressed_data))
