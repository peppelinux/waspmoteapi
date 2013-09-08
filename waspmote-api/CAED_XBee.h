#ifndef CAED_XBee_h
#define CAED_XBee_h

#include <inttypes.h>

#define RADIO_TX_BUFFER_SIZE 120

#define	XBEE_OFF	    0
#define	XBEE_ON		    1
#define	XBEE_HIBERNATE	2

#define XBEE_RATE	38400

#define BROADCAST_PAYLOAD_LEN               92
#define BROADCAST_PAYLOAD_LEN_encrypt       74
#define BROADCAST_PAYLOAD_LEN_encrypt_aps   70

#define UNICAST_PAYLOAD_LEN               84
#define UNICAST_PAYLOAD_LEN_encrypt       66
#define UNICAST_PAYLOAD_LEN_encrypt_aps   62

// generic 16network address 0xFF, 0xFE;

#define MY_TYPE  2  // my addr len in bytes
#define MAC_TYPE 8  // mac addr len in bytes
#define NI_TYPE  10 // needs to be valuated in a condition

#define BROADCAST_ADDRESS 000000000000FFFF

#define ATCOMMAND_MAX_FRAME_SIZE 48 // 2**24(bytes). this "literal" size contains also the hex link-key
#define ATcommand_frame_len  18 // constant lenght of a AT command frame hex literal, with crc: 16+2
#define ATcommand_frame_size 9  // bytes dimention of a AT command, with crc: 8+1 = 9
#define WAIT_READ_FRAME_TIMEOUT     2  // in seconds, maximum interval for a response after an AT command
#define WAIT_AFTER_ATCOMMAND_TX    200 // in milliseconds

class CAED_XBee
/*
 *  Base class, 868, 900 and 2.4 must inherits this class :)
 *  At the moment it is specialized in 2.4ghz...so inheritance will be
 *  implemented in the future. sorry, è domenica.
 */
{

 private:
	uint8_t _uart;            // used for baudrate reference
	uint8_t _pwrMode;         // used for power state
	
    // nice idea...for future. Some commands sends back multiple frames (for example, the ND (Node Discover)
    //uint8_t   _response_types_stack[3]; // it stores the last response frame type
    //uint8_t * _response_value_stack[3]; // it stores pointers to calloc containins response values
    
	//! It prints a number in the specified base
  	/*!
	\param unsigned long n : the number to print
	\param uint8_t base : the base for printing the number
	\return void
	 */
	void printNumber(unsigned long n, uint8_t base);
	
	//! It prints a 'float' number
  	/*!
	\param double number : the number to print
	\param uint8_t digits : the number of non-integer part digits
	\return void
	 */
	void printFloat(double number, uint8_t digits);
    
 public:

    //uint8_t _command[ATCOMMAND_MAX_FRAME_SIZE];   // array for AT command feed
	uint8_t mac_address[8];   // 8 bytes, 16 hex literal lenght


    uint8_t * _rx_data_frame_pntr; // used for managing callocs with frame responses
	uint8_t   _rx_data_frame_len; 
	
    uint8_t   _rx_data_frame_type;    // describe tipology related to _frame_status and _value
    int8_t   _rx_data_frame_status;  // used for tracing AT commands response
    uint8_t * _rx_data_frame_value;   // used for frame value calloc
	uint8_t   _rx_data_frame_value_len;
	uint8_t   _rx_data_identifier[2];
	
    // costructor, begins xbee device, powers on, runs game
    CAED_XBee();

    void begin(uint8_t uart, uint16_t speed);
    void close();    
    void setMode(uint8_t mode);      // mode : XBEE_ON, XBEE_OFF, XBEE_HIBERNATE
    //void close();                  // closes device, implicit power off
    
    // setup features
    uint8_t get_MacAddress();
    
    // state features
    uint8_t available();             // if data are available
    int read();                  // read one byte from xbee RX buffer
    
    // UART Write features
    void flush();
	void print(char c);
	void print(const char[]);
	void print(uint8_t b);
	void print(int n);
	void print(unsigned int n);
	void print(long n);
	void print(unsigned long n);
	void print(long n, int base);
	void print(double n);
    
    void print_escaped( int );
    void print_escaped_hex( int );
    
    uint8_t send_frame(char payload[], uint8_t max_payload_size, uint8_t destADDRtype, uint8_t packet_id, uint8_t _64destADDR[], uint8_t _16destADDR[], uint8_t maximum_hops, uint8_t transmit_options );
	uint8_t send_message( char message[], uint8_t message_len, uint8_t max_payload_size, uint8_t destADDRtype, uint8_t packet_id, uint8_t _64destADDR[], uint8_t _16destADDR[], uint8_t maximum_hops, uint8_t transmit_options );
 
    // RX frame
	uint8_t wait_read_frame();                             // listen 2 seconds for RX and store it in a calloc
	uint8_t wait_for( uint8_t frame_type  );

    // AT commands managment  
    //void clearCommand();
	void build_ATcommand(uint8_t ATcommand_type);
    void execute_ATcommand(char* at_command_template);                              // execute ATcommand resident in _command
    uint8_t parse_ATcommand_response();         // execute ATcommand resident in _rx_data_frame_pntr
	uint8_t parse_TX_response(); 
	
	// support functions
	uint8_t unescapeDATA_frame();
};

extern CAED_XBee SimpleBee;
#endif
