#include "InstructionTable.h"
#include "HLASMDisassembler.h"

ErrorCode assemble_SMI(size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    uint8_t m1 = 0;
    uint16_t ri2 = 0;
    uint8_t b3 = 0;
    uint16_t d3 = 0;
    char buffer[MAX_OPERANDS_LEN];
    size_t i;
    size_t operands_token_len = strlen(operands_token) + 1;
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state = M1;
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    for(i = 0; i < operands_token_len && run;){
        switch (state){
        case M1:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&m1, sizeof(m1), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    state = RI2;
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_1CHR_LEN){
                    Context.error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                    strcpy((char*)&Context.msg_extras[1], "M1");
                    sprintf((char*)&Context.msg_extras[2], "%d", MAX_1CHR_LEN);
                    return Context.error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case RI2:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&ri2, sizeof(ri2), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    state = D3;
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_4CHR_LEN){
                    Context.error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                    strcpy((char*)&Context.msg_extras[1], "RI2");
                    sprintf((char*)&Context.msg_extras[2], "%d", MAX_4CHR_LEN);
                    return Context.error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case D3:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&d3, sizeof(d3), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    state = B3;
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_3CHR_LEN){
                    Context.error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                    strcpy((char*)&Context.msg_extras[1], "D3");
                    sprintf((char*)&Context.msg_extras[2], "%d", MAX_3CHR_LEN);
                    return Context.error_code;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case B3:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&b3, sizeof(b3), b_idx, NO_SKIP, true);
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
                    strcpy((char*)&Context.msg_extras[1], "B3");
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
    // Opcode: bits(0-7)
    bin_buffer[0] = opcode;
    // M1: bits(8-11)
    bin_buffer[1] = m1 << 4;
    // B3: bits(16-19)
    bin_buffer[2] = b3 << 4;
    // D3: bits(20-31)
    bin_buffer[2] = bin_buffer[2] | (d3 >> 8);
    bin_buffer[3] = d3 & 0x0FF;
    // RI2: bits(32-47)
    bin_buffer[4] = bin_buffer[4] | (ri2 >> 8);
    bin_buffer[5] = ri2 & 0x00FF;
    return OK;
}

ErrorCode display_SMI(Instruction* instr){
    if(instr == NULL){
        Context.error_code = NULL_POINTER_TO_OBJECT;
        return Context.error_code;
    }
    uint16_t opcode = INSTRUCTION_TABLE[instr->it_index].opcode;
    uint8_t length = INSTRUCTION_TABLE[instr->it_index].length;
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    printf("+--------+----+----+----+------------+----------------+\n");
    printf("| OPCODE | M1 | // | B3 |     D3     |       RI2      |\n");
    printf("+--------+----+----+----+------------+----------------+\n");
    printf("0        8    C    10   14           20              2F\n");
    // Print general information
    printf("MNEMONIC: %s\n", INSTRUCTION_TABLE[instr->it_index].mnemonic);
    printf("OPERANDS: %s\n", instr->operands_txt);    
    hex_str_2_char_str((void*)&opcode, sizeof(opcode), 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_2CHR_LEN, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_12CHR_LEN, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", length);
    printf("FORMAT:   SMI\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
    printf("M1:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
    printf("B3:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_3CHR_LEN, SKIP, false);
    printf("D3:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_4CHR_LEN, NO_SKIP, false);
    printf("RI2:      %s\n", conv_buffer);
    return OK;
}

ErrorCode disassemble_SMI(size_t table_index, const uint8_t* bin_buffer, char* operands_token){
    char buffer[MAX_OPERANDS_LEN];
    size_t i = 0;
    // B3:
    memset(&buffer, 0, sizeof(buffer));
    hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 1, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
    operands_token[i++] = buffer[0];
    operands_token[i++] = ',';
    // RI2:
    memset(&buffer, 0, sizeof(buffer));
    hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 4, buffer, MAX_OPERANDS_LEN, MAX_4CHR_LEN, NO_SKIP, false);
    operands_token[i++] = buffer[0];
    operands_token[i++] = buffer[1];
    operands_token[i++] = buffer[2];
    operands_token[i++] = buffer[3];
    operands_token[i++] = ',';
    // D3:
    memset(&buffer, 0, sizeof(buffer));
    hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
    operands_token[i++] = buffer[0];
    operands_token[i++] = buffer[1];
    operands_token[i++] = buffer[2];
    // B3:
    memset(&buffer, 0, sizeof(buffer));
    hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
    operands_token[i++] = '(';
    operands_token[i++] = buffer[0];
    operands_token[i++] = ')';
    return OK;
}