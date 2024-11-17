#include "HLASMCompiler.h"

int main(int argc, char argv[]){
    InstructionStream_init();
    process_source_file("../res/source.hlasm");
    InstructionStream_free();
}