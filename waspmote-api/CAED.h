/*
 *  Modified for Waspmote by D. Cuartielles & A. Bielsa, 2009
 *
 *  Copyright (c) 2005-2006 David A. Mellis
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
 */
 

#ifndef CAED_H
#define CAED_H


//#include <avr/io.h>
//#include <avr/sleep.h>
//#include <avr/wdt.h>

#include <inttypes.h>


#ifdef __cplusplus
extern "C"{
#endif

#define version "v 1.0 beta-rc1\n"
#define EOL '\n'
#define errore "Errore"
#define radio_TX "TX"
#define radio_RX "RX"
#define space_char " "
#define frees "free RAM: "
#define printfreeMemory USB.print(frees);  USB.println(freeMemory());
#define DATA_directory "DATA"


#define header_closer "]"      // 0

#define m_status     "#status" //0
#define m_ack      "#ack"        // 1
#define m_response   "#response"   // 2
#define m_sample     "#sample"     // 3
#define m_warning    "#warning"    // 4
#define m_watch      "#watch"    // 5


//In fase di produzione saranno eliminati ed inglobati nel sample
#define gps           "#gps"
#define gps_failed    "#gps_failed"
#define gps_sentence  "#gps_sentence"

#define pressure_message_len    17
#define soiltemp_message_len    20
#define solar_message_len       20
#define temperature_message_len 13
#define watermark_message_len   73
#define sensirion_message_len   25
#define anemometer_message_len  20
#define message_gps_len         31
#define vane_message_len        20
#define pluviometer_message_len 25
#define message_header_len      45 
// MAX_MSG_LEN dovrebbe essere circa 200. Vedi Message.h



/*
//Dichiarazione sensori/board attivi
extern uint8_t ZIGBEE_radio;
extern uint8_t GPS_sens;
extern uint8_t AGR_BOARD;
extern uint8_t AGR_BOARD_mode; //Indica se la scheda è attivata o no
extern uint8_t TEMP_MCP9700A;
extern uint8_t SENSIRION;
extern uint8_t PRESS_MPX4115A;
extern uint8_t SOIL_MOIST_WATERMARK; // se 1, 2 o 3 indica il num di sensori watermak presenti, max 3.
extern uint8_t SOIL_TEMP;
extern uint8_t SOLAR_RADIATION;
//Stazione pluviometrica
extern uint8_t ANEMOMETER;
extern uint8_t WIND_VANE;
extern uint8_t PLUVIOMETER;

extern uint8_t USB_on1;
extern uint8_t SD_log;
extern uint8_t restore_SD;

extern uint8_t delay_sensirion;
*/

#ifdef __cplusplus
} // extern "C"
#endif


#endif // end if CAED_h
