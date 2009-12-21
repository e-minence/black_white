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

//#include "savedata/bsubway_savedata_def.h" //����񂩂�

//======================================================================
//  define
//======================================================================
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
  BSWAY_PLAYDATA_ID_rnd_seed,
  BSWAY_PLAYDATA_ID_MAX,
}BSWAY_PLAYDATA_ID;

//--------------------------------------------------------------
/// �X�R�A�f�[�^���샂�[�h
//--------------------------------------------------------------
typedef enum
{
  BSWAY_SETMODE_get,		///<�f�[�^�擾
  BSWAY_SETMODE_set,		///<�Z�b�g
  BSWAY_SETMODE_reset,	///<���Z�b�g
  BSWAY_SETMODE_inc,		///<1++
  BSWAY_SETMODE_dec,		///<1--
  BSWAY_SETMODE_add,		///<add
  BSWAY_SETMODE_sub,		///<sub
  BSWAY_SETMODE_MAX,
}BSWAY_SETMODE;

//--------------------------------------------------------------
/// �X�R�A�f�[�^�t���O �A�N�Z�X
//--------------------------------------------------------------
typedef enum
{
  BSWAY_SCOREDATA_FLAG_SILVER_GET,
  BSWAY_SCOREDATA_FLAG_GOLD_GET,
  BSWAY_SCOREDATA_FLAG_SILVER_READY,
  BSWAY_SCOREDATA_FLAG_GOLD_READY,
  BSWAY_SCOREDATA_FLAG_WIFI_LOSE_F,
  BSWAY_SCOREDATA_FLAG_WIFI_UPLOAD,
  BSWAY_SCOREDATA_FLAG_WIFI_POKE_DATA,
  BSWAY_SCOREDATA_FLAG_SINGLE_POKE_DATA,
  BSWAY_SCOREDATA_FLAG_SINGLE_RECORD,
  BSWAY_SCOREDATA_FLAG_DOUBLE_RECORD,
  BSWAY_SCOREDATA_FLAG_MULTI_RECORD,
  BSWAY_SCOREDATA_FLAG_CMULTI_RECORD,
  BSWAY_SCOREDATA_FLAG_WIFI_RECORD,
  BSWAY_SCOREDATA_FLAG_COPPER_GET,
  BSWAY_SCOREDATA_FLAG_COPPER_READY,
  BSWAY_SCOREDATA_FLAG_WIFI_MULTI_RECORD,		//�v���`�i�ǉ�
  BSWAY_SCOREDATA_FLAG_MAX,
}BSWAY_SCOREDATA_FLAG;

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
  BSWAY_PLAYMODE_RETRY, //���g���C
  BSWAY_PLAYMODE_WIFI_MULTI, //Wifi�}���`
  BSWAY_PLAYMODE_MAX,
}BSWAY_PLAYMODE;

//--------------------------------------------------------------
/// �萔
//--------------------------------------------------------------
///�I������莝���|�P��������MAX
#define BSUBWAY_STOCK_MEMBER_MAX	(4)
///�ۑ����Ă����ΐ�g���[�i�[�i���o�[�̐�
#define BSUBWAY_STOCK_TRAINER_MAX	(14)
///�ۑ����Ă���AI�}���`�y�A�̃|�P�����p�����[�^��
#define BSUBWAY_STOCK_PAREPOKE_MAX	(2)
///WiFi DL�v���C���[�f�[�^��
#define BSUBWAY_STOCK_WIFI_PLAYER_MAX	(7)
///WiFi DL���[�_�[�f�[�^��
#define BSUBWAY_STOCK_WIFI_LEADER_MAX	(30)
///���[���f�[�^��DL�t���O�G���A�o�b�t�@��
#define BSUBWAY_ROOM_DATA_FLAGS_LEN	(250)

//kari pt_save.h
#define	WAZA_TEMOTI_MAX		(4)		///<1�̂̃|�P���������Ă�Z�̍ő�l

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

//BSUBWAY_SCOREDATA
extern int BSUBWAY_SCOREDATA_GetWorkSize( void );
extern void BSUBWAY_SCOREDATA_Init( BSUBWAY_SCOREDATA *bsw_score );
extern u16 BSUBWAY_SCOREDATA_SetBattlePoint(
    BSUBWAY_SCOREDATA *bsw_score, u16 num, BSWAY_SETMODE mode );
