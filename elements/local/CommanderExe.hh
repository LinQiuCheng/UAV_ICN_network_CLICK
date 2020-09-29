#ifndef CLICK_COMMANDEREXE_HH
#define CLICK_COMMANDEREXE_HH
#include <iostream>
#include <click/element.hh>
#include <unistd.h>//Linux系统下网络通讯的头文件集合
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <fcntl.h>

CLICK_DECLS


class CommanderExe : public Element { public:

  CommanderExe();
  ~CommanderExe() CLICK_COLD;

  const char *class_name() const	{ return "CommanderExe"; }
  const char *port_count() const	{ return "1-2/-"; }
  const char *processing() const	{ return PUSH; }
  //int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;

  void push(int port, Packet *packet);
  
};

struct control_cmd
{
  uint16_t control_type; //控制类型
  uint16_t reserved; //为了对齐的填充字段
  uint32_t param[4]; //飞控参数
};


CLICK_ENDDECLS
#endif