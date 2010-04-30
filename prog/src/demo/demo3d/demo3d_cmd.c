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
#include "system/bmp_winframe.h"

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
static void DEMOCMD_BgmPlay(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param);
static void DEMOCMD_BgmStop(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param);
static void DEMOCMD_BgmFade(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param);
static void DEMOCMD_BgmChangeReq(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param);
static void DEMOCMD_BrightnessReq(DEMO3D_CMD_WORK* wk, DEMO3D_ENGINE_WORK* core, int* param);
static void DEMOCMD_FlashReq(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param);
static void DEMOCMD_LightColorSet(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param);
static void DEMOCMD_LightVectorSet(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param);
static void DEMOCMD_TalkWinReq(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param);
static void DEMOCMD_ScrDrawReq(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param);
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
  DEMOCMD_BgmPlay,
  DEMOCMD_BgmStop,
  DEMOCMD_BgmFade,
  DEMOCMD_BgmChangeReq,
  DEMOCMD_BrightnessReq,
  DEMOCMD_FlashReq,
  DEMOCMD_LightColorSet,
  DEMOCMD_LightVectorSet,
  DEMOCMD_TalkWinReq,
  DEMOCMD_ScrDrawReq,
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
 *	@param	param[1] playerNo 0�`3 : DEMO3D_SE_PARAM_DEFAULT�Ȃ疳��
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_SeStop(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
  SEPLAYER_ID player_no;

  if( param[1] == DEMO3D_SE_PARAM_DEFAULT){
    player_no = PMSND_GetSE_DefaultPlayerID( param[0] );
  }else{
    player_no = param[1];
  }
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

//===================================================================================
///BGM�ݒ�
//===================================================================================
//-----------------------------------------------------------------------------
/**
 *	@brief  BGM�Đ� 
 *
 *	@param	param[0] BGM_Label
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_BgmPlay(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
  PMSND_PlayBGM( param[0] );
}
//-----------------------------------------------------------------------------
/**
 *	@brief  BGM�X�g�b�v
 *
 *	@param  none	
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_BgmStop(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
  PMSND_StopBGM();
}

//-----------------------------------------------------------------------------
/**
 *	@brief  BGM�t�F�[�h
 *
 *	@param  param[0]	�t�F�[�h�p�^�[��(0:fadein,1:fadeout) 
 *  @param  param[1]  ��frame�Ńt�F�[�h���邩�H
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_BgmFade(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
  int frame = sub_ConvFrame( wk, param[1] );

  if(param[0] == 0){
    PMSND_FadeInBGM( frame );
  }else{
    PMSND_FadeOutBGM( frame );
  }
}

//===================================================================================
///BGM�ύX���N�G�X�g
//===================================================================================
typedef struct _TASKWK_BGM_CHANGE_REQ{
  DEMO3D_CMD_WORK* cmd;
  DEMO3D_ENGINE_WORK* core;

  u8  seq;
  u8  push_f;
  u8  pop_f;
  u8  type;
  int bgm_no;
  int fadeout_frame;
  int fadein_frame;

}TASKWK_BGM_CHANGE_REQ;

static void taskAdd_BgmChangeReq( DEMO3D_CMD_WORK* wk, int bgm_no, int fadeout_frame, int fadein_frame, int push_pop );
static void tcb_BgmChangeReq( GFL_TCBL *tcb , void* work);

//-----------------------------------------------------------------------------
/**
 *	@brief  BGM�ύX���N�G�X�g
 *
 *  @param  param[0]  BGM-No(0�`65535)
 *  @param  param[1]  fadeout_frame(0�`65535) 
 *  @param  param[2]  fadein_frame(0�`65535)
 *  @param  param[3]  type("play"=0,"stop"=1,"change"=2,"push"=3,"pop" =4)
 *
 *  type��DEMO3D_BGM_CHG_TYPE�^(demo3d_cmd_def.h)
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_BgmChangeReq(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
  int bgm_no = param[0];
  int fadeout_frame = sub_ConvFrame(wk,param[1]);
  int fadein_frame = sub_ConvFrame(wk,param[2]);
  int type = param[3];

  if( type == DEMO3D_BGM_PLAY ){ //play
    PMSND_PlayBGM( bgm_no );
    PMSND_FadeInBGM( fadein_frame );
  }else{
    taskAdd_BgmChangeReq( wk, bgm_no, fadeout_frame, fadein_frame, type );
  }
}