extern u16 BSUBWAY_SCOREDATA_SetStage(
    BSUBWAY_SCOREDATA *bsw_score, u16 id, BSWAY_SETMODE mode );
extern BOOL BSUBWAY_SCOREDATA_SetFlag( BSUBWAY_SCOREDATA *bsw_score,
    BSWAY_SCOREDATA_FLAG id, BSWAY_SETMODE mode );
extern u8 BSUBWAY_SCOREDATA_SetWifiRank(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_SETMODE mode );
extern u16 BSUBWAY_SCOREDATA_CalcRenshou(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_SETMODE mode );
extern u16 BSUBWAY_SCOREDATA_GetRenshouCount( const BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode );
extern u16 BSUBWAY_SCOREDATA_GetMaxRenshouCount( const BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode );

//----
#if 0 //wb null
//----
//--------------------------------------------------------------
///    �o�g���T�u�E�F�C�@WIFI�|�P�����f�[�^�\���̂ւ̕s���S�^�|�C���^
//--------------------------------------------------------------
//typedef struct _BSUBWAY_POKE  BSUBWAY_POKE;

//--------------------------------------------------------------
///    �o�g���T�u�E�F�C�@WIFI�v���C���[�f�[�^�\���̂ւ̕s���S�^�|�C���^
//--------------------------------------------------------------
//typedef struct _BSUBWAY_WIFI_PLAYER  BSUBWAY_WIFI_PLAYER;

//--------------------------------------------------------------
///    �o�g���T�u�E�F�C�@WIFI���[�_�[�f�[�^�\���̂ւ̕s���S�^�|�C���^
//--------------------------------------------------------------
//typedef struct _BSUBWAY_WIFI_LEADER  BSUBWAY_WIFI_LEADER;

//--------------------------------------------------------------
///    �o�g���T�u�E�F�C�@WIFI�f�[�^�\���̂ւ̕s���S�^�|�C���^
//--------------------------------------------------------------
//typedef struct _BSUBWAY_WIFI_DATA  BSUBWAY_WIFI_DATA;


//--------------------------------------------------------------
///  �T�u�E�F�C�p�[�g�i�[�f�[�^�^
///  �@battle/b_tower_data.h��K�v�ȉӏ��ȊO�ł�
///   include���Ȃ��ōςނ悤�ɒ�`���Ă���
//--------------------------------------------------------------
//typedef struct _BSUBWAY_PARTNER_DATA  BSUBWAY_PARTNER_DATA;

//--------------------------------------------------------------
///    �o�g���T�u�E�F�C�@WIFI���[�_�[�f�[�^�\���̂ւ̕s���S�^�|�C���^
///   battle/b_tower_data.h��K�v�ȉӏ��ȊO�ł�
///   include���Ȃ��ōςނ悤�ɒ�`���Ă���
///  ���g�͌����܂��񂪃|�C���^�o�R�ŎQ�Ƃł��܂�
//--------------------------------------------------------------
//typedef struct _BSUBWAY_LEADER_DATA  BSUBWAY_LEADER_DATA;

//--------------------------------------------------------------
///     �o�g���T�u�E�F�C WIFI���C�u�����\���̂ւ̕s���S�|�C���^
///   Wifi���C�u�����w�b�_���O���ɔ���J�ɂ��Ă������߂ɒ�`
//--------------------------------------------------------------
#if 0
typedef struct Dpw_Bt_PokemonData  DPW_BT_POKEMON_DATA;
typedef struct Dpw_Bt_Player  DPW_BT_PLAYER;
typedef struct Dpw_Bt_Leader  DPW_BT_LEADER;
typedef struct Dpw_Bt_Room    DPW_BT_ROOM;
#endif

//--------------------------------------------------------------
///   @biref  �o�g���T�u�E�F�C WIFI���[���f�[�^ID(rank��roomno)
/// 
///   �����N�ƃ��[���i���o�[���擾�������Ƃ��悤�̌��J�\���̃f�[�^�^
//--------------------------------------------------------------
#if 0
typedef struct
{
  u8  rank;  //���[�������N(1-10)
  u8  no;    //���[��No(1-200)
}BTOWER_ROOMID;
#endif


//--------------------------------------------------------------
///     �T�u�E�F�C�@�|�P�����f�[�^�T�C�Y
//--------------------------------------------------------------
extern int BSUBWAY_WifiPoke_GetWorkSize(void);

