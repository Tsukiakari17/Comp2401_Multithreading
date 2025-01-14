#include "defs.h"

/*Function: initHunter
  Purpose: initialise the fields of a hunter
  in/out: pointer to a hunter
  in: name of the hunter, int representing what type of evidence does the hunter collect
  */
void initHunter(HunterType* aHunter, char* name, int code){
  strcpy(aHunter->name, name);
  aHunter->roomIn = NULL;
  initEvidenceList(&(aHunter->evidenceCollected));
  aHunter->boredom = BOREDOM_MAX;
  aHunter->fear = FEAR_RATE;
  aHunter->indexCode = code;
  aHunter->deviceClass = code;
  for (int i=0;i<4;++i){
    aHunter->acquiredTypes[i]=0;
  }

}

/*Function: TypesColledcted
  Purpose: checks whether a hunter has collected 3 different types of evidence
  in: array that tracks how many different types of evidence the hunter has collected
  out: integer representing how many different types of evidences the hunter has collected so far*/
int TypesCollected(int* array){
  int counter = 0;
  for (int i=0;i<MAX_HUNTERS;++i){
    counter = counter + array[i];
  }

  return counter;
}

/*Function: typesSum
  Purpose: through finding pattern, i've found that each type of ghost return a different value 
  when adding the index of the enum evidence type required to identify them (e,g banshee: emf, temp, sound = 0+1+3 = 4)
  in: array that tracks how many different types of evidence collected
  out: sum that identify what ghost has been detected*/
int typesSum(int* array){
  int sum = 0;
  for (int i=0;i<MAX_HUNTERS;++i){
    sum = sum + (array[i] * i);
  }

  return sum;
}

/*Function: AddStandardEvidence
  Purpose: create an evidence with standard non ghostly values and add to hunter's collection
  in/out: pointer to a hunter*/
void AddStandardEvidence(HunterType* theHunter){
  EvidenceType* newEvi = (EvidenceType*) calloc(1,sizeof(EvidenceType));

  newEvi->evidenceClass = theHunter->deviceClass;
  switch(theHunter->deviceClass){
    case EMF:
      newEvi->value = randFloat(0.00,4.90);
      break;

    case TEMPERATURE:
      newEvi->value = randFloat(0.00,27.00);
      break;

    case FINGERPRINTS:
      newEvi->value = 0;
      break;

    case SOUND:
      newEvi->value = randFloat(40.00,70.00);
      break;
  }

  appendEvidenceList(&(theHunter->evidenceCollected),newEvi);

}

/*Function: refineEvidence
  Purpose: eliminates all standard non ghostly evidence form a hunter's collection
  in:out: pointer to a hunter*/
void refineEvidence(HunterType* theHunter){
  EvidenceListType newHunterEvidenceList;
  initEvidenceList(&(newHunterEvidenceList));
  EvidenceNodeType* curNode = theHunter->evidenceCollected.head;
  while (curNode!=NULL){
    if (curNode->evidenceData->evidenceClass == theHunter->deviceClass){
      if (isGhostly(curNode->evidenceData)==1){
        appendEvidenceList(&(newHunterEvidenceList),curNode->evidenceData);
      }else{
        free(curNode->evidenceData);
      }
    }else{
      appendEvidenceList(&(newHunterEvidenceList),curNode->evidenceData);
    }
    curNode = curNode->next;
  }

  cleanupEviNodes(&(theHunter->evidenceCollected));
  theHunter->evidenceCollected = newHunterEvidenceList;
  printf("Hunter %s has refined evidence\n\n",theHunter->name);

}

/*Function: getRoomGhostlyEvidence
  Purpose: hunter look through a room's evidence and take only evidence that confirms ghost presence
  in/out: pointer to room, pointer to a hunter*/
