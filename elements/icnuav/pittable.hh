#ifndef CLICK_PITTABLE_HH
#define CLICK_PITTABLE_HH

#define LIST_INT_SIZE 100 // 线性表的初始空间
#define LIST_INCREAMENT 10 // 增量
#include <click/element.hh>
#include <click/string.hh>
#include <click/hashmap.hh>
#include<map>
#include<iostream>
#include "simple_pit_table.hh"
CLICK_DECLS




class PitTable : public Element { public:

  PitTable();
  ~PitTable();

  const char *class_name() const	{ return "PitTable"; }
  const char *port_count() const	{ return "1-2/-"; }

  void push(int port, Packet *packet);
  //Packet *simple_action(Packet *packet);
  //uint32_t nid;
  
  PITTable *table;     // 数据表的类
  uint64_t expiretime; // 数据项失效的时间
  
};



CLICK_ENDDECLS
#endif