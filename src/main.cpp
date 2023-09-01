#include <FlexCAN_T4.h>
#include <TeensyCAN.h>

FlexCAN_T4FD<CAN3, RX_SIZE_256, TX_SIZE_256> FD;

TeensyCAN node100 = TeensyCAN(100);

struct pack{}; //基底クラス
struct MasterToTail: public pack{
    float updateTime;
    float drCommand;
    float deCommand;
    float err_state[15];
    float gravity[3];
    float mode;
};

// MasterToIF mip;
// IFToMaster imp;
MasterToTail mtp;

// TailToMaster tmp;
// TailToIF tip;

int loopCount = 0;

void canSniff(const CANFD_message_t &msg) {
  Serial.print("ISR - MB "); Serial.print(msg.mb);
  Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
  Serial.print("  LEN: "); Serial.print(msg.len);
  Serial.print(" EXT: "); Serial.print(msg.flags.extended);
  Serial.print("  EDL: "); Serial.print(msg.edl );
  Serial.print(" TS: "); Serial.print(msg.timestamp);
  Serial.print(" ID: "); Serial.print(msg.id, HEX);
  Serial.print(" Buffer: ");
  for ( uint8_t i = 0; i < msg.len; i++ ) {
    Serial.print(msg.buf[i], HEX); Serial.print(" ");
  } Serial.println();
}

void setup() {
  Serial.begin(115200); delay(400);
  pinMode(6, OUTPUT); digitalWrite(6, LOW); /* enable transceiver */
  FD.begin(); /* enable the FlexCAN controller before assigning it */

  node100.setBus(_CAN3);
  node100.setID(20);

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
  // FD.events();
  // Node.events();
  static uint32_t t = millis();

  // setup Master to Tail pack and send it to CAN bus
  mtp.updateTime = 11.1f * (loopCount % 5 + 1);
  mtp.drCommand = 22.2f * (loopCount % 5 + 1);
  mtp.deCommand = 1.0f * (loopCount % 5 + 1);
  for (int i = 0; i < 15; i++){
    mtp.err_state[i] = 1.0f * i;
  }
  mtp.gravity[0] = 33.4f;
  mtp.gravity[1] = 66.8f;
  mtp.gravity[2] = 99.4f;
  mtp.mode = 2.0f * (loopCount % 5 + 1);

  union send_data_union {
    MasterToTail raw_data;
    uint8_t encoded_data[256];
  };
  send_data_union sendDataUnion; //* unionのインスタンスを宣言
  sendDataUnion.raw_data = mtp; //* 引数に取ったstructをunionへコピー
  // Serial.println("send start");
  // Serial.println(Node.sendMsg(sendDataUnion.encoded_data, 256, 7));

  if ( millis() - t > 1000 ) {
    uint8_t data[1024] = {
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
      11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
      21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
      31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
      41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
      51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
      61, 62, 63, 64, 65, 66, 67, 68, 69, 70
    };
    Serial.println("Send Start");
    // Serial.println(    Node.sendMsg(data, 256, 5)); /* Global always returns 1, no response. */
    // Serial.println(node100.sendMsg(data, 100, 7)); /* ACK: 0x06, TIMEOUT = 0xFF */
    Serial.println(Node.sendMsg(sendDataUnion.encoded_data, 256, 3));
    t = millis();
  }
  loopCount++ ;
}

