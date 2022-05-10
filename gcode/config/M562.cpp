#include "../../inc/MarlinConfig.h"

#include "../gcode.h"

uint8_t invert_byte;
//extern uint8_t invert_byte;
void report_M562()
{
    SERIAL_ECHOLN("M562 Report:");
    //SERIAL_PROTOCOLLN("DIR");
    SERIAL_ECHOLN("XYZAB");
    if (INVERT_X_DIR) SERIAL_ECHO('+');else SERIAL_ECHO('-');
    if (INVERT_Y_DIR) SERIAL_ECHO('+');else SERIAL_ECHO('-');
    if (INVERT_Z_DIR) SERIAL_ECHO('+');else SERIAL_ECHO('-');
    if (INVERT_E0_DIR) SERIAL_ECHO('+');else SERIAL_ECHO('-');
    if (INVERT_E1_DIR) SERIAL_ECHO('+');else SERIAL_ECHO('-');

    /*SERIAL_PROTOCOL('|');
    if (X_HOME_DIR==1) SERIAL_PROTOCOL('+');else SERIAL_PROTOCOL('-');
    if (Y_HOME_DIR==1) SERIAL_PROTOCOL('+');else SERIAL_PROTOCOL('-');*/
    SERIAL_ECHOLN("");
}

void GcodeSuite::M562() {

  //if (parser.seen('S'))
  //  stepper.abort_on_endstop_hit = parser.value_bool();
    
    /*if (parser.seen('P')) invert_byte=code_value();
    else if (parser.seen('M'))
    {
        if (parser.seen('X')) invert_byte^=0x21;
        if (parser.seen('Y')) invert_byte^=0x42;
    }
    else*/
    {
        if (parser.seen('X')) invert_byte^=0x1;
        if (parser.seen('Y')) invert_byte^=0x2;
        if (parser.seen('Z')) invert_byte^=0x4;
        if (parser.seen('E')) invert_byte^=0x8;
        if (parser.seen('A')) invert_byte^=0x8;
        if (parser.seen('B')) invert_byte^=0x10;
    }
    report_M562();
}

