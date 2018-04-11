#include<LPC17xx.h>
#include <stdio.h>

#define TRIG (1<<10) //P2.10
#define ECHO (1<<11) //P2.11

void lcd_init(void);
void wr_cn(void);
void clr_disp(void);
void delay_lcd(unsigned int);
void lcd_com(void);
void wr_dn(void);
void lcd_data(void);
void clear_ports(void);
void lcd_puts(unsigned char *);
void delay_1second(void);
void delay_10micro(void);

unsigned char arr[];
unsigned int timeTaken=0;
unsigned float dist=0;

int main(void)
{
	SystemInit();
	SystemCoreClockUpdate();
	LPC_GPIO2->FIODIR |= 1<<10;    //Set P2.10(TRIG) as output
	LPC_GPIO2->FIODIR &= ~(1<<11); //Set P2.11(ECHO) as input
	LPC_GPIO2->FIOCLR = 1<<10;    //Set P2.10 LOW initially
	while(1)
	{
	    lcd_init();
		//Output 10us HIGH on TRIG pin
		LPC_GPIO2->FIOSET |= TRIG;

		delay_10micro();    //function for 10microseconds delay

		LPC_GPIO2->FIOCLR |= TRIG;

		while(!(LPC_GPIO2->FIOPIN & ECHO)); //Wait for a HIGH on ECHO pin

		counter_init(); //Start counting

		while(LPC_GPIO2->FIOPIN & ECHO); //Wait for a LOW on ECHO pin

		timeTaken = LPC_TIM2->TC; //Store the counter value(us) in timeTaken

		dist = (0.0343 * timeTaken)/2; //Find the distance, 0.0343 is the speed of sound in air in cm/us

        clr_disp();
		sprintf(arr,"Dist = %0.2fcm\n",dist);       //displaying the string on the LCD
		lcd_puts(&arr[0]);

		delay_1second(); //function for 1 second delay
	}
}

void counter_init()    //using TIMER2
{
    LPC_TIM2->TCR=0x02;
    LPC_TIM2->CTCR=0;
    LPC_TIM2->PR=2;
    LPC_TIM2->TCR=0x01;
    return;
}

void delay_10micro(void)    //using TIMER1
{
    LPC_TIM1->TCR=0x02;
    LPC_TIM1->CTCR=0;
    LPC_TIM1->PR=0;
    LPC_TIM1->MR0=9;
    LPC_TIM1->MCR=0x04;
    LPC_TIM1->EMR=0x20;
    LPC_TIM1->TCR=0x01;
    while(!(LPC_TIM1->EMR)&(0x01));
}

void delay_1second(void)    //using TIMER0
{
    LPC_TIM0->TCR=0x02;
    LPC_TIM0->CTCR=0;
    LPC_TIM0->PR=999;
    LPC_TIM0->MR0=2999;
    LPC_TIM0->MCR=0x04;
    LPC_TIM0->EMR=0x20;
    LPC_TIM0->TCR=0x01;
    while(!(LPC_TIM0->EMR)&(0x01));
}

void lcd_init(void) {
    LPC_PINCON->PINSEL3 &= 0xfc003fff;
    LPC_GPIO0->FIODIR |= DT_CTRL;
    LPC_GPIO0->FIODIR |= RS_CTRL;
    LPC_GPIO0->FIODIR |= EN_CTRL;

    clear_ports();
    delay_lcd(3200);

    temp2 = (0x30<<19);
    wr_cn();
    delay_lcd(30000);

    temp2 = (0x30<<19);
    wr_cn();
    delay_lcd(30000);

    temp2 = (0x30<<19);
    wr_cn();
    delay_lcd(30000);

    temp2 = (0x20<<19);
    wr_cn();
    delay_lcd(30000);

    temp1 = 0x28;
    lcd_com();
    delay_lcd(30000);

    temp1 = 0x0c;
    lcd_com();
    delay_lcd(800);

    temp1 = 0x06;
    lcd_com();
    delay_lcd(800);

    temp1 = 0x01;
    lcd_com();
    delay_lcd(10000);

    temp1 = 0x80;
    lcd_com();
    delay_lcd(800);

    return;
}

void lcd_com(void) {
    temp2 = temp1 & 0xf0;
    temp2 = temp2 << 19;
    wr_cn();
    temp2 = temp1 & 0x0f;
    temp2 = temp2 << 23;
    wr_cn();
    delay_lcd(1000);

    return;
}

void wr_cn(void) {
    clear_ports();
    LPC_GPIO0->FIOPIN = temp2;
    LPC_GPIO0->FIOCLR = RS_CTRL;
    LPC_GPIO0->FIOSET = EN_CTRL;
    delay_lcd(25);
    LPC_GPIO0->FIOCLR = EN_CTRL;

    return;
}

void lcd_data(void) {
    temp2 = temp1 & 0xf0;
    temp2 = temp2 << 19;
    wr_dn();
    temp2 = temp1 & 0x0f;
    temp2 = temp2 << 23;
    wr_dn();
    delay_lcd(100000);

    return;
}

void wr_dn(void) {
    clear_ports();
    LPC_GPIO0->FIOPIN = temp2;
    LPC_GPIO0->FIOSET = RS_CTRL;
    LPC_GPIO0->FIOSET = EN_CTRL;
    delay_lcd(25);
    LPC_GPIO0->FIOCLR = EN_CTRL;

    return;
}

void delay_lcd(unsigned int r1) {
    unsigned int r;
    for(r=0; r<r1; r++);
}

void clr_disp(void) {
    temp1 = 0x01;
    lcd_com();
    delay_lcd(10000);

    return;
}

void clear_ports(void) {
    LPC_GPIO0->FIOCLR = DT_CTRL;
    LPC_GPIO0->FIOCLR = RS_CTRL;
    LPC_GPIO0->FIOCLR = EN_CTRL;

    return;
}

void lcd_puts(unsigned char *buf1) {
    unsigned int i = 0;
    while(buf1[i]!='\0')
    {
       temp1 = buf1[i];
       lcd_data();
       i++;
    }
}
