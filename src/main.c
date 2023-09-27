//////////////////////////////////////////////////////////////////////////////
// HAMOOPI(G) by GameDevBoss (Daniel Moura) 2015-2022                       //
// www.youtube.com/c/GameDevBoss                                            //
// HAMOOPIG É GRATUITO E SEMPRE SERÁ - HAMOOPIG IS FREE AND ALWAYS WILL BE  //
// O CONHECIMENTO DEVE SER COMPARTILHADO - KNOWLEDGE MUST BE SHARED         //
//////////////////////////////////////////////////////////////////////////////
// SPECIAL THANKS                                                           //
// SIR MACHO                                                                //
// TITAN DOOM                                                               //
//////////////////////////////////////////////////////////////////////////////

#include <genesis.h>
#include "sprite.h"
#include "gfx.h"
#include "sound.h"

//--- DEFINICOES ---//
#define RELEASE 0
#define WEAK   1
#define MEDIUM 2
#define FIERCE 3
#define LP     1
#define MP     2
#define HP     3
#define LK     4
#define MK     5
#define HK     6
#define INDEX_MAR 0xe3 // Usado na animacao do cenario (endereco de memoria)
#define CIGD 3         // (C)iclo(I)nteracoes(G)ravidade(D)efault (usado na engine de Fisica)
#define BODYSPACE 15   // Distancia minima entre os jogadores (x2)
#define RAGETIMER 600  // Tempo de Fúria no samsho2, antes de voltar ao normal
//SOUND DEFs
#define INGAME_SFX 64
#define P1_SFX     65
#define P2_SFX     66
#define SFX_ARG    70
#define SFX_FATAL  71

//--- FUNCOES ---//
void FUNCAO_INICIALIZACAO();
void FUNCAO_ROUND_INIT();
void FUNCAO_ROUND_RESTART();
void FUNCAO_RELOGIO();
void FUNCAO_BARRAS_DE_ENERGIA();
void PLAYER_STATE(u8 Player, u16 State);
void FUNCAO_PLAY_SND(u8 Player, u16 State);
void FUNCAO_INPUT_SYSTEM();
void FUNCAO_FSM();
void FUNCAO_FSM_HITBOXES(u8 Player);
void FUNCAO_PHYSICS();
bool FUNCAO_CHECK_GUARD(u8 guardFlag, u8 enemy);
void FUNCAO_SPR_POSITION();
void FUNCAO_ANIMACAO();
void FUNCAO_CAMERA_BGANIM();
void FUNCAO_DEPTH(u8 Player);
void FUNCAO_SAMSHOFX();
void FUNCAO_DEBUG();
bool FUNCAO_COLISAO(s16 R1x1, s16 R1y1, s16 R1x2, s16 R1y2, s16 R2x1, s16 R2y1, s16 R2x2, s16 R2y2);
void FUNCAO_UPDATE_LIFESP(u8 Player, u8 EnergyType, s8 Value);
void CLEAR_VDP();

void FUNCAO_FSM_DEFENSE(u8 player, u8 enemy);
void FUNCAO_FSM_NORMAL_ATTACKS(u8 player);
void FUNCAO_FSM_SPECIAL_ATTACKS(u8 player);
void FUNCAO_FSM_COLLISION(u8 player, u8 enemy);
void FUNCAO_CHECK_FRAME_ADVANTAGE();

//--- VARIAVEIS ---//
u8   i;                   //Variavel inteira de uso geral
u8   doubleHitStep=0;
u32  gFrames = 0;         //Contador de frames
bool gPauseSystem = 0;    //sistema de pause
u16  gPauseKoTimer = 0;
bool secretCharsAcvation = 0;
bool gPodeMover = 1;      //Desabilita os inputs dos players em momentos especificos (inicio do Round por exemplo)
u8   gWinnerID;           //Personagem Vencedor da Luta
u8   gLoseID;             //Personagem Perdedor da Luta
bool gContinueOption;     //Reinicia, ou nao, a luta
u8   gRoom = 1;           //'Room / Sala' do jogo (Tela de apresentacao, Menu, In game, etc)
u8   gTimerExit;          //Timer para ir para a proxima tela, usada em algumas rooms
u8   gDescompressionExit; //Usado na Room de Descompressao
s8   gClockTimer=60;      //Relogio, timer de 1 seg
s8   gClockLTimer=9;      //Digito esquerdo do Relogio
s8   gClockRTimer=9;      //Digito direito do Relogio
u8   gRound=1;            //Round Number
u8   gSombraStyle=1;      //Estilo de Sombra, carregamento de 1 ou 2 sombras, por padrao, carregamos 1 para liberar mais tiles na VDP
bool gDebug = 0;          //Exibe o Debug
char gStr[64];            //Variavel para armazenar textos, usada no debug
u16  gInd_tileset;        //Variavel utilizada para carregar dados de background
bool gASG_system;         //variavel usada pelo "Anti Sprite Glitch System" system (A.S.G.S.)
u8   gPing2 = 0;          //Variavel que alterna entre 0 e 1 constantemente
u8   gPing4 = 0;          //Variavel que alterna entre 0 ; 1 ; 2 ; 3 constantemente
u8   gPing10 = 0;         //Variavel que alterna entre 0 ; 1 ; 2 ... 7 ; 8 ; 9 constantemente
u8   gInputTimerCountDown=20; //Utilizada para os comandos de magias
u16  gDistancia;          //Distancia entre os Players
u8   gAlturaPiso;         //Altura do Piso
u8   gBgAnim;             //Timer para animacao do cenario
u16  gLimiteCenarioE= 30; //Limite Esquerdo para controle de movimentacao
u16  gLimiteCenarioD=290; //Limite Direito  para controle de movimentacao
u8   gravidadePadrao = 3; //Utilizado nos controles de Fisica
u8   impulsoPadrao = -14; //Utilizado nos controles de Fisica
u16  gBG_Width;           //Largura do Cenario, em pixels
s16  camPosX = 0;         //Posicao da Camera
s16  camPosXanterior = 0; //Posicao da Camera no frame Anterior
s16  gMeioDaTela = 0;     //Meio da Camera em X
s16  gScrollValues[28] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }; //Scrolling do Cenario
s16  gScrollValue;        //Scrolling de Cenario
u8   gBG_Choice=1;        //Cenario Escolhido
u8   Spark1_countDown;    //Desativa Efeito Spark1
u8   Spark2_countDown;    //Desativa Efeito Spark2
s16  PetalaPX[40];        //Petalas da tela de Apresentacao, X
s16  PetalaPY[40];        //Petalas da tela de Apresentacao, Y

int  frameAdvCounterP1;
int  lastFrameAdvCounterP1;
int  frameAdvCounterP2;
int  lastFrameAdvCounterP2;

//Char Selection Menu
u8   cursorP1linha  = 1;  //Posicao inicial do P1 na tela de Char Select
u8   cursorP1coluna = 1;  //Posicao inicial do P1 na tela de Char Select
u8   cursorP1_ID;         //Atribui ID ao player de acordo com o cursor de selecao
u8   cursorP1ConfirmFX;   //timer auxiliar, ver 'cursorConfirmTimer'
u8   cursorP1ColorChoice; //Escolha de cor
u8   cursorP2linha  = 1;  //Posicao inicial do P2 na tela de Char Select
u8   cursorP2coluna = 1;  //Posicao inicial do P2 na tela de Char Select
u8   cursorP2_ID;         //Atribui ID ao player de acordo com o cursor de selecao
u8   cursorP2ConfirmFX;   //timer auxiliar, ver 'cursorConfirmTimer'
u8   cursorP2ColorChoice; //Escolha de cor
u8   cursorConfirmTimer;  //Timer que confirma a selecao dos Chars, usado pelo efeito do cursor, FX 
bool endP1Selection;      //Encerra as selecoes do Char Select
bool endP2Selection;      //Encerra as selecoes do Char Select
u8   stepRoom=1;          //etapa em que esta a room, var de controle
u8   mainMenuOption=1;    //usada na main menu
u8   charSelStep=0;       //etapa de escolha dos personagens, util no modo P1 vs IA

//LINKUEI INTRO
u16 palette[64];
s16 scrollvalues[224];
s16 scrollvaluesA[224];
int X;
int Y;
int px;
int py;
int x1 = -8-(19*8);
int y1 = 84;
int fps;
int flicker;
int timer;
int framecount;

//Sprites
Sprite* Rect1BB1_Q1; Sprite* Rect1BB1_Q2; Sprite* Rect1BB1_Q3; Sprite* Rect1BB1_Q4; 
Sprite* Rect1HB1_Q1; Sprite* Rect1HB1_Q2; Sprite* Rect1HB1_Q3; Sprite* Rect1HB1_Q4;
Sprite* Rect2BB1_Q1; Sprite* Rect2BB1_Q2; Sprite* Rect2BB1_Q3; Sprite* Rect2BB1_Q4;
Sprite* Rect2HB1_Q1; Sprite* Rect2HB1_Q2; Sprite* Rect2HB1_Q3; Sprite* Rect2HB1_Q4;

Sprite* ClockL;      //Relogio, digito esquedo, N_
Sprite* ClockR;      //Relogio, digito direito, _N
Sprite* HUD_Lethers; //Round N, Duel N, Fight, Begin, etc...
Sprite* Spark[3];    //Efeitos de Hits, Blocks, Magias, etc

//Inputs, Joystick
bool JOY1_UP, JOY1_DOWN, JOY1_LEFT, JOY1_RIGHT, JOY1_A, JOY1_B, JOY1_C, JOY1_X, JOY1_Y, JOY1_Z, JOY1_START, JOY1_MODE;
bool key_JOY1_UP_pressed, key_JOY1_UP_hold, key_JOY1_UP_released;
bool key_JOY1_DOWN_pressed, key_JOY1_DOWN_hold, key_JOY1_DOWN_released;
bool key_JOY1_LEFT_pressed, key_JOY1_LEFT_hold, key_JOY1_LEFT_released;
bool key_JOY1_RIGHT_pressed, key_JOY1_RIGHT_hold, key_JOY1_RIGHT_released;
bool key_JOY1_A_pressed, key_JOY1_A_hold, key_JOY1_A_released;
bool key_JOY1_B_pressed, key_JOY1_B_hold, key_JOY1_B_released;
bool key_JOY1_C_pressed, key_JOY1_C_hold, key_JOY1_C_released;
bool key_JOY1_X_pressed, key_JOY1_X_hold, key_JOY1_X_released;
bool key_JOY1_Y_pressed, key_JOY1_Y_hold, key_JOY1_Y_released;
bool key_JOY1_Z_pressed, key_JOY1_Z_hold, key_JOY1_Z_released;
bool key_JOY1_START_pressed, key_JOY1_START_hold, key_JOY1_START_released;
bool key_JOY1_MODE_pressed, key_JOY1_MODE_hold, key_JOY1_MODE_released;

bool JOY2_UP, JOY2_DOWN, JOY2_LEFT, JOY2_RIGHT, JOY2_A, JOY2_B, JOY2_C, JOY2_X, JOY2_Y, JOY2_Z, JOY2_START, JOY2_MODE;
bool key_JOY2_UP_pressed, key_JOY2_UP_hold, key_JOY2_UP_released;
bool key_JOY2_DOWN_pressed, key_JOY2_DOWN_hold, key_JOY2_DOWN_released;
bool key_JOY2_LEFT_pressed, key_JOY2_LEFT_hold, key_JOY2_LEFT_released;
bool key_JOY2_RIGHT_pressed, key_JOY2_RIGHT_hold, key_JOY2_RIGHT_released;
bool key_JOY2_A_pressed, key_JOY2_A_hold, key_JOY2_A_released;
bool key_JOY2_B_pressed, key_JOY2_B_hold, key_JOY2_B_released;
bool key_JOY2_C_pressed, key_JOY2_C_hold, key_JOY2_C_released;
bool key_JOY2_X_pressed, key_JOY2_X_hold, key_JOY2_X_released;
bool key_JOY2_Y_pressed, key_JOY2_Y_hold, key_JOY2_Y_released;
bool key_JOY2_Z_pressed, key_JOY2_Z_hold, key_JOY2_Z_released;
bool key_JOY2_START_pressed, key_JOY2_START_hold, key_JOY2_START_released;
bool key_JOY2_MODE_pressed, key_JOY2_MODE_hold, key_JOY2_MODE_released;

//Inteligencia Artificial PLAYER 2 by Nemezes ( nemezes.itch.io / mangangateam.com )
bool IAP2;
u16 acaoIA;
u16 varIA;
u16 tempoIA;
u16 tempoIAacao;
u16 tempoIAataque;
u16 tempoIAmagia;
u16 esmurra;
u16 pulandoFrenteTras;
u16 defendeTempo;
bool ctrlResetInput = FALSE;
//modo campanha
u8 P2fase[9];
const Image *BGfase[9];
u8 tempoMaxIAataque[9];
u8 tempoMinIAataque[9];
u8 defesaIA[9];
u8 fase;
u8 faseMAX;

typedef struct {
	Sprite* spriteFBall; //Sprite da Fireball do Player
	s8 direcao;          //Direcao de propagacao da Fireball (eixo X)
	u8 speedX;           //Velocidade da Magia (eixo X)
	bool active;         //Fireball está ativada ou nao
	u8  countDown;       //Para remocao da maiga da tela
	s16 x;               //Posicao X da Magia
	s16 y;               //Posicao Y da Magia
	u8  guardFlag;       //Utilizado para verificar qual defesa protege do ataque
	u16 dataHBox[4];     //Posiconamento das Hit Boxes (caixas vermelhas de ataque)
} FireballDEF;

struct PlayerDEF {
	Sprite* sprite;      //Sprite do Player 
	Sprite* sombra;      //Sprite (sombra) do Player 
	FireballDEF fball;   //Fireball do Player
	u16 bufferSpecial;
	int paleta;          //Paleta do personagem
	u8  palID;           //ID da paleta ativa no momento
	u8  wins;            //Contador de vitorias; Rounds vencidos
	s8  energia;         //Energia do Player, usada para fins de HUD, graficos das barras
	s8  energiaBase;     //Energia Base do Player, essa é a energia real, usada para definir o round
	s8  energiaSP;       //Energia / Barra de Especial
	u16 bebado;
	u8  rageLvl;         //Nivel de Raiva, usado no jogo Samurai Shodown 2 /*samsho2*/
	u16 rageTimerCountdown; //Tempo em que fica furioso no samsho2

	u16 state;           //Controla o estado (animacao) do Player
	u8 stateMoveType;    //Para identificar o estado de movimento do personagem: 0=estado neutro; 1=atacando; 2=acertado por ataques fisicos; 3=acertado por projeteis
	bool control;		 //Para verificar se o personagem esta sobe controle ou nao


	u8  joyDirTimer[10]; //Utilizado para verificar o comando das magias -> Direcionais Timers
	u8  inputArray[5];   //Utilizado para verificar o comando das magias -> Direcionais Ordenados
	u8  attackPower;     //Utilizado para verificar o comando das magias -> Weak; Medium; Fierce
	u8  attackButton;    //Utilizado para verificar o comando das magias -> LP, MP, HK, LK, MK, HK
	u8  guardFlag;       //Utilizado para verificar qual defesa protege do ataque: 0=Sem defesa; 1=Pode defende em pe e abaixado; 2=Tem que defender abaixado; 3=Tem que defender em pe
	u8  id;              //Identificacao numerica do personagem selecionado. Exemplo: Ryu=1; Ken=2; etc...
	s16 x;               //Posicao X do Player
	s16 y;               //Posicao Y do Player
	u8  w;               //Largura do Sprite
	u8  h;               //Altura do Sprite
	u8  axisX;           //Posicao X do Ponto Pivot
	u8  axisY;           //Posicao Y do Ponto Pivot
	s8  direcao;         //Direcao para qual o Player esta olhando (1 ; -1)
	u8  hSpeed;          //Velocidade Horizontal

	u8  puloTimer;       //Tempo do Pulo
	bool shotJump;       //Usado no pulo curto
	u8  cicloInteracoesGravidade;     //Controles de Gravidade
	u8  cicloInteracoesGravidadeCont; //Controles de Gravidade
	s8  impulsoY;                     //Controles de Gravidade
	s8  gravidadeY;                   //Controles de Gravidade

	bool setup;

	u16 frameTimeAtual;  //Tempo atual do frame de animacao corrente
	u16 frameTimeTotal;  //Tempo total do frame de animacao corrente
	u16 animFrame;       //Frame de animacao atual
	u16 animFrameTotal;  //Qtde total de frames deste estado (animacao)

	u8  hitPause;        //Tempo de congelamento apos o Hit
	u8  hitCounter;      //Contador de acertos que o personagem recebeu em sequencia

	u16 dataAnim[60];    //Total de frames disponiveis para cada estado (animacao)
	u16 dataHBox[4];     //Posiconamento das Hit Boxes (caixas vermelhas de ataque)
	u16 dataBBox[4];     //Posiconamento das Body Boxes (caixas azuis, area vulneravel a ataques)
	u16 dataMBox[4];     //Posiconamento das Mass Boxes (caixas de massa, corpo fisico do personagem, usado para empurrar)

	//JOYSTICK
	u8 key_JOY_UP_status; u8 key_JOY_DOWN_status; u8 key_JOY_LEFT_status; u8 key_JOY_RIGHT_status;
	u8 key_JOY_A_status; u8 key_JOY_B_status; u8 key_JOY_C_status;
	u8 key_JOY_X_status; u8 key_JOY_Y_status; u8 key_JOY_Z_status;
	u8 key_JOY_START_status; u8 key_JOY_MODE_status;
	u8 key_JOY_countdown[10]; //timer regressivo, ativado apos pressionar um botao direcional (usado para correr, esquivar, etc)
}; struct PlayerDEF P[3];

struct GraphicElementDEF {
	Sprite* sprite;      //Sprite do Graphic Element
}; struct GraphicElementDEF GE[25];

