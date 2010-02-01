//******************************************************************************
/**
 * 
 * @file    guru2.c
 * @brief   ���邮�����
 * @author  Akito Mori(�ڐA�j / kagaya 
 * @data    2010.01.20
 *
 */
//******************************************************************************
#include <gflib.h>
#include "guru2_local.h"
#include "system/main.h"
#include "app/p_status.h"

//==============================================================================
//  define
//==============================================================================
//--------------------------------------------------------------
/// �Ǘ��V�[�P���X
//--------------------------------------------------------------
enum
{
  SEQNO_G2P_INIT = 0,
  SEQNO_G2P_RECEIPT,
  SEQNO_G2P_POKE_SELECT,
  SEQNO_G2P_POKE_STATUS,
  SEQNO_G2P_GURU2_GAME,
  SEQNO_G2P_END,
  
  SEQNO_G2P_MAX,
};

//==============================================================================
//  struct
//==============================================================================
//--------------------------------------------------------------
/// typedef GURU2_CALL_WORK
//--------------------------------------------------------------
typedef struct _GURU2_CALL_WORK GURU2_CALL_WORK;

//--------------------------------------------------------------
/// GURU2_CALL_WORK
//--------------------------------------------------------------
struct _GURU2_CALL_WORK
{
  int seq_no;
  int psel_pos;
  GURUGURU_PARENT_WORK param;
  GURU2PROC_WORK *g2p;
  GAMEDATA      *gamedata;
  GAMESYS_WORK  *gsys;
  PLIST_DATA *plist;
  PSTATUS_DATA *psd;
};

//==============================================================================
//  proto
//==============================================================================
static BOOL (* const DATA_SeqTbl[SEQNO_G2P_MAX])(GURU2_CALL_WORK*);
static const GFL_PROC_DATA Guru2Receipt_Proc;
static const GFL_PROC_DATA Guru2Main_Proc;

//==============================================================================
//  ���邮�����
//==============================================================================
GFL_PROC_RESULT Guru2Proc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  GURU2_CALL_WORK *work;
  GURUGURU_PARENT_WORK *param = (GURUGURU_PARENT_WORK*)pwk;
  
  work = GFL_PROC_AllocWork( proc, sizeof(GURU2_CALL_WORK),HEAPID_PROC );
  GFL_STD_MemFill( work, 0, sizeof(GURU2_CALL_WORK) );
  
  work->gamedata     = param->gamedata;
//  work->param.sv     = GAMEDATA_GetSaveControlWork( param->gamedata );
//  work->param.record = GAMEDATA_GetRecordPtr( param->gamedata );
  work->param        = *param;

  OS_Printf("���邮��v���Z�X�J�n\n");

  return GFL_PROC_RES_FINISH;
}


GFL_PROC_RESULT Guru2Proc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{

  GURU2_CALL_WORK *g2call = mywk;
  
  if( DATA_SeqTbl[g2call->seq_no](g2call) == TRUE ){
    GFL_HEAP_FreeMemory( g2call );
    return GFL_PROC_RES_FINISH;
  }
  OS_Printf("���邮��v���Z�X���C��\n");

  return GFL_PROC_RES_CONTINUE;
}
GFL_PROC_RESULT Guru2Proc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  OS_Printf("���邮��v���Z�X�I��\n");

  return GFL_PROC_RES_CONTINUE;
}


//--------------------------------------------------------------
/**
 * ���邮������@�v���Z�X�@������
 * @param param GURUGURU_PARENT_WORK
 * @param heap_id �q�[�vID
 * @retval  GURU2PROC_WORK  GURU2PROC_WORK
 */
//--------------------------------------------------------------
GURU2PROC_WORK * Guru2_WorkInit( GURUGURU_PARENT_WORK *param, u32 heap_id )
{
  GURU2PROC_WORK *g2p;
  
  g2p = GFL_HEAP_AllocMemory( heap_id, sizeof(GURU2PROC_WORK) );
  GFL_STD_MemFill( g2p, 0, sizeof(GURU2PROC_WORK) );
  
  //�O���f�[�^�Z�b�g
  g2p->param = *param;
  
  //�ʐM������
  g2p->g2c = Guru2Comm_WorkInit( g2p, heap_id );
  
  return( g2p );
}

//--------------------------------------------------------------
/**
 * ���邮������@�v���Z�X�@�I��
 * @param g2p   GURU2PROC_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
void Guru2_WorkDelete( GURU2PROC_WORK *g2p )
{
  //���[�N���f
  
  //���j�I�����[���ʐM��
  Guru2Comm_CommUnionRoomChange( g2p->g2c );
  
  //�ʐM���[�N�폜
  Guru2Comm_WorkDelete( g2p->g2c );
  
  //���\�[�X�J��
  GFL_HEAP_FreeMemory( g2p );
}

//==============================================================================
//  �p�[�c
//==============================================================================
//--------------------------------------------------------------
/**
 * ��t�I���@�߂�l�`�F�b�N
 * @param g2p GURU2PROC_WORK *
 * @retval  BOOL  TRUE=�p�� FALSE=�I��
 */
