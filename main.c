#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ncurses.h>
#include <unistd.h>
#include <locale.h>

typedef struct ship {
    int startX ;
    int startY ;
    int width ;
    int height ;
    bool destroyed ;
} ship ;

typedef struct bullet {
    int positionX ;
    int positionY ;
    chtype bullte_image ;
    bool direction ; // 0 -> down , 1 -> up
    struct bullet * next ;
} bullet;

typedef struct enemy{
    int positionX ;
    int positionY ;
    int direction ; // 0 -> down , 1-> left , 2 -> right
    int width ;
    int height ;
    int lives ;
    int rank ;
    bool shoot ;
    bool destroyed ;
    struct enemy * next ;
}enemy;

//Functions Prototypes
void Init_ship_params() ;
void Print_ship(int posX , int posY);
void * Player_input();
void * Move_bullets();
void Delete_bullet(bullet * delete);
void Add_bullet(chtype image , bool is_player , int x , int y);
void free_bullets();
void * Draw_game();
void Print_enemy1(enemy * e);
void Add_enemy(int rank);
void  Create_enemys(int number_enemys , int rank);
void * Enemy_generator();
void * Move_enemys();
void Delete_enemy(enemy * delete);
void free_enemys();


//Global Variables
ship * player ;
bool finish = FALSE ;
int input ;
bullet * bullets_list = NULL ;
bullet * last_bullet = NULL;
int enemy_count = 0 ;
enemy * enemy_list = NULL ;
enemy * last_enemy = NULL ;
int score = 0 ;
int turn = 0 ;
bool flag[2] ;


//Mutex
pthread_mutex_t playerMutex ;
pthread_mutex_t bulletsMutex ;
pthread_mutex_t enemysMutex ;
pthread_mutex_t lock ;

//Problemas :
//Segmentation fault , printea raro en la pantalla
int main(void)
{
    setlocale(LC_ALL , "UTF-8");
    initscr();//Init ncurses
    curs_set(0);//Hide cursor
    timeout(1);
    noecho();
    srand(time(NULL));
    cbreak();
    flag[0] = false ;
    flag[1] = false ;
    int maxY ;
    int maxX ;
    getmaxyx(stdscr , maxY , maxX);
    //colors
    start_color();
    init_pair(1 , COLOR_CYAN , COLOR_BLACK);
    init_pair(2 , COLOR_GREEN , COLOR_BLACK);
    init_pair(3 , COLOR_WHITE , COLOR_BLACK);
    init_pair(4 , COLOR_RED , COLOR_BLACK);

    //init mutexs
    pthread_mutex_init(&playerMutex , NULL);
    pthread_mutex_init(&bulletsMutex , NULL);
    pthread_mutex_init(&enemysMutex , NULL);

    //threads
    pthread_t thread_input , thread_bullets , thread_draw , thread_enemys , thread_move_enemys;
    pthread_create(&thread_input , NULL , Player_input , NULL );
    pthread_create(&thread_bullets , NULL , Move_bullets , NULL);
    pthread_create(&thread_draw , NULL , Draw_game , NULL);
    pthread_create(&thread_enemys , NULL , Enemy_generator , NULL);
    pthread_create(&thread_move_enemys , NULL , Move_enemys , NULL);

    Init_ship_params();
    Print_ship(player->startX , player->startY);

    do {

        switch (input) {
            case KEY_LEFT:
                if(player->startX - 2 > 8) {
                    player->startX-=2 ;
                }
                break;
            case KEY_RIGHT:
                if(player->startX + 2 < COLS - 8) {
                    player->startX+=2 ;
                }
                break;
            case KEY_UP:
                if(player->startY - 2 > 0) {
                    player->startY-=2 ;
                }
                break;
            case KEY_DOWN:
                if(player->startY + 2 < LINES - 4) {
                    player->startY+=2 ;
                }
                break;
            case 32 :
                Add_bullet('|' , TRUE , player->startX , player->startY - 1);
                break;
            case 27:
                finish = true ;
                break;
        }
        input = 0 ;

        if(player->destroyed) finish = true ;
        usleep(500);
    }while(!finish);

    free(player);
    free_bullets();
    free_enemys();
    pthread_cancel(thread_input);
    pthread_cancel(thread_bullets);
    pthread_cancel(thread_draw);
    pthread_cancel(thread_enemys);
    pthread_cancel(thread_move_enemys);
    clear();
    mvprintw( maxY / 2 , maxX / 2 - 9  , "GAME OVER");
    refresh();
    sleep(2);
    endwin();
    return 0;
}