int main(u16 hard) /************** MAIN **************/
{
    //Inicializacao da VDP (Video Display Processor)
	SYS_disableInts();
	 VDP_init();                    //Inicializa a VDP (Video Display Processor)
	 VDP_setScreenWidth320();       //Resolucao padrao de 320x224 (Largura)
	 VDP_setScreenHeight224();      //Resolucao padrao de 320x224 (Altura)
	//VDP_setPlaneSize(64,32,TRUE);  //Recomendado para BGs grandes //old: SGDK 1.65
	 VDP_setTextPlane(BG_A);        //Textos serao desenhados no BG_A
	 VDP_setTextPalette(PAL1);      //Textos serao desenhados com a ultima cor da PAL0
    //  SPR_init(127, 384, 256);       //SPR_init(u16 maxSprite, u16 vramSize, u16 unpackBufferSize) //old: SGDK 1.65
	 SPR_init();       		//SPR_initEx(u16 vramSize)
	 VDP_setBackgroundColor(0);     //Range 0-63 //4 Paletas de 16 cores = 64 cores
	SYS_enableInts();
	
	//XGM_setPCM(SFX_ARG, arg_sfx, sizeof(arg_sfx)); 
	//XGM_setPCM(SFX_FATAL, fatalfury_sfx, sizeof(fatalfury_sfx)); 
	
	if(!hard){ SYS_hardReset(); } //Previne bug do Reset //hpig 1.1

	//////////////////////////////////////////////////////I.A. (config)
	fase = 4; //manter o valor igual a 1
	faseMAX = 8; //configurado para 8 fases no maximo (escolher valor de 1 a 8)
	
	IAP2 = FALSE;

	//DIFICULDADE DA IA

	//SE O TEMPO DE ATAQUE ESTIVER ENTRE tempoMinIAataque E tempoMaxIAataque SIGNIFICA QUE O PLAYER 2 ATACA
	tempoMinIAataque[1] =  60; //escolher valor de 2 a 255
	tempoMinIAataque[2] =  60;
	tempoMinIAataque[3] =  50;
	tempoMinIAataque[4] =  50;
	tempoMinIAataque[5] =  40;
	tempoMinIAataque[6] =  30;
	tempoMinIAataque[7] =  20;
	tempoMinIAataque[8] =  10;

	tempoMaxIAataque[1] = 100; //escolher valor de 2 a 255 (valor deve ser maior que o respectivo no tempoMinIAataque)
	tempoMaxIAataque[2] = 100;
	tempoMaxIAataque[3] = 100;
	tempoMaxIAataque[4] = 100;
	tempoMaxIAataque[5] =  55;
	tempoMaxIAataque[6] =  60;
 	tempoMaxIAataque[7] =  50;
	tempoMaxIAataque[8] =  80;

	//QUANTO MENOR O VALOR, MAIS A IA DO PLAYER 2 DEFENDE
	defesaIA[1] =  50; //escolher valor de 10 a 255, valor menor ou igual a 10 = sempre defende
	defesaIA[2] =  50;
	defesaIA[3] =  10;
	defesaIA[4] =  10;
	defesaIA[5] =  90;
	defesaIA[6] =  60;
	defesaIA[7] =  30;
	defesaIA[8] =  15;

	P2fase[1] = 2; //escolha do Player 2 em cada fase: 1="haohmaru", 2="gillius"
	P2fase[2] = 1;
	P2fase[3] = 2;
	P2fase[4] = 1;
	P2fase[5] = 2;
	P2fase[6] = 1;
	P2fase[7] = 2;
	P2fase[8] = 1;
	//////////////////////////////////////////////////////I.A. (config)
	
    while(TRUE) /// LOOP PRINCIPAL ///
    {
        gFrames++; 
		if(gPing2  == 1){ gPing2 = -1; } gPing2++;  //var 'gPing2'  (50%) variacao: 0 ; 1
		if(gPing4  == 3){ gPing4 = -1; } gPing4++;  //var 'gPing4'  (25%) variacao: 0 ; 1 ; 2 ; 3
		if(gPing10 == 9){ gPing10= -1; } gPing10++; //var 'gPing10' (10%) variacao: 0 ; 1 ; 2 ; 3 ; 4 ; 5 ; 6 ; 7 ; 8 ; 9
		
		if(gRoom==1) //TELA HAMOOPIG --------------------------------------------------------------
		{
			FUNCAO_INPUT_SYSTEM(); //Verifica os joysticks 
			
			//inicializacao
			if(gFrames==1)
			{
				//XGM_startPlay(music_stage8);
				//XGM_isPlaying(); //FIX
				
				PAL_setPaletteColors(0, (u16 *)palette_black, CPU); 
				//BG_B
				VDP_loadTileSet(room_0_bgb.tileset, 1, DMA); //Load the tileset
				VDP_setTileMapEx(BG_B,room_0_bgb.tilemap, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, 1), 0, 0, 0, 0, 40, 28, DMA);
				//BG_A
				VDP_loadTileSet(room_0_bga.tileset, 501, DMA); //Load the tileset
				VDP_setTileMapEx(BG_A,room_0_bga.tilemap, TILE_ATTR_FULL(PAL3, 0, FALSE, FALSE, 501), 0, 0, 0, 0, 40, 28, DMA);
				
				//FADE IN
				memcpy(&palette[32], room_0_bgb.palette->data, 18 * 2);  
				memcpy(&palette[48], room_0_bga.palette->data, 18 * 2);  
				PAL_fadeIn(0, (4 * 16) - 1, palette, 20, FALSE);   
			}
			
			if(gFrames==60*2)
			{
				PAL_fadeOutAll(20, FALSE);
				waitMs(200);
			}
			
			//gFrames=60*5; 
			if( (gFrames>=60*2) || (P[1].key_JOY_START_status==1 && gFrames>10) || (P[2].key_JOY_START_status==1 && gFrames>10) ){
				CLEAR_VDP();
				gRoom=9;
				gDescompressionExit=10;
				//if(P[1].key_JOY_START_status==1 || P[1].key_JOY_START_status==2){gRoom=2; XGM_setPCM(P1_SFX, snd_confirm, sizeof(snd_confirm)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				gFrames=1;
				P[1].id=1; PAL_setPalette(PAL2, spr_ryo_pal1.palette->data, CPU); 
				P[2].id=1; PAL_setPalette(PAL3, spr_ryo_pal1.palette->data, CPU);
			};
		}
		
		if(gRoom==9) //DESCOMPRESSION -------------------------------------------------------------
		{
			GE[1].sprite = SPR_addSpriteExSafe(&spr_point,  0, 225, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
			GE[2].sprite = SPR_addSpriteExSafe(&spr_point,  0, 225, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
			GE[3].sprite = SPR_addSpriteExSafe(&spr_point,  0, 225, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
			
			if(gFrames==20)
			{
				if (GE[1].sprite){ SPR_releaseSprite(GE[1].sprite); GE[1].sprite = NULL; }
				if (GE[2].sprite){ SPR_releaseSprite(GE[2].sprite); GE[2].sprite = NULL; }
				if (GE[3].sprite){ SPR_releaseSprite(GE[3].sprite); GE[3].sprite = NULL; }
				gRoom=gDescompressionExit;
				gFrames=1; 
				CLEAR_VDP();
			}
			
		}
		
		if(gRoom==10) //IN GAME -------------------------------------------------------------------
		{
			//buffer de especiais para P1
			if(P[1].hitPause==0 && P[1].bufferSpecial!=0){
				PLAYER_STATE(1, P[1].bufferSpecial);
				P[1].bufferSpecial=0;
			}

			//buffer de especiais para P2
			if(P[2].hitPause==0 && P[2].bufferSpecial!=0){
				PLAYER_STATE(2, P[2].bufferSpecial);
				P[2].bufferSpecial=0;
			}
			
			
			/*//HWA bebado
			if( P[1].id==8 && P[1].bebado>0){ P[1].bebado--; }
			if( P[1].bebado>1 ){
				if(P[1].palID==1){ PAL_setPalette(PAL2, spr_hwa_pal1b.palette->data, CPU); } //hwa
				if(P[1].palID==2){ PAL_setPalette(PAL2, spr_hwa_pal2b.palette->data, CPU); } //hwa
			}
			if( P[1].bebado==1 ){
				if(P[1].palID==1){ PAL_setPalette(PAL2, spr_hwa_pal1.palette->data, CPU); } //hwa
				if(P[1].palID==2){ PAL_setPalette(PAL2, spr_hwa_pal2.palette->data, CPU); } //hwa
			}
			if( P[2].id==8 && P[2].bebado>0){ P[2].bebado--;  }
			if( P[2].bebado>1 ){
				if(P[2].palID==1){ PAL_setPalette(PAL3, spr_hwa_pal1b.palette->data, CPU); } //hwa
				if(P[2].palID==2){ PAL_setPalette(PAL3, spr_hwa_pal2b.palette->data, CPU); } //hwa
			}
			if( P[2].bebado==1 ){
				if(P[2].palID==1){ PAL_setPalette(PAL3, spr_hwa_pal1.palette->data, CPU); } //hwa
				if(P[2].palID==2){ PAL_setPalette(PAL3, spr_hwa_pal2.palette->data, CPU); } //hwa
			}
			*/
			
			//codigo de "SLOW MOTION KO"
			if((P[1].energiaBase==0 || P[2].energiaBase==0) && gFrames>100)
			{
				gPauseSystem=1;
				gPauseKoTimer++;
				if(gPauseKoTimer>=90 && gPauseKoTimer<=320)
				{
					if(gPing2==0){gPauseSystem=0;}
					if(gPing2==1){gPauseSystem=1;}
				}
				if(gPauseKoTimer>320)
				{
					gPauseSystem=0;
				}
			}else{
				gPauseKoTimer=0;
			}
			
			if(gFrames == 1){ 
				gPodeMover=0;
				FUNCAO_INICIALIZACAO(); //Inicializacao
			}
			if(gFrames<=355){ 
				FUNCAO_ROUND_INIT(); //Rotina de Letreiramento de inicio dos rounds
			}else{
				if(gPauseSystem==0) {
					FUNCAO_RELOGIO(); //HUD relogio
					FUNCAO_BARRAS_DE_ENERGIA(); //HUD barras
				}
				if(P[1].energiaBase==0 || P[2].energiaBase==0){ 
					gPodeMover=0; 
				}
			}
			
			//libera os graficos dos sparks
			if(Spark1_countDown>0){ 
				Spark1_countDown--; 
				if(Spark1_countDown==1) { 
					if(Spark[1]){ SPR_releaseSprite(Spark[1]); Spark[1] = NULL; }
				} 
			}
			if(Spark2_countDown>0){ 
				Spark2_countDown--; 
				if(Spark2_countDown==1) { 
					if(Spark[2]){ SPR_releaseSprite(Spark[2]); Spark[2] = NULL; }
				} 
			}
			
			if(doubleHitStep==1 && P[1].hitPause==0 && P[2].hitPause==0){ doubleHitStep=2; }
			
			if(gPauseSystem==0)
			{
				FUNCAO_INPUT_SYSTEM(); //Verifica os joysticks 
				
				FUNCAO_ANIMACAO(); //Atualiza animacao
				
				FUNCAO_FSM(); //FSM = Finite State Machine (Maquina de Estados)
				
				FUNCAO_PHYSICS(); //Funcoes de Fisica
				
				FUNCAO_CAMERA_BGANIM();
				
				FUNCAO_SAMSHOFX(); //Efeitos do jogo SS2
				
				if(gDebug == 1){ FUNCAO_DEBUG(); } //Debug
			}
			
		}
		
		if(gRoom==11) //AFTER MATCH ---------------------------------------------------------------
		{
			FUNCAO_INPUT_SYSTEM(); //Verifica os joysticks 
		}
		
		//--- FINALIZACOES ---//
		// VDP_showFPS(1);        //Mostra a taxa de FPS
		SPR_update();          //Atualiza (desenha) os sprites
        SYS_doVBlankProcess(); //Wait for screen refresh and do all SGDK VBlank tasks
    }
	
    return 0;
}

//--- FUNCOES ---//

void PLAYER_STATE(u8 Player, u16 State)
{
	
	if (P[Player].sprite){ SPR_releaseSprite(P[Player].sprite); P[Player].sprite = NULL; }
	
	P[Player].animFrame      = 1;
	P[Player].frameTimeAtual = 1;
	P[Player].dataAnim[1]    = 1;
	P[Player].animFrameTotal = 1;
	P[Player].state = State;
	
	FUNCAO_PLAY_SND(Player, State); //SOUND; SFX!
	
	//bug fix temporario
	//ffone
	if(State==507){ State=502; }
	
	//virando de lado (mudanca de estado) //hpig 1.1
	if(Player==1)
	{
		if(P[1].direcao== 1 && P[2].x<P[1].x && State==100){ State=607; }
		if(P[1].direcao==-1 && P[1].x<P[2].x && State==100){ State=607; }
		if(P[1].direcao== 1 && P[2].x<P[1].x && State==200){ State=608; }
		if(P[1].direcao==-1 && P[1].x<P[2].x && State==200){ State=608; }
		
		if(P[1].y==P[2].y)
		{
			      if(P[1].direcao== 1 && P[2].x<P[1].x && (State>=501 && State<=550) ){ P[1].direcao=-1; 
			}else if(P[1].direcao==-1 && P[1].x<P[2].x && (State>=501 && State<=550) ){ P[1].direcao= 1; 
			}
		}
	}else{
		if(P[2].direcao== 1 && P[1].x<P[2].x && State==100){ State=607; }
		if(P[2].direcao==-1 && P[2].x<P[1].x && State==100){ State=607; }
		if(P[2].direcao== 1 && P[1].x<P[2].x && State==200){ State=608; }
		if(P[2].direcao==-1 && P[2].x<P[1].x && State==200){ State=608; }
		
		if(P[1].y==P[2].y)
		{
			      if(P[2].direcao== 1 && P[1].x<P[2].x && (State>=501 && State<=550) ){ P[2].direcao=-1; 
			}else if(P[2].direcao==-1 && P[2].x<P[1].x && (State>=501 && State<=550) ){ P[2].direcao= 1; 
			}
		}
	}
	if(State==550)//bugfix
	{
		if(Player==1)
		{
			if(P[1].x>P[2].x && P[1].direcao== 1){ P[1].direcao=-1; 
			}else if(P[1].x<P[2].x && P[1].direcao==-1){ P[1].direcao= 1; }
		}
		if(Player==2)
		{
			if(P[2].x>P[1].x && P[2].direcao== 1){ P[2].direcao=-1; 
			}else if(P[2].x<P[1].x && P[2].direcao==-1){ P[2].direcao= 1; }
		}
	}
	
	//hpig 1.1 bugfix special wrong side after jump
	if(State==606)
	{
		if(Player==1)
		{
			if(P[1].x>P[2].x && P[1].direcao== 1){ u8 a; a=P[1].joyDirTimer[6]; P[1].joyDirTimer[6]=P[1].joyDirTimer[4]; P[1].joyDirTimer[4]=a; }
			if(P[1].x<P[2].x && P[1].direcao==-1){ u8 a; a=P[1].joyDirTimer[6]; P[1].joyDirTimer[6]=P[1].joyDirTimer[4]; P[1].joyDirTimer[4]=a; }
		}
		if(Player==2)
		{
			if(P[2].x>P[1].x && P[2].direcao== 1){ u8 a; a=P[2].joyDirTimer[6]; P[2].joyDirTimer[6]=P[2].joyDirTimer[4]; P[2].joyDirTimer[4]=a; }
			if(P[2].x<P[1].x && P[2].direcao==-1){ u8 a; a=P[2].joyDirTimer[6]; P[2].joyDirTimer[6]=P[2].joyDirTimer[4]; P[2].joyDirTimer[4]=a; }
		}
	}
	if(State==700 || State==710 || State==720 || State==730 || State==740 || State==750 || State==760 || State==770 || State==780 || State==790)
	{
		if(Player==1)
		{
			if( (P[1].x>P[2].x && P[1].direcao== 1) || (P[1].x<P[2].x && P[1].direcao==-1) ){ if(P[1].direcao==1){ P[1].direcao=-1; }else{P[1].direcao=1;} }
		}
		if(Player==2)
		{
			if( (P[2].x>P[2].x && P[2].direcao== 1) || (P[2].x<P[2].x && P[2].direcao==-1) ){ if(P[2].direcao==1){ P[2].direcao=-1; }else{P[2].direcao=1;} }
		}
		
	}
	
	//cross hpig 1.1
	if(State==606)
	{
		if(Player==1)
		{
			if(P[2].state>=501 && P[2].state<550)
			{
				State=100;
				if(P[1].x>P[2].x){P[1].direcao=-1;}else{P[1].direcao=1;}
			}
		}
		if(Player==2)
		{
			if(P[1].state>=501 && P[1].state<550)
			{
				State=100;
				if(P[2].x>P[1].x){P[2].direcao=-1;}else{P[2].direcao=1;}
			}
		}
		
		if(State==100)
		{
			bool test=FALSE;
			if(P[Player].direcao== 1 && P[Player].key_JOY_LEFT_status <=2 && P[Player].key_JOY_LEFT_status !=0){ test=TRUE; PLAYER_STATE(Player, 410); State=410; if (P[Player].sprite){ SPR_releaseSprite(P[Player].sprite); P[Player].sprite = NULL; } }
			if(P[Player].direcao== 1 && P[Player].key_JOY_RIGHT_status<=2 && P[Player].key_JOY_RIGHT_status!=0){ test=TRUE; PLAYER_STATE(Player, 420); State=420; if (P[Player].sprite){ SPR_releaseSprite(P[Player].sprite); P[Player].sprite = NULL; } }
			if(P[Player].direcao==-1 && P[Player].key_JOY_LEFT_status <=2 && P[Player].key_JOY_LEFT_status !=0){ test=TRUE; PLAYER_STATE(Player, 420); State=420; if (P[Player].sprite){ SPR_releaseSprite(P[Player].sprite); P[Player].sprite = NULL; } }
			if(P[Player].direcao==-1 && P[Player].key_JOY_RIGHT_status<=2 && P[Player].key_JOY_RIGHT_status!=0){ test=TRUE; PLAYER_STATE(Player, 410); State=410; if (P[Player].sprite){ SPR_releaseSprite(P[Player].sprite); P[Player].sprite = NULL; } }
			if(test==FALSE){ PLAYER_STATE(Player, 100); State=100; if (P[Player].sprite){ SPR_releaseSprite(P[Player].sprite); P[Player].sprite = NULL; } }
		}
		
	}
	
	//metodo2 SPR (desativado)
	//SPR_setDefinition (P[Player].sprite, &spr_raiden_100);
	
	//--- DEBUG_CHARACTER_TEST ---// #ID:0
	if(P[Player].id==0)
	{
		if(State==100 || State==610){
			P[Player].y = gAlturaPiso;
			P[Player].w = 8;
			P[Player].h = 8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_point, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
	}
	
	//--- RYO KOF94 ---// #ID:1
	if(P[Player].id==1)
	{
		if(State==100 || State==610){ //OK
			P[Player].y = gAlturaPiso;
			P[Player].w = 8*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)+4;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 5;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_100, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==101){ //OK
			P[Player].w = 11*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)+15;
			P[Player].axisY = P[Player].h-1;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_101, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==102){ //OK
			P[Player].w = 12*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)+16;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 8;
			P[Player].dataAnim[4]  = 8;
			P[Player].dataAnim[5]  = 8;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_102, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==103 || State==153 || State==203){  //OK
			P[Player].state=103;
			P[Player].w = 9*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 16;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 7;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_103, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==104){ //OK
			P[Player].w = 12*8;
			P[Player].h = 14*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_104, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==105){ //OK
			P[Player].w = 14*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)+40;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 8;
			P[Player].dataAnim[4]  = 8;
			P[Player].dataAnim[5]  = 9;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_105, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==106 || State==156 || State==206){ //OK
			P[Player].state=106;
			P[Player].w = 16*8;
			P[Player].h = 14*8;
			P[Player].axisX = (P[Player].w/2)+24;
			P[Player].axisY = P[Player].h-1;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 12;
			P[Player].dataAnim[4]  = 8;
			P[Player].dataAnim[5]  = 9;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_106, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==107){ //OK
			P[Player].w = 7*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 4;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_107, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==108){ //OK
			P[Player].w = 7*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 12;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_108, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==109){ //OK
			P[Player].w = 7*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 4;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_107, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==110){ //OK
			P[Player].w = 7*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 12;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_108, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==113){ //OK
			P[Player].w = 12*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)+15;
			P[Player].axisY = P[Player].h-1;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 10;
			P[Player].dataAnim[7]  = 8;
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_113, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==151){ //OK
			P[Player].w = 10*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)+5;
			P[Player].axisY = P[Player].h-1;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 4;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_151, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==152){ //OK
			P[Player].w = 11*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)+15;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 8;
			P[Player].dataAnim[4]  = 9;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_152, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==154){ //OK
			P[Player].w = 14*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)+24;
			P[Player].axisY = P[Player].h+1;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_154, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==155){ //OK
			P[Player].w = 12*8;
			P[Player].h = 17*8;
			P[Player].axisX = (P[Player].w/2)+17;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_155, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==200){ //OK
			P[Player].w = 7*8;
			P[Player].h = 9*8;
			P[Player].axisX = (P[Player].w/2)+2;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_200, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==201){ //OK
			P[Player].w = 9*8;
			P[Player].h = 10*8;
			P[Player].axisX = (P[Player].w/2)+10;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_201, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}		
		if(State==202){ //OK
			P[Player].w = 10*8;
			P[Player].h = 16*8;
			P[Player].axisX = (P[Player].w/2)+14;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 8;
			P[Player].dataAnim[5]  = 8;
			P[Player].dataAnim[6]  = 9;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_202, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==204){ //OK
			P[Player].w = 12*8;
			P[Player].h = 9*8;
			P[Player].axisX = (P[Player].w/2)+18;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_204, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==205){ //OK
			P[Player].w = 16*8;
			P[Player].h = 10*8;
			P[Player].axisX = (P[Player].w/2)+36;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 8;
			P[Player].dataAnim[3]  = 7;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_205, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==207){ //OK
			P[Player].w = 7*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-1;
			P[Player].dataAnim[1]  = 4;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_207, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==208){ //OK
			P[Player].w = 7*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-1;
			P[Player].dataAnim[1]  = 12;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_208, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==209){ //OK
			P[Player].w = 7*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-1;
			P[Player].dataAnim[1]  = 4;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_207, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==210){ //OK
			P[Player].w = 7*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-1;
			P[Player].dataAnim[1]  = 12;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_208, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==300){ //OK
			P[Player].w = 8*8;
			P[Player].h = 15*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 8;
			P[Player].dataAnim[5]  = 99;//18
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_300, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==301 || State==311 || State==321 || State==303 || State==313 || State==323){ //OK
			P[Player].w = 8*8;
			P[Player].h = 10*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h+24;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 99;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_301, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==302 || State==312 || State==322){ //OK
			P[Player].w = 9*8;
			P[Player].h = 15*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h-4;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 8;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 99;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_302, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==304 || State==305){ //OK
			P[Player].w = 14*8;
			P[Player].h = 11*8;
			P[Player].axisX = (P[Player].w/2)+16;
			P[Player].axisY = P[Player].h+20;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 99;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_304, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==306 || State==316 || State==326){ //OK
			P[Player].w = 14*8;
			P[Player].h = 11*8;
			P[Player].axisX = (P[Player].w/2)-8;
			P[Player].axisY = P[Player].h+26;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 99;
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_306, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==310){ //OK
			P[Player].w = 10*8;
			P[Player].h = 15*8;
			P[Player].axisX = (P[Player].w/2)-8;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 7;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 5;
			P[Player].dataAnim[6]  = 5;
			P[Player].dataAnim[7]  = 5;
			P[Player].dataAnim[8]  = 99;
			P[Player].animFrameTotal = 8;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_310, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==320){ //OK
			P[Player].w = 10*8;
			P[Player].h = 15*8;
			P[Player].axisX = (P[Player].w/2)-8;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 7;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 5;
			P[Player].dataAnim[6]  = 5;
			P[Player].dataAnim[7]  = 5;
			P[Player].dataAnim[8]  = 99;
			P[Player].animFrameTotal = 8;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_320, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==314 || State==324){ //OK
			P[Player].w = 12*8;
			P[Player].h = 11*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h+12;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 99;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_324, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==315 || State==325){ //OK
			P[Player].w = 12*8;
			P[Player].h = 15*8;
			P[Player].axisX = (P[Player].w/2)+4;
			P[Player].axisY = P[Player].h-12;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 8;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_325, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==410){ //OK
			P[Player].w = 7*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)+4;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 7;
			P[Player].dataAnim[2]  = 7;
			P[Player].dataAnim[3]  = 7;
			P[Player].dataAnim[4]  = 7;
			P[Player].dataAnim[5]  = 7;
			P[Player].dataAnim[6]  = 7;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_410, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==420){ //OK
			P[Player].w = 8*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)+4;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 6;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_420, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==471){ //OK
			P[Player].w = 10*8;
			P[Player].h = 13*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 18;
			P[Player].dataAnim[2]  = 2;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_471, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==472){ //OK
			P[Player].w = 9*8;
			P[Player].h = 13*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 18;
			P[Player].dataAnim[2]  = 2;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_472, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==501){
			P[Player].w = 9*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 12;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_501, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==502){
			P[Player].w = 9*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_501, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==503){
			P[Player].w = 11*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 12;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_502, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==504){
			P[Player].w = 11*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_502, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==505){
			P[Player].w = 10*8;
			P[Player].h = 10*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 12;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_503, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==506){
			P[Player].w = 10*8;
			P[Player].h = 10*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_503, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==550){
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].y-=(7*8)-4; //Compensa o deslocamento do eixo Y (.axisY)
			P[Player].w = 14*8;
			P[Player].h = 15*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = (P[Player].h)-(7*8)-4;
			P[Player].dataAnim[1]  = 9;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 9;
			P[Player].dataAnim[4]  = 99;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_550, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==551){
			P[Player].w = 15*8;
			P[Player].h =  8*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 99;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_551, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==552){
			P[Player].w = 12*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_552, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==570){
			P[Player].w = 14*8;
			P[Player].h =  4*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h)-4;
			P[Player].dataAnim[1]  = 30;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_570, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==601){ //OK
			P[Player].w = 7*8;
			P[Player].h = 12*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 4;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_606, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==602){ //OK
			P[Player].w = 7*8;
			P[Player].h = 12*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 4;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_606, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==603 || State==604 || State==605){ //OK
			P[Player].w = 7*8;
			P[Player].h = 12*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 2;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_606, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==606){ //OK
			P[Player].w = 7*8;
			P[Player].h = 12*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 2;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_606, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==607){ //OK
			P[Player].w = 7*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_607, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==608){ //OK
			P[Player].w = 6*8;
			P[Player].h = 9*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_608, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==611){ //OK
			P[Player].w = 8*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 250;
			P[Player].animFrameTotal = 8;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_611, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==612 || State==613){ //OK
			P[Player].w = 8*8;
			P[Player].h = 17*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].dataAnim[11] = 250;
			P[Player].animFrameTotal = 11;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_612, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==615){ //OK
			P[Player].w = 9*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 250;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_615, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==700){ //OK
			//P[Player].fBallCountDown=74; //caso queira que a fball desapareca depois de um tempo
			P[Player].w = 15*8;
			P[Player].h = 14*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 1;
			P[Player].dataAnim[8]  = 1;
			P[Player].dataAnim[9]  = 1;
			P[Player].dataAnim[10] = 1;
			P[Player].dataAnim[11] = 2;
			P[Player].dataAnim[12] = 34;
			P[Player].dataAnim[13] = 2;
			P[Player].animFrameTotal = 13;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_700, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==710){
			P[Player].w = 12*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)-8;
			if(P[Player].direcao==-1){ P[Player].axisX+=56; } //hack
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 5;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_710, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==730){
			P[Player].y = gAlturaPiso;
			P[Player].w = 10*8;
			P[Player].h = 19*8;
			P[Player].axisX = (P[Player].w/2)+10;
			P[Player].axisY = P[Player].h-1;
			P[Player].dataAnim[1] = 2;
			P[Player].dataAnim[2] = 2;
			P[Player].dataAnim[3] = 3;
			if(P[Player].attackPower==1){ P[Player].dataAnim[4] = 11; }else{ P[Player].dataAnim[4] = 22; }
			P[Player].dataAnim[5] = 2;
			P[Player].dataAnim[6] = 3;
			P[Player].dataAnim[7] = 3;
			P[Player].dataAnim[8] = 99;
			P[Player].animFrameTotal = 8;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_730, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}

		if(State==800){ //Agarrao
			P[Player].w = 11*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)+15;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_152, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==801){
			P[Player].w = 11*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)+15;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 8;
			P[Player].dataAnim[4]  = 9;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_152, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==802){ //OK
			P[Player].w = 16*8;
			P[Player].h = 10*8;
			P[Player].axisX = (P[Player].w/2)+36;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 8;
			P[Player].dataAnim[3]  = 7;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_205, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(State==803){
			P[Player].y = gAlturaPiso;
			P[Player].w = 8*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)+4;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 5;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_ryo_100, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
	}
		

	//guardFlag
	// 0 = Sem defesa
	// 1 = Pode defende em pe ou abaixado
	// 2 = Tem que defender abaixado
	// 3 = Tem que defender em pe
	if( State == 101 || State == 102  || State == 104 || State == 105 || State == 106 || State == 201 || State == 202 || State == 113){
		P[Player].guardFlag = 1;
	}
	if( State >= 700 && State <= 790 ){
		P[Player].guardFlag = 1;
	}
	if( State == 204 || State == 205 ){
		P[Player].guardFlag = 2;
	}
	if( State >= 301 && State <= 326 ){
		P[Player].guardFlag = 3;
	}
	if( State == 800 ){ //Agarrao
		P[Player].guardFlag = 0;
	}

	//stateMoveType
	//0 = estado neutro; 1 = atacando; 2 = acertado por ataques fisicos; 3 = acertado por projeteis
	if (State==100 || State==200 || State==300 || State==310 || State==320 ||State==410 || State==420) {
		P[Player].stateMoveType = 0;
		P[Player].control = TRUE;
	} else {
		P[Player].control = FALSE;
	}

	if ((State>=101 && State<=106) || (State>=151 && State<=156) || 
		(State>=201 && State<=206) || (State>=301 && State<=306) || 
		(State>=311 && State<=316) || (State>=321 && State<=326) || 
	    (State>=700 && State<=790) ||  State==113 || State==800)
	{
		P[Player].stateMoveType = 1;
	} 

	//Contador de hits
	if (State==100 || State==200 || State==570) {
		P[Player].hitCounter = 0;
	}

	if(P[Player].direcao==1){
		SPR_setHFlip(P[Player].sprite, FALSE);
	}else{
		SPR_setHFlip(P[Player].sprite, TRUE);
	}

	if (State==730) {
		 P[Player].setup = TRUE;
	}



	SPR_setAnimAndFrame(P[Player].sprite, 0, P[Player].animFrame-1);
	P[Player].frameTimeTotal  = P[Player].dataAnim[1];

	if (P[Player].sprite){ FUNCAO_DEPTH(Player); } //Define a prioridade de desenho (se sprite completamente carregado)

	FUNCAO_SPR_POSITION(); //Define a posicao do Sprite
	FUNCAO_FSM_HITBOXES(Player); //Atualiza as Hurt / Hitboxes
}

void FUNCAO_PLAY_SND(u8 Player, u16 State)
{
	//P1 utiliza o 'P1_SFX' e 'SOUND_PCM_CH3'
	//P2 utiliza o 'P2_SFX' e 'SOUND_PCM_CH4'
	
	//Samurai Shodown 2 (SS2) nota:
	//Os golpes <FRACOS> geralmente alternam entre 2 sons
	//Os golpes <MEDIOS> e <FORTES> geralmente, alternam entre 4 sons
	
	if(Player==1)
	{
		/*
		if(P[1].id==1) //ryo
		{
			if(
				State==101 || State==104 || 
				State==201 || State==204 || 
				State==301 || State==304 || 
				State==311 || State==314 || 
				State==321 || State==324
			)
			{
				if(gPing2==0){ XGM_setPCM(P1_SFX, snd_101 , sizeof(snd_101 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing2==1){ XGM_setPCM(P1_SFX, snd_101b, sizeof(snd_101b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			}
			if(
			State==151 || State==154
			)
			{
				XGM_setPCM(P1_SFX, snd_151 , sizeof(snd_151 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3);
			}
			if(
				State==102 || State==105 || 
				State==152 || State==155 || 
				State==202 || State==205 || 
				State==302 || State==305 || 
				State==312 || State==315 || 
				State==322 || State==325 ||
				State==113 || 
				State==106 || State==206 || State==306 || State==316 || State==326 
			)
			{
				if(gPing2==0){ XGM_setPCM(P1_SFX, snd_102 , sizeof(snd_102 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing2==1){ XGM_setPCM(P1_SFX, snd_102b, sizeof(snd_102b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			}
			if(State==110 || State==210){ XGM_setPCM(P1_SFX, snd_110, sizeof(snd_110)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==103 || State==300 || State==310 || State==320 || State==471 || State==472){ XGM_setPCM(P1_SFX, snd_300, sizeof(snd_300)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==501 || State==506 || State==511){ XGM_setPCM(P1_SFX, snd_501, sizeof(snd_501)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==502 || State==507 || State==512 || State==550){ XGM_setPCM(P1_SFX, snd_502, sizeof(snd_502)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==551){ XGM_setPCM(P1_SFX, snd_551, sizeof(snd_551)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==606){ XGM_setPCM(P1_SFX, snd_606, sizeof(snd_606)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			//if(State==618){ XGM_setPCM(P1_SFX, snd_haohmaru_618, sizeof(snd_haohmaru_618)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==700){ XGM_setPCM(P1_SFX, snd_ryo_700, sizeof(snd_ryo_700)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==710){ XGM_setPCM(P1_SFX, snd_ryo_710, sizeof(snd_ryo_710)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==720){ XGM_setPCM(P1_SFX, snd_ryo_720, sizeof(snd_ryo_720)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==730){ XGM_setPCM(P1_SFX, snd_ryo_730, sizeof(snd_ryo_730)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
		}
		*/
	}
	
	if(Player==2)
	{
		/*
		if(P[2].id==1) //ryo
		{
			if(
				State==101 || State==104 || 
				State==201 || State==204 || 
				State==301 || State==304 || 
				State==311 || State==314 || 
				State==321 || State==324
			)
			{
				if(gPing2==0){ XGM_setPCM(P2_SFX, snd_101 , sizeof(snd_101 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
				if(gPing2==1){ XGM_setPCM(P2_SFX, snd_101b, sizeof(snd_101b)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			}
			if(
			State==151 || State==154
			)
			{
				XGM_setPCM(P2_SFX, snd_151 , sizeof(snd_151 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4);
			}
			if(
				State==102 || State==105 || 
				State==152 || State==155 || 
				State==202 || State==205 || 
				State==302 || State==305 || 
				State==312 || State==315 || 
				State==322 || State==325 ||
				State==113 || 
				State==106 || State==206 || State==306 || State==316 || State==326 
			)
			{
				if(gPing2==0){ XGM_setPCM(P2_SFX, snd_102 , sizeof(snd_102 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
				if(gPing2==1){ XGM_setPCM(P2_SFX, snd_102b, sizeof(snd_102b)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			}
			if(State==110 || State==210){ XGM_setPCM(P2_SFX, snd_110, sizeof(snd_110)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==103 || State==300 || State==310 || State==320 || State==471 || State==472){ XGM_setPCM(P2_SFX, snd_300, sizeof(snd_300)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==501 || State==506 || State==511){ XGM_setPCM(P2_SFX, snd_501, sizeof(snd_501)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==502 || State==507 || State==512 || State==550){ XGM_setPCM(P2_SFX, snd_502, sizeof(snd_502)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==551){ XGM_setPCM(P2_SFX, snd_551, sizeof(snd_551)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==606){ XGM_setPCM(P2_SFX, snd_606, sizeof(snd_606)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			//if(State==618){ XGM_setPCM(P2_SFX, snd_haohmaru_618, sizeof(snd_haohmaru_618)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==700){ XGM_setPCM(P2_SFX, snd_ryo_700, sizeof(snd_ryo_700)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==710){ XGM_setPCM(P2_SFX, snd_ryo_710, sizeof(snd_ryo_710)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==720){ XGM_setPCM(P2_SFX, snd_ryo_720, sizeof(snd_ryo_720)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==730){ XGM_setPCM(P2_SFX, snd_ryo_730, sizeof(snd_ryo_730)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
		}
		*/
	}
	
	
	
}

void FUNCAO_RELOGIO()
{
	/*
	if(gClockTimer>0 && (gClockLTimer>0 || gClockRTimer>0) && (P[1].energiaBase>0 && P[2].energiaBase>0) ){ gClockTimer--; }
	//temporario
	if(gClockLTimer==0 && gClockRTimer==1){ gClockLTimer=9; gClockRTimer=9; }
	
	if(gClockTimer==0)
	{
		gClockRTimer--;
		SPR_releaseSprite(ClockR);
		if(gClockRTimer==9){ ClockR = SPR_addSpriteEx(&spr_n9, 161, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
		if(gClockRTimer==8){ ClockR = SPR_addSpriteEx(&spr_n8, 161, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
		if(gClockRTimer==7){ ClockR = SPR_addSpriteEx(&spr_n7, 161, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
		if(gClockRTimer==6){ ClockR = SPR_addSpriteEx(&spr_n6, 161, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
		if(gClockRTimer==5){ ClockR = SPR_addSpriteEx(&spr_n5, 161, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
		if(gClockRTimer==4){ ClockR = SPR_addSpriteEx(&spr_n4, 161, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
		if(gClockRTimer==3){ ClockR = SPR_addSpriteEx(&spr_n3, 161, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
		if(gClockRTimer==2){ ClockR = SPR_addSpriteEx(&spr_n2, 161, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
		if(gClockRTimer==1){ ClockR = SPR_addSpriteEx(&spr_n1, 161, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
		if(gClockRTimer==0){ ClockR = SPR_addSpriteEx(&spr_n0, 161, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
		SPR_setVRAMTileIndex(ClockR, 1447); //define uma posicao especifica para o GFX na VRAM
		if(ClockR){ SPR_setDepth(ClockR, 255); }
		
		if(gClockRTimer==-1)
		{
			if(gClockLTimer>0)
			{
				gClockLTimer--;
				SPR_releaseSprite(ClockL);
				if(gClockLTimer==9){ ClockL = SPR_addSpriteEx(&spr_n9, 146, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
				if(gClockLTimer==8){ ClockL = SPR_addSpriteEx(&spr_n8, 146, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
				if(gClockLTimer==7){ ClockL = SPR_addSpriteEx(&spr_n7, 146, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
				if(gClockLTimer==6){ ClockL = SPR_addSpriteEx(&spr_n6, 146, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
				if(gClockLTimer==5){ ClockL = SPR_addSpriteEx(&spr_n5, 146, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
				if(gClockLTimer==4){ ClockL = SPR_addSpriteEx(&spr_n4, 146, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
				if(gClockLTimer==3){ ClockL = SPR_addSpriteEx(&spr_n3, 146, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
				if(gClockLTimer==2){ ClockL = SPR_addSpriteEx(&spr_n2, 146, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
				if(gClockLTimer==1){ ClockL = SPR_addSpriteEx(&spr_n1, 146, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
				if(gClockLTimer==0){ ClockL = SPR_addSpriteEx(&spr_n0, 146, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
				SPR_setVRAMTileIndex(ClockL, 1441); //define uma posicao especifica para o GFX na VRAM
				gClockRTimer=9;
				SPR_releaseSprite(ClockR);
				ClockR = SPR_addSpriteEx(&spr_n9, 161, 22, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
				SPR_setVRAMTileIndex(ClockR, 1447); //define uma posicao especifica para o GFX na VRAM
				if(ClockL){ SPR_setDepth(ClockL, 255); }
			}
		}
		gClockTimer=38; //reset Clock Count timer
	}
	*/
	
	//if(gClockRTimer==-1 && gClockRTimer==-1)
	//{ 
		/*TIMER OVER!*/ 
		//ClockR = SPR_addSpriteEx(&spr_n0, 160, 32, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		//SPR_setVRAMTileIndex(ClockR,1707); //define uma posicao especifica para o GFX na VRAM
	//}
}

void FUNCAO_BARRAS_DE_ENERGIA()
{
	
	for(i=1; i<=2; i++)
	{
		if( P[i].energia != P[i].energiaBase )
		{ 
			if(P[i].energia > P[i].energiaBase){ P[i].energia--; } //decrementa a 'energia' aos poucos, até igualar a 'energiaBase'
			if(P[i].energia < P[i].energiaBase){ P[i].energia++; } //incrementa a 'energia' aos poucos, até igualar a 'energiaBase'
		} 
		
		u8 subEnergyPos;
		if(GE[2+i].sprite)
		{
			if(                           P[i].energia== 96){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 0); subEnergyPos= 0;
			}else if(P[i].energia<  96 && P[i].energia>= 88){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 1); subEnergyPos= 1;
			}else if(P[i].energia<  88 && P[i].energia>= 80){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 2); subEnergyPos= 2;
			}else if(P[i].energia<  80 && P[i].energia>= 72){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 3); subEnergyPos= 3;
			}else if(P[i].energia<  72 && P[i].energia>= 64){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 4); subEnergyPos= 4;
			}else if(P[i].energia<  64 && P[i].energia>= 56){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 5); subEnergyPos= 5;
			}else if(P[i].energia<  56 && P[i].energia>= 48){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 6); subEnergyPos= 6;
			}else if(P[i].energia<  48 && P[i].energia>= 40){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 7); subEnergyPos= 7;
			}else if(P[i].energia<  40 && P[i].energia>= 32){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 8); subEnergyPos= 8;
			}else if(P[i].energia<  32 && P[i].energia>= 24){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 9); subEnergyPos= 9;
			}else if(P[i].energia<  24 && P[i].energia>= 16){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,10); subEnergyPos=10;
			}else if(P[i].energia<  16 && P[i].energia>=  8){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,11); subEnergyPos=11;
			}else if(P[i].energia<   8 && P[i].energia>=  1){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,12); subEnergyPos=12;
			}else if(                     P[i].energia==  0){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,12); subEnergyPos=12;
			}
		}
		if(GE[4+i].sprite)
		{
			if(i==1){ SPR_setPosition(GE[4+i].sprite,  40+(subEnergyPos*8), 12); }
			if(i==2){ SPR_setPosition(GE[4+i].sprite, 280-(subEnergyPos*8), 12); }
		}
		
		int a=0;
		if(subEnergyPos== 0){ a= 96; }
		if(subEnergyPos== 1){ a= 88; }
		if(subEnergyPos== 2){ a= 80; }
		if(subEnergyPos== 3){ a= 72; }
		if(subEnergyPos== 4){ a= 64; }
		if(subEnergyPos== 5){ a= 56; }
		if(subEnergyPos== 6){ a= 48; }
		if(subEnergyPos== 7){ a= 40; }
		if(subEnergyPos== 8){ a= 32; }
		if(subEnergyPos== 9){ a= 24; }
		if(subEnergyPos==10){ a= 16; }
		if(subEnergyPos==11){ a=  8; }
		if(subEnergyPos==12){ a=  0; }
		if(GE[4+i].sprite){ SPR_setAnimAndFrame(GE[4+i].sprite, 0, (P[i].energia-a) ); }
		
		//Exibe a barra de especial, e fica piscando, caso seja == 32
		/*
		if(P[i].energiaSP<32)
		{
			SPR_setAnimAndFrame(GE[6+i].sprite, 0, P[i].energiaSP);
		}else{
			if(gPing2==1){
				SPR_setAnimAndFrame(GE[6+i].sprite, 0, 32);
			}else{
				SPR_setAnimAndFrame(GE[6+i].sprite, 0, 33);
			}
		}
		*/
		
	}
	
	//if(P[1].energiaSP>=32){ SPR_setVisibility(GE[ 9].sprite, VISIBLE); }else{ SPR_setVisibility(GE[ 9].sprite, HIDDEN); }
	//if(P[2].energiaSP>=32){ SPR_setVisibility(GE[10].sprite, VISIBLE); }else{ SPR_setVisibility(GE[10].sprite, HIDDEN); }
	
}


