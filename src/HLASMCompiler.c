#include "HLASMCompiler.h"
#include "InstructionTable.h"

int process_source_file(const char* filename){
    FILE* file = file = fopen(filename, "r");
    if(file == NULL){
        return -1;
    }
    char line[MAX_LINE_LEN];
    char mnemonic_token[MAX_MNEMONIC_LEN];
    char operands_token[MAX_OPERANDS_LEN];
    Address instr_offset;
    size_t m_idx;
    size_t o_idx;
    TokensParseState tp_state;
    bool run;
    instr_offset = 0;
    tp_state = SPACES_PM;
    run = true;
    m_idx = 0;
    o_idx = 0;
    while(fgets(line, sizeof(line), file)){
        for(size_t i = 0; i < MAX_LINE_LEN && run;){
            switch (tp_state){
            case SPACES_PM:
                if(line[i] != ' '){
                    tp_state = MNEMONIC;
                }
                else{
                    i++;
                }
                break;
            case MNEMONIC:
                if(line[i] == '\n'){
                    tp_state = TPS_DONE;
                }
                else if(line[i] == ' '){
                    tp_state = SPACES_PO;
                }
                else{
                    mnemonic_token[m_idx] = line[i];
                    m_idx++;
                    i++;
                }
                break;
            case SPACES_PO:
                if(line[i] != ' '){
                    tp_state = OPERANDS;
                }
                else{
                    i++;
                }
                break;
            case OPERANDS:
                if(line[i] == 0 || line[i] == '\n'){
                    tp_state = TPS_DONE;
                }
                else{
                    operands_token[o_idx] = line[i];
                    o_idx++;
                    i++;
                }
                break;
            case TPS_DONE:
                run = false;
                break;
            default:
                i++;
                break;
            }
        }
        Instruction* instr = Instruction_init(mnemonic_token, operands_token, instr_offset);
        if(instr == NULL){
            printf("%s\n", "ERROR!");
            return -1;
        }
        if(InstructionStream_add_instruction(instr) != 0){
            return -1;
        };
        instr_offset += mnemonic_to_length(mnemonic_token);
        tp_state = SPACES_PM;
        run = true;
        m_idx = 0;
        o_idx = 0;
        memset(&line, 0, sizeof(line));
        memset(&mnemonic_token, 0, sizeof(mnemonic_token));
        memset(&operands_token, 0, sizeof(operands_token));
    }
    fclose(file);
    return 0;
}

bool is_valid_mnemonic(const char* mnemonic){
    for(size_t i = 0; i < n_inst; i++){
        if(strcmp(mnemonic, INSTRUCTION_TABLE[i].mnemonic) == 0){
            return true;
        }
    }
    return false;
}

bool is_valid_hex_string(const char* input, size_t length){
    const char* hex_chars = "0123456789ABCDEFabcdef";
    size_t hex_chars_len = strlen(hex_chars);
    bool found;
    for(size_t i = 0; i < length; i++){
        found = false;
        for(size_t j = 0; j < hex_chars_len; j++){
            if(input[i] == hex_chars[j]){
                found = true;
                break;
            }
        }
        if(!found){
            return false;
        }
    }
    return true;
}

InstructionFormat mnemonic_to_format(const char* mnemonic){
    for(size_t i = 0; i < n_inst; i++){
        if(strcmp(mnemonic, INSTRUCTION_TABLE[i].mnemonic) == 0){
            return INSTRUCTION_TABLE[i].format;
        }
    }
    return NF;
}

uint16_t mnemonic_to_opcode(const char* mnemonic){
    for(size_t i = 0; i < n_inst; i++){
        if(strcmp(mnemonic, INSTRUCTION_TABLE[i].mnemonic) == 0){
            return INSTRUCTION_TABLE[i].opcode;
        }
    }
    return 0;
}

uint8_t mnemonic_to_length(const char* mnemonic){
    for(size_t i = 0; i < n_inst; i++){
        if(strcmp(mnemonic, INSTRUCTION_TABLE[i].mnemonic) == 0){
            return INSTRUCTION_TABLE[i].length;
        }
    }
    return 0;
}

uint32_t char_2_hex(const char* input){
    uint8_t b1;
    uint8_t b2;
    size_t length = strlen(input);
    uint32_t ret = 0;
    uint8_t byte;
    uint8_t eval;
    for(size_t i = 0; i < length; i++){
        byte = input[i];
        eval = byte & 0x40;
        if(eval != 0){
            ret = ret << 4;
            ret = ret | ((byte & 0x0F | 0x08) + 1);
        }
        else{
            ret = ret << 4;
            ret = ret | (byte & 0x0F);
        }
    }
    return ret;
}

