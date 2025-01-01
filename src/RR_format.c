#include "InstructionTable.h"
#include "HLASMCompiler.h"

ErrorCode assemble_RR(Context* c, size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    bool r2_unused = INSTRUCTION_TABLE[table_index].unused_operands & R2_UNUSED;
    uint8_t r1 = 0;
    uint8_t r2 = 0;
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r1, sizeof(r1), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    if(r2_unused){
                        state = OPS_DONE;
                    }
                    else{
                        state = R2;
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r2, sizeof(r2), b_idx, NO_SKIP, true);
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
                    strcpy((char*)&c->msg_extras[1], "R2");
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
    // R1: bits(8-11)
    bin_buffer[1] = r1 << 4;
    // R2: bits(12-15)
    bin_buffer[1] = bin_buffer[1] | r2;
    return OK;
}

ErrorCode display_RR(Context* c, Instruction* instr){
    if(instr == NULL){
        c->error_code = NULL_POINTER_TO_OBJECT;
        return c->error_code;
    }
    uint16_t opcode = INSTRUCTION_TABLE[instr->it_index].opcode;
    uint8_t length = INSTRUCTION_TABLE[instr->it_index].length;
    bool r2_unused = INSTRUCTION_TABLE[instr->it_index].unused_operands & R2_UNUSED;
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    if(r2_unused){
        printf("+--------+----+----+\n");
        printf("| OPCODE | R1 | // |\n");
        printf("+--------+----+----+\n");
        printf("0        8    C    F\n");
    }
    else{
        printf("+--------+----+----+\n");
        printf("| OPCODE | R1 | R2 |\n");
        printf("+--------+----+----+\n");
        printf("0        8    C    F\n");
    }
    // Print general information
    printf("MNEMONIC: %s\n", INSTRUCTION_TABLE[instr->it_index].mnemonic);
    hex_str_2_char_str((void*)&opcode, sizeof(opcode), 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", length);
    printf("FORMAT:   RR\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
    printf("R1:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
    if(!r2_unused){
        printf("R2:       %s\n", conv_buffer);
    }
    return OK;
}

ErrorCode decode_RR(){
    return OK;
}