void FUNCAO_ANIMACAO()
{
	//CONTROLE DE ANIMACAO E END ANIMATION
	gASG_system = 0; //A.S.G.S. Anti Sprite Glitch System; (Evita a troca simultanea de sprites dos players)
	
	for(i=1; i<=2; i++)
	{
		if (P[i].hitPause==0) {
			
			P[i].frameTimeAtual++; 
			if(P[i].frameTimeAtual>P[i].frameTimeTotal && gASG_system==0) //hora de trocar o frame!
			{
				P[i].animFrame++;
				if(P[i].animFrame>P[i].animFrameTotal) //hora de trocar ou recarregar a animacao!
				{
					gASG_system = 1;
					
					//---FSM CHARS ini--------------------------------------------------
					//ryo 105
					//if(P[i].id==1 && P[i].state==105){ P[i].x+=16*P[i].direcao; }
					//richard 106
					//if(P[i].id==4 && P[i].state==106){ P[i].x+=24*P[i].direcao; }
					//duck 106
					//if(P[i].id==6 && P[i].state==106){ P[i].x+=40*P[i].direcao; }
					//billy 105
					//if(P[i].id==10 && P[i].state==105){ P[i].x+=180*P[i].direcao; }
					//---FSM CHARS end--------------------------------------------------
					
					if(P[i].state==100){ PLAYER_STATE(i,100); }
					if(P[i].state==200){ PLAYER_STATE(i,200); }
					
					// estes aqui previnem bug, caso o tempo de animacao dos Aereos seja definido errado
					// e o player nao tenha tempo de chegar ao solo...
					if(P[i].state==300 || P[i].state==310 || P[i].state==320){ PLAYER_STATE(i,100); }
					if(P[i].state==301 || P[i].state==302 || P[i].state==303){ PLAYER_STATE(i,100); }
					if(P[i].state==304 || P[i].state==305 || P[i].state==306){ PLAYER_STATE(i,100); }
					if(P[i].state==311 || P[i].state==312 || P[i].state==313){ PLAYER_STATE(i,100); }
					if(P[i].state==314 || P[i].state==315 || P[i].state==316){ PLAYER_STATE(i,100); }
					if(P[i].state==321 || P[i].state==322 || P[i].state==323){ PLAYER_STATE(i,100); }
					if(P[i].state==324 || P[i].state==325 || P[i].state==326){ PLAYER_STATE(i,100); }
					if(P[i].state==410){ PLAYER_STATE(i,410); }
					if(P[i].state==420){ PLAYER_STATE(i,420); }
					if(P[i].state==470 || P[i].state==471 || P[i].state==472){ if(P[i].key_JOY_DOWN_status>0){ PLAYER_STATE(i,200); }else{ PLAYER_STATE(i,100); } }
					if(P[i].state==601){ PLAYER_STATE(i,200); }
					if(P[i].state==602){ PLAYER_STATE(i,100); }
					if(P[i].state==603){ P[i].y-=1; P[i].puloTimer=1; PLAYER_STATE(i,310); }
					if(P[i].state==604){ P[i].y-=1; P[i].puloTimer=1; PLAYER_STATE(i,300); }
					if(P[i].state==605){ P[i].y-=1; P[i].puloTimer=1; PLAYER_STATE(i,320); }
					if(P[i].state==607){ PLAYER_STATE(i,100); } //virando em pe
					if(P[i].state==608){ PLAYER_STATE(i,200); } //virando abaixado
					if(P[i].state>=101 && P[i].state<=106){ PLAYER_STATE(i,100); }
					if(P[i].state==107){ PLAYER_STATE(i,108); }
					if(P[i].state==108){ PLAYER_STATE(i,108); }
					if(P[i].state==109){ PLAYER_STATE(i,100); }
					if(P[i].state==110){ PLAYER_STATE(i,109); }
					if(P[i].state==113){ PLAYER_STATE(i,100); }
					if(P[i].state>=151 && P[i].state<=156){ PLAYER_STATE(i,100); }
					if(P[i].state>=201 && P[i].state<=206){ PLAYER_STATE(i,200); }
					if(P[i].state==207){ PLAYER_STATE(i,208); }
					if(P[i].state==208){ PLAYER_STATE(i,208); }
					if(P[i].state==209){ PLAYER_STATE(i,200); }
					if(P[i].state==210){ PLAYER_STATE(i,209); }
					if(P[i].state>=501 && P[i].state<=507)
					{ 
						//desativado, pois eh possivel ir para andar, correr ou parado de acordo com as teclas direcionais pressionadas
						//PLAYER_STATE(i,100);
						
						if(P[i].direcao==1)
						{
							if(P[i].key_JOY_LEFT_status>0)
							{ 
								if(P[i].key_JOY_countdown[4]==0){ PLAYER_STATE(i,410); }else{ 
									if(P[i].inputArray[1]==4 && P[i].inputArray[0]==4){ PLAYER_STATE(i,471);  }else{ PLAYER_STATE(i,410); }
								}
							}
							if(P[i].key_JOY_RIGHT_status>0)
							{ 
								if(P[i].key_JOY_countdown[6]==0){ PLAYER_STATE(i,420); }else{ 
									if(P[i].inputArray[1]==6 && P[i].inputArray[0]==6){ PLAYER_STATE(i,472);  }else{ PLAYER_STATE(i,420); } 
								}
							}
							if(P[i].key_JOY_LEFT_status==0 && P[i].key_JOY_RIGHT_status==0){ PLAYER_STATE(i,100); } //end walk
						}else{
							if(P[i].key_JOY_LEFT_status>0)
							{ 
								if(P[i].key_JOY_countdown[4]==0){ PLAYER_STATE(i,420); }else{ 
									if(P[i].inputArray[1]==6 && P[i].inputArray[0]==6){ PLAYER_STATE(i,472);  }else{ PLAYER_STATE(i,420); } 
								}
							}
							if(P[i].key_JOY_RIGHT_status>0)
							{ 
								if(P[i].key_JOY_countdown[6]==0){ PLAYER_STATE(i,410); }else{ 
									if(P[i].inputArray[1]==4 && P[i].inputArray[0]==4){ PLAYER_STATE(i,471);  }else{ PLAYER_STATE(i,410); }
								}
							}
							if(P[i].key_JOY_LEFT_status==0 && P[i].key_JOY_RIGHT_status==0){ PLAYER_STATE(i,100); } //end walk
						}
						
						/*
						if(P[i].direcao==1)
						{
							
							
							if(P[i].key_JOY_LEFT_status==0 && P[i].key_JOY_RIGHT_status==0){ PLAYER_STATE(i,100); 
							}else if(P[i].key_JOY_LEFT_status>0){ 
								if(P[i].key_JOY_countdown[4]==0){ PLAYER_STATE(i,410); }else{ PLAYER_STATE(i,471); }
							}else if(P[i].key_JOY_RIGHT_status>0){ 
								if(P[i].key_JOY_countdown[6]==0){ PLAYER_STATE(i,420); }else{ PLAYER_STATE(i,472); }
							}
						}else{
							if(P[i].key_JOY_LEFT_status==0 && P[i].key_JOY_RIGHT_status==0){ PLAYER_STATE(i,100); 
							}else if(P[i].key_JOY_LEFT_status>0){ 
								if(P[i].key_JOY_countdown[4]==0){ PLAYER_STATE(i,420); }else{ PLAYER_STATE(i,472);}
							}else if(P[i].key_JOY_RIGHT_status>0){ 
								if(P[i].key_JOY_countdown[6]==0){ PLAYER_STATE(i,410); }else{ PLAYER_STATE(i,471);}
							}
							
						}
						*/
					} //end hurts
					if(P[i].state==511 || P[i].state==512){ PLAYER_STATE(i,200); }
					if(P[i].state==552){ PLAYER_STATE(i,100); } //end queda
					if(P[i].state==570){ if(P[i].energiaBase>0){ PLAYER_STATE(i,552); }else{ PLAYER_STATE(i,570); } } //end pulos
					if(P[i].state==606){ PLAYER_STATE(i,100); } //end pulos
					if(P[i].state==610){ PLAYER_STATE(i,100); } //end Intro
					if(P[i].state==611 || P[i].state==612 || P[i].state==613 || P[i].state==614){ FUNCAO_ROUND_RESTART(); } //end WIN Pose
					if(P[i].state==615){ FUNCAO_ROUND_RESTART(); } //end time lose
					if(P[i].state==618){ PLAYER_STATE(i,100); } //end Rage Explosion
					if(P[i].state>=700 && P[i].state<=790){ PLAYER_STATE(i,100); } //end magias

					if(P[i].state == 800){ PLAYER_STATE(i,100); } //agarrao - fim do inicio
					if(P[i].state == 801){ PLAYER_STATE(i,100); } //agarrao - se pegar, fim do primeiro movimento do player
					if(P[i].state == 802){ PLAYER_STATE(i,100); } //agarrao - se pegar, fim do segundo movimento do player
					if(P[i].state == 803){ PLAYER_STATE(i,100); } //agarrao - segar, fim do estado do inimigo
				}
				P[i].frameTimeAtual=1;
				P[i].frameTimeTotal = P[i].dataAnim[P[i].animFrame];
				
				//hpig 1.1 anti glitch animation overflow
				if(P[i].sprite)
				{
					u16 totalreal = P[i].sprite->animation->numFrame;
					if( (P[i].animFrame-1) < totalreal)
					{ 
						SPR_setAnimAndFrame(P[i].sprite, 0, P[i].animFrame-1); //set the animation if is OK
						FUNCAO_DEPTH(i);
					}
				}
				FUNCAO_FSM_HITBOXES(i); //Atualiza as Hurt / Hitboxes
			}else if(P[i].frameTimeAtual>P[i].frameTimeTotal && gASG_system==1){ //A.S.G.S. 
				if(P[i].frameTimeAtual>1){ P[i].frameTimeAtual--; }
			}
			//*NOTA: No caso do SS2, a animacao abaixando é encurtada (para se abaixar mais rapido) caso se mantenha pressionado para baixo apos o pulo //end pulos 
			/*samsho2*/ if(P[i].state==606 && P[i].animFrame==2 && P[i].frameTimeAtual>=3 && P[i].key_JOY_DOWN_status>=1){ PLAYER_STATE(i,200); } 
		}
	}
	FUNCAO_SPR_POSITION(); //Define a posicao do Sprite
}

void FUNCAO_SPR_POSITION()
{
	//ajusta posicao do sprite
	if(P[1].direcao== 1){ SPR_setPosition(P[1].sprite, P[1].x-(P[1].w-P[1].axisX)-camPosX, P[1].y-P[1].axisY); }
	if(P[1].direcao==-1){ SPR_setPosition(P[1].sprite, P[1].x- P[1].axisX        -camPosX, P[1].y-P[1].axisY); }
	if(P[2].direcao== 1){ SPR_setPosition(P[2].sprite, P[2].x-(P[2].w-P[2].axisX)-camPosX, P[2].y-P[2].axisY); }
	if(P[2].direcao==-1){ SPR_setPosition(P[2].sprite, P[2].x- P[2].axisX        -camPosX, P[2].y-P[2].axisY); }
	
	/*
	if(gSombraStyle==2)
	{
		SPR_setPosition(P[1].sombra, P[1].x-32-camPosX, gAlturaPiso-5); // *NOTA: A sombra do SS2 tem 80px, entao desloquei -40px no eixo X
		SPR_setPosition(P[2].sombra, P[2].x-32-camPosX, gAlturaPiso-5); // para que ela ficasse perfeitamente alinhada aos players 
	}else{
		if(gPing2==0){ SPR_setPosition(P[1].sombra, P[1].x-32-camPosX, gAlturaPiso-5); }
		if(gPing2==1){ SPR_setPosition(P[1].sombra, P[2].x-32-camPosX, gAlturaPiso-5); }
	}
	*/
}

