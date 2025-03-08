# ZHLASMDisassembler

This project implements a Z High-Level Assembly language disassembler in C. It is capable of reading an assembly source file and generate the binary version of the instructions within it. Conversely, it can read a binary file and generate the source assembly language version of it. Additionally, it can generate a detailed output of each instruction processed, whether it is from assembling or disassembling the input instruction stream.

## Repository Structure

- `src`: contains all the source files and make recipes required to build the `zhlasmdis` executable.
- `testing`: contains a Python test program which generates a random instruction stream, then runs `zhlasmdis` to assemble
 and disassemble the original source file. Finally compares the disassembled stream against the original stream.

## Requirements and Dependencies

- Ubuntu 22.04 or later
- C17
- Make

## Build and Run

1. To build the binary, navigate to the `src` directory and issue the command `make`. This will generate the `zhlasmdis` binary.
2. Place the `zhlasmdis` binary anywhere you want, as long as the directory has the necessary (r/w/x) permissions.
3. To run the executable with the following parameters: `./zhlasmdis [flags] <input_file> <output_file>`:
    - `./zhlasmdis -a <source_assembler.hlasm> <output_binary.bin>`: Generates binary from assembler source file.
    - `./zhlasmdis -d <source_assembler.bin> <output_binary.hlasm>`: Generates assembler source from binary file.
    - `./zhlasmdis -dh <source_assembler.bin> <output_binary.hlasm>`: Generates assembler source from ASCII text file.
    - `./zhlasmdis -ap <source_assembler.hlasm> <output_binary.bin>` Generates binary from assembler source file and prints individual instructions in detail.
    - `./zhlasmdis -dp <source_assembler.bin> <output_binary.hlasm>`: Generates assembler source file from binary file and prints individual instructions in detail.
    - `./zhlasmdis -dhp <source_assembler.bin> <output_binary.hlasm>`: Generates assembler source file from ASCII hexadecimal text file and prints individual instructions in detail.

## Changelog v1.0

- Initial release.
