#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <AD9850.h>
#include <Encoder.h>

//const int SCLK_PIN   = 7;
//const int DIN_PIN    = 6;
//const int DC_PIN     = 5;
//const int CS_PIN     = 4;
//const int RST_PIN    = 12;

//const int W_CLK_PIN  = 8;
//const int FQ_UD_PIN  = 9;
//const int DATA_PIN   = 10;
//const int RESET_PIN  = 13;

//const int SHIFT_PIN  = 11;

const int SCLK_PIN   = 9;
const int DIN_PIN    = 8;
const int DC_PIN     = 7;
const int CS_PIN     = 6;
const int RST_PIN    = 5;

const int W_CLK_PIN  = 10;
const int FQ_UD_PIN  = 11;
const int DATA_PIN   = 12;
const int RESET_PIN  = 13;

const int SHIFT_PIN  = 4;

const int ENC_PIN1   = 2;
const int ENC_PIN2   = 3;


const long IF_FREQ   = 10000000L;
const long INIT_FREQ = 4000000L;
const long MHZ       = 1000000L;

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

AD9850 dds(W_CLK_PIN, FQ_UD_PIN, DATA_PIN); // w_clk, fq_ud, d7

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(ENC_PIN1, ENC_PIN2);

long oldPosition  = -999;
uint32_t frequency = INIT_FREQ;
long STEP_FREQ = 50;
long step_multiplier = 1;

void update_frequency(uint32_t freq)
{
  float frequency = ((float)freq + IF_FREQ) / MHZ;
  dds.setfreq(freq);
  display.clearDisplay();   // clears the screen and buffer
  display.print(frequency, 5);
  display.println("MHz");
  display.display();
}

typedef enum {
    KEY_INITIAL,
    KEY_PRESSED,
    KEY_WAIT,
    KEY_WAIT1,
    KEY_WAIT_HIGH,
}keypad_state_t;

typedef enum {
    KEY_NONE,
    KEY1_RESSED,
    KEY1_LONG_PRESSED,
}key_event_t;

void keypad_poll()
{
    static keypad_state_t kaypad_state = KEY_INITIAL;
    static long buttonTime;
    static long buttonTime1;

    switch(kaypad_state)
    {
        case KEY_INITIAL:
            if (digitalRead(SHIFT_PIN) == LOW)
            {
                buttonTime = millis();
                kaypad_state = KEY_WAIT;
            }
            break;
        case KEY_WAIT:
            if (digitalRead(SHIFT_PIN) == HIGH)
            {
                buttonTime1 = millis();
                kaypad_state = KEY_WAIT1;
            }
            break;
        case KEY_WAIT1:
            if((buttonTime1 - buttonTime) > 500)
            {
                kaypad_state = KEY_WAIT_HIGH;
                buttonTime = millis();
                //KEY1_LONG_PRESSED;
                Serial.println("KEY1_LONG_PRESSED\n");
            }
            else if((buttonTime1 - buttonTime) > 50)
            {
                kaypad_state = KEY_WAIT_HIGH;
                buttonTime = millis();

                //KEY1_RESSED
                Serial.println("KEY1_RESSED\n");
                step_multiplier *= 10;
                if (step_multiplier > 10000) step_multiplier = 1;
                STEP_FREQ = 50 * step_multiplier;
            }
            else
            {
                kaypad_state = KEY_INITIAL;
            }
        case KEY_WAIT_HIGH:
            if (buttonTime - millis() > 100)
            {
                kaypad_state = KEY_INITIAL;
            }
            break;
    }
}

void setup()   {
  pinMode(SHIFT_PIN, INPUT);      // set pin to input
  digitalWrite(SHIFT_PIN, HIGH);  // turn on pull-up resistors

  Serial.begin(9600);
  display.begin();
  display.setContrast(60);
  display.clearDisplay(); 
  
  update_frequency(INIT_FREQ);
}

void loop() {
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    
    frequency = INIT_FREQ + newPosition * STEP_FREQ;

    update_frequency(frequency);
    
    //Serial.println(newPosition);
  }

  keypad_poll();

}
