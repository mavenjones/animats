#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <GL/gl.h>
#include <GL/glut.h>   
#include <assert.h>

#include "lions.h"

#define MAX_YEARS 50

#define BIG_GAME 100
#define SMALL_GAME 100

#define VULTURE_RATE 30 //between 0-1000
#define EAT_RATE_V 6

#define EAT_RATE_F 2
#define EAT_RATE_M 2

#define MAX_AGE_M 4000
#define MAX_AGE_F 4000

#define MATING_ENERGY_M 200
#define MATING_ENERGY_F 2000 



typedef struct{
  int x;
  int y;
  int value;
  int numVultures;
  int hasBeenVultured;
} foodT;

typedef struct{
  int numBigGame;
  int numSmallGame;
  foodT* bigGame;
  foodT* smallGame;
  int nestx;
  int nesty;
} worldT;

void initWorld(worldT *world);

void newBigGame(worldT *world, int i);
void newSmallGame(worldT *world, int i);

void printTest(allMT *allM, allFT *allF);

void senseFoodM(allMT *allM, worldT *world);
void senseFoodF(allFT *allF, worldT *world);
void eatFoodF(allFT *allF, worldT *world);
void eatFoodM(allMT *allM, worldT *world);
void eatFoodV(worldT *world);
void chooseBehaviorM(allMT *allM, allFT *allF);
void chooseBehaviorF(allFT *allF, allMT *allM);

int feedForwardF(femaleT f);
void sendSignal(allFT *allF, int f); 
void propagateSignals(allMT *allM,allFT *allF, int f);
void helpFemales(allMT *allM, allFT *allF, worldT *world);

void renderScene();
void drawWorld();
void drawLions();
void drawVultures();


allMT *allM;
allFT *allF;
worldT *world;

int locked;
int count;
float clk;
int signals[5];

int main(int argc,char **argv) {
  
 
  srand(time(NULL));
  world = (worldT*) malloc(sizeof(worldT));
  allM = (allMT*) malloc(sizeof(allMT));
  allF = (allFT*) malloc(sizeof(allFT));
  
  initWorld(world);
  initMales(allM);
  initFemales(allF);

  count=0;
  clk=0;
  locked=0;
  for(int i=0; i<5; i++)
    signals[i]=0;

  glutInit(&argc,argv);
  glutInitWindowSize(600,600);
  glutInitWindowPosition(0,0);
  glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
  glutCreateWindow("Simulation");
  glutDisplayFunc(renderScene);
  glutIdleFunc(renderScene);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0,MAX_X,0,MAX_Y);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0,0.0,0.0,0.0);
  glutMainLoop();

  printTest(allM,allF);
  return 0;
}




void drawWorld() {
  int i,x,y;
  
  //draw nest
  glColor3f(1.0,1.0,1.0);
  glRecti(world->nestx,world->nesty,world->nestx+15,world->nesty+15);

  //draw food
  glColor3f(0.0,1.0,0.0);
  for(i=0; i<world->numBigGame; i++) {
    glBegin(GL_TRIANGLES);
    x=world->bigGame[i].x;
    y=world->bigGame[i].y;
    glVertex2i(x,y);
    glVertex2i(x+7,y+14);
    glVertex2i(x-7,y+14);
    glEnd();
  } 
  glColor3f(0.5,1.0,0.0);
  for(i=0; i<world->numSmallGame; i++) {
    glBegin(GL_TRIANGLES);
    x=world->smallGame[i].x;
    y=world->smallGame[i].y;
    glVertex2i(x,y);
    glVertex2i(x+3,y+6);
    glVertex2i(x-3,y+6);
    glEnd();
  }
}

void drawVultures() {
  int i,j,x,y;
  glColor3f(1.0,0.0,0.0);
  for(i=0; i<world->numBigGame; i++) {
    for(j=0; j<world->bigGame[i].numVultures; j++) {
      x=world->bigGame[i].x+j*7;
      y=world->bigGame[i].y+20;
      glRecti(x,y,x+4,y+4);
    }
  }
}

