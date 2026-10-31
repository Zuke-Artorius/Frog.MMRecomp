#include "frog.h"

PlayerModelManagerHandle gFrogHumanHandle;

PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS void registerBase() {
    PlayerModelManagerHandle frogHandle = PLAYERMODELMANAGER_REGISTER_MODEL("pmm_ctfrog_pack", PMM_MODEL_TYPE_MODEL_PACK);
        PlayerModelManager_setAuthor(frogHandle, "Zuke");
        PlayerModelManager_setDisplayName(frogHandle, "Frog");

    PlayerModelManagerHandle frogHuman = PLAYERMODELMANAGER_REGISTER_MODEL("pmm_ctfrog_human", PMM_MODEL_TYPE_CHILD);
        PlayerModelManager_setAuthor(frogHuman, "Zuke");
        PlayerModelManager_setDisplayName(frogHuman, "Frog");
        PlayerModelManager_setEyesTextures(frogHuman, gFrogEyelids);
        PlayerModelManager_setSkeleton(frogHuman, &gFrogHumanSkel);
        PlayerModelManager_setDisplayList(frogHuman, PMM_DL_LFIST, gFrogHumanFistSkel_bone015_gLinkHumanLeftHandLimb_mesh_layer_Opaque);
        PlayerModelManager_setDisplayList(frogHuman, PMM_DL_RFIST, gFrogHumanFistSkel_bone018_gLinkHumanRightHandLimb_mesh_layer_Opaque);
        PlayerModelManager_addHandleToPack(frogHandle, frogHuman);
        gFrogHumanHandle = frogHuman;

    PlayerModelManagerHandle frogShieldHero = PLAYERMODELMANAGER_REGISTER_MODEL("pmm_ctfrog_shield_hero", PMM_MODEL_TYPE_SHIELD_HERO);
        PlayerModelManager_setAuthor(frogShieldHero, "Zuke");
        PlayerModelManager_setDisplayName(frogShieldHero, "Frog");
        PlayerModelManager_setDisplayList(frogShieldHero, PMM_DL_SHIELD_HERO, gCull);
}

bool isFrog;

RECOMP_HOOK("Player_Draw") void on_Player_Draw(Actor* thisx, PlayState* play) {
    Player* this = (Player*)thisx;
    isFrog = PlayerModelManager_Actor_isModelApplied(thisx, gFrogHumanHandle);

    if (isFrog && this->transformation == PLAYER_FORM_HUMAN) {
        this->ageProperties = &gFrogAgeProperties;
        D_801C08FC[this->transformation] = FrogYIntersectArr[this->transformation];
    }
}

Gfx* gFrogMouth = gCull;

RECOMP_HOOK("Player_DrawImpl") void on_Player_DrawImpl(PlayState* play, void** skeleton, Vec3s* jointTable, s32 dListCount, s32 lod,
                     PlayerTransformation playerForm, s32 boots, s32 face, OverrideLimbDrawFlex overrideLimbDraw,
                     PostLimbDrawFlex postLimbDraw, Actor* actor) {
    s32 eyeIndex = GET_EYE_INDEX_FROM_JOINT_TABLE(jointTable);
    s32 mouthIndex = GET_MOUTH_INDEX_FROM_JOINT_TABLE(jointTable);

    if (eyeIndex < 0) {
        eyeIndex = sPlayerFaces[face].eyeIndex;
    }

    if (mouthIndex < 0) {
        mouthIndex = sPlayerFaces[face].mouthIndex;
    }

    if (isFrog && playerForm == PLAYER_FORM_HUMAN) {
        gFrogMouth = FrogMouthDL[mouthIndex];
        OPEN_DISPS(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x0A, Lib_SegmentedToVirtual(gFrogEyes[eyeIndex]));
        CLOSE_DISPS(play->state.gfxCtx);
    } else {
        gFrogMouth = gCull;
    }

}

u8 gPushedMatrixHead;

RECOMP_HOOK("Player_PostLimbDrawGameplay") void on_Player_PostLimbDrawGameplay(PlayState* play, s32 limbIndex, Gfx** dList1, Gfx** dList2, Vec3s* rot, Actor* actor) {
    Player* player = (Player*)actor;

    if (player->stateFlags1 & PLAYER_STATE1_100000) {
        Vec3f temp;
        SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &player->actor.focus.pos, &temp);
        if (temp.z < 5.0f) {
            gFrogMouth = gCull;
            }
    }

    if (isFrog && limbIndex == PLAYER_LIMB_HEAD) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();
        gPushedMatrixHead++;
        Matrix_Translate(0.f, 0.F, 0.f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gFrogMouth);
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