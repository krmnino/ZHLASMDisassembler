#include "InstructionTable.h"
#include "HLASMDisassembler.h"

ErrorCode assemble_RRE(size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    bool r1_unused = INSTRUCTION_TABLE[table_index].unused_operands & R1_UNUSED;
    bool r2_unused = INSTRUCTION_TABLE[table_index].unused_operands & R2_UNUSED;
    uint8_t r1 = 0;
    uint8_t r2 = 0;
    char buffer[MAX_OPERANDS_LEN];
    size_t i = 0;
    size_t operands_token_len = strlen(operands_token) + 1;
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state;
    if(r1_unused && r2_unused){
        state = OPS_DONE;
    }
    else{
        state = R1;
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
                        Context.error_code = OPERAND_NON_HEX_FOUND;
                        sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                        strcpy((char*)&Context.msg_extras[1], operands_token);
                        return Context.error_code;
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
                        char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r2, sizeof(r2), b_idx, NO_SKIP, true);
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
                        strcpy((char*)&Context.msg_extras[1], "R2");
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
        if(i != operands_token_len){
            Context.error_code = TOO_MANY_OPERANDS;
            sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
            strcpy((char*)&Context.msg_extras[1], INSTRUCTION_TABLE[table_index].mnemonic);
            return Context.error_code;
        }
    }
    if(state != OPS_DONE){
        Context.error_code = MISSING_OPERANDS;
        sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
        strcpy((char*)&Context.msg_extras[1], INSTRUCTION_TABLE[table_index].mnemonic);
        return Context.error_code;
    }
    // Opcode: bits(0-15)
    bin_buffer[0] = opcode >> 8;
    bin_buffer[1] = opcode & 0x00FF;
    // R1: bits(24-27)
    if(!r1_unused){
        bin_buffer[3] = r1 << 4;
    }
    // R2: bits(28-31)
    if(!r2_unused){
        bin_buffer[3] = bin_buffer[3] | r2;
    }
    return OK;
}

ErrorCode display_RRE(Instruction* instr){
    if(instr == NULL){
        Context.error_code = NULL_POINTER_TO_OBJECT;
        return Context.error_code;
    }
    uint16_t opcode = INSTRUCTION_TABLE[instr->it_index].opcode;
    uint8_t length = INSTRUCTION_TABLE[instr->it_index].length;
    bool r1_unused = INSTRUCTION_TABLE[instr->it_index].unused_operands & R1_UNUSED;
    bool r2_unused = INSTRUCTION_TABLE[instr->it_index].unused_operands & R2_UNUSED;
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    if(r1_unused && r2_unused){
        printf("+----------------+---------+----+----+\n");
        printf("|     OPCODE     | /////// | // | // |\n");
        printf("+----------------+---------+----+----+\n");
        printf("0                10        18   1C  1F\n");
    }
    else if(!r1_unused && r2_unused){
        printf("+----------------+---------+----+----+\n");
        printf("|     OPCODE     | /////// | R1 | // |\n");
        printf("+----------------+---------+----+----+\n");
        printf("0                10        18   1C  1F\n");
    }
    else{
        printf("+----------------+---------+----+----+\n");
        printf("|     OPCODE     | /////// | R1 | R2 |\n");
        printf("+----------------+---------+----+----+\n");
        printf("0                10        18   1C  1F\n");
    }
    // Print general information
    printf("MNEMONIC: %s\n", INSTRUCTION_TABLE[instr->it_index].mnemonic);
    printf("OPERANDS: %s\n", instr->operands_txt);
    hex_str_2_char_str((void*)&opcode, sizeof(opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_4CHR_LEN, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_8CHR_LEN, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", length);
    printf("FORMAT:   RRE\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    if(!r1_unused){
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("R1:       %s\n", conv_buffer);
    }
    if(!r2_unused){
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, SKIP, false);
        printf("R2:       %s\n", conv_buffer);
    }
    return OK;
}

ErrorCode disassemble_RRE(size_t table_index, const uint8_t* bin_buffer, char* operands_token){
    char buffer[MAX_OPERANDS_LEN];
    bool r1_unused = INSTRUCTION_TABLE[table_index].unused_operands & R1_UNUSED;
    bool r2_unused = INSTRUCTION_TABLE[table_index].unused_operands & R2_UNUSED;
    size_t i = 0;
    // Check if need to use alternative instruction
    if(strlen(INSTRUCTION_TABLE[table_index].alternative_instr) != 0 && bin_buffer[2] != 0x00){
        return USE_ALTERNATIVE;
    }
    // R1:
    if(!r1_unused){
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = buffer[0];
    }
    // R2:
    if(!r2_unused){
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 3, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, SKIP, false);
        operands_token[i++] = ',';
        operands_token[i++] = buffer[0];
    }
    return OK;
}
