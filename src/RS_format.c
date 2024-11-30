#include "HLASMCompiler.h"

int build_RS(uint16_t opcode, char* operands_token, uint8_t* bin_buffer, InstructionFormat format){
    uint8_t r1 = 0;
    uint8_t r3_m3 = 0;
    uint8_t b2 = 0;
    uint16_t d2 = 0;
    char buffer[MAX_OPERANDS_LEN];
    bool run = true;
    bool redirect = false;
    size_t b_idx = 0;
    OperandsParseState state = R1;
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    for(size_t i = 0; i < MAX_OPERANDS_LEN && run;){
        switch (state){
        case R1:
            if(operands_token[i] == ','){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r1, sizeof(r1), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                switch (format){
                case RSa:
                    state = RSa_R3_D2;
                    break;
                case RSb:
                    state = M3;
                    break;
                default:
                    break;
                }
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
        case RSa_R3_D2:
            if(operands_token[i] == ','){
                if(b_idx == MAX_1CHR_LEN){
                    state = R3;
                }
                else{
                    state = D2;
                }
                redirect = true;
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
        case M3:
        case R3:
            if(operands_token[i] == ',' || redirect == true){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r3_m3, sizeof(r3_m3), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                redirect = false;
                b_idx = 0;
                state = D2;
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
        case D2:
            if(operands_token[i] == ',' || redirect == true){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&d2, sizeof(d2), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                redirect = false;
                b_idx = 0;
                state = B2;
                i++;
            }
            else{
                if(b_idx > MAX_3CHR_LEN){
                    return -1;
                }
                redirect = false;
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case B2:
            if(operands_token[i] == 0){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&b2, sizeof(b2), b_idx, NO_SKIP, true);
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
    bin_buffer[0] = opcode;
    // R1: bits(8-11)
    bin_buffer[1] = r1 << 4;
    // R3/M3: bits(12-15)
    bin_buffer[1] = bin_buffer[1] | r3_m3;
    // B2: bits(16-19)
    bin_buffer[2] = b2 << 4;
    // D2: bits(20-31)
    bin_buffer[2] = bin_buffer[2] | (d2 >> 8);
    bin_buffer[3] = d2 & 0x00FF;
    return 0;
}

int display_RS(Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    uint16_t ret_opcode = mnemonic_to_opcode(instr->mnemonic);
    uint8_t ret_length = mnemonic_to_length(instr->mnemonic);
    InstructionFormat ret_format = mnemonic_to_format(instr->mnemonic);
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    switch (ret_format){
    case RSa:
        printf("+--------+----+----+----+------------+\n");
        printf("| OPCODE | R1 | R3 | B2 |     D2     |\n");
        printf("+--------+----+----+----+------------+\n");
        printf("0        8    C    10   14          1F\n");
        break;
    case RSb:
        printf("+--------+----+----+----+------------+\n");
        printf("| OPCODE | R1 | M3 | B2 |     D2     |\n");
        printf("+--------+----+----+----+------------+\n");
        printf("0        8    C    10   14          1F\n");
        break;
    default:
        return -1;
    }
    // Print general information
    printf("MNEMONIC: %s\n", instr->mnemonic);
    hex_str_2_char_str((void*)&ret_opcode, sizeof(ret_opcode), 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 8, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", ret_length);
    switch (ret_format){
    case RSa:
        printf("FORMAT:   RSa\n");
        break;
    case RSb:
        printf("FORMAT:   RSb\n");
        break;
    default:
        return -1;
    }
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
    printf("R1:       %s\n", conv_buffer);
    switch (ret_format){
    case RSa:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("R3:       %s\n", conv_buffer);
        break;
    case RSb:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("M3:       %s\n", conv_buffer);
        break;
    default:
        return -1;
    }
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
    printf("B2:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 3, SKIP, false);
    printf("D2:       %s\n", conv_buffer);
    return 0;
}