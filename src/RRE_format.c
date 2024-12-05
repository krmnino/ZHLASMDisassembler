#include "InstructionTable.h"
#include "HLASMCompiler.h"

int build_RRE(size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    InstructionFormat format = INSTRUCTION_TABLE[table_index].format;
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
                    if(!is_valid_hex_string(buffer, b_idx)){
                        return -1;
                    }
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
                else{
                    if(b_idx >= MAX_1CHR_LEN){
                        return -1;
                    }
                    buffer[b_idx] = operands_token[i];
                    b_idx++;
                    i++;
                }
                break;
            case R2:
                if(operands_token[i] == 0){
                    if(!is_valid_hex_string(buffer, b_idx)){
                        return -1;
                    }
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r2, sizeof(r2), b_idx, NO_SKIP, true);
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
        if(i != operands_token_len){
            return -1;
        }
    }
    if(state != OPS_DONE){
        return -1;
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
    return 0;
}

int display_RRE(Instruction* instr){
    if(instr == NULL){
        return -1;
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
    hex_str_2_char_str((void*)&opcode, sizeof(opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 8, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", length);
    printf("FORMAT:   RRE\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    if(!r1_unused){
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("R1:       %s\n", conv_buffer);
    }
    if(!r2_unused){
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("R2:       %s\n", conv_buffer);
    }
    return 0;
}