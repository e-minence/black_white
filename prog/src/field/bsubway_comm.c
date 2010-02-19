//======================================================================
/**
 * @file  bsubway_comm.c
 * @brief  �o�g���T�u�E�F�C�@�ʐM�֘A
 * @author  Miyuki Iwasawa
 * @date  2006.05.23
 *
 * 2007.05.24  Satoshi Nohara*
 *
 * @note pl����ڐA kagaya
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "net/network_define.h"

#include "bsubway_scr.h"
#include "bsubway_scrwork.h"

//======================================================================
//  define
//======================================================================
#define COMM_BSUBWAY_TIMINGSYNC_NO (200)

//--------------------------------------------------------------
//  �ʐM�R�}���h
//--------------------------------------------------------------
enum
{
  FC_BSUBWAY_PLAYER_DATA = GFL_NET_CMD_BSUBWAY,
  FC_BSUBWAY_TR_DATA,
  FC_BSUBWAY_RETIRE_SELECT,
  FC_BSUBWAY_MAX,
};

//�R�}���h�e�[�u���v�f��
#define FC_BSUBWAY_CMD_MAX  (FC_BSUBWAY_MAX-GFL_NET_CMD_BSUBWAY)

//======================================================================
//  struct
//======================================================================

//=====================================================================
//  proto
//======================================================================
static void * get_BeaconData( void *pWork );
static int get_BeaconSize( void *pWork );
static BOOL check_ConnectService(
    GameServiceID GameServiceID1, GameServiceID GameServiceID2 );

static void commCmd_RecvBufPlayerData(
    int netID, const int size, const void *pData,
    void *pWork, GFL_NETHANDLE *pNetHandle );
static void commCmd_FrWiFiCounterTowerRecvBufTrainerData(
    int netID, const int size, const void *pData,
    void *pWork, GFL_NETHANDLE *pNetHandle );
static void commCmd_FrWiFiCounterTowerRecvBufRetireSelect(
    int netID, const int size, const void *pData,
    void *pWork, GFL_NETHANDLE *pNetHandle );

//======================================================================
//  �ʐM�f�[�^
//======================================================================
//--------------------------------------------------------------
//  �ʐM�R�}���h�e�[�u��
//--------------------------------------------------------------
static const NetRecvFuncTable data_RecvFuncTbl[FC_BSUBWAY_CMD_MAX] = {
  {commCmd_RecvBufPlayerData, NULL},
  {commCmd_FrWiFiCounterTowerRecvBufTrainerData, NULL},
  {commCmd_FrWiFiCounterTowerRecvBufRetireSelect, NULL},
};

/*
1:COMM_ENTRY_BEACON�ɗp�� OS_GetMacAddress
2:GFLNetInitializeStruct�̏�����
�@�r�[�R���|�C���^�擾�֐��ł͎菇�P�ŗp�ӂ������̂��B
�@�r�[�R���T�C�Y��COMM_ENTRY_BEACON
3:GFL_NET_Init()�Ɏ菇�Q�ŗp�ӂ������̂𓊂���
*/

//--------------------------------------------------------------
/// �ʐM�������f�[�^
//--------------------------------------------------------------
static const GFLNetInitializeStruct data_NetInit = {
  data_RecvFuncTbl, // ��M�֐��e�[�u��
  FC_BSUBWAY_CMD_MAX, // ��M�e�[�u���v�f��
  NULL, // �n�[�h�Őڑ��������ɌĂ΂��
  NULL, // �l�S�V�G�[�V�����������ɃR�[��
  NULL, // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
  NULL, // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
  get_BeaconData, // �r�[�R���f�[�^�擾�֐�  
  get_BeaconSize, // �r�[�R���f�[�^�T�C�Y�擾�֐� 
  check_ConnectService, //�r�[�R���T�[�r�X���r���q���ŗǂ����ǂ������f
  NULL,    // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂��
  NULL,    //FatalError
  NULL,    // �ʐM�ؒf���ɌĂ΂��֐�(�I����
  NULL,    // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
  
#if GFL_NET_WIFI
  NULL, // wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
  NULL, // wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
  NULL, // wifi�t�����h���X�g�폜�R�[���o�b�N
  NULL, // DWC�`���̗F�B���X�g  
  NULL, // DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
  0, ///< DWC�ւ�HEAP�T�C�Y
  TRUE, // �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
  
  0x999,  //ggid  DP=0x333,RANGER=0x178,WII=0x346 <-�K���Ȃ̂�
  GFL_HEAPID_APP, //���ɂȂ�heapid
  HEAPID_NETWORK + HEAPDIR_MASK,    //�ʐM�p��create�����HEAPID
  HEAPID_WIFI + HEAPDIR_MASK,    //wifi�p��create�����HEAPID
  HEAPID_NETWORK + HEAPDIR_MASK,    //
  
  GFL_WICON_POSX, GFL_WICON_POSY,  // �ʐM�A�C�R��XY�ʒu
  2, //_MAXNUM,  //�ő�ڑ��l�� �����܂߂��l
  
  NET_SEND_SIZE_STANDARD, //�ő呗�M�o�C�g��
  
  8,  // �ő�r�[�R�����W��
  
  TRUE,    // CRC�v�Z
  FALSE,    // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
  GFL_NET_TYPE_WIRELESS,    //�ʐM�^�C�v�̎w��
  FALSE,    // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
  
  WB_NET_BSUBWAY,  //GameServiceID
  
#if GFL_NET_IRC
  IRC_TIMEOUT_STANDARD,  // �ԊO���^�C���A�E�g����
#endif
  
  0,//MP�e�ő�T�C�Y 512�܂�
  0,//dummy
};

