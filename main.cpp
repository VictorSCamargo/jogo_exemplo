#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_image.h>
#include "objectsplus.h"
#include "functions.cpp"

//GLOBAL VARIABLES//==================================================================================================================================//

const float WIDTH = 1300; //use between 1600 and 800. I recommend 1300.
const int HEIGHT = 800; //this one is good enough.
const int LIMITE_INF = HEIGHT-40;
const float COEF_MOBILIDADE = 0.1;
float COEF_POSICAO = 0;

const int TANK_MAXLIFE = 3000;
const int LIVES_HELI = 6;

const int TEMPO_MAX = 30*60;
const int TEMPO_MADNESS = 10*60;

const int SPAWNRATE_DROP = 5*60;
const int SPAWNRATE_BOMBS_DELAY = 300;

const int SPEED_EBULLET = 4;

const int BOMB_DROPDELAY = 5*60;
const int DROP_MINLIFE = TANK_MAXLIFE*3/4;

const int NUM_BULLETS = 80;
const int NUM_DROPS = 10;
const int NUM_BOMBS = WIDTH / 130;

const int NUM_HELIS = 500;
const int NUM_EBULLETS = NUM_HELIS;
const int NUM_EXPLOSIONS = 200;
const int CD_METRALHADORA=10;

const int NUM_MISSEIS = 16;
const int CD_MISSEIS = 600;
const int MISSEL_WAIT = 10;

enum KEYS{UP, DOWN, LEFT, RIGHT, Z, X, P, SPACE};
bool keys[8] = {false, false, false, false, false, false, false, false};
enum ESTADO{TITULO, PREJOGO, JOGANDO, PAUSE, DERROTA, MADNESS, VITORIA};

/*variaveis de objetos (global)*/
PlayerTank tank;
Bullet bullets[NUM_BULLETS];
EnemyBullet ebullets[NUM_EBULLETS];
Heli helis[NUM_HELIS];
EBomb bombs[NUM_BOMBS];
Explosion explosions[NUM_EXPLOSIONS];
ExplosionII explosionsII[NUM_EXPLOSIONS];
Missel misseis[NUM_MISSEIS];
Repair drop[NUM_DROPS];
Background background;
Progride progresso;

// FUNCTIONS' DEFINITION //=================================================================================================================================//

//TANK (player's tank):
void InitTank(PlayerTank &tank, ALLEGRO_BITMAP *image, ALLEGRO_BITMAP *image2, ALLEGRO_BITMAP *image3); /* sets tank's values */
void UpdateTank(PlayerTank &tank); /* updates tank's frames */
void DrawTank(PlayerTank &tank); /* draws tank */
void AccelerateLeft(PlayerTank &tank); /* accelerates tank to the left */
void AccelerateRight(PlayerTank &tank); /* accelerates tank to the right */
void MoveTank(PlayerTank &tank); /* moves tank based on acceleration */
void SlowdownTank(PlayerTank &tank); /* slows down tank */

//TANK'S MACHINEGUN (player's weapon):
void InitBullet(Bullet bullet[], int size, PlayerTank &tank, ALLEGRO_BITMAP *image); /* sets bullets' values */
void DrawBullet(Bullet bullet[], int size, PlayerTank &tank); /* draws bullets */
void DrawBulletFire(Bullet bullet[], PlayerTank &tank); /* draws bullet's fire */
void FireBullet(Bullet bullet[], int size, PlayerTank &tank); /* fires bullet and manages cooldown time */
void UpdateBullet(Bullet bullet[], int size); /* updates bullets' position */
void CollideBullet(Bullet bullet[], int bSize, Heli helis[], int cSize, PlayerTank &tank,
                   Explosion explosions[], int eSize); /* checks bullets' collision */

//TANK'S MISSILE LAUNCHER (player's secondary weapon):
void InitMissile(Missel misseis[], int size, PlayerTank &tank, ALLEGRO_BITMAP *image); /* sets missiles' variables */
void DrawMissile(Missel misseis[], int size); /* draws missiles */
void DrawMissileFire(Missel misseis[], int size, PlayerTank &tank); /* draws missile's fire */
void FireMissile(Missel misseis[], int size, PlayerTank &tank); /* fires missiles and manages gusts */
void ReloadMissile(Missel misseis[]); /* manages missiles' cooldown time */
void UpdateMissile(Missel misseis[], int size); /* updates missiles' position */
void CollideMissile(Missel misseis[], int bSize, Heli helis[], int cSize, PlayerTank &tank,
                   Explosion explosions[], int eSize); /* checks missiles' collision */

//HELICOPTERS (enemies):
void InitHeli(Heli helis[], int size, ALLEGRO_BITMAP *image); /* sets helicopters' values */
void DrawHeli(Heli helis[], int size); /* draws helicopters */
void StartHeli(Heli helis[], int size); /* spawns helicopers */
void UpdateHeli(Heli helis[], int size, PlayerTank &tank); /* updates helicopters' position and frames */

//HELICOPTERS' BULLETS (damages the tank):
void InitEnemyBullet(EnemyBullet ebullet[], int size, ALLEGRO_BITMAP *image); /* sets enemy bullets' values */
void DrawEnemyBullet(EnemyBullet ebullet[], int size); /* draws bullets */
void FireEnemyBullet(EnemyBullet ebullet[], int size, Heli helis[], int cSize); /* fires bullet and manages cooldown time */
void UpdateEnemyBullet(EnemyBullet ebullet[], int size); /* updates bullets' position */
void CollideEnemyBullet(EnemyBullet ebullet[], int bSize, Heli helis[], int cSize,
                        PlayerTank &tank, Explosion explosions[], int eSize); /* checks bullets' collision */

//BOMBS (damages the tank):
void InitBomb(EBomb bombs[], int size, ALLEGRO_BITMAP *image, ALLEGRO_BITMAP *anotherimage); /* sets bombs' value */
void DrawBomb(EBomb bombs[], int size); /* draws bombs */
void StartBombEvent(EBomb bombs[], int size); /* starts bomb event */
void StartBomb(EBomb bombs[], int size, PlayerTank &tank); /* starts bomb drop */
void UpdateBomb(EBomb bombs[], int size, PlayerTank &tank, ExplosionII explosionsII[], int eSize); /* updates bombs' position */
void CollideBomb(EBomb bombs[], int bSize, Heli helis[], int cSize,
                 PlayerTank &tank, ExplosionII explosionsII[], int eSize); /* checks bombs' collision */

//REPAIR ITEM (heals the tank):
void InitRepair(Repair drop[], int size, ALLEGRO_BITMAP *image); /* sets repair item values */
void DrawRepair(Repair drop[], int size); /* draws repair item */
void DropRepair(Repair drop[], int size, PlayerTank &tank); /* drops repair item from sky */
void UpdateRepair(Repair drop[], int size); /* updates repair item's position */
void CollideRepair(Repair drop[], int size, PlayerTank &tank); /* checks repair's collision */

//EXPLOSIONS (animations):
void InitExplosions(Explosion explosions[], int size, ALLEGRO_BITMAP *image); /* same functionalities as others */
void DrawExplosions(Explosion explosions[], int size);
void StartExplosions(Explosion explosions[], int size, int x, int y);
void UpdateExplosions(Explosion explosions[], int size);

void InitExplosionsII(ExplosionII explosions[], int size, ALLEGRO_BITMAP *image); /* same functionalities as others */
void DrawExplosionsII(ExplosionII explosions[], int size);
void StartExplosionsII(ExplosionII explosions[], int size, int x, int y);
void UpdateExplosionsII(ExplosionII explosions[], int size);

//BACKGROUND (aesthetic):
void InitBackground(Background &background, ALLEGRO_BITMAP *image); /* background values */
void DrawBackground(Background &background, PlayerTank &tank); /* background moves based on tank's position */

//PROGRESS GERENCIATOR:
void InitProgredir(Progride &progresso, ALLEGRO_FONT *font);
void DrawProgredir(Progride &progresso);
void Progredir(Progride &progresso);
void EndGame(Progride &progresso, int size, Heli helis[]);
void ChangeState(int &state, int newState);


