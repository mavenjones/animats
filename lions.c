#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <GL/glut.h>
#include <assert.h>

#include "lions.h"


void initMales(allMT *allM) {
  int i,j; 
  allM->num=INIT_MALES;
  //printf("%d\n",allM->num);
  allM->males = (maleT*) malloc(MAX_MALES*sizeof(maleT));
  for(i=0; i<allM->num; i++) {
    allM->males[i].energy=0;
    //allM->males[i].x=(rand()%(MAX_X_M-MIN_X_M))+MIN_X_M;
    //allM->males[i].y=(rand()%(MAX_Y_M-MIN_Y_M))+MIN_Y_M;
    allM->males[i].x=rand() % MAX_X;
    allM->males[i].y=rand() % MAX_Y;
    allM->males[i].dir=5;
    allM->males[i].act=0;
    allM->males[i].age=0;
    allM->males[i].inHeat=0;
    allM->males[i].target=NULL;
    for(j=0; j<NUM_WEIGHTS; j++){
      allM->males[i].genesM[j]=rand()%20-10; //weights are from -10 to 10
      allM->males[i].genesM[j]=rand()%20-10;
    }
  }
}

void initFemales(allFT *allF) {
  int i,j;
  allF->num=INIT_FEMALES;
  allF->females = (femaleT*) malloc(MAX_FEMALES*sizeof(femaleT));
  for(i=0; i<allF->num; i++) {
    allF->females[i].energy=0;
    allF->females[i].x=rand() % MAX_X;
    allF->females[i].y=rand() % MAX_Y;
    allF->females[i].age=0; 
    allF->females[i].dir=5;
    allF->females[i].act=0;
    allF->females[i].inHeat=0;
    allF->females[i].target=NULL;
    allF->females[i].signal.type=0;
    for(j=0; j<NUM_WEIGHTS; j++){
      allF->females[i].genesM[j]=rand()%20-10; //weights are from -10 to 10
      allF->females[i].genesF[j]=rand()%20-10;
    }
  }
}


int inRange(int x1, int y1, int x2, int y2, int range) {
  double dx = x1-x2;
  double dy = y1-y2;
  double dist = dx*dx + dy*dy;
  //printf("distsquared: %f\n",dist);
  dist = sqrt(dist);
  //printf("dist %f: %d %d  and %d %d\n",dist,x1,y1,x2,y2);
  return (dist < range)? (int)(dist):0;
}

void setFdir(allFT *allF) {
  int i,dir;
  femaleT female;
  for(i=0; i<allF->num; i++) {
    if(allF->females[i].age==-1) continue;
    female=allF->females[i];
    dir=female.dir;
    if(female.target!=NULL) {
      if(female.target->x > female.x) {
	if(female.target->y > female.y)
	  dir = 9;
	else if(female.target->y < female.y)
	  dir = 3;
	else
	  dir=6;
      }else if(female.target->x < female.x) {
	if(female.target->y > female.y)
	  dir = 7;
	else if(female.target->y < female.y)
	  dir = 1;
	else
	  dir=4;
      }else {
	if(female.target->y > female.y)
	  dir = 8;
	else if(female.target->y < female.y)
	  dir = 2;
	else
	 dir=5;
      }
    } else {  //if target is NULL
      if((female.dir==5)||(rand()%1000<MOVE_RANDOMNESS)) //if no current direction, pick a random direction
      	dir=rand()%8+1;
    }
  
    allF->females[i].dir=dir;
  }
}

