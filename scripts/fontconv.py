#!/usr/bin/python3

from sys import argv

if __name__ == "__main__":
    f = open(argv[1])

    what_read = "common-header"

    char_width, char_height, read_row = 0, 0, 0
    active_char = None
    char_data = [None] * 128
    for line in f:
        line = line.strip()

        if what_read == "common-header":
            char_width_s, char_height_s = line.split()
            char_width = int(char_width_s)
            char_height = int(char_height_s)
            print(char_width, char_height)
            what_read = "char-header"

            for ix in range(0, 128):
                char_data[ix] = [0] * char_width

        elif what_read == "char-header":
            if line[0] == "#":
                if len(line) > 1 and line[1] == "#":
                    active_char = '#'
                else:
                    active_char = chr(int(line[1:], 16))
            else:
                active_char = line[0]

            what_read = "char-data"
            read_row = 0
            print(active_char)

        elif what_read == "char-data":
            print(read_row, line)
            for column in range(0, char_width):
                if line[column] in ['X', 'x']:
                    char_data[ord(active_char)][column] |= (1 << read_row)

            read_row += 1
            if read_row == char_height: 
                what_read = "char-header"

    result = "const uint8_t fontData[] = {%d, %d, \n" % (char_width, char_height)

    for char_ix, char_data in enumerate(char_data):
        result += "    "
        for c in char_data: result += "0x%02x, " % c
        if char_ix >= 32 and char_ix != 127:
            result += " // 0x%02x / %c\n" % (char_ix, chr(char_ix))
        else:
            result += " // 0x%02x\n" % char_ix

    result += "};"
    print(result)







    f.close()
