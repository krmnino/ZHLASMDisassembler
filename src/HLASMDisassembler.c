#include "HLASMDisassembler.h"
#include "InstructionTable.h"

ErrorCode assemble(const char* src_filename, const char* bin_filename){
    char line[MAX_LINE_LEN];
    char mnemonic_token[MAX_MNEMONIC_LEN];
    char operands_token[MAX_OPERANDS_LEN];
    FILE* source_file;
    FILE* binary_file;
    Address instr_offset;
    size_t m_idx;
    size_t o_idx;
    TokensParseState tp_state;
    bool run;
    bool skip_line;
    ErrorCode ret_ec;
    Instruction* instr;
    Instruction* curr;
    source_file = fopen(src_filename, "r");
    binary_file = fopen(bin_filename, "wb");
    if(source_file == NULL){
        return CANNOT_OPEN_FILE;
    }
    if(binary_file == NULL){
        return CANNOT_OPEN_FILE;
    }
    instr_offset = 0;
    tp_state = SPACES_PM;
    run = true;
    m_idx = 0;
    o_idx = 0;
    Context.n_line = 1;
    while(fgets(line, sizeof(line), source_file)){
        skip_line = false;
        memset(&mnemonic_token, 0, sizeof(mnemonic_token));
        memset(&operands_token, 0, sizeof(operands_token));
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
            Context.n_line++;
            continue;
        }
        instr = Instruction_init_a(&ret_ec, mnemonic_token, operands_token, instr_offset);
        if(ret_ec != OK){
            return ret_ec;
        }
        ret_ec = add_instruction(instr); 
        if(ret_ec != OK){
            return ret_ec;
        }
        instr_offset += mnemonic_to_length(mnemonic_token);
        tp_state = SPACES_PM;
        run = true;
        m_idx = 0;
        o_idx = 0;
        memset(&line, 0, sizeof(line));
        memset(&mnemonic_token, 0, sizeof(mnemonic_token));
        memset(&operands_token, 0, sizeof(operands_token));
        Context.n_line++;
    }
    fclose(source_file);
    curr = Context.instr_head;
    while(true){
        if(curr == NULL){
            break;
        }
        fwrite((void*)curr->binary, 1, INSTRUCTION_TABLE[curr->it_index].length, binary_file);
        curr = curr->next;
    }
    fclose(binary_file);
    return OK;
}

ErrorCode disassemble(const char* bin_filename, const char* src_filename){
    uint8_t bin_buffer[MAX_INSTRUCTION_LEN];
    FILE* binary_file;
    FILE* source_file;
    void* bin_buffer_ptr;
    Address instr_offset;
    size_t file_len;
    ErrorCode ret_ec;
    Instruction* instr;
    Instruction* curr;
    uint16_t opcode;
    uint8_t instr_len;
    uint8_t nibble;
    binary_file = fopen(bin_filename, "rb");
    source_file = fopen(src_filename, "w");
    if(binary_file == NULL){
        return CANNOT_OPEN_FILE;
    }
    if(source_file == NULL){
        return CANNOT_OPEN_FILE;
    }
    // Length of binary file
    fseek(binary_file, 0, SEEK_END);
    file_len = ftell(binary_file);
    rewind(binary_file);
    Context.bin_offset = 0;
    instr_offset = 0;
    // Read instruction by instruction
    while(instr_offset < file_len){
        opcode = 0;
        bin_buffer_ptr = &bin_buffer;
        fread(bin_buffer_ptr, sizeof(uint8_t), MAX_1CHR_LEN, binary_file);
        bin_buffer_ptr++;
        nibble = bin_buffer[0] >> 4;
        switch (nibble){
        case 0x0C:
        case 0x0D:
        case 0x0E:
        case 0x0F:
            fread(bin_buffer_ptr, sizeof(uint8_t), MAX_5CHR_LEN, binary_file);
            switch (bin_buffer[0]){
            case 0xE3:
            case 0xE6:
            case 0xE7:
            case 0xEB:
            case 0xEC:
            case 0xED:
                opcode = (bin_buffer[0] << 8) | bin_buffer[5];
                break;
            case 0xE5:
                opcode = (bin_buffer[0] << 8) | bin_buffer[1];
                break;
            case 0xC0:
            case 0xC2:
            case 0xC4:
            case 0xC6:
            case 0xC8:
            case 0xCC:
                opcode = (bin_buffer[0] << 4) | (bin_buffer[1] & 0x0F);
                break;
            default:
                opcode = bin_buffer[0];
                break;
            }
            instr_len = MAX_6CHR_LEN;
            break;        
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x08:
        case 0x09:
        case 0x0A:
        case 0x0B:
            fread(bin_buffer_ptr, sizeof(uint8_t), MAX_3CHR_LEN, binary_file);
            switch (bin_buffer[0]){
            case 0xB2:
            case 0xB3:
            case 0xB9:
                opcode = (bin_buffer[0] << 8) | bin_buffer[1];
                break;
            case 0xA5:
            case 0xA7:
                opcode = (bin_buffer[0] << 4) | (bin_buffer[1] & 0x0F);
                break;
            default:
                opcode = bin_buffer[0];
                break;
            }
            instr_len = MAX_4CHR_LEN;
            break;        
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
            fread(bin_buffer_ptr, sizeof(uint8_t), MAX_1CHR_LEN, binary_file);
            switch (bin_buffer[0]){
            case 0x00:
            case 0x01:
                opcode = (bin_buffer[0] << 8) | bin_buffer[1];
                break;
            default:
                opcode = bin_buffer[0];
                break;
            }
            instr_len = MAX_2CHR_LEN;
            break;        
        default:
            break;
        }
        instr = Instruction_init_d(&ret_ec, opcode, bin_buffer, instr_offset);
        if(ret_ec != OK){
            return ret_ec;
        }
        ret_ec = add_instruction(instr); 
        if(ret_ec != OK){
            return ret_ec;
        }
        Context.bin_offset += instr_len;
        instr_offset += instr_len;
    }
    fclose(binary_file);
    curr = Context.instr_head;
    while(true){
        if(curr == NULL){
            break;
        }
        fprintf(source_file, "%s", INSTRUCTION_TABLE[curr->it_index].mnemonic);
        for(size_t i = 0; i < (MAX_MNEMONIC_LEN - strlen(INSTRUCTION_TABLE[curr->it_index].mnemonic)); i++){
            fprintf(source_file, "%s", " ");
        }
        fprintf(source_file, "%s", curr->operands_txt);
        fprintf(source_file, "%s", "\n");
        curr = curr->next;
    }
    fclose(source_file);
    return OK;
}

