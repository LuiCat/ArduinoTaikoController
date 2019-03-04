#ifndef JOYSTICK_h
#define JOYSTICK_h

#include <HID.h>

/*** 
 *  All enum, class and descriptor definitions are originally written by progmem in library Switch-Fightstick(https://github.com/progmem/Switch-Fightstick).
 *  Original code can be found in:
 *  progmem/Switch-Fightstick/Joystick.h
 *  https://github.com/progmem/Switch-Fightstick/blob/master/Joystick.h
***/

// Enumeration for joystick buttons.
typedef enum {
  SWITCH_BTN_NONE    = 0x00,
  SWITCH_BTN_Y       = 0x01,
  SWITCH_BTN_B       = 0x02,
  SWITCH_BTN_A       = 0x04,
  SWITCH_BTN_X       = 0x08,
  SWITCH_BTN_L       = 0x10,
  SWITCH_BTN_R       = 0x20,
  SWITCH_BTN_ZL      = 0x40,
  SWITCH_BTN_ZR      = 0x80,
  SWITCH_BTN_SELECT  = 0x100,
  SWITCH_BTN_START   = 0x200,
  SWITCH_BTN_LCLICK  = 0x400,
  SWITCH_BTN_RCLICK  = 0x800,
  SWITCH_BTN_HOME    = 0x1000,
  SWITCH_BTN_CAPTURE = 0x2000,
} JoystickButtons_t;

// Enumeration for joystick hats.
typedef enum {
  SWITCH_HAT_U  = 0x00,
  SWITCH_HAT_UR = 0x01,
  SWITCH_HAT_R  = 0x02,
  SWITCH_HAT_DR = 0x03,
  SWITCH_HAT_D  = 0x04,
  SWITCH_HAT_DL = 0x05,
  SWITCH_HAT_L  = 0x06,
  SWITCH_HAT_UL = 0x07,
  SWITCH_HAT_CENTER = 0x08,
} JoystickHats_t;

struct Joystick_
{
  uint16_t Button = SWITCH_BTN_NONE; // 16 buttons; see JoystickButtons_t for bit mapping
  uint8_t  HAT = SWITCH_HAT_CENTER;    // HAT switch; one nibble w/ unused nibble
  uint8_t  LX = 128;     // Left  Stick X
  uint8_t  LY = 128;     // Left  Stick Y
  uint8_t  RX = 128;     // Right Stick X
  uint8_t  RY = 128;     // Right Stick Y
  uint8_t  VendorSpec = 0;
  
  Joystick_();
	void sendState();
};

extern Joystick_ Joystick;

#endif
