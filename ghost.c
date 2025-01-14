#include "defs.h"

/*Function: initGhost
    Purpose: initialise the fields of a ghost structure
    in/out: pointer to a ghost*/
void initGhost(GhostType* theGhost){
    theGhost->ghostClass = randInt(0,4);
    theGhost->boredomTimer = BOREDOM_MAX;
    theGhost->currentRoom= NULL;
}

/*Function: RespawnGhost
    Purpose: move the ghost to a different room obtained from a room linked list
    in/out: pointer to a ghost
    in: pointer to a room linked list*/
void RespawnGhost(GhostType* theGhost,RoomListType* listRooms){
    RoomType* prevRoom = theGhost->currentRoom;
    int randomRoom;
    RoomType* chosenRoom;
    if (prevRoom!=NULL){
        randomRoom = randInt(0,listRooms->size);
        chosenRoom = getChosenRoom(randomRoom,listRooms);
        prevRoom->ghostPresent = NULL;
        printf("The ghost is leaving room %s \t\t",prevRoom->name);
    }else{
        randomRoom = randInt(1,listRooms->size);
        chosenRoom = getChosenRoom(randomRoom,listRooms);
        theGhost->currentRoom = chosenRoom;
        chosenRoom->ghostPresent = theGhost;
        printf("The ghost has spawned to room %s\n\n\n\n",theGhost->currentRoom->name);
        return;
    }

    if (sem_trywait(&(chosenRoom->mutex))==0){
        theGhost->currentRoom = chosenRoom;
        chosenRoom->ghostPresent = theGhost;
        printf("The ghost has moved to room %s\n",theGhost->currentRoom->name);
        sem_post(&(prevRoom->mutex));
        sem_post(&(chosenRoom->mutex));
    }else{
        printf("Error: Can't move right now\n");
        prevRoom->ghostPresent = theGhost;
        sem_post(&(prevRoom->mutex));
    }


}

/*Function: SpawnGhost
    Purpose: place the ghost in random room, only called at the start of program
    in: pointer to the building*/
void SpawnGhost(BuildingType* b){
    printf("Spawning ghost......\n\n");
    RespawnGhost(b->theGhost, &(b->allRooms));
    printf("Starting ghost simulation.....\n\n");
}

/*Function: GhostAction
    Purpose: simulate the actions of the ghost, actions are chosen randomly
    in/out: pointer to the ghost*/
void GhostActions(GhostType* theGhost){
    int action = 1;

    while (theGhost->boredomTimer>0){
        RoomType* currentRoom = theGhost->currentRoom;
        if (currentRoom->noHunters>0){
            //if hunter is in room, room can't move room
            action = randInt(1,3);
        }else{
            //if no hunter is in room, then ghost gets more bored
            action = randInt(0,3);
            theGhost->boredomTimer--;
        }


        if(action==0){
            //change room
            if (sem_trywait(&(theGhost->currentRoom->mutex))==0){
                RespawnGhost(theGhost,&(theGhost->currentRoom->connectedRooms));
            }else{
                printf("Ghost tried to move but current room is locked");
            }

        }else if(action==1){
            //stay in room but do nothing
            printf("Ghost chose to do nothing\n");

        }else{
            //stay in room and leave evidence, either real ghostly or distraction evidence
            if (sem_trywait(&(theGhost->currentRoom->mutex))==0){
                leaveEvidence(theGhost->currentRoom, theGhost->ghostClass);
                printf("Ghost has left evidence in room %s\n",theGhost->currentRoom->name);
                sem_post(&(theGhost->currentRoom->mutex));
            }else{
                printf("Ghost tried to leave evidence but the room is locked\n");
            }
        }
    }

    printf("\n Ghost is bored, exiting building\n\n");
}
