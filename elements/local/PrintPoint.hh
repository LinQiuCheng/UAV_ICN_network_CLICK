#ifndef CLICK_PRINTPOINT_HH
#define CLICK_PRINTPOINT_HH
#include <click/element.hh>
#include <clicknet/ether.h>

CLICK_DECLS



class PrintPoint : public Element { public:

  PrintPoint() CLICK_COLD;
  ~PrintPoint();

  const char *class_name() const	{ return "PrintPoint"; }
  const char *port_count() const	{ return "1-2/-"; }
  const char *processing() const	{ return PUSH; }
 

  void push(int port, Packet *packet);
  //Packet *simple_action(Packet *packet);
  //uint32_t nid=0;
  //uint16_t nid;
  
};



CLICK_ENDDECLS
#endif