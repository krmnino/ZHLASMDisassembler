#ifndef DISASSEMBLER
#define DISASSEMBLER

#include <stdarg.h> 
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LINE_LEN 80
#define MAX_MNEMONIC_LEN 8
#define MAX_OPCODE_LEN 2
#define MAX_INSTRUCTION_LEN 6
#define MAX_OPERANDS_LEN 32
#define MAX_1CHR_LEN 1
#define MAX_2CHR_LEN 2
#define MAX_3CHR_LEN 3
#define MAX_4CHR_LEN 4
#define MAX_5CHR_LEN 5
#define MAX_6CHR_LEN 6
#define MAX_8CHR_LEN 8
#define MAX_12CHR_LEN 12
#define MAX_PRINTOUT_FIELD_LEN 13
#define MAX_MSG_EXTRAS 16
#define MAX_MSG_EXTRA_LEN 128
#define NO_SKIP 0
#define SKIP 1

#define USE_ALL   0x0000
#define B2_UNUSED 0x0001
#define M1_UNUSED 0x0002
#define M3_UNUSED 0x0004
#define M4_UNUSED 0x0008
#define M5_UNUSED 0x0010
#define M6_UNUSED 0x0020
#define R1_UNUSED 0x0040
#define R2_UNUSED 0x0080
#define R3_UNUSED 0x0100
#define D2_UNUSED 0x0200
#define I2_UNUSED 0x0400

typedef uint64_t Address;

typedef enum ErrorCode ErrorCode;
enum ErrorCode{
    OK,
    USE_ALTERNATIVE,
    SRC_FILE_NOT_FOUND,
    OUT_FILE_NOT_WRITABLE,
    CANNOT_OPEN_FILE,
    INVALID_MNEMONIC,
    INVALID_OPERAND_LENGTH,
    OPERAND_NON_HEX_FOUND,
    MALLOC_UNSUCCESSUL,
    NULL_POINTER_TO_OBJECT,
    TOO_MANY_OPERANDS,
    MISSING_OPERANDS,
    INVALID_OPCODE,
    END_OF_FILE,
};

typedef enum TokensParseState TokensParseState;
enum TokensParseState{
    SPACES_PM,
    LINE_COMMENT,
    INLINE_COMMENT,
    MNEMONIC,
    SPACES_PO,
    OPERANDS,
    TPS_DONE,
};

typedef enum OperandsParseState OperandsParseState;
enum OperandsParseState{
    B1, B2, B3, B4,
    D1, D2, D3, D4,
    I1, I2, I3, I4, I5,
    L1, L2,
    M1, M3, M4, M5, M6,
    R1, R2, R3,
    RI2, RI3, RI4,
    V1, V2, V3, V4,
    X2,
    OPS_DONE,
};

typedef enum InstructionFormat InstructionFormat;
enum InstructionFormat{
    NF,
    E,
    I,
    IE,
    MII,
    RIa, RIb, RIc,
    RIEa, RIEb, RIEc, RIEd, RIEe, RIEf, RIEg,
    RILa, RILb, RILc,
    RIS,
    RR,
    RRD,
    RRE,
    RRFa, RRFb, RRFc, RRFd, RRFe,
    RRS,
    RSa, RSb,
    RSI,
    RSLa, RSLb,
    RSYa, RSYb, 
    RXa, RXb, 
    RXE,
    RXF,
    RXYa,  RXYb,
    S,
    SI,
    SIL,
    SIY,
    SMI,
    SSa, SSb, SSc, SSd, SSe, SSf,
    SSE,
    SSF,
    VRIa, VRIb, VRIc, VRId, VRIe, VRIf, VRIg, VRIh, VRIi,
    VRRa, VRRb, VRRc, VRRd, VRRe, VRRf, VRRg, VRRh, VRRi, VRRj, VRRk, 
    VRSa, VRSb, VRSc, VRSd, 
    VRV,
    VRX,
    VSI, 
};

typedef struct Instruction Instruction;
struct Instruction{
    size_t it_index;
    uint8_t binary[MAX_INSTRUCTION_LEN];
    char operands_txt[MAX_OPERANDS_LEN];
    Address offset;
    Instruction* next;
};

