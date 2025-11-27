typedef unsigned char u8;

#define RCGCGPIO   (*((volatile unsigned long *)0x400FE608))

#define GPIODEN_E  (*((volatile unsigned long *)0x4002451C))
#define GPIODIR_E  (*((volatile unsigned long *)0x40024400))
#define GPIODATA_E (*((volatile unsigned long *)0x400243FC))
#define GPIOPUR_E  (*((volatile unsigned long *)0x40024510))

#define GPIODEN_C   (*((volatile unsigned long *) 0x4000651C))	
#define GPIODIR_C   (*((volatile unsigned long *) 0x40006400))	
#define GPIODATA_C  (*((volatile unsigned long *) 0x400063FC))

	
#define RCGCI2C  	(*((volatile unsigned long *)0x400FE620))
#define I2C3_MCR  (*((volatile unsigned long *)0x40023020))		
#define I2C3_MTPR (*((volatile unsigned long *)0x4002300C))		
#define I2C3_MSA  (*((volatile unsigned long *)0x40023000))		
#define I2C3_MDR  (*((volatile unsigned long *)0x40023008))		
#define I2C3_MCS  (*((volatile unsigned long *)0x40023004))	

#define GPIOAFSEL_D (*((volatile unsigned long *)0x40007420))  
#define GPIOPCTL_D  (*((volatile unsigned long *)0x4000752C))  
#define GPIOODR_D   (*((volatile unsigned long *)0x4000750C))  
#define GPIODEN_D   (*((volatile unsigned long *)0x4000751C))
#define GPIODIR_D   (*((volatile unsigned long *)0x40007400))
#define GPIODATA_D  (*((volatile unsigned long *)0x400073FC))
	
// Thanh Ghi Dieu Khien Flash
#define FLASH_FMA        (*((volatile unsigned long *)0x400FD000))
#define FLASH_FMD        (*((volatile unsigned long *)0x400FD004))
#define FLASH_FMC        (*((volatile unsigned long *)0x400FD008))
#define FLASH_FCRIS      (*((volatile unsigned long *)0x400FD00C))
#define FLASH_FCIM       (*((volatile unsigned long *)0x400FD010))
#define FLASH_FCMISC     (*((volatile unsigned long *)0x400FD014))

// Flash FMC bit
#define FLASH_FMC_WRITE      0x00000001   // Write word
#define FLASH_FMC_ERASE      0x00000002   // Page erase
#define FLASH_FMC_MERASE     0x00000004   // Mass erase
#define FLASH_FMC_COMMIT     0x00000008   // Commit write
#define FLASH_FMC_WRKEY      0xA4420000   // Write key

#define FLASH_USER_DATA_ADDR   0x0003F000

//------------------------------------------------------------------------------------------
void delay(int ms);
void setup_GPIO(void);
void setup_I2C(void);

char read_pw[6];

void set_slave_address(u8 slave_address);
void setRW(u8 mode);
void write_byte(u8 data, u8 state);
void I2C_LCD_send(u8 full_data, int rs);
void I2C_LCD_init(void);
void display_character(u8 ch, int row, int col);
void display_string(char* ch, int rowf, int colf);
void display_error(void);

u8 getkey(void);
void clear_check(void);
int check_pass(char fl_pass[6]);
void change_pass(void);
void new_pass(void);

void flash_write(unsigned long address, unsigned char *data, unsigned int length);
void flash_read(unsigned long address, unsigned char *buffer, unsigned int length);
void flash_erase(unsigned long pageAddress);
//------------------------------------------------------------------------------------------


//----------------------HAM DIEU KHIEN FLASH-----------------------------------------------
void flash_read(unsigned long address, unsigned char *buffer, unsigned int length)
{
    unsigned char *ptr = (unsigned char *)address;
    for(unsigned int i = 0; i < length; i++){
        buffer[i] = ptr[i];
    }
}

void flash_erase(unsigned long pageAddress)
{
    FLASH_FMA = pageAddress;
    FLASH_FMC = FLASH_FMC_WRKEY | FLASH_FMC_ERASE;
    while(FLASH_FMC & FLASH_FMC_ERASE);
}

