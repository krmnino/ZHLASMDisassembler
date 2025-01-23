from typing import Any
from typing import List

ASSEMBLE_FN_IDX = 0
DISPLAY_FN_IDX = 1
DISASSEMBLE_FN_IDX = 2
FORMAT_IDX = 3
OPCODE_IDX = 4
UNUSED_IDX = 5
LENGTH_IDX = 6
MNEMONIC_IDX = 7
ALTERNATIVE_INSTR_IDX = 8

def clean_table_entry(raw_entry):
    clean_entry = raw_entry.split(',')
    # Clean assemble_fn entry
    clean_entry[0] = clean_entry[0].strip()
    # Clean display_fn entry
    clean_entry[1] = clean_entry[1].strip()
    # Clean disassemble_fn entry
    clean_entry[2] = clean_entry[2].strip()
    # Clean format entry
    clean_entry[3] = clean_entry[3].strip()
    # Clean opcode entry
    clean_entry[4] = clean_entry[4].strip()
    clean_entry[4] = clean_entry[4].replace('(uint16_t)', '')
    clean_entry[4] = clean_entry[4].replace('0x', '')
    clean_entry[4] = int(clean_entry[4], 16)
    # Clean unused_operands entry
    clean_entry[5] = clean_entry[5].strip()
    # Clean length entry
    clean_entry[6] = clean_entry[6].strip()
    clean_entry[6] = clean_entry[6].replace('(uint8_t)', '')
    clean_entry[6] = clean_entry[6].replace('0x', '')
    clean_entry[6] = int(clean_entry[6])
    # Clean mnemonic entry
    clean_entry[7] = clean_entry[7].replace('\"', '')
    clean_entry[7] = clean_entry[7].strip()
    # Clean alternative_instr entry
    clean_entry[8] = clean_entry[8].replace('\"', '')
    clean_entry[8] = clean_entry[8].strip()
    return clean_entry


def load_table(table : List[Any]):
    with open('../src/InstructionTable.h') as f:
        table_keyword = 'INSTRUCTION_TABLE'
        start_parsing = False
        buffer = ''
        level = 1
        in_entry = False
        for line in f:
            if(start_parsing == False):
                tk_idx = line.find(table_keyword)
                if(tk_idx != -1):
                    start_parsing = True
            else:
                for i in range(0, len(line)):
                    if(line[i] == '{'):
                        level += 1
                        continue
                    elif(line[i] == '}'):
                        level -= 1
                        continue
                    if(level == 2):
                        in_entry = True
                        buffer = buffer + line[i]
                    elif(level == 1):
                        if(not in_entry):
                            continue
                        table.append(clean_table_entry(buffer))
                        buffer = ''
                        in_entry = False
                    else:
                        break
    return table