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
    R1,
    R1_M1,
    X2,
    B2,
    D2,
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
    RSa,RSb,
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
bool is_hex_char(const char*, size_t);
InstructionFormat mnemonic_to_format(const char*);
uint16_t mnemonic_to_opcode(const char*);
uint8_t mnemonic_to_length(const char*);

Instruction* Instruction_init(const char*, char*, Address);
int build_RXa(uint16_t, char*, uint8_t*);

void InstructionStream_init();
void InstructionStream_free();
int InstructionStream_add_instruction(Instruction*);

#endif