void Init_ship_params() {
    //Ship position
    player = (ship*)malloc(sizeof(ship));
    player->height = 5 ;
    player->width = 17 ;
    player->startX = (COLS- player->width)/2;
    player->startY = (LINES - player->width)/2 + 20;
    player->destroyed = FALSE ;
}

void Print_ship(int posX , int posY) {

    int x = posX;
    int y = posY;

    if(player->destroyed) attron(A_BLINK);
    attron(COLOR_PAIR(1));
    mvaddch(y , x ,'-');
    attroff(COLOR_PAIR(1));
    mvaddch(y , x-1 ,'/');
    mvaddch(y , x+1 ,'\\');

    mvaddch(y+1 , x ,'-');
    mvaddch(y+1 , x-1 ,'/');
    mvaddch(y+1 , x-2 ,'/');
    mvaddch(y+1 , x+1 ,'\\');
    mvaddch(y+1 , x+2 ,'\\');
    mvaddch(y+2 , x ,' ');
    attron(COLOR_PAIR(2));
    mvaddch(y+2 , x-1 ,'[');
    mvaddch(y+2 , x+1 ,']');
    mvaddch(y+2 , x+8 ,'\\');
    mvaddch(y+2 , x-8 ,'/');
    attroff(COLOR_PAIR(2));
    mvaddch(y+2 , x-2 ,'_');
    mvaddch(y+2 , x-3 ,'/');
    mvaddch(y+2 , x-4 ,'_');
    mvaddch(y+2 , x-5 ,'-');
    mvaddch(y+2 , x-6 ,'_');
    mvaddch(y+2 , x-7 ,'|');

    mvaddch(y+2 , x+2 ,'_');
    mvaddch(y+2 , x+3 ,'\\');
    mvaddch(y+2 , x+4 ,'_');
    mvaddch(y+2 , x+5 ,'-');
    mvaddch(y+2 , x+6 ,'_');
    mvaddch(y+2 , x+7 ,'|');


    mvaddch(y+3 , x ,'-');
    mvaddch(y+3 , x-1 ,'-');
    attron(COLOR_PAIR(2));
    mvaddch(y+3 , x-2 ,'|');
    mvaddch(y+3 , x-3 ,'|');
    mvaddch(y+3 , x-8 ,'\\');
    mvaddch(y+3 , x+8 ,'/');
    attroff(COLOR_PAIR(2));
    mvaddch(y+3 , x-4 ,'-');
    mvaddch(y+3 , x-5 ,'_');
    mvaddch(y+3 , x-6 ,'-');
    mvaddch(y+3 , x-7 ,'|');

    mvaddch(y+3 , x+1 ,'-');
    attron(COLOR_PAIR(2));
    mvaddch(y+3 , x+2 ,'|');
    mvaddch(y+3 , x+3 ,'|');
    attroff(COLOR_PAIR(2));
    mvaddch(y+3 , x+4 ,'-');
    mvaddch(y+3 , x+5 ,'_');
    mvaddch(y+3 , x+6 ,'-');
    mvaddch(y+3 , x+7 ,'|');

    attron(COLOR_PAIR(2));
    mvaddch(y+4 , x ,'-');
    attroff(COLOR_PAIR(2));
    mvaddch(y+4 , x-1 ,'\\');
    mvaddch(y+4 , x-2,'<');
    mvaddch(y+4 , x+1 ,'/');
    mvaddch(y+4 , x+2 ,'>');
    if(player->destroyed) attroff(A_BLINK);
}

void * Player_input() {
    keypad(stdscr , true) ;
    int key ;
    while (1) {
        flag[0] = true ;
        while (flag[1]) {
            flag[0] = false;
            while (turn!= 0){}
            flag[0] = true ;
        }
        input = getch() ;
        turn = 1 ;
        flag[0] = false ;
    }
    return NULL ;
}

