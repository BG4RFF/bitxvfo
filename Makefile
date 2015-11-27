#OARD_TAG    = uno

BOARD_TAG     = mega2560
MONITOR_PORT  = /dev/ttyACM0

USER_LIB_PATH = libs/
ARDUINO_LIBS = Adafruit-GFX-Library SPI Adafruit-PCD8544-Nokia-5110-LCD-library ad9850 Encoder

include /usr/share/arduino/Arduino.mk

