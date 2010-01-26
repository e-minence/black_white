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
  WIFIBATTLEMATCH_NET_ERRORTYPE_DWC,
  WIFIBATTLEMATCH_NET_ERRORTYPE_SC,
  WIFIBATTLEMATCH_NET_ERRORTYPE_GDB,
  WIFIBATTLEMATCH_NET_ERRORTYPE_MY,
} WIFIBATTLEMATCH_NET_ERRORTYPE;
//-------------------------------------
/// �����̃V�X�e���̃G���[����
//=====================================
typedef enum
{
  WIFIBATTLEMATCH_NET_RESULT_NONE,
  WIFIBATTLEMATCH_NET_RESULT_TIMEOUT,
} WIFIBATTLEMATCH_NET_RESULT;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================

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
    };
    s32 arry[20];
  };
} WIFIBATTLEMATCH_GDB_RND_SCORE_DATA;

//-------------------------------------
///	SAKE����̃f�[�^�󂯎��
//=====================================
#define WIFIBATTLEMATCH_GDB_WIFI_POKEPARTY_SIZE (1334)
typedef struct
{ 
  s32 cheat;
  s32 disconnect;
  s32 win;
  s32 lose;
  s32 rate;
  u8  pokeparty[ WIFIBATTLEMATCH_GDB_WIFI_POKEPARTY_SIZE ];
} WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA;

//�G���[�����^�C�v
typedef enum
{
  WIFIBATTLEMATCH_NET_ERROR_NONE,               //����
  WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETRY,       //������x
  WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT,  //�ؒf�����O�C�������蒼��
  WIFIBATTLEMATCH_NET_ERROR_REPAIR_FATAL,       //�d���ؒf
} WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE;


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
extern WIFIBATTLEMATCH_NET_WORK * WIFIBATTLEMATCH_NET_Init( const DWCUserData *cp_user_data, WIFI_LIST *p_wifilist, HEAPID heapID );
extern void WIFIBATTLEMATCH_NET_Exit( WIFIBATTLEMATCH_NET_WORK *p_wk );
extern void WIFIBATTLEMATCH_NET_Main( WIFIBATTLEMATCH_NET_WORK *p_wk );

//-------------------------------------
///	�G���[
//=====================================
extern BOOL WIFIBATTLEMATCH_NET_CheckError( WIFIBATTLEMATCH_NET_WORK *p_wk );

//-------------------------------------
///	���񏈗�(�K�v�̂Ȃ��ꍇ�͓����Ŏ����I�ɂ���)
//=====================================
extern void WIFIBATTLEMATCH_NET_StartInitialize( WIFIBATTLEMATCH_NET_WORK *p_wk );
extern BOOL WIFIBATTLEMATCH_NET_WaitInitialize( WIFIBATTLEMATCH_NET_WORK *p_wk, SAVE_CONTROL_WORK *p_save, DWCGdbError *p_result );
extern BOOL WIFIBATTLEMATCH_NET_IsInitialize( const WIFIBATTLEMATCH_NET_WORK *cp_wk );

//-------------------------------------
///	�}�b�`���O
//=====================================
//�}�b�`���O
extern void WIFIBATTLEMATCH_NET_StartMatchMake( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_MODE mode, BOOL is_rnd_rate, WIFIBATTLEMATCH_BTLRULE btl_rule );
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
extern void WIFIBATTLEMATCH_SC_Start( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_MODE mode, WIFIBATTLEMATCH_BTLRULE rule, BtlResult result );
extern BOOL WIFIBATTLEMATCH_SC_Process( WIFIBATTLEMATCH_NET_WORK *p_wk, DWCScResult *p_result );