//======================================================================
//  �o�g���T�u�E�F�C�@�ʐM�������֘A
//======================================================================
//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�p�ʐM������
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_COMM_Init( BSUBWAY_SCRWORK *bsw_scr )
{
  GFL_NET_Init( &data_NetInit, NULL, bsw_scr );
}

//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�p�ʐM�I��
 * @param
 * @retval
 */
//--------------------------------------------------------------
void BSUBWAY_COMM_Exit( BSUBWAY_SCRWORK *bsw_scr )
{
  GFL_NET_Exit( NULL );
}

//--------------------------------------------------------------
/**
 * �r�[�R���擾�֐�
 * @param pWork �ʐM�p���[�N
 * @retval void* �r�[�R���f�[�^
 */
//--------------------------------------------------------------
static void * get_BeaconData( void *pWork )
{
  BSUBWAY_SCRWORK *bsw_scr = pWork;
  COMM_ENTRY_BEACON *bcon = &bsw_scr->commBeaconData;
  GAMEDATA *gdata = bsw_scr->gdata;
   
  MyStatus_Copy( GAMEDATA_GetMyStatus(gdata), &bcon->mystatus );
  OS_GetMacAddress( bcon->mac_address );
  return( bcon );
}

//--------------------------------------------------------------
/**
 * �r�[�R���T�C�Y�擾�֐�
 * @param pWork �ʐM�p���[�N
 * @retval int �r�[�R���T�C�Y
 */
//--------------------------------------------------------------
static int get_BeaconSize( void *pWork )
{
  return( sizeof(COMM_ENTRY_BEACON) );
}

//--------------------------------------------------------------
/**
 * �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
 * @param   GameServiceID1    �T�[�r�XID�P
 * @param   GameServiceID2    �T�[�r�XID�Q
 * @retval  BOOL    TRUE:�ڑ�OK�@�@FALSE:�ڑ�NG
 */
//--------------------------------------------------------------
static BOOL check_ConnectService(
    GameServiceID GameServiceID1, GameServiceID GameServiceID2 )
{
  return( GameServiceID1 == GameServiceID2 );
}

#if 0
//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�p�ʐM�R�}���h�e�[�u����ǉ�
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_COMM_AddCommandTable( BSUBWAY_SCRWORK *bsw_scr )
{
  GFL_NET_AddCommandTable( GFL_NET_CMD_BSUBWAY,
      data_RecvFuncTbl, FC_BSUBWAY_MAX, bsw_scr );
}

//--------------------------------------------------------------
/**
 * �ǉ������o�g���T�u�E�F�C�p�ʐM�R�}���h�e�[�u�����폜
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_COMM_DeleteCommandTable( BSUBWAY_SCRWORK *bsw_scr )
{
  GFL_NET_DelCommandTable( GFL_NET_CMD_BSUBWAY );
}
#endif

//======================================================================
//  �o�g���T�u�G�E�B�@�ʐM����
//======================================================================
//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�p�ʐM�����J�n
 * @param nothing
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_COMM_TimingSyncStart( const u8 no )
{
  GFL_NETHANDLE *pNet = GFL_NET_HANDLE_GetCurrentHandle();
  GFL_NET_TimingSyncStart( pNet, no );
}

//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�p�ʐM����
 * @param nothing
 * @retval BOOL TRUE=����
 */
