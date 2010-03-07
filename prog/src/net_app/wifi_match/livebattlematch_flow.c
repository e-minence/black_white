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

static void SEQFUNC_RecvCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_Register( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_StartCup( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_BtlAfter( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_RecAfter( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

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

  switch( p_wk->param.mode )
  { 
  case LIVEBATTLEMATCH_FLOW_MODE_START:  //�ŏ�����J�n
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_RecvCard );
    break;
  case LIVEBATTLEMATCH_FLOW_MODE_MENU:    //���C�����j���[����J�n
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_StartCup );
    break;
  case LIVEBATTLEMATCH_FLOW_MODE_BTL:    //�o�g���ォ��J�n
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_BtlAfter );
    break;
  case LIVEBATTLEMATCH_FLOW_MODE_REC:    //�^��ォ��J�n
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_RecAfter );
    break;
  default:
    GF_ASSERT(0);
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
 *	@brief	�I��؎�M�V�[�P���X
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_RecvCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHECK_CARD,       //�I��؂̑��݃`�F�b�N
    SEQ_START_MSG_NOLOCK, //���łɑI��؂������āA�o�g���{�b�N�X�����b�N����Ă��Ȃ�
    SEQ_START_MSG_LOCK,   //���łɑI��؂������āA�o�g���{�b�N�X�����b�N����Ă���

    SEQ_START_MSG_RECVCARD, //�I��؎󂯎��
    SEQ_START_RECVCARD,
    SEQ_WAIT_RECVCARD,

    SEQ_CHECK_RECVCARD, //�󂯎�����I��؂��`�F�b�N

    SEQ_START_SAVE, //�I��؂������������̂ŃT�C���A�b�v���ăZ�[�u
    SEQ_WAIT_SAVE,

    SEQ_START_MSG_RETIRE,   //���^�C�A���������󂯎���Ă���,
    SEQ_START_MSG_FINISH,   //�I�����������󂯎���Ă���,

    SEQ_SUCCESS_END,      //���������̂Ŏ���
    SEQ_DIRTY_END,        //���s�����̂Ŗ߂�

    SEQ_WAIT_MSG,       //���b�Z�[�W�҂�
  };

  LIVEBATTLEMATCH_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_CHECK_CARD:       //�I��؂̑��݃`�F�b�N
    break;
  case SEQ_START_MSG_NOLOCK: //���łɑI��؂������āA�o�g���{�b�N�X�����b�N����Ă��Ȃ�
    break;
  case SEQ_START_MSG_LOCK:   //���łɑI��؂������āA�o�g���{�b�N�X�����b�N����Ă���
    break;

  case SEQ_START_MSG_RECVCARD: //�I��؎󂯎��
    break;
  case SEQ_START_RECVCARD:
    break;
  case SEQ_WAIT_RECVCARD:
    break;

  case SEQ_CHECK_RECVCARD: //�󂯎�����I��؂��`�F�b�N
    break;

  case SEQ_START_SAVE: //�I��؂������������̂ŃT�C���A�b�v���ăZ�[�u
    break;
  case SEQ_WAIT_SAVE:
    break;

  case SEQ_START_MSG_RETIRE:   //���^�C�A���������󂯎���Ă���:
    break;
  case SEQ_START_MSG_FINISH:   //�I�����������󂯎���Ă���:
    break;

  case SEQ_SUCCESS_END:      //���������̂Ŏ���
    break;
  case SEQ_DIRTY_END:        //���s�����̂Ŗ߂�
    break;

  case SEQ_WAIT_MSG:       //���b�Z�[�W�҂�
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	���o�^�V�[�P���X
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Register( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_DRAW_CARD,  //�I��؂ƃo�g���{�b�N�X�\��
    SEQ_WAIT_DRAW_CARD,
    SEQ_START_MSG_REG,     //�o�g���{�b�N�X�̃|�P������o�^���܂�
    SEQ_START_LIST_REG_YESNO,
    SEQ_WAIT_LIST_REG_YESNO,

    SEQ_START_MSG_CANCEL,     //�Q���|�P�����̓o�^����߂܂���
    SEQ_START_LIST_CANCEL_YESNO,
    SEQ_WAIT_LIST_CANCEL_YESNO,

    SEQ_MOVE_BTLBOX,      //�o�g���{�b�N�X���E
    SEQ_CHECK_REG,        //�o�g���{�b�N�X�̃|�P���������M�����[�V�����ƈ�v���邩�`�F�b�N
    SEQ_START_MSG_DIRTY_REG,  //���M�����[�V�����ƈ�v���Ȃ�����
    SEQ_LOCK_BTLBOX,      //�o�g���{�b�N�X�����b�N

    SEQ_START_SAVE,        //�o�^�����Z�[�u,
    SEQ_WAIT_SAVE,

    SEQ_START_MSG_COMPLATE, //�������b�Z�[�W

    SEQ_SUCCESS_END,      //���������̂Ŏ���
    SEQ_DIRTY_END,        //���s�����̂Ŗ߂�

    SEQ_WAIT_MSG,       //���b�Z�[�W�҂� 
  };

  LIVEBATTLEMATCH_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_DRAW_CARD:  //�I��؂ƃo�g���{�b�N�X�\��
    break;
  case SEQ_WAIT_DRAW_CARD:
    break;
  case SEQ_START_MSG_REG:     //�o�g���{�b�N�X�̃|�P������o�^���܂�
    break;
  case SEQ_START_LIST_REG_YESNO:
    break;
  case SEQ_WAIT_LIST_REG_YESNO:
    break;

  case SEQ_START_MSG_CANCEL:     //�Q���|�P�����̓o�^����߂܂���
    break;
  case SEQ_START_LIST_CANCEL_YESNO:
    break;
  case SEQ_WAIT_LIST_CANCEL_YESNO:
    break;

  case SEQ_MOVE_BTLBOX:      //�o�g���{�b�N�X���E
    break;
  case SEQ_CHECK_REG:        //�o�g���{�b�N�X�̃|�P���������M�����[�V�����ƈ�v���邩�`�F�b�N
    break;
  case SEQ_START_MSG_DIRTY_REG:  //���M�����[�V�����ƈ�v���Ȃ�����
    break;
  case SEQ_LOCK_BTLBOX:      //�o�g���{�b�N�X�����b�N
    break;

  case SEQ_START_SAVE:        //�o�^�����Z�[�u:
    break;
  case SEQ_WAIT_SAVE:
    break;

  case SEQ_START_MSG_COMPLATE: //�������b�Z�[�W
    break;

  case SEQ_SUCCESS_END:      //���������̂Ŏ���
    break;
  case SEQ_DIRTY_END:        //���s�����̂Ŗ߂�
    break;

  case SEQ_WAIT_MSG:       //���b�Z�[�W�҂� 
    break;
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief	���J�n�V�[�P���X
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_StartCup( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_DRAW_CARD,  //�I��ؕ\��
    SEQ_WAIT_DRAW_CARD,
    
    SEQ_START_MSG_MENU,   //���C�����j���[
    SEQ_START_LIST_MENU,
    SEQ_WAIT_LIST_MENU,

    SEQ_START_MSG_UNREG,  //�Q������������
    SEQ_START_LIST_UNREG,
    SEQ_WAIT_LIST_UNREG,
    SEQ_START_MSG_DECIDE, //�{���Ɂ`
    SEQ_START_LIST_DECIDE,
    SEQ_WAIT_LIST_DECIDE,
    SEQ_UNREGISTER,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,
    SEQ_START_MSG_UNLOCK,
  
    SEQ_START_MSG_LOOKBACK, //�U��Ԃ�

    SEQ_LOOKBACK_END,     //�U��Ԃ邽�߂ɏI��
    SEQ_MATCHING_END,     //�}�b�`���O��
    SEQ_UNREGISTER_END,   //�������ďI������

    SEQ_WAIT_MSG,         //���b�Z�[�W�҂� 
  };

  LIVEBATTLEMATCH_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_DRAW_CARD:  //�I��ؕ\��
		break;
  case SEQ_WAIT_DRAW_CARD:
		break;
    
  case SEQ_START_MSG_MENU:   //���C�����j���[
		break;
  case SEQ_START_LIST_MENU:
		break;
  case SEQ_WAIT_LIST_MENU:
		break;

  case SEQ_START_MSG_UNREG:  //�Q������������
		break;
  case SEQ_START_LIST_UNREG:
		break;
  case SEQ_WAIT_LIST_UNREG:
		break;
  case SEQ_START_MSG_DECIDE: //�{���Ɂ`
		break;
  case SEQ_START_LIST_DECIDE:
		break;
  case SEQ_WAIT_LIST_DECIDE:
		break;
  case SEQ_UNREGISTER:
		break;
  case SEQ_START_SAVE:
		break;
  case SEQ_WAIT_SAVE:
		break;
  case SEQ_START_MSG_UNLOCK:
		break;
  
  case SEQ_START_MSG_LOOKBACK: //�U��Ԃ�
		break;

  case SEQ_LOOKBACK_END:     //�U��Ԃ邽�߂ɏI��
		break;
  case SEQ_MATCHING_END:     //�}�b�`���O��
		break;
  case SEQ_UNREGISTER_END:   //�������ďI������
		break;

  case SEQ_WAIT_MSG:         //���b�Z�[�W�҂� 
		break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�`���O�����V�[�P���X
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_MSG_MATCHING, //�}�b�`���O�J�n
    SEQ_WAIT_MATCHING,      //�}�b�`���O��

    SEQ_START_MSG_CANCEL_MATCHING, //�}�b�`���O��߂�
    SEQ_START_CANCEL_MATCHING,
    SEQ_WAIT_CANCEL_MATCHING,

    SEQ_CHECK_MATCH,      //�}�b�`���O����
    SEQ_START_MSG_NOCUP,  //���Ⴄ
    SEQ_START_MSG_OK,     //�}�b�`���O���肪���������I
    SEQ_START_DRAW_OTHERCARD,
    SEQ_WAIT_DRAW_OTHERCARD,
    SEQ_WAIT_SYNC,
    SEQ_START_MSG_WAIT,
    SEQ_WAIT_KEY,

    SEQ_BATTLE_END,       //�o�g����
    SEQ_LIST_END,         //���X�g��

    SEQ_WAIT_MSG,         //���b�Z�[�W�҂� 
  };

  LIVEBATTLEMATCH_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_MSG_MATCHING: //�}�b�`���O�J�n
		break;
  case SEQ_WAIT_MATCHING:      //�}�b�`���O��
		break;

  case SEQ_START_MSG_CANCEL_MATCHING: //�}�b�`���O��߂�
		break;
  case SEQ_START_CANCEL_MATCHING:
		break;
  case SEQ_WAIT_CANCEL_MATCHING:
		break;

  case SEQ_CHECK_MATCH:      //�}�b�`���O����
		break;
  case SEQ_START_MSG_NOCUP:  //���Ⴄ
		break;
  case SEQ_START_MSG_OK:     //�}�b�`���O���肪���������I
		break;
  case SEQ_START_DRAW_OTHERCARD:
		break;
  case SEQ_WAIT_DRAW_OTHERCARD:
		break;
  case SEQ_WAIT_SYNC:
		break;
  case SEQ_START_MSG_WAIT:
		break;
  case SEQ_WAIT_KEY:
		break;

  case SEQ_BATTLE_END:       //�o�g����
		break;
  case SEQ_LIST_END:         //���X�g��
		break;

  case SEQ_WAIT_MSG:         //���b�Z�[�W�҂� 
		break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�퓬��V�[�P���X
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_BtlAfter( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_MSG_SAVE,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,
    SEQ_REC_END,  //�^���

    SEQ_WAIT_MSG,         //���b�Z�[�W�҂� 
  };

  LIVEBATTLEMATCH_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  {
  case SEQ_START_MSG_SAVE:
		break;
  case SEQ_START_SAVE:
		break;
  case SEQ_WAIT_SAVE:
		break;
  case SEQ_REC_END:  //�^���
		break;

  case SEQ_WAIT_MSG:         //���b�Z�[�W�҂� 
		break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�^���V�[�P���X
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_RecAfter( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHECK_BTLCNT, //�K��񐔐����
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,

    SEQ_END,      //���I��
    SEQ_RETURN,   //�܂��킦��

    SEQ_WAIT_MSG,         //���b�Z�[�W�҂� 
  };
  LIVEBATTLEMATCH_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_CHECK_BTLCNT: //�K��񐔐����
		break;
  case SEQ_START_SAVE:
		break;
  case SEQ_WAIT_SAVE:
		break;

  case SEQ_END:      //���I��
		break;
  case SEQ_RETURN:   //�܂��킦��
		break;

  case SEQ_WAIT_MSG:         //���b�Z�[�W�҂� 
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
