#include <unistd.h>

#include "HLASMCompiler.h"

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("USAGE: ./compiler [INPUT_SOURCE_FILE]\n");
        return -1;
    }
    int ret;
    Context* c = &context;
    ErrorCode ret_err;
    Context_init(c);
    // Check if input file exists
    ret = access(argv[1], F_OK);
    if(ret != 0){
        c->error_code = SRC_FILE_NOT_FOUND;
        strcpy((char*)&c->msg_extras[0], argv[1]);
        display_error(c);
        printf("ERROR: Input source file \"%s\" cannot be found.\n", argv[1]);
        return -1;
    }
    ret_err = process_source_file(c, argv[1]);
    if(ret_err != OK){
        display_error(c);
        return -1;
    }
    ret_err = display_stream(c);
    if(ret_err != OK){
        display_error(c);
        return -1;
    }
    Context_free(c);
    return 0;
}