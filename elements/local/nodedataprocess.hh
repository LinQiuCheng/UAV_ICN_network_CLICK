#ifndef CLICK_NODEDATAPROCESS_HH
#define CLICK_NODEDATAPROCESS_HH
#include <click/element.hh>


CLICK_DECLS



class NodeDataProcess : public Element { public:

  NodeDataProcess() CLICK_COLD;
  ~NodeDataProcess();

  const char *class_name() const	{ return "NodeDataProcess"; }
  const char *port_count() const	{ return "1-2/-"; }
  const char *processing() const	{ return PUSH; }
  int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;

  void push(int port, Packet *packet);
  //Packet *simple_action(Packet *packet);
  //uint32_t nid=0;
  uint16_t nid;
  
};



CLICK_ENDDECLS
#endif