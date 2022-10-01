#!/usr/bin/env python

import re
import sys
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np

from idct import read_dat_file

p = lambda *args, **kvargs: print(*args, **kvargs)

if __name__ == "__main__":
    argv = sys.argv
    argc = len(argv)

    if argc != 2:
        print(
            """
usage: ./script.py <directory_of_experiments_folder: str>
        """,
            file=sys.stderr,
        )
        sys.exit(1)

    experiments_dir = Path(sys.argv[1])

    for d in [d for d in experiments_dir.iterdir() if d.is_dir()]:
        test_signal = d / "test_signal.dat"
        compressed_data = d / "compressed_signal.dat"
        decompressed_data = d / "decompressed_signal.dat"
        p(test_signal, compressed_data, decompressed_data)
        if not (
            test_signal.exists()
            and compressed_data.exists()
            and decompressed_data.exists()
        ):
            continue

        m = re.match(r"N([0-9]+)-L([0-9]+)-M([0-9]+)", str(d).split("/")[1])

        if not m:
            p(f"""{str(d)} does NOT match expected dir name.""", file=sys.stderr)
            sys.exit(1)

        N, L, M = m.groups()

        test_signal = read_dat_file(test_signal)
        decompressed_data = read_dat_file(decompressed_data)
        compressed_data = read_dat_file(compressed_data)
        n = list(range(len(test_signal)))

        fig, ax = plt.subplots()

        ax.plot(n, test_signal, "b")
        ax.plot(n, decompressed_data, "r+")

        tae = sum([abs(x - y) for x, y in zip(test_signal, decompressed_data)])
        mtae = sum([abs(x - y) for x, y in zip(test_signal, decompressed_data)]) / len(
            test_signal
        )
        test_signal = np.array(test_signal)
        decompressed_data = np.array(decompressed_data)
        mse = (np.square(test_signal - decompressed_data)).mean(axis=None)

        ax.set_title(
            f"N = {N}, L = {L}, M = {M}, MSE = {mse:.2f}"
        )
        ax.grid(True)
        ax.legend(["signal", "decompressed signal"], loc="lower right")
        ax.set_xlabel("samples")
        ax.set_ylabel("N/A")
        plt.savefig(d / "plot.png")
        plt.savefig(d / "plot.svg")
        ax.clear()
