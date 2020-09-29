#include<iostream>
using namespace std;
struct InfoPacket
{
    uint8_t version_type;////版本4位，类型4位   160 GET  161 DATA
	uint8_t ttl; //生存时间
	uint16_t nid;//NID
	uint32_t sid;//SID
};

struct Control
{
    uint8_t version_type; 
	uint8_t ttl;
	uint16_t nid;
	uint32_t control_type;
};
