
/* Author : Eduardo Felipe Gama Ferreira */

#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>
#include <iostream>
#include <fstream>

#define WIN_X 800
#define WIN_Y 600
#define FIELD_X 131
#define FIELD_Y 21
#define UPDATE_TIME 60
#define BOND_VEL 6

int setup_load(void);
void menu(void);
void alt_tab_in(void);
void alt_tab_out(void);
void end_all(void);

typedef struct _Button{
    int x, y;
    int curr;
    BITMAP* bmp[2];
    int active;
} Button;

typedef struct _Caixa{
    int x, y;
    int dx, dy;
    int i, j;
    int isMoving;
    int dir;
    int percorrido;
    BITMAP* bmp;
} Caixa;

typedef struct _Tiro{
    int x, y;
    int i, j;
    int dx, dy;
    int dir;
    int dest_i, dest_j;
    int percorrido;
    int onbox;
    BITMAP* bmp;   
} Tiro;

typedef struct _Explosao{
    int x, y;
    int curr;
    int active;
    BITMAP* bmp[16];
} Explosao;        

typedef struct _Char{
    int x, y;
    int i, j;
    int dx, dy;
    int cx, cy, cm;
    int walked_x, walked_y;
    int mod[10];
    BITMAP* bmp[10][8];  
} Char;

typedef struct _Square{
    int x, y;    
    int isParede;
    int isCaixa; //Index da Caixa
    int isDestino;    
} Square;

SAMPLE* current_song;
SAMPLE* shot;
SAMPLE* box_move;
SAMPLE* box_ok;
SAMPLE* box_shot;
SAMPLE* box_wrong;
SAMPLE* wall_shot;
SAMPLE* newlevel;
SAMPLE* warning;
FILE* input=NULL;
FILE* fscore=NULL;
FILE* fscore_out=NULL;
Char bond;
Tiro tiro;
Explosao explosao;
Caixa* caixa;
Square** square=NULL;
FONT *terminal;
int current_vol=225;
int keyIsPressed = 0;
int keyMap[5];
int shoot_dest =-1;
int caixas_qtd;
int box_moving=0;
int minutos;
int segundos;
int level=1;
int bullets;
int left;
int movs;
int score=0;
int gameRunning = 0;
int paused = 0;
int gettingScore = 0;
char nome[10] = {0};
int letter_pos = 0;
int rank_qtd;
char linha[100];
int reset = 0;
fpos_t pos_init;


int main (void){
    if (!setup_load()){
        allegro_exit();               
        exit(-1);
    }
    menu();
    
    end_all();
}
END_OF_MAIN()

void readInput(){
     gameRunning = 0;
     int i=0, j=0;
     int caixas=0;
     char r;
     free(caixa);
     if (!reset){
         fgetpos(input, &pos_init);        
     }else{
         reset = 0;
         fsetpos(input, &pos_init); 
     }
     fscanf(input, "%d %d %d", &minutos,&segundos,&bullets);
     if (minutos == -1){
        gettingScore = 1;
        clear_keybuf(); 
        gameRunning = 0;
        left = 99;
        return;
     }
     r = fgetc(input);
     while (!feof(input) && i != 11){
           r = fgetc(input);
           if (r == '\n'){
              i++; j=0; continue;   
           }
           square[i][j].x = FIELD_X+50*j;
           square[i][j].y = FIELD_Y+50*i;
           square[i][j].isParede = -1;
           square[i][j].isCaixa = -1;
           square[i][j].isDestino = -1; 
           if ( r == 'X'){
               square[i][j].isParede = 1;
           }
           if ( r == '1'){
               bond.i = i;
               bond.j = j; 
               bond.x =  FIELD_X+50*j;
               bond.y =  FIELD_Y-10+50*i; 
           }
           if ( r == '2'){
               caixas++; 
               square[i][j].isCaixa = 1;
           } 
           if ( r == '3'){
               square[i][j].isDestino = 1;
           } 
           j++;     
     }
     caixa = (Caixa*) malloc (caixas*sizeof(Caixa));
     left = caixas;
     caixas_qtd = caixas; 
     int k=0;
     for (i=0;i<11;i++)
         for (j=0;j<13;j++)
             if (square[i][j].isCaixa == 1){
                 caixa[k].x = square[i][j].x;
                 caixa[k].y = square[i][j].y;
                 caixa[k].i = i;
                 caixa[k].j = j;
                 caixa[k].dx=0;
                 caixa[k].dy=0;
                 caixa[k].bmp = load_bitmap("Images/Scenario/box.bmp",NULL);
                 caixa[k].isMoving=0;
                 caixa[k].percorrido=0;
                 square[i][j].isCaixa = k;
                 k++;
             }    
     gameRunning = 1;         
}

void MoveCaixa(Caixa* cxa, int dir){
     if (dir == 0){
         if (square[cxa->i][cxa->j-1].isParede == -1 && square[cxa->i][cxa->j-1].isCaixa == -1){
            cxa->isMoving = 1;
            cxa->dir = 0;
            box_moving++;
            play_sample(box_move, 255, 128, 1000, FALSE);
            if (square[cxa->i][cxa->j].isDestino != -1){
               cxa->bmp = load_bitmap("Images/scenario/box.bmp",NULL);  
               play_sample(box_wrong, 255, 128, 1000, FALSE);   
               left++;
            }
         }                                   
     }else if (dir == 1){ 
          if (square[cxa->i][cxa->j+1].isParede == -1 && square[cxa->i][cxa->j+1].isCaixa == -1){
            cxa->isMoving = 1;
            cxa->dir = 1;
            box_moving++;
            play_sample(box_move, 255, 128, 1000, FALSE);
            if (square[cxa->i][cxa->j].isDestino != -1){
               cxa->bmp = load_bitmap("Images/scenario/box.bmp",NULL);  
               play_sample(box_wrong, 255, 128, 1000, FALSE);   
               left++;
            }
          }
     }else if (dir == 2){ 
          if (square[cxa->i-1][cxa->j].isParede == -1 && square[cxa->i-1][cxa->j].isCaixa == -1){
            cxa->isMoving = 1;
            cxa->dir = 2;
            box_moving++;
            play_sample(box_move, 255, 128, 1000, FALSE);
            if (square[cxa->i][cxa->j].isDestino != -1){
               cxa->bmp = load_bitmap("Images/scenario/box.bmp",NULL);  
               play_sample(box_wrong, 255, 128, 1000, FALSE);   
               left++;
            }
          }
     }else if (dir == 3){ 
          if (square[cxa->i+1][cxa->j].isParede == -1 && square[cxa->i+1][cxa->j].isCaixa == -1){
            cxa->isMoving = 1;
            cxa->dir = 3;
            box_moving++;
            play_sample(box_move, 255, 128, 1000, FALSE);
            if (square[cxa->i][cxa->j].isDestino != -1){
               cxa->bmp = load_bitmap("Images/scenario/box.bmp",NULL);
               play_sample(box_wrong, 255, 128, 1000, FALSE);
               left++;   
            }  
          }
     }
}

