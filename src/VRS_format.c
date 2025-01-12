#include "InstructionTable.h"
#include "HLASMCompiler.h"

ErrorCode assemble_VRS(Context* c, size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    InstructionFormat format = INSTRUCTION_TABLE[table_index].format;
    bool m4_unused = INSTRUCTION_TABLE[table_index].unused_operands & M4_UNUSED;
    uint8_t v1_r1 = 0;
    uint8_t v3_r3 = 0;
    uint16_t d2 = 0;
    uint8_t b2 = 0;
    uint8_t m4 = 0;
    char buffer[MAX_OPERANDS_LEN];
    size_t i;
    size_t operands_token_len = strlen(operands_token) + 1;
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state;
    switch (format){
    case VRSa:
    case VRSb:
    case VRSd:
        state = V1;
        break;
    case VRSc:
        state = R1;
        break;
    default:
        break;
    }
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    for(i = 0; i < operands_token_len && run;){
        switch (state){
        case V1:
        case R1:
            if(operands_token[i] == ','){
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
                    case VRSa:
                    case VRSc:
                        state = V3;
                        break;
                    case VRSb:
                    case VRSd:
                        state = R3;
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
        case V3:
        case R3:
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
                    state = D2;
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
        case D2:
            if(operands_token[i] == ','){
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&d2, sizeof(d2), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    state = B2;
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_3CHR_LEN){
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], "D2");
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_3CHR_LEN);
                    return c->error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case B2:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&b2, sizeof(b2), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case VRSa:
                    case VRSb:
                    case VRSc:
                        if(m4_unused){
                            state = OPS_DONE;
                        }
                        else{
                            state = M4;
                        }
                        break;
                    case VRSd:
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
                    strcpy((char*)&c->msg_extras[1], "B2");
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_1CHR_LEN);
                    return c->error_code;
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
                    c->error_code = OPERAND_NON_HEX_FOUND;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], operands_token);
                    return c->error_code;
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
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], "M4");
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
    case VRSa:
    case VRSb:
    case VRSc:
        // V1/R1: bits(8-11)
        bin_buffer[1] = v1_r1 << 4;
        break;
    case VRSd:
        // V1: bits(32-35)
        bin_buffer[4] = v1_r1 << 4;
        break;
    default:
        break;
    }
    // V3/R3: bits(12-15)
    bin_buffer[1] = bin_buffer[1] | v3_r3;
    // B2: bits(16-19)
    bin_buffer[2] = b2 << 4;
    // D2: bits(20-31)
    bin_buffer[2] = bin_buffer[2] | (d2 >> 8);
    bin_buffer[3] = d2 & 0x0FF;
    // M4:
    switch (format){
    case VRSa:
    case VRSb:
    case VRSc:
        // M4: bits(32-35)
        if(!m4_unused){
            bin_buffer[4] = m4 << 4;
        }
        break;
    default:
        break;
    }
    // Opcode (part 2): bits(40-47)
    bin_buffer[5] = opcode & 0x00FF;
    return OK;
}

