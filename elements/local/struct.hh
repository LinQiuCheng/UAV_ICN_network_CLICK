#include<stdio.h>
struct Get_Header  //总长度 24
{
	uint8_t version_type;////版本4位，类型4位   160 GET  161 DATA
	uint8_t ttl; //生存时间
	uint16_t nid;//NID   保存起始发送者的NID
	uint32_t sid;//SID   需要请求的服务标识
	uint32_t total_len; //总长度
	uint16_t checksum;//检验和
	uint16_t split;  //是否分片    
	uint32_t offset;  //偏移量
	uint32_t nonce;  //随机数，区分重传和多路径的包
};

struct Data_Header  //总长度 32
{
	uint8_t version_type;////版本4位，类型4位   160 GET  161 DATA
	uint8_t ttl; //生存时间
	uint16_t nid;//NID   保存起始发送者的NID
	uint32_t sid;//SID   需要请求的服务标识
	uint32_t total_len; //总长度
	uint16_t checksum;//检验和
	uint16_t split;  //是否分片    
	uint32_t offset;  //偏移量
	uint32_t create_time; //创建时间
    uint32_t expire_time; //有效时间
    uint32_t length;    //数据长度
};



#define FIIE_PATH "/Users/qiuchenglin/Desktop"
struct Control
{
	uint8_t version_type; //163 控制包
	uint8_t ttl; //生存时间
	uint16_t c_nid; //发出该控制指令的节点标识
	uint16_t nid; //需要控制的节点标识
	uint16_t control_type; //控制类型
	uint16_t msgtype; // 0为控制信息 ，1为response信息
	uint16_t checksum; //检验和
	uint32_t nonce; //随机数，区别相同指令
	uint32_t param[4]; //飞控参数
};

#define MAX_FRAG_LEN 1400
#define CACHE_SIZE 100