extern WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE WIFIBATTLEMATCH_SC_GetErrorRepairType( DWCScResult error );

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
}WIFIBATTLEMATCH_GDB_GETTYPE;
//WIFIBATTLEMATCH_GDB_GET_RND_SCORE
extern void WIFIBATTLEMATCH_GDB_Start( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_GDB_GETTYPE type, void *p_wk_adrs );
extern BOOL WIFIBATTLEMATCH_GDB_Process( WIFIBATTLEMATCH_NET_WORK *p_wk, DWCGdbError *p_result );

//��������
typedef enum
{ 
  WIFIBATTLEMATCH_GDB_WRITE_DEBUGALL,
  WIFIBATTLEMATCH_GDB_WRITE_POKEPARTY,  //�|�P�����̃o�b�t�@�͊O���̂��̂��g���܂�
  WIFIBATTLEMATCH_GDB_WRITE_WIFI_SCORE,
}WIFIBATTLEMATCH_GDB_WRITETYPE;
//WIFIBATTLEMATCH_GDB_GET_RND_SCORE
extern void WIFIBATTLEMATCH_GDB_StartWrite( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_GDB_WRITETYPE type, const void *cp_wk_adrs );
extern BOOL WIFIBATTLEMATCH_GDB_ProcessWrite( WIFIBATTLEMATCH_NET_WORK *p_wk, DWCGdbError *p_result );

extern WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE WIFIBATTLEMATCH_GDB_GetErrorRepairType( DWCGdbError error );

//�쐬
extern void WIFIBATTLEMATCH_GDB_StartCreateRecord( WIFIBATTLEMATCH_NET_WORK *p_wk );
extern BOOL WIFIBATTLEMATCH_GDB_ProcessCreateRecord( WIFIBATTLEMATCH_NET_WORK *p_wk, DWCGdbError *p_result );


//-------------------------------------
///	����̃f�[�^��M
//=====================================
extern BOOL WIFIBATTLEMATCH_NET_StartEnemyData( WIFIBATTLEMATCH_NET_WORK *p_wk, const void *cp_buff );
extern BOOL WIFIBATTLEMATCH_NET_WaitEnemyData( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_ENEMYDATA **pp_data );


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
} WIFIBATTLEMATCH_RECV_GPFDATA_RET;

extern void WIFIBATTLEMATCH_NET_StartRecvGpfData( WIFIBATTLEMATCH_NET_WORK *p_wk, MYSTATUS *p_mystatus, HEAPID heapID );
extern WIFIBATTLEMATCH_RECV_GPFDATA_RET WIFIBATTLEMATCH_NET_WaitRecvGpfData( WIFIBATTLEMATCH_NET_WORK *p_wk, NHTTPError *p_error );
extern void WIFIBATTLEMATCH_NET_GetRecvGpfData( const WIFIBATTLEMATCH_NET_WORK *cp_wk, DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA *p_recv );

//��������
typedef enum
{
  WIFIBATTLEMATCH_SEND_GPFDATA_RET_UPDATE,
  WIFIBATTLEMATCH_SEND_GPFDATA_RET_SUCCESS,
  WIFIBATTLEMATCH_SEND_GPFDATA_RET_ERROR,
} WIFIBATTLEMATCH_SEND_GPFDATA_RET;

extern void WIFIBATTLEMATCH_NET_StartSendGpfData( WIFIBATTLEMATCH_NET_WORK *p_wk, MYSTATUS *p_mystatus, const DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA *cp_send, HEAPID heapID );
extern WIFIBATTLEMATCH_SEND_GPFDATA_RET WIFIBATTLEMATCH_NET_WaitSendGpfData( WIFIBATTLEMATCH_NET_WORK *p_wk, NHTTPError *p_error );


//-------------------------------------
///	�s�������`�F�b�N
//=====================================
extern void WIFIBATTLEMATCH_NET_StartBadWord( WIFIBATTLEMATCH_NET_WORK *p_wk, const STRCODE *cp_str, u32 str_len );
extern BOOL WIFIBATTLEMATCH_NET_WaitBadWord( WIFIBATTLEMATCH_NET_WORK *p_wk, BOOL *p_is_bad_word );
