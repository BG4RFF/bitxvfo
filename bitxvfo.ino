#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <AD9850.h>
#include <Encoder.h>

const int SCLK_PIN = 9;
const int DIN_PIN = 8;
const int DC_PIN = 7;
const int CS_PIN = 6;
const int RST_PIN = 5;

const int W_CLK_PIN = 10;
const int FQ_UD_PIN = 11;
const int DATA_PIN = 12;
const int RESET_PIN = 13;

const int ENC_PIN1 = 2;
const int ENC_PIN2 =3;

const int IF_FREQ = 10000000;
const int INIT_FREQ = 4000000;

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(SCLK_PIN, DIN_PIN, DC_PIN, CS_PIN, RST_PIN);

// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
// Adafruit_PCD8544 display = Adafruit_PCD8544(5, 4, 3);
// Note with hardware SPI MISO and SS pins aren't used but will still be read
// and written to during SPI transfer.  Be careful sharing these pins!

AD9850 ad(W_CLK_PIN, FQ_UD_PIN, DATA_PIN); // w_clk, fq_ud, d7

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(ENC_PIN1, ENC_PIN2);

long oldPosition  = -999;
uint32_t frequency = INIT_FREQ;

void update_frequency(uint32_t freq)
{
  display.clearDisplay();   // clears the screen and buffer
  display.print(((float)freq)/1000000,6);
  display.println("MHz");
  display.display();
}

void setup()   {
  Serial.begin(9600);
  display.begin();
  display.setContrast(60);
  display.clearDisplay(); 
  
  update_frequency(30000000);
}

void loop() {
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    
    frequency = 4300000 + newPosition*50;
    ad.setfreq(frequency);
    update_frequency(frequency + IF_FREQ);
    
    Serial.println(newPosition);
  }
}
