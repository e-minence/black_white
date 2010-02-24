#include "sound/pm_sndsys.h"

#ifdef WAVE_SE_WB_KON
#undef WAVE_SE_WB_KON
#endif
#ifdef WAVE_SE_WB_TB_START
#undef WAVE_SE_WB_TB_START
#endif
#ifdef WAVE_SE_WB_MSCL_HUUH
#undef WAVE_SE_WB_MSCL_HUUH
#endif
#ifdef BANK_SE_WB_KON
#undef BANK_SE_WB_KON
#endif
#ifdef BANK_SE_WB_TB_START
#undef BANK_SE_WB_TB_START
#endif
#ifdef BANK_SE_WB_MSCL_05
#undef BANK_SE_WB_MSCL_05
#endif
#ifdef SEQ_SE_KON
#undef SEQ_SE_KON
#endif
#ifdef SEQ_SE_TB_START
#undef SEQ_SE_TB_START
#endif
#ifdef SEQ_SE_MSCL_04
#undef SEQ_SE_MSCL_04
#endif
#ifdef SEQ_SE_WB_SLIDE
#undef SEQ_SE_WB_SLIDE
#endif
#ifdef WAVE_SE_WB_SLIDE
#undef WAVE_SE_WB_SLIDE
#endif
#ifdef SEQ_SE_SYS_06
#undef SEQ_SE_SYS_06
#endif
#ifdef WAVE_SE_WB_SYS_06
#undef WAVE_SE_WB_SYS_06
#endif
#ifdef BANK_SE_WB_SYS_06
#undef BANK_SE_WB_SYS_06
#endif
#ifdef SEQ_SE_DUMMY
#undef SEQ_SE_DUMMY
#endif
#ifdef WAVE_SE_WB_DUMMY
#undef WAVE_SE_WB_DUMMY
#endif
#ifdef BANK_SE_WB_DUMMY
#undef BANK_SE_WB_DUMMY
#endif
#include "multiboot/wb_sound_palpark_game.sadl"


#define MB_SND_BOW_PULL (SEQ_SE_PAL_YUMIHIKI)
#define MB_SND_BOW_SHOT (SEQ_SE_PAL_HYUN)
//#define MB_SND_BALL_CATCH (SEQ_SE_SYS_06)
#define MB_SND_BALL_CATCH (SEQ_SE_KON)
#define MB_SND_BALL_NO_HIT (SEQ_SE_KON)
#define MB_SND_BALL_HIT_WOOD (SEQ_SE_PAL_DOSA)
#define MB_SND_BALL_HIT_WATER (SEQ_SE_PAL_BOCHA)
#define MB_SND_BALL_BOUND (SEQ_SE_PAL_HAJIKU)
#define MB_SND_GRASS_SHAKE (SEQ_SE_TB_START)
#define MB_SND_POKE_JUMP (SEQ_SE_MSCL_04)
#define MB_SND_POKE_DOWN (SEQ_SE_PAL_GURUGURU)
#define MB_SND_POKE_CAPTURE (SEQ_SE_PAL_BOWA3)
#define MB_SND_POKE_TRANS (SEQ_SE_PAL_TENSO)

#define MB_SND_POKE_READY (SEQ_SE_PAL_READY)
#define MB_SND_POKE_START (SEQ_SE_PAL_START)
#define MB_SND_POKE_FINISH (SEQ_SE_PAL_FINISH)
#define MB_SND_POKE_HIGH_SCORE (SEQ_SE_PAL_HISCORE)

#define MB_CAP_BONUS_BGM_TEMPO (192)
#define MB_CAP_BONUS_BGM_PITCH (-24)
#define MB_CAP_HURRY_BGM_TEMPO (285)
#define MB_CAP_HURRY_BGM_PITCH ( 85)