//--------------------------------------------------------------
///     �T�u�E�F�C�@Wifi�f�[�^�T�C�Y
//--------------------------------------------------------------
extern int  BSUBWAY_WifiData_GetWorkSize(void);
//--------------------------------------------------------------
///   �@�T�u�E�F�C Wifi/�g���[�i�[���[�h�v���C���[�f�[�^�\���̃f�[�^�T�C�Y
//--------------------------------------------------------------
extern int BSUBWAY_DpwBtPlayer_GetWorkSize(void);

//--------------------------------------------------------------
///     �T�u�E�F�C�@�v���C�f�[�^�N���A
//--------------------------------------------------------------
extern void BSUBWAY_PlayData_Clear(BSUBWAY_PLAYWORK* dat);

//--------------------------------------------------------------
///     �T�u�E�F�C  �X�R�A�f�[�^�N���A
//--------------------------------------------------------------
extern void BSUBWAY_ScoreData_Clear(BSUBWAY_SCOREWORK* dat);

//--------------------------------------------------------------
///     �T�u�E�F�C�@�v���C���[���b�Z�[�W�f�[�^�N���A
//--------------------------------------------------------------
extern void BSUBWAY_PlayerMsg_Clear(BSUBWAY_PLAYER_MSG* dat);

//--------------------------------------------------------------
///     �T�u�E�F�C�@Wifi�f�[�^�N���A
//--------------------------------------------------------------
extern void BSUBWAY_WifiData_Clear(BSUBWAY_WIFI_DATA* dat);

//======================================================================
//  extern �u���b�N�f�[�^�A�N�Z�X�n
//======================================================================
#if 0 //wb
//--------------------------------------------------------------
///     �T�u�E�F�C�@�v���C�f�[�^�ւ̃|�C���^���擾
//--------------------------------------------------------------
extern BSUBWAY_PLAYWORK* SaveData_GetTowerPlayData(SAVEDATA* sv);

//--------------------------------------------------------------
///     �T�u�E�F�C�@�X�R�A�f�[�^�ւ̃|�C���^���擾
//--------------------------------------------------------------
extern BSUBWAY_SCOREWORK* SaveData_GetTowerScoreData(SAVEDATA* sv);

//--------------------------------------------------------------
///     �T�u�E�F�C Wifi�f�[�^�ւ̃|�C���^���擾  
//--------------------------------------------------------------
extern BSUBWAY_WIFI_DATA*  SaveData_GetTowerWifiData(SAVEDATA* sv);
#endif

//--------------------------------------------------------------
///     �v���C�f�[�^�@�擾
///   @param  dat  BTLTOWRE_PLAYWORK*
///   @param  id  �擾����f�[�^ID BSUBWAY_PSD_ID�^
///   @param  buf  void*:�f�[�^�擾�|�C���^
//--------------------------------------------------------------
extern u32 BSUBWAY_PlayData_Get(
    BSUBWAY_PLAYWORK* dat,BSWAY_PSD_ID id,void* buf);

//--------------------------------------------------------------
///     �v���C�f�[�^�@�Z�b�g
///   @param  dat  BTLTOWRE_PLAYWORK*
///   @param  id  �擾����f�[�^ID BTOWER_PSD_ID�^
///   @param  buf  void*:�f�[�^���i�[�����o�b�t�@�ւ̃|�C���^
//--------------------------------------------------------------
extern void BSUBWAY_PlayData_Put(
    BSUBWAY_PLAYWORK* dat,BSWAY_PSD_ID id,const void* buf);

//--------------------------------------------------------------
///    �v���C�f�[�^�@Wifi���R�[�h�f�[�^Add  
///   @param  down  �|���ꂽ�|�P�����ǉ���
///   @param  turn  ���������^�[���ǉ���
///   @param  damage  �󂯂��_���[�W�ǉ��l
//--------------------------------------------------------------
extern void BSUBWAY_PlayData_WifiRecordAdd(
    BSUBWAY_PLAYWORK* dat,u8 down,u16 turn,u16 damage);

//--------------------------------------------------------------
///     �v���C�f�[�^�@�������Z�[�u�ς݂��ǂ����H
///   @retval  TRUE  �������Z�[�u����Ă���
///   @retval FALSE  �Z�[�u����Ă��Ȃ�
//--------------------------------------------------------------
extern BOOL BSUBWAY_PlayData_GetSaveFlag(BSUBWAY_PLAYWORK* dat);

