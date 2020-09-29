#ifndef CLICK_DATARECEIVE_HH
#define CLICK_DATARECEIVE_HH
#include <click/element.hh>



CLICK_DECLS

struct Message_data{ //总长度 1000数据 + 18头（msg——type不算入长度）
  long msg_type;
  uint32_t sid;
  uint32_t total_len; //数据的总长度
  uint32_t length;    //数据长度
  uint32_t offset;
  uint16_t split;
  uint8_t data[1400];
};

class DataReceive : public Element { public:

  DataReceive();
  ~DataReceive();

  int msgid; //消息队列标识 

  const char *class_name() const	{ return "DataReceive"; }
  const char *port_count() const	{ return PORTS_1_1; }
  //const char *processing() const	{ return PUSH; }
  
  void push(int port, Packet *packet);
  //Packet *simple_action(Packet *packet);

  
  
};

CLICK_ENDDECLS
#endif