void checkAnimation(){
     if (gameRunning){
         if (bond.walked_x >=50 || bond.walked_y >=50){
             if (bond.dx>0)
                 bond.j++;      
             else if (bond.dx<0)
                 bond.j--;
             else if (bond.dy>0)
                 bond.i++;
             else if (bond.dy<0)
                 bond.i--; 
             if (bond.dx<0 || bond.dy <0){
                 bond.x = bond.x+bond.dx+(bond.walked_x%50);
                 bond.y = bond.y+bond.dy+(bond.walked_y%50);
             }else{
                 bond.x = bond.x+bond.dx-(bond.walked_x%50);
                 bond.y = bond.y+bond.dy-(bond.walked_y%50);  
             }
             bond.dx=0;
             bond.dy=0;
             bond.cy=bond.cx-1;
             bond.cx=0;
             bond.walked_x=0;
             bond.walked_y=0;    
         }
         if ( bond.cx == 1 ){
              bond.dx -= BOND_VEL;
              bond.cy++;
              bond.walked_x+=BOND_VEL;     
         }
         if ( bond.cx == 2 ){
              bond.dx += BOND_VEL;
              bond.cy++;     
              bond.walked_x+=BOND_VEL;
         }
         if ( bond.cx == 3 ){
              bond.dy -= BOND_VEL;
              bond.cy++;     
              bond.walked_y+=BOND_VEL;
         } 
         if ( bond.cx == 4 ){
              bond.dy += BOND_VEL;
              bond.cy++;     
              bond.walked_y+=BOND_VEL;
         }
         if ( bond.cx >= 5 && bond.cx <= 8 ){
              bond.cy++;
              if ( bond.cy>=9){
                 bond.cy=bond.cx-5;
                 bond.cx=0; 
              }
         }
         if ( shoot_dest != -1){
            if (tiro.percorrido < shoot_dest){
                tiro.percorrido+=12;
            }else{
                tiro.percorrido=0;
                explosao.active=1;
                explosao.x=tiro.x+tiro.dx;
                explosao.y=tiro.y+tiro.dy;
                tiro.x=-50;
                tiro.y=-50;
                tiro.dx=0;
                tiro.dy=0;
                if (tiro.onbox){
                    MoveCaixa(&caixa[square[tiro.dest_i][tiro.dest_j].isCaixa],tiro.dir);
                    play_sample(box_shot, 255, 128, 1000, FALSE);
                }else{
                    play_sample(wall_shot, 255, 128, 1000, FALSE);      
                }
                tiro.onbox=0;
                shoot_dest=-1;
            }
            if (tiro.dir == 0){
                tiro.dx -=12;          
            }else if (tiro.dir == 1){ 
                tiro.dx +=12; 
            }else if (tiro.dir == 2){ 
                tiro.dy -=12;
            }else if (tiro.dir == 3){ 
                tiro.dy +=12;
            }  
         }
         int i;
         for (i=0;i<caixas_qtd;i++){
              if (caixa[i].isMoving){                   
                  if (caixa[i].percorrido >= 50){
                     if (caixa[i].dir == 0){
                          square[caixa[i].i][caixa[i].j-1].isCaixa = square[caixa[i].i][caixa[i].j].isCaixa;
                          square[caixa[i].i][caixa[i].j].isCaixa=-1;      
                          caixa[i].j--;
                          if (square[caixa[i].i][caixa[i].j].isDestino != -1){
                              caixa[i].bmp = load_bitmap("Images/scenario/box_ok.bmp",NULL);  
                              play_sample(box_ok, 255, 128, 1000, FALSE);
                              left--;
                          }                                    
                     }else if (caixa[i].dir == 1){ 
                          square[caixa[i].i][caixa[i].j+1].isCaixa = square[caixa[i].i][caixa[i].j].isCaixa;
                          square[caixa[i].i][caixa[i].j].isCaixa=-1;      
                          caixa[i].j++;  
                          if (square[caixa[i].i][caixa[i].j].isDestino != -1){
                              caixa[i].bmp = load_bitmap("Images/scenario/box_ok.bmp",NULL);
                              play_sample(box_ok, 255, 128, 1000, FALSE);
                              left--;
                          }    
                     }else if (caixa[i].dir == 2){ 
                          square[caixa[i].i-1][caixa[i].j].isCaixa = square[caixa[i].i][caixa[i].j].isCaixa;
                          square[caixa[i].i][caixa[i].j].isCaixa=-1;      
                          caixa[i].i--; 
                          if (square[caixa[i].i][caixa[i].j].isDestino != -1){
                              caixa[i].bmp = load_bitmap("Images/scenario/box_ok.bmp",NULL);
                              play_sample(box_ok, 255, 128, 1000, FALSE);
                              left--;
                          }    
                     }else if (caixa[i].dir == 3){ 
                          square[caixa[i].i+1][caixa[i].j].isCaixa = square[caixa[i].i][caixa[i].j].isCaixa;
                          square[caixa[i].i][caixa[i].j].isCaixa=-1;      
                          caixa[i].i++; 
                          if (square[caixa[i].i][caixa[i].j].isDestino != -1){
                              caixa[i].bmp = load_bitmap("Images/scenario/box_ok.bmp",NULL);
                              play_sample(box_ok, 255, 128, 1000, FALSE);
                              left--;
                          }    
                     }
                     if (left <= 0){
                         play_sample(newlevel, 255, 128, 1000, FALSE); 
                         level++;
                         movs=0;
                         keyIsPressed = 0;
                         for (i=0;i<5;i++)
                             keyMap[i]=0;
                         box_moving = 0;
                         shoot_dest = -1;
                         score += (2*(10*bullets)+1*(5*(minutos*60)+5*segundos))-5*movs;
                         readInput();
                         return;     
                     }  
                     if (caixa[i].dir == 0){              
                         caixa[i].x = caixa[i].x+caixa[i].dx+(caixa[i].percorrido%50);
                     }else if (caixa[i].dir == 1){
                         caixa[i].x = caixa[i].x+caixa[i].dx-(caixa[i].percorrido%50);  
                     }else if (caixa[i].dir == 2){
                         caixa[i].y = caixa[i].y+caixa[i].dy+(caixa[i].percorrido%50);  
                     }else if (caixa[i].dir == 3){
                         caixa[i].y = caixa[i].y+caixa[i].dy-(caixa[i].percorrido%50);  
                     }
                     caixa[i].percorrido = 0;  
                     caixa[i].dx=0;
                     caixa[i].dy=0;
                     box_moving--;
                     caixa[i].isMoving=0;
                  }else{                 
                      if (caixa[i].dir == 0){
                            caixa[i].dx -=5;          
                      }else if (caixa[i].dir == 1){ 
                            caixa[i].dx +=5; 
                      }else if (caixa[i].dir == 2){ 
                            caixa[i].dy -=5;
                      }else if (caixa[i].dir == 3){ 
                            caixa[i].dy +=5;
                      }
                      caixa[i].percorrido+=5;
                  }        
              } 
         }
         if (explosao.active){
             if (explosao.curr==15){
                explosao.active=0;
                explosao.x=-50;
                explosao.y=-50;
                explosao.curr=0;   
             }else{
                explosao.curr++;
             }
         }
         if (bullets == 0 || (minutos<=0 && segundos <= 0) ){
             reset = 1;            
             play_sample(box_wrong, 255, 128, 1000, FALSE);  
             movs=0;
             keyIsPressed = 0;
             for (i=0;i<5;i++)
                 keyMap[i]=0;
             box_moving = 0;
             tiro.x = -50;
             tiro.y = -50;
             shoot_dest = -1;
             if (level!=1 && score >=0){
                score -= 500;
             }else{
                score = 0;      
             }
             readInput();
             return;         
         }
     }
       
}
END_OF_FUNCTION(checkAnimation);

