
#include "MKL25Z4.h"
#include <header.h>


#define RS 		(1<<11)
#define E 		(1<<10)
#define CS1 	(1<<8)
#define CS2 	(1<<9)
#define RST 	(1<<12) 		//PTA
#define CLK 	(1<<17)
#define DAT 	(1<<31)			//PTE
#define STR 	(1<<12)


#define delayEnable 15
#define delay 10


#define SET_X 		0x40
#define SET_Y 		0xB8
#define START_LINE 	0xc0
#define DIS_ON 		0x3f
#define DIS_WIDTH  	0x80
#define DIS_HEIGH  	0x40



uint8_t DDRAM[8][128];
uint8_t curX, curY;


void DDRAM_Init(){
	memset(DDRAM,0,sizeof(DDRAM));
}

void DDRAM_Fill(){
	memset(DDRAM,0xff,sizeof(DDRAM));
}

//funkce pro zapis pixelu do pole, tuto funkci upravit, pridat parametr pro 0,1
void DDRAM_Wr(uint8_t x, uint8_t y, uint8_t bit){

	switch(bit){
		case 1: DDRAM[x/8][y] |= (1 << x % 8);
		break;

		case 0: DDRAM[x/8][y] &= ~(1 << x % 8);
		break;

		default:
		break;
	}
}


uint8_t DDRAM_Read(uint8_t bit){

	uint8_t temp= (bit) ? 0 : 255;
	uint8_t krok=curY * 8;

	for(uint8_t maska= krok; maska < krok + 8; maska++){

		switch(bit){

		case 1:
			if (DDRAM[curY][curX] & (1 << maska % 8))
				temp |= (1 << maska % 8);
		break;

		case 0:
			if (!(DDRAM[curY][curX] & (1 << maska % 8)))
				temp &= ~(1 << maska % 8);
		break;

		default:
		break;
		}
	}
	return temp;

}

//incializace a zapnuti displeje
void GLCD_Init(){

	GLCD_WrCmd(DIS_ON, 0);
	GLCD_WrCmd(SET_X, 0);
	GLCD_WrCmd(SET_Y, 0);
	GLCD_WrCmd(START_LINE, 0);
	//GLCD_WrCmd(DIS_ON, 0);

	GLCD_WrCmd(DIS_ON, 1);
	GLCD_WrCmd(SET_X, 1);
	GLCD_WrCmd(SET_Y, 1);
	GLCD_WrCmd(START_LINE, 1);
	//GLCD_WrCmd(DIS_ON, 1);

	DDRAM_Init();
	curX=curY=0;
}


//funkce pro mazani obrazovky
void GLCD_ClearScreen(uint8_t sirka){

	uint8_t i,j;

	for(i=0;i<64;i+=8){

		GLCD_GoTo(0, i);

		for(j=0;j < sirka;j++){

			GLCD_WrDat(0x00);
		}
	}
}

void GLCD_FillScreen(){

	uint8_t i,j;

	for(i=0;i<64;i+=8){

		GLCD_GoTo(0, i);

		for(j=0;j<128;j++){

			GLCD_WrDat(0xff);
		}
	}
}

void GLCD_FillScreen_Ddram(){
	GLCD_FillScreen();
	DDRAM_Fill();
}

void GLCD_ClearScreen_Ddram(){
	GLCD_ClearScreen(128);
	DDRAM_Init();
}

//funkce pro zapis pozadovane polohy do radicu
void GLCD_GoTo(uint8_t col, uint8_t row){

	curX=col;
	curY=row / 8;

	//cyklus init radicu
	for(int i=0;i<2;i++){

		GLCD_WrCmd(SET_X | 0, i);
		GLCD_WrCmd(SET_Y | curY, i);
		GLCD_WrCmd(START_LINE | 0, i);
	}

	/*tady probiha zapis pozadovane polohy do radkoveho a strankove radice
	 * vyber radice je podle podilu x/64 je to bud 0,1
	 *
	 *
	 */
	GLCD_WrCmd(SET_X | (curX % 64), (curX / 64 ));
	GLCD_WrCmd(SET_Y | curY, (curX / 64 ));

}

//KRESLICI FUNKCE

