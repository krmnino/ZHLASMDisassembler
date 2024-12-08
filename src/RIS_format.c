#include "InstructionTable.h"
#include "HLASMCompiler.h"

int build_RIS(size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    uint8_t r1 = 0;
    uint8_t i2 = 0;
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
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r1, sizeof(r1), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = I2;
                i++;
            }
            else{
                if(b_idx >= MAX_1CHR_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case I2:
            if(operands_token[i] == ','){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i2, sizeof(i2), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = M3;
                i++;
            }
            else{
                if(b_idx >= MAX_2CHR_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case M3:
            if(operands_token[i] == ','){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&m3, sizeof(m3), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = D4;
                i++;
            }
            else{
                if(b_idx >= MAX_1CHR_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case D4:
            if(operands_token[i] == ','){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&d4, sizeof(d4), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = B4;
                i++;
            }
            else{
                if(b_idx >= MAX_3CHR_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case B4:
            if(operands_token[i] == 0){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&b4, sizeof(b4), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = OPS_DONE;
                i++;
            }
            else{
                if(b_idx >= MAX_1CHR_LEN){
                    return -1;
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
        return -1;
    }
    if(i != operands_token_len){
        return -1;
    }
    // Opcode (part 1): bits(0-7)
    bin_buffer[0] = opcode >> 8;
    // R1: bits(8-11)
    bin_buffer[1] = r1 << 4;
    // M3: bits(12-15)
    bin_buffer[1] = bin_buffer[1] | m3;
    // B4: bits(8-11)
    bin_buffer[2] = b4 << 4;
    // D4: bits(20-31)
    bin_buffer[2] = bin_buffer[2] | (d4 >> 8);
    bin_buffer[3] = d4 & 0x0FF;
    // I2: bits(20-31)
    bin_buffer[4] = i2;
     // Opcode (part 2): bits(40-47)
    bin_buffer[5] = opcode & 0x00FF;
    return 0;
}

int display_RIS(Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    uint16_t opcode = INSTRUCTION_TABLE[instr->it_index].opcode;
    uint8_t length = INSTRUCTION_TABLE[instr->it_index].length;
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    printf("+--------+----+----+----+------------+--------+--------+\n");
    printf("| OPCODE | R1 | M3 | B4 |     D4     |   I2   | OPCODE |\n");
    printf("+--------+----+----+----+------------+--------+--------+\n");
    printf("0        8    C    10   14           20       28      2F\n");
    // Print general information
    printf("MNEMONIC: %s\n", INSTRUCTION_TABLE[instr->it_index].mnemonic);
    hex_str_2_char_str((void*)&opcode, sizeof(opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 12, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", length);
    printf("FORMAT:   RIS\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
    printf("R1:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
    printf("M3:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
    printf("B4:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 3, SKIP, false);
    printf("D4:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, false);
    printf("I2:       %s\n", conv_buffer);
    return 0;
}

int decode_RIS(){
    return 0;
}