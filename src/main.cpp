#include <FlexCAN_T4.h>
#include <TeensyCAN.h>

FlexCAN_T4FD<CAN3, RX_SIZE_256, TX_SIZE_16> FD;

TeensyCAN node36 = TeensyCAN(36);

void cb(const uint8_t* buffer, uint16_t length, AsyncTC info) {
  Serial.print("Node: ");
  Serial.print(info.node);
  Serial.print("\tPacketID: ");
  Serial.print(info.packetid);
  Serial.print("\tBroadcast: ");
  Serial.print(info.broadcast);
  Serial.print("\tlength: ");
  Serial.print(length);
  Serial.print("\tData: ");
  for ( uint8_t i = 0; i < 4; i++ ) {
    Serial.print(buffer[i]);
    Serial.print(" ");
  }
  Serial.println();
}


void setup(void) {
  Serial.begin(115200); delay(4000);
  Serial.println("hello world!");
  pinMode(6, OUTPUT); digitalWrite(6, LOW);
  FD.begin();
  Node.setID(100);
  Node.setBus(_CAN3);

  Node.onReceive(cb);
  CANFD_timings_t config;
  config.clock = CLK_24MHz;
  config.baudrate = 1000000;
  config.baudrateFD = 2000000;
  config.propdelay = 190;
  config.bus_length = 1;
  config.sample = 87.5;
  FD.setRegions(64);
  FD.setBaudRate(config);
  // FD.onReceive(canSniff);
  FD.enableMBInterrupts();
  FD.mailboxStatus();
}



void loop() {
  Node.events();
}