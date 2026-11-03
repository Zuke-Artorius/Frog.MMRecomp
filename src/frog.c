#include "frog.h"

PlayerModelManagerHandle gFrogHumanHandle;
PlayerModelManagerHandle gFrogShieldHeroHandle;
PlayerModelManagerHandle gFrogShieldMirrorHandle;
PlayerModelManagerHandle gFrogSwordGildedHandle;

PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS void registerBase() {
    PlayerModelManagerHandle frogHandle = PLAYERMODELMANAGER_REGISTER_MODEL("pmm_ctfrog_pack", PMM_MODEL_TYPE_MODEL_PACK);
        PlayerModelManager_setAuthor(frogHandle, "Zuke");
        PlayerModelManager_setDisplayName(frogHandle, "Frog");

    PlayerModelManagerHandle frogHuman = PLAYERMODELMANAGER_REGISTER_MODEL("pmm_ctfrog_human", PMM_MODEL_TYPE_CHILD);
        PlayerModelManager_setAuthor(frogHuman, "Zuke");
        PlayerModelManager_setDisplayName(frogHuman, "Frog");
        PlayerModelManager_setEyesTextures(frogHuman, gFrogEyelids);
        PlayerModelManager_setSkeleton(frogHuman, &gFrogHumanSkel);
        PlayerModelManager_setDisplayList(frogHuman, PMM_DL_RFIST, gFrogHumanFistSkel_bone018_gLinkHumanRightHandLimb_mesh_layer_Opaque);
        PlayerModelManager_setDisplayList(frogHuman, PMM_DL_LFIST, gFrogHumanFistSkel_bone015_gLinkHumanLeftHandLimb_mesh_layer_Opaque);
        PlayerModelManager_setDisplayList(frogHuman, PMM_DL_LHAND_BOTTLE, gFrogHumanBottleSkel_bone015_gLinkHumanLeftHandLimb_mesh_layer_Opaque);
        PlayerModelManager_setCallback(frogHuman, Frog_EventHandler_Human, NULL);
        PlayerModelManager_addHandleToPack(frogHandle, frogHuman);
        gFrogHumanHandle = frogHuman;

    PlayerModelManagerHandle frogShieldHero = PLAYERMODELMANAGER_REGISTER_MODEL("pmm_ctfrog_shield_hero", PMM_MODEL_TYPE_SHIELD_HERO);
        PlayerModelManager_setAuthor(frogShieldHero, "Zuke");
        PlayerModelManager_setDisplayName(frogShieldHero, "Frog");
        PlayerModelManager_setDisplayList(frogShieldHero, PMM_DL_SHIELD_HERO, gCull);
        PlayerModelManager_setCallback(frogShieldHero, Frog_EventHandler_GetItem, (void*)(uintptr_t)FROG_GID_SHIELD_HERO);
        PlayerModelManager_addHandleToPack(frogHandle, frogShieldHero);
        gFrogShieldHeroHandle = frogShieldHero;
        

    PlayerModelManagerHandle frogShieldMirror = PLAYERMODELMANAGER_REGISTER_MODEL("pmm_ctfrog_shield_mirror", PMM_MODEL_TYPE_SHIELD_MIRROR);
        PlayerModelManager_setAuthor(frogShieldMirror, "Zuke");
        PlayerModelManager_setDisplayName(frogShieldMirror, "Frog");
        PlayerModelManager_setDisplayList(frogShieldMirror, PMM_DL_SHIELD_MIRROR, gCull);
        PlayerModelManager_setCallback(frogShieldMirror, Frog_EventHandler_GetItem, (void*)(uintptr_t)FROG_GID_SHIELD_MIRROR);
        PlayerModelManager_addHandleToPack(frogHandle, frogShieldMirror);
        gFrogShieldMirrorHandle = frogShieldMirror;
        
        PlayerModelManagerHandle frogSwordGilded = PLAYERMODELMANAGER_REGISTER_MODEL("pmm_ctfrog_sword_gilded", PMM_MODEL_TYPE_SWORD_GILDED);
        PlayerModelManager_setAuthor(frogSwordGilded, "Zuke");
        PlayerModelManager_setDisplayName(frogSwordGilded, "Frog");
        PlayerModelManager_setDisplayList(frogSwordGilded, PMM_DL_SWORD_GILDED_HILT, gFrogHilt);
        PlayerModelManager_setDisplayList(frogSwordGilded, PMM_DL_SWORD_GILDED_BLADE, gFrogBlade);
        PlayerModelManager_setDisplayList(frogSwordGilded, PMM_DL_SWORD_GILDED_SHEATH, gFrogScabbard);
        PlayerModelManager_setCallback(frogSwordGilded, Frog_EventHandler_GetItem, (void*)(uintptr_t)FROG_GID_SWORD_GILDED);
        PlayerModelManager_addHandleToPack(frogHandle, frogSwordGilded);
        gFrogSwordGildedHandle = frogSwordGilded;
}

