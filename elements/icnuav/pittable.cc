#include<time.h>
#include <click/config.h>
#include"pittable.hh"
#include"struct.hh"

CLICK_DECLS

PitTable::PitTable(){

   printf("init PitTable suc\n\n");
   table=new PITTable(200);
   expiretime=20;
}

PitTable::~PitTable(){

}
void
PitTable::push(int port, Packet *packet)
{
	struct  InfoPacket info;
	memcpy(&info,(unsigned char *)packet->data(),9);
	if(port==0){      //输入口 0 进入 GET包
        printf("PIT receive a GET\n");
        Value *temp;
		temp=table->get(info.sid);
		if(temp==NULL){   //PIT表中没有该项
			printf("Not find in PIT table\n");
			temp->create_time=time(NULL);
			table->set(info.sid,temp);
			output(0).push(packet);
		}
        else if(temp->create_time+expiretime<time(NULL)){ //数据项过期
			printf("PIT table node over time\n");
			temp->create_time=time(NULL);
			output(0).push(packet);
		}
		else{
			output(1).push(packet);  // 有该表项且没过期，则将get包丢弃
		}
	}
	else if(port==1){
		printf("PIT receive a DATA\n");
		Value *temp;
		temp=table->get(info.sid);
		if(temp==NULL){   //PIT表中没有该项 ，丢弃data包
			printf("data:Not find in PIT table\n");
			output(1).push(packet);
		}
        else if(temp->create_time+expiretime<time(NULL)){ //数据项过期，丢弃data包
			printf("data:PIT table node over time\n");
			table->DeleteNode(info.sid);
			output(1).push(packet);
		}
		else{
            table->DeleteNode(info.sid);
			output(0).push(packet);  // 有该表项且没过期，则将data包发出
		}
	}
}


CLICK_ENDDECLS
EXPORT_ELEMENT(PitTable)
ELEMENT_MT_SAFE(PitTable)