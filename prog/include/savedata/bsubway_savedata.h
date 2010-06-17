//======================================================================
/**
 * @file  bsubway_savedata.h
 * �o�g���T�u�E�F�C�@�Z�[�u�f�[�^�֘A
 * @author iwasawa
 * @date  07.05.28
 * @note �v���`�i���ڐA kagaya
 */
//======================================================================
#ifndef __H_BSUBWAY_SAVEDATA_H__
#define __H_BSUBWAY_SAVEDATA_H__

#include "gamesystem/game_data.h"

//======================================================================
//  define
//======================================================================
//--------------------------------------------------------------
/// �萔
//--------------------------------------------------------------
///�I������莝���|�P��������MAX
#define BSUBWAY_STOCK_MEMBER_MAX  (4)
///�ۑ����Ă����ΐ�g���[�i�[�i���o�[�̐�
#define BSUBWAY_STOCK_TRAINER_MAX (14)
///�ۑ����Ă���AI�}���`�y�A�̃|�P�����p�����[�^��
#define BSUBWAY_STOCK_PAREPOKE_MAX  (2)
///WiFi DL�v���C���[�f�[�^��
#define BSUBWAY_STOCK_WIFI_PLAYER_MAX (7)
///WiFi DL���[�_�[�f�[�^��
#define BSUBWAY_STOCK_WIFI_LEADER_MAX (30)
///���[���f�[�^��DL�t���O�G���A�o�b�t�@��
#define BSUBWAY_ROOM_DATA_FLAGS_LEN (250)
///WiFi �莝���|�P������
#define BSUBWAY_STOCK_WIFI_MEMBER_MAX (3)

#if 0
//pt_save.h
#define WAZA_TEMOTI_MAX (4) ///<1�̂̃|�P���������Ă�Z�̍ő�l
#endif

//--------------------------------------------------------------
/// �v���C�f�[�^�擾ID
//--------------------------------------------------------------
typedef enum
{
  BSWAY_PLAYDATA_ID_playmode,
  BSWAY_PLAYDATA_ID_round,
  BSWAY_PLAYDATA_ID_rec_down,
  BSWAY_PLAYDATA_ID_rec_turn,
  BSWAY_PLAYDATA_ID_rec_damage,
  BSWAY_PLAYDATA_ID_pokeno,
  BSWAY_PLAYDATA_ID_pare_poke,
  BSWAY_PLAYDATA_ID_pare_itemfix,
  BSWAY_PLAYDATA_ID_trainer,
  BSWAY_PLAYDATA_ID_partner,
  BSWAY_PLAYDATA_ID_use_battle_box,
  BSWAY_PLAYDATA_ID_sel_wifi_dl_play,
  BSWAY_PLAYDATA_ID_MAX,
}BSWAY_PLAYDATA_ID;

//--------------------------------------------------------------
/// �X�R�A�f�[�^���샂�[�h
//--------------------------------------------------------------
typedef enum
{
  BSWAY_SETMODE_get,    ///<�f�[�^�擾
  BSWAY_SETMODE_set,    ///<�Z�b�g
  BSWAY_SETMODE_reset,  ///<���Z�b�g
  BSWAY_SETMODE_inc,    ///<1++
  BSWAY_SETMODE_dec,    ///<1--
  BSWAY_SETMODE_add,    ///<add
  BSWAY_SETMODE_sub,    ///<sub
  BSWAY_SETMODE_MAX,
}BSWAY_SETMODE;