// MAIN //==============================================================================================================================//

int main(void)
{

	/*primitives variables*/
	bool done = false;
	bool redraw = true;
	const int FPS = 60;
	int state = -1;

	/*Allegro variables*/
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_FONT *fontWAR24 = NULL;
	ALLEGRO_FONT *fontWAR48 = NULL;
	ALLEGRO_BITMAP *heliImage;
	ALLEGRO_BITMAP *expImage;
	ALLEGRO_BITMAP *tankImage;
	ALLEGRO_BITMAP *projeteis;
	ALLEGRO_BITMAP *fundo;
	ALLEGRO_BITMAP *eProjeteis;
	ALLEGRO_BITMAP *bombsas;
	ALLEGRO_BITMAP *alerta;
	ALLEGRO_BITMAP *expImageII;
	ALLEGRO_BITMAP *kit;
	ALLEGRO_BITMAP *damageSprite;
	ALLEGRO_BITMAP *healSprite;
	ALLEGRO_BITMAP *titulo = NULL;
	ALLEGRO_BITMAP *derrota = NULL;
	ALLEGRO_BITMAP *vitoria = NULL;
	ALLEGRO_BITMAP *pause = NULL;

	if(!al_init())										//starts Allegro
		return -1;

	display = al_create_display(WIDTH, HEIGHT);			//creates display

	if(!display)										//tests display
		return -1;

	al_init_primitives_addon();
	al_install_keyboard();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();

	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);

	heliImage = al_load_bitmap("MINIHELI.png");  //images...

	expImage = al_load_bitmap("EXPLOSIONS1.png");

	expImageII = al_load_bitmap("EXPLOSIONS2.png");

	tankImage = al_load_bitmap("TANKS.png");

	projeteis = al_load_bitmap("PROJETEIS.png");

	fundo = al_load_bitmap("BACKGROUND.png");

	eProjeteis = al_load_bitmap("PLASMA.png");

	bombsas = al_load_bitmap("BOMBS.png");

	alerta = al_load_bitmap("PERIGO.png");

	damageSprite = al_load_bitmap("DAMAGE.png");

	healSprite = al_load_bitmap("HEAL.png");

	titulo = al_load_bitmap("TITULO.png");

	pause = al_load_bitmap ("PAUSE.png");

	derrota = al_load_bitmap("DERROTA.png");

	vitoria = al_load_bitmap("VITORIA.png");

	kit = al_load_bitmap("CURA.png");        //...images

	fontWAR24 = al_load_font("war.ttf", 24, 0); //font

	fontWAR48 = al_load_font("war.ttf", 56, 0); //font

	srand(time(NULL));

    ChangeState(state, TITULO);      //variável para mudança de estado

	InitTank(tank, tankImage, damageSprite, healSprite);
	InitBullet(bullets, NUM_BULLETS, tank, projeteis);
    InitEnemyBullet(ebullets, NUM_EBULLETS, eProjeteis);
    InitBomb(bombs, NUM_BOMBS, bombsas, alerta);
    InitMissile(misseis, NUM_MISSEIS, tank, projeteis);
	InitHeli(helis, NUM_HELIS, heliImage);
	InitExplosions(explosions, NUM_EXPLOSIONS, expImage);
	InitExplosionsII(explosionsII, NUM_BOMBS, expImageII);
	InitRepair(drop, NUM_DROPS, kit);
    InitBackground(background, fundo);
    InitProgredir(progresso, fontWAR24);

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));

