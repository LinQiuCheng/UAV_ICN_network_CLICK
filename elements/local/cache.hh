#ifndef CLICK_CACHE_HH
#define CLICK_CACHE_HH
#include <click/element.hh>
#include <clicknet/ether.h>
#include <iostream>
#include <unordered_map>


CLICK_DECLS

struct Value{
    uint32_t create_time; //创建时间
    uint32_t expire_time; //有效时间
    uint32_t length;    //数据长度
    uint16_t split;      //是否有分片，后面有分片则为1
    uint16_t reserve;    //保留字段，为了字节对齐
    uint32_t offset;    //偏移量
    Value *next;   //指向下一个相同sid的节点
    uint8_t data[1400];
    
};
// 双向链表的节点结构
struct LRUCacheNode {
    uint32_t key;   // sid  
    int flag; //是否分片   0表示没分，1表示有分片
    uint32_t toltal_len;//总长度
    uint32_t current_len; //目前长度
    Value *value;
    LRUCacheNode* prev; 
    LRUCacheNode* next;
    LRUCacheNode():key(0),value(NULL),prev(NULL),next(NULL){}
};

class Cache : public Element { public:

  Cache() CLICK_COLD;
  ~Cache();

  const char *class_name() const	{ return "Cache"; }
  const char *port_count() const	{ return "1-2/-"; }
  const char *processing() const	{ return PUSH; }
  int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;
  
  void push(int port, Packet *packet);
  //Packet *simple_action(Packet *packet);
  //uint32_t nid;
  Value* get( uint32_t key);             // 查询数据项
  void set( uint32_t key, Value *value,int flag, uint32_t totol_len,uint32_t current_len ); // 未满时插入,已满时替换
  void DeleteNode(uint32_t key);
  int getFlag(uint32_t key);   //获取是否分片的标志
  uint32_t getTotalLen(uint32_t key);   // 获取当条cache总长度
  uint32_t getCurrentLen(uint32_t key); //获取当前长度
  void setCurrentLen(uint32_t key,uint32_t len); //设置当前长度
private:
  void removeLRUNode();                 // 删除尾结点（最久未使用）
  void detachNode(LRUCacheNode* node);    // 分离当前结点
  void insertToFront(LRUCacheNode* node); // 节点插入到头部
  std::unordered_map< uint32_t, LRUCacheNode*> m;  // 代替hash_map
  LRUCacheNode* head;     // 指向双链表的头结点
  LRUCacheNode* tail;     // 指向双链表的尾结点
  int capacity;           // Cache的容量
  int count;              // 计数
  uint16_t nid;     // 节点标识
  click_ether self_mac;
};



CLICK_ENDDECLS
#endif