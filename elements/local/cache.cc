#include <time.h>
#include <click/config.h>
#include"cache.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <click/etheraddress.hh>
#include "struct.hh"

CLICK_DECLS

Cache::Cache(){
    printf("init Cache suc\n\n");
    //this->capacity = CACHE_SIZE;
    this->count = 0;
    head = new LRUCacheNode;
    tail = new LRUCacheNode;
    head->prev = NULL;
    head->next = tail;
    tail->prev = head;
    tail->next = NULL;
    
}

Cache::~Cache(){
    delete head;
    delete tail;
}

int
Cache::configure(Vector<String> &conf, ErrorHandler *errh)
{
    return Args(conf, this, errh).read_mp("NID", nid).
    read_mp("CACHE_SIZE",capacity).
    read_p("SRC", EtherAddressArg(), self_mac.ether_shost).
    complete();
}

void
Cache::push(int port, Packet *packet)
{
    click_ether ehead;
    memcpy(&ehead,(unsigned char *)packet->data(),14);
    if(port==0)  //0口进入GET包
    {
        //printf("cache receive a GET \n");
        struct Get_Header info;
        memcpy(&info,(unsigned char *)packet->data()+14,sizeof(info));
        Value *temp;
        temp=this->get(info.sid);
        if(temp==NULL){  //没找到
            //printf("not found the data \n");
            output(0).push(packet);  //传送get到pit
        }
        else if((temp->create_time+temp->expire_time<=time(NULL))){  //超时了
            //printf("over time\n");
            this->DeleteNode(info.sid);
            output(0).push(packet);   //传送get到pit
        }
        else{  //找到了相关sid条目
            printf("find it location\n");
            if(info.split==1||info.offset!=0) //查找分片单片的get包
            {
                while(temp!=NULL){
                    if(temp->offset==info.offset){
                        /*生成data包*/
                        struct Data_Header data; 
                        String _data="";
                        Packet* p= Packet::make(Packet::default_headroom, 
                            _data.data(), _data.length(), 0);
                        p->push(14+sizeof(data)+temp->length);
                        //packet->push(12+temp->length);
                        data.create_time=temp->create_time;
                        data.expire_time=temp->expire_time;
                        data.length=temp->length;
                        memcpy(&data,&info,20); //前20字节字段相同
                        data.nid=nid; //标识为本节点发送的data
                        data.version_type=161;                    //生成数据头部
                        data.ttl=5;
                        memcpy((unsigned char *)p->data()+14,&data,sizeof(data));
                        memcpy((unsigned char *)p->data()+14+sizeof(data),temp->data
                        ,temp->length); //封装数据字段data
                        packet->kill(); //销毁get包
                        if(nid==info.nid){ //本节点自身请求数据
                            output(3).push(p); //发出data包到 datareceive
                        }
                        else
                        {
                            //info.nid=nid;//标识为本节点发送的data
                            //memcpy((unsigned char *)packet->data()+14,&info,sizeof(info));
                            memcpy(ehead.ether_dhost,ehead.ether_shost,6); //将目的地址写入
                            memcpy(ehead.ether_shost,self_mac.ether_shost,6); //将源地址写入
                            memcpy((unsigned char *)p->data(),&ehead,14);
                            output(4).push(p); //发送给传来的接口
                        }
                        break;
                    }
                    temp=temp->next;
                }
                if(temp==NULL) //没有找到该分片
                {
                    printf("not found the data  \n");
                    output(0).push(packet); //将get发给pit
                }
            }
            else{         //查找整片内容的get包
                if(this->getFlag(info.sid)==0) //该内容没有分片
                {
                    /*生成data包*/
                    struct Data_Header data; 
                    String _data="";
                    Packet* p= Packet::make(Packet::default_headroom, 
                        _data.data(), _data.length(), 0);
                    p->push(14+sizeof(data)+temp->length);
                    //packet->push(12+temp->length);
                    data.create_time=temp->create_time;
                    data.expire_time=temp->expire_time;
                    data.length=temp->length;
                    memcpy(&data,&info,20); //前20字节字段相同
                    data.nid=nid; //标识为本节点发送的data
                    data.version_type=161;                    //生成数据头部
                    data.ttl=5;
                    memcpy((unsigned char *)p->data()+14,&data,sizeof(data));
                    memcpy((unsigned char *)p->data()+14+sizeof(data),temp->data
                    ,temp->length); //封装数据字段data
                    packet->kill(); //销毁get包

                    if(nid==info.nid){  //本节点请求的数据
                        output(3).push(p); //发出data包到 datareceive
                    }
                    else
                    {
                        //info.nid=nid; //标识为本节点发送的数据 
                        // memcpy((unsigned char *)packet->data()+14,&info,sizeof(info));  
                        memcpy(ehead.ether_dhost,ehead.ether_shost,6); //将目的地址写入
                        memcpy(ehead.ether_shost,self_mac.ether_shost,6); //将源地址写入
                        memcpy((unsigned char *)p->data(),&ehead,14);
                        output(4).push(p); //发送给传来的接口
                    }
                }
                else{   //分片了
                    if(this->getCurrentLen(info.sid)==this->getTotalLen(info.sid)){ //分片是完整的
                        printf("The data fragment is  completed \n");
                        struct Data_Header data; 
                        info.version_type=161;                    //生成数据头部
                        info.ttl=5;
                        info.total_len=this->getTotalLen(info.sid);
                        memcpy(&data,&info,20); //前20字节相同
                        while(temp!=NULL){ //封装成一个个data包发出
                            String _data="";
                            Packet* p= Packet::make(Packet::default_headroom, 
                                _data.data(), _data.length(), 0);
                            p->push(14+sizeof(data)+temp->length);
                            data.offset=temp->offset;
                            data.split=temp->split;
                            data.nid=nid;
                            memcpy((unsigned char *)p->data()+14,&data,sizeof(data));
                            //memcpy((unsigned char *)p->data()+14+sizeof(info),temp,12);
                            memcpy((unsigned char *)p->data()+14+sizeof(data),
                             temp->data,temp->length);
                            if(nid==info.nid){
                                output(3).push(p); //发出data包到 datareceive
                            }
                            else
                            {
                                memcpy(ehead.ether_dhost,ehead.ether_shost,6); //将目的地址写入
                                memcpy(ehead.ether_shost,self_mac.ether_shost,6); //将源地址写入
                                memcpy((unsigned char *)p->data(),&ehead,14);
                                output(4).push(p); //发送给传来的接口
                            }
                            temp=temp->next;
                        }
                        packet->kill();
                    }
                    else{ //分片不完整，将get发送到pit
                        printf("The data is not completed \n");
                        output(0).push(packet);
                    }
                }
            }
                                                                     
        }
    }
    else { //1口进入DATA包
        printf("cache receive a DATA\n");
        struct Data_Header data;
        memcpy(&data,(unsigned char *)packet->data()+14,sizeof(data));
        Value *add=this->get(data.sid);
        if(add==NULL){ //没有该项
            printf("Create a cache item for SID=%x\n",data.sid);
            Value *insert=new Value;
            int flag=0;
            if(data.split==1||data.offset!=0) flag=1;
            insert->expire_time=data.expire_time;
            insert->length=data.length;
            insert->create_time=time(NULL); //主要是由于各主机之间时间不同步才写的
            insert->offset=data.offset;
            insert->split=data.split;
            memcpy(insert->data,(unsigned char *)packet->data()+14+sizeof(data),insert->length);
            //printf("cache:  %x %x\n",insert->data[998],insert->data[999]);
            //printf("ok2 \n");
            insert->next=NULL;
            this->set(data.sid,insert,flag,data.total_len,insert->length);
            output(1).push(packet);  //转发data给pit
        }
        else{  //已有相应sid项
            if(getFlag(data.sid)==0){  //该项无分片
                printf("It's not fragmention item\n");
                add->expire_time=data.expire_time;
                add->create_time=time(NULL);
                add->length=data.length;
                memcpy(add->data,(unsigned char *)packet->data()+14+sizeof(data),add->length);
                //output(1).push(packet);  //将data发给pit
            }
            else{ //该项有分片
                printf("Fragment offset=%d\n",data.offset);
                Value *pre=add;
                while(add!=NULL){
                    if(add->offset==data.offset) break;
                    pre=add;
                    add=add->next;
                }
                if(add==NULL){ //cache中原本没有该片
                    Value *insert=new Value;
                    insert->expire_time=data.expire_time;
                    insert->length=data.length;
                    insert->create_time=time(NULL); //主要是由于各主机之间时间不同步才写的
                    insert->offset=data.offset;
                    insert->split=data.split;
                    
                    memcpy(insert->data,(unsigned char *)packet->data()+14+sizeof(data),insert->length);
                    //printf("cache : %x %x\n",insert->data[998],insert->data[999]);
                    insert->next=NULL;
                    pre->next=insert;     //连接上新插入的片
                    
                    uint32_t len=getCurrentLen(data.sid)+insert->length; //修改当前数据的长度
                    setCurrentLen(data.sid,len);
                    printf("cur=%d,total=%d\n",getCurrentLen(data.sid),getTotalLen(data.sid));
                    if(getCurrentLen(data.sid)==getTotalLen(data.sid)){ //该项内容已完整，向pit发送清除表项信息
                        String _data="";
                        Packet* p= Packet::make(Packet::default_headroom, 
                                    _data.data(), _data.length(), 0);  //封装一个包发送给pit 
                                                                    //消除pit该表项
                        p->push(4);
                        uint32_t SID=data.sid;
                        memcpy((unsigned char *)packet->data(),&SID,4); 
                        printf("cache send delete item to pit\n");
                        output(2).push(p); 
                    }
                }
                else{  //cache中已有该片
                    /*更新内容*/
                    memcpy(add->data,(unsigned char *)packet->data()+14+sizeof(data),add->length);
                }
            }
            output(1).push(packet);  //转发data给pit
        }


        
    }
}

