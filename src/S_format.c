#include "InstructionTable.h"
#include "HLASMDisassembler.h"

ErrorCode assemble_S(size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    bool b2_unused = INSTRUCTION_TABLE[table_index].unused_operands & B2_UNUSED;
    bool d2_unused = INSTRUCTION_TABLE[table_index].unused_operands & D2_UNUSED;
    uint8_t b2 = 0;
    uint16_t d2 = 0;
    char buffer[MAX_OPERANDS_LEN];
    size_t i;
    size_t operands_token_len = strlen(operands_token) + 1;
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state;
    if(b2_unused && d2_unused){
        state = OPS_DONE;
    }
    else{
        state = D2;
        // Clear buffer
        memset(&buffer, 0, sizeof(buffer));
        for(i = 0; i < operands_token_len && run;){
            switch (state){
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
                        char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&d2, sizeof(d2), b_idx, NO_SKIP, true);
                        memset(&buffer, 0, sizeof(buffer));
                        b_idx = 0;
                        state = B2;
                        i++;
                    }
                }
                else{
                    if(b_idx >= MAX_3CHR_LEN){
                        Context.error_code = INVALID_OPERAND_LENGTH;
                        sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                        strcpy((char*)&Context.msg_extras[1], "D2");
                        sprintf((char*)&Context.msg_extras[2], "%d", MAX_3CHR_LEN);
                        return Context.error_code;
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
                        Context.error_code = OPERAND_NON_HEX_FOUND;
                        sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
                        strcpy((char*)&Context.msg_extras[1], operands_token);
                        return Context.error_code;
                    }
                    else{
                        char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&b2, sizeof(b2), b_idx, NO_SKIP, true);
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
                        strcpy((char*)&Context.msg_extras[1], "B2");
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
    // B2: bits(16-19)
    bin_buffer[2] = b2 << 4;
    // D2: bits(20-31)
    bin_buffer[2] = bin_buffer[2] | (d2 >> 8);
    bin_buffer[3] = d2 & 0x00FF;
    return OK;
}

ErrorCode display_S(Instruction* instr){
    if(instr == NULL){
        Context.error_code = NULL_POINTER_TO_OBJECT;
        return Context.error_code;
    }
    uint16_t opcode = INSTRUCTION_TABLE[instr->it_index].opcode;
    uint8_t length = INSTRUCTION_TABLE[instr->it_index].length;
    bool b2_unused = INSTRUCTION_TABLE[instr->it_index].unused_operands & B2_UNUSED;
    bool d2_unused = INSTRUCTION_TABLE[instr->it_index].unused_operands & D2_UNUSED;
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    if(b2_unused && d2_unused){
        printf("+----------------+----+------------+\n");
        printf("|     OPCODE     | // | /////////// |\n");
        printf("+----------------+----+------------+\n");
        printf("0                10   14          1F\n");
    }
    else{
        printf("+----------------+----+------------+\n");
        printf("|     OPCODE     | B2 |     D2     |\n");
        printf("+----------------+----+------------+\n");
        printf("0                10   14          1F\n");
    }
    // Print general information
    printf("MNEMONIC: %s\n", INSTRUCTION_TABLE[instr->it_index].mnemonic);
    printf("OPERANDS: %s\n", instr->operands_txt);
    hex_str_2_char_str((void*)&opcode, sizeof(opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_4CHR_LEN, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_8CHR_LEN, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", length);
    printf("FORMAT:   S\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    if(!b2_unused && !d2_unused){
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        printf("B2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, MAX_3CHR_LEN, SKIP, false);
        printf("D2:       %s\n", conv_buffer);
    }
    return OK;
}

ErrorCode disassemble_S(size_t table_index, const uint8_t* bin_buffer, char* operands_token){
    char buffer[MAX_OPERANDS_LEN];
    size_t i = 0;
    bool b2_unused = INSTRUCTION_TABLE[table_index].unused_operands & B2_UNUSED;
    bool d2_unused = INSTRUCTION_TABLE[table_index].unused_operands & D2_UNUSED;
    if(!b2_unused && !d2_unused){
        // D2:
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_3CHR_LEN, SKIP, false);
        operands_token[i++] = buffer[0];
        operands_token[i++] = buffer[1];
        operands_token[i++] = buffer[2];
        // B2:
        memset(&buffer, 0, sizeof(buffer));
        hex_str_2_char_str(bin_buffer, MAX_INSTRUCTION_LEN, 2, buffer, MAX_OPERANDS_LEN, MAX_1CHR_LEN, NO_SKIP, false);
        operands_token[i++] = '(';
        operands_token[i++] = buffer[0];
        operands_token[i++] = ')';
    }
    return OK;
}