int hex_2_char(void* input, size_t input_len , size_t input_offset, char* output, size_t output_len, size_t n_chars, size_t skip, bool little_endian){
    if(input == NULL || output == NULL){
        return -1;
    }
    uint8_t* input_bytes  = (uint8_t*)input;
    size_t byte_i = 0;
    size_t output_i = 0;
    uint8_t binary_char = 0;
    // Clear output buffer
    for(size_t i = 0; i < output_len + 1; i++){
        output[i] = 0;
    }
    if(little_endian){
        byte_i = input_len - input_offset - 1;
    }
    else{
        byte_i = input_offset;
    }
    for(size_t i = 0; i < n_chars; i++){
        if((i + skip) % 2 == 0 && i != 0){
            if(little_endian){
                byte_i--;
            }
            else{
                byte_i++;
            }
        }
        if((i + skip) % 2 == 0){
            binary_char = input_bytes[byte_i] >> 4;
        }
        else{
            binary_char = input_bytes[byte_i] & 0x0F;
        }
        switch (binary_char)
        {
        case 0xA:
        case 0xB:
        case 0xC:
        case 0xD:
        case 0xE:
        case 0xF:
            output[output_i] = ((binary_char - 1) & 0x07) | 0x40;
            break;        
        default:
            output[output_i] = binary_char | 0x30;
            break;
        }
        output_i++;
    }
    return 0;
}

Instruction* Instruction_init(const char* mnemonic_token, char* operands_token, Address offset){
    // Check if mnemonic exists in architecture table
    if(!is_valid_mnemonic(mnemonic_token)){
        return NULL;
    }
    InstructionFormat format = mnemonic_to_format(mnemonic_token);
    uint16_t opcode = mnemonic_to_opcode(mnemonic_token);
    size_t o_idx = 0;
    uint8_t bin_buffer[MAX_INSTRUCTION_LEN];
    // Remove spaces and replace parenthesis with commas
    for(size_t i = 0; i < MAX_OPERANDS_LEN; i++){
        if(operands_token[i] == ' '){
            continue;
        }
        if(operands_token[i] == '('){
            operands_token[o_idx] = ',';
            o_idx++;
        }
        else if(is_valid_hex_string(&operands_token[i], 1) || operands_token[i] == ','){
            operands_token[o_idx] = operands_token[i];
            o_idx++;
        }
        if(o_idx <= i){
            operands_token[i] = 0;
        }
    }
    // Clear binary buffer
    memset(&bin_buffer, 0, sizeof(bin_buffer));
    switch (format) {
    case E:
        if(build_E(opcode, NULL, bin_buffer, format) != 0){
            return NULL;
        }
        break;
    case I:
        if(build_I(opcode, operands_token, bin_buffer, format) != 0){
            return NULL;
        }
        break;
    case IE:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case MII:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case RIa:
    case RIb:
    case RIc:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case RIEa:
    case RIEb:
    case RIEc:
    case RIEd:
    case RIEe:
    case RIEf:
    case RIEg:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case RILa:
    case RILb:
    case RILc:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case RIS:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case RR:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case RRD:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case RRE:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case RRFa:
    case RRFb:
    case RRFc:
    case RRFd:
    case RRFe:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case RRS:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case RSa:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case RSb:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case RSI:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case RSLa:
    case RSLb:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case RSYa:
    case RSYb:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //} 
        break;
    case RXa:
    case RXb:
        if(build_RX(opcode, operands_token, bin_buffer, format) != 0){
            return NULL;
        }
        break;
    case RXE:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case RXF:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case RXYa:
    case RXYb:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case S:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case SI:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case SIL:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case SIY:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case SMI:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case SSa:
    case SSb:
    case SSc:
    case SSd:
    case SSe:
    case SSf:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case SSE:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case SSF:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case VRIa:
    case VRIb:
    case VRIc:
    case VRId:
    case VRIe:
    case VRIf:
    case VRIg:
    case VRIh:
    case VRIi:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case VRRa:
    case VRRb:
    case VRRc:
    case VRRd:
    case VRRe:
    case VRRf:
    case VRRg:
    case VRRh:
    case VRRi:
    case VRRj:
    case VRRk:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //} 
        break;
    case VRSa:
    case VRSb:
    case VRSc:
    case VRSd:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //} 
        break;
    case VRV:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case VRX:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    case VSI:
        //if(build_(opcode, operands_token, bin_buffer, format) != 0){
        //    return NULL;
        //}
        break;
    default:
        return NULL;
    }
    Instruction* instr = (Instruction*)malloc(sizeof(Instruction));
    if(instr == NULL){
        return NULL;
    }
    memcpy(&instr->mnemonic, mnemonic_token, MAX_MNEMONIC_LEN);
    memcpy(&instr->binary, bin_buffer, MAX_INSTRUCTION_LEN);
    instr->offset = offset;
    instr->next = NULL;
    return instr;
}

