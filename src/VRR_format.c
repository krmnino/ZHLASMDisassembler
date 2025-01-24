#include "InstructionTable.h"
#include "HLASMDisassembler.h"

ErrorCode assemble_VRR(Context* c, size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    InstructionFormat format = INSTRUCTION_TABLE[table_index].format;
    bool m3_unused = INSTRUCTION_TABLE[table_index].unused_operands & M3_UNUSED;
    bool m4_unused = INSTRUCTION_TABLE[table_index].unused_operands & M4_UNUSED;
    bool m5_unused = INSTRUCTION_TABLE[table_index].unused_operands & M5_UNUSED;
    bool m6_unused = INSTRUCTION_TABLE[table_index].unused_operands & M6_UNUSED;
    uint8_t v1_r1 = 0;
    uint8_t v2_r2 = 0;
    uint8_t v3_r3 = 0;
    uint8_t m3 = 0;
    uint8_t m4_v4 = 0;
    uint8_t m5 = 0;
    uint8_t m6 = 0;
    char buffer[MAX_OPERANDS_LEN];
    size_t i;
    size_t operands_token_len = strlen(operands_token) + 1;
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state;
    switch (format){
    case VRRa:
    case VRRb:
    case VRRc:
    case VRRd:
    case VRRe:
    case VRRf:
    case VRRg:
    case VRRh:
    case VRRj:
    case VRRk:
        state = V1;
        break;
    case VRRi:
        state = R1;
        break;
    default:
        break;
    }
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    for(i = 0; i < operands_token_len && run;){
        switch (state){
        case R1:
        case V1:
            if(operands_token[i] == ',' || operands_token[i] == 0){
                if(b_idx == 0){
                    run = false;
                }
                else if(!is_valid_hex_string(buffer, b_idx)){
                    c->error_code = OPERAND_NON_HEX_FOUND;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], operands_token);
                    return c->error_code;
                }
                else{
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&v1_r1, sizeof(v1_r1), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case VRRa:
                    case VRRb:
                    case VRRc:
                    case VRRd:
                    case VRRe:
                    case VRRh:
                    case VRRi:
                    case VRRj:
                    case VRRk:
                        state = V2;
                        break;
                    case VRRf:
                        state = R2;
                        break;
                    case VRRg:
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
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    switch (state){
                    case V1:
                        strcpy((char*)&c->msg_extras[1], "V1");
                        break;
                    case R1:
                        strcpy((char*)&c->msg_extras[1], "R1");
                        break;
                    default:
                        break;
                    }
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_1CHR_LEN);
                    return c->error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case R2:
        case V2:
            if(operands_token[i] == ',' || operands_token[i] == 0){
                if(b_idx == 0){
                    run = false;
                }
                else if(!is_valid_hex_string(buffer, b_idx)){
                    c->error_code = OPERAND_NON_HEX_FOUND;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], operands_token);
                    return c->error_code;
                }
                else{
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&v2_r2, sizeof(v2_r2), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case VRRa:
                    case VRRh:
                        if(m3_unused){
                            state = OPS_DONE;
                        }
                        else{
                            state = M3;
                        }
                        break;
                    case VRRb:
                    case VRRc:
                    case VRRd:
                    case VRRe:
                    case VRRj:
                        state = V3;
                        break;
                    case VRRf:
                        state = R3;
                        break;
                    case VRRi:
                    case VRRk:
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
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    switch (state){
                    case V2:
                        strcpy((char*)&c->msg_extras[1], "V2");
                        break;
                    case R2:
                        strcpy((char*)&c->msg_extras[1], "R2");
                        break;
                    default:
                        break;
                    }
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_1CHR_LEN);
                    return c->error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case R3:
        case V3:
            if(operands_token[i] == ',' || operands_token[i] == 0){
                if(b_idx == 0){
                    run = false;
                }
                else if(!is_valid_hex_string(buffer, b_idx)){
                    c->error_code = OPERAND_NON_HEX_FOUND;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], operands_token);
                    return c->error_code;
                }
                else{
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&v3_r3, sizeof(v3_r3), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case VRRb:
                        if(m4_unused && m5_unused){
                            state = OPS_DONE;
                        }
                        else{
                            state = M4;
                        }
                        break;
                    case VRRc:
                        if(m4_unused){
                            state = OPS_DONE;
                        }
                        else{
                            state = M4;
                        }
                        break;
                    case VRRd:
                    case VRRe:
                        state = V4;
                        break;
                    case VRRf:
                        state = OPS_DONE;
                        break;
                    case VRRj:
                        state = M4;
                        break;
                    default:
                        break;
                    }
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_1CHR_LEN){
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    switch (state){
                    case V3:
                        strcpy((char*)&c->msg_extras[1], "V3");
                        break;
                    case R3:
                        strcpy((char*)&c->msg_extras[1], "R3");
                        break;
                    default:
                        break;
                    }
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_1CHR_LEN);
                    return c->error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case M3:
            if(operands_token[i] == ',' || operands_token[i] == 0){
                if(b_idx == 0){
                    run = false;
                }
                else if(!is_valid_hex_string(buffer, b_idx)){
                    c->error_code = OPERAND_NON_HEX_FOUND;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], operands_token);
                    return c->error_code;
                }
                else{
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&m3, sizeof(m3), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case VRRa:
                        if(m4_unused && m5_unused){
                            state = OPS_DONE;
                        }
                        else if(m4_unused && !m5_unused){
                            state = M5;
                        }
                        else{
                            state = M4;
                        }
                        break;
                    case VRRh:
                    case VRRk:
                        state = OPS_DONE;
                        break;
                    case VRRi:
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
            }
            else{
                if(b_idx >= MAX_1CHR_LEN){
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], "M3");
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_1CHR_LEN);
                    return c->error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case M4:
        case V4:
            if(operands_token[i] == ',' || operands_token[i] == 0){
                if(b_idx == 0){
                    run = false;
                }
                else if(!is_valid_hex_string(buffer, b_idx)){
                    c->error_code = OPERAND_NON_HEX_FOUND;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], operands_token);
                    return c->error_code;
                }
                else{
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&m4_v4, sizeof(m4_v4), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case VRRa:
                    case VRRb:
                    case VRRc:
                    case VRRd:
                    case VRRe:
                        if(m5_unused){
                            state = OPS_DONE;
                        }
                        else{
                            state = M5;
                        }
                        break;
                    case VRRi:
                    case VRRj:
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
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    switch (state){
                    case M4:
                        strcpy((char*)&c->msg_extras[1], "M4");
                        break;
                    case V4:
                        strcpy((char*)&c->msg_extras[1], "V4");
                        break;
                    default:
                        break;
                    }
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_1CHR_LEN);
                    return c->error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case M5:
            if(operands_token[i] == ',' || operands_token[i] == 0){
                if(b_idx == 0){
                    run = false;
                }
                else if(!is_valid_hex_string(buffer, b_idx)){
                    c->error_code = OPERAND_NON_HEX_FOUND;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], operands_token);
                    return c->error_code;
                }
                else{
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&m5, sizeof(m5), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case VRRa:
                    case VRRb:
                        state = OPS_DONE;
                        break;
                    case VRRc:
                    case VRRd:
                    case VRRe:
                        if(m6_unused){
                            state = OPS_DONE;
                        }
                        else{
                            state = M6;
                        }
                        break;
                    default:
                        break;
                    }
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_1CHR_LEN){
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], "M5");
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_1CHR_LEN);
                    return c->error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case M6:
            if(operands_token[i] == ',' || operands_token[i] == 0){
                if(b_idx == 0){
                    run = false;
                }
                else if(!is_valid_hex_string(buffer, b_idx)){
                    c->error_code = OPERAND_NON_HEX_FOUND;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], operands_token);
                    return c->error_code;
                }
                else{
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&m6, sizeof(m6), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    state = OPS_DONE;
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_1CHR_LEN){
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], "M6");
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_1CHR_LEN);
                    return c->error_code;
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
    if(state != OPS_DONE){
        c->error_code = MISSING_OPERANDS;
        sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
        strcpy((char*)&c->msg_extras[1], INSTRUCTION_TABLE[table_index].mnemonic);
        return c->error_code;
    }
    if(i != operands_token_len){
        c->error_code = TOO_MANY_OPERANDS;
        sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
        strcpy((char*)&c->msg_extras[1], INSTRUCTION_TABLE[table_index].mnemonic);
        return c->error_code;
    }
    // Opcode (part 1): bits(0-7)
    bin_buffer[0] = opcode >> 8;
    // V1/R1:
    switch (format){
    case VRRa:
    case VRRb:
    case VRRc:
    case VRRd:
    case VRRe:
    case VRRf:
    case VRRi:
    case VRRj:
    case VRRk:
        // V1/R1: bits(8-11)
        bin_buffer[1] = v1_r1 << 4;
        break;
    case VRRg:
    case VRRh:
        // V1: bits(12-15)
        bin_buffer[1] = v1_r1;
        break;
    default:
        break;
    }
    // V2/R2:
    switch (format){
    case VRRa:
    case VRRb:
    case VRRc:
    case VRRd:
    case VRRe:
    case VRRf:
    case VRRi:
    case VRRj:
    case VRRk:
        // V2/R2: bits(12-15)
        bin_buffer[1] = bin_buffer[1] | v2_r2;
        break;
    case VRRh:
        // V2: bits(16-19)
        bin_buffer[2] = v2_r2 << 4;
        break;
    default:
        break;
    }
    // V3/R3:
    switch (format){
    case VRRb:
    case VRRc:
    case VRRd:
    case VRRe:
    case VRRf:
    case VRRj:
        // V3/R3: bits(16-19)
        bin_buffer[2] = v3_r3 << 4;
        break;
    default:
        break;
    }
    // M3:
    switch (format){
    case VRRa:
        if(!m3_unused){
            // M3: bits(32-35)
            bin_buffer[4] = m3 << 4;
        }
        break;
    case VRRh:
        if(!m3_unused){
            // M3: bits(24-27)
            bin_buffer[3] = m3 << 4;
        }
        break;
    case VRRi:
    case VRRk:
        // M3: bits(24-27)
        bin_buffer[3] = m3 << 4;
        break;
    default:
        break;
    }
    // M4/V4:
    switch (format){
    case VRRa:
        if(!m4_unused){
            // M4: bits(28-31)
            bin_buffer[3] = m4_v4;
        }
        break;
    case VRRb:
    case VRRc:
        if(!m4_unused){
            // M4: bits(32-35)
            bin_buffer[4] = m4_v4 << 4;
        }
        break;
    case VRRd:
    case VRRe:
        // V4: bits(32-35)
        bin_buffer[4] = m4_v4 << 4;
        break;
    case VRRi:
        if(!m4_unused){
            // M4: bits(28-31)
            bin_buffer[3] = bin_buffer[3] | m4_v4;
        }
        break;
    case VRRj:
        // M4: bits(24-27)
        bin_buffer[3] = m4_v4 << 4;
        break;
    default:
        break;
    }
    // M5:
    switch (format){
    case VRRa:
    case VRRb:
        if(!m5_unused){
            // M5: bits(24-27)
            bin_buffer[3] = bin_buffer[3] | (m5 << 4);
        }
        break;
    case VRRc:
    case VRRe:
        if(!m5_unused){
            // M5: bits(28-31)
            bin_buffer[3] = m5;
        }
        break;
    case VRRd:
        if(!m5_unused){
            // M5: bits(20-23)
            bin_buffer[2] = bin_buffer[2] | m5;
        }
        break;
    default:
        break;
    }
    // M6:
    switch (format){
    case VRRc:
    case VRRd:
        if(!m6_unused){
            // M6: bits(24-27)
            bin_buffer[3] = bin_buffer[3] | (m6 << 4);
        }
        break;
    case VRRe:
        if(!m6_unused){
            // M5: bits(20-23)
            bin_buffer[2] = bin_buffer[2] | m6;
        }
        break;
    default:
        break;
    }
    // Opcode (part 2): bits(40-47)
    bin_buffer[5] = opcode & 0x00FF;
    return OK;
}