RECOMP_HOOK("Player_Draw") void on_Player_Draw(Actor* thisx, PlayState* play) {
    if (!isFrog) {
        return;
    }
    Player* this = (Player*)thisx;
    
    if (this->transformation == PLAYER_FORM_HUMAN) {
        this->ageProperties = &gFrogAgeProperties;
        D_801C08FC[this->transformation] = FrogYIntersectArr[this->transformation];
    }
}

Gfx* gFrogMouth = gCull;
Gfx* gFrogRHand = gCull;

RECOMP_HOOK("Player_DrawImpl") void on_Player_DrawImpl(PlayState* play, void** skeleton, Vec3s* jointTable, s32 dListCount, s32 lod,
                     PlayerTransformation playerForm, s32 boots, s32 face, OverrideLimbDrawFlex overrideLimbDraw,
                     PostLimbDrawFlex postLimbDraw, Actor* actor) {
    if (!isFrog) {
        return;
    }
    s32 eyeIndex = GET_EYE_INDEX_FROM_JOINT_TABLE(jointTable);
    s32 mouthIndex = GET_MOUTH_INDEX_FROM_JOINT_TABLE(jointTable);

    if (eyeIndex < 0) {
        eyeIndex = sPlayerFaces[face].eyeIndex;
    }

    if (mouthIndex < 0) {
        mouthIndex = sPlayerFaces[face].mouthIndex;
    }

    if (playerForm == PLAYER_FORM_HUMAN) {
        gFrogMouth = FrogMouthDL[mouthIndex];
        OPEN_DISPS(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x0A, Lib_SegmentedToVirtual(gFrogEyes[eyeIndex]));
        CLOSE_DISPS(play->state.gfxCtx);
    } else {
        gFrogMouth = gCull;
    }
}

u8 gPushedMatrix;

RECOMP_HOOK("Player_PostLimbDrawGameplay") void on_Player_PostLimbDrawGameplay(PlayState* play, s32 limbIndex, Gfx** dList1, Gfx** dList2, Vec3s* rot, Actor* actor) {
    int modActiveBoolSum = isFrog;
    for (int i=0; i < FROG_GID_MAX ; i++) {
        modActiveBoolSum += frogGearIsApplied[i];
    }

    if (!modActiveBoolSum) {
        return;
    }
    
    Player* player = (Player*)actor;

    if (player->stateFlags1 & PLAYER_STATE1_100000) {
        Vec3f temp;
        SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &player->actor.focus.pos, &temp);
        if (temp.z < 4.0f) {
            gFrogMouth = gCull;
            gFrogRHand = gCull;
            }
    }

    if (isFrog && limbIndex == PLAYER_LIMB_HEAD) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();
        gPushedMatrix++;
        Matrix_Translate(0.f, 0.F, 0.f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gFrogMouth);
        CLOSE_DISPS(play->state.gfxCtx);
    }

    if (((frogGearIsApplied[FROG_GID_SHIELD_HERO] && player->currentShield == PLAYER_SHIELD_HEROS_SHIELD) || 
        (frogGearIsApplied[FROG_GID_SHIELD_MIRROR] && player->currentShield == PLAYER_SHIELD_MIRROR_SHIELD)) && 
        limbIndex == PLAYER_LIMB_RIGHT_HAND && 
        player->rightHandType >= PLAYER_MODELTYPE_RH_OPEN && 
        player->rightHandType < PLAYER_MODELTYPE_RH_BOW &&
        player->leftHandType != PLAYER_MODELTYPE_LH_TWO_HAND_SWORD &&
        player->currentShield > PLAYER_SHIELD_NONE) {
        gFrogRHand = gFrogShield;
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();
        gPushedMatrix++;
        Matrix_Translate(0.f, 0.F, 0.f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gFrogRHand);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}

RECOMP_HOOK_RETURN("Player_PostLimbDrawGameplay") void return_Player_PostLimbDrawGameplay(void) {
    if (!gPushedMatrix) {
        return;
    }
    while (gPushedMatrix) {
        Matrix_Pop();
        gPushedMatrix--;
    }
}

static MtxF sSheathMatrix;

RECOMP_HOOK("Player_OverrideLimbDrawGameplayDefault") void on_Player_OverrideLimbDrawGameplayDefault(PlayState *play, s32 limbIndex, Gfx **dList, Vec3f *pos, Vec3s *rot, Actor *actor) {
    if (!isFrog) {
        return;
    }
    if (limbIndex == PLAYER_LIMB_RIGHT_THIGH) {
        Matrix_Get(&sSheathMatrix);
    } else if (limbIndex == PLAYER_LIMB_SHEATH) {
        Matrix_Put(&sSheathMatrix);

        pos->x += -1000.f;
        pos->z += -900.f;
        pos->y += 200.f;

        rot->z += DEG_TO_BINANG(-112.5f);
        rot->x += DEG_TO_BINANG(90.f);
        rot->y += DEG_TO_BINANG(25.f);
    }
}