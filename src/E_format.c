#include "HLASMCompiler.h"

int build_E(uint16_t opcode, char* operands_token, uint8_t* bin_buffer, InstructionFormat format){
     // Opcode: bits(0-7)
    bin_buffer[0] = opcode >> 8;
     // Opcode: bits(8-16)
    bin_buffer[1] = opcode & 0x00FF;
    return 0;
}

int display_E(Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    uint16_t ret_opcode = mnemonic_to_opcode(instr->mnemonic);
    uint8_t ret_length = mnemonic_to_length(instr->mnemonic);
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    printf("+----------------+\n");
    printf("|     OPCODE     |\n");
    printf("+----------------+\n");
    printf("0                F\n");
    // Print general information
    printf("MNEMONIC: %s\n", instr->mnemonic);
    hex_str_2_char_str((void*)&ret_opcode, sizeof(ret_opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", ret_length);
    printf("FORMAT:   E\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    return 0;
}