// EVENTS // ===================================================================================================================

	al_start_timer(timer);
	while(!done)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_TIMER)
		{

			redraw = true;

            if(state == TITULO)
            {}
            else if(state == JOGANDO || state == MADNESS)
            {

                // BASIC KEYS //

                if(keys[LEFT] && keys[RIGHT]){
                    SlowdownTank(tank);
                }else if(keys[LEFT]){
                    AccelerateLeft(tank);
                }else if(keys[RIGHT]){
                    AccelerateRight(tank);
                }else{
                    SlowdownTank(tank);}
                if(keys[Z]){
                    FireBullet(bullets, NUM_BULLETS, tank);
                }
                if((keys[X])||(misseis->firing==true)){
                    if(misseis->ready==true){
                        FireMissile(misseis, NUM_MISSEIS, tank);
                    }
                }

                // EVENTS' UPDATE //

                if(bombs->event==true){
                    StartBomb(bombs, NUM_BOMBS, tank);
                }

                UpdateTank(tank);
                MoveTank(tank);

				UpdateHeli(helis, NUM_HELIS, tank);

				UpdateBomb(bombs, NUM_BOMBS, tank, explosionsII, NUM_EXPLOSIONS);
				CollideBomb(bombs, NUM_BOMBS, helis, NUM_HELIS, tank, explosionsII, NUM_EXPLOSIONS);

				UpdateBullet(bullets, NUM_BULLETS);
				CollideBullet(bullets, NUM_BULLETS, helis, NUM_HELIS, tank, explosions, NUM_EXPLOSIONS);

                UpdateMissile(misseis, NUM_MISSEIS);
                ReloadMissile(misseis);
                CollideMissile(misseis, NUM_MISSEIS, helis, NUM_HELIS, tank, explosions, NUM_EXPLOSIONS);

				FireEnemyBullet(ebullets, NUM_HELIS, helis, NUM_HELIS);
				UpdateEnemyBullet(ebullets, NUM_EBULLETS);
				CollideEnemyBullet(ebullets, NUM_EBULLETS, helis, NUM_HELIS, tank, explosions, NUM_EXPLOSIONS);

                UpdateExplosions(explosions, NUM_EXPLOSIONS);
                UpdateExplosionsII(explosionsII, NUM_BOMBS);

                DropRepair(drop, NUM_DROPS, tank);
                UpdateRepair(drop, NUM_DROPS);
                CollideRepair(drop, NUM_DROPS, tank);

                // DIFFICULTY PROGRESS //

                if(progresso.difatual != progresso.dificuldade){
                    switch(progresso.dificuldade){
                    case 0:

                        drop->alteraQuantidade=1;

                        helis->alteraQuantidade=5;

                        break;
                    case 1:
                        tank.armor+=5;

                        bullets->CD=8;
                        misseis->CDmax=400;

                        drop->alteraQuantidade=2;
                        drop->waitMAX=7*60;

                        bombs->eventSpawnDelay=300;

                        helis->alteraQuantidade=10;

                        break;
                    case 2:
                        tank.armor+=10;

                        bullets->CD=5;
                        misseis->CDmax=200;

                        drop->alteraQuantidade=3;
                        drop->waitMAX=6*60;

                        bombs->eventSpawnDelay=240;

                        helis->alteraQuantidade=15;

                        break;
                    case 3:
                        tank.armor+=10;

                        bullets->CD=5;
                        misseis->CDmax=100;

                        drop->alteraQuantidade=4;
                        drop->waitMAX=5*60;

                        bombs->eventSpawnDelay=180;

                        helis->alteraQuantidade=20;

                        break;
                    default:
                        break;
                    }
                    progresso.difatual=progresso.dificuldade;
                }

                if(tank.lives <= 0)
					ChangeState(state, DERROTA);

                if(state != MADNESS){

                    Progredir(progresso);

                    StartHeli(helis, NUM_HELIS);
                    StartBombEvent(bombs, NUM_BOMBS);

                } else if(++progresso.contMADNESS<progresso.maxMADNESS){

                    /* makes enemies spawn for a limited time */

                    bullets->CD=1;
                    misseis->CDmax=0;
                    misseis->waitMAX=5;
                    misseis->alteraQuantidade=16;

                    drop->alteraQuantidade=NUM_DROPS;
                    drop->waitMAX=30;
                    drop->minlife=800;

                    bombs->eventSpawnDelay=20;

                    helis->alteraQuantidade=NUM_HELIS;

                    for(int i = 0; i < helis->alteraQuantidade; i++){
                        helis[i].firingCD = 120;
                    }

                    StartHeli(helis, NUM_HELIS);
                }else{
                    progresso.contMADNESS=progresso.maxMADNESS;

                    EndGame(progresso, NUM_HELIS, helis);
                    if(progresso.endgame==true){
                        ChangeState(state, VITORIA);
                    }
                }
            }
            else if(state == PAUSE)
            {}
            else if(state == DERROTA)
            {}
            else if(state == VITORIA)
            {}
		}
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			done = true;
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch(ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = true;
				break;
            case ALLEGRO_KEY_SPACE:
                if(state == TITULO)
					ChangeState(state, PREJOGO);
				else if(state == DERROTA || state == VITORIA)
					ChangeState(state, TITULO);
				break;
            case ALLEGRO_KEY_X:
				keys[X] = true;
                if(progresso.fim==true)
                    ChangeState(state, MADNESS);
				break;
			case ALLEGRO_KEY_Z:
				keys[Z] = true;
				break;
            case ALLEGRO_KEY_P:
                if(state == JOGANDO)
                    ChangeState(state, PAUSE);
                else if(state == PAUSE)
                    ChangeState(state, JOGANDO);
			}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch(ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = false;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = false;
				break;
			case ALLEGRO_KEY_Z:
				keys[Z] = false;
				break;
            case ALLEGRO_KEY_X:
				keys[X] = false;
				break;
			}
		}

		if(redraw && al_is_event_queue_empty(event_queue))
		{
			redraw = false;

            if(state == TITULO)
            {
                al_draw_bitmap(titulo, 0 -(1600-WIDTH)/2 ,0,0);
            }
            else if(state == JOGANDO || state == PAUSE || state == MADNESS)
            {
                DrawBackground(background, tank);
                DrawTank(tank);
                DrawExplosions(explosions, NUM_EXPLOSIONS);
				DrawExplosionsII(explosionsII, NUM_EXPLOSIONS);
                DrawRepair(drop, NUM_DROPS);
				DrawBullet(bullets, NUM_BULLETS, tank);
                DrawBulletFire(bullets, tank);
				DrawEnemyBullet(ebullets, NUM_HELIS);
				DrawHeli(helis, NUM_HELIS);
				DrawBomb(bombs, NUM_BOMBS);
				DrawMissile(misseis, NUM_MISSEIS);
				DrawMissileFire(misseis, NUM_MISSEIS, tank);

				DrawProgredir(progresso);

				if(state == PAUSE){
                    al_draw_scaled_bitmap(pause, 0, 0, 1600, 800, 0, 0, WIDTH, HEIGHT, 0);
				}

                if(state == MADNESS){
                    al_draw_textf(fontWAR48, al_map_rgb(250, 50, 0), WIDTH/2, HEIGHT/2, ALLEGRO_ALIGN_CENTRE, "MADNESS");
                }

                //TEXTOS//

                al_draw_textf(fontWAR24, al_map_rgb(0, 0, 0), 10 + 2, HEIGHT - 25 + 2, 0, "VIDA:");
				al_draw_textf(fontWAR24, al_map_rgb((250 - (tank.lives/TANK_MAXLIFE)*250),(tank.lives/TANK_MAXLIFE)*250, 50), 10, HEIGHT - 25, 0, "VIDA:");

				al_draw_textf(fontWAR24, al_map_rgb(0, 0, 0), 12, 6, 0, "%is", progresso.tempo/60);

				al_draw_textf(fontWAR24, al_map_rgb(0, 0, 0), WIDTH - 53 + 2, HEIGHT - 25 + 2, 0, "K: %i ", tank.score);
				al_draw_textf(fontWAR24, al_map_rgb(220, 40, 40), WIDTH - 53, HEIGHT - 25, 0, "K: %i ", tank.score);
            }
            else if(state == DERROTA)
            {
                al_draw_bitmap(derrota, 0 -(1600-WIDTH)/2 ,0,0);
                al_draw_textf(fontWAR24, al_map_rgb(0, 0, 0), WIDTH / 2, (HEIGHT/14)*11 +20 , ALLEGRO_ALIGN_CENTRE, "REMAINING TIME: %i segundos", (progresso.tempo/60));
                al_draw_textf(fontWAR24, al_map_rgb(0, 0, 0), WIDTH / 2, (HEIGHT/7)*6 +20, ALLEGRO_ALIGN_CENTRE, "KILLS: %i", tank.score);
            }
            else if(state == VITORIA)
            {
                al_draw_bitmap(vitoria, 0 -(1600-WIDTH)/2 ,0,0);
                al_draw_textf(fontWAR24, al_map_rgb(0, 0, 0), WIDTH / 2, (HEIGHT / 6)*5, ALLEGRO_ALIGN_CENTRE, "KILLS: %i", tank.score);
            }

			al_flip_display();
			al_clear_to_color(al_map_rgb(0,0,0));
		}
	}


	//destroi objetos do display

    al_destroy_bitmap(tankImage);
    al_destroy_bitmap(eProjeteis);
    al_destroy_bitmap(bombsas);
    al_destroy_bitmap(alerta);
    al_destroy_bitmap(kit);
    al_destroy_bitmap(fundo);
    al_destroy_bitmap(projeteis);
	al_destroy_bitmap(expImage);
	al_destroy_bitmap(expImageII);
	al_destroy_bitmap(heliImage);
	al_destroy_bitmap(titulo);
	al_destroy_bitmap(pause);
    al_destroy_bitmap(derrota);
    al_destroy_bitmap(vitoria);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_font(fontWAR24);
	al_destroy_font(fontWAR48);
	al_destroy_display(display);

	return 0;
}

//FUNÇÕES=====================================================================================================================================================//

//PROGRESSÃO==================================================================================================================================================//

