#ifndef CLICK_NODECONTROLPROCESS_HH
#define CLICK_NODECONTROLPROCESS_HH
#include <click/element.hh>



CLICK_DECLS


class NodeControlProcess : public Element { public:

  NodeControlProcess() CLICK_COLD;
  ~NodeControlProcess();

  const char *class_name() const	{ return "NodeControlProcess"; }
  const char *port_count() const	{ return "1-2/-"; }
  const char *processing() const	{ return PUSH; }
  int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;
  
  void push(int port, Packet *packet);
  //Packet *simple_action(Packet *packet);
  uint16_t nid;
  
  
};



CLICK_ENDDECLS
#endif