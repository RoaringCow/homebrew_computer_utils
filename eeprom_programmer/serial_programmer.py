import serial
import time

def lower_bytes(data: list) -> list:
    lower_data = [0] * 0x8000
    j = 0
    for x in range(1, len(data), 2):
        lower_data[j] = data[x]
        j += 1
    return lower_data

def higher_bytes(data: list) -> list:
    high_data = [0] * 0x8000
    j = 0
    for x in range(1, len(data), 2):
        high_data[j] = data[x]
        j += 1
    return high_data
# Open serial port
ser = serial.Serial('/dev/cu.usbserial-210', 9600, timeout=1)  # Adjust the port name and baud rate as necessary
time.sleep(3)

# false -> lower byte (so right eeprom), true -> high byte (so left eeprom)
which_eeprom = True
data_to_send = "microcode_data.bin"
with open(data_to_send, "rb") as file:
    data = file.read()
    if which_eeprom:
        ser.write(bytes(higher_bytes(data)))
    else:
        ser.write(bytes(lower_bytes(data)))
        
# Close the serial port
ser.close()