void InitProgredir(Progride &progresso, ALLEGRO_FONT *font = NULL)
{
    progresso.tempo=TEMPO_MAX;
    progresso.dificuldade=0;
    progresso.difatual=-1;
    progresso.mostrartexto=0;
    progresso.conttexto=0;
    progresso.contMAX=240;
    progresso.Dtempo=progresso.tempo;
    progresso.minutos=0;
    progresso.segundos=0;
    progresso.fim=false;
    progresso.vivo=true;

    progresso.endgame=false;

    progresso.contMADNESS=0;
    progresso.maxMADNESS=TEMPO_MADNESS;

    if(font != NULL)
        progresso.font = font;
}
void DrawProgredir(Progride &progresso)
{
    al_draw_filled_rectangle(40, 10, WIDTH - 40, 20, al_map_rgb(0, 0, 0));
    al_draw_filled_rectangle(37+(WIDTH/4), 5, 40 + (WIDTH/4) +3, 25, al_map_rgb(0, 0, 0));
    al_draw_filled_rectangle((WIDTH/2) -3, 5, (WIDTH/2) +3, 25, al_map_rgb(0, 0, 0));
    al_draw_filled_rectangle(37+((WIDTH/4)*3), 5, 40 + ((WIDTH/4)*3) + 3, 25, al_map_rgb(0, 0, 0));
    al_draw_filled_circle(WIDTH-20, 15, 12, al_map_rgb(0, 0, 0));

    if(progresso.mostrartexto==1){
            switch(progresso.dificuldade){
            case 1:
                al_draw_textf(progresso.font, al_map_rgb(250, 50, 50), WIDTH/2, HEIGHT/2 - 30, ALLEGRO_ALIGN_CENTRE, "+ INIMIGOS");
                al_draw_textf(progresso.font, al_map_rgb(250, 50, 50), WIDTH/2, HEIGHT/2, ALLEGRO_ALIGN_CENTRE, "+ BOMBAS");
                al_draw_textf(progresso.font, al_map_rgb(250, 250, 250), WIDTH/2, HEIGHT/2 + 30, ALLEGRO_ALIGN_CENTRE, "+ PODER DE FOGO");
                al_draw_textf(progresso.font, al_map_rgb(250, 250, 250), WIDTH/2, HEIGHT/2 + 60, ALLEGRO_ALIGN_CENTRE, "+ BLINDAGEM");
                break;
            case 2:
                al_draw_textf(progresso.font, al_map_rgb(250, 50, 50), WIDTH/2, HEIGHT/2 - 30, ALLEGRO_ALIGN_CENTRE, "++ INIMIGOS");
                al_draw_textf(progresso.font, al_map_rgb(250, 50, 50), WIDTH/2, HEIGHT/2, ALLEGRO_ALIGN_CENTRE, "++ BOMBAS");
                al_draw_textf(progresso.font, al_map_rgb(250, 250, 250), WIDTH/2, HEIGHT/2 + 30, ALLEGRO_ALIGN_CENTRE, "++ PODER DE FOGO");
                al_draw_textf(progresso.font, al_map_rgb(250, 250, 250), WIDTH/2, HEIGHT/2 + 60, ALLEGRO_ALIGN_CENTRE, "++ BLINDAGEM");
                break;
            case 3:
                al_draw_textf(progresso.font, al_map_rgb(250, 50, 50), WIDTH/2, HEIGHT/2 - 30, ALLEGRO_ALIGN_CENTRE, "+++ INIMIGOS");
                al_draw_textf(progresso.font, al_map_rgb(250, 50, 50), WIDTH/2, HEIGHT/2, ALLEGRO_ALIGN_CENTRE, "+++ BOMBAS");
                al_draw_textf(progresso.font, al_map_rgb(250, 250, 250), WIDTH/2, HEIGHT/2 + 30, ALLEGRO_ALIGN_CENTRE, "+++ PODER DE FOGO");
                al_draw_textf(progresso.font, al_map_rgb(250, 250, 250), WIDTH/2, HEIGHT/2 + 60, ALLEGRO_ALIGN_CENTRE, "+++ BLINDAGEM");
                break;
            case 4:
                al_draw_textf(progresso.font, al_map_rgb(250, 250, 250), WIDTH/2, HEIGHT/2 - 30, ALLEGRO_ALIGN_CENTRE, "MISSEIS CARREGADOS");
                al_draw_textf(progresso.font, al_map_rgb(250, 250, 250), WIDTH/2, HEIGHT/2, ALLEGRO_ALIGN_CENTRE, "PRESSIONE X PARA");
                al_draw_textf(progresso.font, al_map_rgb(250, 50, 50), WIDTH/2, HEIGHT/2 + 30, ALLEGRO_ALIGN_CENTRE, "MODO MADNESS");
                break;
            }
            if(++progresso.conttexto>progresso.contMAX){
                progresso.mostrartexto=0;
                progresso.conttexto=0;
            }
    }

    if(!progresso.fim){
        al_draw_filled_rectangle(42, 12, (WIDTH - 42) - ((progresso.Dtempo/TEMPO_MAX) * (WIDTH - 84)), 18, al_map_rgb(250, 250, 250));
    }else{
        al_draw_filled_rectangle(42, 12, (WIDTH - 42) - ((progresso.Dtempo/TEMPO_MAX) * (WIDTH - 84)), 18, al_map_rgb(250, 20, 20));
        al_draw_filled_circle(WIDTH-20, 15, 9, al_map_rgb(250, 20, 20));
    }
}
void Progredir(Progride &progresso)
{
    if(progresso.tempo<=0){
        progresso.fim=true;
    } else {
    progresso.tempo--;
    progresso.Dtempo=progresso.tempo;
    progresso.segundos=progresso.tempo/60;
    progresso.minutos=progresso.tempo/3600;
    }

    if(progresso.tempo==0){
        progresso.dificuldade=4;
        progresso.mostrartexto=1;
    }else if(progresso.tempo==TEMPO_MAX/4){
        progresso.dificuldade=3;
        progresso.mostrartexto=1;
    }else if(progresso.tempo==TEMPO_MAX/2){
        progresso.dificuldade=2;
        progresso.mostrartexto=1;
    }else if(progresso.tempo==(TEMPO_MAX/4)*3){
        progresso.dificuldade=1;
        progresso.mostrartexto=1;
    }
}
void EndGame(Progride &progresso, int size, Heli helis[])
{
    progresso.vivo=false;

    for(int i = 0; i < size; i++)
	{
		if(helis[i].live)
		{
		    progresso.vivo=true;
		}
    }
    if(progresso.vivo==false){
        progresso.endgame=true;
    }
}


//BACKGROUND===================================================================================================================================================//

void InitBackground(Background &background, ALLEGRO_BITMAP *image = NULL)
{
    if(image != NULL)
        background.image = image;
}
void DrawBackground(Background &background, PlayerTank &tank)
{
    COEF_POSICAO=0-(tank.x*COEF_MOBILIDADE);
    al_draw_scaled_bitmap(background.image, 0, 0, 1506, 525, COEF_POSICAO, 0, 2295, 800, 0);
}


//PLAYER=======================================================================================================================================================//

void InitTank(PlayerTank &tank, ALLEGRO_BITMAP *image = NULL, ALLEGRO_BITMAP *image2 = NULL, ALLEGRO_BITMAP *image3 = NULL)
{
	tank.x = WIDTH/2;
	tank.y = LIMITE_INF-40;
	tank.ID = PLAYER;
	tank.lives = TANK_MAXLIFE;
	tank.armor = 0;
	tank.speed = 0;
	tank.acel = 0.2;
	tank.maxspeed = 3.5;
	tank.boundx = 97;
	tank.boundy = 12;
	tank.score = 0;
	tank.livesCheck = tank.lives;
	tank.frameCount2 = 0;
	tank.imageSelect = 0;

	tank.maxFrame = 4;
	tank.curFrame = 0;
	tank.frameCount = 0;
	tank.frameDelay = 3;
	tank.frameWidth = 226;
	tank.frameHeight = 140;
	tank.animationColumns = 4;
	tank.animationDirection = 1;

	tank.animationRow = 1;

    if(image != NULL)
        tank.image = image;
    if(image != NULL)
        tank.image2 = image2;
    if(image != NULL)
        tank.image3 = image3;
}
void UpdateTank(PlayerTank &tank)
{
    if(++tank.frameCount >= tank.frameDelay)
			{
				tank.curFrame += tank.animationDirection;
				if(tank.curFrame >= tank.maxFrame)
					tank.curFrame = 0;
				else if( tank.curFrame <= 0)
					tank.curFrame = tank.maxFrame - 1;

				tank.frameCount = 0;
			}

	if (tank.livesCheck != tank.lives){
        if (++tank.frameCount2 < 20){
            if (tank.lives < tank.livesCheck){
                tank.imageSelect = 1;
            }else{
                tank.imageSelect = 2;
            }
        }else{

        tank.livesCheck = tank.lives;
        tank.frameCount2 = 0;
        tank.imageSelect = 0;

        }
    }

}
void DrawTank(PlayerTank &tank)
{
    //Tanque//
    int fx = (tank.curFrame % tank.animationColumns) * tank.frameWidth;

    if(tank.imageSelect != 0){
        if (tank.imageSelect == 1){
            al_draw_bitmap(tank.image2, tank.x-(tank.frameWidth/2), tank.y-(tank.frameHeight/2)-25, 0);
        }else{
            al_draw_bitmap(tank.image3, tank.x-(tank.frameWidth/2), tank.y-(tank.frameHeight/2)-25, 0);
        }
    }

    al_draw_bitmap_region(tank.image, fx, 0, tank.frameWidth, tank.frameHeight, tank.x-(tank.frameWidth/2), tank.y-(tank.frameHeight/2)-25, 0);



    //Vida//
    al_draw_filled_rectangle(60, LIMITE_INF + 20, WIDTH - 60, HEIGHT - 10, al_map_rgb(0, 0, 0));
    al_draw_filled_rectangle(64, LIMITE_INF + 24, (WIDTH - 64)*((tank.lives)/TANK_MAXLIFE), HEIGHT - 14, al_map_rgb((250 - (tank.lives/TANK_MAXLIFE)*250),(tank.lives/TANK_MAXLIFE)*250, 50));

}
void AccelerateLeft(PlayerTank &tank)
{
    if (tank.speed <= -tank.maxspeed){
        tank.speed = -tank.maxspeed;
    }
    else
    {
        tank.speed -= tank.acel;
    }
}
void AccelerateRight(PlayerTank &tank)
{
    if (tank.speed >= tank.maxspeed){
        tank.speed = tank.maxspeed;
    }
    else
    {
        tank.speed += tank.acel;
    }
}
void SlowdownTank(PlayerTank &tank)
{
    if(tank.speed > 0.1){
        tank.speed -= tank.acel;
    }else if(tank.speed < -0.1){
        tank.speed += tank.acel;
    }else{
        tank.speed = 0;
    }
}
void MoveTank(PlayerTank &tank)
{
    tank.x += tank.speed;

    if(tank.x > WIDTH-tank.boundx){
		tank.x = WIDTH-tank.boundx;}

    if(tank.x < tank.boundx){
		tank.x = tank.boundx;}

}


