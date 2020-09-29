#include<time.h>
#include <click/config.h>
#include"CommanderExe.hh"
#include"struct.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <sys/msg.h>
#include <errno.h>

CLICK_DECLS

CommanderExe::CommanderExe(){
    printf("init CommanderExe suc\n\n");
}

CommanderExe::~CommanderExe(){

}

/*int
CommanderExe::configure(Vector<String> &conf, ErrorHandler *errh)
{
    return Args(conf, this, errh).read_mp("NID", nid).complete();
}*/


void
CommanderExe::push(int port, Packet *packet)
{
    struct  Control control;
    struct control_cmd cmd;
    
	memcpy(&control,(unsigned char *)packet->data(),sizeof(control));

    printf("execute commander/n");
    /*回复 回应消息*/
    control.ttl=5;
    control.msgtype=1;
    memcpy((unsigned char *)packet->data(),&control,sizeof(control));
    output(0).push(packet);

    String _data="";
    Packet* p= Packet::make(Packet::default_headroom, _data.data(), _data.length(), 0);
    p->push(20);
    cmd.control_type=control.control_type;
    cmd.param[0]=control.param[0];
    cmd.param[1]=control.param[1];
    cmd.param[2]=control.param[2];
    cmd.param[3]=control.param[3];
    memcpy((unsigned char *)p->data(),&cmd,20);
    output(1).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CommanderExe)
ELEMENT_MT_SAFE(CommanderExe)