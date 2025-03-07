#include <unistd.h>

#include "HLASMDisassembler.h"

#define OPTS_MAX_LEN 4
#define ASSEMBLE_OPT 0x1
#define DISASSEMBLE_OPT 0x2
#define PRINT_OPT 0x4

void print_usage(){
    printf("USAGE: ./zhlasmdis [-OPTIONS] [INPUT_FILE] [OUTPUT_FILE]\n");
    printf("OPTIONS:\n");
    printf("d: disassemble binary into source file\n");
    printf("a: assemble binary from source file\n");
    printf("p: optional - generate detailed print out from source/binary file\n");
}

int main(int argc, char* argv[]){
    if(argc != 4){
        print_usage();
        return -1;
    }
    int ret;
    ErrorCode ret_err;
    uint32_t options = 0;
    Context_init();
    // Make sure options start with a dash
    if(argv[1][0] != '-'){
        print_usage();
        return -1;
    }
    // Loop though the options
    for(size_t i = 1; i < OPTS_MAX_LEN; i++){
        if(argv[1][i] == 0){
            break;
        }
        switch (argv[1][i]){
        case 'a':
            options = options | ASSEMBLE_OPT;
            break;        
        case 'd':
            options = options | DISASSEMBLE_OPT;
            break;        
        case 'p':
            options = options | PRINT_OPT;
            break;        
        default:
            print_usage();
            return -1;
        }
    }
    // Either assemble or disassemble, not both
    if((options & ASSEMBLE_OPT) + ((options & DISASSEMBLE_OPT) >> 1) != 1){
        print_usage();
        return -1;
    }
    // Check if input file exists
    ret = access(argv[2], F_OK);
    if(ret != 0){
        Context.error_code = SRC_FILE_NOT_FOUND;
        strcpy((char*)&Context.msg_extras[0], argv[2]);
        display_error();
        return -1;
    }
    // Check if output file is writable
    ret = access(argv[3], F_OK);
    if(ret == 0){
        ret = access(argv[3], W_OK);
        if(ret != 0){
            Context.error_code = OUT_FILE_NOT_WRITABLE;
            strcpy((char*)&Context.msg_extras[0], argv[3]);
            display_error();
            return -1;
        }
    }
    // Assemble or disassemble
    if((options & ASSEMBLE_OPT) == 1){
        ret_err = assemble(argv[2], argv[3]);
        if(ret_err != OK){
            display_error();
            return -1;
        }
    }
    else if((options & DISASSEMBLE_OPT) >> 1 == 1){
        ret_err = disassemble(argv[2], argv[3]);
        if(ret_err != OK){
            display_error();
            return -1;
        }
    }
    // If print option selected, do it
    if((options & PRINT_OPT) >> 2 == 1){
        ret_err = display_stream();
        if(ret_err != OK){
            display_error();
            return -1;
        }
    }
    Context_free();
    return 0;
}