void GLCD_SetPixel(uint8_t x, uint8_t y, uint8_t bit){

	uint8_t temp=0;
	DDRAM_Wr(y, x, bit);
	GLCD_GoTo(x, y);
	temp=DDRAM_Read(bit);
	GLCD_WrDat(temp);

}

void GLCD_ClearPixel(uint8_t x, uint8_t y, uint8_t bit){

	uint8_t temp=0;
	DDRAM_Wr(y,x,bit);
	GLCD_GoTo(x, y);
	temp=DDRAM_Read(bit);
	//cekej(20);
	GLCD_WrDat(temp);
}

//vykresleni primky odvozeno od Bresenhamova algoritmu
void Draw_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t barva){

	int dx, dy, idx, idy, px, py, x, y;

	dx=x2-x1;
	dy=y2-y1;

	idx=(dx < 0) ? -1 : 1;
	idy=(dy < 0) ? -1 : 1;

	dx= idx*dx + 1;
	dy= idy*dy + 1;

	px=x1;
	py=y1;
	x=y=0;

	if(dx >= dy){

		for(uint8_t x =0; x<dx; x++ ){
			GLCD_SetPixel(px, py, barva);
			y+=dy;
			if(y >= dx){
				y-=dx;
				py+=idy;
			}
			px+=idx;
		}
	}else {

		for(uint8_t y =0; y<dy; y++ ){
			GLCD_SetPixel(px, py, barva);
			x+=dx;
			if(x >= dy){
				x-=dy;
				px+=idx;
			}
			py+=idy;
		}
	}
}


void H_line(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t barva){

	for(uint8_t j=x1; j<=x2; j++){
		GLCD_SetPixel(j, y1, barva);
		//cekej(1);
	}
}

void V_line(uint8_t x1, uint8_t y1, uint8_t y2, uint8_t barva){

	for(uint8_t j=y1; j<=y2; j++){
		GLCD_SetPixel(x1, j, barva);
		//cekej(1);
	}
}

//vykresleni obdelniku/ctverce
void Draw_Obdelnik(uint8_t x, uint8_t y, uint8_t a, uint8_t b, uint8_t barva){

	uint8_t j;
	for (j = 0; j < b; j++) {
		GLCD_SetPixel(x, y + j, barva);
		GLCD_SetPixel(x + a - 1, y + j, barva);
	}

	for (j = 0; j < a; j++)	{
		GLCD_SetPixel(x + j, y, barva);
		GLCD_SetPixel(x + j, y + b - 1, barva);
	}

}


//vykresleni celeho obdelniku/ctverce
void Fill_Obdelnik(uint8_t x, uint8_t y, uint8_t a, uint8_t b, uint8_t barva){

	for(uint8_t j=0;j<=a;j++)
		V_line(x+j, y, y+b, barva);
}


//funkce pro vykresleni kruhu odvozeno od Bresenhamova algoritmu
void Draw_Circle(uint8_t x, uint8_t y, uint8_t r, uint8_t barva){

	int xx, yy, xc, yc, re;
	xx=r;
	yy=0;
	xc= 1 - (r << 1);
	yc=1;
	re=0;

	while (xx >= yy) {

		GLCD_SetPixel(x + xx, y + yy, barva);
		GLCD_SetPixel(x - xx, y + yy, barva);
		GLCD_SetPixel(x - xx, y - yy, barva);
		GLCD_SetPixel(x + xx, y - yy, barva);
		GLCD_SetPixel(x + yy, y + xx, barva);
		GLCD_SetPixel(x - yy, y + xx, barva);
		GLCD_SetPixel(x - yy, y - xx, barva);
		GLCD_SetPixel(x + yy, y - xx, barva);

	    yy++;
	    re += yc;
	    yc += 2;

	    if ((re << 1) + xc > 0) {
	        xx--;
	        re += xc;
	        xc += 2;
	    }
	}
}


//vykresleni plneho kruhu
void Fill_Circle(uint8_t x1, uint8_t y1, uint8_t r, uint8_t barva){
	for(int y=-r; y<=r; y++){
		for(int x=-r; x<=r; x++){
			if(x*x + y*y <=r*r) GLCD_SetPixel(x1+x, y1+y, barva);
		}
	}

}