void flash_write(unsigned long address, unsigned char *data, unsigned int length)
{
    unsigned int i = 0;

    while(i < length){
        unsigned long word = 0xFFFFFFFF;

        for(int b = 0; b < 4; b++){
            if(i + b < length)
                ((unsigned char*)&word)[b] = data[i + b];
        }

        FLASH_FMA = address + i;
        FLASH_FMD = word;
        FLASH_FMC = FLASH_FMC_WRKEY | FLASH_FMC_WRITE;

        while(FLASH_FMC & FLASH_FMC_WRITE);
        i += 4;
    }
}
//------------------------------------------------------------------------------------------


// HAM DELAY
void delay(int ms){
	unsigned int i;
	for (i = 0; i < ms*2500; i++);
}


/*
    c1-C4
    c2-C5
    c3-C6
    c4-C7
		r1-E1
    r2-E2
    r3-E3
    r4-E4	
*/ 


//------------------------HAM SETUP CHAN GPIO & I2C-----------------------------------------
void setup_GPIO(void){
	
	RCGCGPIO |= (1 << 2);
	while ((RCGCGPIO & (1 << 2)) == 0); 
	
  GPIODEN_C |= 0xF0;
  GPIODIR_C |= 0xF0;
  
	RCGCGPIO |= (1 << 4);
	while ((RCGCGPIO & (1 << 4)) == 0);
	GPIODEN_E |=  0x1E;
	GPIODIR_E &= ~0x1E;
	GPIOPUR_E |= 0x1E;
}

void setup_I2C(void){									
	RCGCI2C |= (1 << 3);								
	while((RCGCI2C & (1 << 3)) == 0);		
	RCGCGPIO |= (1 << 3);								
	while((RCGCGPIO & (1 << 3)) == 0);	
	
	GPIOAFSEL_D |= (1 << 0) | (1 << 1);	
	GPIOPCTL_D = (3 << 0) | (3 << 4);		
	GPIODEN_D |= (1 << 0) | (1 << 1);		
	GPIODIR_D |= (1 << 0) | (1 << 1);		
	GPIOODR_D |= (1 << 1);
	
	I2C3_MCR = 0x00;										
	I2C3_MCR |= (1 << 4);								
	I2C3_MTPR = (7 << 0);								
}
//------------------------------------------------------------------------------------------

//--------------------------HAM DIEU KHIEN I2C LCD------------------------------------------
void set_slave_address(u8 slave_address){		
	I2C3_MSA = (slave_address << 1);					
}

void setRW(u8 mode){												
	if (mode == 0) I2C3_MSA &= ~(1 << 0);			
	else I2C3_MSA |= (1 << 0);								
}

void write_byte(u8 data, u8 state){					
	I2C3_MDR = data;													
	I2C3_MCS = state;													
	
	while((I2C3_MCS & (1 << 0)) == 0);				
	
	if((I2C3_MCS & (1 << 1)) != 0){						
		if((I2C3_MCS & (1 << 1)) != 0){
			
		}else{
			I2C3_MCS = (1 << 2);
			while((I2C3_MCS & (1 << 0)) != 0);
		}
	}
}


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