//--------------------------------------------------------------
BOOL BSUBWAY_COMM_IsTimingSync( const u8 no )
{
  GFL_NETHANDLE *pNet = GFL_NET_HANDLE_GetCurrentHandle();
  return GFL_NET_IsTimingSync( pNet, no );
}

//======================================================================
//  �o�g���T�u�E�F�C�@�ʐM�f�[�^�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * @brief  �o�g���T�u�E�F�C�@�����Ă����v���C���[�f�[�^���󂯎��
 * @param bsw_scr BSUBWAY_SCRWORK
 * @param recv_buf ��M�f�[�^�o�b�t�@
 * @retval u16
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCRWORK_CommReceivePlayerData(
    BSUBWAY_SCRWORK *bsw_scr, const u16 *recv_buf )
{
  u16 ret = 0;
  
  bsw_scr->pare_sex = (u8)recv_buf[0];
  bsw_scr->pare_poke[0] = recv_buf[1];
  bsw_scr->pare_poke[1] = recv_buf[2];
  bsw_scr->pare_stage_no = recv_buf[3];
  
  bsw_scr->partner = 5 + bsw_scr->pare_sex;
  
  OS_Printf( "sio multi mem = %d,%d:%d,%d\n",
      bsw_scr->mem_poke[0], bsw_scr->mem_poke[1],
      bsw_scr->pare_poke[0], bsw_scr->pare_poke[1] );

  if( bsw_scr->mem_poke[0] == bsw_scr->pare_poke[0] ||
      bsw_scr->mem_poke[0] == bsw_scr->pare_poke[1]){
    ret += 1;
  }
  
  if( bsw_scr->mem_poke[1] == bsw_scr->pare_poke[0] ||
      bsw_scr->mem_poke[1] == bsw_scr->pare_poke[1] ){
    ret += 2;
  }
  
  return( ret );
}

//--------------------------------------------------------------
/**
 * @brief  �o�g���T�u�E�F�C�@�����Ă����g���[�i�[�f�[�^���󂯎��
 * @param bsw_scr BSUBWAY_SCRWORK
 * @param recv_buf ��M�f�[�^�o�b�t�@
 * @retval u16
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCRWORK_CommReciveTrainerData(
    BSUBWAY_SCRWORK *bsw_scr, const u16* recv_buf )
{
  if( GFL_NET_SystemGetCurrentID() == GFL_NET_NO_PARENTMACHINE ){
    return( 0 );  //�e�͑��M���邾���Ȃ̂Ŏ󂯎��Ȃ�
  }
  
  MI_CpuCopy8( recv_buf, bsw_scr->trainer, BSUBWAY_STOCK_TRAINER_MAX*2 );
  
  OS_Printf("sio multi trainer01 = %d,%d:%d,%d\n",
      bsw_scr->trainer[0], bsw_scr->trainer[1],
      bsw_scr->trainer[2], bsw_scr->trainer[3] );
  OS_Printf("sio multi trainer02 = %d,%d:%d,%d\n",
      bsw_scr->trainer[4], bsw_scr->trainer[5],
      bsw_scr->trainer[6], bsw_scr->trainer[7] );
  OS_Printf("sio multi trainer03 = %d,%d:%d,%d\n",
      bsw_scr->trainer[8], bsw_scr->trainer[9],
      bsw_scr->trainer[10], bsw_scr->trainer[11] );
  OS_Printf("sio multi trainer04 = %d,%d\n",
      bsw_scr->trainer[12], bsw_scr->trainer[13] );
  
  return( 1 );
}

//--------------------------------------------------------------
/**
 * @brief �o�g���T�u�E�F�C �����Ă������^�C�A���邩�ǂ����̌��ʂ��󂯎��
 * @param bsw_scr BSUBWAY_SCRWORK
 * @param recv_buf ��M�f�[�^�o�b�t�@
 * @retval  0  ���^�C�A���Ȃ�
 * @retval  1  ���^�C�A����
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCRWORK_CommReciveRetireSelect(
    BSUBWAY_SCRWORK *bsw_scr, const u16 *recv_buf )
{
  OS_Printf( "sio multi retire = %d,%d\n",
      bsw_scr->retire_f, recv_buf[0] );
  
  if( bsw_scr->retire_f || recv_buf[0] ){
    return( 1 );
  }
  
  return( 0 );
}
  
//--------------------------------------------------------------
/**
 * @brief  �o�g���T�u�E�F�C�@���@���ʂƃ����X�^�[No�𑗐M
 * @param bsw_scr BSUBWAY_SCRWORK
 * @param gdata GAMEDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_CommSendPlayerData(
    BSUBWAY_SCRWORK *bsw_scr, GAMEDATA *gdata )
{
  int i;
  POKEMON_PARAM *pp;
  MYSTATUS *my = GAMEDATA_GetMyStatus( gdata );
  const POKEPARTY *party = GAMEDATA_GetMyPokemon( gdata );

  bsw_scr->send_buf[0] = MyStatus_GetMySex( my );
  
  for( i = 0; i < 2; i++ ){
    bsw_scr->send_buf[1+i] =
      PP_Get( PokeParty_GetMemberPointer(party,bsw_scr->member[i]),
            ID_PARA_monsno, NULL );
  }
  
  //������BTWR_MODE_COMM_MULTI��p
  OS_Printf( "bsw_scr->play_mode = %d\n", bsw_scr->play_mode );

  bsw_scr->send_buf[3] = BSUBWAY_SCOREDATA_GetStageNo(
      bsw_scr->scoreData, bsw_scr->play_mode  );
}

//--------------------------------------------------------------
/**
 * @brief  �o�g���T�u�E�F�C�@�ʐM�}���`�@���I�����g���[�i�[No���q�@�ɑ��M
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_CommSendTrainerData( BSUBWAY_SCRWORK *bsw_scr )
{
  MI_CpuCopy8( bsw_scr->trainer, bsw_scr->send_buf,
      BSUBWAY_STOCK_TRAINER_MAX*2);
}

//--------------------------------------------------------------
/**
 * @brief  �o�g���T�u�E�F�C�@�ʐM�}���`�@���^�C�A���邩�ǂ������݂��ɑ��M
 * @param bsw_scr BSUBWAY_SCRWORK
 * @param retire  TRUE�Ȃ烊�^�C�A
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_CommSendRetireSelect(
    BSUBWAY_SCRWORK *bsw_scr, u16 retire )
{
  //�����̑I�����ʂ����[�N�ɕۑ�
  bsw_scr->retire_f = retire;
  bsw_scr->send_buf[0] = retire;
}

//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�@�f�[�^���M
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval BOOL TRUE=���M����
 */
//--------------------------------------------------------------
BOOL BSUBWAY_SCRWORK_CommSendData(
    BSUBWAY_SCRWORK *bsw_scr, u16 mode, u16 param )
{
  int command,size;
  GAMEDATA *gdata  = bsw_scr->gdata;
  
  OS_Printf( "�ʐM�}���`�f�[�^���M�J�n\n" );
  
  switch( mode ){
  case BSWAY_COMM_PLAYER_DATA:  //�|�P�����I��
    command = FC_BSUBWAY_PLAYER_DATA;
    BSUBWAY_SCRWORK_CommSendPlayerData( bsw_scr, gdata );
    break;
  case BSWAY_COMM_TR_DATA:  //���I�g���[�i�[
    command = FC_BSUBWAY_TR_DATA;
    BSUBWAY_SCRWORK_CommSendTrainerData( bsw_scr );
    break;
  case BSWAY_COMM_RETIRE_SELECT:  //���^�C�A��I�Ԃ��H
    command = FC_BSUBWAY_RETIRE_SELECT;
    BSUBWAY_SCRWORK_CommSendRetireSelect( bsw_scr, param );
    break;
  }
  
  OS_Printf( ">>btwr send = %d,%d,%d\n",
      bsw_scr->send_buf[0], bsw_scr->send_buf[1], bsw_scr->send_buf[2] );
  
//  BSUBWAY_COMM_AddCommandTable( bsw_scr );
  
  size = BSWAY_SIO_BUF_LEN;
  
  if( GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),
        command,size,bsw_scr->send_buf) == TRUE ){
    OS_Printf( "�ʐM�}���`�f�[�^���M����\n" );
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�@�f�[�^��M�J�n
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_CommRecieveDataStart(
    BSUBWAY_SCRWORK *bsw_scr, u8 comm_mode )
{
  bsw_scr->comm_mode = comm_mode;
}

//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�@�f�[�^��M
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval BOOL TRUE=��M����
 */
//--------------------------------------------------------------
BOOL BSUBWAY_SCRWORK_CommRecieveData( BSUBWAY_SCRWORK *bsw_scr, u16 *ret_buf )
{
  u8 check_num;
  
  //�f�[�^��M�҂�
  if( bsw_scr->comm_mode == BSWAY_COMM_TR_DATA ){
    check_num = 1;
  }else{
    check_num = 2;
  }
  
  if( bsw_scr->comm_recieve_count == check_num ){
    bsw_scr->comm_recieve_count = 0;
    
    if( ret_buf != NULL ){
      *ret_buf = bsw_scr->comm_check_work;
    }
    
    return( TRUE );
  }
  
  return( FALSE );
}


#if 0
BOOL BSUBWAY_SCRWORK_CommSendData(
    BSUBWAY_SCRWORK *bsw_scr, u16 mode, u16 param, u16 *ret_wk )
{
  int command,size;
  const MYSTATUS* my;
  u16 mode  = VMGetWorkValue(core);
  u16 param  = VMGetWorkValue(core);
  u16* ret_wk = VMGetWork(core);
   
  KAGAYA_Printf( "�ʐM�}���`�f�[�^���M\n" );

  *ret_wk = 0;
  
  switch( mode ){
  case BSWAY_COMM_PLAYER_DATA:  //�|�P�����I��
    command = FC_TOWER_PLAYER_DATA;
    BTowerComm_SendPlayerData(core->fsys->btower_wk,core->fsys->savedata);
    break;
  case TOWER_COMM_TR_DATA:    //���I�g���[�i�[
    command = FC_TOWER_TR_DATA;
    BTowerComm_SendTrainerData(core->fsys->btower_wk);
    break;
  case TOWER_COMM_RETIRE_SELECT:  //���^�C�A��I�Ԃ��H
    command = FC_TOWER_RETIRE_SELECT;
    BTowerComm_SendRetireSelect(core->fsys->btower_wk,param);
    break;
  }
  OS_Printf(">>btwr send = %d,%d,%d\n",wk->send_buf[0],wk->send_buf[1],wk->send_buf[2]);

  //�����A����̂ǂ��炩��DP�̎��́ADP�̒ʐM����
  if( Frontier_CheckDPRomCode(core->fsys->savedata) == 1 ){

    OS_Printf( "DP�̌`���̒ʐM����\n" );
    //CommToolSetTempData(CommGetCurrentID(),wk->send_buf);
    //*ret_wk = 1;    //����
#if 1
    //�����҂��A��M�l���҂����Ȃ��̂ŁA�đ��M���Ă����ꂻ�������ꉞ�B
    if( CommToolSetTempData(CommGetCurrentID(),wk->send_buf) == TRUE ){
      *ret_wk = 1;    //����
    }else{
      return 1;      //���ӁI
    }
#endif
  }else{
    OS_Printf( "PL�̌`���̒ʐM����\n" );
    CommCommandFrontierInitialize( wk );
    //size = 35;          //��`���Ȃ��H�ۂ��̂ŁA�Ƃ肠�������l
    size = BTWR_SIO_BUF_LEN;    //buf��[35]�ŁA2byte�Ȃ̂ŁAsize=70
    if( CommSendData(command,wk->send_buf,size) == TRUE ){
      *ret_wk = 1;  //����
    }
  }

  return 0;
}
#endif

//--------------------------------------------------------------
/**
 * @brief �ʐM�}���`�f�[�^��M
 * @param bsw_scr BSUBWAY_SCRWORK
 * @param mode
 * @param 
 */
//--------------------------------------------------------------
#if 0
static BOOL EvWaitBattleTowerRecvBuf(VM_MACHINE * core);
BOOL EvCmdBattleTowerRecvBuf(VM_MACHINE* core)
{
  u16  retwk_id;
  u16  mode;
  BTOWER_SCRWORK* wk = core->fsys->btower_wk;

  OS_Printf( "�ʐM�}���`�f�[�^��M\n" );

  mode = VMGetWorkValue(core);
  retwk_id = VMGetU16(core);
  
  //�����A����̂ǂ��炩��DP�̎��́ADP�̒ʐM����
  if( Frontier_CheckDPRomCode(core->fsys->savedata) == 1 ){
    OS_Printf( "DP�̌`���̒ʐM����\n" );
    EventCmd_BTowerSioRecvBuf(core->fsys->event,mode,retwk_id);
  }else{
    OS_Printf( "PL�̌`���̒ʐM����\n" );
    wk->ret_wkno = retwk_id;
    wk->mode = mode;
    VM_SetWait( core, EvWaitBattleTowerRecvBuf );
  }
  return 1;
}
#endif

//return 1 = �I��
#if 0
static BOOL EvWaitBattleTowerRecvBuf(VM_MACHINE * core)
{
  u8 check_num;
  BTOWER_SCRWORK* wk = core->fsys->btower_wk;
  u16* ret_wk = GetEventWorkAdrs( core->fsys, wk->ret_wkno );

  //�g���[�i�[�f�[�^�͐e�������M�A�q������M�̌`�ɂȂ��Ă��邽��
  if( wk->mode == TOWER_COMM_TR_DATA ){
    check_num = 1;
  }else{
    check_num = 2;
    //check_num = FRONTIER_COMM_PLAYER_NUM;
  }

  if( wk->recieve_count == check_num ){ 
    wk->recieve_count = 0;
    *ret_wk = wk->check_work;
    return 1;
  }
  return 0;
}
#endif

//--------------------------------------------------------------
/**
 * @brief  ���I�����g���[�i�[No���q�@�ɑ��M
 * @param  bsw_scr  BSUBWAY_SCRWORK
 * @return BOOL TRUE = ���M����
 */
//--------------------------------------------------------------
BOOL BSUBWAY_SCRWORK_CommFrWiFiCounterTowerSendBufTrainerData(
    BSUBWAY_SCRWORK *bsw_scr )
{
  int ret = FALSE;
  int size;
  
  size = (BSUBWAY_STOCK_TRAINER_MAX * 2);
  
  OS_Printf( "WIFI��t �o�g���T�u�E�F�C�@�g���[�i�[No�𑗐M����\n" );
  
  MI_CpuCopy8( bsw_scr->trainer, bsw_scr->send_buf, size );
   
  if( GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),
        FC_BSUBWAY_TR_DATA,size,bsw_scr->send_buf) == TRUE ){
    ret = TRUE;
  }
  
  return ret;
}

