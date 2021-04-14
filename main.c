/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <header.h>


#define M_LED_R (1<<8)

#define M_SW1  (1<<4)
#define M_SW2  (1<<5)
#define M_SW3  (1<<16)
#define M_SW4  (1<<17)

#define NUM_ROWS 3     //ROZMERY POLE PRO POZICE POSTAV
#define NUM_COLS 5
#define MAX_WIDTH 95	//SIRKA HERNI PLOCHY


#define RYCHLOST 6
#define postavaDx 11			//SIRKA NEPRATEL
#define postavaDy 7				//VYSKA NEPRATEL
#define INVADERS_DROP 4			//SKOK DOLU
//#define SPACE_BETWEEN_ALIENS 4
//#define SPACE_BETWEEN_ROWS 1


#define ACTIVE 0				//OBJEKT SE BUDE ZOBRAZOVAT
#define ZNICEN 2				//OBJEKT SE NEBUDE ZOBRAZOVAT

#define TANK_WIDTH 13
#define TANK_HEIGH 8
#define TANK_STEPX 2
#define TANK_STARTXPOS 63
#define TANK_STARTYPOS 60

#define RAKETA_HEIGH 4
#define RAKETA_WIDTH 1
#define RAKETA_RYCHLOST 4



static int i = 0;

typedef struct gameObject{
	int x;
	int y;
	uint8_t status;
}GameObject;


typedef struct alienObject{
	GameObject ord;
}AlienObject;


typedef struct tankObject{
	GameObject ord;
}TankObject;


													//HERNI OBJEKTY
AlienObject aliens[NUM_ROWS][NUM_COLS];
TankObject hrac;
GameObject raketa;
uint8_t score1,score2,score3;



uint8_t chobot_bitMap[]={

		0b01110000,
		0b00011000,
		0b01111100,
		0b10110110,
		0b00111100,
		0b00111100,
		0b00111100,
		0b10110110,
		0b01111100,
		0b00011000,
		0b01110000
	};

uint8_t chobot1_bitMap[]={

		0b00001110,
		0b10010000,
		0b01111100,
		0b00110110,
		0b00111100,
		0b00111100,
		0b00111100,
		0b00110110,
		0b01111100,
		0b10010000,
		0b00001110

	};

uint8_t meduza_bitMap[]={

		0b10000000,
		0b10110000,
		0b01001000,
		0b11111110,
		0b01111110,
		0b11011110,
		0b01111110,
		0b11111110,
		0b01001000,
		0b10110000,
		0b10000000
};

uint8_t meduza1_bitMap[]={

		0b01000000,
		0b10000000,
		0b01111000,
		0b11111110,
		0b01111110,
		0b11011110,
		0b01111110,
		0b11111110,
		0b01111000,
		0b10000000,
		0b01000000
};


uint8_t krakatice_bitMap[]={

		0b10000000,
		0b10000000,
		0b11011100,
		0b00110100,
		0b11111110,
		0b00111010,
		0b11111110,
		0b00110100,
		0b11011100,
		0b10000000,
		0b10000000
};


uint8_t krakatice1_bitMap[]={

		0b10000000,
		0b10000000,
		0b11011100,
		0b00110100,
		0b11101110,
		0b00101010,
		0b11101110,
		0b00110100,
		0b11011100,
		0b10000000,
		0b10000000
};


uint8_t tank_bitMap[]={

		0b11100000,
		0b11100000,
		0b11100000,
		0b11100000,
		0b11110000,
		0b11111000,
		0b11111111,
		0b11111000,
		0b11110000,
		0b11100000,
		0b11100000,
		0b11100000,
		0b11100000
};

uint8_t raketa_bitMap[]={

		0b11110000
};









void Enemaci_init();
void Tank_init();
void Displej_zobrazit();


////////////////////////////////vykreslovani
void Tank_vykreslit();
void Raketa_vykreslit();

void Chobot_vykreslit();
void Chobot1_vykreslit();
void Meduza_vykreslit();
void Meduza1_vykreslit();
void Krakatice_vykreslit();
void Krakatice1_vykreslit();

////////////////////////////pozice
int RightMosPos();
int LeftMosPos();

//////////////////////////////ridici logika
void Enemaci_control();
void Tank_control();
void Raketa_control();
void Kolize_control();
void Kolize_postava_raketa();
_Bool Kolize(uint8_t alienX, uint8_t alienY);

/////////////////////////////////////////obrazovky
void Konec_hry(int poloha);
void Vyhra();
void Start_screen();
void Game_stats();


uint8_t pozice_chobotnicX[NUM_COLS]={4, 19, 34, 49, 64};		//79, 94, 109 POZICE POSTAV
uint8_t pozice_chobotnicY[NUM_ROWS]={0,9,17};

int pohybX=2;
int EnemaciCounter = RYCHLOST;
_Bool animace = 0;
_Bool GAME_ON = 1;


