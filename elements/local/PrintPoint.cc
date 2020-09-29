#include<time.h>
#include <click/config.h>
#include"PrintPoint.hh"
#include"struct.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <click/etheraddress.hh>

CLICK_DECLS

PrintPoint::PrintPoint(){
   printf("init PrintPoint suc\n\n");
   //nid=1;
}

PrintPoint::~PrintPoint(){

}



void
PrintPoint::push(int port, Packet *packet)
{
   click_ether ether_hd;
   if(port==0){
     printf("receive a packet in port 0 \n");
     
     memcpy(&ether_hd,(unsigned char *)packet->data(),14);
     printf("type=%x des=%x %x\n",ether_hd.ether_type,ether_hd.ether_dhost[0],ether_hd.ether_dhost[1]);

     output(0).push(packet);
   }
   else{
     printf("receive a packet in port 1\n");
     output(0).push(packet);
   }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(PrintPoint)
ELEMENT_MT_SAFE(PrintPoint)