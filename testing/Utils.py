import sys
from random import randint

def gen_random_integer(low_range = 0, high_range=0) -> int:
    if(low_range == 0 and high_range == 0):
        return randint(0, sys.maxsize)
    return randint(low_range, high_range)

def gen_operand(high_range) -> str:
    n_chars = 0
    input_val = high_range
    while(input_val != 0):
        input_val = input_val >> 4
        n_chars += 1
    rand_val = hex(gen_random_integer(0x0, high_range)).replace('0x', '')
    out_operand = ['0' for i in range(0, n_chars)]
    for i in range(0, len(rand_val)):
        out_operand[n_chars - i - 1] = rand_val[n_chars - (n_chars - len(rand_val)) - i - 1]
    out_operand = ''.join(out_operand).upper()
    return out_operand