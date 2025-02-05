from typing import List

import ConfigLoader as cl

from LoadTable import FORMAT_IDX
from LoadTable import MNEMONIC_IDX

available_instructions = []

pgm_parms = {
    'continue_on_error' : 0,
    'max_stream_size' : 0,
    'enable_instructions' : [],
    'disable_instructions' : [],
    'enabled_formats' : [],
    'disabled_formats' : []
}

def load_parameters(table : List[str]) -> int:
    config = cl.Config('config.cl')
    ret = validate_parms(config, table)
    pgm_parms['continue_on_error'] = config.get_value('continue_on_error')
    pgm_parms['max_stream_size'] = config.get_value('max_stream_size')
    pgm_parms['enable_formats'] = config.get_value('enable_formats')
    pgm_parms['disable_formats'] = config.get_value('disable_formats')
    pgm_parms['enable_instructions'] = config.get_value('enable_instructions')
    pgm_parms['disable_instructions'] = config.get_value('disable_instructions')
    if(ret != 0):
        return -1
    if(len(pgm_parms['enable_formats']) != 0):
        enable_formats_parms(table)
    elif(len(pgm_parms['disable_formats']) != 0):
        disable_formats_parms(table)
    elif(len(pgm_parms['enable_instructions']) != 0):
        enable_instructions_parms(table)
    elif(len(pgm_parms['disable_instructions']) != 0):
        disable_instructions_parms(table)
    else:
        for i, entry in enumerate(table):
            if(i == 0):
                continue
            available_instructions.append(entry)
    return 0

def validate_parms(config : cl.Config, table : List[str]) -> int:
    if((not config.key_exists('continue_on_error')) or \
       (not config.key_exists('max_stream_size')) or \
       (not config.key_exists('enable_instructions'))  or \
       (not config.key_exists('disable_instructions'))  or \
       (not config.key_exists('enable_formats'))  or \
       (not config.key_exists('disable_formats'))):
        return -1
    enable_instrs = config.get_value('enable_instructions')
    disable_instrs = config.get_value('disable_instructions')
    enable_fmts = config.get_value('enable_formats')
    disable_fmts = config.get_value('disable_formats')
    # Only one of the parm lists must be populated 
    if(len(enable_instrs) != 0 and len(disable_instrs) != 0 and \
       len(enable_fmts) != 0 and len(disable_fmts) != 0):
        return -1
    found = False
    if(len(enable_instrs) != 0):
        for instr in enable_instrs:
            for entry in table:
                if(instr == entry[MNEMONIC_IDX]):
                    found = True
                    break
            if(not found):
                return -1
            found = False
    if(len(disable_instrs) != 0):
        for instr in disable_instrs:
            for entry in table:
                if(instr == entry[MNEMONIC_IDX]):
                    found = True
                    break
            if(not found):
                return -1
            found = False
    if(len(enable_fmts) != 0):
        for instr in enable_fmts:
            for entry in table:
                if(instr == entry[FORMAT_IDX]):
                    found = True
                    break
            if(not found):
                return -1
            found = False
    if(len(disable_fmts) != 0):
        for instr in disable_fmts:
            for entry in table:
                if(instr == entry[FORMAT_IDX]):
                    found = True
                    break
            if(not found):
                return -1
            found = False
    return 0

def enable_formats_parms(table : List[str]) -> None:
    for entry in table:
        if(entry[FORMAT_IDX] in pgm_parms['enable_formats']):
            available_instructions.append(entry)

def disable_formats_parms(table : List[str]) -> None:
    for entry in table:
        if(entry[FORMAT_IDX] in pgm_parms['disable_formats']):
            continue
        available_instructions.append(entry)

def enable_instructions_parms(table : List[str]) -> None:
    for i, entry in enumerate(table):
        if(i == 0):
            continue
        if(entry[MNEMONIC_IDX] in pgm_parms['enable_instructions']):
            available_instructions.append(entry)

def disable_instructions_parms(table : List[str]) -> None:
    for i, entry in enumerate(table):
        if(i == 0):
            continue
        if(entry[MNEMONIC_IDX] in pgm_parms['disable_instructions']):
            continue
        available_instructions.append(entry)
    