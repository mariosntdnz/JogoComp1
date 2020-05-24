//gcc jogo.c -o jogo -lSDL2 -lSDL2_image -lSDL2_ttf
//ou só dar o comando make
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include "jogo.h"
#include<time.h>

#define QTD_CARROS 			4
#define QTD_PISTAS			4
#define altura_colisao 		130
#define largura_colisao 	70
#define COR_CARRO_ROSA  	0
#define COR_CARRO_AZUL		1
#define COR_CARRO_VERDE	  	2
#define COR_CARRO_LARANJA 	3
#define PISTA1 				420
#define PISTA2				550
#define PISTA3				680
#define PISTA4				810
#define TELA_MENU 			0
#define TELA_JOGO 			1
#define TELA_RECORDE 		2
#define TELA_CREDITOS		3
#define TELA_GAMEOVER 		4
#define DIV_ALTURA_PISTA    3
#define ALTURA1 			0
#define ALTURA2 			200
#define ALTURA3  			400
#define TOP_RECORDS 		5
#define MAIOR 				1
#define MENOR 				0

static const int width = 1366;
static const int height = 768;

int compare(const void *a, const void *b)
{
	return(*(int*)a - *(int*)b);
}

void eventos_teclado(SDL_Event evento,OBJETO *player, int velocidade)
{
	player->sprite.x = 0;

	if((evento.type == SDL_KEYDOWN)&&(evento.key.keysym.sym == SDLK_LEFT))
	{
		player->sprite.x=1008;
		player->texdestination.x-=velocidade;
		if(player->texdestination.x<400)
		{
			player->texdestination.x=400;
		}
		return;
	}

	if((evento.type == SDL_KEYDOWN)&&(evento.key.keysym.sym == SDLK_RIGHT))
	{
		player->sprite.x=504;
		player->texdestination.x+=velocidade;
		if(player->texdestination.x>820)
		{
			player->texdestination.x=820;
		}
		return;
	}
}

void desenha_pista(OBJETO *gramado,OBJETO *fundo1,OBJETO *fundo2,SDL_Renderer *render)
{
	if(fundo1->texdestination.y > 768) fundo1->texdestination.y = -765;
	if(fundo2->texdestination.y > 768) fundo2->texdestination.y = -765;
	
	fundo1->texdestination.y += 4;
	fundo2->texdestination.y += 4;

	SDL_RenderCopy(render,gramado->tmptexture,&gramado->sprite,&gramado->texdestination);
	SDL_RenderCopy(render,fundo1->tmptexture,&fundo1->sprite,&fundo1->texdestination);
	SDL_RenderCopy(render,fundo2->tmptexture,&fundo2->sprite,&fundo2->texdestination);
}



int getRecorde(int maior_ou_menor)
{
	FILE *arquivo = fopen("recorde.txt","r+");
	char aux[20];
	int a[TOP_RECORDS];
	for (int i = 0; i < TOP_RECORDS; i++){

		fscanf(arquivo,"%s",aux);
		fscanf(arquivo,"%d",&a[i]);
		//printf("a - %d\n", a[i]);
	}
	fclose(arquivo);
	qsort(a,TOP_RECORDS,sizeof(int),compare);

	return (maior_ou_menor == MAIOR ? a[TOP_RECORDS-1]:a[0]);
}

void freeObjText(OBJETO_TXT obj)
{
	TTF_CloseFont(obj.fonte);
	SDL_DestroyTexture(obj.mensagem);
	SDL_FreeSurface(obj.surfaceMensagem);
}

int posicaoAleatoriaAlturaPista(void)
{
	int posicao;
	posicao = rand() % DIV_ALTURA_PISTA;

	switch(posicao)
	{
		case 0:
			return ALTURA1;
		case 1:
			return ALTURA2;
		case 2:
			return ALTURA3;
	}
}