void drawLions() {
  int i,x,y;
  for(i=0; i<allM->num; i++) {
    glColor3f(1.0,1.0,0.0);
    if(allM->males[i].age==-1) continue;
    if((allM->males[i].target!=NULL)&&(allM->males[i].target->type==1))
      glColor3f(0.0,0.0,1.0);
    x=allM->males[i].x;
    y=allM->males[i].y;
    glRecti(x,y,x+10,y+10);
  }
  
  for(i=0; i<allF->num; i++) {
    glColor3f(1.0,0.4,0.0);
    if(allF->females[i].age==-1) continue;
    if(allF->females[i].signal.type>0)
      glColor3f(1.0,0.0,1.0);
    x=allF->females[i].x;
    y=allF->females[i].y;
    glRecti(x,y,x+10,y+10);
  }
}

void renderScene() {
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  //drawWorld();
  //drawLions();
  //drawVultures();

  //glutSwapBuffers();
  
  senseFoodM(allM, world);
  senseFoodF(allF, world);

  setMdir(allM);
  setFdir(allF);

  newPositions(allF, allM);

  eatFoodM(allM, world);
  eatFoodF(allF, world);
  helpFemales(allM,allF,world);
  eatFoodV(world);
  
  mateLions(allM,allF);

  chooseBehaviorM(allM,allF);
  chooseBehaviorF(allF,allM);

  count++;
  if(count==1000){
    clk=clk+1;
    count=0;
    printf("%f years (1000 ticks/year) have passed\n",clk);
    printTest(allM,allF);
  }
  if(clk==MAX_YEARS) {
    printTest(allM,allF);
    assert(1==2);
  }
}

void initWorld(worldT* world) {
  int i;
  world->nestx=MAX_X/2;
  world->nesty=MAX_Y/2;

  world->numBigGame = BIG_GAME;
  world->numSmallGame = SMALL_GAME;
  world->bigGame = (foodT*) malloc(BIG_GAME*sizeof(foodT));
  for(i=0; i<BIG_GAME; i++) {
    world->bigGame[i].value=(rand() % 600) + 1000;    //big game is ranged from 1000-1600
    world->bigGame[i].numVultures=0; 
    world->bigGame[i].x=rand()%(MAX_X); 
    world->bigGame[i].y=rand()%(MAX_Y);
    //world->bigGame[i].x=(rand()%(MAX_X/3))+(rand()%2)*MAX_X/3*2; //located in outer 2/3s
    //world->bigGame[i].y=(rand()%(MAX_Y/3))+(rand()%2)*MAX_Y/3*2; //located in outer 2/3s
    //printf("big game at %d %d\n",world->bigGame[i].x,world->bigGame[i].y);
  }
  world->smallGame = (foodT*) malloc(SMALL_GAME*sizeof(foodT));
  for(i=0; i<SMALL_GAME; i++) {
    world->smallGame[i].value=(rand() % 50) + 50;    //small game is ranged from 50-100
    world->smallGame[i].numVultures=0;
    world->smallGame[i].x=rand()%(MAX_X); 
    world->smallGame[i].y=rand()%(MAX_Y); 
    //world->smallGame[i].x=(rand()%(MAX_X_M-MIN_X_M))+MIN_X_M; //located in range of MIN_X_M
    //world->smallGame[i].y=(rand()%(MAX_Y_M-MIN_Y_M))+MIN_Y_M; //located in range of MIN_Y_M
    //printf("small game at %d %d\n",world->smallGame[i].x,world->smallGame[i].y);
  }
}


void chooseBehaviorM(allMT *allM, allFT *allF) {
  int i;
  for(i=0; i<allM->num; i++) {
    if(allM->males[i].age==-1) continue;
    

    allM->males[i].age++;
    if(allM->males[i].age>MAX_AGE_M) {
      //printf("male %d died\n",i);
      if(allM->males[i].target!=NULL)
	free(allM->males[i].target);
      allM->males[i].age=-1;
      continue;
    }
    
    if(allM->males[i].act==2) {
      if(foundTargetM(allM->males[i])) {
	allM->males[i].inHeat=1;
	//printf("male is ready to mate\n");
      }
      continue; //mating takes precedence over all
    }
    
    if(allM->males[i].energy>MATING_ENERGY_M) {
      allM->males[i].act=2;
      if(allM->males[i].target!=NULL)
	free(allM->males[i].target);
      allM->males[i].target = (targetT*) malloc(sizeof(targetT));
      allM->males[i].target->x=world->nestx;
      allM->males[i].target->y=world->nesty;
      allM->males[i].target->id=0;
      allM->males[i].target->type=2;
    }


    if(allM->males[i].act==0)
      if(foundTargetM(allM->males[i])) { 
	if(allM->males[i].target->type==0){
	  //printf("male %d started eating\n",i);
	  allM->males[i].act=1;
	}
	if(allM->males[i].target->type==1) {
	  allM->males[i].act=3;
	}
      }
  }	
}