bool is_valid_hex_string(const char* input, size_t length){
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

size_t opcode_to_table_index(uint16_t opcode){
    for(size_t i = 0; i < N_INST; i++){
        if(opcode == INSTRUCTION_TABLE[i].opcode){
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

Instruction* Instruction_init_a(ErrorCode* ec, const char* mnemonic_token, char* operands_token, Address offset){
    size_t it_index = mnemonic_to_table_index(mnemonic_token);
    size_t o_idx = 0;
    uint8_t bin_buffer[MAX_INSTRUCTION_LEN];
    char clean_operands_token[MAX_OPERANDS_LEN];
    ErrorCode ret_ec;
    // Check if mnemonic exists in instruction table
    if(it_index == 0){
        Context.error_code = INVALID_MNEMONIC;
        sprintf((char*)&Context.msg_extras[0], "%ld", Context.n_line);
        strcpy((char*)&Context.msg_extras[1], mnemonic_token);
        *ec = Context.error_code;
        return NULL;
    }
    // Clear operand buffer
    memset(&bin_buffer, 0, sizeof(bin_buffer));
    // Remove spaces and replace parenthesis with commas
    for(size_t i = 0; i < MAX_OPERANDS_LEN; i++){
        if(operands_token[i] == ' ' || operands_token[i] == ')'){
            continue;
        }
        else if(operands_token[i] == '('){
            clean_operands_token[o_idx] = ',';
            o_idx++;
        }
        else{
            clean_operands_token[o_idx] = operands_token[i];
            o_idx++;
        }
        if(o_idx <= i){
            clean_operands_token[i] = 0;
        }
    }
    // Clear binary buffer
    memset(&bin_buffer, 0, sizeof(bin_buffer));
    ret_ec = INSTRUCTION_TABLE[it_index].assemble_fn(it_index, clean_operands_token, bin_buffer);
    if(ret_ec != OK){
        *ec = ret_ec;
        return NULL;
    }
    Instruction* instr = (Instruction*)malloc(sizeof(Instruction));
    if(instr == NULL){
        Context.error_code = MALLOC_UNSUCCESSUL;
        strcpy((char*)&Context.msg_extras[0], mnemonic_token);
        *ec = Context.error_code;
        return NULL;
    }
    memcpy(&instr->binary, bin_buffer, MAX_INSTRUCTION_LEN);
    memcpy(&instr->operands_txt, operands_token, MAX_OPERANDS_LEN);
    instr->it_index = it_index;
    instr->offset = offset;
    instr->next = NULL;
    *ec = OK;
    return instr;
}

Instruction* Instruction_init_d(ErrorCode* ec, uint16_t opcode, const uint8_t* bin_buffer, Address offset){
    size_t it_index = opcode_to_table_index(opcode);
    char operands_buffer[MAX_OPERANDS_LEN];
    ErrorCode ret_ec;
    // Check if mnemonic exists in instruction table
    if(it_index == 0){
        Context.error_code = INVALID_MNEMONIC;
        sprintf((char*)&Context.msg_extras[0], "%ld", Context.bin_offset);
        sprintf((char*)&Context.msg_extras[1], "%x", opcode);
        *ec = Context.error_code;
        return NULL;
    }
    // Clear operands buffer
    memset(&operands_buffer, 0, sizeof(operands_buffer));
    ret_ec = INSTRUCTION_TABLE[it_index].disassemble_fn(it_index, bin_buffer, operands_buffer);
    // If initial disassembler function call requires using alternative instruction, then try again with alternative
    if(ret_ec == USE_ALTERNATIVE){
        it_index = mnemonic_to_table_index(INSTRUCTION_TABLE[it_index].alternative_instr);
        ret_ec = INSTRUCTION_TABLE[it_index].disassemble_fn(it_index, bin_buffer, operands_buffer);
        if(ret_ec != OK){
            *ec = ret_ec;
            return NULL;
        }
    }
    else if(ret_ec != OK){
        *ec = ret_ec;
        return NULL;
    }
    Instruction* instr = (Instruction*)malloc(sizeof(Instruction));
    if(instr == NULL){
        Context.error_code = MALLOC_UNSUCCESSUL;
        strcpy((char*)&Context.msg_extras[0], INSTRUCTION_TABLE[it_index].mnemonic);
        *ec = Context.error_code;
        return NULL;
    }
    memcpy(&instr->binary, bin_buffer, MAX_INSTRUCTION_LEN);
    memcpy(&instr->operands_txt, operands_buffer, MAX_OPERANDS_LEN);
    instr->it_index = it_index;
    instr->offset = offset;
    instr->next = NULL;
    *ec = OK;
    return instr;
}

void Context_init(){
    Context.instr_head = NULL;
    Context.instr_tail = NULL;
    Context.n_instr = 0;
    Context.error_code = OK;
    for(size_t i = 0; i < MAX_MSG_EXTRAS; i++){
        memset(&Context.msg_extras[i], 0, sizeof(MAX_MSG_EXTRA_LEN));
    }
}

void Context_free(){
    Instruction* curr;
    Instruction* next;
    curr = Context.instr_head;
    while(curr != NULL){
        next = curr->next;
        free(curr);
        curr = next;
    }
}

ErrorCode add_instruction(Instruction* instr){
    if(instr == NULL){
        Context.error_code = NULL_POINTER_TO_OBJECT;
        return Context.error_code;
    }
    if(Context.n_instr == 0){
        Context.instr_head = instr;
        Context.instr_tail = instr;
        Context.n_instr++;
        return OK;
    }
    Context.instr_tail->next = instr;
    Context.instr_tail = instr;
    Context.n_instr++;
    return OK;
}

ErrorCode display_stream(){
    Instruction* curr = Context.instr_head;
    int ret;
    while (curr != NULL){
        ret = INSTRUCTION_TABLE[curr->it_index].display_fn(curr);
        if(ret != 0){
            return ret;
        }
        curr = curr->next;
    }
    return OK;
}

void display_error(){
    switch (Context.error_code){
    case OK:
        printf("OK: No error was encountered.\n");
        break;
    case SRC_FILE_NOT_FOUND:
        printf("ERROR: SRC_FILE_NOT_FOUND -> Input source file \"%s\" cannot be found.\n", Context.msg_extras[0]);
        break;
    case OUT_FILE_NOT_WRITABLE:
        printf("ERROR: OUT_FILE_NOT_WRITABLE -> Output file \"%s\" cannot be written into.\n", Context.msg_extras[0]);
        break;
    case CANNOT_OPEN_FILE:
        printf("ERROR: CANNOT_OPEN_FILE -> Cannot open file \"%s\".\n", Context.msg_extras[0]);
        break;
    case INVALID_MNEMONIC:
        printf("ERROR @ line %s: INVALID_MNEMONIC -> The mnemonic \"%s\" is not valid.\n", Context.msg_extras[0], Context.msg_extras[1]);
        break;
    case OPERAND_NON_HEX_FOUND:
        printf("ERROR @ line %s: OPERAND_NON_HEX_FOUND -> Operand fields \"%s\" contain non-hexidecimal characters.\n", Context.msg_extras[0], Context.msg_extras[1]);
        break;
    case INVALID_OPERAND_LENGTH:
        printf("ERROR @ line %s: INVALID_OPERAND_LENGTH -> The operand \"%s\" must have a maximum length of %s character(s).\n", Context.msg_extras[0], Context.msg_extras[1], Context.msg_extras[2]);
        break;
    case MALLOC_UNSUCCESSUL:
        printf("ERROR: MALLOC_UNSUCCESSUL -> Could not allocate memory for instruction instance.\n");
        break;
    case NULL_POINTER_TO_OBJECT:
        printf("ERROR: NULL_POINTER_TO_OBJECT -> Attempted to reference an object with a null pointer.\n");
        break;
    case TOO_MANY_OPERANDS:
        printf("ERROR @ line %s: TOO_MANY_OPERANDS -> The instruction \"%s\" has been assigned too many operands.\n", Context.msg_extras[0], Context.msg_extras[1]);
        break;
    case MISSING_OPERANDS:
        printf("ERROR @ line %s: MISSING_OPERANDS -> The instruction \"%s\" is missing some required operands.\n", Context.msg_extras[0], Context.msg_extras[1]);
        break;
    case INVALID_OPCODE:
        printf("ERROR @ offset %s: INVALID_OPCODE -> The opcode \"%s\" is not valid.\n", Context.msg_extras[0], Context.msg_extras[1]);
        break;
    default:
        break;
    }
}