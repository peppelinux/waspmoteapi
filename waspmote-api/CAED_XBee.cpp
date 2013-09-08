
// defines libelium compatibility
#ifndef __WPROGRAM_H__
  #include "WaspClasses.h"
#endif

#ifndef _INTTYPES_H
    #include <inttypes.h>
#endif

CAED_XBee::CAED_XBee() {}

void CAED_XBee::begin( uint8_t uart, uint16_t speed )
{
  _uart = uart;
  beginSerial(XBEE_RATE, _uart);
  pinMode(XBEE_PW,OUTPUT);
  delay(500);
}

void CAED_XBee::close() { closeSerial(_uart); }

/*
 Function: It clears the UART buffer
 Returns: void 
*/
void CAED_XBee::flush() { serialFlush(_uart); }

/*
 Function: Checks if there is available data in selected UART
 Returns: number of available bytes.  
*/
uint8_t CAED_XBee::available()  { return serialAvailable(_uart); }

int CAED_XBee::read() 		{ return serialRead(_uart); }

void CAED_XBee::setMode(uint8_t mode)
{
  
  _pwrMode = mode;
  // set the GPS in the defined power mode
  switch (_pwrMode)
  {
    // Keeped legacy with Libelium resident API.
	case XBEE_ON:
		if(!_uart) digitalWrite(XBEE_PW,HIGH);
		else
		{	
			Utils.setMuxGPRS(); // Switch Multiplexor to socket1 (UART1) 
			pinMode(DIGITAL6,OUTPUT); 
			digitalWrite(DIGITAL6,HIGH);
			// FIXME: This will disappear in the final version
			pinMode(GPRS_PW,OUTPUT);
			digitalWrite(GPRS_PW,HIGH); 
		}
		break;
	
	case XBEE_OFF:
		if(!_uart) digitalWrite(XBEE_PW,LOW);
		else
		{
			Utils.setMux(MUX_TO_LOW,MUX_TO_LOW);
			pinMode(DIGITAL6,OUTPUT);
			digitalWrite(DIGITAL6,LOW);
			// FIXME: This will disappear in the final version
			pinMode(GPRS_PW,OUTPUT);
			digitalWrite(GPRS_PW,LOW); 
		}
		break;
	
	default:
		break;
  }
    
}

/*
void CAED_XBee::clearCommand() 
{ 
	uint8_t i;
    for(i=0;i<ATCOMMAND_MAX_FRAME_SIZE;i++)  _command[i]=0;
}
*/


/*
 Function: Generates the API frame to send to the XBee module
 Parameters:
 	data : The string that contains part of the API frame
 	param : The param to set
 Returns: Nothing
 Values: Stores in 'command' variable the API frame to send to the XBee module
*/
void CAED_XBee::execute_ATcommand(char* at_command_template)
{ 
    uint8_t it;
    uint8_t cmd_len = 0;
    uint8_t _command[ATCOMMAND_MAX_FRAME_SIZE];
    
    //clearCommand();
    for (it = 0; it < ATCOMMAND_MAX_FRAME_SIZE; it++)
    {   
        if ( at_command_template[it] == '\0'  ) break;
         
        sscanf(&at_command_template[it * 2], "%2hhx", &_command[it]);
    }
	
	cmd_len = it;
	
    // assumes that all defined at command templates are escaped natively
    for (it=0; it < cmd_len; it++) SimpleBee.print(_command[it]);
    
    delay(WAIT_AFTER_ATCOMMAND_TX);
}


/*
 * GETS macaddress and store in public variable */