static void taskAdd_BgmChangeReq( DEMO3D_CMD_WORK* wk, int bgm_no, int fadeout_frame, int fadein_frame, int type )
{
  GFL_TCBL* tcb;
  TASKWK_BGM_CHANGE_REQ* twk;

  tcb = GFL_TCBL_Create( wk->tcbsys, tcb_BgmChangeReq, sizeof(TASKWK_BGM_CHANGE_REQ), 0 );

  twk = GFL_TCBL_GetWork(tcb);
  MI_CpuClear8( twk, sizeof( TASKWK_BGM_CHANGE_REQ ));

  twk->cmd = wk;
  twk->type = type;
  twk->push_f = ( type == DEMO3D_BGM_PUSH );
  twk->pop_f = ( type == DEMO3D_BGM_POP );
  
  twk->bgm_no = bgm_no;
  twk->fadeout_frame = fadeout_frame;
  twk->fadein_frame = fadein_frame;
  
  if( twk->fadeout_frame ){
    PMSND_FadeOutBGM( twk->fadeout_frame );
  }else{
    twk->seq = 1; //�t�F�[�h�҂������X�L�b�v
  }
}

static void tcb_BgmChangeReq( GFL_TCBL *tcb , void* tcb_wk)
{
  TASKWK_BGM_CHANGE_REQ* twk = (TASKWK_BGM_CHANGE_REQ*)tcb_wk;

  switch(twk->seq){
  case 0:
    if( PMSND_CheckFadeOnBGM()){
      return;
    }
    twk->seq++;
    //�u���C�N�X���[ 
  case 1:
    if( twk->push_f ){
      if( !twk->cmd->bgm_push_f ){
        PMSND_PushBGM();
        twk->cmd->bgm_push_f = TRUE;
      }else{
        GF_ASSERT_MSG(0,"BGM��push�͈��܂łł�\n�R�}���h����������Ă�������\n");
      }
    }else{
      PMSND_StopBGM();
      if( twk->type == DEMO3D_BGM_STOP ){
        break;
      }
    }
    if( twk->pop_f ){
      if( twk->cmd->bgm_push_f ) {
        PMSND_PopBGM();
        twk->cmd->bgm_push_f = FALSE;
      }else{
        GF_ASSERT_MSG(0,"BGM��push���ꂸ��pop����悤�Ƃ��Ă��܂�\n�R�}���h����������Ă�������\n");
      }
    }else{
      PMSND_PlayBGM( twk->bgm_no );
    }
    if( twk->fadein_frame ){
      PMSND_FadeInBGM( twk->fadein_frame );
    }
    break;
  }
  GFL_TCBL_Delete(tcb);
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �P�x�ω��G�t�F�N�g��p�����u���C�g�l�X 
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
///�P�x�ω��G�t�F�N�g��p�����t���b�V�����N�G�X�g
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
 *	@brief  �P�x�ω��G�t�F�N�g��p�����t���b�V�����N�G�X�g
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
  GXRgb col = GX_RGB(param[1],param[2],param[3]);

  DEMO3D_GRAPHIC_3DLightColorSet( core->graphic, param[0], col );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���C�g�x�N�g���ύX 
 *
 *	@param	param[0]  light_no(0�`3)
 *	@param  param[1]  light_vec_x(fx16 -7.9�`7.9)
 *	@param  param[2]  light_vec_y(fx16 -7.9�`7.9)
 *	@param  param[3]  light_vec_z(fx16 -7.9�`7.9)
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_LightVectorSet(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
  VecFx16 vec;
  
  vec.x = (fx16)param[1];
  vec.y = (fx16)param[2];
  vec.z = (fx16)param[3];

  DEMO3D_GRAPHIC_3DLightVectorSet( core->graphic, param[0], &vec );
}

//===================================================================================
///��b�E�B���h�E�\��
//===================================================================================
typedef struct _TASKWK_TALKWIN_REQ{
  DEMO3D_CMD_WORK* cmd;

  int seq;
  int ct;
  int sync;

  int msg_arc;
  int msg_id;
  GFL_ARCUTIL_TRANSINFO tInfo;

  GFL_BMPWIN *win;
  PRINT_STREAM* printStream;
  
}TASKWK_TALKWIN_REQ;

#define TALKWIN_BG_FRM    ( DEMO3D_CMD_FREE_BG_M1 )
#define TALKWIN_FRAME_CGX (1)
#define TALKWIN_FRAME_PAL (0)
#define TALKWIN_FONT_PAL  (1)

static void taskAdd_TalkWinReq( DEMO3D_CMD_WORK* wk, int msg_arc, int msg_id, int sync, u8 px, u8 py, u8 sx, u8 sy );
static void tcb_TalkWinReq( GFL_TCBL *tcb , void* work);
static void taskwk_TalkWinReqInit( DEMO3D_CMD_WORK* wk, TASKWK_TALKWIN_REQ* twk,
    int msg_arc, int msg_id, int sync, u8 px, u8 py, u8 sx, u8 sy );
static void taskwk_TalkWinReqFree( TASKWK_TALKWIN_REQ* wk );

//-----------------------------------------------------------------------------
/**
 *	@brief   ��b�E�B���h�E�\��
 *
 *	@param	param[0]  ���b�Z�[�W�A�[�J�C�uID 
 *	@param	param[1]  ���b�Z�[�WID
 *	@param	param[2]  ���b�Z�[�W�Z�b�g��( ���ۂɕ\������郁�b�Z�[�WID = param[2]*�V�[��ID+param[1])
 *	@param	param[3]  �E�B���h�E��\������t���[����(1�`)
 *	@param	param[4]  �E�B���h�E����X���W(1�`30) 
 *	@param	param[5]  �E�B���h�E����Y���W(1�`21)
 *	@param	param[6]  �E�B���h�E�T�C�YX(2�`28)
 *	@param	param[7]  �E�B���h�E�T�C�YY(2�`22) 
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_TalkWinReq(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
  int msg_id = param[1]+param[2]*core->env.scene_id;

  taskAdd_TalkWinReq( wk, param[0], msg_id, sub_ConvFrame(wk,param[3]) , param[4], param[5], param[6], param[7] );
}

static void taskAdd_TalkWinReq( DEMO3D_CMD_WORK* wk, int msg_arc, int msg_id, int sync, u8 px, u8 py, u8 sx, u8 sy )
{
  GFL_TCBL* tcb;
  TASKWK_TALKWIN_REQ* twk;

  tcb = GFL_TCBL_Create( wk->tcbsys, tcb_TalkWinReq, sizeof(TASKWK_TALKWIN_REQ), 1 );

  twk = GFL_TCBL_GetWork(tcb);

  //BMP�E�B���h�E&�X�g���[������
  taskwk_TalkWinReqInit( wk, twk, msg_arc, msg_id, sync, px, py, sx, sy );
}

static void tcb_TalkWinReq( GFL_TCBL *tcb , void* tcb_wk )
{
  TASKWK_TALKWIN_REQ* twk = (TASKWK_TALKWIN_REQ*)tcb_wk;
  DEMO3D_CMD_WORK* cmd = twk->cmd;

  //�V�X�e���I���Ŏ��E
  if( twk->cmd->cmdsys_end_f ){
    taskwk_TalkWinReqFree( twk );
    GFL_TCBL_Delete(tcb);
    return;
  }
  switch(twk->seq){
  case 0:
    {
      GFL_MSGDATA *msg_man;
      STRBUF *str;

      //�X�g���[���J�n
      msg_man = GFL_MSG_Create(
		  GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, twk->msg_arc, cmd->tmpHeapID );

      str = GFL_MSG_CreateString( msg_man, twk->msg_id );

      GFL_FONTSYS_SetColor( 1, 2, 15 );
      twk->printStream = PRINTSYS_PrintStream( twk->win, 0, 0,
        str, cmd->fontHandle, cmd->msg_spd, cmd->tcbsys, 0, cmd->heapID, 15 );

      GFL_BMPWIN_MakeTransWindow( twk->win );
      BmpWinFrame_WriteAreaMan( twk->win, WINDOW_TRANS_ON_V, twk->tInfo, TALKWIN_FRAME_PAL );

      GFL_STR_DeleteBuffer( str );
      GFL_MSG_Delete( msg_man );

      GFL_BG_SetPriority( TALKWIN_BG_FRM, 0 );
      GFL_BG_SetPriority( DEMO3D_3D_BG_M, 1 );
      GFL_BG_SetVisible( TALKWIN_BG_FRM, VISIBLE_ON );
    }
    twk->seq++;
    break;
  case 1:
    if(twk->ct++ < twk->sync) {
      break;
    }
    taskwk_TalkWinReqFree( twk );
    GFL_TCBL_Delete(tcb);
    break;
  }
}

static void taskwk_TalkWinReqInit( DEMO3D_CMD_WORK* wk, TASKWK_TALKWIN_REQ* twk,
    int msg_arc, int msg_id, int sync, u8 px, u8 py, u8 sx, u8 sy )
{
  
  MI_CpuClear8( twk, sizeof( TASKWK_TALKWIN_REQ ));
  
  twk->cmd = wk;
  twk->sync = sync;
  twk->msg_arc = msg_arc;
  twk->msg_id = msg_id;

  //Bmp�E�B���h�E�O���t�B�b�N�Z�b�g
  twk->tInfo = BmpWinFrame_GraphicSetAreaMan(
	  TALKWIN_BG_FRM, TALKWIN_FRAME_PAL, MENU_TYPE_TALK, wk->tmpHeapID );

  //�t�H���g�p���b�g�Z�b�g
	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT,
			NARC_font_default_nclr, PALTYPE_MAIN_BG, TALKWIN_FONT_PAL*0x20, 0x20, wk->tmpHeapID );
  
  twk->win = GFL_BMPWIN_Create( TALKWIN_BG_FRM, px, py, sx, sy, TALKWIN_FONT_PAL, GFL_BMP_CHRAREA_GET_F );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( twk->win ), 15 );
}

static void taskwk_TalkWinReqFree( TASKWK_TALKWIN_REQ* twk )
{
  if( twk->printStream != NULL ){
    PRINTSYS_PrintStreamDelete( twk->printStream );
  }
  GFL_FONTSYS_SetDefaultColor();
  
  BmpWinFrame_Clear( twk->win, WINDOW_TRANS_ON_V );
  GFL_BMPWIN_Delete( twk->win );
  BmpWinFrame_GraphicFreeAreaMan( TALKWIN_BG_FRM, twk->tInfo );
  
  MI_CpuClear8( twk, sizeof( TASKWK_TALKWIN_REQ ));
  
  GFL_BG_SetPriority( TALKWIN_BG_FRM, 1 );
  GFL_BG_SetPriority( DEMO3D_3D_BG_M, DEMO3D_3D_BG_PRI );
  GFL_BG_SetVisible( TALKWIN_BG_FRM, VISIBLE_OFF );
}

//===================================================================================
///BG�ꖇ�G�\��
//===================================================================================
typedef struct _TASKWK_SCR_DRAW_REQ{
  DEMO3D_CMD_WORK* cmd;

  int seq;
  int ct;
  int sync;

  int pltt_id;
  int char_id;
  int scr_id;

  int s_frm;
  int e_frm;

  fx32  alpha_n;
  fx32  alpha_d;
}TASKWK_SCR_DRAW_REQ;

#define SCR_DRAW_ALPHA_1ST  (GX_BLEND_PLANEMASK_BG1)
#define SCR_DRAW_ALPHA_2ND  (GX_BLEND_PLANEMASK_BG0)
#define ALPHA_EV2_NORMAL  (3)
#define ALPHA_EV1_NORMAL  (16-ALPHA_EV2_NORMAL)

static void taskAdd_ScrDrawReq( DEMO3D_CMD_WORK* wk,
    int pltt_id, int char_id, int scr_id, int sync, int s_frm, int e_frm );
static void tcb_ScrDrawReq( GFL_TCBL *tcb , void* work);
static void taskwk_ScrDrawReqInit( DEMO3D_CMD_WORK* wk, TASKWK_SCR_DRAW_REQ* twk,
    int pltt_id, int char_id, int scr_id, int sync, int s_frm, int e_frm );
static void taskwk_ScrDrawReqFree( TASKWK_SCR_DRAW_REQ* wk );

//-----------------------------------------------------------------------------
/**
 *	@brief   BG�ꖇ�G�\��
 *
 *  @param  param[0]  �K�p������ < "ver_all"(0xFF),"ver_black"(VERSION_BLACK),"ver_white"(VERSION_WHITE),"ver_gray"(VERSION_WHITE+1) >
 *	@param	param[1]  �p���b�g�A�[�J�C�uID 
 *	@param	param[2]  �L�����N�^�A�[�J�C�uID
 *	@param	param[3]  �X�N���[���A�[�J�C�uID
 *	@param	param[4]  �E�B���h�E��\������t���[����(1�`)
 *	@param	param[5]  ���t�F�[�h�C���t���[���� 
 *	@param	param[6]  ���t�F�[�h�A�E�g�t���[����
 */
//-----------------------------------------------------------------------------
static void DEMOCMD_ScrDrawReq(DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, int* param)
{
  if( param[0] != 0xFF && param[0] != CasetteVersion ){
    return; //����
  }
  taskAdd_ScrDrawReq( wk, param[1], param[2], param[3],
    sub_ConvFrame(wk,param[4]), sub_ConvFrame(wk,param[5]), sub_ConvFrame(wk,param[6]) );
}

static void taskAdd_ScrDrawReq( DEMO3D_CMD_WORK* wk,
    int pltt_id, int char_id, int scr_id, int sync, int s_frm, int e_frm )
{
  GFL_TCBL* tcb;
  TASKWK_SCR_DRAW_REQ* twk;

  tcb = GFL_TCBL_Create( wk->tcbsys, tcb_ScrDrawReq, sizeof(TASKWK_SCR_DRAW_REQ), 1 );

  twk = GFL_TCBL_GetWork(tcb);

  //BMP�E�B���h�E&�X�g���[������
  taskwk_ScrDrawReqInit( wk, twk, pltt_id, char_id, scr_id, sync, s_frm, e_frm );
}

static void tcb_ScrDrawReq( GFL_TCBL *tcb , void* tcb_wk )
{
  TASKWK_SCR_DRAW_REQ* twk = (TASKWK_SCR_DRAW_REQ*)tcb_wk;
  DEMO3D_CMD_WORK* cmd = twk->cmd;
 
  //�V�X�e���I���Ŏ��E
  if( twk->cmd->cmdsys_end_f) {
    taskwk_ScrDrawReqFree( twk );
    GFL_TCBL_Delete(tcb);
    return;
  }
  switch( twk->seq ){
  case 0:
    if( cmd->delay_f ){
      break;  //�������ו��U�̂��߁A�x���t���[���Ȃ珈�����Ȃ�
    }
    GFL_ARCHDL_UTIL_TransVramPalette( cmd->h_2dgra, twk->pltt_id, PALTYPE_MAIN_BG, 0, 0, cmd->tmpHeapID );
 	  GFL_ARCHDL_UTIL_TransVramScreenCharOfsVBlank(	cmd->h_2dgra, twk->scr_id,
						DEMO3D_CMD_FREE_BG_M1, 0, 0, 0, TRUE, cmd->tmpHeapID );	
    twk->seq++;
    break;
  case 1:
    if( cmd->delay_f ){
      break;  //�K���]����҂��߁A�x���t���[���Ȃ珈�����Ȃ�
    }
    twk->ct = 0;
    twk->alpha_n = 0;
    twk->alpha_d = FX_Div( FX32_CONST(16),FX32_CONST(twk->s_frm));

    GFL_BG_SetPriority( TALKWIN_BG_FRM, 0 );
    GFL_BG_SetPriority( DEMO3D_3D_BG_M, 1 );
    G2_SetBlendAlpha( SCR_DRAW_ALPHA_1ST,SCR_DRAW_ALPHA_2ND, 0, 16 );
    GFL_BG_SetVisible( TALKWIN_BG_FRM, VISIBLE_ON );
    twk->seq++;
    break;
  case 2:
    {
      int ev;
      
      if( twk->ct++ < twk->s_frm ){
        twk->alpha_n += twk->alpha_d;
        ev = FX_Whole( twk->alpha_n);
        G2_ChangeBlendAlpha( ev, 16-ev);
        break;
      }
      G2_ChangeBlendAlpha( 16, 0 );
      twk->ct = 0;
      twk->seq++;
    }
    break;
  case 3:
    if( twk->ct++ < twk->sync ){
      break;
    }
    twk->ct = 0;
    twk->alpha_n = 0;
    twk->alpha_d = FX_Div( FX32_CONST(16),FX32_CONST(twk->e_frm));
    twk->seq++;
    break;
  case 4:
    {
      int ev;
      
      if( twk->ct++ < twk->e_frm ){
        twk->alpha_n += twk->alpha_d;
        ev = FX_Whole( twk->alpha_n);
        G2_ChangeBlendAlpha( 16-ev, ev);
        break;
      }
      G2_ChangeBlendAlpha( 0, 16 );
      twk->ct = 0;
      twk->seq++;
    }
    break;
  default:
    taskwk_ScrDrawReqFree( twk );
    GFL_TCBL_Delete(tcb);
    break;
  }
}

static void taskwk_ScrDrawReqInit( DEMO3D_CMD_WORK* wk, TASKWK_SCR_DRAW_REQ* twk,
    int pltt_id, int char_id, int scr_id, int sync, int s_frm, int e_frm )
{
  GFL_MSGDATA *msg_man;
  STRBUF *str;
  
  MI_CpuClear8( twk, sizeof( TASKWK_SCR_DRAW_REQ ));
  
  twk->cmd = wk;
  twk->sync = sync;

  twk->pltt_id = pltt_id;
  twk->char_id = char_id;
  twk->scr_id = scr_id;

  twk->s_frm = s_frm;
  twk->e_frm = e_frm;
  GFL_ARCHDL_UTIL_TransVramBgCharacter(	wk->h_2dgra, twk->char_id,
						DEMO3D_CMD_FREE_BG_M1, 0, 0, TRUE, wk->tmpHeapID );
}

static void taskwk_ScrDrawReqFree( TASKWK_SCR_DRAW_REQ* twk )
{
  MI_CpuClear8( twk, sizeof( TASKWK_SCR_DRAW_REQ ));
  
  GFL_BG_SetVisible( TALKWIN_BG_FRM, VISIBLE_OFF );
  GFL_BG_SetPriority( DEMO3D_3D_BG_M, DEMO3D_3D_BG_PRI );
  GFL_BG_SetPriority( TALKWIN_BG_FRM, 1 );

  G2_BlendNone();
  GFL_BG_ClearFrame( DEMO3D_CMD_FREE_BG_M1 );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���[�V�����u���[ �J�n
 *
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


