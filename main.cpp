#include "mbed.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "uLCD_4DGL.h"

#define UINT14_MAX        16383

// FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR0 (0x1E<<1) // with pins SA0=0, SA1=0
#define FXOS8700CQ_SLAVE_ADDR1 (0x1D<<1) // with pins SA0=1, SA1=0
#define FXOS8700CQ_SLAVE_ADDR2 (0x1C<<1) // with pins SA0=0, SA1=1
#define FXOS8700CQ_SLAVE_ADDR3 (0x1F<<1) // with pins SA0=1, SA1=1
// FXOS8700CQ internal register addresses
#define FXOS8700Q_STATUS 0x00
#define FXOS8700Q_OUT_X_MSB 0x01
#define FXOS8700Q_OUT_Y_MSB 0x03
#define FXOS8700Q_OUT_Z_MSB 0x05
#define FXOS8700Q_M_OUT_X_MSB 0x33
#define FXOS8700Q_M_OUT_Y_MSB 0x35
#define FXOS8700Q_M_OUT_Z_MSB 0x37
#define FXOS8700Q_WHOAMI 0x0D
#define FXOS8700Q_XYZ_DATA_CFG 0x0E
#define FXOS8700Q_CTRL_REG1 0x2A
#define FXOS8700Q_M_CTRL_REG1 0x5B
#define FXOS8700Q_M_CTRL_REG2 0x5C
#define FXOS8700Q_WHOAMI_VAL 0xC7


uLCD_4DGL uLCD(D1, D0, D2);
I2C i2c( PTD9,PTD8);
//Serial pc(USBTX, USBRX);
Ticker test;

EventQueue queue;

int m_addr = FXOS8700CQ_SLAVE_ADDR1;

void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len);
void FXOS8700CQ_writeRegs(uint8_t * data, int len);

void evaluate();

DigitalOut greenLED(LED2);
void blink(){
    greenLED = !greenLED;
}


uint8_t who_am_i, data[2], res[6];
int16_t acc16;
float t[3];

int main() {
   //pc.baud(115200);

   uLCD.text_width(2); //4X size text
   uLCD.text_height(2);

   // Enable the FXOS8700Q
   FXOS8700CQ_readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
   data[1] |= 0x01;
   data[0] = FXOS8700Q_CTRL_REG1;
   FXOS8700CQ_writeRegs(data, 2);

   // Get the slave address
   FXOS8700CQ_readRegs(FXOS8700Q_WHOAMI, &who_am_i, 1);
   //pc.printf("Here is %x\r\n", who_am_i);
   test.attach( &blink,1.0f);
   //evaluate();
   //wait(5.0f);
   //uLCD.filled_rectangle(0,0, 125, 125, 0x000000);
   
   queue.call_every(2000,evaluate);
   queue.dispatch();
   
   
   while (true);


}


void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len) {
   char t = addr;
   i2c.write(m_addr, &t, 1, true);
   i2c.read(m_addr, (char *)data, len);
}


void FXOS8700CQ_writeRegs(uint8_t * data, int len) {
   i2c.write(m_addr, (char *)data, len);
}

void evaluate(){
      uLCD.filled_rectangle(0,0, 125, 125, 0x000000);
      uLCD.locate(0,0);
      FXOS8700CQ_readRegs(FXOS8700Q_OUT_X_MSB, res, 6);
      acc16 = (res[0] << 6) | (res[1] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[0] = ((float)acc16) / 4096.0f;
      acc16 = (res[2] << 6) | (res[3] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[1] = ((float)acc16) / 4096.0f;
      acc16 = (res[4] << 6) | (res[5] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[2] = ((float)acc16) / 4096.0f;

      uLCD.printf("ACC:\nX=%1.4f\nY=%1.4f\nZ=%1.4f\r\n",t[0], t[1], t[2] );
}
/*
#include "mbed.h"
#include "uLCD_4DGL.h"
uLCD_4DGL uLCD(D1, D0, D2);

int main(){
      uLCD.printf("\nHello uLCD World\n"); //Default Green on black text
      wait(30);
}
*/

/*
#include "mbed.h"

int main() {
    // creates a queue with the default size
    EventQueue queue;

    // events are simple callbacks
    queue.call(printf, "called immediately\r\n");
    queue.call_in(2000, printf, "called in 2 seconds\r\n");
    queue.call_every(1000, printf, "called every 1 seconds\r\n");

    // events are executed by the dispatch method
    queue.dispatch();
}
*/