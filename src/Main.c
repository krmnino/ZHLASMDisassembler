#include "HLASMCompiler.h"

int main(int argc, char argv[]){
    InstructionStream_init();
    //process_source_file("../res/test.hlasm");
    process_source_file("../res/source.hlasm");
    int ret = InstructionStream_display();
    InstructionStream_free();
}