//======================================================================
//  �ʐM�R�}���h
//======================================================================
//--------------------------------------------------------------
/**
 * @brief  recv_buf�̃o�g���T�u�E�F�C�����Ă����v���C���[�f�[�^���擾
 * @param   id_no    ���M�҂̃l�b�gID
 * @param   size    ��M�f�[�^�T�C�Y
 * @param   pData    ��M�f�[�^
 * @param   work    FRONTIER_SYSTEM�ւ̃|�C���^
 * @return  none
 */
//--------------------------------------------------------------
static void commCmd_RecvBufPlayerData(
    int netID, const int size, const void *pData,
    void *pWork, GFL_NETHANDLE *pNetHandle )
{
  int num;
  u16 ret;
  BSUBWAY_SCRWORK *bsw_scr = pWork;
  const u16 *recv_buf = pData;
  
  OS_Printf( "WIFI��t �o�g���T�u�E�F�C�@�v���C���[�f�[�^����M\n" );
  OS_Printf( "netID = %d\n", netID );
  
  ret = 0;
  num = 0;
  bsw_scr->comm_recieve_count++;
  
  //�����̃f�[�^�͎󂯎��Ȃ�
  if( GFL_NET_SystemGetCurrentID() == netID ){
    return;
  }
  
  bsw_scr->pare_sex = (u8)recv_buf[0];
  bsw_scr->pare_poke[0] = recv_buf[1];
  bsw_scr->pare_poke[1] = recv_buf[2];
  bsw_scr->pare_stage_no = recv_buf[3];
  
  bsw_scr->partner = 5 + bsw_scr->pare_sex;
  
  OS_Printf( "sio multi mem = %d,%d:%d,%d\n",
      bsw_scr->mem_poke[0], bsw_scr->mem_poke[1],
      bsw_scr->pare_poke[0], bsw_scr->pare_poke[1] );
  
  if( bsw_scr->mem_poke[0] == bsw_scr->pare_poke[0] ||
      bsw_scr->mem_poke[0] == bsw_scr->pare_poke[1]){
    ret += 1;
  }
  
  if( bsw_scr->mem_poke[1] == bsw_scr->pare_poke[0] ||
      bsw_scr->mem_poke[1] == bsw_scr->pare_poke[1]){
    ret += 2;
  }
  
  bsw_scr->comm_check_work = ret;
}