uint8_t CAED_XBee::get_MacAddress()
{
	uint8_t it = 0;
	
	SimpleBee.flush();
	
	SimpleBee.execute_ATcommand( get_own_mac_high );
	SimpleBee.wait_for( 0x88 );
	
    if ( SimpleBee._rx_data_frame_type != 0x88 || SimpleBee._rx_data_identifier[1] != 0x48 ) return 1;
    else { for (it = 0; it < 4; it++) SimpleBee.mac_address[it] = 0; }
    for (it = 0; it < 4; it++) SimpleBee.mac_address[it] = SimpleBee._rx_data_frame_value[it];
	
	SimpleBee.execute_ATcommand( get_own_mac_low );
	SimpleBee.wait_for( 0x88 );

    if ( SimpleBee._rx_data_frame_type != 0x88 || SimpleBee._rx_data_identifier[1] != 0x4c ) return 1;
    else { for (it = 4; it < 8; it++) SimpleBee.mac_address[it] = 0; }

    for (it = 4; it < 8; it++) SimpleBee.mac_address[it] = SimpleBee._rx_data_frame_value[it-4];
	

	if ( (SimpleBee.mac_address[0] == SimpleBee.mac_address[4]) && (SimpleBee.mac_address[1] == SimpleBee.mac_address[5]) && (SimpleBee.mac_address[2] == SimpleBee.mac_address[6]) && (SimpleBee.mac_address[3] == SimpleBee.mac_address[7]) )
	{
		return 1;
	}
	
	SimpleBee.flush();
	
	return 0;

}

void CAED_XBee::build_ATcommand(uint8_t ATcommand_type)
{
	
	char* payload = "ciao mamma :)" ;
	
	
    #define bytes_before_len 3 
    #define bytes_between_len_and_payload 15
    #define payload_start_position 18
    
    //for (i=0; i < payload_len; i++) SimpleBee.print(payload[i], HEX); 
    //SimpleBee.print(payload); 
    //return 0;
    
    uint8_t i = 0; 
    char pl;
    uint8_t payload_len = 0;
    uint8_t addr_len;
    uint8_t* TX = (uint8_t*) calloc(RADIO_TX_BUFFER_SIZE, sizeof(uint8_t));
    
    uint8_t checksum_tx_index;
    
    // check payload lenght
    while (1)
    {
        pl = payload[i];
        i+=1;
        if (i > 84 || pl == '\0') break;
    } 
    payload_len = i+1; // got it !
    
    //payload_len = 12;
    
    switch(MAC_TYPE)
    {
        case MY_TYPE: 	addr_len=2;     break;
        case MAC_TYPE: 	addr_len=8;     break;
        case NI_TYPE: 	break;          // should be implemented
        default:	break;
    }
    
    TX[0]=0x7E;
    TX[1]=0x00;
    
    TX[3]=0x10;             // frame id -> tx request 
    TX[4]=0x52;
    
    for (i = 0; i < addr_len; i++) TX[5+i] = 0x33;
    
    TX[13] = 0x33;
    TX[14] = 0x33;
    
    TX[15]= 0x00;
    
    /* Transmit options bitfield.
    Unused bits must be set to 0.
    0x01 - Disable retries and route repair
    0x20 - Enable APS encryption (if EE=1) Enabling APS encryption decreases the maximum number of RF payload bytes by 4 (below the value reported by NP).
    0x40 - Use the extended transmission timeout for this destination. Setting the extended timeout bit causes the stack to set the extended transmission timeout for the destination address.
    */
    TX[16]= 0x00;
    
    for(i=0;i<payload_len;i++) TX[i+payload_start_position-1]= long( payload[i] ); checksum_tx_index = i;
    
    // len value just in time !
    TX[2]=bytes_between_len_and_payload+payload_len-bytes_before_len;   // 14 is the minimum tx request len

    checksum_tx_index = checksum_tx_index + bytes_between_len_and_payload+1;

    
    /*   To test data integrity, a checksum is calculated and verified on non-escaped data.
    To calculate: Not including frame delimiters and length, add all bytes keeping only the lowest 8
    bits of the result and subtract the result from 0xFF.
    To verify: Add all bytes (include checksum, but not the delimiter and length). If the checksum is
    correct, the sum will equal 0xFF.  */
    
    TX[checksum_tx_index] = 0;      // calculating checksum
    for(i=3;i<(TX[2]+3);i++) TX[checksum_tx_index]=TX[checksum_tx_index]+TX[i]; 
    while((TX[checksum_tx_index]>255)) TX[checksum_tx_index]=TX[checksum_tx_index]-256; 
    TX[checksum_tx_index]=255-TX[checksum_tx_index];
    TX[ bytes_between_len_and_payload + payload_len ]= TX[checksum_tx_index];
    
    //TX[checksum_tx_index] = 0x0B;
        
    //for (i=0; i < ( bytes_between_len_and_payload + payload_len + 1 ); i++) SimpleBee.print_escaped(TX[i]);
    
    // for debug only !
    //SimpleBee.print(EOL);
    //for (i=0; i < ( ext_frame_bytes_len + payload_len + 1 ); i++) SimpleBee.print_escaped_hex((int)TX[i]);

    free(TX);
    TX=NULL;

}

