import os
import subprocess

from ProcessParms import load_parameters
from ProcessParms import pgm_parms
from LoadTable import load_table
from LoadTable import mnemonic_to_table_index
from GenInstruction import generate_stream
from Utils import gen_random_integer

from LoadTable import ALTERNATIVE_INSTR_IDX

def main() -> int:
    table = load_table()
    ret = load_parameters(table)
    if(ret != 0):
        return -1
    pass_counter = 0
    error = False
    while(True):
        stream = generate_stream()
        with open('res/source.hlasm', 'w') as f:
            for instr in stream:
                f.write(instr + '\n')
        pgm_output = subprocess.run(['./zhlasmdis', '-a', 'res/source.hlasm', 'res/assembled.bin'], capture_output=True, text=True)
        pgm_output_str = pgm_output.stdout
        if(pgm_output_str.find('ERROR') != -1):
            print(f'!!!!! zhlasmdis -a -> ERROR IN PASS: {pass_counter} !!!!!')
            print(f'MESSAGE: {pgm_output_str}')
            break
        try_ascii_disassembler = gen_random_integer(0,100)
        if(try_ascii_disassembler < 50):
            conv_output = subprocess.run(['xxd', '-p', 'res/assembled.bin'], capture_output=True, text=True)
            conv_output_str = conv_output.stdout
            with open('res/assembled.bin', 'w') as f:
                f.write(conv_output_str)
            pgm_output = subprocess.run(['./zhlasmdis', '-dh', 'res/assembled.bin', 'res/disassembled.hlasm'], capture_output=True, text=True)
            pgm_output_str = pgm_output.stdout
        else:
            pgm_output = subprocess.run(['./zhlasmdis', '-d', 'res/assembled.bin', 'res/disassembled.hlasm'], capture_output=True, text=True)
            pgm_output_str = pgm_output.stdout
        if(pgm_output_str.find('ERROR') != -1):
            print(f'!!!!! zhlasmdis -d -> ERROR IN PASS: {pass_counter} !!!!!')
            print(f'MESSAGE: {pgm_output_str}')
            break
        with open('res/disassembled.hlasm', 'r') as f:
            for i, line in enumerate(f):
                stream_instr = stream[i].strip().replace('\n', '')
                file_instr = line.strip().replace('\n', '')
                if(stream_instr != file_instr):
                    stream_mnemonic = stream_instr[:stream_instr.find(' ')]
                    stream_operands = stream_instr[stream_instr.rfind(' '):]
                    file_mnemonic = file_instr[:file_instr.find(' ')]
                    file_operands = file_instr[file_instr.rfind(' '):]
                    table_idx = mnemonic_to_table_index(table, stream_mnemonic)
                    if(table[table_idx][ALTERNATIVE_INSTR_IDX] == file_mnemonic and 
                       stream_operands == file_operands):
                        continue
                    print(table[table_idx])
                    print(f'!!!!! MISMATCH -> ERROR IN PASS: {pass_counter} !!!!!')
                    print(f'{stream_instr} != {file_instr}')
                    error = True
                    break
            if(error and pgm_parms['continue_on_error'] == 0):
                break
        if(pass_counter % 100 == 0):
            print(f'PASS: {pass_counter}')
        pass_counter += 1
    return 0

if(__name__ == '__main__'):
    main()
