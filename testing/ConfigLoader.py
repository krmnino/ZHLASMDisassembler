import os.path
from enum import Enum
from typing import Any
from typing import List
from typing import Tuple

class CLErrorCodes(Enum):
    OK = 0
    SEMICOLON = 1
    EQUALS_SIGN = 2
    KEY_NOT_FOUND = 3
    ADD_KEY_REPEAT = 4
    FAILED_TO_OPEN = 5

class CLError(Exception):
    def __init__(self, err_code : CLErrorCodes, opt=[]):
        self.error_code = err_code
        if(self.error_code == CLErrorCodes.OK):
            self.message = ''
        elif(self.error_code == CLErrorCodes.SEMICOLON):
            self.message = f'Missing semicolon at \'{opt[0]}\' in configuration file.'
        elif(self.error_code == CLErrorCodes.EQUALS_SIGN):
            self.message = f'Missing equals sign at the key-value entry \'{opt[0]}\' in configuration file.'
        elif(self.error_code == CLErrorCodes.KEY_NOT_FOUND):
            self.message = f'The key \'{opt[0]}\' could not be found in configuration file.'
        elif(self.error_code == CLErrorCodes.ADD_KEY_REPEAT):
            self.message = f'The key \'{opt[0]}\' already exists in configuration file.'
        elif(self.error_code == CLErrorCodes.FAILED_TO_OPEN):
            self.message = f'Could not locate configuration file with the path \'{opt[0]}\'.'
        else:
            self.message = 'Undefinded error.'
        super().__init__(self.message)

class Config:        
    def __init__(self, path=''):
        # If path is not defined, then simply initialize a blank Config object
        if(path == ''):
            self.contents = {}
            self.entries = 0
        else:
            # Check if path provided to configuration file is valid 
            if(not os.path.isfile(path)):
                raise CLError(CLErrorCodes.FAILED_TO_OPEN, [path])
            # Initialize Config data members
            self.contents = {}
            self.entries = 0
            buffer = ''
            with open(path) as file:
                for line in file:
                    # Remove starting and trailing blanks in line and newline characters in line
                    line = line.strip()
                    line = line.replace('\n', '')
                    # Check if line contains a comment
                    find_start_comment = line.find('#')
                    # If line contains a comment, then remove it and remove any trailing spaces
                    if(find_start_comment != -1):
                        line = line[:find_start_comment]
                        line = line.strip()
                    # If line is blank, then skip it and move to the next one
                    if(len(line) == 0):
                        continue
                    # If the end on line contains a comma, then the value wraps to the next line
                    if(line[len(line) - 1] == ','):
                        buffer += line
                        continue
                    # Otherwise, if the line does not contain a semicolon which delimits key-value 
                    # pairs, then raise an exception
                    elif(line[len(line) - 1] != ';'):
                        raise CLError(CLErrorCodes.SEMICOLON, [line])
                    buffer += line
            # Remove any tabs in the buffer of key-value pairs, then split it by semicolons
            buffer = buffer.replace('\t', '')
            split_buffer = buffer.split(';')
            for entry in split_buffer:
                # If entry contains no key-value pair, then skip it and move to the next one
                if(len(entry) == 0):
                    continue
                key_val = entry.split('=')
                # If entry is not a valid key-value pair, then raise an exception
                if(len(key_val) != 2):
                    raise CLError(CLErrorCodes.EQUALS_SIGN, [entry])
                key = key_val[0].strip()
                value = self.__process_value(key_val[1])
                if(key in self.contents):
                    self.contents[key] = value
                else:    
                    self.contents[key] = value
                    self.entries += 1

    def __process_value(self, raw_string : str) -> Any:
        # Remove starting and trailing blanks in value
        raw_string = raw_string.strip()
        # If value contain square brackets at the beginning and end, then it is list
        if(raw_string[0] == '[' and raw_string[len(raw_string) - 1] == ']'):
            # Remove square brackets and split elements by commas
            raw_string = raw_string[1:len(raw_string)-1]
            split_list = raw_string.split(',')
            if(len(split_list) == 1 and split_list[0] == ''):
                return []
            for i in range(0, len(split_list)):
                split_list[i] = split_list[i].strip()
                split_list[i] = self.__convert_to_type(split_list[i])
            return split_list
        else:
            raw_string = raw_string.strip()
            return self.__convert_to_type(raw_string)

    def __convert_to_type(self, value : str) -> Any:
        if('.' in value):
            try:
                to_float = float(value)
                return to_float
            except:
                pass
        else:
            try:
                to_int = int(value)
                return to_int
            except:
                pass
        return value

    def get_n_entries(self) -> int:
        return self.entries

    def get_all_keys(self) -> List[str]:
        return [i for i in self.contents.keys()]

    def get_value(self, key : str) -> Any:
        if(key not in self.contents):
            raise CLError(CLErrorCodes.KEY_NOT_FOUND, [key])
        return self.contents[key]

    def get_all_key_values(self) -> List[Tuple[str, Any]]:
        key_vals = []
        keys = [i for i in self.contents.keys()]
        for i in range(0, self.entries):
            key_vals.append((keys[i], self.get_value(keys[i])))
        return key_vals

    def key_exists(self, key : str) -> bool:
        if(key not in self.contents):
            return False
        return True

    def add_entry(self, key : str, value : Any) -> None:
        if(key in self.contents):
            raise CLError(CLErrorCodes.ADD_KEY_REPEAT, [key])
        self.contents[key] = value
        self.entries += 1

    def edit_value(self, key : str, value : Any) -> None:
        if(key not in self.contents):
            raise CLError(CLErrorCodes.KEY_NOT_FOUND, [key])
        self.contents[key] = value

    def save_config(self, path) -> None:
        with open(path, 'w') as file:
            keys = [i for i in self.contents.keys()]
            for i in range(0, self.entries):
                file.write(keys[i] + ' = ')
                file.write(str(self.contents[keys[i]]) + ';\n')
