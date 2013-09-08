/*
 *  Copyright (C) 2012 Libelium Comunicaciones Distribuidas S.L.
 *  http://www.libelium.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 2.1 of the License, or
 *  (at your option) any later version.
   
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
  
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Version:		0.7
 *  Design:			David Gascón
 *  Implementation:	David Cuartielles, Alberto Bielsa, Yuri Carmona
 */
   
  
// strings and math
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// peppelinux :)
//#include <time.h>
#define	SENS_ON		0
#define	SENS_5V		1
#define	SENS_3V3    0

// fine ppelinux's

// interrupts and sleep
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/sleep.h>

// interrupt core (must be called before Wire)
//#include "WInterrupts.h"

#include <alloca.h>

// I2C libraries
#include "Wire.h"
#include "twi.h"

#include "pins_waspmote.h"

#include "WaspConstants.h"


#ifndef __WASPXBEECONSTANTS_H__
	#include "WaspXBeeConstants.h"
#endif


#include "wiring.h"

// SD
#include "sd_raw_config.h"
#include "sd_raw.h"
#include "partition.h"
#include "fat_config.h"
#include "fat.h"

#include "Sd2Card.h"
#include "Sd2PinMap.h"
#include "SdFat.h"
#include "SdFatUtil.h"
#include "SdInfo.h"


#ifdef __cplusplus

#include "WaspUtils.h"
#include "HardwareSerial.h"
#include "WaspUSB.h"
//#include "WaspXBee.h"
#include "WaspGPS.h"
//#include "WaspGPRS.h"
//#include "WaspGPRSconstants.h"
#include "Wire.h"
#include "WaspRTC.h"
#include "WaspACC.h"
#include "WaspSD.h"
#include "WaspPWR.h"
//#include "WaspXBeeCore.h"
//#include "WaspXBee802.h"
//#include "WaspXBeeZB.h"
//#include "WaspXBeeDM.h"
//#include "WaspXBee868.h"
//#include "WaspXBeeXSC.h"
//#include "WaspSensorAgr.h"
//#include "WaspSensorGas.h"
//#include "WaspSensorEvent.h"
//#include "WaspSensorPrototyping.h"
//#include "WaspBT.h"
//#include "WaspBTConstants.h"
//#include "WaspSensorSmart.h"
#include "MemoryFree.h"
//#include "WaspSensorCities.h"
//#include "WaspSensorParking.h"
//#include "WaspBT_Pro.h"
//#include "WaspWIFI.h"
//#include "WaspGPRS_Pro.h"
//#include "WaspGPRS_Proconstants.h"
//#include "WaspSensorSmart_v20.h"
//#include "WaspSensorRadiation.h"
#include "WaspSensorAgr_v20.h"
//#include "WaspRFID13.h"
#include "SoftwareSerial.h"
//#include "WaspSensorGas_v20.h"

// CAED 
#include "Message.h"
#include "CAED.h"
#include "CAED_XBee.h"

// random prototypes
long random(long);
long random(long, long);
void randomSeed(unsigned int);

#endif
