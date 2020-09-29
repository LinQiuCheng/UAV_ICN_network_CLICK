#include<time.h>
#include <click/config.h>
#include"ForwardTable.hh"
#include"struct.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <click/etheraddress.hh>
#define EXPIRE_TIME 500
#define MAX_HOP 6

ForwardTable::ForwardTable(){
    expiretime=EXPIRE_TIME;
    memset(broadcast_addr,0xff,6);
	printf("init ForwardTable suc\n\n");
	
}

ForwardTable::~ForwardTable(){
	
}

int
ForwardTable::configure(Vector<String> &conf, ErrorHandler *errh)
{
    return Args(conf, this, errh).read_mp("NID", nid).
    read_p("SRC", EtherAddressArg(), self_mac.ether_shost).
    complete();
}


void
ForwardTable::push(int port, Packet *packet)
{

    /*读取以太帧包头*/
    click_ether ether_hd;
    click_ether record;
    memcpy(&ether_hd,(unsigned char *)packet->data(),14);
    memcpy(&record,&ether_hd,14);  //把原本的以太帧头复制一下，后面要用到源地址

    /*将以太帧头的源地址改为本节点地址*/
    memcpy(ether_hd.ether_shost,self_mac.ether_shost,6);
    


	if(port == 0){ //输入口 0 进入 该由本节点产生数据的GET包
        /*读取get包头*/
        struct  Get_Header info; 
        memcpy(&info,(unsigned char *)packet->data()+14,sizeof(info));
        uint16_t key = (uint16_t)((info.sid & 0xffff0000)>>16);  //取SID中的NID部分
        uint16_t node=info.nid;    //记录由哪个节点发来的包
        output(0).push(packet);  //先把数据包交给下一个模块进行处理，DataGen
        if(forwardtable.count(node)==0){  //没有该节点的路由
            
            forwardtable_item insert;
            insert.create_time=GetTickCount();
            insert.metric=MAX_HOP - info.ttl;
            memcpy(insert.next_hop,ether_hd.ether_shost,6);
            forwardtable.insert(std::pair< uint16_t, forwardtable_item>(node,insert));
        }
        else{
            
            std::pair<multimap_iterator,multimap_iterator> it =
                forwardtable.equal_range(node);
            if(it.first->second.create_time + EXPIRE_TIME < GetTickCount()){ //过期了
                it.first->second.create_time=GetTickCount();
                it.first->second.metric=MAX_HOP - info.ttl;
                memcpy(it.first->second.next_hop,ether_hd.ether_shost,6);
            }
            else{
                if(it.first->second.metric>=(MAX_HOP-info.ttl)){ //如果路径更好，替换  路径一样，更新路由表项生存时间
                    it.first->second.create_time=GetTickCount();
                    it.first->second.metric=MAX_HOP - info.ttl;
                    if(strncmp((char *)it.first->second.next_hop,(char *)record.ether_shost,6)!=0) //下一跳有变化
                        memcpy(it.first->second.next_hop,record.ether_shost,6);
                }
            }
            
        }

    }
    else if(port==1){      //输入口 1 进入 要转发GET包
        
        /*首先查询是否有路由可以转发GET包 */

        /*读取get包头*/
        struct  Get_Header info; 
        memcpy(&info,(unsigned char *)packet->data()+14,sizeof(info));
        uint16_t key = (uint16_t)((info.sid & 0xffff0000)>>16);  //取SID中的NID部分
        uint16_t node=info.nid;    //记录由哪个节点发来的包

        if(forwardtable.count(key)==0){ //没有到数据生产者的相关路由
            //printf("not found route\n");
            memcpy(ether_hd.ether_dhost,broadcast_addr,6);   //给予目的地址为广播地址
            memcpy((unsigned char *)packet->data(),&ether_hd,14);
            output(1).push(packet);
        }
        else{   //有到数据生产者的路由
            //printf("found route\n");
            std::pair<multimap_iterator,multimap_iterator> it =
                forwardtable.equal_range(key);
            /*
            multimap_iterator k;
            for(k = it.first; k != it.second; k++){
                
            }
            */
            if(it.first->second.create_time + EXPIRE_TIME < GetTickCount()){//过期了
                forwardtable.erase(it.first); //删除该项
                memcpy(ether_hd.ether_dhost,broadcast_addr,6); //给予目的地址为广播地址
                memcpy((unsigned char *)packet->data(),&ether_hd,14);
                printf("route over time\n");
                output(1).push(packet);
            }
            else
            {
                memcpy(ether_hd.ether_dhost,it.first->second.next_hop,6);
                memcpy((unsigned char *)packet->data(),&ether_hd,14);
                output(1).push(packet);
            }
            
            
        }

        /*通过get包学习本节点到发送get包的节点的路由*/
        
        if(node != nid){ //不是本节点自身发出的get
            forwardtable_item insert;
            if(forwardtable.count(node)==0){ //没有到该节点的路由，可以由get学习得到
                insert.create_time=GetTickCount();
                insert.metric=MAX_HOP - info.ttl;
                memcpy(insert.next_hop,record.ether_shost,6);
                forwardtable.insert(std::pair< uint16_t, forwardtable_item>(node,insert));
            }
            else{ 
                std::pair<multimap_iterator,multimap_iterator> it =
                    forwardtable.equal_range(node); 
                if(it.first->second.metric>=(MAX_HOP-info.ttl)){ //如果路径更好，替换  路径一样，更新路由表项生存时间
                    it.first->second.create_time=GetTickCount();
                    it.first->second.metric=MAX_HOP - info.ttl;
                    if(strncmp((char *)it.first->second.next_hop,(char *)record.ether_shost,6)!=0) //下一跳有变化
                        memcpy(it.first->second.next_hop,record.ether_shost,6);
                }
            }
        }
	}
	else if(port==2){  //输入口2 进入需要转发的data包

        /*读取data包头*/
        struct  Data_Header info; 
        memcpy(&info,(unsigned char *)packet->data()+14,sizeof(info));
        uint16_t key = (uint16_t)((info.sid & 0xffff0000)>>16);  //取SID中的NID部分
        uint16_t node=info.nid;    //记录由哪个节点发来的包

        memcpy((unsigned char *)packet->data(),&ether_hd,14);
        output(1).push(packet);  //先把数据包交给下一个模块进行处理

		if(node!=key) //缓存节点发来的数据
        {
            printf("it's from cache \n");

        }
        else{  //数据生产者发来的数据
            if(forwardtable.count(node)==0){
                forwardtable_item insert;
                insert.create_time=GetTickCount();
                insert.metric=MAX_HOP - info.ttl;
                memcpy(insert.next_hop,record.ether_shost,6);
                forwardtable.insert(std::pair< uint16_t, forwardtable_item>(node,insert));
            }
            else{
                std::pair<multimap_iterator,multimap_iterator> it =
                    forwardtable.equal_range(node); 
                if(it.first->second.metric>=(MAX_HOP-info.ttl)){ //如果路径更好，替换  路径一样，更新路由表项生存时间
                    it.first->second.create_time=GetTickCount();
                    it.first->second.metric=MAX_HOP - info.ttl;
                    if(strncmp((char *)it.first->second.next_hop,(char *)record.ether_shost,6)!=0) //下一跳有变化
                        memcpy(it.first->second.next_hop,record.ether_shost,6);
                }
            }
        }
           
	}
    else{     //输入口 3 进入本节点需要的DATA包
        
        
        /*读取data包头*/
        struct  Data_Header info; 
        memcpy(&info,(unsigned char *)packet->data()+14,sizeof(info));
        uint16_t key = (uint16_t)((info.sid & 0xffff0000)>>16);  //取SID中的NID部分
        uint16_t node=info.nid;    //记录由哪个节点发来的包

        
        output(2).push(packet);  //先把数据包交给下一个模块进行处理
        if(node!=key) //缓存节点发来的数据
        {
            printf("it's from cache \n");

        }
        else{  //数据生产者发来的数据
            if(forwardtable.count(node)==0){
                forwardtable_item insert;
                insert.create_time=GetTickCount();
                insert.metric=MAX_HOP - info.ttl;
                memcpy(insert.next_hop,record.ether_shost,6);
                forwardtable.insert(std::pair< uint16_t, forwardtable_item>(node,insert));
            }
            else{
                std::pair<multimap_iterator,multimap_iterator> it =
                    forwardtable.equal_range(node); 
                if(it.first->second.metric>=(MAX_HOP-info.ttl)){ //如果路径更好，替换  路径一样，更新路由表项生存时间
                    it.first->second.create_time=GetTickCount();
                    it.first->second.metric=MAX_HOP - info.ttl;
                    if(strncmp((char *)it.first->second.next_hop,(char *)record.ether_shost,6)!=0) //下一跳有变化
                        memcpy(it.first->second.next_hop,record.ether_shost,6);
                }
            }
        }
    }
}


unsigned long ForwardTable::GetTickCount(){
    struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}



CLICK_ENDDECLS
EXPORT_ELEMENT(ForwardTable)
ELEMENT_MT_SAFE(ForwardTable)