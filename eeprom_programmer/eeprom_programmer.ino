#define SHIFT_DATA 2
#define SHIFT_CLK 4
#define SHIFT_LATCH 3
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

// author: Ben Eater (thanks)
// i just outsourced it because mine wasnt stable

/*
 * Output the address bits and outputEnable signal using shift registers.
 */
void setAddress(int address, bool outputEnable) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);

  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}


#define CE 0b1000000000000000 // Counter enable
#define RI 0b0100000000000000 // RAM in
#define RO 0b0010000000000000 // RAM out
#define II 0b0001000000000000 // Instruction register in
#define EO 0b0000100000000000 // EEPROM OUT
#define AddrI 0b0000010000000000 // Address in
#define SO 0b0000001000000000 // sum out
#define AO 0b0000000100000000 // A register in
//---------------------------
#define BO 0b0000000010000000 // B register out
#define AI 0b0000000001000000 // A register in
#define SF 0b0000000000100000 // subtract flag
#define BI 0b0000000000010000 // B register in
/*
 * Read a byte from the EEPROM at the specified address.
 */
byte readEEPROM(int address) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, INPUT);
  }
  setAddress(address, /*outputEnable*/ true);

  byte data = 0;
  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  delay(1);
  return data;
}


/*
 * Write a byte to the EEPROM at the specified address.
 */
void writeEEPROM(int address, byte data) {
  setAddress(address, /*outputEnable*/ false);
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, OUTPUT);
  }

  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }
  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_EN, HIGH);
  delay(10);
}


/*
 * Read the contents of the EEPROM and print them to the serial monitor.
 */
void printContents() {
  for (int base = 0; base <= 0x2FF; base += 16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}

void unlock_eeprom() {
  writeEEPROM(0x5555, 0xAA);
  writeEEPROM(0x2AAA, 0x55);
  writeEEPROM(0x5555, 0x80);
  writeEEPROM(0x5555, 0xAA);
  writeEEPROM(0x2AAA, 0x55);
  writeEEPROM(0x5555, 0x20);
  writeEEPROM(0x5555, 0xAA);
  writeEEPROM(0xFFFF, 0xAA);

}


void setup() {
  // put your setup code here, to run once:
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  //unlock_eeprom();
  Serial.begin(9600);  // Start serial communication at 9600 baud rate
    while (!Serial) {
        ; // Wait for the serial port to connect. Needed for native USB
    }

  // Erase entire EEPROM
  Serial.println("Erasing EEPROM");
  for (uint32_t address = 0; address <= 0x7FFF; address += 1) {
    writeEEPROM(address, 0x0);

    if (address % 0x8F == 0) {
      Serial.print("%");
      Serial.println((address / 32768.0) * 100);
    }
  }
  Serial.println(" done");
/*

  // Program data bytes
  Serial.print("Programming EEPROM");
  for (int address = 0; address < sizeof(data); address += 1) {
    writeEEPROM(address, data[address]);

    if (address % 0x8F == 0) {
      Serial.print("%");
      Serial.println((address / 32768.0) * 100);
    }
  }
  Serial.println(" done");

*/
  // Read and print out the contents of the EERPROM
  Serial.println("Reading EEPROM");
  printContents();


}

int a = 0;
void loop() {
  /*
  if (Serial.available()) {
    byte receivedData = Serial.read();
    writeEEPROM(a++, receivedData);
  }
  */
}