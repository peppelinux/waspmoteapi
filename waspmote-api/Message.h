#ifndef Message_h
#define Message_h


#include <inttypes.h>

#define MAX_MSG_LEN 232


class Message
{

 private:
    uint8_t i;

 public:
    uint8_t msg_size;
    char message[ MAX_MSG_LEN ];
    
    Message(void);
    
    void append(char* pntr_msg);
    void flush();
};

extern Message MSG;
#endif