//--------------------------------------------------------------
/**
 * @brief  recv_buf�̃o�g���T�u�E�F�C�����Ă����g���[�i�[�f�[�^���擾
 * @param   id_no    ���M�҂̃l�b�gID
 * @param   size    ��M�f�[�^�T�C�Y
 * @param   pData    ��M�f�[�^
 * @param   work    FRONTIER_SYSTEM�ւ̃|�C���^
 * @return  none
 */
//--------------------------------------------------------------
static void commCmd_FrWiFiCounterTowerRecvBufTrainerData(
    int netID, const int size, const void *pData,
    void *pWork, GFL_NETHANDLE *pNetHandle )
{
  int num;
  BSUBWAY_SCRWORK *bsw_scr = pWork;
  const u16 *recv_buf = pData;
  
  OS_Printf( "WIFI��t �o�g���T�u�E�F�C�@�g���[�i�[No����M����\n" );
  OS_Printf( "id_no = %d\n", netID );
  OS_Printf( "����id = %d\n", GFL_NET_SystemGetCurrentID() );
  
  num = 0;
  bsw_scr->comm_recieve_count++;

  OS_Printf( "bsw_scr->comm_recieve_count = %d\n",
      bsw_scr->comm_recieve_count );
  
  //�����̃f�[�^�͎󂯎��Ȃ�
  if( GFL_NET_SystemGetCurrentID() == netID ){
    return;
  }
  
  //�e�͑��M���邾���Ȃ̂Ŏ󂯎��Ȃ�
  if( GFL_NET_SystemGetCurrentID() == GFL_NET_NO_PARENTMACHINE ){
    return;
  }
  
  MI_CpuCopy8( recv_buf, bsw_scr->trainer, BSUBWAY_STOCK_TRAINER_MAX*2 );
  
  OS_Printf( "sio multi trainer01 = %d,%d:%d,%d\n",
        bsw_scr->trainer[0], bsw_scr->trainer[1],
        bsw_scr->trainer[2], bsw_scr->trainer[3] );
  OS_Printf(  "sio multi trainer02 = %d,%d:%d,%d\n",
        bsw_scr->trainer[4], bsw_scr->trainer[5],
        bsw_scr->trainer[6], bsw_scr->trainer[7] );
  OS_Printf(  "sio multi trainer03 = %d,%d:%d,%d\n",
        bsw_scr->trainer[8], bsw_scr->trainer[9],
        bsw_scr->trainer[10], bsw_scr->trainer[11] );
  OS_Printf(  "sio multi trainer04 = %d,%d\n",
        bsw_scr->trainer[12], bsw_scr->trainer[13] );
}

