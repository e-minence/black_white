//******************************************************************************
/**
 * @file    guru2_comm.c
 * @brief   ���邮������@�ʐM
 * @author  Akito Mori(�ڐA�j / kagaya 
 * @data    2010.01.20
 */
//******************************************************************************
#include <gflib.h>
#include "net/network_define.h"
#include "arc_def.h"

#include "print/wordset.h"
#include "system/bmp_menu.h"
//#include "system/snd_tool.h"
//#include "communication/wh.h"

#include "guru2_local.h"
#include "guru2_receipt.h"

#ifdef DEBUG_ONLY_FOR_mori
#define D_GURU2_PRINTF_ON //��`��OS_Printf�L��
#endif

#define GURU2_MAIN_FUNC_OFF


//==============================================================================
//  typedef
//==============================================================================
//--------------------------------------------------------------
/// WUSE_SENDWORK
/// �ʐM�]���p�ėp���[�N�B
/// �T�C�Y��GURU2_WIDEUSE_SENDWORK_SIZE�ŌŒ�
//--------------------------------------------------------------
typedef struct
{
  u32 cmd;
  u8 buf[GURU2_WIDEUSE_SENDWORK_SIZE-4];  //-4=cmd size
}WUSE_SENDWORK;

//==============================================================================
//  proto
//==============================================================================
static BOOL guru2Comm_WideUseSendWorkSend(
  GURU2COMM_WORK *g2c, u32 code, const void *buf, u32 size );

static void Guru2Comm_PokeSelOyaEndTcbSet( GURU2COMM_WORK *g2c );


