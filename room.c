#include "defs.h"

/*Function: initRoom
    Purpose: initialse the fields of the romm structure 
    in/out: pointer to a roomtype structure*/
void initRoom(RoomType* aRoom, char* name){
    strcpy(aRoom->name,name);
    initRoomList(&(aRoom->connectedRooms));
    initEvidenceList(&(aRoom->evidenceLeft));
    aRoom->ghostPresent = NULL;
    for (int i=0;i<MAX_HUNTERS;i++){
      aRoom->HuntersIn[i] = NULL;
    }
    sem_init(&(aRoom->mutex),0,1);
}

/*Function: initRoomList
  Purpose: initialise the field of a room linked list
  in/out: pointer to a room linked list/rommlisttype structure*/
void initRoomList(RoomListType* roomList){
    roomList->head = NULL;
    roomList->tail = NULL;
    roomList->size = 0;
}

/*Function: appendRoomList
  Purpose: add a room to a room linked list
  in/out: pointer to a room linked list/roomlisttype structure , pointer to a room*/
void appendRoomList(RoomListType* roomList, RoomType* aRoom){
    RoomNodeType* newNode = (RoomNodeType*) calloc(1, sizeof(RoomNodeType));
    newNode->roomData = aRoom;
    newNode-> next = NULL;

    if (roomList->head ==NULL){
        roomList->head = newNode;
        roomList->tail = newNode;
    }else{
        roomList->tail->next = newNode;
        roomList->tail = newNode;
    }

    roomList->size = roomList->size + 1;
}

/*Function: connectRooms
  Purpose: connect/link 2 rooms together
  in/out: 2 room pointers which each respectively pointing to a room*/
void connectRooms(RoomType* roomA, RoomType* roomB){
    appendRoomList(&(roomA->connectedRooms), roomB);
    appendRoomList(&(roomB->connectedRooms),roomA);
}

/*Function: getChosenRoom
  Purpose: get the room at index x in a room linked list
  in: room linked list and index of required room
  out: pointer to the room saved at index x in the room linked list*/
RoomType* getChosenRoom (int index, RoomListType* theList){
    if (index == 0){
        return theList->head->roomData;
    }else if (index == theList->size - 1){
        return theList->tail->roomData;
    }else{
        RoomNodeType* curNode = theList->head;
        for (int j =0;j<index;j++){
            curNode = curNode->next;
        }
        return curNode->roomData;
    }
}


/*Function: HunterEntering
  Purpose: tracks how many hunters are in the room
  in/out: pointer to the room
  in:pointer to the hunter entering the room*/
void HunterEntering(RoomType* theRoom, HunterType* theHunter){
    int index = theHunter->indexCode;
    theRoom->HuntersIn[index] = theHunter;
    theRoom->noHunters +=1;
    printf("Hunter %s is entering room %s\n\n\n",theHunter->name, theRoom->name );

}

/*Function: HunterLeaving
  Purpose: tracks how many hunters are in the room
  in/out: pointer to the room, 
  in:pointer to the hunter leaving the room*/
void HunterLeaving(RoomType* theRoom, HunterType* theHunter){
  int index = theHunter->indexCode;
  theRoom->HuntersIn[index] = NULL;
  theRoom->noHunters -=1;
  printf("Hunter %s is leaving room %s\n",theHunter->name,theRoom->name);

}

/*Function: cleanupRoomData
  Purpose: free the room pointers, previously calloc-ed
  in/out: pointer to a room linked list*/
void cleanupRoomData(RoomListType* roomList){
    RoomNodeType* curNode;
    curNode = roomList->head;

    while(curNode!=NULL){
        free(curNode->roomData);
        curNode= curNode->next;
    }
}


/*Function: cleanupRoomNodes
  Purpose: free the nodes, previously calloc-ed, of a romm linked list
  in/out: pointer to a room linked list*/
void cleanupRoomNodes(RoomListType* roomList){
    if (roomList->head==NULL){
        return;
    }

    RoomNodeType* curNode;
    RoomNodeType* nextNode;

    curNode = roomList->head;
    nextNode = roomList->head->next;

    while(nextNode!=NULL){
        free(curNode);
        curNode = nextNode;
        nextNode = nextNode->next;
    }

    free(curNode);
}

/*Function: viewRoomEvidence
  Purpose: display all the evidence currently in a room
  in:pointer to a room*/
void viewRoomEvidence(RoomType* theRoom){
    displayEvidenceList(&(theRoom->evidenceLeft));
}