void FUNCAO_INPUT_SYSTEM()
{
	
	//joystick P1
	u16 JOY1 = JOY_readJoypad(JOY_1);
	if(JOY1 & BUTTON_UP    ) { JOY1_UP    = TRUE; } else { JOY1_UP    = FALSE; }
	if(JOY1 & BUTTON_DOWN  ) { JOY1_DOWN  = TRUE; } else { JOY1_DOWN  = FALSE; }
	if(JOY1 & BUTTON_LEFT  ) { JOY1_LEFT  = TRUE; } else { JOY1_LEFT  = FALSE; }
	if(JOY1 & BUTTON_RIGHT ) { JOY1_RIGHT = TRUE; } else { JOY1_RIGHT = FALSE; }
	if(JOY1 & BUTTON_A     ) { JOY1_A     = TRUE; } else { JOY1_A     = FALSE; }
	if(JOY1 & BUTTON_B     ) { JOY1_B     = TRUE; } else { JOY1_B     = FALSE; }
	if(JOY1 & BUTTON_C     ) { JOY1_C     = TRUE; } else { JOY1_C     = FALSE; }
	if(JOY1 & BUTTON_X     ) { JOY1_X     = TRUE; } else { JOY1_X     = FALSE; }
	if(JOY1 & BUTTON_Y     ) { JOY1_Y     = TRUE; } else { JOY1_Y     = FALSE; }
	if(JOY1 & BUTTON_Z     ) { JOY1_Z     = TRUE; } else { JOY1_Z     = FALSE; }
	if(JOY1 & BUTTON_START ) { JOY1_START = TRUE; } else { JOY1_START = FALSE; }
	if(JOY1 & BUTTON_MODE  ) { JOY1_MODE  = TRUE; } else { JOY1_MODE  = FALSE; }
	
	//joystick P2
	/*
	u16 JOY2 = JOY_readJoypad(JOY_2);
	if(JOY2 & BUTTON_UP    ) { JOY2_UP    = TRUE; } else { JOY2_UP    = FALSE; }
	if(JOY2 & BUTTON_DOWN  ) { JOY2_DOWN  = TRUE; } else { JOY2_DOWN  = FALSE; }
	if(JOY2 & BUTTON_LEFT  ) { JOY2_LEFT  = TRUE; } else { JOY2_LEFT  = FALSE; }
	if(JOY2 & BUTTON_RIGHT ) { JOY2_RIGHT = TRUE; } else { JOY2_RIGHT = FALSE; }
	if(JOY2 & BUTTON_A     ) { JOY2_A     = TRUE; } else { JOY2_A     = FALSE; }
	if(JOY2 & BUTTON_B     ) { JOY2_B     = TRUE; } else { JOY2_B     = FALSE; }
	if(JOY2 & BUTTON_C     ) { JOY2_C     = TRUE; } else { JOY2_C     = FALSE; }
	if(JOY2 & BUTTON_X     ) { JOY2_X     = TRUE; } else { JOY2_X     = FALSE; }
	if(JOY2 & BUTTON_Y     ) { JOY2_Y     = TRUE; } else { JOY2_Y     = FALSE; }
	if(JOY2 & BUTTON_Z     ) { JOY2_Z     = TRUE; } else { JOY2_Z     = FALSE; }
	if(JOY2 & BUTTON_START ) { JOY2_START = TRUE; } else { JOY2_START = FALSE; }
	if(JOY2 & BUTTON_MODE  ) { JOY2_MODE  = TRUE; } else { JOY2_MODE  = FALSE; }
	*/
	
	//joystick P2
    //IAP2 = TRUE; //FALSE = IA do P2 desligada,  TRUE = IA do P2 ligada

	if (gRoom != 10 || (IAP2 == FALSE && gRoom == 10))
    {
        u16 JOY2 = JOY_readJoypad(JOY_2);
        if(JOY2 & BUTTON_UP    ) { JOY2_UP    = TRUE; } else { JOY2_UP    = FALSE; }
        if(JOY2 & BUTTON_DOWN  ) { JOY2_DOWN  = TRUE; } else { JOY2_DOWN  = FALSE; }
        if(JOY2 & BUTTON_LEFT  ) { JOY2_LEFT  = TRUE; } else { JOY2_LEFT  = FALSE; }
        if(JOY2 & BUTTON_RIGHT ) { JOY2_RIGHT = TRUE; } else { JOY2_RIGHT = FALSE; }
        if(JOY2 & BUTTON_A     ) { JOY2_A     = TRUE; } else { JOY2_A     = FALSE; }
        if(JOY2 & BUTTON_B     ) { JOY2_B     = TRUE; } else { JOY2_B     = FALSE; }
        if(JOY2 & BUTTON_C     ) { JOY2_C     = TRUE; } else { JOY2_C     = FALSE; }
        if(JOY2 & BUTTON_X     ) { JOY2_X     = TRUE; } else { JOY2_X     = FALSE; }
        if(JOY2 & BUTTON_Y     ) { JOY2_Y     = TRUE; } else { JOY2_Y     = FALSE; }
        if(JOY2 & BUTTON_Z     ) { JOY2_Z     = TRUE; } else { JOY2_Z     = FALSE; }
        if(JOY2 & BUTTON_START ) { JOY2_START = TRUE; } else { JOY2_START = FALSE; }
        if(JOY2 & BUTTON_MODE  ) { JOY2_MODE  = TRUE; } else { JOY2_MODE  = FALSE; }
    }
    else if (IAP2 == TRUE && gRoom == 10)
    {
        if (gPodeMover && P[1].energiaBase > 0 && P[2].energiaBase > 0)
        {
            //INTERLIGENCIA ARTIFICIAL DO P2

            //TEMPO PARA ATACAR O PLAYER 1
            if (tempoIAataque < tempoMaxIAataque[fase]) tempoIAataque++;
            else if (tempoIAataque == tempoMaxIAataque[fase]) tempoIAataque = 0;

            if (tempoIAmagia >= 1 && tempoIAmagia < 600) tempoIAmagia++;
            else if (tempoIAmagia >= 600) tempoIAmagia = 0;

            //ATIVA OU NAO O ATAQUE QUANDO A DISTANCIA ENTRE P1 E P2 ESTIVER MENOR QUE 130 PIXELS
            if (abs(P[1].x-P[2].x) < 150 /*P[1].x >= P[2].x-130 && P[1].x <= P[2].x+130*/ && tempoIAataque >= tempoMinIAataque[fase])
            {
                esmurra = random();
                esmurra = esmurra % 4;

                if (esmurra == 1)
                {
                    u16 chuteMurro;
                    chuteMurro = random();
                    chuteMurro = chuteMurro % 10;

                    JOY2_A = FALSE;
                    JOY2_B = FALSE;
                    JOY2_C = FALSE;
                    JOY2_X = FALSE;
                    JOY2_Y = FALSE;
                    JOY2_Z = FALSE;

                    u16 intensidadeGolpe;
                    intensidadeGolpe = random();
                    intensidadeGolpe = intensidadeGolpe % 12;

                    if (intensidadeGolpe <= 2)
                    {
                        if (chuteMurro <= 4) JOY2_A = TRUE;
                        else JOY2_X = TRUE;
                    }
                    else if (intensidadeGolpe >= 3 && intensidadeGolpe <= 6)
                    {
                        if (chuteMurro <= 4) JOY2_B = TRUE;
                        else JOY2_Y = TRUE;
                    }
                    else if (intensidadeGolpe >= 7 && intensidadeGolpe <= 9)
                    {
                        if (chuteMurro <= 4) JOY2_C = TRUE;
                        else JOY2_Z = TRUE;
                    }

                    tempoIAataque = 0;
                    tempoIA = 0;
                }
            }
            else if (abs(P[1].x-P[2].x) > 200 && tempoIAataque >= tempoMinIAataque[fase] && tempoIAmagia == 0)
            {
                if (P[2].y == gAlturaPiso)
                {
                    JOY2_A = FALSE;
                    JOY2_B = FALSE;
                    JOY2_C = FALSE;
                    JOY2_X = FALSE;
                    JOY2_Y = FALSE;
                    JOY2_Z = FALSE;

                    JOY2_X = TRUE;

					//MAGIAS!
					if(P[2].id == 1)
					{
						if(gPing10==0){ PLAYER_STATE(2,700); }
						if(gPing10 >0 && gPing10<= 2){ PLAYER_STATE(2,700); }
						if(gPing10>=3 && gPing10<= 4){ PLAYER_STATE(2,710); }
						if(gPing10>=5 && gPing10<= 6){ PLAYER_STATE(2,720); }
						if(gPing10>=7 && gPing10<= 8){ PLAYER_STATE(2,730); }
						if(gPing10>=9 && gPing10<=10){ PLAYER_STATE(2,700); }
					}else if(P[2].id == 2)
					{
						if(gPing10==0){ PLAYER_STATE(2,700); }
						if(gPing10 >0 && gPing10<= 2){ PLAYER_STATE(2,700); }
						if(gPing10>=3 && gPing10<= 4){ PLAYER_STATE(2,710); }
						if(gPing10>=5 && gPing10<= 6){ PLAYER_STATE(2,720); }
						if(gPing10>=7 && gPing10<= 8){ PLAYER_STATE(2,730); }
						if(gPing10>=9 && gPing10<=10){ PLAYER_STATE(2,700); }
					}else if(P[2].id == 3)
					{
						if(gPing10==0){ PLAYER_STATE(2,700); }
						if(gPing10 >0 && gPing10<= 2){ PLAYER_STATE(2,700); }
						if(gPing10>=3 && gPing10<= 4){ PLAYER_STATE(2,710); }
						if(gPing10>=5 && gPing10<= 6){ PLAYER_STATE(2,720); }
						if(gPing10>=7 && gPing10<= 8){ PLAYER_STATE(2,730); }
						if(gPing10>=9 && gPing10<=10){ PLAYER_STATE(2,700); }
					}else if(P[2].id == 7)
					{
						if(gPing10==0){ PLAYER_STATE(2,700); }
						if(gPing10 >0 && gPing10<= 2){ PLAYER_STATE(2,700); }
						if(gPing10>=3 && gPing10<= 4){ PLAYER_STATE(2,710); }
						if(gPing10>=5 && gPing10<= 6){ PLAYER_STATE(2,700); }
						if(gPing10>=7 && gPing10<= 8){ PLAYER_STATE(2,710); }
						if(gPing10>=9 && gPing10<=10){ PLAYER_STATE(2,700); }
					}else if(P[2].id ==12) //ryu
					{
						if(gPing10==0){ PLAYER_STATE(2,700); }
						if(gPing10 >0 && gPing10<= 2){ PLAYER_STATE(2,700); }
						if(gPing10>=3 && gPing10<= 4){ PLAYER_STATE(2,710); }
						if(gPing10>=5 && gPing10<= 6){ PLAYER_STATE(2,720); }
						if(gPing10>=7 && gPing10<= 8){ PLAYER_STATE(2,710); }
						if(gPing10>=9 && gPing10<=10){ PLAYER_STATE(2,700); }
					}
                }

                tempoIAataque = 0;
                tempoIA = 0;
                tempoIAmagia = 1;
            }

            if (tempoIA >= 1 && defendeTempo == 0)
            {
                tempoIA++;

                JOY2_RIGHT = FALSE;
                JOY2_LEFT = FALSE;
                JOY2_UP = FALSE;

                if (tempoIA > 10)
                {
                    tempoIA = 0;
                    acaoIA = 1;
                }
            }

            if (P[1].x >= P[2].x-160 && P[1].x <= P[2].x+160 && defendeTempo == 0)
            {
                if (JOY1_A || JOY1_B || JOY1_C || JOY1_X || JOY1_Y || JOY1_Z)
                {
                    u16 defendeGolpe;
                    defendeGolpe = random();
                    defendeGolpe = defendeGolpe % defesaIA[fase];

                    if (defendeGolpe < 10) defendeTempo = 1;
                }
            }

            if (P[1].fball.active
                && P[1].fball.x >= P[2].x-80 && P[1].fball.x <= P[2].x+80 && defendeTempo == 0)
            {
                u16 defendeGolpe;
                defendeGolpe = random();
                defendeGolpe = defendeGolpe % defesaIA[fase];

                if (defendeGolpe < 10) defendeTempo = 1;
            }

            if (defendeTempo)
            {
                tempoIA = 1;
                varIA = 5;
                acaoIA = 0;

                JOY2_UP = FALSE;

                defendeTempo++;

                if (P[1].x >= P[2].x)
                {
                    JOY2_LEFT = TRUE;
                    JOY2_RIGHT = FALSE;
                }
                else if (P[1].x < P[2].x)
                {
                    JOY2_LEFT = FALSE;
                    JOY2_RIGHT = TRUE;
                }

                if (defendeTempo >= 20)
                {
                    JOY2_LEFT = FALSE;
                    JOY2_RIGHT = FALSE;
                    defendeTempo = 0;
                }

            }

            if (tempoIA == 0)
            {
                if (acaoIA == 1)
                {
                    varIA = random();
                    varIA = varIA % 3;

                    tempoIAacao = 70;

                    acaoIA = 0;
                }

                if (tempoIAacao > 0)
                {
                    tempoIAacao--;

                    if (abs(P[1].x-P[2].x) > 200)
                    {
                        if (P[1].x < P[2].x && (varIA == 0 || varIA == 2))
                        {
                            tempoIAacao = 0;
                        }
                        else if (P[1].x > P[2].x && (varIA == 1 || varIA == 2))
                        {
                            tempoIAacao = 0;
                        }
                    }
                }
                else if (tempoIAacao == 0)
                {
                    tempoIA = 1;
                }

                u16 distanciaP1P2;
                distanciaP1P2 = abs(P[1].x-P[2].x);

                if (varIA == 0 && tempoIAacao > 0)
                {
                    JOY2_RIGHT = TRUE;
                    JOY2_LEFT = FALSE;
                }
                else if (varIA == 1 && tempoIAacao > 0)
                {
                    JOY2_RIGHT = FALSE;
                    JOY2_LEFT = TRUE;
                }
                else if (varIA == 2 && tempoIAacao > 0)
                {
                    JOY2_UP = TRUE;

                    pulandoFrenteTras = random();
                    pulandoFrenteTras = pulandoFrenteTras % 3;

                    if (pulandoFrenteTras == 0)
                    {
                        JOY2_RIGHT = TRUE;
                        JOY2_LEFT = FALSE;
                    }
                    else if (pulandoFrenteTras == 1)
                    {
                        JOY2_RIGHT = FALSE;
                        JOY2_LEFT = FALSE;
                    }
                    else if (pulandoFrenteTras == 2)
                    {
                        JOY2_RIGHT = FALSE;
                        JOY2_LEFT = TRUE;
                    }

                    if (distanciaP1P2 > 130 && P[1].x > P[2].x)
                    {
                        JOY2_RIGHT = TRUE;
                        JOY2_LEFT = FALSE;
                    }
                    else if (distanciaP1P2 > 130 && P[1].x < P[2].x)
                    {
                        JOY2_RIGHT = FALSE;
                        JOY2_LEFT = TRUE;
                    }
                }
            }
        }
        else if (gPodeMover && (P[1].energiaBase == 0 || P[2].energiaBase == 0))
        {
            JOY2_UP    = FALSE;
            JOY2_DOWN  = FALSE;
            JOY2_LEFT  = FALSE;
            JOY2_RIGHT = FALSE;
            JOY2_A     = FALSE;
            JOY2_B     = FALSE;
            JOY2_C     = FALSE;
            JOY2_X     = FALSE;
            JOY2_Y     = FALSE;
            JOY2_Z     = FALSE;
            JOY2_START = FALSE;
            JOY2_MODE  = FALSE;
        }
    }
	
	//---
	
	if(gPodeMover==0)
	{
		JOY1_UP    = FALSE; JOY2_UP    = FALSE;
		JOY1_DOWN  = FALSE; JOY2_DOWN  = FALSE;
		JOY1_LEFT  = FALSE; JOY2_LEFT  = FALSE;
		JOY1_RIGHT = FALSE; JOY2_RIGHT = FALSE;
		JOY1_A     = FALSE; JOY2_A     = FALSE;
		JOY1_B     = FALSE; JOY2_B     = FALSE;
		JOY1_C     = FALSE; JOY2_C     = FALSE;
		JOY1_X     = FALSE; JOY2_X     = FALSE;
		JOY1_Y     = FALSE; JOY2_Y     = FALSE;
		JOY1_Z     = FALSE; JOY2_Z     = FALSE;
		JOY1_START = FALSE; JOY2_START = FALSE;
		JOY1_MODE  = FALSE; JOY2_MODE  = FALSE;
	}
	
	//---P1
	
	if (JOY1_UP) {
	if (key_JOY1_UP_pressed==1 && key_JOY1_UP_hold==0) { key_JOY1_UP_hold=1; key_JOY1_UP_pressed=0; }
	if (key_JOY1_UP_pressed==0 && key_JOY1_UP_hold==0) { key_JOY1_UP_pressed=1; }}
	if (!JOY1_UP) {
	if (key_JOY1_UP_released==1) { key_JOY1_UP_released=0; key_JOY1_UP_pressed=0; key_JOY1_UP_hold=0; }
	if (key_JOY1_UP_pressed>0 || key_JOY1_UP_hold>0) {key_JOY1_UP_released=1;}}
	if (key_JOY1_UP_pressed==0 && key_JOY1_UP_hold==0 && key_JOY1_UP_released==0){ P[1].key_JOY_UP_status=0; }
	if (key_JOY1_UP_pressed  ==1 ) { P[1].key_JOY_UP_status=1; }
	if (key_JOY1_UP_hold     ==1 ) { P[1].key_JOY_UP_status=2; }
	if (key_JOY1_UP_released ==1 ) { P[1].key_JOY_UP_status=3; }
	
	if (JOY1_DOWN) {
	if (key_JOY1_DOWN_pressed==1 && key_JOY1_DOWN_hold==0) { key_JOY1_DOWN_hold=1; key_JOY1_DOWN_pressed=0; }
	if (key_JOY1_DOWN_pressed==0 && key_JOY1_DOWN_hold==0) { key_JOY1_DOWN_pressed=1; }}
	if (!JOY1_DOWN) {
	if (key_JOY1_DOWN_released==1) { key_JOY1_DOWN_released=0; key_JOY1_DOWN_pressed=0; key_JOY1_DOWN_hold=0; }
	if (key_JOY1_DOWN_pressed>0 || key_JOY1_DOWN_hold>0) {key_JOY1_DOWN_released=1;}}
	if (key_JOY1_DOWN_pressed==0 && key_JOY1_DOWN_hold==0 && key_JOY1_DOWN_released==0){ P[1].key_JOY_DOWN_status=0; }
	if (key_JOY1_DOWN_pressed  ==1 ) { P[1].key_JOY_DOWN_status=1; }
	if (key_JOY1_DOWN_hold     ==1 ) { P[1].key_JOY_DOWN_status=2; }
	if (key_JOY1_DOWN_released ==1 ) { P[1].key_JOY_DOWN_status=3; }
	
	if (JOY1_LEFT) {
	if (key_JOY1_LEFT_pressed==1 && key_JOY1_LEFT_hold==0) { key_JOY1_LEFT_hold=1; key_JOY1_LEFT_pressed=0; }
	if (key_JOY1_LEFT_pressed==0 && key_JOY1_LEFT_hold==0) { key_JOY1_LEFT_pressed=1; }}
	if (!JOY1_LEFT) {
	if (key_JOY1_LEFT_released==1) { key_JOY1_LEFT_released=0; key_JOY1_LEFT_pressed=0; key_JOY1_LEFT_hold=0; }
	if (key_JOY1_LEFT_pressed>0 || key_JOY1_LEFT_hold>0) {key_JOY1_LEFT_released=1;}}
	if (key_JOY1_LEFT_pressed==0 && key_JOY1_LEFT_hold==0 && key_JOY1_LEFT_released==0){ P[1].key_JOY_LEFT_status=0; }
	if (key_JOY1_LEFT_pressed  ==1 ) { P[1].key_JOY_LEFT_status=1; }
	if (key_JOY1_LEFT_hold     ==1 ) { P[1].key_JOY_LEFT_status=2; }
	if (key_JOY1_LEFT_released ==1 ) { P[1].key_JOY_LEFT_status=3; }
	
	if (JOY1_RIGHT) {
	if (key_JOY1_RIGHT_pressed==1 && key_JOY1_RIGHT_hold==0) { key_JOY1_RIGHT_hold=1; key_JOY1_RIGHT_pressed=0; }
	if (key_JOY1_RIGHT_pressed==0 && key_JOY1_RIGHT_hold==0) { key_JOY1_RIGHT_pressed=1; }}
	if (!JOY1_RIGHT) {
	if (key_JOY1_RIGHT_released==1) { key_JOY1_RIGHT_released=0; key_JOY1_RIGHT_pressed=0; key_JOY1_RIGHT_hold=0; }
	if (key_JOY1_RIGHT_pressed>0 || key_JOY1_RIGHT_hold>0) {key_JOY1_RIGHT_released=1;}}
	if (key_JOY1_RIGHT_pressed==0 && key_JOY1_RIGHT_hold==0 && key_JOY1_RIGHT_released==0){ P[1].key_JOY_RIGHT_status=0; }
	if (key_JOY1_RIGHT_pressed  ==1 ) { P[1].key_JOY_RIGHT_status=1; }
	if (key_JOY1_RIGHT_hold     ==1 ) { P[1].key_JOY_RIGHT_status=2; }
	if (key_JOY1_RIGHT_released ==1 ) { P[1].key_JOY_RIGHT_status=3; }
	
	if (JOY1_A) {
	if (key_JOY1_A_pressed==1 && key_JOY1_A_hold==0) { key_JOY1_A_hold=1; key_JOY1_A_pressed=0; }
	if (key_JOY1_A_pressed==0 && key_JOY1_A_hold==0) { key_JOY1_A_pressed=1; }}
	if (!JOY1_A) {
	if (key_JOY1_A_released==1) { key_JOY1_A_released=0; key_JOY1_A_pressed=0; key_JOY1_A_hold=0; }
	if (key_JOY1_A_pressed>0 || key_JOY1_A_hold>0) {key_JOY1_A_released=1;}}
	if (key_JOY1_A_pressed==0 && key_JOY1_A_hold==0 && key_JOY1_A_released==0){ P[1].key_JOY_A_status=0; }
	if (key_JOY1_A_pressed  ==1 ) { P[1].key_JOY_A_status=1; }
	if (key_JOY1_A_hold     ==1 ) { P[1].key_JOY_A_status=2; }
	if (key_JOY1_A_released ==1 ) { P[1].key_JOY_A_status=3; }
	
	if (JOY1_B) {
	if (key_JOY1_B_pressed==1 && key_JOY1_B_hold==0) { key_JOY1_B_hold=1; key_JOY1_B_pressed=0; }
	if (key_JOY1_B_pressed==0 && key_JOY1_B_hold==0) { key_JOY1_B_pressed=1; }}
	if (!JOY1_B) {
	if (key_JOY1_B_released==1) { key_JOY1_B_released=0; key_JOY1_B_pressed=0; key_JOY1_B_hold=0; }
	if (key_JOY1_B_pressed>0 || key_JOY1_B_hold>0) {key_JOY1_B_released=1;}}
	if (key_JOY1_B_pressed==0 && key_JOY1_B_hold==0 && key_JOY1_B_released==0){ P[1].key_JOY_B_status=0; }
	if (key_JOY1_B_pressed  ==1 ) { P[1].key_JOY_B_status=1; }
	if (key_JOY1_B_hold     ==1 ) { P[1].key_JOY_B_status=2; }
	if (key_JOY1_B_released ==1 ) { P[1].key_JOY_B_status=3; }
	
	if (JOY1_C) {
	if (key_JOY1_C_pressed==1 && key_JOY1_C_hold==0) { key_JOY1_C_hold=1; key_JOY1_C_pressed=0; }
	if (key_JOY1_C_pressed==0 && key_JOY1_C_hold==0) { key_JOY1_C_pressed=1; }}
	if (!JOY1_C) {
	if (key_JOY1_C_released==1) { key_JOY1_C_released=0; key_JOY1_C_pressed=0; key_JOY1_C_hold=0; }
	if (key_JOY1_C_pressed>0 || key_JOY1_C_hold>0) {key_JOY1_C_released=1;}}
	if (key_JOY1_C_pressed==0 && key_JOY1_C_hold==0 && key_JOY1_C_released==0){ P[1].key_JOY_C_status=0; }
	if (key_JOY1_C_pressed  ==1 ) { P[1].key_JOY_C_status=1; }
	if (key_JOY1_C_hold     ==1 ) { P[1].key_JOY_C_status=2; }
	if (key_JOY1_C_released ==1 ) { P[1].key_JOY_C_status=3; }
	
	if (JOY1_X) {
	if (key_JOY1_X_pressed==1 && key_JOY1_X_hold==0) { key_JOY1_X_hold=1; key_JOY1_X_pressed=0; }
	if (key_JOY1_X_pressed==0 && key_JOY1_X_hold==0) { key_JOY1_X_pressed=1; }}
	if (!JOY1_X) {
	if (key_JOY1_X_released==1) { key_JOY1_X_released=0; key_JOY1_X_pressed=0; key_JOY1_X_hold=0; }
	if (key_JOY1_X_pressed>0 || key_JOY1_X_hold>0) {key_JOY1_X_released=1;}}
	if (key_JOY1_X_pressed==0 && key_JOY1_X_hold==0 && key_JOY1_X_released==0){ P[1].key_JOY_X_status=0; }
	if (key_JOY1_X_pressed  ==1 ) { P[1].key_JOY_X_status=1; }
	if (key_JOY1_X_hold     ==1 ) { P[1].key_JOY_X_status=2; }
	if (key_JOY1_X_released ==1 ) { P[1].key_JOY_X_status=3; }
	
	if (JOY1_Y) {
	if (key_JOY1_Y_pressed==1 && key_JOY1_Y_hold==0) { key_JOY1_Y_hold=1; key_JOY1_Y_pressed=0; }
	if (key_JOY1_Y_pressed==0 && key_JOY1_Y_hold==0) { key_JOY1_Y_pressed=1; }}
	if (!JOY1_Y) {
	if (key_JOY1_Y_released==1) { key_JOY1_Y_released=0; key_JOY1_Y_pressed=0; key_JOY1_Y_hold=0; }
	if (key_JOY1_Y_pressed>0 || key_JOY1_Y_hold>0) {key_JOY1_Y_released=1;}}
	if (key_JOY1_Y_pressed==0 && key_JOY1_Y_hold==0 && key_JOY1_Y_released==0){ P[1].key_JOY_Y_status=0; }
	if (key_JOY1_Y_pressed  ==1 ) { P[1].key_JOY_Y_status=1; }
	if (key_JOY1_Y_hold     ==1 ) { P[1].key_JOY_Y_status=2; }
	if (key_JOY1_Y_released ==1 ) { P[1].key_JOY_Y_status=3; }
	
	if (JOY1_Z) {
	if (key_JOY1_Z_pressed==1 && key_JOY1_Z_hold==0) { key_JOY1_Z_hold=1; key_JOY1_Z_pressed=0; }
	if (key_JOY1_Z_pressed==0 && key_JOY1_Z_hold==0) { key_JOY1_Z_pressed=1; }}
	if (!JOY1_Z) {
	if (key_JOY1_Z_released==1) { key_JOY1_Z_released=0; key_JOY1_Z_pressed=0; key_JOY1_Z_hold=0; }
	if (key_JOY1_Z_pressed>0 || key_JOY1_Z_hold>0) {key_JOY1_Z_released=1;}}
	if (key_JOY1_Z_pressed==0 && key_JOY1_Z_hold==0 && key_JOY1_Z_released==0){ P[1].key_JOY_Z_status=0; }
	if (key_JOY1_Z_pressed  ==1 ) { P[1].key_JOY_Z_status=1; }
	if (key_JOY1_Z_hold     ==1 ) { P[1].key_JOY_Z_status=2; }
	if (key_JOY1_Z_released ==1 ) { P[1].key_JOY_Z_status=3; }
	
	if (JOY1_START) {
	if (key_JOY1_START_pressed==1 && key_JOY1_START_hold==0) { key_JOY1_START_hold=1; key_JOY1_START_pressed=0; }
	if (key_JOY1_START_pressed==0 && key_JOY1_START_hold==0) { key_JOY1_START_pressed=1; }}
	if (!JOY1_START) {
	if (key_JOY1_START_released==1) { key_JOY1_START_released=0; key_JOY1_START_pressed=0; key_JOY1_START_hold=0; }
	if (key_JOY1_START_pressed>0 || key_JOY1_START_hold>0) {key_JOY1_START_released=1;}}
	if (key_JOY1_START_pressed==0 && key_JOY1_START_hold==0 && key_JOY1_START_released==0){ P[1].key_JOY_START_status=0; }
	if (key_JOY1_START_pressed  ==1 ) { P[1].key_JOY_START_status=1; }
	if (key_JOY1_START_hold     ==1 ) { P[1].key_JOY_START_status=2; }
	if (key_JOY1_START_released ==1 ) { P[1].key_JOY_START_status=3; }
	
	if (JOY1_MODE) {
	if (key_JOY1_MODE_pressed==1 && key_JOY1_MODE_hold==0) { key_JOY1_MODE_hold=1; key_JOY1_MODE_pressed=0; }
	if (key_JOY1_MODE_pressed==0 && key_JOY1_MODE_hold==0) { key_JOY1_MODE_pressed=1; }}
	if (!JOY1_MODE) {
	if (key_JOY1_MODE_released==1) { key_JOY1_MODE_released=0; key_JOY1_MODE_pressed=0; key_JOY1_MODE_hold=0; }
	if (key_JOY1_MODE_pressed>0 || key_JOY1_MODE_hold>0) {key_JOY1_MODE_released=1;}}
	if (key_JOY1_MODE_pressed==0 && key_JOY1_MODE_hold==0 && key_JOY1_MODE_released==0){ P[1].key_JOY_MODE_status=0; }
	if (key_JOY1_MODE_pressed  ==1 ) { P[1].key_JOY_MODE_status=1; }
	if (key_JOY1_MODE_hold     ==1 ) { P[1].key_JOY_MODE_status=2; }
	if (key_JOY1_MODE_released ==1 ) { P[1].key_JOY_MODE_status=3; }
	
	//---P2
	
	if (JOY2_UP) {
	if (key_JOY2_UP_pressed==1 && key_JOY2_UP_hold==0) { key_JOY2_UP_hold=1; key_JOY2_UP_pressed=0; }
	if (key_JOY2_UP_pressed==0 && key_JOY2_UP_hold==0) { key_JOY2_UP_pressed=1; }}
	if (!JOY2_UP) {
	if (key_JOY2_UP_released==1) { key_JOY2_UP_released=0; key_JOY2_UP_pressed=0; key_JOY2_UP_hold=0; }
	if (key_JOY2_UP_pressed>0 || key_JOY2_UP_hold>0) {key_JOY2_UP_released=1;}}
	if (key_JOY2_UP_pressed==0 && key_JOY2_UP_hold==0 && key_JOY2_UP_released==0){ P[2].key_JOY_UP_status=0; }
	if (key_JOY2_UP_pressed  ==1 ) { P[2].key_JOY_UP_status=1; }
	if (key_JOY2_UP_hold     ==1 ) { P[2].key_JOY_UP_status=2; }
	if (key_JOY2_UP_released ==1 ) { P[2].key_JOY_UP_status=3; }
	
	if (JOY2_DOWN) {
	if (key_JOY2_DOWN_pressed==1 && key_JOY2_DOWN_hold==0) { key_JOY2_DOWN_hold=1; key_JOY2_DOWN_pressed=0; }
	if (key_JOY2_DOWN_pressed==0 && key_JOY2_DOWN_hold==0) { key_JOY2_DOWN_pressed=1; }}
	if (!JOY2_DOWN) {
	if (key_JOY2_DOWN_released==1) { key_JOY2_DOWN_released=0; key_JOY2_DOWN_pressed=0; key_JOY2_DOWN_hold=0; }
	if (key_JOY2_DOWN_pressed>0 || key_JOY2_DOWN_hold>0) {key_JOY2_DOWN_released=1;}}
	if (key_JOY2_DOWN_pressed==0 && key_JOY2_DOWN_hold==0 && key_JOY2_DOWN_released==0){ P[2].key_JOY_DOWN_status=0; }
	if (key_JOY2_DOWN_pressed  ==1 ) { P[2].key_JOY_DOWN_status=1; }
	if (key_JOY2_DOWN_hold     ==1 ) { P[2].key_JOY_DOWN_status=2; }
	if (key_JOY2_DOWN_released ==1 ) { P[2].key_JOY_DOWN_status=3; }
	
	if (JOY2_LEFT) {
	if (key_JOY2_LEFT_pressed==1 && key_JOY2_LEFT_hold==0) { key_JOY2_LEFT_hold=1; key_JOY2_LEFT_pressed=0; }
	if (key_JOY2_LEFT_pressed==0 && key_JOY2_LEFT_hold==0) { key_JOY2_LEFT_pressed=1; }}
	if (!JOY2_LEFT) {
	if (key_JOY2_LEFT_released==1) { key_JOY2_LEFT_released=0; key_JOY2_LEFT_pressed=0; key_JOY2_LEFT_hold=0; }
	if (key_JOY2_LEFT_pressed>0 || key_JOY2_LEFT_hold>0) {key_JOY2_LEFT_released=1;}}
	if (key_JOY2_LEFT_pressed==0 && key_JOY2_LEFT_hold==0 && key_JOY2_LEFT_released==0){ P[2].key_JOY_LEFT_status=0; }
	if (key_JOY2_LEFT_pressed  ==1 ) { P[2].key_JOY_LEFT_status=1; }
	if (key_JOY2_LEFT_hold     ==1 ) { P[2].key_JOY_LEFT_status=2; }
	if (key_JOY2_LEFT_released ==1 ) { P[2].key_JOY_LEFT_status=3; }
	
	if (JOY2_RIGHT) {
	if (key_JOY2_RIGHT_pressed==1 && key_JOY2_RIGHT_hold==0) { key_JOY2_RIGHT_hold=1; key_JOY2_RIGHT_pressed=0; }
	if (key_JOY2_RIGHT_pressed==0 && key_JOY2_RIGHT_hold==0) { key_JOY2_RIGHT_pressed=1; }}
	if (!JOY2_RIGHT) {
	if (key_JOY2_RIGHT_released==1) { key_JOY2_RIGHT_released=0; key_JOY2_RIGHT_pressed=0; key_JOY2_RIGHT_hold=0; }
	if (key_JOY2_RIGHT_pressed>0 || key_JOY2_RIGHT_hold>0) {key_JOY2_RIGHT_released=1;}}
	if (key_JOY2_RIGHT_pressed==0 && key_JOY2_RIGHT_hold==0 && key_JOY2_RIGHT_released==0){ P[2].key_JOY_RIGHT_status=0; }
	if (key_JOY2_RIGHT_pressed  ==1 ) { P[2].key_JOY_RIGHT_status=1; }
	if (key_JOY2_RIGHT_hold     ==1 ) { P[2].key_JOY_RIGHT_status=2; }
	if (key_JOY2_RIGHT_released ==1 ) { P[2].key_JOY_RIGHT_status=3; }
	
	if (JOY2_A) {
	if (key_JOY2_A_pressed==1 && key_JOY2_A_hold==0) { key_JOY2_A_hold=1; key_JOY2_A_pressed=0; }
	if (key_JOY2_A_pressed==0 && key_JOY2_A_hold==0) { key_JOY2_A_pressed=1; }}
	if (!JOY2_A) {
	if (key_JOY2_A_released==1) { key_JOY2_A_released=0; key_JOY2_A_pressed=0; key_JOY2_A_hold=0; }
	if (key_JOY2_A_pressed>0 || key_JOY2_A_hold>0) {key_JOY2_A_released=1;}}
	if (key_JOY2_A_pressed==0 && key_JOY2_A_hold==0 && key_JOY2_A_released==0){ P[2].key_JOY_A_status=0; }
	if (key_JOY2_A_pressed  ==1 ) { P[2].key_JOY_A_status=1; }
	if (key_JOY2_A_hold     ==1 ) { P[2].key_JOY_A_status=2; }
	if (key_JOY2_A_released ==1 ) { P[2].key_JOY_A_status=3; }
	
	if (JOY2_B) {
	if (key_JOY2_B_pressed==1 && key_JOY2_B_hold==0) { key_JOY2_B_hold=1; key_JOY2_B_pressed=0; }
	if (key_JOY2_B_pressed==0 && key_JOY2_B_hold==0) { key_JOY2_B_pressed=1; }}
	if (!JOY2_B) {
	if (key_JOY2_B_released==1) { key_JOY2_B_released=0; key_JOY2_B_pressed=0; key_JOY2_B_hold=0; }
	if (key_JOY2_B_pressed>0 || key_JOY2_B_hold>0) {key_JOY2_B_released=1;}}
	if (key_JOY2_B_pressed==0 && key_JOY2_B_hold==0 && key_JOY2_B_released==0){ P[2].key_JOY_B_status=0; }
	if (key_JOY2_B_pressed  ==1 ) { P[2].key_JOY_B_status=1; }
	if (key_JOY2_B_hold     ==1 ) { P[2].key_JOY_B_status=2; }
	if (key_JOY2_B_released ==1 ) { P[2].key_JOY_B_status=3; }
	
	if (JOY2_C) {
	if (key_JOY2_C_pressed==1 && key_JOY2_C_hold==0) { key_JOY2_C_hold=1; key_JOY2_C_pressed=0; }
	if (key_JOY2_C_pressed==0 && key_JOY2_C_hold==0) { key_JOY2_C_pressed=1; }}
	if (!JOY2_C) {
	if (key_JOY2_C_released==1) { key_JOY2_C_released=0; key_JOY2_C_pressed=0; key_JOY2_C_hold=0; }
	if (key_JOY2_C_pressed>0 || key_JOY2_C_hold>0) {key_JOY2_C_released=1;}}
	if (key_JOY2_C_pressed==0 && key_JOY2_C_hold==0 && key_JOY2_C_released==0){ P[2].key_JOY_C_status=0; }
	if (key_JOY2_C_pressed  ==1 ) { P[2].key_JOY_C_status=1; }
	if (key_JOY2_C_hold     ==1 ) { P[2].key_JOY_C_status=2; }
	if (key_JOY2_C_released ==1 ) { P[2].key_JOY_C_status=3; }
	
	if (JOY2_X) {
	if (key_JOY2_X_pressed==1 && key_JOY2_X_hold==0) { key_JOY2_X_hold=1; key_JOY2_X_pressed=0; }
	if (key_JOY2_X_pressed==0 && key_JOY2_X_hold==0) { key_JOY2_X_pressed=1; }}
	if (!JOY2_X) {
	if (key_JOY2_X_released==1) { key_JOY2_X_released=0; key_JOY2_X_pressed=0; key_JOY2_X_hold=0; }
	if (key_JOY2_X_pressed>0 || key_JOY2_X_hold>0) {key_JOY2_X_released=1;}}
	if (key_JOY2_X_pressed==0 && key_JOY2_X_hold==0 && key_JOY2_X_released==0){ P[2].key_JOY_X_status=0; }
	if (key_JOY2_X_pressed  ==1 ) { P[2].key_JOY_X_status=1; }
	if (key_JOY2_X_hold     ==1 ) { P[2].key_JOY_X_status=2; }
	if (key_JOY2_X_released ==1 ) { P[2].key_JOY_X_status=3; }
	
	if (JOY2_Y) {
	if (key_JOY2_Y_pressed==1 && key_JOY2_Y_hold==0) { key_JOY2_Y_hold=1; key_JOY2_Y_pressed=0; }
	if (key_JOY2_Y_pressed==0 && key_JOY2_Y_hold==0) { key_JOY2_Y_pressed=1; }}
	if (!JOY2_Y) {
	if (key_JOY2_Y_released==1) { key_JOY2_Y_released=0; key_JOY2_Y_pressed=0; key_JOY2_Y_hold=0; }
	if (key_JOY2_Y_pressed>0 || key_JOY2_Y_hold>0) {key_JOY2_Y_released=1;}}
	if (key_JOY2_Y_pressed==0 && key_JOY2_Y_hold==0 && key_JOY2_Y_released==0){ P[2].key_JOY_Y_status=0; }
	if (key_JOY2_Y_pressed  ==1 ) { P[2].key_JOY_Y_status=1; }
	if (key_JOY2_Y_hold     ==1 ) { P[2].key_JOY_Y_status=2; }
	if (key_JOY2_Y_released ==1 ) { P[2].key_JOY_Y_status=3; }
	
	if (JOY2_Z) {
	if (key_JOY2_Z_pressed==1 && key_JOY2_Z_hold==0) { key_JOY2_Z_hold=1; key_JOY2_Z_pressed=0; }
	if (key_JOY2_Z_pressed==0 && key_JOY2_Z_hold==0) { key_JOY2_Z_pressed=1; }}
	if (!JOY2_Z) {
	if (key_JOY2_Z_released==1) { key_JOY2_Z_released=0; key_JOY2_Z_pressed=0; key_JOY2_Z_hold=0; }
	if (key_JOY2_Z_pressed>0 || key_JOY2_Z_hold>0) {key_JOY2_Z_released=1;}}
	if (key_JOY2_Z_pressed==0 && key_JOY2_Z_hold==0 && key_JOY2_Z_released==0){ P[2].key_JOY_Z_status=0; }
	if (key_JOY2_Z_pressed  ==1 ) { P[2].key_JOY_Z_status=1; }
	if (key_JOY2_Z_hold     ==1 ) { P[2].key_JOY_Z_status=2; }
	if (key_JOY2_Z_released ==1 ) { P[2].key_JOY_Z_status=3; }
	
	if (JOY2_START) {
	if (key_JOY2_START_pressed==1 && key_JOY2_START_hold==0) { key_JOY2_START_hold=1; key_JOY2_START_pressed=0; }
	if (key_JOY2_START_pressed==0 && key_JOY2_START_hold==0) { key_JOY2_START_pressed=1; }}
	if (!JOY2_START) {
	if (key_JOY2_START_released==1) { key_JOY2_START_released=0; key_JOY2_START_pressed=0; key_JOY2_START_hold=0; }
	if (key_JOY2_START_pressed>0 || key_JOY2_START_hold>0) {key_JOY2_START_released=1;}}
	if (key_JOY2_START_pressed==0 && key_JOY2_START_hold==0 && key_JOY2_START_released==0){ P[2].key_JOY_START_status=0; }
	if (key_JOY2_START_pressed  ==1 ) { P[2].key_JOY_START_status=1; }
	if (key_JOY2_START_hold     ==1 ) { P[2].key_JOY_START_status=2; }
	if (key_JOY2_START_released ==1 ) { P[2].key_JOY_START_status=3; }
	
	if (JOY2_MODE) {
	if (key_JOY2_MODE_pressed==1 && key_JOY2_MODE_hold==0) { key_JOY2_MODE_hold=1; key_JOY2_MODE_pressed=0; }
	if (key_JOY2_MODE_pressed==0 && key_JOY2_MODE_hold==0) { key_JOY2_MODE_pressed=1; }}
	if (!JOY2_MODE) {
	if (key_JOY2_MODE_released==1) { key_JOY2_MODE_released=0; key_JOY2_MODE_pressed=0; key_JOY2_MODE_hold=0; }
	if (key_JOY2_MODE_pressed>0 || key_JOY2_MODE_hold>0) {key_JOY2_MODE_released=1;}}
	if (key_JOY2_MODE_pressed==0 && key_JOY2_MODE_hold==0 && key_JOY2_MODE_released==0){ P[2].key_JOY_MODE_status=0; }
	if (key_JOY2_MODE_pressed  ==1 ) { P[2].key_JOY_MODE_status=1; }
	if (key_JOY2_MODE_hold     ==1 ) { P[2].key_JOY_MODE_status=2; }
	if (key_JOY2_MODE_released ==1 ) { P[2].key_JOY_MODE_status=3; }
	
	//---InputTimerCountDown; InputArray
	u8 dir=0;
	
	if(P[1].key_JOY_UP_status   ==1){ P[1].joyDirTimer[8]=gInputTimerCountDown+1; dir=8; }
	if(P[1].key_JOY_DOWN_status ==1){ P[1].joyDirTimer[2]=gInputTimerCountDown+1; dir=2; }
	if(P[1].key_JOY_LEFT_status ==1){ P[1].joyDirTimer[4]=gInputTimerCountDown+1; dir=4; }
	if(P[1].key_JOY_RIGHT_status==1){ P[1].joyDirTimer[6]=gInputTimerCountDown+1; dir=6; }
	
	if(dir!=0)
	{
		P[1].inputArray[4] = P[1].inputArray[3];
		P[1].inputArray[3] = P[1].inputArray[2];
		P[1].inputArray[2] = P[1].inputArray[1];
		P[1].inputArray[1] = P[1].inputArray[0];
		P[1].inputArray[0] = dir;
	}
	
	dir=0;
	
	if(P[2].key_JOY_UP_status   ==1){ P[2].joyDirTimer[8]=gInputTimerCountDown+1; dir=8; }
	if(P[2].key_JOY_DOWN_status ==1){ P[2].joyDirTimer[2]=gInputTimerCountDown+1; dir=2; }
	if(P[2].key_JOY_LEFT_status ==1){ P[2].joyDirTimer[4]=gInputTimerCountDown+1; dir=4; }
	if(P[2].key_JOY_RIGHT_status==1){ P[2].joyDirTimer[6]=gInputTimerCountDown+1; dir=6; }
	
	if(dir!=0)
	{
		P[2].inputArray[4] = P[2].inputArray[3];
		P[2].inputArray[3] = P[2].inputArray[2];
		P[2].inputArray[2] = P[2].inputArray[1];
		P[2].inputArray[1] = P[2].inputArray[0];
		P[2].inputArray[0] = dir;
	}
	
	if(P[1].hitPause==0)
	{
		if( P[1].joyDirTimer[8]>0 ){ P[1].joyDirTimer[8]--; }
		if( P[1].joyDirTimer[2]>0 ){ P[1].joyDirTimer[2]--; }
		if( P[1].joyDirTimer[4]>0 ){ P[1].joyDirTimer[4]--; }
		if( P[1].joyDirTimer[6]>0 ){ P[1].joyDirTimer[6]--; }	
	}
	
	if(P[2].hitPause==0)
	{
		if( P[2].joyDirTimer[8]>0 ){ P[2].joyDirTimer[8]--; }
		if( P[2].joyDirTimer[2]>0 ){ P[2].joyDirTimer[2]--; }
		if( P[2].joyDirTimer[4]>0 ){ P[2].joyDirTimer[4]--; }
		if( P[2].joyDirTimer[6]>0 ){ P[2].joyDirTimer[6]--; }	
	}
	
	//---AttackPower & AttackButton
	//P[1].attackPower =0;
	P[1].attackButton=0;
	//P[2].attackPower =0;
	P[2].attackButton=0;
	
	if(P[1].key_JOY_A_status==1){ P[1].attackPower=1; P[1].attackButton=4; }
	if(P[1].key_JOY_B_status==1){ P[1].attackPower=2; P[1].attackButton=5; }
	if(P[1].key_JOY_C_status==1){ /*P[1].attackPower=3;*/ P[1].attackButton=6; }
	if(P[1].key_JOY_X_status==1){ P[1].attackPower=1; P[1].attackButton=1; }
	if(P[1].key_JOY_Y_status==1){ P[1].attackPower=2; P[1].attackButton=2; }
	if(P[1].key_JOY_Z_status==1){ /*P[1].attackPower=3;*/ P[1].attackButton=3; }
	
	if(P[2].key_JOY_A_status==1){ P[2].attackPower=1; P[2].attackButton=4; }
	if(P[2].key_JOY_B_status==1){ P[2].attackPower=2; P[2].attackButton=5; }
	if(P[2].key_JOY_C_status==1){ /*P[2].attackPower=3;*/ P[2].attackButton=6; }
	if(P[2].key_JOY_X_status==1){ P[2].attackPower=1; P[2].attackButton=1; }
	if(P[2].key_JOY_Y_status==1){ P[2].attackPower=2; P[2].attackButton=2; }
	if(P[2].key_JOY_Z_status==1){ /*P[2].attackPower=3;*/ P[2].attackButton=3; }
	//---
	if((P[1].key_JOY_MODE_status>=1 && P[1].key_JOY_START_status==1) && gDebug==0) //Ativa o Debug com P1_MODE
	{ 
		gDebug=1; 
		P[1].key_JOY_MODE_status=0; 
		P[1].key_JOY_START_status=0; 
		if(RELEASE==0)
		{
			SPR_setVisibility(GE[1].sprite, VISIBLE);
			SPR_setVisibility(GE[2].sprite, VISIBLE);
		}
	} 
	if((P[1].key_JOY_MODE_status>=1 && P[1].key_JOY_START_status==1) && gDebug==1) //Desativa o Debug com P1_MODE
	{ 
		gDebug=0; 
		P[1].key_JOY_MODE_status=0; 
		P[1].key_JOY_START_status=0; 
		VDP_drawText("                              ", 1, 1);
		VDP_drawText("                              ", 1, 2);
		VDP_drawText("                              ", 1, 3);
		if(RELEASE==0)
		{
			SPR_setPosition(Rect1BB1_Q1,-8,-8); SPR_setPosition(Rect1BB1_Q2,-8,-8); SPR_setPosition(Rect1BB1_Q3,-8,-8); SPR_setPosition(Rect1BB1_Q4,-8,-8);
			SPR_setPosition(Rect1HB1_Q1,-8,-8); SPR_setPosition(Rect1HB1_Q2,-8,-8); SPR_setPosition(Rect1HB1_Q3,-8,-8); SPR_setPosition(Rect1HB1_Q4,-8,-8);
			SPR_setPosition(Rect2BB1_Q1,-8,-8); SPR_setPosition(Rect2BB1_Q2,-8,-8); SPR_setPosition(Rect2BB1_Q3,-8,-8); SPR_setPosition(Rect2BB1_Q4,-8,-8);
			SPR_setPosition(Rect2HB1_Q1,-8,-8); SPR_setPosition(Rect2HB1_Q2,-8,-8); SPR_setPosition(Rect2HB1_Q3,-8,-8); SPR_setPosition(Rect2HB1_Q4,-8,-8);
		}
		if(RELEASE==0)
		{
			SPR_setVisibility(GE[1].sprite, HIDDEN);
			SPR_setVisibility(GE[2].sprite, HIDDEN);
		}
	} 
	//---
	
	// Troca de Paleta
	if(P[1].key_JOY_MODE_status==1)
	{
		if( (P[1].id==P[2].id) && P[1].palID==P[2].palID-1){ P[1].palID++; } //evita paleta repetida in game
		
		if(P[1].id==1) //haohmaru
		{
			/*
			if(      P[1].palID==1){ PAL_setPalette(PAL2, spr_haohmaru_pal2_1a.palette->data, CPU); P[1].palID=2; 
			}else if(P[1].palID==2){ PAL_setPalette(PAL2, spr_haohmaru_pal3_1a.palette->data, CPU); P[1].palID=3; 
			}else if(P[1].palID==3){ PAL_setPalette(PAL2, spr_haohmaru_pal4_1a.palette->data, CPU); P[1].palID=4; 
			}else if(P[1].palID==4){ PAL_setPalette(PAL2, spr_haohmaru_pal5_1a.palette->data, CPU); P[1].palID=5; 
			}else if(P[1].palID==5){ PAL_setPalette(PAL2, spr_haohmaru_pal6_1a.palette->data, CPU); P[1].palID=6; 
			}else if(P[1].palID==6){ PAL_setPalette(PAL2, spr_haohmaru_pal7_1a.palette->data, CPU); P[1].palID=7; 
			}else if(P[1].palID==7){ PAL_setPalette(PAL2, spr_haohmaru_pal8_1a.palette->data, CPU); P[1].palID=8; 
			}else if(P[1].palID==8){ PAL_setPalette(PAL2, spr_haohmaru_pal1_1a.palette->data, CPU); P[1].palID=1; 
			}
			*/
		}
		if(P[1].id==2) //gillius
		{
			/*
			if(      P[1].palID==1){ PAL_setPalette(PAL2, spr_gillius_pal2.palette->data, CPU); P[1].palID=2; 
			}else if(P[1].palID==2){ PAL_setPalette(PAL2, spr_gillius_pal3.palette->data, CPU); P[1].palID=3;
			}else if(P[1].palID==3){ PAL_setPalette(PAL2, spr_gillius_pal4.palette->data, CPU); P[1].palID=4;
			}else if(P[1].palID==4){ PAL_setPalette(PAL2, spr_gillius_pal5.palette->data, CPU); P[1].palID=5;
			}else if(P[1].palID==5){ PAL_setPalette(PAL2, spr_gillius_pal6.palette->data, CPU); P[1].palID=6;
			}else if(P[1].palID==6){ PAL_setPalette(PAL2, spr_gillius_pal7.palette->data, CPU); P[1].palID=7;
			}else if(P[1].palID==7){ PAL_setPalette(PAL2, spr_gillius_pal8.palette->data, CPU); P[1].palID=8;
			}else if(P[1].palID==8){ PAL_setPalette(PAL2, spr_gillius_pal1.palette->data, CPU); P[1].palID=1;
			}
			*/
		}
		
	}
	if(P[2].key_JOY_MODE_status==1)
	{
		if( (P[1].id==P[2].id) && P[2].palID==P[1].palID-1){ P[2].palID++; } //evita paleta repetida in game
		
		if(P[2].id==1) //haohmaru
		{
			/*
			if(      P[2].palID==1){ PAL_setPalette(PAL3, spr_haohmaru_pal2_1a.palette->data, CPU); P[2].palID=2; 
			}else if(P[2].palID==2){ PAL_setPalette(PAL3, spr_haohmaru_pal3_1a.palette->data, CPU); P[2].palID=3; 
			}else if(P[2].palID==3){ PAL_setPalette(PAL3, spr_haohmaru_pal4_1a.palette->data, CPU); P[2].palID=4; 
			}else if(P[2].palID==4){ PAL_setPalette(PAL3, spr_haohmaru_pal5_1a.palette->data, CPU); P[2].palID=5; 
			}else if(P[2].palID==5){ PAL_setPalette(PAL3, spr_haohmaru_pal6_1a.palette->data, CPU); P[2].palID=6; 
			}else if(P[2].palID==6){ PAL_setPalette(PAL3, spr_haohmaru_pal7_1a.palette->data, CPU); P[2].palID=7; 
			}else if(P[2].palID==7){ PAL_setPalette(PAL3, spr_haohmaru_pal8_1a.palette->data, CPU); P[2].palID=8; 
			}else if(P[2].palID==8){ PAL_setPalette(PAL3, spr_haohmaru_pal1_1a.palette->data, CPU); P[2].palID=1; 
			}
			*/
		}
		if(P[2].id==2) //gillius
		{
			/*
			if(      P[2].palID==1){ PAL_setPalette(PAL3, spr_gillius_pal2.palette->data, CPU); P[2].palID=2; 
			}else if(P[2].palID==2){ PAL_setPalette(PAL3, spr_gillius_pal3.palette->data, CPU); P[2].palID=3;
			}else if(P[2].palID==3){ PAL_setPalette(PAL3, spr_gillius_pal4.palette->data, CPU); P[2].palID=4;
			}else if(P[2].palID==4){ PAL_setPalette(PAL3, spr_gillius_pal5.palette->data, CPU); P[2].palID=5;
			}else if(P[2].palID==5){ PAL_setPalette(PAL3, spr_gillius_pal6.palette->data, CPU); P[2].palID=6;
			}else if(P[2].palID==6){ PAL_setPalette(PAL3, spr_gillius_pal7.palette->data, CPU); P[2].palID=7;
			}else if(P[2].palID==7){ PAL_setPalette(PAL3, spr_gillius_pal8.palette->data, CPU); P[2].palID=8;
			}else if(P[2].palID==8){ PAL_setPalette(PAL3, spr_gillius_pal1.palette->data, CPU); P[2].palID=1;
			}
			*/
		}
	}
}

