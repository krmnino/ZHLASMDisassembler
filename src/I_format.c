#include "HLASMCompiler.h"

int build_I(uint16_t opcode, char* operands_token, uint8_t* bin_buffer, InstructionFormat format){
    uint8_t i; 
    if(!is_valid_hex_string(operands_token, 1)){
        return -1;
    }
    char_str_2_hex_str(operands_token, MAX_OPERANDS_LEN, (void*)&i, sizeof(i), 2, NO_SKIP, true);
     // Opcode: bits(0-7)
    bin_buffer[0] = opcode;
    // I: bits(8-15)
    bin_buffer[1] = i;
    return 0;
}

int display_I(Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    uint16_t ret_opcode = mnemonic_to_opcode(instr->mnemonic);
    uint8_t ret_length = mnemonic_to_length(instr->mnemonic);
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    printf("+--------+--------+\n");
    printf("| OPCODE |   I    |\n");
    printf("+--------+--------+\n");
    printf("0        8        F\n");
    // Print general information
    printf("MNEMONIC: %s\n", instr->mnemonic);
    hex_str_2_char_str((void*)&ret_opcode, sizeof(ret_opcode), 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", ret_length);
    printf("FORMAT:   I\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, false);
    printf("I:        %s\n", conv_buffer);
    return 0;
}