static struct Context{
    ErrorCode error_code;
    char msg_extras[MAX_MSG_EXTRAS][MAX_MSG_EXTRA_LEN];
    size_t n_line;
    size_t bin_offset;
    size_t file_idx;
    size_t file_len;
    size_t n_instr;
    Instruction* instr_head;
    Instruction* instr_tail;
} Context;

ErrorCode assemble(const char*, const char*);
ErrorCode disassemble(const char*, const char*, bool);
bool is_valid_hex_string(const char*, size_t);
InstructionFormat mnemonic_to_format(const char*);
uint16_t mnemonic_to_opcode(const char*);
uint8_t mnemonic_to_length(const char*);
size_t mnemonic_to_table_index(const char*);
size_t opcode_to_table_index(uint16_t);
int char_str_2_hex_str(const char*, size_t, void*, size_t, size_t, size_t, bool);
int hex_str_2_char_str(const void*, size_t, size_t, char*, size_t, size_t, size_t, bool);
ErrorCode read_ascii_hex(FILE*, char*, size_t);

Instruction* Instruction_init_a(ErrorCode*, const char*, char*, Address);
Instruction* Instruction_init_d(ErrorCode*, uint16_t, const uint8_t*, Address);
ErrorCode assemble_E(size_t, const char*, uint8_t*);
ErrorCode assemble_I(size_t, const char*, uint8_t*);
ErrorCode assemble_IE(size_t, const char*, uint8_t*);
ErrorCode assemble_MII(size_t, const char*, uint8_t*);
ErrorCode assemble_RI(size_t, const char*, uint8_t*);
ErrorCode assemble_RIE(size_t, const char*, uint8_t*);
ErrorCode assemble_RIL(size_t, const char*, uint8_t*);
ErrorCode assemble_RIS(size_t, const char*, uint8_t*);
ErrorCode assemble_RR(size_t, const char*, uint8_t*);
ErrorCode assemble_RRD(size_t, const char*, uint8_t*);
ErrorCode assemble_RRE(size_t, const char*, uint8_t*);
ErrorCode assemble_RRF(size_t, const char*, uint8_t*);
ErrorCode assemble_RRS(size_t, const char*, uint8_t*);
ErrorCode assemble_RS(size_t, const char*, uint8_t*);
ErrorCode assemble_RSI(size_t, const char*, uint8_t*);
ErrorCode assemble_RSL(size_t, const char*, uint8_t*);
ErrorCode assemble_RSY(size_t, const char*, uint8_t*);
ErrorCode assemble_RX(size_t, const char*, uint8_t*);
ErrorCode assemble_RXE(size_t, const char*, uint8_t*);
ErrorCode assemble_RXF(size_t, const char*, uint8_t*);
ErrorCode assemble_RXY(size_t, const char*, uint8_t*);
ErrorCode assemble_S(size_t, const char*, uint8_t*);
ErrorCode assemble_SI(size_t, const char*, uint8_t*);
ErrorCode assemble_SIL(size_t, const char*, uint8_t*);
ErrorCode assemble_SIY(size_t, const char*, uint8_t*);
ErrorCode assemble_SMI(size_t, const char*, uint8_t*);
ErrorCode assemble_SS(size_t, const char*, uint8_t*);
ErrorCode assemble_SSE(size_t, const char*, uint8_t*);
ErrorCode assemble_SSF(size_t, const char*, uint8_t*);
ErrorCode assemble_VRI(size_t, const char*, uint8_t*);
ErrorCode assemble_VRR(size_t, const char*, uint8_t*);
ErrorCode assemble_VRS(size_t, const char*, uint8_t*);
ErrorCode assemble_VRV(size_t, const char*, uint8_t*);
ErrorCode assemble_VRX(size_t, const char*, uint8_t*);
ErrorCode assemble_VSI(size_t, const char*, uint8_t*);

