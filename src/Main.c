#include <unistd.h>

#include "HLASMCompiler.h"

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("USAGE: ./compiler [INPUT_SOURCE_FILE]\n");
    }
    int ret;
    // Check if input file exists
    ret = access(argv[1], F_OK);
    if(ret != 0){
        printf("ERROR: Input source file \"%s\" cannot be found.\n", argv[1]);
        return -1;
    }
    InstructionStream_init();
    // TODO: error data when ret != 0
    ret = process_source_file(argv[1]);
    ret = InstructionStream_display();
    InstructionStream_free();
}