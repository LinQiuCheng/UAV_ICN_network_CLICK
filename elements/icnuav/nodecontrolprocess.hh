#ifndef CLICK_NODECONTROLPROCESS_HH
#define CLICK_NODECONTROLPROCESS_HH
#include <click/element.hh>
#include <click/string.hh>
#include <click/hashmap.hh>
#include<map>
#include<iostream>

CLICK_DECLS


class NodeControlProcess : public Element { public:

  NodeControlProcess();
  ~NodeControlProcess();

  const char *class_name() const	{ return "NodeControlProcess"; }
  const char *port_count() const	{ return "1-2/-"; }

  void push(int port, Packet *packet);
  //Packet *simple_action(Packet *packet);
  uint16_t nid;
  
  
};



CLICK_ENDDECLS
#endif