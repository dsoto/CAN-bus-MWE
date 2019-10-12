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

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup()
{
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
  return ((uint32_t)buf[index] << 24 | 
                    (uint32_t)buf[index + 1] << 16 | 
                    (uint32_t)buf[index + 2] << 8 | 
                    (uint32_t)buf[index+3]);
   
}

void loop()
{
    unsigned char len = 0;
    unsigned char buf[8];

    if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
      // readMsgBuf - readMsgBufID - readRxTxStatus - mcp2515_read_canMsg
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

        uint32_t canId = CAN.getCanId();

        //Serial.println("-----------------------------");
        //Serial.print("ID: ");
        //Serial.println(canId, HEX);

        if (canId == 0x1E0A) {
          Serial.println(canId, HEX);
          int i = 0;
          uint32_t value = (uint32_t)buf[i]<<24 | (uint32_t)buf[i+1]<<16 | (uint32_t)buf[i+2]<<8 | (uint32_t)buf[i+3];
          Serial.println(value/100000.);
          Serial.println(get_32(0, buf)/1E5);

        for(int i = 0; i<len; i++)    // print the data
        {
            Serial.print(buf[i]);
            Serial.print("\t");
        }          
        Serial.println();
        }

        if (canId == 0x210A) {
          Serial.print(canId, HEX);
          Serial.print("\t");
        for(int i = 0; i<len; i++)    // print the data
        {
            Serial.print(buf[i]);
            Serial.print("\t");
        }          
        Serial.println();
        }

        /*
        Serial.println();
        */
    }
}
