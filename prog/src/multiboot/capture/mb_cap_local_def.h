//======================================================================
/**
 * @file	  mb_cap_local_def.c
 * @brief	  �ߊl�Q�[���E��`
 * @author	ariizumi
 * @data	  09/11/24
 *
 * ���W���[�����FMB_CAP_
 */
//======================================================================
#pragma once
#include "multiboot/mb_data_def.h"

//======================================================================
//	define
//======================================================================

//------------------------------------------------------
//�f�o�b�O�L��
//------------------------------------------------------
#define MB_CAP_DEB (1)


#pragma mark [> define
#define MB_CAPTURE_FRAME_FRAME (GFL_BG_FRAME1_M)
#define MB_CAPTURE_FRAME_DEBUG (GFL_BG_FRAME2_M)
#define MB_CAPTURE_FRAME_BG  (GFL_BG_FRAME3_M)

#define MB_CAPTURE_FRAME_SUB_BOW_LINE  (GFL_BG_FRAME1_S)
#define MB_CAPTURE_FRAME_SUB_BG  (GFL_BG_FRAME2_S)
#define MB_CAPTURE_FRAME_SUB_BOW  (GFL_BG_FRAME3_S)

#define MB_CAPTURE_PAL_SUB_BG_BOW (0) //3�{
#define MB_CAPTURE_PAL_SUB_BG     (3) //1�{

#define MB_CAPTURE_PAL_SUB_OBJ_MAIN (0) //4�{
#define MB_CAPTURE_PAL_SUB_OBJ_MAIN_NUM (4) //4�{

//------------------------------------------------------
//OBJ�n
//------------------------------------------------------
#define MB_CAP_OBJ_X_NUM (5)
#define MB_CAP_OBJ_Y_NUM (3)
#define MB_CAP_OBJ_MAIN_NUM (MB_CAP_OBJ_X_NUM*MB_CAP_OBJ_Y_NUM)
#define MB_CAP_OBJ_SUB_U_NUM (MB_CAP_OBJ_X_NUM)
#define MB_CAP_OBJ_SUB_D_NUM (MB_CAP_OBJ_X_NUM)
#define MB_CAP_OBJ_SUB_R_NUM (MB_CAP_OBJ_Y_NUM)
#define MB_CAP_OBJ_SUB_L_NUM (MB_CAP_OBJ_Y_NUM)
#define MB_CAP_OBJ_SUB_U_START (MB_CAP_OBJ_MAIN_NUM)
#define MB_CAP_OBJ_SUB_D_START (MB_CAP_OBJ_SUB_U_START+MB_CAP_OBJ_SUB_U_NUM)
#define MB_CAP_OBJ_SUB_R_START (MB_CAP_OBJ_SUB_D_START+MB_CAP_OBJ_SUB_D_NUM)
#define MB_CAP_OBJ_SUB_L_START (MB_CAP_OBJ_SUB_R_START+MB_CAP_OBJ_SUB_R_NUM)

#define MB_CAP_OBJ_NUM (MB_CAP_OBJ_MAIN_NUM + MB_CAP_OBJ_X_NUM*2 + MB_CAP_OBJ_Y_NUM*2)

#define MB_CAP_OBJ_MAIN_TOP (72)
#define MB_CAP_OBJ_MAIN_LEFT (48)
#define MB_CAP_OBJ_MAIN_X_SPACE (40)
#define MB_CAP_OBJ_MAIN_Y_SPACE (40)

#define MB_CAP_OBJ_SUB_U_TOP (48)
#define MB_CAP_OBJ_SUB_D_TOP (184)
#define MB_CAP_OBJ_SUB_R_LEFT (244)
#define MB_CAP_OBJ_SUB_L_LEFT (10)

#define MB_CAP_BALL_NUM (10)

//------------------------------------------------------
//�|�n
//------------------------------------------------------
#define MB_CAP_DOWN_BOW_PULL_LEN_MAX (FX32_CONST(100))

//------------------------------------------------------
//��n���l
//------------------------------------------------------
#if MB_CAP_DEB

extern int  MB_CAP_UPPER_BALL_POS_BASE_Y;
extern fx32 MB_CAP_BALL_SHOT_SPEED;

#else //MB_CAP_DEB

//�{�[���̔�Ԋ�{�ʒu(�^�[�Q�b�g�̉�]���S)
#define MB_CAP_UPPER_BALL_POS_BASE_Y ( 256 )
//X192:Y120�̋�����226 //66�t���[���Ŕ��ōs���ɂ�3.5���炢
#define MB_CAP_BALL_SHOT_SPEED (FX32_CONST(3.5f))

#endif  //MB_CAP_DEB

//�{�[���̍Œ�E�ō��򋗗�
#define MB_CAP_UPPER_BALL_LEN_MIN ( MB_CAP_UPPER_BALL_POS_BASE_Y-192 )
#define MB_CAP_UPPER_BALL_LEN_MAX ((int)( MB_CAP_UPPER_BALL_POS_BASE_Y*1.2f ))

//------------------------------------------------------
//�|�P�����֌W
//------------------------------------------------------
//====================================
#if MB_CAP_DEB  

extern int MB_CAP_POKE_HIDE_LOOK_TIME;
extern int MB_CAP_POKE_HIDE_HIDE_TIME;
extern int MB_CAP_POKE_RUN_LOOK_TIME;
extern int MB_CAP_POKE_RUN_HIDE_TIME;
extern int MB_CAP_POKE_DOWN_TIME;

#else //MB_CAP_DEB

