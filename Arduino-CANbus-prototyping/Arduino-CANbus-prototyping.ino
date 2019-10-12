// demo: CAN-BUS Shield, receive data with check mode
// send data coming to fast, such as less than 10ms, you can use this way
// loovee, 2014-6-13

#include <SPI.h>
#include "mcp_can.h"

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 53;
const int LED        = 8;
boolean ledON        = 1;

enum measurements {
  BATTERY_VOLTAGE,
  CONTROLLER_TEMPERATURE
};

struct vehicleMeasurement {
  int label;
  float value;
};

vehicleMeasurement vm[2] = {
  {BATTERY_VOLTAGE, 0.0},
  {CONTROLLER_TEMPERATURE, 0.0}
};

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup() {
  Serial.begin(115200);
  pinMode(LED,OUTPUT);

  // begin - mcp2515_init
  while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
  {
      Serial.println("CAN BUS Shield init fail");
      Serial.println("Init CAN BUS Shield again");
      delay(100);
  }
  Serial.println("CAN BUS Shield init ok!");
}

uint32_t get_32(uint8_t index, uint8_t buf[8]) {
  return ((uint32_t)buf[index]     << 24 |
          (uint32_t)buf[index + 1] << 16 |
          (uint32_t)buf[index + 2] <<  8 |
          (uint32_t)buf[index+3]);

}

uint16_t get_16(uint8_t index, uint8_t buf[8]) {
  return ((uint16_t)buf[index] << 8 |
          (uint16_t)buf[index + 1]);

}

void loop() {

// TODO: if time lag do big or statement for filter and then case (or do in hardware)
// TODO: make a big struct or loop for reading?

  unsigned char len = 0;
  unsigned char buf[8];
  bool found_ID = 0;

  found_ID = 0;
  while (!found_ID) {
    if(CAN_MSGAVAIL == CAN.checkReceive()) {
      CAN.readMsgBuf(&len, buf);
      uint32_t canId = CAN.getCanId();

      if (canId == 0x1E0A) {
        found_ID = 1;
        vm[BATTERY_VOLTAGE].value = get_32(0, buf)/1E5;
      }
    }
  }

  found_ID = 0;
  while (!found_ID) {
    if(CAN_MSGAVAIL == CAN.checkReceive()) {
      CAN.readMsgBuf(&len, buf);
      uint32_t canId = CAN.getCanId();

      if (canId == 0x1016) {
        found_ID = 1;
        vm[CONTROLLER_TEMPERATURE].value = get_16(0, buf)/1E1;
      }
    }
  }

  Serial.print("BATTERY_VOLTAGE: ");
  Serial.println(vm[BATTERY_VOLTAGE].value);
  Serial.print("CONTROLLER_TEMPERATURE: ");
  Serial.println(vm[CONTROLLER_TEMPERATURE].value);
}