void getRoomGhostlyEvidence(RoomType* theRoom,HunterType* theHunter){
  EvidenceListType newRoomEvidenceList;
  initEvidenceList(&(newRoomEvidenceList));
  int detected = 0;
  EvidenceNodeType* curNode = theRoom->evidenceLeft.head;
  while (curNode!=NULL){
    if (curNode->evidenceData->evidenceClass == theHunter->deviceClass){
      if (isGhostly(curNode->evidenceData)==1){
        detected = 1;
        appendEvidenceList(&(theHunter->evidenceCollected),curNode->evidenceData);
      }else{
        appendEvidenceList(&(newRoomEvidenceList),curNode->evidenceData);
      }

    }else{
      appendEvidenceList(&(newRoomEvidenceList),curNode->evidenceData);
    }
    curNode = curNode->next;
  }

  cleanupEviNodes(&(theRoom->evidenceLeft));
  theRoom->evidenceLeft = newRoomEvidenceList;

  if (detected == 1){
    theHunter->boredom = BOREDOM_MAX;
    theHunter->acquiredTypes[theHunter->indexCode] = 1;
    printf("Hunter %s is collected ghostly evidence from room %s\n\n",theHunter->name,theHunter->roomIn->name);
  }else{
    theHunter->boredom -= 1;
  }

}

/*Function: moveHunter
  Purpose: move a hunter from 1 room to another
  in/out: pointer to a room*/
void moveHunter(HunterType* theHunter){
    RoomType* oldRoom = theHunter->roomIn;
    RoomType* newRoom;

    int newRoomIndex = randInt(0,oldRoom->connectedRooms.size);
    newRoom = getChosenRoom(newRoomIndex,&(oldRoom->connectedRooms));

    if (sem_trywait(&(oldRoom->mutex))==0){
      if (sem_trywait(&(newRoom->mutex))==0){
        HunterLeaving(oldRoom,theHunter);
        sem_post(&(oldRoom->mutex));
        HunterEntering(newRoom,theHunter);
        sem_post(&(newRoom->mutex));
        theHunter->roomIn = newRoom;
      }else{
        printf("Error; next room not available,refine evidence instead\n\n");
        refineEvidence(theHunter);
        sem_post(&(oldRoom->mutex));
      }
    }else{
      printf("current room locked, can't leave\n");
    }

}

/*Function: contains
  Purpose: check whether an evidence already exist in an evidence list, to prevent unecessary duplicates
  in: pointer to an evidence linked list, pointer to an evidence
  out: 1/0, boolean to know whether true or false*/
int contains(EvidenceListType* theList, EvidenceType* anEvidence){
  EvidenceNodeType* curNode = theList->head;

  while(curNode!=NULL){
    if (curNode->evidenceData == anEvidence){
      return 1;
    }
    curNode = curNode->next;
  }

  return 0;
}

/*Function: addDuplicateEvidence
  Purpose: solution to doubble frees, when communicating, 
  instead of pointing to an evidence in another hunter's linked list, make a copy of the evidence received
  in/out: pointer to a hunter evidence linked list, pointer to the evidence data to be copied*/
void addDuplicateEvidence(EvidenceListType* eviList,EvidenceType* theEvi){
  EvidenceType* duplicatedEvidence = (EvidenceType*) calloc(1,sizeof(EvidenceType));
  duplicatedEvidence->evidenceClass = theEvi->evidenceClass;
  duplicatedEvidence->value = theEvi->value;
  appendEvidenceList(eviList,duplicatedEvidence);
}

/*Function: communicate
  Purpose: append only ghostly evidence collected by hunter A to the evidence linked list of hunter B
  in/out: pointer to the hunter that is communicating, note:only 1 way communication is done*/
