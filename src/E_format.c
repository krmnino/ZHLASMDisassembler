#include "InstructionTable.h"
#include "HLASMCompiler.h"

ErrorCode assemble_E(Context* c, size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode; 
    // Opcode: bits(0-7)
    bin_buffer[0] = opcode >> 8;
    // Opcode: bits(8-16)
    bin_buffer[1] = opcode & 0x00FF;
    return OK;
}

ErrorCode display_E(Context* c, Instruction* instr){
    if(instr == NULL){
        c->error_code = NULL_POINTER_TO_OBJECT;
        return c->error_code;
    }
    uint16_t opcode = INSTRUCTION_TABLE[instr->it_index].opcode;
    uint8_t length = INSTRUCTION_TABLE[instr->it_index].length;
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    printf("+----------------+\n");
    printf("|     OPCODE     |\n");
    printf("+----------------+\n");
    printf("0                F\n");
    // Print general information
    printf("MNEMONIC: %s\n", INSTRUCTION_TABLE[instr->it_index].mnemonic);
    hex_str_2_char_str((void*)&opcode, sizeof(opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", length);
    printf("FORMAT:   E\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    return OK;
}

ErrorCode decode_E(){
    return OK;
}