//BULLET ALIADO=====================================================================================================================================================//

void InitBullet(Bullet bullet[], int size, PlayerTank &tank, ALLEGRO_BITMAP *image = NULL)
{
    bullet->alterna=0;
    bullet->cont=0;
    bullet->CD=CD_METRALHADORA;

    if(image != NULL)
        bullet->image = image;

	for(int i = 0; i < size; i++)
	{
		bullet[i].ID = BULLET;
		bullet[i].speed = 30;
		bullet[i].live = false;
	}

}
void DrawBullet(Bullet bullet[], int size, PlayerTank &tank)
{
	for( int i = 0; i < size; i++)
	{
		if(bullet[i].live)
			al_draw_bitmap_region(bullet->image, 17, 0, 15, 85, bullet[i].x-15/2, bullet[i].y-85/2, 0);
	}
}
void DrawBulletFire(Bullet bullet[], PlayerTank &tank)
{
    if(bullet->shot==true){
        if(bullet->alterna==1){
                al_draw_bitmap_region(bullet->image, 32, 0, 40, 85, (tank.x+8)-40/2, (tank.y-76)-75, 0);
            }else{
                al_draw_bitmap_region(bullet->image, 32, 0, 40, 85, (tank.x+32)-40/2, (tank.y-76)-75, 0);
        }
        bullet->shot=false;
    }
}
void FireBullet(Bullet bullet[], int size, PlayerTank &tank)
{
	for( int i = 0; i < size; i++)
	{
		if(!bullet[i].live)
		{
		   if((++bullet->cont) > (bullet->CD)){
		    if(bullet->alterna==0){
                bullet[i].x = tank.x+6;
                bullet[i].y = tank.y-76;
                bullet[i].live = true;
                bullet->alterna=1;
			}else{
                bullet[i].x = tank.x+30;
                bullet[i].y = tank.y-76;
                bullet[i].live = true;
                bullet->alterna=0;
			}
            bullet->cont=0;
            bullet->shot = true;
		    }
            break;
		}
	}
}
void UpdateBullet(Bullet bullet[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(bullet[i].live)
		{
			bullet[i].y -= bullet[i].speed;
			if(bullet[i].y < 0)
				bullet[i].live = false;
		}
	}
}
void CollideBullet(Bullet bullet[], int bSize, Heli helis[], int cSize, PlayerTank &tank, Explosion explosions[], int eSize)
{
	for(int i = 0; i < bSize; i++)
	{
		if(bullet[i].live)
		{
			for(int j =0; j < cSize; j++)
			{
				if(helis[j].live)
				{
					if(bullet[i].x > (helis[j].x - helis[j].boundx) &&
						bullet[i].x < (helis[j].x + helis[j].boundx) &&
						bullet[i].y > (helis[j].y - helis[j].boundy) &&
						bullet[i].y < (helis[j].y + helis[j].boundy))
					{
						bullet[i].live = false;
						helis[j].lives-=1;
						if(helis[j].lives<=0){
                            helis[j].live = false;
                            tank.score++;
                            StartExplosions(explosions, eSize, bullet[i].x, bullet[i].y);
						}
					}
				}
			}
		}
	}
}


//MISSEIS=====================================================================================================================================================//

void InitMissile(Missel misseis[], int size, PlayerTank &tank, ALLEGRO_BITMAP *image = NULL)
{
   misseis->alterna=false;
   misseis->alteraQuantidade=4;
   misseis->CD=0;
   misseis->CDmax=CD_MISSEIS;
   misseis->ready=false;
   misseis->firing=false;
   misseis->shot=false;
   misseis->wait=0;
   misseis->waitMAX=MISSEL_WAIT;
   misseis->cont=0;
   misseis->cano=0;
   misseis->boundx=50;
   misseis->boundy=80;


   if(image != NULL)
        misseis->image=image;

	for(int i = 0; i < size; i++)
	{
		misseis[i].ID = MISSEIS;
		misseis[i].speed = 5;
		misseis[i].aceler = 0.3;
		misseis[i].live = false;
	}
}
void DrawMissile(Missel misseis[], int size)
{
    if(misseis->CD>misseis->CDmax){
        misseis->CD = misseis->CDmax;
    }

    al_draw_filled_rectangle(WIDTH - 40, HEIGHT - 240, WIDTH - 20, HEIGHT - 40, al_map_rgb(0, 0, 0));
    al_draw_filled_rectangle(WIDTH - 40 + 6, (HEIGHT - 46) - ((misseis->CD/misseis->CDmax)*186), WIDTH - 20 - 6, HEIGHT - 40 - 6, al_map_rgb(252, 151, 18));


    if(misseis->ready){
       al_draw_bitmap_region(misseis->image, 0, 0, 17, 85, WIDTH - 39,  HEIGHT - 322, 0);
    }

	for( int i = 0; i < size; i++)
	{
		if(misseis[i].live)
			al_draw_bitmap_region(misseis->image, 0, 0, 17, 85, misseis[i].x-17/2, misseis[i].y-85/2, 0);
	}
}
void DrawMissileFire(Missel misseis[], int size, PlayerTank &tank)
{
    if(misseis->shot){
            if(misseis->alterna==false){
                al_draw_bitmap_region(misseis->image, 72, 0, 31, 85, (tank.x+2)-40/2, (tank.y-76) - 68, 0);
            }else{
                al_draw_bitmap_region(misseis->image, 72, 0, 31, 85, (tank.x+30)-40/2, (tank.y-76) - 68, 0);
        }
        misseis->shot=false;
    }
}
void ReloadMissile(Missel misseis[])
{
    if(misseis->ready!=true){

        if(++misseis->CD>=misseis->CDmax){
            misseis->ready=true;
        }
    }
}
void FireMissile(Missel misseis[], int size, PlayerTank &tank)
{
    misseis->firing=true;

        if(misseis->cont < misseis->alteraQuantidade){

            if(++misseis->wait>misseis->waitMAX){

                for( int i = 0; i < misseis->alteraQuantidade; i++){

                    if(!misseis[i].live){

                        if(misseis->cano<4){

                            if(misseis->alterna==false){

                                switch(misseis->cano){
                                case 0:
                                    misseis[i].x = tank.x+2;
                                    misseis[i].y = tank.y-77;
                                    break;
                                case 1:
                                    misseis[i].x = tank.x-5;
                                    misseis[i].y = tank.y-77;
                                    break;
                                case 2:
                                    misseis[i].x = tank.x-1;
                                    misseis[i].y = tank.y-77;
                                    break;
                                case 3:
                                    misseis[i].x = tank.x-8;
                                    misseis[i].y = tank.y-77;
                                    break;
                                    }

                            }else{

                                switch(misseis->cano){
                                case 0:
                                    misseis[i].x = tank.x+2+25;
                                    misseis[i].y = tank.y-76;
                                    break;
                                case 1:
                                    misseis[i].x = tank.x-5+25;
                                    misseis[i].y = tank.y-76;
                                    break;
                                case 2:
                                    misseis[i].x = tank.x-1+25;
                                    misseis[i].y = tank.y-76;
                                    break;
                                case 3:
                                    misseis[i].x = tank.x-8+25;
                                    misseis[i].y = tank.y-76;
                                    break;
                                }
                            }

                                misseis[i].live = true;
                                misseis->cano++;
                                misseis->wait=0;
                                misseis->cont++;

                        } else {

                            misseis->alterna = !misseis->alterna;
                            misseis->cano=0;

                            }

                        break;
                    }
                }

                misseis->wait=0;
                misseis->shot=true;
                misseis->CD=0;
            }

        }else{

            misseis->ready=false;
            misseis->firing=false;
            misseis->cont=0;
    }
}
void UpdateMissile(Missel misseis[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(misseis[i].live)
		{
			misseis[i].y -= misseis[i].speed;
			misseis[i].speed += misseis[i].aceler;

			if(misseis[i].y < 0){
				misseis[i].live = false;
				misseis[i].speed = 5;
			}
	}
}
}
void CollideMissile(Missel misseis[], int bSize, Heli helis[], int cSize, PlayerTank &tank, Explosion explosions[], int eSize)
{
	for(int i = 0; i < bSize; i++)
	{
		if(misseis[i].live)
		{
			for(int j = 0; j < cSize; j++)
			{
				if(helis[j].live)
				{
					if(misseis[i].x > (helis[j].x - helis[j].boundx) &&
						misseis[i].x < (helis[j].x + helis[j].boundx) &&
						misseis[i].y > (helis[j].y - helis[j].boundy) &&
						misseis[i].y < (helis[j].y + helis[j].boundy))
					{
						misseis[i].live = false;
						misseis[i].speed = 5;

						for(int k = 0; k < cSize; k++)
                        {
                            if(helis[j].x + 40 > (helis[k].x - helis[k].boundx) &&
                                helis[j].x - 40 < (helis[k].x + helis[k].boundx) &&
                                helis[j].y + 40 > (helis[k].y - helis[k].boundy) &&
                                helis[j].y -40 < (helis[k].y + helis[k].boundy))
                                {
                                    helis[k].live = false;
                                    tank.score++;
                                    StartExplosions(explosions, eSize, helis[k].x, helis[k].y);
                                }
                        }
                        tank.score++;
                        helis[i].live = false;
                        StartExplosions(explosions, eSize, misseis[i].x, misseis[i].y);
                    }
                }
            }
        }
    }
}