void FUNCAO_FSM_DEFENSE(u8 player, u8 enemy){

	//O calculo abaixo eh usado para defender magias :)
	u8 distancia_Player_X_Magia = 250;
	if(P[enemy].fball.active)
	{
		if(P[player].direcao == 1){
			distancia_Player_X_Magia = P[enemy].fball.x - P[player].x + 25;
		}
		if(P[player].direcao ==-1){
			distancia_Player_X_Magia = P[player].x - P[enemy].fball.x;
		}
	}
	
	//defesa start - EM PE e ABAIXADO!
	if ((P[enemy].stateMoveType == 1 || (P[enemy].fball.active == TRUE && distancia_Player_X_Magia <= 200)) && 
	  (((P[player].direcao ==  1 && P[player].key_JOY_LEFT_status  == 2) ||
		(P[player].direcao == -1 && P[player].key_JOY_RIGHT_status == 2)) &&
		(P[player].state == 100 || P[player].state == 200 || P[player].state == 410 || P[player].state == 420)))
	{
		//defesa (107 = em pe ; 207 = abaixado)
		if(P[player].key_JOY_DOWN_status == 2){ 
			PLAYER_STATE(player,207); 
		}else{ 
			PLAYER_STATE(player,107); 
		}
	}
	
	//soltou a defesa - EM PE
	if((P[player].state == 107 || P[player].state == 108) && 
	  ((P[player].direcao ==  1 && P[player].key_JOY_LEFT_status  == 0) || 
	   (P[player].direcao == -1 && P[player].key_JOY_RIGHT_status == 0)))
	{ PLAYER_STATE(player,109);} 
	
	//defesa se encerrou com o final do ataque do inimigo - EM PE
	if(P[player].state == 108 && P[enemy].stateMoveType == 0)
	{ PLAYER_STATE(player,109); } 
	
	//soltou a defesa - ABAIXADO
	if((P[player].state == 207 || P[player].state == 208) && 
	  ((P[player].direcao ==  1 && P[player].key_JOY_LEFT_status  == 0) || 
	   (P[player].direcao == -1 && P[player].key_JOY_RIGHT_status == 0))
	){ PLAYER_STATE(player,209); } 
	
	//defesa se encerrou com o final do ataque do inimigo - ABAIXADO
	if(P[player].state == 208 && P[enemy].stateMoveType == 0)
	{ PLAYER_STATE(player,209); } 
	
	//variacao de defesa em pe e abaixado
	if(P[player].state == 108 && P[player].key_JOY_DOWN_status == 2){ PLAYER_STATE(player,208); }
	if(P[player].state == 208 && P[player].key_JOY_DOWN_status == 0){ PLAYER_STATE(player,108); }
}

void FUNCAO_FSM_NORMAL_ATTACKS(u8 player) {
	
	//golpes normais!
	if(gDistancia > 45)
	{
		//golpes em pe de longe
		if(P[player].key_JOY_X_status==1 && (P[player].state==100 || P[player].state==410 || P[player].state==420))
		{ 
			if((player==1 && Spark2_countDown>0) || (player==2 && Spark1_countDown>0))
			{
				if(P[player].state==101){ P[player].x+=4*(P[player].direcao*-1); }
			}
			PLAYER_STATE(player,101); 
		}
		if(P[player].key_JOY_Y_status==1 && (P[player].state==100 || P[player].state==481 || P[player].state==410 || P[player].state==420)){ PLAYER_STATE(player,102); }
		if(P[player].key_JOY_Z_status==1 && (P[player].state==100 || P[player].state==481 || P[player].state==410 || P[player].state==420))
		{ 
			if(P[player].state==410 || P[player].state==420){
				if(P[player].state==410){ PLAYER_STATE(player,471); }
				if(P[player].state==420){ PLAYER_STATE(player,472); }
			}else{
				PLAYER_STATE(player,103); 
			}
		}
		if(P[player].key_JOY_A_status==1 && (P[player].state==100 || P[player].state==481 || P[player].state==410 || P[player].state==420))
		{ 
			if((player==1 && Spark2_countDown>0) || (player==2 && Spark1_countDown>0))
			{
				if(P[player].state==104){ P[player].x+=4*(P[player].direcao*-1); }
			}
			PLAYER_STATE(player,104); 
		}
		if(P[player].key_JOY_B_status==1 && (P[player].state==100 || P[player].state==481 || P[player].state==410 || P[player].state==420)){ PLAYER_STATE(player,105); }
		if(P[player].key_JOY_C_status==1 && (P[player].state==100 || P[player].state==481 || P[player].state==410 || P[player].state==420)){ PLAYER_STATE(player,106); }
	
	}else{
		//golpes em pe de perto
		if(P[player].key_JOY_X_status==1 && (P[player].state==100 || P[player].state==481 || P[player].state==410 || P[player].state==420 || P[player].state==606)){ PLAYER_STATE(player,151); }
		if(P[player].key_JOY_Y_status==1 && (P[player].state==100 || P[player].state==481 || P[player].state==410 || P[player].state==420 || P[player].state==606)){ PLAYER_STATE(player,152); }
		if(P[player].key_JOY_Z_status==1 && (P[player].state==100 || P[player].state==481 || P[player].state==410 || P[player].state==420))
		{ 
			if(P[player].state==410 || P[player].state==420){
				if(P[player].state==410){ PLAYER_STATE(player,471); }
				if(P[player].state==420){ PLAYER_STATE(player,472); }
			}else{
				PLAYER_STATE(player,103); 
			}
		}
		if(P[player].key_JOY_A_status==1 && (P[player].state==100 || P[player].state==481 || P[player].state==410 || P[player].state==420 || P[player].state==606)){ PLAYER_STATE(player,154); }
		if(P[player].key_JOY_B_status==1 && (P[player].state==100 || P[player].state==481 || P[player].state==410 || P[player].state==420 || P[player].state==606)){ PLAYER_STATE(player,155); }
		if(P[player].key_JOY_C_status==1 && (P[player].state==100 || P[player].state==481 || P[player].state==410 || P[player].state==420 || P[player].state==606)){ PLAYER_STATE(player,106); }
	}
	
	//golpes abaixados
	if(P[player].key_JOY_X_status==1 && (P[player].state==200) )
	{ 
		if((player==1 && Spark2_countDown>0) || (player==2 && Spark1_countDown>0))
		{
			if(P[player].state==201){ P[player].x+=4*(P[player].direcao*-1); }
		}
		PLAYER_STATE(player,201); 
	}
	if(P[player].key_JOY_Y_status==1 && P[player].state==200){ PLAYER_STATE(player,202); }
	if(P[player].key_JOY_Z_status==1 && P[player].state==200){ PLAYER_STATE(player,203); }
	if(P[player].key_JOY_A_status==1 && (P[player].state==200) )
	{ 
		if((player==1 && Spark2_countDown>0) || (player==2 && Spark1_countDown>0))
		{
			if(P[player].state==204){ P[player].x+=4*(P[player].direcao*-1); }
		}
		PLAYER_STATE(player,204); 
	}
	if(P[player].key_JOY_B_status==1 && P[player].state==200){ PLAYER_STATE(player,205); }
	if(P[player].key_JOY_C_status==1 && P[player].state==200){ PLAYER_STATE(player,106); }


	//golpes aereos, pulo neutro
	if(P[player].key_JOY_X_status==1 && P[player].state==300){ PLAYER_STATE(player,301); }
	if(P[player].key_JOY_Y_status==1 && P[player].state==300){ PLAYER_STATE(player,302); }
	if(P[player].key_JOY_Z_status==1 && P[player].state==300){ PLAYER_STATE(player,303); }
	if(P[player].key_JOY_A_status==1 && P[player].state==300){ PLAYER_STATE(player,304); }
	if(P[player].key_JOY_B_status==1 && P[player].state==300){ PLAYER_STATE(player,305); }
	if(P[player].key_JOY_C_status==1 && P[player].state==300){ PLAYER_STATE(player,306); }
	
	//golpes aereos, pulo para tras
	if(P[player].key_JOY_X_status==1 && P[player].state==310){ PLAYER_STATE(player,311); }
	if(P[player].key_JOY_Y_status==1 && P[player].state==310){ PLAYER_STATE(player,312); }
	if(P[player].key_JOY_Z_status==1 && P[player].state==310){ PLAYER_STATE(player,313); }
	if(P[player].key_JOY_A_status==1 && P[player].state==310){ PLAYER_STATE(player,314); }
	if(P[player].key_JOY_B_status==1 && P[player].state==310){ PLAYER_STATE(player,315); }
	if(P[player].key_JOY_C_status==1 && P[player].state==310){ PLAYER_STATE(player,316); }
	
	//golpes aereos, pulo para frente
	if(P[player].key_JOY_X_status==1 && P[player].state==320){ PLAYER_STATE(player,321); }
	if(P[player].key_JOY_Y_status==1 && P[player].state==320){ PLAYER_STATE(player,322); }
	if(P[player].key_JOY_Z_status==1 && P[player].state==320){ PLAYER_STATE(player,323); }
	if(P[player].key_JOY_A_status==1 && P[player].state==320){ PLAYER_STATE(player,324); }
	if(P[player].key_JOY_B_status==1 && P[player].state==320){ PLAYER_STATE(player,325); }
	if(P[player].key_JOY_C_status==1 && P[player].state==320){ PLAYER_STATE(player,326); }
}

