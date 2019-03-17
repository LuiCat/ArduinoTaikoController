#include "Joystick.h"
#include "HIDReportData.h"

/*** 
 *  All enum, class and descriptor definitions are originally written by progmem in library Switch-Fightstick(https://github.com/progmem/Switch-Fightstick).
 *  Original code can be found in:
 *  progmem/Switch-Fightstick/Joystick.h
 *  https://github.com/progmem/Switch-Fightstick/blob/master/Joystick.h
***/

/*** 
 *  Descriptor modified from 
 *  progmem/Switch-Fightstick/Joystick.c
 *  https://github.com/progmem/Switch-Fightstick/blob/master/Joystick.h
***/

static const uint8_t _hidReportDescriptor[] PROGMEM = {
  HID_RI_USAGE_PAGE(8,1), /* Generic Desktop */
  HID_RI_USAGE(8,5), /* Joystick */
  HID_RI_COLLECTION(8,1), /* Application */
    // Report ID
    HID_RI_REPORT_ID(8,3),
    // Buttons (2 bytes)
    HID_RI_LOGICAL_MINIMUM(8,0),
    HID_RI_LOGICAL_MAXIMUM(8,1),
    HID_RI_PHYSICAL_MINIMUM(8,0),
    HID_RI_PHYSICAL_MAXIMUM(8,1),
    // The Switch will allow us to expand the original HORI descriptors to a full 16 buttons.
    // The Switch will make use of 14 of those buttons.
    HID_RI_REPORT_SIZE(8,1),
    HID_RI_REPORT_COUNT(8,16),
    HID_RI_USAGE_PAGE(8,9),
    HID_RI_USAGE_MINIMUM(8,1),
    HID_RI_USAGE_MAXIMUM(8,16),
    HID_RI_INPUT(8,2),
    // HAT Switch (1 nibble)
    HID_RI_USAGE_PAGE(8,1),
    HID_RI_LOGICAL_MAXIMUM(8,7),
    HID_RI_PHYSICAL_MAXIMUM(16,315),
    HID_RI_REPORT_SIZE(8,4),
    HID_RI_REPORT_COUNT(8,1),
    HID_RI_UNIT(8,20),
    HID_RI_USAGE(8,57),
    HID_RI_INPUT(8,66),
    // There's an additional nibble here that's utilized as part of the Switch Pro Controller.
    // I believe this -might- be separate U/D/L/R bits on the Switch Pro Controller, as they're utilized as four button descriptors on the Switch Pro Controller.
    HID_RI_UNIT(8,0),
    HID_RI_REPORT_COUNT(8,1),
    HID_RI_INPUT(8,1),
    // Joystick (4 bytes)
    HID_RI_LOGICAL_MAXIMUM(16,255),
    HID_RI_PHYSICAL_MAXIMUM(16,255),
    HID_RI_USAGE(8,48),
    HID_RI_USAGE(8,49),
    HID_RI_USAGE(8,50),
    HID_RI_USAGE(8,53),
    HID_RI_REPORT_SIZE(8,8),
    HID_RI_REPORT_COUNT(8,4),
    HID_RI_INPUT(8,2),
    // ??? Vendor Specific (1 byte)
    // This byte requires additional investigation.
    HID_RI_USAGE_PAGE(16,65280),
    HID_RI_USAGE(8,32),
    HID_RI_REPORT_COUNT(8,1),
    HID_RI_INPUT(8,2),
    // Output (8 bytes)
    // Original observation of this suggests it to be a mirror of the inputs that we sent.
    // The Switch requires us to have these descriptors available.
    HID_RI_USAGE(16,9761),
    HID_RI_REPORT_COUNT(8,8),
    HID_RI_OUTPUT(8,2),
  HID_RI_END_COLLECTION(0),
};

Joystick_::Joystick_()
{
	static HIDSubDescriptor node(_hidReportDescriptor, sizeof(_hidReportDescriptor));
	HID().PrependDescriptor(&node); // This descriptor need to be the first one so that the device can be recognized by Switch
}

void Joystick_::sendState()
{
	HID().SendRaw(this, 8); // No sending report ID. Switch doesn't deal with that properly.
}

Joystick_ Joystick;
