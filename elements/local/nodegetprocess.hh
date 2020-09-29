#ifndef CLICK_NODEGETPROCESS_HH
#define CLICK_NODEGETPROCESS_HH
#include <click/element.hh>


CLICK_DECLS


class NodeGetProcess : public Element { public:

  NodeGetProcess();
  ~NodeGetProcess() CLICK_COLD;

  const char *class_name() const	{ return "NodeGetProcess"; }
  const char *port_count() const	{ return "1-2/-"; }
  const char *processing() const	{ return PUSH; }
  int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;

  void push(int port, Packet *packet);
  //Packet *simple_action(Packet *packet);
  uint16_t nid;
  
  
};



CLICK_ENDDECLS
#endif