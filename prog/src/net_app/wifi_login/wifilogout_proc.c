//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifilogout_proc.c
 *	@brief  WIFI ���O�A�E�g���
 *	@author	Toru=Nagihashi
 *	@data		2010.02.04
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/main.h"
#include "system/wipe.h"
#include "net/dwc_error.h"
#include "sound/pm_sndsys.h"

//�A�[�J�C�u
#include "message.naix"
#include "msg/msg_wifi_system.h"

//�����̃��W���[��
#include "wifilogin_local.h"  //wiflogin�Ɠ������̂��g��

//�O�����J
#include "net_app/wifi_logout.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================

//-------------------------------------
///	SEQFUNCTION
//=====================================
typedef struct _WIFILOGOUT_WORK WIFILOGOUT_WORK;  //�ȉ��̊֐��̂��ߑO���錾
typedef void (*SEQ_FUNCTION)( WIFILOGOUT_WORK* p_wk );

//-------------------------------------
///	���C�����[�N
//=====================================
struct _WIFILOGOUT_WORK
{
  WIFILOGIN_YESNO_WORK    *p_select;    //�I����  ���󖢎g�p
  WIFILOGIN_DISP_WORK     *p_display;   //�`��n
  WIFILOGIN_MESSAGE_WORK  *p_message;   //���b�Z�[�W�n
  SEQ_FUNCTION            seq_funcion;  //�V�[�P���X
  WIFILOGOUT_PARAM        *p_param;     //����
  HEAPID                  heapID;       //�q�[�vID

  BOOL                    is_end_net;
} ;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT WIFILOGOUT_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFILOGOUT_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFILOGOUT_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	SEQ wifilogin�Ɠ�������
//=====================================
static void SEQ_Change( WIFILOGOUT_WORK *p_wk, SEQ_FUNCTION seq_funcion );
static void SEQ_ChangeDebug( WIFILOGOUT_WORK *p_wk, SEQ_FUNCTION seq_funcion, int line );
static BOOL SEQ_Main( WIFILOGOUT_WORK *p_wk );

#if PM_DEBUG
#define   SEQ_CHANGE_STATE(p_wk, state)  SEQ_Change( p_wk, state )
#else  //PM_DEBUG
#define   SEQ_CHANGE_STATE(p_wk, state)  SEQ_ChangeDebug( p_wk, state, __LINE__ )
#endif //PM_DEBUG

//-------------------------------------
///	SEQ_FUNC
//    ��{�I�Ɂ��ւ̗��ꏇ�ł�
//=====================================
static void SEQFUNCTION_StartFadeIn( WIFILOGOUT_WORK *p_wk );
static void SEQFUNCTION_WaitFadeIn( WIFILOGOUT_WORK *p_wk );

static void SEQFUNCTION_Callback( WIFILOGOUT_WORK *p_wk );

static void SEQFUNCTION_StartDisConnectMessage( WIFILOGOUT_WORK *p_wk );
static void SEQFUNCTION_WaitDisConnectMessage( WIFILOGOUT_WORK *p_wk );
static void SEQFUNCTION_StartDisConnect( WIFILOGOUT_WORK *p_wk );
static void SEQFUNCTION_WaitDisConnect( WIFILOGOUT_WORK *p_wk );
static void SEQFUNCTION_StartEndMessage( WIFILOGOUT_WORK *p_wk );
static void SEQFUNCTION_WaitEndMessage( WIFILOGOUT_WORK *p_wk );

static void SEQFUNCTION_StartFadeOut( WIFILOGOUT_WORK *p_wk );
static void SEQFUNCTION_WaitFadeOut( WIFILOGOUT_WORK *p_wk );

//-------------------------------------
///	�ʐM�R�[���o�b�N
//=====================================
static void NETCALLBACK_End( void* p_wk_adrs );   ///< �ʐM�I�����ɌĂ΂��R�[���o�b�N

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
const GFL_PROC_DATA WiFiLogout_ProcData = 
{
  WIFILOGOUT_PROC_Init,
  WIFILOGOUT_PROC_Main,
  WIFILOGOUT_PROC_Exit,
};