uint8_t CAED_XBee::unescapeDATA_frame()
{
    uint8_t it;
    //uint8_t prev = 0;
    uint8_t n;
    uint8_t prev_cnt = 0;
    
    uint8_t bytes_between = SimpleBee._rx_data_frame_len - SimpleBee._rx_data_frame_value_len;
    
	SimpleBee._rx_data_frame_value  = (uint8_t*) calloc( SimpleBee._rx_data_frame_value_len, sizeof(uint8_t));

	for (it=0; it < SimpleBee._rx_data_frame_value_len; it++)  {

		n = SimpleBee._rx_data_frame_pntr[bytes_between+it];
		
		//SimpleBee._rx_data_frame_value[it-prev_cnt] = n;
		
		if ((n == 0x5E) || (n == 0x5D) || (n == 0x31) || (n == 0x33)) SimpleBee._rx_data_frame_value[it-prev_cnt] = n ^ 0x20;
		else if ( n == 0x7D ) { prev_cnt++; continue; }
		else { SimpleBee._rx_data_frame_value[it-prev_cnt] = n; }

	}
	
}

uint8_t CAED_XBee::parse_TX_response()
{   
    #define MAX_0x8B_FRAME_DATA_LEN  7
    #define MAX_0x8B_VALUE_LEN       1
    #define MIN_0x8B_FRAME_DATA_LEN  3

    uint8_t bytes_between = 4; // used to be equal to min_$_frame_data_len
    
    if (SimpleBee._rx_data_frame_len <= MAX_0x8B_FRAME_DATA_LEN) // avoids memory corruptions 
    {
        SimpleBee._rx_data_frame_type   = 0x8B;
        SimpleBee._rx_data_frame_status = SimpleBee._rx_data_frame_pntr[5];
		SimpleBee._rx_data_frame_value_len = SimpleBee._rx_data_frame_len-bytes_between;
		
        if ( SimpleBee._rx_data_frame_len >= MIN_0x8B_FRAME_DATA_LEN ) // else there's no need to store value. it simply doesn't exists ! 
        {	SimpleBee.unescapeDATA_frame(); }
        
        return 0;
    }
    else { return 2; }
    
}

uint8_t CAED_XBee::parse_ATcommand_response()
{   
    #define MAX_0x88_FRAME_DATA_LEN 26
    #define MAX_0x88_VALUE_LEN      18
    #define MIN_0x88_FRAME_DATA_LEN  4
    
    uint8_t bytes_between = 5; // used to be equal to min_$_frame_data_len
    uint8_t it;
    
    // 
    if (SimpleBee._rx_data_frame_len <= MAX_0x88_FRAME_DATA_LEN) // avoids memory corruptions 
    {
        SimpleBee._rx_data_frame_type   = 0x88;
        SimpleBee._rx_data_frame_status = SimpleBee._rx_data_frame_pntr[4]; // 7 - 3

		SimpleBee._rx_data_identifier[0] = SimpleBee._rx_data_frame_pntr[2];
		SimpleBee._rx_data_identifier[1] = SimpleBee._rx_data_frame_pntr[3];

		SimpleBee._rx_data_frame_value_len = SimpleBee._rx_data_frame_len-bytes_between;

        if (SimpleBee._rx_data_frame_len >= MIN_0x88_FRAME_DATA_LEN ) // else there's no need to store value. it simply doesn't exists ! 
        {	SimpleBee.unescapeDATA_frame(); }
        /*
        {
            SimpleBee._rx_data_frame_value  = (uint8_t*) calloc( SimpleBee._rx_data_frame_value_len, sizeof(uint8_t));

            for (it=0; it < SimpleBee._rx_data_frame_value_len; it++) 
            {
				SimpleBee._rx_data_frame_value[it] = SimpleBee._rx_data_frame_pntr[bytes_between+it];            
			}
        }
        */
        return 0;
        
    }
    else { return 2; }
    
}


