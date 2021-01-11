# Copyright (c) 2021, Manfred Moitzi
# License: MIT License
from pathlib import Path

CPP_PATH = Path("../../src").resolve()
INCLUDE_PATH = Path("../../include/ezdxf").resolve()

if __name__ == "__main__":
    print("Code generation configuration:")
    print("==============================")
    print(f"Store C++ source files at: {CPP_PATH}")
    print(f"Store Include files at: {INCLUDE_PATH}")
