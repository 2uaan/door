typedef unsigned char u8;

#define RCGCGPIO (*((volatile unsigned long *)0x400FE608))
#define RCGCI2C  (*((volatile unsigned long *)0x400FE620))
#define RCGCUART (*((volatile unsigned long *) 0x400FE618))			//Thanh ghi chon module UART

#define GPIODEN_B  (*((volatile unsigned long *)0x4000551C))
#define GPIODIR_B  (*((volatile unsigned long *)0x40005400))
#define GPIODATA_B (*((volatile unsigned long *)0x400053FC))
#define GPIOPUR_B  (*((volatile unsigned long *)0x40005510))
	
#define GPIOAFSEL_A (*((volatile unsigned long *)0x40004420))  //Thanh ghi bat chuc nang thay the cho chan GPIOD
#define GPIOPCTL_A  (*((volatile unsigned long *)0x4000452C))  //Thanh ghi chon chuc nang thay the cho chan GPIOD
#define GPIODIR_A   (*((volatile unsigned long *)0x40004400))  //Thanh ghi set chuc nang input/output cho chan GPIOD
#define GPIODEN_A   (*((volatile unsigned long *)0x4000451C))	 //Thanh ghi bat che do tin hieu digital cho chan GPIOD
#define GPIODATA_A  (*((volatile unsigned long *)0x400043FC))	 //Thanh ghi chua du lieu chan GPIOD
#define GPIOODR_A   (*((volatile unsigned long *)0x4000450C))  //Thanh ghi bat drain control cho chan GPIOD
#define GPIOPUR_A  (*((volatile unsigned long *)0x40004510))

#define I2C1_MCR  (*((volatile unsigned long *)0x40021020))		//Thanh ghi cau hinh che do Master/Slave
#define I2C1_MTPR (*((volatile unsigned long *)0x4002100C))		//Thanh ghi thiet lap chu ki cho SCL
#define I2C1_MSA  (*((volatile unsigned long *)0x40021000))		//Thanh ghi chua dia chi Slave va bit R/W
#define I2C1_MDR  (*((volatile unsigned long *)0x40021008))		//Thanh ghi chua du lieu
#define I2C1_MCS  (*((volatile unsigned long *)0x40021004))	
	
#define GPIOAFSEL_C (*((volatile unsigned long *) 0x40006420))	//Thanh ghi bat chuc nang thay the cho chan GPIO
#define GPIOPCTL_C  (*((volatile unsigned long *) 0x4000652C))	//Thanh ghi chon chuc nang thay the cho chan GPIO
#define GPIODEN_C   (*((volatile unsigned long *) 0x4000651C))	//Thanh ghi bat che do Digital cho chan GPIO
#define GPIODIR_C   (*((volatile unsigned long *) 0x40006400))	//Thanh ghi cau hinh Input/Output cho chan GPIO
#define GPIODATA_C  (*((volatile unsigned long *) 0x400063FC))	//Thanh ghi chua du lieu cua chan GPIO

#define UART4CTL  (*((volatile unsigned long *) 0x40010030))		//Thanh ghi chua cac bit dieu khien UART
#define UART4IBRD (*((volatile unsigned long *) 0x40010024))		//Thanh ghi chua phan nguyen cua Baud rate
#define UART4FBRD (*((volatile unsigned long *) 0x40010028))		//Thanh ghi chua phan phan so cua Baud rate
#define UART4LCRH (*((volatile unsigned long *) 0x4001002C))		//Thanh ghi thiet lap du lieu (do dai, stop bit, parity, . . .)
#define UART4CC   (*((volatile unsigned long *) 0x40010FC8))		//Thanh ghi kiem soat nguon xung cho module UART
#define UART4FR   (*((volatile unsigned long *) 0x40010018))		//Thanh ghi chua cac co trang thai cua module UART
#define UART4DR   (*((volatile unsigned long *) 0x40010000))	

char pass[6] = "123123";

void delay(int ms);
void setup_GPIO(void);


void setup_I2C(void);
void set_slave_address(u8 slave_address);
void setRW(u8 mode);
void write_byte(u8 data, u8 state);
void I2C_LCD_send(u8 full_data, int rs);
void I2C_LCD_init(void);
void LCD_showtime(unsigned int value, u8 type);
void display_character(u8 ch, int row, int col);
void display_string(char* ch, int rowf, int colf);

