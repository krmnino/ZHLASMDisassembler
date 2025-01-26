import subprocess

from ProcessParms import load_parameters
from LoadTable import load_table
from GenInstruction import generate_stream

def main() -> int:
    table = load_table()
    ret = load_parameters(table)
    if(ret != 0):
        return -1
    stream = generate_stream()
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
            print(f'!!!!! ERROR IN PASS: {i} !!!!!')
            print(f'MESSAGE: {pgm_output_str}')
            break
        i += 1
    return 0

if(__name__ == '__main__'):
    main()