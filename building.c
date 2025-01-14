#include "defs.h"

/*Function: loadArea
    Purpose: contruct the building and the rooms and connect the rooms
    in\out: pointer to the building
    in: name of the building*/
void loadArea(BuildingType* b, char* name){
    initBuilding(b,name);
    populateBuilding(b);
}

/*Function: initBuilding
    Purpose: initialise the fields of the building
    in/out: pointer to the building
    in: name of the building*/
void initBuilding(BuildingType* b, char* name){
    strcpy(b->name, name);
    initRoomList(&(b->allRooms));
    for(int j=0;j<MAX_HUNTERS;j++){
      b->allHunters[j] = NULL;
    }
}

/*Function: populateBuilding
    Purpose: build all the rooms in the building and connect them where needed
    in/out: pointer to the building*/
void populateBuilding(BuildingType* b){
    RoomType* van = (RoomType*) calloc(1, sizeof(RoomType));
    initRoom(van, "Van");
    appendRoomList(&(b->allRooms),van);

    RoomType* hallway = (RoomType*) calloc(1, sizeof(RoomType));
    initRoom(hallway, "Hallway");;
    appendRoomList(&(b->allRooms),hallway);

    RoomType* master_bedroom = (RoomType*) calloc(1, sizeof(RoomType));
    initRoom(master_bedroom, "Master Bedroom");
    appendRoomList(&(b->allRooms),master_bedroom);

    RoomType* boys_bedroom = (RoomType*) calloc(1, sizeof(RoomType));
    initRoom(boys_bedroom, "Boy's Bedroom");
    appendRoomList(&(b->allRooms),boys_bedroom);

    RoomType* bathroom = (RoomType*) calloc(1, sizeof(RoomType));
    initRoom(bathroom, "Bathroom");
    appendRoomList(&(b->allRooms),bathroom);;

    RoomType* basement_hallway = (RoomType*) calloc(1, sizeof(RoomType));
    initRoom(basement_hallway, "Basement Hallway");
    appendRoomList(&(b->allRooms),basement_hallway);

    RoomType* right_storage_room = (RoomType*) calloc(1, sizeof(RoomType));
    initRoom(right_storage_room, "Right Storage Room");
    appendRoomList(&(b->allRooms),right_storage_room);

    RoomType* left_storage_room = (RoomType*) calloc(1, sizeof(RoomType));
    initRoom(left_storage_room, "Left Storage Room");
    appendRoomList(&(b->allRooms),left_storage_room);

    RoomType* kitchen = (RoomType*) calloc(1, sizeof(RoomType));
    initRoom(kitchen, "Kitchen");
    appendRoomList(&(b->allRooms),kitchen);

    RoomType* living_room = (RoomType*) calloc(1, sizeof(RoomType));
    initRoom(living_room, "Living Room");
    appendRoomList(&(b->allRooms),living_room);

    RoomType* garage = (RoomType*) calloc(1, sizeof(RoomType));
    initRoom(garage, "Garage");
    appendRoomList(&(b->allRooms),garage);

    RoomType* utility_room = (RoomType*) calloc(1, sizeof(RoomType));
    initRoom(utility_room, "Utility Room");
    appendRoomList(&(b->allRooms),utility_room);

    connectRooms(van, hallway);
    connectRooms(hallway,master_bedroom);
    connectRooms(hallway,boys_bedroom);
    connectRooms(hallway,bathroom);
    connectRooms(hallway,kitchen);
    connectRooms(hallway,basement_hallway);
    connectRooms(basement_hallway,right_storage_room);
    connectRooms(basement_hallway,left_storage_room);
    connectRooms(kitchen,living_room);
    connectRooms(kitchen,garage);
    connectRooms(garage,utility_room);
}

/*Function: BuildingView
    Purpose: provide a summary of the building name and rooms and shows what rooms are connected to what other rooms
    in/out: pointer to the building*/
void BuildingView(BuildingType* b){

    printf("Overview of %s building\nHas the following rooms\n",b->name);

    RoomNodeType* curNode;
    curNode = b->allRooms.head;

    while(curNode!=NULL){
        printf("%s :",curNode->roomData->name);
        RoomNodeType* innerCon = curNode->roomData->connectedRooms.head;
        while (innerCon->next!=NULL){
            printf(" %s,",innerCon->roomData->name);
            innerCon = innerCon->next;
        }
        printf("%s ",innerCon->roomData->name);
        printf("\n\n");
        curNode = curNode->next;
    }
    printf("\n\n\n\n");
}

/*Function: ViewBuildingAfter
    Purpose: provides a summary of all evidence left in rooms(i.e not collected by hunters) and collected by hunters
    in: pointer to the building */
void ViewBuildingAfter(BuildingType* b){
    printf("%s building after ghost hunt\n",b->name);
    printf("The following rooms have the following evidence left\n");
    RoomNodeType* curNode = b->allRooms.head;

    while (curNode!=NULL){
        printf("%s : \n",curNode->roomData->name);
        viewRoomEvidence(curNode->roomData);
        printf("\n\n");
        curNode = curNode->next;
    }

    for (int i=0;i<MAX_HUNTERS;++i){
        printf("Hunter %s has the following evidence\n",b->allHunters[i]->name);
        displayEvidenceList(&(b->allHunters[i]->evidenceCollected));
        printf("\n\n");
    }
}

/*Function: cleanupBuilding
    Purpose: clean up all dynamically allocated data related to the building
    in/out: pointer to the building*/
void cleanupBuilding(BuildingType* b){
    RoomNodeType* curNode = b->allRooms.head;

    while(curNode!=NULL){
        cleanupRoomNodes(&(curNode->roomData->connectedRooms));

        cleanupEviData(&(curNode->roomData->evidenceLeft));

        cleanupEviNodes(&(curNode->roomData->evidenceLeft));

        curNode = curNode->next;
    }

    cleanupRoomData(&(b->allRooms));
    cleanupRoomNodes(&(b->allRooms));


    for (int i = 0;i<MAX_HUNTERS;i++){
        cleanupEviHunter(b->allHunters[i]);
    }

}