//--------------------------------------------------------------
BOOL Guru2_ReceiptRetCheck( GURU2PROC_WORK *g2p )
{
  return( g2p->receipt_ret );
}

//==============================================================================
//  �C�x���g
//==============================================================================
#if 0
//--------------------------------------------------------------
/**
 * ���邮����������p���[�N�쐬
 * @param GAMEDATA *gamedata
 * @retval  void* 
 */
//--------------------------------------------------------------
void * EventCmd_Guru2ProcWorkAlloc( GAMEDATA *gamedata )
{
  GURU2_CALL_WORK *work;
  
  work = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(GURU2_CALL_WORK) );
  GFL_STD_MemFill( work, 0, sizeof(GURU2_CALL_WORK) );
  
  work->gamedata     = gamedata;
  work->param.sv     = GAMEDATA_GetSaveControlWork( gamedata );
  work->param.record = GAMEDATA_GetRecordPtr( gamedata );
  return( work );
}

//--------------------------------------------------------------
/**
 * ���邮������C�x���g����
 * @param wk    EventCmd_Guru2ProcWorkAlloc()�̖߂�l�@�I�����J��
 * @retval  BOOL  TRUE=�I��
 */
//--------------------------------------------------------------
BOOL EventCmd_Guru2Proc( void *wk )
{
  GURU2_CALL_WORK *g2call = wk;
  
  if( DATA_SeqTbl[g2call->seq_no](g2call) == TRUE ){
    GFL_HEAP_FreeMemory( g2call );
    return( TRUE );
  }
  
  return( FALSE );
}

