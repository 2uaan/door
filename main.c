typedef unsigned char u8;


#define RCGCI2C  (*((volatile unsigned long *)0x400FE620))
#define RCGCUART (*((volatile unsigned long *) 0x400FE618))			//Thanh ghi chon module UART

#define GPIODEN_B  (*((volatile unsigned long *)0x4000551C))
#define GPIODIR_B  (*((volatile unsigned long *)0x40005400))
#define GPIODATA_B (*((volatile unsigned long *)0x400053FC))
#define GPIOPUR_B  (*((volatile unsigned long *)0x40005510))

#define RCGCGPIO    (*((volatile unsigned long *)0x400FE608))
#define GPIOAFSEL_A (*((volatile unsigned long *)0x40004420))  
#define GPIOPCTL_A  (*((volatile unsigned long *)0x4000452C))  
#define GPIODIR_A   (*((volatile unsigned long *)0x40004400))  
#define GPIODEN_A   (*((volatile unsigned long *)0x4000451C))	 
#define GPIODATA_A  (*((volatile unsigned long *)0x400043FC))	 
#define GPIOODR_A   (*((volatile unsigned long *)0x4000450C))  
#define GPIOPUR_A  	(*((volatile unsigned long *)0x40004510))

#define I2C1_MCR  (*((volatile unsigned long *)0x40021020))		
#define I2C1_MTPR (*((volatile unsigned long *)0x4002100C))		
#define I2C1_MSA  (*((volatile unsigned long *)0x40021000))		
#define I2C1_MDR  (*((volatile unsigned long *)0x40021008))		
#define I2C1_MCS  (*((volatile unsigned long *)0x40021004))	
	
#define GPIOAFSEL_C (*((volatile unsigned long *) 0x40006420))	//Thanh ghi bat chuc nang thay the cho chan GPIO
#define GPIOPCTL_C  (*((volatile unsigned long *) 0x4000652C))	//Thanh ghi chon chuc nang thay the cho chan GPIO
#define GPIODEN_C   (*((volatile unsigned long *) 0x4000651C))	//Thanh ghi bat che do Digital cho chan GPIO
#define GPIODIR_C   (*((volatile unsigned long *) 0x40006400))	//Thanh ghi cau hinh Input/Output cho chan GPIO
#define GPIODATA_C  (*((volatile unsigned long *) 0x400063FC))	//Thanh ghi chua du lieu cua chan GPIO

#define UART4CTL  (*((volatile unsigned long *) 0x40010030))		
#define UART4IBRD (*((volatile unsigned long *) 0x40010024))		
#define UART4FBRD (*((volatile unsigned long *) 0x40010028))		
#define UART4LCRH (*((volatile unsigned long *) 0x4001002C))		
#define UART4CC   (*((volatile unsigned long *) 0x40010FC8))		
#define UART4FR   (*((volatile unsigned long *) 0x40010018))		
#define UART4DR   (*((volatile unsigned long *) 0x40010000))	

char pass[6] = "555555";

void delay(int ms);
void setup_GPIO(void);


void setup_I2C(void);
void set_slave_address(u8 slave_address);
void setRW(u8 mode);
void write_byte(u8 data, u8 state);
void I2C_LCD_send(u8 full_data, int rs);
void I2C_LCD_init(void);
void display_character(u8 ch, int row, int col);
void display_string(char* ch, int rowf, int colf);
u8 getkey(void);

void setup_UART(void);
char read_char(void);
void reset_UART_FIFO(void);

void delay(int ms){
	unsigned int i;
	for (i = 0; i < ms*2500; i++);
}

