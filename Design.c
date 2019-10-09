#include "address_map_arm.h"
#include "lcd_driver.h"
#include "lcd_graphic.h"


volatile int *buttons = (int*)KEY_BASE;
volatile char *led = (char*)LED_BASE;
volatile char *ptr = (char*)SW_BASE;
volatile int *timer_ptr = (int *)HPS_TIMER0_BASE;
volatile char *sml = (char*)HEX3_HEX0_BASE;
volatile char *sml2 = (char*)HEX5_HEX4_BASE;
unsigned char lookupTable[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67};
int ms1=0, ms2=0, s1=0, s2=0, m1=0, m2 = 0;
int hs1=0, hs2=0, hl1=0, hl2=0, h1=0, h2 = 0;
int running = 0;
volatile int delay_count;
volatile int x, y, length, dir_x, dir_y;



int ReadSwitches(void)
{
    return *(ptr) %8;
}


void panoramaX()
{
	moveTo(0,((SCREEN_HEIGHT/2)-length/2));
	while(x + length <= (SCREEN_WIDTH - 1))
	{
		LCD_rect(x, y, length, length, 0, 1);
		x += dir_x;
		*led=0b1111111111;
		displayX_Y();
		LCD_rect(x, y, length, length, 1, 1);
		refresh_buffer();
		for (delay_count = 100000; delay_count != 0; --delay_count);
	}
	clear();
}

void panoramaZ()
{
	moveTo((SCREEN_WIDTH/2),(SCREEN_HEIGHT-(length+3)));
	
	while(y >=1&& dir_y == 1)
	{
		LCD_rect(x, y, length, length, 0, 1);
		y -= dir_y;
		*led=0b1111111111;
		displayX_Y();
		LCD_rect(x, y, length, length, 1, 1);
		refresh_buffer();
		for (delay_count = 100000; delay_count != 0; --delay_count);
	}
	clear();
}

void center()
{
	moveTo((SCREEN_WIDTH/2),(SCREEN_HEIGHT/2));
}

void totalCapture()
{
	moveTo(0,0);
	while(x<=SCREEN_WIDTH&&y<=(SCREEN_HEIGHT-length))
	{
		*led=0b1111111111;
		LCD_rect(x, y, length, length, 0, 1);
		
		if ((x + length >= SCREEN_WIDTH - 1 && dir_x == 1) ||(x <= 0 && dir_x == -1)){
			y += length;
            dir_x = -dir_x;
		}
		x += dir_x;

		displayX_Y();
		LCD_rect(x, y, length, length, 1, 1);
		refresh_buffer();
		for (delay_count = 100000; delay_count != 0; --delay_count);
		
		
	}
	clear();
}



void activeLeft()
{
	*led=0b0000000001;
	if(x>=1&& dir_x == 1)
	{
		x -= dir_x;
	}
	
}

void activeRight()
{
	*led=0b0000000010;
	if(x + length <= (SCREEN_WIDTH - 1)&& dir_x == 1)
	{
		x += dir_x;
	}
	
}

void activeUp()
{
	*led=0b0000000100;
	if(y + length <= SCREEN_HEIGHT - 2&& dir_y == 1)
	{
	y += dir_y;
	}
	
}

void activeDown()
{
	*led=0b0000001000;
	if(y >= 1 && dir_y == 1)
	{
	y -= dir_y;
	}
}

void clear()
{
	*led=0x0;
}

void moveTo(int newX,int newY)
{
	while(x!=newX||y!=newY)
	{
		LCD_rect(x, y, length, length, 0, 1);
		
		if(x>newX)
		{
			x -= dir_x;
		}
		else if(x<newX)
		{
			x += dir_x;
		}
		
		if(y>newY)
		{
			y -= dir_y;
		}
		else if(y<newY)
		{
			y += dir_y;
		}


		displayX_Y();
		LCD_rect(x, y, length, length, 1, 1);
		refresh_buffer();
		for (delay_count = 100000; delay_count != 0; --delay_count);
		
		
	}
}

void displayX_Y()
{
	int h1x, h1y,h2x,h2y,h3x;
	h1x = x;
	h1y = y;
	h2x=0;
	h2y=0;
	h3x=0;
	
	if(x>=100)
	{
		h3x=1;
		h1x-=100;	
		while(h1x>=10)
		{
			h1x-=10;
			h2x+=1;
		}	
			
	}
	else
	{
		while(h1x>=10)
		{
			h1x-=10;
			h2x+=1;
		}
	}
	
	while(h1y>=10)
	{
		h1y-=10;
		h2y+=1;
	}
	
	*sml = lookupTable[h1x];
	*(sml + 1) = lookupTable[h2x];
	*(sml + 2) = lookupTable[h3x];
	*(sml2) = lookupTable[h1y];
	*(sml2+1) = lookupTable[h2y];	
}




int main(void)
{


char text_top_lcd[17]    = "   INTEL FPGA   \0";
char text_bottom_lcd[17] = "COMPUTER SYSTEMS\0";

init_spim0();
init_lcd();

clear_screen();

/* initialize first position of box */
    x      = SCREEN_WIDTH/2;
    y      = SCREEN_HEIGHT/2;
    length = 8;
    dir_x  = 1;
    dir_y  = 1;
	displayX_Y();
    LCD_rect(x, y, length, length, 1, 1);

	refresh_buffer();







	
*(timer_ptr + 2) = 0b000;
int runTime = 1000000;
*(timer_ptr + 2) = 0b011;

	while (1)
	{
		LCD_rect(x, y, length, length, 0, 1);
		
		if(ReadSwitches()==1)
		{
			if(*buttons == 0b1000)
			{
				panoramaX();
			}
			else if(*buttons == 0b0100)
			{
				panoramaZ();
			}
			else if(*buttons == 0b0010)
			{
				center();
			}
			else if(*buttons == 0b0001)
			{
				totalCapture();
			}
			
		}
		else
		{
			if(*buttons == 0b1000)
			{
				activeLeft();
			}
			else if(*buttons == 0b0100)
			{
				activeRight();
			}
			else if(*buttons == 0b0010)
			{
				activeUp();
			}
			else if(*buttons == 0b0001)
			{
				activeDown();
			}
			else
			{
				clear();
			}
		}
		
		displayX_Y();
		LCD_rect(x, y, length, length, 1, 1);
		refresh_buffer();
		for (delay_count = 100000; delay_count != 0; --delay_count);
	}
}