void atualizaTempo(){
     if (gameRunning){
         if (segundos == 0){
             minutos--;
             if (minutos==0){}
             segundos=59;         
         }else{  
             segundos--;      
         }       
         if (minutos==0 && segundos <= 30)
            play_sample(warning, 255, 128, 1000, FALSE);
     }
}
END_OF_FUNCTION(atualizaTempo);

int setup_load (void){
    allegro_init();
    install_keyboard();
    int i;
    for (i=0;i<5;i++)
        keyMap[i]=0;
    install_mouse();
    install_timer();
    LOCK_VARIABLE(bond);
    LOCK_FUNCTION(checkAnimation);
    LOCK_VARIABLE(minutos);
    LOCK_VARIABLE(segundos);
    LOCK_FUNCTION(atualizaTempo);
    install_int(checkAnimation, UPDATE_TIME);
    install_int(atualizaTempo, 1000);
    if ( desktop_color_depth() != 0 )
        set_color_depth(desktop_color_depth());
    else
        set_color_depth(32);  
    if (set_gfx_mode(GFX_AUTODETECT, WIN_X, WIN_Y, 0, 0) < 0){
        printf("Erro ao inicializar o modo grafico");
        return (FALSE);
    }
    if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) < 0){
        printf("Erro ao inicializar o som");
        return (FALSE);
    }
    if( set_display_switch_mode(SWITCH_BACKAMNESIA) !=0){
        fprintf(stderr,"Error: can not change switch mode to BACKAMNESIA");
        return FALSE;
    }
    set_display_switch_callback(SWITCH_OUT,alt_tab_out);
    set_display_switch_callback(SWITCH_IN,alt_tab_in);
    
     shot = load_sample("Songs/gun.wav");
     box_move = load_sample("Songs/box_move.wav");
     box_ok = load_sample("Songs/box_ok.wav");
     box_shot = load_sample("Songs/box_shot.wav");
     box_wrong = load_sample("Songs/box_wrong.wav");
     wall_shot = load_sample("Songs/wall_shot.wav");
     newlevel = load_sample("Songs/newlevel.wav");
     warning = load_sample("Songs/warning.wav");
     
    return (TRUE);
}

void alt_tab_in(void){
     if (current_song != NULL)
         adjust_sample(current_song, current_vol, 128, 1000, TRUE); 
} 
void alt_tab_out(void){
     if (current_song != NULL)
         adjust_sample(current_song, 0, 128, 1000, TRUE);  
}