//--------------------------------------------------------------
/**
 * @brief recv_buf�̃o�g���T�u�E�F�C�����Ă���
 * ���^�C�A���邩�ǂ����̌��ʂ��擾
 * @param   id_no    ���M�҂̃l�b�gID
 * @param   size    ��M�f�[�^�T�C�Y
 * @param   pData    ��M�f�[�^
 * @param   work    FRONTIER_SYSTEM�ւ̃|�C���^
 * @retval  0  ���^�C�A���Ȃ�
 * @retval  1  ���^�C�A����
 */
//--------------------------------------------------------------
static void commCmd_FrWiFiCounterTowerRecvBufRetireSelect(
    int netID, const int size, const void *pData,
    void *pWork, GFL_NETHANDLE *pNetHandle )
{
  int num;
  BSUBWAY_SCRWORK *bsw_scr = pWork;
  const u16 *recv_buf = pData;
  
  OS_Printf( "WIFI��t �o�g���T�u�E�F�C�@�g���[�i�[No����M\n" );
  OS_Printf( "id_no = %d\n", netID );
  
  num = 0;
  bsw_scr->comm_check_work = 0;
  bsw_scr->comm_recieve_count++;

  //�����̃f�[�^�͎󂯎��Ȃ�
  if( GFL_NET_SystemGetCurrentID() == netID ){
    return;
  }
    
  OS_Printf( "sio multi retire = %d,%d\n",
      bsw_scr->retire_f, recv_buf[0] );
  
  if( bsw_scr->retire_f || recv_buf[0] ){
    bsw_scr->comm_check_work = 1;
  }
}