void setMdir(allMT *allM) {
  int i,dir;
  maleT male;
  for(i=0; i<allM->num; i++) {
    if(allM->males[i].age==-1) continue;
    male=allM->males[i];
    dir=male.dir;
    if(male.target!=NULL) {
      if(male.target->x > male.x) {
	if(male.target->y > male.y)
	  dir = 9;
	else if(male.target->y < male.y)
	  dir = 3;
	else
	  dir=6;
      }else if(male.target->x < male.x) {
	if(male.target->y > male.y)
	  dir = 7;
	else if(male.target->y < male.y)
	  dir = 1;
	else
	  dir=4;
      }else {
	if(male.target->y > male.y)
	  dir = 8;
	else if(male.target->y < male.y)
	  dir = 2;
	else
	 dir=5;
      }
    } else {  //if target is NULL
      if((male.dir==5)||(rand()%1000<MOVE_RANDOMNESS))//if no current direction, pick a random direction
	  dir=rand()%8+1;
    }
    
    allM->males[i].dir=dir;
  }
}
	
      
void newPositions(allFT *allF, allMT *allM) {
  int i;
  for(i=0; i<allF->num; i++) {
    if(allF->females[i].age==-1) continue;
    switch (allF->females[i].dir) {
    case 1 : allF->females[i].x -= FSTEP;
      allF->females[i].y -= FSTEP; break;
    case 2 : allF->females[i].y -= FSTEP; break;
    case 3 : allF->females[i].x += FSTEP;
      allF->females[i].y -= FSTEP; break;
    case 4 : allF->females[i].x -= FSTEP; break;
    case 6 : allF->females[i].x += FSTEP; break;
    case 7 : allF->females[i].x -= FSTEP;
      allF->females[i].y += FSTEP; break;
    case 8 : allF->females[i].y += FSTEP; break;
    case 9 : allF->females[i].x += FSTEP;
      allF->females[i].y += FSTEP; break;
    }
    if(allF->females[i].y>MAX_Y)
      allF->females[i].y=0; 
    if(allF->females[i].y<0)
      allF->females[i].y=MAX_Y; 
    if(allF->females[i].x>MAX_X)
      allF->females[i].x=0; 
    if(allF->females[i].x<0)
      allF->females[i].x=MAX_X;
  }
  for(i=0; i<allM->num; i++) {
    if(allM->males[i].age==-1) continue;
    switch (allM->males[i].dir) {
    case 1 : allM->males[i].x -= MSTEP;
      allM->males[i].y -= MSTEP; break;
    case 2 : allM->males[i].y -= MSTEP; break;
    case 3 : allM->males[i].x += MSTEP;
      allM->males[i].y -= MSTEP; break;
    case 4 : allM->males[i].x -= MSTEP; break;
    case 6 : allM->males[i].x += MSTEP; break;
    case 7 : allM->males[i].x -= MSTEP;
      allM->males[i].y += MSTEP; break;
    case 8 : allM->males[i].y += MSTEP; break;
    case 9 : allM->males[i].x += MSTEP;
      allM->males[i].y += MSTEP; break;
    }
    if(allM->males[i].y>MAX_Y)
      allM->males[i].y=0; 
    if(allM->males[i].y<0)
      allM->males[i].y=MAX_Y; 
    if(allM->males[i].x>MAX_X)
      allM->males[i].x=0; 
    if(allM->males[i].x<0)
      allM->males[i].x=MAX_X;
  }
}

int foundTargetM(maleT male) {
  if(male.target==NULL) return 0;
  return inRange(male.x,male.y,male.target->x,male.target->y,2*MSTEP);
}
int foundTargetF(femaleT female) {
  if(female.target==NULL) return 0;
  return inRange(female.x,female.y,female.target->x,female.target->y,2*FSTEP);
}

int getInsertionM(allMT *allM) {
  int i;
  for(i=0; i<allM->num; i++) {
    if(allM->males[i].age==-1)
      return i;
  }
  assert(allM->num+1<MAX_MALES);
  if(allM->num+1==MAX_MALES) printf("male overflow!!!!!\n");
  return allM->num++;
}  

int getInsertionF(allFT *allF) {
  int i;
  for(i=0; i<allF->num; i++) {
    if(allF->females[i].age==-1)
      return i;
  }
  assert(allF->num+1<MAX_FEMALES);
  if(allF->num+1==MAX_FEMALES) printf("female overflow!!!!!\n");
  return allF->num++;
}
 
maleT newMale() {
  maleT male;
  male.energy=0;
  //allM->males[i].x=(rand()%(MAX_X_M-MIN_X_M))+MIN_X_M;
  //allM->males[i].y=(rand()%(MAX_Y_M-MIN_Y_M))+MIN_Y_M;
  //male.x=rand() % MAX_X;
  //male.y=rand() % MAX_Y;
  male.x=MAX_X/2;
  male.y=MAX_Y/2;
  male.dir=5;
  male.act=0;
  male.age=0;
  male.inHeat=0;
  male.target=NULL;
  return male;
}

femaleT newFemale() {
  femaleT female;
  female.energy=0;
  //allM->males[i].x=(rand()%(MAX_X_M-MIN_X_M))+MIN_X_M;
  //allM->males[i].y=(rand()%(MAX_Y_M-MIN_Y_M))+MIN_Y_M;
  //female.x=rand() % MAX_X;
  //female.y=rand() % MAX_Y;
  female.x=MAX_X/2;
  female.y=MAX_Y/2;
  female.dir=5;
  female.act=0;
  female.age=0;
  female.inHeat=0;
  female.target=NULL;
  female.signal.type=0;
  return female;
}
 
