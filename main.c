#include "defs.h"

/*Function/function pointer: ghostSimulation
  Purpose: ghost thread will be passed through this function which 
  itself calls the function to simulate the ghost actions
  in: void pointer, which is then casted to ghost pointer*/
void* ghostSimulation(void* arg){
  GhostType* ghostTh = (GhostType*) arg;
  GhostActions(ghostTh);
}

/*Function/function pointer: hunterSimulation
  Purpose: hunter thread will be passed through this function which 
  itself calls the function to simulate the hunter actions
  in: void pointer, which is then casted to hunter pointer*/
void* hunterSimulation(void* arg){
  HunterType* hunterTh = (HunterType*) arg;
  HunterActions(hunterTh);
}

int main(){

  // Initialize a random seed for the random number generators
  srand(time(NULL));


  pthread_t hunterThreads[MAX_HUNTERS];
  pthread_t ghostThread;

  char name1[MAX_STR];
  char name2[MAX_STR];
  char name3[MAX_STR];
  char name4[MAX_STR];

  //get the 4 names and creates the 4 hunters
  int count =1;
  printf("Enter name %d: ",count);
  fgets(name1,sizeof(name1),stdin);
  name1[strlen(name1)-1]='\0';
  count++;

  printf("Enter name %d: ",count);
  fgets(name2,sizeof(name2),stdin);
  name2[strlen(name2)-1]='\0';
  count++;

  printf("Enter name %d: ",count);
  fgets(name3,sizeof(name3),stdin);
  name3[strlen(name3)-1]='\0';
  count++;

  printf("Enter name %d: ",count);
  fgets(name4,sizeof(name4),stdin);
  name4[strlen(name4)-1]='\0';


  //create the building and pupulate it with rooms
  BuildingType theBuilding;
  loadArea(&(theBuilding), "Willow Street House");
  BuildingView(&theBuilding);


  //create the ghost, initialise it, attach it to the building
  //and spawn it in a random building, but not the van
  GhostType theGhost;
  initGhost(&(theGhost));
  theBuilding.theGhost = &(theGhost);
  SpawnGhost(&(theBuilding));

  //get the room van, it's the head of buildinf all rooms linked list
  RoomType* theVan = theBuilding.allRooms.head->roomData;

  //create the 4 hunters and place them in the van
  HunterType aHunter0;;
  initHunter(&(aHunter0),name1,0);
  theBuilding.allHunters[0] = &(aHunter0);
  SpawnHunter(theVan,theBuilding.allHunters[0]);

  HunterType aHunter1;
  initHunter(&(aHunter1),name2,1);
  theBuilding.allHunters[1] = &(aHunter1);
  SpawnHunter(theVan,theBuilding.allHunters[1]);

  HunterType aHunter2;
  initHunter(&(aHunter2),name3,2);
  theBuilding.allHunters[2] = &(aHunter2);
  SpawnHunter(theVan,theBuilding.allHunters[2]);

  HunterType aHunter3;
  initHunter(&(aHunter3),name4,3);
  theBuilding.allHunters[3] = &(aHunter3);
  SpawnHunter(theVan,theBuilding.allHunters[3]);



  //start multi-threading


  //create the ghost thread
  pthread_create(&(ghostThread),NULL,ghostSimulation,theBuilding.theGhost);

  //create the hunters thread from a for loop
  for (int i=0;i<MAX_HUNTERS;i++){
    pthread_create(hunterThreads+i,NULL,hunterSimulation,theBuilding.allHunters[i]);
  }


  //wait for all threads to finish and join back
  for (int j = 0; j < MAX_HUNTERS; j++){
    pthread_join(hunterThreads[j], NULL);
  }
  pthread_join(ghostThread,NULL);


  //display the state of the building after the hunt
  printf("\n\n\n\n");
  ViewBuildingAfter(&(theBuilding));


  //free dynamically memory previouly allocated to the buidling
  cleanupBuilding(&theBuilding);


  //after hunt, use data to find out who won, and determine ghost type
  char type[20];
  switch(theBuilding.theGhost->ghostClass){
    case POLTERGEIST:
      strcpy(type,"POLTERGEIST");
      break;
    case BANSHEE:
      strcpy(type,"BANSHEE");
      break;
    case BULLIES:
      strcpy(type,"BULLIES");
      break;
    case PHANTOM:
      strcpy(type,"PHANTOM");
      break;
  }


  int countf=0;
  //list evevry hunter that got scared
  for (int k = 0;k<MAX_HUNTERS;k++){
    if (theBuilding.allHunters[k]->fear >=100){
      printf("Hunter %s had fear >= 100, they got scared\n", theBuilding.allHunters[k]->name);
      countf +=1;
    }
  }

  //find out if any hunter actually collected 3 different types of evidences
  int acqh0 = TypesCollected(theBuilding.allHunters[0]->acquiredTypes);
  int acqh1 = TypesCollected(theBuilding.allHunters[1]->acquiredTypes);
  int acqh2 = TypesCollected(theBuilding.allHunters[2]->acquiredTypes);
  int acqh3 = TypesCollected(theBuilding.allHunters[3]->acquiredTypes);

  if (acqh0==3){
    printf("Repeat: Hunter %s got enough evidence and won\n",theBuilding.allHunters[0]->name);
    switch (typesSum(theBuilding.allHunters[0]->acquiredTypes)){
      case 3:
        printf("Ghost is a poltergeist\n");
        break;

      case 4:
        printf("Ghost is a banshee\n");
        break;

      case 5:
        printf("Ghost is a bully\n");
        break;

      case 6:
        printf("Ghost is a phantom\n");
    }

  }
  else if (acqh1==3){
    printf("Repeat: Hunter %s got enough evidence and won\n",theBuilding.allHunters[1]->name);
    switch (typesSum(theBuilding.allHunters[1]->acquiredTypes)){
      case 3:
        printf("Ghost is a poltergeist\n");
        break;

      case 4:
        printf("Ghost is a banshee\n");
        break;

      case 5:
        printf("Ghost is a bully\n");
        break;

      case 6:
        printf("Ghost is a phantom\n");
    }

  }
  else if(acqh2==3){
    printf("Repeat: Hunter %s got enough evidence and won\n",theBuilding.allHunters[2]->name);
    switch (typesSum(theBuilding.allHunters[2]->acquiredTypes)){
      case 3:
        printf("Ghost is a poltergeist\n");
        break;

      case 4:
        printf("Ghost is a banshee\n");
        break;

      case 5:
        printf("Ghost is a bully\n");
        break;

      case 6:
        printf("Ghost is a phantom\n");
    }
  }else if (acqh3==3){
    printf("Repeat: Hunter %s got enough evidence and won\n",theBuilding.allHunters[3]->name);
    switch (typesSum(theBuilding.allHunters[3]->acquiredTypes)){
      case 3:
        printf("Ghost is a poltergeist\n");
        break;

      case 4:
        printf("Ghost is a banshee\n");
        break;

      case 5:
        printf("Ghost is a bully\n");
        break;

      case 6:
        printf("Ghost is a phantom\n");
    }
  }else if (countf == 4){
    printf("Ghost won, ghost was of class %s\n",type);

  }else{
    //if no one got 3 different types, find out who couldn't collect their respective evidence 
    //and infer ghost type from that
    EvidenceClassType notObtained = 0;
    int multiple = -1;
    for (int v = 0;v<MAX_HUNTERS;v++){
      int indexC = theBuilding.allHunters[v]->indexCode;
      if (theBuilding.allHunters[v]->acquiredTypes[indexC]==0){
        notObtained = indexC;
        multiple = multiple + 1;
      }
    }

    if (multiple>0){
      printf("Not enough evidence was obtained by the hunters to determine the ghost type\nBy default ghost won, ghost was of type %s\n\n",type);
    }else{
      switch (notObtained){
        case EMF:
          printf("From evidence collected, ghost was a Phantom\n");
          break;

        case TEMPERATURE:
          printf("From evidence collected, ghost was a Bully\n");
          break;

        case FINGERPRINTS:
          printf("From evidence collected, ghost was a Banshee\n");
          break;

        case SOUND:
          printf("From evidence collected, ghost was a Poltergeist\n");
          break;
      }
    }

  }


  return(0);
}


/*
  Function:  randInt
  Purpose:   returns a pseudo randomly generated number, 
             in the range min to (max - 1), inclusively
       in:   upper end of the range of the generated number
   return:   randomly generated integer in the range [min, max-1) 
*/
int randInt(int min, int max)
{
    return rand() % (max - min) + min;
}


/*
  Function:  randFloat
  Purpose:   returns a pseudo randomly generated number, 
             in the range min to max, inclusively
       in:   upper end of the range of the generated number
   return:   randomly generated integer in the range [0, max-1) 
*/
float randFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    return random * (b - a) + a;
}
