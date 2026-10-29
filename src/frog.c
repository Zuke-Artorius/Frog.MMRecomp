#include "frog.h"

PlayerModelManagerHandle gFrogHumanHandle;

PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS void registerBase() {
    PlayerModelManagerHandle frogHandle = PLAYERMODELMANAGER_REGISTER_MODEL("pmm_ctfrog_pack", PMM_MODEL_TYPE_MODEL_PACK);
        PlayerModelManager_setAuthor(frogHandle, "Zuke");
        PlayerModelManager_setDisplayName(frogHandle, "Frog");

    PlayerModelManagerHandle frogHuman = PLAYERMODELMANAGER_REGISTER_MODEL("pmm_ctfrog_human", PMM_MODEL_TYPE_CHILD);
        PlayerModelManager_setAuthor(frogHuman, "Zuke");
        PlayerModelManager_setDisplayName(frogHuman, "Frog");
        PlayerModelManager_setSkeleton(frogHuman, &gFrogHumanSkel);
        PlayerModelManager_addHandleToPack(frogHandle, frogHuman);
        gFrogHumanHandle = frogHuman;
}

bool isFrog;

RECOMP_HOOK("Player_Draw") void on_Player_Draw(Actor* thisx, PlayState* play) {
    Player* this = (Player*)thisx;
    isFrog = PlayerModelManager_Actor_isModelApplied(thisx, gFrogHumanHandle);

    if (this->transformation == PLAYER_FORM_HUMAN && isFrog) {
        this->ageProperties = &gFrogAgeProperties;
        D_801C08FC[this->transformation] = frogYIntersectArr[this->transformation];
    }
}