int posicaoAleatoriaPista(void)
{
	int posicao;
	
	posicao = rand() % QTD_PISTAS;

	switch(posicao)
	{
		case 0:
			return PISTA1;
		case 1:
			return PISTA2;
		case 2:
			return PISTA3;
		case 3:
			return PISTA4;
	}
}

int randCor(void)
{
	int cor;
	cor = rand() % QTD_CARROS;
	
	return cor;
}

void initObjeto(OBJETO *objeto,int sX,int sY,int sW,int sH,int tX,int tY,int tW, int tH,char *file_img,SDL_Renderer *renderer)
{
	objeto->tmpsurface = IMG_Load(file_img);
	objeto->tmptexture = SDL_CreateTextureFromSurface(renderer,objeto->tmpsurface);
	SDL_FreeSurface(objeto->tmpsurface);
	objeto->sprite.x = sX;
	objeto->sprite.y = sY;
	objeto->sprite.w = sW;
	objeto->sprite.h = sH;
	objeto->texdestination.x = tX;
	objeto->texdestination.y = tY;
	objeto->texdestination.w = tW;
	objeto->texdestination.h = tH;
}
void initCenario(OBJETO *pista0, OBJETO *pista1, OBJETO *grama, SDL_Renderer *renderer)
{
	initObjeto(pista0,0,0,840 ,650,263,-765,840 ,768,"pista.png",renderer);
	initObjeto(pista1,0,0,840 ,650,263,0   ,840 ,768,"pista.png",renderer);
	initObjeto(grama, 0,0,1366,768,0  ,0   ,1366,768,"grama.png",renderer);
}

OBJETO getCarroJogador(SDL_Renderer *renderer)
{
	OBJETO jogador;
	initObjeto(&jogador,0,0,504,417,650,600,150,150,"carro.png",renderer);
	return jogador;
}

OBJETO getCarroAleatorio(SDL_Renderer *renderer)
{
	OBJETO carro;
	int  cor 	     = randCor();
	char *corCarro   = getImageCarro(cor);
	initObjeto(&carro,0,0,504,417,posicaoAleatoriaPista(),posicaoAleatoriaAlturaPista(),150,150,corCarro,renderer);
	return carro;
}

char* getImageCarro(int cor)
{
	switch(cor)
	{
		case COR_CARRO_VERDE:
			return "CarroVerde.png";
		case COR_CARRO_LARANJA:
			return "CarroLaranja.png";
		case COR_CARRO_ROSA:
			return "CarroRosa.png";
		case COR_CARRO_AZUL:
			return "CarroAzul.png";
	}
}

int colisao(OBJETO *inimigo,OBJETO *player)
{
	if((inimigo->texdestination.x <= player->texdestination.x && inimigo->texdestination.x + largura_colisao >= player->texdestination.x)
	&& (inimigo->texdestination.y <= player->texdestination.y && inimigo->texdestination.y + altura_colisao  >= player->texdestination.y))
	{
		return 1;
	}

	if((inimigo->texdestination.x - largura_colisao <= player->texdestination.x && inimigo->texdestination.x >= player->texdestination.x)
	&& (inimigo->texdestination.y <= player->texdestination.y && inimigo->texdestination.y + altura_colisao  >= player->texdestination.y))
	{
		return 1;
	}

	return 0;
}

OBJETO_TXT newTextoNaTela(SDL_Renderer *renderer,char *fonte,int tam_font, int R, int G, int B,char *texto,int x, int y, int w, int h)
{
	
	OBJETO_TXT obj_txt;

	obj_txt.fonte 		    = TTF_OpenFont(fonte, tam_font); 
	obj_txt.cor.r   		= R;
	obj_txt.cor.g   		= G;
	obj_txt.cor.b   		= B;
	obj_txt.surfaceMensagem = TTF_RenderText_Solid(obj_txt.fonte, texto, obj_txt.cor); 
	obj_txt.mensagem 		= SDL_CreateTextureFromSurface(renderer, obj_txt.surfaceMensagem); 
	obj_txt.message_rect.x  = x;
	obj_txt.message_rect.y  = y;
	obj_txt.message_rect.w  = w;
	obj_txt.message_rect.h  = h;

	return obj_txt;
}

