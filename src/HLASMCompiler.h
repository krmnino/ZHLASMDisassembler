#ifndef COMPILER
#define COMPILER

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
#define MAX_6CHR_LEN 6
#define MAX_8CHR_LEN 8
#define MAX_PRINTOUT_FIELD_LEN 13
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
    I1, I2, I3, I4, I5,
    M1, M3, M4,
    R1, R2, R3,
    RI2, RI3, RI4,
    D2, D4,
    B2, B4,
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

static struct{
    size_t n_instr;
    Instruction* head;
    Instruction* tail;
} InstructionStream;

int process_source_file(const char*);
bool is_valid_mnemonic(const char*);
bool is_valid_hex_string(const char*, size_t);
InstructionFormat mnemonic_to_format(const char*);
uint16_t mnemonic_to_opcode(const char*);
uint8_t mnemonic_to_length(const char*);
size_t mnemonic_to_table_index(const char*);
int char_str_2_hex_str(const char*, size_t, void*, size_t, size_t, size_t, bool);
int hex_str_2_char_str(const void*, size_t, size_t, char*, size_t, size_t, size_t, bool);

Instruction* Instruction_init(const char*, char*, Address);
int build_E(size_t, const char*, uint8_t*);
int build_I(size_t, const char*, uint8_t*);
int build_IE(size_t, const char*, uint8_t*);
int build_MII(size_t, const char*, uint8_t*);
int build_RI(size_t, const char*, uint8_t*);
int build_RIE(size_t, const char*, uint8_t*);
int build_RIL(size_t, const char*, uint8_t*);
int build_RIS(size_t, const char*, uint8_t*);
int build_RR(size_t, const char*, uint8_t*);
int build_RRD(size_t, const char*, uint8_t*);
int build_RRE(size_t, const char*, uint8_t*);
int build_RRF(size_t, const char*, uint8_t*);
int build_RRS(size_t, const char*, uint8_t*);
int build_RS(size_t, const char*, uint8_t*);
int build_RSI(size_t, const char*, uint8_t*);
int build_RX(size_t, const char*, uint8_t*);

int display_E(Instruction*);
int display_I(Instruction*);
int display_IE(Instruction*);
int display_MII(Instruction*);
int display_RI(Instruction*);
int display_RIE(Instruction*);
int display_RIL(Instruction*);
int display_RIS(Instruction*);
int display_RR(Instruction*);
int display_RRD(Instruction*);
int display_RRE(Instruction*);
int display_RRF(Instruction*);
int display_RRS(Instruction*);
int display_RS(Instruction*);
int display_RSI(Instruction*);
int display_RX(Instruction*);

void InstructionStream_init();
void InstructionStream_free();
int InstructionStream_add_instruction(Instruction*);
int InstructionStream_display();

#endif