void mateLions(allMT *allM, allFT *allF) {
  int i,j,m,f,cross,g;
  for(i=0; i<allF->num; i++) {
    if(allF->females[i].age==-1) continue;
    if(allF->females[i].inHeat==0) continue;
    for(j=0; j<allM->num; j++) {
      if(allM->males[j].age==-1) continue;
      if(allM->males[j].inHeat==0) continue;
      m=getInsertionM(allM);
      f=getInsertionF(allF);
      
      //allM->males[m]=newMale();
      allM->males[m].energy=0;
     
      allM->males[m].x=MAX_X/2;
      allM->males[m].y=MAX_Y/2;
      allM->males[m].dir=5;
      allM->males[m].act=0;
      allM->males[m].age=0;
      allM->males[m].inHeat=0;
      allM->males[m].target=NULL;
      
      cross=rand()%(NUM_WEIGHTS*2);
      for(g=0; g<cross; g++){
	if(g<NUM_WEIGHTS) //male genes
	  allM->males[m].genesM[g]=allM->males[j].genesM[g];
	else //female genes
	  allM->males[m].genesF[g-NUM_WEIGHTS]=allM->males[j].genesF[g-NUM_WEIGHTS];
      }
      for(g=cross; g<NUM_WEIGHTS*2; g++){
	if(g<NUM_WEIGHTS) //male genes
	  allM->males[m].genesM[g]=allF->females[i].genesM[g];
	else //female genes
	  allM->males[m].genesF[g-NUM_WEIGHTS]=allF->females[i].genesF[g-NUM_WEIGHTS];
      }

      //allF->females[f]=newFemale();
      allF->females[f].energy=0;
      
      allF->females[f].x=MAX_X/2;
      allF->females[f].y=MAX_Y/2;
      allF->females[f].dir=5;
      allF->females[f].act=0;
      allF->females[f].age=0;
      allF->females[f].inHeat=0;
      allF->females[f].target=NULL;
      allF->females[f].signal.type=0;

      cross=rand()%(NUM_WEIGHTS*2);
      for(g=0; g<cross; g++){
	if(g<NUM_WEIGHTS) //male genes
	  allF->females[f].genesM[g]=allM->males[j].genesM[g];
	else //female genes
	  allF->females[f].genesF[g-NUM_WEIGHTS]=allM->males[j].genesF[g-NUM_WEIGHTS];
      }
      for(g=cross; g<NUM_WEIGHTS*2; g++){
	if(g<NUM_WEIGHTS) //male genes
	  allF->females[f].genesM[g]=allF->females[i].genesM[g];
	else //female genes
	  allF->females[f].genesF[g-NUM_WEIGHTS]=allF->females[i].genesF[g-NUM_WEIGHTS];
      }
       
      //printBirth(allF->females[i],allM->males[j],allF->females[f],allM->males[m]);
      //printf("male %d and female %d are born!!!!\n",m,f);
      //the parents go do other stuff now
      allF->females[i].energy=0;
      allM->males[j].energy=0;
      if(allF->females[i].target!=NULL)
      free(allF->females[i].target);
      allF->females[i].target=NULL;
      if(allM->males[j].target!=NULL)
	free(allM->males[j].target);
      allM->males[j].target=NULL;
      allF->females[i].act=0;
      allM->males[j].act=0;
      allF->females[i].inHeat=0;
      allM->males[j].inHeat=0;
      break;
    }
  }
}

void printBirth(femaleT mom,maleT dad,femaleT daughter,maleT son) {
  int i;
  printf("Mother: ");
  for(i=0; i<NUM_WEIGHTS; i++)
    printf("%d ",mom.genesM[i]);
  printf("\n         ");
  for(i=0; i<NUM_WEIGHTS; i++)
    printf("%d ",mom.genesF[i]);
  printf("\nFather: ");
  for(i=0; i<NUM_WEIGHTS; i++)
    printf("%d ",dad.genesM[i]);
  printf("\n         ");
  for(i=0; i<NUM_WEIGHTS; i++)
    printf("%d ",dad.genesF[i]); 
  printf("\nDaughter: ");
  for(i=0; i<NUM_WEIGHTS; i++)
    printf("%d ",daughter.genesM[i]);
  printf("\n         ");
  for(i=0; i<NUM_WEIGHTS; i++)
    printf("%d ",daughter.genesF[i]);
  printf("\nSon: ");
  for(i=0; i<NUM_WEIGHTS; i++)
    printf("%d ",son.genesM[i]);
  printf("\n         ");
  for(i=0; i<NUM_WEIGHTS; i++)
    printf("%d ",son.genesF[i]);
  printf("\n");
}
