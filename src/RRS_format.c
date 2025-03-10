#include "InstructionTable.h"
#include "HLASMDisassembler.h"

ErrorCode assemble_RRS(size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    uint8_t r1 = 0;
    uint8_t r2 = 0;
    uint8_t m3 = 0;
    uint8_t b4 = 0;
    uint16_t d4 = 0;
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
                    state = R2;
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r2, sizeof(r2), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    state = M3;
                    i++;
                }
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&m3, sizeof(m3), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    state = D4;
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
        case D4:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&d4, sizeof(d4), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    state = B4;
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_3CHR_LEN){
                    Context.error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                    strcpy((char*)&Context.msg_extras[1], "D4");
                    sprintf((char*)&Context.msg_extras[2], "%d", MAX_3CHR_LEN);
                    return Context.error_code;
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
                    Context.error_code = OPERAND_NON_HEX_FOUND;
                    sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                    strcpy((char*)&Context.msg_extras[1], operands_token);
                    return Context.error_code;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&b4, sizeof(b4), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = OPS_DONE;
                i++;
            }
            else{
                if(b_idx >= MAX_1CHR_LEN){
                    Context.error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                    strcpy((char*)&Context.msg_extras[1], "B4");
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
    // R1: bits(8-11)
    bin_buffer[1] = r1 << 4;
    // R2: bits(12-15)
    bin_buffer[1] = bin_buffer[1] | r2;
    // B4: bits(8-11)
    bin_buffer[2] = b4 << 4;
    // D4: bits(20-31)
    bin_buffer[2] = bin_buffer[2] | (d4 >> 8);
    bin_buffer[3] = d4 & 0x0FF;
    // M3: bits(32-35)
    bin_buffer[4] = m3 << 4;
    // Opcode (part 2): bits(40-47)
    bin_buffer[5] = opcode & 0x00FF;
    return OK;
}

ErrorCode display_RRS(Instruction* instr){
    if(instr == NULL){
        Context.error_code = NULL_POINTER_TO_OBJECT;
        return Context.error_code;
    }
    uint16_t opcode = INSTRUCTION_TABLE[instr->it_index].opcode;
    uint8_t length = INSTRUCTION_TABLE[instr->it_index].length;
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    printf("+--------+----+----+----+------------+----+----+--------+\n");
    printf("| OPCODE | R1 | R2 | B4 |     D4     | M3 | // | OPCODE |\n");
    printf("+--------+----+----+----+------------+----+----+--------+\n");
    printf("0        8    C    10   14           20   24   28      2F\n");
    // Print general information
    printf("MNEMONIC: %s\n", INSTRUCTION_TABLE[instr->it_index].mnemonic);
    printf("OPERANDS: %s\n", instr->operands_txt);
    hex_str_2_char_str((void*)&opcode, sizeof(opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_4CHR_LEN, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_12CHR_LEN, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", length);
    printf("FORMAT:   RRS\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
    printf("R1:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
    printf("R2:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
    printf("B4:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_3CHR_LEN, SKIP, false);
    printf("D4:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
    printf("M3:       %s\n", conv_buffer);
    return OK;
}

ErrorCode disassemble_RRS(size_t table_index, const uint8_t* bin_buffer, char* operands_token){
    char buffer[MAX_OPERANDS_LEN];
    size_t i = 0;
    // R1:
    memset(&buffer, 0, sizeof(buffer));
    hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
    operands_token[i++] = buffer[0];
    operands_token[i++] = ',';
    // R2:
    memset(&buffer, 0, sizeof(buffer));
    hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
    operands_token[i++] = buffer[0];
    operands_token[i++] = ',';
    // M3:
    memset(&buffer, 0, sizeof(buffer));
    hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
    operands_token[i++] = buffer[0];
    operands_token[i++] = ',';
    // D4:
    memset(&buffer, 0, sizeof(buffer));
    hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
    operands_token[i++] = buffer[0];
    operands_token[i++] = buffer[1];
    operands_token[i++] = buffer[2];
    // B4:
    memset(&buffer, 0, sizeof(buffer));
    hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
    operands_token[i++] = '(';
    operands_token[i++] = buffer[0];
    operands_token[i++] = ')';
    return OK;
}