//--------------------------------------------------------------
/// �X�R�A�f�[�^�t���O �A�N�Z�X
//--------------------------------------------------------------
typedef enum
{
  BSWAY_SCOREDATA_FLAG_WIFI_LOSE_F,///<wifi�A���s��t���O
  BSWAY_SCOREDATA_FLAG_WIFI_UPLOAD,///<wifi���уA�b�v���[�h�t���O
  BSWAY_SCOREDATA_FLAG_WIFI_POKE_DATA,///<wifi�|�P�����X�g�b�N�t���O
  BSWAY_SCOREDATA_FLAG_SINGLE_POKE_DATA,///<single�|�P�����X�g�b�N�t���O
  BSWAY_SCOREDATA_FLAG_BOSS_CLEAR_SINGLE,///<�{�X�N���A�t���O �V���O��
  BSWAY_SCOREDATA_FLAG_BOSS_CLEAR_DOUBLE,///<�{�X�N���A�t���O �_�u��
  BSWAY_SCOREDATA_FLAG_BOSS_CLEAR_MULTI,///<�{�X�N���A�t���O �}���`
  BSWAY_SCOREDATA_FLAG_BOSS_CLEAR_S_SINGLE,///<�{�X�N���A�t���O S�V���O��
  BSWAY_SCOREDATA_FLAG_BOSS_CLEAR_S_DOUBLE,///<�{�X�N���A�t���O S�_�u��
  BSWAY_SCOREDATA_FLAG_BOSS_CLEAR_S_MULTI,///<�{�X�N���A�t���O S�}���`
  BSWAY_SCOREDATA_FLAG_SUPPORT_ENCOUNT_END,///<�T�|�[�g�����ς݃t���O
#if 0 //wb null
  BSWAY_SCOREDATA_FLAG_SILVER_GET,  ///<�V���o�[�g���t�B�[�Q�b�g
  BSWAY_SCOREDATA_FLAG_GOLD_GET,    ///<�S�[���h�g���t�B�[�Q�b�g
  BSWAY_SCOREDATA_FLAG_COPPER_GET,///<�J�b�p�[�g���t�B�[�Q�b�g
  BSWAY_SCOREDATA_FLAG_SILVER_READY,///<�V���o�[�Ⴆ�܂�
  BSWAY_SCOREDATA_FLAG_GOLD_READY,  ///<�S�[���h�Ⴆ�܂�
  BSWAY_SCOREDATA_FLAG_COPPER_READY,///<�J�b�p�[�Ⴆ�܂�
  BSWAY_SCOREDATA_FLAG_SINGLE_RECORD,///<�V���O�����R�[�h���풆�t���O
  BSWAY_SCOREDATA_FLAG_DOUBLE_RECORD,///<�_�u�����R�[�h���풆�t���O
  BSWAY_SCOREDATA_FLAG_MULTI_RECORD,///<�}���`���R�[�h���풆�t���O
  BSWAY_SCOREDATA_FLAG_CMULTI_RECORD,///<�ʐM�}���`���R�[�h���풆�t���O
  BSWAY_SCOREDATA_FLAG_WIFI_RECORD,///<Wifi���R�[�h���풆�t���O
#endif
  BSWAY_SCOREDATA_FLAG_MAX,
}BSWAY_SCOREDATA_FLAG;

//�t���O32bit�����`�F�b�N
#if (32 < BSWAY_SCOREDATA_FLAG_MAX)
#error (32 < BSWAY_SCOREDATA_FLAG_MAX)
#endif

//--------------------------------------------------------------
/// �v���C���[���b�Z�[�W�f�[�^�@�A�N�Z�XID
//--------------------------------------------------------------
typedef enum
{
 BSWAY_MSG_PLAYER_READY,
 BSWAY_MSG_PLAYER_WIN,
 BSWAY_MSG_PLAYER_LOSE,
 BSWAY_MSG_LEADER,
}BSWAY_PLAYER_MSG_ID;

//--------------------------------------------------------------
/// �X�R�A�f�[�^�@�|�P�����f�[�^�A�N�Z�X���[�h
//--------------------------------------------------------------
typedef enum
{
 BSWAY_SCORE_POKE_SINGLE,
 BSWAY_SCORE_POKE_WIFI,
}BSWAY_SCORE_POKE_DATA;

//--------------------------------------------------------------
/// BSWAY_PLAYMODE
//--------------------------------------------------------------
typedef enum
{
  BSWAY_PLAYMODE_SINGLE, //�V���O��
  BSWAY_PLAYMODE_DOUBLE, //�_�u��
  BSWAY_PLAYMODE_MULTI, //�}���`
  BSWAY_PLAYMODE_COMM_MULTI, //�ʐM�}���`
  BSWAY_PLAYMODE_WIFI, //Wifi
  BSWAY_PLAYMODE_S_SINGLE, //�X�[�p�[�V���O��
  BSWAY_PLAYMODE_S_DOUBLE, //�X�[�p�[�_�u��
  BSWAY_PLAYMODE_S_MULTI, //�X�[�p�[�}���`
  BSWAY_PLAYMODE_S_COMM_MULTI, //�X�[�p�[�ʐM�}���`
  BSWAY_PLAYMODE_MAX,
}BSWAY_PLAYMODE;

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// �o�g���T�u�E�F�C �v���C�f�[�^�\����
//--------------------------------------------------------------
typedef struct _BSUBWAY_PLAYDATA BSUBWAY_PLAYDATA;