void chooseBehaviorF(allFT *allF, allMT *allM) {
  int i;
  for(i=0; i<allF->num; i++) {
    if(allF->females[i].age==-1) continue;
    allF->females[i].age++;
    
    if(allF->females[i].age>MAX_AGE_F) {
      //printf("female %d died\n",i);
      if(allF->females[i].target!=NULL)
	free(allF->females[i].target);
      allF->females[i].age=-1;
      continue;
    }
    if(allF->females[i].signal.type>0)
      propagateSignals(allM,allF,i);
    
    if(allF->females[i].act==2) {
      if(foundTargetF(allF->females[i])) {
	allF->females[i].inHeat=1;
	//printf("female is ready to mate\n");
      }
      continue; //mating takes precedence over all
    }
    
    if(allF->females[i].energy>MATING_ENERGY_F) {
      allF->females[i].act=2;
      if(allF->females[i].target!=NULL)
	free(allF->females[i].target);
      allF->females[i].target = (targetT*) malloc(sizeof(targetT));
      allF->females[i].target->x=world->nestx;
      allF->females[i].target->y=world->nesty;
      allF->females[i].target->id=0;
      allF->females[i].target->type=2;
    }

    if(allF->females[i].act==0)
      if(foundTargetF(allF->females[i])) 
	if(allF->females[i].target->type==0) {
	  allF->females[i].act=1;
	  sendSignal(allF,i);
	  //printf("female %d started eating\n",i);
	}
  }
}	

void eatFoodV(worldT *world) {
  int i;
  for(i=0; i<world->numBigGame; i++) {
    world->bigGame[i].value-=world->bigGame[i].numVultures*EAT_RATE_V;
    //if((world->bigGame[i].value<=0)&&(world->bigGame[i].numVultures>0))
    //    printf("vulture finished food %d\n",i);
  }
}

void newSmallGame(worldT *world, int i) {
  world->smallGame[i].value=(rand() % 50) + 50;    //small game is ranged from 50-100
  world->smallGame[i].numVultures=0;
  world->smallGame[i].x=rand()%(MAX_X); 
  world->smallGame[i].y=rand()%(MAX_Y); 
  //world->smallGame[i].x=(rand()%(MAX_X_M-MIN_X_M))+MIN_X_M; //located in range of MIN_X_M
  //world->smallGame[i].y=(rand()%(MAX_Y_M-MIN_Y_M))+MIN_Y_M; //located in range of MIN_Y_M
  //printf("small game at %d %d\n",world->smallGame[i].x,world->smallGame[i].y);
}

void eatFoodM(allMT *allM, worldT *world) {
  int i;
  for(i=0; i<allM->num; i++) {
    if(allM->males[i].age==-1) continue;
    if(allM->males[i].act!=1) continue;
    if((world->smallGame[allM->males[i].target->id].x!=allM->males[i].target->x)||
       (world->smallGame[allM->males[i].target->id].y!=allM->males[i].target->y)) {	
      allM->males[i].act=0; 
      free(allM->males[i].target);
      allM->males[i].target = NULL;
      continue;
    } //if food isn't there anymore
    else if(world->smallGame[allM->males[i].target->id].value <= 0) {
      //printf("male %d finished food\n",i);
      newSmallGame(world, allM->males[i].target->id);
      allM->males[i].act=0;
      free(allM->males[i].target);
      allM->males[i].target = NULL;
      continue;
    } 
    world->smallGame[allM->males[i].target->id].value -= EAT_RATE_M;
    allM->males[i].energy += EAT_RATE_M;
  }
}
void helpFemales(allMT *allM, allFT *allF, worldT *world) {
  int i,id;
  for(i=0; i<allM->num; i++) {
    if(allM->males[i].age==-1) continue;
   
    if((allM->males[i].target==NULL)||(allM->males[i].target->type!=1))
      continue; //if not looking for a females
    id=allM->males[i].target->id;
    
    if(allF->females[id].target==NULL) {	
      allM->males[i].act=0; 
      free(allM->males[i].target);
      allM->males[i].target = NULL;
      //printf("male %d:%d is freed from service\n",i,locked);
      locked--;
      continue;
    } //if food isn't there anymore
    
    if(allM->males[i].act!=3) continue;  //if hasn't arrived yet

    
    //if(world->bigGame[allF->females[id].target->id].numVultures>0) {
    world->bigGame[allF->females[id].target->id].numVultures=0;
      //printf("scared off a vulture\n");
    //scare off a vulture if there are vultures
    //}
    world->bigGame[allF->females[id].target->id].value -= EAT_RATE_M;
    allM->males[i].energy += EAT_RATE_M;
  }
}

