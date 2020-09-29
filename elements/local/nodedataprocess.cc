#include<time.h>
#include <click/config.h>
#include"nodedataprocess.hh"
#include"struct.hh"
#include <click/args.hh>
#include <click/error.hh>

CLICK_DECLS

NodeDataProcess::NodeDataProcess(){
   printf("init NodeDataProcess suc\n\n");
   //nid=1;
}

NodeDataProcess::~NodeDataProcess(){

}

int
NodeDataProcess::configure(Vector<String> &conf, ErrorHandler *errh)
{
    return Args(conf, this, errh).read_mp("NID", nid).complete();
}

void
NodeDataProcess::push(int port, Packet *packet)
{
	struct  Data_Header data;
	memcpy(&data,(unsigned char *)packet->data()+14,sizeof(data));
    printf("Receive a DATA packet\n");
	data.ttl--;
	if(data.ttl!=0&&((data.sid&0xffff0000)>>16)!=nid)  
	{
		
		memcpy((unsigned char *)packet->data()+14,&data,sizeof(data));
		output(0).push(packet);
    }
	else 
		output(1).push(packet);    //ttl超限或者为节点本身发出的data包，进行丢弃
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NodeDataProcess)
ELEMENT_MT_SAFE(NodeDataProcess)