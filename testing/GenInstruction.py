from typing import List

import Utils
from ProcessParms import pgm_parms
from ProcessParms import available_instructions
from LoadTable import FORMAT_IDX
from LoadTable import UNUSED_IDX
from LoadTable import MNEMONIC_IDX

MNEMONIC_MAX_LEN = 8

def generate_stream() -> List[str]:
    stream = []
    i = 0
    while(i < pgm_parms['max_stream_size']):
        if(len(available_instructions) == 1):
            rand_idx = 0
        else:
            rand_idx = Utils.gen_random_integer(0, len(available_instructions) - 1)
        format = available_instructions[rand_idx][FORMAT_IDX]
        if(format == 'E'):
            stream.append(generate_E(rand_idx))
        elif(format == 'I'):
            stream.append(generate_I(rand_idx))
        elif(format == 'IE'):
            stream.append(generate_IE(rand_idx))
        elif(format == 'MII'):
            stream.append(generate_MII(rand_idx))
        elif(format == 'RIa' or \
             format == 'RIb' or \
             format == 'RIc'):
            stream.append(generate_RI(rand_idx))
        elif(format == 'RIEa' or \
             format == 'RIEb' or \
             format == 'RIEc' or \
             format == 'RIEd' or \
             format == 'RIEe' or \
             format == 'RIEf' or \
             format == 'RIEg'):
            stream.append(generate_RIE(rand_idx))
        elif(format == 'RILa' or \
             format == 'RILb' or \
             format == 'RILc'):
            stream.append(generate_RIL(rand_idx))
        elif(format == 'RIS'):
            stream.append(generate_RIS(rand_idx))
        elif(format == 'RR'):
            stream.append(generate_RR(rand_idx))
        elif(format == 'RRD'):
            stream.append(generate_RRD(rand_idx))
        elif(format == 'RRE'):
            stream.append(generate_RRE(rand_idx))
        elif(format == 'RRFa' or \
             format == 'RRFb' or \
             format == 'RRFc' or \
             format == 'RRFd' or \
             format == 'RRFe'):
            stream.append(generate_RRF(rand_idx))
        elif(format == 'RRS'):
            stream.append(generate_RRS(rand_idx))
        elif(format == 'RSa' or \
             format == 'RSb'):
            stream.append(generate_RS(rand_idx))
        elif(format == 'RSI'):
            stream.append(generate_RSI(rand_idx))
        elif(format == 'RSLa' or \
             format == 'RSLb'):
            stream.append(generate_RSL(rand_idx))
        elif(format == 'RSYa' or \
             format == 'RSYb'):
            stream.append(generate_RSY(rand_idx))
        elif(format == 'RXa' or \
             format == 'RXb'):
            stream.append(generate_RX(rand_idx))
        elif(format == 'RXE'):
            stream.append(generate_RXE(rand_idx))
        elif(format == 'RXF'):
            stream.append(generate_RXF(rand_idx))
        elif(format == 'RXYa' or \
             format == 'RXYb'):
            stream.append(generate_RXY(rand_idx))
        elif(format == 'S'):
            stream.append(generate_S(rand_idx))
        elif(format == 'SI'):
            stream.append(generate_SI(rand_idx))
        elif(format == 'SIL'):
            stream.append(generate_SIL(rand_idx))
        elif(format == 'SIY'):
            stream.append(generate_SIY(rand_idx))
        elif(format == 'SMI'):
            stream.append(generate_SMI(rand_idx))
        elif(format == 'SSa' or \
             format == 'SSb' or \
             format == 'SSc' or \
             format == 'SSd' or \
             format == 'SSe' or \
             format == 'SSf'):
            stream.append(generate_SS(rand_idx))
        elif(format == 'SSE'):
            stream.append(generate_SSE(rand_idx))
        elif(format == 'SSF'):
            stream.append(generate_SSF(rand_idx))
        elif(format == 'VRIa' or \
             format == 'VRIb' or \
             format == 'VRIc' or \
             format == 'VRId' or \
             format == 'VRIe' or \
             format == 'VRIf' or \
             format == 'VRIg' or \
             format == 'VRIh' or \
             format == 'VRIi'):
            stream.append(generate_VRI(rand_idx))
        elif(format == 'VRRa' or \
             format == 'VRRb' or \
             format == 'VRRc' or \
             format == 'VRRd' or \
             format == 'VRRe' or \
             format == 'VRRf' or \
             format == 'VRRg' or \
             format == 'VRRh' or \
             format == 'VRRi' or \
             format == 'VRRj' or \
             format == 'VRRk'):
            stream.append(generate_VRR(rand_idx))
        elif(format == 'VRRa' or \
             format == 'VRRb' or \
             format == 'VRRc' or \
             format == 'VRRd'):
            stream.append(generate_VRS(rand_idx))
        elif(format == 'VRV'):
            stream.append(generate_VRV(rand_idx))
        elif(format == 'VRX'):
            stream.append(generate_VRX(rand_idx))
        elif(format == 'VSI'):
            stream.append(generate_VSI(rand_idx))
        else:
            continue
        i += 1
    return stream