//--------------------------------------------------------------
///     �v���C�f�[�^�@�Z�[�u��ԃt���O���Z�b�g
//--------------------------------------------------------------
extern void BSUBWAY_PlayData_SetSaveFlag(BSUBWAY_PLAYWORK* dat,BOOL flag);

//======================================================================
//  extern �X�R�A�f�[�^�擾�n
//======================================================================
//--------------------------------------------------------------
///     �X�R�A�f�[�^ �o�g���|�C���g�Z�b�g
///   �L���R�}���h set/get/add/sub
//--------------------------------------------------------------
extern u16 BSUBWAY_ScoreData_SetBattlePoint(
    BSUBWAY_SCOREWORK* dat,u16 num,BSWAY_DATA_SETID mode);

//--------------------------------------------------------------
///     SAVEDATA����o�g���|�C���g��Get
//--------------------------------------------------------------
#if 0 //wb
static inline u16 BattlePoint_Get(SAVEDATA* sv)
{
  return BSUBWAY_ScoreData_SetBattlePoint(
      SaveData_GetTowerScoreData(sv),0,BSWAY_DATA_get);
}
#endif

//--------------------------------------------------------------
///     SAVEDATA����o�g���|�C���g��Set
//--------------------------------------------------------------
#if 0 //wb
static inline u16 BattlePoint_Set(SAVEDATA* sv,u16 num)
{
  return BSUBWAY_ScoreData_SetBattlePoint(
      SaveData_GetTowerScoreData(sv),num,BSWAY_DATA_set);
}
#endif

//--------------------------------------------------------------
///     SAVEDATA����o�g���|�C���g��Add
//--------------------------------------------------------------
#if 0 //wb
static inline u16  BattlePoint_Add(SAVEDATA* sv,u16 num)
{
  return BSUBWAY_ScoreData_SetBattlePoint(
      SaveData_GetTowerScoreData(sv),num,BSWAY_DATA_add);
}
#endif

//--------------------------------------------------------------
///     SAVEDATA����o�g���|�C���g��Sub
//--------------------------------------------------------------
#if 0 //wb
static inline u16  BattlePoint_Sub(SAVEDATA* sv,u16 num)
{
  return BSUBWAY_ScoreData_SetBattlePoint(
      SaveData_GetTowerScoreData(sv),num,BSWAY_DATA_sub);
}
#endif

//--------------------------------------------------------------
///     �X�R�A�f�[�^�@Wifi�A���s��J�E���g����
///   �L���R�}���h get/reset/inc
///   @return  �����̃J�E���g
//--------------------------------------------------------------
extern u8 BSUBWAY_ScoreData_SetWifiLoseCount(
    BSUBWAY_SCOREWORK* dat,BSWAY_DATA_SETID mode);

//--------------------------------------------------------------
///     �X�R�A�f�[�^�@Wifi�����N����
/// 
///   �L���R�}���h get/reset/inc/dec
//--------------------------------------------------------------
extern u8  BSUBWAY_ScoreData_SetWifiRank(
    BSUBWAY_SCOREWORK* dat,BSWAY_DATA_SETID mode);

//--------------------------------------------------------------
///     �g�p�|�P�����f�[�^�ۑ�
//--------------------------------------------------------------
extern void BSUBWAY_ScoreData_SetUsePokeData( BSUBWAY_SCOREWORK* dat,
    BSWAY_SCORE_POKE_DATA mode,BSUBWAY_POKEMON* poke);

//--------------------------------------------------------------
///     �g�p�|�P�����f�[�^���o�b�t�@�ɃR�s�[���Ď擾(WiFi�\���̃f�[�^�^)
//--------------------------------------------------------------
extern void BSUBWAY_ScoreData_GetUsePokeDataDpw(BSUBWAY_SCOREWORK* dat,
      BSWAY_SCORE_POKE_DATA mode,DPW_BT_POKEMON_DATA* poke);

//--------------------------------------------------------------
/// �g�p�|�P�����f�[�^���o�b�t�@�ɃR�s�[���Ď擾
//--------------------------------------------------------------
extern void BSUBWAY_ScoreData_GetUsePokeData(BSUBWAY_SCOREWORK* dat,
      BSWAY_SCORE_POKE_DATA mode,BSUBWAY_POKEMON* poke);

