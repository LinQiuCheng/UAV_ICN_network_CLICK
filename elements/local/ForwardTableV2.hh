#ifndef CLICK_FORWARDTABELV2_HH
#define CLICK_FORWARDTABELV2_HH


#include <click/element.hh>
#include <clicknet/ether.h>
#include <iostream>
#include <map>

CLICK_DECLS

typedef struct forwardtablev2_item{ //nid作为key
    unsigned long create_time; //创建时间
    uint8_t route_count;
    uint8_t metric;     //从当前路过去需要多少花费
    uint8_t	next_hop[6]; //下一跳mac地址
}forwardtablev2_item;




class ForwardTableV2 : public Element { public:

  ForwardTableV2() CLICK_COLD;
  ~ForwardTableV2();

  const char *class_name() const	{ return "ForwardTableV2"; }
  const char *port_count() const	{ return "-/-"; }
  const char *processing() const	{ return PUSH; }
  int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;


  void push(int port, Packet *packet);
  //Packet *simple_action(Packet *packet);
  //uint32_t nid;
  
  // 数据表的类

private:
  
  std::multimap<uint16_t,forwardtablev2_item> forwardtablev2;    
  typedef std::multimap<uint16_t,forwardtablev2_item>::iterator multimap_iterator;  //查询功能的迭代器

  unsigned long expiretime; // 数据项失效的时间

  uint16_t nid;
  click_ether self_mac; //保存本节点mac地址
  uint8_t broadcast_addr[6];  //广播地址
  unsigned long GetTickCount();   //get mile seconds from the start of pc
};



CLICK_ENDDECLS
#endif