//funkce pro zapis prikazu do radice v parametru control
void GLCD_WrCmd(uint8_t cmd, uint8_t control){

	//cekej(1);
	SR_Write_Data(cmd);
	//cekej(10);
	GLCD_RS_L();
	GLCD_EnableControler(control);
	GLCD_Enable_H();
	cekej(delayEnable);
	GLCD_Enable_L();
	GLCD_DisableControler(control);

}


//funkce pro zapis dat do radice na zaklade aktualniho kurzoru
void GLCD_WrDat(uint8_t dat){

	//cekej(1);
	SR_Write_Data(dat);
	//cekej(10);
	GLCD_RS_H();
	GLCD_EnableControler(curX / 64);
	GLCD_Enable_H();
	cekej(delayEnable);
	GLCD_Enable_L();
	GLCD_DisableControler(curX / 64);
	if(++curX >= DIS_WIDTH) curX=0;
}


//funkce pro vyber a odpojeni radicu
void GLCD_EnableControler(uint8_t control){
	switch(control){
	case 0: PTC->PSOR = CS1;
	break;
	case 1: PTC->PSOR = CS2;
	break;
	}
}
void GLCD_DisableControler(uint8_t control){
	switch(control){
	case 0: PTC->PCOR = CS1;
	break;
	case 1: PTC->PCOR = CS2;
	break;
	}
}

uint8_t GLCD_Active_Controller(){
	return (curX / 64);
}


// DATA SIGNAL
void GLCD_RS_H(){
	PTC->PSOR = RS;
}

//INS SIGNAL
void GLCD_RS_L(){
	PTC->PCOR = RS;
}

//ENABLE SIGNAL H
void GLCD_Enable_H(){
	PTC->PSOR = E;
}

//ENABLE SIGNAL L
void GLCD_Enable_L(){
	PTC->PCOR = E;
}

//INICIALIZACE PINU
void Pin_Init(){

	SIM->SCGC5 |= 1<<11;
	SIM->SCGC5 |= 1<<9;					//port A
	SIM->SCGC5 |= 1<<13;


	PORTC->PCR[8]= 0x100;
	PORTC->PCR[9]= 0x100;
	PORTC->PCR[10]= 0x100;
	PORTC->PCR[11]= 0x100;
	PORTC->PCR[12]= 0x100;
	PORTC->PCR[17]= 0x100;

	PORTA->PCR[12]= 0x100;
	PORTE->PCR[31]= 0x100;

	PTC->PDDR |= RS;
	PTC->PCOR = RS;

	PTC->PDDR |= E;
	PTC->PCOR = E;			//mozna SOR nwm

	PTC->PDDR |= CS1;
	PTC->PCOR = CS1;

	PTC->PDDR |= CS2;
	PTC->PCOR = CS2;

	PTC->PDDR |= STR;
	PTC->PCOR = STR;

	PTC->PDDR |= CLK;
	PTC->PCOR = CLK;

	PTA->PDDR |= RST;
	//PTA->PCOR = RST;    //vypnuti resetu
	PTA->PSOR = RST;    //vypnuti resetu

	PTE->PDDR |= DAT;
	PTE->PCOR = DAT;

	//cekej(10);

}

//cekani za pomoci instrukce v asm nop...nedelat nic
void cekej(uint32_t delay_ms){
static volatile uint32_t i;
	for(i = 0; i < delay_ms; i++)
		__asm("nop");

}


/*
 * 4094 FUNKCE
 * POSLI DATA
 * HODINY
 * STROBO
 * ZAPIS H
 * ZAPIS L
 */
void SR_Write_Data(uint8_t data){

	for(int i=0;i<8;i++){

		if((data << i) & 0b10000000){

			SR_H();
		}
		else{

			SR_L();
		}

		SR_CLK();
	}
	SR_STR();
}

void SR_CLK(){

	PTC->PCOR =CLK;
	PTC->PSOR =CLK;
	cekej(10);
	PTC->PCOR =CLK;
	cekej(10);
}

void SR_STR(){

	PTC->PCOR =STR;
	PTC->PSOR =STR;
	cekej(10);        //20
	PTC->PCOR =STR;
}

void SR_L(){
	PTE->PCOR = DAT;
}

void SR_H(){
	PTE->PSOR = DAT;
}




//ZNAKOVA SADA A FUNKCE