void * Move_bullets() {
    while (1) {
        pthread_mutex_lock(&bulletsMutex);
        bullet * current_bullet = bullets_list ;
        enemy * current_enemy = enemy_list ;
        while (current_bullet != NULL) {
            //if direction == 1 then up else down
            if(current_bullet->direction) {
                current_bullet->positionY -= 1 ;
            }else current_bullet->positionY += 1 ;

            //if bullet is out of bounds then delete it
            if(current_bullet->positionY < 0 || current_bullet->positionY > LINES) {
                Delete_bullet(current_bullet);
            }
            else if(!current_bullet->direction) {
                if(current_bullet->positionY == player->startY + 1
                    && current_bullet->positionX >= player->startX - 1
                    && current_bullet->positionX <= player->startX + 1
                    || current_bullet->positionY == player->startY + 2
                    && current_bullet->positionX >= player->startX - 2
                    && current_bullet->positionX <= player->startX + 2
                    || current_bullet->positionY == player->startY + 3
                    && current_bullet->positionX >= player->startX - 8
                    && current_bullet->positionX <= player->startX + 8) {
                    player->destroyed = TRUE ;
                    Delete_bullet(current_bullet);
                    beep();
                }
            }
            else {
                pthread_mutex_lock(&enemysMutex);
                current_enemy = enemy_list ;
                while (current_enemy != NULL) {
                    if(current_enemy->rank == 1 && !current_enemy->destroyed) {
                        if(current_bullet->positionY == current_enemy->positionY
                            && current_bullet->positionX >= current_enemy->positionX - 2
                            && current_bullet->positionX <= current_enemy->positionX + 2) {
                            current_enemy->destroyed = TRUE ;
                            score += 1 ;
                            Delete_bullet(current_bullet);
                        }
                    }
                    current_enemy = current_enemy->next ;
                }
                pthread_mutex_unlock(&enemysMutex);
            }
            current_bullet = current_bullet->next ;
        }
        pthread_mutex_unlock(&bulletsMutex);
        usleep(50000);
    }
    return NULL ;
}

void Delete_bullet(bullet * delete) {
    bullet * current = bullets_list ;
    bullet * prev = current ;
    if(delete == bullets_list) {
        bullets_list = delete->next ;
        free(delete);
    }
    else {
        while (current->next != delete) {
            current = current->next ;
        }
        prev = current ;
        current = current->next ;
        if(current == last_bullet) {
            last_bullet = prev ;
        }
        prev->next = current->next ;
        free(current);

    }
}

void Add_bullet(chtype image , bool is_player , int x , int y) {
    bullet * current = bullets_list;
    bool bullet_direction = FALSE ;
    if(is_player) bullet_direction = TRUE ;
    if(current == NULL) {
        bullets_list = (bullet*)malloc(sizeof(bullet));
        current = bullets_list ;
        last_bullet = bullets_list ;
        current->bullte_image = image;
        current->direction = bullet_direction;
        current->next = NULL ;
        current->positionX = x ;
        current->positionY = y;
    }
    else {
        current = last_bullet ;
        current->next = (bullet*)malloc(sizeof(bullet));
        current = current->next ;
        current->bullte_image = image;
        current->direction = bullet_direction ;
        current->next = NULL ;
        current->positionX = x ;
        current->positionY = y ;
        last_bullet = current ;
    }
}

void free_bullets() {
    bullet * current = bullets_list ;
    bullet * next ;
    while (current != NULL) {
        next = current->next ;
        free(current);
        current = next ;
    }
}

void * Draw_game() {
    while (1) {
        flag[1] = true ;
        while (flag[0]) {
            flag[1] = false;
            while (turn!= 1){};
            flag[1] = true ;
        }
        //Draw player
        //pthread_mutex_lock(&lock);
        clear();
        Print_ship(player->startX , player->startY);
        //pthread_mutex_unlock(&playerMutex);

        //Draw enemys
        //pthread_mutex_lock(&enemysMutex);
        enemy * current_enemy = enemy_list ;
        while (current_enemy != NULL) {
            Print_enemy1(current_enemy);
            current_enemy = current_enemy->next ;
        }
        //pthread_mutex_unlock(&enemysMutex);
        //Draw bullets
        //pthread_mutex_lock(&bulletsMutex);
        bullet * current_bullet = bullets_list ;
        int color ;
        while (current_bullet != NULL) {
            if(current_bullet->direction) {
                color = 1 ;
            }
            else color = 4 ;
            attron(COLOR_PAIR(color)) ;
            mvaddch(current_bullet->positionY , current_bullet->positionX , current_bullet->bullte_image) ;
            attroff(COLOR_PAIR(color)) ;
            current_bullet = current_bullet->next ;
        }
        mvprintw(2 , 2 , "[ Score : %d ]" , score);
        refresh();
        //pthread_mutex_unlock(&bulletsMutex);
        turn = 0 ;
        flag[1] = false ;
    }
    return  NULL ;
}