void setup_UART(void){											//Ham cau hinh module UART
	RCGCUART |= (1 << 4);											//Chon module UART 4
	while((RCGCUART & (1 << 4)) == 0);				//Cho den khi cau hinh xong
	RCGCGPIO |= (1 << 2);											//UART4 tuong ung voi chan PC4, PC5 ==> chon Port C
	while((RCGCGPIO & (1 << 2)) == 0);
	
	
	GPIOAFSEL_C |= (1 << 4);									//Bat chuc nang thay the
	GPIOPCTL_C |= (1 << 16);									//Chon UART la chuc nang thay the cho chan PC4 PC5
	GPIODEN_C |= (1 << 4) | (1 << 5);					//Bat Digital cho 2 chan PC4 PC5
	GPIODIR_C |= (1 << 5);
	
	
	UART4CTL &= ~(1 << 0);										//Tat module UART de thuc hien cau hinh			
	UART4IBRD = 104;													//Phan nguyen cua Baud rate duoc tinh theo cong thuc trong datasheet
	UART4FBRD = 11;														//Phan phan so cua Baud rate duoc tinh theo cong thuc trong datasheet
	UART4LCRH |= (0x3 << 5);									//Bat che do 8-bits
	UART4CC = 0x0;														//Lua chon clock he thong
	UART4CTL |= (1 << 0) | (1 << 9);					//Bat lai module UART, bat che do nhan du lieu 
}


char read_char(void){												//Ham doc ki tu 
	char c;
  while ((UART4FR & (1 << 4)) != 0); 				//Cho doi du lieu nhan khong trong
  c = UART4DR & 0xFF; 											//Lay du lieu tu thanh ghi UARTDR
	return c;																	//Tra ve du lieu
}

/*
    r1-B0
    r2-B1
    r3-B2
    r4-B3
    c1-B4
    c2-B5
    c3-B6
    c4-A5
*/ 
void setup_GPIO(void){
	RCGCGPIO |= (1 << 1);
	while ((RCGCGPIO & (1 << 1)) == 0);
    
    GPIODEN_B |= 0xFF;
    GPIODIR_B = 0x0F;
    GPIODIR_B &= ~0xF0;
    GPIOPUR_B = 0xF0;
	
}

void setup_I2C(void){									
	RCGCI2C |= (1 << 1);								
	while((RCGCI2C & (1 << 1)) == 0);		
	RCGCGPIO |= (1 << 0);								
	while((RCGCGPIO & (1 << 0)) == 0);	
	
	GPIOAFSEL_A |= (1 << 6) | (1 << 7);	
	GPIOPCTL_A = (3 << 24) | (3 << 28);		
	GPIODEN_A |= (1 << 5) | (1 << 6) | (1 << 7);		
	GPIODIR_A |= (1 << 6) | (1 << 7);		
	GPIODIR_A &= ~(1 << 5);
	GPIOODR_A |= (1 << 7);		
	GPIOPUR_A = (1<< 5);
	
	I2C1_MCR = 0x00;										
	I2C1_MCR |= (1 << 4);								
	I2C1_MTPR = (7 << 0);								
}

void set_slave_address(u8 slave_address){		
	I2C1_MSA = (slave_address << 1);					
}

void setRW(u8 mode){												
	if (mode == 0) I2C1_MSA &= ~(1 << 0);			
	else I2C1_MSA |= (1 << 0);								
}

void write_byte(u8 data, u8 state){					
	I2C1_MDR = data;													
	I2C1_MCS = state;													
	
	while((I2C1_MCS & (1 << 0)) == 0);				
	
	if((I2C1_MCS & (1 << 1)) != 0){						
		if((I2C1_MCS & (1 << 1)) != 0){
			
		}else{
			I2C1_MCS = (1 << 2);
			while((I2C1_MCS & (1 << 0)) != 0);
		}
	}
}


/*D1 D2 D3 D4 BL EN RW RS*/
void I2C_LCD_send(u8 full_data, int rs) {		
  u8 	upper = full_data & 0xF0,							
			lower = (full_data << 4);							
	
	u8 BL, RS, RW, EN, data;									
	BL = 0x08;																
	RS = 0x00 | rs;														 
	RW = 0x00;																
	EN = 0x04;																
	
	data = upper | BL | EN | RW | RS;					
	
	write_byte(data, 7);											
	delay(5);
	write_byte(data & ~EN, 7);								
	delay(5);																	
	
	data = lower | BL | EN | RW | RS;					
	
	write_byte(data, 7);
	delay(5);
	write_byte(data & ~EN, 7);
	delay(5);
}


