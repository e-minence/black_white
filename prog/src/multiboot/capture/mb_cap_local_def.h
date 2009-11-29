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
#pragma mark [> define
#define MB_CAPTURE_FRAME_FRAME (GFL_BG_FRAME1_M)
#define MB_CAPTURE_FRAME_BG  (GFL_BG_FRAME3_M)

#define MB_CAPTURE_FRAME_SUB_BOW_LINE  (GFL_BG_FRAME1_S)
#define MB_CAPTURE_FRAME_SUB_BG  (GFL_BG_FRAME2_S)
#define MB_CAPTURE_FRAME_SUB_BOW  (GFL_BG_FRAME3_S)

#define MB_CAPTURE_PAL_SUB_BG_BOW (0) //3�{
#define MB_CAPTURE_PAL_SUB_BG     (3) //1�{

#define MB_CAPTURE_PAL_SUB_OBJ_MAIN (0) //4�{
#define MB_CAPTURE_PAL_SUB_OBJ_MAIN_NUM (4) //4�{

//OBJ�n
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
#define MB_CAP_OBJ_SUB_R_LEFT (10)
#define MB_CAP_OBJ_SUB_L_LEFT (244)

#define MB_CAP_BALL_NUM (10)

//�|�n
#define MB_CAP_DOWN_BOW_PULL_LEN_MAX (FX32_CONST(100))

//��n���l
//�{�[���̔�Ԋ�{�ʒu(�^�[�Q�b�g�̉�]���S)
#define MB_CAP_UPPER_BALL_POS_BASE_Y ( 256 )
//�{�[���̍Œ�E�ō��򋗗�
#define MB_CAP_UPPER_BALL_LEN_MIN ( 32 )
#define MB_CAP_UPPER_BALL_LEN_MAX ( 300 )


#define MB_CAP_OBJ_BASE_Z (20)
#define MB_CAP_POKE_BASE_Z (20)
#define MB_CAP_UPPER_BALL_BASE_Z (50)
#define MB_CAP_UPPER_TARGET_BASE_Z (280)

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

  MCBR_MAX,
}MB_CAP_BBD_RES;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct _MB_CAPTURE_WORK MB_CAPTURE_WORK;


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
extern const HEAPID MB_CAPTURE_GetHeapId( const MB_CAPTURE_WORK *work );
extern GFL_BBD_SYS* MB_CAPTURE_GetBbdSys( MB_CAPTURE_WORK *work );
extern ARCHANDLE* MB_CAPTURE_GetArcHandle( MB_CAPTURE_WORK *work );
extern const DLPLAY_CARD_TYPE MB_CAPTURE_GetCardType( const MB_CAPTURE_WORK *work );
extern const int MB_CAPTURE_GetBbdResIdx( const MB_CAPTURE_WORK *work , const MB_CAP_BBD_RES resType );