void newBigGame(worldT *world, int i) {
  world->bigGame[i].value=(rand() % 600) + 1000;    //big game is ranged from 400-600
  world->bigGame[i].numVultures=0;
  world->bigGame[i].hasBeenVultured=0;
  world->bigGame[i].x=rand()%(MAX_X); 
  world->bigGame[i].y=rand()%(MAX_Y); 
  //world->bigGame[i].x=(rand()%(MAX_X/3))+(rand()%2)*MAX_X/3*2; //located in outer 2/3s
  //world->bigGame[i].y=(rand()%(MAX_Y/3))+(rand()%2)*MAX_Y/3*2; //located in outer 2/3s
  //printf("big game at %d %d\n",world->bigGame[i].x,world->bigGame[i].y); 

}

void eatFoodF(allFT *allF, worldT *world) {
  int i,j;
  //for(j=0; j<world->numBigGame; j++) 
  //world->bigGame[i].hasBeenVultured=0;  //new round of vultures can come in
  
  for(i=0; i<allF->num; i++) {
    if(allF->females[i].age==-1) continue;
    if(allF->females[i].act!=1) continue;
    if((world->bigGame[allF->females[i].target->id].x!=allF->females[i].target->x)||
       (world->bigGame[allF->females[i].target->id].y!=allF->females[i].target->y)) {	
      allF->females[i].act=0; 
      free(allM->males[i].target);
      allF->females[i].target = NULL;
      continue;
    } //if food isn't there anymore
    else if(world->bigGame[allF->females[i].target->id].value <= 0) {
      newBigGame(world, allF->females[i].target->id);
      //printf("female %d finished food\n",i);
      allF->females[i].act=0;
      free(allF->females[i].target);
      allF->females[i].target = NULL;
      continue;
    } 
    //if(world->bigGame[allF->females[i].target->id].hasBeenVultured==0) {
      if((rand()%1000)<VULTURE_RATE) {
	world->bigGame[allF->females[i].target->id].numVultures++;
	//printf("NEW vulture\n");
      }
      //world->bigGame[allF->females[i].target->id].hasBeenVultured=1;
      //}
    world->bigGame[allF->females[i].target->id].value -= EAT_RATE_F;
    allF->females[i].energy += EAT_RATE_F;
  }
}

void senseFoodF(allFT *allF, worldT *world) {
  int i,j,best,bestx,besty,bestid,next;
  femaleT female;
  for(i=0; i<allF->num; i++) {
    if(allF->females[i].age==-1) continue;
    female=allF->females[i];
    if(female.target != NULL) continue;
    best=MAX_X;
    for(j=0; j<world->numBigGame; j++) {
      if(next=inRange(female.x,female.y,world->bigGame[j].x,world->bigGame[j].y,F_RANGE)) {
	if(next<best) {
	  //printf("female %d found food %d\n",i,j);
	  best=next;
	  bestid=j;
	  bestx=world->bigGame[j].x;
	  besty=world->bigGame[j].y;
	}
      }
    }
    if(best==MAX_X) continue;
    female.target = (targetT*) malloc(sizeof(targetT));
    female.target->x=bestx;
    female.target->y=besty;
    female.target->type=0;
    female.target->id=bestid;
    allF->females[i]=female;
   
  }
}
void senseFoodM(allMT *allM, worldT *world) {
  int i,j,best,bestx,besty,bestid,next;
  maleT male;
  for(i=0; i<allM->num; i++) {
    if(allM->males[i].age==-1) continue;
    male=allM->males[i];
    if(male.target != NULL) continue;
    best=MAX_X;
    for(j=0; j<world->numSmallGame; j++) {
      if(next=inRange(male.x,male.y,world->smallGame[j].x,world->smallGame[j].y,M_RANGE)) {
	if(next<best) {
	  //printf("male %d found food %d\n",i,j);
	  best=next;
	  bestx=world->smallGame[j].x;
	  besty=world->smallGame[j].y;
	  bestid=j;
	}
      }
    }
    if(best==MAX_X) continue;
    male.target = (targetT*) malloc(sizeof(targetT));
    male.target->x=bestx;
    male.target->y=besty;
    male.target->type=0;
    male.target->id=bestid;
    allM->males[i]=male;
  }
}