//HELICOPTEROS INIMIGOS//==================================================================================================================================================//

void InitHeli(Heli helis[], int size, ALLEGRO_BITMAP *image = NULL)
{

	for(int i = 0; i < size; i++)
	{
		helis[i].ID = ENEMY;
		helis[i].lives = LIVES_HELI;
		helis[i].live = false;
		helis[i].speed = (6 + (rand()%6)) * (WIDTH/1400);
		helis[i].boundx = 50;
		helis[i].boundy = 35;
		helis[i].firing = false;
		helis[i].firingCD = 3*60 + rand() % (60*3);
        helis[i].firingCDCont = 0;
        helis[i].shotCont = 0;

		helis[i].maxFrame = 3;
		helis[i].curFrame = 0;
		helis[i].frameCount = 0;
        helis[i].frameCountFire = 0;
		helis[i].frameDelay = 2;
		helis[i].frameWidth = 100;
		helis[i].frameHeight = 139;
		helis[i].animationColumns = 3;

		helis[i].mode = 1;

        helis[i].animationDirection = 1;

        if(image != NULL)
            helis[i].image = image;
	}
}
void DrawHeli(Heli helis[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(helis[i].live)
		{
			int fx = (helis[i].curFrame % helis[i].animationColumns) * helis[i].frameWidth;
            int fy = (helis[i].mode) * helis[i].frameHeight;

            al_draw_bitmap_region(helis[i].image, fx, fy, helis[i].frameWidth, helis[i].frameHeight, helis[i].x - helis[i].frameWidth / 2, helis[i].y - helis[i].frameHeight / 2, helis[i].side);
		}
	}
}
void StartHeli(Heli helis[], int size)
{
	for(int i = 0; i < helis->alteraQuantidade; i++)
	{
		if(!helis[i].live)
		{
			if(rand() % 50 == 0)
			{
			    helis[i].lives = LIVES_HELI;
				helis[i].live = true;
				helis[i].firing = false;
				helis[i].shotCont = 0;

				if(rand() % 2){
                    helis[i].x = 30;
                    helis[i].side = 1;
                    helis[i].animationDirection = 1;
				} else {
				    helis[i].x = WIDTH - 30;
				    helis[i].side = 0;
				    helis[i].animationDirection = -1;
				    }
                helis[i].mode=1;
				helis[i].y = 30 + rand() % (210);
				break;
			}
		}else{
		    if(!helis[i].firing){
                if(++helis[i].firingCDCont > helis[i].firingCD){
                    helis[i].firing = true;
                    helis[i].firingCDCont = 0;
                }
            }
		}
	}
}
void UpdateHeli(Heli helis[], int size, PlayerTank &tank)
{
	for(int i = 0; i < size; i++)
	{
		if(helis[i].live)
		{
			if(++helis[i].frameCount >= helis[i].frameDelay)
			{
				helis[i].curFrame += helis[i].animationDirection;
				if(helis[i].curFrame >= helis[i].maxFrame){
					helis[i].curFrame = 0;
                }
				else if( helis[i].curFrame <= 0){
					helis[i].curFrame = helis[i].maxFrame;
                }
				helis[i].frameCount = 0;

                    if(helis[i].mode==2){
                        helis[i].frameCountFire++;
                        if(helis[i].frameCountFire==3)
                            {
                                helis[i].frameCountFire=0;
                                helis[i].mode=1;
                            }
                    }

            //heli alterna direção

            if(helis[i].side==0){
                helis[i].x -= helis[i].speed;
            }else{
                helis[i].x += helis[i].speed;
            }

            if(helis[i].side==0){
                    if(helis[i].x <= 0){
                        helis[i].side = 1;}
            }
            if(helis[i].side==1){
                if(helis[i].x >= WIDTH-10){
                    helis[i].side=0;
                }
            }
        }
    }
}
}


//BULLETS INIMIGOS==================================================================================================================================================//

void InitEnemyBullet(EnemyBullet ebullet[], int size, ALLEGRO_BITMAP *image = NULL)
{
    ebullet->shotDelay = 5;

	for(int i = 0; i < size; i++)
	{
		ebullet[i].ID = EBULLET;
		ebullet[i].live = false;
        ebullet[i].speed = SPEED_EBULLET;
        ebullet[i].shotDelayCount = 0;
        ebullet[i].shooting = false;

		ebullet[i].maxFrame = 2;
        ebullet[i].curFrame = 0;
        ebullet[i].frameCount = 0;
        ebullet[i].frameDelay = 10;
        ebullet[i].frameWidth = 10;
        ebullet[i].frameHeight = 10;
        ebullet[i].animationColumns = 1;

        if(image != NULL)
            ebullet[i].image = image;
	}
}
void DrawEnemyBullet(EnemyBullet ebullet[], int size)
{
	for( int i = 0; i < size; i++)
	{
	    int fy = (ebullet[i].curFrame / ebullet[i].animationColumns) * ebullet->frameWidth;

		if(ebullet[i].live){
            al_draw_bitmap_region(ebullet[i].image, 0, fy, 12, 12, ebullet[i].x-12/2, ebullet[i].y-12/2, 0);
		}
	}
}
void FireEnemyBullet(EnemyBullet ebullet[], int size, Heli helis[], int cSize)
{
	for( int i = 0; i < helis->alteraQuantidade; i++)
	{
	    if(helis[i].live){
            if(helis[i].firing){

                if(++ebullet[i].shotDelayCount > ebullet->shotDelay){
                        if(++helis[i].shotCont <= 4){

                            for(int j = 0; j < NUM_EBULLETS; j++)
                            {
                                if(!ebullet[j].live)
                                {
                                    ebullet[j].x = helis[i].x;
                                    ebullet[j].y = helis[i].y;
                                    ebullet[j].live = true;
                                    ebullet[i].shotDelayCount = 0;
                                    helis[i].mode = 2;
                                    break;
                                }
                        }
                    }else{
                        helis[i].firing = false;
                        helis[i].shotCont = 0;
                    }
                }
            }
        }
    }
}
void UpdateEnemyBullet(EnemyBullet ebullet[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(ebullet[i].live)
		{
			ebullet[i].y += ebullet[i].speed;
			if(ebullet[i].y > LIMITE_INF)
				ebullet[i].live = false;
		}

		if(++ebullet[i].frameCount >= ebullet[i].frameDelay)
			{
				ebullet[i].curFrame++;
				if(ebullet[i].curFrame >= ebullet[i].maxFrame)
					ebullet[i].curFrame = 0;

				ebullet[i].frameCount = 0;
			}
	}
}
void CollideEnemyBullet(EnemyBullet ebullet[], int bSize, Heli helis[], int cSize, PlayerTank &tank, Explosion explosions[], int eSize)
{
	for(int i = 0; i < bSize; i++)
	{
		if(ebullet[i].live)
		{
                if(ebullet[i].x > (tank.x - tank.boundx) &&
                    ebullet[i].x < (tank.x + tank.boundx) &&
                    ebullet[i].y > (tank.y - tank.boundy) &&
                    ebullet[i].y < (tank.y + tank.boundy))
                {
                    ebullet[i].live = false;
                    tank.lives-=(50-tank.armor);
                }
		}
	}
}


