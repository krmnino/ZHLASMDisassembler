#include "HLASMCompiler.h"

int build_IE(uint16_t opcode, char* operands_token, uint8_t* bin_buffer, InstructionFormat format){
    uint8_t i1; 
    uint8_t i2; 
    char buffer[MAX_OPERANDS_LEN];
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state = I1;
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    for(size_t i = 0; i < MAX_OPERANDS_LEN && run;){
        switch (state){
        case I1:
            if(operands_token[i] == ','){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i1, sizeof(i1), 1, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = I2;
                i++;
            }
            else{
                if(b_idx > MAX_1CHR_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case I2:
            if(operands_token[i] == 0){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i2, sizeof(i2), 1, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = OPS_DONE;
                i++;
            }
            else{
                if(b_idx > MAX_1CHR_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case OPS_DONE:
            run = false;
        default:
            break;
        }
    }
     // Opcode: bits(0-7)
    bin_buffer[0] = opcode >> 8;
     // Opcode: bits(8-16)
    bin_buffer[1] = opcode & 0x00FF;
    // I1: bits(24-27)
    bin_buffer[3] = i1;
    bin_buffer[3] = bin_buffer[3] << 4;
    // I2: bits(28-31)
    bin_buffer[3] = bin_buffer[3] | i2;
    return 0;
}

int display_IE(Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    uint16_t ret_opcode = mnemonic_to_opcode(instr->mnemonic);
    uint8_t ret_length = mnemonic_to_length(instr->mnemonic);
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    printf("+----------------+--------+----+----+\n");
    printf("|     OPCODE     | ////// | I1 | I2 |\n");
    printf("+----------------+--------+----+----+\n");
    printf("0                1A       18   1C  1F\n");
    // Print general information
    printf("MNEMONIC: %s\n", instr->mnemonic);
    hex_str_2_char_str((void*)&ret_opcode, sizeof(ret_opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 8, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", ret_length);
    printf("FORMAT:   IE\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
    printf("I1:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
    printf("I2:       %s\n", conv_buffer);
    return 0;
}