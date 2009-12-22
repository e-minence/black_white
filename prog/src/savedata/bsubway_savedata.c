//======================================================================
/**
 * @file  bsubway_savedata.c
 * @brief  �o�g���T�u�E�F�C�@�Z�[�u�f�[�^�֘A
 * @authaor  iwasawa
 * @date  2008.12.11
 * @note PL���ڐA kagaya
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "libdpw/dpw_bt.h"

#include "savedata/bsubway_savedata.h"
#include "bsubway_savedata_local.h"
#include "../field/bsubway_scr_def.h"

#if 0 //wb null
#ifdef _NITRO
// �\���̂��z��̃T�C�Y�ƂȂ��Ă��邩�`�F�b�N
SDK_COMPILER_ASSERT(sizeof(BSUBWAY_LEADER_DATA) == 34);
SDK_COMPILER_ASSERT(sizeof(BSUBWAY_POKEMON) == 56);
SDK_COMPILER_ASSERT(sizeof(BSUBWAY_WIFI_PLAYER) == 228);
#endif
#endif

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// �o�g���T�u�E�F�C�v���C�i�s�f�[�^
/// ����J�n���ɃN���A�����
/// size=20byte
//--------------------------------------------------------------
struct _BSUBWAY_PLAYDATA
{
  u8  itemfix_f  :1;  ///<�Œ�A�C�e�����ǂ����̃t���O
  u8  saved_f    :1;  ///<�Z�[�u�ς݂��ǂ���
  u8  play_mode  :3;  ///<���݂ǂ��Ƀ`�������W����?
  u8  partner    :3;  ///<���ݒN�Ƒg��ł��邩?
  u8  dummy; ///<4byte���E�_�~�[
  u8  tower_round;    ///<�o�g���T�u�E�F�C�����l�ځH
  u8  wifi_rec_down;    ///<���������܂łɓ|���ꂽ�|�P������
  u16  wifi_rec_turn;    ///<���������ɂ��������^�[����
  u16  wifi_rec_damage;  ///<���������܂łɎ󂯂��_���[�W��
  
  u8  member_poke[BSUBWAY_STOCK_MEMBER_MAX];    ///<�I�������|�P�����̈ʒu
  u16  trainer_no[BSUBWAY_STOCK_TRAINER_MAX];    ///<�ΐ�g���[�i�[No�ۑ�
  
  u32  play_rnd_seed;  ///<�^���[�v���C�ω������_���V�[�h�ۑ��ꏊ
  
  ///<AI�}���`�̃y�A�̃|�P�����Đ����ɕK�v�ȃp�����[�^
  struct _BSUBWAY_PAREPOKE_PARAM  pare_poke;
};

//--------------------------------------------------------------
/// �o�g���T�u�E�F�C�@�X�R�A�f�[�^
/// size= 20+168+168=356byte
/// �Q�[����ʂ��ĕۑ�&�Ǘ����Ă����A�v���C���[����
//--------------------------------------------------------------
struct _BSUBWAY_SCOREDATA
{
  u16  btl_point;  ///<�o�g���|�C���g
  u8  wifi_lose;  ///<�A���s��J�E���g
  u8  wifi_rank;  ///<WiFi�����N
  
  u32  day_rnd_seed;  ///<�^���[�p���t�ω������_���V�[�h�ۑ��ꏊ
  
  union{
    struct{
    u16  silver_get:1;    ///<�V���o�[�g���t�B�[�Q�b�g
    u16  gold_get:1;      ///<�S�[���h�g���t�B�[�Q�b�g
    u16  silver_ready:1;    ///<�V���o�[�Ⴆ�܂�
    u16  gold_ready:1;    ///<�S�[���h�Ⴆ�܂�
    u16  wifi_lose_f:1;    ///<wifi�A���s��t���O
    u16  wifi_update:1;    ///<wifi���уA�b�v���[�h�t���O
    u16  wifi_poke_data:1;  ///<wifi�|�P�����f�[�^�X�g�b�N�L��i�V�t���O
    u16  single_poke_data:1;  ///<single�|�P�����f�[�^�X�g�b�N�L��i�V�t���O
    u16  single_record:1;  ///<�V���O�����R�[�h���풆�t���O
    u16  double_record:1;  ///<�_�u�����R�[�h���풆�t���O
    u16  multi_record:1;    ///<�}���`���R�[�h���풆�t���O
    u16  cmulti_record:1;  ///<�ʐM�}���`���R�[�h���풆�t���O
    u16  wifi_record:1;    ///<Wifi���R�[�h���풆�t���O
    u16  copper_get:1;    ///<�J�b�p�[�g���t�B�[�Q�b�g
    u16  copper_ready:1;    ///<�J�b�p�[�Ⴆ�܂�
    u16  :1;  ///<���E�_�~�[
    };
    u16  flags;
  };
  
  ///<�o�g���T�u�E�F�C����(padding��WIFI_MULTI�p�ɉ�����)
  u16  tower_stage[6];
  
  //�A���L�^
  u16 renshou[BSWAY_PLAYMODE_MAX];
  u16 renshou_max[BSWAY_PLAYMODE_MAX];
 
  //WiFi�`�������W�f�[�^
  u16  wifi_score;  ///<WiFi����
  
  //WiFi�|�P�����f�[�^�X�g�b�N
  struct _BSUBWAY_POKEMON  wifi_poke[3];
  //�g���[�i�[���[�h�p�V���O���f�[�^�X�g�b�N
  struct _BSUBWAY_POKEMON  single_poke[3];
};

//======================================================================
//  proto
//======================================================================

//======================================================================
//  �o�g���T�u�E�F�C�@�v���C�f�[�^
//======================================================================
//--------------------------------------------------------------
/**
 * �v���C�f�[�^�T�C�Y
 * @param nothing
 * @retval int ���[�N�T�C�Y
 */
