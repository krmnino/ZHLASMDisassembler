import Utils
from LoadTable import FORMAT_IDX
from LoadTable import UNUSED_IDX
from LoadTable import MNEMONIC_IDX

MNEMONIC_MAX_LEN = 8

def generate_E(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX] 
    asm_instr += mnemonic
    return asm_instr

def generate_I(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    i = Utils.gen_operand(0xFF)
    asm_instr += i
    return asm_instr

def generate_IE(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    i1 = Utils.gen_operand(0xF)
    i2 = Utils.gen_operand(0xF)
    asm_instr += i1
    asm_instr += ','
    asm_instr += i2
    return asm_instr

def generate_MII(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
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

def generate_RI(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
    format = table[tidx][FORMAT_IDX]
    unused = table[tidx][UNUSED_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r1_m1 = Utils.gen_operand(0xF)
    i2_ri2 = Utils.gen_operand(0xFFFF)
    if(format != 'RIc' or (format == 'RIc' and 'M1_UNUSED' not in unused)):
        asm_instr += r1_m1
        asm_instr += ','
    asm_instr += i2_ri2
    return asm_instr

def generate_RIE(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
    format = table[tidx][FORMAT_IDX]
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

def generate_RIL(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
    asm_instr += mnemonic 
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r1_m1 = Utils.gen_operand(0xF)
    i2_ri2 = Utils.gen_operand(0xFFFFFFFF)
    asm_instr += r1_m1
    asm_instr += ','
    asm_instr += i2_ri2
    return asm_instr

def generate_RIS(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
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

def generate_RR(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
    unused = table[tidx][UNUSED_IDX]
    asm_instr += mnemonic
    asm_instr += ''.join([' ' for j in range(0, MNEMONIC_MAX_LEN - len(mnemonic))])
    r1 = Utils.gen_operand(0xF)
    r2 = Utils.gen_operand(0xF)
    asm_instr += r1
    if('R2_UNUSED' not in unused):
        asm_instr += ','
        asm_instr += r2
    return asm_instr

def generate_RRD(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
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

def generate_RRE(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
    unused = table[tidx][UNUSED_IDX]
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

def generate_RRF(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
    format = table[tidx][FORMAT_IDX]
    unused = table[tidx][UNUSED_IDX]
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

def generate_RRS(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
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

def generate_RS(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
    format = table[tidx][FORMAT_IDX]
    unused = table[tidx][UNUSED_IDX]
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

def generate_RSI(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
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

def generate_RSL(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
    format = table[tidx][FORMAT_IDX]
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

def generate_RSY(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
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

def generate_RX(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
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

def generate_RXE(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
    unused = table[tidx][UNUSED_IDX]
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

def generate_RXF(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
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

def generate_RXY(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
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

def generate_S(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
    unused = table[tidx][UNUSED_IDX]
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

def generate_SI(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
    unused = table[tidx][UNUSED_IDX]
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

def generate_SIL(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
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

def generate_SIY(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
    unused = table[tidx][UNUSED_IDX]
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

def generate_SMI(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
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

def generate_SS(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
    format = table[tidx][FORMAT_IDX]
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

def generate_SSE(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
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

def generate_SSF(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
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

def generate_VRI(table, tidx):
    asm_instr = ''
    mnemonic = table[tidx][MNEMONIC_IDX]
    format = table[tidx][FORMAT_IDX]
    unused = table[tidx][UNUSED_IDX]
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
        i3 = Utils.gen_operand(0xFF)
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