const unsigned char font []= {
		0x00, 0x00, 0x00, 0x00, 0x00,// (mezera)
		0x00, 0x00, 0x5F, 0x00, 0x00,// !
		0x00, 0x07, 0x00, 0x07, 0x00,// "
		0x14, 0x7F, 0x14, 0x7F, 0x14,// #
		0x24, 0x2A, 0x7F, 0x2A, 0x12,// $
		0x23, 0x13, 0x08, 0x64, 0x62,// %
		0x36, 0x49, 0x55, 0x22, 0x50,// &
		0x00, 0x05, 0x03, 0x00, 0x00,// '
		0x00, 0x1C, 0x22, 0x41, 0x00,// (
		0x00, 0x41, 0x22, 0x1C, 0x00,// )
		0x08, 0x2A, 0x1C, 0x2A, 0x08,// *
		0x08, 0x08, 0x3E, 0x08, 0x08,// +
		0x00, 0x50, 0x30, 0x00, 0x00,// ,
		0x08, 0x08, 0x08, 0x08, 0x08,// -
		0x00, 0x30, 0x30, 0x00, 0x00,// .
		0x20, 0x10, 0x08, 0x04, 0x02,// /
		0x3E, 0x51, 0x49, 0x45, 0x3E,// 0
		0x00, 0x42, 0x7F, 0x40, 0x00,// 1
		0x42, 0x61, 0x51, 0x49, 0x46,// 2
		0x21, 0x41, 0x45, 0x4B, 0x31,// 3
		0x18, 0x14, 0x12, 0x7F, 0x10,// 4
		0x27, 0x45, 0x45, 0x45, 0x39,// 5
		0x3C, 0x4A, 0x49, 0x49, 0x30,// 6
		0x01, 0x71, 0x09, 0x05, 0x03,// 7
		0x36, 0x49, 0x49, 0x49, 0x36,// 8
		0x06, 0x49, 0x49, 0x29, 0x1E,// 9
		0x00, 0x36, 0x36, 0x00, 0x00,// :
		0x00, 0x56, 0x36, 0x00, 0x00,// ;
		0x00, 0x08, 0x14, 0x22, 0x41,// <
		0x14, 0x14, 0x14, 0x14, 0x14,// =
		0x41, 0x22, 0x14, 0x08, 0x00,// >
		0x02, 0x01, 0x51, 0x09, 0x06,// ?
		0x32, 0x49, 0x79, 0x41, 0x3E,// @
		0x7E, 0x11, 0x11, 0x11, 0x7E,// A
		0x7F, 0x49, 0x49, 0x49, 0x36,// B
		0x3E, 0x41, 0x41, 0x41, 0x22,// C
		0x7F, 0x41, 0x41, 0x22, 0x1C,// D
		0x7F, 0x49, 0x49, 0x49, 0x41,// E
		0x7F, 0x09, 0x09, 0x01, 0x01,// F
		0x3E, 0x41, 0x41, 0x51, 0x32,// G
		0x7F, 0x08, 0x08, 0x08, 0x7F,// H
		0x00, 0x41, 0x7F, 0x41, 0x00,// I
		0x20, 0x40, 0x41, 0x3F, 0x01,// J
		0x7F, 0x08, 0x14, 0x22, 0x41,// K
		0x7F, 0x40, 0x40, 0x40, 0x40,// L
		0x7F, 0x02, 0x04, 0x02, 0x7F,// M
		0x7F, 0x04, 0x08, 0x10, 0x7F,// N
		0x3E, 0x41, 0x41, 0x41, 0x3E,// O
		0x7F, 0x09, 0x09, 0x09, 0x06,// P
		0x3E, 0x41, 0x51, 0x21, 0x5E,// Q
		0x7F, 0x09, 0x19, 0x29, 0x46,// R
		0x46, 0x49, 0x49, 0x49, 0x31,// S
		0x01, 0x01, 0x7F, 0x01, 0x01,// T
		0x3F, 0x40, 0x40, 0x40, 0x3F,// U
		0x1F, 0x20, 0x40, 0x20, 0x1F,// V
		0x7F, 0x20, 0x18, 0x20, 0x7F,// W
		0x63, 0x14, 0x08, 0x14, 0x63,// X
		0x03, 0x04, 0x78, 0x04, 0x03,// Y
		0x61, 0x51, 0x49, 0x45, 0x43,// Z
		0x00, 0x00, 0x7F, 0x41, 0x41,// [
		0x02, 0x04, 0x08, 0x10, 0x20,// "\"
		0x41, 0x41, 0x7F, 0x00, 0x00,// ]
		0x04, 0x02, 0x01, 0x02, 0x04,// ^
		0x40, 0x40, 0x40, 0x40, 0x40,// _
		0x00, 0x01, 0x02, 0x04, 0x00,// `
		0x20, 0x54, 0x54, 0x54, 0x78,// a
		0x7F, 0x48, 0x44, 0x44, 0x38,// b
		0x38, 0x44, 0x44, 0x44, 0x20,// c
		0x38, 0x44, 0x44, 0x48, 0x7F,// d
		0x38, 0x54, 0x54, 0x54, 0x18,// e
		0x08, 0x7E, 0x09, 0x01, 0x02,// f
		0x08, 0x14, 0x54, 0x54, 0x3C,// g
		0x7F, 0x08, 0x04, 0x04, 0x78,// h
		0x00, 0x44, 0x7D, 0x40, 0x00,// i
		0x20, 0x40, 0x44, 0x3D, 0x00,// j
		0x00, 0x7F, 0x10, 0x28, 0x44,// k
		0x00, 0x41, 0x7F, 0x40, 0x00,// l
		0x7C, 0x04, 0x18, 0x04, 0x78,// m
		0x7C, 0x08, 0x04, 0x04, 0x78,// n
		0x38, 0x44, 0x44, 0x44, 0x38,// o
		0x7C, 0x14, 0x14, 0x14, 0x08,// p
		0x08, 0x14, 0x14, 0x18, 0x7C,// q
		0x7C, 0x08, 0x04, 0x04, 0x08,// r
		0x48, 0x54, 0x54, 0x54, 0x20,// s
		0x04, 0x3F, 0x44, 0x40, 0x20,// t
		0x3C, 0x40, 0x40, 0x20, 0x7C,// u
		0x1C, 0x20, 0x40, 0x20, 0x1C,// v
		0x3C, 0x40, 0x30, 0x40, 0x3C,// w
		0x44, 0x28, 0x10, 0x28, 0x44,// x
		0x0C, 0x50, 0x50, 0x50, 0x3C,// y
		0x44, 0x64, 0x54, 0x4C, 0x44,// z
		0x00, 0x08, 0x36, 0x41, 0x00,// {
		0x00, 0x00, 0x7F, 0x00, 0x00,// |
		0x00, 0x41, 0x36, 0x08, 0x00,// }
		0x08, 0x08, 0x2A, 0x1C, 0x08,// ->
		0x08, 0x1C, 0x2A, 0x08, 0x08 // <-
};




