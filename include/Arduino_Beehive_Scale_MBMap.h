/*
 * Arduino_Beehive_Scale_MBMap.h
 *
 *  Created on: Oct 4, 2014
 *      Author: nuptse
 */

#ifndef ARDUINO_BEEHIVE_SCALE_MBMAP_H_
#define ARDUINO_BEEHIVE_SCALE_MBMAP_H_



//////////////// registers of your slave ///////////////////
enum
{
  // just add or remove registers and your good to go...
  // The first register starts at address 0
  ID,
  MB_SLAVE_ID,
  WRITE_AND_REBOOT,
  DEBUG_ENABLE,
  CLEAR_EEPROM,
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
////////////////////////////////////////////////////////////



#endif /* ARDUINO_BEEHIVE_SCALE_MBMAP_H_ */
