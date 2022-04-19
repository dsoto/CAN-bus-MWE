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
  BATTERY_CURRENT,
  BATTERY_TEMPERATURE,
  CELL_VOLTAGE_LOW,
  CELL_VOLTAGE_HIGH,
  RPM,
  MOTOR_CURRENT,
  CONTROLLER_TEMPERATURE,
};

struct vehicleMeasurement {
  int label;
  float value;
};

vehicleMeasurement vm[9] = {
  {BATTERY_VOLTAGE, 0.0},
  {BATTERY_CURRENT, 0.0},
  {BATTERY_TEMPERATURE, 0.0},
  {CELL_VOLTAGE_LOW, 0.0},
  {CELL_VOLTAGE_HIGH, 0.0},
  {RPM, 0.0},
  {MOTOR_CURRENT, 0.0},
  {CONTROLLER_TEMPERATURE, 0.0}
};

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup() {
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  int test;
  test = CAN.begin(CAN_500KBPS);
  // begin - mcp2515_init
  while ( test != CAN_OK)              // init can bus : baudrate = 500k
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
        vm[BATTERY_CURRENT].value = get_32(4, buf)/1E5;
      }
    }
  }

  found_ID = 0;
  while (!found_ID) {
    if(CAN_MSGAVAIL == CAN.checkReceive()) {
      CAN.readMsgBuf(&len, buf);
      uint32_t canId = CAN.getCanId();

      if (canId == 0x210A) {
        found_ID = 1;
        vm[BATTERY_TEMPERATURE].value = get_16(6, buf)/1E2;
      }
    }
  }

  found_ID = 0;
  while (!found_ID) {
    if(CAN_MSGAVAIL == CAN.checkReceive()) {
      CAN.readMsgBuf(&len, buf);
      uint32_t canId = CAN.getCanId();

      if (canId == 0x1F0A) {
        found_ID = 1;
        vm[CELL_VOLTAGE_LOW].value = get_32(0, buf)/1E5;
        vm[CELL_VOLTAGE_HIGH].value = get_32(4, buf)/1E5;
      }
    }
  }

  found_ID = 0;
  while (!found_ID) {
    if(CAN_MSGAVAIL == CAN.checkReceive()) {
      CAN.readMsgBuf(&len, buf);
      uint32_t canId = CAN.getCanId();

      if (canId == 0x0916) {
        found_ID = 1;
        vm[RPM].value = get_32(0, buf)/1E0;
        vm[MOTOR_CURRENT].value = get_16(4, buf)/1E1;
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
  Serial.print("BATTERY_CURRENT: ");
  Serial.println(vm[BATTERY_CURRENT].value);
  Serial.print("BATTERY_TEMPERATURE: ");
  Serial.println(vm[BATTERY_TEMPERATURE].value);
  Serial.print("CELL_VOLTAGE_LOW: ");
  Serial.println(vm[CELL_VOLTAGE_LOW].value);
  Serial.print("CELL_VOLTAGE_HIGH: ");
  Serial.println(vm[CELL_VOLTAGE_HIGH].value);
  Serial.print("RPM: ");
  Serial.println(vm[RPM].value);
  Serial.print("MOTOR_CURRENT: ");
  Serial.println(vm[MOTOR_CURRENT].value);
  Serial.print("CONTROLLER_TEMPERATURE: ");
  Serial.println(vm[CONTROLLER_TEMPERATURE].value);
  Serial.print(millis());
  Serial.println();
  Serial.println();
}