void loadBond (Char* bond){
     BITMAP* stand = load_bitmap("Images/James Bond/stand1.bmp",NULL);
     BITMAP* standUpDown = load_bitmap("Images/James Bond/standUpDown.bmp",NULL);
     BITMAP* run1 = load_bitmap("Images/James Bond/run1.bmp",NULL);
     BITMAP* run2 = load_bitmap("Images/James Bond/run2.bmp",NULL);
     BITMAP* run3 = load_bitmap("Images/James Bond/run3.bmp",NULL);
     BITMAP* run4 = load_bitmap("Images/James Bond/run4.bmp",NULL);
     BITMAP* run5 = load_bitmap("Images/James Bond/run5.bmp",NULL);
     BITMAP* run6 = load_bitmap("Images/James Bond/run6.bmp",NULL);
     BITMAP* run7 = load_bitmap("Images/James Bond/run7.bmp",NULL);
     BITMAP* run8 = load_bitmap("Images/James Bond/run8.bmp",NULL);
     BITMAP* runUpDown1 = load_bitmap("Images/James Bond/runUpDown1.bmp",NULL);
     BITMAP* runUpDown2 = load_bitmap("Images/James Bond/runUpDown2.bmp",NULL);
     BITMAP* runUpDown3 = load_bitmap("Images/James Bond/runUpDown3.bmp",NULL);
     BITMAP* runUpDown4 = load_bitmap("Images/James Bond/runUpDown4.bmp",NULL);
     BITMAP* shoot1 = load_bitmap("Images/James Bond/shoot1.bmp",NULL);
     BITMAP* shoot2 = load_bitmap("Images/James Bond/shoot2.bmp",NULL);
     BITMAP* shoot3 = load_bitmap("Images/James Bond/shoot3.bmp",NULL);
     BITMAP* shoot4 = load_bitmap("Images/James Bond/shoot4.bmp",NULL);
     BITMAP* shootUpDown1 = load_bitmap("Images/James Bond/shootUpDown1.bmp",NULL);
     BITMAP* shootUpDown2 = load_bitmap("Images/James Bond/shootUpDown2.bmp",NULL);
    
     bond->bmp[0][0] = create_sub_bitmap(stand,0,0,50,50);
     bond->bmp[0][1] = create_sub_bitmap(stand,50,0,50,50);
     bond->bmp[0][2] = create_sub_bitmap(standUpDown,0,0,50,50);
     bond->bmp[0][3] = create_sub_bitmap(standUpDown,50,0,50,50);
     bond->bmp[1][0] = create_sub_bitmap(run1,0,0,50,50);
     bond->bmp[1][1] = create_sub_bitmap(run2,0,0,50,50);
     bond->bmp[1][2] = create_sub_bitmap(run3,0,0,50,50);
     bond->bmp[1][3] = create_sub_bitmap(run4,0,0,50,50);
     bond->bmp[1][4] = create_sub_bitmap(run5,0,0,50,50);
     bond->bmp[1][5] = create_sub_bitmap(run6,0,0,50,50);
     bond->bmp[1][6] = create_sub_bitmap(run7,0,0,50,50);
     bond->bmp[1][7] = create_sub_bitmap(run8,0,0,50,50);
     bond->bmp[2][0] = create_sub_bitmap(run1,50,0,50,50);
     bond->bmp[2][1] = create_sub_bitmap(run2,50,0,50,50);
     bond->bmp[2][2] = create_sub_bitmap(run3,50,0,50,50);
     bond->bmp[2][3] = create_sub_bitmap(run4,50,0,50,50);
     bond->bmp[2][4] = create_sub_bitmap(run5,50,0,50,50);
     bond->bmp[2][5] = create_sub_bitmap(run6,50,0,50,50);
     bond->bmp[2][6] = create_sub_bitmap(run7,50,0,50,50);
     bond->bmp[2][7] = create_sub_bitmap(run8,50,0,50,50);
     bond->bmp[3][0] = create_sub_bitmap(runUpDown1,0,0,50,50);
     bond->bmp[3][1] = create_sub_bitmap(runUpDown3,0,0,50,50);
     bond->bmp[3][2] = create_sub_bitmap(runUpDown2,0,0,50,50);
     bond->bmp[3][3] = create_sub_bitmap(runUpDown4,0,0,50,50);
     bond->bmp[3][4] = create_sub_bitmap(runUpDown1,0,0,50,50);
     bond->bmp[3][5] = create_sub_bitmap(runUpDown3,0,0,50,50);
     bond->bmp[3][6] = create_sub_bitmap(runUpDown2,0,0,50,50);
     bond->bmp[3][7] = create_sub_bitmap(runUpDown4,0,0,50,50);
     bond->bmp[4][0] = create_sub_bitmap(runUpDown1,50,0,50,50);
     bond->bmp[4][1] = create_sub_bitmap(runUpDown3,50,0,50,50);
     bond->bmp[4][2] = create_sub_bitmap(runUpDown2,50,0,50,50);
     bond->bmp[4][3] = create_sub_bitmap(runUpDown4,50,0,50,50);
     bond->bmp[4][4] = create_sub_bitmap(runUpDown1,50,0,50,50);
     bond->bmp[4][5] = create_sub_bitmap(runUpDown3,50,0,50,50);
     bond->bmp[4][6] = create_sub_bitmap(runUpDown2,50,0,50,50);
     bond->bmp[4][7] = create_sub_bitmap(runUpDown4,50,0,50,50);
     bond->bmp[5][0] = create_sub_bitmap(shoot1,0,0,50,50);
     bond->bmp[5][1] = create_sub_bitmap(shoot1,0,0,50,50);
     bond->bmp[5][2] = create_sub_bitmap(shoot2,0,0,50,50);
     bond->bmp[5][3] = create_sub_bitmap(shoot2,0,0,50,50);
     bond->bmp[5][4] = create_sub_bitmap(shoot3,0,0,50,50);
     bond->bmp[5][5] = create_sub_bitmap(shoot3,0,0,50,50);
     bond->bmp[5][6] = create_sub_bitmap(shoot4,0,0,50,50);
     bond->bmp[5][7] = create_sub_bitmap(shoot4,0,0,50,50);
     bond->bmp[6][0] = create_sub_bitmap(shoot1,50,0,50,50);
     bond->bmp[6][1] = create_sub_bitmap(shoot1,50,0,50,50);
     bond->bmp[6][2] = create_sub_bitmap(shoot2,50,0,50,50);
     bond->bmp[6][3] = create_sub_bitmap(shoot2,50,0,50,50);
     bond->bmp[6][4] = create_sub_bitmap(shoot3,50,0,50,50);
     bond->bmp[6][5] = create_sub_bitmap(shoot3,50,0,50,50);
     bond->bmp[6][6] = create_sub_bitmap(shoot4,50,0,50,50);
     bond->bmp[6][7] = create_sub_bitmap(shoot4,50,0,50,50);
     bond->bmp[7][0] = create_sub_bitmap(shootUpDown1,0,0,50,50);
     bond->bmp[7][1] = create_sub_bitmap(shootUpDown1,0,0,50,50);
     bond->bmp[7][2] = create_sub_bitmap(shootUpDown1,0,0,50,50);
     bond->bmp[7][3] = create_sub_bitmap(shootUpDown1,0,0,50,50);
     bond->bmp[7][4] = create_sub_bitmap(shootUpDown2,0,0,50,50);
     bond->bmp[7][5] = create_sub_bitmap(shootUpDown2,0,0,50,50);
     bond->bmp[7][6] = create_sub_bitmap(shootUpDown2,0,0,50,50);
     bond->bmp[7][7] = create_sub_bitmap(shootUpDown2,0,0,50,50);
     bond->bmp[8][0] = create_sub_bitmap(shootUpDown1,50,0,50,50);
     bond->bmp[8][1] = create_sub_bitmap(shootUpDown1,50,0,50,50);
     bond->bmp[8][2] = create_sub_bitmap(shootUpDown1,50,0,50,50);
     bond->bmp[8][3] = create_sub_bitmap(shootUpDown1,50,0,50,50);
     bond->bmp[8][4] = create_sub_bitmap(shootUpDown2,50,0,50,50);
     bond->bmp[8][5] = create_sub_bitmap(shootUpDown2,50,0,50,50);
     bond->bmp[8][6] = create_sub_bitmap(shootUpDown2,50,0,50,50);
     bond->bmp[8][7] = create_sub_bitmap(shootUpDown2,50,0,50,50);
     
     bond->mod[0]=10;
     bond->mod[1]=8;
     bond->mod[2]=8;
     bond->mod[3]=8;
     bond->mod[4]=8;
     bond->mod[5]=8;
     bond->mod[6]=8;
     bond->mod[7]=8;
     bond->mod[8]=8;
     
     bond->cx = 0;
     bond->cy = 1;
     bond->cm = 0;
     bond->dx = 0;
     bond->dy = 0;
     bond->walked_x = 0;
     bond->walked_y = 0;
     
     tiro.bmp = load_bitmap("Images/scenario/shoot.bmp",NULL);
     tiro.percorrido=0;
     tiro.x=-50;
     tiro.y=-50;
     tiro.dx=0;
     tiro.dy=0;
     tiro.onbox=0;
     shoot_dest=-1; 
     box_moving=0;
     
     BITMAP* explosion;
     explosion = load_bitmap("Images/scenario/explosion.bmp",NULL);
     
     int i,j;
     for (i=0;i<4;i++)
        for (j=0;j<4;j++)
            explosao.bmp[i*4+j] = create_sub_bitmap(explosion,j*16,i*16,16,16);
     explosao.x=-50;
     explosao.y=-50; 
     explosao.curr=0;
     explosao.active=0;
     
     terminal = load_font("Font/terminal.pcx", NULL, NULL);
     movs=0;      
     paused=0;
            
}