OBJETO newMenu(SDL_Renderer *renderer)
{
	OBJETO menu;
	initObjeto(&menu,0,0,1366,768,0,0,1366,768,"Menu.png",renderer);
	return menu;
}

OBJETO newCredito(SDL_Renderer *renderer)
{
	OBJETO credito;
	initObjeto(&credito,0,0,1366,768,0,0,1366,768,"credito.png",renderer);
	return credito;
}

OBJETO newRecorde(SDL_Renderer *renderer)
{
	OBJETO recorde;
	initObjeto(&recorde,0,0,1366,768,0,0,1366,768,"recorde.png",renderer);
	return recorde;
}

OBJETO newPonteiro(SDL_Renderer *renderer)
{
	OBJETO ponteiro;
	initObjeto(&ponteiro,0,0,721,390,800,150,200,100,"ponteiro.png",renderer);
	return ponteiro;
}

OBJETO newGameOver(SDL_Renderer *renderer)
{
	OBJETO fim;
	initObjeto(&fim,0,0,1366,768,0,0,1366,768,"GameOver.png",renderer);
	return fim;
}

int exibe_ponteiro(int select)
{
	switch(select)
	{
		case 1:
			return 150;
		case 2:
			return 280;
		case 3:
			return 410;
	}
}

int teclado_menu(SDL_Event evento,OBJETO *po,int select)
{
	
	if((evento.type == SDL_KEYDOWN)&&(evento.key.keysym.sym == SDLK_UP))   select--;
	if((evento.type == SDL_KEYDOWN)&&(evento.key.keysym.sym == SDLK_DOWN)) select++;
	if(select > 3) select = 1;
	if(select < 1) select = 3;

	return select;
}