//=============================================================================
/**
 *					�v���Z�X
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	WIFILOGOUT	���C���v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFILOGOUT_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFILOGOUT_WORK   *p_wk;
  WIFILOGOUT_PARAM  *p_param  = p_param_adrs;

  //�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFILOGOUT, 0x10000 );

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(WIFILOGOUT_WORK), HEAPID_WIFILOGOUT );
	GFL_STD_MemClear( p_wk, sizeof(WIFILOGOUT_WORK) );	
  p_wk->p_param = p_param;
  p_wk->heapID  = HEAPID_WIFILOGOUT;

  //�f�t�H���g�ݒ�
  if( p_wk->p_param->fade == WIFILOGIN_FADE_DEFAULT )
  { 
    p_wk->p_param->fade = WIFILOGIN_FADE_BLACK;
  }

  //���W���[���쐬
  p_wk->p_display = WIFILOGIN_DISP_Init( p_wk->heapID, p_param->bg,p_param->display );
  p_wk->p_message = WIFILOGIN_MESSAGE_Init( p_wk->heapID, NARC_message_wifi_system_dat, p_param->display );

  //�J�n�V�[�P���X
  SEQ_CHANGE_STATE( p_wk, SEQFUNCTION_StartFadeIn );  
  
  
  if( p_param->bg == WIFILOGIN_BG_NORMAL )
  { 
    WIFILOGIN_MESSAGE_TitleDisp( p_wk->p_message, dwc_title_0001);

    if( p_param->bgm == WIFILOGIN_BGM_NORMAL )
    { 
      PMSND_PlayBGM( SEQ_BGM_WIFI_ACCESS );
    }
  }

  GFL_NET_ReloadIcon();

  if( p_param->display == WIFILOGIN_DISPLAY_DOWN )
  { 
    GFL_NET_WirelessIconEasy_HoldLCD(FALSE, p_wk->heapID);
  }
  else
  { 
    GFL_NET_WirelessIconEasy_HoldLCD(TRUE, p_wk->heapID);
  }
  GFL_NET_ReloadIcon();

  return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	WIFILOGOUT	���C���v���Z�X�I��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFILOGOUT_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFILOGOUT_WORK   *p_wk     = p_wk_adrs;
  WIFILOGOUT_PARAM  *p_param  = p_param_adrs;

  //���W���[���j��
  if( p_param->bg == WIFILOGIN_BG_NORMAL )
  { 
    WIFILOGIN_MESSAGE_TitleEnd(p_wk->p_message);
  }
  WIFILOGIN_MESSAGE_End( p_wk->p_message );
  WIFILOGIN_DISP_End( p_wk->p_display );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_WIFILOGOUT );

  return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	WIFILOGOUT	���C���v���Z�X����
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFILOGOUT_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFILOGOUT_WORK   *p_wk     = p_wk_adrs;

  //�G���[�`�F�b�N
  //�iGAMESYSTEM�����̉��ʂ�PROC��NetErr_DispCall(FALSE);���Ă΂�Ă���̂��O��ł��j
  if( GFL_NET_IsInit() )
  { 
    if( GFL_NET_DWC_ERROR_RESULT_NONE != GFL_NET_DWC_ERROR_ReqErrorDisp( TRUE ) )
    {
      return GFL_PROC_RES_FINISH;
    }
  }

  //���W���[������
  if( p_wk->p_select )
  {
    WIFILOGIN_MESSAGE_YesNoUpdate( p_wk->p_select );
  }
  WIFILOGIN_DISP_Main( p_wk->p_display );
  WIFILOGIN_MESSAGE_Main( p_wk->p_message );

  //�V�[�P���X���s
  { 
    GFL_PROC_RESULT ret = GFL_PROC_RES_CONTINUE;

    if( SEQ_Main( p_wk ) )
    { 
      ret = GFL_PROC_RES_FINISH;
    }

    return ret;
  }
}

//=============================================================================
/**
 *    SEQ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �V�[�P���X�ύX
 *
 *	@param	WIFILOGOUT_WORK *p_wk ���[�N  
 *	@param	*p_state              �ݒ肷��V�[�P���X
 */