ErrorCode display_E(Instruction*);
ErrorCode display_I(Instruction*);
ErrorCode display_IE(Instruction*);
ErrorCode display_MII(Instruction*);
ErrorCode display_RI(Instruction*);
ErrorCode display_RIE(Instruction*);
ErrorCode display_RIL(Instruction*);
ErrorCode display_RIS(Instruction*);
ErrorCode display_RR(Instruction*);
ErrorCode display_RRD(Instruction*);
ErrorCode display_RRE(Instruction*);
ErrorCode display_RRF(Instruction*);
ErrorCode display_RRS(Instruction*);
ErrorCode display_RS(Instruction*);
ErrorCode display_RSI(Instruction*);
ErrorCode display_RSL(Instruction*);
ErrorCode display_RSY(Instruction*);
ErrorCode display_RX(Instruction*);
ErrorCode display_RXE(Instruction*);
ErrorCode display_RXF(Instruction*);
ErrorCode display_RXY(Instruction*);
ErrorCode display_S(Instruction*);
ErrorCode display_SI(Instruction*);
ErrorCode display_SIL(Instruction*);
ErrorCode display_SIY(Instruction*);
ErrorCode display_SMI(Instruction*);
ErrorCode display_SS(Instruction*);
ErrorCode display_SSE(Instruction*);
ErrorCode display_SSF(Instruction*);
ErrorCode display_VRI(Instruction*);
ErrorCode display_VRR(Instruction*);
ErrorCode display_VRS(Instruction*);
ErrorCode display_VRV(Instruction*);
ErrorCode display_VRX(Instruction*);
ErrorCode display_VSI(Instruction*);

ErrorCode disassemble_E(size_t, const uint8_t*, char*);
ErrorCode disassemble_I(size_t, const uint8_t*, char*);
ErrorCode disassemble_IE(size_t, const uint8_t*, char*);
ErrorCode disassemble_MII(size_t, const uint8_t*, char*);
ErrorCode disassemble_RI(size_t, const uint8_t*, char*);
ErrorCode disassemble_RIE(size_t, const uint8_t*, char*);
ErrorCode disassemble_RIL(size_t, const uint8_t*, char*);
ErrorCode disassemble_RIS(size_t, const uint8_t*, char*);
ErrorCode disassemble_RR(size_t, const uint8_t*, char*);
ErrorCode disassemble_RRD(size_t, const uint8_t*, char*);
ErrorCode disassemble_RRE(size_t, const uint8_t*, char*);
ErrorCode disassemble_RRF(size_t, const uint8_t*, char*);
ErrorCode disassemble_RRS(size_t, const uint8_t*, char*);
ErrorCode disassemble_RS(size_t, const uint8_t*, char*);
ErrorCode disassemble_RSI(size_t, const uint8_t*, char*);
ErrorCode disassemble_RSL(size_t, const uint8_t*, char*);
ErrorCode disassemble_RSY(size_t, const uint8_t*, char*);
ErrorCode disassemble_RX(size_t, const uint8_t*, char*);
ErrorCode disassemble_RXE(size_t, const uint8_t*, char*);
ErrorCode disassemble_RXF(size_t, const uint8_t*, char*);
ErrorCode disassemble_RXY(size_t, const uint8_t*, char*);
ErrorCode disassemble_S(size_t, const uint8_t*, char*);
ErrorCode disassemble_SI(size_t, const uint8_t*, char*);
ErrorCode disassemble_SIL(size_t, const uint8_t*, char*);
ErrorCode disassemble_SIY(size_t, const uint8_t*, char*);
ErrorCode disassemble_SMI(size_t, const uint8_t*, char*);
ErrorCode disassemble_SS(size_t, const uint8_t*, char*);
ErrorCode disassemble_SSE(size_t, const uint8_t*, char*);
ErrorCode disassemble_SSF(size_t, const uint8_t*, char*);
ErrorCode disassemble_VRI(size_t, const uint8_t*, char*);
ErrorCode disassemble_VRR(size_t, const uint8_t*, char*);
ErrorCode disassemble_VRS(size_t, const uint8_t*, char*);
ErrorCode disassemble_VRV(size_t, const uint8_t*, char*);
ErrorCode disassemble_VRX(size_t, const uint8_t*, char*);
ErrorCode disassemble_VSI(size_t, const uint8_t*, char*);

void Context_init();
void Context_free();
ErrorCode add_instruction(Instruction*);
ErrorCode display_stream();
void display_error();

#endif