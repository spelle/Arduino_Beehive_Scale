/*
 * Arduino_Beehive_Scale.cpp
 *
 *  Created on: Oct 3, 2014
 *      Author: spelle
 */



#include <Arduino.h>
#include <SimpleSWModbusSlave.h>
#include "SoftwareSerial.h"
#include <EEPROM.h>

#include <avr/wdt.h>

#define UPDATE_GAUGES_PERIOD 5000
#define BLINK_LED_PERIOD 1000
#define CHECK_WRITES_PERIOD 60000

//////////////// registers of your slave ///////////////////
enum
{
  // just add or remove registers and your good to go...
  // The first register starts at address 0
  ID,
  MB_SLAVE_ID,
  WRITE_AND_REBOOT,
  DEBUG,
  CLEAR,
  SCALE_VAL,
  GAUGE1_VAL,
  GAUGE2_VAL,
  GAUGE3_VAL,
  SCALE_RAW,
  GAUGE1_RAW,
  GAUGE2_RAW,
  GAUGE3_RAW,
  HOLDING_REGS_SIZE // leave this one
  // total number of registers for function 3 and 16 share the same register array
  // i.e. the same address space
};

unsigned int holdingRegs[HOLDING_REGS_SIZE]; // function 3 and 16 register array
////////////////////////////////////////////////////////////

SoftwareSerial SWSerial(10, 11) ; // RX, TX

void setup()
{
	// Set PB5 as OUTPUT
	DDRB |= (1 << PB5 ) ;

	// Initialize HoldingRegs area
	memset( holdingRegs, 0, HOLDING_REGS_SIZE * sizeof(unsigned int)) ;

	// Read Beehive ID  from EEPROM
	holdingRegs[ID] = EEPROM.read(ID);

	// Read Beehive Modbus Slave ID (MB_SLAVE_ID) from EEPROM
	holdingRegs[MB_SLAVE_ID] = EEPROM.read(MB_SLAVE_ID);

	// Enable Debug
	holdingRegs[DEBUG] = 0x0001 ;

	Serial.begin(115200);

	/* parameters(SoftwareSerial* SerialPort,
			long baudrate,
			// unsigned char byteFormat,
			unsigned char ID,
			unsigned char transmit enable pin,
			unsigned int holding registers size,
			unsigned int* holding register array)
	 */

	/* Valid modbus byte formats are:
SERIAL_8N2: 1 start bit, 8 data bits, 2 stop bits
SERIAL_8E1: 1 start bit, 8 data bits, 1 Even parity bit, 1 stop bit
SERIAL_8O1: 1 start bit, 8 data bits, 1 Odd parity bit, 1 stop bit

You can obviously use SERIAL_8N1 but this does not adhere to the
Modbus specifications. That said, I have tested the SERIAL_8N1 option
on various commercial masters and slaves that were suppose to adhere
to this specification and was always able to communicate... Go figure.

These byte formats are already defined in the Arduino global name space.
	 */
	unsigned char sID = (unsigned char)((0 == holdingRegs[ID] ? 0x0002 : holdingRegs[ID]) & 0x00FF) ;
	modbus_configure(&SWSerial, 9600, /*SERIAL_8N2,*/ sID, 3, HOLDING_REGS_SIZE, holdingRegs);

	Serial.println("--- Arduino Beehive Scale ---");
	Serial.print("Beehive ID : ");
	Serial.println(holdingRegs[ID]);
	Serial.print("Beehive Modbus Slave ID : ");
	Serial.println(sID);
}

void loop()
{
	modbus_update() ;

	static unsigned long ulCurrentTime = 0 ;
	static unsigned long ulNextTimeGauges = UPDATE_GAUGES_PERIOD ;
	static unsigned long ulNextTimeChecks = CHECK_WRITES_PERIOD ;
	static unsigned long ulNextTimeLED = BLINK_LED_PERIOD ;

	ulCurrentTime = millis() ;

	if( ulCurrentTime > ulNextTimeGauges )
	{
		if( 0x0000 != holdingRegs[DEBUG] )
		{
			Serial.print(ulCurrentTime) ;
			Serial.println(" : Reading Gauges");
		}

		// Update gauges values
		holdingRegs[GAUGE1_RAW] = analogRead(0) ;
		holdingRegs[GAUGE1_RAW] = analogRead(1) ;
		holdingRegs[GAUGE2_RAW] = analogRead(2) ;

		ulNextTimeGauges = ulCurrentTime + UPDATE_GAUGES_PERIOD ;
	}

	if( ulCurrentTime > ulNextTimeChecks )
	{
		boolean rebootFlag = false ;

		if( 0x0000 != holdingRegs[DEBUG] )
		{
			Serial.print(ulCurrentTime) ;
			Serial.println("Processing eventual Writes");
		}

		if( holdingRegs[ID] != EEPROM.read(ID) )
		{
			EEPROM.write(ID, holdingRegs[ID]) ;
			rebootFlag = true ;
		}

		if( holdingRegs[MB_SLAVE_ID] != EEPROM.read(MB_SLAVE_ID) )
		{
			EEPROM.write(MB_SLAVE_ID, holdingRegs[MB_SLAVE_ID]) ;
			rebootFlag = true ;
		}

		if( 0x0000 != holdingRegs[CLEAR] )
		{
			// write a 0 to all 512 bytes of the EEPROM
			for (int i = 0; i < 512; i++)
				EEPROM.write(i, 0);

		  rebootFlag = true ;
		}


		if( true == rebootFlag )
		{
			if( 0x0000 != holdingRegs[DEBUG] )
			{
				Serial.print(ulCurrentTime) ;
				Serial.println("Writes occured ; reboot");
			}

			wdt_enable(WDTO_15MS); // reboot !
		}

		ulNextTimeChecks = ulCurrentTime + CHECK_WRITES_PERIOD ;
	}

	if( ulCurrentTime > ulNextTimeLED )
	{
		PORTB ^= (1 << PB5 ) ;

		ulNextTimeLED = ulCurrentTime + BLINK_LED_PERIOD ;
	}
}




