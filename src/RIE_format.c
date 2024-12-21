#include "InstructionTable.h"
#include "HLASMCompiler.h"

ErrorCode build_RIE(Context* c, size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    InstructionFormat format = INSTRUCTION_TABLE[table_index].format;
    uint8_t r1 = 0; 
    uint8_t r2 = 0; 
    uint8_t r3_m3 = 0; 
    uint8_t i2 = 0; 
    uint8_t i3 = 0; 
    uint8_t i4 = 0; 
    uint8_t i5 = 0; 
    uint16_t i2_ri2_ri4 = 0; 
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
                    c->error_code = OPERAND_NON_HEX_FOUND;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], operands_token);
                    return c->error_code;
                }
                else{
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r1, sizeof(r1), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case RIEa:
                    case RIEc:
                    case RIEg:
                        state = I2;
                        break;
                    case RIEb:
                    case RIEf:
                        state = R2;
                        break;
                    case RIEd:
                    case RIEe:
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
                    strcpy((char*)&c->msg_extras[1], "R1");
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_1CHR_LEN);
                    return c->error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case R2:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r2, sizeof(r2), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case RIEb:
                        state = M3;
                        break;
                    case RIEf:
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
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], "R2");
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_1CHR_LEN);
                    return c->error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case I2:
        case RI2:
        case RI4:
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
                    switch (format){
                    case RIEa:
                    case RIEb:
                    case RIEd:
                    case RIEe:
                    case RIEg:
                        char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i2_ri2_ri4, sizeof(i2_ri2_ri4), b_idx, NO_SKIP, true);
                        break;
                    case RIEc:
                        if(state == RI4){
                            char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i2_ri2_ri4, sizeof(i2_ri2_ri4), b_idx, NO_SKIP, true);
                        }
                        else{
                            char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i2, sizeof(i2), b_idx, NO_SKIP, true);
                        }
                        break;
                    default:
                        break;
                    }
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case RIEa:
                    case RIEg:
                        state = M3;
                        break;
                    case RIEc:
                        if(state == RI4){
                            state = OPS_DONE;
                        }
                        else{
                            state = M3;
                        }
                        break;
                    case RIEb:
                    case RIEd:
                    case RIEe:
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
                case RIEa:
                case RIEd:
                case RIEe:
                case RIEg:
                    if(b_idx >= MAX_4CHR_LEN){
                        c->error_code = INVALID_OPERAND_LENGTH;
                        sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                        switch(format){
                        case RIEe:
                            strcpy((char*)&c->msg_extras[1], "RI2");
                            break;
                        default:
                            strcpy((char*)&c->msg_extras[1], "I2");
                            break;
                        }
                        sprintf((char*)&c->msg_extras[2], "%d", MAX_4CHR_LEN);
                        return c->error_code;
                    }
                    break;
                case RIEb:
                case RIEc:
                    if(state == RI4){ 
                        if(b_idx >= MAX_4CHR_LEN){
                            c->error_code = INVALID_OPERAND_LENGTH;
                            sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                            strcpy((char*)&c->msg_extras[1], "RI4");
                            sprintf((char*)&c->msg_extras[2], "%d", MAX_4CHR_LEN);
                            return c->error_code;
                        }
                    }
                    else{
                        if(b_idx >= MAX_2CHR_LEN){
                            c->error_code = INVALID_OPERAND_LENGTH;
                            sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                            strcpy((char*)&c->msg_extras[1], "I2");
                            sprintf((char*)&c->msg_extras[2], "%d", MAX_2CHR_LEN);
                            return c->error_code;
                        }
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
        case R3:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r3_m3, sizeof(r3_m3), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case RIEa:
                    case RIEg:
                        state = OPS_DONE;
                        break;
                    case RIEb:
                    case RIEc:
                        state = RI4;
                        break;
                    case RIEd:
                        state = I2;
                        break;
                    case RIEe:
                        state = RI2;
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
                    switch(format){
                    case RIEa:
                    case RIEb:
                    case RIEc:
                    case RIEg:
                        strcpy((char*)&c->msg_extras[1], "M3");
                        break;
                    case RIEd:
                    case RIEe:
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
        case I3:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i3, sizeof(i3), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    state = I4;
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_2CHR_LEN){
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], "I3");
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_2CHR_LEN);
                    return c->error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case I4:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i4, sizeof(i4), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    state = I5;
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_2CHR_LEN){
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], "I4");
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_2CHR_LEN);
                    return c->error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case I5:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i5, sizeof(i5), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    state = OPS_DONE;
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_2CHR_LEN){
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], "I5");
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_2CHR_LEN);
                    return c->error_code;
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
    // R1/M1: bits(8-11)
    bin_buffer[1] = r1 << 4;
    // R2/I2/RI2: 
    switch (format){
    case RIEa:        
    case RIEd:        
    case RIEe:        
    case RIEg:        
        // I2: bits(16-31)
        bin_buffer[2] = i2_ri2_ri4 >> 8;
        bin_buffer[3] = bin_buffer[3] | (i2_ri2_ri4 & 0x00FF);
        break;
    case RIEb:
    case RIEf:
        // R2: bits(12-15)
        bin_buffer[1] = bin_buffer[1] | r2;
        break;
    case RIEc:
        // I2: bits(32-39)
        bin_buffer[4] = i2;
        break;        
    default:
        break;
    }
    // R3/M3/I3:
    switch (format){
    case RIEa:        
    case RIEb:        
        // M3: bits(32-35)
        bin_buffer[4] = r3_m3 << 4;
        break;
    case RIEc:        
    case RIEd:        
    case RIEe:        
    case RIEg:        
        // R3/M3: bits(12-15)
        bin_buffer[1] = bin_buffer[1] | r3_m3;
        break;
    case RIEf:        
        // I3: bits(16-23)
        bin_buffer[2] = i3;
        break;
    default:
        break;
    }
    // RI4/I4: 
    switch (format){
    case RIEb:        
    case RIEc:        
        // RI4: bits(32-35)
        bin_buffer[2] = i2_ri2_ri4 >> 8;
        bin_buffer[3] = bin_buffer[3] | (i2_ri2_ri4 & 0x00FF);
        break;
    case RIEf:        
        // I4: bits(24-31)
        bin_buffer[3] = i4;
        break;
    default:
        break;
    }
    // I5:  
    switch (format){
    case RIEf:        
        // I4: bits(32-39)
        bin_buffer[4] = i5;
        break;
    default:
        break;
    }
    // Opcode (part 2): bits(40-47)
    bin_buffer[5] = opcode & 0x00FF;
    return OK;
}