//-----------------------------------------------------------------------------
static void SEQ_Change( WIFILOGOUT_WORK *p_wk, SEQ_FUNCTION seq_funcion )
{ 
  p_wk->seq_funcion = seq_funcion;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �V�[�P���X�ύX  �f�o�b�O��
 *
 *	@param	WIFILOGOUT_WORK *p_wk ���[�N
 *	@param	*p_state              �ݒ肷��V�[�P���X
 *	@param	line                  �f�o�b�O�p�s��
 */
//-----------------------------------------------------------------------------
static void SEQ_ChangeDebug( WIFILOGOUT_WORK *p_wk, SEQ_FUNCTION seq_funcion, int line )
{ 
  OS_TPrintf( "neg: %d\n", line );
  SEQ_Change( p_wk, seq_funcion );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �V�[�P���X�ύX  ���s���C��
 *
 *	@param	WIFILOGOUT_WORK *p_wk   ���[�N
 *
 *	@return TRUE�ŏI��  FALSE�Ŏ��s��
 */
//-----------------------------------------------------------------------------
static BOOL SEQ_Main( WIFILOGOUT_WORK *p_wk )
{
  if( p_wk->seq_funcion )
  { 
    p_wk->seq_funcion( p_wk );
    return FALSE;
  }

  return TRUE;
}

//=============================================================================
/**
 *    SEQFUNCTION
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�C���J�n
 *
 *	@param	WIFILOGOUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNCTION_StartFadeIn( WIFILOGOUT_WORK *p_wk )
{ 
  int fade;
  if( p_wk->p_param->fade & WIFILOGIN_FADE_WHITE_IN )
  { 
    fade  = WIPE_FADE_WHITE;
  }
  else
  { 
    fade  = WIPE_FADE_BLACK;
  }

  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
      fade, WIPE_DEF_DIV , WIPE_DEF_SYNC , p_wk->heapID );
  SEQ_CHANGE_STATE( p_wk, SEQFUNCTION_WaitFadeIn );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�C���҂�
 *
 *	@param	WIFILOGOUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNCTION_WaitFadeIn( WIFILOGOUT_WORK *p_wk )
{ 
  if(WIPE_SYS_EndCheck())
  {
    SEQ_CHANGE_STATE( p_wk, SEQFUNCTION_StartDisConnectMessage );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �R�[���o�b�N����
 *
 *	@param	WIFILOGOUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNCTION_Callback( WIFILOGOUT_WORK *p_wk )
{ 
  if( GFL_NET_IsExit() )
  { 
    SEQ_CHANGE_STATE( p_wk, SEQFUNCTION_StartDisConnectMessage );
  }
  else
  { 
    WIFILOGIN_CALLBACK_RESULT result;
    result  = p_wk->p_param->logout_before_callback( p_wk->p_message, p_wk->p_param->p_callback_wk );
    if( result != WIFILOGIN_CALLBACK_RESULT_CONTINUE )
    { 
      SEQ_CHANGE_STATE( p_wk, SEQFUNCTION_StartDisConnectMessage );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�J�n
 *
 *	@param	WIFILOGOUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNCTION_StartDisConnectMessage( WIFILOGOUT_WORK *p_wk )
{ 
  WIFILOGIN_MESSAGE_InfoMessageDispWaitIcon( p_wk->p_message, dwc_message_0011 );
  SEQ_CHANGE_STATE( p_wk, SEQFUNCTION_WaitDisConnectMessage );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�҂�
 *
 *	@param	WIFILOGOUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNCTION_WaitDisConnectMessage( WIFILOGOUT_WORK *p_wk )
{ 
  if(WIFILOGIN_MESSAGE_InfoMessageEndCheck( p_wk->p_message ))
  {
    SEQ_CHANGE_STATE( p_wk, SEQFUNCTION_StartDisConnect );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ؒf�J�n
 *
 *	@param	WIFILOGOUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNCTION_StartDisConnect( WIFILOGOUT_WORK *p_wk )
{ 
  if( GFL_NET_IsExit() )
  { 
    //�������Ă����牽�����Ȃ�
    SEQ_CHANGE_STATE( p_wk, SEQFUNCTION_StartEndMessage );
    OS_TPrintf( "WIFILOGOUT ���Ƀl�b�g�͉������Ă���\n" );
  }
  else
  { 
    //����������Ă�������
    if( GFL_NET_Exit( NETCALLBACK_End ) )
    { 
      SEQ_CHANGE_STATE( p_wk, SEQFUNCTION_WaitDisConnect );
      OS_TPrintf( "WIFILOGOUT �l�b�g����J�n\n" );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ؒf��
 *
 *	@param	WIFILOGOUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNCTION_WaitDisConnect( WIFILOGOUT_WORK *p_wk )
{ 
  if( p_wk->is_end_net )
  {
    SEQ_CHANGE_STATE( p_wk, SEQFUNCTION_StartEndMessage );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�J�n
 *
 *	@param	WIFILOGOUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNCTION_StartEndMessage( WIFILOGOUT_WORK *p_wk )
{ 
  WIFILOGIN_MESSAGE_InfoMessageDisp( p_wk->p_message, dwc_message_0012 );
  SEQ_CHANGE_STATE( p_wk, SEQFUNCTION_WaitEndMessage );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�҂�
 *
 *	@param	WIFILOGOUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNCTION_WaitEndMessage( WIFILOGOUT_WORK *p_wk )
{ 
  if(WIFILOGIN_MESSAGE_InfoMessageEndCheck( p_wk->p_message ))
  {
    SEQ_CHANGE_STATE( p_wk, SEQFUNCTION_StartFadeOut );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�A�E�g�J�n
 *
 *	@param	WIFILOGOUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNCTION_StartFadeOut( WIFILOGOUT_WORK *p_wk )
{   
  int fade;
  if( p_wk->p_param->fade & WIFILOGIN_FADE_WHITE_OUT )
  { 
    fade  = WIPE_FADE_WHITE;
  }
  else
  { 
    fade  = WIPE_FADE_BLACK;
  }

  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
      fade, WIPE_DEF_DIV , WIPE_DEF_SYNC , p_wk->heapID );
  SEQ_CHANGE_STATE( p_wk, SEQFUNCTION_WaitFadeOut );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�A�E�g�҂�
 *
 *	@param	WIFILOGOUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNCTION_WaitFadeOut( WIFILOGOUT_WORK *p_wk )
{ 
  if(WIPE_SYS_EndCheck())
  {
    SEQ_CHANGE_STATE( p_wk, NULL ); //�I��
  }
}

//=============================================================================
/**
 *    �ʐM�R�[���o�b�N
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �ʐM�I�����ɌĂ΂��R�[���o�b�N
 *
 *	@param	void* p_wk_adrs ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void NETCALLBACK_End( void* p_wk_adrs )
{ 
  WIFILOGOUT_WORK *p_wk = p_wk_adrs;
  p_wk->is_end_net  = TRUE;
}
