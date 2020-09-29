#include<time.h>
#include <click/config.h>
#include"nodegetprocess.hh"
#include"struct.hh"
//#include "LRUCache.hh"
CLICK_DECLS

NodeGetProcess::NodeGetProcess(){
   printf("init NodeGetProcess suc\n\n");
 //  LRUCache  *lrucache = LRUCache::getInstance(20);
  //  cout<<lrucache;
  nid=1;
}

NodeGetProcess::~NodeGetProcess(){

}
void
NodeGetProcess::push(int port, Packet *packet)
{
	struct  InfoPacket get;
	memcpy(&get,(unsigned char *)packet->data(),9);
    printf("Receive a GET packet/n");
	get.ttl--;
	if(get.ttl!=0&&get.nid!=nid)   
	{
		memcpy((unsigned char *)packet->data(),&get,9);
		output(0).push(packet);
	}
	else           //ttl到时  或者 为节点本身发出的包
		output(1).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NodeGetProcess)
ELEMENT_MT_SAFE(NodeGetProcess)