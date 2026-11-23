#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "playermodelmanager_api.h"

#include "skel/gFrogHumanSkel.h"
#include "skel/gFrogHumanFistSkel.h"
#include "skel/gFrogHumanBottleSkel.h"
#include "skel/gFrogHumanOcarinaSkel.h"

#include "dl/gCull.h"
#include "dl/gFrogMouthClosed.h"
#include "dl/gFrogMouthHalfOpen.h"
#include "dl/gFrogMouthOpen.h"
#include "dl/gFrogMouthSmile.h"
#include "dl/gFrogShield.h"
#include "dl/gFrogScabbard.h"
#include "dl/gFrogHilt.h"
#include "dl/gFrogBlade.h"

#include "dl/gGiFrogGildedSword.h"
#include "dl/gGiFrogShield.h"

#define FROG_HEIGHT 0.51f
#define FROG_BREMEN_HEIGHT 0.76f
#define FROG_YINTERSECT -350.0f

#define MTX_HOOD_SCALE 0.5f
#define MTX_HOOD_LOC_X 0.f
#define MTX_HOOD_LOC_Y 0.f
#define MTX_HOOD_LOC_Z 0.f

typedef enum FrogGetItemDrawId {
    /* -1 */   FROG_GID_NONE = -1,
    /*  0 */   FROG_GID_SWORD_GILDED,
    /*  1 */   FROG_GID_SHIELD_HERO,
    /*  2 */   FROG_GID_SHIELD_MIRROR,
    /*  3 */   FROG_GID_MAX,
} FrogGetItemDrawId;

extern struct PlayerFaceIndices {
    /* 0x0 */ u8 eyeIndex;
    /* 0x1 */ u8 mouthIndex;
} PlayerFaceIndices; // size = 0x2

extern struct PlayerFaceIndices sPlayerFaces[];

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
    FROG_YINTERSECT,  // PLAYER_FORM_HUMAN
};

extern f32 D_801C08FC[PLAYER_FORM_MAX];
//    1265.0f, // PLAYER_FORM_FIERCE_DEITY
//    1056.0f, // PLAYER_FORM_GORON
//    1506.0f, // PLAYER_FORM_ZORA
//    359.0f,  // PLAYER_FORM_DEKU
//    826.0f,  // PLAYER_FORM_HUMAN

void GetItem_DrawOpa0(PlayState* play, s16 drawId);
void GetItem_DrawOpa0Xlu1(PlayState* play, s16 drawId);
void GetItem_DrawOpa01(PlayState* play, s16 drawId);

extern Gfx gGiGildedSwordDL[];
extern Gfx gGiGildedSwordEmptyDL[];
extern Gfx gGiHerosShieldDL[];
extern Gfx gGiHerosShieldEmblemDL[];
extern Gfx gGiMirrorShieldDL[];
extern Gfx gGiMirrorShieldEmptyDL[];

typedef struct {
    /* 0x0 */ void (*drawFunc)(PlayState*, s16);
    /* 0x4 */ void* drawResources[8]; // Either display lists (Gfx*) or matrices (Mtx*)
} DrawItemTableEntry; 

static DrawItemTableEntry sOriginalDrawItemEntries[] = {
    // GID_SWORD_GILDED, OBJECT_GI_SWORD_3
    { GetItem_DrawOpa01, { gGiGildedSwordDL, gGiGildedSwordEmptyDL } },
    // GID_SHIELD_HERO, OBJECT_GI_SHIELD_2
    { GetItem_DrawOpa0Xlu1, { gGiHerosShieldEmblemDL, gGiHerosShieldDL } },
    // GID_SHIELD_MIRROR, OBJECT_GI_SHIELD_3
    { GetItem_DrawOpa0Xlu1, { gGiMirrorShieldEmptyDL, gGiMirrorShieldDL } },
};

extern DrawItemTableEntry sDrawItemTable[];

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
extern LinkAnimationHeader gPlayerAnim_clink_demo_goto_future;
extern LinkAnimationHeader gPlayerAnim_clink_demo_return_to_future;
extern LinkAnimationHeader gPlayerAnim_clink_demo_Tbox_open;
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
    FROG_HEIGHT,
    // unk_0C
    71.0f,
    // unk_10
    50.0f,
    // unk_14
    49.0f,
    // unk_18
    39.0f,
    // unk_1C
    27.0f,
    // unk_20
    19.0f,
    // unk_24
    22.0f,
    // unk_28
    32.4f,
    // unk_2C
    32.0f,
    // unk_30
    48.0f,
    // unk_34
    11.0f / 17.0f * 70.0f,
    // wallCheckRadius
    14.0f,
    // unk_3C
    12.0f,
    // unk_40
    55.0f,
    // unk_44
    { -0x18, 0xDED, 0x36C },
    {
        { -0x18, 0xD92, 0x35E },
        { -0x18, 0x1371, 0x3A9 },
        { 8, 0x1256, 0x17C },
        { 9, 0x17EA, 0x167 },
    },
    {
        { -0x18, 0x1371, 0x3A9 },
        { -0x18, 0x195F, 0x3A9 },
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
    SFX_VOICE_BANK_SIZE * 1,
    // surfaceSfxIdOffset
    0,
    // unk_98
    22.0f,
    // unk_9C
    29.4343f,
    // openChestAnim
    &gPlayerAnim_pn_Tbox_open,
    // timeTravelStartAnim
    &gPlayerAnim_clink_demo_goto_future,
    // timeTravelEndAnim
    &gPlayerAnim_clink_demo_return_to_future,
    // unk_AC
    &gPlayerAnim_clink_normal_climb_startA,
    // unk_B0
    &gPlayerAnim_clink_normal_climb_startB,
    // unk_B4
    {
        &gPlayerAnim_clink_normal_climb_upL,
        &gPlayerAnim_clink_normal_climb_upR,
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
        &gPlayerAnim_clink_normal_climb_endAL,
        &gPlayerAnim_clink_normal_climb_endAR,
    },
    // unk_D4
    {
        &gPlayerAnim_clink_normal_climb_endBR,
        &gPlayerAnim_clink_normal_climb_endBL,
    },
};

/*
typedef enum PlayerTransformation {
     0  PLAYER_FORM_FIERCE_DEITY,
     1  PLAYER_FORM_GORON,
     2  PLAYER_FORM_ZORA,
     3  PLAYER_FORM_DEKU,
     4  PLAYER_FORM_HUMAN,
     5  PLAYER_FORM_MAX
} PlayerTransformation;
*/