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
        display_E(instr);
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

bool is_hex_char(const char* input, size_t length){
    const char* hex_chars = "0123456789ABCDEFabcdef";
    size_t hex_chars_len = strlen(hex_chars);
    for(size_t i = 0; i < length; i++){
        for(size_t j = 0; j < hex_chars_len; j++){
            if(input[i] == hex_chars[j]){
                return true;
            }
        }
    }
    return false;
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

int hex_2_char(void* input, char* output, size_t n_chars){
    if(input == NULL || output == NULL){
        return -1;
    }
    char buffer[MAX_PRINTOUT_FIELD_LEN];
    uint8_t* input_bytes  = (uint8_t*)input;
    size_t byte_i = 0;
    uint8_t binary_char = 0;
    // Clear output buffer
    for(size_t i = 0; i < MAX_PRINTOUT_FIELD_LEN + 1; i++){
        output[i] = 0;
    }
    for(size_t i = 0; i < n_chars; i++){
        if(i % 2 == 0 && i != 0){
            byte_i++;
        }
        if(i % 2 == 0){
            binary_char = input_bytes[byte_i] & 0x0F;
        }
        else{
            binary_char = input_bytes[byte_i] >> 4;
        }
        switch (binary_char)
        {
        case 0xA:
        case 0xB:
        case 0xC:
        case 0xD:
        case 0xE:
        case 0xF:
            output[n_chars - i - 1] = ((binary_char - 1) & 0x07) | 0x40;
            break;        
        default:
            output[n_chars - i - 1] = binary_char | 0x30;
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
    // Remove spaces and replace parenthesis with commas
    for(size_t i = 0; i < MAX_OPERANDS_LEN; i++){
        if(operands_token[i] == ' '){
            continue;
        }
        if(operands_token[i] == '('){
            operands_token[o_idx] = ',';
            o_idx++;
        }
        else if(is_hex_char(&operands_token[i], 1) || operands_token[i] == ','){
            operands_token[o_idx] = operands_token[i];
            o_idx++;
        }
        if(o_idx <= i){
            operands_token[i] = 0;
        }
    }
    // Clear binary buffer
    memset(&bin_buffer, 0, sizeof(bin_buffer));
    switch(format) {
        case E:
            if(build_E(opcode, NULL, bin_buffer) != 0){
                return NULL;
            }
            break;
        case RXa:
            if(build_RXa(opcode, operands_token, bin_buffer) != 0){
                return NULL;
            }
            break;
        case RXYa:
            break;
        case SSe:
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

int build_E(uint16_t opcode, char* operands_token, uint8_t* bin_buffer){
     // Opcode: bits(0-8)
    bin_buffer[0] = opcode >> 8;
     // Opcode: bits(0-8)
    bin_buffer[0] = opcode & 0x00FF;
    return 0;
}

int build_RXa(uint16_t opcode, char* operands_token, uint8_t* bin_buffer){
    uint8_t r1;
    uint16_t d2;
    uint8_t x2;
    uint8_t b2;
    uint32_t conv_char;
    char buffer[MAX_OPERANDS_LEN];
    OperandsParseState state = R1;
    bool run = true;
    size_t b_idx = 0;
    // Clear buffer
    memset(&buffer, 0, sizeof(buffer));
    for(size_t i = 0; i < MAX_OPERANDS_LEN && run;){
        switch (state){
        case R1:
            if(operands_token[i] == ','){
                if(!is_hex_char(buffer, b_idx)){
                    return -1;
                }
                r1 = char_2_hex(buffer);
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
                if(!is_hex_char(buffer, b_idx)){
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
                if(!is_hex_char(buffer, b_idx)){
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
                if(!is_hex_char(buffer, b_idx)){
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
    // Opcode: bits(0-8)
    bin_buffer[0] = opcode;
    // R1/M1: bits(8-11)
    bin_buffer[1] = r1;
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
    printf("+----------------+\n");
    printf("|     OPCODE     |\n");
    printf("+----------------+\n");
    printf("0               F \n");
    printf("MNEMONIC: %s\n", instr->mnemonic);
    hex_2_char((void*)&ret_opcode, conv_buffer, 4);
    printf("OPCODE:   %s\n", conv_buffer);
    //printf("LENGTH: 0x%x", mnemonic_to_length);
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
