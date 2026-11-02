#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "playermodelmanager_api.h"

#include "skel/gFrogHumanSkel.h"
#include "skel/gFrogHumanFistSkel.h"
#include "skel/gFrogHumanBottleSkel.h"

#include "dl/gCull.h"
#include "dl/gFrogMouthClosed.h"
#include "dl/gFrogMouthHalfOpen.h"
#include "dl/gFrogMouthOpen.h"
#include "dl/gFrogMouthSmile.h"
#include "dl/gFrogShield.h"
#include "dl/gFrogScabbard.h"
#include "dl/gFrogHilt.h"
#include "dl/gFrogBlade.h"

TexturePtr gFrogEyes[] = {
    gFrogMouthClosed_tmem_eyes_ci8,
    gFrogMouthClosed_tmem_eyes_ci8,
    gFrogMouthClosed_tmem_eyes_ci8,
    gFrogMouthClosed_tmem_eyes_l_ci8,
    gFrogMouthClosed_tmem_eyes_r_ci8,
    gFrogMouthClosed_tmem_eyes_ci8,
    gFrogMouthClosed_tmem_eyes_ci8,
    gFrogMouthClosed_tmem_eyes_ci8,
};

TexturePtr gFrogEyelids[] = {
    gFrogMouthClosed_tmem_eyelids_ci8,
    gFrogMouthClosed_tmem_eyelids_half_ci8,
    gFrogMouthClosed_tmem_eyelids_closed_ci8,
    gFrogMouthClosed_tmem_eyelids_ci8,
    gFrogMouthClosed_tmem_eyelids_ci8,
    gFrogMouthClosed_tmem_eyelids_ci8,
    gFrogMouthClosed_tmem_eyelids_ci8,
    gFrogMouthClosed_tmem_eyelids_ci8,
};

static Gfx* FrogMouthDL[PLAYER_MOUTH_MAX] = {
    gFrogMouthClosed,
    gFrogMouthHalfOpen,
    gFrogMouthOpen,
    gFrogMouthSmile,
};

/*  This is used to initialize a vector which is checked
    against yIntersect
*/
f32 FrogYIntersectArr[PLAYER_FORM_MAX] = {
    1265.0f, // PLAYER_FORM_FIERCE_DEITY
    1056.0f, // PLAYER_FORM_GORON
    1506.0f, // PLAYER_FORM_ZORA
    359.0f,  // PLAYER_FORM_DEKU
    -350.0f,  // PLAYER_FORM_HUMAN
};

extern f32 D_801C08FC[PLAYER_FORM_MAX];
//    1265.0f, // PLAYER_FORM_FIERCE_DEITY
//    1056.0f, // PLAYER_FORM_GORON
//    1506.0f, // PLAYER_FORM_ZORA
//    359.0f,  // PLAYER_FORM_DEKU
//    826.0f,  // PLAYER_FORM_HUMAN

extern struct PlayerFaceIndices {
    /* 0x0 */ u8 eyeIndex;
    /* 0x1 */ u8 mouthIndex;
} PlayerFaceIndices; // size = 0x2

extern struct PlayerFaceIndices sPlayerFaces[];

extern LinkAnimationHeader gPlayerAnim_pn_Tbox_open;
extern LinkAnimationHeader gPlayerAnim_link_demo_back_to_past;
extern LinkAnimationHeader gPlayerAnim_link_demo_return_to_past;
extern LinkAnimationHeader gPlayerAnim_clink_normal_climb_startA;
extern LinkAnimationHeader gPlayerAnim_clink_normal_climb_startB;
extern LinkAnimationHeader gPlayerAnim_clink_normal_climb_upL;
extern LinkAnimationHeader gPlayerAnim_clink_normal_climb_upR;
extern LinkAnimationHeader gPlayerAnim_link_normal_Fclimb_upL;
extern LinkAnimationHeader gPlayerAnim_link_normal_Fclimb_upR;
extern LinkAnimationHeader gPlayerAnim_link_normal_Fclimb_sideL;
extern LinkAnimationHeader gPlayerAnim_link_normal_Fclimb_sideR;
extern LinkAnimationHeader gPlayerAnim_clink_normal_climb_endAL;
extern LinkAnimationHeader gPlayerAnim_clink_normal_climb_endAR;
extern LinkAnimationHeader gPlayerAnim_clink_normal_climb_endBR;
extern LinkAnimationHeader gPlayerAnim_clink_normal_climb_endBL;

