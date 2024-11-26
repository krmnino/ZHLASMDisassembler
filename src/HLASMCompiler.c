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
                else if(line[i] == ' '){
                    i++;
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

int char_str_2_hex_str(const char* input, size_t input_len, void* output, size_t output_len, size_t n_chars, size_t skip, bool little_endian){
    if(input == NULL || output == NULL){
        return -1;
    }
    uint8_t* output_bytes = (uint8_t*)output;
    size_t byte_i = 0;
    uint8_t str_char = 0;
    uint8_t skipper = 0;
    if(little_endian){
        skipper = skip || (n_chars % 2);
    }
    // Clear output buffer
    for(size_t i = 0; i < output_len; i++){
        output_bytes[i] = 0;
    }
    if(little_endian){
        byte_i = output_len - 1 - (output_len - ((n_chars + 1) / 2));
    }
    else{
        byte_i = 0;
    }
    for(size_t i = 0; i < n_chars; i++){
        if((i + skipper) % 2 == 0 && i != 0){
            if(little_endian){
                byte_i--;
            }
            else{
                byte_i++;
            }
        }
        str_char = input[i];
        switch (str_char) {
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            str_char = ((str_char & 0x0F) | 0x08) + 1;
            break;        
        default:
            str_char = str_char & 0x0F;
            break;
        }
        if((i + skipper) % 2 == 0){
            output_bytes[byte_i] = (output_bytes[byte_i] | str_char) << 4;
        }
        else{
            output_bytes[byte_i] = output_bytes[byte_i] | str_char;
        }
    }
    return 0;
}
int hex_str_2_char_str(const void* input, size_t input_len , size_t input_offset, char* output, size_t output_len, size_t n_chars, size_t skip, bool little_endian){
    if(input == NULL || output == NULL){
        return -1;
    }
    uint8_t* input_bytes = (uint8_t*)input;
    size_t byte_i = 0;
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
        switch (binary_char){
        case 0xA:
        case 0xB:
        case 0xC:
        case 0xD:
        case 0xE:
        case 0xF:
            output[i] = ((binary_char - 1) & 0x07) | 0x40;
            break;        
        default:
            output[i] = binary_char | 0x30;
            break;
        }
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
    int ret;
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
        ret = build_E(opcode, NULL, bin_buffer, format);
        break;
    case I:
        ret = build_I(opcode, operands_token, bin_buffer, format);
        break;
    case IE:
        ret = build_IE(opcode, operands_token, bin_buffer, format);
        break;
    case MII:
        ret = build_MII(opcode, operands_token, bin_buffer, format);
        break;
    case RIa:
    case RIb:
    case RIc:
        ret = build_RI(opcode, operands_token, bin_buffer, format);
        break;
    case RIEa:
    case RIEb:
    case RIEc:
    case RIEd:
    case RIEe:
    case RIEf:
    case RIEg:
        ret = build_RIE(opcode, operands_token, bin_buffer, format);
        break;
    case RILa:
    case RILb:
    case RILc:
        ret = build_RIL(opcode, operands_token, bin_buffer, format);
        break;
    case RIS:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case RR:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case RRD:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case RRE:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case RRFa:
    case RRFb:
    case RRFc:
    case RRFd:
    case RRFe:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case RRS:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case RSa:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case RSb:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case RSI:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case RSLa:
    case RSLb:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case RSYa:
    case RSYb:
        //ret = build_(opcode, operands_token, bin_buffer, format); 
        break;
    case RXa:
    case RXb:
        ret = build_RX(opcode, operands_token, bin_buffer, format);
        break;
    case RXE:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case RXF:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case RXYa:
    case RXYb:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case S:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case SI:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case SIL:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case SIY:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case SMI:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case SSa:
    case SSb:
    case SSc:
    case SSd:
    case SSe:
    case SSf:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case SSE:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case SSF:
        //ret = build_(opcode, operands_token, bin_buffer, format);
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
        //ret = build_(opcode, operands_token, bin_buffer, format);
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
        //ret = build_(opcode, operands_token, bin_buffer, format); 
        break;
    case VRSa:
    case VRSb:
    case VRSc:
    case VRSd:
        //ret = build_(opcode, operands_token, bin_buffer, format); 
        break;
    case VRV:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case VRX:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    case VSI:
        //ret = build_(opcode, operands_token, bin_buffer, format);
        break;
    default:
        return NULL;
    }
    if(ret != 0){
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
    if(!is_valid_hex_string(operands_token, 1)){
        return -1;
    }
    char_str_2_hex_str(operands_token, MAX_OPERANDS_LEN, (void*)&i, sizeof(i), 2, NO_SKIP, true);
     // Opcode: bits(0-7)
    bin_buffer[0] = opcode;
    // I: bits(8-15)
    bin_buffer[1] = i;
    return 0;
}

int build_IE(uint16_t opcode, char* operands_token, uint8_t* bin_buffer, InstructionFormat format){
    uint8_t i1; 
    uint8_t i2; 
    char buffer[MAX_OPERANDS_LEN];
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state = I1;
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    for(size_t i = 0; i < MAX_OPERANDS_LEN && run;){
        switch (state){
        case I1:
            if(operands_token[i] == ','){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i1, sizeof(i1), 1, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = I2;
                i++;
            }
            else{
                if(b_idx > MAX_1CHR_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case I2:
            if(operands_token[i] == 0){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i2, sizeof(i2), 1, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = OPS_DONE;
                i++;
            }
            else{
                if(b_idx > MAX_1CHR_LEN){
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
    bin_buffer[0] = opcode >> 8;
     // Opcode: bits(8-16)
    bin_buffer[1] = opcode & 0x00FF;
    // I1: bits(24-27)
    bin_buffer[3] = i1;
    bin_buffer[3] = bin_buffer[3] << 4;
    // I2: bits(28-31)
    bin_buffer[3] = bin_buffer[3] | i2;
    return 0;
}

int build_MII(uint16_t opcode, char* operands_token, uint8_t* bin_buffer, InstructionFormat format){
    uint8_t m1; 
    uint16_t ri2; 
    uint32_t ri3; 
    char buffer[MAX_OPERANDS_LEN];
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state = M1;
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    for(size_t i = 0; i < MAX_OPERANDS_LEN && run;){
        switch (state){
        case M1:
            if(operands_token[i] == ','){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&m1, sizeof(m1), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = RI2;
                i++;
            }
            else{
                if(b_idx > MAX_1CHR_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case RI2:
            if(operands_token[i] == ','){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&ri2, sizeof(ri2), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = RI3;
                i++;
            }
            else{
                if(b_idx > MAX_3CHR_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case RI3:
            if(operands_token[i] == 0){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&ri3, sizeof(ri3), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = OPS_DONE;
                i++;
            }
            else{
                if(b_idx > MAX_6CHR_LEN){
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
    // M1: bits(8-11)
    bin_buffer[1] = m1 << 4;
    // RI2: bits(12-23)
    bin_buffer[1] = bin_buffer[1] | (ri2 >> 8);
    bin_buffer[2] = ri2 & 0x00FF;
    // RI3: bits(24-47)
    bin_buffer[3] = ri3 >> 16;
    bin_buffer[4] = (ri3 >> 8) & 0x0000FF;
    bin_buffer[5] = ri3 & 0x0000FF;
    return 0;
}

int build_RI(uint16_t opcode, char* operands_token, uint8_t* bin_buffer, InstructionFormat format){
    uint8_t r1_m1; 
    uint16_t i2_ri2; 
    char buffer[MAX_OPERANDS_LEN];
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
    for(size_t i = 0; i < MAX_OPERANDS_LEN && run;){
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
                if(b_idx > MAX_1CHR_LEN){
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
                if(b_idx > MAX_4CHR_LEN){
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

int build_RIE(uint16_t opcode, char* operands_token, uint8_t* bin_buffer, InstructionFormat format){
    uint8_t r1; 
    uint8_t r2; 
    uint8_t r3_m3; 
    uint8_t i2; 
    uint8_t i3; 
    uint8_t i4; 
    uint8_t i5; 
    uint16_t i2_ri2_ri4; 
    char buffer[MAX_OPERANDS_LEN];
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state = R1;
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    for(size_t i = 0; i < MAX_OPERANDS_LEN && run;){
        switch (state){
        case R1:
            if(operands_token[i] == ','){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r1, sizeof(r1), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                switch (format){
                case RIEa:
                case RIEc:
                case RIEg:
                    state = I2;
                    break;
                case RIEb:
                case RIEf:
                    state = R2;
                    break;
                case RIEd:
                case RIEe:
                    state = R3;
                    break;
                default:
                    break;
                }
                i++;
            }
            else{
                if(b_idx > MAX_1CHR_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case R2:
            if(operands_token[i] == ','){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r2, sizeof(r2), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                switch (format){
                case RIEb:
                    state = M3;
                    break;
                case RIEf:
                    state = I3;
                    break;
                default:
                    break;
                }
                i++;
            }
            else{
                if(b_idx > MAX_1CHR_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case I2:
        case RI2:
        case RI4:
            if(operands_token[i] == ',' || operands_token[i] == 0){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                switch (format){
                case RIEa:
                case RIEb:
                case RIEd:
                case RIEe:
                case RIEg:
                    char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i2_ri2_ri4, sizeof(i2_ri2_ri4), b_idx, NO_SKIP, true);
                    break;
                case RIEc:
                    if(state == RI4){
                        char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i2_ri2_ri4, sizeof(i2_ri2_ri4), b_idx, NO_SKIP, true);
                    }
                    else{
                        char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i2, sizeof(i2), b_idx, NO_SKIP, true);
                    }
                    break;
                default:
                    break;
                }
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                switch (format){
                case RIEa:
                case RIEg:
                    state = M3;
                    break;
                case RIEc:
                    if(state == RI4){
                        state = OPS_DONE;
                    }
                    else{
                        state = M3;
                    }
                    break;
                case RIEb:
                case RIEd:
                case RIEe:
                    state = OPS_DONE;
                    break;
                default:
                    break;
                }
                i++;
            }
            else{
                switch (format){
                case RIEa:
                case RIEd:
                case RIEg:
                    if(b_idx > MAX_4CHR_LEN){
                        return -1;
                    }
                    break;
                case RIEc:
                    if(state == RI4){ 
                        if(b_idx > MAX_4CHR_LEN){
                            return -1;
                        }
                    }
                    else{
                        if(b_idx > MAX_2CHR_LEN){
                            return -1;
                        }
                    }
                    break;
                default:
                    break;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case R3:
        case M3:
            if(operands_token[i] == ',' || operands_token[i] == 0){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r3_m3, sizeof(r3_m3), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                switch (format){
                case RIEa:
                case RIEg:
                    state = OPS_DONE;
                    break;
                case RIEb:
                case RIEc:
                    state = RI4;
                    break;
                case RIEd:
                    state = I2;
                    break;
                case RIEe:
                    state = RI2;
                    break;
                default:
                    break;
                }
                i++;
            }
            else{
                if(b_idx > MAX_1CHR_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case I3:
            if(operands_token[i] == ',' || operands_token[i] == 0){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i3, sizeof(i3), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = I4;
                i++;
            }
            else{
                if(b_idx > MAX_2CHR_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case I4:
            if(operands_token[i] == ',' || operands_token[i] == 0){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i4, sizeof(i4), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = I5;
                i++;
            }
            else{
                if(b_idx > MAX_2CHR_LEN){
                    return -1;
                }
                buffer[b_idx] = operands_token[i];
                b_idx++;
                i++;
            }
            break;
        case I5:
            if(operands_token[i] == 0){
                if(!is_valid_hex_string(buffer, b_idx)){
                    return -1;
                }
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&i5, sizeof(i5), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = OPS_DONE;
                i++;
            }
            else{
                if(b_idx > MAX_2CHR_LEN){
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
    // Opcode (part 1): bits(0-7)
    bin_buffer[0] = opcode >> 8;
    // R1/M1: bits(8-11)
    bin_buffer[1] = r1 << 4;
    // R2/I2/RI2: 
    switch (format){
    case RIEa:        
    case RIEd:        
    case RIEe:        
    case RIEg:        
        // I2: Bits(16-31)
        bin_buffer[2] = i2_ri2_ri4 >> 8;
        bin_buffer[3] = bin_buffer[3] | (i2_ri2_ri4 & 0x00FF);
        break;
    case RIEb:
    case RIEf:
        // R2: Bits(12-15)
        bin_buffer[1] = bin_buffer[1] | r2;
        break;
    case RIEc:
        // I2: Bits(32-39)
        bin_buffer[4] = i2;
        break;        
    default:
        break;
    }
    // R3/M3/I3:
    switch (format){
    case RIEa:        
    case RIEb:        
        // M3: Bits(32-35)
        bin_buffer[4] = r3_m3 << 4;
        break;
    case RIEc:        
    case RIEd:        
    case RIEe:        
    case RIEg:        
        // R3/M3: Bits(1-35)
        bin_buffer[1] = bin_buffer[1] | r3_m3;
        break;
    case RIEf:        
        // I3: Bits(16-23)
        bin_buffer[2] = i3;
        break;
    default:
        break;
    }
    // RI4/I4: 
    switch (format){
    case RIEb:        
    case RIEc:        
        // RI4: Bits(32-35)
        bin_buffer[2] = i2_ri2_ri4 >> 8;
        bin_buffer[3] = bin_buffer[3] | (i2_ri2_ri4 & 0x00FF);
        break;
    case RIEf:        
        // I4: Bits(24-31)
        bin_buffer[3] = i4;
        break;
    default:
        break;
    }
    // I5:  
    switch (format){
    case RIEf:        
        // I4: Bits(32-39)
        bin_buffer[4] = i5;
        break;
    default:
        break;
    }
    // Opcode (part 2): bits(40-47)
    bin_buffer[5] = opcode & 0x00FF;
    return 0;
}

int build_RIL(uint16_t opcode, char* operands_token, uint8_t* bin_buffer, InstructionFormat format){
    uint8_t r1_m1; 
    uint32_t i2_ri2; 
    char buffer[MAX_OPERANDS_LEN];
    bool run = true;
    size_t b_idx = 0;
    OperandsParseState state;
    switch (format){
    case RILa:
    case RILb:
        state = R1;
        break;
    case RILc:
        state = M1;
        break;
    default:
        break;
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
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r1_m1, sizeof(r1_m1), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                switch (format){
                case RILa:
                    state = I2;
                    break;
                case RILb:
                case RILc:
                    state = RI2;
                    break;
                default:
                    break;
                }
                i++;
            }
            else{
                if(b_idx > MAX_1CHR_LEN){
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
                if(b_idx > MAX_8CHR_LEN){
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
     // Opcode (part 1): bits(0-7)
    bin_buffer[0] = opcode >> 4;
    // R1/M1: bits(8-11)
    bin_buffer[1] = r1_m1 << 4;
    // Opcode (part 2): bits(12-15)
    bin_buffer[1] = bin_buffer[1] | (opcode & 0x000F);
    // I2/RI2: bits(16-47)
    bin_buffer[2] = i2_ri2 >> 24;
    bin_buffer[3] = (i2_ri2 >> 16) & 0x000000FF;
    bin_buffer[4] = (i2_ri2 >> 8) & 0x000000FF;
    bin_buffer[5] = i2_ri2 & 0x000000FF;
    return 0;
}

int build_RX(uint16_t opcode, char* operands_token, uint8_t* bin_buffer, InstructionFormat format){
    uint8_t r1_m1;
    uint16_t d2;
    uint8_t x2;
    uint8_t b2;
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
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&r1_m1, sizeof(r1_m1), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = D2;
                i++;
            }
            else{
                if(b_idx > MAX_1CHR_LEN){
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
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&d2, sizeof(d2), b_idx, NO_SKIP, true);
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
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&x2, sizeof(x2), b_idx, NO_SKIP, true);
                memset(&buffer, 0, sizeof(buffer));
                b_idx = 0;
                state = B2;
                i++;
            }
            else{
                if(b_idx > MAX_1CHR_LEN){
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
                char_str_2_hex_str(buffer, MAX_OPERANDS_LEN, (void*)&b2, sizeof(b2), b_idx, NO_SKIP, true);
                state = OPS_DONE;
                i++;
            }
            else{
                if(b_idx > MAX_1CHR_LEN){
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
    printf("0                F\n");
    // Print general information
    printf("MNEMONIC: %s\n", instr->mnemonic);
    hex_str_2_char_str((void*)&ret_opcode, sizeof(ret_opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
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
    hex_str_2_char_str((void*)&ret_opcode, sizeof(ret_opcode), 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", ret_length);
    printf("FORMAT:   I\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, false);
    printf("I:        %s\n", conv_buffer);
    return 0;
}

int display_IE(Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    uint16_t ret_opcode = mnemonic_to_opcode(instr->mnemonic);
    uint8_t ret_length = mnemonic_to_length(instr->mnemonic);
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    printf("+----------------+--------+----+----+\n");
    printf("|     OPCODE     | ////// | I1 | I2 |\n");
    printf("+----------------+--------+----+----+\n");
    printf("0                1A       18   1C  1F\n");
    // Print general information
    printf("MNEMONIC: %s\n", instr->mnemonic);
    hex_str_2_char_str((void*)&ret_opcode, sizeof(ret_opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 8, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", ret_length);
    printf("FORMAT:   IE\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
    printf("I1:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
    printf("I2:       %s\n", conv_buffer);
    return 0;
}

int display_MII(Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    uint16_t ret_opcode = mnemonic_to_opcode(instr->mnemonic);
    uint8_t ret_length = mnemonic_to_length(instr->mnemonic);
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    printf("+--------+----+------------+------------------------+\n");
    printf("| OPCODE | M1 |    RI2     |          RI3           |\n");
    printf("+--------+----+------------+------------------------+\n");
    printf("0        8    C            18                      2F\n");
    // Print general information
    printf("MNEMONIC: %s\n", instr->mnemonic);
    hex_str_2_char_str((void*)&ret_opcode, sizeof(ret_opcode), 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 12, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", ret_length);
    printf("FORMAT:   MII\n");
    printf("OFFSET:   0x%lx\n", instr->offset);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
    printf("M1:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 3, SKIP, false);
    printf("RI2:      %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 6, NO_SKIP, false);
    printf("RI3:      %s\n", conv_buffer);
    return 0;
}

int display_RI(Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    uint16_t ret_opcode = mnemonic_to_opcode(instr->mnemonic);
    uint8_t ret_length = mnemonic_to_length(instr->mnemonic);
    InstructionFormat ret_format = mnemonic_to_format(instr->mnemonic);
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    switch (ret_format){
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
    printf("MNEMONIC: %s\n", instr->mnemonic);
    hex_str_2_char_str((void*)&ret_opcode, sizeof(ret_opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 3, SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 8, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", ret_length);
    switch (ret_format){
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
    switch (ret_format){
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

int display_RIE(Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    uint16_t ret_opcode = mnemonic_to_opcode(instr->mnemonic);
    uint8_t ret_length = mnemonic_to_length(instr->mnemonic);
    InstructionFormat ret_format = mnemonic_to_format(instr->mnemonic);
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    switch (ret_format){
    case RIEa:
        printf("+--------+----+----+----------------+----+----+--------+\n");
        printf("| OPCODE | R1 | // |       I2       | M3 | // | OPCODE |\n");
        printf("+--------+----+----+----------------+----+----+--------+\n");
        printf("0        8    C    10               20   24   28      2F\n");
        break;
    case RIEb:
        printf("+--------+----+----+----------------+----+----+--------+\n");
        printf("| OPCODE | R1 | R2 |      RI4       | M3 | // | OPCODE |\n");
        printf("+--------+----+----+----------------+----+----+--------+\n");
        printf("0        8    C    10               20   24   28      2F\n");
        break;
    case RIEc:
        printf("+--------+----+----+----------------+--------+--------+\n");
        printf("| OPCODE | R1 | M3 |      RI4       |   I2   | OPCODE |\n");
        printf("+--------+----+----+----------------+--------+--------+\n");
        printf("0        8    C    10               20       28      2F\n");
        break;
    case RIEd:
        printf("+--------+----+----+----------------+--------+--------+\n");
        printf("| OPCODE | R1 | R3 |       I2       | ////// | OPCODE |\n");
        printf("+--------+----+----+----------------+--------+--------+\n");
        printf("0        8    C    10               20       28      2F\n");
        break;
    case RIEe:
        printf("+--------+----+----+----------------+--------+--------+\n");
        printf("| OPCODE | R1 | R3 |      RI2       | ////// | OPCODE |\n");
        printf("+--------+----+----+----------------+--------+--------+\n");
        printf("0        8    C    10               20       28      2F\n");
        break;
    case RIEf:
        printf("+--------+----+----+--------+--------+--------+--------+\n");
        printf("| OPCODE | R1 | R2 |   I3   |   I4   |   I5   | OPCODE |\n");
        printf("+--------+----+----+--------+--------+--------+--------+\n");
        printf("0        8    C    10       18       20       28      2F\n");
        break;
    case RIEg:
        printf("+--------+----+----+----------------+--------+--------+\n");
        printf("| OPCODE | R1 | M3 |       I2       | ////// | OPCODE |\n");
        printf("+--------+----+----+----------------+--------+--------+\n");
        printf("0        8    C    10               20       28      2F\n");
        break;
    default:
        return -1;
    }
    // Print general information
    printf("MNEMONIC: %s\n", instr->mnemonic);
    hex_str_2_char_str((void*)&ret_opcode, sizeof(ret_opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 12, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", ret_length);
    switch (ret_format){
    case RIEa:
        printf("FORMAT:   RIEa\n");
        break;
    case RIEb:
        printf("FORMAT:   RIEb\n");
        break;
    case RIEc:
        printf("FORMAT:   RIEc\n");
        break;
    case RIEd:
        printf("FORMAT:   RIEd\n");
        break;
    case RIEe:
        printf("FORMAT:   RIEe\n");
        break;
    case RIEf:
        printf("FORMAT:   RIEf\n");
        break;
    case RIEg:
        printf("FORMAT:   RIEg\n");
        break;
    default:
        return -1;
    }
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
    printf("R1:       %s\n", conv_buffer);
    switch (ret_format){
    case RIEa:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
        printf("I2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("M3:       %s\n", conv_buffer);
        break;
    case RIEb:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("R2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
        printf("RI4:      %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("M3:       %s\n", conv_buffer);
        break;
    case RIEc:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("M3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
        printf("RI4:      %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, false);
        printf("I2:       %s\n", conv_buffer);
        break;
    case RIEd:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("R3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
        printf("I2:       %s\n", conv_buffer);
        break;
    case RIEe:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("R3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
        printf("RI2       %s\n", conv_buffer);
        break;
    case RIEf:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("R2:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, false);
        printf("I3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 3, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, false);
        printf("I4:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 4, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, false);
        printf("I5:       %s\n", conv_buffer);
        break;
    case RIEg:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
        printf("M3:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 4, NO_SKIP, false);
        printf("I2:       %s\n", conv_buffer);
        break;
        break;
    default:
        return -1;
    }
    return 0;
}

int display_RIL(Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    uint16_t ret_opcode = mnemonic_to_opcode(instr->mnemonic);
    uint8_t ret_length = mnemonic_to_length(instr->mnemonic);
    InstructionFormat ret_format = mnemonic_to_format(instr->mnemonic);
    char conv_buffer[MAX_PRINTOUT_FIELD_LEN];
    // Print instruction layout
    switch (ret_format){
    case RILa:
        printf("+--------+----+----+--------------------------------+\n");
        printf("| OPCODE | R1 | OP |               I2               |\n");
        printf("+--------+----+----+--------------------------------+\n");
        printf("0        8    C    10                              2F\n");
        break;
    case RILb:
        printf("+--------+----+----+--------------------------------+\n");
        printf("| OPCODE | R1 | OP |              RI2               |\n");
        printf("+--------+----+----+--------------------------------+\n");
        printf("0        8    C    10                              2F\n");
        break;
    case RILc:
        printf("+--------+----+----+--------------------------------+\n");
        printf("| OPCODE | M1 | OP |              RI2               |\n");
        printf("+--------+----+----+--------------------------------+\n");
        printf("0        8    C    10                              2F\n");
        break;
    default:
        return -1;
    }
    // Print general information
    printf("MNEMONIC: %s\n", instr->mnemonic);
    hex_str_2_char_str((void*)&ret_opcode, sizeof(ret_opcode), 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 3, SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 12, NO_SKIP, false);
    printf("BINARY:   %s\n", conv_buffer);
    printf("LENGTH:   0x%x\n", ret_length);
    switch (ret_format){
    case RILa:
        printf("FORMAT:   RILa\n");
        break;
    case RILb:
        printf("FORMAT:   RILb\n");
        break;
    case RILc:
        printf("FORMAT:   RIc\n");
        break;
    default:
        return -1;
    }
    printf("OFFSET:   0x%lx\n", instr->offset);
    switch (ret_format){
    case RILa:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("R1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 8, NO_SKIP, false);
        printf("I2:       %s\n", conv_buffer);
        break;
    case RILb:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("R1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 8, NO_SKIP, false);
        printf("RI2:      %s\n", conv_buffer);
        break;
    case RILc:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("M1:       %s\n", conv_buffer);
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 8, NO_SKIP, false);
        printf("RI2:      %s\n", conv_buffer);
        break;
    default:
        return -1;
    }
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
        return -1;
    }
    // Print general information
    printf("MNEMONIC: %s\n", instr->mnemonic);
    hex_str_2_char_str((void*)&ret_opcode, sizeof(ret_opcode), 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 2, NO_SKIP, true);
    printf("OPCODE:   %s\n", conv_buffer);
    hex_str_2_char_str((void*)&instr->binary, MAX_INSTRUCTION_LEN, 0, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 8, NO_SKIP, false);
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
        return -1;
    }
    printf("OFFSET:   0x%lx\n", instr->offset);
    // Print operands
    switch (ret_format){
    case RXa:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("R1:       %s\n", conv_buffer);
        break;
    case RXb:
        hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
        printf("M1:       %s\n", conv_buffer);
        break;
    default:
        return -1;
    }
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 1, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, SKIP, false);
    printf("X2:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 1, NO_SKIP, false);
    printf("B2:       %s\n", conv_buffer);
    hex_str_2_char_str(((void*)&instr->binary), MAX_INSTRUCTION_LEN, 2, conv_buffer, MAX_PRINTOUT_FIELD_LEN, 3, SKIP, false);
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
            ret = display_IE(curr);
            break;
        case MII:
            ret = display_MII(curr);
            break;
        case RIa:
        case RIb:
        case RIc:
            ret = display_RI(curr);
            break;
        case RIEa:
        case RIEb:
        case RIEc:
        case RIEd:
        case RIEe:
        case RIEf:
        case RIEg:
            ret = display_RIE(curr);
            break;
        case RILa:
        case RILb:
        case RILc:
            ret = display_RIL(curr);
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