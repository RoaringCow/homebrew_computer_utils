def write_data(file_address: str, data: list):
    with open(file_address, "wb") as file:
        file.write(bytes(data))

def read_data(file_address: str):
    with open(file_address, "rb") as file:
        data = file.read()
        for byte in data:
            print(hex(byte))


def main():
    #write_microcode("microcode_data.bin")
    #read_data("microcode_data.bin")
    write_data("test.bin", [0b00010111, 0b11001100])


def write_microcode(file_address: str):
    microcode_data = [0] * 0x8000
    instr0 = [0, 0, 0, 0, 0, 0, 0, 0] # halt
    instr1 = [0x1800, 0x8000, 0x00c0, 0, 0, 0, 0, 0] #ld A, B    0x03
    instr2 = [0x1800, 0x8000, 0x0110, 0, 0, 0, 0, 0] #ld B, A    0x04
    instr3 = [0x1800, 0x8000, 0x0c00, 0x2040, 0x8000, 0, 0, 0] #ld A, a8   0x05
    instr4 = [0x1800, 0x8000, 0x0c00, 0x2010, 0x8000, 0, 0, 0] #ld B, a8   0x06
    instr5 = [0x1800, 0x8000, 0x0c00, 0x4100, 0x8000, 0, 0, 0] #ld a8, A   0x07
    instr6 = [0x1800, 0x8000, 0x0c00, 0x4080, 0x8000, 0, 0 ,0] #ld a8, B   0x08
    instr7 = [0x1800, 0x8000, 0x0840, 0x8000, 0, 0, 0, 0] #ld A, d8   0x09
    instr8 = [0x1800, 0x8000, 0x0810, 0x8000, 0, 0, 0, 0] #ld B, d8   0x0A
    instr9 = [0x1800, 0x8000, 0x0840, 0x8000, 0x0c00, 0x4100, 0, 0] #ld a8, d8  0x10
    instr10 = [0x1800, 0x8000, 0x0240, 0, 0, 0, 0, 0] #Add A, B  0x16
    instr11 = [0x1800, 0x8000, 0x0260, 0, 0, 0, 0, 0] #Sub A, B  0x17


    for x in range(0, 0x1000):
        microcode_data[x * 8] = 0x1800
        microcode_data[x * 8 + 1] = 0x8000

    # didnt bother to find any other way
    for x in range(0, 8):
        microcode_data[x * 256] = instr0[x];
    for x in range(0, 8):
        microcode_data[0x03 + x * 256] = instr1[x];
    for x in range(0, 8):
        microcode_data[0x04 + x * 256] = instr2[x];
    for x in range(0, 8):
        microcode_data[0x05 + x * 256] = instr3[x];
    for x in range(0, 8):
        microcode_data[0x06 + x * 256] = instr4[x];
    for x in range(0, 8):
        microcode_data[0x07 + x * 256] = instr5[x];
    for x in range(0, 8):
        microcode_data[0x08 + x * 256] = instr6[x];
    for x in range(0, 8):
        microcode_data[0x09 + x * 256] = instr7[x];
    for x in range(0, 8):
        microcode_data[0x0A + x * 256] = instr8[x];
    for x in range(0, 8):
        microcode_data[0x10 + x * 256] = instr9[x];
    for x in range(0, 8):
        microcode_data[0x16 + x * 256] = instr10[x];
    for x in range(0, 8):
        microcode_data[0x17 + x * 256] = instr11[x];


    with open(file_address, "wb") as file:
        file.write(bytes(seperate_to_bytes(microcode_data)))


# honestly too lazy to change the lists
def seperate_to_bytes(data):
    seperated_data = []
    for value in data:
        seperated_data.append(value & 0xFF)
        seperated_data.append((value >> 8) & 0xFF)
    while len(seperated_data) < 16:
        seperated_data.append(0)
    return seperated_data




if __name__ == "__main__":
    main()