/*
 * 
 * Wait for a specific frame after a TX 
 * 
 */
uint8_t CAED_XBee::wait_for( uint8_t frame_type  )
{
	SimpleBee.wait_read_frame();
	if ( SimpleBee._rx_data_frame_type == frame_type ) return 0;
	
	SimpleBee.wait_read_frame();
	if ( SimpleBee._rx_data_frame_type == frame_type ) return 0;
	else { return 1; }
	
}

uint8_t CAED_XBee::wait_read_frame()
{   /* Parses frames identified by 0x7E,
    If another 0x7e occours will parse it in 
    successive loop */
    
    /* it blocks for 2 seconds */
    /* it feeds _frame_status and _frame_value
       returns  0 if success
       returns  1 if frame bytes are invalid -> could not understand type or lenght are invalid
       returns -1 if nothing appeared
    */
    
    uint8_t pos = 0;
    //uint8_t p_cnt = 0; 
    long start_millis = millis();
    long current_millis = millis();
    int8_t parse_exit_status = -1;
    uint8_t incomingByte;
    
    // frees previous frame_bytes
    free(SimpleBee._rx_data_frame_value); SimpleBee._rx_data_frame_value = NULL;
    SimpleBee._rx_data_frame_type = 0; SimpleBee._rx_data_frame_status = -1; SimpleBee._rx_data_frame_value_len = 0;
    SimpleBee._rx_data_identifier[0] = 0; SimpleBee._rx_data_identifier[1] = 0;
    
    while ( (current_millis - start_millis) < WAIT_READ_FRAME_TIMEOUT*1000 ) 
    {
            incomingByte=SimpleBee.read();
            
            if(incomingByte == 0x7E){       // 126 = 0x7E
              SimpleBee.read();                  // flushes MSB 1
              SimpleBee._rx_data_frame_len = SimpleBee.read();      // get   LSB 2
              
              if ( SimpleBee._rx_data_frame_len > RADIO_TX_BUFFER_SIZE * 2) continue; // avoids a calloc abuse attack
              
              SimpleBee._rx_data_frame_pntr = (uint8_t*)calloc( SimpleBee._rx_data_frame_len, sizeof(uint8_t)); 
              
              uint8_t c_pos; // get all the frame without final checksum (zigbee protocol assures this !)
              for (c_pos=0; c_pos < SimpleBee._rx_data_frame_len; c_pos++ )
              {  SimpleBee._rx_data_frame_pntr[c_pos] = SimpleBee.read();  }
              
              switch(_rx_data_frame_pntr[0])  // parses frame type and call delegated func
              {   
                  case 0x08: /* AT Command */ break;	
                  case 0x09: /* AT Command - Queue parameter value */ break;
                  case 0x10: /* TX request */ break;
                  case 0x11: /* Explicit Addressing Command Frame */ break;
                  case 0x17: /* Remote AT command request */ break;
                  case 0x88: /* AT command response */ parse_exit_status = SimpleBee.parse_ATcommand_response(); break;
                  case 0x8A: /* Modem status */ break;
                  case 0x8B: /* Transmit status */ parse_exit_status = SimpleBee.parse_TX_response(); break;
                  case 0x90: /* Receive Packet */ break;
                  case 0x97: /* Remote Command Response */ break;
                  
                  default: parse_exit_status = 1; break;
              }
              
              free(SimpleBee._rx_data_frame_pntr); SimpleBee._rx_data_frame_pntr=NULL; // flush memory !
          
              break;
              
            } // fine if 0x7e
			
			current_millis = millis();
			
      } // fine while 
    
    return parse_exit_status;
}