void Print_enemy1(enemy * e) {
    int x = e->positionX ;
    int y = e->positionY ;
    if(e->destroyed) attron(A_BLINK);
    attron(COLOR_PAIR(4));
    mvaddch(y , x , '^');
    attroff(COLOR_PAIR(4));
    attron(COLOR_PAIR(2));
    mvaddch(y-1 , x , '-');
    mvaddch(y-1 , x-1 ,'-' );
    mvaddch(y-1 , x+1 , '-');
    if(e->shoot) {
        mvaddch(y , x-1 , '\\');
        mvaddch(y , x+1 , '/');
    }
    else {
        mvaddch(y , x-1 , '/');
        mvaddch(y , x+1 , '\\');
    }
    mvaddch(y , x-2 , '|');
    mvaddch(y , x+2 , '|');
    attroff(COLOR_PAIR(2));
    if(e->destroyed) attroff(A_BLINK);
}

void * Enemy_generator() {
    while (1) {
        pthread_mutex_lock(&enemysMutex);
        Create_enemys(5, 1);
        pthread_mutex_unlock(&enemysMutex);
        sleep(10);
    }
    return NULL ;
}

void  Create_enemys(int number_enemys , int rank) {
    int n = number_enemys ;
    enemy_count += number_enemys ;
    while (n != 0 ) {
        Add_enemy(rank);
        n-=1;
    }
}

void Add_enemy(int rank) {
    enemy * current = enemy_list;

    if(current == NULL) {
        enemy_list = (enemy*)malloc(sizeof(enemy));
        current = enemy_list ;
        last_enemy = enemy_list ;
        current->direction = rand() % 3 ;
        current->lives = 10 ;
        current->next = NULL ;
        current->height = 2 ;
        current->width = 5 ;
        current->rank = rank ;
        current->positionY = + 3 ;
        current->positionX = rand() % COLS - 5;
        current->shoot = FALSE ;
        current->destroyed = FALSE ;
    }
    else {
        current = last_enemy ;
        current->next = (enemy*)malloc(sizeof(enemy));
        current = current->next ;
        current->direction = rand() % 3 ;
        current->lives = 10 ;
        current->next = NULL ;
        current->height = 2 ;
        current->width = 5 ;
        current->rank = 1 ;
        current->positionY = + 3 ;
        current->positionX = rand() % COLS - 5;
        current->shoot = FALSE ;
        current->destroyed = FALSE ;
        last_enemy = current ;
    }
}

void * Move_enemys() {
    while (1) {
        pthread_mutex_lock(&enemysMutex);
        enemy * current = enemy_list ;
        while (current != NULL) {
            if(current->rank == 1) {
                if(current->direction == 0) {
                    current->positionY+=1;
                }
                else if(current->direction == 1) {
                    if(current->positionX-=1 < 5) {
                        current->positionY+=1;
                    }
                    else current->positionX-=1;
                }
                else {
                    if(current->positionX+=1 > COLS - 5) {
                        current->positionY+=1;
                    }
                    else current->positionX+=1;
                }
                current->direction = rand() % 3;

                if(current->positionY > LINES || current->destroyed) {
                    Delete_enemy(current);
                    enemy_count -= 1;

                }
                if(rand() % 20 == 1) {
                    Add_bullet('|' , FALSE , current->positionX , current->positionY + 1 );
                    current->shoot = TRUE ;
                }
                else {
                    current->shoot = FALSE;
                }
            }
            current = current->next ;
        }
        pthread_mutex_unlock(&enemysMutex);
        usleep(500000);
    }
    return NULL ;
}

void Delete_enemy(enemy * delete) {
    enemy * current = enemy_list ;
    enemy * prev = current ;
    if(delete == enemy_list) {
        enemy_list = delete->next ;
        free(delete);
    }
    else {
        while (current->next != delete) {
            current = current->next ;
        }
        prev = current ;
        current = current->next ;
        if (current == last_enemy) {
            last_enemy = prev ;
        }
        prev->next = current->next ;
        free(current);
    }
}

void free_enemys() {
    enemy * current = enemy_list ;
    enemy * next ;
    while (current != NULL) {
        next = current->next ;
        free(current);
        current = next ;
    }
}
