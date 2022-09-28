import sys
from math import cos, pi, sqrt

import numpy as np

p = lambda *args: print(*args)


def fmt_np_array_as_C_array_literal(arr: np.ndarray) -> str:
    row, col = arr.shape
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

    p(f"#define L {L}\n")
    print(f"static const float H[{L}][{L}] = ", end="")
    H_as_C_array_literal = fmt_np_array_as_C_array_literal(H)
    print(H_as_C_array_literal)
    p()

    print(f"static const float H_inv[{L}][{L}] = ", end="")
    H_inv = np.linalg.inv(H)
    H_inv_as_C_array_literal = fmt_np_array_as_C_array_literal(H_inv)
    print(H_inv_as_C_array_literal)
    y = H @ np.array([1, 2, 3, 4, 5, 6, 7, 8]).T
    print(y)


