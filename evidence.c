#include "defs.h"

/*Function: initEvidenceList
    Purpose: initialise the fields of a evidence linked list
    in/out: pointer to an evidence linked list*/
void initEvidenceList(EvidenceListType* evidenceList){
    evidenceList->head=NULL;
    evidenceList->tail=NULL;
}

/*Function: appendEvidenceList
    Purpose: add a new evidence to an evidence linked list
    in: pointer to an evidence
    in/out: pointer to an evidence linked list*/
void appendEvidenceList(EvidenceListType* eviList, EvidenceType* newEv){
    EvidenceNodeType* newNode = (EvidenceNodeType*) calloc(1,sizeof(EvidenceNodeType));
    newNode->evidenceData = newEv;;
    newNode->next = NULL;

    if(eviList->head==NULL){
        eviList->head = newNode;
        eviList->tail = newNode;
    }else{
        eviList->tail->next = newNode;
        eviList->tail = newNode;
    }
}

/*Function: leaveEvidence
  Purpose: called by the ghost, used to leave ghostly evidence in a room
  in: class of the ghost
  in/out: room to which evidence is being left by ghost*/
void leaveEvidence(RoomType* aRoom, GhostClassType class){
    EvidenceType* newEvidence = (EvidenceType*) calloc(1,sizeof(EvidenceType));

    switch(class){
        case POLTERGEIST:
            do{
                newEvidence->evidenceClass = randInt(0,4);

            }while(newEvidence->evidenceClass==3);
            break;

        case BANSHEE:
            do{
                newEvidence->evidenceClass = randInt(0,4);

            }while(newEvidence->evidenceClass==2);
            break;

        case BULLIES:
             do{
                newEvidence->evidenceClass = randInt(0,4);

            }while(newEvidence->evidenceClass==1);
            break;

        case PHANTOM:
             do{
                newEvidence->evidenceClass = randInt(0,4);

            }while(newEvidence->evidenceClass==0);
            break;

    }


    switch(newEvidence->evidenceClass){

        case EMF:
            newEvidence->value = randFloat(4.70,5.00);
            break;

        case TEMPERATURE:
            newEvidence->value = randFloat(-10.00,1.00);
            break;

        case FINGERPRINTS:
            newEvidence->value = 1;
            break;

        case SOUND:
            newEvidence->value = randFloat(65.00,75.00);
            break;

    }
    appendEvidenceList(&(aRoom->evidenceLeft),newEvidence);
}

/*Function: isGhostly
    Purpose: determine whether an evidence is ghostly or not, through threshold of standard evidence
    in: pointer to an evidence
    out: 1/0, boolean to determine if ghostly or not*/
int isGhostly(EvidenceType* anEvidence){
    float threshold;

    switch(anEvidence->evidenceClass){
        case EMF:
            threshold = 4.90;
            break;

        case TEMPERATURE:
            threshold = 0;
            break;

        case FINGERPRINTS:
            threshold = 0;
            break;

        case SOUND:
            threshold = 70;
            break;
  }

  if(anEvidence->evidenceClass == TEMPERATURE){
    if (anEvidence->value < threshold){
        return 1;
    }else{
        return 0;
    }
  }else{
    if (anEvidence->value > threshold){
        return 1;
    }else{
        return 0;
    }
  }


}

/*Function: displayAnEvidence
    Purpose: display the class and value of an evidence
    in: pointer to an evidence*/
void displayAnEvidence(EvidenceType* anEvidence){
    char evClass[15];

    switch(anEvidence->evidenceClass){
        case EMF:
            strcpy(evClass, "EMF");
            break;

        case TEMPERATURE:
            strcpy(evClass, "TEMPERATURE");
            break;

        case FINGERPRINTS:
            strcpy(evClass, "FINGERPRINTS");
            break;

        case SOUND:
            strcpy(evClass, "SOUND");
            break;
    }

    if (anEvidence->value < 0){
      printf("\t\t %15s : %06.2f\n",evClass,anEvidence->value);
    }else{
      printf("\t\t %15s :  %05.2f\n",evClass,anEvidence->value);
    }
}

/*Function: displayEvidenceList
    Purpose: display all contents of an evidence linked list
    in: pointer to an evidence linked list*/
void displayEvidenceList(EvidenceListType* evList){

    if (evList->head == NULL){
      printf("\t\t\t NIL\n");
    }
    EvidenceNodeType* curNode = evList->head;

    while (curNode!=NULL){
        displayAnEvidence(curNode->evidenceData);
        curNode = curNode->next;
    }
}

/*Function: cleanupEviNodes
    Purpose: free all previously allocated nodes of an evidence linke list
    in\out: pointer to an evidence linked list*/
void cleanupEviNodes(EvidenceListType* evidenceList){

    if (evidenceList->head==NULL){
        return;
    }

    EvidenceNodeType* curNode = evidenceList->head;
    EvidenceNodeType* nextNode = evidenceList->head->next;

    while(nextNode!=NULL){
        free(curNode);
        curNode = nextNode;
        nextNode = nextNode->next;
    }
    free(curNode);
}

/*Function: cleanupEviData
    Purpose: free all the evidence data/pointers previously calloc-ed from an evidence linked list
    in/out: pointer to an evidence linked list*/
void cleanupEviData(EvidenceListType* evidenceList){
    EvidenceNodeType* curNode;
    curNode = evidenceList->head;

    while(curNode!=NULL){
        free(curNode->evidenceData);
        curNode= curNode->next;
    }

}

/*Function: cleanupEviHunter
    Purpose: free all  data attached to a hunter that was previously calloc-ed
    in/out: pointer to a hunter*/
void cleanupEviHunter(HunterType* theHunter){
    cleanupEviData(&(theHunter->evidenceCollected));
    cleanupEviNodes(&(theHunter->evidenceCollected));

}
