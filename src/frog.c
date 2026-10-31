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
        PlayerModelManager_setDisplayList(frogHuman, PMM_DL_LFIST, gFrogHumanFistSkel_bone015_gLinkHumanLeftHandLimb_mesh_layer_Opaque);
        PlayerModelManager_setDisplayList(frogHuman, PMM_DL_RFIST, gFrogHumanFistSkel_bone018_gLinkHumanRightHandLimb_mesh_layer_Opaque);
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

Gfx* frogHead = gCull;

RECOMP_HOOK("Player_DrawImpl") void on_Player_DrawImpl(PlayState* play, void** skeleton, Vec3s* jointTable, s32 dListCount, s32 lod,
                     PlayerTransformation playerForm, s32 boots, s32 face, OverrideLimbDrawFlex overrideLimbDraw,
                     PostLimbDrawFlex postLimbDraw, Actor* actor) {
    s32 eyeIndex = GET_EYE_INDEX_FROM_JOINT_TABLE(jointTable);
    s32 mouthIndex = GET_MOUTH_INDEX_FROM_JOINT_TABLE(jointTable);

    if (mouthIndex < 0) {
        mouthIndex = sPlayerFaces[face].mouthIndex;
    }

    if (isFrog && playerForm == PLAYER_FORM_HUMAN) {

        switch (mouthIndex) {
            case PLAYER_MOUTH_CLOSED:
                frogHead = gFrogMouthClosed;
                break;
            case PLAYER_MOUTH_TEETH:
                frogHead = gFrogMouthClosed;
                break;
            case PLAYER_MOUTH_ANGRY:
                frogHead = gFrogMouthOpen;
                break;
            case PLAYER_MOUTH_HAPPY:
                frogHead = gFrogMouthOpen;
                break;
            default:
                frogHead = gCull;
                break;
        }
    } else {
        frogHead = gCull;
    }
}

u8 gPushedMatrixHead;

RECOMP_HOOK("Player_PostLimbDrawGameplay") void on_Player_PostLimbDrawGameplay(PlayState* play, s32 limbIndex, Gfx** dList1, Gfx** dList2, Vec3s* rot, Actor* actor) {
    Player* player = (Player*)actor;

    if (isFrog && limbIndex == PLAYER_LIMB_HEAD) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();
        gPushedMatrixHead++;
        Matrix_Translate(0.f, 0.F, 0.f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, frogHead);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}

RECOMP_HOOK_RETURN("Player_PostLimbDrawGameplay") void return_Player_PostLimbDrawGameplay(void) {
    while (gPushedMatrixHead) {
        Matrix_Pop();
        gPushedMatrixHead--;
    }
    gPushedMatrixHead = 0;
}