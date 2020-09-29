#include<time.h>
#include <click/config.h>
#include"nodecontrolprocess.hh"
#include"struct.hh"
//#include "LRUCache.hh"
CLICK_DECLS

NodeControlProcess::NodeControlProcess(){
   printf("init NodeControlProcess suc\n\n");
  // LRUCache  *lrucache = LRUCache::getInstance(20);
   //cout<<lrucache;
   nid=1;
}

NodeControlProcess::~NodeControlProcess(){

}
void
NodeControlProcess::push(int port, Packet *packet)
{
	struct  Control control;
	memcpy(&control,(unsigned char *)packet->data()+14,9);
   if(control.version_type==163)
	   printf("receive a control packet \n verType:%d nid=%d  control_type =%d\n\n",
	   control.version_type,
	   	control.nid,control.control_type);
	output(0).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NodeControlProcess)
ELEMENT_MT_SAFE(NodeControlProcess)