#include "InstructionTable.h"
#include "HLASMCompiler.h"

int build_RI(size_t table_index, const char* operands_token, uint8_t* bin_buffer){
    uint16_t opcode = INSTRUCTION_TABLE[table_index].opcode;
    InstructionFormat format = INSTRUCTION_TABLE[table_index].format;
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
        state = M1;
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
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
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
        case RI2:
            if(operands_token[i] == 0){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i2_ri2, sizeof(i2_ri2), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = OPS_DONE;
                i++;
            }
            else{
                if(b_idx >= MAX_4CHR_LEN){
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
    if(state != OPS_DONE){
        return -1;
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
    return 0;
}

int display_RI(Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    uint16_t opcode = INSTRUCTION_TABLE[instr->it_index].opcode;
    uint8_t length = INSTRUCTION_TABLE[instr->it_index].length;
    InstructionFormat format = INSTRUCTION_TABLE[instr->it_index].format;
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
        printf("+--------+----+----+----------------+\n");
        printf("| OPCODE | M1 | OP |      RI2       |\n");
        printf("+--------+----+----+----------------+\n");
        printf("0        8    C    10              1F\n");
        break;
    default:
        return -1;
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
        return -1;
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
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("M1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
        printf("RI2:      %s\n", conv_buffer);
        break;
    default:
        return -1;
    }
    return 0;
}