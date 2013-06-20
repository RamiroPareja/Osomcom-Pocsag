#ifndef _TOOLS_H_
#define _TOOLS_H_

unsigned char ascii2bin(char *, unsigned char);
void bin2ascii(unsigned char, char *, unsigned char);
void byte2hex( unsigned char c, char *output);
void nibble2hex( unsigned char c, char *output);

void delay_s(void);

#endif