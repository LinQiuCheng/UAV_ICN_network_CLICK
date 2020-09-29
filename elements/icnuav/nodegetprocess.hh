#ifndef CLICK_NODEGETPROCESS_HH
#define CLICK_NODEGETPROCESS_HH
#include <click/element.hh>
#include <click/string.hh>
#include <click/hashmap.hh>
#include<map>
#include<iostream>

CLICK_DECLS


class NodeGetProcess : public Element { public:

  NodeGetProcess();
  ~NodeGetProcess();

  const char *class_name() const	{ return "NodeGetProcess"; }
  const char *port_count() const	{ return "1-2/-"; }

  void push(int port, Packet *packet);
  //Packet *simple_action(Packet *packet);
  uint16_t nid;
  
  
};



CLICK_ENDDECLS
#endif