void display_error(void){
	display_string("xxxxxx", 1,5);
	delay(200);
	display_string("      ", 1,5);
	delay(200);
	display_string("xxxxxx", 1,5);
	delay(200);
	display_string("      ", 1,5);
	delay(200);
	display_string("xxxxxx", 1,5);
	delay(200);
	display_string("      ", 1,5);
	delay(200);
	display_string("******", 1,5);
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
//------------------------------------------------------------------------------------------



//----------------------------HAM CHUC NANG CHUONG TRINH-----------------------------------
void set_row(unsigned char mask);
void set_row(unsigned char mask) {
    GPIODATA_C |= 0xF0;     
    GPIODATA_C &= ~mask;    
}


u8 getkey(void){
	u8 key = '.';
		
		set_row(0x10);
		if ((GPIODATA_E & 0x02) == 0) key = '1';
    if ((GPIODATA_E & 0x04) == 0) key = '2';
    if ((GPIODATA_E & 0x08) == 0) key = '3';
    if ((GPIODATA_E & 0x10) == 0) key = 'A';
    
    delay(10);

		set_row(0x20);
		if ((GPIODATA_E & 0x02) == 0) key = '4';
    if ((GPIODATA_E & 0x04) == 0) key = '5';
    if ((GPIODATA_E & 0x08) == 0) key = '6';
    if ((GPIODATA_E & 0x10) == 0) key = 'B';
    
    delay(10);

		set_row(0x40);
		if ((GPIODATA_E & 0x02) == 0) key = '7';
    if ((GPIODATA_E & 0x04) == 0) key = '8';
    if ((GPIODATA_E & 0x08) == 0) key = '9';
    if ((GPIODATA_E & 0x10) == 0) key = 'C';
    
    delay(10);

		set_row(0x80);
		if ((GPIODATA_E & 0x02) == 0) key = '*';
    if ((GPIODATA_E & 0x04) == 0) key = '0';
    if ((GPIODATA_E & 0x08) == 0) key = '#';
    if ((GPIODATA_E & 0x10) == 0) key = 'D';

    delay(10);
	return key;
}

char check[6];

void clear_check(void){
	unsigned int i;
	for (i = 0; i< 6; i++) check[i] = ' ';
}

int check_pass(char fl_pass[6]){
	unsigned int i;
	for (i = 0; i< 6; i++) if (read_pw[i] != fl_pass[i]) return 0;
	return 1;
}

void new_pass(void){
	display_string("|---NEW-PASS---|", 0, 0);
	display_string("<|******|>", 1, 3);
	int count = 6;
	char new_pass[6];
	while(1){
				char key = getkey();
				if (count == 12) {
					flash_erase(FLASH_USER_DATA_ADDR);
					flash_write(FLASH_USER_DATA_ADDR, (unsigned char*)new_pass, 6);
					break;
				}
				if (key != '.'){
					if (key == '*') {
						count = 6;
						display_string("******", 1,5);
					}else if((int)key > 47 && (int)key < 58){
						new_pass[count - 6] = key;
						count++;
						display_character(key, 1, count - 2);
					}else if(key == 'A'){
						if (count > 6) {
							display_character('*', 1, count - 2);
							count--;
						}
					}
				}
        delay(100);
    }
}

void change_pass(void){
	display_string("|---OLD-PASS---|", 0, 0);
	display_string("<|******|>", 1, 3);
	int count_error = 0;
	int count = 6;
	while(1){
				char key = getkey();
				if (count == 12) {
					flash_read(FLASH_USER_DATA_ADDR, (unsigned char*)read_pw, 6);
					int result = check_pass(check);
					if (result == 0){
						count = 0;
						display_error();
						count = 6;
					}else{
						display_string("<Correct.>", 1, 3 );
						I2C_LCD_send(0x01, 0);
						new_pass();
						break;
					}
				}
				if (key != '.'){
					if (key == '*') {
						count = 6;
						display_string("******", 1,5);
					}else if((int)key > 47 && (int)key < 58){
						check[count - 6] = key;
						count++;
						display_character(key, 1, count - 2);
					}else if(key == 'A'){
						if (count > 6) {
							display_character('*', 1, count - 2);
							count--;
						}
					}
				}
        delay(100);
    }
}
//------------------------------------------------------------------------------------------

int main(void){
    setup_GPIO();
    setup_I2C();
    set_slave_address(0x27);
		setRW(0);
    I2C_LCD_init();
		char num = '.';
		display_string("|-----DOOR-----|", 0, 0);
		display_string("<|******|>", 1, 3);
		int count = 6;
		while(1){
				char key = getkey();
				if (count == 12) {
					flash_read(FLASH_USER_DATA_ADDR, (unsigned char*)read_pw, 6);
					
					int result = check_pass(check);
					if (result == 0){
						count = 0;
						display_error();
						count = 6;
					}else{
						display_string("<Correct.>", 1, 3 );
						GPIODATA_C |= (1 << 5);
						delay(4000);
						GPIODATA_C &= ~(1 << 5);
						I2C_LCD_send(0x01, 0);
						display_string("|-----DOOR-----|", 0, 0);
						display_string("<|******|>", 1, 3);
						count = 6;
					}
				}
				if (key != '.'){
					if (key == '*') {
						count = 6;
						display_string("******", 1,5);
					}else if((int)key > 47 && (int)key < 58){
						check[count - 6] = key;
						count++;
						display_character(key, 1, count - 2);
					}else if(key == 'A'){
						if (count > 6) {
							display_character('*', 1, count - 2);
							count--;
						}
					}else if(key == 'B'){
						change_pass();
						display_string("|-----DOOR-----|", 0, 0);
						display_string("<|******|>", 1, 3);
						count = 6;
					}
				}
			delay(100);
		}
}



