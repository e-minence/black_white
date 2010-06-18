//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_battle.c
 *	@brief
 *	@author		Toru=Nagihashi
 *	@data		2010.02.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	���C�u����
#include <gflib.h>

//	�V�X�e��
#include "system/main.h"
#include "gamesystem/game_data.h"
#include "gamesystem/btl_setup.h"
#include "poke_tool/pokeparty.h"
#include "sound/pm_sndsys.h"

//�l�b�g
#include "net/dwc_rap.h"
#include "net/dwc_error.h"

//WIFI�o�g���}�b�`�̃��W���[��
#include "wifibattlematch_net.h"

//�O�����J
#include "wifibattlematch_battle.h"

//-------------------------------------
///	DEBUG
//=====================================
#ifdef PM_DEBUG

#if defined(DEBUG_ONLY_FOR_toru_nagihashi) || defined(DEBUG_ONLY_FOR_shimoyamada)
#define WBM_SYS_PRINT_ON
#endif

#endif //PM_DEBUG

#ifdef WBM_SYS_PRINT_ON
#define WBM_BTL_Printf(...)  OS_TFPrintf( 3, __VA_ARGS__ )
#else
#define WBM_BTL_Printf(...) /*  */
#endif

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define WBM_BTL_ERROR_TIMEOUT (60*30)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�o�g���ڑ��v���Z�X���C�����[�N
//=====================================
typedef struct 
{
  GFL_PROCSYS *p_procsys;
  u32         cnt;
  BOOL        is_battle_overlay;
  BOOL        is_add_cmd_tbl;
} WIFIBATTLEMATCH_BATTLELINK_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT WIFIBATTLEMATCH_BATTLELINK_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_BATTLELINK_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_BATTLELINK_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
const GFL_PROC_DATA	WifiBattleMatch_BattleLink_ProcData =
{	
	WIFIBATTLEMATCH_BATTLELINK_PROC_Init,
	WIFIBATTLEMATCH_BATTLELINK_PROC_Main,
	WIFIBATTLEMATCH_BATTLELINK_PROC_Exit,
};

//=============================================================================
/**
 *					PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`���	�o�g���ڑ��v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_p_wk						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_BATTLELINK_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
  WIFIBATTLEMATCH_BATTLELINK_WORK  *p_wk;
  WIFIBATTLEMATCH_BATTLELINK_PARAM *p_param  = p_param_adrs;

  //�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIBATTLEMATCH_SUB, 0x150 );

  //�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(WIFIBATTLEMATCH_BATTLELINK_WORK), HEAPID_WIFIBATTLEMATCH_SUB );
	GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_BATTLELINK_WORK) );	
  p_param->result = WIFIBATTLEMATCH_BATTLELINK_RESULT_SUCCESS;

  //���W���[��������
  p_wk->p_procsys = GFL_PROC_LOCAL_boot( HEAPID_WIFIBATTLEMATCH_SUB );
  PMSND_FadeOutBGM( 16 );

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`���	�o�g���ڑ��v���Z�X�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_p_wk						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_BATTLELINK_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  WIFIBATTLEMATCH_BATTLELINK_WORK  *p_wk     = p_wk_adrs;
  WIFIBATTLEMATCH_BATTLELINK_PARAM *p_param  = p_param_adrs;

  if( p_wk->is_battle_overlay )
  {
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );
    p_wk->is_battle_overlay = FALSE;
  }

  if( p_wk->is_add_cmd_tbl )
  {
    GFL_NET_DelCommandTable(GFL_NET_CMD_BATTLE);
    p_wk->is_add_cmd_tbl  = FALSE;
  }

  //���W���[���j��
  GFL_PROC_LOCAL_Exit( p_wk->p_procsys );

  //�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

  //�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_WIFIBATTLEMATCH_SUB );

  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`���	�o�g���ڑ����C���v���Z�X����
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_p_wk						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_BATTLELINK_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_START_DEMO = 0,  ///< �o�g���O�f���Ăяo��
    SEQ_WAIT_START_DEMO,
    SEQ_BATTLE_TIMING_INIT,
    SEQ_BATTLE_TIMING_WAIT,
    SEQ_BATTLE_INIT,
    SEQ_BATTLE_WAIT,
    SEQ_BATTLE_END,
    SEQ_CALL_END_DEMO,        ///< �o�g����f���Ăяo��
    SEQ_WAIT_END_DEMO,
    SEQ_CALL_BTL_REC_SEL,     ///< �ʐM�ΐ��̘^��I�����
    SEQ_END,

    SEQ_ERROR_WAIT,
  };

  WIFIBATTLEMATCH_BATTLELINK_WORK  *p_wk     = p_wk_adrs;
  WIFIBATTLEMATCH_BATTLELINK_PARAM *p_param  = p_param_adrs;
  GFL_PROC_MAIN_STATUS status;

  status = GFL_PROC_LOCAL_Main( p_wk->p_procsys );

  switch (*p_seq) {
  case SEQ_CALL_START_DEMO:
    PMSND_PlayBGM( p_param->p_btl_setup_param->musicDefault );
    // �ʐM�o�g���O�f���Ăяo��
    {
      p_param->p_demo_param->type = COMM_BTL_DEMO_TYPE_NORMAL_START;
      GFL_PROC_LOCAL_CallProc( p_wk->p_procsys, FS_OVERLAY_ID( comm_btl_demo ), &CommBtlDemoProcData, p_param->p_demo_param);
    }
    (*p_seq) = SEQ_WAIT_START_DEMO;
    break;
  
  case SEQ_WAIT_START_DEMO:
    if ( status != GFL_PROC_MAIN_VALID )
    {
      (*p_seq) = SEQ_BATTLE_TIMING_INIT;
    }
    break;
  
  case SEQ_BATTLE_TIMING_INIT:
    {
      GFL_OVERLAY_Load( FS_OVERLAY_ID( battle ) );
      p_wk->is_battle_overlay = TRUE;
      GFL_NET_AddCommandTable(GFL_NET_CMD_BATTLE, BtlRecvFuncTable, BTL_NETFUNCTBL_ELEMS, NULL);
      p_wk->is_add_cmd_tbl  = TRUE;
      GFL_NET_HANDLE_TimeSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), 200, WB_NET_WIFIMATCH );
      WBM_BTL_Printf("�퓬�p�ʐM�R�}���h�e�[�u����Add�����̂œ������\n");
      (*p_seq) = SEQ_BATTLE_TIMING_WAIT;
    }
    break;
  case SEQ_BATTLE_TIMING_WAIT:
    if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(), 200, WB_NET_WIFIMATCH ) )
    {
      WBM_BTL_Printf("�퓬�p�ʐM�R�}���h�e�[�u����Add��̓�����芮��\n");
      (*p_seq) = SEQ_BATTLE_INIT;
    }
    break;
  case SEQ_BATTLE_INIT:
    GFL_PROC_LOCAL_CallProc(p_wk->p_procsys, NO_OVERLAY_ID, &BtlProcData, p_param->p_btl_setup_param);

    (*p_seq) = SEQ_BATTLE_WAIT;
    break;
  case SEQ_BATTLE_WAIT:
    if ( status != GFL_PROC_MAIN_VALID )
    {
      (*p_seq) = SEQ_BATTLE_END;
    }
    break;
  case SEQ_BATTLE_END:
    WBM_BTL_Printf("�o�g������\n");
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );
    p_wk->is_battle_overlay = FALSE;
    p_wk->is_add_cmd_tbl  = FALSE;

    (*p_seq) = SEQ_CALL_END_DEMO;
    break;
  case SEQ_CALL_END_DEMO:
    // �ʐM�o�g����f���Ăяo��
    {
      BOOL is_next = TRUE;

      // �}���`�o�g������
      if( p_param->p_btl_setup_param->multiMode == 0 )
      {
        p_param->p_demo_param->type = COMM_BTL_DEMO_TYPE_NORMAL_END;
      }
      else
      {
        p_param->p_demo_param->type = COMM_BTL_DEMO_TYPE_MULTI_END;
      }

      OS_TFPrintf( 3, "�f���퓬���� %d", p_param->p_btl_setup_param->result );
      
      // ���s�ݒ�
      switch( p_param->p_btl_setup_param->result )
      {
      case BTL_RESULT_RUN_ENEMY:
      case BTL_RESULT_WIN :
        p_param->p_demo_param->result = COMM_BTL_DEMO_RESULT_WIN;
        break;
      case BTL_RESULT_LOSE :
      case BTL_RESULT_RUN :
        p_param->p_demo_param->result = COMM_BTL_DEMO_RESULT_LOSE;
        break;
      case BTL_RESULT_DRAW :
        p_param->p_demo_param->result = COMM_BTL_DEMO_RESULT_DRAW;
        break;

      case BTL_RESULT_COMM_ERROR:

        //�����ɗ����牺�L�G���[�`�F�b�N�Ɉ���������
        OS_TPrintf( "�o�g���G���[�I���I" );
        is_next = FALSE;
        p_wk->cnt = 0;
        (*p_seq)  = SEQ_ERROR_WAIT;
        break;

      default : 
        p_param->p_demo_param->result = COMM_BTL_DEMO_RESULT_DRAW;
      }

      //�{�[������
      {
        int max, tr_no, my_pos, my_party_start_id, enemy_party_start_id, client_no, party_no;

        
        if(p_param->p_btl_setup_param->multiMode == 0){
          max = COMM_BTL_DEMO_TRDATA_B;
          my_party_start_id = COMM_BTL_DEMO_TRDATA_A;
          enemy_party_start_id = COMM_BTL_DEMO_TRDATA_B;
        }
        else{
          max = COMM_BTL_DEMO_TRDATA_D;
          my_party_start_id = COMM_BTL_DEMO_TRDATA_A;
          enemy_party_start_id = COMM_BTL_DEMO_TRDATA_C;
        }
        
        my_pos = p_param->p_btl_setup_param->commPos;
        for(client_no = 0; client_no <= max; client_no++){
          OS_TPrintf("zzz client_no = %d party_no = ", client_no);
          for(party_no = 0; party_no < 6; party_no++){
            OS_TPrintf("%d ", p_param->p_btl_setup_param->party_state[client_no][party_no]);
          }
          OS_TPrintf("\n");
        }

        my_pos = p_param->p_btl_setup_param->commPos;
        for(tr_no = 0; tr_no <= max; tr_no++){
          if((tr_no & 1) == (my_pos & 1)){  //�����p�[�e�B
            if(my_pos & 1){
              client_no = BTL_CLIENT_ENEMY1 + (tr_no & 2);
            }
            else{
              client_no = BTL_CLIENT_PLAYER + (tr_no & 2);
            }
            for(party_no = 0; party_no < DEMO_POKEPARTY_MAX; party_no++){
              p_param->p_demo_param->trainer_data[my_party_start_id + (tr_no >> 1)].party_state[party_no] = p_param->p_btl_setup_param->party_state[client_no][party_no];
            }
          }
          else{ //�G�p�[�e�B
            if(my_pos & 1){
              client_no = BTL_CLIENT_PLAYER + (tr_no & 2);
            }
            else{
              client_no = BTL_CLIENT_ENEMY1 + (tr_no & 2);
            }
            for(party_no = 0; party_no < DEMO_POKEPARTY_MAX; party_no++){
              p_param->p_demo_param->trainer_data[enemy_party_start_id + (tr_no >> 1)].party_state[party_no] = p_param->p_btl_setup_param->party_state[client_no][party_no];
            }
          }
        }
      }



      if( is_next )
      {
        GFL_PROC_LOCAL_CallProc(p_wk->p_procsys, FS_OVERLAY_ID( comm_btl_demo ), &CommBtlDemoProcData, p_param->p_demo_param);
        (*p_seq) = SEQ_WAIT_END_DEMO;
      }
    }
    break;
  case SEQ_WAIT_END_DEMO:
    if ( status != GFL_PROC_MAIN_VALID )
    {
      (*p_seq) = SEQ_END;
    }
    break;

  case SEQ_END:
    return GFL_PROC_RES_FINISH;

  case SEQ_ERROR_WAIT:
    //���̃G���[�ɂЂ�������Ȃ��Ƃ��̑Ώ�
    if( p_wk->cnt++ > WBM_BTL_ERROR_TIMEOUT )
    {
      NetErr_App_ReqErrorDisp();
      p_param->result = WIFIBATTLEMATCH_BATTLELINK_RESULT_ERROR_RETURN_LOGIN_WIFI;
      return GFL_PROC_RES_FINISH;
    }
    OS_TPrintf( "�G���[�Ώ��҂�\n" );
    break;

  default:
    GF_ASSERT(0);
    break;
  }

  //�G���[����
  //�o�g�����̓o�g���̎����I����҂�
  if( GFL_NET_IsInit() && !(SEQ_BATTLE_INIT<= *p_seq && *p_seq <= SEQ_CALL_END_DEMO ) )
  {
    if( GFL_NET_GetNETInitStruct()->bNetType == GFL_NET_TYPE_IRC )
    { 
      if( NET_ERR_CHECK_NONE != NetErr_App_CheckError() )
      { 
        NetErr_App_ReqErrorDisp();
        p_param->result = WIFIBATTLEMATCH_BATTLELINK_RESULT_ERROR_LIVE;
        return GFL_PROC_RES_FINISH;
      }
    }
    else 
    { 
      GFL_NET_DWC_ERROR_RESULT result  = GFL_NET_DWC_ERROR_ReqErrorDisp( TRUE, TRUE );

      switch( result )
      { 
      case GFL_NET_DWC_ERROR_RESULT_NONE:
        /*  �G���[���������Ă��Ȃ�  */
        break;

      case GFL_NET_DWC_ERROR_RESULT_RETURN_PROC:
      case GFL_NET_DWC_ERROR_RESULT_FATAL:
      default:
        OS_TPrintf( "�l�b�g�ؒf\n" );
        p_param->result = WIFIBATTLEMATCH_BATTLELINK_RESULT_ERROR_RETURN_LOGIN_WIFI;
        return GFL_PROC_RES_FINISH;

      case GFL_NET_DWC_ERROR_RESULT_TIMEOUT:
        p_param->result = WIFIBATTLEMATCH_BATTLELINK_RESULT_ERROR_DISCONNECT_WIFI;
        OS_TPrintf( "���肪�ؒf\n" );
        return GFL_PROC_RES_FINISH;
      }
    }
  }

  return GFL_PROC_RES_CONTINUE;
}