//--------------------------------------------------------------
int BSUBWAY_PLAYDATA_GetWorkSize( void )
{
  return( sizeof(BSUBWAY_PLAYDATA) );
}

//--------------------------------------------------------------
/**
 * �v���C�f�[�^�@������
 * @param bsw_play BSUBWAY_PLAYDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_PLAYDATA_Init( BSUBWAY_PLAYDATA *bsw_play )
{
  MI_CpuClear8( bsw_play, sizeof(BSUBWAY_PLAYDATA) );
}

//--------------------------------------------------------------
/**
 * �v���C�f�[�^�@�Z�[�u�t���O�Z�b�g
 * @param bsw_play BSUBWAY_PLAYDATA
 * @param flag TRUE=�Z�[�u���� FALSE=�Ȃ�
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_PLAYDATA_SetSaveFlag( BSUBWAY_PLAYDATA *bsw_play, BOOL flag )
{
  bsw_play->saved_f = flag;
}

//--------------------------------------------------------------
/**
 *  �v���C�f�[�^�@�������Z�[�u�ς݂��ǂ����H
 *  @retval  TRUE  �������Z�[�u����Ă���
 *  @retval FALSE  �Z�[�u����Ă��Ȃ�
 */
//--------------------------------------------------------------
BOOL BSUBWAY_PLAYDATA_GetSaveFlag( BSUBWAY_PLAYDATA *bsw_play )
{
  return bsw_play->saved_f;
}

//--------------------------------------------------------------
/**
 * �v���C�f�[�^�@�擾
 * @param bsw_play BSUBWAY_PLAYDATA
 * @param id �擾����BSWAY_PLAYDATA_ID
 * @param buf �f�[�^�擾�|�C���^
 * @retval u32 ID����擾��������
 */
//--------------------------------------------------------------
u32 BSUBWAY_PLAYDATA_GetData(
    const BSUBWAY_PLAYDATA *bsw_play, BSWAY_PLAYDATA_ID id, void *buf )
{
  switch( id )
  {
  case BSWAY_PLAYDATA_ID_playmode:
    return (u32)bsw_play->play_mode;
  case BSWAY_PLAYDATA_ID_round:
    return (u32)bsw_play->tower_round;
  case BSWAY_PLAYDATA_ID_rec_down:
    return (u32)bsw_play->wifi_rec_down;
  case BSWAY_PLAYDATA_ID_rec_turn:
    return bsw_play->wifi_rec_turn;
  case BSWAY_PLAYDATA_ID_rec_damage:
    return bsw_play->wifi_rec_damage;
  case BSWAY_PLAYDATA_ID_pokeno:
    GF_ASSERT( buf != NULL );
    MI_CpuCopy8( bsw_play->member_poke, buf, BSUBWAY_STOCK_MEMBER_MAX );
    return 0;
  case BSWAY_PLAYDATA_ID_pare_poke:
    GF_ASSERT( buf != NULL );
    MI_CpuCopy8( &bsw_play->pare_poke, buf, sizeof(BSUBWAY_PAREPOKE_PARAM) );
    return 0;
  case BSWAY_PLAYDATA_ID_pare_itemfix:
    return bsw_play->itemfix_f;
  case BSWAY_PLAYDATA_ID_trainer:
    GF_ASSERT( buf != NULL );
    MI_CpuCopy8(bsw_play->trainer_no,buf, 2*BSUBWAY_STOCK_TRAINER_MAX );
    return 0;
  case BSWAY_PLAYDATA_ID_partner:
    return bsw_play->partner;
  case BSWAY_PLAYDATA_ID_rnd_seed:
    return bsw_play->play_rnd_seed;
  default:
    GF_ASSERT( 0 );
  }
  
  return 0;
}

//--------------------------------------------------------------
/**
 * �v���C�f�[�^�@�ݒ�
 * @param bsw_play BSUBWAY_PLAYDATA
 * @param id �擾����BSWAY_PLAYDATA_ID
 * @param buf ���f����f�[�^�|�C���^
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_PLAYDATA_SetData(
    BSUBWAY_PLAYDATA *bsw_play, BSWAY_PLAYDATA_ID id, const void *buf )
{
  u32 *buf32 = (u32*)buf;
  u16 *buf16 = (u16*)buf;
  u8 *buf8 = (u8*)buf;
  
  GF_ASSERT( buf != NULL );
  
  switch(id){
  case BSWAY_PLAYDATA_ID_playmode:
    bsw_play->play_mode = buf8[0];
    break;
  case BSWAY_PLAYDATA_ID_round:
    bsw_play->tower_round = buf8[0];
    break;
  case BSWAY_PLAYDATA_ID_rec_down:
    bsw_play->wifi_rec_down = buf8[0];
    break;
  case BSWAY_PLAYDATA_ID_rec_turn:
    bsw_play->wifi_rec_turn = buf16[0];
    break;
  case BSWAY_PLAYDATA_ID_rec_damage:
    bsw_play->wifi_rec_damage = buf16[0];
    break;
  case BSWAY_PLAYDATA_ID_pokeno:
    MI_CpuCopy8( buf8, bsw_play->member_poke, 4 );
    break;
  case BSWAY_PLAYDATA_ID_pare_poke:
    MI_CpuCopy8( buf16,
        &bsw_play->pare_poke, sizeof(BSUBWAY_PAREPOKE_PARAM) );
    break;
  case BSWAY_PLAYDATA_ID_pare_itemfix:
    bsw_play->itemfix_f = buf8[0];
    break;
  case BSWAY_PLAYDATA_ID_trainer:
    MI_CpuCopy8( buf16, bsw_play->trainer_no, 2*BSUBWAY_STOCK_TRAINER_MAX );
    break;
  case BSWAY_PLAYDATA_ID_rnd_seed:
    bsw_play->play_rnd_seed = buf32[0];
    break;
  case BSWAY_PLAYDATA_ID_partner:
    bsw_play->partner = buf8[0];
    break;
  default:
    GF_ASSERT( 0 );
  }
  
  /* //wb
  #if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
  #endif //CRC_LOADCHECK
  */
}

