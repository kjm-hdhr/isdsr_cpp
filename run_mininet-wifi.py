#!/usr/bin/python

import sys
import time
import math

from mininet.log import setLogLevel, info
from mn_wifi.link import wmediumd, adhoc
from mn_wifi.cli import CLI
from mn_wifi.net import Mininet_wifi
from mn_wifi.wmediumdConnector import interference
from mn_wifi.wmediumdConnector import error_prob
from mn_wifi.node import CPULimitedStation

class TopologyGenerator:
  def __init__(self,width=3,height=3):
    self.x=width;
    self.y=height;
    self.prot="";
    self.jars=["jpbc-pbc-2.0.0.jar","jpbc-plaf-2.0.0.jar","jpbc-pbc-2.0.0.jar"];

  def params(self,args):
    for param in args:
      if param.startswith("-w"):
        self.x=int(param.split(":")[1]);
      if param.startswith("-h"):
        self.y=int(param.split(":")[1]);
      if param.startswith("-p"):
        self.prot=param.split(":")[1];
      if param.startswith("-m"):
        self.mode=param.split(":")[1];
      if param.startswith("-er"):
        self.error_rate=param.split(":")[1];
      if param.startswith("-cpu"):
        self.cpu_rate=param.split(":")[1];
        

    print("w:"+str(self.x)+" h:"+str(self.y)+" p:"+self.prot);

  def generate(self):
    #net = Mininet_wifi(link=wmediumd, wmediumd_mode=interference,noise_th=-91, fading_cof=1)
    net = Mininet_wifi(link=wmediumd, wmediumd_mode=interference)
    #net = Mininet_wifi(link=wmediumd)
    num=self.x*self.y;
    
    info("*** Creating nodes\n")
    self.stas = []

    print("*** Creating nodes")
    interval=70;
    #sta1 = net.addStation('sta1', ip6='fe80::1',cls=CPULimitedStation,position='10,10,0', **kwargs)
    #sta1.setCPUFrac(f=0.01)
    for i in range(num):
        sta = net.addStation('sta' + str(i+1), cls=CPULimitedStation, position=str(i%self.x*interval)+','+str(math.floor(i/self.x)*interval)+',0', range=100)
        sta.setCPUFrac(f=float(self.cpu_rate))
        self.stas.append(sta)
        #self.stas.append(net.addStation('sta' + str(i+1), position=str(i%self.x*interval)+','+str(math.floor(i/self.x)*interval)+',0', range=100))
		
    info("*** Configuring Propagation Model\n")
    net.setPropagationModel(model="logDistance", exp=4)

    print("*** Configuring wifi nodes")
    net.configureWifiNodes()

    print("*** Creating links")
    for i in range(num):
        net.addLink(self.stas[i], cls=adhoc, intf='sta'+str(i+1)+'-wlan0',ssid='adhocNet',mode='g', channel=5, ht_cap='HT40+')
        #net.addLink(self.stas[i], cls=adhoc, intf='sta'+str(i+1)+'-wlan0',ssid='adhocNet',mode='g', channel=5)
    #net.plotGraph(max_x=500, max_y=500)

    return net;

  def run(self,net):
    
    info("*** Starting network\n")
    net.build()

    #self.sendCommand();
    self.writeCommand();
    info("*** Running CLI\n")
    time.sleep(0.5);
    cliwifi=CLI(net)
    info("*** Stopping network\n")
    net.stop()
  
  def writeCommand(self):
    num=self.x*self.y;
    with open("cmd.txt", mode='w') as f:
      for i in range(1,num):
        f.write("py sta"+str(i+1)+".cmd(\"xterm -n sta"+str(i+1)+" -hold -e bash run_isdsr_"+self.prot.lower()+".sh -i sta"+str(i+1)+"-wlan0 &\")\n");
  
  def sendCommand(self):
    num=self.x*self.y;
    for i in range(1,num):
      print("index:"+str(i)+ " is set cmd: "+self.prot.lower()+".sh");
      #self.stas[i].sendCmd("xterm -e "+self.cmdf);
      #if i != 10:
      time.sleep(0.1);
      self.stas[i].sendCmd("xterm -n sta"+str(num)+" -hold -e bash "+self.prot.lower()+".sh");

  def setPromisc(self):
    info("*** Interface configuration ***")
    num=self.x*self.y;
    for i in range(num):
      self.stas[i].cmd('ifconfig sta'+str(i+1)+'-wlan0 promisc');
      self.stas[i].cmd('iwconfig sta'+str(i+1)+'-wlan0 rts off');
      self.stas[i].cmd('iwconfig sta'+str(i+1)+'-wlan0 retry max 1');

  def command_source(self,cmd):
    self.stas[0].cmd('xterm -hold -e '+cmd);

if __name__ == '__main__':
  setLogLevel('info')
  args=sys.argv;

  if len(args) != 4:
    print("usage python location.py -w:width -h:height -p:protocol ");
  
  
  topo=TopologyGenerator();
  topo.params(args);
  net=topo.generate();
  #topo.setPromisc();
  topo.run(net);
  #topo.command('java -cp ./bin ou.ist.de.Main -protocol:DSR -port:10000 -frag:1000 &');