#define MB_CAP_POKE_HIDE_LOOK_TIME (60*3)
#define MB_CAP_POKE_HIDE_HIDE_TIME (60*4)
#define MB_CAP_POKE_RUN_LOOK_TIME (60*1)
#define MB_CAP_POKE_RUN_HIDE_TIME (60*5)
#define MB_CAP_POKE_DOWN_TIME (60*3)

#endif  //MB_CAP_DEB
//====================================

#define MB_CAP_POKE_JUMP_HEIGHT (24)
#define MB_CAP_POKE_HIDE_TOTAL_TIME (MB_CAP_POKE_HIDE_LOOK_TIME*2+MB_CAP_POKE_HIDE_HIDE_TIME*2)

//------------------------------------------------------
//�G�t�F�N�g
//------------------------------------------------------
//���K��
#define MB_CAP_EFFECT_NUM (16)

#define MB_CAP_EFFECT_SMOKE_FRAME (5)
#define MB_CAP_EFFECT_SMOKE_SPEED (5)
#define MB_CAP_EFFECT_SMOKE_FRAME_MAX (MB_CAP_EFFECT_SMOKE_FRAME*MB_CAP_EFFECT_SMOKE_SPEED)

#define MB_CAP_EFFECT_TRANS_FRAME (6)
#define MB_CAP_EFFECT_TRANS_SPEED (4)
#define MB_CAP_EFFECT_TRANS_FRAME_MAX (MB_CAP_EFFECT_TRANS_FRAME*MB_CAP_EFFECT_TRANS_SPEED)


#define MB_CAP_OBJ_BASE_Z (50)
#define MB_CAP_POKE_BASE_Z (45)
#define MB_CAP_OBJ_LINEOFS_Z (30)

#define MB_CAP_UPPER_BALL_BASE_Z (250)
#define MB_CAP_EFFECT_Z (270)
#define MB_CAP_UPPER_TARGET_BASE_Z (280)

#define MB_CAP_BALL_HITSIZE   (4)
#define MB_CAP_OBJ_HITSIZE_XY (12)
#define MB_CAP_OBJ_HITSIZE_Z  (4)
#define MB_CAP_POKE_HITSIZE_X (10)
#define MB_CAP_POKE_HITSIZE_Y ( 6)
#define MB_CAP_POKE_HITSIZE_Z ( 8)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  MCBR_SHADOW,
  MCBR_BALL,
  MCBR_BALL_BONUS,
  //�ȉ�OBJ�͕��т�OBJ��TYPE��`�Ƃ��킹��
  MCBR_OBJ_GRASS,
  MCBR_OBJ_GRASS_SIDE,
  MCBR_OBJ_WOOD,
  MCBR_OBJ_WATER,

  //�ȉ�EFF�͕��т�EFFECT��TYPE��`�Ƃ��킹��
  MCBR_EFF_HIT,
  MCBR_EFF_SMOKE,
  MCBR_EFF_TRANS,
  MCBR_EFF_DOWN,

  MCBR_MAX,
}MB_CAP_BBD_RES;

typedef enum
{
  MCET_HIT,
  MCET_CAPTURE_SMOKE,
  MCET_CAPTURE_TRANS,
  MCET_DOWN,
  
  MCET_MAX
}MB_CAP_EFFECT_TYPE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct _MB_CAPTURE_WORK MB_CAPTURE_WORK;
typedef struct _MB_CAP_OBJ MB_CAP_OBJ;
typedef struct _MB_CAP_POKE MB_CAP_POKE;
typedef struct _MB_CAP_BALL MB_CAP_BALL;
typedef struct _MB_CAP_EFFECT MB_CAP_EFFECT;

//�����蔻��̃`�F�b�N�Ɏg��
typedef struct
{
  VecFx32 *pos;
  fx32    height;
  VecFx32 size;
}MB_CAP_HIT_WORK;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
extern const HEAPID MB_CAPTURE_GetHeapId( const MB_CAPTURE_WORK *work );
extern GFL_BBD_SYS* MB_CAPTURE_GetBbdSys( MB_CAPTURE_WORK *work );
extern ARCHANDLE* MB_CAPTURE_GetArcHandle( MB_CAPTURE_WORK *work );
extern const DLPLAY_CARD_TYPE MB_CAPTURE_GetCardType( const MB_CAPTURE_WORK *work );
extern const int MB_CAPTURE_GetBbdResIdx( const MB_CAPTURE_WORK *work , const MB_CAP_BBD_RES resType );
extern MB_CAP_OBJ* MB_CAPTURE_GetObjWork( MB_CAPTURE_WORK *work , const u8 idx );
extern MB_CAP_POKE* MB_CAPTURE_GetPokeWork( MB_CAPTURE_WORK *work , const u8 idx );
extern const BOOL MB_CAPTURE_IsBonusTime( MB_CAPTURE_WORK *work );

extern void MB_CAPTURE_GetPokeFunc( MB_CAPTURE_WORK *work , MB_CAP_BALL *ballWork , const u8 pokeIdx );
extern MB_CAP_EFFECT* MB_CAPTURE_CreateEffect( MB_CAPTURE_WORK *work , VecFx32 *pos , const MB_CAP_EFFECT_TYPE type );

extern void MB_CAPTURE_GetGrassObjectPos( const s8 idxX , const s8 idxY , VecFx32 *ret );
extern const BOOL MB_CAPTURE_CheckHit( const MB_CAP_HIT_WORK *work1 , MB_CAP_HIT_WORK *work2 );
