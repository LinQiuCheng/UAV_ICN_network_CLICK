#include<time.h>
#include <click/config.h>
#include"PitTableV2.hh"
#include"struct.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <click/etheraddress.hh>
#define EXPIRE_TIME 1

PitTableV2::PitTableV2(){
    expiretime=EXPIRE_TIME;
	printf("init PitTableV2 suc\n\n");
	
}

PitTableV2::~PitTableV2(){
	
}

int
PitTableV2::configure(Vector<String> &conf, ErrorHandler *errh)
{
    return Args(conf, this, errh).read_mp("NID", nid).
    read_p("SRC", EtherAddressArg(), self_mac.ether_shost).
    complete();
}


void
PitTableV2::push(int port, Packet *packet)
{

    /*读取以太帧包头*/
    click_ether ether_hd;
    memcpy(&ether_hd,(unsigned char *)packet->data(),14);

	if(port==0){      //输入口 0 进入 GET包
        /*读取icn包头*/
        struct  Get_Header info; 
	    memcpy(&info,(unsigned char *)packet->data()+14,sizeof(info));
        printf("PIT receive a GET\n");
        if(((info.sid&0xffff0000)>>16)==nid) { //获取本节点数据的GET
            if(pittablev2.count(info.sid)==0) // 没有找到相同sid的表项
            {
                ValueNode2 add;
                add.split=info.split;
                add.offset=info.offset;
                add.create_time=time(NULL);
                add.nonce=info.nonce;
                memcpy(add.ether_shost,ether_hd.ether_shost,6);
                pittablev2.insert(std::pair< uint32_t, ValueNode2>(info.sid,add));
                printf("Not find the PIT item about it ,trans to self app\n");
                
                /*将源地址改为本节点mac地址*/
                memcpy(ether_hd.ether_shost,self_mac.ether_shost,6);
                memcpy((unsigned char *)packet->data(),&ether_hd,14);
                
                output(4).push(packet);   //转发get包到上层应用
            }
            else{
                std::pair<multimap_iterator,multimap_iterator> it = 
                            pittablev2.equal_range(info.sid);   //使用迭代器找出所有结果
                multimap_iterator k;
                for(k = it.first; k != it.second; k++)
                {
                    if(k->second.offset==info.offset&&
                            strncmp((char *)k->second.ether_shost,(char *)ether_hd.ether_shost,6)==0&&
                            k->second.split==info.split&&k->second.nonce==info.nonce){  //偏移量相等、请求者相同、是否分片字段、随机数相等
                        break;   
                    }
                }
                if(k==it.second){ //没有重复项
                    ValueNode2 add;
                    add.split=info.split;
                    add.offset=info.offset;
                    add.create_time=time(NULL);
                    add.nonce=info.nonce;
                    memcpy(add.ether_shost,ether_hd.ether_shost,6);
                    pittablev2.insert(std::pair< uint32_t, ValueNode2>(info.sid,add));
                    /*将源地址改为本节点mac地址*/
                    memcpy(ether_hd.ether_shost,self_mac.ether_shost,6);
                    memcpy((unsigned char *)packet->data(),&ether_hd,14);
                
                    output(4).push(packet); //将get包传给上层
                }
                else{ //找到重复项
                    if(k->second.create_time+expiretime<=time(NULL)){ //表项超时
                        k->second.create_time = time(NULL);
                        /*将源地址改为本节点mac地址*/
                        memcpy(ether_hd.ether_shost,self_mac.ether_shost,6);
                        memcpy((unsigned char *)packet->data(),&ether_hd,14);

                        output(4).push(packet);  //将get包传给上层
                    }
                    else   
                        output(3).push(packet);    //丢弃该包
                }
            }
        }
        else{  //获取其他节点数据的GET
            if(pittablev2.count(info.sid)==0) // 没有找到相同sid的表项
            {
                ValueNode2 add;
                add.split=info.split;
                add.offset=info.offset;
                add.create_time=time(NULL);
                add.nonce=info.nonce;
                memcpy(add.ether_shost,ether_hd.ether_shost,6);
                pittablev2.insert(std::pair< uint32_t, ValueNode2>(info.sid,add));
                printf("Not find the PIT item about it \n");
                
                /*将源地址改为本节点mac地址*/
                memcpy(ether_hd.ether_shost,self_mac.ether_shost,6);
                memcpy((unsigned char *)packet->data(),&ether_hd,14);
                
                output(0).push(packet);   //转发get包
            }
            else{
                std::pair<multimap_iterator,multimap_iterator> it = 
                                pittablev2.equal_range(info.sid);   //使用迭代器找出所有结果
                multimap_iterator k;
                for(k = it.first; k != it.second; k++)
                {
                    if(k->second.offset==info.offset&&
                            strncmp((char *)k->second.ether_shost,(char *)ether_hd.ether_shost,6)==0&&
                            k->second.split==info.split&&k->second.nonce==info.nonce){  //偏移量相等、请求者相同、是否分片字段相等
                        break;   
                    } 
                }
                if(k==it.second){ //没有重复项
                    ValueNode2 add;
                    add.split=info.split;
                    add.offset=info.offset;
                    add.create_time=time(NULL);
                    add.nonce=info.nonce;
                    memcpy(add.ether_shost,ether_hd.ether_shost,6);
                    pittablev2.insert(std::pair< uint32_t, ValueNode2>(info.sid,add));
                    
                    
                    /*将源地址改为本节点mac地址*/
                    memcpy(ether_hd.ether_shost,self_mac.ether_shost,6);
                    memcpy((unsigned char *)packet->data(),&ether_hd,14);
                    
                    
                    output(0).push(packet);
                }
                else{ //找到重复项
                    if(k->second.create_time+expiretime<=time(NULL)){ //表项超时
                        k->second.create_time = time(NULL);
                        /*将源地址改为本节点mac地址*/
                        memcpy(ether_hd.ether_shost,self_mac.ether_shost,6);
                        memcpy((unsigned char *)packet->data(),&ether_hd,14);

                        output(0).push(packet);
                    }
                    else 
                        output(3).push(packet);    //丢弃该包
                }
            }
        }
		
	}
	else if(port==1){  //输入口1 进入data包
        struct Data_Header info;
        memcpy(&info,(unsigned char *)packet->data()+14,sizeof(info));
		printf("PIT receive a DATA sid=%x\n",info.sid);
		//printf("PIT receive a DATA sid=%x\n",info.sid);
        if(pittablev2.count(info.sid)==0){ //没找到该项 ，将该包丢弃 
            printf("not find item discard\n"); 
            output(1).push(packet);
        }
        else{
            std::pair<multimap_iterator,multimap_iterator> it = 
                           pittablev2.equal_range(info.sid);   //使用迭代器找出所有结果
            int flag1=0,flag2=0; //记录是否
            if(info.split==0&&info.offset==0){ //无分片的数据包
                printf("Not fragment\n");
                
                while(it.first!=it.second){
                    String _data="";
                    Packet * packet1 = Packet::make(Packet::default_headroom, 
                    _data.data(), _data.length(), 0);     
                    //uint32_t data_len;
                    //memcpy(&data_len,(unsigned char*)packet->data()+14+sizeof(info)+8,4);
                    packet1->push(14+sizeof(info)+info.length);
                    memcpy((unsigned char*)packet1->data(),(unsigned char*)packet->data(),14+sizeof(info)+info.length);
                    if(it.first->second.offset==0&&it.first->second.split==0){
                        if(strcmp((char*)it.first->second.ether_shost,(char *)self_mac.ether_shost)==0){   // 节点本身请求的data
                            output(2).push(packet1);
                        }
                        else{ //其他节点请求的data
                            memcpy((unsigned char*)packet1->data(),it.first->second.ether_shost,6); //将目的地址写入
                            memcpy((unsigned char*)packet1->data()+6,self_mac.ether_shost,6); //将源地址写入
                            output(0).push(packet1);
                        }
                        it.first=pittablev2.erase(it.first);
                        continue;
                    }
                    it.first++;
                }
            }
            else{   //分片的数据包
                printf("Data Fragment : offset=%d\n",info.offset);
                while(it.first!=it.second){
                    if(it.first->second.offset==info.offset&&it.first->second.split==info.split){//请求单片的表项
                        String _data="";
                        Packet * packet1 = Packet::make(Packet::default_headroom, 
                        _data.data(), _data.length(), 0);     
                       // uint32_t data_len;
                        //memcpy(&data_len,(unsigned char*)packet->data()+14+sizeof(info)+8,4);
                        packet1->push(14+sizeof(info)+info.length);
                        memcpy((unsigned char*)packet1->data(),(unsigned char*)packet->data(),14+sizeof(info)+info.length);
                        
                        if(strcmp((char*)it.first->second.ether_shost,(char *)self_mac.ether_shost)==0){   // 节点本身请求的data
                            output(2).push(packet1); //传给datareceive 数据接收模块
                        }
                        else{ //其他节点请求的data
                            memcpy((unsigned char*)packet1->data(),it.first->second.ether_shost,6); //将目的地址写入
                            memcpy((unsigned char*)packet1->data()+6,self_mac.ether_shost,6); //将源地址写入
                            output(0).push(packet1);
                        }
                        it.first=pittablev2.erase(it.first);
                        continue;
                    }
                    else if(it.first->second.offset==0&&it.first->second.split==0){ //请求整个内容的表项
                        String _data="";
                        Packet * packet1 = Packet::make(Packet::default_headroom, 
                        _data.data(), _data.length(), 0);     //复制出另一个数据包，这样就有两个包发向两个口
                        //uint32_t data_len;
                        //memcpy(&data_len,(unsigned char*)packet->data()+14+sizeof(info)+8,4);
                        packet1->push(14+sizeof(info)+info.length);
                        memcpy((unsigned char*)packet1->data(),(unsigned char*)packet->data(),14+sizeof(info)+info.length);
                        if(strcmp((char*)it.first->second.ether_shost,(char *)self_mac.ether_shost)==0){   // 节点本身请求的data
                            output(2).push(packet1); //传给datareceive 数据接收模块
                        }
                        else{ //其他节点请求的data
                            memcpy((unsigned char*)packet1->data(),it.first->second.ether_shost,6); //将目的地址写入
                            memcpy((unsigned char*)packet1->data()+6,self_mac.ether_shost,6); //将源地址写入
                            output(0).push(packet1);
                        }
                    }
                    it.first++;
                }
            }
            output(1).push(packet);//将原本的包丢弃
        }	
	}
    else{  //cache发来的完整包sid信息，消除pit表中相应项
        uint32_t SID;
        memcpy(&SID,(unsigned char*)packet->data(),4);
        std::pair<multimap_iterator,multimap_iterator> it = 
                           pittablev2.equal_range(SID);   //使用迭代器找出所有结果
        while(it.first!=it.second){
            if(it.first->second.offset==0&&it.first->second.split==0){
                it.first=pittablev2.erase(it.first);
                continue;
            }
            it.first++;
        }
        packet->kill();
    }
}





CLICK_ENDDECLS
EXPORT_ELEMENT(PitTableV2)
ELEMENT_MT_SAFE(PitTableV2)