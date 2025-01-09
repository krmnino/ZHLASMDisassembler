#include "InstructionTable.h"
#include "HLASMCompiler.h"

ErrorCode assemble_IE(Context* c, size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    uint8_t i1 = 0; 
    uint8_t i2 = 0; 
    char buffer[MAX_OPERANDS_LEN];
    size_t i;
    size_t operands_token_len = strlen(operands_token) + 1;
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state = I1;
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    for(i = 0; i < operands_token_len && run;){
        switch (state){
        case I1:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i1, sizeof(i1), 1, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    state = I2;
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_1CHR_LEN){
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    strcpy((char*)&c->msg_extras[1], "I1");
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i2, sizeof(i2), 1, NO_SKIP, true);
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
                    strcpy((char*)&c->msg_extras[1], "I2");
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
    bin_buffer[0] = opcode >> 8;
    // Opcode: bits(8-16)
    bin_buffer[1] = opcode & 0x00FF;
    // I1: bits(24-27)
    bin_buffer[3] = i1;
    bin_buffer[3] = bin_buffer[3] << 4;
    // I2: bits(28-31)
    bin_buffer[3] = bin_buffer[3] | i2;
    return OK;
}

ErrorCode display_IE(Context* c, Instruction* instr){
    if(instr == NULL){
        c->error_code = NULL_POINTER_TO_OBJECT;
        return c->error_code;
    }
    uint16_t opcode = INSTRUCTION_TABLE[instr->it_index].opcode;
    uint8_t length = INSTRUCTION_TABLE[instr->it_index].length;
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    printf("+----------------+--------+----+----+\n");
    printf("|     OPCODE     | ////// | I1 | I2 |\n");
    printf("+----------------+--------+----+----+\n");
    printf("0                1A       18   1C  1F\n");
    // Print general information
    printf("MNEMONIC: %s\n", INSTRUCTION_TABLE[instr->it_index].mnemonic);
    printf("OPERANDS: %s\n", instr->operands_txt);
    hex_str_2_char_str((void*)&opcode, sizeof(opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_4CHR_LEN, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_8CHR_LEN, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", length);
    printf("FORMAT:   IE\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
    printf("I1:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
    printf("I2:       %s\n", conv_buffer);
    return OK;
}

ErrorCode disassemble_IE(Context* c, size_t table_index, const uint8_t* bin_buffer, char* operands_token){
    char buffer[MAX_OPERANDS_LEN];
    size_t i = 0;
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    // I1:
    hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
    operands_token[i++] = buffer[0];
    operands_token[i++] = ',';
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    // I2:
    hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
    operands_token[i++] = buffer[0];
    return OK;
}