//--------------------------------------------------------------
/**
 *  @brief �v���C�f�[�^�@Wifi���R�[�h�f�[�^Add  
 *  @param  down  �|���ꂽ�|�P�����ǉ���
 *  @param  turn  ���������^�[���ǉ���
 *  @param  damage  �󂯂��_���[�W�ǉ��l
 */
//--------------------------------------------------------------
void BSUBWAY_PLAYDATA_AddWifiRecord(
    BSUBWAY_PLAYDATA *bsw_play, u8 down,u16 turn,u16 damage )
{
  if(bsw_play->wifi_rec_down + down < 255){
    bsw_play->wifi_rec_down += down;
  }
  if(bsw_play->wifi_rec_turn + turn < 65535){
    bsw_play->wifi_rec_turn += turn;
  }
  if(bsw_play->wifi_rec_damage + damage < 65535){
    bsw_play->wifi_rec_damage += damage;
  }

#if  PL_T0855_080710_FIX
  /*
  OS_Printf( "********************\n" );
  OS_Printf( "dat->wifi_rec_turn = %d\n", dat->wifi_rec_turn );
  OS_Printf( "dat->wifi_rec_down = %d\n", dat->wifi_rec_down );
  OS_Printf( "dat->wifi_rec_damage = %d\n", dat->wifi_rec_damage );
  */
#endif

/*
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
*/
}