void FUNCAO_FSM_SPECIAL_ATTACKS(u8 player) {

	//MAGIAS STARTUP (FSM CHANGESTATE)!!!!!!
	//Condicoes para que se possa soltar as magias, e posterior Acionamento delas!
	//ACIONAMENTO DAS MAGIAS //INICIO DAS MAGIAS
	
	//bug fix para longo acionamento do shoryuken hpig 1.1
	if(P[player].joyDirTimer[4]==0 && P[player].inputArray[0]==4){ P[player].inputArray[0]=0; }
	if(P[player].joyDirTimer[6]==0 && P[player].inputArray[0]==6){ P[player].inputArray[0]=0; }
	
	if(((P[player].state >= 100 && P[player].state <= 156) || 
		(P[player].state >= 200 && P[player].state <= 256) || 
		 P[player].state == 410 || P[player].state == 420 || 
		 P[player].state == 471 || P[player].state == 472 || 
		 P[player].state == 602 || P[player].state == 606 ))
	{
		u8 magic_avaliable=1;
		
		if(P[player].id==1) //RYO
		{
			//730
			//Shoryuken Roris type
			if // >, v, v>, > + P ; frente, baixo, baixo+frente, frente + Soco
			(
				magic_avaliable==1 &&
				(
					//(P[player].direcao== 1 && ( P[player].inputArray[2]==6 && P[player].joyDirTimer[2] < P[player].joyDirTimer[6] ) && (P[player].joyDirTimer[2]!=0) ) 
					( P[player].direcao== 1 && P[player].inputArray[2]==6 && P[player].inputArray[1]==2 && P[player].inputArray[0]==6 && P[player].joyDirTimer[6]!=0 )
					|| 
					//(P[player].direcao==-1 && ( P[player].inputArray[2]==4 && P[player].joyDirTimer[2] < P[player].joyDirTimer[4] ) && (P[player].joyDirTimer[2]!=0) )
					( P[player].direcao==-1 && P[player].inputArray[2]==4 && P[player].inputArray[1]==2 && P[player].inputArray[0]==4 && P[player].joyDirTimer[4]!=0 )
				) &&
				( P[player].attackButton==1 ||  P[player].attackButton==2)
			)
			{
				if(Spark1_countDown>0){ SPR_releaseSprite(Spark[1]); Spark1_countDown=0; }
				if(Spark2_countDown>0){ SPR_releaseSprite(Spark[2]); Spark2_countDown=0; }

				if(P[player].hitPause==0){
					PLAYER_STATE(player,730);
					magic_avaliable=0; 
				}else{
					P[player].bufferSpecial = 730;
					magic_avaliable=0; 
				}
			}
			
			//700
			//hadouken type
			if // v, v>, > + P ; baixo, baixo+frente, frente + Soco
			(
				magic_avaliable==1 && P[player].fball.active == FALSE &&
				(
					//(P[player].direcao== 1 && ( P[player].joyDirTimer[2] < P[player].joyDirTimer[6] ) && (P[player].joyDirTimer[2]!=0) ) 
					( P[player].direcao== 1 && P[player].inputArray[1]==2 && P[player].inputArray[0]==6 && P[player].joyDirTimer[6]!=0 )
					|| 
					//(P[player].direcao==-1 && ( P[player].joyDirTimer[2] < P[player].joyDirTimer[4] ) && (P[player].joyDirTimer[2]!=0) )
					( P[player].direcao==-1 && P[player].inputArray[1]==2 && P[player].inputArray[0]==4 && P[player].joyDirTimer[4]!=0 )
				) &&			
				( P[player].attackButton==1 ||  P[player].attackButton==2)
			)
			{
				if(Spark1_countDown>0){ SPR_releaseSprite(Spark[1]); Spark1_countDown=0; }
				if(Spark2_countDown>0){ SPR_releaseSprite(Spark[2]); Spark2_countDown=0; }

				if(P[player].hitPause==0){
					PLAYER_STATE(player,700);
					magic_avaliable=0;
				}else{
					P[player].bufferSpecial = 700;
					magic_avaliable=0; 
				}
			}
			
			/*
			//710
			//Tatsumaki type
			if // v, <v, < + P ; baixo, baixo+tras, tras + Soco
			(
				magic_avaliable==1 && 
				(
					//(P[player].direcao==-1 && ( P[player].joyDirTimer[2] < P[player].joyDirTimer[6] ) && (P[player].joyDirTimer[2]!=0) ) 
					( P[player].direcao== 1 && P[player].inputArray[1]==2 && P[player].inputArray[0]==4 && P[player].joyDirTimer[4]!=0 )
					|| 
					//(P[player].direcao== 1 && ( P[player].joyDirTimer[2] < P[player].joyDirTimer[4] ) && (P[player].joyDirTimer[2]!=0) )
					( P[player].direcao==-1 && P[player].inputArray[1]==2 && P[player].inputArray[0]==6 && P[player].joyDirTimer[6]!=0 )
				) &&			
				( P[player].attackButton==1 ||  P[player].attackButton==2)
			)
			{
				if(P[1].hitPause==0 && P[2].hitPause==0){
					PLAYER_STATE(player,710);
					magic_avaliable=0;
				}else{
					P[player].bufferSpecial = 710;
					magic_avaliable=0; 
				}
			}
			
			//720
			//Tatsumaki type
			if // v, <v, < + P ; baixo, baixo+tras, tras + Chute
			(
				magic_avaliable==1 && 
				(
					//(P[player].direcao==-1 && ( P[player].joyDirTimer[2] < P[player].joyDirTimer[6] ) && (P[player].joyDirTimer[2]!=0) ) 
					( P[player].direcao== 1 && P[player].inputArray[1]==2 && P[player].inputArray[0]==4 && P[player].joyDirTimer[4]!=0 )
					|| 
					//(P[player].direcao== 1 && ( P[player].joyDirTimer[2] < P[player].joyDirTimer[4] ) && (P[player].joyDirTimer[2]!=0) )
					( P[player].direcao==-1 && P[player].inputArray[1]==2 && P[player].inputArray[0]==6 && P[player].joyDirTimer[6]!=0 )
				) &&			
				( P[player].attackButton==4 ||  P[player].attackButton==5)
			)
			{
				if(P[1].hitPause==0 && P[2].hitPause==0){
					PLAYER_STATE(player,720);
					magic_avaliable=0;
				}else{
					P[player].bufferSpecial = 720;
					magic_avaliable=0; 
				}
			}
			*/
			
		}//---end char; ryo
		
		if(magic_avaliable==0)
		{
			//reset input array
			P[player].inputArray[0]=0; P[player].inputArray[1]=0; P[player].inputArray[2]=0; P[player].inputArray[3]=0; P[player].inputArray[4]=0;
		}
	}
	
	//ryo MAGIA 701 (FIREBALL)
	if(P[player].id==1 && P[player].state==700 && P[player].animFrame==12 && P[player].frameTimeAtual==1)
	{
		if(P[player].fball.active==TRUE){ 
			if(P[player].fball.spriteFBall){ 
				SPR_releaseSprite(P[player].fball.spriteFBall); 
				P[player].fball.spriteFBall = NULL; 
			} 
		}
		P[player].fball.active=TRUE;
		P[player].fball.x = P[player].x+(18*P[player].direcao);
		if(P[player].direcao==-1){
			P[player].fball.x-=70;
		}
		P[player].fball.y = P[player].y-90;

		P[player].fball.dataHBox[0] = +10;
		P[player].fball.dataHBox[1] = +5;
		P[player].fball.dataHBox[2] = +50;
		P[player].fball.dataHBox[3] = +40;

		P[player].fball.guardFlag = 1;

		if(player==1)
		{
			P[player].fball.spriteFBall = SPR_addSpriteExSafe(&spr_ryo_701, 
				P[player].fball.x, P[player].fball.y, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 
				SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		if(player==2)
		{
			P[player].fball.spriteFBall = SPR_addSpriteExSafe(&spr_ryo_701, 
				P[player].fball.x, P[player].fball.y, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 
				SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
		}
		
		if(P[player].fball.spriteFBall){ 
			SPR_setDepth(P[player].fball.spriteFBall, 1); 
		}
		if(P[player].direcao == -1){ 
			SPR_setHFlip(P[player].fball.spriteFBall, TRUE); 
		}
		P[player].fball.direcao = P[player].direcao;
	}
}

void FUNCAO_FSM_COLLISION(u8 player, u8 enemy) {

	/////////////
	// Colisao //
	/////////////
	
	if( FUNCAO_COLISAO(
		P[player].x+P[player].dataHBox[0], P[player].y+P[player].dataHBox[1], P[player].x+P[player].dataHBox[2], P[player].y+P[player].dataHBox[3], 
		P[enemy].x+P[enemy].dataBBox[0], P[enemy].y+P[enemy].dataBBox[1], P[enemy].x+P[enemy].dataBBox[2], P[enemy].y+P[enemy].dataBBox[3]
	)==1 )
	{
		if(FUNCAO_CHECK_GUARD(P[player].guardFlag, enemy))
		{
			//DEFENDEU! em pe
			if(P[enemy].state>=107 && P[enemy].state<=110){
				PLAYER_STATE(enemy,110);
			}
			//DEFENDEU! em abaixado
			if(P[enemy].state>=207 && P[enemy].state<=210){
				PLAYER_STATE(enemy,210);
			}

			if(P[player].attackPower==1){
				P[player].hitPause=8;
				P[enemy].hitPause=8; 
			}else{ 
				P[player].hitPause=12;
				P[enemy].hitPause=12;  
			} 

			P[enemy].stateMoveType = 2;
			
			u8 AlturaDoHit = P[player].y+P[player].dataHBox[3];
			P[player].dataHBox[0]=0; P[player].dataHBox[1]=0; P[player].dataHBox[2]=0; P[player].dataHBox[3]=0; //desativa a hitbox
			
			//dano de magias, geral
			if(P[player].state==700){ FUNCAO_UPDATE_LIFESP(enemy,1,-2); }
			if(P[player].state==710){ FUNCAO_UPDATE_LIFESP(enemy,1,-2); }
			if(P[player].state==720){ FUNCAO_UPDATE_LIFESP(enemy,1,-2); }
			if(P[player].state==730){ FUNCAO_UPDATE_LIFESP(enemy,1,-2); }
			if(P[player].state==740){ FUNCAO_UPDATE_LIFESP(enemy,1,-2); }
			if(P[player].state==750){ FUNCAO_UPDATE_LIFESP(enemy,1,-2); }
			if(P[player].state==760){ FUNCAO_UPDATE_LIFESP(enemy,1,-2); }
			if(P[player].state==770){ FUNCAO_UPDATE_LIFESP(enemy,1,-2); }
			if(P[player].state==780){ FUNCAO_UPDATE_LIFESP(enemy,1,-2); }
			if(P[player].state==790){ FUNCAO_UPDATE_LIFESP(enemy,1,-2); }
			
			if(Spark1_countDown>0){ SPR_releaseSprite(Spark[1]); }
			if(Spark2_countDown>0){ SPR_releaseSprite(Spark[2]); }
			
			if(enemy==1){Spark1_countDown=28-1;}
			if(enemy==2){Spark2_countDown=28-1;}
			Spark[enemy] = SPR_addSpriteExSafe( &spr_spark0, P[enemy].x-14-camPosX, AlturaDoHit-14, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
			
			if(P[enemy].direcao == -1){ SPR_setHFlip(Spark[enemy], TRUE); }
			SPR_setDepth(Spark[enemy], 1);
			
		}else{
			//NAO DEFENDEU!
			u8 AlturaDoHit = P[player].y+P[player].dataHBox[3];
			P[player].dataHBox[0]=0; P[player].dataHBox[1]=0; P[player].dataHBox[2]=0; P[player].dataHBox[3]=0; //desativa a hitbox
			
			//Agarrao
			if(P[player].state == 800){
				if ((P[enemy].y > gAlturaPiso) || P[enemy].state == 602) { //O agarrao nao pega
					PLAYER_STATE(player,152); //Se o agarrao errar, faz o lutador dar um soco forte

				} else {
					PLAYER_STATE(player,801); //Se o agarrao pegar, primeiro movimento do player
					PLAYER_STATE(enemy,803); //Se o agarrao pegar, estado do inimigo
				}

			} else {

				u8 SparkType = 0;

				if(P[player].attackPower==1){
					P[player].hitPause=8;
					P[enemy].hitPause=8; 
				}else{ 
					P[player].hitPause=12;
					P[enemy].hitPause=12;  
				} 
				
				P[enemy].stateMoveType = 2;
				P[enemy].hitCounter++;

				//golpes de longe
				if(P[player].state==101)
				{ 
					if(P[enemy].y!=gAlturaPiso)
					{
						PLAYER_STATE(enemy,550);
					}else{
						if(P[enemy].state>=200 && P[enemy].state<=206)
						{
							PLAYER_STATE(enemy,505);
						}else{
							PLAYER_STATE(enemy,501);
						}
					} 
					FUNCAO_UPDATE_LIFESP(enemy,1,-4); //decrementa 4 da energia 
					FUNCAO_UPDATE_LIFESP(enemy,2,3); 
					SparkType= 1; 
				}
				if(P[player].state==102){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{if(P[enemy].state>=200 && P[enemy].state<=206){PLAYER_STATE(enemy,506);}else{PLAYER_STATE(enemy,502);}} FUNCAO_UPDATE_LIFESP(enemy,1,-10); FUNCAO_UPDATE_LIFESP(enemy,2,6); SparkType= 2; }
				if(P[player].state==104){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{if(P[enemy].state>=200 && P[enemy].state<=206){PLAYER_STATE(enemy,505);}else{PLAYER_STATE(enemy,501);}} FUNCAO_UPDATE_LIFESP(enemy,1,-4); FUNCAO_UPDATE_LIFESP(enemy,2,3); SparkType= 1; }
				if(P[player].state==105){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{if(P[enemy].state>=200 && P[enemy].state<=206){PLAYER_STATE(enemy,506);}else{PLAYER_STATE(enemy,502);}} FUNCAO_UPDATE_LIFESP(enemy,1,-10); FUNCAO_UPDATE_LIFESP(enemy,2,6); SparkType= 2; }
				if(P[player].state==106){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{PLAYER_STATE(enemy,550);} FUNCAO_UPDATE_LIFESP(enemy,1,-12); FUNCAO_UPDATE_LIFESP(enemy,2,6); SparkType= 3; }
				if(P[player].state==113){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{PLAYER_STATE(enemy,550);} FUNCAO_UPDATE_LIFESP(enemy,1,- 8); FUNCAO_UPDATE_LIFESP(enemy,2,6); SparkType= 2; }
				
				//golpes de perto
				if(P[player].state==151){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{if(P[enemy].state>=200 && P[enemy].state<=206){PLAYER_STATE(enemy,505);}else{PLAYER_STATE(enemy,503);}} FUNCAO_UPDATE_LIFESP(enemy,1,- 5); FUNCAO_UPDATE_LIFESP(enemy,2,3); SparkType= 1; }
				if(P[player].state==152){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{if(P[enemy].state>=200 && P[enemy].state<=206){PLAYER_STATE(enemy,506);}else{PLAYER_STATE(enemy,504);}} FUNCAO_UPDATE_LIFESP(enemy,1,-10); FUNCAO_UPDATE_LIFESP(enemy,2,6); SparkType= 2; }
				if(P[player].state==154){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{if(P[enemy].state>=200 && P[enemy].state<=206){PLAYER_STATE(enemy,505);}else{PLAYER_STATE(enemy,503);}} FUNCAO_UPDATE_LIFESP(enemy,1,- 5); FUNCAO_UPDATE_LIFESP(enemy,2,3); SparkType= 1; }
				if(P[player].state==155){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{if(P[enemy].state>=200 && P[enemy].state<=206){PLAYER_STATE(enemy,506);}else{PLAYER_STATE(enemy,504);}} FUNCAO_UPDATE_LIFESP(enemy,1,-10); FUNCAO_UPDATE_LIFESP(enemy,2,6); SparkType= 2; }
				
				//golpes abaixado
				if(P[player].state==201){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{if(P[enemy].state>=200 && P[enemy].state<=206){PLAYER_STATE(enemy,505);}else{PLAYER_STATE(enemy,503);}} FUNCAO_UPDATE_LIFESP(enemy,1,- 4); FUNCAO_UPDATE_LIFESP(enemy,2,3); SparkType=  1; }
				if(P[player].state==202){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{if(P[enemy].state>=200 && P[enemy].state<=206){PLAYER_STATE(enemy,506);}else{PLAYER_STATE(enemy,504);}} FUNCAO_UPDATE_LIFESP(enemy,1,-10); FUNCAO_UPDATE_LIFESP(enemy,2,6); SparkType=  2; }
				if(P[player].state==204){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{if(P[enemy].state>=200 && P[enemy].state<=206){PLAYER_STATE(enemy,505);}else{PLAYER_STATE(enemy,503);}} FUNCAO_UPDATE_LIFESP(enemy,1,- 4); FUNCAO_UPDATE_LIFESP(enemy,2,3); SparkType=  1; }
				if(P[player].state==205){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{PLAYER_STATE(enemy,550);} FUNCAO_UPDATE_LIFESP(enemy,1,-10); FUNCAO_UPDATE_LIFESP(enemy,2,6); SparkType=  2; }
				
				//golpes aereos
				
				//cross hpig 1.1
				if(P[player].state>=301 && P[player].state<=326)
				{
					if(P[enemy].state==607 || P[enemy].state==608)
					{
						if(P[enemy].direcao==1){P[enemy].direcao=-1;}else{P[enemy].direcao=1;}
					}
				}
				if(P[player].state==301 || P[player].state==311 || P[player].state==321){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{if(P[enemy].state>=200 && P[enemy].state<=256){PLAYER_STATE(enemy,505);}else{PLAYER_STATE(enemy,501);}} FUNCAO_UPDATE_LIFESP(enemy,1,- 5); FUNCAO_UPDATE_LIFESP(enemy,2, 3); SparkType= 1; }
				if(P[player].state==302 || P[player].state==312 || P[player].state==322){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{if(P[enemy].state>=200 && P[enemy].state<=256){PLAYER_STATE(enemy,506);}else{PLAYER_STATE(enemy,502);}} FUNCAO_UPDATE_LIFESP(enemy,1,-10); FUNCAO_UPDATE_LIFESP(enemy,2, 6); SparkType= 2; }
				if(P[player].state==303 || P[player].state==313 || P[player].state==323){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{if(P[enemy].state>=200 && P[enemy].state<=256){PLAYER_STATE(enemy,506);}else{PLAYER_STATE(enemy,501);}} FUNCAO_UPDATE_LIFESP(enemy,1,- 5); FUNCAO_UPDATE_LIFESP(enemy,2,11); SparkType= 1; }
				if(P[player].state==304 || P[player].state==314 || P[player].state==324){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{if(P[enemy].state>=200 && P[enemy].state<=256){PLAYER_STATE(enemy,505);}else{PLAYER_STATE(enemy,501);}} FUNCAO_UPDATE_LIFESP(enemy,1,- 5); FUNCAO_UPDATE_LIFESP(enemy,2, 3); SparkType= 1; }
				if(P[player].state==305 || P[player].state==315 || P[player].state==325){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{if(P[enemy].state>=200 && P[enemy].state<=256){PLAYER_STATE(enemy,506);}else{PLAYER_STATE(enemy,502);}} FUNCAO_UPDATE_LIFESP(enemy,1,-10); FUNCAO_UPDATE_LIFESP(enemy,2, 6); SparkType= 2; }
				if(P[player].state==306 || P[player].state==316 || P[player].state==326){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{PLAYER_STATE(enemy,550);} FUNCAO_UPDATE_LIFESP(enemy,1,- 8); FUNCAO_UPDATE_LIFESP(enemy,2,11); SparkType= 3; }
				

				//Agarrao
				if(P[player].state==801){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{if(P[enemy].state>=200 && P[enemy].state<=206){PLAYER_STATE(enemy,506);}else{PLAYER_STATE(enemy,504);}} FUNCAO_UPDATE_LIFESP(enemy,1,-10); FUNCAO_UPDATE_LIFESP(enemy,2,6); SparkType= 2; }
				if(P[player].state==802){ if(P[enemy].y!=gAlturaPiso){PLAYER_STATE(enemy,550);}else{PLAYER_STATE(enemy,550);} FUNCAO_UPDATE_LIFESP(enemy,1,-10); FUNCAO_UPDATE_LIFESP(enemy,2,6); SparkType=  2; }
				

				//u8 customHitPause = 0;
				
				//MAGIAS HITS /////////////////////////////
				//RYO ID:1
				if(P[player].id==1)
				{
					if(P[player].state==710)
					{ 
						if(P[player].animFrame<=3){PLAYER_STATE(enemy,503);}else{PLAYER_STATE(enemy,550);} 
						FUNCAO_UPDATE_LIFESP(enemy,1,-10); FUNCAO_UPDATE_LIFESP(enemy,2,5); SparkType= 1; 
					}
					if(P[player].state==720)
					{ 
						if(P[enemy].y!=gAlturaPiso){ PLAYER_STATE(enemy,550); }else{if(P[player].animFrame<=6){ PLAYER_STATE(enemy,503); }else{ PLAYER_STATE(enemy,550); } } 
						FUNCAO_UPDATE_LIFESP(enemy,1,-5); FUNCAO_UPDATE_LIFESP(enemy,2,5); SparkType= 1; 
					}
					if(P[player].state==730)
					{ 
						if(P[player].animFrame<=3){PLAYER_STATE(enemy,503);}else{PLAYER_STATE(enemy,550);}
						FUNCAO_UPDATE_LIFESP(enemy,1,-10); FUNCAO_UPDATE_LIFESP(enemy,2,5); SparkType=2; 
					}
				}
				///////////////////////////////////////////

				//Spark, efeito FX, do golpe acertando o oponente
				if(SparkType!=0)
				{
					//P[enemy].hitPause=16+customHitPause; //hitpause Shake
					
					if(Spark1_countDown>0){ if(Spark[1]){ SPR_releaseSprite(Spark[1]); Spark[1] = NULL; } }
					if(Spark2_countDown>0){ if(Spark[2]){ SPR_releaseSprite(Spark[2]); Spark[2] = NULL; } }
					
					if(SparkType== 1)
					{ 
						if(enemy==1){Spark1_countDown=28-1;}
						if(enemy==2){Spark2_countDown=28-1;}
						Spark[enemy] = SPR_addSprite(&spr_spark1, P[enemy].x-16-camPosX, AlturaDoHit-16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE)); 
						//SPR_setVRAMTileIndex(Spark[PR], 1519); //define uma posicao especifica para o GFX na VRAM
					}
					//SPR_addSpriteExSafe( &spr_spark1, P[PR].x-16, AlturaDoHit-32, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
					if(SparkType== 2)
					{ 
						if(enemy==1){Spark1_countDown=28-1;}
						if(enemy==2){Spark2_countDown=28-1;}
						Spark[enemy] = SPR_addSprite(&spr_spark2, P[enemy].x-24-camPosX, AlturaDoHit-24, TILE_ATTR(PAL1, FALSE, FALSE, FALSE)); 
						//SPR_setVRAMTileIndex(Spark[PR], 1519); //define uma posicao especifica para o GFX na VRAM
					}
					//SPR_addSpriteExSafe( &spr_spark2, P[PR].x-16, AlturaDoHit-32, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
					if(SparkType== 3)
					{ 
						if(enemy==1){Spark1_countDown=44-1;}
						if(enemy==2){Spark2_countDown=44-1;}
						Spark[enemy] = SPR_addSprite(&spr_spark3, P[enemy].x-52-camPosX, AlturaDoHit-90, TILE_ATTR(PAL1, FALSE, FALSE, FALSE)); 
						//SPR_setVRAMTileIndex(Spark[PR], 1519); //define uma posicao especifica para o GFX na VRAM
					}
					//SPR_addSpriteExSafe( &spr_spark3, P[PR].x-16, AlturaDoHit-32, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
					
					if(P[enemy].direcao == 1){ if(Spark[enemy]){ SPR_setHFlip(Spark[enemy], TRUE); }}
					if(Spark[enemy]){ SPR_setDepth(Spark[enemy], 1); }
				}
			}
		}
	}
	
	/////////////////////////
	// COLISAO DE HITBOXES //
	/////////////////////////
	
	if( FUNCAO_COLISAO(
			P[player].x+P[player].dataHBox[0], P[player].y+P[player].dataHBox[1], P[player].x+P[player].dataHBox[2], P[player].y+P[player].dataHBox[3], 
			P[enemy].x+P[enemy].dataHBox[0], P[enemy].y+P[enemy].dataHBox[1], P[enemy].x+P[enemy].dataHBox[2], P[enemy].y+P[enemy].dataHBox[3]
		)==1 )
	{
		if(P[player].dataBBox[0]==0 && P[player].dataBBox[1]==0 && P[player].dataBBox[2]==0 && P[player].dataBBox[3]==0)
		{
			//nao faz nada, esta invencivel. ex shoryuken
		}else{
			
			P[player].stateMoveType = 2;
			P[player].hitCounter++;

			P[enemy].stateMoveType = 2;
			P[enemy].hitCounter++;

			if(doubleHitStep==0)
			{
				if(P[player].attackPower==1){
					P[enemy].hitPause= 8;
				}else{
					P[enemy].hitPause=12;
				}
				if(P[enemy].attackPower==1){
					P[player].hitPause= 8;
				}else{
					P[player].hitPause=12;
				}
				
				doubleHitStep=1;
			}
			
			if(doubleHitStep==2)
			{
				FUNCAO_UPDATE_LIFESP(1,1,-2); 
				FUNCAO_UPDATE_LIFESP(2,1,-2); 
				if(P[player].state>=100 && P[player].state<=199){ PLAYER_STATE(player, 501); }
				if(P[player].state>=200 && P[player].state<=299){ PLAYER_STATE(player, 511); }
				if(P[player].state>=300 && P[player].state<=399){ PLAYER_STATE(player, 550); }
				if(P[player].state>=700 && P[player].state<=799)
				{
					if(P[player].y!=gAlturaPiso){ PLAYER_STATE(player, 550); }else{ PLAYER_STATE(player, 501); }
				}
			}
			
		}
		if(player==2 && doubleHitStep==2)
		{
			u8 AlturaDoHit = P[player].y+P[player].dataHBox[3];
			
			if(Spark1_countDown>0){ if(Spark[1]){ SPR_releaseSprite(Spark[1]); Spark[1] = NULL; } }
			if(Spark2_countDown>0){ if(Spark[2]){ SPR_releaseSprite(Spark[2]); Spark[2] = NULL; } }
			
			Spark1_countDown=28-1;
			Spark2_countDown=28-1;
			
			Spark[1] = SPR_addSprite(&spr_spark1, P[1].x-16-camPosX, AlturaDoHit-16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE)); 
			Spark[2] = SPR_addSprite(&spr_spark1, P[2].x-16-camPosX, AlturaDoHit-16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE)); 
			
			SPR_setVRAMTileIndex(Spark[1], 1519); //define uma posicao especifica para o GFX na VRAM
			SPR_setVRAMTileIndex(Spark[2], 1519); //define uma posicao especifica para o GFX na VRAM
			
			if(Spark[1]){ SPR_setDepth(Spark[1], 1); }
			if(Spark[2]){ SPR_setDepth(Spark[2], 1); }
			
			P[1].dataHBox[0]=0; P[1].dataHBox[1]=0; P[1].dataHBox[2]=0; P[1].dataHBox[3]=0;
			P[2].dataHBox[0]=0; P[2].dataHBox[1]=0; P[2].dataHBox[2]=0; P[2].dataHBox[3]=0;
			
			//XGM_setPCM(P1_SFX, snd_501, sizeof(snd_501)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3);
			
			doubleHitStep=0;
		}
		//101 102 104 105 106 113
		//151 152 154 155 
		//201 202 204 205 
	}
	
	////////////////////
	// CORPO x MAGIAS //
	//////////////////// -=≡ΣO)) x PLAYER
	
	if( P[player].fball.active && FUNCAO_COLISAO(
			P[player].fball.x + P[player].fball.dataHBox[0], P[player].fball.y + P[player].fball.dataHBox[1], P[player].fball.x + P[player].fball.dataHBox[2], P[player].fball.y + P[player].fball.dataHBox[3],
			P[enemy].x+P[enemy].dataBBox[0], P[enemy].y+P[enemy].dataBBox[1], P[enemy].x+P[enemy].dataBBox[2], P[enemy].y+P[enemy].dataBBox[3]
		)==1)
	{
		if(FUNCAO_CHECK_GUARD(P[player].fball.guardFlag, enemy))
		{
			//DEFENDEU! em pe
			if(P[enemy].state>=107 && P[enemy].state<=109){
				PLAYER_STATE(enemy,110);
				FUNCAO_UPDATE_LIFESP(enemy,1,-2);
			}
			//DEFENDEU! em baixado
			if(P[enemy].state>=207 && P[enemy].state<=209){
				PLAYER_STATE(enemy,210);
				FUNCAO_UPDATE_LIFESP(enemy,1,-2);
			}

			if(P[player].attackPower==1){
				P[enemy].hitPause=16;
			}else{
				P[enemy].hitPause=20;
			} 

			P[enemy].stateMoveType = 3;

		}else{
			//NAO DEFENDEU!
			if(P[player].attackPower==1){
				P[enemy].hitPause=16;
			}else{
				P[enemy].hitPause=20;
			} 
			FUNCAO_UPDATE_LIFESP(enemy,1,-8); 
			FUNCAO_UPDATE_LIFESP(enemy,2, 11);
			
			P[enemy].stateMoveType = 3;
			P[enemy].hitCounter++;

			/*
			if(P[enemy].y<gAlturaPiso || (P[enemy].state>=300 && P[enemy].state<=326) )
			{ 
				PLAYER_STATE(enemy,550); //cai apos cair sobre a magia
			}else{
				PLAYER_STATE(enemy,502); //recebe o hit da magia em pe
			}*/
			if(P[enemy].y==gAlturaPiso){ PLAYER_STATE(enemy,502); }else{ P[enemy].puloTimer=0; PLAYER_STATE(enemy,550); }
			
		}
		
		//desativa a magia ja que colidiu
		if(P[player].fball.active==1)
		{ 
			if(P[player].fball.spriteFBall)
			{ 
				SPR_releaseSprite(P[player].fball.spriteFBall); 
				P[player].fball.spriteFBall = NULL; 
			} 
			P[player].fball.active=0;
		}			
		
		//SPARK DE MAGIA SUMINDO
		if(Spark1_countDown>0){ SPR_releaseSprite(Spark[1]); }
		if(Spark2_countDown>0){ SPR_releaseSprite(Spark[2]); }
		if(player==1){Spark1_countDown=20;}
		if(player==2){Spark2_countDown=20;}
		
		if(player==1)//p1
		{
			//ryo
			if(P[player].direcao == 1)
			{
				if(P[player].id==1){ Spark[player] = SPR_addSpriteExSafe( &spr_ryo_702, P[enemy].x-80-camPosX, P[player].fball.y, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
			}else{
				if(P[player].id==1){ Spark[player] = SPR_addSpriteExSafe( &spr_ryo_702, P[enemy].x-camPosX, P[player].fball.y, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
			}
			
		}
		if(player==2)//p2
		{
			//ryo
			if(P[player].direcao == 1)
			{
				// Sprite* SPR_addSpriteExSafe(const SpriteDefinition* spriteDef, s16 x, s16 y, u16 attribut, u16 flag)

				if(P[player].id==1){ Spark[player] = SPR_addSpriteExSafe( &spr_ryo_702, P[enemy].x-80-camPosX, P[player].fball.y, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC |SPR_FLAG_AUTO_TILE_UPLOAD); }
			}else{
				if(P[player].id==1){ Spark[player] = SPR_addSpriteExSafe( &spr_ryo_702, P[enemy].x-camPosX, P[player].fball.y, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD); }
			}
			
		}
		
		//se o spark foi carregado com sucesso...
		if (Spark[player])
		{ 
			if(P[player].direcao == -1){ SPR_setHFlip(Spark[player], TRUE); }
			SPR_setDepth(Spark[player], 1); 
		}
		
		//reseta posicao da magia
		P[player].fball.x=-250;
		P[player].fball.y=-250; 
	}
	
	/////////////////////
	// MAGIAS x MAGIAS //
	///////////////////// -=≡ΣO)) x ((O3≡=-
	
	if( (P[player].fball.active && P[enemy].fball.active) && FUNCAO_COLISAO(
			P[player].fball.x + P[player].fball.dataHBox[0], P[player].fball.y + P[player].fball.dataHBox[1], P[player].fball.x + P[player].fball.dataHBox[2], P[player].fball.y + P[player].fball.dataHBox[3],
			P[enemy].fball.x + P[enemy].fball.dataHBox[0], P[enemy].fball.y + P[enemy].fball.dataHBox[1], P[enemy].fball.x + P[enemy].fball.dataHBox[2], P[enemy].fball.y + P[enemy].fball.dataHBox[3]
		)==1
		&& ( (P[player].fball.x!=0 && P[player].fball.y!=0) && (P[player].fball.x!=-250 && P[player].fball.y!=-250) ) )
	{
		//desativa a magia
		if(P[player].fball.active==1)
		{ 
			if(P[player].fball.spriteFBall)
			{ 
				SPR_releaseSprite(P[player].fball.spriteFBall); 
				P[player].fball.spriteFBall = NULL; 
			} 
			P[player].fball.active=0;
		}			
		if(P[enemy].fball.active==1)
		{ 
			if(P[enemy].fball.spriteFBall)
			{ 
				SPR_releaseSprite(P[enemy].fball.spriteFBall); 
				P[enemy].fball.spriteFBall = NULL; 
			} 
			P[enemy].fball.active=0;
		}	
		
		//SPARK DE MAGIA SUMINDO
		if(Spark1_countDown>0){ SPR_releaseSprite(Spark[1]); }
		if(Spark2_countDown>0){ SPR_releaseSprite(Spark[2]); }
		if(player==1){Spark1_countDown=20;}
		if(player==2){Spark2_countDown=20;}
		
		//se o spark foi carregado com sucesso...
		if (Spark[player])
		{ 
			if(P[player].direcao == -1){ SPR_setHFlip(Spark[player], TRUE); }
			SPR_setDepth(Spark[player], 1); 
		}
		if (Spark[enemy])
		{ 
			if(P[enemy].direcao == -1){ SPR_setHFlip(Spark[enemy], TRUE); }
			SPR_setDepth(Spark[enemy], 1); 
		}
		
		//reseta posicao da magia
		P[player].fball.x=-250;
		P[player].fball.y=-250; 
		P[enemy].fball.x=-250;
		P[enemy].fball.y=-250;
	}
}

void FUNCAO_FSM()
{
	
	if(P[1].direcao==1){
		gDistancia = P[2].x - P[1].x;
	}else{
		gDistancia = P[1].x - P[2].x;
	}

	for(i=1; i<=2; i++)
	{
		u8 PA; //Player Attack (Player que ataca)
		u8 PR; //Player Receive (Player que recebe o dano)
		if(i==1){ PA=1; PR=2; }else{ PA=2; PR=1; }
		
		//hpig 1.1 bugfix - virar enquanto anda
		if((P[PA].state==410 || P[PA].state==420) && P[PA].direcao== 1 && (P[PA].x>P[PR].x)){ PLAYER_STATE(i,100); }
		if((P[PA].state==410 || P[PA].state==420) && P[PA].direcao==-1 && (P[PA].x<P[PR].x)){ PLAYER_STATE(i,100); }
		
		//Ataque depois da Esquiva
		if(P[i].state==103 && (
			P[i].key_JOY_A_status==1 || 
			P[i].key_JOY_B_status==1 || 
			P[i].key_JOY_C_status==1 || 
			P[i].key_JOY_X_status==1 || 
			P[i].key_JOY_Y_status==1 
		)	
		){ PLAYER_STATE(i,113); }

		//abaixando ; esquivando; levantando
		if((P[i].key_JOY_DOWN_status==1 || P[i].key_JOY_DOWN_status==2) && (P[i].state==100 || P[i].state==410 || P[i].state==420))
		{ 
			if(P[i].key_JOY_countdown[2]==0){ PLAYER_STATE(i,601); }else{ PLAYER_STATE(i,601);/*antigo 470*/ } //abaixar ou esquivar
		}
		if(P[i].key_JOY_DOWN_status==0 && P[i].state==200){ PLAYER_STATE(i,602); } //levantar
		
		//interrupcao da esquiva abaixada
		if(P[i].state==470 && P[i].animFrame==2 && P[i].key_JOY_DOWN_status==0)
		{ 
			P[i].animFrame=3; 
			P[i].frameTimeAtual=1; 
			P[i].frameTimeTotal=P[i].dataAnim[3]; 
		}
		
		//virando de lado (mudanca de estado)
		if(P[PA].direcao== 1 && P[PR].x<P[PA].x && P[PA].state==100){ PLAYER_STATE(i,607); P[PA].direcao=-1; }
		if(P[PA].direcao==-1 && P[PA].x<P[PR].x && P[PA].state==100){ PLAYER_STATE(i,607); P[PA].direcao= 1; }
		if(P[PA].direcao== 1 && P[PR].x<P[PA].x && P[PA].state==200){ PLAYER_STATE(i,608); P[PA].direcao=-1; }
		if(P[PA].direcao==-1 && P[PA].x<P[PR].x && P[PA].state==200){ PLAYER_STATE(i,608); P[PA].direcao= 1; }
		
		//ini pulos
		if((P[i].key_JOY_UP_status==1 || P[i].key_JOY_UP_status==2) && (P[i].state==100 || P[i].state==410 || P[i].state==420 || P[i].state==602))
		{ 
			if(P[i].key_JOY_RIGHT_status==2){ if(P[i].direcao==1){PLAYER_STATE(i,605);}else{PLAYER_STATE(i,603);} }
			if(P[i].key_JOY_RIGHT_status==0 && P[i].key_JOY_LEFT_status==0)PLAYER_STATE(i,604); 
			if(P[i].key_JOY_LEFT_status ==2){ if(P[i].direcao==1){PLAYER_STATE(i,603);}else{PLAYER_STATE(i,605);} }
		}
		
		//end pulos
		if((P[i].state==300 || P[i].state==310 || P[i].state==320) && P[i].y>=gAlturaPiso)
		{ 
			P[i].y=gAlturaPiso; P[i].puloTimer=0; P[i].hSpeed=0; 
			if(P[i].key_JOY_DOWN_status==2){PLAYER_STATE(i,200);}else{PLAYER_STATE(i,606);}
		}
		if((P[i].state>=301 && P[i].state<=326) && P[i].y>=gAlturaPiso)
		{ 
			P[i].y=gAlturaPiso; P[i].hSpeed=0; 
			if(P[i].key_JOY_DOWN_status==2){PLAYER_STATE(i,200);}else{PLAYER_STATE(i,606);}
		}
		
		//andando - andar ou correr
		if(P[i].direcao==1)
		{
			if(P[i].key_JOY_LEFT_status>0 && P[i].state==100){ 
				if(P[i].key_JOY_countdown[4]==0){ PLAYER_STATE(i,410); }else{ 
					if(P[i].inputArray[1]==4 && P[i].inputArray[0]==4){ PLAYER_STATE(i,471);  }else{ PLAYER_STATE(i,410); }
				}
			}
			if(P[i].key_JOY_RIGHT_status>0 && P[i].state==100){ 
				if(P[i].key_JOY_countdown[6]==0){ PLAYER_STATE(i,420); }else{ 
					if(P[i].inputArray[1]==6 && P[i].inputArray[0]==6){ PLAYER_STATE(i,472);  }else{ PLAYER_STATE(i,420); } 
				}
			}
			
			if(P[i].key_JOY_LEFT_status==0 && P[i].state==410){ PLAYER_STATE(i,100); } //end walk
			if(P[i].key_JOY_RIGHT_status==0 && P[i].state==420){ PLAYER_STATE(i,100); }

		}else{
			if(P[i].key_JOY_LEFT_status>0 && P[i].state==100){ 
				if(P[i].key_JOY_countdown[4]==0){ PLAYER_STATE(i,420); }else{ 
					if(P[i].inputArray[1]==4 && P[i].inputArray[0]==4){ PLAYER_STATE(i,472);  }else{ PLAYER_STATE(i,420); } 
				}
			}
			
			if(P[i].key_JOY_RIGHT_status>0 && P[i].state==100){ 
				if(P[i].key_JOY_countdown[6]==0){ PLAYER_STATE(i,410); }else{ 
					if(P[i].inputArray[1]==6 && P[i].inputArray[0]==6){ PLAYER_STATE(i,471);  }else{ PLAYER_STATE(i,410); }
				}
			}
			
			if(P[i].key_JOY_LEFT_status ==0 && P[i].state==420){ P[i].hSpeed=0; PLAYER_STATE(i,100); } //end walk
			if(P[i].key_JOY_RIGHT_status==0 && P[i].state==410){ P[i].hSpeed=0; PLAYER_STATE(i,100); }
		}
		
		//key_JOY_countdown eh um timer regressivo, ativado quando se aperta algum botao direcional, usado para Corrida e Esquiva, entre outros...
		if(P[i].key_JOY_countdown[8]>0){ P[i].key_JOY_countdown[8]--; }
		if(P[i].key_JOY_countdown[2]>0){ P[i].key_JOY_countdown[2]--; }
		if(P[i].key_JOY_countdown[4]>0){ P[i].key_JOY_countdown[4]--; }
		if(P[i].key_JOY_countdown[6]>0){ P[i].key_JOY_countdown[6]--; }
		
		//ativacao de 'key_JOY_countdown'...
		if(P[i].state==410 || P[i].state==420 || P[i].state==601 || P[i].state==603 || P[i].state==604 || P[i].state==605)
		{
			if(P[i].key_JOY_UP_status   ==1){ P[i].key_JOY_countdown[8]=12; P[i].key_JOY_countdown[2]=0; }
			if(P[i].key_JOY_LEFT_status ==1){ P[i].key_JOY_countdown[4]=12; P[i].key_JOY_countdown[6]=0; }
			if(P[i].key_JOY_RIGHT_status==1){ P[i].key_JOY_countdown[6]=12; P[i].key_JOY_countdown[4]=0; }
			if(P[i].key_JOY_DOWN_status ==1){ P[i].key_JOY_countdown[2]=12; P[i].key_JOY_countdown[8]=0; P[i].key_JOY_countdown[4]=0; P[i].key_JOY_countdown[6]=0; }
		}
		
		//time over decision; timeover
		if((gClockLTimer==0 && gClockRTimer==0) && P[1].state!=611 && P[1].state!=615)
		{
			bool updateWinsFlag=0;
			if(P[1].energiaBase> P[2].energiaBase){ PLAYER_STATE(1,611); P[1].wins++; updateWinsFlag=1; PLAYER_STATE(2,615); }
			if(P[1].energiaBase< P[2].energiaBase){ PLAYER_STATE(2,611); P[2].wins++; updateWinsFlag=2; PLAYER_STATE(1,615); }
			if(P[1].energiaBase==P[2].energiaBase){ PLAYER_STATE(1,615); PLAYER_STATE(2,615); }
			if(updateWinsFlag!=0){
				if(updateWinsFlag==1) //P1
				{
					//if(P[1].wins>=1){ SPR_setVisibility(GE[12].sprite, VISIBLE); }
					//if(P[1].wins>=2){ SPR_setVisibility(GE[13].sprite, VISIBLE); }
				}
				if(updateWinsFlag==2) //P2
				{
					//if(P[2].wins>=1){ SPR_setVisibility(GE[15].sprite, VISIBLE); }
					//if(P[2].wins>=2){ SPR_setVisibility(GE[14].sprite, VISIBLE); }
				}
			}
		}
		
		//Destroi Magias!
		if(P[i].fball.countDown>0){ P[i].fball.countDown--; }
		if(P[i].fball.countDown==1)
		{
			if(P[i].fball.active==1)
			{ 
				if(P[i].id==1 || P[i].id==2 || P[i].id==3)//ryo & ANDY & JOE
				{
					if(P[i].fball.spriteFBall)
					{ 
						SPR_releaseSprite(P[i].fball.spriteFBall); 
						P[i].fball.spriteFBall = NULL; 
						P[i].fball.x=-250;
						P[i].fball.y=-250; 
					} 
					P[i].fball.active=0;
				}
			}		
		}
		
		
		//------------------------------------------------------
		//Agarrao
		if(P[i].key_JOY_Y_status==1 && gDistancia < 40 &&
		 ((P[i].direcao== 1 && (P[i].key_JOY_RIGHT_status==1 || P[i].key_JOY_RIGHT_status==2)) ||
	      (P[i].direcao==-1 && (P[i].key_JOY_LEFT_status ==1 || P[i].key_JOY_LEFT_status ==2))) &&
		  (P[i].state==100 || P[i].state==410 || P[i].state==420))
		{ 
			PLAYER_STATE(i,800); //Inicio do agarrao
		}

		if (P[i].state == 801 && P[i].animFrame == 4) {
			PLAYER_STATE(i,802); //Se o agarrao pegar, segundo movimento do player
		}
		//------------------------------------------------------


		FUNCAO_FSM_DEFENSE(PA, PR); //Verifica o estado de defesa
		FUNCAO_FSM_NORMAL_ATTACKS(PA); //Verifica os ataques normais
		FUNCAO_FSM_SPECIAL_ATTACKS(PA); //Verificando os ataques especiais
		FUNCAO_FSM_COLLISION(PA, PR); //Verificando colisoes

		
		//Rage Explosion! Rage timer!
		/*
		if( 
		P[i].rageTimerCountdown==RAGETIMER && P[i].energiaSP>=32 
		&& (P[i].state==100 || P[i].state==200 || P[i].state==410 || P[i].state==420) 
		&& gPodeMover==1
		)
		{
			P[i].rageTimerCountdown--;
			PLAYER_STATE(i,618);
		}
		if(P[i].energiaSP>=32 && P[i].rageTimerCountdown<RAGETIMER && P[i].energiaBase>0 && gFrames>350){ if(P[i].rageTimerCountdown>0){ P[i].rageTimerCountdown--; } }
		if(P[i].rageTimerCountdown==0 && (P[i].state==100 || P[i].state==200)){ P[i].rageTimerCountdown=RAGETIMER; P[i].energiaSP=0; }
		*/
		
		//Vitoria
		if(
		 (P[PA].state==100 || P[PA].state==200 || P[PA].state==410 || P[PA].state==420) 
		 && (P[PA].state!=611 || P[PA].state!=612 || P[PA].state!=613 || P[PA].state!=614) 
		 && (P[PR].energiaBase<=0)
		)
		{
			//Carrega 1 entre 4 poses de vitoria
			//2 neste caso :) 611;612
			//Ajuste a probabilidade conforme preferir. gPing10 = 10% ; variacao de 0 a 9
			if(gPing10>=1 && gPing10<=5){ PLAYER_STATE(PA,611); P[PA].wins++; }
			if(gPing10>=6 && gPing10<=9){ PLAYER_STATE(PA,612); P[PA].wins++; }
			//if(gPing10>=7 && gPing10<=9){ PLAYER_STATE(PA,613); P[PA].wins++; }
			if(gPing10==0              ){ PLAYER_STATE(PA,612); P[PA].wins++; } //Vc pode carregar uma pose diferente aqui, eu estou usando 3!
			
			//Exibe o contador de Wins correspondente
			if(PA==1) //P1
			{
				if(P[PA].wins>=1){ SPR_setVisibility(GE[12].sprite, VISIBLE); }
				if(P[PA].wins>=2){ SPR_setVisibility(GE[13].sprite, VISIBLE); }
			}
			if(PA==2) //P2
			{
				if(P[PA].wins>=1){ SPR_setVisibility(GE[15].sprite, VISIBLE); }
				if(P[PA].wins>=2){ SPR_setVisibility(GE[14].sprite, VISIBLE); }
			}
		}
	}
}

void FUNCAO_FSM_HITBOXES(u8 Player)
{
	bool reset_BBox=0;
	bool reset_HBox=0;
	
	switch ( P[Player].id )
	{

		case 0: //char_default
			switch ( P[Player].state )
			{
				case 100:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
			
				case 101:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				//...
				break;
				
				case 102:
				//...
				break;
				
				case 103:
				reset_HBox=1; 
				reset_BBox=1;
				break;
				
				case 104:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				//...
				break;
				
				case 105:
				//...
				break;
				
				case 106:
				//...
				break;
				
				case 113:
				//...
				break;
				
				case 151:
				//...
				break;
				
				case 152:
				//...
				break;
				
				case 154:
				//...
				break;
				
				case 155:
				//...
				break;
				
				case 200:
				//...
				break;
				
				case 201:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				//...
				break;
				
				case 202:
				//...
				break;
				
				case 204:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				//...
				break;
				
				case 205:
				//...
				break;
				
				case 301:
				case 311:
				case 321:
				//...
				break;
				
				case 302:
				case 312:
				case 322:
				//...
				break;
				
				case 304:
				case 314:
				case 324:
				//...
				break;
				
				case 305:
				case 315:
				case 325:
				//...
				break;
				
				case 306:
				case 316:
				case 326:
				//...
				break;
				
				case 601:
				case 602:
				case 603:
				case 604:
				case 605:
				case 606:
				reset_HBox=1;
				break;
			}
		break;
		
		case 1: //RYO
			switch ( P[Player].state )
			{
				case 100:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
			
				case 101:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 92; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-72; }
				if(P[Player].animFrame==3){ reset_HBox=1; }
				break;
				
				case 102:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 85; P[Player].dataHBox[2]= 67; P[Player].dataHBox[3]=-70; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 103:
				reset_HBox=1; 
				reset_BBox=1;
				break;
				
				case 104:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=-110; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-85; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 105:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 95; P[Player].dataHBox[2]= 95; P[Player].dataHBox[3]=-75; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 106:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 35; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 90; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-70; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 113:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==5){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 90; P[Player].dataHBox[2]= 65; P[Player].dataHBox[3]=-60; }
				if(P[Player].animFrame==6){ reset_HBox=1; }
				break;
				
				case 151:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 80; P[Player].dataHBox[2]= 45; P[Player].dataHBox[3]=-60; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 152:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 90; P[Player].dataHBox[2]= 65; P[Player].dataHBox[3]=-60; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 154:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 30; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=-10; }
				if(P[Player].animFrame==3){ reset_HBox=1; }
				break;
				
				case 155:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=   0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-130; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=- 50; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 200:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 65; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==1){ reset_HBox=1; }
				break;
				
				case 201:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 65; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 50; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==3){ reset_HBox=1; }
				break;
				
				case 202:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 65; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 90; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-70; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 204:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 65; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 20; P[Player].dataHBox[2]= 65; P[Player].dataHBox[3]=  0; }
				if(P[Player].animFrame==3){ reset_HBox=1; }
				break;
				
				case 205:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 65; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 20; P[Player].dataHBox[2]=100; P[Player].dataHBox[3]=  0; }
				if(P[Player].animFrame==3){ reset_HBox=1; }
				break;
				
				case 300:
				case 310:
				case 320:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=-60; }
				
				case 301:
				case 311:
				case 321:
				case 303:
				case 313:
				case 323:
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 80; P[Player].dataHBox[2]= 40; P[Player].dataHBox[3]=-40; }
				break;
				
				case 302:
				case 312:
				case 322:
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 50; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 304:
				case 305:
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 65; P[Player].dataHBox[2]= 75; P[Player].dataHBox[3]=-45; }
				break;
				
				case 314:
				case 324:
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 40; P[Player].dataHBox[2]= 50; P[Player].dataHBox[3]=  0; }
				break;
				
				case 315:
				case 325:
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 40; P[Player].dataHBox[2]= 50; P[Player].dataHBox[3]=-10; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
				
				case 306:
				case 316:
				case 326:
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=-30; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 50; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
				
				case 601:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 80; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==1){ reset_HBox=1; }
				break;
				
				case 602:
				case 603:
				case 604:
				case 605:
				case 606:
				reset_HBox=1;
				break;
				
				case 710:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 50; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-100; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=-70; }
				if(P[Player].animFrame==8){ reset_HBox=1; }
				if(P[Player].animFrame==4){ reset_BBox=1; }
				if(P[Player].animFrame==8){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
				
				case 720:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==5){ P[Player].dataHBox[0]=-30; P[Player].dataHBox[1]=-100; P[Player].dataHBox[2]= 40; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==6){ P[Player].dataHBox[0]=-30; P[Player].dataHBox[1]=-130; P[Player].dataHBox[2]= 40; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==7){ P[Player].dataHBox[0]=-30; P[Player].dataHBox[1]=-100; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==8){ reset_HBox=1; }
				if(P[Player].animFrame==5){ reset_BBox=1; }
				if(P[Player].animFrame==8){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
				
				case 730:
				if(P[Player].animFrame== 1){ reset_BBox=1; }
				if(P[Player].animFrame== 7){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=-120; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=-70; }
				if(P[Player].animFrame== 3){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 80; P[Player].dataHBox[2]= 50; P[Player].dataHBox[3]=-60; }
				if(P[Player].animFrame== 4){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=-130; P[Player].dataHBox[2]= 40; P[Player].dataHBox[3]=-70; }
				if(P[Player].animFrame== 5){ reset_HBox=1; }
				break;

				//----------------------------------------------------------------
				//Agarrao
				case 800:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 90; P[Player].dataHBox[2]= 65; P[Player].dataHBox[3]=-60; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;

				case 801:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 90; P[Player].dataHBox[2]= 65; P[Player].dataHBox[3]=-60; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;

				case 802:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 65; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 20; P[Player].dataHBox[2]=100; P[Player].dataHBox[3]=  0; }
				if(P[Player].animFrame==3){ reset_HBox=1; }
				break;

				case 803:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
				//----------------------------------------------------------------
			}
		break;
	}
	
	if(P[Player].direcao==-1)//P[Player].dataHBox[0]<P[Player].dataHBox[2]) //Normaliza a box
	{
		s16 a;
		a = P[Player].dataHBox[0];
		P[Player].dataHBox[0] = P[Player].dataHBox[2];
		P[Player].dataHBox[2] = a;
		P[Player].dataHBox[0] = P[Player].dataHBox[0] * P[Player].direcao;
		P[Player].dataHBox[2] = P[Player].dataHBox[2] * P[Player].direcao;
	}
	
	//padroes
	if(P[Player].state==100 && P[Player].animFrame==1){ reset_HBox=1; } //parado
	//if( (P[Player].state==471 || P[Player].state==472) && P[Player].animFrame==1){ reset_BBox=1; } //esquiva dash
	
	if( (P[Player].state==471 || P[Player].state==472) && P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
	
	if( (P[Player].state>=610 && P[Player].state<=618) /*&& P[Player].animFrame==1*/ ){ reset_BBox=1; } //win poses, time over; rage explosion
	if( (P[Player].state==550) /*&& P[Player].animFrame==1*/ ){ reset_BBox=1; } //win poses, time over; rage explosion
	
	if(P[Player].state==300 || P[Player].state==310 || P[Player].state==320)
	{ 
		P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=-120; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=-70; 
		reset_HBox=1; 
	}
	
	if(P[Player].state==606){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
	
	if(Player==1 && (P[2].state==110 || P[2].state==210)){ reset_HBox=1; }
	if(Player==2 && (P[1].state==110 || P[1].state==210)){ reset_HBox=1; }
	
	//resets
	if(reset_HBox==1){ P[Player].dataHBox[0]=0; P[Player].dataHBox[1]=0; P[Player].dataHBox[2]=0; P[Player].dataHBox[3]=0; } //Hit Boxes
	if(reset_BBox==1){ P[Player].dataBBox[0]=0; P[Player].dataBBox[1]=0; P[Player].dataBBox[2]=0; P[Player].dataBBox[3]=0; } //Body Boxes
	
}

void FUNCAO_PHYSICS() //FISICA!
{
	for(i=1; i<=2; i++)
	{
		/*KOF94*/
		//-----------------------------------------------------------------------------------------
		if(P[i].id==1) //RYO
		{
			
			//106
			if(P[i].state==106 && P[i].hitPause==0)
			{
				if(P[i].animFrame==3){ if(gPing2==1){ P[i].x+=1*P[i].direcao; } }
			}
			
			//velocidade da magia - Hadouken Type
			if(P[i].state==700 && P[i].animFrame==5)
			{
				if(P[i].attackPower==1){ P[i].fball.speedX = 2; }
				if(P[i].attackPower==2){ P[i].fball.speedX = 4; }
			}
			
			//710
			if(P[i].state==710 && P[i].hitPause==0)
			{
				if(P[i].animFrame>=4 && P[i].animFrame<=7)
				{
					//X
					if(P[i].attackPower==1){ if(P[i].direcao==1){ P[i].x+=4; }else{ P[i].x-=4; } }
					if(P[i].attackPower==2){ if(P[i].direcao==1){ P[i].x+=7; }else{ P[i].x-=7; } }
					
					//Y
					if(P[i].animFrame==4){ P[i].y-=2; }
					if(P[i].animFrame==5)
					{ 
						if(P[i].frameTimeAtual==1){ P[i].y--; }
						if(P[i].frameTimeAtual==2){ P[i].y--; }
						if(P[i].frameTimeAtual==3){  }
						if(P[i].frameTimeAtual==4){  }
					}
					if(P[i].animFrame==6)
					{ 
						if(P[i].frameTimeAtual==1){  }
						if(P[i].frameTimeAtual==2){  }
						if(P[i].frameTimeAtual==3){ P[i].y++; }
						if(P[i].frameTimeAtual==4){ P[i].y++; }
					}
					if(P[i].animFrame==7){ P[i].y+=2; }
				}
			}
			
			//720
			if(P[i].state==720 && P[i].hitPause==0) 
			{
				//X
				if(P[i].animFrame==5)
				{
					if(P[i].direcao==1){ P[i].x+=9; }else{ P[i].x-=9; }
				}
				if(P[i].animFrame>=6 && P[i].animFrame<=7)
				{
					if(P[i].direcao==1){ P[i].x+=7; }else{ P[i].x-=7; }
				}
				if(P[i].animFrame==8)
				{
					if(P[i].direcao==1){ P[i].x+=3; }else{ P[i].x-=3; }
				}
				//Y
				if(P[i].animFrame>=5 && P[i].animFrame<=6)
				{
					if(P[i].attackPower==1){ P[i].y-=1; }
					if(P[i].attackPower==2){ P[i].y-=3; }
				}
				if(P[i].animFrame>=7 && P[i].animFrame<=8)
				{
					P[i].y+=3;
				}
				
				if(P[i].y>gAlturaPiso)
				{
					P[i].y=gAlturaPiso;
					PLAYER_STATE(i, 606);
				}
				
			}
			
			//730 - Roris Type
			if(P[i].state==730 && P[i].hitPause==0) 
			{
				if (i == 1) { //P1
					//Y
					if (P[i].animFrame >= 4) {
						if (P[i].animFrame == 4 && P[i].setup) {
							P[i].gravidadeY = 2;

							if(P[i].attackPower == 1){
								P[i].impulsoY = -14;
							} else {
								P[i].impulsoY = -18;
							}

							P[i].setup = FALSE;
						}

						if (P[i].animFrame == 5 && P[i].frameTimeAtual == 1) {
							P[i].impulsoY = 0;
						}

						P[i].cicloInteracoesGravidadeCont++;
						if(P[i].cicloInteracoesGravidadeCont >= P[i].cicloInteracoesGravidade + 1){
							P[i].cicloInteracoesGravidadeCont = 0;

							P[i].y += P[i].impulsoY;
							P[i].impulsoY += P[i].gravidadeY;
						}
					}
				}

				if (i == 2) { //P2
					//Y
					if(P[i].animFrame < 5){
						if(P[i].animFrame==4){ P[i].y-=4; }
					}else{
						if(P[i].animFrame==5){ P[i].y+=0; }
						if(P[i].animFrame==6){ P[i].y+=2; }
						if(P[i].animFrame==7){ P[i].y+=4; }
						if(P[i].animFrame==8){ P[i].y+=6; }
					}
				}

				//X
				if(P[i].animFrame == 4){
					if(P[i].attackPower == 1){
						P[i].x += 1 * P[i].direcao; 
					}else{
						P[i].x += 2 * P[i].direcao; 
					}
				}

				if (P[i].y > gAlturaPiso) {
					P[i].y = gAlturaPiso;
					PLAYER_STATE(i, 606); 
				}
			}
		}
		//-----------------------------------------------------------------------------------------
		
		//Hits Movement, deslocamento dos Players para tras, durante os ataques
		if(P[i].state>=500 && P[i].state<=550 && P[i].hitPause==0)
		{
			u8 PA=0; //Player que Ataca
			u8 PR=0; //Player que Recebe o Ataque
			if(i == 1){ PA = 2; PR = 1; }else{ PA = 1; PR = 2; }
			
			//Nota, sem as condicoes abaixo, o deslocamento sera continuo, enquanto durar a animacao de Hit
			//Esse algoritimo simples simula a desaceleracao, ou friccao, causada pelo atrito com o solo
			if(P[PR].animFrame<=3)
			{ 
				if(P[PR].animFrame==1) {
					P[PR].hSpeed = 3;
					if(P[PA].state>=301 && P[PA].state<=326){
						P[PR].hSpeed = 2;
					} 
				} //vel 2 ou 3
				if(P[PR].animFrame==2) {
					P[PR].hSpeed = 2; 
				} //vel 2
				if(P[PR].animFrame==3) {
					P[PR].hSpeed = 1; 
				} //vel 1
				
				//ryo; state:730
				if((P[PA].id==1 && P[PA].state==730 && P[PA].animFrame<=3))
				{
					P[PR].hSpeed=1;
				}
				
				P[PR].x += P[PR].hSpeed*(P[PR].direcao*-1);
			}
			
			//Limites laterais
			if(P[PR].x >= gBG_Width-30 && P[PR].stateMoveType == 2)
			{
				if (P[PA].attackPower==1) { 
					P[PA].hSpeed = 2; 
				} else { 
					P[PA].hSpeed = 4; 
				}
				if (gPing2 == 1 && P[PR].state != 550) {
					P[PA].x += P[PA].hSpeed*(P[PA].direcao*-1); 
				}
			}
			if(P[PR].x <= 30 && P[PR].stateMoveType == 2)
			{
				if (P[PA].attackPower==1) {
					P[PA].hSpeed = 2;
				} else {
					P[PA].hSpeed = 4;
				}
				if (gPing2 == 1 && P[PR].state != 550) {
					P[PA].x += P[PA].hSpeed*(P[PA].direcao*-1);
				}
			}
		}
		
		//andar
		if(P[i].state==410 || P[i].state==420)
		{
			P[i].hSpeed=2;
			if(P[i].state==410){P[i].x+=P[i].hSpeed*(P[i].direcao*-1);}
			if(P[i].state==420){P[i].x+=P[i].hSpeed*(P[i].direcao);}
		}
		
		//esquiva frente e tras
		if(P[i].state==471 || P[i].state==472)
		{
			if(P[i].animFrame>=1){ P[i].hSpeed=6; if(P[i].state==471){ P[i].hSpeed=7; } }
			if(P[i].animFrame>=2){ P[i].hSpeed=3; }
			if(P[i].animFrame>=3){ P[i].hSpeed=2; }
			if(P[i].animFrame>=4){ P[i].hSpeed=1; } //anula o deslocamento
			
			if(P[i].id==5)
			{
				if(P[i].animFrame>=1){ P[i].hSpeed=0; if(P[i].state==471){ P[i].hSpeed=0; } }
				if(P[i].animFrame>=2){ P[i].hSpeed=6; }
				if(P[i].animFrame>=3){ P[i].hSpeed=6; }
				if(P[i].animFrame>=4){ P[i].hSpeed=4; } //anula o deslocamento
			}
			
			if(P[i].state==471){P[i].x+=P[i].hSpeed*(P[i].direcao*-1);}
			if(P[i].state==472){P[i].x+=P[i].hSpeed*(P[i].direcao);}
		}
		
		//defesas
		if( (P[i].state==110 || P[i].state==210) && P[i].hitPause==0)
		{
			P[i].hSpeed=4;
			if(P[i].state==110){P[i].x+=P[i].hSpeed*(P[i].direcao*-1);}//defesa em pe
			if(P[i].state==210){P[i].x+=P[i].hSpeed*(P[i].direcao*-1);}//defesa abaixado
			
			u8 PA=0; //Player que Ataca
			u8 PR=0; //Player que Recebe o Ataque
			if(i == 1){ PA = 2; PR = 1; }else{ PA = 1; PR = 2; }
			
			//Limites laterais
			if(P[PR].x >= gBG_Width-30 && P[PR].stateMoveType == 2)
			{
				if(P[PA].attackPower==1){
					P[PA].hSpeed = 3;
				}else{
					P[PA].hSpeed = 5; 
				} 
				if(gPing2 == 1){ 
					P[PA].x += P[PA].hSpeed*(P[PA].direcao*-1); 
				}
			}
			if(P[PR].x <= 30 && P[PR].stateMoveType == 2)
			{
				if(P[PA].attackPower==1){ 
					P[PA].hSpeed = 3; 
				}else{ 
					P[PA].hSpeed = 5; 
				} 
				if(gPing2 == 1){
					P[PA].x += P[PA].hSpeed*(P[PA].direcao*-1); 
				}
			}
		}
		
		//pulos
		if(P[i].hitPause==0)
		{
			if(P[i].puloTimer!=0){ P[i].puloTimer++; }

			if (i == 1) { //P1
				if (P[i].state >= 300 && P[i].state <= 326) {

					if (P[i].puloTimer == 2) {
						P[i].gravidadeY = 2;
						P[i].impulsoY = -20;
						P[i].hSpeed = 3;
						P[i].shotJump = FALSE;
					}

					if (P[i].puloTimer == 8 && P[i].key_JOY_UP_status == 0) {
						P[i].shotJump = TRUE;
						P[i].impulsoY = -8;
					}

					P[i].cicloInteracoesGravidadeCont++;
					if(P[i].cicloInteracoesGravidadeCont >= P[i].cicloInteracoesGravidade + 1){
						P[i].cicloInteracoesGravidadeCont = 0;

						P[i].y += P[i].impulsoY;
						P[i].impulsoY += P[i].gravidadeY;
					}

					if(P[i].state >= 320 && P[i].state <= 326){
						P[i].x += P[i].hSpeed *(P[i].direcao);
					}
					if(P[i].state >= 310 && P[i].state <= 316){
						P[i].x += P[i].hSpeed *(P[i].direcao*-1);
					}
				}
			}

			if (i == 2) { //P2
				if(P[i].state>=300 && P[i].state<=326)
				{ 
					P[i].hSpeed=3;
					/*samsho2*/ //hard code :P
					//o metodo a seguir eh extremamente preciso, mas exige notacao manual!
					//para outros metodos de movimento, ver fisica no state 550, ou outros golpes por exemplo
					if(P[i].puloTimer== 1){ P[i].y-=11; P[i].shotJump=FALSE; }
					if(P[i].puloTimer== 2){ P[i].y-=11; P[i].shotJump=FALSE; }
					if(P[i].puloTimer== 3){ P[i].y-=10; }
					if(P[i].puloTimer== 4){ P[i].y-= 9; }
					if(P[i].puloTimer== 5){ P[i].y-= 9; }
					if(P[i].puloTimer== 6){ P[i].y-= 8; }
					if(P[i].puloTimer== 7){ P[i].y-= 8; }
					if(P[i].puloTimer== 8){ P[i].y-= 8; }
					
					if(P[i].puloTimer== 8){ if(P[i].key_JOY_UP_status==0){ P[i].shotJump=TRUE; } }
					
					//pulo curto
					if(P[i].shotJump==TRUE)
					{
						if(P[i].puloTimer== 9){ P[i].y-= 4; }
						if(P[i].puloTimer==10){ P[i].y-= 2; }
						if(P[i].puloTimer==11){ P[i].y-= 2; } 
						if(P[i].puloTimer==12){ P[i].y-= 1; }
						if(P[i].puloTimer==13){ P[i].y-= 1; }
						if(P[i].puloTimer==14){ P[i].y+= 0; }
						if(P[i].puloTimer==15){ P[i].y+= 0; }
						if(P[i].puloTimer==16){ P[i].y+= 0; }
						if(P[i].puloTimer==17){ P[i].y+= 0; }
						if(P[i].puloTimer==18){ P[i].y+= 1; }
						if(P[i].puloTimer==19){ P[i].y+= 1; }
						if(P[i].puloTimer==20){ P[i].y+= 2; }
						if(P[i].puloTimer==21){ P[i].y+= 2; }
						if(P[i].puloTimer==22){ P[i].y+= 2; }
						if(P[i].puloTimer==23){ P[i].y+= 2; }
						if(P[i].puloTimer==24){ P[i].y+= 4; }
						if(P[i].puloTimer==25){ P[i].y+= 4; } 
						if(P[i].puloTimer==26){ P[i].y+= 6; }
						if(P[i].puloTimer==27){ P[i].y+= 7; }
						if(P[i].puloTimer==28){ P[i].y+= 8; }
						if(P[i].puloTimer==29){ P[i].y+= 9; }
						if(P[i].puloTimer==30){ P[i].y+= 10; }
						if(P[i].puloTimer==31){ P[i].y+= 11; }
						if(P[i].puloTimer==32){ P[i].y+= 11; }
						if(P[i].puloTimer==33){ P[i].puloTimer=45; P[i].y+= 1; }
					}
					
					//pulo longo
					if(P[i].shotJump==FALSE)
					{
						if(P[i].puloTimer== 9){ P[i].y-= 7; }
						if(P[i].puloTimer==10){ P[i].y-= 6; } 
						if(P[i].puloTimer==11){ P[i].y-= 6; }
						if(P[i].puloTimer==12){ P[i].y-= 5; }
						if(P[i].puloTimer==13){ P[i].y-= 5; }
						if(P[i].puloTimer==14){ P[i].y-= 4; }
						if(P[i].puloTimer==15){ P[i].y-= 4; }
						if(P[i].puloTimer==16){ P[i].y-= 3; }
						if(P[i].puloTimer==17){ P[i].y-= 3; }
						if(P[i].puloTimer==18){ P[i].y-= 3; }
						if(P[i].puloTimer==19){ P[i].y-= 1; }
						if(P[i].puloTimer==20){ P[i].y-= 2; } 
						if(P[i].puloTimer==21){ P[i].y-= 0; }
						if(P[i].puloTimer==22){ P[i].y-= 1; }
						if(P[i].puloTimer==23){ P[i].y+= 1; }
						if(P[i].puloTimer==24){ P[i].y+= 0; }
						if(P[i].puloTimer==25){ P[i].y+= 2; }
						if(P[i].puloTimer==26){ P[i].y+= 1; }
						if(P[i].puloTimer==27){ P[i].y+= 3; }
						if(P[i].puloTimer==28){ P[i].y+= 3; }
						if(P[i].puloTimer==29){ P[i].y+= 3; }
						if(P[i].puloTimer==30){ P[i].y+= 4; } 
						if(P[i].puloTimer==31){ P[i].y+= 4; }
						if(P[i].puloTimer==32){ P[i].y+= 5; }
						if(P[i].puloTimer==33){ P[i].y+= 5; }
						if(P[i].puloTimer==34){ P[i].y+= 6; }
						if(P[i].puloTimer==35){ P[i].y+= 6; }
						if(P[i].puloTimer==36){ P[i].y+= 7; }
						if(P[i].puloTimer==37){ P[i].y+= 8; }
						if(P[i].puloTimer==38){ P[i].y+= 8; }
						if(P[i].puloTimer==39){ P[i].y+= 8; }
						if(P[i].puloTimer==40){ P[i].y+= 9; } 
						if(P[i].puloTimer==41){ P[i].y+= 9; }
						if(P[i].puloTimer==42){ P[i].y+=10; }
						if(P[i].puloTimer==43){ P[i].y+=11; }
						if(P[i].puloTimer==44){ P[i].y+=11; }
						if(P[i].puloTimer==45){ P[i].y+= 1; }
					}
					
					if(P[i].state>=320 && P[i].state<=326){ P[i].x+=P[i].hSpeed*(P[i].direcao); }
					if(P[i].state>=310 && P[i].state<=316){ P[i].x+=P[i].hSpeed*(P[i].direcao*-1); }
				}
				
				if(P[i].state==1310)
				{
					P[i].hSpeed=4;
					
					if(P[i].puloTimer== 1){ P[i].y-=6; }
					if(P[i].puloTimer== 2){ P[i].y-=6; }
					if(P[i].puloTimer== 3){ P[i].y-=6; }
					if(P[i].puloTimer== 4){ P[i].y-=6; }
					if(P[i].puloTimer== 5){ P[i].y-=4; }
					if(P[i].puloTimer== 6){ P[i].y-=4; }
					if(P[i].puloTimer== 7){ P[i].y-=4; }
					if(P[i].puloTimer== 8){ P[i].y-=4; }
					if(P[i].puloTimer== 9){ P[i].y+=4; }
					if(P[i].puloTimer==10){ P[i].y+=4; } 
					if(P[i].puloTimer==11){ P[i].y+=4; }
					if(P[i].puloTimer==12){ P[i].y+=4; }
					if(P[i].puloTimer==13){ P[i].y+=6; }
					if(P[i].puloTimer==14){ P[i].y+=6; }
					if(P[i].puloTimer==15){ P[i].y+=6; }
					if(P[i].puloTimer==16){ P[i].y=gAlturaPiso; PLAYER_STATE(i, 606); P[i].puloTimer=0; }
					
					P[i].x+=P[i].hSpeed*(P[i].direcao*-1);
				}
			}
		}
		
		/*samsho2*/
		//'516' CAINDO (Apos Magia do Haohmaru)
		if( P[i].hitPause==0)
		{
			P[i].cicloInteracoesGravidadeCont++;
			
			if(P[i].state==516)
			{
				if(P[i].cicloInteracoesGravidadeCont>=P[i].cicloInteracoesGravidade){
					P[i].cicloInteracoesGravidadeCont=0;
					P[i].impulsoY = P[i].impulsoY+P[i].gravidadeY;
					P[i].impulsoY-=1;
				}
				P[i].y += P[i].impulsoY;
				if(gPing2==1){ if(P[i].direcao==1){ P[i].x-=1; }else{ P[i].x+=1; } }
			}
		}
		
		//'516' > Altura Piso
		if(P[i].state==516 && P[i].y>gAlturaPiso)
		{
			P[i].y=gAlturaPiso-1;
			P[i].gravidadeY = gravidadePadrao;
			P[i].impulsoY = -6;//-14
			PLAYER_STATE(i,551);
		}
		
		//'550' CAINDO
		if( P[i].hitPause==0)
		{
			P[i].cicloInteracoesGravidadeCont++;
			
			if(P[i].state==550)
			{
				if(P[i].cicloInteracoesGravidadeCont>=P[i].cicloInteracoesGravidade){
					P[i].cicloInteracoesGravidadeCont=0;
					P[i].impulsoY = P[i].impulsoY+P[i].gravidadeY;
					P[i].impulsoY-=1;
				}
				P[i].y += P[i].impulsoY;
				if(P[i].direcao==1){ P[i].x-=2; }else{ P[i].x+=2; }
			}
		}
		
		//'550' > Altura Piso
		if(P[i].state==550 && P[i].y>gAlturaPiso)
		{
			P[i].y=gAlturaPiso-1;
			P[i].gravidadeY = gravidadePadrao;
			P[i].impulsoY = -6;//-14
			PLAYER_STATE(i,551);
		}
		
		if(P[i].state==551){
			if(P[i].cicloInteracoesGravidadeCont>=P[i].cicloInteracoesGravidade){
				P[i].cicloInteracoesGravidadeCont=0;
				P[i].impulsoY = P[i].impulsoY+P[i].gravidadeY;
				P[i].impulsoY-=1;
			}
			P[i].y += P[i].impulsoY;
			if(P[i].direcao==1){ P[i].x-=2; }else{ P[i].x+=2; }
		}
		
		//'551' > Altura Piso
		if(P[i].state==551 && P[i].y>gAlturaPiso)
		{
			P[i].y=gAlturaPiso;
			PLAYER_STATE(i,570);
		}
		
		//-----------------------------------------------------------------------------------------
		
		//'MAGIAS' FISICA
		/*
		if( P[i].hitPause==0)
		{
			//P[i].cicloInteracoesGravidadeCont++;
			
			if( (P[i].state==710 || P[i].state==720 || P[i].state==730 || P[i].state==740 || P[i].state==750) && P[i].y!=gAlturaPiso )
			{
				if(P[i].cicloInteracoesGravidadeCont>=P[i].cicloInteracoesGravidade){
					P[i].cicloInteracoesGravidadeCont=0;
					P[i].impulsoY = P[i].impulsoY+P[i].gravidadeY;
					P[i].impulsoY-=1;
				}
				P[i].y += P[i].impulsoY;
				if(P[i].direcao==-1){ P[i].x-=2; }else{ P[i].x+=2; }
			}
		}
		*/
		
		//'MAGIAS' FISICA > Altura Piso
		//HAOHMARU (magia 720)
		/*
		if( (P[i].state==720) && P[i].y+2>gAlturaPiso)
		{
			P[i].y = gAlturaPiso;
			P[i].frameTimeAtual = P[i].frameTimeTotal; //Passa para o proximo frame de animacao
		}
		*/
		
		//-----------------------------------------------------------------------------------------
		
		//Movimento das Magias
		if(P[i].fball.active == 1)
		{
			P[i].fball.x += P[i].fball.speedX * P[i].fball.direcao;
			SPR_setPosition( P[i].fball.spriteFBall, P[i].fball.x-camPosX, P[i].fball.y );
			//if(P[i].fBallX<gLimiteCenarioE-100){ SPR_releaseSprite(P[i].fBall); P[i].fBallActive=0; P[i].fBallX=-255; P[i].fBallY=-255; }
			//if(P[i].fBallX>gLimiteCenarioD+100){ SPR_releaseSprite(P[i].fBall); P[i].fBallActive=0; P[i].fBallX=-255; P[i].fBallY=-255; }

			if((P[i].fball.x < camPosX-60) || (P[i].fball.x > camPosX+320-20)) {
				SPR_releaseSprite(P[i].fball.spriteFBall); 
				P[i].fball.spriteFBall = NULL; 
				P[i].fball.active=0; 
				P[i].fball.x=-255; 
				P[i].fball.y=-255; 
			}
		}

		//hitpause shake
		if(P[i].hitPause>0)
		{
			if(P[i].energiaBase>0 && (P[i].stateMoveType == 2 || P[i].stateMoveType == 3))
			{
				if(gPing2==0){ P[i].x-=2; }
				if(gPing2==1){ P[i].x+=2; }
			}			
			P[i].hitPause--;
		}

		//Colisao Mass Boxes
		if(P[1].state!=472 && P[2].state!=472)
		{
			if(FUNCAO_COLISAO(
				P[1].x+P[1].dataMBox[0], P[1].y+P[1].dataMBox[1], P[1].x+P[1].dataMBox[2], P[1].y+P[1].dataMBox[3], 
				P[2].x+P[2].dataMBox[0], P[2].y+P[2].dataMBox[1], P[2].x+P[2].dataMBox[2], P[2].y+P[2].dataMBox[3]
			)){

				if (P[1].y < gAlturaPiso || P[2].y < gAlturaPiso) { //Um dos lutadores esta no ar

					u8 PAir = 0; //Player que esta no ar
					u8 PGround = 0; //Player que esta no chao
					if(P[1].y < gAlturaPiso){
						PAir = 1; PGround = 2;
					}else{
						PAir = 2; PGround = 1;
					}

					if (P[PAir].x <= P[PGround].x) {
						P[PAir].x -= 5;
					} else if (P[PAir].x >= P[PGround].x) {
						P[PAir].x += 5;
					}

				} else { //Os dois lutadoes estao no chao
					u8 tempBigHSpeed=0;

					if(P[1].hSpeed >= P[2].hSpeed){
						tempBigHSpeed = P[1].hSpeed;
					}else{
						tempBigHSpeed = P[2].hSpeed;
					}
					if(tempBigHSpeed == 0){
						tempBigHSpeed = 5;
					}

					if(P[1].x < P[2].x){
						P[1].x -= tempBigHSpeed;
						P[2].x += tempBigHSpeed;
					}else{
						P[1].x += tempBigHSpeed;
						P[2].x -= tempBigHSpeed;
					}
				}
			}
		}

		//-----------------------------------------------------------------------------------------
		//corrige posX //limites do cenario //Essa parte tem que ficar aqui no final
		//metoo novo, hamoopig 1.1
		if (P[i].x < 30) {
			P[i].x = 30;
			if (P[i].y < gAlturaPiso) {
				P[i].x += 5;
			}
		}
		if(P[i].x > gBG_Width-30) {
			P[i].x = gBG_Width-30;
			if (P[i].y < gAlturaPiso) {
				P[i].x -= 5;
			}
		}

		if (P[1].x <= P[2].x) {
			gDistancia = P[2].x-P[1].x;
		} else {
			gDistancia = P[1].x-P[2].x;
		}
		
		if (gDistancia > 260) { //Verifica a distancia maxima entre os lutadores
			if (P[1].x < P[2].x) {
				if (i == 1) {
					P[1].x += gDistancia - 260;
				} else if (i == 2) {
					P[2].x -= gDistancia - 260;
				}
			} else {
				if (i == 1) {
					P[1].x -= gDistancia - 260;
				} else if (i == 2) {
					P[2].x += gDistancia - 260;
				}
			}
		}
	}
}

bool FUNCAO_CHECK_GUARD(u8 guardFlag, u8 enemy)
{
	bool retorno = FALSE;
	if((guardFlag == 1 || guardFlag == 3) && 
	(
		(P[enemy].state==107 || P[enemy].state==108 || P[enemy].state==109 || P[enemy].state==110) || 
		(P[enemy].state==100 && P[enemy].direcao== 1 && (P[enemy].key_JOY_LEFT_status ==1 || P[enemy].key_JOY_LEFT_status ==2)) || 
		(P[enemy].state==100 && P[enemy].direcao==-1 && (P[enemy].key_JOY_RIGHT_status==1 || P[enemy].key_JOY_RIGHT_status==2)) 
	)){
		retorno = TRUE;
	}
	if((guardFlag == 1 || guardFlag == 2) && 
	(
		(P[enemy].state==207 || P[enemy].state==208 || P[enemy].state==209 || P[enemy].state==210) || 
		(P[enemy].state==200 && P[enemy].direcao== 1 && (P[enemy].key_JOY_LEFT_status ==1 || P[enemy].key_JOY_LEFT_status ==2)) || 
		(P[enemy].state==200 && P[enemy].direcao==-1 && (P[enemy].key_JOY_RIGHT_status==1 || P[enemy].key_JOY_RIGHT_status==2)) 
	)){
		retorno = TRUE;
	}
	return retorno;
}

void FUNCAO_CAMERA_BGANIM()
{
	//---------------------------------------------------------------------------------------------new cam ini
	//Metodo novo, hamoopig 1.1
	//Calcula o meio da tela, para centralizar a camera
	if (P[1].x > P[2].x) {
		gMeioDaTela = (P[2].x + ((P[1].x - P[2].x)/2)) - (320 / 2);
	} else {
		gMeioDaTela = (P[1].x + ((P[2].x - P[1].x)/2)) - (320 / 2);
	}

	//Posiciona a camera
	camPosX = gMeioDaTela;

	//Limites do cenario
	if( camPosX < 0 ){ camPosX = 0; } 
	if( camPosX > gBG_Width-320 ){ camPosX = gBG_Width-320; }

	//Movimenta o BG
	if(camPosXanterior != camPosX) {
		VDP_setHorizontalScroll(BG_B, camPosX*-1);
		camPosXanterior=camPosX;
	}
	//---------------------------------------------------------------------------------------------new cam end
}

void FUNCAO_DEPTH(u8 Player)
{
	//Qto menor o 'depth', mais para frente o objeto fica na tela
	u8 depth;
	if(P[Player].state>=500 && P[Player].state<=572)                         {depth = Player+20;} //Hurts, Caindo
	if(P[Player].state==100)                                                 {depth = Player+18;} //Parado
	if(P[Player].state==471 || P[Player].state==472)                         {depth = Player+16;} //Rolamento, Dash, Esquiva
	if(P[Player].state==410 || P[Player].state==420)                         {depth = Player+14;} //Andando
	if(P[Player].state>=107 && P[Player].state<=110)                         {depth = Player+12;} //Defesa em Pe
	if(P[Player].state>=207 && P[Player].state<=210)                         {depth = Player+12;} //Defesa Abaixado
	if(P[Player].state>=101 && P[Player].state<=156)                         {depth = Player+ 8;} //Golpes em Pe
	if(P[Player].state>=200 && P[Player].state<=206)                         {depth = Player+ 6;} //Golpes Abaixados
	if(P[Player].state==300 || P[Player].state==310 || P[Player].state==320) {depth = Player+ 4;} //Pulos
	if(P[Player].state>=301 && P[Player].state<=326)                         {depth = Player+ 2;} //Golpes Aereos
	if(P[Player].state>=611 && P[Player].state<=614)                         {depth = Player+ 2;} //Vitoria
	if(P[Player].state>=700 && P[Player].state<=790)                         {depth = Player+ 2;} //Magias
	//define o Depth, SE e SOMENTE SE o sprite estiver carregado. 
	//O comando 'SPR_setDepth' pode ser potencialmente perigoso e TRAVAR o jogo se o SPRITE nao estiver completamente carregado na VRAM
	if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, depth); }
}

