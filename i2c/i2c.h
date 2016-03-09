/**
  ******************************************************************************
  *	@file		i2c.h (ATmega16)
  * @author		Yohanes Erwin Setiawan
  * @version	1.0
  * @date		21 September 2015
  ******************************************************************************
  */

#ifndef I2C_H_
#define I2C_H_

/** Includes ---------------------------------------------------------------- */
#include <avr/io.h>

/** Defines ----------------------------------------------------------------- */
// Status codes in TWSR7:3 for master mode
// Prescaler bits are 0
// Start condition transmitted
#define MT_START					0x08
// Repeated start condition transmitted
#define MT_REP_START				0x10
// SLA+W transmitted, ACK received
#define MT_SLA_WRITE_ACK			0x18
// SLA+W transmitted, NACK received
#define MT_SLA_WRITE_NACK			0x20
// Data transmitted, ACK received 
#define MT_DATA_TRANSMITTED_ACK		0x28
// Data transmitted, NACK received 
#define MT_DATA_TRANSMITTED_NACK	0x30
// Arbitration lost in SLA+W or data
#define MT_ARB_LOST					0x38
// SLA+R transmitted, ACK received
#define MT_SLA_READ_ACK				0x40
// SLA+R transmitted, NACK received
#define MT_SLA_READ_NACK			0x48
// Data received, ACK returned
#define MT_DATA_RECEIVED_ACK		0x50
// Data received, NACK returned
#define MT_DATA_RECEIVED_NACK		0x58

// I2C address write (SLA+W)
#define I2C_WRITE		0
// I2C address read (SLA+R)
#define I2C_READ		1 

// I2C result
#define SUCCESS		0
#define ERROR		1

/** Public function prototypes ---------------------------------------------- */
void i2c_init(void);
uint8_t i2c_is_device_connected(uint8_t address);
uint8_t i2c_write_no_reg(uint8_t address, uint8_t data);
uint8_t i2c_write_with_reg(uint8_t address, uint8_t reg, uint8_t data);
uint8_t i2c_write_multi_no_reg(uint8_t address, uint8_t* data, uint8_t len);
uint8_t i2c_write_multi_with_reg(uint8_t address, uint8_t reg, uint8_t* data, uint8_t len);
uint8_t i2c_read_no_reg(uint8_t address, uint8_t* data);
uint8_t i2c_read_with_reg(uint8_t address, uint8_t reg, uint8_t* data);
uint8_t i2c_read_multi_no_reg(uint8_t address, uint8_t len, uint8_t* data);
uint8_t i2c_read_multi_with_reg(uint8_t address, uint8_t reg, uint8_t len, uint8_t* data);

#endif

/********************************* END OF FILE ********************************/
/******************************************************************************/