int main(void)
{

	Pin_Init();
	GLCD_Init();
	//GLCD_ClearScreen(128);
	//GLCD_FillScreen_Ddram();
	GLCD_FillScreen();
	//Draw_Line(0,0,50,50,0);
	GLCD_SetPixel(10,10,0);







	/*Enemaci_init();
	Tank_init();
	Game_stats();

	while(GAME_ON){

		Displej_zobrazit();
		Enemaci_control();
		Raketa_control();
		Kolize_control();
		Tank_control();
		GLCD_ClearScreen(99);
	}*/


    return 0;
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////


void Vyhra(){

	if(score1 + score2 + score3 == 15){
		GAME_ON=0;
		GLCD_ClearScreen(128);
		GLCD_GoTo(35,32);
		GLCD_WrString("VYHRAL JSI",1);
	}
}

void Game_stats(){

	//od 95 asi, vyska postav 7, sirka 11

	uint8_t i,j;

	Vyhra();

	for(i=0;i<64;i+=8){
		GLCD_GoTo(100, i);
		for(j=0;j < 28;j++){
			GLCD_WrDat(0x00);
		}
	}


	//Draw_Obdelnik(100,0,27,63,1);

	GLCD_GoTo(102,15);
	Chobot_vykreslit();
	GLCD_GoTo(114,15);
	GLCD_WrString("=", 1);
	GLCD_GoTo(120,15);
	GLCD_WrChar((char)score1+'0', 1);

	GLCD_GoTo(102,25);
	Meduza_vykreslit();
	GLCD_GoTo(114,25);
	GLCD_WrString("=", 1);
	GLCD_GoTo(120,25);
	GLCD_WrChar((char)score2+'0', 1);

	GLCD_GoTo(102,40);
	Krakatice_vykreslit();
	GLCD_GoTo(114,40);
	GLCD_WrString("=", 1);
	GLCD_GoTo(120,40);
	GLCD_WrChar((char)score3+'0', 1);

}


void Start_screen(){

	Fill_Obdelnik(5,1,10,5, 1);
	Fill_Obdelnik(5,5,5,10, 1);
	Fill_Obdelnik(5,14,5,10, 1);
	Fill_Obdelnik(5,23,10,5, 1);

	GLCD_GoTo(18, 22);
	GLCD_WrString("ORTEX-", 1);
	V_line(55,1,28, 1);
	Draw_Line(55, 1, 65, 10, 1);
	Draw_Line(65, 10, 76, 1, 1);
	V_line(75,1,28, 1);

	Fill_Circle(93,16,15, 1);

	Fill_Obdelnik(110,16,10,2, 1);
	Fill_Obdelnik(114,12,2,10, 1);

	Fill_Obdelnik(0,35,127,28, 1);
	GLCD_GoTo(25,45);
	GLCD_WrString("**UTB Zlin**",0);

}

void Tank_init(){

			PORTA->PCR[4] = 0x100;
			PORTA->PCR[5] = 0x100;
			PORTA->PCR[16] = 0x100;
			PORTA->PCR[17] = 0x100;

			PTA->PDDR &= ~(1<<4);  		//tlacitko 1-4 jako vstup
			PTA->PDDR &= ~(1<<5);
			PTA->PDDR &= ~(1<<16);
			PTA->PDDR &= ~(1<<17);

			hrac.ord.x=TANK_STARTXPOS;
			hrac.ord.y=TANK_STARTYPOS;

			raketa.status = ZNICEN;

			score1=score2=score3=0;

}

void Tank_control(){

	if((PTA->PDIR & M_SW2) == 0 && hrac.ord.x + TANK_WIDTH < MAX_WIDTH )
		hrac.ord.x += TANK_STEPX;
	if((PTA->PDIR & M_SW1) == 0 && hrac.ord.x > 0)
		hrac.ord.x -= TANK_STEPX;
	//raketa
	if((PTA->PDIR & M_SW3) == 0 & raketa.status != ACTIVE){
		raketa.x = hrac.ord.x + (TANK_WIDTH / 2);
		raketa.y = TANK_STARTYPOS;
		raketa.status = ACTIVE;
	}

}

void Raketa_control(){

	if(raketa.status == ACTIVE){
		raketa.y -= RAKETA_RYCHLOST;

		if(raketa.y + RAKETA_HEIGH < 0)
			raketa.status = ZNICEN;
	}
}

void Kolize_control(){

	Kolize_postava_raketa();
}

void Kolize_postava_raketa(){


	if(raketa.status == ACTIVE){

		for(int i=0;i<NUM_ROWS;i++){
			for(int j=0;j<NUM_COLS;j++){

				if(raketa.status == ACTIVE){
					if(aliens[i][j].ord.status == ACTIVE){
						if(Kolize(aliens[i][j].ord.x, aliens[i][j].ord.y)){				//zasah

							aliens[i][j].ord.status=ZNICEN;
							switch(i){
							case 0: score1++;
							break;
							case 1: score2++;
							break;
							case 2: score3++;
							break;
							}
							Game_stats();
							raketa.status=ZNICEN;
						}
					}
				}
			}
		}
	}


}

//kontrola kolize
_Bool Kolize(uint8_t alienX, uint8_t alienY ){
	return((raketa.x  > alienX) & (raketa.x < alienX + postavaDx) & (raketa.y + RAKETA_HEIGH > alienY) & (raketa.y < alienY + postavaDy ));
}

void Enemaci_control(){

	if((EnemaciCounter--) < 0){

		uint8_t drop=0;

		if(RightMosPos() + pohybX >= MAX_WIDTH || LeftMosPos() + pohybX <= 0){
			pohybX = -pohybX;
			drop = 1;
		}

		for(int i=0;i<NUM_ROWS;i++){

			for(int j=0;j<NUM_COLS;j++){

				if(aliens[i][j].ord.status == ACTIVE){
					if(drop == 0){

						aliens[i][j].ord.x += pohybX;
					}else {

						aliens[i][j].ord.y += 4;
						Konec_hry(aliens[i][j].ord.y);   //funkce pro konec hry
					}
				}
			}
		}
		EnemaciCounter = RYCHLOST;		//15 ide
		animace = !animace;

	}
}


int LeftMosPos(){

	int i,down, nejmensi;
	nejmensi=200;
	for(i=0;i<NUM_COLS;i++){
		down=0;
		while(down < NUM_ROWS){
			if(aliens[down][i].ord.status == ACTIVE){
				if(aliens[down][i].ord.x < nejmensi)
					nejmensi=aliens[down][i].ord.x;
			}
			down++;
		}
		if(nejmensi < 200) return nejmensi;
	}
	return 0;
}

int RightMosPos(){
	int i,down, nejvetsi, rightPos;
	nejvetsi=0;
	for(i=NUM_COLS - 1; i >=0; i--){
		down=0;
		while(down < NUM_ROWS){

			if(aliens[down][i].ord.status == ACTIVE){
				rightPos=aliens[down][i].ord.x + postavaDx;
				if(rightPos > nejvetsi)
					nejvetsi=rightPos;
			}
			down++;
		}
		if(nejvetsi > 0) return nejvetsi;
	}
	return 0;
}

void Enemaci_init(){
	for(int i=0;i < NUM_ROWS;i++){
		for(int j=0;j < NUM_COLS;j++){
			aliens[i][j].ord.x=pozice_chobotnicX[j];
			aliens[i][j].ord.y=pozice_chobotnicY[i];
			aliens[i][j].ord.status=ACTIVE;

		}
	}

}

void Displej_zobrazit(){

	for(int j=0;j < NUM_ROWS;j++){

		for(int i=0;i < NUM_COLS ;i++){

			if(aliens[j][i].ord.status == ACTIVE){
				GLCD_GoTo(aliens[j][i].ord.x, aliens[j][i].ord.y );

				switch(j){

						case 0:

								if(animace) Chobot_vykreslit();
								else Chobot1_vykreslit();

						break;

						case 1:
								if(animace) Meduza_vykreslit();
								else Meduza1_vykreslit();

						break;

						case 2:

								if(animace) Krakatice_vykreslit();
								else Krakatice1_vykreslit();
						break;
			  }
		  }

		}
	}
	//vykresleni zbytku

	Tank_vykreslit();
	if(raketa.status == ACTIVE) Raketa_vykreslit();
}


void Konec_hry(int poloha){
	if(poloha >= hrac.ord.y - 5){
		GAME_ON = 0;
	}
}



void End_screen(){
	GLCD_ClearScreen(128);
	GLCD_GoTo(30, 35);
	GLCD_WrString("Konec hry", 1);
}


void Chobot_vykreslit(){

	for(int x=0; x < postavaDx; x++)
		GLCD_WrDat(chobot_bitMap[x]);
	//cekej(10);
}

void Chobot1_vykreslit(){

	for(int x=0; x < postavaDx; x++)
		GLCD_WrDat(chobot1_bitMap[x]);
	//cekej(10);
}

void Tank_vykreslit(){

	GLCD_GoTo(hrac.ord.x, hrac.ord.y );
	for(int x=0; x < TANK_WIDTH; x++)
			GLCD_WrDat(tank_bitMap[x]);
		//cekej(100);
}

void Raketa_vykreslit(){

	GLCD_GoTo(raketa.x, raketa.y );
	GLCD_WrDat(raketa_bitMap[0]);

}

void Meduza_vykreslit(){

	for(int x=0; x < postavaDx; x++)
			GLCD_WrDat(meduza_bitMap[x]);
		//cekej(1000);

}

void Meduza1_vykreslit(){

	for(int x=0; x < postavaDx; x++)
			GLCD_WrDat(meduza1_bitMap[x]);
		//cekej(1000);
}

void Krakatice_vykreslit(){

	for(int x=0; x < postavaDx; x++)
			GLCD_WrDat(krakatice_bitMap[x]);
		//cekej(1000);
}

void Krakatice1_vykreslit(){

	for(int x=0; x < postavaDx; x++)
			GLCD_WrDat(krakatice1_bitMap[x]);
		//cekej(1000);
}