ErrorCode display_VRR(Context* c, Instruction* instr){
    if(instr == NULL){
        c->error_code = NULL_POINTER_TO_OBJECT;
        return c->error_code;
    }
    uint16_t opcode = INSTRUCTION_TABLE[instr->it_index].opcode;
    uint8_t length = INSTRUCTION_TABLE[instr->it_index].length;
    InstructionFormat format = INSTRUCTION_TABLE[instr->it_index].format;
    bool m3_unused = INSTRUCTION_TABLE[instr->it_index].unused_operands & M3_UNUSED;
    bool m4_unused = INSTRUCTION_TABLE[instr->it_index].unused_operands & M4_UNUSED;
    bool m5_unused = INSTRUCTION_TABLE[instr->it_index].unused_operands & M5_UNUSED;
    bool m6_unused = INSTRUCTION_TABLE[instr->it_index].unused_operands & M6_UNUSED;
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    switch (format){
    case VRRa:
        if(m3_unused && m4_unused && m5_unused){
            printf("+--------+----+----+--------+----+----+----+----+--------+\n");
            printf("| OPCODE | V1 | V2 | ////// | // | // | // | RX | OPCODE |\n");
            printf("+--------+----+----+--------+----+----+----+----+--------+\n");
            printf("0        8    C    10       18   1C   20   24   28      2F\n");
        }
        else if(!m3_unused && m4_unused && m5_unused){
            printf("+--------+----+----+--------+----+----+----+----+--------+\n");
            printf("| OPCODE | V1 | V2 | ////// | // | // | M3 | RX | OPCODE |\n");
            printf("+--------+----+----+--------+----+----+----+----+--------+\n");
            printf("0        8    C    10       18   1C   20   24   28      2F\n");
        }
        else if(!m3_unused && !m4_unused && m5_unused){
            printf("+--------+----+----+--------+----+----+----+----+--------+\n");
            printf("| OPCODE | V1 | V2 | ////// | // | M4 | M3 | RX | OPCODE |\n");
            printf("+--------+----+----+--------+----+----+----+----+--------+\n");
            printf("0        8    C    10       18   1C   20   24   28      2F\n");
        }
        else if(!m3_unused && m4_unused && !m5_unused){
            printf("+--------+----+----+--------+----+----+----+----+--------+\n");
            printf("| OPCODE | V1 | V2 | ////// | M5 | // | M3 | RX | OPCODE |\n");
            printf("+--------+----+----+--------+----+----+----+----+--------+\n");
            printf("0        8    C    10       18   1C   20   24   28      2F\n");
        }
        else{
            printf("+--------+----+----+--------+----+----+----+----+--------+\n");
            printf("| OPCODE | V1 | V2 | ////// | M5 | M4 | M3 | RX | OPCODE |\n");
            printf("+--------+----+----+--------+----+----+----+----+--------+\n");
            printf("0        8    C    10       18   1C   20   24   28      2F\n");
        }
        break;
    case VRRb:
        printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
        printf("| OPCODE | V1 | V2 | V3 | // | M5 | // | M4 | RX | OPCODE |\n");
        printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
        printf("0        8    C    10   14   18   1C   20   24   28      2F\n");
        break;
    case VRRc:
        if(m4_unused && m5_unused && m6_unused){
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("| OPCODE | V1 | V2 | V3 | // | // | // | // | RX | OPCODE |\n");
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("0        8    C    10   14   18   1C   20   24   28      2F\n");
        }
        else if(!m4_unused && m5_unused && m6_unused){
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("| OPCODE | V1 | V2 | V3 | // | // | // | M4 | RX | OPCODE |\n");
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("0        8    C    10   14   18   1C   20   24   28      2F\n");
        }
        else if(!m4_unused && !m5_unused && m6_unused){
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("| OPCODE | V1 | V2 | V3 | // | // | M5 | M4 | RX | OPCODE |\n");
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("0        8    C    10   14   18   1C   20   24   28      2F\n");
        }
        else{
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("| OPCODE | V1 | V2 | V3 | // | M6 | M5 | M4 | RX | OPCODE |\n");
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("0        8    C    10   14   18   1C   20   24   28      2F\n");
        }
        break;
    case VRRd:
        if(m5_unused && m6_unused){
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("| OPCODE | V1 | V2 | V3 | // | // | // | V4 | RX | OPCODE |\n");
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("0        8    C    10   14   18   1C   20   24   28      2F\n");
        }
        else if(!m5_unused && m6_unused){
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("| OPCODE | V1 | V2 | V3 | M5 | // | // | V4 | RX | OPCODE |\n");
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("0        8    C    10   14   18   1C   20   24   28      2F\n");
        }
        else{
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("| OPCODE | V1 | V2 | V3 | M5 | M6 | // | V4 | RX | OPCODE |\n");
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("0        8    C    10   14   18   1C   20   24   28      2F\n");
        }
        break;
    case VRRe:
        if(m5_unused && m6_unused){
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("| OPCODE | V1 | V2 | V3 | // | // | // | V4 | RX | OPCODE |\n");
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("0        8    C    10   14   18   1C   20   24   28      2F\n");
        }
        else if(!m5_unused && m6_unused){
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("| OPCODE | V1 | V2 | V3 | // | // | M5 | V4 | RX | OPCODE |\n");
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("0        8    C    10   14   18   1C   20   24   28      2F\n");
        }
        else{
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("| OPCODE | V1 | V2 | V3 | M6 | // | M5 | V4 | RX | OPCODE |\n");
            printf("+--------+----+----+----+----+----+----+----+----+--------+\n");
            printf("0        8    C    10   14   18   1C   20   24   28      2F\n");
        }
        break;
    case VRRf:
        printf("+--------+----+----+----+----------------+----+--------+\n");
        printf("| OPCODE | V1 | R2 | R3 | ////////////// | RX | OPCODE |\n");
        printf("+--------+----+----+----+----------------+----+--------+\n");
        printf("0        8    C    10   14               24   28      2F\n");
        break;
    case VRRg:
        printf("+--------+----+----+--------------------+----+--------+\n");
        printf("| OPCODE | // | V1 | ////////////////// | RX | OPCODE |\n");
        printf("+--------+----+----+--------------------+----+--------+\n");
        printf("0        8    C    10                   24   28      2F\n");
        break;
    case VRRh:
        if(m3_unused){
            printf("+--------+----+----+----+----+----+--------+----+--------+\n");
            printf("| OPCODE | // | V1 | V2 | // | // | ////// | RX | OPCODE |\n");
            printf("+--------+----+----+----+----+----+--------+----+--------+\n");
            printf("0        8    C    10   14   18   1C       24   28      2F\n");
        }
        else{
            printf("+--------+----+----+----+----+----+--------+----+--------+\n");
            printf("| OPCODE | // | V1 | V2 | // | M3 | ////// | RX | OPCODE |\n");
            printf("+--------+----+----+----+----+----+--------+----+--------+\n");
            printf("0        8    C    10   14   18   1C       24   28      2F\n");
        }
        break;
    case VRRi:
        if(m4_unused){
            printf("+--------+----+----+--------+----+----+----+----+--------+\n");
            printf("| OPCODE | R1 | V2 | ////// | M3 | // | // | RX | OPCODE |\n");
            printf("+--------+----+----+--------+----+----+----+----+--------+\n");
            printf("0        8    C    10       18   1C   20   24   28      2F\n");
        }
        else{
            printf("+--------+----+----+--------+----+----+----+----+--------+\n");
            printf("| OPCODE | R1 | V2 | ////// | M3 | M4 | // | RX | OPCODE |\n");
            printf("+--------+----+----+--------+----+----+----+----+--------+\n");
            printf("0        8    C    10       18   1C   20   24   28      2F\n");
        }
        break;
    case VRRj:
        printf("+--------+----+----+----+----+----+--------+----+--------+\n");
        printf("| OPCODE | V1 | V2 | V3 | // | M4 | ////// | RX | OPCODE |\n");
        printf("+--------+----+----+----+----+----+--------+----+--------+\n");
        printf("0        8    C    10   14   18   1C       24   28      2F\n");
        break;
    case VRRk:
        printf("+--------+----+----+--------+----+--------+----+--------+\n");
        printf("| OPCODE | V1 | V2 | ////// | M3 | ////// | RX | OPCODE |\n");
        printf("+--------+----+----+--------+----+--------+----+--------+\n");
        printf("0        8    C    10       18   1C       24   28      2F\n");
        break;
    default:
        break;
    }
    // Print general information
    printf("MNEMONIC: %s\n", INSTRUCTION_TABLE[instr->it_index].mnemonic);
    hex_str_2_char_str((void*)&opcode, sizeof(opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 12, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", length);
    switch (format){
    case VRRa:
        printf("FORMAT:   VRRa\n");
        break;
    case VRRb:
        printf("FORMAT:   VRRb\n");
        break;
    case VRRc:
        printf("FORMAT:   VRRc\n");
        break;
    case VRRd:
        printf("FORMAT:   VRRd\n");
        break;
    case VRRe:
        printf("FORMAT:   VRRe\n");
        break;
    case VRRf:
        printf("FORMAT:   VRRf\n");
        break;
    case VRRg:
        printf("FORMAT:   VRRg\n");
        break;
    case VRRh:
        printf("FORMAT:   VRRh\n");
        break;
    case VRRi:
        printf("FORMAT:   VRRi\n");
        break;
    case VRRj:
        printf("FORMAT:   VRRj\n");
        break;
    case VRRk:
        printf("FORMAT:   VRRk\n");
        break;
    default:
        break;
    }
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    switch (format){
    case VRRa:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("V2:       %s\n", conv_buffer);
        if(!m5_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
            printf("M5:       %s\n", conv_buffer);
        }
        if(!m4_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
            printf("M4:       %s\n", conv_buffer);
        }
        if(!m3_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
            printf("M3:       %s\n", conv_buffer);
        }
        break;
    case VRRb:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("V2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V3:       %s\n", conv_buffer);
        if(!m5_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
            printf("M5:       %s\n", conv_buffer);
        }
        if(!m4_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
            printf("M4:       %s\n", conv_buffer);
        }
        break;
    case VRRc:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("V2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V3:       %s\n", conv_buffer);
        if(!m6_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
            printf("M6:       %s\n", conv_buffer);
        }
        if(!m5_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
            printf("M5:       %s\n", conv_buffer);
        }
        if(!m4_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
            printf("M4:       %s\n", conv_buffer);
        }
        break;
    case VRRd:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("V2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V3:       %s\n", conv_buffer);
        if(!m5_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
            printf("M5:       %s\n", conv_buffer);
        }
        if(!m6_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
            printf("M6:       %s\n", conv_buffer);
        }
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V4:       %s\n", conv_buffer);
        break;
    case VRRe:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("V2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V3:       %s\n", conv_buffer);
        if(!m6_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
            printf("M6:       %s\n", conv_buffer);
        }
        if(!m5_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
            printf("M5:       %s\n", conv_buffer);
        }
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V4:       %s\n", conv_buffer);
        break;
    case VRRf:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("R2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("R3:       %s\n", conv_buffer);
        break;
    case VRRg:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("V1:       %s\n", conv_buffer);
        break;
    case VRRh:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("V1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V2:       %s\n", conv_buffer);
        if(!m3_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
            printf("M3:       %s\n", conv_buffer);
        }
        break;
    case VRRi:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("R1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("V2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("M3:       %s\n", conv_buffer);
        if(!m4_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
            printf("M4:       %s\n", conv_buffer);
        }
        break;
    case VRRj:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("V2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("M4:       %s\n", conv_buffer);
        break;
    case VRRk:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("V2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("M3:       %s\n", conv_buffer);
        break;
    default:
        break;
    }
    return OK;
}

ErrorCode disassemble_VRR(Context* c, size_t table_index, const uint8_t* bin_buffer, char* operands_token){
    char buffer[MAX_OPERANDS_LEN];
    InstructionFormat format = INSTRUCTION_TABLE[table_index].format;
    bool m3_unused = INSTRUCTION_TABLE[table_index].unused_operands & M3_UNUSED;
    bool m4_unused = INSTRUCTION_TABLE[table_index].unused_operands & M4_UNUSED;
    bool m5_unused = INSTRUCTION_TABLE[table_index].unused_operands & M5_UNUSED;
    bool m6_unused = INSTRUCTION_TABLE[table_index].unused_operands & M6_UNUSED;
    size_t i = 0;
    switch (format){
    case VRRa:
        // V1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // V2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        // M3:
        if(!m3_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        // M4:
        if(!m4_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        // M5:
        if(!m5_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        break;
    case VRRb:
        // V1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // V2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // V3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        // M4:
        if(!m4_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        // M5:
        if(!m5_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        break;
    case VRRc:
        // V1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // V2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // V3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        // M4:
        if(!m4_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        // M5:
        if(!m5_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        // M6:
        if(!m6_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        break;
    case VRRd:
        // V1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // V2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // V3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // V4:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        // M5:
        if(!m5_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        // M6:
        if(!m6_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        break;
    case VRRe:
        // V1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // V2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // V3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // V4:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        // M5:
        if(!m5_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        // M6:
        if(!m6_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        break;
    case VRRf:
        // V1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // R2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // R3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        break;
    case VRRg:
        // V1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        break;
    case VRRh:
        // V1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // V2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        // M3:
        if(!m3_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        break;
    case VRRi:
        // R1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // V2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // M3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        // M4:
        if(!m4_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        break;
    case VRRj:
        // V1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // V2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // V3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // M4:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        break;
    case VRRk:
        // V1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // V2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // M3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        break;
    default:
        break;
    }
    return OK;
}