Value * Cache::get( uint32_t key)
{
    if(m.find(key) == m.end())  // 没找到
        return NULL;
    else
    {
        LRUCacheNode* node = m[key];
        detachNode(node);      // 命中，移至头部
        insertToFront(node);
        return node->value;
    }
}

void Cache::set( uint32_t key, Value *value, int flag, uint32_t totol_len,uint32_t current_len )
{
        LRUCacheNode* node = new LRUCacheNode;
        if(count == capacity)   // Cache已满
        {
            printf("cache is full,delete lrunode\n");
            removeLRUNode();
        }
        node->key = key;
        node->value = value;
        node->current_len=current_len;
        node->flag=flag;
        node->toltal_len=totol_len;
        m[key] = node;          // 插入哈希表
        insertToFront(node);    // 插入链表头部
        ++count;
}

void Cache::DeleteNode(uint32_t key)
{
    LRUCacheNode* node=m[key];
    detachNode(node);
    m.erase(node->key);
    this->count--;
}

void Cache::removeLRUNode()
{
    LRUCacheNode* node = tail->prev;
    detachNode(node);
    m.erase(node->key);
    --this->count;
}



void Cache::detachNode(LRUCacheNode* node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
}


void Cache::insertToFront(LRUCacheNode* node)
{
    node->next = head->next;
    node->prev = head;
    head->next = node;
    node->next->prev = node;
}

int Cache::getFlag(uint32_t key)
{
    LRUCacheNode* node = m[key];
    return node->flag;
}
uint32_t Cache::getTotalLen(uint32_t key)
{
    LRUCacheNode* node = m[key];
    return node->toltal_len;
    
}
uint32_t Cache::getCurrentLen(uint32_t key)
{
    LRUCacheNode* node = m[key];
    return node->current_len;
}
void Cache::setCurrentLen(uint32_t key,uint32_t len)
{
    LRUCacheNode* node = m[key];
    node->current_len=len;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(Cache)
ELEMENT_MT_SAFE(Cache)
