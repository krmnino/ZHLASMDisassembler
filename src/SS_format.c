#include "InstructionTable.h"
#include "HLASMDisassembler.h"

ErrorCode assemble_SS(Context* c, size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    InstructionFormat format = INSTRUCTION_TABLE[table_index].format;
    uint8_t l1_r1 = 0;
    uint8_t l2 = 0;
    uint16_t d1 = 0;
    uint8_t b1 = 0;
    uint16_t d2 = 0;
    uint8_t b2 = 0;
    uint16_t d4 = 0;
    uint8_t b4 = 0;
    uint8_t i3_r3 = 0;
    char buffer[MAX_OPERANDS_LEN];
    size_t i;
    size_t operands_token_len = strlen(operands_token) + 1;
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state;
    switch (format){
    case SSa:
    case SSb:
    case SSc:
    case SSd:
    case SSf:
        state = D1;
        break;
    case SSe:
        state = R1;
        break;
    default:
        break;
    }
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    for(i = 0; i < operands_token_len && run;){
        switch (state){
        case D1:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&d1, sizeof(d1), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case SSa:
                    case SSb:
                    case SSc:
                        state = L1;
                        break;
                    case SSd:
                        state = R1;
                        break;
                    case SSf:
                        state = B1;
                        break;
                    default:
                        break;
                    }
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_3CHR_LEN){
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], "D1");
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_3CHR_LEN);
                    return c->error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case R1:
        case L1:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&l1_r1, sizeof(l1_r1), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case SSa:
                    case SSb:
                    case SSc:
                    case SSd:
                        state = B1;
                        break;
                    case SSe:
                        state = R3;
                        break;
                    default:
                        break;
                    }
                    i++;
                }
            }
            else{
                switch (format){
                case SSa:
                    if(b_idx >= MAX_2CHR_LEN){
                        c->error_code = INVALID_OPERAND_LENGTH;
                        sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                        strcpy((char*)&c->msg_extras[1], "L1");
                        sprintf((char*)&c->msg_extras[2], "%d", MAX_2CHR_LEN);
                        return c->error_code;
                    }
                    break;
                case SSb:
                case SSc:
                    if(b_idx >= MAX_1CHR_LEN){
                        c->error_code = INVALID_OPERAND_LENGTH;
                        sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                        strcpy((char*)&c->msg_extras[1], "L1");
                        sprintf((char*)&c->msg_extras[2], "%d", MAX_1CHR_LEN);
                        return c->error_code;
                    }
                    break;
                case SSd:
                case SSe:
                    if(b_idx >= MAX_1CHR_LEN){
                        c->error_code = INVALID_OPERAND_LENGTH;
                        sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                        strcpy((char*)&c->msg_extras[1], "R1");
                        sprintf((char*)&c->msg_extras[2], "%d", MAX_1CHR_LEN);
                        return c->error_code;
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&b1, sizeof(b1), b_idx, NO_SKIP, true);
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
                    strcpy((char*)&c->msg_extras[1], "B1");
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
                    switch (format){
                    case SSa:
                    case SSc:
                    case SSd:
                    case SSe:
                        state = B2;
                        break;
                    case SSb:
                    case SSf:
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
        case L2:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&l2, sizeof(l2), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    state = B2;
                    i++;
                }
            }
            else{
                switch (format){
                case SSb:
                    if(b_idx >= MAX_1CHR_LEN){
                        c->error_code = INVALID_OPERAND_LENGTH;
                        sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                        strcpy((char*)&c->msg_extras[1], "L2");
                        sprintf((char*)&c->msg_extras[2], "%d", MAX_1CHR_LEN);
                        return c->error_code;
                    }
                    break;
                case SSf:
                    if(b_idx >= MAX_2CHR_LEN){
                        c->error_code = INVALID_OPERAND_LENGTH;
                        sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                        strcpy((char*)&c->msg_extras[1], "L2");
                        sprintf((char*)&c->msg_extras[2], "%d", MAX_2CHR_LEN);
                        return c->error_code;
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
                    switch(format){
                    case SSa:
                    case SSb:
                    case SSf:
                        state = OPS_DONE;
                        break;
                    case SSc:
                        state = I3;
                        break;
                    case SSd:
                        state = R3;
                        break;
                    case SSe:
                        state = D4;
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
        case R3:
        case I3:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i3_r3, sizeof(i3_r3), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case SSa:
                    case SSb:
                    case SSc:
                    case SSd:
                        state = OPS_DONE;
                        break;
                    case SSe:
                        state = D2;
                        break;
                    default:
                        break;
                    }
                    i++;
                }
            }
            else{
                switch (format){
                case SSc:
                    if(b_idx >= MAX_1CHR_LEN){
                        c->error_code = INVALID_OPERAND_LENGTH;
                        sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                        strcpy((char*)&c->msg_extras[1], "I3");
                        sprintf((char*)&c->msg_extras[2], "%d", MAX_1CHR_LEN);
                        return c->error_code;
                    }
                    break;
                case SSd:
                case SSe:
                    if(b_idx >= MAX_1CHR_LEN){
                        c->error_code = INVALID_OPERAND_LENGTH;
                        sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                        strcpy((char*)&c->msg_extras[1], "R3");
                        sprintf((char*)&c->msg_extras[2], "%d", MAX_1CHR_LEN);
                        return c->error_code;
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
        case D4:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&d4, sizeof(d4), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    state = B4;
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_3CHR_LEN){
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], "D4");
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_3CHR_LEN);
                    return c->error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case B4:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&b4, sizeof(b4), b_idx, NO_SKIP, true);
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
                    strcpy((char*)&c->msg_extras[1], "B4");
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
    // Opcode: bits(0-7)
    bin_buffer[0] = opcode;
    // L1/R1:
    switch (format){
    case SSa:
        // L1: bits(8-15)
        bin_buffer[1] = l1_r1;
        break;
    case SSb:
    case SSc:
    case SSd:
    case SSe:
        // L1/R1: bits(8-11)
        bin_buffer[1] = l1_r1 << 4;
        break;
    default:
        break;
    }
    // L2: 
    switch (format){
    case SSb:
        // L2: bits(12-15)
        bin_buffer[1] = bin_buffer[1] | l2;
        break;
    case SSf:
        // L1: bits(8-15)
        bin_buffer[1] = l2;
    default:
        break;
    }
    // I3/R3: 
    switch (format){
    case SSc:
    case SSd:
    case SSe:
        // I3/R3: bits(12-15)
        bin_buffer[1] = bin_buffer[1] | i3_r3;
        break;
    default:
        break;
    }
    // B1/D1:
    switch (format){
    case SSa:
    case SSb:
    case SSc:
    case SSd:
    case SSf:
        // B1: bits(16-19)
        bin_buffer[2] = b1 << 4;
        // D1: bits(20-31)
        bin_buffer[2] = bin_buffer[2] | (d1 >> 8);
        bin_buffer[3] = d1 & 0x0FF;
        break;
    default:
        break;
    }
    // B2/D2:
    switch (format){
    case SSa:
    case SSb:
    case SSc:
    case SSd:
    case SSf:
        // B2: bits(32-35)
        bin_buffer[4] = b2 << 4;
        // D2: bits(36-47)
        bin_buffer[4] = bin_buffer[4] | (d2 >> 8);
        bin_buffer[5] = d2 & 0x0FF;
        break;
    case SSe:
        // B2: bits(16-19)
        bin_buffer[2] = b2 << 4;
        // D2: bits(20-31)
        bin_buffer[2] = bin_buffer[2] | (d2 >> 8);
        bin_buffer[3] = d2 & 0x0FF;
        break;
    default:
        break;
    }
    // B4/D4:
    switch (format){
    case SSe:
        // B4: bits(32-35)
        bin_buffer[4] = b4 << 4;
        // D4: bits(36-47)
        bin_buffer[4] = bin_buffer[4] | (d4 >> 8);
        bin_buffer[5] = d4 & 0x0FF;
        break;
    default:
        break;
    }
    return OK;
}

