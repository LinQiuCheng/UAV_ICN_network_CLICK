//

//
//  Created by Qiucheng LIN on 2020/6/01.
//  Copyright © 2019 Qiucheng LIN. All rights reserved.
//
//  vertion: have FIB ,GET broadcast/unicast  DATA unicast

require(library local.click);

define($DEV en0, $MAC 38:f9:d3:2d:b4:64, $BMAC ff:ff:ff:ff:ff:ff,$CACHE_SZ 50,
	$NID 1, $ETHERTYPE 0X1110,$IP 10.0.1.1)


nodegetprocess::NodeGetProcess($NID);
nodedataprocess::NodeDataProcess($NID);
nodecontrolprocess::NodeControlProcess($NID);
c::Classifier(12/1110 14/a0,12/1110 14/a1,12/1110 14/a3,-);
cache::Cache(NID $NID,CACHE_SIZE $CACHE_SZ,SRC $MAC);
pit::PitTable(NID $NID,SRC $MAC);
out1 :: Queue(200);
datareceive::DataReceive;
tasktable::TaskTable($NID);
commanderexe::CommanderExe;
//videoreceive::VideoReceive(INTERVAL 0.1 , NODE $NID);
//datagen::DataGen( NID $NID , SRC $MAC);
forwardtable::ForwardTable(NID $NID , SRC $MAC);
set_nonce::SetNonce;
//videogen::VideoGen(NID $NID , SRC $MAC);
//genget::GenGet;

FromDevice($DEV)->c;

c[0]->[0]nodegetprocess;
c[1]->[0]nodedataprocess;
c[2]->Strip(14)->[0]nodecontrolprocess;
c[3]->IPPrint->Discard;

nodegetprocess[1]->Discard;
nodedataprocess[1]->Discard;
nodecontrolprocess[1]->Discard;

nodegetprocess[0]->[0]cache;  
nodedataprocess[0]->[1]cache; 
nodecontrolprocess[0]->[0]tasktable;


//SendGet(INTERVAL 0.005 , NODE $NID , ETHERTYPE $ETHERTYPE , 
 //           SRC $MAC , DES $BMAC)->[0]cache;

tohost :: ToHost(fake0);
FromHost(fake0, fake, ETHER fake)
	-> fromhost_cl :: Classifier(12/0806, 12/0800 );
fromhost_cl[0] 
	-> ARPResponder(0.0.0.0/0 1:1:1:1:1:1) 
	-> tohost;
fromhost_cl[1] -> ipcla :: IPClassifier(dst udp port 8300,dst udp port 8301, dst udp port 8303,-);


ipcla -> Strip(42) -> EtherEncap(0X1110,$MAC,$BMAC)->[0]set_nonce[0]->[0]cache;  //get packet
ipcla[1]-> Strip(42) -> EtherEncap(0X1110,$MAC,$BMAC)->[1]cache;  //data packet
ipcla[2]-> Strip(42) -> [1]set_nonce[1]->[0]tasktable;  //control packet
ipcla[3]->Discard;

//SendCtr(INTERVAL 0.005 , NODE $NID)->[0]tasktable;

//SendVideo(INTERVAL 0.0001 , NODE $NID , ETHERTYPE $ETHERTYPE,
            //SRC $MAC)->out1;

cache[0]->[0]pit; //get
cache[1]->[1]pit; //data
cache[2]->[2]pit; //delete PIT item
cache[3]->Strip(14)->datareceive; // cache have data to self
//cache[3]->Strip(14)->videoreceive;
cache[4]->out1; // cache have data to other

pit[0]->[1]forwardtable; //get
pit[1]->[2]forwardtable; //data
//pit[2]->datareceive;
pit[2]->[3]forwardtable; //self data
pit[3]->Discard;
pit[4]->[0]forwardtable; //self get

//forwardtable[2]->Strip(14)->videoreceive;
forwardtable[0]->Strip(14)->UDPIPEncap(10.0.1.24, 8299, $IP, 9000)
->EtherEncap(0X0800, 1:1:1:1:1:1 ,fake)->tohost; //get to self
forwardtable[1]->out1; // data/get to other
forwardtable[2]->Strip(14)->datareceive;
datareceive->UDPIPEncap(10.0.1.24, 8299, $IP, 9001) 
-> EtherEncap(0X0800, 1:1:1:1:1:1 ,fake)->tohost; 



tasktable[0]->Discard;
tasktable[1]->EtherEncap(0X1110,$MAC,$BMAC)->out1;
tasktable[2]->[0]commanderexe;

commanderexe[0]->EtherEncap(0X1110,$MAC,$BMAC)->out1;
commanderexe[1]->UDPIPEncap(10.0.1.24, 8299, $IP, 9003) -> EtherEncap(0X0800, 1:1:1:1:1:1 ,fake)->tohost;

//out1->EtherEncap(0X1110,$MAC,FF:FF:FF:FF:FF:FF)
out1->ToDevice($DEV);
//EtherEncap(0X1110,$MAC,dc:a6:32:5f:d5:1b)




