//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_net.h
 *	@brief  WIFI�o�g���p�l�b�g���W���[��
 *	@author	Toru=Nagihashi
 *	@date		2009.11.25
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "sc/dwc_sc.h"
#include "gdb/dwc_gdb.h"
#include "battle/battle.h"
#include "wifibattlematch_data.h"
#include "pm_define.h"
#include "net_app/wifibattlematch.h"
#include "savedata/regulation.h"
#include "net/nhttp_rap.h"
#include "savedata/wifilist.h"
#include "system/net_err.h"
#include "net/nhttp_rap_evilcheck.h"
#include "poke_tool/pokeparty.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�}�b�`�V�[�P���X
//=====================================
typedef enum
{
  WIFIBATTLEMATCH_NET_SEQ_MATCH_IDLE,
  WIFIBATTLEMATCH_NET_SEQ_MATCH_START,
  WIFIBATTLEMATCH_NET_SEQ_MATCH_START2,
  WIFIBATTLEMATCH_NET_SEQ_MATCH_WAIT,

  WIFIBATTLEMATCH_NET_SEQ_CONNECT_START,
  WIFIBATTLEMATCH_NET_SEQ_CONNECT_CHILD,
  WIFIBATTLEMATCH_NET_SEQ_CONNECT_PARENT,
  WIFIBATTLEMATCH_NET_SEQ_TIMING_END,

  WIFIBATTLEMATCH_NET_SEQ_CANCEL,

  WIFIBATTLEMATCH_NET_SEQ_MATCH_END,
} WIFIBATTLEMATCH_NET_SEQ;

//-------------------------------------
///	�G���[�̎��
//=====================================
typedef enum
{
  WIFIBATTLEMATCH_NET_ERRORTYPE_NONE,
  WIFIBATTLEMATCH_NET_ERRORTYPE_SC,
  WIFIBATTLEMATCH_NET_ERRORTYPE_GDB,
  WIFIBATTLEMATCH_NET_ERRORTYPE_ND,
  WIFIBATTLEMATCH_NET_ERRORTYPE_NHTTP,
  WIFIBATTLEMATCH_NET_ERRORTYPE_SYS,
} WIFIBATTLEMATCH_NET_ERRORTYPE;
//-------------------------------------
/// �����̃V�X�e���̃G���[����
//=====================================
typedef enum
{
  WIFIBATTLEMATCH_NET_SYSERROR_NONE,    //�G���[�͔������Ă��Ȃ�
  WIFIBATTLEMATCH_NET_SYSERROR_TIMEOUT, //�A�v���P�[�V�����^�C���A�E�g����������
  WIFIBATTLEMATCH_NET_SYSERROR_NO_RECORD, //�����̃T�P�e�[�u��ID���擾���쐬�ł��Ȃ�
} WIFIBATTLEMATCH_NET_SYSERROR;

//-------------------------------------
///	�G���[�����^�C�v
//=====================================
typedef enum
{
  WIFIBATTLEMATCH_NET_ERROR_NONE,               //����
  WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN,      //���A�\�n�_�܂Ŗ߂�
  WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT,  //�ؒf�����O�C�������蒼��
  WIFIBATTLEMATCH_NET_ERROR_REPAIR_FATAL,       //�d���ؒf
  WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT,     //�^�C���A�E�g
} WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE;


//�T�P�T�[�o�[�̃A�N�Z�X����
#define WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC (60*30)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�����^
//=====================================
typedef struct 
{
  u8  year;                      // �N ( 0 ~ 99 )
  u8  month;                     // �� ( 1 ~ 12 )
  u8  day;                       // �� ( 1 ~ 31 )
  u8  hour;                      // �� ( 0 ~ 23 )
  u8  minute;                    // �� ( 0 ~ 59 )
  u8  second;                    // �b ( 0 ~ 59 )
} WBM_NET_DATETIME;

//-------------------------------------
///	�|�P�����f�[�^
//=====================================
typedef struct
{ 
  char  data[128];
}WIFIBATTLEMATCH_ND_POKEMON;
typedef struct 
{
	///	���ݕێ����Ă���|�P������
	int poke_count;
	///	�|�P�����f�[�^
	WIFIBATTLEMATCH_ND_POKEMON member[TEMOTI_POKEMAX];
} WIFIBATTLEMATCH_ND_POKEPARTY;

//-------------------------------------
///	SAKE����̃f�[�^�󂯎��
//=====================================
typedef struct
{ 
  union
  { 
    struct
    { 
      s32 double_rate;
      s32 rot_rate;
      s32 shooter_rate;
      s32 single_rate;
      s32 triple_rate;
      s32 wificup_rate;
      s32 cheat;
      s32 complete;
      s32 disconnect;

      s32 double_lose;
      s32 double_win;
      s32 rot_lose;
      s32 rot_win;
      s32 shooter_lose;
      s32 shooter_win;
      s32 single_lose;
      s32 single_win;
      s32 triple_lose;
      s32 triple_win;
      s32 init_profileID;
      s32 now_profileID;
      s32 record_save_idx;
      s32 recordID;
    };
    s32 arry[23];
  };
} WIFIBATTLEMATCH_GDB_RND_SCORE_DATA;

//-------------------------------------
///	SAKE�֐�уf�[�^��������
//=====================================
typedef struct 
{
  WIFIBATTLEMATCH_RECORD_DATA record_data;
  u32                         save_idx;
} WIFIBATTLEMATCH_GDB_RND_RECORD_DATA;


//-------------------------------------
///	SAKE����̃f�[�^�󂯎��
//=====================================
#define WIFIBATTLEMATCH_GDB_WIFI_POKEPARTY_SIZE (POKEPARTY_WORK_SIZE)
typedef struct
{ 
  s32 cheat;
  s32 disconnect;
  s32 win;
  s32 lose;
  s32 rate;
  s32 record_save_idx;
  u8  pokeparty[ WIFIBATTLEMATCH_GDB_WIFI_POKEPARTY_SIZE ];
} WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA;


//-------------------------------------
///	�l�b�g���W���[��
//=====================================
typedef struct _WIFIBATTLEMATCH_NET_WORK WIFIBATTLEMATCH_NET_WORK;


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	���[�N�쐬
//=====================================
extern WIFIBATTLEMATCH_NET_WORK * WIFIBATTLEMATCH_NET_Init( u32 sake_recordID, GAMEDATA *p_gamedata, DWCSvlResult *p_svl_result, HEAPID heapID );
extern void WIFIBATTLEMATCH_NET_Exit( WIFIBATTLEMATCH_NET_WORK *p_wk );
extern void WIFIBATTLEMATCH_NET_Main( WIFIBATTLEMATCH_NET_WORK *p_wk );

//-------------------------------------
///	�G���[
//=====================================
extern WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE WIFIBATTLEMATCH_NET_CheckErrorRepairType( WIFIBATTLEMATCH_NET_WORK *p_wk, BOOL is_heavy );

//-------------------------------------
///	���񏈗�(�K�v�̂Ȃ��ꍇ�͓����Ŏ����I�ɂ���)
//=====================================
extern void WIFIBATTLEMATCH_NET_StartInitialize( WIFIBATTLEMATCH_NET_WORK *p_wk );
extern BOOL WIFIBATTLEMATCH_NET_WaitInitialize( WIFIBATTLEMATCH_NET_WORK *p_wk );

//-------------------------------------
///	�}�b�`���O
//=====================================
typedef struct 
{
  u32 rate;
  u32 disconnect;
  u32 cup_no;
} WIFIBATTLEMATCH_MATCH_KEY_DATA;
extern void WIFIBATTLEMATCH_NET_StartMatchMake( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_TYPE mode, BOOL is_rnd_rate, WIFIBATTLEMATCH_BTLRULE btl_rule, const WIFIBATTLEMATCH_MATCH_KEY_DATA *cp_data );
extern BOOL WIFIBATTLEMATCH_NET_WaitMatchMake( WIFIBATTLEMATCH_NET_WORK *p_wk );
extern WIFIBATTLEMATCH_NET_SEQ WIFIBATTLEMATCH_NET_GetSeqMatchMake( const WIFIBATTLEMATCH_NET_WORK *cp_wk );

extern void WIFIBATTLEMATCH_NET_StartMatchMakeDebug( WIFIBATTLEMATCH_NET_WORK *p_wk );

//�ؒf
extern BOOL WIFIBATTLEMATCH_NET_SetDisConnect( WIFIBATTLEMATCH_NET_WORK *p_wk, BOOL is_force );

//�L�����Z��
extern void WIFIBATTLEMATCH_NET_StopConnect( WIFIBATTLEMATCH_NET_WORK *p_wk, BOOL is_stop );

//-------------------------------------
///	ATLAS���v�E�����֌W�iSC�j
//=====================================
extern void WIFIBATTLEMATCH_SC_Start( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_TYPE mode, WIFIBATTLEMATCH_BTLRULE rule, const BATTLEMATCH_BATTLE_SCORE *cp_btl_score );
extern BOOL WIFIBATTLEMATCH_SC_Process( WIFIBATTLEMATCH_NET_WORK *p_wk );

extern void WIFIBATTLEMATCH_SC_StartNoMatch( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_TYPE mode, WIFIBATTLEMATCH_BTLRULE rule, const BATTLEMATCH_BATTLE_SCORE *cp_btl_score );
extern BOOL WIFIBATTLEMATCH_SC_ProcessNoMatch( WIFIBATTLEMATCH_NET_WORK *p_wk );

typedef struct
{
  DWCScGameResult my_result;
  u8 my_single_win;
  u8 my_single_lose;
  u8 my_double_win;
  u8 my_double_lose;
  u8 my_rot_win;
  u8 my_rot_lose;
  u8 my_triple_win;
  u8 my_triple_lose;
  u8 my_shooter_win;
  u8 my_shooter_lose;
  u8 my_single_rate;
  u8 my_double_rate;
  u8 my_rot_rate;
  u8 my_triple_rate;
  u8 my_shooter_rate;
  u8 my_cheat;
  DWCScGameResult you_result;  
  u8 you_single_win;
  u8 you_single_lose;
  u8 you_double_win;
  u8 you_double_lose;
  u8 you_rot_win;
  u8 you_rot_lose;
  u8 you_triple_win;
  u8 you_triple_lose;
  u8 you_shooter_win;
  u8 you_shooter_lose;
  u8 you_single_rate;
  u8 you_double_rate;
  u8 you_rot_rate;
  u8 you_triple_rate;
  u8 you_shooter_rate;
  u8 you_cheat;
} WIFIBATTLEMATCH_SC_DEBUG_DATA;
extern void WIFIBATTLEMATCH_SC_StartDebug( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_SC_DEBUG_DATA * p_data, BOOL is_auth );

//-------------------------------------
///	SAKE�f�[�^�x�[�X�֌W�iGDB)
//=====================================
//�ǂݎ��
typedef enum
{ 
  WIFIBATTLEMATCH_GDB_GET_RND_SCORE,
  WIFIBATTLEMATCH_GDB_GET_WIFI_SCORE,
  WIFIBATTLEMATCH_GDB_GET_RECORDID,
  WIFIBATTLEMATCH_GDB_GET_PARTY,
  WIFIBATTLEMATCH_GDB_GET_LOGIN_DATE, //DATETIME�̎󂯎��o�b�t�@��ݒ肵�Ă�������

  WIFIBATTLEMATCH_GDB_GET_DEBUGALL  = WIFIBATTLEMATCH_GDB_GET_RND_SCORE,
}WIFIBATTLEMATCH_GDB_GETTYPE;

//�����̃f�[�^����擾����ꍇ
#define WIFIBATTLEMATCH_GDB_MYRECORD  (-1)

extern void WIFIBATTLEMATCH_GDB_Start( WIFIBATTLEMATCH_NET_WORK *p_wk, int recordID, WIFIBATTLEMATCH_GDB_GETTYPE type, void *p_wk_adrs );
extern BOOL WIFIBATTLEMATCH_GDB_Process( WIFIBATTLEMATCH_NET_WORK *p_wk );

#define WIFIBATTLEMATCH_GDB_DEFAULT_RATEING (1500)

//��������
typedef enum
{ 
  WIFIBATTLEMATCH_GDB_WRITE_POKEPARTY,  //�|�P�����̃o�b�t�@�͊O���̂��̂��g���܂� + score������
//  WIFIBATTLEMATCH_GDB_WRITE_WIFI_SCORE,
  WIFIBATTLEMATCH_GDB_WRITE_LOGIN_DATE, //�f�[�^�́A����܂���i�����Ń��O�C�����Ԃ��o�b�t�@�ɒ��߂܂��j
  WIFIBATTLEMATCH_GDB_WRITE_MYINFO, //�f�[�^�́A����܂���{LOGIN_DATE���ݒ肵�܂�

  WIFIBATTLEMATCH_GDB_WRITE_RECORD,     //��уf�[�^����������(WIFIBATTLEMATCH_GDB_RND_RECORD_DATA)

  WIFIBATTLEMATCH_GDB_WRITE_DEBUGALL,
}WIFIBATTLEMATCH_GDB_WRITETYPE;
extern void WIFIBATTLEMATCH_GDB_StartWrite( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_GDB_WRITETYPE type, const void *cp_wk_adrs );
extern BOOL WIFIBATTLEMATCH_GDB_ProcessWrite( WIFIBATTLEMATCH_NET_WORK *p_wk );

//�쐬
extern void WIFIBATTLEMATCH_GDB_StartCreateRecord( WIFIBATTLEMATCH_NET_WORK *p_wk );
extern BOOL WIFIBATTLEMATCH_GDB_ProcessCreateRecord( WIFIBATTLEMATCH_NET_WORK *p_wk );

//���R�[�hID�擾
extern u32 WIFIBATTLEMATCH_GDB_GetRecordID( const WIFIBATTLEMATCH_NET_WORK *cp_wk );

//-------------------------------------
///	����̃f�[�^��M  ���݂��̃f�[�^�𑗂�  SENDDATA�n
//=====================================
//���݂��̏��𑗂肠��
extern BOOL WIFIBATTLEMATCH_NET_StartEnemyData( WIFIBATTLEMATCH_NET_WORK *p_wk, const void *cp_buff );
//�Ȃ�������n�����Ƃ����ƁAWIFIBATTLEMATCH_ENEMYDATA�̓t���L�V�u���ȍ\���̂Ȃ̂Ń|�C���^���󂯎��A�O��MemCopy���Ă��炤���߂ł��B
extern BOOL WIFIBATTLEMATCH_NET_WaitEnemyData( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_ENEMYDATA **pp_data );
//�|�P�p�[�e�C�𑗂肠��
extern BOOL WIFIBATTLEMATCH_NET_SendPokeParty( WIFIBATTLEMATCH_NET_WORK *p_wk, const POKEPARTY *cp_party );
extern BOOL WIFIBATTLEMATCH_NET_RecvPokeParty( WIFIBATTLEMATCH_NET_WORK *p_wk, POKEPARTY *p_party );
//�s���J�E���^�𑗂荇��
extern BOOL WIFIBATTLEMATCH_NET_SendDirtyCnt( WIFIBATTLEMATCH_NET_WORK *p_wk, const u32 *cp_dirty_cnt );
extern BOOL WIFIBATTLEMATCH_NET_RecvDirtyCnt( WIFIBATTLEMATCH_NET_WORK *p_wk, u32 *p_dirty_cnt );
//-------------------------------------
///	�_�E�����[�h�T�[�o�[  (ND)
//=====================================
typedef enum
{ 
  WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_DOWNLOADING, //�擾��
  WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_SUCCESS, //��M����
  WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_EMPTY,   //���݂��Ȃ�����
  WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_ERROR,   //�G���[
}WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET;

extern void WIFIBATTLEMATCH_NET_StartDownloadDigCard( WIFIBATTLEMATCH_NET_WORK *p_wk, int cup_no );
extern WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET WIFIBATTLEMATCH_NET_WaitDownloadDigCard( WIFIBATTLEMATCH_NET_WORK *p_wk );
extern void WIFIBATTLEMATCH_NET_GetDownloadDigCard( const WIFIBATTLEMATCH_NET_WORK *cp_wk, REGULATION_CARDDATA *p_recv );

//-------------------------------------
///	GPF�T�[�o�[            (HTTP)
//=====================================
//��M
typedef enum
{
  WIFIBATTLEMATCH_RECV_GPFDATA_RET_UPDATE,
  WIFIBATTLEMATCH_RECV_GPFDATA_RET_SUCCESS,
  WIFIBATTLEMATCH_RECV_GPFDATA_RET_ERROR,
  WIFIBATTLEMATCH_RECV_GPFDATA_RET_DIRTY, //���炦�Ȃ�����
} WIFIBATTLEMATCH_RECV_GPFDATA_RET;

extern void WIFIBATTLEMATCH_NET_StartRecvGpfData( WIFIBATTLEMATCH_NET_WORK *p_wk, HEAPID heapID );
extern WIFIBATTLEMATCH_RECV_GPFDATA_RET WIFIBATTLEMATCH_NET_WaitRecvGpfData( WIFIBATTLEMATCH_NET_WORK *p_wk );  //@todo���[�e�B���O��WIFI���ł̓G���[���͂��o��
extern void WIFIBATTLEMATCH_NET_GetRecvGpfData( const WIFIBATTLEMATCH_NET_WORK *cp_wk, DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA *p_recv );

//��������
typedef enum
{
  WIFIBATTLEMATCH_SEND_GPFDATA_RET_UPDATE,
  WIFIBATTLEMATCH_SEND_GPFDATA_RET_SUCCESS,
  WIFIBATTLEMATCH_SEND_GPFDATA_RET_ERROR,
} WIFIBATTLEMATCH_SEND_GPFDATA_RET;

extern void WIFIBATTLEMATCH_NET_StartSendGpfData( WIFIBATTLEMATCH_NET_WORK *p_wk, const DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA *cp_send, HEAPID heapID );
extern WIFIBATTLEMATCH_SEND_GPFDATA_RET WIFIBATTLEMATCH_NET_WaitSendGpfData( WIFIBATTLEMATCH_NET_WORK *p_wk );


//-------------------------------------
///	�s�������`�F�b�N
//=====================================
extern void WIFIBATTLEMATCH_NET_StartBadWord( WIFIBATTLEMATCH_NET_WORK *p_wk, MYSTATUS *p_mystatus, HEAPID heapID );
extern BOOL WIFIBATTLEMATCH_NET_WaitBadWord( WIFIBATTLEMATCH_NET_WORK *p_wk, BOOL *p_is_bad_word );

//-------------------------------------
///	�|�P�����s���`�F�b�N  (NHTTP)
//=====================================
//���ʃR�[�h
typedef enum
{
  WIFIBATTLEMATCH_NET_EVILCHECK_RET_UPDATE,
  WIFIBATTLEMATCH_NET_EVILCHECK_RET_SUCCESS,
  WIFIBATTLEMATCH_NET_EVILCHECK_RET_ERROR,
} WIFIBATTLEMATCH_NET_EVILCHECK_RET;
//�f�[�^
typedef struct 
{
  u8 status_code;
  u32 poke_result[TEMOTI_POKEMAX];
  s8 sign[ NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN ];
} WIFIBATTLEMATCH_NET_EVILCHECK_DATA;

typedef enum
{ 
  WIFIBATTLEMATCH_NET_EVILCHECK_TYPE_PP,
  WIFIBATTLEMATCH_NET_EVILCHECK_TYPE_PARTY,
} WIFIBATTLEMATCH_NET_EVILCHECK_TYPE;

extern void WIFIBATTLEMATCH_NET_StartEvilCheck( WIFIBATTLEMATCH_NET_WORK *p_wk, const void *cp_void, WIFIBATTLEMATCH_NET_EVILCHECK_TYPE type );
extern WIFIBATTLEMATCH_NET_EVILCHECK_RET WIFIBATTLEMATCH_NET_WaitEvilCheck( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_NET_EVILCHECK_DATA *p_data );

extern void WIFIBATTLEMATCH_NETICON_SetDraw( WIFIBATTLEMATCH_NET_WORK *p_wk, BOOL is_visible );