//--------------------------------------------------------------
/// �o�g���T�u�E�F�C�@�X�R�A�f�[�^�\����
//--------------------------------------------------------------
typedef struct _BSUBWAY_SCOREDATA BSUBWAY_SCOREDATA;

//--------------------------------------------------------------
/// �o�g���T�u�E�F�C �v���C���[���b�Z�[�W�f�[�^�\����
//--------------------------------------------------------------
typedef struct _BSUBWAY_PLAYER_MSG  BSUBWAY_PLAYER_MSG;

//--------------------------------------------------------------
/// �o�g���T�u�E�F�C�@AI�}���`�y�A�̕ۑ����K�v�ȃ|�P�����p�����[�^
//  battle/bsubway_battle_data.h
//--------------------------------------------------------------
typedef struct _BSUBWAY_PAREPOKE_PARAM BSUBWAY_PAREPOKE_PARAM;

//--------------------------------------------------------------
/// �T�u�E�F�C�|�P�����f�[�^�^
/// battle/b_tower_data.h��K�v�ȉӏ��ȊO�ł�
/// include���Ȃ��ōςނ悤�ɒ�`���Ă���
//--------------------------------------------------------------
typedef struct _BSUBWAY_POKEMON  BSUBWAY_POKEMON;

//--------------------------------------------------------------
///  �T�u�E�F�C�p�[�g�i�[�f�[�^�^
///  �@battle/b_tower_data.h��K�v�ȉӏ��ȊO�ł�
///   include���Ȃ��ōςނ悤�ɒ�`���Ă���
//--------------------------------------------------------------
typedef struct _BSUBWAY_PARTNER_DATA  BSUBWAY_PARTNER_DATA;

//--------------------------------------------------------------
///  �T�u�E�F�C���[�_�[�f�[�^�^
///  �@battle/b_tower_data.h��K�v�ȉӏ��ȊO�ł�
///   include���Ȃ��ōςނ悤�ɒ�`���Ă���
//--------------------------------------------------------------
typedef struct _BSUBWAY_LEADER_DATA  BSUBWAY_LEADER_DATA;

//--------------------------------------------------------------
/// �o�g���T�u�E�F�C WIFI�f�[�^�^
//--------------------------------------------------------------
typedef struct _BSUBWAY_WIFI_DATA  BSUBWAY_WIFI_DATA;

//--------------------------------------------------------------
/// �o�g���T�u�E�F�C WIFI�v���C���[�f�[�^�^
//--------------------------------------------------------------
typedef struct _BSUBWAY_WIFI_PLAYER  BSUBWAY_WIFI_PLAYER;



//======================================================================
//  GAMEDATA����擾
//======================================================================
extern BSUBWAY_PLAYDATA * GAMEDATA_GetBSubwayPlayData( GAMEDATA* gamedata );
extern BSUBWAY_SCOREDATA * GAMEDATA_GetBSubwayScoreData( GAMEDATA* gamedata );
extern BSUBWAY_WIFI_DATA * GAMEDATA_GetBSubwayWifiData( GAMEDATA* gamedata );


//======================================================================
//  extern
//======================================================================
//BSUBWAY_PLAYDATA
extern int BSUBWAY_PLAYDATA_GetWorkSize( void );
extern void BSUBWAY_PLAYDATA_Init( BSUBWAY_PLAYDATA *bsw_play );
extern void BSUBWAY_PLAYDATA_SetSaveFlag(
    BSUBWAY_PLAYDATA *bsw_play, BOOL flag );
extern BOOL BSUBWAY_PLAYDATA_GetSaveFlag( BSUBWAY_PLAYDATA *bsw_play );
extern u32 BSUBWAY_PLAYDATA_GetData(
  const BSUBWAY_PLAYDATA *bsw_play, BSWAY_PLAYDATA_ID id, void *buf );
extern void BSUBWAY_PLAYDATA_SetData(
    BSUBWAY_PLAYDATA *bsw_play, BSWAY_PLAYDATA_ID id, const void *buf );
extern void BSUBWAY_PLAYDATA_AddWifiRecord(
    BSUBWAY_PLAYDATA *bsw_play, u8 down,u16 turn,u16 damage );