void CAED_XBee::print(char c)
{
  printByte(c,  _uart);
}

void CAED_XBee::print(const char c[])
{
  printString(c,  _uart);
}

void CAED_XBee::print(uint8_t b)
{
  printByte(b,  _uart);
}

void CAED_XBee::print(int n)
{
  print((long) n);
}

void CAED_XBee::print(unsigned int n)
{
  print((unsigned long) n);
}

void CAED_XBee::print(long n)
{
  if (n < 0) {
    print('-');
    n = -n;
  }
  printNumber(n, 10);
}

void CAED_XBee::print(unsigned long n)
{
  printNumber(n, 10);
}

void CAED_XBee::print(double n)
{
	printFloat(n, 10);
}

void CAED_XBee::print(long n, int base)
{
  if (base == 0)
    print((char) n);
  else if (base == 10)
    print(n);
  else
    printNumber(n, base);
}

void CAED_XBee::printNumber(unsigned long n, uint8_t base)
{
  printIntegerInBase(n, base,  _uart);
}

void CAED_XBee::printFloat(double number, uint8_t digits) 
{
  // Handle negative numbers
  if (number < 0.0)
  {
     print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    print(toPrint);
    remainder -= toPrint; 
  }
}

void CAED_XBee::print_escaped( int n )
{
    /*
    Theese bytes need to be escaped
          0x7E 0x7D 0x11 0x13
      if 0x7E is in first position index
      rapresents frame start byte.
      We avoids '~' symbol in message
      so we avoids 0x7E escaping
    */
    
    if ( /*(n == 0x7E) ||*/ (n == 0x7D) || (n == 0x11) || (n == 0x13) ) 
    {   
        print((long) 0x7D, BYTE); 
        print((long) (n ^ 0x20), BYTE);
    }
    else { print((long)n, BYTE); }
}

void CAED_XBee::print_escaped_hex(int n)
{
    /*
    For debug only ! :)
    Theese bytes need to be escaped
          0x7E 0x7D 0x11 0x13
    */
    
    if ( /*(n == 0x7E) ||*/ (n == 0x7D) || (n == 0x11) || (n == 0x13) ) 
    {   
        print((int) 0x7D, HEX); 
        print((int) (n ^ 0x20), HEX);
    }
    else { print((int)n, HEX); }
    
    print(' ');
    
}

/*
 * Send a message, frags it if necessary
 * returns 0 if everything goes fine
 * returns 1 if ack fails
 */
