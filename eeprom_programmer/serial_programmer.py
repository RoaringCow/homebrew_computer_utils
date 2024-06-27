import serial
import time


# Open serial port
ser = serial.Serial('/dev/cu.usbserial-210', 9600)
time.sleep(3)

# microcodes
data_to_send = "microcode_data.bin"
# program
#data_to_send = "../assembler/output.bin"

data = []
with open(data_to_send, "rb") as file:
    data = file.read()


total_bytes = len(data)
print(f"Total bytes to send: {total_bytes}")

for i, byte in enumerate(data):
    ser.write(bytes([byte]))
    print(f"\nSent byte {i+1}/{total_bytes}: {hex(byte)}")
    time.sleep(0.01)

    # Read response from the receiver
    while ser.in_waiting == 0:
        pass  # Wait for the response

    received_byte = ser.read(1)
    if received_byte != bytes([byte]):
        print(f"Mismatch at byte {i+1}: Sent {hex(byte)}, Received {hex(received_byte)}")
        raise Exception("mismatch")
    else:
        print(f"Byte {i+1} confirmed: {received_byte.hex()}")

ser.close()