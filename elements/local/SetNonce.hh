#ifndef CLICK_SETNONCE_HH
#define CLICK_SETNONCE_HH
#include <click/element.hh>


CLICK_DECLS



class SetNonce : public Element { public:

  SetNonce();
  ~SetNonce();

  const char *class_name() const	{ return "SetNonce"; }
  const char *port_count() const	{ return "2/2"; }
  //const char *processing() const	{ return PUSH; }
  
  void push(int port, Packet *packet);
  //Packet *simple_action(Packet *packet)

  uint32_t generate_nonce();
  
  
};

CLICK_ENDDECLS
#endif