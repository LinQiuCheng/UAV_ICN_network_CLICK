#include<time.h>
#include <click/config.h>
#include"datareceive.hh"
#include"struct.hh"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/msg.h>
#include <errno.h>
CLICK_DECLS

DataReceive::DataReceive(){
   printf("init DataReceive suc\n\n");
  
}

DataReceive::~DataReceive(){

}
void
DataReceive::push(int port, Packet *packet)
{
    //printf("Receive wanted data packet\n");
	//uint32_t len;
   // struct Data_r data;
   
  //  memcpy(&data,(unsigned char*)packet->data()+12,12+len);
    /*struct Message_data sendMes;
    memcpy(&sendMes.sid,(unsigned char*)packet->data()+4,4);
    memcpy(&sendMes.total_len,(unsigned char*)packet->data()+8,4);
    memcpy(&sendMes.split,(unsigned  char*)packet->data()+14,2);
    memcpy(&sendMes.offset,(unsigned char*)packet->data()+16,4);
    memcpy(&sendMes.length,(unsigned char*)packet->data()+28,4);
    sendMes.msg_type=161;
    memcpy(&sendMes.data,(unsigned char*)packet->data()+32,sendMes.length);
   // printf("datareceive:  %x %x\n",sendMes.data[998],sendMes.data[999]);
    //printf("%x %d %s\n",sendMes.sid,sendMes.length,sendMes.data);
    msgsnd(msgid, (void*)&sendMes, 18+sendMes.length, 0);

    printf("Done ,send out to the client\n");
    packet->kill();*/
    output(0).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(DataReceive)
ELEMENT_MT_SAFE(DataReceive)