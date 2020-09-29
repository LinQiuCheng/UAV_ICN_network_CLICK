#include<time.h>
#include <click/config.h>
#include"SetNonce.hh"
#include"struct.hh"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/msg.h>
#include <errno.h>
CLICK_DECLS

SetNonce::SetNonce(){
   printf("init SetNonce suc\n\n");
  
}

SetNonce::~SetNonce(){

}
void
SetNonce::push(int port, Packet *packet)
{
    
    if(port==0){ //get包
        struct Get_Header get;
        memcpy(&get,(unsigned char *)packet->data()+14,sizeof(get));
        get.nonce=generate_nonce();
        memcpy((unsigned char *)packet->data()+14,&get,sizeof(get));
        output(0).push(packet);
    }
    else{ //control包
        struct Control ctr;
        memcpy(&ctr,(unsigned char *)packet->data()+14,sizeof(ctr));
        ctr.nonce=generate_nonce();
        memcpy((unsigned char *)packet->data()+14,&ctr,sizeof(ctr));
        output(1).push(packet);
    }
    
}

uint32_t SetNonce::generate_nonce(){
    srand(time(NULL));
    return rand();
}

CLICK_ENDDECLS
EXPORT_ELEMENT(SetNonce)
ELEMENT_MT_SAFE(SetNonce)