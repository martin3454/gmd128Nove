
#include "MKL25Z4.h"

void cekej (uint32_t delay_ms);

void Pin_Init();


//4094 FUNKCE SR
void SR_STR();
void SR_CLK();
void SR_L();
void SR_H();
void SR_Write_Data(uint8_t data);

//GMD LOW LEVEL
void GLCD_Enable_H();
void GLCD_Enable_L();
void GLCD_RS_L();
void GLCD_RS_H();
void GLCD_EnableControler(uint8_t control);
void GLCD_DisableControler(uint8_t control);
uint8_t GLCD_Active_Controller();
void GLCD_Init();
void GLCD_WrCmd(uint8_t cmd, uint8_t control);
void GLCD_WrDat(uint8_t dat);



void GLCD_GoTo(uint8_t x, uint8_t y);
void GLCD_SetPixel(uint8_t x, uint8_t y, uint8_t bit);
void GLCD_ClearPixel(uint8_t x, uint8_t y, uint8_t bit);




void GLCD_FillScreen();
void GLCD_ClearScreen();
void GLCD_FillScreen_Ddram();
void GLCD_ClearScreen_Ddram();





void Draw_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t barva);
void Draw_Obdelnik(uint8_t x, uint8_t y, uint8_t a, uint8_t b, uint8_t barva);
void Fill_Obdelnik(uint8_t x, uint8_t y, uint8_t a, uint8_t b, uint8_t barva);
void V_line(uint8_t x1, uint8_t y1, uint8_t y2, uint8_t barva);
void H_line(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t barva);
void Draw_Circle(uint8_t x, uint8_t y, uint8_t r, uint8_t barva);
void Fill_Circle(uint8_t x, uint8_t y, uint8_t r, uint8_t barva); //brute force verze, je treba vylepsit


//ddram
void DDRAM_Init();
void DDRAM_Fill();
uint8_t DDRAM_Read();
void DDRAM_Wr(uint8_t y, uint8_t x, uint8_t bit);


//ZNAKOVA SADA
void GLCD_WrChar(char charToWrite);
void GLCD_WrString(char* str);
unsigned char GLCD_ReadByteFromROMMemory(char * ptr);