def generate_E(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX] 
    asm_instr += mnemonic
    return asm_instr

def generate_I(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    i = Utils.gen_operand(0xFF)
    asm_instr += i
    return asm_instr

def generate_IE(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    i1 = Utils.gen_operand(0xF)
    i2 = Utils.gen_operand(0xF)
    asm_instr += i1
    asm_instr += ','
    asm_instr += i2
    return asm_instr

def generate_MII(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    m1 = Utils.gen_operand(0xF)
    ri2 = Utils.gen_operand(0xFFF)
    ri3 = Utils.gen_operand(0xFFFFFF)
    asm_instr += m1
    asm_instr += ','
    asm_instr += ri2
    asm_instr += ','
    asm_instr += ri3
    return asm_instr

def generate_RI(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    format = available_instructions[tidx][FORMAT_IDX]
    unused = available_instructions[tidx][UNUSED_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r1_m1 = Utils.gen_operand(0xF)
    i2_ri2 = Utils.gen_operand(0xFFFF)
    if(format != 'RIc' or (format == 'RIc' and 'M1_UNUSED' not in unused)):
        asm_instr += r1_m1
        asm_instr += ','
    asm_instr += i2_ri2
    return asm_instr

def generate_RIE(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    format = available_instructions[tidx][FORMAT_IDX]
    asm_instr += mnemonic
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    if(format == 'RIEa'):
        r1 = Utils.gen_operand(0xF)
        i2 = Utils.gen_operand(0xFFFF)
        m3 = Utils.gen_operand(0xF)
        asm_instr += r1
        asm_instr += ','
        asm_instr += i2
        asm_instr += ','
        asm_instr += m3
    elif(format == 'RIEb'):
        r1 = Utils.gen_operand(0xF)
        r2 = Utils.gen_operand(0xF)
        m3 = Utils.gen_operand(0xF)
        ri4 = Utils.gen_operand(0xFFFF)
        asm_instr += r1
        asm_instr += ','
        asm_instr += r2
        asm_instr += ','
        asm_instr += m3
        asm_instr += ','
        asm_instr += ri4
    elif(format == 'RIEc'):
        r1 = Utils.gen_operand(0xF)
        i2 = Utils.gen_operand(0xFF)
        m3 = Utils.gen_operand(0xF)
        ri4 = Utils.gen_operand(0xFFFF)
        asm_instr += r1
        asm_instr += ','
        asm_instr += i2
        asm_instr += ','
        asm_instr += m3
        asm_instr += ','
        asm_instr += ri4
    elif(format == 'RIEd' or format == 'RIEe'):
        r1 = Utils.gen_operand(0xF)
        r3 = Utils.gen_operand(0xF)
        i2_ri2 = Utils.gen_operand(0xFFFF)
        asm_instr += r1
        asm_instr += ','
        asm_instr += r3
        asm_instr += ','
        asm_instr += i2_ri2
    elif(format == 'RIEf'):
        r1 = Utils.gen_operand(0xF)
        r2 = Utils.gen_operand(0xF)
        i3 = Utils.gen_operand(0xFF)
        i4 = Utils.gen_operand(0xFF)
        i5 = Utils.gen_operand(0xFF)
        asm_instr += r1
        asm_instr += ','
        asm_instr += r2
        asm_instr += ','
        asm_instr += i3
        asm_instr += ','
        asm_instr += i4
        asm_instr += ','
        asm_instr += i5
    elif(format == 'RIEg'):
        r1 = Utils.gen_operand(0xF)
        i2 = Utils.gen_operand(0xFFFF)
        m3 = Utils.gen_operand(0xF)
        asm_instr += r1
        asm_instr += ','
        asm_instr += i2
        asm_instr += ','
        asm_instr += m3
    return asm_instr

def generate_RIL(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r1_m1 = Utils.gen_operand(0xF)
    i2_ri2 = Utils.gen_operand(0xFFFFFFFF)
    asm_instr += r1_m1
    asm_instr += ','
    asm_instr += i2_ri2
    return asm_instr

def generate_RIS(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r1 = Utils.gen_operand(0xF)
    i2 = Utils.gen_operand(0xFF)
    m3 = Utils.gen_operand(0xF)
    d4 = Utils.gen_operand(0xFFF)
    b4 = Utils.gen_operand(0xF)
    asm_instr += r1
    asm_instr += ','
    asm_instr += i2
    asm_instr += ','
    asm_instr += m3
    asm_instr += ','
    asm_instr += d4
    asm_instr += '('
    asm_instr += b4
    asm_instr += ')'
    return asm_instr

def generate_RR(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    unused = available_instructions[tidx][UNUSED_IDX]
    asm_instr += mnemonic
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r1 = Utils.gen_operand(0xF)
    r2 = Utils.gen_operand(0xF)
    asm_instr += r1
    if('R2_UNUSED' not in unused):
        asm_instr += ','
        asm_instr += r2
    return asm_instr

def generate_RRD(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r1 = Utils.gen_operand(0xF)
    r3 = Utils.gen_operand(0xF)
    r2 = Utils.gen_operand(0xF)
    asm_instr += r1
    asm_instr += ','
    asm_instr += r3
    asm_instr += ','
    asm_instr += r2
    return asm_instr

def generate_RRE(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    unused = available_instructions[tidx][UNUSED_IDX]
    asm_instr += mnemonic
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r1 = Utils.gen_operand(0xF)
    r2 = Utils.gen_operand(0xF)
    if('R1_UNUSED' not in unused):
        asm_instr += r1
    if('R2_UNUSED' not in unused):
        asm_instr += ','
        asm_instr += r2
    return asm_instr

def generate_RRF(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    format = available_instructions[tidx][FORMAT_IDX]
    unused = available_instructions[tidx][UNUSED_IDX]
    asm_instr += mnemonic
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    if(format == 'RRFa'):
        r1 = Utils.gen_operand(0xF)
        r2 = Utils.gen_operand(0xF)
        r3 = Utils.gen_operand(0xF)
        m4 = Utils.gen_operand(0xF)
        asm_instr += r1
        asm_instr += ','
        asm_instr += r2
        asm_instr += ','
        asm_instr += r3
        if('M4_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m4
    elif(format == 'RRFb'):
        r1 = Utils.gen_operand(0xF)
        r3 = Utils.gen_operand(0xF)
        r2 = Utils.gen_operand(0xF)
        m4 = Utils.gen_operand(0xF)
        asm_instr += r1
        asm_instr += ','
        asm_instr += r3
        asm_instr += ','
        asm_instr += r2
        if('M4_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m4
    elif(format == 'RRFc'):
        r1 = Utils.gen_operand(0xF)
        r2 = Utils.gen_operand(0xF)
        m3 = Utils.gen_operand(0xF)
        asm_instr += r1
        asm_instr += ','
        asm_instr += r2
        if('M3_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m3
    elif(format == 'RRFd'):
        r1 = Utils.gen_operand(0xF)
        r2 = Utils.gen_operand(0xF)
        m4 = Utils.gen_operand(0xF)
        asm_instr += r1
        asm_instr += ','
        asm_instr += r2
        if('M4_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m4
    elif(format == 'RRFe'):
        r1 = Utils.gen_operand(0xF)
        m3 = Utils.gen_operand(0xF)
        r2 = Utils.gen_operand(0xF)
        m4 = Utils.gen_operand(0xF)
        asm_instr += r1
        asm_instr += ','
        asm_instr += m3
        asm_instr += ','
        asm_instr += r2
        if('M4_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m4
    return asm_instr

def generate_RRS(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r1 = Utils.gen_operand(0xF)
    r2 = Utils.gen_operand(0xF)
    m3 = Utils.gen_operand(0xF)
    d4 = Utils.gen_operand(0xFFF)
    b4 = Utils.gen_operand(0xF)
    asm_instr += r1
    asm_instr += ','
    asm_instr += r2
    asm_instr += ','
    asm_instr += m3
    asm_instr += ','
    asm_instr += d4
    asm_instr += '('
    asm_instr += b4
    asm_instr += ')'
    return asm_instr

def generate_RS(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    format = available_instructions[tidx][FORMAT_IDX]
    unused = available_instructions[tidx][UNUSED_IDX]
    asm_instr += mnemonic
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r1 = Utils.gen_operand(0xF)
    r3_m3 = Utils.gen_operand(0xF)
    b2 = Utils.gen_operand(0xF)
    d2 = Utils.gen_operand(0xFFF)
    if(format == 'RSa'):
        asm_instr += r1
        if(unused not in 'R3_UNUSED'):
            asm_instr += ','
            asm_instr += r3_m3
        asm_instr += ','
        asm_instr += d2
        asm_instr += '('
        asm_instr += b2
        asm_instr += ')'
    elif(format == 'RSb'):
        asm_instr += r1
        asm_instr += ','
        asm_instr += r3_m3
        asm_instr += ','
        asm_instr += d2
        asm_instr += '('
        asm_instr += b2
        asm_instr += ')'
    return asm_instr

def generate_RSI(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r1 = Utils.gen_operand(0xF)
    r3 = Utils.gen_operand(0xF)
    ri2 = Utils.gen_operand(0xFFFF)
    asm_instr += r1
    asm_instr += ','
    asm_instr += r3
    asm_instr += ','
    asm_instr += ri2
    return asm_instr

def generate_RSL(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    format = available_instructions[tidx][FORMAT_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    if(format == 'RSLa'):
        d1 = Utils.gen_operand(0xFFF)
        l1 = Utils.gen_operand(0xF)
        b1 = Utils.gen_operand(0xF)
        asm_instr += d1
        asm_instr += '('
        asm_instr += l1
        asm_instr += ','
        asm_instr += b1
        asm_instr += ')'
    elif(format == 'RSLb'):
        r1 = Utils.gen_operand(0xF)
        d2 = Utils.gen_operand(0xFFF)
        l2 = Utils.gen_operand(0xFF)
        b2 = Utils.gen_operand(0xF)
        m3 = Utils.gen_operand(0xF)
        asm_instr += r1
        asm_instr += ','
        asm_instr += d2
        asm_instr += '('
        asm_instr += l2
        asm_instr += ','
        asm_instr += b2
        asm_instr += ')'
        asm_instr += ','
        asm_instr += m3
    return asm_instr

def generate_RSY(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r1 = Utils.gen_operand(0xF)
    r3_m3 = Utils.gen_operand(0xF)
    d2 = Utils.gen_operand(0xFFFFF)
    b2 = Utils.gen_operand(0xF)
    asm_instr += r1
    asm_instr += ','
    asm_instr += r3_m3
    asm_instr += ','
    asm_instr += d2
    asm_instr += '('
    asm_instr += b2
    asm_instr += ')'
    return asm_instr

def generate_RX(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r1_m1 = Utils.gen_operand(0xF)
    d2 = Utils.gen_operand(0xFFF)
    x2 = Utils.gen_operand(0xF)
    b2 = Utils.gen_operand(0xF)
    asm_instr += r1_m1
    asm_instr += ','
    asm_instr += d2
    asm_instr += '('
    asm_instr += x2
    asm_instr += ','
    asm_instr += b2
    asm_instr += ')'
    return asm_instr

def generate_RXE(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    unused = available_instructions[tidx][UNUSED_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r1 = Utils.gen_operand(0xF)
    d2 = Utils.gen_operand(0xFFF)
    x2 = Utils.gen_operand(0xF)
    b2 = Utils.gen_operand(0xF)
    m3 = Utils.gen_operand(0xF)
    asm_instr += r1
    asm_instr += ','
    asm_instr += d2
    asm_instr += '('
    asm_instr += x2
    asm_instr += ','
    asm_instr += b2
    asm_instr += ')'
    if('M3_UNUSED' not in unused):
        asm_instr += ','
        asm_instr += m3
    return asm_instr

def generate_RXF(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r1 = Utils.gen_operand(0xF)
    r3 = Utils.gen_operand(0xF)
    d2 = Utils.gen_operand(0xFFF)
    x2 = Utils.gen_operand(0xF)
    b2 = Utils.gen_operand(0xF)
    asm_instr += r1
    asm_instr += ','
    asm_instr += r3
    asm_instr += ','
    asm_instr += d2
    asm_instr += '('
    asm_instr += x2
    asm_instr += ','
    asm_instr += b2
    asm_instr += ')'
    return asm_instr

def generate_RXY(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r1_m1 = Utils.gen_operand(0xF)
    d2 = Utils.gen_operand(0xFFFFF)
    x2 = Utils.gen_operand(0xF)
    b2 = Utils.gen_operand(0xF)
    asm_instr += r1_m1
    asm_instr += ','
    asm_instr += d2
    asm_instr += '('
    asm_instr += x2
    asm_instr += ','
    asm_instr += b2
    asm_instr += ')'
    return asm_instr

def generate_S(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    unused = available_instructions[tidx][UNUSED_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    d2 = Utils.gen_operand(0xFFF)
    b2 = Utils.gen_operand(0xF)
    if('D2_UNUSED' not in unused):
        asm_instr += d2
    if('B2_UNUSED' not in unused):
        asm_instr += '('
        asm_instr += b2
        asm_instr += ')'
    return asm_instr

def generate_SI(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    unused = available_instructions[tidx][UNUSED_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    d1 = Utils.gen_operand(0xFFF)
    b1 = Utils.gen_operand(0xF)
    i2 = Utils.gen_operand(0xFF)
    asm_instr += d1
    asm_instr += '('
    asm_instr += b1
    asm_instr += ')'
    if('I2_UNUSED' not in unused):
        asm_instr += ','
        asm_instr += i2
    return asm_instr

def generate_SIL(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    d1 = Utils.gen_operand(0xFFF)
    b1 = Utils.gen_operand(0xF)
    i2 = Utils.gen_operand(0xFFFF)
    asm_instr += d1
    asm_instr += '('
    asm_instr += b1
    asm_instr += ')'
    asm_instr += ','
    asm_instr += i2
    return asm_instr

def generate_SIY(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    unused = available_instructions[tidx][UNUSED_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    d1 = Utils.gen_operand(0xFFFFF)
    b1 = Utils.gen_operand(0xF)
    i2 = Utils.gen_operand(0xFF)
    asm_instr += d1
    asm_instr += '('
    asm_instr += b1
    asm_instr += ')'
    if('I2_UNUSED' not in unused):
        asm_instr += ','
        asm_instr += i2
    return asm_instr

def generate_SMI(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    m1 = Utils.gen_operand(0xF)
    ri2 = Utils.gen_operand(0xFFFF)
    d3 = Utils.gen_operand(0xFFF)
    b3 = Utils.gen_operand(0xF)
    asm_instr += m1
    asm_instr += ','
    asm_instr += ri2
    asm_instr += ','
    asm_instr += d3
    asm_instr += '('
    asm_instr += b3
    asm_instr += ')'
    return asm_instr

def generate_SS(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    format = available_instructions[tidx][FORMAT_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    if(format == 'SSa'):
        d1 = Utils.gen_operand(0xFFF)
        l1 = Utils.gen_operand(0xFF)
        b1 = Utils.gen_operand(0xF)
        d2 = Utils.gen_operand(0xFFF)
        b2 = Utils.gen_operand(0xF)
        asm_instr += d1
        asm_instr += '('
        asm_instr += l1
        asm_instr += ','
        asm_instr += b1
        asm_instr += ')'
        asm_instr += ','
        asm_instr += d2
        asm_instr += '('
        asm_instr += b2
        asm_instr += ')'
    elif(format == 'SSb'):
        d1 = Utils.gen_operand(0xFFF)
        l1 = Utils.gen_operand(0xF)
        b1 = Utils.gen_operand(0xF)
        d2 = Utils.gen_operand(0xFFF)
        l2 = Utils.gen_operand(0xF)
        b2 = Utils.gen_operand(0xF)
        asm_instr += d1
        asm_instr += '('
        asm_instr += l1
        asm_instr += ','
        asm_instr += b1
        asm_instr += ')'
        asm_instr += ','
        asm_instr += d2
        asm_instr += '('
        asm_instr += l2
        asm_instr += ','
        asm_instr += b2
        asm_instr += ')'
    elif(format == 'SSc' or format == 'SSd'):
        d1 = Utils.gen_operand(0xFFF)
        l1_r1 = Utils.gen_operand(0xF)
        b1 = Utils.gen_operand(0xF)
        d2 = Utils.gen_operand(0xFFF)
        b2 = Utils.gen_operand(0xF)
        i3_r3 = Utils.gen_operand(0xF)
        asm_instr += d1
        asm_instr += '('
        asm_instr += l1_r1
        asm_instr += ','
        asm_instr += b1
        asm_instr += ')'
        asm_instr += ','
        asm_instr += d2
        asm_instr += '('
        asm_instr += b2
        asm_instr += ')'
        asm_instr += ','
        asm_instr += i3_r3
    elif(format == 'SSe'):
        r1 = Utils.gen_operand(0xF)
        r3 = Utils.gen_operand(0xF)
        d2 = Utils.gen_operand(0xFFF)
        b2 = Utils.gen_operand(0xF)
        d4 = Utils.gen_operand(0xFFF)
        b4 = Utils.gen_operand(0xF)
        asm_instr += r1
        asm_instr += ','
        asm_instr += r3
        asm_instr += ','
        asm_instr += d2
        asm_instr += '('
        asm_instr += b2
        asm_instr += ')'
        asm_instr += ','
        asm_instr += d4
        asm_instr += '('
        asm_instr += b4
        asm_instr += ')'
    elif(format == 'SSf'):
        d1 = Utils.gen_operand(0xFFF)
        b1 = Utils.gen_operand(0xF)
        d2 = Utils.gen_operand(0xFFF)
        l2 = Utils.gen_operand(0xFF)
        b2 = Utils.gen_operand(0xF)
        asm_instr += d1
        asm_instr += '('
        asm_instr += b1
        asm_instr += ')'
        asm_instr += ','
        asm_instr += d2
        asm_instr += '('
        asm_instr += l2
        asm_instr += ','
        asm_instr += b2
        asm_instr += ')'
    return asm_instr

def generate_SSE(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    d1 = Utils.gen_operand(0xFFF)
    b1 = Utils.gen_operand(0xF)
    d2 = Utils.gen_operand(0xFFF)
    b2 = Utils.gen_operand(0xF)
    asm_instr += d1
    asm_instr += '('
    asm_instr += b1
    asm_instr += ')'
    asm_instr += ','
    asm_instr += d2
    asm_instr += '('
    asm_instr += b2
    asm_instr += ')'
    return asm_instr

def generate_SSF(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r3 = Utils.gen_operand(0xF)
    d1 = Utils.gen_operand(0xFFF)
    b1 = Utils.gen_operand(0xF)
    d2 = Utils.gen_operand(0xFFF)
    b2 = Utils.gen_operand(0xF)
    asm_instr += r3
    asm_instr += ','
    asm_instr += d1
    asm_instr += '('
    asm_instr += b1
    asm_instr += ')'
    asm_instr += ','
    asm_instr += d2
    asm_instr += '('
    asm_instr += b2
    asm_instr += ')'
    return asm_instr

def generate_VRI(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    format = available_instructions[tidx][FORMAT_IDX]
    unused = available_instructions[tidx][UNUSED_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    if(format == 'VRIa'):
        v1 = Utils.gen_operand(0xF)
        i2 = Utils.gen_operand(0xFFFF)
        m3 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += i2
        if('M3_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m3
    elif(format == 'VRIb'):
        v1 = Utils.gen_operand(0xF)
        i2 = Utils.gen_operand(0xFF)
        i3 = Utils.gen_operand(0xFF)
        m4 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += i2
        asm_instr += ','
        asm_instr += i3
        asm_instr += ','
        asm_instr += m4
    elif(format == 'VRIc'):
        v1 = Utils.gen_operand(0xF)
        v3 = Utils.gen_operand(0xF)
        i2 = Utils.gen_operand(0xFFFF)
        m4 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += v3
        asm_instr += ','
        asm_instr += i2
        asm_instr += ','
        asm_instr += m4
    elif(format == 'VRId'):
        v1 = Utils.gen_operand(0xF)
        v2 = Utils.gen_operand(0xF)
        v3 = Utils.gen_operand(0xF)
        i4 = Utils.gen_operand(0xFF)
        m5 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += v2
        asm_instr += ','
        asm_instr += v3
        asm_instr += ','
        asm_instr += i4
        if('M5_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m5
    elif(format == 'VRIe'):
        v1 = Utils.gen_operand(0xF)
        v2 = Utils.gen_operand(0xF)
        i3 = Utils.gen_operand(0xFFF)
        m4 = Utils.gen_operand(0xF)
        m5 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += v2
        asm_instr += ','
        asm_instr += i3
        asm_instr += ','
        asm_instr += m4
        asm_instr += ','
        asm_instr += m5
    elif(format == 'VRIf'):
        v1 = Utils.gen_operand(0xF)
        v2 = Utils.gen_operand(0xF)
        v3 = Utils.gen_operand(0xF)
        i4 = Utils.gen_operand(0xFF)
        m5 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += v2
        asm_instr += ','
        asm_instr += v3
        asm_instr += ','
        asm_instr += i4
        asm_instr += ','
        asm_instr += m5
    elif(format == 'VRIg'):
        v1 = Utils.gen_operand(0xF)
        v2 = Utils.gen_operand(0xF)
        i3 = Utils.gen_operand(0xFF)
        i4 = Utils.gen_operand(0xFF)
        m5 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += v2
        asm_instr += ','
        asm_instr += i3
        asm_instr += ','
        asm_instr += i4
        asm_instr += ','
        asm_instr += m5
    elif(format == 'VRIh'):
        v1 = Utils.gen_operand(0xF)
        i2 = Utils.gen_operand(0xFFFF)
        i3 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += i2
        asm_instr += ','
        asm_instr += i3
    elif(format == 'VRIi'):
        v1 = Utils.gen_operand(0xF)
        r2 = Utils.gen_operand(0xF)
        i3 = Utils.gen_operand(0xFF)
        m4 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += r2
        asm_instr += ','
        asm_instr += i3
        asm_instr += ','
        asm_instr += m4
    return asm_instr

def generate_VRR(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    format = available_instructions[tidx][FORMAT_IDX]
    unused = available_instructions[tidx][UNUSED_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    if(format == 'VRRa'):
        v1 = Utils.gen_operand(0xF)
        v2 = Utils.gen_operand(0xF)
        m3 = Utils.gen_operand(0xF)
        m4 = Utils.gen_operand(0xF)
        m5 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += v2
        if('M3_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m3
        if('M4_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m4
        if('M5_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m5
    elif(format == 'VRRb'):
        v1 = Utils.gen_operand(0xF)
        v2 = Utils.gen_operand(0xF)
        v3 = Utils.gen_operand(0xF)
        m4 = Utils.gen_operand(0xF)
        m5 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += v2
        asm_instr += ','
        asm_instr += v3
        if('M4_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m4
        if('M5_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m5
    elif(format == 'VRRc'):
        v1 = Utils.gen_operand(0xF)
        v2 = Utils.gen_operand(0xF)
        v3 = Utils.gen_operand(0xF)
        m4 = Utils.gen_operand(0xF)
        m5 = Utils.gen_operand(0xF)
        m6 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += v2
        asm_instr += ','
        asm_instr += v3
        if('M4_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m4
        if('M5_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m5
        if('M6_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m6
    elif(format == 'VRRd'):
        v1 = Utils.gen_operand(0xF)
        v2 = Utils.gen_operand(0xF)
        v3 = Utils.gen_operand(0xF)
        v4 = Utils.gen_operand(0xF)
        m5 = Utils.gen_operand(0xF)
        m6 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += v2
        asm_instr += ','
        asm_instr += v3
        asm_instr += ','
        asm_instr += v4
        if('M5_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m5
        if('M6_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m6
    elif(format == 'VRRe'):
        v1 = Utils.gen_operand(0xF)
        v2 = Utils.gen_operand(0xF)
        v3 = Utils.gen_operand(0xF)
        v4 = Utils.gen_operand(0xF)
        m5 = Utils.gen_operand(0xF)
        m6 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += v2
        asm_instr += ','
        asm_instr += v3
        asm_instr += ','
        asm_instr += v4
        if('M5_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m5
        if('M6_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m6
    elif(format == 'VRRf'):
        v1 = Utils.gen_operand(0xF)
        r2 = Utils.gen_operand(0xF)
        r3 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += r2
        asm_instr += ','
        asm_instr += r3
    elif(format == 'VRRg'):
        v1 = Utils.gen_operand(0xF)
        asm_instr += v1
    elif(format == 'VRRh'):
        v1 = Utils.gen_operand(0xF)
        v2 = Utils.gen_operand(0xF)
        m3 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += v2
        if('M3_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m3
    elif(format == 'VRRi'):
        r1 = Utils.gen_operand(0xF)
        v2 = Utils.gen_operand(0xF)
        m3 = Utils.gen_operand(0xF)
        m4 = Utils.gen_operand(0xF)
        asm_instr += r1
        asm_instr += ','
        asm_instr += v2
        asm_instr += ','
        asm_instr += m3
        if('M4_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m4
    elif(format == 'VRRj'):
        v1 = Utils.gen_operand(0xF)
        v2 = Utils.gen_operand(0xF)
        v3 = Utils.gen_operand(0xF)
        m4 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += v2
        asm_instr += ','
        asm_instr += v3
        asm_instr += ','
        asm_instr += m4
    elif(format == 'VRRk'):
        v1 = Utils.gen_operand(0xF)
        v2 = Utils.gen_operand(0xF)
        m3 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += v2
        asm_instr += ','
        asm_instr += m3
    return asm_instr

def generate_VRS(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    format = available_instructions[tidx][FORMAT_IDX]
    unused = available_instructions[tidx][UNUSED_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    if(format == 'VRSa' or format == 'VRSb'):
        v1 = Utils.gen_operand(0xF)
        r3_v3 = Utils.gen_operand(0xF)
        d2 = Utils.gen_operand(0xFFF)
        b2 = Utils.gen_operand(0xF)
        m4 = Utils.gen_operand(0xF)
        asm_instr += v1
        asm_instr += ','
        asm_instr += r3_v3
        asm_instr += ','
        asm_instr += d2
        asm_instr += '('
        asm_instr += b2
        asm_instr += ')'
        if('M4_UNUSED' not in unused):
            asm_instr += ','
            asm_instr += m4
    elif(format == 'VRSc'):
        r1 = Utils.gen_operand(0xF)
        v3 = Utils.gen_operand(0xF)
        d2 = Utils.gen_operand(0xFFF)
        b2 = Utils.gen_operand(0xF)
        m4 = Utils.gen_operand(0xF)
        asm_instr += r1
        asm_instr += ','
        asm_instr += v3
        asm_instr += ','
        asm_instr += d2
        asm_instr += '('
        asm_instr += b2
        asm_instr += ')'
        asm_instr += ','
        asm_instr += m4
    elif(format == 'VRSd'):
        v1 = Utils.gen_operand(0xF)
        r3 = Utils.gen_operand(0xF)
        d2 = Utils.gen_operand(0xFFF)
        b2 = Utils.gen_operand(0xF)
        asm_instr += r1
        asm_instr += ','
        asm_instr += r3
        asm_instr += ','
        asm_instr += d2
        asm_instr += '('
        asm_instr += b2
        asm_instr += ')'
    return asm_instr

def generate_VRV(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    unused = available_instructions[tidx][UNUSED_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    v1 = Utils.gen_operand(0xF)
    d2 = Utils.gen_operand(0xFFF)
    v2 = Utils.gen_operand(0xF)
    b2 = Utils.gen_operand(0xF)
    m3 = Utils.gen_operand(0xF)
    asm_instr += v1
    asm_instr += ','
    asm_instr += d2
    asm_instr += '('
    asm_instr += v2
    asm_instr += ','
    asm_instr += b2
    asm_instr += ')'
    if('M3_UNUSED' not in unused):
        asm_instr += ','
        asm_instr += m3
    return asm_instr

def generate_VRX(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    unused = available_instructions[tidx][UNUSED_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    v1 = Utils.gen_operand(0xF)
    d2 = Utils.gen_operand(0xFFF)
    x2 = Utils.gen_operand(0xF)
    b2 = Utils.gen_operand(0xF)
    m3 = Utils.gen_operand(0xF)
    asm_instr += v1
    asm_instr += ','
    asm_instr += d2
    asm_instr += '('
    asm_instr += x2
    asm_instr += ','
    asm_instr += b2
    asm_instr += ')'
    if('M3_UNUSED' not in unused):
        asm_instr += ','
        asm_instr += m3
    return asm_instr

def generate_VSI(tidx : int) -> str:
    asm_instr = ''
    mnemonic = available_instructions[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    v1 = Utils.gen_operand(0xF)
    d2 = Utils.gen_operand(0xFFF)
    b2 = Utils.gen_operand(0xF)
    i3 = Utils.gen_operand(0xFF)
    asm_instr += v1
    asm_instr += ','
    asm_instr += d2
    asm_instr += '('
    asm_instr += b2
    asm_instr += ')'
    asm_instr += ','
    asm_instr += i3
    return asm_instr