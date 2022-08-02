# Decoder

## Structure

- `decode.py` contains main logic for decoding.
- `codes.bin` contains codes that huffman symbols are written in in raw encoded file.
- `tables.py` contains tables for code to symbol translations and quantization.
- `test` directory contains unit test suite for `BitStream` implemented in `decode.py`

## Calling

- This code is intended to be called as a module from parent directory (see parent directory readme for more details).
