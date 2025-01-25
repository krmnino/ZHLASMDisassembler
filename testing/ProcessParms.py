from typing import List

import ConfigLoader as cl

pgm_parms = {
    'max_stream_size' : 0
}

enabled_formats = {
    'E'     : True,
    'I'     : True,
    'IE'    : True,
    'MII'   : True,
    'RIa'   : True, 
    'RIb'   : True, 
    'RIc'   : True,
    'RIEa'  : True, 
    'RIEb'  : True, 
    'RIEc'  : True, 
    'RIEd'  : True, 
    'RIEe'  : True, 
    'RIEf'  : True, 
    'RIEg'  : True,
    'RILa'  : True, 
    'RILb'  : True, 
    'RILc'  : True,
    'RIS'   : True,
    'RR'    : True,
    'RRD'   : True,
    'RRE'   : True,
    'RRFa'  : True, 
    'RRFb'  : True, 
    'RRFc'  : True, 
    'RRFd'  : True, 
    'RRFe'  : True,
    'RRS'   : True,
    'RSa'   : True, 
    'RSb'   : True,
    'RSI'   : True,
    'RSLa'  : True, 
    'RSLb'  : True,
    'RSYa'  : True, 
    'RSYb'  : True, 
    'RXa'   : True, 
    'RXb'   : True, 
    'RXE'   : True,
    'RXF'   : True,
    'RXYa'  : True,  
    'RXYb'  : True,
    'S'     : True,
    'SI'    : True,
    'SIL'   : True,
    'SIY'   : True,
    'SMI'   : True,
    'SSa'   : True, 
    'SSb'   : True, 
    'SSc'   : True, 
    'SSd'   : True, 
    'SSe'   : True, 
    'SSf'   : True,
    'SSE'   : True,
    'SSF'   : True,
    'VRIa'  : True, 
    'VRIb'  : True, 
    'VRIc'  : True, 
    'VRId'  : True, 
    'VRIe'  : True, 
    'VRIf'  : True, 
    'VRIg'  : True, 
    'VRIh'  : True, 
    'VRIi'  : True,
    'VRRa'  : True, 
    'VRRb'  : True, 
    'VRRc'  : True, 
    'VRRd'  : True, 
    'VRRe'  : True, 
    'VRRf'  : True, 
    'VRRg'  : True, 
    'VRRh'  : True, 
    'VRRi'  : True, 
    'VRRj'  : True, 
    'VRRk'  : True, 
    'VRSa'  : True, 
    'VRSb'  : True, 
    'VRSc'  : True, 
    'VRSd'  : True, 
    'VRV'   : True,
    'VRX'   : True,
    'VSI'   : True, 
}

def load_parameters() -> int:
    config = cl.Config('config.cl')
    ret = validate_parms(config)
    if(ret != 0):
        return -1
    pgm_parms['max_stream_size'] = config.get_value('max_stream_size')
    enable_fmts = config.get_value('enable_formats')
    disable_fmts = config.get_value('disable_formats')
    if(len(enable_fmts) != 0):
        enable_formats_parms(enable_fmts)
    if(len(disable_fmts) != 0):
        disable_formats_parms(disable_fmts)
    return 0

def validate_parms(config : cl.Config) -> int:
    if((not config.key_exists('max_stream_size')) or \
       (not config.key_exists('enable_instructions'))  or \
       (not config.key_exists('disable_instructions'))  or \
       (not config.key_exists('enable_formats'))  or \
       (not config.key_exists('disable_formats'))):
        return -1
    enable_fmts = config.get_value('enable_formats')
    disable_fmts = config.get_value('disable_formats')
    if(len(enable_fmts) != 0 and len(disable_fmts) != 0):
        return -1
    if(len(enable_fmts) != 0):
        for e_key in enable_fmts:
            if(e_key not in enabled_formats.keys()):
                return -1
    if(len(disable_fmts) != 0):
        for d_key in disable_fmts:
            if(d_key not in enabled_formats.keys()):
                return -1
    if(len(enable_fmts) != 0 and len(disable_fmts) != 0):
        return -1
    return 0

def enable_formats_parms(in_enable_fmts : List[str]) -> None:
    for key in enabled_formats.keys():
        if(key in in_enable_fmts):
            continue
        enabled_formats[key] = False

def disable_formats_parms(in_disable_fmts : List[str]) -> None:
    for key in enabled_formats.keys():
        if(key in in_disable_fmts):
            enabled_formats[key] = False