void i2c_write_byte(unsigned char b, bit is_last);
unsigned char i2c_read_byte(bit is_last);
void dac_write(unsigned char addr, unsigned short value);
void dac_sync();
