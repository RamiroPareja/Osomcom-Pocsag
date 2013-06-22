#ifndef __IO_H
#define __IO_H

#define initPorts()     ANSEL=0x00; ANSELH=0x00; WPUB=0x10; INTCON2bits.NOT_RABPU=0; TRISA= 0x0B; LATA=0x00; TRISB = 0x30; LATB = 0; TRISC = 0x04; LATC = 0;
    
  
#define GP0				LATAbits.LATA5
#define GP1				LATAbits.LATA4
#define GP2				LATCbits.LATC5
#define GP3				LATCbits.LATC4

#define GP0_on()        GP0 = 1;
#define GP0_off()       GP0 = 0;
#define GP0_toggle()    GP0 = !GP0;

#define GP1_on()        GP1 = 1;
#define GP1_off()       GP1 = 0;
#define GP1_toggle()    GP1 = !GP1;

#define GP2_on()        GP2 = 1;
#define GP2_off()       GP2 = 0;
#define GP2_toggle()    GP2 = !GP2;

#define GP3_on()        GP3 = 1;
#define GP3_off()       GP3 = 0;
#define GP3_toggle()    GP3 = !GP3;


#define LED0            LATAbits.LATA2
#define LED0_on()       LED0 = 1;    
#define LED0_off()      LED0 = 0;    
#define LED0_toggle()   LED0 = !LED0;    


#define PB0		PORTBbits.RB4


#define TX_PIN          LATBbits.LATB7
#define RX_PIN          PORTBbits.RB5


#define SDO		LATCbits.LATC7
#define SCK		LATBbits.LATB6
#define LE		LATCbits.LATC3
#define CE		LATCbits.LATC6
#define MUXOUT		PORTCbits.RC2


#define RADIO_DATA	LATCbits.LATC0
#define RADIO_CLK	LATCbits.LATC1



void io_processLoop(void);
void led_blinkStatus(void);




#endif