//BOMBAS INIMIGAS==================================================================================================================================================//

void InitBomb(EBomb bombs[], int size, ALLEGRO_BITMAP *image = NULL, ALLEGRO_BITMAP *anotherimage = NULL)
{
    bombs->event = false;

    bombs->dropDelay = BOMB_DROPDELAY;
    bombs->eventSpawnDelay = SPAWNRATE_BOMBS_DELAY;
	bombs->eventSpawnDelayCount = 0;
	bombs->spawnSet = false;

	bombs->bombDelay = 10;
	bombs->bombDelayCount = 0;
	bombs->cont=0;
	bombs->alt=0;

	bombs->delayCount = 0;

	for(int i = 0; i < size; i++)
	{
		bombs[i].ID = ENEMY;
		bombs[i].speed = 25;
		bombs[i].live = false;

        bombs[i].alert = false;

		bombs[i].alertWidth = 22;
		bombs[i].alertHeight = 86;

		bombs[i].maxFrame = 3;
		bombs[i].curFrame = 0;
		bombs[i].frameCount = 0;
        bombs[i].frameCountFire = 0;
		bombs[i].frameDelay = 2;
		bombs[i].frameWidth = 16; //256
		bombs[i].frameHeight = 29; //80
		bombs[i].animationColumns = 3;
		bombs[i].deslocamento = 0;

		if(image != NULL)
            bombs[i].image = image;

        if(anotherimage != NULL)
            bombs[i].anotherimage = anotherimage;
	}
}
void DrawBomb(EBomb bombs[], int size)
{
	for(int i = 0; i < size; i++)
	{
	    if(bombs[i].live)
		{
            al_draw_bitmap_region(bombs[i].image, 0, 0, bombs[i].frameWidth, bombs[i].frameHeight, bombs[i].x - bombs[i].frameWidth / 2, bombs[i].y - bombs[i].frameHeight / 2, 0);
		}
		else if(bombs[i].alert)
        {
            al_draw_bitmap_region(bombs[i].anotherimage, 0, 0, bombs[i].alertWidth, bombs[i].alertHeight, bombs[i].x - bombs[i].alertWidth / 2 + COEF_POSICAO, LIMITE_INF - bombs[i].alertHeight, 0);
        }
	}
	/*al_draw_bitmap_region(tank.image, 0, 0, 226,
                                      140, bombs->bombDelayCount, LIMITE_INF/4, 0);*/
}
void StartBombEvent(EBomb bombs[], int size)
{
    if(!bombs->event){
        if(++bombs->eventSpawnDelayCount>bombs->eventSpawnDelay)
        {
            bombs->event=true;
            bombs->eventSpawnDelayCount=0;
        }
    }
}
void StartBomb(EBomb bombs[], int size, PlayerTank &tank)
{
    if(!bombs->spawnSet){

        /* defines a random bomb to not spawn based on player's position. More than one if the screen is too wide. */

        int random1 = -1;
        int random2 = -1;

        if (tank.x <= (WIDTH+WIDTH*COEF_MOBILIDADE)/2){

            random1 = rand() % (size/2) + (size-1)/2 - 1;

            if (WIDTH>=1600){
                random2 = 1 + rand() % (size/2);
            }

        }else if (tank.x > (WIDTH+WIDTH*COEF_MOBILIDADE)/2){

            random1 = 1 + rand() % (size/2);
            if (WIDTH>=1600){
                random2 = rand() % (size/2) - 1;
            }

        }

        /* sets bombs' spawnpoint */

        for(int i = 0; i < size ; i++){
                if ((i != random1)&&(i != random2)){
                    bombs[i].x = 100 + ((WIDTH+WIDTH*COEF_MOBILIDADE)/size)*i;
                    bombs[i].y = 0;
                    bombs[i].deslocamento = bombs[i].x;
                    bombs[i].alert = true;
                }
        }

        bombs->spawnSet = true;

    }else{

        /* counts up to the bomb event's drop delay, dropping them afterwards */

        if(++bombs->delayCount > bombs->dropDelay){

            if(bombs->alt==0){

                if(bombs->bombDelayCount <= (WIDTH + WIDTH*COEF_MOBILIDADE)){

                    bombs->bombDelayCount += 20;

                    if(bombs[bombs->cont].x <= bombs->bombDelayCount){

                        if (bombs[bombs->cont].alert == true){
                            bombs[bombs->cont].live = true;
                        }

                        bombs->cont++;
                    }

                }else{

                    bombs->delayCount = 0;
                    bombs->event = false;
                    bombs->spawnSet = false;
                    bombs->alt=1;
                    bombs->cont=NUM_BOMBS-1;

                }

            }else{

                if(bombs->bombDelayCount >= 0 - WIDTH*COEF_MOBILIDADE){

                    bombs->bombDelayCount -= 20;

                    if(bombs[bombs->cont].x >= bombs->bombDelayCount){

                        if (bombs[bombs->cont].alert == true){
                            bombs[bombs->cont].live = true;
                        }

                        bombs->cont--;
                    }

                }else{

                    bombs->delayCount = 0;
                    bombs->event = false;
                    bombs->spawnSet = false;
                    bombs->alt=0;
                    bombs->cont=0;

                }
            }
        }
    }
}
void UpdateBomb(EBomb bombs[], int size, PlayerTank &tank, ExplosionII explosionsII[], int eSize)
{
	for(int i = 0; i < size; i++)
	{
		if(bombs[i].live)
		{
            bombs[i].x = bombs[i].deslocamento + COEF_POSICAO;
            bombs[i].y += bombs[i].speed;

                if(bombs[i].y>=LIMITE_INF){
                    StartExplosionsII(explosionsII, eSize, bombs[i].deslocamento, bombs[i].y);
                    bombs[i].alert = false;
                    bombs[i].live = false;
            }
        }
    }
}
void CollideBomb(EBomb bombs[], int bSize, Heli helis[], int cSize, PlayerTank &tank, ExplosionII explosionsII[], int eSize)
{
	for(int i = 0; i < bSize; i++)
	{
		if(bombs[i].live)
		{
                    if(bombs[i].x > (tank.x - tank.boundx) &&
						bombs[i].x < (tank.x + tank.boundx) &&
						bombs[i].y > (tank.y - tank.boundy) &&
						bombs[i].y < (tank.y + tank.boundy))
					{
                        bombs[i].alert = false;
						bombs[i].live = false;

						tank.lives -= (TANK_MAXLIFE/5 - tank.armor);

						StartExplosionsII(explosionsII, eSize, bombs[i].deslocamento, bombs[i].y);
					}
		}
	}
}


//DROPS DE REPARO============================================================================================================================================//

