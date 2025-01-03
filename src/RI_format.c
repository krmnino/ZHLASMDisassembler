#include "InstructionTable.h"
#include "HLASMCompiler.h"

ErrorCode build_RI(Context* c, size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    InstructionFormat format = INSTRUCTION_TABLE[table_index].format;
    bool m1_unused = INSTRUCTION_TABLE[table_index].unused_operands & M1_UNUSED;
    uint8_t r1_m1 = 0; 
    uint16_t i2_ri2 = 0; 
    char buffer[MAX_OPERANDS_LEN];
    size_t i;
    size_t operands_token_len = strlen(operands_token) + 1;
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state;
    switch (format){
    case RIa:
    case RIb:
        state = R1;
        break;
    case RIc:
        if(m1_unused){
            state = RI2;
        }
        else{
            state = M1;
        }
        break;
    default:
        break;
    }
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    for(i = 0; i < operands_token_len && run;){
        switch (state){
        case R1:
        case M1:
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r1_m1, sizeof(r1_m1), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    switch (format){
                    case RIa:
                        state = I2;
                        break;
                    case RIb:
                    case RIc:
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
                    if(state == R1){
                        strcpy((char*)&c->msg_extras[1], "R1");
                    }
                    else{
                        strcpy((char*)&c->msg_extras[1], "M1");
                    }
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
            if(operands_token[i] == 0){
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
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i2_ri2, sizeof(i2_ri2), b_idx, NO_SKIP, true);
                    memset(&buffer, 0, sizeof(buffer));
                    b_idx = 0;
                    state = OPS_DONE;
                    i++;
                }
            }
            else{
                if(b_idx >= MAX_4CHR_LEN){
                    c->error_code = INVALID_OPERAND_LENGTH;
                    sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
                    if(state == I2){
                        strcpy((char*)&c->msg_extras[1], "I2");
                    }
                    else{
                        strcpy((char*)&c->msg_extras[1], "RI2");
                    }
                    sprintf((char*)&c->msg_extras[2], "%d", MAX_4CHR_LEN);
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
    bin_buffer[0] = opcode >> 4;
    // R1/M1: bits(8-11)
    bin_buffer[1] = r1_m1 << 4;
    // Opcode (part 2): bits(12-15)
    bin_buffer[1] = bin_buffer[1] | (opcode & 0x000F);
    // I2/RI2: bits(16-31)
    bin_buffer[2] = i2_ri2 >> 8;
    bin_buffer[3] = i2_ri2 & 0x00FF;
    return OK;
}

ErrorCode display_RI(Context* c, Instruction* instr){
    if(instr == NULL){
        c->error_code = NULL_POINTER_TO_OBJECT;
        return c->error_code;
    }
    uint16_t opcode = INSTRUCTION_TABLE[instr->it_index].opcode;
    uint8_t length = INSTRUCTION_TABLE[instr->it_index].length;
    InstructionFormat format = INSTRUCTION_TABLE[instr->it_index].format;
    bool m1_unused = INSTRUCTION_TABLE[instr->it_index].unused_operands & M1_UNUSED;
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    switch (format){
    case RIa:
        printf("+--------+----+----+----------------+\n");
        printf("| OPCODE | R1 | OP |       I2       |\n");
        printf("+--------+----+----+----------------+\n");
        printf("0        8    C    10              1F\n");
        break;
    case RIb:
        printf("+--------+----+----+----------------+\n");
        printf("| OPCODE | R1 | OP |      RI2       |\n");
        printf("+--------+----+----+----------------+\n");
        printf("0        8    C    10              1F\n");
        break;
    case RIc:
        if(m1_unused){
            printf("+--------+----+----+----------------+\n");
            printf("| OPCODE | // | OP |      RI2       |\n");
            printf("+--------+----+----+----------------+\n");
            printf("0        8    C    10              1F\n");
        }
        else{
            printf("+--------+----+----+----------------+\n");
            printf("| OPCODE | M1 | OP |      RI2       |\n");
            printf("+--------+----+----+----------------+\n");
            printf("0        8    C    10              1F\n");
        }
        break;
    default:
        break;
    }
    // Print general information
    printf("MNEMONIC: %s\n", INSTRUCTION_TABLE[instr->it_index].mnemonic);
    hex_str_2_char_str((void*)&opcode, sizeof(opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 3, SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 8, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", length);
    switch (format){
    case RIa:
        printf("FORMAT:   RIa\n");
        break;
    case RIb:
        printf("FORMAT:   RIb\n");
        break;
    case RIc:
        printf("FORMAT:   RIc\n");
        break;
    default:
        break;
    }
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    switch (format){
    case RIa:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("R1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
        printf("I2:       %s\n", conv_buffer);
        break;
    case RIb:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("R1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
        printf("RI2:      %s\n", conv_buffer);
        break;
    case RIc:
        if(!m1_unused){
            hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
            printf("M1:       %s\n", conv_buffer);
        }
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
        printf("RI2:      %s\n", conv_buffer);
        break;
    default:
        break;
    }
    return OK;
}

ErrorCode decode_RI(){
    return OK;
}