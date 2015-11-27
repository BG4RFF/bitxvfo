#OARD_TAG    = uno

BOARD_TAG     = mega2560
MONITOR_PORT  = /dev/ttyACM0

USER_LIB_PATH = libs/
ARDUINO_LIBS = AdafruitGFXLibrary SPI pcd8544library ad9850 Encoder

include /usr/share/arduino/Arduino.mk

