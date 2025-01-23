import ConfigLoader as cl

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
    stream_size = config.get_value('stream_size')
    enable = config.get_value('enable_formats')
    disable = config.get_value('disable_formats')
    if(len(enable) != 0 and len(disable) != 0):
        # ERROR
        return -1
    if(len(enable) != 0):
        for e_key in enable:
            if(e_key not in enabled_formats.keys()):
                # ERROR
                return -1
        enable_formats_parms(enable)
        return stream_size
    if(len(disable) != 0):
        for d_key in disable:
            if(d_key not in enabled_formats.keys()):
                # ERROR
                return -1
        disable_formats_parms(disable)
        return stream_size
    return stream_size

def enable_formats_parms(enable) -> None:
    for key in enabled_formats.keys():
        if(key in enable):
            continue
        enabled_formats[key] = False

def disable_formats_parms(disable) -> None:
    for key in enabled_formats.keys():
        if(key in disable):
            enabled_formats[key] = False