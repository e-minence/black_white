//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		livebattlematch_flow.c
 *	@brief  ���C�u���t���[
 *	@author	Toru=Nagihashi
 *	@date		2010.03.06
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
#include "poke_tool/poke_regulation.h"
#include "sound/pm_sndsys.h"
#include "system/net_err.h"

//�v���Z�X


//�A�[�J�C�u
#include "msg/msg_battle_championship.h"

//�l�b�g
#include "net/network_define.h"
#include "net/dreamworld_netdata.h"

//WIFIBATTLEMATCH�̃��W���[��
#include "net_app/wifi_match/wifibattlematch_util.h"
#include "net_app/wifi_match/wifibattlematch_graphic.h"
#include "net_app/wifi_match/wifibattlematch_view.h"

//�O�����J
#include "livebattlematch_flow.h"

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
///	���C�u���[�N
//=====================================
struct _LIVEBATTLEMATCH_FLOW_WORK
{ 
  //�V�[�P���X���䃂�W���[��
  WBM_SEQ_WORK                  *p_seq;

  //�I�����\�����W���[��
  WBM_LIST_WORK                 *p_list;
  u32                           list_type;

  //�߂�l
  LIVEBATTLEMATCH_FLOW_RET      retcode;

  //����
  LIVEBATTLEMATCH_FLOW_PARAM    param;

  //�q�[�vID
  HEAPID                        heapID;

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

//-------------------------------------
///	�֗��֐�
//=====================================
//���X�g
typedef enum
{ 
  LVM_MENU_TYPE_YESNO,
}LVM_MENU_TYPE;
static void UTIL_LIST_Create( LIVEBATTLEMATCH_FLOW_WORK *p_wk, LVM_MENU_TYPE type );
static void UTIL_LIST_Delete( LIVEBATTLEMATCH_FLOW_WORK *p_wk );
static u32 UTIL_LIST_Main( LIVEBATTLEMATCH_FLOW_WORK *p_wk );

//�e�L�X�g
static void UTIL_TEXT_Print( LIVEBATTLEMATCH_FLOW_WORK *p_wk, u32 strID );
static BOOL UTIL_TEXT_IsEnd( LIVEBATTLEMATCH_FLOW_WORK *p_wk );

//�t���[�I��
static void UTIL_FLOW_End( LIVEBATTLEMATCH_FLOW_WORK *p_wk, LIVEBATTLEMATCH_FLOW_RET ret );


//=============================================================================
/**
 *					�f�[�^
*/
//=============================================================================
//=============================================================================
/**
 *					�O�����J
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���C�u���t���[  ����
 *
 *	@param	const LIVEBATTLEMATCH_FLOW_PARAM *cp_param  ����
 *	@param	heapID  �q�[�vID 
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
LIVEBATTLEMATCH_FLOW_WORK *LIVEBATTLEMATCH_FLOW_Init( const LIVEBATTLEMATCH_FLOW_PARAM *cp_param, HEAPID heapID )
{ 
  LIVEBATTLEMATCH_FLOW_WORK *p_wk;

  //���[�N�쐬
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(LIVEBATTLEMATCH_FLOW_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(LIVEBATTLEMATCH_FLOW_WORK) );
  p_wk->param   = *cp_param;
  p_wk->heapID  = heapID;

  //���W���[���쐬
  p_wk->p_seq = WBM_SEQ_Init( p_wk, SEQFUNC_Start, heapID );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���C�u���t���[  �j��
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void LIVEBATTLEMATCH_FLOW_Exit( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{ 
  //���W���[���j��
  WBM_SEQ_Exit( p_wk->p_seq);

  //���[�N�j��
  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���C�u���t���[  ���C������
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void LIVEBATTLEMATCH_FLOW_Main( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{ 
  WBM_SEQ_Main( p_wk->p_seq );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�u���t���[  �I�����m
 *
 *	@param	const LIVEBATTLEMATCH_FLOW_WORK *cp_wk  ���[�N
 *
 *	@return �I���R�[�h�i�ڍׂ͗񋓌^���Q�Ɓj
 */
//-----------------------------------------------------------------------------
LIVEBATTLEMATCH_FLOW_RET LIVEBATTLEMATCH_FLOW_IsEnd( const LIVEBATTLEMATCH_FLOW_WORK *cp_wk )
{ 
  if( WBM_SEQ_IsEnd( cp_wk->p_seq )  )
  { 
    return cp_wk->retcode;
  }
  return LIVEBATTLEMATCH_FLOW_RET_CONTINUE;
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
  LIVEBATTLEMATCH_FLOW_WORK *p_wk = p_wk_adrs;
  
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
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 *	@param	type                                �\�����X�g�^�C�v
 */
//-----------------------------------------------------------------------------
static void UTIL_LIST_Create( LIVEBATTLEMATCH_FLOW_WORK *p_wk, LVM_MENU_TYPE type )
{ 
  enum
  { 
    POS_CENTER,
    POS_RIGHT_DOWN,
  } pos;

  u8 x,y,w,h;

  WBM_LIST_SETUP  setup;
  GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
  setup.p_msg   = p_wk->param.p_msg;
  setup.p_font  = p_wk->param.p_font;
  setup.p_que   = p_wk->param.p_que;


  switch( type )
  { 
  case LVM_MENU_TYPE_YESNO:
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
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void UTIL_LIST_Delete( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{ 
  WBM_LIST_Exit( p_wk->p_list );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g����
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 *
 *	@retval �I�����C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static u32 UTIL_LIST_Main( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{ 
  u32 ret = WBM_LIST_Main( p_wk->p_list );

  if( ret == WBM_LIST_SELECT_CALNCEL )
  { 
    if( p_wk->list_type == LVM_MENU_TYPE_YESNO )
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
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 *	@param	strID                               ����
 */
//-----------------------------------------------------------------------------
static void UTIL_TEXT_Print( LIVEBATTLEMATCH_FLOW_WORK *p_wk, u32 strID )
{ 
  WBM_TEXT_Print( p_wk->param.p_text, p_wk->param.p_msg, strID, WBM_TEXT_TYPE_STREAM );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g�\���҂�
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 *
 *	@return TRUE�����`�抮��  FALSE�����`�撆
 */
//-----------------------------------------------------------------------------
static BOOL UTIL_TEXT_IsEnd( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{ 
  return WBM_TEXT_IsEnd( p_wk->param.p_text );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �t���[�I��
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 *	@param	ret                                 �߂�l
 */
//-----------------------------------------------------------------------------
static void UTIL_FLOW_End( LIVEBATTLEMATCH_FLOW_WORK *p_wk, LIVEBATTLEMATCH_FLOW_RET ret )
{ 
  p_wk->retcode = ret;
  WBM_SEQ_SetNext( p_wk->p_seq, SEQFUNC_End );
}
