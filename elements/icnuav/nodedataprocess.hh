#ifndef CLICK_NODEDATAPROCESS_HH
#define CLICK_NODEDATAPROCESS_HH
#include <click/element.hh>
#include <click/string.hh>
#include <click/hashmap.hh>
#include<map>
#include<iostream>

CLICK_DECLS


class NodeDataProcess : public Element { public:

  NodeDataProcess();
  ~NodeDataProcess();

  const char *class_name() const	{ return "NodeDataProcess"; }
  const char *port_count() const	{ return "1-2/-"; }

  void push(int port, Packet *packet);
  //Packet *simple_action(Packet *packet);
  //uint32_t nid=0;
  
  
};



CLICK_ENDDECLS
#endif