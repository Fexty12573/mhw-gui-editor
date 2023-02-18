import json
import re
import sys

def read_params(output, obj_name: str, lines: list, line_index: int):
    obj_data = {
        'Name': obj_name,
        'Fields': []
    }

    while line_index < len(lines):
        line = lines[line_index]
        line_index += 1

        if line.startswith('};'):
            output[obj_name] = obj_data
            return line_index

        if line.startswith('    '):
            match = re.match(r'([^ \t]+) \'([^ ]+)\'(\[[0-9\*]+\])? *; \/\/ Offset:(0x[0-9A-F]+)', line)
            if match:
                param_type = match.group(1)
                param_name = match.group(2)
                param_offset = match.group(4)
                if int(param_offset, base=16) == 0x7FFFFFFFFFFFFFFF:
                    obj_data['Fields'].append({
                        'Name': param_name,
                        'Type': param_type
                    })
            else:
                print('Error: Could not parse line: ' + line)

    output[obj_name] = obj_data
    return line_index

def get_gui_object_params(filename: str):
    with open(filename, 'r') as f:
        lines = f.readlines()

    output = {}

    for i in range(len(lines)):
        line = lines[i]
        if line.startswith('class '):
            match = re.match(r'class ([^ ]+) \/\*', line)
            if match:
                name = match.group(1)
                if 'cGUIObject' in line:
                    i = read_params(output, name, lines, i + 1)

    return output

def main():
    if len(sys.argv) < 2:
        print('Usage: python gui_object_param_getter.py DtiDump')
        return

    output = get_gui_object_params(sys.argv[1])
    with open('gui_object_params.json', 'w') as f:
        json.dump(output, f, indent=4)

if __name__ == '__main__':
    main()