void buscaTiroDest(int dir){
    int i = tiro.i;
    int j = tiro.j;
    int count=0;
    if (dir == 0){
        for (j=j-1;j>=0;j--){
             count++;
             if (square[i][j].isParede != -1){   
                 shoot_dest = 3+50*(count-1);
                 break;
             }else if(square[i][j].isCaixa != -1){
                 shoot_dest = 11+50*(count-1);
                 tiro.onbox=1;      
                 break;            
             }
        }  
    }
    if (dir == 1){
       for (j=j+1;j<=13;j++){
             count++;
             if (square[i][j].isParede != -1){   
                 shoot_dest = 50*(count-1)+10;
                 break;
             }else if(square[i][j].isCaixa != -1){ 
                 shoot_dest = 50*(count-1)+22; 
                 tiro.onbox=1;
                 break;                    
             }
        }  
    }
    if (dir == 2){
       for (i=i-1;i>=0;i--){
             count++;
             if (square[i][j].isParede != -1){   
                 shoot_dest = 50*(count-1)-3;
                 break;
             }else if(square[i][j].isCaixa != -1){
                 shoot_dest = 50*(count-1);
                 tiro.onbox=1;   
                 break;                
             }
        }  
    }
    if (dir == 3){
       for (i=i+1;i<=11;i++){
             count++;
             if (square[i][j].isParede != -1){   
                 shoot_dest = 20+50*(count-1);
                 break;
             }else if(square[i][j].isCaixa != -1){
                 shoot_dest = 25+50*(count-1); 
                 tiro.onbox=1;
                 break;                  
             }
        }
    }
    tiro.dest_i = i;
    tiro.dest_j = j;     
}

int nextSquareIsFree(int i, int j, int dir){
    if (dir == 0){
        if (square[i][j-1].isParede == -1 && square[i][j-1].isCaixa == -1){
           return 1;        
        }
        else
           return 0;    
    }
    if (dir == 1){
        if (square[i][j+1].isParede == -1 && square[i][j+1].isCaixa == -1)
           return 1;        
        else
           return 0;    
    }
    if (dir == 2){
        if (square[i-1][j].isParede == -1 && square[i-1][j].isCaixa == -1)
           return 1;        
        else
           return 0;    
    }
    if (dir == 3){
        if (square[i+1][j].isParede == -1 && square[i+1][j].isCaixa == -1)
           return 1;        
        else
           return 0;    
    }
    
}

void checkKeyboard (Char* bond){
     if (key[KEY_LEFT] && !keyMap[0] && !keyIsPressed){     
         keyMap[0]=1;
         keyIsPressed++;
         if ( bond->cx == 0 && bond->cy != 0 )
              bond->cy = 0;
         else if ( bond->cx == 0 && bond->cy == 0 && nextSquareIsFree(bond->i,bond->j,0) ){
              bond->cx = 1;
              bond->cy = 0;
              bond->cm = 1;
              movs++;
         }         
     }else if (!key[KEY_LEFT] && keyMap[0]){
         keyIsPressed--;
         keyMap[0]=0;     
     }
     
     if (key[KEY_RIGHT] && !keyMap[1] && !keyIsPressed){           
         keyMap[1]=1;
         keyIsPressed++;
         if ( bond->cx == 0 && bond->cy != 1 )
              bond->cy = 1;
         else if ( bond->cx == 0 && bond->cy == 1 && nextSquareIsFree(bond->i,bond->j,1) ){
              bond->cx = 2;
              bond->cy = 0;
              bond->cm = 2;
              movs++;
         }              
     }else if (!key[KEY_RIGHT] && keyMap[1]){
         keyIsPressed--;
         keyMap[1]=0;       
     }
     
     if (key[KEY_UP] && !keyMap[2] && !keyIsPressed){           
         keyMap[2]=1;
         keyIsPressed++;
         if ( bond->cx == 0 && bond->cy != 2 )
              bond->cy = 2;  
         else if ( bond->cx == 0 && bond->cy == 2 && nextSquareIsFree(bond->i,bond->j,2) ){
              bond->cx = 3;
              bond->cy = 0;
              bond->cm = 3;
              movs++;
         }         
     }else if (!key[KEY_UP] && keyMap[2]){
         keyIsPressed--;
         keyMap[2]=0;       
     }
     
     if (key[KEY_DOWN] && !keyMap[3] && !keyIsPressed){           
         keyMap[3]=1;
         keyIsPressed++;
         if ( bond->cx == 0 && bond->cy != 3 )
              bond->cy = 3;  
         else if ( bond->cx == 0 && bond->cy == 3 && nextSquareIsFree(bond->i,bond->j,3) ){
              bond->cx = 4;
              bond->cy = 0;
              bond->cm = 4;
              movs++;
         }      
     }else if (!key[KEY_DOWN] && keyMap[3]){
         keyIsPressed--;
         keyMap[3]=0;       
     }
     if (key[KEY_X] && !keyMap[4] && !keyIsPressed){           
         keyMap[4]=1;
         keyIsPressed++;
         if ( bond->cx == 0 && shoot_dest == -1 && box_moving == 0 ){
            bond->cx = bond->cy+5;
            bond->cm = bond->cy+5;
            tiro.i = bond->i;
            tiro.j = bond->j;
            tiro.dx=0;
            tiro.dy=0;
            tiro.percorrido=0;
            if (bond->cy == 0){
               tiro.dir=0;    
               tiro.x = bond->x;
               tiro.y = bond->y+20;
               buscaTiroDest(0); 
            }else if (bond->cy == 1){
               tiro.dir=1;
               tiro.x = bond->x+30;
               tiro.y = bond->y+20;
               buscaTiroDest(1);    
            }else if (bond->cy == 2){
               tiro.dir=2;   
               tiro.x = bond->x+25;
               tiro.y = bond->y;
               buscaTiroDest(2);    
            }else if (bond->cy == 3){
               tiro.dir=3;   
               tiro.x = bond->x+25;
               tiro.y = bond->y+35;
               buscaTiroDest(3);    
            }
             bond->cy = 0;
             bullets--;
             play_sample(shot, 255, 128, 1000, FALSE);
             if (bullets <= 10)
                play_sample(warning, 255, 128, 1000, FALSE);  
         }
     }else if (!key[KEY_X] && keyMap[4]){
         keyIsPressed--;
         keyMap[4]=0;       
     }           
     
}

