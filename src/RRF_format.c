#include "HLASMCompiler.h"

int build_RRF(uint16_t opcode, char* operands_token, uint8_t* bin_buffer, InstructionFormat format){
    uint8_t r1 = 0;
    uint8_t r2 = 0;
    uint8_t r3_m3 = 0;
    uint8_t m4 = 0;
    char buffer[MAX_OPERANDS_LEN];
    bool run = true;
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
                case RRFa:
                case RRFc:
                case RRFd:
                    state = R2;
                    break;
                case RRFb:
                    state = R3;
                    break;
                case RRFe:
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
        case R2:
            if(operands_token[i] == ','){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r2, sizeof(r2), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                switch (format){
                case RRFa:
                case RRFc:
                    state = R3;
                    break;
                case RRFb:
                case RRFd:
                case RRFe:
                    state = M4;
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
        case M3:
        case R3:
            if(operands_token[i] == ',' || operands_token[i] == 0){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r3_m3, sizeof(r3_m3), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                switch (format){
                case RRFa:
                    state = M4;
                    break;
                case RRFb:
                case RRFe:
                    state = R2;
                    break;
                case RRFc:
                    state = OPS_DONE;
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
        case M4:
            if(operands_token[i] == 0){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&m4, sizeof(m4), b_idx, NO_SKIP, true);
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
    // Opcode: bits(0-15)
    bin_buffer[0] = opcode >> 8;
    bin_buffer[1] = opcode & 0x00FF;
    // R3: bits(16-19)
    switch (format){
    case RRFa:
    case RRFb:
    case RRFc:
    case RRFe:
        bin_buffer[2] = r3_m3 << 4;
        break;
    default:
        break;
    }
    // M4: bits(20-23)
    switch (format){
    case RRFa:
    case RRFb:
    case RRFd:
    case RRFe:
        bin_buffer[2] = bin_buffer[2] | m4;
        break;
    default:
        break;
    }
    // R1: bits(24-27)
    bin_buffer[3] = r1 << 4;
    // R2: bits(28-31)
    bin_buffer[3] = bin_buffer[3] | r2;
    return 0;
}

int display_RRF(Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    uint16_t ret_opcode = mnemonic_to_opcode(instr->mnemonic);
    uint8_t ret_length = mnemonic_to_length(instr->mnemonic);
    InstructionFormat ret_format = mnemonic_to_format(instr->mnemonic);
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    switch (ret_format){
    case RRFa:
    case RRFb:
        printf("+----------------+----+----+----+----+\n");
        printf("|     OPCODE     | R3 | M4 | R1 | R2 |\n");
        printf("+----------------+----+----+----+----+\n");
        printf("0                10   14   18   1C  1F\n");
        break;
    case RRFc:
        printf("+----------------+----+----+----+----+\n");
        printf("|     OPCODE     | M3 | // | R1 | R2 |\n");
        printf("+----------------+----+----+----+----+\n");
        printf("0                10   14   18   1C  1F\n");
        break;
    case RRFd:
        printf("+----------------+----+----+----+----+\n");
        printf("|     OPCODE     | // | M4 | R1 | R2 |\n");
        printf("+----------------+----+----+----+----+\n");
        printf("0                10   14   18   1C  1F\n");
        break;
    case RRFe:
        printf("+----------------+----+----+----+----+\n");
        printf("|     OPCODE     | M3 | M4 | R1 | R2 |\n");
        printf("+----------------+----+----+----+----+\n");
        printf("0                10   14   18   1C  1F\n");
        break;
    default:
        break;
    }
    // Print general information
    printf("MNEMONIC: %s\n", instr->mnemonic);
    hex_str_2_char_str((void*)&ret_opcode, sizeof(ret_opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 8, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", ret_length);
    switch (ret_format){
    case RRFa:
        printf("FORMAT:   RRFa\n");
        break;
    case RRFb:
        printf("FORMAT:   RRFb\n");
        break;
    case RRFc:
        printf("FORMAT:   RRFc\n");
        break;
    case RRFd:
        printf("FORMAT:   RRFd\n");
        break;
    case RRFe:
        printf("FORMAT:   RRFE\n");
        break;
    default:
        break;
    }
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    switch (ret_format){
    case RRFa:
    case RRFb:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("R3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("M4:       %s\n", conv_buffer);
        break;
    case RRFc:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("M3:       %s\n", conv_buffer);
        break;
    case RRFd:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("M4:       %s\n", conv_buffer);
    case RRFe:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("M3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("M4:       %s\n", conv_buffer);
        break;
    default:
        break;
    }
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
    printf("R1:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
    printf("R2:       %s\n", conv_buffer);
    return 0;
}