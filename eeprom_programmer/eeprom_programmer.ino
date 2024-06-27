#define SHIFT_DATA 2
#define SHIFT_CLK 4
#define SHIFT_LATCH 3
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

// author: Ben Eater (thanks)
// i just outsourced it because mine wasnt stable
// i also did some customizations but the foundation is made by Ben Eater


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
void printContents(int lines_to_print) {
  for (int base = 0x0; base <= lines_to_print; base += 16) {
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
void check_reset() {
  for (uint32_t x = 0; x < 0x8000; x++) {
    if (readEEPROM(x) != 0) {
      Serial.print("oh shit not good at:  ");
      Serial.println(x, HEX);
    }
  }

  Serial.println("all done on reset check");
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


byte data[] = {0x9, 0x55, 0xa, 0x11};
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
    writeEEPROM(address, 0x00);

    if (address % 0x8F == 0) {
      Serial.print("%");
      Serial.println((address / 32768.0) * 100);
    }
  }
  Serial.println(" done");

  check_reset();
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

  // Read and print out the contents of the EERPROM
  Serial.println("Reading EEPROM");
  printContents();
*/
  write_microcodes(true);


}

uint16_t a = 0;
void loop() {
  /*
  if (Serial.available()) {
    byte receivedData = Serial.read();
    writeEEPROM(a++, receivedData);
    
    // Send back the received byte for confirmation
    Serial.write(receivedData);
  }
  */
}



void write_microcodes(bool mode){

  // set fetcher
  Serial.println("Setting Fetcher");
  for (int x = 0; x < 0x100; x++) {
    writeEEPROM(x, mode ? 0x18: 0x0);
    writeEEPROM(0b100000000 | x, mode ? 0x80: 0x0);
  }


  int instr0[] = {0, 0, 0, 0, 0, 0, 0, 0}; // halt
  int instr1[] = {0x1800, 0x8000, 0x00c0, 0, 0, 0, 0, 0}; //ld A, B    0x03
  int instr2[] = {0x1800, 0x8000, 0x0110, 0, 0, 0, 0, 0}; //ld B, A    0x04
  int instr3[] = {0x1800, 0x8000, 0x0c00, 0x2040, 0x8000, 0, 0, 0}; //ld A, a8   0x05
  int instr4[] = {0x1800, 0x8000, 0x0c00, 0x2010, 0x8000, 0, 0, 0}; //ld B, a8   0x06
  int instr5[] = {0x1800, 0x8000, 0x0c00, 0x4100, 0x8000, 0, 0, 0}; //ld a8, A   0x07
  int instr6[] = {0x1800, 0x8000, 0x0c00, 0x4080, 0x8000, 0, 0, 0}; //ld a8, B   0x08
  int instr7[] = {0x1800, 0x8000, 0x0840, 0x8000, 0, 0, 0, 0}; //ld A, d8   0x09
  int instr8[] = {0x1800, 0x8000, 0x0810, 0x8000, 0, 0, 0, 0}; //ld B, d8   0x0A
  int instr9[] = {0x1800, 0x8000, 0x0840, 0x8000, 0x0c00, 0x4100, 0, 0}; //ld a8, d8  0x10
  int instr10[] = {0x1800, 0x8000, 0x0240, 0, 0, 0, 0, 0}; //Add A, B  0x16
  int instr11[] = {0x1800, 0x8000, 0x0260, 0, 0, 0, 0, 0}; //Sub A, B  0x17

  int instruction_count = 0x18;
  int* instruction_list[instruction_count] = {NULL, instr0, NULL, instr1, instr2, instr3, instr4, instr5, instr6, instr7, instr8, NULL, NULL, NULL, NULL, NULL, instr9, NULL, NULL, NULL, NULL, NULL, instr10, instr11};

  for (int x = 0; x < instruction_count; x++) {
    if (instruction_list[x] == NULL) {
      continue;
    }
    if (mode) { // high byte eeprom
      for (int i = 0; i < 8; i++) {
        int data = instruction_list[x][i];
        /*
        Serial.print((i << 8) | x, HEX);
        Serial.print(":  ");
        Serial.println((data >> 8) & 0xFF, HEX);
        */
        writeEEPROM((i << 8) | x, (data >> 8) & 0xFF);
      }
    }else {     // low byte eeprom
      for (int i = 0; i < 8; i++) {
        int data = instruction_list[x][i];
        /*
        Serial.print((i << 8) | x, HEX);
        Serial.print(":  ");
        Serial.println(data & 0XFF, HEX);
        */
        writeEEPROM((i << 8) | x, data & 0xFF);
      }
    }
  }
  Serial.println("Microcode writing done");
  printContents(0x2f);
}