ErrorCode display_SS(Context* c, Instruction* instr){
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
    case SSa:
        printf("+--------+--------+----+------------+----+------------+\n");
        printf("| OPCODE |   L1   | B1 |     D1     | B2 |     D2     |\n");
        printf("+--------+--------+----+------------+----+------------+\n");
        printf("0        8        10   14           20   24          2F\n");
        break;
    case SSb:
        printf("+--------+----+----+----+------------+----+------------+\n");
        printf("| OPCODE | L1 | L2 | B1 |     D1     | B2 |     D2     |\n");
        printf("+--------+----+----+----+------------+----+------------+\n");
        printf("0        8    C    10   14           20   24          2F\n");
        break;
    case SSc:
        printf("+--------+----+----+----+------------+----+------------+\n");
        printf("| OPCODE | L1 | I3 | B1 |     D1     | B2 |     D2     |\n");
        printf("+--------+----+----+----+------------+----+------------+\n");
        printf("0        8    C    10   14           20   24          2F\n");
        break;
    case SSd:
        printf("+--------+----+----+----+------------+----+------------+\n");
        printf("| OPCODE | R1 | R3 | B1 |     D1     | B2 |     D2     |\n");
        printf("+--------+----+----+----+------------+----+------------+\n");
        printf("0        8    C    10   14           20   24          2F\n");
        break;
    case SSe:
        printf("+--------+----+----+----+------------+----+------------+\n");
        printf("| OPCODE | R1 | R3 | B2 |     D2     | B4 |     D4     |\n");
        printf("+--------+----+----+----+------------+----+------------+\n");
        printf("0        8    C    10   14           20   24          2F\n");
        break;
    case SSf:
        printf("+--------+--------+----+------------+----+------------+\n");
        printf("| OPCODE |   L2   | B1 |     D1     | B2 |     D2     |\n");
        printf("+--------+--------+----+------------+----+------------+\n");
        printf("0        8        10   14           20   24          2F\n");
        break;
    default:
        break;
    }
    // Print general information
    printf("MNEMONIC: %s\n", INSTRUCTION_TABLE[instr->it_index].mnemonic);
    printf("OPERANDS: %s\n", instr->operands_txt);
    hex_str_2_char_str((void*)&opcode, sizeof(opcode), 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_2CHR_LEN, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_12CHR_LEN, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", length);
    switch (format){
    case SSa:
        printf("FORMAT:   SSa\n");
        break;
    case SSb:
        printf("FORMAT:   SSb\n");
        break;
    case SSc:
        printf("FORMAT:   SSc\n");
        break;
    case SSd:
        printf("FORMAT:   SSd\n");
        break;
    case SSe:
        printf("FORMAT:   SSe\n");
        break;
    case SSf:
        printf("FORMAT:   SSf\n");
        break;
    default:
        break;
    }
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    switch (format){
    case SSa:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        printf("L1:       %s\n", conv_buffer);
        break;
    case SSb:
    case SSc:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("L1:       %s\n", conv_buffer);
        break;
    case SSd:
    case SSe:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("R1:       %s\n", conv_buffer);
        break;
    default:
        break;
    }
    switch (format){
    case SSb:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
        printf("L2:       %s\n", conv_buffer);
        break;
    case SSc:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
        printf("I3:       %s\n", conv_buffer);
        break;
    case SSd:
    case SSe:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
        printf("R3:       %s\n", conv_buffer);
        break;
    case SSf:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        printf("L2:       %s\n", conv_buffer);
        break;
    default:
        break;
    }
    switch (format){
    case SSa:
    case SSb:
    case SSc:
    case SSd:
    case SSf:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("B1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_3CHR_LEN, SKIP, false);
        printf("D1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("B2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_3CHR_LEN, SKIP, false);
        printf("D2:       %s\n", conv_buffer);
        break;
    case SSe:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("B2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_3CHR_LEN, SKIP, false);
        printf("D2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("B4:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_3CHR_LEN, SKIP, false);
        printf("D4:       %s\n", conv_buffer);
        break;
    default:
        break;
    }
    return OK;
}

ErrorCode disassemble_SS(Context* c, size_t table_index, const uint8_t* bin_buffer, char* operands_token){
    char buffer[MAX_OPERANDS_LEN];
    InstructionFormat format = INSTRUCTION_TABLE[table_index].format;
    size_t i = 0;
    switch (format){
    case SSa:
        // D1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        // L1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        operands_token[i++] = '(';
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = ',';
        // B1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ')';
        // D2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
        operands_token[i++] = ',';
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        // B2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = '(';
        operands_token[i++] = buffer[0];
        operands_token[i++] = ')';
        break;
    case SSb:
        // D1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        // L1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        operands_token[i++] = '(';
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // B1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ')';
        // D2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
        operands_token[i++] = ',';
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        // L2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, SKIP, false);
        operands_token[i++] = '(';
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // B2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ')';
        break;
    case SSc:
        // D1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        // L1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        operands_token[i++] = '(';
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // B1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ')';
        // D2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
        operands_token[i++] = ',';
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        // B2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = '(';
        operands_token[i++] = buffer[0];
        operands_token[i++] = ')';
        // I3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, SKIP, false);
        operands_token[i++] = ',';
        operands_token[i++] = buffer[0];
        break;
    case SSd:
        // D1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        // R1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        operands_token[i++] = '(';
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // B1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ')';
        // D2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
        operands_token[i++] = ',';
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        // B2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = '(';
        operands_token[i++] = buffer[0];
        operands_token[i++] = ')';
        // R3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, SKIP, false);
        operands_token[i++] = ',';
        operands_token[i++] = buffer[0];
        break;
    case SSe:
        // R1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // R3:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ',';
        // D1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        // B1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = '(';
        operands_token[i++] = buffer[0];
        operands_token[i++] = ')';
        // D2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
        operands_token[i++] = ',';
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        // B2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = '(';
        operands_token[i++] = buffer[0];
        operands_token[i++] = ')';
        break;
    case SSf:
        // D1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        // B1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = '(';
        operands_token[i++] = buffer[0];
        operands_token[i++] = ')';
        // D2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
        operands_token[i++] = ',';
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        // L1:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        operands_token[i++] = '(';
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = ',';
        // B2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = ')';
        break;
    default:
        break;
    }
    return OK;
}
