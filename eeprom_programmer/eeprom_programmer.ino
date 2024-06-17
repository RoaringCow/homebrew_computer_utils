
// ----- shift register pins -----
#define address_SRCLK_pin 3// data shifting
#define address_SER_pin 2 // serial input

#define data_SRCLK_pin 7   // data shifting
#define data_SER_pin 8   // serial input

// common
#define output_enable_pin 4 // G
#define RCLK_pin 5         // put the contents to output register
#define SRCLR_pin 6        // clear the registers


#define ADDRESS_LEN 15
#define DATA_LEN 8
// --------------------------------

// EEPROM control pins
#define eeprom_write_enable 9
#define eeprom_output_enable 10
#define eeprom_chip_enable 11


byte data[8] = {0b01010101, 0b10101010, 0b11001100, 0b00110011, 0b11110000, 0b00001111, 0b10010010, 0b10000001}; 
void setup() {
  // set the pins
  pinMode(address_SRCLK_pin, OUTPUT);
  pinMode(address_SER_pin, OUTPUT);
  pinMode(data_SRCLK_pin, OUTPUT);
  pinMode(data_SER_pin, OUTPUT);
  pinMode(output_enable_pin, OUTPUT);
  pinMode(RCLK_pin, OUTPUT);
  pinMode(SRCLR_pin, OUTPUT);

  digitalWrite(output_enable_pin, LOW);
  digitalWrite(SRCLR_pin, HIGH);

  pinMode(eeprom_write_enable, OUTPUT);
  pinMode(eeprom_output_enable, OUTPUT);
  pinMode(eeprom_chip_enable, OUTPUT);

  digitalWrite(eeprom_chip_enable, LOW); // LOW enables, HIGH disables for these enable pins
  digitalWrite(eeprom_output_enable, HIGH); // disable on start
  digitalWrite(eeprom_write_enable, HIGH); // disable on start


  // read mode -> OE: LOW, CE: LOW, WE: HIGH 
  // write mode-> 

  Serial.begin(9600);

  
  pinMode(12, OUTPUT);

  delay(2000);
  
  unlock_eeprom();

  delay(1000);
  
  for (unsigned int x = 0; x <= 0x7FFF; x++) {
    write_byte(x, 0);
    delay(10);
    Serial.print(x);
    Serial.print("   ");
    Serial.print(x <= 0x7FFF);
    Serial.print("   ");
    Serial.println(0x7FFF);
    if (x == 0x7FFF) break;
  }

  digitalWrite(12, HIGH);
  
}

void loop() {
  /*
  load_data_into_pins(0b10101010);
  delay(1000);
  load_data_into_pins(0b01010101);
  delay(1000);
  */
}

void slow_write_array(uint16_t start_address, byte array[], int array_size) {
  for (int x = 0; x < array_size; x++) {
    write_byte(start_address + x, array[x]);
  }
}

void write_byte(uint16_t address, uint8_t data) {
  // load necesarry things into shift registers
  digitalWrite(RCLK_pin, LOW); // latch data to output
  load_address_into_pins(address);
  load_data_into_pins(data);
  digitalWrite(RCLK_pin, HIGH);

  // initiate write operation
  digitalWrite(eeprom_chip_enable, LOW);
  digitalWrite(eeprom_write_enable, LOW);
  digitalWrite(eeprom_output_enable, HIGH);

  delayMicroseconds(150);

  digitalWrite(eeprom_write_enable, HIGH);  // Disable write
}

void unlock_eeprom() {
  // holy shit it actually worked
  write_byte(0x5555, 0xAA);
  write_byte(0x2AAA, 0x55);
  write_byte(0x5555, 0x80);
  write_byte(0x5555, 0xAA);
  write_byte(0x2AAA, 0x55);
  write_byte(0x5555, 0x20);
  write_byte(0x5555, 0xAA);
  write_byte(0xFFFF, 0xAA);
}

void load_address_into_pins(uint16_t address) {
  shiftOut(address_SER_pin, address_SRCLK_pin, MSBFIRST, highByte(address));

  shiftOut(address_SER_pin, address_SRCLK_pin, MSBFIRST, lowByte(address));

  // moved RCLK pin pulse to write_byte
}

void load_data_into_pins(uint8_t value) {
  shiftOut(data_SER_pin, data_SRCLK_pin, MSBFIRST, value);
}