void communicate(HunterType* theHunter){
  printf("Hunter %s is initiating communication\n",theHunter->name);
  if (theHunter->evidenceCollected.head == NULL){
    printf("Hunter %s have no evidence themselves\n",theHunter->name);
    return;
  }
  RoomType* currentRoom = theHunter->roomIn;
  HunterType* otherHunter;
  do{
    int index = randInt(0,MAX_HUNTERS);
    otherHunter = currentRoom->HuntersIn[index];

  }while (otherHunter == theHunter || otherHunter==NULL);

  printf("Hunter %s is communicating with hunter %s",theHunter->name, otherHunter->name);
  int detected = 0;
  EvidenceNodeType* curNode = theHunter->evidenceCollected.head;
  while (curNode!=NULL){
    if (isGhostly(curNode->evidenceData)==1){
      if(contains(&(otherHunter->evidenceCollected),curNode->evidenceData)==0){
        addDuplicateEvidence(&(otherHunter->evidenceCollected),curNode->evidenceData);
        detected = 1;
      }
    }
    curNode = curNode->next;
  }

  if (detected == 1){
    otherHunter->boredom = BOREDOM_MAX;
    otherHunter->acquiredTypes[theHunter->indexCode] = 1;
  }
  printf("Communication done\n\n\n");

}

/*Function: SpawnHunter
  Purpsose: place hunter in the van/starter romm at start of program
  in:pointer to room, pointert to the hunter*/
void SpawnHunter(RoomType* starter, HunterType* theHunter){
  HunterEntering(starter,theHunter);
  theHunter->roomIn = starter;
}

/*Function: HunterActions
  Purpose: simulate the hunter action, that is change room, collect evidence or communicate evidence
  in/out: pointer to a hunter*/
void HunterActions(HunterType* aHunter){
  int action = 0;
  while((aHunter->boredom >0)&&(aHunter->fear <100)&&(TypesCollected(aHunter->acquiredTypes)<3)){
    if (aHunter->roomIn->noHunters > 1){
      //if other hunter is in room then communication is possible
      action= randInt(0,3);
    }else{
      action = randInt(0,2);
    }

    if(aHunter->roomIn->ghostPresent!=NULL){
      //if the ghost is also in the room, then increment fear
      aHunter->fear++;
    }

    if(action==0){
      //change room
      moveHunter(aHunter);
      aHunter->boredom--;

    }else if(action ==1){
      //stay in room and collect evidence
      if (aHunter->roomIn->evidenceLeft.head==NULL){
        //if room has no evidence left by ghost
        AddStandardEvidence(aHunter);
        aHunter->boredom -=1;
        printf("Hunter %s has collected some random evidence from room %s\n\n",aHunter->name,aHunter->roomIn->name);
      }else{
        if (sem_trywait(&(aHunter->roomIn->mutex))==0){
          getRoomGhostlyEvidence(aHunter->roomIn,aHunter);
          sem_post(&(aHunter->roomIn->mutex));
        }else{
          printf("Current room is locked, can't colllect evidence\n\n");
        }
      }


    }else{
      //communicate with other hunter
      printf("Hunter %s is refining their evidence collection before communication\n",aHunter->name);
      //hunter polish their evidence before they share their findings
      refineEvidence(aHunter);
      printf("Hunter %s will start communicating now\n",aHunter->name);
      if (sem_trywait(&(aHunter->roomIn->mutex))==0){
        communicate(aHunter);
        sem_post(&(aHunter->roomIn->mutex));
      }else{
        printf("Current room is locked, Hunter %s could not communicate\n",aHunter->name);
      }

      aHunter->boredom -=1;
    }
  }

  while (1){
    //when hunters leave have their update the room to reflect correct number of hunters in the room
    if (sem_trywait(&(aHunter->roomIn->mutex))==0){
      HunterLeaving(aHunter->roomIn,aHunter);
      sem_post(&(aHunter->roomIn->mutex));
      break;
    }
  }


  if (aHunter->boredom <= 0){
    printf("Hunter %s got bored and left\n\n",aHunter->name);
  }

  if (aHunter->fear>=100){
    printf("Hunter %s got too scared and left\n\n",aHunter->name);
  }

  if (TypesCollected(aHunter->acquiredTypes)>=3){
    printf("Hunter %s got all required evidence. Hence they won!!!\n\n",aHunter->name);
  }

}