/*Funkce pro praci se znaky, znakova sada neni kompletni, jsou zde jen zakladni pismena, cisla a znaky
 *	zacina od hodnoty 32 v ascii tabulce, je tady 96 znaku namapovanych
*/


unsigned char GLCD_ReadByteFromROMMemory(char * ptr){

	return *(ptr);
}


/*funkci WrChar() predam znak co chci zobrazit, dal ho predam funkci ReadByte..(), ktera ocekava
 * adresu znaku, delam to tak ze adresu pole font[] pretypuju na int, prictu 5nasobek cisla znaku, to je skrz to ze
 * jeden znak odpovida peti hodnotam v poli font[] a iteruju, pak se ta hodnota pretypuje na typ ukazatele na char a predam to
 * funkci ReadByte..(), ktera vrati polozku v poli font[] a to dam jako parametr funkci WrDat()
 *
 */
void GLCD_WrChar(char charToWrite, uint8_t barva){

	int i;
	charToWrite -= 32;

	for(i = 0; i < 5; i++){
		if(barva) GLCD_WrDat(GLCD_ReadByteFromROMMemory((char *)((int)font + (5 * charToWrite) + i)));
		else GLCD_WrDat(~GLCD_ReadByteFromROMMemory((char *)((int)font + (5 * charToWrite) + i)));
	}
	if (barva)GLCD_WrDat(0x00);
	else GLCD_WrDat(0xff);
}

void GLCD_WrString(char *str, uint8_t barva){

	while(*str){
		GLCD_WrChar(*str++, barva);

	}
}







