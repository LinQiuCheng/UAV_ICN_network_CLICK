#ifndef CLICK_PITTABLE_HH
#define CLICK_PITTABLE_HH


#include <click/element.hh>
#include <clicknet/ether.h>
#include <iostream>
#include <map>

CLICK_DECLS

typedef struct ValueNode{
    uint32_t create_time; //创建时间
    uint16_t split;      //是否有分片，后面有分片则为1
    uint32_t offset;    //偏移量
    uint32_t nonce;
    uint8_t	ether_shost[6];
}ValueNode;




class PitTable : public Element { public:

  PitTable() CLICK_COLD;
  ~PitTable();

  const char *class_name() const	{ return "PitTable"; }
  const char *port_count() const	{ return "-/-"; }
  const char *processing() const	{ return PUSH; }
  int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;


  void push(int port, Packet *packet);
  //Packet *simple_action(Packet *packet);
  //uint32_t nid;
  
  // 数据表的类

private:
  
  std::multimap<uint32_t,ValueNode> pitTable;    //pit表存储，采用multimap
  typedef std::multimap<uint32_t,ValueNode>::iterator multimap_iterator;  //查询功能的迭代器

  uint32_t expiretime; // 数据项失效的时间

  uint16_t nid;
  click_ether self_mac; //保存本节点mac地址
};



CLICK_ENDDECLS
#endif