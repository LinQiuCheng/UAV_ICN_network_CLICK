#ifndef CLICK_TASKTABLE_HH
#define CLICK_TASKTABLE_HH


#include <click/element.hh>
#include <iostream>
#include <map>

CLICK_DECLS

typedef struct key_Struct{        //多个关键字作为key
  uint16_t c_nid; //发出该控制指令的节点标识
	uint16_t nid;   //需要控制的节点标识
	uint16_t control_type; //控制类型
  uint16_t reserve; //保留字段，为了对齐
	uint32_t nonce; //区别相同指令
  bool operator<(const key_Struct& k)const {   //重载 < 运算符
    return (c_nid < k.c_nid)||(c_nid==k.c_nid && nid<k.nid)||
    (c_nid==k.c_nid && nid==k.nid && control_type<k.control_type)||
    (c_nid==k.c_nid && nid==k.nid && control_type==k.control_type && nonce<k.nonce);
  }
}key_Struct;

typedef struct AdditionalValue{
  uint32_t create_time; //创建时间
  uint32_t param[4]; //飞控参数
}AdditionalValue;




class TaskTable: public Element { public:

  TaskTable() CLICK_COLD;
  ~TaskTable();

  const char *class_name() const	{ return "TaskTable"; }
  const char *port_count() const	{ return "-/-"; }
  const char *processing() const	{ return PUSH; }
  int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;


  void push(int port, Packet *packet);
  //Packet *simple_action(Packet *packet);
  //uint32_t nid;
  
  // 数据表的类
  
  std::map<key_Struct,AdditionalValue> taskTable;    //pit表存储，采用multimap
  typedef std::map<key_Struct,AdditionalValue>::iterator iterator;  //查询功能的迭代器

  uint32_t expiretime; // 数据项失效的时间

  uint16_t nid;

  bool IsInTable(key_Struct key);
  void AddInTable(key_Struct key,AdditionalValue value);
  void DeleteInTable(key_Struct key);
};



CLICK_ENDDECLS
#endif