uint8_t CAED_XBee::send_message(char message[], uint8_t message_len, uint8_t max_payload_size, uint8_t destADDRtype, uint8_t packet_id, uint8_t _64destADDR[], uint8_t _16destADDR[], uint8_t maximum_hops, uint8_t transmit_options )
{
	#define outlet_template " %d>>"
	#define inlet_template "<<%d "
	#define frag_identifier_len 4
	#define max_message_len 1800 // bytes
	
	uint8_t current_frame = 0;
	uint8_t delivery_status = 1;

	if (message_len <= max_payload_size) 
	{
		SimpleBee.send_frame(message, max_payload_size, destADDRtype, packet_id, _64destADDR, _16destADDR, maximum_hops, transmit_options ); 
		delivery_status = SimpleBee.wait_for(0x8B);	
                USB.print("single frame: "); USB.print(EOL);	
	}
	
	else 
	{
		if ( message_len > max_message_len ) return 2;
		
		// fragmentation occours 
		uint8_t frag_num = 0;
		uint8_t packet_number = 0;
		uint8_t it;
		uint8_t pos_message = 0;
		uint8_t pos_payload = 0;

		float message_len_float = message_len;	
		
        char payload[ max_payload_size ];
                
		/*
		 * for 3 frames we have: 
		 * 		$payload 2>> <<2 $payload 1>> <<1 $payload;;
		 */
		packet_number = ceil( message_len_float / max_payload_size );
		message_len_float = message_len_float + ( frag_identifier_len *2 ) + ( (packet_number - 2)*frag_identifier_len  ) + 2;
		
		packet_number = ceil( message_len_float / max_payload_size );
		char rendered_template[6];
		
		// TODO
		// RICALCOLARE QUANDO SUPERANO e sommare sempre +4 e +8 bytes alla serie.
		
		for (frag_num=packet_number; frag_num > 0; frag_num--)
		{

            // clean the payload array first !
            for ( it=0; it <= max_payload_size; it++ ) { payload[it] = ' '; }

		    if (frag_num == packet_number) // first fragment
					{

						// feeds payload until -4 for appending frag identifier template
						for (it=0; it < (max_payload_size - frag_identifier_len -1); it++ ) payload[it] = message[it]; 
                        
                        pos_message += it; //it+1; // save last position in message

						sprintf(rendered_template, outlet_template, frag_num -1); // render values into template
						
						// feeds payload with fragment identifier
						for (it=0; it <= frag_identifier_len; it++ ) payload[it+pos_message] = rendered_template[ it ];
                    
                                                
                  }
					
	        else if ( frag_num == 1 ) // last packet
				{

					sprintf(rendered_template, inlet_template, frag_num); // render values into template
					pos_payload = 0;

					// feeds payload with fragment identifier
					for (it=0; it < frag_identifier_len; it++ ) payload[it] = rendered_template[it];						
					pos_payload += it; //it+1;

					// feeds payload after frag identifier
					//for (it=pos_payload; it < (max_payload_size - frag_identifier_len); it++ ) payload[it] = message[ (it - (pos_cursor+1)+ pos_sum ) ]; 
				        
					it = 0;
					for (;;) 
					{
					  if ( message[ pos_message+it ] != '\0' ) 
					  {
						payload[pos_payload + it ] = message[ pos_message+it ];
						it++;
					  }
					  else { break; }
					  
					  if (it > (max_payload_size - frag_identifier_len -1 -2)) { break; }
					  
					}
					
					// closing chars
					payload[pos_payload + it +1 ] = ';'; //payload[pos_payload + it +3 ] = '\n';
												

}
	        else	// packets between first and last 
				{
  
   					sprintf(rendered_template, inlet_template, frag_num); // render values into template
					pos_payload = 0;

					// feeds payload with fragment identifier
					for (it=0; it < frag_identifier_len; it++ ) payload[pos_payload+it] = rendered_template[it];						
					pos_payload += it; //it+1;
					
					for (it=0; it < (max_payload_size - frag_identifier_len - frag_identifier_len -2); it++ ) payload[it+pos_payload] = message[it+pos_message]; 
					pos_payload += it; //it+1;
					pos_message += it; //it+1;

					sprintf(rendered_template, outlet_template, frag_num-1); // render values into template
					
					// feeds payload with fragment identifier
					for (it=0; it < frag_identifier_len; it++ ) payload[pos_payload+it] = rendered_template[ it ];

}
						
			// sends the frame and wait for specific response type
			SimpleBee.send_frame( payload, max_payload_size, destADDRtype, packet_id, _64destADDR, _16destADDR, maximum_hops, transmit_options );
			delivery_status = SimpleBee.wait_for(0x8B);
						
		    if (delivery_status == 1) return 1;
		} 			// end of for
		
	} 				// end of else
	
	return delivery_status;
}