extern LinkAnimationHeader gPlayerAnim_link_demo_Tbox_open;
extern LinkAnimationHeader gPlayerAnim_link_normal_climb_startA;
extern LinkAnimationHeader gPlayerAnim_link_normal_climb_startB;
extern LinkAnimationHeader gPlayerAnim_link_normal_climb_upL;
extern LinkAnimationHeader gPlayerAnim_link_normal_climb_upR;
extern LinkAnimationHeader gPlayerAnim_link_normal_climb_endAL;
extern LinkAnimationHeader gPlayerAnim_link_normal_climb_endAR;
extern LinkAnimationHeader gPlayerAnim_link_normal_climb_endBR;
extern LinkAnimationHeader gPlayerAnim_link_normal_climb_endBL;

PlayerAgeProperties gFrogAgeProperties = {
    // ceilingCheckHeight
    40.0f,
    // shadowScale
    50.0f,
    // unk_08
    0.51f,
    // unk_0C
    71.0f,
    // unk_10
    105.0f,
    // unk_14
    119.100006f,
    // unk_18
    88.5f,
    // unk_1C
    61.5f,
    // unk_20
    28.5f,
    // unk_24
    54.0f,
    // unk_28
    75.0f,
    // unk_2C
    84.0f,
    // unk_30
    102.0f,
    // unk_34
    70.0f,
    // wallCheckRadius
    27.0f,
    // unk_3C
    24.75f,
    // unk_40
    105.0f,
    // unk_44
    { 9, 0x123F, 0x167 },
    {
        { 8, 0x1256, 0x17C },
        { 9, 0x17EA, 0x167 },
        { 8, 0x1256, 0x17C },
        { 9, 0x17EA, 0x167 },
    },
    {
        { 9, 0x17EA, 0x167 },
        { 9, 0x1E0D, 0x17C },
        { 9, 0x17EA, 0x167 },
        { 9, 0x1E0D, 0x17C },
    },
    {
        { 8, 0x1256, 0x17C },
        { 9, 0x17EA, 0x167 },
        { -0x638, 0x1256, 0x17C },
        { -0x637, 0x17EA, 0x167 },
    },
    // voiceSfxIdOffset
    SFX_VOICE_BANK_SIZE * 0,
    // surfaceSfxIdOffset
    0x80,
    // unk_98
    33.0f,
    // unk_9C
    44.15145f,
    // openChestAnim
    &gPlayerAnim_pn_Tbox_open,
    // timeTravelStartAnim
    &gPlayerAnim_link_demo_back_to_past,
    // timeTravelEndAnim
    &gPlayerAnim_link_demo_return_to_past,
    // unk_AC
    &gPlayerAnim_clink_normal_climb_startA,
    // unk_B0
    &gPlayerAnim_link_normal_climb_startB,
    // unk_B4
    {
        &gPlayerAnim_link_normal_climb_upL,
        &gPlayerAnim_link_normal_climb_upR,
        &gPlayerAnim_link_normal_Fclimb_upL,
        &gPlayerAnim_link_normal_Fclimb_upR,
    },
    // unk_C4
    {
        &gPlayerAnim_link_normal_Fclimb_sideL,
        &gPlayerAnim_link_normal_Fclimb_sideR,
    },
    // unk_CC
    {
        &gPlayerAnim_link_normal_climb_endAL,
        &gPlayerAnim_link_normal_climb_endAR,
    },
    // unk_D4
    {
        &gPlayerAnim_link_normal_climb_endBR,
        &gPlayerAnim_link_normal_climb_endBL,
    },
};