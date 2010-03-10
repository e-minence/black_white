//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		livebattlematch_flor.c
 *	@brief  ���C�����j���[ �t���[
 *	@author	Toru=Nagihashi
 *	@data		2010.03.04
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID
#include "poke_tool/pokeparty.h"
#include "gamesystem/btl_setup.h"
#include "print/str_tool.h"
#include "net/network_define.h"
#include "poke_tool/poke_regulation.h"
#include "sound/pm_sndsys.h"
#include "system/net_err.h"
#include "system/ds_system.h"

//�v���Z�X
#include "net_app/wifibattlematch.h"
#include "digitalcardcheck.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "message.naix"
#include "msg/msg_battle_championship.h"
#include "script_message.naix"
#include "msg/script/msg_common_scr.h"

//�l�b�g
#include "net/dreamworld_netdata.h"

//WIFIBATTLEMATCH�̃��W���[��
#include "net_app/wifi_match/wifibattlematch_util.h"
#include "net_app/wifi_match/wifibattlematch_graphic.h"
#include "net_app/wifi_match/wifibattlematch_view.h"

//�O�����J
#include "battle_championship_flow.h"

//-------------------------------------
///	DEBUG
//=====================================
#ifdef PM_DEBUG
#endif //PM_DEBUG

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
///	�V�X�e�����[�N
//=====================================
struct _BATTLE_CHAMPIONSHIP_FLOW_WORK
{ 
  //�V�[�P���X���䃂�W���[��
  WBM_SEQ_WORK                    *p_seq;

  //�I�����\�����W���[��
  WBM_LIST_WORK                   *p_list;
  u32                             list_type;

  //�G���[���b�Z�[�W
  GFL_MSGDATA                     *p_err_msg;

  //�߂�l
  BATTLE_CHAMPIONSHIP_FLOW_RET    retcode;

  //����
  BATTLE_CHAMPIONSHIP_FLOW_PARAM  param;

  //�q�[�vID
  HEAPID                          heapID;

  //�f�W�^���I��؃t���[
  DIGITALCARD_CHECK_WORK          *p_flow;
};
//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�V�[�P���X�֐�
//=====================================
static void SEQFUNC_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_End( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
//static void SEQFUNC_FadeIn( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
//static void SEQFUNC_FadeOut( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_MainMenu( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_Info( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_WifiMenu( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_WifiInfo( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_WifiDigitalCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_LiveDigitalCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_LiveMenu( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_LiveInfo( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

static void SEQFUNC_RestrictUGC( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_EnableWireless( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	�֗��֐�
//=====================================
//���X�g
typedef enum
{ 
  BC_MENU_TYPE_FIRST,
  BC_MENU_TYPE_WIFI,
  BC_MENU_TYPE_LIVE,
  BC_MENU_TYPE_YESNO,
}BC_MENU_TYPE;
static void UTIL_LIST_Create( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk, BC_MENU_TYPE type );
static void UTIL_LIST_Delete( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk );
static u32 UTIL_LIST_Main( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk );

//�e�L�X�g
static void UTIL_TEXT_Print( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk, u32 strID );
static void UTIL_TEXT_PrintErr( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk, u32 strID );
static BOOL UTIL_TEXT_IsEnd( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk );

//�t���[�I��
static void UTIL_FLOW_End( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk, BATTLE_CHAMPIONSHIP_FLOW_RET ret );

//=============================================================================
/**
 *					�f�[�^
*/
//=============================================================================
//=============================================================================
/**
 *					PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���C�����j���[�t���[  ����
 *
 *	@param	const BATTLE_CHAMPIONSHIP_FLOW_PARAM *cp_param  ����
 *	@param	heapID  �q�[�vID 
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
BATTLE_CHAMPIONSHIP_FLOW_WORK *BATTLE_CHAMPIONSHIP_FLOW_Init( const BATTLE_CHAMPIONSHIP_FLOW_PARAM *cp_param, HEAPID heapID )
{ 
  BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(BATTLE_CHAMPIONSHIP_FLOW_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(BATTLE_CHAMPIONSHIP_FLOW_WORK) );
  p_wk->param   = *cp_param;
  p_wk->heapID  = heapID;

  p_wk->p_err_msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE, NARC_script_message_common_scr_dat, heapID );

  //���W���[���쐬
  p_wk->p_seq = WBM_SEQ_Init( p_wk, SEQFUNC_Start, heapID );
  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C�����j���[�t���[  �j��
 *
 *	@param	p_wk	���[�N

 */
//-----------------------------------------------------------------------------
void BATTLE_CHAMPIONSHIP_FLOW_Exit( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk )
{
  WBM_SEQ_Exit( p_wk->p_seq);

  GFL_MSG_Delete( p_wk->p_err_msg );
  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C�����j���[�t���[  �又��
 *
 *	@param	p_wk	���[�N

 */
//-----------------------------------------------------------------------------
void BATTLE_CHAMPIONSHIP_FLOW_Main( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk )
{
  WBM_SEQ_Main( p_wk->p_seq );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�����j���[�t���[  �I�����m
 *
 *	@param	const BATTLE_CHAMPIONSHIP_FLOW_WORK *cp_wk ���[�N
 *
 *	@return �I���R�[�h�i�񋓌^���Q�Ɓj
 */
//-----------------------------------------------------------------------------
BATTLE_CHAMPIONSHIP_FLOW_RET BATTLE_CHAMPIONSHIP_FLOW_IsEnd( const BATTLE_CHAMPIONSHIP_FLOW_WORK *cp_wk )
{ 
  if( WBM_SEQ_IsEnd( cp_wk->p_seq )  )
  { 
    return cp_wk->retcode;
  }
  return BATTLE_CHAMPIONSHIP_FLOW_RET_CONTINUE;
}

//=============================================================================
/**
 *  �V�[�P���X�֐�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�J�n
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk = p_wk_adrs;
  
  switch( p_wk->param.mode )
  { 
  case BATTLE_CHAMPIONSHIP_FLOW_MODE_MAINMENU:   //���C�����j���[����J�n
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_MainMenu );
    break;
  case BATTLE_CHAMPIONSHIP_FLOW_MODE_WIFIMENU:   //WIFI���j���[����J�n
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_WifiMenu );
    break;
  case BATTLE_CHAMPIONSHIP_FLOW_MODE_LIVEMENU:   //LIVE���j���[����J�n
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_LiveMenu );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I��
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_seqwk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_End( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  //�I��
  WBM_SEQ_End( p_seqwk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�C��
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_seqwk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeIn( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
	enum
	{	
		SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
		SEQ_END,
	};

	switch( *p_seq )
	{	
	case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_END;
		}
		break;

	case SEQ_END:
		WBM_SEQ_SetNext( p_seqwk, SEQFUNC_Start );
		break;

	default:
		GF_ASSERT(0);
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�A�E�g
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeOut( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
	enum
	{
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
		SEQ_EXIT,
	};	

	switch( *p_seq )
	{	
	case SEQ_FADEIN_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
		*p_seq	= SEQ_FADEIN_WAIT;
		break;

	case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_EXIT;
		}
		break;

	case SEQ_EXIT:
		WBM_SEQ_SetNext( p_seqwk, SEQFUNC_End );
		break;

	default:
		GF_ASSERT(0);
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���C�����j���[
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_MainMenu( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{
  enum
  { 
    SEQ_START_FIRSTMSG,
    SEQ_WAIT_FIRSTMSG,
    SEQ_START_MENU,
    SEQ_WAIT_MENU,
  };
  BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_FIRSTMSG:
    UTIL_TEXT_Print( p_wk, BC_STR_01 );
    *p_seq = SEQ_WAIT_FIRSTMSG;
    break;

  case SEQ_WAIT_FIRSTMSG:
    if( UTIL_TEXT_IsEnd( p_wk ) == TRUE )
    {
      *p_seq = SEQ_START_MENU;
    }
    break;

  case SEQ_START_MENU:
    UTIL_LIST_Create( p_wk, BC_MENU_TYPE_FIRST );
    *p_seq = SEQ_WAIT_MENU;
    break;

  case SEQ_WAIT_MENU:
    {
      const u32 ret = UTIL_LIST_Main( p_wk );
      if( ret != WBM_LIST_SELECT_NULL )
      {
        switch( ret )
        {
        case 0: //WiFi���
          WBM_SEQ_SetNext( p_seqwk, SEQFUNC_WifiMenu );
          break;
        case 1: //�C�x���g���
          WBM_SEQ_SetNext( p_seqwk, SEQFUNC_LiveMenu );
          break;
        case 2: //����������
          WBM_SEQ_SetNext( p_seqwk, SEQFUNC_Info );
          break;
        case 3: //��߂�
          UTIL_FLOW_End( p_wk, BATTLE_CHAMPIONSHIP_FLOW_RET_TITLE );
          break;
        }
        UTIL_LIST_Delete( p_wk );
      }
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�������
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Info( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_INFOMSG,
    SEQ_WAIT_INFOMSG,
  };

  BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_INFOMSG:
    UTIL_TEXT_Print( p_wk, BC_STR_02 );
    *p_seq  = SEQ_WAIT_INFOMSG;
    break;
  
    break;
  case SEQ_WAIT_INFOMSG:
    if( UTIL_TEXT_IsEnd( p_wk ) == TRUE )
    {
      WBM_SEQ_SetNext( p_seqwk, SEQFUNC_MainMenu );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	Wifi���j���[
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_WifiMenu( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_WIFIMSG,
    SEQ_WAIT_WIFIMSG,
    SEQ_START_WIFIMENU,
    SEQ_WAIT_WIFIMENU,

    SEQ_START_CANCELMSG,
    SEQ_WAIT_CANCELMSG,
    SEQ_START_CANCELMENU,
    SEQ_WAIT_CANCELMENU,
  };
  BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_WIFIMSG:
    UTIL_TEXT_Print( p_wk, BC_STR_04 );
    *p_seq = SEQ_WAIT_WIFIMSG;
    break;

  case SEQ_WAIT_WIFIMSG:
    if( UTIL_TEXT_IsEnd( p_wk ) == TRUE )
    {
      *p_seq = SEQ_START_WIFIMENU;
    }
    break;

  case SEQ_START_WIFIMENU:
    UTIL_LIST_Create( p_wk, BC_MENU_TYPE_WIFI );
    *p_seq = SEQ_WAIT_WIFIMENU;
    break;

  case SEQ_WAIT_WIFIMENU:
    {
      const u32 ret = UTIL_LIST_Main( p_wk );
      if( ret != WBM_LIST_SELECT_NULL )
      {
        switch( ret )
        {
        case 0: //���񂩂���
          if( !DS_SYSTEM_IsAvailableWireless() )
          { 
            //DS�̖����ݒ�ŒʐM�s�̂Ƃ�
            WBM_SEQ_SetNext( p_seqwk, SEQFUNC_EnableWireless );
          }
          else if( DS_SYSTEM_IsRestrictUGC() )
          { 
            //�y�A�����^���R���g���[���ő���M���ۂ��Ă���Ƃ�
            WBM_SEQ_SetNext( p_seqwk, SEQFUNC_RestrictUGC );
          }
          else
          { 
            //�ڑ��ł���Ƃ�
            UTIL_FLOW_End( p_wk, BATTLE_CHAMPIONSHIP_FLOW_RET_WIFICUP );
          }
          break;
        case 1: //�����𕷂�
          WBM_SEQ_SetNext( p_seqwk, SEQFUNC_WifiInfo );
          break;
        case 2: //�f�W�^���I��؂��݂�
          WBM_SEQ_SetNext( p_seqwk, SEQFUNC_WifiDigitalCard );
          break;
        case 3: //��߂�
          *p_seq  = SEQ_START_CANCELMSG;
          break;
        }
        UTIL_LIST_Delete( p_wk );
      }
    }
    break;

  case SEQ_START_CANCELMSG:
    UTIL_TEXT_Print( p_wk, BC_STR_05 );
    *p_seq = SEQ_WAIT_CANCELMSG;
    break;

  case SEQ_WAIT_CANCELMSG:
    if( UTIL_TEXT_IsEnd( p_wk ) == TRUE )
    {
      *p_seq = SEQ_START_CANCELMENU;
    }
    break;

  case SEQ_START_CANCELMENU:
    UTIL_LIST_Create( p_wk, BC_MENU_TYPE_YESNO );
    *p_seq = SEQ_WAIT_CANCELMENU;
    break;

  case SEQ_WAIT_CANCELMENU:
    {
      const u32 ret = UTIL_LIST_Main( p_wk );
      if( ret != WBM_LIST_SELECT_NULL )
      {
        switch( ret )
        {
        case 0: //�͂�
          WBM_SEQ_SetNext( p_seqwk, SEQFUNC_MainMenu );
          break;
        case 1: //������
          *p_seq  =SEQ_START_WIFIMSG;
          break;
        }
        UTIL_LIST_Delete( p_wk );
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	Wifi�������
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_WifiInfo( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_INFOMSG,
    SEQ_WAIT_INFOMSG,
  };

  BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_INFOMSG:
    UTIL_TEXT_Print( p_wk, BC_STR_03 );
    *p_seq  = SEQ_WAIT_INFOMSG;
    break;
  
    break;
  case SEQ_WAIT_INFOMSG:
    if( UTIL_TEXT_IsEnd( p_wk ) == TRUE )
    {
      WBM_SEQ_SetNext( p_seqwk, SEQFUNC_WifiMenu );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�f�W�^���I���
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_WifiDigitalCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_END,
  };

  BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
    {
      DIGITALCARD_CHECK_PARAM  param;
      GFL_STD_MemClear( &param, sizeof(DIGITALCARD_CHECK_PARAM) );
      param.type  = REGULATION_CARD_TYPE_WIFI;
      param.p_text  = p_wk->param.p_text;
      param.p_font  = p_wk->param.p_font;
      param.p_msg   = p_wk->param.p_msg;
      param.p_que   = p_wk->param.p_que;
      param.p_view  = p_wk->param.p_view;
      param.p_graphic = p_wk->param.p_graphic;
      param.p_gamedata  = p_wk->param.p_gamedata;
      p_wk->p_flow  = DIGITALCARD_CHECK_Init( &param, p_wk->heapID );
    }
    (*p_seq)++;
    break;

  case SEQ_MAIN:
    DIGITALCARD_CHECK_Main( p_wk->p_flow );
    if( DIGITALCARD_CHECK_IsEnd( p_wk->p_flow ) )
    { 
      (*p_seq)++;
    }
    break;

  case SEQ_END:
    DIGITALCARD_CHECK_Exit( p_wk->p_flow );
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_WifiMenu );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	LIVE�f�W�^���I���
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_LiveDigitalCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_END,
  };

  BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
    {
      DIGITALCARD_CHECK_PARAM  param;
      GFL_STD_MemClear( &param, sizeof(DIGITALCARD_CHECK_PARAM) );
      param.type  = REGULATION_CARD_TYPE_LIVE;
      param.p_text  = p_wk->param.p_text;
      param.p_font  = p_wk->param.p_font;
      param.p_msg   = p_wk->param.p_msg;
      param.p_que   = p_wk->param.p_que;
      param.p_view  = p_wk->param.p_view;
      param.p_graphic = p_wk->param.p_graphic;
      param.p_gamedata  = p_wk->param.p_gamedata;
      p_wk->p_flow  = DIGITALCARD_CHECK_Init( &param, p_wk->heapID );
    }
    (*p_seq)++;
    break;

  case SEQ_MAIN:
    DIGITALCARD_CHECK_Main( p_wk->p_flow );
    if( DIGITALCARD_CHECK_IsEnd( p_wk->p_flow ) )
    { 
      (*p_seq)++;
    }
    break;

  case SEQ_END:
    DIGITALCARD_CHECK_Exit( p_wk->p_flow );
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_LiveMenu );
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C�u���j���[���
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_LiveMenu( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_LIVEMSG,
    SEQ_WAIT_LIVEMSG,
    SEQ_START_LIVEMENU,
    SEQ_WAIT_LIVEMENU,

    SEQ_START_CANCELMSG,
    SEQ_WAIT_CANCELMSG,
    SEQ_START_CANCELMENU,
    SEQ_WAIT_CANCELMENU,
  };
  BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_LIVEMSG:
    UTIL_TEXT_Print( p_wk, BC_STR_06 );
    *p_seq = SEQ_WAIT_LIVEMSG;
    break;

  case SEQ_WAIT_LIVEMSG:
    if( UTIL_TEXT_IsEnd( p_wk ) == TRUE )
    {
      *p_seq = SEQ_START_LIVEMENU;
    }
    break;

  case SEQ_START_LIVEMENU:
    UTIL_LIST_Create( p_wk, BC_MENU_TYPE_LIVE );
    *p_seq = SEQ_WAIT_LIVEMENU;
    break;

  case SEQ_WAIT_LIVEMENU:
    {
      const u32 ret = UTIL_LIST_Main( p_wk );
      if( ret != WBM_LIST_SELECT_NULL )
      {
        switch( ret )
        {
        case 0: //���񂩂���
          if( !DS_SYSTEM_IsAvailableWireless() )
          { 
            //DS�̖����ݒ�ŒʐM�s�̂Ƃ�
            WBM_SEQ_SetNext( p_seqwk, SEQFUNC_EnableWireless );
          }
          else if( DS_SYSTEM_IsRestrictUGC() )
          { 
            //�y�A�����^���R���g���[���ő���M���ۂ��Ă���Ƃ�
            WBM_SEQ_SetNext( p_seqwk, SEQFUNC_RestrictUGC );
          }
          else
          { 
            //�ڑ��ł���Ƃ�
            UTIL_FLOW_End( p_wk, BATTLE_CHAMPIONSHIP_FLOW_RET_LIVECUP );
          }
          break;
        case 1: //�����𕷂�
          WBM_SEQ_SetNext( p_seqwk, SEQFUNC_LiveInfo );
          break;
        case 2: //�f�W�^���I��؂��݂�
          WBM_SEQ_SetNext( p_seqwk, SEQFUNC_LiveDigitalCard );
          break;
        case 3: //��߂�
          *p_seq  = SEQ_START_CANCELMSG;
          break;
        }
        UTIL_LIST_Delete( p_wk );
      }
    }
    break;

  case SEQ_START_CANCELMSG:
    UTIL_TEXT_Print( p_wk, BC_STR_05 );
    *p_seq = SEQ_WAIT_CANCELMSG;
    break;

  case SEQ_WAIT_CANCELMSG:
    if( UTIL_TEXT_IsEnd( p_wk ) == TRUE )
    {
      *p_seq = SEQ_START_CANCELMENU;
    }
    break;

  case SEQ_START_CANCELMENU:
    UTIL_LIST_Create( p_wk, BC_MENU_TYPE_YESNO );
    *p_seq = SEQ_WAIT_CANCELMENU;
    break;

  case SEQ_WAIT_CANCELMENU:
    {
      const u32 ret = UTIL_LIST_Main( p_wk );
      if( ret != WBM_LIST_SELECT_NULL )
      {
        switch( ret )
        {
        case 0: //�͂�
          WBM_SEQ_SetNext( p_seqwk, SEQFUNC_MainMenu );
          break;
        case 1: //������
          *p_seq  =SEQ_START_LIVEMSG;
          break;
        }
        UTIL_LIST_Delete( p_wk );
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	���C�u���������
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_LiveInfo( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_INFOMSG,
    SEQ_WAIT_INFOMSG,
  };

  BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_INFOMSG:
    UTIL_TEXT_Print( p_wk, BC_STR_07 );
    *p_seq  = SEQ_WAIT_INFOMSG;
    break;
  
    break;
  case SEQ_WAIT_INFOMSG:
    if( UTIL_TEXT_IsEnd( p_wk ) == TRUE )
    {
      WBM_SEQ_SetNext( p_seqwk, SEQFUNC_LiveMenu );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�y�A�����^���R���g���[���Ń��[�U�[�쐬�R���e���c�̑���M�����ۂ��Ă����ꍇ
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_RestrictUGC( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_MSG,
    SEQ_WAIT_MSG,
  };

  BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_MSG:
    //@todo
    UTIL_TEXT_PrintErr( p_wk, msg_common_wireless_off_keywait );
    *p_seq  = SEQ_WAIT_MSG;
    break;
  
    break;
  case SEQ_WAIT_MSG:
    if( UTIL_TEXT_IsEnd( p_wk ) == TRUE )
    {
      WBM_SEQ_SetNext( p_seqwk, SEQFUNC_MainMenu );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	DS�̖����ʐM�ݒ�ŒʐM���ׂɂ��Ă����ꍇ
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_EnableWireless( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_MSG,
    SEQ_WAIT_MSG,
  };

  BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_MSG:
    UTIL_TEXT_PrintErr( p_wk, msg_common_wireless_off_keywait );
    *p_seq  = SEQ_WAIT_MSG;
    break;
  
  case SEQ_WAIT_MSG:
    if( UTIL_TEXT_IsEnd( p_wk ) == TRUE )
    {
      WBM_SEQ_SetNext( p_seqwk, SEQFUNC_MainMenu );
    }
    break;
  }
}
//=============================================================================
/**
 *  �֗��֐�
 */
//=============================================================================
//-------------------------------------
///	���X�g
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g��\��
 *
 *	@param	BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk ���[�N
 *	@param	type                                �\�����X�g�^�C�v
 */
//-----------------------------------------------------------------------------
static void UTIL_LIST_Create( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk, BC_MENU_TYPE type )
{ 
  enum
  { 
    POS_CENTER,
    POS_RIGHT_DOWN,
  } pos;

  u8 x,y,w,h;
  REGULATION_SAVEDATA*  p_sv = SaveData_GetRegulationSaveData( GAMEDATA_GetSaveControlWork(p_wk->param.p_gamedata) );

  WBM_LIST_SETUP  setup;
  GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
  setup.p_msg   = p_wk->param.p_msg;
  setup.p_font  = p_wk->param.p_font;
  setup.p_que   = p_wk->param.p_que;


  switch( type )
  { 
  case BC_MENU_TYPE_FIRST:
    setup.strID[0]= BC_SELECT_01;
    setup.strID[1]= BC_SELECT_02;
    setup.strID[2]= BC_SELECT_04;
    setup.strID[3]= BC_SELECT_05;
    setup.list_max= 4;
    pos = POS_CENTER;
    break;
  case BC_MENU_TYPE_WIFI:
    {
      const REGULATION_CARDDATA* cp_reg  = RegulationSaveData_GetRegulationCard( p_sv, REGULATION_CARD_TYPE_WIFI );

      setup.strID[0]= BC_SELECT_06;
      setup.strID[1]= BC_SELECT_04;
      if( cp_reg->status == DREAM_WORLD_MATCHUP_SIGNUP || cp_reg->status == DREAM_WORLD_MATCHUP_ENTRY )
      { 
        setup.strID[2]= BC_SELECT_09;
      }
      else
      { 
        setup.strID[2]= BC_SELECT_03;
      }
    }
    setup.strID[3]= BC_SELECT_05;
    setup.list_max= 4;
    pos = POS_CENTER;
    break;
  case BC_MENU_TYPE_LIVE:
    {
      const REGULATION_CARDDATA* cp_reg  = RegulationSaveData_GetRegulationCard( p_sv, REGULATION_CARD_TYPE_LIVE );

      setup.strID[0]= BC_SELECT_06;
      setup.strID[1]= BC_SELECT_04;
      if( cp_reg->status == DREAM_WORLD_MATCHUP_SIGNUP || cp_reg->status == DREAM_WORLD_MATCHUP_ENTRY )
      { 
        setup.strID[2]= BC_SELECT_09;
      }
      else
      { 
        setup.strID[2]= BC_SELECT_03;
      }
      setup.strID[3]= BC_SELECT_05;
      setup.list_max= 4;
      pos = POS_CENTER;
    }
    break;
  case BC_MENU_TYPE_YESNO:
    setup.strID[0]= BC_SELECT_07;
    setup.strID[1]= BC_SELECT_08;
    setup.list_max= 2;
    pos = POS_RIGHT_DOWN;
    break;
  }
  setup.frm     = BG_FRAME_M_TEXT;
  setup.font_plt= PLT_FONT_M;
  setup.frm_plt = PLT_LIST_M;
  setup.frm_chr = CGR_OFS_M_LIST;


  switch( pos )
  { 
  case POS_CENTER:
    w  = 28;
    h  = setup.list_max * 2;
    x  = 32 / 2  - w /2; 
    y  = (24-6) /2  - h /2;
    break;
  case POS_RIGHT_DOWN:
    w  = 12;
    h  = setup.list_max * 2;
    x  = 32 - w - 1; //1�̓t���[����
    y  = 24 - h - 1 - 6; //�P�͎����̃t���[������6�̓e�L�X�g��
    break;
  }

  p_wk->p_list  = WBM_LIST_InitEx( &setup, x, y, w, h, p_wk->heapID );
  p_wk->list_type = type;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g���폜
 *
 *	@param	BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void UTIL_LIST_Delete( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk )
{ 
  WBM_LIST_Exit( p_wk->p_list );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g����
 *
 *	@param	BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk ���[�N
 *
 *	@retval �I�����C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static u32 UTIL_LIST_Main( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk )
{ 
  u32 ret = WBM_LIST_Main( p_wk->p_list );

  if( ret == WBM_LIST_SELECT_CALNCEL )
  { 
    if( p_wk->list_type == BC_MENU_TYPE_YESNO )
    { 
      ret = 1;  //NO�ɂ���
    }
    else
    { 
      //�L�����Z�������Ȃ�
      ret = WBM_LIST_SELECT_NULL;
    }
  }

  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g�����\��
 *
 *	@param	BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk ���[�N
 *	@param	strID                               ����
 */
//-----------------------------------------------------------------------------
static void UTIL_TEXT_Print( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk, u32 strID )
{ 
  WBM_TEXT_Print( p_wk->param.p_text, p_wk->param.p_msg, strID, WBM_TEXT_TYPE_STREAM );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g�����\��  �G���[��
 *
 *	@param	BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk ���[�N
 *	@param	strID                               ����
 */
//-----------------------------------------------------------------------------
static void UTIL_TEXT_PrintErr( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk, u32 strID )
{ 

  WBM_TEXT_Print( p_wk->param.p_text, p_wk->p_err_msg, strID, WBM_TEXT_TYPE_STREAM );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g�\���҂�
 *
 *	@param	BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk ���[�N
 *
 *	@return TRUE�����`�抮��  FALSE�����`�撆
 */
//-----------------------------------------------------------------------------
static BOOL UTIL_TEXT_IsEnd( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk )
{ 
  return WBM_TEXT_IsEnd( p_wk->param.p_text );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �t���[�I��
 *
 *	@param	BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk ���[�N
 *	@param	ret                                 �߂�l
 */
//-----------------------------------------------------------------------------
static void UTIL_FLOW_End( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk, BATTLE_CHAMPIONSHIP_FLOW_RET ret )
{ 
  p_wk->retcode = ret;
  WBM_SEQ_SetNext( p_wk->p_seq, SEQFUNC_End );
}
