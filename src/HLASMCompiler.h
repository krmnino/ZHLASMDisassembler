#ifndef COMPILER
#define COMPILER

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
    RSa_R3_D2,
    OPS_DONE,
};

typedef enum InstructionFormat InstructionFormat;
enum InstructionFormat{
    NF,
    E,
    I,
    IE,
    MII,
    RI, RIa, RIb, RIc,
    RIE, RIEa, RIEb, RIEc, RIEd, RIEe, RIEf, RIEg,
    RIL, RILa, RILb, RILc,
    RIS,
    RR,
    RRD,
    RRE,
    RRF, RRFa, RRFb, RRFc, RRFd, RRFe,
    RRS,
    RS, RSa, RSb,
    RSI,
    RSL, RSLa, RSLb,
    RSY, RSYa, RSYb, 
    RX, RXa, RXb, 
    RXE,
    RXF,
    RXY, RXYa,  RXYb,
    S,
    SI,
    SIL,
    SIY,
    SMI,
    SS, SSa, SSb, SSc, SSd, SSe, SSf,
    SSE,
    SSF,
    VRI, VRIa, VRIb, VRIc, VRId, VRIe, VRIf, VRIg, VRIh, VRIi,
    VRR, VRRa, VRRb, VRRc, VRRd, VRRe, VRRf, VRRg, VRRh, VRRi, VRRj, VRRk, 
    VRS, VRSa, VRSb, VRSc, VRSd, 
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

typedef struct Context Context;
struct Context{
    ErrorCode error_code;
    char msg_extras[MAX_MSG_EXTRAS][MAX_MSG_EXTRA_LEN];
    size_t n_line;
    size_t bin_offset;
    size_t n_instr;
    Instruction* instr_head;
    Instruction* instr_tail;
};
static Context context;

ErrorCode assemble(Context* c, const char*, const char*);
ErrorCode disassemble(Context* c, const char*, const char*);
bool is_valid_hex_string(const char*, size_t);
InstructionFormat mnemonic_to_format(const char*);
uint16_t mnemonic_to_opcode(const char*);
uint8_t mnemonic_to_length(const char*);
size_t mnemonic_to_table_index(const char*);
size_t opcode_to_table_index(uint16_t);
int char_str_2_hex_str(const char*, size_t, void*, size_t, size_t, size_t, bool);
int hex_str_2_char_str(const void*, size_t, size_t, char*, size_t, size_t, size_t, bool);

Instruction* Instruction_init_a(Context* c, ErrorCode*, const char*, char*, Address);
Instruction* Instruction_init_d(Context* c, ErrorCode*, uint16_t, const uint8_t*, Address);
ErrorCode assemble_E(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_I(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_IE(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_MII(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_RI(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_RIE(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_RIL(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_RIS(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_RR(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_RRD(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_RRE(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_RRF(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_RRS(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_RS(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_RSI(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_RSL(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_RSY(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_RX(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_RXE(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_RXF(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_RXY(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_S(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_SI(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_SIL(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_SIY(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_SMI(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_SS(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_SSE(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_SSF(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_VRI(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_VRR(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_VRS(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_VRV(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_VRX(Context*, size_t, const char*, uint8_t*);
ErrorCode assemble_VSI(Context*, size_t, const char*, uint8_t*);

ErrorCode display_E(Context*, Instruction*);
ErrorCode display_I(Context*, Instruction*);
ErrorCode display_IE(Context*, Instruction*);
ErrorCode display_MII(Context*, Instruction*);
ErrorCode display_RI(Context*, Instruction*);
ErrorCode display_RIE(Context*, Instruction*);
ErrorCode display_RIL(Context*, Instruction*);
ErrorCode display_RIS(Context*, Instruction*);
ErrorCode display_RR(Context*, Instruction*);
ErrorCode display_RRD(Context*, Instruction*);
ErrorCode display_RRE(Context*, Instruction*);
ErrorCode display_RRF(Context*, Instruction*);
ErrorCode display_RRS(Context*, Instruction*);
ErrorCode display_RS(Context*, Instruction*);
ErrorCode display_RSI(Context*, Instruction*);
ErrorCode display_RSL(Context*, Instruction*);
ErrorCode display_RSY(Context*, Instruction*);
ErrorCode display_RX(Context*, Instruction*);
ErrorCode display_RXE(Context*, Instruction*);
ErrorCode display_RXF(Context*, Instruction*);
ErrorCode display_RXY(Context*, Instruction*);
ErrorCode display_S(Context*, Instruction*);
ErrorCode display_SI(Context*, Instruction*);
ErrorCode display_SIL(Context*, Instruction*);
ErrorCode display_SIY(Context*, Instruction*);
ErrorCode display_SMI(Context*, Instruction*);
ErrorCode display_SS(Context*, Instruction*);
ErrorCode display_SSE(Context*, Instruction*);
ErrorCode display_SSF(Context*, Instruction*);
ErrorCode display_VRI(Context*, Instruction*);
ErrorCode display_VRR(Context*, Instruction*);
ErrorCode display_VRS(Context*, Instruction*);
ErrorCode display_VRV(Context*, Instruction*);
ErrorCode display_VRX(Context*, Instruction*);
ErrorCode display_VSI(Context*, Instruction*);

ErrorCode disassemble_E(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_I(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_IE(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_MII(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_RI(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_RIE(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_RIL(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_RIS(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_RR(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_RRD(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_RRE(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_RRF(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_RRS(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_RS(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_RSI(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_RSL(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_RSY(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_RX(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_RXE(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_RXF(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_RXY(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_S(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_SI(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_SIL(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_SIY(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_SMI(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_SS(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_SSE(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_SSF(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_VRI(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_VRR(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_VRS(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_VRV(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_VRX(Context*, size_t, const uint8_t*, char*);
ErrorCode disassemble_VSI(Context*, size_t, const uint8_t*, char*);

void Context_init(Context* c);
void Context_free(Context* c);
ErrorCode add_instruction(Context* c, Instruction*);
ErrorCode display_stream(Context* c);
void display_error(Context* c);

#endif