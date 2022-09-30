import sys
from math import cos, pi, sqrt

import numpy as np

p = lambda *args, **kvargs: print(*args, **kvargs)


def gen_test_signal(N: int) -> np.ndarray:
    noise = np.random.normal(0, 0.1, N)
    signal = (
        5 * np.cos(np.linspace(0, 10, N) * 2)
        + np.cos(np.linspace(0, 10, N) * 8 - np.pi / 2)
        + noise
    )
    return signal


def fmt_nparray_as_C_array_literal(arr: np.ndarray) -> str:
    arr = arr.reshape(-1, 32)
    arr = ["".join([",".join(f"{x:.3f}f" for x in row)]) + "," for row in arr]
    return "{ " + "\n".join(arr) + " }"


if __name__ == "__main__":
    argc = len(sys.argv)
    if argc != 3:
        print(
            """
usage: ./script.py <N: int> <c|dat: str>
        """,
            file=sys.stderr,
        )
        sys.exit(1)

    N = int(sys.argv[1])

    signal = gen_test_signal(N)

    if sys.argv[2] == "c":
        p(f"#define SIGNAL_LENGTH {N}\n")
        p(f"static const float test_signal[{N}] = ", end="")
        p(fmt_nparray_as_C_array_literal(signal) + ";")
    elif sys.argv[2] == "dat":
        for x in signal:
            p(f"{x:.5f}")
