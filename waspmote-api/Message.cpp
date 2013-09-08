/*! \file Message.cpp
    \brief Library containing useful general functions
 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2.1 of the License, or
    (at your option) any later version.
   
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
  
    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
  
    Version:		0.1

    Design:		Giuseppe De Marco <peppelinux@yahoo.it>

    Implementation:	Giuseppe De Marco <peppelinux@yahoo.it>

*/
#ifndef __WPROGRAM_H__
	#include "WaspClasses.h"
#endif

#include <inttypes.h>

Message::Message() { msg_size = 0; }

void Message::append(char* pntr_msg) {
        uint8_t prev_msg_size = msg_size;
        
        for (i=0; i <= MAX_MSG_LEN; i++)
        {
            if (pntr_msg[i] != '\0') { msg_size+= 1; }  
            else { break; }
        }

        for (i=prev_msg_size; i <= msg_size; i++) 
        {

            if (msg_size <= MAX_MSG_LEN) message[i] = pntr_msg[i-prev_msg_size];
        }
        
    }

void Message::flush() { msg_size = 0; for (i=0; i<=MAX_MSG_LEN; i++) message[i] = '\0'; }

Message MSG = Message();