u8 getkey(void);

void delay(int ms){
	unsigned int i;
	for (i = 0; i < ms*2100; i++);
}

/*
    r1-B0
    r2-B1
    r3-B2
    r4-B3
    c1-B4
    c2-B5
    c3-B6
    c4-B7
*/ 
void setup_GPIO(void){
	RCGCGPIO |= (1 << 1);
	while ((RCGCGPIO & (1 << 1)) == 0);
    
    GPIODEN_B |= 0xFF;
    GPIODIR_B = 0x0F;
    GPIODIR_B &= ~0xF0;
    GPIOPUR_B = 0xF0;
	
}

void setup_I2C(void){									//Ham cau hinh I2C module 3
	RCGCI2C |= (1 << 1);								//Chon module 3 cua I2C
	while((RCGCI2C & (1 << 1)) == 0);		//Doi bit duoc thiet lap xong
	RCGCGPIO |= (1 << 0);								//Bit 3 tuong ung voi chan GPIOD
	while((RCGCGPIO & (1 << 0)) == 0);	
	
	GPIOAFSEL_A |= (1 << 6) | (1 << 7);	//Bat chuc nang thay the cho chan PD0 va PD1
	GPIOPCTL_A = (3 << 24) | (3 << 28);		//Dat gia tri byte 0 va byte 1 thanh 3 de chon I2C
	GPIODEN_A |= (1 << 5) | (1 << 6) | (1 << 7);		//Bat tin hieu digital cho chan PD0 va PD1
	GPIODIR_A |= (1 << 6) | (1 << 7);		//Dat chan PD0 va PD1 thanh chan OUTPUT
	GPIODIR_A &= ~(1 << 5);
	GPIOODR_A |= (1 << 7);		//Bat drain control cho chan PD1;
	GPIOPUR_A = (1<< 5);
	
	I2C1_MCR = 0x00;										//Reset gia tri thanh ghi
	I2C1_MCR |= (1 << 4);								//Thiet lap module la Master
	I2C1_MTPR = (7 << 0);								//Dua theo cong thuc tu datasheet tinh ra duoc gia tri chu ki SCL
}

void set_slave_address(u8 slave_address){		//Ham thiet lap dia chi Slave
	I2C1_MSA = (slave_address << 1);					//Dia chi Slave duoc luu tu bit 1 -> bit 7 trong thanh ghi
}

void setRW(u8 mode){												//Ham thiet lap che do Read/Write
	if (mode == 0) I2C1_MSA &= ~(1 << 0);			//Gan gia tri tuong ung vao bit 0 cua thanh ghi 
	else I2C1_MSA |= (1 << 0);								// 0-Transmit va 1-Receive
}

void write_byte(u8 data, u8 state){					//Ham gui mot byte - dau vao la data(du lieu) va state(trang thai)
	I2C1_MDR = data;													//Dua du lieu truyen di vao thanh ghi I2CMDR
	I2C1_MCS = state;													//Dua cac bit trang thai vao thanh ghi I2CMCS (Start - Run - Stop)
	
	while((I2C1_MCS & (1 << 0)) == 0);				//Kiem tra bit Busy cua thanh ghi I2CMCS
	
	if((I2C1_MCS & (1 << 1)) != 0){						//Kiem tra bit loi trong thanh ghi I2CMCS
		if((I2C1_MCS & (1 << 1)) != 0){
			
		}else{
			I2C1_MCS = (1 << 2);
			while((I2C1_MCS & (1 << 0)) != 0);
		}
	}
}