void FUNCAO_SAMSHOFX() //ESPECIFICO DO SAMURAI SHODOWN 2
{
	//ESPECIFICO DE SAMURAI SHODOW. AJUSTE DA BARRA DE RAGE
	if(P[1].energiaSP ==  0){  P[1].rageLvl=0; }
	if(P[1].energiaSP >   0 && P[1].energiaSP<=15){ P[1].rageLvl=0; }
	if(P[1].energiaSP >= 16 && P[1].energiaSP<=31){ P[1].rageLvl=1; }
	if(P[1].energiaSP >= 32){  P[1].rageLvl=2; }
	
	if(P[2].energiaSP ==  0){  P[2].rageLvl=0; }
	if(P[2].energiaSP >   0 && P[2].energiaSP<=15){ P[2].rageLvl=0; }
	if(P[2].energiaSP >= 16 && P[2].energiaSP<=31){ P[2].rageLvl=1; }
	if(P[2].energiaSP >= 32){  P[2].rageLvl=2; }
	
	//ESPECIFICO DO SAMURAI SHODOW. ESPADA PISCANDO E SOMBRA
	/*
	if(gSombraStyle==1)
	{
		if(gPing2==0)
		{ 
			//P1
			if(gSombraStyle==2){ SPR_setVisibility(P[1].sombra, VISIBLE); }
			//P2
			if(gSombraStyle==2){ SPR_setVisibility(P[2].sombra, HIDDEN); }
		}
		if(gPing2==1)
		{ 
			//P1
			if(gSombraStyle==2){ SPR_setVisibility(P[1].sombra, HIDDEN); }
			//P2
			if(gSombraStyle==2){ SPR_setVisibility(P[2].sombra, VISIBLE); }
		}
	}
	*/
	
}