void new_game(void){
     BITMAP* scenario;
     BITMAP* scenario_background;
     BITMAP* grid;
     BITMAP* parede;
     BITMAP* tile;
     BITMAP* box;
     BITMAP* dest;
     BITMAP* panel;
     BITMAP* bullet;
     BITMAP* mini_box;
     BITMAP* solve;
     BITMAP* pause;
     BITMAP* quit;
     BITMAP* pausado;
     BITMAP* reset_button;
     BITMAP* getScore;
     BITMAP* ranking;
     Button button[4];
     
     scenario = create_bitmap(WIN_X, WIN_Y);
     scenario_background = load_bitmap("Images/scenario/stage.bmp",NULL);
     grid = load_bitmap("Images/scenario/grid.bmp",NULL);
     parede = load_bitmap("Images/scenario/wall.bmp",NULL);
     tile = load_bitmap("Images/scenario/tile.bmp",NULL);
     box = load_bitmap("Images/scenario/box.bmp",NULL);
     dest = load_bitmap("Images/scenario/destination.bmp",NULL);
     panel = load_bitmap("Images/scenario/panel.bmp",NULL);
     bullet = load_bitmap("Images/scenario/bullet.bmp",NULL);
     mini_box = load_bitmap("Images/scenario/box_icon.bmp",NULL);
     solve = load_bitmap("Images/scenario/solve_button.bmp",NULL);
     pause = load_bitmap("Images/scenario/pause_button.bmp",NULL);
     quit = load_bitmap("Images/scenario/quit_button.bmp",NULL);
     pausado = load_bitmap("Images/scenario/pausado.bmp",NULL);
     reset_button = load_bitmap("Images/scenario/reset_button.bmp",NULL);
     getScore = load_bitmap("Images/scenario/getScore.bmp",NULL);
     ranking = load_bitmap("Images/scenario/ranking.bmp",NULL);
     button[0].bmp[0] = create_sub_bitmap(solve,0,0,117,56);
     button[0].bmp[1] = create_sub_bitmap(solve,117,0,117,56);
     button[1].bmp[0] = create_sub_bitmap(pause,0,0,117,56);
     button[1].bmp[1] = create_sub_bitmap(pause,117,0,117,56);
     button[2].bmp[0] = create_sub_bitmap(reset_button,0,0,117,56);
     button[2].bmp[1] = create_sub_bitmap(reset_button,117,0,117,56);
     button[3].bmp[0] = create_sub_bitmap(quit,0,0,117,56);
     button[3].bmp[1] = create_sub_bitmap(quit,117,0,117,56);
     button[0].x = 6; button[0].y = 350;
     button[1].x = 6; button[1].y = 410;
     button[2].x = 6; button[2].y = 470;
     button[3].x = 6; button[3].y = 530;
     button[0].curr = 0;
     button[1].curr = 0;
     button[2].curr = 0;
     button[3].curr = 0;
     
     int i,j;
     char s;
     if (square!=NULL)
        for (i=0;i<11;i++)
            free(square[i]);
     free(square);
     square = (Square**) malloc (11*sizeof(Square*));
     for (i=0;i<11;i++)
         square[i] = (Square*) malloc (13*sizeof(Square));
     
     loadBond(&bond);
     
     if (input!=NULL)
        fclose(input);
     input = fopen ("sokoban.007","r");
     readInput();
      
     while(gameRunning || paused || gettingScore){                        
          draw_sprite(scenario,scenario_background,0,0);
          draw_sprite(scenario,panel,6,18);
          draw_sprite(scenario,bullet,20,152);
          draw_sprite(scenario,mini_box,19,207);
          for (i=0;i<4;i++){
              draw_sprite(scenario,button[i].bmp[button[i].curr],6,350+(i*60));    
          }
          for ( i=0; i < 4; i++){
                  if ( mouse_x >= button[i].x && mouse_x <= (button[i].x + button[i].bmp[0]->w) &&
                       mouse_y >= button[i].y && mouse_y <= (button[i].y + button[i].bmp[0]->h)){
                      button[i].curr=1;           
                  }else{
                      button[i].curr=0;                                                                
                  }
          }
          textprintf_ex(scenario, terminal, 35, 58, makecol(255,255,255), -1, "%d", level);
          if (!(minutos == 0 && segundos <= 30)){
              if (segundos>=10)
                 textprintf_ex(scenario, terminal, 20, 108, makecol(255,255,255), -1, "%d:%d", minutos,segundos);
              else
                 textprintf_ex(scenario, terminal, 20, 108, makecol(255,255,255), -1, "%d:0%d", minutos,segundos); 
          }else{
              if (segundos>=10)
                 textprintf_ex(scenario, terminal, 20, 108, makecol(255,0,0), -1, "%d:%d", minutos,segundos);
              else
                 textprintf_ex(scenario, terminal, 20, 108, makecol(255,0,0), -1, "%d:0%d", minutos,segundos);  
               
          }        
          if (bullets > 10)
             textprintf_ex(scenario, terminal, 31, 160, makecol(255,255,255), -1, "x%d", bullets);
          else
             textprintf_ex(scenario, terminal, 31, 160, makecol(255,0,0), -1, "x%d", bullets);   
          textprintf_ex(scenario, terminal, 42, 213, makecol(255,255,255), -1, "x%d", left);
          textprintf_ex(scenario, terminal, 35, 258, makecol(255,255,255), -1, "%d", movs);    
          textprintf_ex(scenario, terminal, 35, 308, makecol(255,255,255), -1, "%d", score);
         
          for (i=0;i<11;i++){
              for (j=0;j<13;j++){
                   if (square[i][j].isParede != -1)
                       draw_sprite(scenario, parede, FIELD_X+50*j, FIELD_Y+50*i);
                   else
                       draw_sprite(scenario, tile, FIELD_X+50*j, FIELD_Y+50*i);
                   if (square[i][j].isDestino != -1)
                       draw_sprite(scenario, dest, FIELD_X+50*j, FIELD_Y+50*i);
              }
          }
          draw_sprite(scenario,grid,FIELD_X,FIELD_Y);
          for (i=0;i<caixas_qtd;i++){
               draw_sprite(scenario, caixa[i].bmp, caixa[i].x+caixa[i].dx, caixa[i].y+caixa[i].dy);
          }    
          draw_sprite(scenario, tiro.bmp, tiro.x+tiro.dx, tiro.y+tiro.dy);
          draw_sprite(scenario, explosao.bmp[explosao.curr], explosao.x, explosao.y);
          draw_sprite(scenario, bond.bmp[bond.cx][bond.cy%bond.mod[bond.cm]], bond.x+bond.dx, bond.y+bond.dy);   
          
          if (!paused || !gettingScore)
             checkKeyboard(&bond);
             
          if (mouse_b & 1){ 
             if (button[2].curr==1){
                 while(mouse_b & 1);         
                 reset = 1;            
                 play_sample(box_wrong, 255, 128, 1000, FALSE);  
                 movs=0;
                 keyIsPressed = 0;
                 for (i=0;i<5;i++)
                     keyMap[i]=0;
                 box_moving = 0;
                 shoot_dest = -1;
                 bond.dx=0;
                 bond.dy=0;
                 bond.cx=0;
                 bond.walked_x=0;
                 bond.walked_y=0;
                 caixas_qtd = 0;
                 readInput();
             } 
             if (button[3].curr==1){
                 while(mouse_b & 1);                    
                 play_sample(box_wrong, 255, 128, 1000, FALSE); 
                 level=0;
                 movs=0;
                 keyIsPressed = 0;
                 for (i=0;i<5;i++)
                     keyMap[i]=0;
                 box_moving = 0;
                 shoot_dest = -1;
                 score = 0;
                 paused = 0;
                 return;                           
             }
             if (button[1].curr==1){
                 while(mouse_b & 1);
                 if (!paused){                    
                    play_sample(box_wrong, 255, 128, 1000, FALSE); 
                    gameRunning = 0;    
                    paused = 1;                     
                 }else{
                    play_sample(box_wrong, 255, 128, 1000, FALSE);
                    gameRunning = 1;    
                    paused = 0;
                 }   
             }
          }
          if (paused)
             draw_sprite(scenario,pausado,130,20);
          if (gettingScore){
             draw_sprite(scenario,getScore,0,0);
             textprintf_ex(scenario, terminal, 355, 126, makecol(255,0,0), -1, "%d", score);
             if (nome[0] != 0){
                for (i=0;i<letter_pos;i++)      
                   textprintf_ex(scenario, terminal, 318+(i*25), 380, makecol(255,255,255), -1, "%c", nome[i]);         
             }
             if (keypressed()){
                 if (key[KEY_ENTER]){
                     play_sample(box_wrong, 255, 128, 1000, FALSE); 
                     draw_sprite(screen,ranking,0,0);
                     fscore = fopen("score.007", "r");
                     fscore_out = fopen("scoreaux.007", "w");
                     fscanf (fscore, "%d", &rank_qtd);
                     fprintf(fscore_out,"%d\n",rank_qtd+1);
                     fgets(linha,100,fscore);
                     for (i=0;i<rank_qtd;i++){
                         fgets(linha,100,fscore);
                         textprintf_ex(screen, terminal, 125, 75+(i*35), makecol(255,255,255), -1, "%s", linha);
                         fprintf(fscore_out,"%s",linha);
                     }
                     textprintf_ex(screen, terminal, 125, 75+(i*35), makecol(255,255,255), -1,"%s - %d\n",nome,score);    
                     fprintf(fscore_out,"%s - %d\n",nome,score);
                     fclose(fscore);
                     fclose(fscore_out);
                     remove("score.007");
                     rename("scoreaux.007", "score.007");
                     clear_keybuf();
                     readkey();
                     play_sample(box_wrong, 255, 128, 1000, FALSE); 
                     level=0;
                     movs=0;
                     keyIsPressed = 0;
                     for (i=0;i<5;i++)
                         keyMap[i]=0;
                     box_moving = 0;
                     shoot_dest = -1;
                     score = 0;
                     paused = 0;
                     gettingScore = 0;
                     gameRunning = 0;
                     return;          
                 }              
                 if (!(letter_pos > 6 && letter_pos >= 0)){             
                     s = readkey();
                     if (s == '\b'){
                        letter_pos--;
                        nome[letter_pos]= 'a';              
                     }else{
                        nome[letter_pos] = s;
                        letter_pos++;                  
                     }
                 }else{
                     s = readkey();  
                     if (s == '\b' && letter_pos>0){
                        letter_pos--;
                        nome[letter_pos]= 'a';
                     }else{
                        clear_keybuf();      
                     }
                 }
             }            
          }
          show_mouse(scenario);          
          draw_sprite(screen, scenario,0,0);
          show_mouse(NULL);
          rest(20);      
          clear(scenario);
     }
}

