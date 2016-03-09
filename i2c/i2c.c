/**
  ******************************************************************************
  *	@file		i2c.c (ATmega16)
  * @author		Yohanes Erwin Setiawan
  * @version	1.0
  * @date		21 September 2015
  ******************************************************************************
  */

/** Includes ---------------------------------------------------------------- */
#include "i2c.h"

/** Private function prototypes --------------------------------------------- */
void i2c_wait_until_finish(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_transmit(uint8_t byte);
uint8_t i2c_receive_ack(void);
uint8_t i2c_receive_nack(void);
uint8_t i2c_check_status(uint8_t status_code);

/** Public functions -------------------------------------------------------- */
/**
  ******************************************************************************
  *	@brief	Initialize I2C in master mode (100kHz clock)
  * @param	None
  * @retval	None
  ******************************************************************************
  */
void i2c_init()
{
	// Set the SCL frequency by using this equation
	// SCL frequency = F_CPU / (16 + (2 * TWBR * (4 ^ TWPS)))
	// Here we set the SCL frequency to 100kHz with F_CPU = 16MHz
	// 16MHz / (16 + (2 * 18 * (4 ^ 1))) = 100kHz 
	TWBR = 18;
	
	// TWPS1 and TWPS0 in TWSR = 0 means that prescaler is 1
	TWSR = 0;
	
	// Enable TWI module
	TWCR |= (1 << TWEN);
}

/**
  ******************************************************************************
  *	@brief	Check if I2C slave device is connected or not
  * @param	Slave device address
  * @retval	Connection status (SUCCESS or ERROR)
  ******************************************************************************
  */
uint8_t i2c_is_device_connected(uint8_t address)
{
	i2c_start();
	if (i2c_check_status(MT_START) == ERROR)
		return ERROR;
		
	// Check device is connected or not
	// If connected, device will send ACK after
	// master transmit SLA+W or SLA+R
	i2c_transmit((address << 1) | I2C_WRITE);
	if (i2c_check_status(MT_SLA_WRITE_ACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}	
	
	i2c_stop();

	return SUCCESS;	
}

/**
  ******************************************************************************
  *	@brief	Write byte to slave without specify register address
  * @param	Slave device address (7-bit right aligned)
  * @param	Data byte
  * @retval	Result status (SUCCESS or ERROR)
  ******************************************************************************
  */
uint8_t i2c_write_no_reg(uint8_t address, uint8_t data)
{
	i2c_start();
	if (i2c_check_status(MT_START) == ERROR) 
		return ERROR;

	i2c_transmit((address << 1) | I2C_WRITE);
	if (i2c_check_status(MT_SLA_WRITE_ACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}

	i2c_transmit(data);
	if (i2c_check_status(MT_DATA_TRANSMITTED_ACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}

	i2c_stop();

	return SUCCESS;
}

/**
  ******************************************************************************
  *	@brief	Write byte to slave with specify register address
  * @param	Slave device address (7-bit right aligned)
  * @param	Register address
  * @param	Data byte
  * @retval	Result status (SUCCESS or ERROR)
  ******************************************************************************
  */
uint8_t i2c_write_with_reg(uint8_t address, uint8_t reg, uint8_t data)
{
	i2c_start();
	if (i2c_check_status(MT_START) == ERROR)
		return ERROR;

	i2c_transmit((address << 1) | I2C_WRITE);
	if (i2c_check_status(MT_SLA_WRITE_ACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}

	i2c_transmit(reg);
	if (i2c_check_status(MT_DATA_TRANSMITTED_ACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}

	i2c_transmit(data);
	if (i2c_check_status(MT_DATA_TRANSMITTED_ACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}

	i2c_stop();

	return SUCCESS;
}

/**
  ******************************************************************************
  *	@brief	Write bytes to slave without specify register address where to
  *			start write
  * @param	Slave device address (7-bit right aligned)
  * @param	Pointer to data byte array
  * @param	Number of bytes to write
  * @retval	Result status (SUCCESS or ERROR)
  ******************************************************************************
  */
uint8_t i2c_write_multi_no_reg(uint8_t address, uint8_t* data, uint8_t len)
{
	i2c_start();
	if (i2c_check_status(MT_START) == ERROR)
		return ERROR;

	i2c_transmit((address << 1) | I2C_WRITE);
	if (i2c_check_status(MT_SLA_WRITE_ACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}

	for (int i = 0; i < len; i++)
	{
		i2c_transmit(data[i]);
		if (i2c_check_status(MT_DATA_TRANSMITTED_ACK) == ERROR)
		{
			i2c_stop();
			return ERROR;
		}	
	}

	i2c_stop();

	return SUCCESS;
}

/**
  ******************************************************************************
  *	@brief	Write bytes to slave with specify register address where to
  *			start write
  * @param	Slave device address (7-bit right aligned)
  * @param	Register address where to start write
  * @param	Pointer to data byte array
  * @param	Number of bytes to write
  * @retval	Result status (SUCCESS or ERROR)
  ******************************************************************************
  */
uint8_t i2c_write_multi_with_reg(uint8_t address, uint8_t reg, uint8_t* data, uint8_t len)
{
	i2c_start();
	if (i2c_check_status(MT_START) == ERROR)
		return ERROR;

	i2c_transmit((address << 1) | I2C_WRITE);
	if (i2c_check_status(MT_SLA_WRITE_ACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}

	i2c_transmit(reg);
	if (i2c_check_status(MT_DATA_TRANSMITTED_ACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}

	for (int i = 0; i < len; i++)
	{
		i2c_transmit(data[i]);
		if (i2c_check_status(MT_DATA_TRANSMITTED_ACK) == ERROR)
		{
			i2c_stop();
			return ERROR;
		}
	}

	i2c_stop();

	return SUCCESS;
}

/**
  ******************************************************************************
  *	@brief	Read byte from slave without specify register address
  * @param	Slave device address (7-bit right aligned)
  * @param	Pointer to data byte to store data from slave
  * @retval	Result status (SUCCESS or ERROR)
  ******************************************************************************
  */
uint8_t i2c_read_no_reg(uint8_t address, uint8_t* data)
{
	i2c_start();
	if (i2c_check_status(MT_START) == ERROR)
		return ERROR;
	
	i2c_transmit((address << 1) | I2C_READ);
	if (i2c_check_status(MT_SLA_READ_ACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}

	*data = i2c_receive_nack(); 
	if (i2c_check_status(MT_DATA_RECEIVED_NACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}

	i2c_stop();

	return SUCCESS;
}

/**
  ******************************************************************************
  *	@brief	Read byte from slave with specify register address
  * @param	Slave device address (7-bit right aligned)
  * @param	Register address
  * @param	Pointer to data byte to store data from slave
  * @retval	Result status (SUCCESS or ERROR)
  ******************************************************************************
  */
uint8_t i2c_read_with_reg(uint8_t address, uint8_t reg, uint8_t* data)
{
	i2c_start();
	if (i2c_check_status(MT_START) == ERROR)
		return ERROR;

	i2c_transmit((address << 1) | I2C_WRITE);
	if (i2c_check_status(MT_SLA_WRITE_ACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}
	
	i2c_transmit(reg);
	if (i2c_check_status(MT_DATA_TRANSMITTED_ACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}
	
	i2c_start();
	if (i2c_check_status(MT_REP_START) == ERROR)
		return ERROR;

	i2c_transmit((address << 1) | I2C_READ);
	if (i2c_check_status(MT_SLA_READ_ACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}

	*data = i2c_receive_nack();
	if (i2c_check_status(MT_DATA_RECEIVED_NACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}

	i2c_stop();

	return SUCCESS;
}

/**
  ******************************************************************************
  *	@brief	Read bytes from slave without specify register address
  * @param	Slave device address (7-bit right aligned)
  * @param	Number of data bytes to read from slave 
  * @param	Pointer to data array byte to store data from slave
  * @retval	Result status (SUCCESS or ERROR)
  ******************************************************************************
  */
uint8_t i2c_read_multi_no_reg(uint8_t address, uint8_t len, uint8_t* data)
{
	i2c_start();
	if (i2c_check_status(MT_START) == ERROR)
		return ERROR;

	i2c_transmit((address << 1) | I2C_READ);
	if (i2c_check_status(MT_SLA_READ_ACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}

	for (int i = 0; i < len; i++)
	{
		if (i == (len - 1))
		{
			data[i] =  i2c_receive_nack();
			if (i2c_check_status(MT_DATA_RECEIVED_NACK) == ERROR)
			{
				i2c_stop();
				return ERROR;
			}
		}
		else
		{
			data[i] = i2c_receive_ack();
			if (i2c_check_status(MT_DATA_RECEIVED_ACK) == ERROR)
			{
				i2c_stop();
				return ERROR;
			}
		}
	}

	i2c_stop();

	return SUCCESS;
}

/**
  ******************************************************************************
  *	@brief	Read bytes from slave with specify register address
  * @param	Slave device address (7-bit right aligned)
  * @param	Register address
  * @param	Number of data bytes to read from slave 
  * @param	Pointer to data array byte to store data from slave
  * @retval	Result status (SUCCESS or ERROR)
  ******************************************************************************
  */
uint8_t i2c_read_multi_with_reg(uint8_t address, uint8_t reg, uint8_t len, uint8_t* data)
{
	i2c_start();
	if (i2c_check_status(MT_START) == ERROR)
		return ERROR;

	i2c_transmit((address << 1) | I2C_WRITE);
	if (i2c_check_status(MT_SLA_WRITE_ACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}

	i2c_transmit(reg);
	if (i2c_check_status(MT_DATA_TRANSMITTED_ACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}

	i2c_start();
	if (i2c_check_status(MT_REP_START) == ERROR)
		return ERROR;

	i2c_transmit((address << 1) | I2C_READ);
	if (i2c_check_status(MT_SLA_READ_ACK) == ERROR)
	{
		i2c_stop();
		return ERROR;
	}	

	for (int i = 0; i < len; i++)
	{
		if (i == (len - 1))
		{
			data[i] =  i2c_receive_nack();
			if (i2c_check_status(MT_DATA_RECEIVED_NACK) == ERROR)
			{
				i2c_stop();
				return ERROR;
			}
		}
		else
		{
			data[i] = i2c_receive_ack();
			if (i2c_check_status(MT_DATA_RECEIVED_ACK) == ERROR)
			{
				i2c_stop();
				return ERROR;
			}
		}
	}

	i2c_stop();

	return SUCCESS;
}

/** Private functions ------------------------------------------------------- */
/**
  ******************************************************************************
  *	@brief	Wait until I2C module finish its current job
  * @param	None
  * @retval	None
  ******************************************************************************
  */
void i2c_wait_until_finish()
{
	// Wait until TWINT bit in TWCR register is set
	// TWINT bit is set by hardware when I2C/TWI module
	// has finished its job
	while (!(TWCR & (1 << TWINT)));
}

/**
  ******************************************************************************
  *	@brief	Generate I2C start condition
  * @param	None
  * @retval	None
  ******************************************************************************
  */
void i2c_start()
{
	// Generate start condition
	// TWEN is for enable TWI module
	// TWSTA is for initiate start condition
	// TWINT is for initiate TWI module to start its job
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	
	// Wait until start condition is finished
	i2c_wait_until_finish();
}

/**
  ******************************************************************************
  *	@brief	Transmit data byte to I2C bus
  * @param	Data byte to transmit
  * @retval	None
  ******************************************************************************
  */
void i2c_transmit(uint8_t byte)
{
	// Copy data byte to TWDR register
	TWDR = byte;
	
	// Start sending the byte
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	// Wait until data byte transmitted completely
	i2c_wait_until_finish();
}

/**
  ******************************************************************************
  *	@brief	Receive data byte from I2C bus, then return ACK
  * @param	None
  * @retval	Received data byte
  ******************************************************************************
  */
uint8_t i2c_receive_ack()
{
	// Start receiving byte then generate ACK after byte received
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
	
	// Wait until data byte received completely
	i2c_wait_until_finish();
	
	return TWDR;
}

/**
  ******************************************************************************
  *	@brief	Receive data byte from I2C bus, then return NACK
  * @param	None
  * @retval	Received data byte
  ******************************************************************************
  */
uint8_t i2c_receive_nack()
{
	// Start receiving byte without generate ACK
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	// Wait until data byte received completely
	i2c_wait_until_finish();
	
	return TWDR;
}

/**
  ******************************************************************************
  *	@brief	Generate I2C stop condition
  * @param	None
  * @retval	None
  ******************************************************************************
  */
void i2c_stop()
{
	// Generate stop condition
	// TWSTO is for initiate stop condition
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

/**
  ******************************************************************************
  *	@brief	Check I2C module status code, after do its job
  * @param	Success status code
  * @retval	Result status (SUCCESS or ERROR)
  ******************************************************************************
  */
uint8_t i2c_check_status(uint8_t status_code)
{
	// Check TWSR, if equal, then return success, otherwise error
	if ((TWSR & 0xF8) == status_code)
		return SUCCESS;
	else
		return ERROR;
}

/********************************* END OF FILE ********************************/
/******************************************************************************/