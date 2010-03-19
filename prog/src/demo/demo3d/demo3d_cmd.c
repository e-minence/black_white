//=============================================================================
/**
 *
 *	@file		demo3d_cmd.c
 *	@brief  3D�f���R�}���h
 *	@author	hosaka genya
 *	@data		2009.12.09
 *
 */
//=============================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/brightness.h"

// ������֘A
#include "font/font.naix"
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/printsys.h" //PRINT_QUE
#include "gamesystem/msgspeed.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "message.naix"
#include "demo3d_cmd.h"

#include "sound/pm_sndsys.h" // for SEQ_SE_XXX

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static void DEMOCMD_SePlay(DEMO3D_CMD_WORK* wk, DEMO3D_ENGINE_WORK* core, int* param);
static void DEMOCMD_SeStop(DEMO3D_CMD_WORK* wk, DEMO3D_ENGINE_WORK* core, int* param);
static void DEMOCMD_SeVolumeEffectReq(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param);
static void DEMOCMD_SePanEffectReq(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param);
static void DEMOCMD_SePitchEffectReq(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param);
static void DEMOCMD_BrightnessReq(DEMO3D_CMD_WORK* wk, DEMO3D_ENGINE_WORK* core, int* param);
static void DEMOCMD_FlashReq(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param);
static void DEMOCMD_MotionBL_Start(DEMO3D_CMD_WORK* wk, DEMO3D_ENGINE_WORK* core, int* param);
static void DEMOCMD_MotionBL_End(DEMO3D_CMD_WORK* wk, DEMO3D_ENGINE_WORK* core, int* param);

static int sub_ConvFrame( DEMO3D_CMD_WORK* wk, int frame );


// DEMO3D_CMD_TYPE �ƑΉ�
//--------------------------------------------------------------
///	�R�}���h�e�[�u�� (�֐��|�C���^�e�[�u��)
//==============================================================
void (*DATA_Demo3D_CmdTable[ DEMO3D_CMD_TYPE_MAX ])() = 
{ 
  NULL, // null
  DEMOCMD_SePlay,
  DEMOCMD_SeStop,
  DEMOCMD_SeVolumeEffectReq,
  DEMOCMD_SePanEffectReq,
  DEMOCMD_SePitchEffectReq,
  DEMOCMD_BrightnessReq,
  DEMOCMD_FlashReq,
  DEMOCMD_MotionBL_Start,
  DEMOCMD_MotionBL_End,
  NULL, // end
};

