import subprocess

from ProcessParms import max_stream_size
from ProcessParms import load_parameters
from LoadTable import load_table
from GenInstruction import generate_stream

def main():
    load_parameters()
    table = []
    table = load_table(table)
    stream = generate_stream(table, max_stream_size)
    i = 0
    while(True):
        with open('res/source.hlasm', 'w') as f:
            for instr in stream:
                f.write(instr + '\n')
        pgm_output = subprocess.run(['./zhlasmdis', '-a', 'res/source.hlasm', 'res/disassemble.bin'])
        pgm_output = subprocess.run(['./zhlasmdis', '-d', 'res/disassemble.bin', 'res/assemble.hlasm'], capture_output=True, text=True)
        pgm_output_str = pgm_output.stdout
        if(i % 100 == 0):
            print(f'PASS: {i}')
        if(pgm_output_str.find('ERROR') != -1):
            print(f'PASS: {i} -> {pgm_output_str}')
            break
        i += 1
    return 0

if(__name__ == '__main__'):
    main()