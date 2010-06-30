//==============================================================================
/**
 * @file  title_control.c
 * @brief �^�C�g����PROC����
 * @author  matsuda
 * @date  2008.12.05(��)
 *
 * �t�@�C�����Ƌ@�\����v���Ă��Ȃ��悤�ȋC������
 */
//==============================================================================
#include <gflib.h>
#include <procsys.h>
#include "system\main.h"
#include "title/title.h"
#include "demo/command_demo.h"
#include "corporate.h"

#include "playable_version.h"
#include "pm_version.h"
#include "system/gfl_use.h"

#ifdef  PLAYABLE_VERSION
extern void SetVersion( u8 version );
#endif
//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT TitleControlProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT TitleControlProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT TitleControlProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

//==============================================================================
//  �f�[�^
//==============================================================================

const GFL_PROC_DATA TitleControlProcData = {
  TitleControlProcInit,
  TitleControlProcMain,
  TitleControlProcEnd,
};

static CORP_PARAM  CorpParam={0,0};   // �Ж��\���������[�N
static COMMANDDEMO_DATA cdemo_data;   // �f�������f�[�^


//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   
 *
 * @param   proc    
 * @param   seq   
 * @param   pwk   
 * @param   mywk    
 *
 * @retval  
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT TitleControlProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
#ifdef  PLAYABLE_VERSION
  //L�������Ă�����z���C�g�AR�������Ă�����u���b�N�A����ȊO�̓����_���ɂ���
  u8 version = GFUser_GetPublicRand( 2 ) == 0 ? VERSION_BLACK : VERSION_WHITE ;
  u16 key = GFL_UI_KEY_GetCont();

  if ( key & PAD_BUTTON_L )
  {
    version = VERSION_WHITE;
  }
  else if ( key & PAD_BUTTON_R )
  {
    version = VERSION_BLACK;
  }
  SetVersion( version );

#endif
  return GFL_PROC_RES_FINISH;
}

#include "debug/debug_hudson.h"
//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT TitleControlProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
#ifdef PM_DEBUG
  switch( *seq  ){
  case 0:   // �Ж��\��
#ifdef DEBUG_ONLY_FOR_hudson
    if( HUDSON_IsSkipTitle() )
    {
      // HUDSON�Ŏ��s�����ꍇ�͒��Ńt�B�[���h��
      CorpParam.ret = CORPORATE_RET_DEBUG;
      *seq = 4;
    }
    else
#endif
    {
      GFL_PROC_SysCallProc( FS_OVERLAY_ID(title), &CorpProcData, &CorpParam );
      *seq = 1;
    }
    break;

  case 1:   // �f�e���S
    CorpParam.mode = ONLY_DEVELOP_VIEW;     // 2��ڈȍ~�̌����ҕ\�����[�h�ݒ�
    if( CorpParam.ret == CORPORATE_RET_NORMAL ){
      cdemo_data.mode = COMMANDDEMO_MODE_GF_LOGO;
      cdemo_data.skip = COMMANDDEMO_SKIP_DEBUG;
      cdemo_data.ret  = COMMANDDEMO_RET_NORMAL;
      GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &cdemo_data );
#ifdef  PLAYABLE_VERSION
      *seq = 3;
#else
      *seq = 2;
#endif
    }else{
      *seq = 4;   // �^�C�g����
    }
    break;

  case 2:		// �f���P
    if( cdemo_data.ret != COMMANDDEMO_RET_SKIP_DEBUG ){
      cdemo_data.mode = COMMANDDEMO_MODE_OP_MOVIE1;
      cdemo_data.ret  = COMMANDDEMO_RET_NORMAL;
      GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &cdemo_data );
      *seq = 3;
    }else{
      *seq = 4;   // �^�C�g����
    }
    break;

  case 3:		// �f���Q
    if( cdemo_data.ret != COMMANDDEMO_RET_SKIP_DEBUG ){
      cdemo_data.mode = COMMANDDEMO_MODE_OP_MOVIE2;
      cdemo_data.ret  = COMMANDDEMO_RET_NORMAL;
      GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &cdemo_data );
    }
    *seq = 4;   // �^�C�g����
    break;

  case 4:   // ���C���^�C�g��
    if( cdemo_data.ret == COMMANDDEMO_RET_SKIP_DEBUG ){
      CorpParam.ret = CORPORATE_RET_DEBUG;
    }
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(title), &TitleProcData, &CorpParam.ret );
    *seq = 0;
    break;
  }
#else
  switch( *seq  ){
  case 0:   // �Ж��\��
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(title), &CorpProcData, &CorpParam );
    *seq = 1;
    break;

  case 1:   // �f�e���S
		CorpParam.mode = ONLY_DEVELOP_VIEW;     // 2��ڈȍ~�̌����ҕ\�����[�h�ݒ�
    cdemo_data.mode = COMMANDDEMO_MODE_GF_LOGO;
    cdemo_data.skip = COMMANDDEMO_SKIP_ON;
    cdemo_data.ret  = COMMANDDEMO_RET_NORMAL;
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &cdemo_data );
#ifdef  PLAYABLE_VERSION
    *seq = 3;
#else
    *seq = 2;
    break;

  case 2:		// �f���P
		if( cdemo_data.ret == COMMANDDEMO_RET_NORMAL ){
			cdemo_data.mode = COMMANDDEMO_MODE_OP_MOVIE1;
			cdemo_data.ret  = COMMANDDEMO_RET_NORMAL;
			GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &cdemo_data );
			*seq = 3;
		}else{
			*seq = 4;   // �^�C�g����
		}
		break;
#endif

  case 3:		// �f���Q
		cdemo_data.mode = COMMANDDEMO_MODE_OP_MOVIE2;
		cdemo_data.ret  = COMMANDDEMO_RET_NORMAL;
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &cdemo_data );
		*seq = 4;   // �^�C�g����
		break;

  case 4:   // ���C���^�C�g��
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(title), &TitleProcData, NULL );
    *seq = 0;
    break;
  }
#endif

  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT TitleControlProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  return GFL_PROC_RES_FINISH;
}
