import sys
from math import cos, pi, sqrt

import numpy as np

p = lambda *args, **kvargs: print(*args, **kvargs)


def gen_dct_matrix(L: int):
    delta = lambda k: 1 if k == 0 else 0
    H = np.zeros((L, L))
    for k in range(L):
        for n in range(L):
            H[k, n] = sqrt((2 - delta(k)) / L) * cos((pi / L) * (n + 1 / 2) * k)
    return H


def gen_inverse_dct_matrix(L: int):
    return np.linalg.inv(gen_dct_matrix(L))


def fmt_np_array_as_C_array_literal(arr: np.ndarray) -> str:
    row, _ = arr.shape
    fmt_row = (
        lambda row_idx: "\t{"
        + ",".join(
            [
                *[f" {x:.3f}f" for x in arr[row_idx, :-1]],
            ]
        )
        + f", {arr[row_idx, -1]:.3f}f"
        + f" }}{'' if  row_idx == row -1 else ','}"
    )

    return "\n".join(["{", *[fmt_row(row_idx) for row_idx in range(row)], "};"])


if __name__ == "__main__":
    argc = len(sys.argv)
    L = 8 if argc == 1 else sys.argv[1]
    L = int(L)

    delta = lambda k: 1 if k == 0 else 0
    H = np.zeros((L, L))
    for k in range(L):
        for n in range(L):
            H[k, n] = sqrt((2 - delta(k)) / L) * cos((pi / L) * (n + 1 / 2) * k)

    H = gen_dct_matrix(L)

    p(f"#define L {L}\n")
    print(f"static const float H[{L}][{L}] = ", end="")
    H_as_C_array_literal = fmt_np_array_as_C_array_literal(H)
    print(H_as_C_array_literal)

    print(f"static const float H_inv[{L}][{L}] = ", end="")
    H_inv = gen_inverse_dct_matrix(L)
    H_inv_as_C_array_literal = fmt_np_array_as_C_array_literal(H_inv)
    print(H_inv_as_C_array_literal)