//-----------------------------------------------------------------------------
/**
 *	@brief  SE�Đ��R�}���h
 *
 *	@param	param[0] SE_Label
 *	@param	param[1] volume 0�`127 : DEMO3D_SE_PARAM_DEFAULT�Ȃ疳��
 *	@param	param[2] pan -128�`127 �FDEMO3D_SE_PARAM_DEFAULT�Ȃ疳��
 *	@param	param[3] pitch -32768�`32767(+/-64�Ŕ���)�FDEMO3D_SE_PARAM_DEFAULT�Ȃ疳��
 *	@param	param[4] playerNo 0�`3 : DEMO3D_SE_PARAM_DEFAULT�Ȃ疳��
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_SePlay(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
  SEPLAYER_ID player_no;

  //�v���C���[No
  if(param[4] != DEMO3D_SE_PARAM_DEFAULT){
    PMSND_PlaySE_byPlayerID( param[0], param[4] );
    player_no = param[4];
  }else{
    PMSND_PlaySE(param[0]);
    player_no = PMSND_GetSE_DefaultPlayerID( param[0] );
  }

  // volume
  if( param[1] != DEMO3D_SE_PARAM_DEFAULT )
  {
    PMSND_PlayerSetVolume( player_no, param[1] );
  }

  // pan
  if( param[2] != DEMO3D_SE_PARAM_DEFAULT )
  {
		PMSND_SetStatusSE_byPlayerID( player_no, PMSND_NOEFFECT, PMSND_NOEFFECT, param[2] );
  }

  //pitch
  if( param[3] != DEMO3D_SE_PARAM_DEFAULT){
		PMSND_SetStatusSE_byPlayerID( player_no, PMSND_NOEFFECT, param[3], PMSND_NOEFFECT );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  SE:�Đ���~
 *
 *	@param	param[0] SE_Label
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_SeStop(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
  SEPLAYER_ID player_no;

  player_no = PMSND_GetSE_DefaultPlayerID( param[0] );

  PMSND_StopSE_byPlayerID( player_no );
}

//===================================================================================
///SE�̓��I�p�����[�^�ω����N�G�X�g
//===================================================================================
typedef struct _TASKWK_SE_EFFECT_REQ{
  DEMO3D_CMD_WORK* cmd;

  u8  seq;
  u8  type;
  u8  playerNo;
  int frame;
  int start,end;
  fx32  now,diff;
}TASKWK_SE_EFFECT_REQ;

static void taskAdd_SeEffectReq( DEMO3D_CMD_WORK* wk, u8 type, u16 seNo, int start, int end, int frame,int playerNo );
static void tcb_SeEffectReq( GFL_TCBL *tcb , void* work);
static void sub_SetSeEffect( DEMO3D_SE_EFFECT type, u8 playerNo, int value );

//-----------------------------------------------------------------------------
/**
 *	@brief  SE�̃{�����[�����I�ω����N�G�X�g
 *
 *  @param  param[0]  SE_Label
 *  @param  param[1]  �X�^�[�g�̒l(0�`127) 
 *  @param  param[2]  �G���h�̒l(0�`127)
 *  @param  param[3]  �X�^�[�g�`�G���h�܂ł̃t���[����(1�`)
 *  @param  param[4]  �v���C���[No(0�`3 �f�t�H���g�ł�������DEMO3D_SE_PARAM_DEFAULT)
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_SeVolumeEffectReq(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
  taskAdd_SeEffectReq( wk, DEMO3D_SE_EFF_VOLUME, param[0], param[1], param[2], param[3], param[4] );
}
//-----------------------------------------------------------------------------
/**
 *	@brief  SE�̃p�����I�ω����N�G�X�g
 *
 *  @param  param[0]  SE_Label
 *  @param  param[1]  �X�^�[�g�̒l(-128�`127) 
 *  @param  param[2]  �G���h�̒l(-128�`127)
 *  @param  param[3]  �X�^�[�g�`�G���h�܂ł̃t���[����(1�`)
 *  @param  param[4]  �v���C���[No(0�`3 �f�t�H���g�ł�������DEMO3D_SE_PARAM_DEFAULT)
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_SePanEffectReq(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
  taskAdd_SeEffectReq( wk, DEMO3D_SE_EFF_PAN, param[0], param[1], param[2], param[3], param[4] );
}
//-----------------------------------------------------------------------------
/**
 *	@brief  SE�̃s�b�`���I�ω����N�G�X�g
 *
 *  @param  param[0]  SE_Label
 *  @param  param[1]  �X�^�[�g�̒l(-32768�`32767 +/-64�Ŕ���) 
 *  @param  param[2]  �G���h�̒l(-32768�`32767 +/-64�Ŕ���) 
 *  @param  param[3]  �X�^�[�g�`�G���h�܂ł̃t���[����(1�`)
 *  @param  param[4]  �v���C���[No(0�`3 �f�t�H���g�ł�������DEMO3D_SE_PARAM_DEFAULT)
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_SePitchEffectReq(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
  taskAdd_SeEffectReq( wk, DEMO3D_SE_EFF_PITCH, param[0], param[1], param[2], param[3], param[4] );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  SE�̓��I�p�����[�^�ω��^�X�N�ǉ�
 *
 *  @param  param[0]  SE_Label
 *  @param  param[0]  �K�p������� DEMO3D_SE_EFFECT�^ 
 *  @param  param[1]  �X�^�[�g�̒l 
 *  @param  param[2]  �G���h�̒l 
 *  @param  param[3]  �X�^�[�g�`�G���h�܂ł̃t���[����(1�`)
 *  @param  param[4]  �v���C���[No(0�`3 �f�t�H���g�ł�������DEMO3D_SE_PARAM_DEFAULT)
 *
 *  �X�^�[�g/�G���h�̒l��min/max��type�ɂ���ĕς��܂�
 *	  volume 0�`127
 *	  pan -128�`127
 *	  pitch -32768�`32767(+/-64�Ŕ���)
 */