void printTest(allMT *allM, allFT *allF) {
  int i,c=0,total=0,sum=0;
  double ave;
  for(i=0; i<allM->num; i++) {
    if(allM->males[i].age==-1) continue;
    c++;
  }
  printf("%d LIONS AFTER %d YEARS\n",c,MAX_YEARS);
  for(i=0; i<5; i++){
    printf("signal %d: %d\n",i,signals[i]);
    total+=signals[i];
    sum+=signals[i]*i;
    signals[i]=0;
  }
  ave=(double)sum/total;
  printf("average signal: %f\n",ave);
} 

void sendSignal(allFT *allF, int f) {
  int output;

  output=feedForwardF(allF->females[f]);

  if(output<=-200)
    allF->females[f].signal.type=0;
  else if(output<-0) 
    allF->females[f].signal.type=1;
  else if(output<200) 
    allF->females[f].signal.type=2;
  else if(output<400)
    allF->females[f].signal.type=3;
  else 
    allF->females[f].signal.type=4;

  signals[allF->females[f].signal.type]++;
  //printf("sent signal of type %d\n",allF->females[f].signal.type);
  allF->females[f].signal.duration=25;
  allF->females[f].signal.spread=0;
}


void propagateSignals(allMT *allM, allFT *allF,int f) {
  //go through all females and advance their signals
  //   go through all males and sense the signals
  int i;
  for(i=0; i<allM->num; i++) {
    if(allM->males[i].target!=NULL)
      if(allM->males[i].target->type==1)
	continue; //don't get distracted from other females
    if((allM->males[i].act==2)||(allM->males[i].act==3))
      continue; //if male is going to mate, don't respond
    if(inRange(allM->males[i].x,allM->males[i].y,allF->females[f].x,allF->females[f].y,allF->females[f].signal.spread)) {
      if(allM->males[i].target!=NULL)
	free(allM->males[i].target);
      allM->males[i].target= (targetT*) malloc(sizeof(targetT));
      allM->males[i].target->x=allF->females[f].x;
      allM->males[i].target->y=allF->females[f].y;
      allM->males[i].target->type=1;
      allM->males[i].target->id=f;
      locked++;
      //printf("male %d:%d acquires a female target\n",i,locked);
    }
  }
  allF->females[f].signal.duration--;
  allF->females[f].signal.spread+=2*allF->females[f].signal.type;
  //for duration 25, will spread 2*25*T units, faster for farther
  if(allF->females[f].signal.duration<=0)
    allF->females[f].signal.type=0;
}

int feedForwardF(femaleT f){
  int i,c,out=0;
  int inputs[NUM_WEIGHTS];
  double dx = f.x-world->nestx;
  double dy = f.y-world->nesty;
  double dist = dx*dx + dy*dy;
  dist = sqrt(dist);
  
  inputs[0]=10; //bias factor
  
 for(i=0; i<allM->num; i++) {
    if(allM->males[i].age==-1) continue;
    c++;
  }
  inputs[1]=c;  //estimation of population
  inputs[2]=(int)dist/100; //distance back to nest
  inputs[3]=(MATING_ENERGY_F-f.energy)/100;  //hunger level

  for(i=0; i<NUM_WEIGHTS; i++) 
    out+=inputs[i]*f.genesF[i];
  
  return out;
}
    