void InitRepair(Repair drop[], int size, ALLEGRO_BITMAP *image = NULL)
{
    drop->wait=0;
    drop->waitMAX=SPAWNRATE_DROP;
    drop->width=WIDTH;

	for(int i = 0; i < size; i++)
	{
		drop[i].ID = DROP;
		drop[i].speed = 5;
		drop[i].live = false;

		drop[i].maxFrame = 2;
        drop[i].curFrame = 0;
        drop[i].frameCount = 0;
        drop[i].frameDelay = 10;
        drop[i].frameWidth = 50;
        drop[i].frameHeight = 90;
        drop[i].animationColumns = 2;
        drop[i].deslocamento = 0;

        if(image!=NULL)
            drop[i].image = image;
	}
}
void DrawRepair(Repair drop[], int size)
{
	for( int i = 0; i < size; i++)
	{
		if(drop[i].live){
            if(drop[i].y<=LIMITE_INF-30){
            al_draw_bitmap_region(drop[i].image, 0, 0, drop[i].frameWidth, drop[i].frameHeight, drop[i].x - drop[i].frameWidth/2, drop[i].y - drop[i].frameHeight, 0);
	    } else {
            al_draw_bitmap_region(drop[i].image, drop[i].frameWidth, 0, drop[i].frameWidth, drop[i].frameHeight, drop[i].x - drop[i].frameWidth/2, drop[i].y - drop[i].frameHeight, 0);
            }
		}
	}
}
void DropRepair(Repair drop[], int size, PlayerTank &tank)
{

    if(tank.lives<=DROP_MINLIFE){

        if((++drop->wait)>(drop->waitMAX)){

                for( int i = 0; i < drop->alteraQuantidade; i++){

                    if(!drop[i].live){

                        drop[i].x = 80 + rand() % (drop->width - 80);
                        drop[i].deslocamento = drop[i].x;
                        drop[i].y = 0;
                        drop[i].live = true;
                        drop->wait=0;

                        break;

                }
            }
        }
    }
}
void UpdateRepair(Repair drop[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(drop[i].live)
		{
		    drop[i].x = drop[i].deslocamento + COEF_POSICAO;
			drop[i].y += drop[i].speed;
			if(drop[i].y > LIMITE_INF){
				drop[i].y = LIMITE_INF;
			}
		}
    }
}
void CollideRepair(Repair drop[], int size, PlayerTank &tank)
{
	for(int i = 0; i < size; i++)
	{
		if(drop[i].live)
		{
            if(drop[i].x + 10 > (tank.x - tank.boundx) &&
                drop[i].x - 10 < (tank.x + tank.boundx) &&
                drop[i].y + 30 > (tank.y - tank.boundy) &&
                drop[i].y - 30 < (tank.y + tank.boundy))
                {
                drop[i].live = false;

                tank.lives += 200;

                if(tank.lives>TANK_MAXLIFE){
                    tank.lives = TANK_MAXLIFE;
                }

            }
        }
    }
}


//EXPLOSOES==================================================================================================================================================//

void InitExplosions(Explosion explosions[], int size, ALLEGRO_BITMAP *image = NULL)
{
	for(int i = 0; i < size; i++)
	{
		explosions[i].live = false;

		explosions[i].maxFrame = 31;
		explosions[i].curFrame = 0;
		explosions[i].frameCount = 0;
		explosions[i].frameDelay = 2;
		explosions[i].frameWidth = 128;
		explosions[i].frameHeight = 128;
		explosions[i].animationColumns = 8;
		explosions[i].animationDirection = 1;
		explosions[i].deslocamento = 0;

        if(image != NULL)
            explosions[i].image = image;
	}
}
void DrawExplosions(Explosion explosions[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(explosions[i].live)
		{
			int fx = (explosions[i].curFrame % explosions[i].animationColumns) * explosions[i].frameWidth;
			int fy = (explosions[i].curFrame / explosions[i].animationColumns) * explosions[i].frameHeight;

			al_draw_bitmap_region(explosions[i].image, fx, fy, explosions[i].frameWidth, explosions[i].frameHeight, explosions[i].x - explosions[i].frameWidth / 2, explosions[i].y - explosions[i].frameHeight / 2, 0);
		}
	}
}
void StartExplosions(Explosion explosions[], int size, int x, int y)
{
	for(int i = 0; i < size; i++)
	{
		if(!explosions[i].live)
		{
			explosions[i].live = true;
			explosions[i].x = x;
			explosions[i].deslocamento = x;
			explosions[i].y = y;
			break;
		}
	}
}
void UpdateExplosions(Explosion explosions[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(explosions[i].live)
		{
			if(++explosions[i].frameCount >= explosions[i].frameDelay)
			{
				explosions[i].curFrame += explosions[i].animationDirection*2;

				if(explosions[i].curFrame >= explosions[i].maxFrame)
				{
					explosions[i].curFrame = 0;
					explosions[i].live = false;
				}

				explosions[i].frameCount = 0;
			}
		}
	}
}

void InitExplosionsII(ExplosionII explosionsII[], int size, ALLEGRO_BITMAP *image = NULL)
{
	for(int i = 0; i < size; i++)
	{
		explosionsII[i].live = false;

		explosionsII[i].maxFrame = 20;
		explosionsII[i].curFrame = 0;
		explosionsII[i].frameCount = 0;
		explosionsII[i].frameDelay = 4;
		explosionsII[i].frameWidth = 104;
		explosionsII[i].frameHeight = 204;
		explosionsII[i].animationColumns = 8;
		explosionsII[i].animationDirection = 1;

		if(image != NULL)
            explosionsII[i].image = image;
	}
}
void DrawExplosionsII(ExplosionII explosionsII[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(explosionsII[i].live)
		{
			int fx = (explosionsII[i].curFrame % explosionsII[i].animationColumns) * explosionsII[i].frameWidth;
			int fy = (explosionsII[i].curFrame / explosionsII[i].animationColumns) * explosionsII[i].frameHeight;

			al_draw_bitmap_region(explosionsII[i].image, fx, fy, explosionsII[i].frameWidth,
            explosionsII[i].frameHeight, explosionsII[i].x - explosionsII[i].frameWidth / 2 + COEF_POSICAO, explosionsII[i].y - explosionsII[i].frameHeight, 0);
		}
	}
}
void StartExplosionsII(ExplosionII explosionsII[], int size, int x, int y)
{
	for(int i = 0; i < size; i++)
	{
		if(!explosionsII[i].live)
		{
			explosionsII[i].live = true;
			explosionsII[i].x = x;
			explosionsII[i].deslocamento = x;
			explosionsII[i].y = y;
			break;
		}
	}
}
void UpdateExplosionsII(ExplosionII explosionsII[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(explosionsII[i].live)
		{
		    //explosionsII[i].x = explosionsII[i].deslocamento + COEF_POSICAO;

			if(++explosionsII[i].frameCount >= explosionsII[i].frameDelay)
			{
				explosionsII[i].curFrame += explosionsII[i].animationDirection;
				if(explosionsII[i].curFrame >= explosionsII[i].maxFrame)
				{
					explosionsII[i].curFrame = 0;
					explosionsII[i].live = false;
				}

				explosionsII[i].frameCount = 0;
			}
		}
	}
}


//MUDA ESTADOS=============================================================================================================================================//

void ChangeState(int &state, int newState)
{
	if(state == TITULO)
	{}
	else if(state == PREJOGO)
    {}
	else if(state == JOGANDO)
	{}
	else if(state == PAUSE)
	{}
	else if(state == DERROTA)
    {}
    else if(state == MADNESS)
    {}
    else if(state == VITORIA)
    {}

	state = newState;

	if(state == TITULO)
	{}
	else if(state == PREJOGO)
    {
        InitTank(tank);
        InitBullet(bullets, NUM_BULLETS, tank);
        InitEnemyBullet(ebullets, NUM_EBULLETS);
        InitBomb(bombs, NUM_BOMBS);
        InitMissile(misseis, NUM_MISSEIS, tank);
        InitHeli(helis, NUM_HELIS);
        InitExplosions(explosions, NUM_EXPLOSIONS);
        InitExplosionsII(explosionsII, NUM_BOMBS);
        InitBackground(background);
        InitRepair(drop, NUM_DROPS);
        InitProgredir(progresso);
        ChangeState(state, JOGANDO);

    }
	else if(state == JOGANDO)
	{}
	else if(state == PAUSE)
	{}
	else if(state == DERROTA)
    {}
    else if(state == MADNESS)
    {
        progresso.dificuldade=5;
    }
    else if(state == VITORIA)
    {}
}