void FUNCAO_CHECK_FRAME_ADVANTAGE() {
	if ((P[1].control == TRUE  && P[2].control == TRUE  && frameAdvCounterP1 != 0) ||
	    (P[1].control == FALSE && P[2].control == FALSE && frameAdvCounterP1 != 0)) {
		
		//P!
		lastFrameAdvCounterP1 = frameAdvCounterP1;
		frameAdvCounterP1 = 0;

		//P2
		lastFrameAdvCounterP2 = frameAdvCounterP2;
		frameAdvCounterP2 = 0;

	} else {

		if (P[1].control && !P[2].control) {
			frameAdvCounterP1++;
			frameAdvCounterP2--;
			
		} else if (!P[1].control && P[2].control) {
			frameAdvCounterP1--;
			frameAdvCounterP2++;
			
		}
	}
}

void FUNCAO_DEBUG()
{
	//Atencao! O debug só é funcional se você não printar sprites em cima dos seus caracteres alfanuméricos!
	// Procure pelo comando: SPR_setVRAMTileIndex(GE[XX].sprite, 1441); 
	// Onde, 1441 é o comeco dos tiles alfanumericos, e 1535 é o final desses tiles!
	
	//Debug em texto, desativado:
	VDP_drawText("HAMOOPIG ENGINE", 1, 1);
	sprintf(gStr, "P1-> %i,%i S:%i T:%i/%i F:%i/%i    ", P[1].x, P[1].y, P[1].state, P[1].frameTimeAtual, P[1].frameTimeTotal, P[1].animFrame, P[1].animFrameTotal ); 
	sprintf(gStr, "P1-> S:%i T:%i/%i F:%i/%i    ", P[1].state, P[1].frameTimeAtual, P[1].frameTimeTotal, P[1].animFrame, P[1].animFrameTotal ); 
	VDP_drawText(gStr, 1, 2);
	sprintf(gStr, "P2-> S:%i T:%i/%i F:%i/%i    ", P[2].state, P[2].frameTimeAtual, P[2].frameTimeTotal, P[2].animFrame, P[2].animFrameTotal ); 
	VDP_drawText(gStr, 1, 3);
	sprintf(gStr, "UP:%i DOWN:%i LEFT:%i RIGHT:%i ATK_B:%i  ", P[1].joyDirTimer[8], P[1].joyDirTimer[2], P[1].joyDirTimer[4], P[1].joyDirTimer[6], P[1].attackButton ); 
	

	u8 dist = 0;
	if (P[1].x > P[2].x) {
		dist = P[1].x - P[2].x;
	} else {
		dist = P[2].x - P[1].x;
	}

	KLog_U2("P1 state: ", P[1].state, ", P2 state: ", P[2].state);
	KLog_U2("P1 x: ", P[1].x, ", P2 x: ", P[2].x);
	KLog_U2("P1 y: ", P[1].y, ", P2 y: ", P[2].y);
	KLog_U1("Distancia entre P1 e P2: ", dist);
	KLog_U2("P1 hits: ", P[1].hitCounter, ", P2 hits: ", P[2].hitCounter);

	FUNCAO_CHECK_FRAME_ADVANTAGE();
	KLog_S2("P1 frame adv: ", lastFrameAdvCounterP1, ", P2 frame adv: ", lastFrameAdvCounterP2);



	if(RELEASE==0)
	{
		SPR_setPosition(GE[1].sprite, P[1].x-4-camPosX, P[1].y-5);
		SPR_setPosition(GE[2].sprite, P[2].x-4-camPosX, P[2].y-5);
	}
	
	if(RELEASE==0)
	{
		//P1
		//bodyboxes
		if(P[1].dataBBox[0]==0 && P[1].dataBBox[2]==0){
			SPR_setPosition(Rect1BB1_Q1,-8,-8); SPR_setPosition(Rect1BB1_Q2,-8,-8); SPR_setPosition(Rect1BB1_Q3,-8,-8); SPR_setPosition(Rect1BB1_Q4,-8,-8);
		}else{
			SPR_setPosition(Rect1BB1_Q1, P[1].x+P[1].dataBBox[0]  -camPosX, P[1].y+P[1].dataBBox[1]);
			SPR_setPosition(Rect1BB1_Q2, P[1].x+P[1].dataBBox[2]-8-camPosX, P[1].y+P[1].dataBBox[1]);
			SPR_setPosition(Rect1BB1_Q3, P[1].x+P[1].dataBBox[0]  -camPosX, P[1].y+P[1].dataBBox[3]-8);
			SPR_setPosition(Rect1BB1_Q4, P[1].x+P[1].dataBBox[2]-8-camPosX, P[1].y+P[1].dataBBox[3]-8);
		}
		//hitboxes
		if(P[1].dataHBox[0]==0 && P[1].dataHBox[2]==0){
			SPR_setPosition(Rect1HB1_Q1,-8,-8); SPR_setPosition(Rect1HB1_Q2,-8,-8); SPR_setPosition(Rect1HB1_Q3,-8,-8); SPR_setPosition(Rect1HB1_Q4,-8,-8);
		}else{
			SPR_setPosition(Rect1HB1_Q1, P[1].x+P[1].dataHBox[0]  -camPosX, P[1].y+P[1].dataHBox[1]);
			SPR_setPosition(Rect1HB1_Q2, P[1].x+P[1].dataHBox[2]-8-camPosX, P[1].y+P[1].dataHBox[1]);
			SPR_setPosition(Rect1HB1_Q3, P[1].x+P[1].dataHBox[0]  -camPosX, P[1].y+P[1].dataHBox[3]-8);
			SPR_setPosition(Rect1HB1_Q4, P[1].x+P[1].dataHBox[2]-8-camPosX, P[1].y+P[1].dataHBox[3]-8);
		}

		//P2
		//bodyboxes
		if(P[2].dataBBox[0]==0 && P[2].dataBBox[2]==0){
			SPR_setPosition(Rect2BB1_Q1,-8,-8); SPR_setPosition(Rect2BB1_Q2,-8,-8); SPR_setPosition(Rect2BB1_Q3,-8,-8); SPR_setPosition(Rect2BB1_Q4,-8,-8);
		}else{
			SPR_setPosition(Rect2BB1_Q1, P[2].x+P[2].dataBBox[0]  -camPosX, P[2].y+P[2].dataBBox[1]);
			SPR_setPosition(Rect2BB1_Q2, P[2].x+P[2].dataBBox[2]-8-camPosX, P[2].y+P[2].dataBBox[1]);
			SPR_setPosition(Rect2BB1_Q3, P[2].x+P[2].dataBBox[0]  -camPosX, P[2].y+P[2].dataBBox[3]-8);
			SPR_setPosition(Rect2BB1_Q4, P[2].x+P[2].dataBBox[2]-8-camPosX, P[2].y+P[2].dataBBox[3]-8);
		}
		//hitboxes
		if(P[2].dataHBox[0]==0 && P[2].dataHBox[2]==0){
			SPR_setPosition(Rect2HB1_Q1,-8,-8); SPR_setPosition(Rect2HB1_Q2,-8,-8); SPR_setPosition(Rect2HB1_Q3,-8,-8); SPR_setPosition(Rect2HB1_Q4,-8,-8);
		}else{
			SPR_setPosition(Rect2HB1_Q1, P[2].x+P[2].dataHBox[0]  -camPosX, P[2].y+P[2].dataHBox[1]);
			SPR_setPosition(Rect2HB1_Q2, P[2].x+P[2].dataHBox[2]-8-camPosX, P[2].y+P[2].dataHBox[1]);
			SPR_setPosition(Rect2HB1_Q3, P[2].x+P[2].dataHBox[0]  -camPosX, P[2].y+P[2].dataHBox[3]-8);
			SPR_setPosition(Rect2HB1_Q4, P[2].x+P[2].dataHBox[2]-8-camPosX, P[2].y+P[2].dataHBox[3]-8);
		}

		//hitboxes das magias para o P1
		if (P[1].fball.active == 1) {
			SPR_setPosition(Rect1HB1_Q1, P[1].fball.x + P[1].fball.dataHBox[0]   -camPosX, P[1].fball.y + P[1].fball.dataHBox[1]);
			SPR_setPosition(Rect1HB1_Q2, P[1].fball.x + P[1].fball.dataHBox[2] -8-camPosX, P[1].fball.y + P[1].fball.dataHBox[1]);
			SPR_setPosition(Rect1HB1_Q3, P[1].fball.x + P[1].fball.dataHBox[0]   -camPosX, P[1].fball.y + P[1].fball.dataHBox[3] -8);
			SPR_setPosition(Rect1HB1_Q4, P[1].fball.x + P[1].fball.dataHBox[2] -8-camPosX, P[1].fball.y + P[1].fball.dataHBox[3] -8);
		}
		//hitboxes das magias para o P2
		if (P[2].fball.active == 1) {
			SPR_setPosition(Rect1HB1_Q1, P[2].fball.x + P[2].fball.dataHBox[0]   -camPosX, P[2].fball.y + P[2].fball.dataHBox[1]);
			SPR_setPosition(Rect1HB1_Q2, P[2].fball.x + P[2].fball.dataHBox[2] -8-camPosX, P[2].fball.y + P[2].fball.dataHBox[1]);
			SPR_setPosition(Rect1HB1_Q3, P[2].fball.x + P[2].fball.dataHBox[0]   -camPosX, P[2].fball.y + P[2].fball.dataHBox[3] -8);
			SPR_setPosition(Rect1HB1_Q4, P[2].fball.x + P[2].fball.dataHBox[2] -8-camPosX, P[2].fball.y + P[2].fball.dataHBox[3] -8);
		}
		
		//metodo de display HBoxes Info, (piscando) = reduz os tiles simultaneos em tela (DESATIVADO)
		//para facilitar a vida de outros desenvolvedores, decidi manter a exibicao das caixas o tempo todo
		if(gPing2==0)
		{
			/*
			SPR_setVisibility(Rect1HB1_Q1, VISIBLE); SPR_setVisibility(Rect1HB1_Q2, VISIBLE);
			SPR_setVisibility(Rect1HB1_Q3, VISIBLE); SPR_setVisibility(Rect1HB1_Q4, VISIBLE);
			SPR_setVisibility(Rect1BB1_Q1, VISIBLE); SPR_setVisibility(Rect1BB1_Q2, VISIBLE);
			SPR_setVisibility(Rect1BB1_Q3, VISIBLE); SPR_setVisibility(Rect1BB1_Q4, VISIBLE);
			
			SPR_setVisibility(Rect2HB1_Q1, HIDDEN); SPR_setVisibility(Rect2HB1_Q2, HIDDEN);
			SPR_setVisibility(Rect2HB1_Q3, HIDDEN); SPR_setVisibility(Rect2HB1_Q4, HIDDEN);
			SPR_setVisibility(Rect2BB1_Q1, HIDDEN); SPR_setVisibility(Rect2BB1_Q2, HIDDEN);
			SPR_setVisibility(Rect2BB1_Q3, HIDDEN); SPR_setVisibility(Rect2BB1_Q4, HIDDEN);
		}else{
			SPR_setVisibility(Rect1HB1_Q1, HIDDEN); SPR_setVisibility(Rect1HB1_Q2, HIDDEN);
			SPR_setVisibility(Rect1HB1_Q3, HIDDEN); SPR_setVisibility(Rect1HB1_Q4, HIDDEN);
			SPR_setVisibility(Rect1BB1_Q1, HIDDEN); SPR_setVisibility(Rect1BB1_Q2, HIDDEN);
			SPR_setVisibility(Rect1BB1_Q3, HIDDEN); SPR_setVisibility(Rect1BB1_Q4, HIDDEN);
			
			SPR_setVisibility(Rect2HB1_Q1, VISIBLE); SPR_setVisibility(Rect2HB1_Q2, VISIBLE);
			SPR_setVisibility(Rect2HB1_Q3, VISIBLE); SPR_setVisibility(Rect2HB1_Q4, VISIBLE);
			SPR_setVisibility(Rect2BB1_Q1, VISIBLE); SPR_setVisibility(Rect2BB1_Q2, VISIBLE);
			SPR_setVisibility(Rect2BB1_Q3, VISIBLE); SPR_setVisibility(Rect2BB1_Q4, VISIBLE);
			*/
		}
	}
}

bool FUNCAO_COLISAO(s16 R1x1, s16 R1y1, s16 R1x2, s16 R1y2, s16 R2x1, s16 R2y1, s16 R2x2, s16 R2y2)
{
	
	s16 temp=0;
	if(R1x1>R1x2){ temp=R1x1; R1x1=R1x2; R1x2=temp; }
	if(R2x1>R2x2){ temp=R2x1; R2x1=R2x2; R2x2=temp; }
	
	if ( (R1x1+(R1x2-R1x1)>R2x1) && (R1x1<R2x1+(R2x2-R2x1)) && 
		 (R1y1+(R1y2-R1y1)>R2y1) && (R1y1<R2y1+(R2y2-R2y1)) )
	{ 
		
		if(R1x1==R1x2 && R1y1==R1y2){
			return 0;
			}else if(R2x1==R2x2 && R2y1==R2y2){
				return 0;
			}else{
				/*houve colisao*/
				return 1;
			}
	}else{ 
		/*nao houve colisao*/
		return 0;
	}
}

void FUNCAO_UPDATE_LIFESP(u8 Player, u8 EnergyType, s8 Value)
{
	if(EnergyType==1) //energia
	{
		if(P[Player].energiaBase<Value*-1){ P[Player].energiaBase=0;
		}else{ P[Player].energiaBase = P[Player].energiaBase+Value; }
	}
	if(P[Player].energiaBase==0){  PLAYER_STATE(Player, 550); }
	/*bugado
	if(P[Player].state==110 && P[Player].state==210){
		if(P[Player].energiaBase==0){ PLAYER_STATE(Player, 550); }
	}else{
		if(P[Player].energiaBase==0){  PLAYER_STATE(Player, 550); P[Player].y=gAlturaPiso-8; }
	}
	*/
	
	/*
	if(EnergyType==2) //energia especial
	{
		if(P[Player].energiaSP<Value*-1){ P[Player].energiaSP=0;
		}else{ P[Player].energiaSP = P[Player].energiaSP+Value; }
	}
	*/
	
	//temporario
	if(P[Player].energiaBase<=20){ P[Player].energiaBase = 96; }

}

void FUNCAO_INICIALIZACAO()
{
	gPodeMover=0;
	doubleHitStep=0;
	
	//BG_B
	gInd_tileset=1; //Antes de carregar o Background, definir o ponto de inicio de carregamento na VRAM
	
	//Load the tileset 'BGB1'
	if(gBG_Choice==1){ 
		gBG_Width = 512; // Largura do Cenario em pixels!
		gScrollValue=-(gBG_Width-320)/2;
		VDP_loadTileSet(gfx_bgb1.tileset,gInd_tileset,DMA); 
		VDP_setTileMapEx(BG_B,gfx_bgb1.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,gInd_tileset),0,0,0,0,gBG_Width/8,28,DMA_QUEUE);
		PAL_setPalette(PAL0, gfx_bgb1.palette->data, CPU);
		gInd_tileset += gfx_bgb1.tileset->numTile;
	} 
	
	//Load the tileset 'BGB2'
	if(gBG_Choice==2){ 
		gBG_Width = 512; // Largura do Cenario em pixels!
		gScrollValue=-(gBG_Width-320)/2;
		VDP_loadTileSet(gfx_bgb2.tileset,gInd_tileset,DMA); 
		VDP_setTileMapEx(BG_B,gfx_bgb2.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,gInd_tileset),0,0,0,0,gBG_Width/8,28,DMA_QUEUE);
		PAL_setPalette(PAL0, gfx_bgb2.palette->data, CPU);
		gInd_tileset += gfx_bgb2.tileset->numTile;
	} 
	
	//load palette HUD in PAL1, GFX load AFTER round intro...
	//PALETA DA HUD!!!
	PAL_setPalette(PAL1, spr_point.palette->data, CPU); 
	
	u8 divisoes = 22;
	gAlturaPiso = (224/divisoes)*divisoes-1; gAlturaPiso-=4;
	
	for(i=1; i<=2; i++) {
		P[i].key_JOY_countdown[2]=0;
		P[i].key_JOY_countdown[4]=0;
		P[i].key_JOY_countdown[6]=0;
		P[i].key_JOY_countdown[8]=0;
	}
	
	//P1
	P[1].energia = 96;
	P[1].energiaBase = 96;
	P[1].energiaSP = 0;
	P[1].rageTimerCountdown=RAGETIMER;
	P[1].wins = 0;
	P[1].x = (gBG_Width/2)-80; //P[1].x = (320/4);
	P[1].y = gAlturaPiso;
	P[1].hitPause = 0;
	P[1].direcao = 1;
	P[1].state = 610; //610, Intro State!
	P[1].puloTimer = 0;
	P[1].dataMBox[0] = -BODYSPACE;
	P[1].dataMBox[1] = -60;
	P[1].dataMBox[2] = +BODYSPACE;
	P[1].dataMBox[3] = - 1;
	P[1].paleta = PAL2;
	P[1].cicloInteracoesGravidade = CIGD; //CIGD eh uma definicao global! ver inicio do codigo
	P[1].cicloInteracoesGravidadeCont = 0;
	P[1].fball.active = 0;
	P[1].bufferSpecial = 0;
	P[1].hitCounter = 0;
	P[1].stateMoveType = 0;
	
	//P2
	P[2].energia = 96;
	P[2].energiaBase = 96;
	P[2].energiaSP = 0;
	P[2].rageTimerCountdown=RAGETIMER;
	P[2].wins = 0;
	P[2].x = (gBG_Width/2)+80; //P[2].x = (320/4)*3;
	P[2].y = gAlturaPiso;
	P[2].hitPause = 0;
	P[2].direcao = -1;
	P[2].state = 610; //610, Intro State!
	P[2].puloTimer = 0;
	P[2].dataMBox[0] = -BODYSPACE;
	P[2].dataMBox[1] = -60;
	P[2].dataMBox[2] = +BODYSPACE;
	P[2].dataMBox[3] = - 1;
	P[2].paleta = PAL3;
	P[2].cicloInteracoesGravidade = CIGD; //CIGD eh uma definicao global! ver inicio do codigo
	P[2].cicloInteracoesGravidadeCont = 0;
	P[2].fball.active = 0;
	P[2].bufferSpecial = 0;
	P[2].hitCounter = 0;
	P[2].stateMoveType = 0;


	//Para verificar a vantagem ou desvantagem em relacao aos frames
	frameAdvCounterP1 = 0;
	lastFrameAdvCounterP1 = 0;
	frameAdvCounterP2 = 0;
	lastFrameAdvCounterP2 = 0;

	/*
	P[1].sombra = SPR_addSpriteExSafe(&spr_sombra, P[1].x-P[1].axisX, P[1].y-2, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
	SPR_setVRAMTileIndex(P[1].sombra, 1492); //define uma posicao especifica para o GFX na VRAM
	if(gSombraStyle==2)
	{
		P[2].sombra = SPR_addSpriteExSafe(&spr_sombra, P[2].x-P[1].axisX, P[2].y-2, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
	}
	*/
	
	//reset Graphic Elements
	if (GE[ 1].sprite){ SPR_releaseSprite(GE[ 1].sprite); GE[ 1].sprite = NULL; }
	if (GE[ 2].sprite){ SPR_releaseSprite(GE[ 2].sprite); GE[ 2].sprite = NULL; }
	if (GE[ 3].sprite){ SPR_releaseSprite(GE[ 3].sprite); GE[ 3].sprite = NULL; }
	if (GE[ 4].sprite){ SPR_releaseSprite(GE[ 4].sprite); GE[ 4].sprite = NULL; }
	if (GE[ 5].sprite){ SPR_releaseSprite(GE[ 5].sprite); GE[ 5].sprite = NULL; }
	if (GE[ 6].sprite){ SPR_releaseSprite(GE[ 6].sprite); GE[ 6].sprite = NULL; }
	if (GE[ 7].sprite){ SPR_releaseSprite(GE[ 7].sprite); GE[ 7].sprite = NULL; }
	if (GE[ 8].sprite){ SPR_releaseSprite(GE[ 8].sprite); GE[ 8].sprite = NULL; }
	if (GE[ 9].sprite){ SPR_releaseSprite(GE[ 9].sprite); GE[ 9].sprite = NULL; }
	if (GE[10].sprite){ SPR_releaseSprite(GE[10].sprite); GE[10].sprite = NULL; }
	if (GE[11].sprite){ SPR_releaseSprite(GE[11].sprite); GE[11].sprite = NULL; }
	if (GE[12].sprite){ SPR_releaseSprite(GE[12].sprite); GE[12].sprite = NULL; }
	if (GE[13].sprite){ SPR_releaseSprite(GE[13].sprite); GE[13].sprite = NULL; }
	if (GE[14].sprite){ SPR_releaseSprite(GE[14].sprite); GE[14].sprite = NULL; }
	if (GE[15].sprite){ SPR_releaseSprite(GE[15].sprite); GE[15].sprite = NULL; }
	if (GE[16].sprite){ SPR_releaseSprite(GE[16].sprite); GE[16].sprite = NULL; }
	if (GE[17].sprite){ SPR_releaseSprite(GE[17].sprite); GE[17].sprite = NULL; }
	if (GE[18].sprite){ SPR_releaseSprite(GE[18].sprite); GE[18].sprite = NULL; }
	if (GE[19].sprite){ SPR_releaseSprite(GE[19].sprite); GE[19].sprite = NULL; }
	if (GE[20].sprite){ SPR_releaseSprite(GE[20].sprite); GE[20].sprite = NULL; }
	
	gClockTimer=60; //Relogio, timer de 1 seg
	gClockLTimer=9; //Digito esquerdo do Relogio
	gClockRTimer=9; //Digito direito do Relogio
	gRound=1;       //Round Number
	
	
	if(RELEASE==0)
	{
		Rect1BB1_Q1 = SPR_addSprite(&spr_rect_bb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setVRAMTileIndex(Rect1BB1_Q1, 1453); //define uma posicao especifica para o GFX na VRAM
		Rect1BB1_Q2 = SPR_addSprite(&spr_rect_bb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setVRAMTileIndex(Rect1BB1_Q2, 1453); //define uma posicao especifica para o GFX na VRAM
		Rect1BB1_Q3 = SPR_addSprite(&spr_rect_bb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setVRAMTileIndex(Rect1BB1_Q1, 1453); //define uma posicao especifica para o GFX na VRAM
		Rect1BB1_Q4 = SPR_addSprite(&spr_rect_bb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setVRAMTileIndex(Rect1BB1_Q2, 1453); //define uma posicao especifica para o GFX na VRAM
		Rect1HB1_Q1 = SPR_addSprite(&spr_rect_hb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setVRAMTileIndex(Rect1HB1_Q1, 1454); //define uma posicao especifica para o GFX na VRAM
		Rect1HB1_Q2 = SPR_addSprite(&spr_rect_hb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setVRAMTileIndex(Rect1HB1_Q2, 1454); //define uma posicao especifica para o GFX na VRAM
		Rect1HB1_Q3 = SPR_addSprite(&spr_rect_hb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setVRAMTileIndex(Rect1HB1_Q1, 1454); //define uma posicao especifica para o GFX na VRAM
		Rect1HB1_Q4 = SPR_addSprite(&spr_rect_hb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setVRAMTileIndex(Rect1HB1_Q2, 1454); //define uma posicao especifica para o GFX na VRAM
		
		Rect2BB1_Q1 = SPR_addSprite(&spr_rect_bb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setVRAMTileIndex(Rect2BB1_Q1, 1453); //define uma posicao especifica para o GFX na VRAM
		Rect2BB1_Q2 = SPR_addSprite(&spr_rect_bb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setVRAMTileIndex(Rect2BB1_Q2, 1453); //define uma posicao especifica para o GFX na VRAM
		Rect2BB1_Q3 = SPR_addSprite(&spr_rect_bb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setVRAMTileIndex(Rect2BB1_Q1, 1453); //define uma posicao especifica para o GFX na VRAM
		Rect2BB1_Q4 = SPR_addSprite(&spr_rect_bb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setVRAMTileIndex(Rect2BB1_Q2, 1453); //define uma posicao especifica para o GFX na VRAM
		Rect2HB1_Q1 = SPR_addSprite(&spr_rect_hb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setVRAMTileIndex(Rect2HB1_Q1, 1454); //define uma posicao especifica para o GFX na VRAM
		Rect2HB1_Q2 = SPR_addSprite(&spr_rect_hb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setVRAMTileIndex(Rect2HB1_Q2, 1454); //define uma posicao especifica para o GFX na VRAM
		Rect2HB1_Q3 = SPR_addSprite(&spr_rect_hb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setVRAMTileIndex(Rect2HB1_Q1, 1454); //define uma posicao especifica para o GFX na VRAM
		Rect2HB1_Q4 = SPR_addSprite(&spr_rect_hb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setVRAMTileIndex(Rect2HB1_Q2, 1454); //define uma posicao especifica para o GFX na VRAM
		
		SPR_setHFlip(Rect1BB1_Q2, TRUE);
		SPR_setVFlip(Rect1BB1_Q3, TRUE);
		SPR_setHFlip(Rect1BB1_Q4, TRUE); SPR_setVFlip(Rect1BB1_Q4, TRUE);
		SPR_setHFlip(Rect1HB1_Q2, TRUE);
		SPR_setVFlip(Rect1HB1_Q3, TRUE);
		SPR_setHFlip(Rect1HB1_Q4, TRUE); SPR_setVFlip(Rect1HB1_Q4, TRUE);
		
		SPR_setHFlip(Rect2BB1_Q2, TRUE);
		SPR_setVFlip(Rect2BB1_Q3, TRUE);
		SPR_setHFlip(Rect2BB1_Q4, TRUE); SPR_setVFlip(Rect2BB1_Q4, TRUE);
		SPR_setHFlip(Rect2HB1_Q2, TRUE);
		SPR_setVFlip(Rect2HB1_Q3, TRUE);
		SPR_setHFlip(Rect2HB1_Q4, TRUE); SPR_setVFlip(Rect2HB1_Q4, TRUE);
		
		SPR_setDepth(Rect1BB1_Q1, 1);
		SPR_setDepth(Rect1BB1_Q2, 1);
		SPR_setDepth(Rect1BB1_Q3, 1);
		SPR_setDepth(Rect1BB1_Q4, 1);
		SPR_setDepth(Rect1HB1_Q1, 1);
		SPR_setDepth(Rect1HB1_Q2, 1);
		SPR_setDepth(Rect1HB1_Q3, 1);
		SPR_setDepth(Rect1HB1_Q4, 1);
		
		SPR_setDepth(Rect2BB1_Q1, 1);
		SPR_setDepth(Rect2BB1_Q2, 1);
		SPR_setDepth(Rect2BB1_Q3, 1);
		SPR_setDepth(Rect2BB1_Q4, 1);
		SPR_setDepth(Rect2HB1_Q1, 1);
		SPR_setDepth(Rect2HB1_Q2, 1);
		SPR_setDepth(Rect2HB1_Q3, 1);
		SPR_setDepth(Rect2HB1_Q4, 1);
	}
	
	//Intro State, Player Start
	//P[1].sprite = SPR_addSpriteExSafe(&spr_point, P[1].x-P[1].axisX, P[1].y-P[1].axisY, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
	//P[2].sprite = SPR_addSpriteExSafe(&spr_point, P[2].x-P[2].axisX, P[2].y-P[2].axisY, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
	PLAYER_STATE(1,100);
	PLAYER_STATE(2,100);
	
	SPR_setHFlip(P[2].sprite, TRUE);
	
	//Define a paleta dos players...
	P[1].palID=cursorP1ColorChoice;
	P[2].palID=cursorP2ColorChoice; 
	
	if( P[1].id==P[2].id && P[1].palID==P[2].palID)
	{
		//se ambos os players forem iguais e com a mesma paleta...
		P[1].palID=1; //Por padrao, P1 fica com a paleta 1
		P[2].palID=2; //Por padrao, P2 fica com a paleta 2 'cor alternativa'
	}
	
	//--P1--
	
	if(P[1].id== 1 && P[1].palID==1){ PAL_setPalette(PAL2, spr_ryo_pal1.palette->data, CPU); } //ryo
	if(P[1].id== 1 && P[1].palID==2){ PAL_setPalette(PAL2, spr_ryo_pal2.palette->data, CPU); } //ryo
	
	//--P2--
	
	if(P[2].id== 1 && P[2].palID==1){ PAL_setPalette(PAL3, spr_ryo_pal1.palette->data, CPU); } //ryo
	if(P[2].id== 1 && P[2].palID==2){ PAL_setPalette(PAL3, spr_ryo_pal2.palette->data, CPU); } //ryo
	
	//AXIS
	if(RELEASE==0)
	{
		if (GE[1].sprite){ SPR_releaseSprite(GE[1].sprite); GE[1].sprite = NULL; }
		if (GE[2].sprite){ SPR_releaseSprite(GE[2].sprite); GE[2].sprite = NULL; }
		GE[1].sprite = SPR_addSprite(&spr_point, P[1].x-4, P[1].y-5, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		GE[2].sprite = SPR_addSprite(&spr_point, P[2].x-4, P[2].y-5, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setVRAMTileIndex(GE[1].sprite, 1455); //define uma posicao especifica para o GFX na VRAM
		SPR_setVRAMTileIndex(GE[2].sprite, 1455); //define uma posicao especifica para o GFX na VRAM
		SPR_setVisibility(GE[1].sprite, HIDDEN);
		SPR_setVisibility(GE[2].sprite, HIDDEN);
		
		//DEPTH
		SPR_setDepth(GE[1].sprite, 1 );
		SPR_setDepth(GE[2].sprite, 2 );
	}
	
	//depth 3 e 4 reservados
	if (P[1].sprite){ SPR_setDepth(P[1].sprite,  5 ); } 
	if (P[2].sprite){ SPR_setDepth(P[2].sprite,  6 ); } 
	//depth 7 e 8 reservados
	//if(P[1].sombra){ SPR_setDepth(P[1].sombra, 98 ); }
	//if(gSombraStyle==2){ SPR_setDepth(P[2].sombra, 99 ); }
}

void FUNCAO_ROUND_INIT()
{
	gPodeMover=1;
}

void FUNCAO_ROUND_RESTART()
{
	
}

void CLEAR_VDP()
{
	SYS_disableInts();
	 SPR_reset();
	 //VDP_resetSprites();
	 //VDP_releaseAllSprites();
	 //SPR_defragVRAM();
	 VDP_clearPlane(BG_A, TRUE);
	 VDP_clearPlane(BG_B, TRUE);	
	 VDP_setTextPlane(BG_A);  
	 VDP_setHorizontalScroll(BG_B, 0); 
	 VDP_setVerticalScroll(BG_B, 0); 
	 VDP_setHorizontalScroll(BG_A, 0); 
	 VDP_setVerticalScroll(BG_A, 0);
	 VDP_setBackgroundColor(0);
	 VDP_resetScreen();
	 PAL_setPaletteColors(0, (u16*) palette_black, CPU);
	SYS_enableInts();
	gInd_tileset=0;
}

//EOF - END OF FILE; by GAMEDEVBOSS 2015-2022