//-----------------------------------------------------------------------------
static void taskAdd_SeEffectReq( DEMO3D_CMD_WORK* wk, u8 type, u16 seNo, int start, int end, int frame,int playerNo )
{
  GFL_TCBL* tcb;
  TASKWK_SE_EFFECT_REQ* twk;

  tcb = GFL_TCBL_Create( wk->tcbsys, tcb_SeEffectReq, sizeof(TASKWK_SE_EFFECT_REQ), 0 );

  twk = GFL_TCBL_GetWork(tcb);
  MI_CpuClear8( twk, sizeof( TASKWK_SE_EFFECT_REQ ));

  twk->cmd = wk;
  twk->type = type;
  twk->start = start;
  twk->end = end;
  twk->frame =  sub_ConvFrame( wk, frame );
  
  if( playerNo == DEMO3D_SE_PARAM_DEFAULT ){
    twk->playerNo = playerNo;
  }else{
    twk->playerNo = PMSND_GetSE_DefaultPlayerID( seNo );
  }
  
  twk->now = FX32_CONST(start);
  twk->diff = FX_Div((FX32_CONST(end)-twk->now),FX32_CONST(frame));
}

static void tcb_SeEffectReq( GFL_TCBL *tcb , void* tcb_wk)
{
  TASKWK_SE_EFFECT_REQ* twk = (TASKWK_SE_EFFECT_REQ*)tcb_wk;

  if( twk->frame-- > 0 ){
    sub_SetSeEffect( twk->type, twk->playerNo, FX_Whole(twk->now) );
    twk->now += twk->diff;
    return;
  }
  sub_SetSeEffect( twk->type, twk->playerNo, twk->end );

  GFL_TCBL_Delete(tcb);
}

