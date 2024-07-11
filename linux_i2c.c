#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

static int file_i2c = 0;
/*Explain
	- In Unix-like operating systems, hardware devices represented by as files, we can interact with hardware devices 
	by using same system calls as for regular files.
	- For I2C devices, the device file represents the I2C bus.By opening this file, you 're gaining access to the I2C bus,
	which in turn allows you to communicate with the I2C devices connected to that bus.
	- The I2C device file is usually located at /dev/i2c-<bus_number>. For example, /dev/i2c-1 represents the I2C bus 1.

	*/
uint8_t _i2c_init(int i2c, int dev_addr)
{
	if (file_i2c == 0)
	{
		char filename[32];
  		sprintf(filename, "/dev/i2c-%d", i2c); // I2C bus number passed
		file_i2c = open(filename, O_RDWR); // open the device file for I2C bus, allow read and write access to the bus
		
		//Check if the file was opened successfully
		if (file_i2c < 0)
		{
			file_i2c = 0;
			return 1; //means connection failed
		}
		//Specify the address of the slave device you want to communicate with
		if (ioctl(file_i2c, I2C_SLAVE, dev_addr) < 0) // set slave address
		{
			close(file_i2c);
			file_i2c = 0;
			return 1; //means connection failed
		}
		
		return 0; // if both open and ioctl calls are successful, connection is established
	}
	// else connection already established/files already opened
	// assume done init already
	return 0;
}

uint8_t _i2c_close()
{
	if (file_i2c != 0)
	{
		close(file_i2c);
		// reset file_i2c to 0 to indicate that the file is closed
		file_i2c = 0;
		return 0;
	}
	
	return 1; // case the file was not opened
}
//ptr is the pointer to the data to be written, len is the length of the data to be written
uint8_t _i2c_write(uint8_t* ptr, int16_t len)
{
	if (file_i2c == 0 || ptr == 0 || len <= 0)
		return 1; //failed to write
				
	write(file_i2c, ptr, len);
	
	return 0;
}

//ptr is the pointer to the data to be read, len is the length of the data to be read
uint8_t _i2c_read(uint8_t *ptr, int16_t len)
{
	if (file_i2c == 0 || ptr == 0 || len <= 0) // file closed or ptr is null or len is negative
		return 1; //failed to read
				
	read(file_i2c, ptr, len);

	return 0;
}
