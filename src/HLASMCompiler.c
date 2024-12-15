#include "HLASMCompiler.h"
#include "InstructionTable.h"

ErrorCode process_source_file(Context* c, const char* filename){
    FILE* file = file = fopen(filename, "r");
    if(file == NULL){
        // TODO:
        return CANNOT_OPEN_SRC_FILE;
    }
    char line[MAX_LINE_LEN];
    char mnemonic_token[MAX_MNEMONIC_LEN];
    char operands_token[MAX_OPERANDS_LEN];
    Address instr_offset;
    size_t m_idx;
    size_t o_idx;
    TokensParseState tp_state;
    bool run;
    bool skip_line;
    ErrorCode ret_ec;
    Instruction* instr;
    instr_offset = 0;
    tp_state = SPACES_PM;
    run = true;
    m_idx = 0;
    o_idx = 0;
    c->n_line = 1;
    while(fgets(line, sizeof(line), file)){
        skip_line = false;
        for(size_t i = 0; i < MAX_LINE_LEN && run;){
            switch (tp_state){
            case SPACES_PM:
                if(line[i] == 0 || line[i] == '\n'){
                    skip_line = true;
                    tp_state = TPS_DONE;
                }
                else if(line[i] == '!'){
                    tp_state = LINE_COMMENT;
                }
                else if(line[i] != ' '){
                    tp_state = MNEMONIC;
                }
                else{
                    i++;
                }
                break;
            case LINE_COMMENT:
                if(line[i] == 0 || line[i] == '\n'){
                    skip_line = true;
                    tp_state = TPS_DONE;
                }
                else{
                    i++;
                }
                break;
            case INLINE_COMMENT:
                if(line[i] == 0 || line[i] == '\n'){
                    tp_state = TPS_DONE;
                }
                else{
                    i++;
                }
                break;
            case MNEMONIC:
                if(line[i] == '!'){
                    tp_state = INLINE_COMMENT;
                }
                else if(line[i] == '\n'){
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
                if(line[i] == '!'){
                    tp_state = INLINE_COMMENT;
                }
                else if(line[i] != ' '){
                    tp_state = OPERANDS;
                }
                else{
                    i++;
                }
                break;
            case OPERANDS:
                if(line[i] == '!'){
                    tp_state = INLINE_COMMENT;
                }
                else if(line[i] == 0 || line[i] == '\n'){
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
        if(skip_line){
            tp_state = SPACES_PM;
            run = true;
            m_idx = 0;
            o_idx = 0;
            continue;
        }
        instr = Instruction_init(c, &ret_ec, mnemonic_token, operands_token, instr_offset);
        if(ret_ec != OK){
            return ret_ec;
        }
        if(add_instruction(c, instr) != 0){
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
        c->n_line++;
    }
    fclose(file);
    return OK;
}

bool is_valid_mnemonic(const char* mnemonic){
    if(mnemonic_to_table_index(mnemonic) != 0){
        return true;
    }
    return false;
}

bool is_valid_hex_string(const char* input, size_t length){
    const char* hex_chars = "0123456789ABCDEFabcdef";
    size_t hex_chars_len = strlen(hex_chars);
    bool valid = true;
    for(size_t i = 0; i < length && valid; i++){
        if(!((input[i] >= '0' && input[i] <= '9') || 
             (input[i] >= 'A' && input[i] <= 'F') || 
             (input[i] >= 'a' && input[i] <= 'f'))){
            valid = false;
        }
    }
    return valid;
}

InstructionFormat mnemonic_to_format(const char* mnemonic){
    for(size_t i = 0; i < N_INST; i++){
        if(strcmp(mnemonic, INSTRUCTION_TABLE[i].mnemonic) == 0){
            return INSTRUCTION_TABLE[i].format;
        }
    }
    return NF;
}

uint16_t mnemonic_to_opcode(const char* mnemonic){
    for(size_t i = 0; i < N_INST; i++){
        if(strcmp(mnemonic, INSTRUCTION_TABLE[i].mnemonic) == 0){
            return INSTRUCTION_TABLE[i].opcode;
        }
    }
    return OK;
}

uint8_t mnemonic_to_length(const char* mnemonic){
    for(size_t i = 0; i < N_INST; i++){
        if(strcmp(mnemonic, INSTRUCTION_TABLE[i].mnemonic) == 0){
            return INSTRUCTION_TABLE[i].length;
        }
    }
    return OK;
}

size_t mnemonic_to_table_index(const char* mnemonic){
    for(size_t i = 0; i < N_INST; i++){
        if(strcmp(mnemonic, INSTRUCTION_TABLE[i].mnemonic) == 0){
            return i;
        }
    }
    return OK;
}

int char_str_2_hex_str(const char* input, size_t input_len, void* output, size_t output_len, size_t n_chars, size_t skip, bool little_endian){
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

Instruction* Instruction_init(Context* c, ErrorCode* ec, const char* mnemonic_token, char* operands_token, Address offset){
    // Check if mnemonic exists in architecture table
    if(!is_valid_mnemonic(mnemonic_token)){
        c->error_code = INVALID_MNEMONIC;
        sprintf((char*)&c->msg_extras[0], "%ld", c->n_line);
        strcpy((char*)&c->msg_extras[1], mnemonic_token);
        *ec = c->error_code;
        return NULL;
    }
    size_t it_index = mnemonic_to_table_index(mnemonic_token);
    InstructionFormat format = INSTRUCTION_TABLE[it_index].format;
    size_t o_idx = 0;
    uint8_t bin_buffer[MAX_INSTRUCTION_LEN];
    ErrorCode ret_ec;
    // Remove spaces and replace parenthesis with commas
    for(size_t i = 0; i < MAX_OPERANDS_LEN; i++){
        if(operands_token[i] == ' ' || operands_token[i] == ')'){
            continue;
        }
        else if(operands_token[i] == '('){
            operands_token[o_idx] = ',';
            o_idx++;
        }
        else{
            operands_token[o_idx] = operands_token[i];
            o_idx++;
        }
        if(o_idx <= i){
            operands_token[i] = 0;
        }
    }
    // Clear binary buffer
    memset(&bin_buffer, 0, sizeof(bin_buffer));
    ret_ec = INSTRUCTION_TABLE[it_index].build_fn(c, it_index, operands_token, bin_buffer);
    if(ret_ec != 0){
        *ec = ret_ec;
        return NULL;
    }
    Instruction* instr = (Instruction*)malloc(sizeof(Instruction));
    if(instr == NULL){
        c->error_code = MALLOC_UNSUCCESSUL;
        strcpy((char*)&c->msg_extras[0], mnemonic_token);
        *ec = c->error_code;
        return NULL;
    }
    memcpy(&instr->binary, bin_buffer, MAX_INSTRUCTION_LEN);
    instr->it_index = it_index;
    instr->offset = offset;
    instr->next = NULL;
    return instr;
}

void Context_init(Context* c){
    c->instr_head = NULL;
    c->instr_tail = NULL;
    c->n_instr = 0;
    c->error_code = OK;
    for(size_t i = 0; i < MAX_MSG_EXTRAS; i++){
        memset(&c->msg_extras[i], 0, sizeof(MAX_MSG_EXTRA_LEN));
    }
}

void Context_free(Context* c){
    Instruction* curr;
    Instruction* next;
    curr = c->instr_head;
    while(curr != NULL){
        next = curr->next;
        free(curr);
        curr = next;
    }
}

int add_instruction(Context* c, Instruction* instr){
    if(instr == NULL){
        return -1;
    }
    if(c->n_instr == 0){
        c->instr_head = instr;
        c->instr_tail = instr;
        c->n_instr++;
        return OK;
    }
    c->instr_tail->next = instr;
    c->instr_tail = instr;
    c->n_instr++;
    return OK;
}

int display_stream(Context* c){
    Instruction* curr = c->instr_head;
    InstructionFormat format;
    int ret;
    while (curr != NULL){
        ret = INSTRUCTION_TABLE[curr->it_index].display_fn(c, curr);
        if(ret != 0){
            return -1;
        }
        curr = curr->next;
    }
    return OK;
}

void display_error(Context* c){
    switch (c->error_code){
    case OK:
        printf("OK: No error was encountered.\n");
        break;
    case SRC_FILE_NOT_FOUND:
        printf("ERROR: SRC_FILE_NOT_FOUND -> Input source file \"%s\" cannot be found.\n", c->msg_extras[0]);
        break;
    case CANNOT_OPEN_SRC_FILE:
        printf("ERROR: CANNOT_OPEN_SRC_FILE -> Cannot open source file \"%s\".\n", c->msg_extras[0]);
        break;
    case INVALID_MNEMONIC:
        printf("ERROR @ line %s: INVALID_MNEMONIC -> The mnemonic \"%s\" is not valid.\n", c->msg_extras[0], c->msg_extras[1]);
        break;
    case OPERAND_NON_HEX_FOUND:
        printf("ERROR @ line %s: OPERAND_NON_HEX_FOUND -> Operand fields \"%s\" contain non-hexidecimal characters.\n", c->msg_extras[0], c->msg_extras[1]);
        break;
    case INVALID_OPERAND_LENGTH:
        printf("ERROR @ line %s: INVALID_OPERAND_LENGTH -> The operand \"%s\" must have a maximum length of %s character(s).\n", c->msg_extras[0], c->msg_extras[1], c->msg_extras[2]);
        break;
    case MALLOC_UNSUCCESSUL:
        printf("ERROR: MALLOC_UNSUCCESSUL -> Could not allocate memory for instruction instance.\n");
        break;
    case NULL_POINTER_TO_OBJECT:
        printf("ERROR: NULL_POINTER_TO_OBJECT -> Attempted to reference an object with a null pointer.\n");
        break;
    case TOO_MANY_OPERANDS:
        printf("ERROR @ line %s: TOO_MANY_OPERANDS -> The instruction \"%s\" has been assigned too many operands.\n", c->msg_extras[0], c->msg_extras[1]);
        break;
    case MISSING_OPERANDS:
        printf("ERROR @ line %s: MISSING_OPERANDS -> The instruction \"%s\" is missing some required operands.\n", c->msg_extras[0], c->msg_extras[1]);
        break;
    default:
        break;
    }
}