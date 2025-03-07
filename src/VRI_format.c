#include "InstructionTable.h"
#include "HLASMDisassembler.h"

ErrorCode assemble_VRI(size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    InstructionFormat format = INSTRUCTION_TABLE[table_index].format;
    bool m3_unused = INSTRUCTION_TABLE[table_index].unused_operands & M3_UNUSED;
    bool m5_unused = INSTRUCTION_TABLE[table_index].unused_operands & M5_UNUSED;
    uint8_t v1 = 0;
    uint8_t v2_r2 = 0;
    uint8_t v3 = 0;
    uint16_t i2 = 0;
    uint16_t i3 = 0;
    uint8_t i4 = 0;
    uint8_t m3_m4 = 0;
    uint8_t m5 = 0;
    char buffer[MAX_OPERANDS_LEN];
    size_t i;
    size_t operands_token_len = strlen(operands_token) + 1;
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state = V1;
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    for(i = 0; i < operands_token_len && run;){
        switch (state){
        case V1:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&v1, sizeof(v1), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case VRIa:
                    case VRIb:
                    case VRIh:
                        state = I2;
                        break;
                    case VRIc:
                        state = V3;
                        break;
                    case VRId:
                    case VRIe:
                    case VRIf:
                    case VRIg:
                        state = V2;
                        break;
                    case VRIi:
                        state = R2;
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
                    strcpy((char*)&Context.msg_extras[1], "V1");
                    sprintf((char*)&Context.msg_extras[2], "%d", MAX_1CHR_LEN);
                    return Context.error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case V2:
        case R2:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&v2_r2, sizeof(v2_r2), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case VRId:
                    case VRIf:
                        state = V3;
                        break;
                    case VRIe:
                    case VRIg:
                    case VRIi:
                        state = I3;
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
                    strcpy((char*)&Context.msg_extras[1], "V2");
                    sprintf((char*)&Context.msg_extras[2], "%d", MAX_1CHR_LEN);
                    return Context.error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case V3:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&v3, sizeof(v3), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case VRIc:
                        state = I2;
                        break;
                    case VRId:
                    case VRIf:
                        state = I4;
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
                    strcpy((char*)&Context.msg_extras[1], "V3");
                    sprintf((char*)&Context.msg_extras[2], "%d", MAX_1CHR_LEN);
                    return Context.error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case I2:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i2, sizeof(i2), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case VRIa:
                        if(m3_unused){
                            state = OPS_DONE;
                        }
                        else{
                            state = M3;
                        }
                        break;
                    case VRIb:
                    case VRIh:
                        state = I3;
                        break;
                    case VRIc:
                        state = M4;
                        break;
                    default:
                        break;
                    }
                    i++;
                }
            }
            else{
                switch (format){
                case VRIa:
                case VRIc:
                    if(b_idx >= MAX_4CHR_LEN){
                        Context.error_code = INVALID_OPERAND_LENGTH;
                        sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                        strcpy((char*)&Context.msg_extras[1], "I2");
                        sprintf((char*)&Context.msg_extras[2], "%d", MAX_4CHR_LEN);
                        return Context.error_code;
                    }
                    break;
                case VRIb:
                    if(b_idx >= MAX_2CHR_LEN){
                        Context.error_code = INVALID_OPERAND_LENGTH;
                        sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                        strcpy((char*)&Context.msg_extras[1], "I2");
                        sprintf((char*)&Context.msg_extras[2], "%d", MAX_2CHR_LEN);
                        return Context.error_code;
                    }
                    break;
                default:
                    break;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case I3:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i3, sizeof(i3), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case VRIb:
                    case VRIe:
                    case VRIi:
                        state = M4;
                        break;
                    case VRIg:
                        state = I4;
                        break;
                    case VRIh:
                        state = OPS_DONE;
                        break;
                    default:
                        break;
                    }
                    i++;
                }
            }
            else{
                switch (format){
                case VRIb:
                case VRIg:
                case VRIi:
                    if(b_idx >= MAX_2CHR_LEN){
                        Context.error_code = INVALID_OPERAND_LENGTH;
                        sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                        strcpy((char*)&Context.msg_extras[1], "I3");
                        sprintf((char*)&Context.msg_extras[2], "%d", MAX_2CHR_LEN);
                        return Context.error_code;
                    }
                    break;
                case VRIe:
                    if(b_idx >= MAX_3CHR_LEN){
                        Context.error_code = INVALID_OPERAND_LENGTH;
                        sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                        strcpy((char*)&Context.msg_extras[1], "I3");
                        sprintf((char*)&Context.msg_extras[2], "%d", MAX_3CHR_LEN);
                        return Context.error_code;
                    }
                    break;
                case VRIh:
                    if(b_idx >= MAX_1CHR_LEN){
                        Context.error_code = INVALID_OPERAND_LENGTH;
                        sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                        strcpy((char*)&Context.msg_extras[1], "I3");
                        sprintf((char*)&Context.msg_extras[2], "%d", MAX_1CHR_LEN);
                        return Context.error_code;
                    }
                    break;
                default:
                    break;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case I4:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i4, sizeof(i4), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case VRId:
                        if(m5_unused){
                            state = OPS_DONE;
                        }
                        else{
                            state = M5;
                        }
                        break;
                    case VRIf:
                    case VRIg:
                        state = M5;
                        break;
                    default:
                        break;
                    }
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_2CHR_LEN){
                    Context.error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                    strcpy((char*)&Context.msg_extras[1], "I4");
                    sprintf((char*)&Context.msg_extras[2], "%d", MAX_2CHR_LEN);
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case M3:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&m3_m4, sizeof(m3_m4), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case VRIa:
                    case VRIb:
                    case VRIc:
                    case VRIi:
                        state = OPS_DONE;
                        break;                
                    case VRIe:
                        state = M5;
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
                    switch (state){
                    case M3:
                        strcpy((char*)&Context.msg_extras[1], "M3");
                        break;                
                    case M4:
                        strcpy((char*)&Context.msg_extras[1], "M4");
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
        case M5:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&m5, sizeof(m5), b_idx, NO_SKIP, true);
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
                    strcpy((char*)&Context.msg_extras[1], "M5");
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
    // Opcode (part 1): bits(0-7)
    bin_buffer[0] = opcode >> 8;
    // V1: bits(8-11)
    bin_buffer[1] = v1 << 4;
    // V2/R2:
    switch (format){
    case VRId:
    case VRIe:
    case VRIf:
    case VRIg:
    case VRIi:
        // V2/R2: bits(12-15)
        bin_buffer[1] = bin_buffer[1] | v2_r2;
        break;
    default:
        break;
    }
    // V3:
    switch (format){
    case VRIc:
        // V3: bits(12-15)
        bin_buffer[1] = bin_buffer[1] | v3;
        break;
    case VRId:
    case VRIf:
        // V3: bits(16-19)
        bin_buffer[2] = v3 << 4;
        break;
    default:
        break;
    }
    // I2:
    switch (format){
    case VRIa:
    case VRIc:
    case VRIh:
        // I2: bits(16-31)
        bin_buffer[2] = i2 >> 8;
        bin_buffer[3] = bin_buffer[3] | (i2 & 0x00FF);
        break;
    case VRIb:
        // I2: bits(16-23)
        bin_buffer[2] = i2;
        break;
    default:
        break;
    }
    // I3:
    switch (format){
    case VRIb:
        // I3: bits(24-31)
        bin_buffer[3] = i3;
        break;
    case VRIe:
        // I3: bits(16-27)
        bin_buffer[2] = i3 >> 4;
        bin_buffer[3] = (i3 & 0x000F) << 4;
        break;
    case VRIg:
    case VRIi:
        // I3: bits(28-35)
        bin_buffer[3] = i3 >> 4;
        bin_buffer[4] = (i3 & 0x0F) << 4;
    case VRIh:
        // I3: bits(32-35)
        bin_buffer[4] = i3 << 4;
    default:
        break;
    }
    // I4:
    switch (format){
    case VRId:
        // I4: bits(24-31)
        bin_buffer[3] = i4;
        break;
    case VRIf:
        // I4: bits(28-35)
        bin_buffer[3] = i4 >> 4;
        bin_buffer[4] = (i4 & 0x0F) << 4;
        break;
    case VRIg:
        // I4: bits(16-23)
        bin_buffer[2] = i4;
        break;
    default:
        break;
    }
    // M3/M4/M5:
    switch (format){
    case VRIa:
        // M3: bits(32-35)
        if(!m3_unused){
            bin_buffer[4] = m3_m4 << 4;
        }
        break;
    case VRIb:
    case VRIc:
        // M4: bits(32-35)
        bin_buffer[4] = m3_m4 << 4;
        break;
    case VRId:
        // M5: bits(32-35)
        bin_buffer[4] = m5 << 4;
        break;
    case VRIe:
        // M5: bits(28-31)
        bin_buffer[3] = bin_buffer[3] | m5;
        // M4: bits(32-35)
        bin_buffer[4] = m3_m4 << 4;
        break;
    case VRIf:
    case VRIg:
        // M5: bits(24-27)
        bin_buffer[3] = bin_buffer[3] | (m5 << 4);
        break;
    case VRIi:
        // M4: bits(24-27)
        bin_buffer[3] = bin_buffer[3] | (m3_m4 << 4);
        break;
    default:
        break;
    }
    // Opcode (part 2): bits(40-47)
    bin_buffer[5] = opcode & 0x00FF;
    return OK;
}

