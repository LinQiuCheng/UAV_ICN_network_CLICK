define($DEV en0, $MAC 38:f9:d3:2d:b4:64, $NID 1)

nodegetprocess::NodeGetProcess($NID);
nodedataprocess::NodeDataProcess($NID);
nodecontrolprocess::NodeControlProcess($NID);
c::Classifier(12/1110 14/a0,12/1110 14/a1,12/1110 14/a3,-);
//cache::Cache($NID);
pit::PitTable($NID);
out1 :: Queue(200);

FromDevice($DEV)->c;

c[0]->Strip(14)->[0]nodegetprocess;
c[1]->Strip(14)->[0]nodedataprocess;
c[2]->Strip(14)->[0]nodecontrolprocess;
c[3]->IPPrint->Discard;

nodegetprocess[1]->Discard;
nodedataprocess[1]->Discard;
nodecontrolprocess[0]->Discard;

nodegetprocess[0]->[0]pit;
nodedataprocess[0]->[1]pit;

nodegetprocess[2]->DataGen[0]->out1;

SendGet(INTERVAL 0.001 , NODE $NID)->[0]pit;




pit[0]->out1;
pit[1]->Discard;
pit[2]->DataReceive;

   
out1->EtherEncap(0X1110,$MAC,FF:FF:FF:FF:FF:FF)
->ToDevice($DEV);