ErrorCode display_RIE(Context* c, Instruction* instr){
    if(instr == NULL){
        c->error_code = NULL_POINTER_TO_OBJECT;
        return c->error_code;
    }
    uint16_t opcode = INSTRUCTION_TABLE[instr->it_index].opcode;
    uint8_t length = INSTRUCTION_TABLE[instr->it_index].length;
    InstructionFormat format = INSTRUCTION_TABLE[instr->it_index].format;
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    switch (format){
    case RIEa:
        printf("+--------+----+----+----------------+----+----+--------+\n");
        printf("| OPCODE | R1 | // |       I2       | M3 | // | OPCODE |\n");
        printf("+--------+----+----+----------------+----+----+--------+\n");
        printf("0        8    C    10               20   24   28      2F\n");
        break;
    case RIEb:
        printf("+--------+----+----+----------------+----+----+--------+\n");
        printf("| OPCODE | R1 | R2 |      RI4       | M3 | // | OPCODE |\n");
        printf("+--------+----+----+----------------+----+----+--------+\n");
        printf("0        8    C    10               20   24   28      2F\n");
        break;
    case RIEc:
        printf("+--------+----+----+----------------+--------+--------+\n");
        printf("| OPCODE | R1 | M3 |      RI4       |   I2   | OPCODE |\n");
        printf("+--------+----+----+----------------+--------+--------+\n");
        printf("0        8    C    10               20       28      2F\n");
        break;
    case RIEd:
        printf("+--------+----+----+----------------+--------+--------+\n");
        printf("| OPCODE | R1 | R3 |       I2       | ////// | OPCODE |\n");
        printf("+--------+----+----+----------------+--------+--------+\n");
        printf("0        8    C    10               20       28      2F\n");
        break;
    case RIEe:
        printf("+--------+----+----+----------------+--------+--------+\n");
        printf("| OPCODE | R1 | R3 |      RI2       | ////// | OPCODE |\n");
        printf("+--------+----+----+----------------+--------+--------+\n");
        printf("0        8    C    10               20       28      2F\n");
        break;
    case RIEf:
        printf("+--------+----+----+--------+--------+--------+--------+\n");
        printf("| OPCODE | R1 | R2 |   I3   |   I4   |   I5   | OPCODE |\n");
        printf("+--------+----+----+--------+--------+--------+--------+\n");
        printf("0        8    C    10       18       20       28      2F\n");
        break;
    case RIEg:
        printf("+--------+----+----+----------------+--------+--------+\n");
        printf("| OPCODE | R1 | M3 |       I2       | ////// | OPCODE |\n");
        printf("+--------+----+----+----------------+--------+--------+\n");
        printf("0        8    C    10               20       28      2F\n");
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
    case RIEa:
        printf("FORMAT:   RIEa\n");
        break;
    case RIEb:
        printf("FORMAT:   RIEb\n");
        break;
    case RIEc:
        printf("FORMAT:   RIEc\n");
        break;
    case RIEd:
        printf("FORMAT:   RIEd\n");
        break;
    case RIEe:
        printf("FORMAT:   RIEe\n");
        break;
    case RIEf:
        printf("FORMAT:   RIEf\n");
        break;
    case RIEg:
        printf("FORMAT:   RIEg\n");
        break;
    default:
        break;
    }
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
    printf("R1:       %s\n", conv_buffer);
    switch (format){
    case RIEa:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
        printf("I2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("M3:       %s\n", conv_buffer);
        break;
    case RIEb:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("R2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
        printf("RI4:      %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("M3:       %s\n", conv_buffer);
        break;
    case RIEc:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("M3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
        printf("RI4:      %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, false);
        printf("I2:       %s\n", conv_buffer);
        break;
    case RIEd:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("R3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
        printf("I2:       %s\n", conv_buffer);
        break;
    case RIEe:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("R3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
        printf("RI2       %s\n", conv_buffer);
        break;
    case RIEf:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("R2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, false);
        printf("I3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, false);
        printf("I4:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, false);
        printf("I5:       %s\n", conv_buffer);
        break;
    case RIEg:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("M3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
        printf("I2:       %s\n", conv_buffer);
        break;
        break;
    default:
        break;
    }
    return OK;
}


ErrorCode decode_RIE(){
    return OK;
}