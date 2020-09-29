#include<time.h>
#include <click/config.h>
#include"nodegetprocess.hh"
#include"struct.hh"
#include <click/args.hh>
#include <click/error.hh>

CLICK_DECLS

NodeGetProcess::NodeGetProcess(){
   printf("init NodeGetProcess suc\n\n");
 //  LRUCache  *lrucache = LRUCache::getInstance(20);
  //  cout<<lrucache;
  //nid=1;
}

NodeGetProcess::~NodeGetProcess(){

}

int
NodeGetProcess::configure(Vector<String> &conf, ErrorHandler *errh)
{
    return Args(conf, this, errh).read_mp("NID", nid).complete();
}

void
NodeGetProcess::push(int port, Packet *packet)
{
	struct  Get_Header get;
	memcpy(&get,(unsigned char *)packet->data()+14,sizeof(get));
    printf("Receive a GET packet\n");
	get.ttl--;
	if(get.ttl!=0&&get.nid!=nid)   
	{
			memcpy((unsigned char *)packet->data()+14,&get,sizeof(get));
			output(0).push(packet);
	}
 	else           //ttl到时  或者 为节点本身发出的包
		output(1).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NodeGetProcess)
ELEMENT_MT_SAFE(NodeGetProcess)