uint8_t CAED_XBee::send_frame(char payload[], uint8_t max_payload_size, uint8_t destADDRtype, uint8_t packet_id, uint8_t _64destADDR[], uint8_t _16destADDR[], uint8_t maximum_hops, uint8_t transmit_options )
{   
    #define bytes_before_len 3 
    #define bytes_between_len_and_payload 15
    #define payload_start_position 18
    
    //for (i=0; i < payload_len; i++) SimpleBee.print(payload[i], HEX); 
    //SimpleBee.print(payload); 
    //return 0;
    
    uint8_t i = 0; 
    char pl;
    uint8_t payload_len = 0;
    uint8_t addr_len;
    uint8_t* TX = (uint8_t*) calloc(RADIO_TX_BUFFER_SIZE, sizeof(uint8_t));
    
    uint8_t checksum_tx_index;
    
    // check payload lenght
    while (1)
    {
        pl = payload[i];
        i+=1;
        if (i > max_payload_size || pl == '\0') break;
    } 
    payload_len = i+1; // got it !
    
    //payload_len = 12;
    
    switch(destADDRtype)
    {
        case MY_TYPE: 	addr_len=2;     break;
        case MAC_TYPE: 	addr_len=8;     break;
        case NI_TYPE: 	break;          // should be implemented
        default:	break;
    }
    
    TX[0]=0x7E;
    TX[1]=0x00;
    
    TX[3]=0x10;             // frame id -> tx request 
    TX[4]=packet_id;
    
    for (i = 0; i < addr_len; i++) TX[5+i] = _64destADDR[i];
    
    TX[13] = _16destADDR[0];
    TX[14] = _16destADDR[1];
    
    TX[15]= maximum_hops;
    
    /* Transmit options bitfield.
    Unused bits must be set to 0.
    0x01 - Disable retries and route repair
    0x20 - Enable APS encryption (if EE=1) Enabling APS encryption decreases the maximum number of RF payload bytes by 4 (below the value reported by NP).
    0x40 - Use the extended transmission timeout for this destination. Setting the extended timeout bit causes the stack to set the extended transmission timeout for the destination address.
    */
    TX[16]= transmit_options;
    
    for(i=0;i<payload_len;i++) TX[i+payload_start_position-1]= long( payload[i] ); checksum_tx_index = i;
    
    // len value just in time !
    TX[2]=bytes_between_len_and_payload+payload_len-bytes_before_len;   // 14 is the minimum tx request len

    checksum_tx_index = checksum_tx_index + bytes_between_len_and_payload+1;

    
    /*   To test data integrity, a checksum is calculated and verified on non-escaped data.
    To calculate: Not including frame delimiters and length, add all bytes keeping only the lowest 8
    bits of the result and subtract the result from 0xFF.
    To verify: Add all bytes (include checksum, but not the delimiter and length). If the checksum is
    correct, the sum will equal 0xFF.  */
    
    TX[checksum_tx_index] = 0;      // calculating checksum
    for(i=3;i<(TX[2]+3);i++) TX[checksum_tx_index]=TX[checksum_tx_index]+TX[i]; 
    while((TX[checksum_tx_index]>255)) TX[checksum_tx_index]=TX[checksum_tx_index]-256; 
    TX[checksum_tx_index]=255-TX[checksum_tx_index];
    TX[ bytes_between_len_and_payload + payload_len ]= TX[checksum_tx_index];
    
    //TX[checksum_tx_index] = 0x0B;
    
    //for (i=0; i < ( ext_frame_bytes_len + payload_len + 1 ); i++) { SimpleBee.print(TX[i], HEX); SimpleBee.print(' '); }
    
    for (i=0; i < ( bytes_between_len_and_payload + payload_len + 1 ); i++) SimpleBee.print_escaped(TX[i]);
    
    // for debug only !
    //SimpleBee.print(EOL);
    //for (i=0; i < ( ext_frame_bytes_len + payload_len + 1 ); i++) SimpleBee.print_escaped_hex((int)TX[i]);

    free(TX);
    TX=NULL;
    
    delay(WAIT_READ_FRAME_TIMEOUT*500);
}

CAED_XBee SimpleBee = CAED_XBee();
