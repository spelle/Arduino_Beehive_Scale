/*
 * Arduino_Beehive_Scale.cpp
 *
 *  Created on: Oct 3, 2014
 *      Author: spelle
 */



#include <SimpleSWModbusSlave.h>
#include "SoftwareSerial.h"
#include <EEPROM.h>

#include <avr/wdt.h>

#include "Arduino_Beehive_Scale.h"



#define UPDATE_GAUGES_PERIOD 4999
#define BLINK_LED_PERIOD 999
#define CHECK_WRITES_PERIOD 59999
#ifdef SCALE_SIMU
		#define SIMULATIONSINE_PERIOD  999
#endif
#define TX_ENABLE_PIN  9

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
	holdingRegs[DEBUG_ENABLE] = 0x0001 ;

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
	modbus_configure(&SWSerial, 9600, /*SERIAL_8N2,*/ sID, TX_ENABLE_PIN, HOLDING_REGS_SIZE, holdingRegs);

#ifdef DEBUG
	PDEBUG( 0x00, " : --- Arduino Beehive Scale ---" ) ;
	PDEBUG( 0x00, " : Beehive ID : ");
	PDEBUG( 0x00, holdingRegs[ID]);
	PDEBUG( 0x00, ": Beehive Modbus Slave ID : ");
	PDEBUG( 0x00, sID);
#endif

}

void loop()
{
	modbus_update() ;

	static unsigned long ulCurrentTime = 0 ;
	static unsigned long ulNextTimeGauges = UPDATE_GAUGES_PERIOD ;
	static unsigned long ulNextTimeChecks = CHECK_WRITES_PERIOD ;
	static unsigned long ulNextTimeLED = BLINK_LED_PERIOD ;

#ifdef SCALE_SIMU
	static unsigned long ulNextTimeIncrementSineIndex = SIMULATIONSINE_PERIOD ;

	static unsigned int uiSimulationIndex = 0 ;
#endif

	ulCurrentTime = millis() ;

	if( ulCurrentTime > ulNextTimeGauges )
	{
#ifndef SCALE_SIMU

		PDEBUG( ulCurrentTime, " : Reading Gauges" ) ;

		// Update gauges values
		holdingRegs[GAUGE1_RAW] = analogRead(0) ;
		PDEBUG( ulCurrentTime, " : Gauge #1 : " ) ;
		PDEBUG( ulCurrentTime, holdingRegs[GAUGE1_RAW] ) ;
		holdingRegs[GAUGE2_RAW] = analogRead(1) ;
		PDEBUG( ulCurrentTime, " : Gauge #2 : " ) ;
		PDEBUG( ulCurrentTime, holdingRegs[GAUGE2_RAW] ) ;
		holdingRegs[GAUGE3_RAW] = analogRead(2) ;
		PDEBUG( ulCurrentTime, " : Gauge #3 : " ) ;
		PDEBUG( ulCurrentTime, holdingRegs[GAUGE3_RAW] ) ;

		holdingRegs[SCALE_RAW] =
				holdingRegs[GAUGE1_RAW] +
				holdingRegs[GAUGE2_RAW] +
				holdingRegs[GAUGE3_RAW] ;
#endif

		PDEBUG( ulCurrentTime, ": Scale Raw Value :" ) ;
		PDEBUG( ulCurrentTime, holdingRegs[SCALE_RAW] ) ;

		ulNextTimeGauges = ulCurrentTime + UPDATE_GAUGES_PERIOD ;
	}

	if( ulCurrentTime > ulNextTimeChecks )
	{
		boolean rebootFlag = false ;

		PDEBUG( ulCurrentTime, " : Processing eventual Writes") ;

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

		if( 0x0000 != holdingRegs[CLEAR_EEPROM] )
		{
			// write a 0 to all 512 bytes of the EEPROM
			for (int i = 0; i < 512; i++)
				EEPROM.write(i, 0);

		  rebootFlag = true ;
		}

		if( true == rebootFlag )
		{
			PDEBUG( ulCurrentTime, " : Writes occured ; reboot");

			wdt_enable(WDTO_15MS); // reboot !
		}

		ulNextTimeChecks = ulCurrentTime + CHECK_WRITES_PERIOD ;
	}

#ifdef SCALE_SIMU

	if( ulCurrentTime > ulNextTimeIncrementSineIndex )
	{
		if( SIMULATIONSINE_NB_STEP >= uiSimulationIndex )
		{
			uiSimulationIndex++ ;
		}
		else
		{
			uiSimulationIndex = 0 ;
		}

		PDEBUG( ulCurrentTime, " : Processing Sine Simulation" ) ;

		holdingRegs[SCALE_RAW] = simulationSine[uiSimulationIndex] ;

		ulNextTimeIncrementSineIndex = ulCurrentTime + SIMULATIONSINE_PERIOD ;
	}

#endif


	if( ulCurrentTime > ulNextTimeLED )
	{
		PORTB ^= (1 << PB5 ) ;

		ulNextTimeLED = ulCurrentTime + BLINK_LED_PERIOD ;
	}
}




