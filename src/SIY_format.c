#include "InstructionTable.h"
#include "HLASMCompiler.h"

ErrorCode assemble_SIY(Context* c, size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    bool i2_unused = INSTRUCTION_TABLE[table_index].unused_operands & I2_UNUSED;
    uint8_t b1 = 0;
    uint32_t d1 = 0;
    uint8_t i2 = 0;
    char buffer[MAX_OPERANDS_LEN];
    size_t i;
    size_t operands_token_len = strlen(operands_token) + 1;
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state = D1;
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
                    state = B1;
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_5CHR_LEN){
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], "D1");
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_5CHR_LEN);
                    return c->error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case B1:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&b1, sizeof(b1), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    if(i2_unused){
                        state = OPS_DONE;
                    }
                    else{
                        state = I2;
                    }
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
        case I2:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i2, sizeof(i2), b_idx, NO_SKIP, true);
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
                    strcpy((char*)&c->msg_extras[1], "I2");
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
    // I2: bits(8-15)
    if(!i2_unused){
        bin_buffer[1] = i2;
    }
    // B1: bits(16-19)
    bin_buffer[2] = b1 << 4;
    // DL2: bits(20-31)
    bin_buffer[2] = bin_buffer[2] | ((d1 >> 8) & 0x0F);
    bin_buffer[3] = d1 & 0x00FF;
    // DH2: bits(32-35)
    bin_buffer[4] = d1 >> 12;
    // Opcode (part 2): bits(40-47)
    bin_buffer[5] = opcode & 0x00FF;
    return OK;
}

ErrorCode display_SIY(Context* c, Instruction* instr){
    if(instr == NULL){
        c->error_code = NULL_POINTER_TO_OBJECT;
        return c->error_code;
    }
    uint16_t opcode = INSTRUCTION_TABLE[instr->it_index].opcode;
    uint8_t length = INSTRUCTION_TABLE[instr->it_index].length;
    bool i2_unused = INSTRUCTION_TABLE[instr->it_index].unused_operands & I2_UNUSED;
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    printf("+--------+--------+----+------------+--------+--------+\n");
    printf("| OPCODE |   I2   | B1 |     D1     |  DH1   | OPCODE |\n");
    printf("+--------+--------+----+------------+--------+--------+\n");
    printf("0        8        10   14           20       28      2F\n");
    // Print general information
    printf("MNEMONIC: %s\n", INSTRUCTION_TABLE[instr->it_index].mnemonic);
    printf("OPERANDS: %s\n", instr->operands_txt);
    hex_str_2_char_str((void*)&opcode, sizeof(opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_4CHR_LEN, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_12CHR_LEN, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", length);
    printf("FORMAT:   SIY\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    if(!i2_unused){
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        printf("I2:       %s\n", conv_buffer);
    }
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
    printf("B1:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_3CHR_LEN, SKIP, false);
    printf("DL1:      %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_2CHR_LEN, NO_SKIP, false);
    printf("DH1:      %s\n", conv_buffer);
    return OK;
}

ErrorCode disassemble_SIY(Context* c, size_t table_index, const uint8_t* bin_buffer, char* operands_token){
    char buffer[MAX_OPERANDS_LEN];
    bool i2_unused = INSTRUCTION_TABLE[table_index].unused_operands & I2_UNUSED;
    size_t i = 0;
    // D1:
    memset(&buffer, 0, sizeof(buffer));
    hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, NO_SKIP, false);
    operands_token[i++] = buffer[0];
    operands_token[i++] = buffer[1];
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
    // I2:
    if(!i2_unused){
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_2CHR_LEN, NO_SKIP, false);
        operands_token[i++] = ',';
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
    }
    return OK;
}
