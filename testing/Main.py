from ProcessParms import enabled_formats
from ProcessParms import load_parameters
from LoadTable import FORMAT_IDX
from LoadTable import load_table
from GenInstruction import generate_E
from GenInstruction import generate_I
from GenInstruction import generate_IE
from GenInstruction import generate_MII
from GenInstruction import generate_RI
from GenInstruction import generate_RIE
from GenInstruction import generate_RIL
from GenInstruction import generate_RIS
from GenInstruction import generate_RR
from GenInstruction import generate_RRD
from GenInstruction import generate_RRE
from GenInstruction import generate_RRF
from GenInstruction import generate_RRS
from GenInstruction import generate_RS
from GenInstruction import generate_RSI
from GenInstruction import generate_RSL
from GenInstruction import generate_RSY
from GenInstruction import generate_RX
from GenInstruction import generate_RXE
from GenInstruction import generate_RXF
from GenInstruction import generate_RXY
from GenInstruction import generate_S
from GenInstruction import generate_SI
from GenInstruction import generate_SIL
from GenInstruction import generate_SIY
from GenInstruction import generate_SMI
from GenInstruction import generate_SS
from GenInstruction import generate_SSE
from GenInstruction import generate_SSF
from GenInstruction import generate_VRI
import Utils

def main():
    n_instructions = load_parameters()
    table = []
    table = load_table(table)
    rand_idx = 0
    stream = []
    i = 0
    while(i < n_instructions):
        rand_idx = Utils.gen_random_integer(1, len(table) - 1)
        format = table[rand_idx][FORMAT_IDX]
        if(enabled_formats['E'] and format == 'E'):
            stream.append(generate_E(table, rand_idx))
        elif(enabled_formats['I'] and  format == 'I'):
            stream.append(generate_I(table, rand_idx))
        elif(enabled_formats['IE'] and format == 'IE'):
            stream.append(generate_IE(table, rand_idx))
        elif(enabled_formats['MII'] and format == 'MII'):
            stream.append(generate_MII(table, rand_idx))
        elif((enabled_formats['RIa'] and format == 'RIa') or \
             (enabled_formats['RIb'] and format == 'RIb') or \
             (enabled_formats['RIc'] and format == 'RIc')):
            stream.append(generate_RI(table, rand_idx))
        elif((enabled_formats['RIEa'] and format == 'RIEa') or \
             (enabled_formats['RIEb'] and format == 'RIEb') or \
             (enabled_formats['RIEc'] and format == 'RIEc') or \
             (enabled_formats['RIEd'] and format == 'RIEd') or \
             (enabled_formats['RIEe'] and format == 'RIEe') or \
             (enabled_formats['RIEf'] and format == 'RIEf') or \
             (enabled_formats['RIEg'] and format == 'RIEg')):
            stream.append(generate_RIE(table, rand_idx))
        elif((enabled_formats['RILa'] and format == 'RILa') or \
             (enabled_formats['RILb'] and format == 'RILb') or \
             (enabled_formats['RILc'] and format == 'RILc')):
            stream.append(generate_RIL(table, rand_idx))
        elif(enabled_formats['RIS'] and format == 'RIS'):
            stream.append(generate_RIS(table, rand_idx))
        elif(enabled_formats['RR'] and format == 'RR'):
            stream.append(generate_RR(table, rand_idx))
        elif(enabled_formats['RRD'] and format == 'RRD'):
            stream.append(generate_RRD(table, rand_idx))
        elif(enabled_formats['RRE'] and format == 'RRE'):
            stream.append(generate_RRE(table, rand_idx))
        elif((enabled_formats['RRFa'] and format == 'RRFa') or \
             (enabled_formats['RRFb'] and format == 'RRFb') or \
             (enabled_formats['RRFc'] and format == 'RRFc') or \
             (enabled_formats['RRFd'] and format == 'RRFd') or \
             (enabled_formats['RRFe'] and format == 'RRFe')):
            stream.append(generate_RRF(table, rand_idx))
        elif(enabled_formats['RRS'] and format == 'RRS'):
            stream.append(generate_RRS(table, rand_idx))
        elif((enabled_formats['RSa'] and format == 'RSa') or \
             (enabled_formats['RSb'] and format == 'RSb')):
            stream.append(generate_RS(table, rand_idx))
        elif(enabled_formats['RSI'] and format == 'RSI'):
            stream.append(generate_RSI(table, rand_idx))
        elif((enabled_formats['RSLa'] and format == 'RSLa') or \
             (enabled_formats['RSLb'] and format == 'RSLb')):
            stream.append(generate_RSL(table, rand_idx))
        elif((enabled_formats['RSYa'] and format == 'RSYa') or \
             (enabled_formats['RSYb'] and format == 'RSYb')):
            stream.append(generate_RSY(table, rand_idx))
        elif((enabled_formats['RXa'] and format == 'RXa') or \
             (enabled_formats['RXb'] and format == 'RXb')):
            stream.append(generate_RX(table, rand_idx))
        elif(enabled_formats['RXE'] and format == 'RXE'):
            stream.append(generate_RXE(table, rand_idx))
        elif(enabled_formats['RXF'] and format == 'RXF'):
            stream.append(generate_RXF(table, rand_idx))
        elif((enabled_formats['RXYa'] and format == 'RXYa') or \
             (enabled_formats['RXYb'] and format == 'RXYb')):
            stream.append(generate_RXY(table, rand_idx))
        elif(enabled_formats['S'] and format == 'S'):
            stream.append(generate_S(table, rand_idx))
        elif(enabled_formats['SI'] and format == 'SI'):
            stream.append(generate_SI(table, rand_idx))
        elif(enabled_formats['SIL'] and format == 'SIL'):
            stream.append(generate_SIL(table, rand_idx))
        elif(enabled_formats['SIY'] and format == 'SIY'):
            stream.append(generate_SIY(table, rand_idx))
        elif(enabled_formats['SMI'] and format == 'SMI'):
            stream.append(generate_SMI(table, rand_idx))
        elif((enabled_formats['SSa'] and format == 'SSa') or \
             (enabled_formats['SSb'] and format == 'SSb') or \
             (enabled_formats['SSc'] and format == 'SSc') or \
             (enabled_formats['SSd'] and format == 'SSd') or \
             (enabled_formats['SSe'] and format == 'SSe') or \
             (enabled_formats['SSf'] and format == 'SSf')):
            stream.append(generate_SS(table, rand_idx))
        elif(enabled_formats['SSE'] and format == 'SSE'):
            stream.append(generate_SSE(table, rand_idx))
        elif(enabled_formats['SSF'] and format == 'SSF'):
            stream.append(generate_SSF(table, rand_idx))
        elif((enabled_formats['VRIa'] and format == 'VRIa') or \
             (enabled_formats['VRIb'] and format == 'VRIb') or \
             (enabled_formats['VRIc'] and format == 'VRIc') or \
             (enabled_formats['VRId'] and format == 'VRId') or \
             (enabled_formats['VRIe'] and format == 'VRIe') or \
             (enabled_formats['VRIf'] and format == 'VRIf') or \
             (enabled_formats['VRIg'] and format == 'VRIg') or \
             (enabled_formats['VRIh'] and format == 'VRIh') or \
             (enabled_formats['VRIi'] and format == 'VRIi')):
            stream.append(generate_VRI(table, rand_idx))
        else:
            continue
        i += 1
   
    for instr in stream:
        print(instr) 
    return 0

if(__name__ == '__main__'):
    main()