//--------------------------------------------------------------
///     �X�R�A�f�[�^�@���񐔑���
//--------------------------------------------------------------
extern u16 BSUBWAY_ScoreData_SetStage(BSUBWAY_SCOREWORK* dat,u16 id,BSWAY_DATA_SETID mode);

//--------------------------------------------------------------
///     ���݂̃X�e�[�W�l�𒼐ڃZ�b�g����
//--------------------------------------------------------------
extern u16 BSUBWAY_ScoreData_SetStageValue(BSUBWAY_SCOREWORK* dat,u16 id,u16 value);

#ifdef PM_DEBUG
//--------------------------------------------------------------
///     �f�o�b�O����@���݂̃X�e�[�W�l�𒼐ڃZ�b�g����
//--------------------------------------------------------------
extern u16 BSUBWAY_ScoreData_DebugSetStageValue(BSUBWAY_SCOREWORK* dat,u16 id,u16 value);
#endif  //PM_DEBUG

//--------------------------------------------------------------
///     �X�R�A�f�[�^ Wifi���ё���
//--------------------------------------------------------------
extern u16  BSUBWAY_ScoreData_SetWifiScore(BSUBWAY_SCOREWORK* dat,BSUBWAY_PLAYWORK *playdata);

//--------------------------------------------------------------
///     �X�R�A�f�[�^�@Wifi����0�N���A
//--------------------------------------------------------------
extern void BSUBWAY_ScoreData_ClearWifiScore(BSUBWAY_SCOREWORK* dat);

//--------------------------------------------------------------
///     �X�R�A�f�[�^�@Wifi���ю擾
//--------------------------------------------------------------
extern u16  BSUBWAY_ScoreData_GetWifiScore(BSUBWAY_SCOREWORK* dat);

//--------------------------------------------------------------
///     �X�R�A�f�[�^���珟�������������擾
//--------------------------------------------------------------
extern u8  BSUBWAY_ScoreData_GetWifiWinNum(BSUBWAY_SCOREWORK* dat);

//--------------------------------------------------------------
///     �X�R�A�f�[�^ �t���O�G���A�Z�b�g
/// 
///   �L���R�}���h�Freset/set/get
//--------------------------------------------------------------
extern BOOL BSUBWAY_ScoreData_SetFlags(BSUBWAY_SCOREWORK* dat,u16 id,BSWAY_DATA_SETID mode);

//--------------------------------------------------------------
///     �T�u�E�F�C�p���t�ω������_���V�[�h�ۑ�
//--------------------------------------------------------------
extern void BSUBWAY_ScoreData_SetDayRndSeed(BSUBWAY_SCOREWORK* dat,u32 rnd_seed);

//--------------------------------------------------------------
///     �T�u�E�F�C�p���t�ω������_���V�[�h�擾
//--------------------------------------------------------------
extern u32 BSUBWAY_ScoreData_GetDayRndSeed(BSUBWAY_SCOREWORK* dat);

//======================================================================
//�@extern �T�u�E�F�C�@�v���C���[���b�Z�[�W�f�[�^�A�N�Z�X�n
//======================================================================
#if 0 //wb
//--------------------------------------------------------------
///     �ȈՉ�b�f�[�^���Z�b�g
//--------------------------------------------------------------
extern void BSUBWAY_PlayerMsg_Set(
    SAVEDATA* sv,BSWAY_PLAYER_MSG_ID id,PMS_DATA* src);

//--------------------------------------------------------------
///     �ȈՉ�b�f�[�^���擾
//--------------------------------------------------------------
extern PMS_DATA* BSUBWAY_PlayerMsg_Get(
    SAVEDATA* sv,BSWAY_PLAYER_MSG_ID id);
#endif

//======================================================================
//  extern�@�T�u�E�F�C�@Wifi�f�[�^�A�N�Z�X�n
//======================================================================
#if 0 //wb
//--------------------------------------------------------------
///     �w�肵�����[���f�[�^�̎擾�t���O�𗧂Ă�
/// 
///   @param  rank  1�I���W���Ȃ̂Œ���
///   @param  roomno  1�I���W���Ȃ̂Œ���
//--------------------------------------------------------------
extern void BSUBWAY_WifiData_SetRoomDataFlag(BSUBWAY_WIFI_DATA* dat,
    u8 rank,u8 roomno,RTCDate *day);

