#include "../../../inc/MarlinConfig.h"
#include "../../gcode.h"

#ifdef MALYAN_LCD
char wifi_ssid[32];
char wifi_password[32];
uint8_t wifi_ip[4];
extern void write_to_lcd_P(const char * const message);

void GcodeSuite::M550() {
    //extern char wifi_ssid[32];
    if (parser.string_arg!=0) strcpy(wifi_ssid,parser.string_arg);
    else SERIAL_ECHOLN(wifi_ssid);
}

void GcodeSuite::M551() {
    //extern char wifi_password[32];
    if (parser.string_arg!=0)
    {
        //extern char wifi_ssid[32];

        strcpy(wifi_password,parser.string_arg);
        write_to_lcd_P((char *)"{WS:");
        write_to_lcd_P(wifi_ssid);
        write_to_lcd_P((char *)"}{WP:");
        write_to_lcd_P(wifi_password);
        write_to_lcd_P((char *)"}");
    }
    else SERIAL_ECHOLN(wifi_password);
}

void GcodeSuite::M552() {
    //extern uint8_t wifi_ip[4];
    write_to_lcd_P((char *)"{R:A}");
    char ip[64];
    sprintf(ip,"%d.%d.%d.%d",wifi_ip[0],wifi_ip[1],wifi_ip[2],wifi_ip[3]);
    SERIAL_ECHOLN(ip);

    /*MYSERIAL.print(wifi_ip[0],10);
    SERIAL_ECHO('.');
    MYSERIAL.print(wifi_ip[1],10);
    SERIAL_ECHO('.');
    MYSERIAL.print(wifi_ip[2],10);
    SERIAL_ECHO('.');
    MYSERIAL.print(wifi_ip[3],10);
    SERIAL_EOL;*/
}

#else
void GcodeSuite::M550(){}
void GcodeSuite::M551(){}
#endif