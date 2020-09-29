#include<time.h>
#include <click/config.h>
#include"tasktable.hh"
#include"struct.hh"
#include <click/args.hh>
#include <click/error.hh>
#define EXPIRE_TIME 2

TaskTable::TaskTable(){
    expiretime=EXPIRE_TIME;
	printf("init TaskTable suc\n\n");
	

}

TaskTable::~TaskTable(){
	
}

int
TaskTable::configure(Vector<String> &conf, ErrorHandler *errh)
{
    return Args(conf, this, errh).read_mp("NID", nid).complete();
}


void
TaskTable::push(int port, Packet *packet)
{
	struct  Control control;
	memcpy(&control,(unsigned char *)packet->data(),sizeof(control));
    key_Struct key;
    key.c_nid=control.c_nid;
    key.control_type=control.control_type;
    key.nonce=control.nonce;
    key.nid=control.nid;
    key.reserve=0;
    bool flag=IsInTable(key);
    if(control.msgtype==0)// 控制消息
    {
        printf("receive a control msg\n");
        if(flag==false){//没有相关条目
            AdditionalValue value;
            value.create_time=time(NULL);
            value.param[0]=control.param[0];
            value.param[1]=control.param[1];
            value.param[2]=control.param[2];
            value.param[3]=control.param[3];
            AddInTable(key,value);  //插入表项
            if(control.nid==nid){ //控制本节点的控制消息
                printf("execute this cmd\n");
                output(2).push(packet); //发送给控制命令执行单元
            }
            else{ //本节点为中继
                printf("this node is a control MTU");
                output(1).push(packet); //直接由网卡发送模块转发该控制消息
            }
        }
        else{ //有相关条目
            printf("Have this cmd item\n");
            if(control.nid==nid) //控制本节点的控制消息
            { /*有条目说明该控制已经被执行，有可能是因为该回应消息传输丢失，
                造成发起方不知道控制消息已经顺利达到，所以这次再次回应控制消息*/
                control.msgtype=1;
                control.ttl=5;
                memcpy((unsigned char *)packet->data(),&control,sizeof(control));
                output(1).push(packet);// 发送回应消息给网卡发送模块
            }
            else{
                output(0).push(packet);//丢弃，避免重复转发
            }       
        }
    }
    else{//收到回应控制消息
        printf("receive a reply msg\n");
        if(flag==true){//有相关条目
            DeleteInTable(key); //删除表项
            if(control.c_nid==nid){//是对本节点发出的控制消息的回应
                printf("receive reply for %d\n",control.control_type);
                output(0).push(packet); //丢弃
            }
            else{ //该节点只是中继，转发该回应控制消息
                printf("this node is a reply MTU\n");
                output(1).push(packet); //发送到网卡发送模块
            }
        }
        else
            output(0).push(packet);  //丢弃
    }
    
}

bool 
TaskTable::IsInTable(key_Struct key){
    iterator itr=taskTable.find(key);
    if(itr==taskTable.end()){
        return false;
    }
    else return true;
}
void 
TaskTable::AddInTable(key_Struct key,AdditionalValue value){
    taskTable.insert(std::map<key_Struct, AdditionalValue>::value_type(key,value));
}

void 
TaskTable::DeleteInTable(key_Struct key){
    taskTable.erase(key);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(TaskTable)
ELEMENT_MT_SAFE(TaskTable)