void menu (void){
     BITMAP* menu;
     BITMAP* menu_background;
     BITMAP* about;
     BITMAP* instructions;
     Button button[5];
     Button back_arrow[2];
     SAMPLE* menu_song;
     
     menu = create_bitmap(WIN_X, WIN_Y);
     menu_background = load_bitmap("Images/menu_big.bmp",NULL);
     about = load_bitmap("Images/about_page.bmp",NULL);
     instructions = load_bitmap("Images/instructions_page.bmp",NULL);
     back_arrow[0].bmp[0] = load_bitmap("Images/back_arrow.bmp",NULL);
     back_arrow[1].bmp[0] = load_bitmap("Images/back_arrow.bmp",NULL);
     button[0].bmp[0] = load_bitmap("Images/new_game.bmp",NULL);
     button[1].bmp[0] = load_bitmap("Images/load_game.bmp",NULL);
     button[2].bmp[0] = load_bitmap("Images/instructions.bmp",NULL);
     button[3].bmp[0] = load_bitmap("Images/about.bmp",NULL);
     button[4].bmp[0] = load_bitmap("Images/exit.bmp",NULL);
     back_arrow[0].bmp[1] = load_bitmap("Images/back_arrow2.bmp",NULL);
     back_arrow[1].bmp[1] = load_bitmap("Images/back_arrow2.bmp",NULL);
     button[0].bmp[1] = load_bitmap("Images/new_game2.bmp",NULL);
     button[1].bmp[1] = load_bitmap("Images/load_game2.bmp",NULL);
     button[2].bmp[1] = load_bitmap("Images/instructions2.bmp",NULL);
     button[3].bmp[1] = load_bitmap("Images/about2.bmp",NULL);
     button[4].bmp[1] = load_bitmap("Images/exit2.bmp",NULL);
     
     button[0].x = 476; button[0].y = 194;
     button[1].x = 467; button[1].y = 263;
     button[2].x = 439; button[2].y = 339;
     button[3].x = 516; button[3].y = 421;
     button[4].x = 530; button[4].y = 497;
     back_arrow[0].x = 557; back_arrow[0].y = 502; back_arrow[0].active=0;
     back_arrow[1].x = 549; back_arrow[1].y = 511; back_arrow[1].active=0;
     
     draw_sprite(menu,menu_background,0,0);
     draw_sprite(menu,button[0].bmp[0],476,194);
     draw_sprite(menu,button[1].bmp[0],467,263);
     draw_sprite(menu,button[2].bmp[0],439,339);
     draw_sprite(menu,button[3].bmp[0],516,421);
     draw_sprite(menu,button[4].bmp[0],530,497);
     
     menu_song = load_sample("Songs/menu.wav");
     current_song = menu_song;
     play_sample(menu_song, current_vol, 128, 1000, TRUE);  
     
     int i;
     int isPageOFF=1;
     int isPageOFF2=1;
     while(!key[KEY_ESC]){                
          for ( i=0; i < 5 && isPageOFF && isPageOFF2; i++){
                  if ( mouse_x >= button[i].x && mouse_x <= (button[i].x + button[i].bmp[0]->w) &&
                       mouse_y >= button[i].y && mouse_y <= (button[i].y + button[i].bmp[0]->h)){
                      draw_sprite(menu,button[i].bmp[1],button[i].x,button[i].y);
                      button[i].active=1;            
                  }else{
                      draw_sprite(menu,button[i].bmp[0],button[i].x,button[i].y);
                      button[i].active=0;                                                                
                  }
          }
          if (!isPageOFF){               
              if ( mouse_x >= back_arrow[0].x && mouse_x <= (back_arrow[0].x + back_arrow[0].bmp[0]->w) &&
                   mouse_y >= back_arrow[0].y && mouse_y <= (back_arrow[0].y + back_arrow[0].bmp[0]->h)){
                  draw_sprite(menu,back_arrow[0].bmp[1],back_arrow[0].x,back_arrow[0].y);
                  back_arrow[i].active=1;             
              }else{
                  draw_sprite(menu,back_arrow[0].bmp[0],back_arrow[0].x,back_arrow[0].y);
                  back_arrow[0].active=0;      
              }                 
          }
          if (!isPageOFF2){               
              if ( mouse_x >= back_arrow[1].x && mouse_x <= (back_arrow[1].x + back_arrow[1].bmp[0]->w) &&
                   mouse_y >= back_arrow[1].y && mouse_y <= (back_arrow[1].y + back_arrow[1].bmp[0]->h)){
                  draw_sprite(menu,back_arrow[1].bmp[1],back_arrow[1].x,back_arrow[1].y);
                  back_arrow[1].active=1;             
              }else{
                  draw_sprite(menu,back_arrow[1].bmp[0],back_arrow[1].x,back_arrow[1].y);
                  back_arrow[1].active=0;      
              }                 
          }              
          if (mouse_b & 1){ 
             if (button[3].active){
                 while(mouse_b & 1);
                 play_sample(box_wrong, 255, 128, 1000, FALSE);                  
                 draw_sprite(menu,about,413,176);
                 draw_sprite(menu,back_arrow[0].bmp[0],back_arrow[0].x,back_arrow[0].y);
                 isPageOFF=0;
             }
             if (button[2].active){
                 while(mouse_b & 1);
                 play_sample(box_wrong, 255, 128, 1000, FALSE);                  
                 draw_sprite(menu,instructions,413,176);
                 draw_sprite(menu,back_arrow[1].bmp[1],back_arrow[1].x,back_arrow[1].y);
                 isPageOFF2=0;
             }               
             if (button[0].active){
                 while(mouse_b & 1);
                 play_sample(box_wrong, 255, 128, 1000, FALSE);    
                 new_game();
             }
             if (back_arrow[0].active || back_arrow[1].active){
                 while(mouse_b & 1);
                 play_sample(box_wrong, 255, 128, 1000, FALSE);                   
                 clear(menu);
                 draw_sprite(menu,menu_background,0,0);
                 isPageOFF=1;
                 isPageOFF2=1;
                 back_arrow[0].active=0;
                 back_arrow[1].active=0;                       
             }  
             if (button[4].active)
                 end_all();          
          }
          
          show_mouse(menu);                                                                   
          draw_sprite(screen,menu,0,0);
          show_mouse(NULL);
          rest(10);
     }
}


void end_all(){
    if (input!=NULL)
       fclose(input); 
    allegro_exit();
    exit(0); 
}