ErrorCode display_VRS(Context* c, Instruction* instr){
    if(instr == NULL){
        c->error_code = NULL_POINTER_TO_OBJECT;
        return c->error_code;
    }
    uint16_t opcode = INSTRUCTION_TABLE[instr->it_index].opcode;
    uint8_t length = INSTRUCTION_TABLE[instr->it_index].length;
    InstructionFormat format = INSTRUCTION_TABLE[instr->it_index].format;
    bool m4_unused = INSTRUCTION_TABLE[instr->it_index].unused_operands & M4_UNUSED;
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    switch (format){
    case VRSa:
        if(m4_unused){
            printf("+--------+----+----+----+------------+----+----+--------+\n");
            printf("| OPCODE | V1 | V3 | B2 |     D2     | // | RX | OPCODE |\n");
            printf("+--------+----+----+----+------------+----+----+--------+\n");
            printf("0        8    C    10   14           20   24   28      2F\n");
        }
        else{
            printf("+--------+----+----+----+------------+----+----+--------+\n");
            printf("| OPCODE | V1 | V3 | B2 |     D2     | M4 | RX | OPCODE |\n");
            printf("+--------+----+----+----+------------+----+----+--------+\n");
            printf("0        8    C    10   14           20   24   28      2F\n");
        }
        break;
    case VRSb:
        if(m4_unused){
            printf("+--------+----+----+----+------------+----+----+--------+\n");
            printf("| OPCODE | V1 | R3 | B2 |     D2     | // | RX | OPCODE |\n");
            printf("+--------+----+----+----+------------+----+----+--------+\n");
            printf("0        8    C    10   14           20   24   28      2F\n");
        }
        else{
            printf("+--------+----+----+----+------------+----+----+--------+\n");
            printf("| OPCODE | V1 | R3 | B2 |     D2     | M4 | RX | OPCODE |\n");
            printf("+--------+----+----+----+------------+----+----+--------+\n");
            printf("0        8    C    10   14           20   24   28      2F\n");
        }
        break;
    case VRSc:
        printf("+--------+----+----+----+------------+----+----+--------+\n");
        printf("| OPCODE | R1 | V3 | B2 |     D2     | M4 | RX | OPCODE |\n");
        printf("+--------+----+----+----+------------+----+----+--------+\n");
        printf("0        8    C    10   14           20   24   28      2F\n");
        break;
    case VRSd:
        printf("+--------+----+----+----+------------+----+----+--------+\n");
        printf("| OPCODE | // | R3 | B2 |     D2     | v1 | RX | OPCODE |\n");
        printf("+--------+----+----+----+------------+----+----+--------+\n");
        printf("0        8    C    10   14           20   24   28      2F\n");
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
    case VRSa:
        printf("FORMAT:   VRSa\n");
        break;
    case VRSb:
        printf("FORMAT:   VRSb\n");
        break;
    case VRSc:
        printf("FORMAT:   VRSc\n");
        break;
    case VRSd:
        printf("FORMAT:   VRSd\n");
        break;
    default:
        break;
    }
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    switch (format){
    case VRSa:
    case VRSb:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V1:       %s\n", conv_buffer);
        break;
    case VRSc:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("R1:       %s\n", conv_buffer);
        break;
    default:
        break;
    }
    switch (format){
    case VRSa:
    case VRSc:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("V3:       %s\n", conv_buffer);
        break;
    case VRSb:
    case VRSd:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("R3:       %s\n", conv_buffer);
        break;
    default:
        break;
    }
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
    printf("B2:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 3, SKIP, false);
    printf("D2:       %s\n", conv_buffer);
    switch (format){
    case VRSa:
    case VRSb:
    case VRSc:
        if(!m4_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
            printf("M4:       %s\n", conv_buffer);
        }
        break;
    case VRSd:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("V1:       %s\n", conv_buffer);
        break;
    default:
        break;
    }
    return OK;
}

ErrorCode disassemble_VRS(Context* c, size_t table_index, const uint8_t* bin_buffer, char* operands_token){
char buffer[MAX_OPERANDS_LEN];
    InstructionFormat format = INSTRUCTION_TABLE[table_index].format;
    bool m4_unused = INSTRUCTION_TABLE[table_index].unused_operands & M4_UNUSED;
    size_t i = 0;
    switch (format){
    case VRSa:
    case VRSb:
    case VRSc:
        // V1/R1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // V3/R3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // D2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        // B2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = '(';
        operands_token[i++] = buffer[0];
        operands_token[i++] = ')';
        // M4:
        if(!m4_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        break;
    case VRSd:
        // V1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // R3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // D2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        // B2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = '(';
        operands_token[i++] = buffer[0];
        operands_token[i++] = ')';
        break;
    default:
        break;
    }
    return OK;
}