int main(int argc, char *argv[])
{
	int FPS = 60;
	int frame_delay = 1000/FPS;
	Uint32 frame_start;
	int frame_time;
	OBJETO carros[QTD_CARROS];
	OBJETO pista0,pista1,grama;
	int velocidade = 5;
	int vidas	   = 2;
	int seletor = 1, tela = 0, pontos = 0, distancia = 0;
	int ultimoRecorde = getRecorde(MENOR);
	int maiorRecorde  = getRecorde(MAIOR);
	char vidas_txt[10];
	char pontos_txt[100];
	char recorde_txt[100];
	char nivel_txt[12];
	char matriz_recorde[TOP_RECORDS][20];
	int  pontos_recordes[TOP_RECORDS];
	srand(time(NULL));

	/***************inicializando SDL*********************/

	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	SDL_Window  *window = SDL_CreateWindow("jogo",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,SDL_WINDOW_FULLSCREEN);
	SDL_Surface *windowSurface = NULL;
	SDL_Renderer *renderer = SDL_CreateRenderer(window , -1 , SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(renderer , 255 , 255 , 255 , 255);
	IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG);
	bool rodando = true;
	SDL_Event event;
	FILE *pa;
	char arq[20] = "pontos.txt",vetor[7];

	/***************inicializando SDL*********************/
	OBJETO_TXT top[TOP_RECORDS];

	initCenario(&pista0,&pista1,&grama,renderer);
	OBJETO jogador  = getCarroJogador(renderer);
	OBJETO menu 	= newMenu(renderer);
	OBJETO credito	= newCredito(renderer);
	OBJETO recorde  = newRecorde(renderer);
	OBJETO ponteiro = newPonteiro(renderer);
	OBJETO fim 		= newGameOver(renderer);

	sprintf(vidas_txt,"vidas %d",vidas);
	sprintf(pontos_txt,"pontos %d",pontos);
	sprintf(recorde_txt,"recorde %d",maiorRecorde);
	sprintf(nivel_txt,"nivel %d",velocidade-4);

	OBJETO_TXT vida_tela   		= newTextoNaTela(renderer,"vidas.ttf",25,0,0,0,vidas_txt,0,0,200,100);
	OBJETO_TXT pontos_tela 		= newTextoNaTela(renderer,"pontuacao.ttf",20,0,0,0,pontos_txt,1000,0,250,100);
	OBJETO_TXT recordeAtt_tela 	= newTextoNaTela(renderer,"pontuacao.ttf",20,0,0,0,recorde_txt,1000,100,250,100); 
	OBJETO_TXT nivel_tela 		= newTextoNaTela(renderer,"pontuacao.ttf",20,0,0,0,recorde_txt,550,0,250,100); 
	OBJETO_TXT parabens_re 		= newTextoNaTela(renderer,"pontuacao.ttf",20,255,255,255,"Mas bateu um Recorde no TOP5",50,400,900,150); 

	for (int i = 0; i < QTD_CARROS; i++) carros[i] = getCarroAleatorio(renderer);
	int newRecord = 0;

	while(rodando)
	{               
		srand(time(NULL));

		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
				rodando = false;
		}
		if((event.type == SDL_KEYUP)&&(event.key.keysym.sym == SDLK_ESCAPE)) rodando = false;

		if(jogador.texdestination.y>=768) tela = TELA_GAMEOVER;

		frame_start = SDL_GetTicks();

		if(tela == TELA_MENU)
		{
			vidas = 2;
			velocidade = 5;
			FPS = 10;
			frame_delay = 1000/FPS;
			seletor = teclado_menu(event,&ponteiro,seletor);
			ponteiro.texdestination.y = exibe_ponteiro(seletor);
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer,menu.tmptexture,&menu.sprite,&menu.texdestination);
			SDL_RenderCopy(renderer,ponteiro.tmptexture,&ponteiro.sprite,&ponteiro.texdestination);
			SDL_RenderPresent(renderer);
			if((event.type == SDL_KEYDOWN)&&(event.key.keysym.scancode == SDL_SCANCODE_RETURN))
			{
				tela = seletor;
			}
		}
		else if(tela == TELA_JOGO)
		{
			sprintf(vidas_txt,"vidas %d",vidas);
			vida_tela.surfaceMensagem = TTF_RenderText_Solid(vida_tela.fonte, vidas_txt, vida_tela.cor);
			vida_tela.mensagem = SDL_CreateTextureFromSurface(renderer, vida_tela.surfaceMensagem);

			sprintf(pontos_txt,"pontos %d",pontos);
			pontos_tela.surfaceMensagem = TTF_RenderText_Solid(pontos_tela.fonte, pontos_txt, pontos_tela.cor);
			pontos_tela.mensagem = SDL_CreateTextureFromSurface(renderer, pontos_tela.surfaceMensagem);

			maiorRecorde = getRecorde(MAIOR);
			sprintf(recorde_txt,"recorde %d",maiorRecorde);
			recordeAtt_tela.surfaceMensagem = TTF_RenderText_Solid(recordeAtt_tela.fonte, recorde_txt, recordeAtt_tela.cor);
			recordeAtt_tela.mensagem = SDL_CreateTextureFromSurface(renderer, recordeAtt_tela.surfaceMensagem);

			sprintf(nivel_txt,"nivel %d",velocidade-4);
			nivel_tela.surfaceMensagem = TTF_RenderText_Solid(nivel_tela.fonte, nivel_txt, nivel_tela.cor);
			nivel_tela.mensagem = SDL_CreateTextureFromSurface(renderer, nivel_tela.surfaceMensagem);

			distancia += 1;
			if(distancia % 100 == 0)
			{
				pontos++;
				if(pontos % 10 == 0) velocidade++;
			}

			FPS = 60;
			frame_delay = 1000/FPS;
			frame_start = SDL_GetTicks();

			for (int i = 0; i < QTD_CARROS; i++)
			{
				if(colisao(&carros[i],&jogador)){
					carros[i] = getCarroAleatorio(renderer);
					carros[i].texdestination.y = -150;
					vidas -= 1;
					ultimoRecorde = getRecorde(MENOR);

					if (!vidas){

						if(pontos > ultimoRecorde){

							newRecord = 1;
							FILE *arquivo = fopen("recorde.txt","r+");
							for (int i = 0; i < TOP_RECORDS; i++){
								fscanf(arquivo,"%s",matriz_recorde[i]);
								fscanf(arquivo,"%d",&pontos_recordes[i]);
							}
							fclose(arquivo);

							qsort(pontos_recordes,TOP_RECORDS,sizeof(int),compare);
							pontos_recordes[0] = pontos;

							/*int zero = 1;

							for (int i = 0; i < TOP_RECORDS; i++)
							{
								if(pontos_recordes[i] == 10){
									pontos_recordes[i] = pontos;
									if(i != TOP_RECORDS-1) break;
								}
								if(i == TOP_RECORDS-1) zero = 0;
							}
							if(!zero) pontos_recordes[0] = pontos;*/

							arquivo = fopen("recorde.txt","w");
							for (int i = 0; i < TOP_RECORDS; i++){
								fprintf(arquivo,"%s ",matriz_recorde[i]);
								fprintf(arquivo,"%d\n",pontos_recordes[i]);
							}
							fclose(arquivo);
						}
						tela = TELA_GAMEOVER;
						vidas = 2;
						pontos = 0;
					}
				}
				else{
					carros[i].texdestination.y += velocidade;
				}
			}

			for (int i = 0; i < QTD_CARROS; i++)
			{
				if(carros[i].texdestination.y > height){
					carros[i] = getCarroAleatorio(renderer);
					carros[i].texdestination.y = -150;
				}
			}

			for (int i = 0; i < QTD_CARROS; i++)
			{
				for (int j = 0; j < QTD_CARROS; j++)
				{
					if(colisao(&carros[i],&carros[j]) && i != j){
						carros[i].texdestination.y = posicaoAleatoriaAlturaPista();
						carros[i].texdestination.x = posicaoAleatoriaPista();
						i = 0;
						j = 0;
					}
				}
			}

			eventos_teclado(event, &jogador,velocidade);
			SDL_RenderClear(renderer);//limpando buffer
			desenha_pista(&grama,&pista0,&pista1,renderer);
			SDL_RenderCopy(renderer,jogador.tmptexture,&jogador.sprite,&jogador.texdestination);
			SDL_RenderCopy(renderer, pontos_tela.mensagem, NULL, &pontos_tela.message_rect); 
			SDL_RenderCopy(renderer, vida_tela.mensagem, NULL, &vida_tela.message_rect);
			SDL_RenderCopy(renderer, recordeAtt_tela.mensagem, NULL, &recordeAtt_tela.message_rect);
			SDL_RenderCopy(renderer, nivel_tela.mensagem, NULL, &nivel_tela.message_rect);
			

			for(int i = 0; i < QTD_CARROS; i++)
			{
				SDL_RenderCopy(renderer,carros[i].tmptexture,&carros[i].sprite,&carros[i].texdestination);
			}

			SDL_RenderPresent(renderer);//exibindo

			frame_time = SDL_GetTicks() - frame_start;
			SDL_RenderClear(renderer);
		}
		else if(tela == TELA_RECORDE)
		{
			FPS = 10;
			frame_delay = 1000/FPS;
			frame_start = SDL_GetTicks();

			int altura_record = 100;
			int aux_pontos[5];
			FILE *arquivo = fopen("recorde.txt","r+");
			for (int i = 0; i < TOP_RECORDS; i++){
				fscanf(arquivo,"%s",matriz_recorde[i]);
				fscanf(arquivo,"%d",&pontos_recordes[i]);
				aux_pontos[i] = pontos_recordes[i];
			}
			fclose(arquivo);

			qsort(pontos_recordes,TOP_RECORDS,sizeof(int),compare);

			for (int i = 0; i < TOP_RECORDS; i++)
			{
				char *aux = (char*) malloc(sizeof(char)*50);
				char aux2[10];
				char aux3[50];
				sprintf(aux2,"%d",pontos_recordes[TOP_RECORDS-i-1]);
				sprintf(aux,"%s"," ");
				strcat(aux,matriz_recorde[i]);	
				strcat(aux,"        ");
				strcat(aux,aux2);
				top[i] = newTextoNaTela(renderer,"pontuacao.ttf",25,0,0,0,aux,500,altura_record,300,100);
				altura_record += 100;
				free(aux);
			}
			OBJETO_TXT obj = newTextoNaTela(renderer,"pontuacao.ttf",25,0,0,0,"TAB para voltar",800,altura_record,300,100);
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer,recorde.tmptexture,&recorde.sprite,&recorde.texdestination);
			SDL_RenderCopy(renderer,obj.mensagem,NULL,&obj.message_rect);

			for (int i = 0; i < TOP_RECORDS; i++) SDL_RenderCopy(renderer, top[i].mensagem, NULL, &top[i].message_rect);
			
			SDL_RenderPresent(renderer);

			frame_time = SDL_GetTicks() - frame_start;

			for (int i = 0; i < TOP_RECORDS; i++) freeObjText(top[i]);
			freeObjText(obj);
			SDL_RenderClear(renderer);

			if((event.type == SDL_KEYUP)&&(event.key.keysym.scancode == SDL_SCANCODE_TAB))
			{
				tela = TELA_MENU;
			}
		}
		else if(tela == TELA_CREDITOS)
		{
			FPS = 10;
			frame_delay = 1000/FPS;
			SDL_RenderClear(renderer);
			OBJETO_TXT obj = newTextoNaTela(renderer,"pontuacao.ttf",25,0,0,0,"TAB para voltar",800,600,300,100);
			SDL_RenderCopy(renderer,credito.tmptexture,&credito.sprite,&credito.texdestination);
			SDL_RenderCopy(renderer,obj.mensagem,NULL,&obj.message_rect);			
			SDL_RenderPresent(renderer);
			freeObjText(obj);
			if((event.type == SDL_KEYUP)&&(event.key.keysym.scancode == SDL_SCANCODE_TAB))
			{
				tela = TELA_MENU;
			}

		}
		else if(tela == TELA_GAMEOVER)
		{
			FPS = 10;
			frame_delay = 1000/FPS;
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer,fim.tmptexture,&fim.sprite,&fim.texdestination);
			if(newRecord)SDL_RenderCopy(renderer, parabens_re.mensagem, NULL, &parabens_re.message_rect);
			SDL_RenderPresent(renderer);
			if((event.type == SDL_KEYDOWN)&&(event.key.keysym.scancode == SDL_SCANCODE_RETURN))
			{
				jogador.texdestination.y = 600;
				if(newRecord) tela = TELA_RECORDE;
				else		  tela = TELA_MENU;
				SDL_Delay(500);
				newRecord = 0;
			}
		}

		frame_time = SDL_GetTicks() - frame_start;

		if(frame_delay > frame_time)
		{
			SDL_Delay(frame_delay - frame_time);
		}

	}

	for (int i = 0; i < QTD_CARROS; i++)
	{
		SDL_DestroyTexture(carros[i].tmptexture);
	}

	SDL_DestroyTexture(fim.tmptexture);
	SDL_DestroyTexture(recorde.tmptexture);
	SDL_DestroyTexture(ponteiro.tmptexture);
	SDL_DestroyTexture(menu.tmptexture);
	SDL_DestroyTexture(credito.tmptexture);
	SDL_DestroyTexture(grama.tmptexture);
	SDL_DestroyTexture(pista1.tmptexture);
	SDL_DestroyTexture(pista0.tmptexture);
	SDL_DestroyTexture(jogador.tmptexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit(); 

	return 0;
}