void display_character(u8 ch, int row, int col){
	u8 pos;
		
		if (row == 0) pos = 0x80;
		else if (row == 1) pos = 0xC0;
		else if (row == 2) pos = 0x94;
		else pos = 0xD4;
		
		pos = pos + col;
		
		I2C_LCD_send(pos, 0);
		delay(2);
		I2C_LCD_send(ch, 1);
		delay(2);
}

void I2C_LCD_init(void) {					
    delay(50);
    I2C_LCD_send(0x02, 0);		
		delay(50);				
    I2C_LCD_send(0x28, 0);		
		delay(50);
    I2C_LCD_send(0x0C, 0); 		
		delay(50);
    I2C_LCD_send(0x01, 0); 		
    delay(50);
    I2C_LCD_send(0x80, 0); 		
    delay(50);
}

								u8 getkey(void){
									u8 key = '.';
										unsigned int i;
										GPIODATA_B &= ~0x01;
										GPIODATA_B |= 0x0E;
										if ((GPIODATA_B & 0x10) == 0) key = '1';
										if ((GPIODATA_B & 0x20) == 0) key = '2';
										if ((GPIODATA_B & 0x40) == 0) key = '3';
										if ((GPIODATA_A & 0x20) == 0) key = 'A';
										GPIODATA_B |= 0x0F;

										delay(10);

										GPIODATA_B &= ~0x02;
										GPIODATA_B |= 0x0D;
										if ((GPIODATA_B & 0x10) == 0) key = '4';
										if ((GPIODATA_B & 0x20) == 0) key = '5';
										if ((GPIODATA_B & 0x40) == 0) key = '6';
										if ((GPIODATA_A & 0x20) == 0) key = 'B';
										GPIODATA_B |= 0x0F;

										delay(10);

										GPIODATA_B &= ~0x04;
										GPIODATA_B |= 0x0B;
										if ((GPIODATA_B & 0x10) == 0) key = '7';
										if ((GPIODATA_B & 0x20) == 0) key = '8';
										if ((GPIODATA_B & 0x40) == 0) key = '9';
										if ((GPIODATA_A & 0x20) == 0) key = 'C';
										GPIODATA_B |= 0x0F;

										delay(10);

										GPIODATA_B &= ~0x08;
										GPIODATA_B |= 0x07;
										if ((GPIODATA_B & 0x10) == 0) key = 'O';
										if ((GPIODATA_B & 0x20) == 0) key = '0';
										if ((GPIODATA_B & 0x40) == 0) key = '=';
										if ((GPIODATA_A & 0x20) == 0) key = 'D';
										GPIODATA_B |= 0x0F;

										delay(10);
									return key;
								}

void display_string(char* ch, int rowf, int colf){
	char *temp = ch;
	while(*temp != '\0'){
		display_character(*temp, rowf, colf);
		colf++;
		temp++;
		if (colf == 21){
			if (rowf == 1){
				rowf++;
				colf = 1;
			}else break;
		}
	}
}

char check[6];

void clear_check(void);
void clear_check(void){
	unsigned int i;
	for (i = 0; i< 6; i++) check[i] = ' ';
}

int check_pass(void);
int check_pass(void){
	unsigned int i;
	for (i = 0; i< 6; i++) if (pass[i] != check[i]) return 0;
	return 1;
}

