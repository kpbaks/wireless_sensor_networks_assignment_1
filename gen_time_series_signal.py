import sys
from math import cos, pi, sqrt
from pprint import pp

import numpy as np

p = lambda *args, **kvargs: print(*args, **kvargs)


def fmt_nparray_as_C_array_literal(arr: np.ndarray) -> str:
    arr = arr.reshape(-1, 32)
    arr = ["".join([",".join(f"{x:.3f}f" for x in row)]) + "," for row in arr]
    return "{ " + "\n".join(arr) + " }"


if __name__ == "__main__":
    argc = len(sys.argv)
    N = 512 if argc == 1 else sys.argv[1]
    N = int(N)

    noise = np.random.normal(0, 1, N)
    signal = np.cos(np.linspace(0, 10, N)) + noise

    p(f"#define N {N}\n")
    p(f"static const float signal[{N}] = ", end="")
    p(fmt_nparray_as_C_array_literal(signal) + ";")