static void CommCB_Receipt_EndChild(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommCB_Receipt_End(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommCB_Receipt_ChildJoin(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommCB_Receipt_Start(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommCB_Receipt_Data(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommCB_Receipt_BanFlag (
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommCB_Main_Signal     (
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommCB_Main_Join       (
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommCB_Main_Button     (
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommCB_Main_GameData   (
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommCB_Main_PlayNo     (
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommCB_Main_PlayMax    (
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommCB_Main_TradeNo    (
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommCB_Main_GameResult (
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommCB_Main_EggDataNG  (
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommCB_Main_EggDataOK  (
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommCB_Guru2PokeParty  (
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);

static u8 * _getPokePartyRecvBuff( int netID, void *pWork, int size );


//static const CommPacketTbl DATA_CommPacketTbl[CF_COMMAND_MAX];
//static const PTRCommRecvFunc DATA_Guru2CommPacketTbl[G2COMM_MAX];
//--------------------------------------------------------------
/// �ʐM�R�[���o�b�N�֐�
//--------------------------------------------------------------
  
///�ʐM�R�}���h�e�[�u��
const NetRecvFuncTable _Guru2CommPacketTbl[] = {
//  { CommCB_Receipt_Stop,      NULL, },  // G2COMM_RC_STOP,     /
//  { CommCB_Receipt_ReStart,   NULL, },  // G2COMM_RC_RESTART,   
  { CommCB_Receipt_EndChild,  NULL, },  // G2COMM_RC_END_CHILD, 
  { CommCB_Receipt_End,       NULL, },  // G2COMM_RC_END
  { CommCB_Receipt_ChildJoin, NULL, },  // G2COMM_RC_CHILD_JOIN,
  { CommCB_Receipt_Start,     NULL, },  // G2COMM_RC_START,    /
//  { CommCB_Receipt_Data,      NULL, },  // G2COMM_RC_DATA,     /
//  { CommCB_Receipt_BanFlag,   NULL, },  // G2COMM_RC_BAN,      /
  { CommCB_Main_Signal,       NULL, },  // G2COMM_GM_SIGNAL,   /
  { CommCB_Main_Join,         NULL, },  // G2COMM_GM_JOIN,     /
  { CommCB_Main_Button,       NULL, },  // G2COMM_GM_BTN,      /
  { CommCB_Main_GameData,     NULL, },  // G2COMM_GM_GAMEDATA,  
  { CommCB_Main_PlayNo,       NULL, },  // G2COMM_GM_PLAYNO,   /
  { CommCB_Main_PlayMax,      NULL, },  // G2COMM_GM_PLAYMAX,   
  { CommCB_Main_TradeNo,      NULL, },  // G2COMM_GM_TRADE_POS, 
  { CommCB_Main_GameResult,   NULL, },  // G2COMM_GM_GAMERESULT,
  { CommCB_Main_EggDataNG,    NULL, },  // G2COMM_GM_EGG_DATA_NG
  { CommCB_Main_EggDataOK,    NULL, },  // G2COMM_GM_EGG_DATA_OK
  { CommCB_Guru2PokeParty,    _getPokePartyRecvBuff,}, // G2COMM_GM_SEND_EGG_DATA
  
};



//----------------------------------------------------------------------------------
/**
 * @brief 
 *
 * @param   wk    
 *
 * @retval  UNION_APP_PTR   
 */
//----------------------------------------------------------------------------------
static UNION_APP_PTR _get_unionwork( GURU2COMM_WORK *wk)
{
  return wk->g2p->param.uniapp;
}


//==============================================================================
//  ���邮������@�ʐM�R�}���h
//==============================================================================
//--------------------------------------------------------------
/**
 * �ʐM�R�}���h���[�N������
 * @param g2p GURU2PROC_WORK
 * @retval  g2c GURU2COMM_WORK
 */
//--------------------------------------------------------------
GURU2COMM_WORK * Guru2Comm_WorkInit( GURU2PROC_WORK *g2p, u32 heap_id )
{
  GURU2COMM_WORK *g2c;
  
  g2c = GFL_HEAP_AllocMemory( heap_id, sizeof(GURU2COMM_WORK) );
  GF_ASSERT( g2c != NULL );
  GFL_STD_MemFill( g2c, 0, sizeof(GURU2COMM_WORK) );
  
  g2c->g2p = g2p;
  
  g2c->recv_poke_party_buf = GFL_HEAP_AllocMemory( heap_id, 
                                                   G2MEMBER_MAX*POKEPARTY_SEND_ONCE_SIZE );
  g2c->friend_poke_party_buf = GFL_HEAP_AllocMemory( heap_id, 
                                                   G2MEMBER_MAX*POKEPARTY_SEND_ONCE_SIZE );
  
  return( g2c );
}

//--------------------------------------------------------------
/**
 * �ʐM�R�}���h���[�N�폜
 * @param g2c GURU2COMM_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
void Guru2Comm_WorkDelete( GURU2COMM_WORK *g2c )
{
  GFL_HEAP_FreeMemory( g2c->recv_poke_party_buf );
  GFL_HEAP_FreeMemory( g2c->friend_poke_party_buf );
  GFL_HEAP_FreeMemory( g2c );
}

//--------------------------------------------------------------
/**
 * �ʐM�R�}���h������ ��M�R�[���o�b�N�ݒ�
 * @param g2c GURU2COMM_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
void Guru2Comm_CommandInit( GURU2COMM_WORK *g2c )
{
//  CommCommandInitialize( DATA_CommPacketTbl, CF_COMMAND_MAX, g2c );
  GFL_NET_AddCommandTable( GFL_NET_CMD_IRCTRADE,_Guru2CommPacketTbl,
                           NELEMS(_Guru2CommPacketTbl), g2c);

}

//--------------------------------------------------------------
/**
 * ���j�I�����[���ʐM�֖߂�
 * @param g2c GURU2COMM_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
void Guru2Comm_CommUnionRoomChange( GURU2COMM_WORK *g2c )
{
//  CommStateSetLimitNum(2);
//  CommStateUnionBconCollectionRestart();
//  UnionRoomView_ObjInit( g2c->g2p->param.union_view );
//  Union_BeaconChange( UNION_PARENT_MODE_FREE );
}

//==============================================================================
//  ���M�p�ėp���[�N
//==============================================================================
//--------------------------------------------------------------
/**
 * �ėp���[�N���M
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL guru2Comm_WideUseSendWorkSend(
  GURU2COMM_WORK *g2c, u32 cmd, const void *buf, u32 size )
{
  GF_ASSERT( cmd < G2COMM_MAX );
  GF_ASSERT( size+4 <= GURU2_WIDEUSE_SENDWORK_SIZE );
  
  {
    GFL_NETHANDLE *pNet = GFL_NET_HANDLE_GetCurrentHandle();
    BOOL ret;
    WUSE_SENDWORK *work = (void *)g2c->wideuse_sendwork;
    work->cmd = cmd;
    GFL_STD_MemCopy( buf, work->buf, size );
//    ret = ( CG_GURU2_CMD, work, GURU2_WIDEUSE_SENDWORK_SIZE );
    ret=GFL_NET_SendData( pNet, cmd, GURU2_WIDEUSE_SENDWORK_SIZE, work->buf);
    
    #ifdef PM_DEBUG
    if( ret == FALSE ){
      #ifdef D_GURU2_PRINTF_ON
      OS_Printf( "���邮��@�ėp�f�[�^���M���s �R�}���h(%d)\n", cmd );
      #endif
    }
    #endif
    
    return( ret );
  }
}

//==============================================================================
//  ���M
//==============================================================================
//--------------------------------------------------------------
/**
 * ���邮������f�[�^���M
 * @param g2c   GURU2COMM_WORK
 * @param   code  G2C
 * @param data  data
 * @param   size  size
 * @retval  BOOL  ���M�L���[�ɓ��������ǂ���
 * */
//--------------------------------------------------------------
BOOL Guru2Comm_SendData(
  GURU2COMM_WORK *g2c, u32 code, const void *data, u32 size )
{
  if( g2c->send_stop_flag == TRUE ){  //���M��~
    return( FALSE );
  }
  
  return( guru2Comm_WideUseSendWorkSend(g2c,code,data,size) );
}

//==============================================================================
//  �R�[���o�b�N�֐��@���C��
//==============================================================================

//--------------------------------------------------------------
/**
 * ��t�@�|�P�����p�[�e�B����M
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 * @retval  none    
 */
//--------------------------------------------------------------
static void CommCB_Guru2PokeParty(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  void *buf;
  GURU2COMM_WORK *g2c = pWk;
  
  g2c->comm_game_egg_recv_bit |= 1 << netID;
  buf = Guru2Comm_FriendPokePartyGet( g2c, netID );
  GFL_STD_MemCopy( pData, buf,  POKEPARTY_SEND_ONCE_SIZE );

  #ifdef D_GURU2_PRINTF_ON
  OS_Printf( "�^�}�S��M" );
  #endif
}

//==============================================================================
//  �R�[���o�b�N�֐��@��t
//==============================================================================


//------------------------------------------------------------------
/**
 * ��t�@���邮������@�|�P�����Z���N�g�J�n�@�e���M
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 * @retval  nothing
 */
//------------------------------------------------------------------
static void CommCB_Receipt_Start(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  if( GFL_NET_SystemGetCurrentID() != 0 ){  //�e���M �q�̃V�[�P���X�ύX bug 0212 fix
    GURU2COMM_WORK *wk = pWk;
  
    wk->recv_count = 0;
    wk->record_execute = TRUE;
  
    
    Guru2Rc_MainSeqForceChange(
      wk->g2p->g2r, RECORD_MODE_GURU2_POKESEL_START, netID );
    
  //  CommStateSetErrorCheck( TRUE,TRUE ); // �����ȍ~�͐ؒf���G���[
  }
}

//--------------------------------------------------------------
/**
 * ��t�@�q�@�����E�B
 * pData �q(���E��)����̑��M��0:���E���m�F�B�@1:���E���s
 * �e����̑��M�����4�r�b�g�F�e��shareNum(0xf�̏ꍇ�͗��ENG)
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 * @retval  nothing
 */
//--------------------------------------------------------------
static void CommCB_Receipt_EndChild(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  GURU2COMM_WORK *g2c = pWk;
  GURU2COMM_END_CHILD_WORK trans_work;
  const GURU2COMM_END_CHILD_WORK *recieve_work = pData;
  
  //�q�@(���E��)����̑��M
  if( netID != 0 ){           //���E���܂��ʒm
    if( GFL_NET_SystemGetCurrentID()==0){     // �q�@����(�e�@���󂯎��)
      // �q�@�����Ȃ��Ȃ�������S���ɒʒm����
      trans_work = *recieve_work;
      trans_work.ridatu_id = netID;
      trans_work.oya_share_num = g2c->shareNum;
      
      switch(recieve_work->request){
      case CREC_REQ_RIDATU_CHECK:
        if(g2c->shareNum != Union_App_GetMemberNum(_get_unionwork(g2c)) 
          || g2c->shareNum != MATH_CountPopulation(Union_App_GetMemberNetBit(_get_unionwork(g2c)))){
          trans_work.ridatu_kyoka = FALSE;  //���ENG�I
        }else{
          g2c->ridatu_bit |= 1 << netID;
          trans_work.ridatu_kyoka = TRUE;
          //���EOK�Ȃ̂ŎQ��������������
          //(����������΂����瑤�Ő������͂������͂�)
          //CommStateSetLimitNum( Union_App_GetMemberNum(_get_unionwork) );
        }
        break;
      case CREC_REQ_RIDATU_EXE:
        break;
      }
    
//      Guru2Comm_SendData( g2c, G2COMM_RC_END_CHILD,
//        &trans_work, sizeof(GURU2COMM_END_CHILD_WORK) );
      #ifdef D_GURU2_PRINTF_ON
      OS_Printf("�q�@%d���痣�E���󂯎���������M\n",netID);
      #endif
    }
  }else{  //�e����̑��M
    switch(recieve_work->request){
    case CREC_REQ_RIDATU_CHECK:
      //���E�m�F�Ȃ̂ŁA���E���悤�Ƃ����q�@�ɂ̂݌��ʂ𑗂�
      if(recieve_work->ridatu_id == GFL_NET_SystemGetCurrentID()){
        if(recieve_work->ridatu_kyoka == FALSE){
          Guru2Rc_MainSeqForceChange(
            g2c->g2p->g2r, RECORD_MODE_END_SELECT_ANSWER_NG,
            recieve_work->ridatu_id );
        }else{
          g2c->oya_share_num = recieve_work->oya_share_num;
          Guru2Rc_MainSeqForceChange(
            g2c->g2p->g2r, RECORD_MODE_END_SELECT_ANSWER_OK,
            recieve_work->ridatu_id );
        }
      }
      
      break;
    case CREC_REQ_RIDATU_EXE:
      #ifdef D_GURU2_PRINTF_ON
      OS_Printf("�e�@���q�@%d�̗��E�ʒm����\n",recieve_work->ridatu_id);
      #endif
      Guru2Rc_MainSeqForceChange(
        g2c->g2p->g2r, RECORD_MODE_LOGOUT_CHILD, recieve_work->ridatu_id );
      break;
    }
  }
}


//--------------------------------------------------------------
/**
 * ��t�@�e�@����߂�̂ŋ����I��������
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 * @retval  none    
 */
//--------------------------------------------------------------
static void CommCB_Receipt_End(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  GURU2COMM_WORK *wk = pWk;
  
  #ifdef D_GURU2_PRINTF_ON
  OS_Printf("�e�@����̏I���ʒm�������̂ł�߂�\n");
  #endif
  
  // �e�@�ȊO�͂��̃R�}���h�ŋ����I��
  if( GFL_NET_SystemGetCurrentID() != 0 ){
    Guru2Rc_MainSeqForceChange( wk->g2p->g2r, RECORD_MODE_FORCE_END, 0  );
  }
}

//--------------------------------------------------------------
/**
 * ��t�@�R��ځE�S��ځE�T��ڂ̎q�@���u�G������[�����v�ƌ���
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 * @retval  none    
 */
//--------------------------------------------------------------
static void CommCB_Receipt_ChildJoin(
  const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  u8 id;
  GURU2COMM_WORK *g2c = pWk;
  
  // �e�@���󂯎������(�Ƃ������e�����󂯎��Ȃ����ǁj
  if( GFL_NET_SystemGetCurrentID() == 0 ){
    id  = netID;
    // �S��Ɂu���ꂩ��G�𑗂�̂Ŏ~�܂��Ă��������v�Ƒ��M����
//    Guru2Comm_SendData( g2c, G2COMM_RC_STOP, &id, 1 );
    #ifdef D_GURU2_PRINTF_ON
    OS_Printf("�q�@(%d = %d)����̗����^�G�̂��������ʒm\n",id,netID);
    #endif
  }
}

//==============================================================================
//  �R�[���o�b�N�֐��@�Q�[�����C��
//==============================================================================
//--------------------------------------------------------------
/**
 * �Q�[�����C���@�V�O�i����M
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 * @retval  none    
 */
//--------------------------------------------------------------
static void CommCB_Main_Signal(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  const u16 *buff = pData;
  GURU2COMM_WORK *wk = pWk;

  wk->comm_game_signal_bit |= *buff;
  
  #ifdef D_GURU2_PRINTF_ON
  OS_Printf("���邮�� �V�O�i��(0x%x)��M ��MID=0x%x\n",*buff,netID);
  #endif
}

//--------------------------------------------------------------
/**
 * �Q�[�����C���@�Q��
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 * @retval  none    
 */
//--------------------------------------------------------------
static void CommCB_Main_Join(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  GURU2COMM_WORK *wk = pWk;
  wk->comm_game_join_bit |= 1 << netID;
  #ifdef D_GURU2_PRINTF_ON
  OS_Printf( "���邮�� �Q�[���Q���󂯎�� ID= 0x%d\n", netID );
  #endif
}

//--------------------------------------------------------------
/**
 * �Q�[�����C���@�v���C�ԍ�����M
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 * @retval  none    
 */
//--------------------------------------------------------------
static void CommCB_Main_PlayNo(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  GURU2COMM_WORK *g2c = pWk;
  const GURU2COMM_PLAYNO *no = pData;
#ifndef GURU2_MAIN_FUNC_OFF
  Guru2Main_CommPlayNoDataSet( g2c->g2p->g2m, no );
#endif
}

//--------------------------------------------------------------
/**
 * �Q�[�����C���@�v���C�ő吔����M
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 * @retval  none    
 */
//--------------------------------------------------------------
static void CommCB_Main_PlayMax(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  const int *max = pData;
  
  GURU2COMM_WORK *g2c = pWk;
#ifndef GURU2_MAIN_FUNC_OFF
  Guru2Main_CommPlayMaxSet( g2c->g2p->g2m, *max );
#endif
}

//--------------------------------------------------------------
/**
 * �Q�[���@�{�^��������M
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 * @retval  none    
 */
//--------------------------------------------------------------
static void CommCB_Main_Button(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  const u8 *btn = pData;
  GURU2COMM_WORK *g2c = pWk;
#ifndef GURU2_MAIN_FUNC_OFF
  Guru2Main_CommButtonSet( g2c->g2p->g2m, *btn );
#endif
}

//--------------------------------------------------------------
/**
 * �Q�[���@�Q�[��������M
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 * @retval  none    
 */
//--------------------------------------------------------------
static void CommCB_Main_GameData(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  if( GFL_NET_SystemGetCurrentID() != 0 ){  //���M�����e�ȊO
    GURU2COMM_WORK *g2c = pWk;
    const GURU2COMM_GAMEDATA *data = pData;
#ifndef GURU2_MAIN_FUNC_OFF
    Guru2Main_CommGameDataSet( g2c->g2p->g2m, data );
#endif
  }
}

//--------------------------------------------------------------
/**
 * �Q�[�����C���@�����ʒu����M
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 * @retval  none    
 */
//--------------------------------------------------------------
static void CommCB_Main_TradeNo(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  const int *no = pData;
  GURU2COMM_WORK *g2c = pWk;
#ifndef GURU2_MAIN_FUNC_OFF
  Guru2Main_TradeNoSet( g2c->g2p->g2m, netID, *no );
#endif
}

//--------------------------------------------------------------
/**
 * �Q�[�����C���@�Q�[�����ʂ���M
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 * @retval  none    
 */
//--------------------------------------------------------------
static void CommCB_Main_GameResult(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  GURU2COMM_WORK *g2c = pWk;
  const GURU2COMM_GAMERESULT *result = pData;
#ifndef GURU2_MAIN_FUNC_OFF
  Guru2Main_GameResultSet( g2c->g2p->g2m, result );
#endif
}

//--------------------------------------------------------------
/**
 * �Q�[�����C���@���f�[�^�ُ��M
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 * @retval  none    
 */
//--------------------------------------------------------------
static void CommCB_Main_EggDataNG(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  u32 bit = 1 << netID;
  GURU2COMM_WORK *g2c = pWk;
  g2c->comm_game_egg_check_error_bit |= bit;
}

//--------------------------------------------------------------
/**
 * �Q�[�����C���@���f�[�^�����M
 * @param   netID
 * @param   size
 * @param   pData
 * @param   pWk
 * @retval  none
 */
//--------------------------------------------------------------
static void CommCB_Main_EggDataOK(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  u32 bit = 1 << netID;
  GURU2COMM_WORK *g2c = pWk;
  g2c->comm_game_egg_check_ok_bit |= bit;
}

//==============================================================================
//  �T�C�Y
//==============================================================================
//--------------------------------------------------------------
/// GURU2_WIDEUSE_SENDWORK_SIZE
//--------------------------------------------------------------
static int _getGuru2WideUseSendWorkSize( void )
{
  return( GURU2_WIDEUSE_SENDWORK_SIZE );
}

//--------------------------------------------------------------
/// GURU2COMM_END_CHILD_WORK
//--------------------------------------------------------------
static int _getCRECW(void)
{
  return sizeof(GURU2COMM_END_CHILD_WORK);
}

//--------------------------------------------------------------
/// POKEPARTY_SEND_ONCE_SIZE
//--------------------------------------------------------------
static int _getPokePartyOnceSize( void )
{
  return POKEPARTY_SEND_ONCE_SIZE;
}

//==============================================================================
//  ��M�o�b�t�@
//==============================================================================
//--------------------------------------------------------------
/**
 * �|�P�����f�[�^��M�o�b�t�@
 * @param
 * @retval
 */
//--------------------------------------------------------------
static u8 * _getPokePartyRecvBuff( int netID, void *pWork, int size )
{
  u32 buf;
  GURU2COMM_WORK *g2c = pWork;
  
  buf  = (u32)(g2c->recv_poke_party_buf);
  buf += netID * POKEPARTY_SEND_ONCE_SIZE;
  return( (u8*)buf );
}

//==============================================================================
//  �p�[�c
//==============================================================================

//--------------------------------------------------------------
/**
 * �|�P�����p�[�e�B��M�ς݃o�b�t�@��Ԃ�
 * @param
 * @retval
 */
//--------------------------------------------------------------
void * Guru2Comm_FriendPokePartyGet( GURU2COMM_WORK *g2c, int id )
{
  u32 buf;
  buf = (u32)(g2c->friend_poke_party_buf);
  buf += id * POKEPARTY_SEND_ONCE_SIZE;
  return( (void*)buf );
}

//==============================================================================
//  data
//==============================================================================