static void sub_SetSeEffect( DEMO3D_SE_EFFECT type, u8 playerNo, int value )
{
  switch(type){
  case DEMO3D_SE_EFF_VOLUME:
    PMSND_PlayerSetVolume( playerNo, value );
    break;
  case DEMO3D_SE_EFF_PAN:
		PMSND_SetStatusSE_byPlayerID( playerNo, PMSND_NOEFFECT, PMSND_NOEFFECT, value );
    break;
  case DEMO3D_SE_EFF_PITCH:
		PMSND_SetStatusSE_byPlayerID( playerNo, PMSND_NOEFFECT, value, PMSND_NOEFFECT );
    break;
  default:
    GF_ASSERT(0);
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �}�X�^�[�P�x��p�����u���C�g�l�X 
 *
 *  @param  param[0]  �K�p������(1:main,2:sub,3:double) 
 *  @param  param[1]  ��frame�Ńt�F�[�h���邩�H (0�͊J�n���P�x�l�𑦎����f)
 *  @param  param[2]  �t�F�[�h�J�n���̋P�x < -16�`16(���`��) >
 *  @param  param[3]  �t�F�[�h�I�����̋P�x < -16�`16(���`��) >
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_BrightnessReq(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
  if(param[1] == 0){
    SetBrightness( param[2], PLANEMASK_ALL, param[0]);
  }else{
    ChangeBrightnessRequest( sub_ConvFrame( wk, param[1] ), param[3], param[2],PLANEMASK_ALL, param[0] );
  }
}

//===================================================================================
///�}�X�^�[�P�x��p�����t���b�V�����N�G�X�g
//===================================================================================
typedef struct _TASKWK_FLASH_REQ{
  DEMO3D_CMD_WORK* cmd;
  DEMO3D_ENGINE_WORK* core;

  s8  seq;
  s8  start,max,end;
  int frame_s,frame_e;
  u8  lcd_mask;
}TASKWK_FLASH_REQ;

static void taskAdd_FlashReq( DEMO3D_CMD_WORK* wk, u8 lcd_mask, s8 start, s8 max, s8 end, int frame_s, int frame_e );
static void tcb_FlashReq( GFL_TCBL *tcb , void* work);

//-----------------------------------------------------------------------------
/**
 *	@brief  �}�X�^�[�P�x��p�����t���b�V�����N�G�X�g
 *
 *  @param  param[0]  �K�p������ < "main"(1),"sub"(2),"double"(3) >
 *  @param  param[1]  start�̋P�x < -16�`16(���`��) >
 *  @param  param[2]  max�P�x     < -16�`16(���`��) >
 *  @param  param[3]  end�̋P�x   < -16�`16(���`��) >
 *  @param  param[4]  start����max�܂ł̃t���[�� < 1�` >
 *  @param  param[5]  max����end�܂ł̃t���[�� < 1�` >
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_FlashReq(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
  taskAdd_FlashReq( wk, param[0], param[1], param[2], param[3], sub_ConvFrame( wk, param[4] ), sub_ConvFrame( wk, param[5] ) );
}

static void taskAdd_FlashReq( DEMO3D_CMD_WORK* wk, u8 lcd_mask, s8 start, s8 max, s8 end, int frame_s, int frame_e )
{
  GFL_TCBL* tcb;
  TASKWK_FLASH_REQ* twk;

  tcb = GFL_TCBL_Create( wk->tcbsys, tcb_FlashReq, sizeof(TASKWK_FLASH_REQ), 0 );

  twk = GFL_TCBL_GetWork(tcb);
  MI_CpuClear8( twk, sizeof( TASKWK_FLASH_REQ ));

  twk->cmd = wk;
  twk->start = start;
  twk->max = max;
  twk->end = end;
  twk->lcd_mask = lcd_mask;
  twk->frame_s = frame_s;
  twk->frame_e = frame_e;
   
  ChangeBrightnessRequest( twk->frame_s, twk->max, twk->start, PLANEMASK_ALL, twk->lcd_mask );
}

static void tcb_FlashReq( GFL_TCBL *tcb , void* tcb_wk)
{
  TASKWK_FLASH_REQ* twk = (TASKWK_FLASH_REQ*)tcb_wk;

  switch(twk->seq){
  case 0:
    if( IsFinishedBrightnessChg( twk->lcd_mask )){
      ChangeBrightnessRequest( twk->frame_e, twk->end, twk->max, PLANEMASK_ALL, twk->lcd_mask );
      twk->seq++;
    }
    return;
  case 1:
    if( IsFinishedBrightnessChg( twk->lcd_mask )){
      break;
    }
    return;
  }
  GFL_TCBL_Delete(tcb);
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���C�g�J���[�ύX 
 *
 *	@param	param[0]  light_no(0�`3)
 *	@param  param[1]  light_col_r(0�`31)
 *	@param  param[2]  light_col_g(0�`31)
 *	@param  param[3]  light_col_b(0�`31)
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_LightColorSet(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
#if 0
  GRAPHIC_G3D_WORK* g3d = &core->graphic->g3d;
//  GXRgb col = ;

  //���C�g�Đݒ�
	GFL_G3D_LIGHT_SetColor( g3d->p_lightset, param[0], (u16*)&f_light->light);
  GFL_G3D_LIGHT_Switching( g3d->p_lightset );
#endif
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���[�V�����u���[ �J�n
 *
 *	@param	wk
 *	@param	param[0] ���[�V�����u���[�W�� �V�����u�����h����G�̃��W��
 *	@param	param[1] ���[�V�����u���[�W�� ���Ƀo�b�t�@����Ă���G�̃��W��
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_MotionBL_Start(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
  GF_ASSERT( wk->mb == NULL );

	wk->mb = DEMO3D_MotionBlInit( wk->tcbsys, param[0], param[1] );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���[�V�����u���[ ��~
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_MotionBL_End(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
  DEMO3D_MotionBlExit( wk->mb );
  wk->mb = NULL;
}

//=============================================================================
/**
 *								���̑��T�u���[�`��
 */
//=============================================================================

//-----------------------------------------------------------------------------
/*
 *  @brief  �w��̃t���[�������A�t���[�����[�g�ɉ����ĕϊ�����
 *
 *  ���^�X�N�^�̃R�}���h�͕K��60fps�œ����̂ŁA30fps�ݒ�̃f���Ŏw�肳�ꂽ�t���[������
 *   60fps���Z�Ɍv�Z���Ȃ���
 */
//-----------------------------------------------------------------------------
static int sub_ConvFrame( DEMO3D_CMD_WORK* wk, int frame )
{
  if(wk->core->scene->frame_rate == DEMO3D_FRAME_RATE_30FPS ){
    frame *= 2;
  }
  return frame;
}


