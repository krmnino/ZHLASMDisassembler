#include "HLASMCompiler.h"

int build_MII(uint16_t opcode, char* operands_token, uint8_t* bin_buffer, InstructionFormat format){
    uint8_t m1; 
    uint16_t ri2; 
    uint32_t ri3; 
    char buffer[MAX_OPERANDS_LEN];
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state = M1;
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    for(size_t i = 0; i < MAX_OPERANDS_LEN && run;){
        switch (state){
        case M1:
            if(operands_token[i] == ','){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&m1, sizeof(m1), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = RI2;
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
        case RI2:
            if(operands_token[i] == ','){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&ri2, sizeof(ri2), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = RI3;
                i++;
            }
            else{
                if(b_idx > MAX_3CHR_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case RI3:
            if(operands_token[i] == 0){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&ri3, sizeof(ri3), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = OPS_DONE;
                i++;
            }
            else{
                if(b_idx > MAX_6CHR_LEN){
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
    bin_buffer[0] = opcode;
    // M1: bits(8-11)
    bin_buffer[1] = m1 << 4;
    // RI2: bits(12-23)
    bin_buffer[1] = bin_buffer[1] | (ri2 >> 8);
    bin_buffer[2] = ri2 & 0x00FF;
    // RI3: bits(24-47)
    bin_buffer[3] = ri3 >> 16;
    bin_buffer[4] = (ri3 >> 8) & 0x0000FF;
    bin_buffer[5] = ri3 & 0x0000FF;
    return 0;
}

int display_MII(Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    uint16_t ret_opcode = mnemonic_to_opcode(instr->mnemonic);
    uint8_t ret_length = mnemonic_to_length(instr->mnemonic);
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    printf("+--------+----+------------+------------------------+\n");
    printf("| OPCODE | M1 |    RI2     |          RI3           |\n");
    printf("+--------+----+------------+------------------------+\n");
    printf("0        8    C            18                      2F\n");
    // Print general information
    printf("MNEMONIC: %s\n", instr->mnemonic);
    hex_str_2_char_str((void*)&ret_opcode, sizeof(ret_opcode), 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 12, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", ret_length);
    printf("FORMAT:   MII\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
    printf("M1:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 3, SKIP, false);
    printf("RI2:      %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 6, NO_SKIP, false);
    printf("RI3:      %s\n", conv_buffer);
    return 0;
}