//--------------------------------------------------------------
///     ���[���f�[�^�擾�t���O���N���A����
//--------------------------------------------------------------
extern void BSUBWAY_WifiData_ClearRoomDataFlag(BSUBWAY_WIFI_DATA* dat);

//--------------------------------------------------------------
///     �w�肵�����[����DL�t���O�������Ă��邩�`�F�b�N
///   @param  rank  1�I���W���Ȃ̂Œ���
///   @param  roomno  1�I���W���Ȃ̂Œ���
//--------------------------------------------------------------
extern BOOL BSUBWAY_WifiData_CheckRoomDataFlag(BSUBWAY_WIFI_DATA* dat,u8 rank,u8 roomno,RTCDate* day);

//--------------------------------------------------------------
///     �v���C���[�f�[�^�����݂��邩�`�F�b�N
//--------------------------------------------------------------
extern BOOL BSUBWAY_WifiData_IsPlayerDataEnable(BSUBWAY_WIFI_DATA* dat);

//--------------------------------------------------------------
///     ���[�_�[�f�[�^�����邩�ǂ����`�F�b�N
//--------------------------------------------------------------
extern BOOL BSUBWAY_WifiData_IsLeaderDataEnable(BSUBWAY_WIFI_DATA* dat);

//--------------------------------------------------------------
///     �v���C���[�f�[�^���Z�[�u
///   
///   @param  rank  1�I���W���Ȃ̂Œ���
///   @param  roomno  1�I���W���Ȃ̂Œ���
//--------------------------------------------------------------
extern void BSUBWAY_WifiData_SetPlayerData(BSUBWAY_WIFI_DATA* dat,DPW_BT_PLAYER* src,u8 rank,u8 roomno);

//--------------------------------------------------------------
///     �v���C���[�f�[�^���N���A
//--------------------------------------------------------------
extern void BSUBWAY_WifiData_ClearPlayerData(BSUBWAY_WIFI_DATA* dat);

//--------------------------------------------------------------
///     �v���C���[�f�[�^��RoomID(�����N��roomNo)���擾
/// 
///   @param  roomid  BTOWER_ROOMID�^(b_tower.h�Ō��J)
//--------------------------------------------------------------
extern void BSUBWAY_WifiData_GetPlayerDataRoomID(BSUBWAY_WIFI_DATA* dat,BTOWER_ROOMID *roomid);

//--------------------------------------------------------------
///     �v���C���[�f�[�^���T�u�E�F�C�퓬�p�ɉ�
//--------------------------------------------------------------
extern void BSUBWAY_WifiData_GetBtlPlayerData(BSUBWAY_WIFI_DATA* dat,
    BSUBWAY_PARTNER_DATA* player,const u8 round);

//--------------------------------------------------------------
///     ���[�_�[�f�[�^���Z�[�u
///   
///   @param  rank  1�I���W���Ȃ̂Œ���
///   @param  roomno  1�I���W���Ȃ̂Œ���
//--------------------------------------------------------------
extern void BSUBWAY_WifiData_SetLeaderData(
    BSUBWAY_WIFI_DATA* dat,DPW_BT_LEADER* src,u8 rank,u8 roomno);

//--------------------------------------------------------------
///     ���[�_�[�f�[�^���N���A
//--------------------------------------------------------------
extern void BSUBWAY_WifiData_ClearLeaderData(BSUBWAY_WIFI_DATA* dat);

//--------------------------------------------------------------
///     ���[�_�[�f�[�^��RoomID(�����N��roomNo)���擾
/// 
///   @param  roomid  BTOWER_ROOMID�^(b_tower.h�Ō��J)
//--------------------------------------------------------------
extern void BSUBWAY_WifiData_GetLeaderDataRoomID(
    BSUBWAY_WIFI_DATA* dat,BTOWER_ROOMID *roomid);

//--------------------------------------------------------------
///     ���[�_�[�f�[�^��Alloc�����������ɃR�s�[���Ď擾����
/// 
///   ��������Alloc�����������̈��Ԃ��̂ŁA
///   �Ăяo�����������I�ɉ�����邱�ƁI
//--------------------------------------------------------------
extern BSUBWAY_LEADER_DATA* BSUBWAY_WifiData_GetLeaderDataAlloc(
    BSUBWAY_WIFI_DATA* dat,int heapID) ;
#endif
//----
#endif //wb null
//----
#endif //__H_BSUBWAY_SAVEDATA_H__