#endif
//==============================================================================
//  ���邮������@�v���Z�X
//==============================================================================
//--------------------------------------------------------------
/**
 * ���邮������@������
 * @param g2call  GURU2_CALL_WORK
 * @retval  BOOL  TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL _seq_Init( GURU2_CALL_WORK *g2call )
{
  g2call->g2p = Guru2_WorkInit( &g2call->param, HEAPID_PROC );
  g2call->seq_no = SEQNO_G2P_RECEIPT;
//  GameSystem_StartSubProc( g2call->fsys, &Guru2Receipt_Proc, g2call->g2p );
  GFL_PROC_SysCallProc( NO_OVERLAY_ID, &Guru2Receipt_Proc, g2call->g2p );
  return( FALSE );
}


//--------------------------------------------------------------------------------------------
/**
 * �|�P�������X�g�̃��[�N�쐬
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param mode  �Ăяo�����[�h
 * @param item  ��������A�C�e��
 * @param pos   �����ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static PLIST_DATA* Guru2PokeListWorkCreate( GURU2_CALL_WORK *g2call, u32 mode,  u16 pos )
{
  PLIST_DATA * pld = GFL_HEAP_AllocClearMemory( HEAPID_MAILBOX_SYS, sizeof(PLIST_DATA) );

  pld->pp        = GAMEDATA_GetMyPokemon( g2call->gamedata );
  pld->myitem    = GAMEDATA_GetMyItem( g2call->gamedata );
  pld->type      = PL_TYPE_SINGLE;
  pld->mode      = mode;
  pld->ret_sel   = pos;

  return pld;
}
FS_EXTERN_OVERLAY(pokelist);
FS_EXTERN_OVERLAY(poke_status);

//--------------------------------------------------------------
/**
 * ���邮������@�Q���Ҏ�t
 * @param g2c GURU2_CALL_WORK
 * @retval  BOOL  TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL _seq_Receipt( GURU2_CALL_WORK *g2call )
{
    if( Guru2_ReceiptRetCheck(g2call->g2p) == FALSE ){
      g2call->seq_no = SEQNO_G2P_END;
    }else{
      GFL_NET_SetAutoErrorCheck( TRUE );
      g2call->plist = Guru2PokeListWorkCreate( g2call, PL_MODE_GURU2, 0 );
      GFL_PROC_SysCallProc( FS_OVERLAY_ID(pokelist), &PokeList_ProcData, &g2call->plist );

//    g2call->plist = Guru2ListEvent_SetProc( g2call->fsys, g2call->psel_pos );
//    g2call->g2p->guru2_mode = GURU2MODE_POKESEL;
//    g2call->seq_no = SEQNO_G2P_POKE_SELECT;
    }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���邮������@�|�P�����I��
 * @param g2call  GURU2_CALL_WORK
 * @retval  BOOL  TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL _seq_PokeSelect( GURU2_CALL_WORK *g2call )
{
    int ret = g2call->plist->ret_sel;
    GFL_HEAP_FreeMemory( g2call->plist );
    
    if( g2call->plist->ret_mode == PL_RET_STATUS ){ //�X�e�[�^�X�{��
//      g2call->psd = PSTATUS_Temoti_Create( g2call->fsys, HEAPID_BASE_APP, PST_MODE_NORMAL );
      MYSTATUS *mystatus = GAMEDATA_GetMyStatus( g2call->gamedata );

      g2call->psd = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(PSTATUS_DATA) );
      GFL_STD_MemFill( g2call->psd, 0, sizeof(PSTATUS_DATA));
      g2call->psd->ppd        = GAMEDATA_GetMyPokemon( g2call->gamedata );
      g2call->psd->game_data  = g2call->gamedata;
      
      g2call->psd->player_name = MyStatus_GetMyName( mystatus );
      g2call->psd->player_id   = MyStatus_GetID( mystatus );
      g2call->psd->player_sex  =  MyStatus_GetMySex( mystatus );
      g2call->psd->ppt  =PST_PP_TYPE_POKEPARAM;

      g2call->psd->max  = 1;
      g2call->psd->pos  = 0;
      g2call->psd->mode = PST_MODE_NO_WAZACHG; // �Z����ւ��֎~�ɂ���
      g2call->psd->waza = 0;
      g2call->psd->page = PPT_INFO;

      
      g2call->psel_pos = ret;
      g2call->psd->pos = ret;
//      FieldPokeStatus_SetProc( g2call->fsys, g2call->psd ); 
      GFL_PROC_SysCallProc( FS_OVERLAY_ID(poke_status), &PokeStatus_ProcData, &g2call->psd );

      g2call->seq_no = SEQNO_G2P_POKE_STATUS;
    }else{                      //�Q�[����
      g2call->g2p->trade_no = ret;
//      GameSystem_StartSubProc(
//        g2call->fsys, &Guru2Main_Proc, g2call->g2p );
        GFL_PROC_SysCallProc( NO_OVERLAY_ID, &Guru2Main_Proc, g2call->g2p );
      g2call->g2p->guru2_mode = GURU2MODE_GAME_MAIN;
      g2call->seq_no = SEQNO_G2P_GURU2_GAME;
    }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���邮������@�|�P�����X�e�[�^�X�{��
 * @param g2call  GURU2_CALL_WORK
 * @retval  BOOL  TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL _seq_PokeStatus( GURU2_CALL_WORK *g2call )
{
    GFL_HEAP_FreeMemory( g2call->psd );
//    g2call->plist =
//      Guru2ListEvent_SetProc( g2call->fsys, g2call->psel_pos );
      g2call->plist = Guru2PokeListWorkCreate( g2call, PL_MODE_GURU2, 0 );
      GFL_PROC_SysCallProc( FS_OVERLAY_ID(pokelist), &PokeList_ProcData, &g2call->plist );

    g2call->seq_no = SEQNO_G2P_POKE_SELECT;
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���邮������@���邮������Q�[��
 * @param g2call  GURU2_CALL_WORK
 * @retval  BOOL  TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL _seq_Guru2Game( GURU2_CALL_WORK *g2call )
{
  g2call->seq_no = SEQNO_G2P_END;
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���邮������@�I��
 * @param g2call  GURU2_CALL_WORK
 * @retval  BOOL  TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL _seq_End( GURU2_CALL_WORK *g2call )
{
  Guru2_WorkDelete( g2call->g2p );
  return( TRUE );
}

//--------------------------------------------------------------
/// ���邮������@�v���Z�X�܂Ƃ�
//--------------------------------------------------------------
static BOOL (* const DATA_SeqTbl[SEQNO_G2P_MAX])(GURU2_CALL_WORK*) =
{
  _seq_Init,
  _seq_Receipt,
  _seq_PokeSelect,
  _seq_PokeStatus,
  _seq_Guru2Game,
  _seq_End,
};

//==============================================================================
//  data
//==============================================================================
// �I�[�o�[���CID�錾
FS_EXTERN_OVERLAY(guru2);

//--------------------------------------------------------------
/// ���邮���t�v���Z�X�f�[�^
//--------------------------------------------------------------
static const GFL_PROC_DATA Guru2Receipt_Proc = {
  Guru2ReceiptProc_Init,
  Guru2ReceiptProc_Main,
  Guru2ReceiptProc_End,
};

//--------------------------------------------------------------
/// ���邮�郁�C���v���Z�X�f�[�^
//--------------------------------------------------------------
static const GFL_PROC_DATA Guru2Main_Proc = {
  Guru2ReceiptProc_Init,
  Guru2ReceiptProc_Main,
  Guru2ReceiptProc_End,
//  Guru2MainProc_Init,
//  Guru2MainProc_Main,
//  Guru2MainProc_End,
};

//--------------------------------------------------------------
/// ���邮������{�̃v���b�N�f�[�^�i�O���Q�Ɨp�j
//--------------------------------------------------------------
const GFL_PROC_DATA Guru2ProcData = {
  Guru2Proc_Init,
  Guru2Proc_Main,
  Guru2Proc_End,
};