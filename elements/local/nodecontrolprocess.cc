#include<time.h>
#include <click/config.h>
#include"nodecontrolprocess.hh"
#include"struct.hh"
#include <click/args.hh>
#include <click/error.hh>

CLICK_DECLS

NodeControlProcess::NodeControlProcess(){
   printf("init NodeControlProcess suc\n\n");
  // LRUCache  *lrucache = LRUCache::getInstance(20);
   //cout<<lrucache;
   //printf("%d\n",nid);
}

NodeControlProcess::~NodeControlProcess(){

}

int
NodeControlProcess::configure(Vector<String> &conf, ErrorHandler *errh)
{
    return Args(conf, this, errh).read_mp("NID", nid).complete();
}


void
NodeControlProcess::push(int port, Packet *packet)
{
	struct  Control control;
	memcpy(&control,(unsigned char *)packet->data(),sizeof(control));
   control.ttl--;
   if(control.ttl!=0){
      printf("receive a control packet \n verType:%d nid=%d  control_type =%d\n\n",
      control.version_type,
      control.nid,control.control_type);
      if(control.msgtype==0) //控制信息
      {
         if(control.c_nid==nid)//本节点发出的控制信息 ，直接丢弃
         {
            printf("receive self send control msg,discard \n");
            output(1).push(packet);
         }
         else{
            output(0).push(packet);
         }
      }
      else{   //回应信息
         if(control.nid==nid)  //本节点发出的回应消息，直接丢弃
         {
            printf("receive self send reply msg,discard \n");
            output(1).push(packet);
         }
         else{
            output(0).push(packet);
         }
      }
	   
   }
   else //ttl失效
   {
      output(1).push(packet);
   }
   
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NodeControlProcess)
ELEMENT_MT_SAFE(NodeControlProcess)