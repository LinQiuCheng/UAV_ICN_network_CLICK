#ifndef CLICK_CACHE_HH
#define CLICK_CACHE_HH
#include <click/element.hh>
#include <click/string.hh>
#include <click/hashmap.hh>
#include<map>
#include<iostream>
#include "LRUCache.hh"
CLICK_DECLS


class Cache : public Element { public:

  Cache();
  ~Cache();

  const char *class_name() const	{ return "Cache"; }
  const char *port_count() const	{ return "1-2/-"; }

  void push(int port, Packet *packet);
  //Packet *simple_action(Packet *packet);
  //uint32_t nid;
  LRUCache *lrucache;
  
};



CLICK_ENDDECLS
#endif