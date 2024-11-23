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
#define MAX_REG_LEN 1
#define MAX_DISP_LEN 3
#define MAX_LONG_DISP_LEN 5
#define MAX_PRINTOUT_FIELD_LEN 9
#define NO_SKIP 0
#define SKIP 1

typedef uint64_t Address;

typedef enum TokensParseState TokensParseState;
enum TokensParseState{
    SPACES_PM,
    MNEMONIC,
    SPACES_PO,
    OPERANDS,
    TPS_DONE,
};

typedef enum OperandsParseState OperandsParseState;
enum OperandsParseState{
    I1,
    I2,
    M1,
    R1,
    D2,
    B2,
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
    char mnemonic[MAX_MNEMONIC_LEN];
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
uint32_t char_2_hex(const char*);
int hex_2_char(void*, size_t, size_t, char*, size_t, size_t, size_t, bool);

Instruction* Instruction_init(const char*, char*, Address);
int build_E(uint16_t, char*, uint8_t*, InstructionFormat);
int build_I(uint16_t, char*, uint8_t*, InstructionFormat);
int build_IE(uint16_t, char*, uint8_t*, InstructionFormat);
int build_RX(uint16_t, char*, uint8_t*, InstructionFormat);

int display_E(Instruction*);
int display_I(Instruction*);
int display_IE(Instruction*);
int display_RX(Instruction*);

void InstructionStream_init();
void InstructionStream_free();
int InstructionStream_add_instruction(Instruction*);
int InstructionStream_display();

#endif