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
#define M3_UNUSED 0x0002
#define M4_UNUSED 0x0004
#define M5_UNUSED 0x0008
#define M6_UNUSED 0x0010
#define R1_UNUSED 0x0020
#define R2_UNUSED 0x0040
#define R3_UNUSED 0x0080
#define D2_UNUSED 0x0100
#define I2_UNUSED 0x0200

typedef uint64_t Address;

typedef enum ErrorCode ErrorCode;
enum ErrorCode{
    OK,
    SRC_FILE_NOT_FOUND,
    CANNOT_OPEN_SRC_FILE,
    INVALID_MNEMONIC,
    INVALID_OPERAND_LENGTH,
    OPERAND_NON_HEX_FOUND,
    MALLOC_UNSUCCESSUL,
    NULL_POINTER_TO_OBJECT,
    TOO_MANY_OPERANDS,
    MISSING_OPERANDS,
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
    M1, M3, M4,
    R1, R2, R3,
    RI2, RI3, RI4,
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
    Address offset;
    Instruction* next;
};

typedef struct Context Context;
struct Context{
    ErrorCode error_code;
    char msg_extras[MAX_MSG_EXTRAS][MAX_MSG_EXTRA_LEN];
    size_t n_line;
    size_t n_instr;
    Instruction* instr_head;
    Instruction* instr_tail;
};
static Context context;

ErrorCode process_source_file(Context* c, const char*);
bool is_valid_mnemonic(const char*);
bool is_valid_hex_string(const char*, size_t);
InstructionFormat mnemonic_to_format(const char*);
uint16_t mnemonic_to_opcode(const char*);
uint8_t mnemonic_to_length(const char*);
size_t mnemonic_to_table_index(const char*);
int char_str_2_hex_str(const char*, size_t, void*, size_t, size_t, size_t, bool);
int hex_str_2_char_str(const void*, size_t, size_t, char*, size_t, size_t, size_t, bool);

Instruction* Instruction_init(Context* c, ErrorCode*, const char*, char*, Address);
ErrorCode build_E(Context*, size_t, const char*, uint8_t*);
ErrorCode build_I(Context*, size_t, const char*, uint8_t*);
ErrorCode build_IE(Context*, size_t, const char*, uint8_t*);
ErrorCode build_MII(Context*, size_t, const char*, uint8_t*);
ErrorCode build_RI(Context*, size_t, const char*, uint8_t*);
ErrorCode build_RIE(Context*, size_t, const char*, uint8_t*);
ErrorCode build_RIL(Context*, size_t, const char*, uint8_t*);
ErrorCode build_RIS(Context*, size_t, const char*, uint8_t*);
ErrorCode build_RR(Context*, size_t, const char*, uint8_t*);
ErrorCode build_RRD(Context*, size_t, const char*, uint8_t*);
ErrorCode build_RRE(Context*, size_t, const char*, uint8_t*);
ErrorCode build_RRF(Context*, size_t, const char*, uint8_t*);
ErrorCode build_RRS(Context*, size_t, const char*, uint8_t*);
ErrorCode build_RS(Context*, size_t, const char*, uint8_t*);
ErrorCode build_RSI(Context*, size_t, const char*, uint8_t*);
ErrorCode build_RSL(Context*, size_t, const char*, uint8_t*);
ErrorCode build_RSY(Context*, size_t, const char*, uint8_t*);
ErrorCode build_RX(Context*, size_t, const char*, uint8_t*);
ErrorCode build_RXE(Context*, size_t, const char*, uint8_t*);
ErrorCode build_RXF(Context*, size_t, const char*, uint8_t*);
ErrorCode build_RXY(Context*, size_t, const char*, uint8_t*);
ErrorCode build_S(Context*, size_t, const char*, uint8_t*);
ErrorCode build_SI(Context*, size_t, const char*, uint8_t*);
ErrorCode build_SIL(Context*, size_t, const char*, uint8_t*);
ErrorCode build_SIY(Context*, size_t, const char*, uint8_t*);
ErrorCode build_SMI(Context*, size_t, const char*, uint8_t*);
ErrorCode build_SS(Context*, size_t, const char*, uint8_t*);
ErrorCode build_SSE(Context*, size_t, const char*, uint8_t*);
ErrorCode build_SSF(Context*, size_t, const char*, uint8_t*);
ErrorCode build_VRI(Context*, size_t, const char*, uint8_t*);
ErrorCode build_VRR(Context*, size_t, const char*, uint8_t*);
ErrorCode build_VRS(Context*, size_t, const char*, uint8_t*);
ErrorCode build_VRV(Context*, size_t, const char*, uint8_t*);
ErrorCode build_VRX(Context*, size_t, const char*, uint8_t*);
ErrorCode build_VSI(Context*, size_t, const char*, uint8_t*);

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

ErrorCode decode_E();
ErrorCode decode_I();
ErrorCode decode_IE();
ErrorCode decode_MII();
ErrorCode decode_RI();
ErrorCode decode_RIE();
ErrorCode decode_RIL();
ErrorCode decode_RIS();
ErrorCode decode_RR();
ErrorCode decode_RRD();
ErrorCode decode_RRE();
ErrorCode decode_RRF();
ErrorCode decode_RRS();
ErrorCode decode_RS();
ErrorCode decode_RSI();
ErrorCode decode_RSL();
ErrorCode decode_RSY();
ErrorCode decode_RX();
ErrorCode decode_RXE();
ErrorCode decode_RXF();
ErrorCode decode_RXY();
ErrorCode decode_S();
ErrorCode decode_SI();
ErrorCode decode_SIL();
ErrorCode decode_SIY();
ErrorCode decode_SMI();
ErrorCode decode_SS();
ErrorCode decode_SSE();
ErrorCode decode_SSF();
ErrorCode decode_VRI();
ErrorCode decode_VRR();
ErrorCode decode_VRS();
ErrorCode decode_VRV();
ErrorCode decode_VRX();
ErrorCode decode_VSI();

void Context_init(Context* c);
void Context_free(Context* c);
int add_instruction(Context* c, Instruction*);
int display_stream(Context* c);
void display_error(Context* c);

//void ErrorHandler_init();

#endif