/*D1 D2 D3 D4 BL EN RW RS*/
void I2C_LCD_send(u8 full_data, int rs) {		//Ham gui mot lenh toi I2C LCD
  u8 	upper = full_data & 0xF0,							//Vi I2C LCD truyen du lieu 4bit nen phai chia du lieu thanh 2 de truyen di
			lower = (full_data << 4);							// upper(4bit cao) va lower(4bit thap)
	
	u8 BL, RS, RW, EN, data;									//Khai bao cac bit dieu khien lenh va du lieu truyen di
	BL = 0x08;																//bit bl su dung de bat/tat den nen cua LCD
	RS = 0x00 | rs;														//bit rs su dung de chon du lieu gui di command(rs = 0) hay la data(rs = 1) 
	RW = 0x00;																//bit Read/Write
	EN = 0x04;																//bit en su dung de xac thuc du lieu
	
	data = upper | BL | EN | RW | RS;					//Ket hop tat ca de thanh 1 byte hoan chinh co the truyen di
	
	write_byte(data, 7);											//Truyen byte do di voi bit en = 1
	delay(5);
	write_byte(data & ~EN, 7);								//Sau do truyen di voi bit en = 0
	delay(5);																	// bit en tu 1->0 se co chuc nang giup LCD tiep nhan du lieu
	
	data = lower | BL | EN | RW | RS;					// Tuong tu voi 4bit thap
	
	write_byte(data, 7);
	delay(5);
	write_byte(data & ~EN, 7);
	delay(5);
}


void display_character(u8 ch, int row, int col){
	u8 pos = (row == 1) ? 0x80 : 0xC0;
	pos = pos | (col - 1);
	
	I2C_LCD_send(pos, 0);
	delay(2);
	I2C_LCD_send(ch, 1);
	delay(2);
}

void I2C_LCD_init(void) {					//Ham thiet lap LCD
    delay(50);
    I2C_LCD_send(0x02, 0);		
		delay(50);				
    I2C_LCD_send(0x28, 0);		//Khoi tao LCD o che do 4bit 
		delay(50);
    I2C_LCD_send(0x0C, 0); 		//Bat Man hinh - Tat Con Tro
		delay(50);
    I2C_LCD_send(0x01, 0); 		//Xoa man hinh LCD
    delay(50);
    I2C_LCD_send(0x80, 0); 		//Dua con tro ve vi tri dau tien (hang 1 - cot 1)
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
    if ((GPIODATA_A & 0x20) == 0) key = '+';
    GPIODATA_B |= 0x0F;

    delay(10);

    GPIODATA_B &= ~0x02;
    GPIODATA_B |= 0x0D;
		if ((GPIODATA_B & 0x10) == 0) key = '4';
    if ((GPIODATA_B & 0x20) == 0) key = '5';
    if ((GPIODATA_B & 0x40) == 0) key = '6';
    if ((GPIODATA_A & 0x20) == 0) key = '-';
    GPIODATA_B |= 0x0F;

    delay(10);

    GPIODATA_B &= ~0x04;
    GPIODATA_B |= 0x0B;
		if ((GPIODATA_B & 0x10) == 0) key = '7';
    if ((GPIODATA_B & 0x20) == 0) key = '8';
    if ((GPIODATA_B & 0x40) == 0) key = '9';
    if ((GPIODATA_A & 0x20) == 0) key = 'x';
    GPIODATA_B |= 0x0F;

    delay(10);

    GPIODATA_B &= ~0x08;
    GPIODATA_B |= 0x07;
		if ((GPIODATA_B & 0x10) == 0) key = 'C';
    if ((GPIODATA_B & 0x20) == 0) key = '0';
    if ((GPIODATA_B & 0x40) == 0) key = '=';
    if ((GPIODATA_A & 0x20) == 0) key = '/';
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
		if (colf == 17){
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

int main(void){
    setup_GPIO();
    setup_I2C();
    set_slave_address(0x27);
		setRW(0);
    I2C_LCD_init();
		display_string("Enter passwork: ", 1,1);
		I2C_LCD_send(0xC0, 0);
		int count = 0;
    while(1){
				char key = getkey();
				if (count == 6) {
					int result = check_pass();
					if (result == 0){
						count = 0;
						display_string("xxxxxx", 2,1);
						delay(200);
						display_string("      ", 2,1);
						delay(200);
						display_string("xxxxxx", 2,1);
						delay(200);
						display_string("      ", 2,1);
						delay(200);
						display_string("xxxxxx", 2,1);
						delay(200);
						display_string("      ", 2,1);
						delay(200);
					}else{
						display_string("Correct", 2, 1);
					}
				}
				if (key != '.'){
					if (key == 'C') {
						count = 0;
						display_string("      ", 2,1);
					}else if((int)key > 47 && (int)key < 58){
						check[count++] = key;
						display_character(key, 2, count);
					}else if(key == '+'){
						display_character(' ', 2, count);
						count--;
					}
				}
				
				
        delay(200);
    }
}
