#include "InstructionTable.h"
#include "HLASMDisassembler.h"

ErrorCode assemble_RSL(size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    InstructionFormat format = INSTRUCTION_TABLE[table_index].format;
    uint16_t d1_d2 = 0;
    uint8_t l1_l2 = 0;
    uint8_t b1_b2 = 0;
    uint8_t r1 = 0;
    uint8_t m3 = 0;
    char buffer[MAX_OPERANDS_LEN];
    size_t i;
    size_t operands_token_len = strlen(operands_token) + 1;
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state;
    switch (format){
    case RSLa:
        state = D1;
        break;
    case RSLb:
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
                    state = D2;
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
        case D1:
        case D2:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&d1_d2, sizeof(d1_d2), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case RSLa:
                        state = L1;
                        break;
                    case RSLb:
                        state = L2;
                        break;
                    default:
                        break;
                    }
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_3CHR_LEN){
                    Context.error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                    switch(format){
                    case RSLa:
                        strcpy((char*)&Context.msg_extras[1], "D1");
                        break;
                    case RSLb:
                        strcpy((char*)&Context.msg_extras[1], "D2");
                        break;
                    default:
                        break;
                    }
                    sprintf((char*)&Context.msg_extras[2], "%d", MAX_3CHR_LEN);
                    return Context.error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case L1:
        case L2:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&l1_l2, sizeof(l1_l2), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case RSLa:
                        state = B1;
                        break;
                    case RSLb:
                        state = B2;
                        break;
                    default:
                        break;
                    }
                    i++;
                }
            }
            else{
                switch (format){
                case RSLa:
                    if(b_idx >= MAX_1CHR_LEN){
                        Context.error_code = INVALID_OPERAND_LENGTH;
                        sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                        strcpy((char*)&Context.msg_extras[1], "L1");
                        sprintf((char*)&Context.msg_extras[2], "%d", MAX_1CHR_LEN);
                        return Context.error_code;
                    }
                    break;
                case RSLb:
                    if(b_idx >= MAX_2CHR_LEN){
                        Context.error_code = INVALID_OPERAND_LENGTH;
                        sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                        strcpy((char*)&Context.msg_extras[1], "L2");
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
        case B1:
        case B2:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&b1_b2, sizeof(b1_b2), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case RSLa:
                        state = OPS_DONE;
                        break;
                    case RSLb:
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
                    switch(format){
                    case RSLa:
                        strcpy((char*)&Context.msg_extras[1], "B1");
                        break;
                    case RSLb:
                        strcpy((char*)&Context.msg_extras[1], "B2");
                        break;
                    default:
                        break;
                    }
                    sprintf((char*)&Context.msg_extras[2], "%d", MAX_3CHR_LEN);
                    return Context.error_code;
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
                    Context.error_code = OPERAND_NON_HEX_FOUND;
                    sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                    strcpy((char*)&Context.msg_extras[1], operands_token);
                    return Context.error_code;
                }
                else{
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&m3, sizeof(m3), b_idx, NO_SKIP, true);
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
                    strcpy((char*)&Context.msg_extras[1], "M3");
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
    switch (format){
    case RSLa:
        // L1: bits(8-11)
        bin_buffer[1] = l1_l2 << 4;
        break;
    case RSLb:
        // L2: bits(8-15)
        bin_buffer[1] = l1_l2;
        break;
    default:
        break;
    }
    // B2: bits(8-11)
    bin_buffer[2] = b1_b2 << 4;
    // D2: bits(20-31)
    bin_buffer[2] = bin_buffer[2] | (d1_d2 >> 8);
    bin_buffer[3] = d1_d2 & 0x0FF;
    switch (format){
    case RSLb:
        // R1: bits(20-31)
        bin_buffer[4] = r1 << 4;
        // M3: bits(20-31)
        bin_buffer[4] = bin_buffer[4] | m3;
        break;
    default:
        break;
    }
    // Opcode (part 2): bits(40-47)
    bin_buffer[5] = opcode & 0x00FF;
    return OK;
}

ErrorCode display_RSL(Instruction* instr){
    if(instr == NULL){
        Context.error_code = NULL_POINTER_TO_OBJECT;
        return Context.error_code;
    }
    uint16_t opcode = INSTRUCTION_TABLE[instr->it_index].opcode;
    uint8_t length = INSTRUCTION_TABLE[instr->it_index].length;
    InstructionFormat format = INSTRUCTION_TABLE[instr->it_index].format;
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    switch (format){
    case RSLa:
        printf("+--------+----+----+----+------------+--------+--------+\n");
        printf("| OPCODE | L1 | // | B1 |     D1     | ////// | OPCODE |\n");
        printf("+--------+----+----+----+------------+--------+--------+\n");
        printf("0        8    C    10   14           20       28      2F\n");
        break;
    case RSLb:
        printf("+--------+--------+----+------------+----+----+--------+\n");
        printf("| OPCODE |   L2   | B4 |     D4     | R1 | M3 | OPCODE |\n");
        printf("+--------+--------+----+------------+----+----+--------+\n");
        printf("0        8        10   14           20   24   28      2F\n");
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
    case RSLa:
        printf("FORMAT:   RSLa\n");
        break;
    case RSLb:
        printf("FORMAT:   RSLa\n");
        break;
    default:
        break;
    }
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    switch (format){
    case RSLa:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("L1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("B1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_3CHR_LEN, SKIP, false);
        printf("D1:       %s\n", conv_buffer);
        break;
    case RSLb:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        printf("L2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("B2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_3CHR_LEN, SKIP, false);
        printf("D2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("R1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
        printf("M3:       %s\n", conv_buffer);
        break;
    default:
        break;
    }
    return OK;
}

ErrorCode disassemble_RSL(size_t table_index, const uint8_t* bin_buffer, char* operands_token){
    char buffer[MAX_OPERANDS_LEN];
    InstructionFormat format = INSTRUCTION_TABLE[table_index].format;
    size_t i = 0;
    switch (format){
    case RSLa:
        // D1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        // L1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = '(';
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // B1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ')';
        break;
    case RSLb:
        // R1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // D2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        // L2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        operands_token[i++] = '(';
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = ',';
        // B2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ')';
        operands_token[i++] = ',';
        // M3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        break;
    default:
        break;
    }
    return OK;
}