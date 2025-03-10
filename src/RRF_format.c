#include "InstructionTable.h"
#include "HLASMDisassembler.h"

ErrorCode assemble_RRF(size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    InstructionFormat format = INSTRUCTION_TABLE[table_index].format;
    bool m4_unused = INSTRUCTION_TABLE[table_index].unused_operands & M4_UNUSED;
    uint8_t r1 = 0;
    uint8_t r2 = 0;
    uint8_t r3_m3 = 0;
    uint8_t m4 = 0;
    char buffer[MAX_OPERANDS_LEN];
    size_t i;
    size_t operands_token_len = strlen(operands_token) + 1;
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state = R1;
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    for(i = 0; i < operands_token_len && run;){
        switch (state){
        case R1:
            if(operands_token[i] == ','){
                if(b_idx == 0){
                    run = false;
                }
                else if(!is_valid_hex_string(buffer, b_idx)){
                    Context.error_code = OPERAND_NON_HEX_FOUND;
                    sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                    strcpy((char*)&Context.msg_extras[1], operands_token);
                    return Context.error_code;
                }
                else{
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
            }
            else{
                if(b_idx >= MAX_1CHR_LEN){
                    Context.error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                    strcpy((char*)&Context.msg_extras[1], "R1");
                    sprintf((char*)&Context.msg_extras[2], "%d", MAX_1CHR_LEN);
                    return Context.error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case R2:
            if(operands_token[i] == ',' || operands_token[i] == 0){
                if(b_idx == 0){
                    run = false;
                }
                else if(!is_valid_hex_string(buffer, b_idx)){
                    Context.error_code = OPERAND_NON_HEX_FOUND;
                    sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                    strcpy((char*)&Context.msg_extras[1], operands_token);
                    return Context.error_code;
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
                    if(m4_unused){
                        state = OPS_DONE;
                    }
                    else{
                        state = M4;
                    }
                    break;
                default:
                    break;
                }
                i++;
            }
            else{
                if(b_idx >= MAX_1CHR_LEN){
                    Context.error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                    strcpy((char*)&Context.msg_extras[1], "R2");
                    sprintf((char*)&Context.msg_extras[2], "%d", MAX_1CHR_LEN);
                    return Context.error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case M3:
        case R3:
            if(operands_token[i] == ',' || operands_token[i] == 0){
                if(b_idx == 0){
                    run = false;
                }
                else if(!is_valid_hex_string(buffer, b_idx)){
                    Context.error_code = OPERAND_NON_HEX_FOUND;
                    sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                    strcpy((char*)&Context.msg_extras[1], operands_token);
                    return Context.error_code;
                }
                else{
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r3_m3, sizeof(r3_m3), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case RRFa:
                        if(m4_unused){
                            state = OPS_DONE;
                        }
                        else{
                            state = M4;
                        }
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
            }
            else{
                if(b_idx >= MAX_1CHR_LEN){
                    Context.error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                    switch (format){
                    case RRFa:
                    case RRFb:
                        strcpy((char*)&Context.msg_extras[1], "R3");
                        break;
                    case RRFc:
                    case RRFe:
                        strcpy((char*)&Context.msg_extras[1], "M3");
                        break;
                    default:
                        break;
                    }
                    sprintf((char*)&Context.msg_extras[2], "%d", MAX_1CHR_LEN);
                    return Context.error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case M4:
            if(operands_token[i] == ',' || operands_token[i] == 0){
                if(b_idx == 0){
                    run = false;
                }
                else if(!is_valid_hex_string(buffer, b_idx)){
                    Context.error_code = OPERAND_NON_HEX_FOUND;
                    sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                    strcpy((char*)&Context.msg_extras[1], operands_token);
                    return Context.error_code;
                }
                else{
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&m4, sizeof(m4), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    state = OPS_DONE;
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_1CHR_LEN){
                    Context.error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                    strcpy((char*)&Context.msg_extras[1], "M4");
                    sprintf((char*)&Context.msg_extras[2], "%d", MAX_1CHR_LEN);
                    return Context.error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case OPS_DONE:
            run = false;
            break;
        default:
            break;
        }
    }
    if(state != OPS_DONE){
        Context.error_code = MISSING_OPERANDS;
        sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
        strcpy((char*)&Context.msg_extras[1], INSTRUCTION_TABLE[table_index].mnemonic);
        return Context.error_code;
    }
    if(i != operands_token_len){
        Context.error_code = TOO_MANY_OPERANDS;
        sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
        strcpy((char*)&Context.msg_extras[1], INSTRUCTION_TABLE[table_index].mnemonic);
        return Context.error_code;
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
    if(!m4_unused){
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
    }
    // R1: bits(24-27)
    bin_buffer[3] = r1 << 4;
    // R2: bits(28-31)
    bin_buffer[3] = bin_buffer[3] | r2;
    return OK;
}

ErrorCode display_RRF(Instruction* instr){
    if(instr == NULL){
        Context.error_code = NULL_POINTER_TO_OBJECT;
        return Context.error_code;
    }
    uint16_t opcode = INSTRUCTION_TABLE[instr->it_index].opcode;
    uint8_t length = INSTRUCTION_TABLE[instr->it_index].length;
    InstructionFormat format = INSTRUCTION_TABLE[instr->it_index].format;
    bool m4_unused = INSTRUCTION_TABLE[instr->it_index].unused_operands & M4_UNUSED;
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    switch (format){
    case RRFa:
    case RRFb:
        if(m4_unused){
            printf("+----------------+----+----+----+----+\n");
            printf("|     OPCODE     | R3 | // | R1 | R2 |\n");
            printf("+----------------+----+----+----+----+\n");
            printf("0                10   14   18   1C  1F\n");
        }
        else{
            printf("+----------------+----+----+----+----+\n");
            printf("|     OPCODE     | R3 | M4 | R1 | R2 |\n");
            printf("+----------------+----+----+----+----+\n");
            printf("0                10   14   18   1C  1F\n");
        }
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
        if(m4_unused){
            printf("+----------------+----+----+----+----+\n");
            printf("|     OPCODE     | M3 | // | R1 | R2 |\n");
            printf("+----------------+----+----+----+----+\n");
            printf("0                10   14   18   1C  1F\n");
        }
        else{
            printf("+----------------+----+----+----+----+\n");
            printf("|     OPCODE     | M3 | M4 | R1 | R2 |\n");
            printf("+----------------+----+----+----+----+\n");
            printf("0                10   14   18   1C  1F\n");
        }
        break;
    default:
        break;
    }
    // Print general information
    printf("MNEMONIC: %s\n", INSTRUCTION_TABLE[instr->it_index].mnemonic);
    printf("OPERANDS: %s\n", instr->operands_txt);
    hex_str_2_char_str((void*)&opcode, sizeof(opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_4CHR_LEN, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_8CHR_LEN, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", length);
    switch (format){
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
        printf("FORMAT:   RRFe\n");
        break;
    default:
        break;
    }
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    switch (format){
    case RRFa:
    case RRFb:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("R3:       %s\n", conv_buffer);
        if(!m4_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
            printf("M4:       %s\n", conv_buffer);
        }
        break;
    case RRFc:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("M3:       %s\n", conv_buffer);
        break;
    case RRFd:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
        printf("M4:       %s\n", conv_buffer);
        break;
    case RRFe:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("M3:       %s\n", conv_buffer);
        if(!m4_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
            printf("M4:       %s\n", conv_buffer);
        }
        break;
    default:
        break;
    }
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
    printf("R1:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
    printf("R2:       %s\n", conv_buffer);
    return OK;
}

ErrorCode disassemble_RRF(size_t table_index, const uint8_t* bin_buffer, char* operands_token){
    char buffer[MAX_OPERANDS_LEN];
    InstructionFormat format = INSTRUCTION_TABLE[table_index].format;
    bool m3_unused = INSTRUCTION_TABLE[table_index].unused_operands & M3_UNUSED;
    bool m4_unused = INSTRUCTION_TABLE[table_index].unused_operands & M4_UNUSED;
    size_t i = 0;
    // Check if need to use alternative instruction
    if(strlen(INSTRUCTION_TABLE[table_index].alternative_instr) != 0 && ((bin_buffer[2] & 0x0F) != 0x00) && m4_unused){
        return USE_ALTERNATIVE;
    }
    // R1:
    memset(&buffer, 0, sizeof(buffer));
    hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
    operands_token[i++] = buffer[0];
    operands_token[i++] = ',';
    switch (format){
    case RRFa:
        // R2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // R3/M3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        break;    
    case RRFb:
        // R3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // R2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        break;    
    case RRFc:
    case RRFd:
        // R2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        // M3:
        if(!m3_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        break;    
    case RRFe:
        // M3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // R2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        break;    
    default:
        break;
    }
    // M4:
    if(!m4_unused){
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = ',';
        operands_token[i++] = buffer[0];
    }
    return OK;
}