int build_E(uint16_t opcode, char* operands_token, uint8_t* bin_buffer, InstructionFormat format){
     // Opcode: bits(0-7)
    bin_buffer[0] = opcode >> 8;
     // Opcode: bits(8-16)
    bin_buffer[1] = opcode & 0x00FF;
    return 0;
}

int build_I(uint16_t opcode, char* operands_token, uint8_t* bin_buffer, InstructionFormat format){
    uint8_t i; 
    char buffer[MAX_OPERANDS_LEN];
    // Opcode: bits(0-7)
    bin_buffer[0] = opcode & 0x00FF;
    // Opcode: bits(8-15)
    if(!is_valid_hex_string(operands_token, 1)){
        return -1;
    }
    i = char_2_hex(operands_token);
    bin_buffer[1] = i;
    return 0;
}

int build_RX(uint16_t opcode, char* operands_token, uint8_t* bin_buffer, InstructionFormat format){
    uint8_t r1_m1;
    uint16_t d2;
    uint8_t x2;
    uint8_t b2;
    uint32_t conv_char;
    char buffer[MAX_OPERANDS_LEN];
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state;
    switch (format){
    case RXa:
        state = R1;
        break;
    case RXb:
        state = M1;
        break;
    default:
        return -1;
    }
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    for(size_t i = 0; i < MAX_OPERANDS_LEN && run;){
        switch (state){
        case R1:
        case M1:
            if(operands_token[i] == ','){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                r1_m1 = char_2_hex(buffer);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = D2;
                i++;
            }
            else{
                if(b_idx > MAX_REG_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case D2:
            if(operands_token[i] == ','){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                d2 = char_2_hex(buffer);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = X2;
                i++;
            }
            else{
                if(b_idx > MAX_DISP_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case X2:
            if(operands_token[i] == ','){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                x2 = char_2_hex(buffer);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = B2;
                i++;
            }
            else{
                if(b_idx > MAX_REG_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case B2:
            if(operands_token[i] == 0){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                b2 = char_2_hex(buffer);
                state = OPS_DONE;
                i++;
            }
            else{
                if(b_idx > MAX_REG_LEN){
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
    // Opcode: bits(0-7)
    bin_buffer[0] = opcode;
    // R1/M1: bits(8-11)
    bin_buffer[1] = r1_m1;
    bin_buffer[1] = bin_buffer[1] << 4;
    // X2: bits(12-15)
    bin_buffer[1] = bin_buffer[1] | x2;
    // B2: bits(16-19)
    bin_buffer[2] = b2;
    bin_buffer[2] = bin_buffer[2] << 4;
    // D2: bits(20-31)
    bin_buffer[2] = bin_buffer[2] | (d2 >> 8);
    bin_buffer[3] = d2 & 0x00FF;
    return 0;
}

int display_E(Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    uint16_t ret_opcode = mnemonic_to_opcode(instr->mnemonic);
    uint8_t ret_length = mnemonic_to_length(instr->mnemonic);
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    printf("+----------------+\n");
    printf("|     OPCODE     |\n");
    printf("+----------------+\n");
    printf("0               F \n");
    // Print general information
    printf("MNEMONIC: %s\n", instr->mnemonic);
    hex_2_char((void*)&ret_opcode, sizeof(ret_opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_2_char((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", ret_length);
    printf("FORMAT:   E\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    return 0;
}

int display_I(Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    uint16_t ret_opcode = mnemonic_to_opcode(instr->mnemonic);
    uint8_t ret_length = mnemonic_to_length(instr->mnemonic);
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    printf("+--------+--------+\n");
    printf("| OPCODE |   I    |\n");
    printf("+--------+--------+\n");
    printf("0        8        F\n");
    // Print general information
    printf("MNEMONIC: %s\n", instr->mnemonic);
    hex_2_char((void*)&ret_opcode, sizeof(ret_opcode), 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_2_char((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", ret_length);
    printf("FORMAT:   I\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    return 0;
}

int display_RX(Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    uint16_t ret_opcode = mnemonic_to_opcode(instr->mnemonic);
    uint8_t ret_length = mnemonic_to_length(instr->mnemonic);
    InstructionFormat ret_format = mnemonic_to_format(instr->mnemonic);
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    switch (ret_format){
    case RXa:
        printf("+--------+----+----+----+------------+\n");
        printf("| OPCODE | R1 | X2 | B2 |     D2     |\n");
        printf("+--------+----+----+----+------------+\n");
        printf("0        8    C    10   14          1F\n");
        break;
    case RXb:
        printf("+--------+----+----+----+------------+\n");
        printf("| OPCODE | M1 | X2 | B2 |     D2     |\n");
        printf("+--------+----+----+----+------------+\n");
        printf("0        8    C    10   14          1F\n");
        break;
    default:
        break;
    }
    // Print general information
    printf("MNEMONIC: %s\n", instr->mnemonic);
    hex_2_char((void*)&ret_opcode, sizeof(ret_opcode), 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_2_char((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 8, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", ret_length);
    switch (ret_format){
    case RXa:
        printf("FORMAT:   RXa\n");
        break;
    case RXb:
        printf("FORMAT:   RXb\n");
        break;
    default:
        break;
    }
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    switch (ret_format){
    case RXa:
        hex_2_char(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("R1:       %s\n", conv_buffer);
        break;
    case RXb:
        hex_2_char(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("M1:       %s\n", conv_buffer);
        break;
    default:
        break;
    }
    hex_2_char(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
    printf("X2:       %s\n", conv_buffer);
    hex_2_char(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
    printf("B2:       %s\n", conv_buffer);
    hex_2_char(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 3, SKIP, false);
    printf("D2:       %s\n", conv_buffer);
    return 0;
}

void InstructionStream_init(){
    InstructionStream.head = NULL;
    InstructionStream.tail = NULL;
    InstructionStream.n_instr = 0;
}

void InstructionStream_free(){
    Instruction* curr;
    Instruction* next;
    curr = InstructionStream.head;
    while(curr != NULL){
        next = curr->next;
        free(curr);
        curr = next;
    }
}

int InstructionStream_add_instruction(Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    if(InstructionStream.n_instr == 0){
        InstructionStream.head = instr;
        InstructionStream.tail = instr;
        InstructionStream.n_instr++;
        return 0;
    }
    InstructionStream.tail->next = instr;
    InstructionStream.tail = instr;
    InstructionStream.n_instr++;
    return 0;
}

int InstructionStream_display(){
    Instruction* curr = InstructionStream.head;
    InstructionFormat format;
    int ret;
    while (curr != NULL){
        format = mnemonic_to_format(curr->mnemonic);
        switch (format) {
        case E:
            ret = display_E(curr);
            break;
        case I:
            ret = display_I(curr);
            break;
        case IE:
            //ret = display_(curr);
            break;
        case MII:
            //ret = display_(curr);
            break;
        case RIa:
        case RIb:
        case RIc:
            //ret = display_(curr);
            break;
        case RIEa:
        case RIEb:
        case RIEc:
        case RIEd:
        case RIEe:
        case RIEf:
        case RIEg:
            //ret = display_(curr);
            break;
        case RILa:
        case RILb:
        case RILc:
            //ret = display_(curr);
            break;
        case RIS:
            //ret = display_(curr);
            break;
        case RR:
            //ret = display_(curr);
            break;
        case RRD:
            //ret = display_(curr);
            break;
        case RRE:
            //ret = display_(curr);
            break;
        case RRFa:
        case RRFb:
        case RRFc:
        case RRFd:
        case RRFe:
            //ret = display_(curr);
            break;
        case RRS:
            //ret = display_(curr);
            break;
        case RSa:
        case RSb:
            //ret = display_(curr);
            break;
        case RSI:
            //ret = display_(curr);
            break;
        case RSLa:
        case RSLb:
            //ret = display_(curr);
            break;
        case RSYa:
        case RSYb:
            //ret = display_(curr); 
            break;
        case RXa:
        case RXb:
            ret = display_RX(curr);  
            break;
        case RXE:
            //ret = display_(curr);
            break;
        case RXF:
            //ret = display_(curr);
            break;
        case RXYa:
        case RXYb:
            //ret = display_(curr);
            break;
        case S:
            //ret = display_(curr);
            break;
        case SI:
            //ret = display_(curr);
            break;
        case SIL:
            //ret = display_(curr);
            break;
        case SIY:
            //ret = display_(curr);
            break;
        case SMI:
            //ret = display_(curr);
            break;
        case SSa:
        case SSb:
        case SSc:
        case SSd:
        case SSe:
        case SSf:
            //ret = display_(curr);
            break;
        case SSE:
            //ret = display_(curr);
            break;
        case SSF:
            //ret = display_(curr);
            break;
        case VRIa:
        case VRIb:
        case VRIc:
        case VRId:
        case VRIe:
        case VRIf:
        case VRIg:
        case VRIh:
        case VRIi:
            //ret = display_(curr);
            break;
        case VRRa:
        case VRRb:
        case VRRc:
        case VRRd:
        case VRRe:
        case VRRf:
        case VRRg:
        case VRRh:
        case VRRi:
        case VRRj:
        case VRRk:
            //ret = display_(curr); 
            break;
        case VRSa:
        case VRSb:
        case VRSc:
        case VRSd:
            //ret = display_(curr); 
            break;
        case VRV:
            //ret = display_(curr);
            break;
        case VRX:
            //ret = display_(curr);
            break;
        case VSI:
            //ret = display_(curr);
            break;
        default:
            break;
        }
        if(ret != 0){
            return -1;
        }
        curr = curr->next;
    }
    return 0;
}