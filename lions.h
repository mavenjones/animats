#ifndef _LIONS_H_
#define _LIONS_H_


#define M_RANGE 200
#define F_RANGE 200
#define FSTEP 4
#define MSTEP 4
#define MOVE_RANDOMNESS 15 //between 0-1000

#define MAX_MALES 1000
#define MAX_FEMALES 1000
#define INIT_MALES 50
#define INIT_FEMALES 50

#define MAX_X 1600
#define MAX_Y 1600
#define MAX_X_M 1200
#define MAX_Y_M 1200
#define MIN_X_M 400
#define MIN_Y_M 400
#define NUM_WEIGHTS 4

typedef struct{
  int type;
  int duration;
  int spread;
} signalT;

typedef struct{
  int x;
  int y;
  int type;    //0 if target is food, 1 if female, 2 if nest
  int id;
} targetT;

typedef struct{
  int x;
  int y;
  int act;  //current behavior, 0 wander, 1 eat, 2 mate, 3 scare
  int dir;  //1-9, like the number pad
  int energy;
  int age;
  int inHeat;
  targetT* target;
  int genesM[NUM_WEIGHTS];
  int genesF[NUM_WEIGHTS];
} maleT;

typedef struct{
  int x;
  int y;
  int act; //current behavior, 0 wander, 1 eat
  int dir; //1-9, like the number pad
  int energy;
  int age;
  int inHeat;
  targetT* target;
  signalT signal;
  int genesM[NUM_WEIGHTS];
  int genesF[NUM_WEIGHTS];
} femaleT;

typedef struct{
  int num;
  maleT* males;
} allMT;

typedef struct{
  int num;
  femaleT* females;
} allFT;

void initMales(allMT *allM);
void initFemales(allFT *allF);
void setMdir(allMT *allM);
void setFdir(allFT *allF);
void newPositions(allFT *allF, allMT *allM);
int foundTargetM(maleT male);
int foundTargetF(femaleT female);
int inRange(int x1, int y1, int x2, int y2, int range);

void mateLions(allMT *allM, allFT *allF);
int getInsertionM(allMT *allM);
int getInsertionF(allFT *allF);
maleT newMale();
femaleT newFemale();

void printBirth(femaleT mom,maleT dad,femaleT daughter,maleT son);
#endif
