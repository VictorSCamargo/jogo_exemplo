#ifndef OBJECTSPLUS_H_INCLUDED
#define OBJECTSPLUS_H_INCLUDED

//objects

enum IDS {PLAYER, BULLET, EBULLET, MISSEIS, ENEMY, DROP};

//player

struct PlayerTank {
	int ID;
	int x;
	int y;
	float lives;
	int armor;
	float speed;
	float acel;
	float maxspeed;
	int boundx;
	int boundy;
	int score;

	int maxFrame;
	int curFrame;
	int frameCount;
	int frameDelay;
	int frameWidth;
	int frameHeight;
	int animationColumns;
	int animationDirection;

	int livesCheck;
	int frameCount2;
	int imageSelect;

	int animationRow;

	ALLEGRO_BITMAP *image;
	ALLEGRO_BITMAP *image2;
	ALLEGRO_BITMAP *image3;
};

struct Bullet {
	int ID;
	int x;
	int y;
	bool live;
	int speed;
	int alterna;
	int cont;

	int CD; //ALTERA COM O TEMPO

	bool shot;

	ALLEGRO_BITMAP *image;
};

struct Missel {
	int ID;
	int x;
	int y;
	int boundx;
	int boundy;
	bool live;
	float speed;
	float aceler;
	bool alterna;
	int cano;

	float CD;
	float CDmax; //ALTERA COM O TEMPO
	int alteraQuantidade;
	int wait;
	int waitMAX;

	int cont;
	bool ready;
	bool firing;
	bool shot;

	ALLEGRO_BITMAP *image;
};

struct EnemyBullet {
    int ID;
	int x;
	int y;
	bool live;
	int speed;

	int alteraSpawnrate;
	int alteraQuantidade;

	int shotDelay;
	int shotDelayCount;
	int shotCont;
	bool shooting;

	int maxFrame;
	int curFrame;
	int frameCount;
	int frameDelay;
	int frameWidth;
	int frameHeight;
	int animationColumns;
	int animationDirection;

	ALLEGRO_BITMAP *image;

};

struct Repair {
    int ID;
	int x;
	int y;
	bool live;
	int speed;
	int width;

	int alteraQuantidade;
	int spawnrate;
	int minlife;
	float wait;
	float waitMAX;
	float deslocamento;

	int maxFrame;
	int curFrame;
	int frameCount;
	int frameDelay;
	int frameWidth;
	int frameHeight;
	int animationColumns;
	int animationDirection;

	ALLEGRO_BITMAP *image;

};

struct EBomb {
	int ID;
	int x;
	int y;
	bool live;
	int speed;

	bool event;
	bool spawnSet;
	bool alert;

	int dropDelay;

	int alteraQuantidade; //ALTERA COM O TEMPO

	int eventSpawnDelay;
	int eventSpawnDelayCount;

	int delayCount;

	int bombDelay;
	int bombDelayCount;
	int cont;
	int alt;

	int alertWidth;
	int alertHeight;
	float deslocamento;

	int maxFrame;
	int curFrame;
	int frameCount;
	int frameCountFire;
	int frameDelay;
	int frameWidth;
	int frameHeight;
	int animationColumns;
	int animationDirection;

	ALLEGRO_BITMAP *image;
	ALLEGRO_BITMAP *anotherimage;
};

struct Heli {
	int ID;
	int x;
	int y;
	int lives;
	bool live;
	int boundx;
	int boundy;
	int side;
	int speed;
	bool firing;
	int firingCD;
	int firingCDCont;
	int shotCont;

	int alteraQuantidade;

	int maxFrame;
	int curFrame;
	int frameCount;
	int frameCountFire;
	int frameDelay;
	int frameWidth;
	int frameHeight;
	int animationColumns;
	int animationDirection;
	int mode;


	ALLEGRO_BITMAP *image;
};

struct Explosion {
	int x;
	int y;
	bool live;

	int maxFrame;
	int curFrame;
	int frameCount;
	int frameDelay;
	int frameWidth;
	int frameHeight;
	int animationColumns;
	int animationDirection;
	float deslocamento;

	ALLEGRO_BITMAP *image;
};

struct ExplosionII {
	int x;
	int y;
	bool live;

	int maxFrame;
	int curFrame;
	int frameCount;
	int frameDelay;
	int frameWidth;
	int frameHeight;
	int animationColumns;
	int animationDirection;
	float deslocamento;

	ALLEGRO_BITMAP *image;
};

struct Background {
    ALLEGRO_BITMAP *image;
};

struct Progride {
    int tempo;
    float Dtempo;
    int minutos;
    int segundos;
    bool fim;

    bool vivo;
    bool endgame;

    int dificuldade;
    int difatual;
    int mostrartexto;
    int conttexto;
    int contMAX;

    int contMADNESS;
    int maxMADNESS;

    ALLEGRO_FONT *font;
};


#endif // OBJECTSPLUS_H_INCLUDED