void run_keypad(void);
void run_keypad(void){
		display_string("|-----PASSWORD-----|", 0,0);
		
		display_string("<|******|>", 2, 5);
		int count = 6;
    while(1){
				char key = getkey();
				if (count == 12) {
					int result = check_pass();
					if (result == 0){
						count = 0;
						display_string("xxxxxx", 1,7);
						delay(200);
						display_string("      ", 1,7);
						delay(200);
						display_string("xxxxxx", 1,7);
						delay(200);
						display_string("      ", 1,7);
						delay(200);
						display_string("xxxxxx", 1,7);
						delay(200);
						display_string("      ", 1,7);
						delay(200);
						display_string("******", 2,7);
						count = 6;
					}else{
						display_string("<Correct.>", 2, 5 );
						GPIODATA_C |= (1 << 5);
						delay(4000);
						GPIODATA_C &= ~(1 << 5);
						I2C_LCD_send(0x01, 0);
						display_string("DOOR", 0, 8);
						display_string("A-By Password", 1, 3);
						display_string("B-By RFID card", 2, 3);
						display_string("C-By Bluetooth", 3, 3);
						break;
					}
				}
				if (key != '.'){
					if (key == 'O') {
						count = 6;
						display_string("******", 2,7);
					}else if((int)key > 47 && (int)key < 58){
						check[count - 6] = key;
						count++;
						display_character(key, 2, count);
					}else if(key == 'A'){
						if (count > 6) {
							display_character('*', 2, count);
							count--;
						}
					}else if(key == 'D') {
						I2C_LCD_send(0x01, 0);
						display_string("OPEN", 0, 8);
						display_string("A-By Password", 1, 3);
						display_string("B-By RFID card", 2, 3);
						display_string("C-By Bluetooth", 3, 3);
						break;
					}
				}
				
        delay(100);
    }
}


void run_RFID(void);
/*
void run_RFID(void){
	
	display_string("|-------CARD-------|", 0,0);
	char before = '*';
	
	while(1){
		char key = getkey();
		
		if (before != '*'){
			if (read_char() != before) {						//Kiem tra neu du lieu nhan thay doi
												//Bat den
				delay(3000);													//Delay 3 giay
			}		
		}else{
				
				delay(1000);
		}
												//Tat den
		
		
		if (key != '.'){
			if(key == 'D') {
					I2C_LCD_send(0x01, 0);
					display_string("DOOR", 0, 8);
					display_string("A-By Password", 1, 3);
					display_string("B-By RFID card", 2, 3);
					display_string("C-By Bluetooth", 3, 3);
					break;
			}else{
				display_string("Minh Quan", 2, 1);
			}
		}
		//before = read_char();
	}

}
*/

void run_bluetooth(void);
void run_bluetooth(void){
	
	display_string("|-----BLUETOOTH----|", 0,0);
	display_string("CLOSED", 2, 7);
	char before = '*';
	
	while(1){
		char key = getkey();
		
		if (before != '*'){
			if (read_char() != before) {						//Kiem tra neu du lieu nhan thay doi
				display_string(" OPEN ", 2, 7);
				GPIODATA_C |= (1 << 5);
				delay(4000);
				GPIODATA_C &= ~(1 << 5);
				I2C_LCD_send(0x01, 0);
				display_string("DOOR", 0, 8);
				display_string("A-By Password", 1, 3);
				display_string("B-By RFID card", 2, 3);
				display_string("C-By Bluetooth", 3, 3);
				break;
			}		
		}else{
				
				delay(1000);
		}
		display_string("CLOSED", 2, 7);
												//Tat den
		before = read_char();
	}

}

int st = 0;

int main(void){
    setup_GPIO();
    setup_I2C();
		setup_UART();
    set_slave_address(0x27);
		setRW(0);
    I2C_LCD_init();
		char num = '.';
		display_string("DOOR", 0, 8);
		display_string("A-By Password", 1, 3);
		display_string("B-By RFID card", 2, 3);
		display_string("C-By Bluetooth", 3, 3);
		while(1){
			num = getkey();
			switch (num){
				case 'A':{
					I2C_LCD_send(0x01, 0);
					run_keypad();
					num = '.';
					break;
				}
				case 'B':{
					I2C_LCD_send(0x01, 0);
					run_RFID();
					num = '.';
					break;
				}
				case 'C':{
					I2C_LCD_send(0x01, 0);
					run_bluetooth();
					num = '.';
					break;
				}
				default:{
					if(st == 0){
						display_string("--->", 0, 12);
						display_string("    ", 0, 4);
						st = 1;
					}else{
						display_string("<---", 0, 4);
						display_string("    ", 0, 12);
						st = 0;
					}
					break;
				}
			}
			delay(100);
		}
}
