
#include "MKL25Z4.h"

void cekej (uint32_t delay_ms);

void Pin_Init();

//4094 FUNKCE SR
void SR_STR();
void SR_CLK();
void SR_L();
void SR_H();
void SR_Write_Data(uint8_t data);

//GDM LOW LEVEL
void GLCD_Enable_H();
void GLCD_Enable_L();
void GLCD_RS_L();
void GLCD_RS_H();
void GLCD_EnableControler(uint8_t control);
void GLCD_DisableControler(uint8_t control);
void GLCD_Init();
void GLCD_WrCmd(uint8_t cmd, uint8_t control);
void GLCD_WrDat(uint8_t dat);


void GLCD_GoTo(uint8_t col, uint8_t row);
void GLCD_SetPixel(uint8_t x, uint8_t y, uint8_t bit);



void GLCD_FillScreen();
void GLCD_ClearScreen();
void GLCD_FillScreen_Ddram();
void GLCD_ClearScreen_Ddram();


void Primka(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t barva);
void Obdelnik(uint8_t x, uint8_t y, uint8_t a, uint8_t b, uint8_t barva);
void Fill_Obdelnik(uint8_t x, uint8_t y, uint8_t a, uint8_t b, uint8_t barva);
void V_Linka(uint8_t x1, uint8_t y1, uint8_t y2, uint8_t barva);
void H_Linka(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t barva);
void Kruh(uint8_t x, uint8_t y, uint8_t r, uint8_t barva);
void Fill_Kruh(uint8_t x, uint8_t y, uint8_t r, uint8_t barva); //brute force verze, je treba vylepsit
void BitMap_Print(int *bitmap, int delka);
void Start_screen();


//ddram
void DDRAM_Init();
void DDRAM_Fill();
uint8_t DDRAM_Read();
void DDRAM_Wr(uint8_t y, uint8_t x, uint8_t bit);


//ZNAKOVA SADA
void GLCD_WrChar(char charToWrite, uint8_t barva);
void GLCD_WrString(char* str, uint8_t barva);
unsigned char GLCD_ReadByteFromROMMemory(char * ptr);