extern void BSUBWAY_PLAYDATA_ResetRoundNo( BSUBWAY_PLAYDATA *bsw_play );
extern void BSUBWAY_PLAYDATA_IncRoundNo( BSUBWAY_PLAYDATA *bsw_play );
extern void BSUBWAY_PLAYDATA_SetRoundNo( BSUBWAY_PLAYDATA *bsw_play, u8 round );
extern u16 BSUBWAY_PLAYDATA_GetRoundNo( const BSUBWAY_PLAYDATA *bsw_play );

//BSUBWAY_SCOREDATA
extern int BSUBWAY_SCOREDATA_GetWorkSize( void );
extern void BSUBWAY_SCOREDATA_Init( BSUBWAY_SCOREDATA *bsw_score );
extern void BSUBWAY_SCOREDATA_AddBattlePoint(
    BSUBWAY_SCOREDATA *bsw_score, u16 num );
extern void BSUBWAY_SCOREDATA_SubBattlePoint(
    BSUBWAY_SCOREDATA *bsw_score, u16 num );
extern void BSUBWAY_SCOREDATA_SetBattlePoint(
    BSUBWAY_SCOREDATA *bsw_score, u16 num );
extern u16 BSUBWAY_SCOREDATA_GetBattlePoint(
    const BSUBWAY_SCOREDATA *bsw_score );
extern void BSUBWAY_SCOREDATA_ResetRenshou(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode );
extern u16 BSUBWAY_SCOREDATA_GetRenshou(
    const BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode );
extern u16 BSUBWAY_SCOREDATA_GetRenshouMax(
    const BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode );
extern void BSUBWAY_SCOREDATA_SetRenshouMax(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode, u16 count );
extern void BSUBWAY_SCOREDATA_UpdateRenshouMax(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode, u16 count );
extern void BSUBWAY_SCOREDATA_SetRenshou(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode, u16 count );

extern void BSUBWAY_SCOREDATA_ErrorStageNo(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode );
extern void BSUBWAY_SCOREDATA_InitStageNo(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode );
extern BOOL BSUBWAY_SCOREDATA_CheckExistStageNo(
    const BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode );
extern void BSUBWAY_SCOREDATA_IncStageNo(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode );
extern void BSUBWAY_SCOREDATA_SetStageNo_Org1(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode, u16 stage );
extern u16 BSUBWAY_SCOREDATA_GetStageNo_Org0(
    const BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode );

#if 0
extern void BSUBWAY_SCOREDATA_IncStageCount(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode );
extern void BSUBWAY_SCOREDATA_ResetStageCount(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode );
extern u16 BSUBWAY_SCOREDATA_GetStageCount(
    const BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode );
#endif
extern BOOL BSUBWAY_SCOREDATA_SetFlag( BSUBWAY_SCOREDATA *bsw_score,
    BSWAY_SCOREDATA_FLAG id, BSWAY_SETMODE mode );
extern void BSUBWAY_SCOREDATA_IncWifiRank( BSUBWAY_SCOREDATA *bsw_score );
extern void BSUBWAY_SCOREDATA_DecWifiRank( BSUBWAY_SCOREDATA *bsw_score );
extern u8 BSUBWAY_SCOREDATA_GetWifiRank(
    const BSUBWAY_SCOREDATA *bsw_score );
extern void BSUBWAY_SCOREDATA_SetWifiRank( BSUBWAY_SCOREDATA *bsw_score, u8 rank );
extern u8 BSUBWAY_SCOREDATA_SetWifiLoseCount(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_SETMODE mode );
extern void BSUBWAY_SCOREDATA_SetUsePokeData( BSUBWAY_SCOREDATA *bsw_score, BSWAY_SCORE_POKE_DATA mode, const BSUBWAY_POKEMON* poke_tbl );
extern void BSUBWAY_SCOREDATA_GetUsePokeData( const BSUBWAY_SCOREDATA *bsw_score, BSWAY_SCORE_POKE_DATA mode, BSUBWAY_POKEMON* poke_tbl );
extern void BSUBWAY_SCOREDATA_SetWifiScore( BSUBWAY_SCOREDATA *bsw_score, const BSUBWAY_PLAYDATA *bsw_play );
extern void BSUBWAY_SCOREDATA_ClearWifiScore( BSUBWAY_SCOREDATA *bsw_score );
extern u16 BSUBWAY_SCOREDATA_GetWifiScore( const BSUBWAY_SCOREDATA *bsw_score );
extern u8 BSUBWAY_SCOREDATA_GetWifiNum( const BSUBWAY_SCOREDATA *bsw_score );
extern u8 BSUBWAY_SCOREDATA_DEBUG_GetFlag( const BSUBWAY_SCOREDATA *bsw_score );
extern void BSUBWAY_SCOREDATA_DEBUG_SetFlag( BSUBWAY_SCOREDATA *bsw_score, u8 flag );