ErrorCode display_VRI(Instruction* instr){
    if(instr == NULL){
        Context.error_code = NULL_POINTER_TO_OBJECT;
        return Context.error_code;
    }
    uint16_t opcode = INSTRUCTION_TABLE[instr->it_index].opcode;
    uint8_t length = INSTRUCTION_TABLE[instr->it_index].length;
    InstructionFormat format = INSTRUCTION_TABLE[instr->it_index].format;
    bool m3_unused = INSTRUCTION_TABLE[instr->it_index].unused_operands & M3_UNUSED;
    bool m5_unused = INSTRUCTION_TABLE[instr->it_index].unused_operands & M5_UNUSED;
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    switch (format){
    case VRIa:
        if(m3_unused){
            printf("+--------+----+----+----------------+----+----+--------+\n");
            printf("| OPCODE | V1 | // |       I2       | // | RX | OPCODE |\n");
            printf("+--------+----+----+----------------+----+----+--------+\n");
            printf("0        8    C    10               20   24   28      2F\n");
        }
        else{
            printf("+--------+----+----+----------------+----+----+--------+\n");
            printf("| OPCODE | V1 | // |       I2       | M3 | RX | OPCODE |\n");
            printf("+--------+----+----+----------------+----+----+--------+\n");
            printf("0        8    C    10               20   24   28      2F\n");
        }
        break;
    case VRIb:
        printf("+--------+----+----+--------+--------+----+----+--------+\n");
        printf("| OPCODE | V1 | // |   I2   |   I3   | M4 | RX | OPCODE |\n");
        printf("+--------+----+----+--------+--------+----+----+--------+\n");
        printf("0        8    C    10       18       20   24   28      2F\n");
        break;
    case VRIc:
        printf("+--------+----+----+----------------+----+----+--------+\n");
        printf("| OPCODE | V1 | V3 |       I2       | M4 | RX | OPCODE |\n");
        printf("+--------+----+----+----------------+----+----+--------+\n");
        printf("0        8    C    10               20   24   28      2F\n");
        break;
    case VRId:
        if(m5_unused){
            printf("+--------+----+----+----+----+--------+----+----+--------+\n");
            printf("| OPCODE | V1 | V2 | V3 | // |   I4   | // | RX | OPCODE |\n");
            printf("+--------+----+----+----+----+--------+----+----+--------+\n");
            printf("0        8    C    10   14   18       20   24   28      2F\n");
        }
        else{
            printf("+--------+----+----+----+----+--------+----+----+--------+\n");
            printf("| OPCODE | V1 | V2 | V3 | // |   I4   | M5 | RX | OPCODE |\n");
            printf("+--------+----+----+----+----+--------+----+----+--------+\n");
            printf("0        8    C    10   14   18       20   24   28      2F\n");
        }
        break;
    case VRIe:
        printf("+--------+----+----+------------+----+----+----+--------+\n");
        printf("| OPCODE | V1 | V2 |     I3     | M5 | M4 | RX | OPCODE |\n");
        printf("+--------+----+----+------------+----+----+----+--------+\n");
        printf("0        8    C    10           18   20   24   28      2F\n");
        break;
    case VRIf:
        printf("+--------+----+----+----+----+----+--------+----+--------+\n");
        printf("| OPCODE | V1 | V2 | V3 | // | M5 |   I4   | RX | OPCODE |\n");
        printf("+--------+----+----+----+----+----+--------+----+--------+\n");
        printf("0        8    C    10   14   18   1C       24   28      2F\n");
        break;
    case VRIg:
        printf("+--------+----+----+--------+----+--------+----+--------+\n");
        printf("| OPCODE | V1 | V2 |   I4   | M5 |   I3   | RX | OPCODE |\n");
        printf("+--------+----+----+--------+----+--------+----+--------+\n");
        printf("0        8    C    10       18   1C       24   28      2F\n");
        break;
    case VRIh:
        printf("+--------+----+----+----------------+----+----+--------+\n");
        printf("| OPCODE | V1 | // |       I2       | I3 | RX | OPCODE |\n");
        printf("+--------+----+----+----------------+----+----+--------+\n");
        printf("0        8    C    10               20   24   28      2F\n");
        break;
    case VRIi:
        printf("+--------+----+----+--------+----+--------+----+--------+\n");
        printf("| OPCODE | V1 | R2 | ////// | M4 |   I3   | RX | OPCODE |\n");
        printf("+--------+----+----+--------+----+--------+----+--------+\n");
        printf("0        8    C    10       18   1C       24   28      2F\n");
        break;
    default:
        break;
    }
    // Print general information
    printf("MNEMONIC: %s\n", INSTRUCTION_TABLE[instr->it_index].mnemonic);
    printf("OPERANDS: %s\n", instr->operands_txt);
    hex_str_2_char_str((void*)&opcode, sizeof(opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_4CHR_LEN, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_12CHR_LEN, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", length);
    switch (format){
    case VRIa:
        printf("FORMAT:   VRIa\n");
        break;
    case VRIb:
        printf("FORMAT:   VRIb\n");
        break;
    case VRIc:
        printf("FORMAT:   VRIc\n");
        break;
    case VRId:
        printf("FORMAT:   VRId\n");
        break;
    case VRIe:
        printf("FORMAT:   VRIe\n");
        break;
    case VRIf:
        printf("FORMAT:   VRIf\n");
        break;
    case VRIg:
        printf("FORMAT:   VRIg\n");
        break;
    case VRIh:
        printf("FORMAT:   VRIh\n");
        break;
    case VRIi:
        printf("FORMAT:   VRIi\n");
        break;
    default:
        break;
    }
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
    printf("V1:       %s\n", conv_buffer);
    switch (format){
    case VRIa:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_4CHR_LEN, NO_SKIP, false);
        printf("I2:       %s\n", conv_buffer);
        if(!m3_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
            printf("M3:       %s\n", conv_buffer);
        }
        break;
    case VRIb:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        printf("I2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        printf("I3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("M4:       %s\n", conv_buffer);
        break;
    case VRIc:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
        printf("V3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_4CHR_LEN, NO_SKIP, false);
        printf("I2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("M4:       %s\n", conv_buffer);
        break;
    case VRId:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
        printf("V2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("V3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        printf("I4:       %s\n", conv_buffer);
        if(!m5_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
            printf("M5:       %s\n", conv_buffer);
        }
        break;
    case VRIe:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
        printf("V2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_3CHR_LEN, NO_SKIP, false);
        printf("I3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
        printf("M5:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("M4:       %s\n", conv_buffer);
        break;
    case VRIf:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
        printf("V2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("V3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("M5:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_2CHR_LEN, SKIP, false);
        printf("I4:       %s\n", conv_buffer);
        break;
    case VRIg:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
        printf("V2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        printf("I4:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("M5:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_2CHR_LEN, SKIP, false);
        printf("I3:       %s\n", conv_buffer);
        break;
    case VRIh:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_4CHR_LEN, NO_SKIP, false);
        printf("I2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("I3:       %s\n", conv_buffer);
        break;
    case VRIi:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
        printf("R2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("M4:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_2CHR_LEN, SKIP, false);
        printf("I3:       %s\n", conv_buffer);
        break;
    default:
        break;
    }
    return OK;
}

ErrorCode disassemble_VRI(size_t table_index, const uint8_t* bin_buffer, char* operands_token){
    char buffer[MAX_OPERANDS_LEN];
    InstructionFormat format = INSTRUCTION_TABLE[table_index].format;
    bool m3_unused = INSTRUCTION_TABLE[table_index].unused_operands & M3_UNUSED;
    bool m5_unused = INSTRUCTION_TABLE[table_index].unused_operands & M5_UNUSED;
    size_t i = 0;
    // V1:
    memset(&buffer, 0, sizeof(buffer));
    hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
    operands_token[i++] = buffer[0];
    operands_token[i++] = ',';
    switch (format){
    case VRIa:
        // I2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_4CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        operands_token[i++] = buffer[3];
        // M3:
        if(!m3_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        break;
    case VRIb:
        // I2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = ',';
        // I3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = ',';
        // M4:
        if(!m3_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
            operands_token[i++] = buffer[0];
        }
        break;
    case VRIc:
        // V3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // I2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_4CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        operands_token[i++] = buffer[3];
        operands_token[i++] = ',';
        // M4:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        break;
    case VRId:
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
        // I4:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        // M5:
        if(!m5_unused){
            memset(&buffer, 0, sizeof(buffer));
            hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
            operands_token[i++] = ',';
            operands_token[i++] = buffer[0];
        }
        break;
    case VRIe:
        // V2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // I3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        operands_token[i++] = ',';
        // M4:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // M5:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        break;
    case VRIf:
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
        // I4:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = ',';
        // M5:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        break;
    case VRIg:
        // V2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // I3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = ',';
        // I4:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = ',';
        // M5:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        break;
    case VRIh:
        // I2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_4CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        operands_token[i++] = buffer[3];
        operands_token[i++] = ',';
        // I3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];    
        break;
    case VRIi:
        // R2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // I3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = ',';
        // M4:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        break;
    default:
        break;
    }
    return OK;
}