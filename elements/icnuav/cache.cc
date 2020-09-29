#include<time.h>
#include <click/config.h>
#include"cache.hh"
#include"struct.hh"

CLICK_DECLS

Cache::Cache(){
   printf("init Cache suc\n\n");
  
   lrucache=new LRUCache(200);
}

Cache::~Cache(){

}
void
Cache::push(int port, Packet *packet)
{
	struct  InfoPacket info;
	memcpy(&info,(unsigned char *)packet->data(),9);
    if(info.version_type==160)
    {
        printf("cache receive a GET \n");
        Value *temp;
        temp=lrucache->get(info.sid);
        if(temp==NULL){
            printf("not found the data  ,broadcast!\n");
            output(0).push(packet);
        }
        else if(temp->create_time+temp->expire_time<=time(NULL)){
            printf("over time\n");
            lrucache->DeleteNode(info.sid);
            output(0).push(packet);
        }
        else{
            printf("find it\n");
            packet->push(20+temp->length);
            Value insert;
            insert.create_time=temp->create_time;
            insert.expire_time=temp->expire_time;
            insert.length=temp->length;
            for(int i=0;i<temp->length;i++){
                insert.data[i]=temp->data[i];
            }
            memcpy((unsigned char *)packet->data()+9,&insert,20+temp->length); //封装数据
            
            info.version_type=161;                    //生成数据头部
            info.ttl=5;
            memcpy((unsigned char *)packet->data(),&info,9); 
            output(0).push(packet);   //封装好的数据包从1端口发出
        }
    }
    else {
        printf("cache receive a DATA\n");
        Value out;
        Value *add=new Value;
        uint32_t length;
        memcpy(&length,(unsigned char *)packet->data()+25,4);
        memcpy(&out,(unsigned char *)packet->data()+9,20+length);
        add->create_time=out.create_time;
        add->expire_time=out.expire_time;
        add->length=out.length;
        for(int j=0;j<length;j++){
            add->data[j]=out.data[j];
        }
        lrucache->set(info.sid,add);
        output(0).push(packet);
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(Cache)
ELEMENT_MT_SAFE(Cache)