#include<time.h>
#include <click/config.h>
#include"nodedataprocess.hh"
#include"struct.hh"
//#include "LRUCache.hh"
CLICK_DECLS

NodeDataProcess::NodeDataProcess(){
   printf("init NodeDataProcess suc\n\n");
  // LRUCache  *lrucache = LRUCache::getInstance(20);
   // cout<<lrucache;
}

NodeDataProcess::~NodeDataProcess(){

}
void
NodeDataProcess::push(int port, Packet *packet)
{
	struct  InfoPacket data;
	memcpy(&data,(unsigned char *)packet->data()+14,9);
    printf("Receive a DATA packet/n");
	data.ttl--;
	if(data.ttl!=0)
	{
		memcpy((unsigned char *)packet->data(),&data,9);
		output(0).push(packet);
	}
	else 
		output(1).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NodeDataProcess)
ELEMENT_MT_SAFE(NodeDataProcess)