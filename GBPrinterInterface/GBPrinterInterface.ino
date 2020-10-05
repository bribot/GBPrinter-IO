op//#include<SPI.h>
volatile int i = 0;
volatile boolean clearFlag = true;
volatile byte transferData = 0;
char buf[300];
#define SCK_PIN   13
#define MISO_PIN  12
#define MOSI_PIN  11                                                                                                                                                               
#define SS_PIN    10
#define SS_CONTROL 2

#define COMMAND_INITIALIZE 0x01
#define COMMAND_START_PRINTING 0x02
#define COMMAND_FILL_BUFFER 0X04
#define COMMAND_READ_STATUS 0X0F

#define PREAMBLE0 0X88
#define PREAMBLE1 0X33
// TODO CHECK COMPRESSION ALGORITM
#define COMPRESSION 0X00

// PRINTER STATUS
#define PRINTER_PREAMBLE 0X81
#define PRINTER_LOW_BATTERY       7
#define PRINTER_OTHER_ERROR       6
#define PRINTER_PAPER_JAM         5
#define PRINTER_PACKET_ERROR      4
#define PRINTER_UNPROCESSED_DATA  3
#define PRINTER_IMAGE_DATA_FULL   2
#define PRINTER_PRINTING          1
#define PRINTER_CHECKSUM_ERROR    0

volatile byte PRINTER_STATUS = 0x00
volatile bool SEND_FLAG = false;

volatile int nSheets = 0;
volatile char margin;
volatile char palette;
volatile char exposure;


void setup()
{
  Serial.begin(9600); 
  // Pin init
  pinMode(SS_PIN, INPUT);
  pinMode(MISO_PIN, OUTPUT);
  pinMode(MOSI_PIN, INPUT);
  pinMode(SCK_PIN, INPUT);
  pinMode(SS_CONTROL, OUTPUT);
  digitalWrite(SS_CONTROL, LOW);
  // wait for serial to be ready
  while(!Serial);
  // Enable SPI and SPI Interrupt
  SPCR |= _BV(SPE);
  SPCR |= _BV(SPIE);
  SPCR |= _BV(CPOL);
  SPCR |= _BV(CPHA);
  SPCR |= (1<<SPR1)|(1<<SPR0);

  Serial.println("Initializing...");
}

// TODO REPLACE MAGIC NUMBERS WITH DEFINED STATES
void loop(void)
{
  if (i > 2)
  {
    if(buf[2] == 0x0F && i == 7)
    {
      // NULL / STATUS REQUEST
      Serial.println("Status...");
      if (stat == 0x8106)
        stat = 0x8104;
      i = 0; 
      
    } else if (buf[2] == 0x01 && i == 15) {
      // CLEAR BUFFER
      Serial.println("Clear Buffer");
      for (int j = 0; j<10 ; j++){
        int data = (int)buf[j];
        Serial.println(data, HEX);
      }
      //SPI_Transfer(0x81);
//      digitalWrite(SS_CONTROL,HIGH);
//      SPCR &= (0<<SPIE)
      clearFlag = true;
      transferData = 0x81;
      
      //SPI.transfer(0x00);
      i = 0;
      //SPI_Transfer(0x81);
      
    } else if (buf[2] == 0x02 && i==11 ) {
      // PRINT!
      Serial.println("Start Printing");
      nSheets = (int)buf[4];
      margin = buf[5];
      palette = buf[6];
      exposure = buf[7];
      //SPI.transfer(0x8100);
      //SPI.transfer(0x00);
      stat = 0x8106;
      i == 0;
      
    } else if (buf[2] == 0x04 && i == (int)buf[4]<<8|(int)buf[5] + 9)  {
      Serial.println("Fill Buffer");
      char img = (int)buf[6];
      for (int j = 0; j < (int)buf[4]<<8|(int)buf[5] - 1 ; j++) {
        img = img<<(int)buf[j+6];
      }
      i == 0;
      //SPI.transfer(stat);
      stat = 0x8108;
    }
  }
}

ISR (SPI_STC_vect)   //Inerrrput routine function
{
  buf[i] = SPDR;
  SPDR = PRINTER_STATUS;
  if (SEND_FLAG){
    SEND_FLAG = false;
    PRINTER_STATUS = 0x00;
  }
  //THIS IS STUPI FIX IT!
//  if (i == 7){
//    SPDR = 0x81;
//  } else {
//    SPDR = 0;
//  }
    i++;

}

void clearBuffer() {
  for (int j = 0; j < sizeof(buf)-1; j++){
    buf[j]=0x00;
  }
}

//void SPI_Transfer(byte data){
//  pinMode(SS, OUTPUT);
//  pinMode(MISO, INPUT);
//  pinMode(MOSI, OUTPUT);
//  pinMode(SCK, OUTPUT);
//  digitalWrite(SCK, HIGH);
//  SPCR &= (0<<SPE);
//  SPCR |= _BV(SPIE);
//  SPCR |= _BV(CPOL);
//  SPCR |= _BV(CPHA);
//  SPCR |= _BV(MSTR);
//  SPCR |= _BV(SPR0);
//  SPCR |= _BV(SPR1);
//  SPCR |= _BV(SPE);
//  Serial.println("Transfering...");
//  SPDR = data;
//  while(!(SPSR & (1<<SPIF)));
//  pinMode(SS_PIN, INPUT);
//  pinMode(MISO_PIN, OUTPUT);
//  pinMode(MOSI_PIN, INPUT);
//  pinMode(SCK_PIN, INPUT);
//  SPCR &= (0<<SPE);
//  SPCR |= (0<<MSTR);
//  SPCR |= _BV(SPE);
}
