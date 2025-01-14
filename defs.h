#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_STR            64
#define FEAR_RATE           1
#define MAX_FEAR          100
#define MAX_HUNTERS         4
#define USLEEP_TIME     50000
#define BOREDOM_MAX        99

// You may rename these types if you wish
typedef enum { EMF, TEMPERATURE, FINGERPRINTS, SOUND } EvidenceClassType;
typedef enum { POLTERGEIST, BANSHEE, BULLIES, PHANTOM } GhostClassType;

int randInt(int, int);          // Generates a pseudorandom integer between the parameters
float randFloat(float, float);

typedef struct RoomNode{
    struct RoomType* roomData;
    struct RoomNode* next;
}RoomNodeType;

typedef struct RoomListType{
    RoomNodeType* head;
    RoomNodeType* tail;
    int size;
}RoomListType;

typedef struct{
    EvidenceClassType evidenceClass;
    float value;
}EvidenceType;

typedef struct EvNode{
    EvidenceType* evidenceData;
    struct EvNode* next;
}EvidenceNodeType;

typedef struct{
    EvidenceNodeType* head;
    EvidenceNodeType* tail;
}EvidenceListType;

typedef struct HunterType{
    char name[MAX_STR];
    struct RoomType* roomIn;
    EvidenceListType evidenceCollected;
    EvidenceClassType deviceClass;
    int boredom;
    int fear;
    int indexCode;
    int acquiredTypes[4];
}HunterType;

typedef struct RoomType{
    char name[25];
    RoomListType connectedRooms;
    EvidenceListType evidenceLeft;
    struct GhostType* ghostPresent;
    HunterType* HuntersIn[MAX_HUNTERS];
    int noHunters;
    sem_t mutex;
}RoomType;

typedef struct GhostType{
    GhostClassType ghostClass;
    int boredomTimer;
    RoomType* currentRoom;
}GhostType;

typedef struct{
    char name[MAX_STR];
    RoomListType allRooms;
    GhostType* theGhost;
    HunterType* allHunters[MAX_HUNTERS];
}BuildingType;

void initRoom(RoomType*, char*);
void initRoomList(RoomListType*);
void appendRoomList(RoomListType*, RoomType*);
void connectRooms(RoomType*, RoomType*);
RoomType* getChosenRoom (int, RoomListType*);
void HunterEntering(RoomType*, HunterType*);
void HunterLeaving(RoomType*,HunterType*);
void cleanupRoomData(RoomListType*);
void cleanupRoomNodes(RoomListType*);

void initBuilding(BuildingType*,char*);
void populateBuilding(BuildingType*);
void BuildingView(BuildingType*);
void loadArea(BuildingType*, char*);
void ViewBuildingAfter(BuildingType*);
void cleanupBuilding(BuildingType*);

void initEvidenceList(EvidenceListType*);
void createGhostlyEvidence(GhostClassType, EvidenceType*);
void leaveEvidence(RoomType*,GhostClassType);
int isGhostly(EvidenceType*);
void appendEvidenceList(EvidenceListType*, EvidenceType*);
void addDuplicateEvidence(EvidenceListType*,EvidenceType*);
void displayAnEvidence(EvidenceType*);
void displayEvidenceList(EvidenceListType*);
void viewRoomEvidence(RoomType*);
void cleanupEviNodes(EvidenceListType*);
void cleanupEviData(EvidenceListType*);
void cleanupEviHunter(HunterType*);

void initGhost(GhostType*);
void MoveGhost(GhostType*,RoomListType*);
void SpawnGhost(BuildingType*);
void GhostActions(GhostType*);

void initHunter(HunterType*,char*,int);
int TypesCollected(int*);
void AddStandardEvidence(HunterType*);
void getRoomGhostlyEvidence(RoomType*,HunterType*);
void refineEvidence(HunterType*);
int contains(EvidenceListType*, EvidenceType*);
void moveHunter(HunterType*);
void SpawnHunter(RoomType*, HunterType*);
void communicate(HunterType*);
void HunterActions(HunterType*);
int typesSum(int*);