//BSUBWAY_WIFIDATA
extern int BSUBWAY_WIFIDATA_GetWorkSize( void );
extern void BSUBWAY_WIFIDATA_Init( BSUBWAY_WIFI_DATA *bsw_wifi );
extern void BSUBWAY_WIFIDATA_SetRoomDataFlag( BSUBWAY_WIFI_DATA *bsw_wifi, u8 rank, u8 room, const RTCDate *day );
extern void BSUBWAY_WIFIDATA_ClearRoomDataFlag( BSUBWAY_WIFI_DATA *bsw_wifi );
extern BOOL BSUBWAY_WIFIDATA_CheckRoomDataFlag( BSUBWAY_WIFI_DATA *bsw_wifi, u8 rank, u8 room, const RTCDate *day );
extern BOOL BSUBWAY_WIFIDATA_CheckPlayerDataEnable( const BSUBWAY_WIFI_DATA *bsw_wifi );
extern BOOL BSUBWAY_WIFIDATA_CheckLeaderDataEnable( const BSUBWAY_WIFI_DATA *bsw_wifi );
extern void BSUBWAY_WIFIDATA_SetPlayerData( BSUBWAY_WIFI_DATA *bsw_wifi, const BSUBWAY_WIFI_PLAYER* dat, u8 rank, u8 room );
extern void BSUBWAY_WIFIDATA_ClearPlayerData( BSUBWAY_WIFI_DATA *bsw_wifi );
extern u8 BSUBWAY_WIFIDATA_GetPlayerRank( const BSUBWAY_WIFI_DATA *bsw_wifi );
extern u8 BSUBWAY_WIFIDATA_GetPlayerRoomNo( const BSUBWAY_WIFI_DATA *bsw_wifi );
extern void BSUBWAY_WIFIDATA_GetBtlPlayerData( const BSUBWAY_WIFI_DATA *bsw_wifi, BSUBWAY_PARTNER_DATA *player, u8 round, HEAPID heapID );

extern void BSUBWAY_WIFIDATA_SetLeaderData( BSUBWAY_WIFI_DATA *bsw_wifi, const BSUBWAY_LEADER_DATA* dat, u8 rank, u8 room );
extern void BSUBWAY_WIFIDATA_ClearLeaderData( BSUBWAY_WIFI_DATA *bsw_wifi );
extern u8 BSUBWAY_WIFIDATA_GetLeaderRank( const BSUBWAY_WIFI_DATA *bsw_wifi );
extern u8 BSUBWAY_WIFIDATA_GetLeaderRoomNo( const BSUBWAY_WIFI_DATA *bsw_wifi );
extern BSUBWAY_LEADER_DATA* BSUBWAY_WIFIDATA_GetLeaderDataAlloc( const BSUBWAY_WIFI_DATA *bsw_wifi, HEAPID heapID );
extern int BSUBWAY_LEADERDATA_GetDataNum( const BSUBWAY_LEADER_DATA *bsw_leader );

#define BSUBWAY_SCOREDATA_GetRenshouCount(a,b) \
  BSUBWAY_SCOREDATA_GetRenshou(a,b)
#define BSUBWAY_SCOREDATA_GetMaxRenshouCount(a,b) \
  BSUBWAY_SCOREDATA_GetRenshouMax(a,b)


#ifdef PM_DEBUG
// �_�~�[����ݒ�
extern void DEBUG_BSUBWAY_WIFIDATA_SetPlayerData( BSUBWAY_WIFI_DATA *bsw_wifi, u8 rank, u8 room );
extern void DEBUG_BSUBWAY_WIFIDATA_SetLeaderData( BSUBWAY_WIFI_DATA *bsw_wifi, u8 rank, u8 room );
#endif

#endif //__H_BSUBWAY_SAVEDATA_H__