//======================================================================
//  �o�g���T�u�E�F�C�@�X�R�A�f�[�^
//======================================================================
//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�T�C�Y
 * @param none
 * @retval int ���[�N�T�C�Y
 */
//--------------------------------------------------------------
int BSUBWAY_SCOREDATA_GetWorkSize( void )
{
  return( sizeof(BSUBWAY_SCOREDATA) );
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^������
 * @param bsw_score BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_Init( BSUBWAY_SCOREDATA *bsw_score )
{
  MI_CpuClear8( bsw_score, sizeof(BSUBWAY_SCOREDATA) );
  bsw_score->wifi_rank = 1;
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�@�o�g���|�C���g����
 * @param bsw_score BSUBWAY_SCOREDATA
 * @param num �Z�b�g����l
 * @param mode BSWAY_SETMODE
 * @retval u16 �����̃o�g���|�C���g
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCOREDATA_SetBattlePoint(
    BSUBWAY_SCOREDATA *bsw_score, u16 num, BSWAY_SETMODE mode )
{
  switch( mode ){
  case BSWAY_SETMODE_set:
    if( num > 9999 ){
      bsw_score->btl_point = 9999;
    }else{
      bsw_score->btl_point = num;
    }
    break;
  case BSWAY_SETMODE_add:
    if( bsw_score->btl_point+num > 9999 ){
      bsw_score->btl_point = 9999;
    }else{
      bsw_score->btl_point += num;
    }
    break;
  case BSWAY_SETMODE_sub:
    if( bsw_score->btl_point < num ){
      bsw_score->btl_point = 0;
    }else{
      bsw_score->btl_point -= num;
    }
  case BSWAY_SETMODE_get:
    break;
  default:
    GF_ASSERT( 0 );
  }

  /* //wb
  #if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
  #endif //CRC_LOADCHECK
  */
  return bsw_score->btl_point;
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�@�A��������
 * @param
 * @retval
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCOREDATA_CalcRenshou(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_SETMODE mode )
{
  switch(mode){
  case BSWAY_SETMODE_reset:
    bsw_score->renshou[mode] = 0;
    break;
  case BSWAY_SETMODE_inc:
    if( bsw_score->renshou[mode] < 65534 ){
      bsw_score->renshou[mode]++;
    }
    break;
  case BSWAY_SETMODE_get:
    break;
  default:
    GF_ASSERT( 0 );
  }
  
  return( bsw_score->renshou[mode] );
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�@���񐔑���
 * @param bsw_score BSUBWAY_SCOREDATA
 * @param id �X�R�AID
 * @param value �Z�b�g����l
 * @param mode BSWAY_SETMODE
 * @retval u16 �����̎���
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCOREDATA_SetStage(
    BSUBWAY_SCOREDATA *bsw_score, u16 id, BSWAY_SETMODE mode )
{
  u16 id2;
  
  if( id == BSWAY_MODE_RETRY ){
    return 0;  //���g���C���[�h�ł̓J�E���g���Ȃ�
  }
  
  //�v���`�i�ǉ��@wifi�}���`
  if( id == BSWAY_MODE_WIFI_MULTI ){
    id2 = 5; //tower_stage[5]
  }else{
    id2 = id;
  }

  switch(mode){
  case BSWAY_SETMODE_reset:
    bsw_score->tower_stage[id2] = 0;
    break;
  case BSWAY_SETMODE_inc:
    if( bsw_score->tower_stage[id2] < 65534 ){
      bsw_score->tower_stage[id2] += 1;
    }
    break;
  }
  
  /* //wb null
  #if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
  #endif //CRC_LOADCHECK
  */
  return bsw_score->tower_stage[id2];
}
  
//--------------------------------------------------------------
/**
 *  �X�R�A�f�[�^ �t���O�G���A�Z�b�g
 *  @param bsw_score BSUBWAY_SCOREDATA*
 *  @param id BSWAY_SCOREDATA_FLAG
 *  @param mode BSWAY_SETMODE
 *  @retval BOOL �t���O�̏��
 */
//--------------------------------------------------------------
BOOL BSUBWAY_SCOREDATA_SetFlag( BSUBWAY_SCOREDATA *bsw_score,
    BSWAY_SCOREDATA_FLAG id, BSWAY_SETMODE mode )
{
  u16 i;
  u16 flag = 1;
  
  //�G���[�`�F�b�N
  if( id >= BSWAY_SCOREDATA_FLAG_MAX ){
    GF_ASSERT( 0 );
    return 0;
  }

  //�t���OID����
  for( i = 0; i < id; i++ ){
    flag <<= 1;
  }
  
  switch( mode ){
  case BSWAY_SETMODE_reset:
    flag = (flag^0xFFFF);
    bsw_score->flags &= flag;
    break;
  case BSWAY_SETMODE_set:
    bsw_score->flags |= flag;
    break;
  case BSWAY_SETMODE_get:
    return (BOOL)((bsw_score->flags>>id)&0x0001);
  default:
    GF_ASSERT( 0 );
  }
  
  /* //wb null
  #if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
  #endif //CRC_LOADCHECK
  */
  return 0;
}

//--------------------------------------------------------------
/**
 *  �X�R�A�f�[�^�@Wifi�����N����
 *  �L���R�}���h get/reset/inc/dec
 *  @param bsw_score BSUBWAY_SCOREDATA*
 *  @param mode BSWAY_SETMODE
 *  @retval u8 Wifi�����L���O
 */
//--------------------------------------------------------------
u8 BSUBWAY_SCOREDATA_SetWifiRank(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_SETMODE mode )
{
  switch( mode ){
  case BSWAY_SETMODE_reset:
    bsw_score->wifi_rank = 1;
    break;
  case BSWAY_SETMODE_inc:
    if( bsw_score->wifi_rank < 10 ){
      bsw_score->wifi_rank += 1;
    }
    break;
  case BSWAY_SETMODE_dec:
    if( bsw_score->wifi_rank > 1 ){
      bsw_score->wifi_rank -= 1;
    }
    break;
  }
/*  //wb null  
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
*/
  return bsw_score->wifi_rank;
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�A�O��̘A���L�^���擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCOREDATA_GetRenshouCount( const BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  return bsw_score->renshou[mode];
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�A�O��̘A���L�^���Z�b�g
 * @param
 * @retval
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCOREDATA_SetRenshouCount(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode, u16 count )
{
  bsw_score->renshou[mode] = count;
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�A�ő�A���L�^���擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCOREDATA_GetMaxRenshouCount(
    const BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  return bsw_score->renshou_max[mode];
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�A�ő�A���L�^���Z�b�g
 * @param
 * @retval
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCOREDATA_SetMaxRenshouCount(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode, u16 count )
{
  bsw_score->renshou_max[mode] = count;
}

//======================================================================
//  wb null
//======================================================================
//----
#if 0 //wb null
//----
//--------------------------------------------------------------
/**
 *  @brief  �T�u�E�F�C�@�|�P�����f�[�^�T�C�Y
 */
//--------------------------------------------------------------
int BSUBWAY_WifiPoke_GetWorkSize(void)
{
  return sizeof(BSUBWAY_POKEMON);
}

//--------------------------------------------------------------
/**
 *  @brief  �T�u�E�F�C�@Wifi�f�[�^�T�C�Y
 */
//--------------------------------------------------------------
int  BSUBWAY_WifiData_GetWorkSize(void)
{
  return sizeof(BSUBWAY_WIFI_DATA);
}

//--------------------------------------------------------------
/**
 *  @brief�@�T�u�E�F�C Wifi/�g���[�i�[���[�h�v���C���[�f�[�^�\���̃f�[�^�T�C�Y
 */
//--------------------------------------------------------------
int BSUBWAY_DpwBtPlayer_GetWorkSize(void)
{
  return sizeof(Dpw_Bt_Player);
}

//--------------------------------------------------------------
/**
 *  @brief  �T�u�E�F�C�@�v���C�f�[�^�N���A
 */
//--------------------------------------------------------------
void BSUBWAY_PlayData_Clear(BSUBWAY_PLAYWORK* dat)
{
  MI_CpuClear8(dat,sizeof(BSUBWAY_PLAYWORK));
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  �T�u�E�F�C  �X�R�A�f�[�^�N���A
 */
//--------------------------------------------------------------
void BSUBWAY_ScoreData_Clear(BSUBWAY_SCOREWORK* dat)
{
  MI_CpuClear8(dat,sizeof(BSUBWAY_SCOREWORK));
  dat->wifi_rank = 1;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  �T�u�E�F�C�@�v���C���[���b�Z�[�W�f�[�^�N���A
 */
//--------------------------------------------------------------
void BSUBWAY_PlayerMsg_Clear(BSUBWAY_PLAYER_MSG* dat)
{
#if 0 //wb
  PMSDAT_SetupDefaultBattleTowerMessage(&dat->msg[0], BSWAY_MSG_PLAYER_READY);
  PMSDAT_SetupDefaultBattleTowerMessage(&dat->msg[1], BSWAY_MSG_PLAYER_WIN);
  PMSDAT_SetupDefaultBattleTowerMessage(&dat->msg[2], BSWAY_MSG_PLAYER_LOSE);
  PMSDAT_SetupDefaultBattleTowerMessage(&dat->msg[3], BSWAY_MSG_LEADER);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
#endif
}

//--------------------------------------------------------------
/**
 *  @brief  �T�u�E�F�C�@Wifi�f�[�^�N���A
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_Clear(BSUBWAY_WIFI_DATA* dat)
{
  MI_CpuClear8(dat,sizeof(BSUBWAY_WIFI_DATA));
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//======================================================================
//�@�T�u�E�F�C�@�v���C�f�[�^�A�N�Z�X�n
//======================================================================

//======================================================================
//�@�T�u�E�F�C�@�X�R�A�f�[�^�A�N�Z�X�n
//======================================================================
//--------------------------------------------------------------
/**
 *  @brief  �X�R�A�f�[�^�@Wifi�A���s��J�E���g����
 *  �L���R�}���h get/reset/inc
 *  @return  �����̃J�E���g
 */
//--------------------------------------------------------------
u8 BSUBWAY_ScoreData_SetWifiLoseCount(BSUBWAY_SCOREWORK* dat,BSWAY_DATA_SETID mode)
{
  switch(mode){
  case BSWAY_DATA_reset:
    dat->wifi_lose = 0;
    dat->wifi_lose_f = 0;
    break;
  case BSWAY_DATA_inc:
    if(dat->wifi_lose_f){
      //�A���s�풆
      dat->wifi_lose += 1;
    }else{
      dat->wifi_lose = 1;
      dat->wifi_lose_f = 1;
    }
    break;
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
  return dat->wifi_lose;
}

//--------------------------------------------------------------
/**
 *  @brief  �g�p�|�P�����f�[�^�ۑ�
 */
//--------------------------------------------------------------
void BSUBWAY_ScoreData_SetUsePokeData(BSUBWAY_SCOREWORK* dat,
        BSWAY_SCORE_POKE_DATA mode,BSUBWAY_POKEMON* poke)
{
  if(mode == BSWAY_SCORE_POKE_SINGLE){
    MI_CpuCopy8(poke,dat->single_poke,sizeof(BSUBWAY_POKEMON)*3);
  }else{
    MI_CpuCopy8(poke,dat->wifi_poke,sizeof(BSUBWAY_POKEMON)*3);
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  �g�p�|�P�����f�[�^���o�b�t�@�ɃR�s�[���Ď擾
 */
//--------------------------------------------------------------
void BSUBWAY_ScoreData_GetUsePokeData(BSUBWAY_SCOREWORK* dat,
      BSWAY_SCORE_POKE_DATA mode,BSUBWAY_POKEMON* poke)
{
  if(mode == BSWAY_SCORE_POKE_SINGLE){
    MI_CpuCopy8(dat->single_poke,poke,sizeof(BSUBWAY_POKEMON)*3);
  }else{
    MI_CpuCopy8(dat->wifi_poke,poke,sizeof(BSUBWAY_POKEMON)*3);
  }
}

//--------------------------------------------------------------
/**
 *  @brief  �g�p�|�P�����f�[�^���o�b�t�@�ɃR�s�[���Ď擾(WiFi�\���̃f�[�^�^)
 */
//--------------------------------------------------------------
void BSUBWAY_ScoreData_GetUsePokeDataDpw(BSUBWAY_SCOREWORK* dat,
      BSWAY_SCORE_POKE_DATA mode,DPW_BT_POKEMON_DATA* poke)
{
  if(mode == BSWAY_SCORE_POKE_SINGLE){
    MI_CpuCopy8(dat->single_poke,poke,sizeof(BSUBWAY_POKEMON)*3);
  }else{
    MI_CpuCopy8(dat->wifi_poke,poke,sizeof(BSUBWAY_POKEMON)*3);
  }
}

//--------------------------------------------------------------
/**
 *  @brief  �X�R�A�f�[�^ Wifi���ё���
 */
//--------------------------------------------------------------
u16  BSUBWAY_ScoreData_SetWifiScore(BSUBWAY_SCOREWORK* dat,BSUBWAY_PLAYWORK *playdata)
{
  u16  sa,sb,sc,sd,st;
  u16  score = 0;

  //���E���h���͏���������+1�ɂȂ��Ă���̂Ń}�C�i�X�P���Čv�Z����
  sa = (playdata->tower_round-1)*1000;
  sb = playdata->wifi_rec_turn*10;
  sc = playdata->wifi_rec_down*20;
  if(sb+sc > 950){
    st = 0;
  }else{
    st = 950-(sb+sc);
  }
  if(playdata->wifi_rec_damage>(1000-30)){
    sd = 0;
  }else{
    sd = (1000-playdata->wifi_rec_damage)/30;
  }
  score = sa+st+sd;
  dat->wifi_score = score;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
  return score;
}

//--------------------------------------------------------------
/**
 *  @brief  �X�R�A�f�[�^�@Wifi����0�N���A
 */
//--------------------------------------------------------------
void BSUBWAY_ScoreData_ClearWifiScore(BSUBWAY_SCOREWORK* dat)
{
  dat->wifi_score = 0;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  �X�R�A�f�[�^�@Wifi���ю擾
 */
//--------------------------------------------------------------
u16  BSUBWAY_ScoreData_GetWifiScore(BSUBWAY_SCOREWORK* dat)
{
  return dat->wifi_score;
}

//--------------------------------------------------------------
/**
 *  @brief  �X�R�A�f�[�^���珟�������������擾
 */
//--------------------------------------------------------------
u8  BSUBWAY_ScoreData_GetWifiWinNum(BSUBWAY_SCOREWORK* dat)
{
  u8 ret = 0;
  ret = (dat->wifi_score)/1000;

  return ret;
}

//--------------------------------------------------------------
/**
 *  @brief  �X�R�A�f�[�^�@���񐔑���
 *  �L���R�}���h reset/inc/get
 */
//--------------------------------------------------------------

//--------------------------------------------------------------
/**
 *  @brief  ���݂̃X�e�[�W�l���Z�b�g����
 */
//--------------------------------------------------------------
u16 BSUBWAY_ScoreData_SetStageValue(BSUBWAY_SCOREWORK* dat,u16 id,u16 value)
{
  u16 id2;

  //if(id >= BSWAY_MODE_RETRY){
  if(id == BSWAY_MODE_RETRY){
    return 0;  //���g���C���[�h�ł̓J�E���g���Ȃ�
  }

  //�v���`�i�ǉ�
  if(id == BSWAY_MODE_WIFI_MULTI){
    id2 = 5;            //tower_stage[5]
  }else{
    id2 = id;
  }

  dat->tower_stage[id2] = value;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
  return dat->tower_stage[id2];
}

//--------------------------------------------------------------
/**
 *  @brief  �f�o�b�O����@���݂̃X�e�[�W�l���Z�b�g����
 */
//--------------------------------------------------------------
#ifdef PM_DEBUG
u16 BSUBWAY_ScoreData_DebugSetStageValue(BSUBWAY_SCOREWORK* dat,u16 id,u16 value)
{
  return BSUBWAY_ScoreData_SetStageValue( dat, id, value );
}
#endif  //PM_DEBUG
  
//--------------------------------------------------------------
/**
 *  @brief  �X�R�A�f�[�^ �t���O�G���A�Z�b�g
 */
//--------------------------------------------------------------
BOOL  BSUBWAY_ScoreData_SetFlags(
    BSUBWAY_SCOREWORK* dat,u16 id,BSWAY_DATA_SETID mode)
{
  u16  i;
  u16  flag = 1;

  //�G���[�`�F�b�N
  if( id >= BSWAY_SFLAG_MAX ){
    GF_ASSERT( (0) && "TowerScoreData_SetFlags ID���s���ł��I" );
    return 0;
  }

  //�t���OID����
  for(i = 0;i < id;i++){
    flag <<= 1;
  }
  switch(mode){
  case BSWAY_DATA_reset:
    flag = (flag^0xFFFF);
    dat->flags &= flag;
    break;
  case BSWAY_DATA_set:
    dat->flags |= flag;
    break;
  case BSWAY_DATA_get:
    return (BOOL)((dat->flags>>id)&0x0001);
    
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
  return 0;
}

//--------------------------------------------------------------
/**
 *  @brief  �T�u�E�F�C�p���t�ω������_���V�[�h�ۑ�
 */
//--------------------------------------------------------------
void BSUBWAY_ScoreData_SetDayRndSeed(BSUBWAY_SCOREWORK* dat,u32 rnd_seed)
{
  dat->day_rnd_seed = rnd_seed;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  �T�u�E�F�C�p���t�ω������_���V�[�h�擾
 */
//--------------------------------------------------------------
u32 BSUBWAY_ScoreData_GetDayRndSeed(BSUBWAY_SCOREWORK* dat)
{
  return dat->day_rnd_seed;
}

//======================================================================
//�@�T�u�E�F�C�@�v���C���[���b�Z�[�W�f�[�^�A�N�Z�X�n
//======================================================================
//--------------------------------------------------------------
/**
 *  @brief  �ȈՉ�b�f�[�^���Z�b�g
 */
//--------------------------------------------------------------
#if 0 //wb
void BSUBWAY_PlayerMsg_Set(SAVEDATA* sv,BSWAY_PLAYER_MSG_ID id,PMS_DATA* src)
{
  FRONTIER_SAVEWORK* data = SaveData_Get(sv,GMDATA_ID_FRONTIER);
  
  PMSDAT_Copy(&(data->tower.player_msg.msg[id]),src);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  �ȈՉ�b�f�[�^���擾
 */
//--------------------------------------------------------------
#if 0 //wb
PMS_DATA* BSUBWAY_PlayerMsg_Get(SAVEDATA* sv,BSWAY_PLAYER_MSG_ID id)
{
  FRONTIER_SAVEWORK* data = SaveData_Get(sv,GMDATA_ID_FRONTIER);
  
  return &(data->tower.player_msg.msg[id]);
}
#endif

//======================================================================
//�@�T�u�E�F�C�@Wifi�f�[�^�A�N�Z�X�n
//======================================================================
#if 0 //wb
//--------------------------------------------------------------
/**
 *  @brief  �w�肵�����[���f�[�^�̎擾�t���O�𗧂Ă�
 *
 *  @param  rank  1�I���W���Ȃ̂Œ���
 *  @param  roomno  1�I���W���Ȃ̂Œ���
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_SetRoomDataFlag(BSUBWAY_WIFI_DATA* dat,
    u8 rank,u8 roomno,RTCDate *day)
{
  u8  idx,ofs;
  u8  flag = 1;
  u16  roomid;
  
  if(roomno == 0 || roomno > 200){
    return;
  }
  if(rank == 0 || rank > 10){
    return;
  }
  //����1�I���W���Ȃ̂�-1���Čv�Z
  roomid = (rank-1)*200+(roomno-1);
  
  idx = roomid/8;
  ofs = roomid%8;
  flag <<= ofs;

  dat->flags[idx] |= flag;

  dat->day = RTCDate2GFDate(day);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  ���[���f�[�^�擾�t���O���N���A����
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_ClearRoomDataFlag(BSUBWAY_WIFI_DATA* dat)
{
  MI_CpuClear8(dat->flags,BSUBWAY_ROOM_DATA_FLAGS_LEN);
  MI_CpuClear8(&dat->day,sizeof(GF_DATE));
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  ���t���ς���Ă��邩�ǂ����`�F�b�N
 */
//--------------------------------------------------------------
static BOOL check_day(RTCDate* new,RTCDate* old)
{
  if(new->year > old->year){
    return TRUE;
  }
  if(new->month > old->month){
    return TRUE;
  }
  if(new->day > old->day){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 *  @brief  �w�肵�����[����DL�t���O�������Ă��邩�`�F�b�N
 *
 *  @param  rank  1�I���W���Ȃ̂Œ���
 *  @param  roomno  1�I���W���Ȃ̂Œ���
 */
//--------------------------------------------------------------
BOOL BSUBWAY_WifiData_CheckRoomDataFlag(BSUBWAY_WIFI_DATA* dat,u8 rank,u8 roomno,RTCDate* day)
{
  u8  idx,ofs;
  u8  flag = 1;
  u16  roomid;
  RTCDate old_day;
  
  if(roomno > 200 || rank > 10){
    return FALSE;
  }

  //�Ō��DL�������t����A�����ς���Ă��邩�ǂ����`�F�b�N
  GFDate2RTCDate(dat->day,&old_day);
  if(check_day(day,&old_day)){
    //�����ς���Ă���̂ŁA�t���O�Q���I�[���N���A
    BSUBWAY_WifiData_ClearRoomDataFlag(dat);
    return FALSE;
  }
  //����1�I���W���Ȃ̂�-1���Čv�Z
  roomid = (rank-1)*200+(roomno-1);
  
  idx = roomid/8;
  ofs = roomid%8;
  flag <<= ofs;

  if(dat->flags[idx] & flag){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 *  @brief  �v���C���[�f�[�^�����݂��Ă��邩�`�F�b�N
 */
//--------------------------------------------------------------
BOOL BSUBWAY_WifiData_IsPlayerDataEnable(BSUBWAY_WIFI_DATA* dat)
{
  return (BOOL)dat->player_data_f;
}

//--------------------------------------------------------------
/**
 *  @brief  ���[�_�[�f�[�^�����邩�ǂ����`�F�b�N
 */
//--------------------------------------------------------------
BOOL BSUBWAY_WifiData_IsLeaderDataEnable(BSUBWAY_WIFI_DATA* dat)
{
  return (BOOL)dat->leader_data_f;
}

//--------------------------------------------------------------
/**
 *  @brief  �v���C���[�f�[�^���Z�[�u
 *  
 *  @param  rank  1�I���W���Ȃ̂Œ��� 
 *  @param  roomno  1�I���W���Ȃ̂Œ���
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_SetPlayerData(BSUBWAY_WIFI_DATA* dat,DPW_BT_PLAYER* src,u8 rank,u8 roomno)
{
  MI_CpuCopy8(src,dat->player,
    sizeof(BSUBWAY_WIFI_PLAYER)*BSUBWAY_STOCK_WIFI_PLAYER_MAX);
  
  //roomno��rank��ۑ�
  dat->player_rank = rank;
  dat->player_room = roomno;
  dat->player_data_f = 1;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  �v���C���[�f�[�^���N���A
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_ClearPlayerData(BSUBWAY_WIFI_DATA* dat)
{
  MI_CpuClear8(dat->player,
    sizeof(BSUBWAY_WIFI_PLAYER)*BSUBWAY_STOCK_WIFI_PLAYER_MAX);
  dat->player_data_f = 0;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  �v���C���[�f�[�^��RoomID(�����N��roomNo)���擾
 *
 *  @param  roomid  BSUBWAY_ROOMID�^(b_tower.h�Ō��J)
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_GetPlayerDataRoomID(BSUBWAY_WIFI_DATA* dat,BSUBWAY_ROOMID *roomid)
{
  roomid->rank = dat->player_rank;
  roomid->no = dat->player_room;
}

//--------------------------------------------------------------
/**
 *  @brief  �v���C���[�f�[�^���T�u�E�F�C�퓬�p�ɉ�
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_GetBtlPlayerData(BSUBWAY_WIFI_DATA* dat,
    BSUBWAY_PARTNER_DATA* player,const u8 round)
{
  BSUBWAY_TRAINER  *tr;      //�g���[�i�[�f�[�^
  BSUBWAY_POKEMON  *poke;    //�����|�P�����f�[�^
  BSUBWAY_WIFI_PLAYER* src;
  MSGDATA_MANAGER* pMan;

  tr = &(player->bt_trd);
  poke = player->btpwd;
  src = &(dat->player[round]);

  //�g���[�i�[�p�����[�^�擾
  tr->player_id = BSUBWAY_TRAINER_ID;//src->id_no;  //�T�u�E�F�C�pID�͌Œ�l
  tr->tr_type = src->tr_type;
  //NG�l�[���t���O�`�F�b�N
  if(src->ngname_f){
    pMan = MSGMAN_Create(MSGMAN_TYPE_NORMAL,ARC_MSG,
        NARC_msg_btower_app_dat,HEAPID_WORLD);

    MSGMAN_GetStr(pMan,msg_def_player_name01+src->gender,tr->name);
    MSGMAN_Delete(pMan);
  }else{
    MI_CpuCopy8(src->name,tr->name,16);
  }
  MI_CpuCopy8(src->appear_word,tr->appear_word,8);
  MI_CpuCopy8(src->win_word,tr->win_word,8);
  MI_CpuCopy8(src->lose_word,tr->lose_word,8);

  //�|�P�����f�[�^�擾
  MI_CpuCopy8(src->poke,poke,sizeof(BSUBWAY_POKEMON)*3);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

/**
 *  @brief  ���[�_�[�f�[�^���Z�[�u
 *  
 *  @param  rank  1�I���W���Ȃ̂Œ���
 *  @param  roomno  1�I���W���Ȃ̂Œ���
 */
void BSUBWAY_WifiData_SetLeaderData(BSUBWAY_WIFI_DATA* dat,DPW_BT_LEADER* src,u8 rank,u8 roomno)
{
  MI_CpuCopy8(src,&dat->leader,
    sizeof(BSUBWAY_LEADER_DATA)*BSUBWAY_STOCK_WIFI_LEADER_MAX);
  
  //roomno��rank��ۑ�
  dat->leader_rank = rank;
  dat->leader_room = roomno;
  dat->leader_data_f = 1;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  ���[�_�[�f�[�^���N���A
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_ClearLeaderData(BSUBWAY_WIFI_DATA* dat)
{
  MI_CpuClear8(&dat->leader,
    sizeof(BSUBWAY_LEADER_DATA)*BSUBWAY_STOCK_WIFI_LEADER_MAX);
  dat->leader_data_f = 0;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  ���[�_�[�f�[�^��RoomID(�����N��roomNo)���擾
 *
 *  @param  roomid  BSUBWAY_ROOMID�^(b_tower.h�Ō��J)
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_GetLeaderDataRoomID(BSUBWAY_WIFI_DATA* dat,BSUBWAY_ROOMID *roomid)
{
  roomid->rank = dat->leader_rank;
  roomid->no = dat->leader_room;
}

//--------------------------------------------------------------
/**
 *  @brief  ���[�_�[�f�[�^��Alloc�����������ɃR�s�[���Ď擾����
 *
 *  ��������Alloc�����������̈��Ԃ��̂ŁA�Ăяo�����������I�ɉ�����邱�ƁI
 */
//--------------------------------------------------------------
BSUBWAY_LEADER_DATA* BSUBWAY_WifiData_GetLeaderDataAlloc(BSUBWAY_WIFI_DATA* dat,int heapID) 
{
  BSUBWAY_LEADER_DATA* bp;

  bp = sys_AllocMemory(heapID,sizeof(BSUBWAY_LEADER_DATA)*BSUBWAY_STOCK_WIFI_LEADER_MAX);
  MI_CpuCopy8(dat->leader,bp,sizeof(BSUBWAY_LEADER_DATA)*BSUBWAY_STOCK_WIFI_LEADER_MAX);

  return bp;
}
#endif

//======================================================================
//  �T�u�E�F�C�@�Z�[�u�f�[�^�u���b�N�֘A
//======================================================================
#if 0 //wb
//--------------------------------------------------------------
/**
 *  @brief  �T�u�E�F�C�@�v���C�f�[�^�ւ̃|�C���^���擾
 */
//--------------------------------------------------------------
BSUBWAY_PLAYWORK* SaveData_GetTowerPlayData(SAVEDATA* sv)
{
  FRONTIER_SAVEWORK* data = SaveData_Get(sv,GMDATA_ID_FRONTIER);
  
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_CheckCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
  return &data->play_tower;
}

//--------------------------------------------------------------
/**
 *  @brief  �T�u�E�F�C�@�X�R�A�f�[�^�ւ̃|�C���^���擾
 */
//--------------------------------------------------------------
BSUBWAY_SCOREWORK* SaveData_GetTowerScoreData(SAVEDATA* sv)
{
  FRONTIER_SAVEWORK* data = SaveData_Get(sv,GMDATA_ID_FRONTIER);
  
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_CheckCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
  return &data->tower.score;
}

//--------------------------------------------------------------
/**
 *  @brief  �T�u�E�F�C Wifi�f�[�^�ւ̃|�C���^���擾  
 */
//--------------------------------------------------------------
BSUBWAY_WIFI_DATA*  SaveData_GetTowerWifiData(SAVEDATA* sv)
{
  FRONTIER_SAVEWORK* data = SaveData_Get(sv,GMDATA_ID_FRONTIER);
  
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_CheckCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
  return &data->tower.wifi;
}
#endif

//----
#endif //wb null
//----
