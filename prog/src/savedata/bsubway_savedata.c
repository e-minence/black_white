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

#include <dpw_bt.h>

#include "savedata/bsubway_savedata.h"
#include "bsubway_savedata_local.h"
#include "../field/bsubway_scr_def.h"
#include "net/dwc_tool.h"

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
  u16 itemfix_f  :1;  ///<�Œ�A�C�e�����ǂ����̃t���O
  u16 saved_f    :1;  ///<�Z�[�u�ς݂��ǂ���
  u16 play_mode  :4;  ///<���݂ǂ��Ƀ`�������W����?
  u16 partner    :3;  ///<���ݒN�Ƒg��ł��邩?
  u16 use_battle_box:1; ///<�o�g���{�b�N�X���g�p���邩
  u16 sel_wifi_dl_play:1; ///<wifi�_�E�����[�h�v���C���s����
  u16 padding_bit:5; ///<�]��
  u8 round;  ///<�o�g���T�u�E�F�C ���E���h��
  u8 wifi_rec_down;    ///<���������܂łɓ|���ꂽ�|�P������
  u16 wifi_rec_turn;    ///<���������ɂ��������^�[����
  u16 wifi_rec_damage;  ///<���������܂łɎ󂯂��_���[�W��
  
  u8 member_poke[BSUBWAY_STOCK_MEMBER_MAX];    ///<�I�������|�P�����̈ʒu
  u16 trainer_no[BSUBWAY_STOCK_TRAINER_MAX];    ///<�ΐ�g���[�i�[No�ۑ�
  
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
  u16 btl_point;  ///<�o�g���|�C���g
  u8 wifi_lose;  ///<�A���s��J�E���g
  u8 wifi_rank;  ///<WiFi�����N
  
  u32  flags;
  
  //�A���L�^ 0 origin
  u16 renshou[BSWAY_PLAYMODE_MAX];
  u16 renshou_max[BSWAY_PLAYMODE_MAX];
  
  //�X�e�[�W���L�^ 1 origin 0 = error
  u16 stage[BSWAY_PLAYMODE_MAX];
  
  //WiFi�`�������W�f�[�^
  u16  wifi_score;  ///<WiFi����
  u8 debug_flag; ///<�f�o�b�O�p�t���O
  u8 padding;
  
  //WiFi�|�P�����f�[�^�X�g�b�N
  struct _BSUBWAY_POKEMON  wifi_poke[BSUBWAY_STOCK_WIFI_MEMBER_MAX];
  //�g���[�i�[���[�h�p�V���O���f�[�^�X�g�b�N
  struct _BSUBWAY_POKEMON  single_poke[BSUBWAY_STOCK_WIFI_MEMBER_MAX];
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
    return (u32)bsw_play->round;
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
  case BSWAY_PLAYDATA_ID_use_battle_box:
    return bsw_play->use_battle_box;
  case BSWAY_PLAYDATA_ID_sel_wifi_dl_play:
    return bsw_play->sel_wifi_dl_play;
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
    bsw_play->round = buf8[0];
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
  case BSWAY_PLAYDATA_ID_partner:
    bsw_play->partner = buf8[0];
    break;
  case BSWAY_PLAYDATA_ID_use_battle_box:
    bsw_play->use_battle_box = buf8[0];
    break;
  case BSWAY_PLAYDATA_ID_sel_wifi_dl_play:
    bsw_play->sel_wifi_dl_play = buf8[0];
    break;
  default:
    GF_ASSERT( 0 );
  }
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
}

//--------------------------------------------------------------
/**
 * �v���C�f�[�^�@���E���h�����Z�b�g
 * @param bsw_play BSUBWAY_PLAYDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_PLAYDATA_ResetRoundNo( BSUBWAY_PLAYDATA *bsw_play )
{
  u8 buf = 0;
  BSUBWAY_PLAYDATA_SetData( bsw_play, BSWAY_PLAYDATA_ID_round, &buf );
}

//--------------------------------------------------------------
/**
 * �v���C�f�[�^�@���E���h������
 * @param bsw_play BSUBWAY_PLAYDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_PLAYDATA_IncRoundNo( BSUBWAY_PLAYDATA *bsw_play )
{
  u8 buf = BSUBWAY_PLAYDATA_GetData( 
      bsw_play, BSWAY_PLAYDATA_ID_round, NULL );
  if( buf < 0xff ){
    buf++;
  }
  KAGAYA_Printf( "BSW ���E���h������ %d\n", buf );
  BSUBWAY_PLAYDATA_SetData( bsw_play, BSWAY_PLAYDATA_ID_round, &buf );
}

//--------------------------------------------------------------
/**
 * �v���C�f�[�^�@���E���h���Z�b�g
 * @param bsw_play BSUBWAY_PLAYDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_PLAYDATA_SetRoundNo( BSUBWAY_PLAYDATA *bsw_play, u8 round )
{
  BSUBWAY_PLAYDATA_SetData( bsw_play, BSWAY_PLAYDATA_ID_round, &round );
}

//--------------------------------------------------------------
/**
 * �v���C�f�[�^�@���E���h���擾
 * @param bsw_play BSUBWAY_PLAYDATA
 * @retval nothing
 */
//--------------------------------------------------------------
u16 BSUBWAY_PLAYDATA_GetRoundNo( const BSUBWAY_PLAYDATA *bsw_play )
{
  u16 buf = BSUBWAY_PLAYDATA_GetData( bsw_play, BSWAY_PLAYDATA_ID_round, NULL );
  return( buf );
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
 * �X�R�A�f�[�^�@�o�g���|�C���g�ǉ�
 * @param bsw_score BSUBWAY_SCOREDATA
 * @param num �Z�b�g����l
 * @retval u16 �ǉ���̃o�g���|�C���g
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_AddBattlePoint( BSUBWAY_SCOREDATA *bsw_score, u16 num )
{
  if( bsw_score->btl_point+num > 9999 ){
    bsw_score->btl_point = 9999;
  }else{
    bsw_score->btl_point += num;
  }
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�@�o�g���|�C���g����
 * @param bsw_score BSUBWAY_SCOREDATA
 * @param num �Z�b�g����l
 * @retval u16 �ǉ���̃o�g���|�C���g
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_SubBattlePoint( BSUBWAY_SCOREDATA *bsw_score, u16 num )
{
  if( (s16)((s16)bsw_score->btl_point - (s16)num) >= 0 ){
    bsw_score->btl_point -= num;
  }else{
    bsw_score->btl_point = 0;
  }
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�@�o�g���|�C���g�Z�b�g
 * @param bsw_score BSUBWAY_SCOREDATA
 * @param num �Z�b�g����l
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_SetBattlePoint( BSUBWAY_SCOREDATA *bsw_score, u16 num )
{
  if( num > 9999 ){
    bsw_score->btl_point = 9999;
  }else{
    bsw_score->btl_point = num;
  }
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�@�o�g���|�C���g�擾
 * @param bsw_score BSUBWAY_SCOREDATA
 * @retval u16 �o�g���|�C���g
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCOREDATA_GetBattlePoint( const BSUBWAY_SCOREDATA *bsw_score )
{
  return( bsw_score->btl_point );
}

//--------------------------------------------------------------
/**
 * �ʐM�}���`�A�X�[�p�[�ʐM�}���`���}���`�A�X�[�p�[�}���`�ɕϊ�
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BSWAY_PLAYMODE scoreData_ChgMultiMode( BSWAY_PLAYMODE mode )
{
  if( mode == BSWAY_PLAYMODE_COMM_MULTI ){
    mode = BSWAY_PLAYMODE_MULTI;
  }else if( mode == BSWAY_PLAYMODE_S_COMM_MULTI ){
    mode = BSWAY_PLAYMODE_S_MULTI;
  }
  return( mode );
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�@�A��������
 * @param bsw_score BSUBWAY_SCOREDATA
 * @param mode BSWAY_PLAYMODE
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_IncRenshou(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  
  if( bsw_score->renshou[mode] < 65534 ){
    bsw_score->renshou[mode]++;
  }
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�@�A�������Z�b�g
 * @param bsw_score BSUBWAY_SCOREDATA
 * @param mode BSWAY_PLAYMODE
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_ResetRenshou(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  bsw_score->renshou[mode] = 0;
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�@�A�����擾
 * @param bsw_score BSUBWAY_SCOREDATA
 * @param mode BSWAY_PLAYMODE
 * @retval u16 �A����
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCOREDATA_GetRenshou(
    const BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  return( bsw_score->renshou[mode] );
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�A�ő�A���L�^���擾
 * @param bsw_score BSUBWAY_SCOREDATA
 * @retval u16
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCOREDATA_GetRenshouMax(
    const BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  return bsw_score->renshou_max[mode];
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�A�ő�A���L�^���Z�b�g
 * @param bsw_score BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_SetRenshouMax(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode, u16 count )
{
  mode = scoreData_ChgMultiMode( mode );
  bsw_score->renshou_max[mode] = count;
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�A�ő�A���L�^�X�V
 * @param bsw_score BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_UpdateRenshouMax(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode, u16 count )
{
  mode = scoreData_ChgMultiMode( mode );
  if( bsw_score->renshou_max[mode] < count ){
    bsw_score->renshou_max[mode] = count;
  }
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�A�A�����Z�b�g
 * @param bsw_score BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_SetRenshou(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode, u16 count )
{
  mode = scoreData_ChgMultiMode( mode );
  bsw_score->renshou[mode] = count;
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�@�X�e�[�W�����G���[��
 * @param bsw_play BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_ErrorStageNo(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  bsw_score->stage[mode] = 0;
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�@�X�e�[�W��������
 * @param bsw_play BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_InitStageNo(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  bsw_score->stage[mode] = 1;
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�@�X�e�[�W�������邩�`�F�b�N
 * @param bsw_play BSUBWAY_SCOREDATA
 * @retval BOOL
 */
//--------------------------------------------------------------
BOOL BSUBWAY_SCOREDATA_CheckExistStageNo(
    const BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  
  if( bsw_score->stage[mode] == 0 ){
    return( FALSE );
  }
  return( TRUE );
}


//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�@�X�e�[�W������
 * @param bsw_play BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_IncStageNo(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  
  if( bsw_score->stage[mode] < (0xffff-1) ){ //-1=�G���[����p
    bsw_score->stage[mode]++;
  }
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�@�X�e�[�W���Z�b�g
 * @param bsw_play BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_SetStageNo_Org1(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode, u16 stage )
{
  mode = scoreData_ChgMultiMode( mode );
  
  if( stage == 0 ){
    GF_ASSERT( 0 );
    stage = 1;
  }
  bsw_score->stage[mode] = stage;
}
//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�@�X�e�[�W���擾 0origin
 * @param bsw_play BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCOREDATA_GetStageNo_Org0(
    const BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  
  {
    u16 stage = bsw_score->stage[mode];
    stage--;
  
    if( (s16)stage < 0 ){
      GF_ASSERT( 0 );
      stage = 0;
    }
  
    return( stage );
  }
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
  u32 i;
  u32 flag = 1;
  
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
    flag = (flag^0xFFFFFFFF);
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
  
  return 0;
}

//--------------------------------------------------------------
/**
 *  �X�R�A�f�[�^�@Wifi�����N����
 *  @param bsw_score BSUBWAY_SCOREDATA*
 *  @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_IncWifiRank( BSUBWAY_SCOREDATA *bsw_score )
{
  if( bsw_score->wifi_rank < 10 ){
    bsw_score->wifi_rank++;
  }
}

//--------------------------------------------------------------
/**
 *  �X�R�A�f�[�^�@Wifi�����N����
 *  @param bsw_score BSUBWAY_SCOREDATA*
 *  @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_DecWifiRank( BSUBWAY_SCOREDATA *bsw_score )
{
  if( bsw_score->wifi_rank > 1 ){
      bsw_score->wifi_rank--;
  }
}

//--------------------------------------------------------------
/**
 *  �X�R�A�f�[�^�@Wifi�����N�擾
 *  @param bsw_score BSUBWAY_SCOREDATA*
 *  @retval u8 wifi�����N
 */
//--------------------------------------------------------------
u8 BSUBWAY_SCOREDATA_GetWifiRank( const BSUBWAY_SCOREDATA *bsw_score )
{
  return( bsw_score->wifi_rank );
}

//--------------------------------------------------------------
/**
 *  �X�R�A�f�[�^�@Wifi�����N�Z�b�g
 *  @param bsw_score BSUBWAY_SCOREDATA*
 *  @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_SetWifiRank( BSUBWAY_SCOREDATA *bsw_score, u8 rank )
{
  if( (int)rank < 1 || (int)rank > 10 ){
    GF_ASSERT( 0 );
    rank = 1;
  }
  
  bsw_score->wifi_rank = rank;
}

//--------------------------------------------------------------
/**
 *  @brief  �X�R�A�f�[�^�@Wifi�A���s��J�E���g����
 *  @return  �����̃J�E���g
 */
//--------------------------------------------------------------
u8 BSUBWAY_SCOREDATA_SetWifiLoseCount(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_SETMODE mode )
{
  BSWAY_SCOREDATA_FLAG flag = BSWAY_SCOREDATA_FLAG_WIFI_LOSE_F;

  switch(mode){
  case BSWAY_SETMODE_reset:
    BSUBWAY_SCOREDATA_SetFlag( bsw_score, flag, BSWAY_SETMODE_reset );
    bsw_score->wifi_lose = 0;
    break;
  case BSWAY_SETMODE_inc:
    if( BSUBWAY_SCOREDATA_SetFlag(bsw_score,flag,BSWAY_SETMODE_get) ){
      if( bsw_score->wifi_lose < 0xff ){
        bsw_score->wifi_lose++; //�A���s�풆
      }
    }else{
      BSUBWAY_SCOREDATA_SetFlag( bsw_score, flag, BSWAY_SETMODE_set );
      bsw_score->wifi_lose = 1;
    }
    break;
  }
  
  //get
  return bsw_score->wifi_lose;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �X�R�A�f�[�^  �g�p�|�P������ݒ�
 *
 *  @param  bsw_score     �X�R�A���[�N
 *  @param  mode          �ݒ肷�郂�[�h
 *  @param  poke          �|�P�������(BSUBWAY_POKEMON * BSUBWAY_STOCK_WIFI_MEMBER_MAX�@���̃o�b�t�@���K�v)
 */
//-----------------------------------------------------------------------------
void BSUBWAY_SCOREDATA_SetUsePokeData( BSUBWAY_SCOREDATA *bsw_score, BSWAY_SCORE_POKE_DATA mode, const BSUBWAY_POKEMON* poke_tbl )
{
  if( mode == BSWAY_SCORE_POKE_SINGLE ){
    GFL_STD_MemCopy( poke_tbl, bsw_score->single_poke, sizeof(BSUBWAY_POKEMON)*BSUBWAY_STOCK_WIFI_MEMBER_MAX );
  }else{
    GFL_STD_MemCopy( poke_tbl, bsw_score->wifi_poke, sizeof(BSUBWAY_POKEMON)*BSUBWAY_STOCK_WIFI_MEMBER_MAX );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �X�R�A�f�[�^  �g�p�|�P�������擾
 *
 *  @param  bsw_score     �X�R�A���[�N
 *  @param  mode          �ݒ肷�郂�[�h
 *  @param  poke          �|�P�������(BSUBWAY_POKEMON * 3�@���̃o�b�t�@���K�v)
 */
//-----------------------------------------------------------------------------
void BSUBWAY_SCOREDATA_GetUsePokeData( const BSUBWAY_SCOREDATA *bsw_score, BSWAY_SCORE_POKE_DATA mode, BSUBWAY_POKEMON* poke_tbl )
{
  if( mode == BSWAY_SCORE_POKE_SINGLE ){
    GFL_STD_MemCopy( bsw_score->single_poke, poke_tbl, sizeof(BSUBWAY_POKEMON)*BSUBWAY_STOCK_WIFI_MEMBER_MAX );
  }else{
    GFL_STD_MemCopy( bsw_score->wifi_poke, poke_tbl, sizeof(BSUBWAY_POKEMON)*BSUBWAY_STOCK_WIFI_MEMBER_MAX );
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  �X�R�A�f�[�^ Wifi���ё���
 *
 *  @param  bsw_score   �X�R�A���[�N
 *  @param  bsw_play    �v���C���[�N
 */
//-----------------------------------------------------------------------------
void BSUBWAY_SCOREDATA_SetWifiScore( BSUBWAY_SCOREDATA *bsw_score, const BSUBWAY_PLAYDATA *bsw_play )
{
  u16  sa,sb,sc,sd,st;
  u16  score = 0;

  //���E���h���͏���������+1�ɂȂ��Ă���̂Ń}�C�i�X�P���Čv�Z����
  sa = (bsw_play->round-1)*1000;
  sb = bsw_play->wifi_rec_turn*10;
  sc = bsw_play->wifi_rec_down*20;
  
  if(sb+sc > 950){
    st = 0;
  }else{
    st = 950-(sb+sc);
  }
  
  if(bsw_play->wifi_rec_damage>(1000-30)){
    sd = 0;
  }else{
    sd = (1000-bsw_play->wifi_rec_damage)/30;
  }
  
  score = sa+st+sd;
  bsw_score->wifi_score = score;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �X�R�A�f�[�^�@Wifi����0�N���A
 *
 *  @param  bsw_score   ���[�N
 */
//-----------------------------------------------------------------------------
void BSUBWAY_SCOREDATA_ClearWifiScore( BSUBWAY_SCOREDATA *bsw_score )
{
  bsw_score->wifi_score = 0;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �X�R�A�f�[�^�@Wifi���ю擾
 *
 *  @param  bsw_score   ���[�N
 *
 *  @return Wifi����
 */
//-----------------------------------------------------------------------------
u16 BSUBWAY_SCOREDATA_GetWifiScore( const BSUBWAY_SCOREDATA *bsw_score )
{
  return bsw_score->wifi_score;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �X�R�A�f�[�^  Wifi���т���@���������������擾
 *
 *  @param  bsw_score   ���[�N
 *
 *  @return ������������
 */
//-----------------------------------------------------------------------------
u8 BSUBWAY_SCOREDATA_GetWifiNum( const BSUBWAY_SCOREDATA *bsw_score )
{
  u8 ret = 0;
  ret = (bsw_score->wifi_score)/1000;
  return ret;
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�@�f�o�b�O�p�t���O�擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
u8 BSUBWAY_SCOREDATA_DEBUG_GetFlag( const BSUBWAY_SCOREDATA *bsw_score )
{
#ifdef PM_DEBUG
  return( bsw_score->debug_flag );
#else
  return( 0 ); //ROM�ł̍ۂ͑S�ăt���O�I�t
#endif
}

//--------------------------------------------------------------
/**
 * �X�R�A�f�[�^�@�f�o�b�O�p�t���O�Z�b�g
 * @param
 * @retval
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_DEBUG_SetFlag( BSUBWAY_SCOREDATA *bsw_score, u8 flag )
{
#ifdef PM_DEBUG
  bsw_score->debug_flag = flag;
#else
  //ROM�ł̍ۂ͖���
#endif
}

//======================================================================
//  BSUBWAY_WIFIDATA
//======================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  BSUBWAY_WIFIDATA�̃T�C�Y���擾
 *  @return �T�C�Y
 */ 
//-----------------------------------------------------------------------------
int BSUBWAY_WIFIDATA_GetWorkSize( void )
{
  return sizeof(BSUBWAY_WIFI_DATA);
}

//----------------------------------------------------------------------------
/**
 *  @brief  WiFi�f�[�^  ������
 *
 *  @param  bsw_wifi  WiFi�f�[�^
 */
//-----------------------------------------------------------------------------
void BSUBWAY_WIFIDATA_Init( BSUBWAY_WIFI_DATA *bsw_wifi )
{
  GFL_STD_MemClear( bsw_wifi, sizeof(BSUBWAY_WIFI_DATA) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  WiFi�f�[�^  �w�胋�[���f�[�^�̎擾�t���O��ݒ肷��B
 *
 *  @param  bsw_wifi  WiFi�f�[�^  
 *  @param  rank      �����N�i1�I���W���j
 *  @param  room      ���[��(1�I���W��)
 *  @param  day       ��M����
 */
//-----------------------------------------------------------------------------
void BSUBWAY_WIFIDATA_SetRoomDataFlag( BSUBWAY_WIFI_DATA *bsw_wifi, u8 rank, u8 room, const RTCDate *day )
{
  u8  idx,ofs;
  u8  flag = 1;
  u16  roomid;
  
  if(room == 0 || room > 200){
    return;
  }
  if(rank == 0 || rank > 10){
    return;
  }
  //����1�I���W���Ȃ̂�-1���Čv�Z
  roomid = (rank-1)*200+(room-1);
  
  idx = roomid/8;
  ofs = roomid%8;
  flag <<= ofs;

  bsw_wifi->flags[idx] |= flag;

  bsw_wifi->day = GFDATE_RTCDate2GFDate(day);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[���f�[�^�擾�t���O���N���A����
 *
 *  @param  bsw_wifi  
 */
//-----------------------------------------------------------------------------
void BSUBWAY_WIFIDATA_ClearRoomDataFlag( BSUBWAY_WIFI_DATA *bsw_wifi )
{
  GFL_STD_MemClear(bsw_wifi->flags,BSUBWAY_ROOM_DATA_FLAGS_LEN);
  GFL_STD_MemClear(&bsw_wifi->day,sizeof(GFDATE));
}

//--------------------------------------------------------------
/**
 *  @brief  ���t���ς���Ă��邩�ǂ����`�F�b�N
 */
//--------------------------------------------------------------
static BOOL check_day(const RTCDate* new,const RTCDate* old)
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
//----------------------------------------------------------------------------
/**
 *  @brief  �w�胋�[���̏�񂪃_�E�����[�h����Ă��邩�`�F�b�N
 *
 *  @param  bsw_wifi    ���[�N
 *  @param  rank        �����N
 *  @param  room        ���[���i���o�[
 *  @param  day         ���t
 *
 *  *���t���ς���Ă���ƃt���O���I�[���N���A����B
 *
 *  @retval TRUE    ��M����Ă���
 *  @retval FALSE   ��M����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL BSUBWAY_WIFIDATA_CheckRoomDataFlag( BSUBWAY_WIFI_DATA *bsw_wifi, u8 rank, u8 room, const RTCDate *day )
{
  u8  idx,ofs;
  u8  flag = 1;
  u16  roomid;
  RTCDate old_day;
  
  if(room > 200 || rank > 10){
    return FALSE;
  }

  //�Ō��DL�������t����A�����ς���Ă��邩�ǂ����`�F�b�N
  GFDATE_GFDate2RTCDate(bsw_wifi->day,&old_day);
  if(check_day(day,&old_day)){
    //�����ς���Ă���̂ŁA�t���O�Q���I�[���N���A
    BSUBWAY_WIFIDATA_ClearRoomDataFlag(bsw_wifi);
    return FALSE;
  }
  //����1�I���W���Ȃ̂�-1���Čv�Z
  roomid = (rank-1)*200+(room-1);
  
  idx = roomid/8;
  ofs = roomid%8;
  flag <<= ofs;

  if(bsw_wifi->flags[idx] & flag){
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �v���C���[�f�[�^�����݂��Ă��邩�`�F�b�N
 *
 *  @param  bsw_wifi  ���[�N
 *
 *  @retval TRUE    ���݂��Ă���
 *  @retval FALSE   ���݂��Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL BSUBWAY_WIFIDATA_CheckPlayerDataEnable( const BSUBWAY_WIFI_DATA *bsw_wifi )
{
  return bsw_wifi->player_data_f;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�_�[�f�[�^�����݂��Ă��邩�`�F�b�N
 *
 *  @param  bsw_wifi  ���[�N
 *
 *  @retval TRUE    ���݂��Ă���
 *  @retval FALSE   ���݂��Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL BSUBWAY_WIFIDATA_CheckLeaderDataEnable( const BSUBWAY_WIFI_DATA *bsw_wifi )
{
  return bsw_wifi->leader_data_f;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �v���C���[����ۑ�
 *
 *  @param  bsw_wifi    ���[�N
 *  @param  dat         �v���C���[���e�[�u��
 *  @param  rank        �����N
 *  @param  room        ����No
 */
//-----------------------------------------------------------------------------
void BSUBWAY_WIFIDATA_SetPlayerData( BSUBWAY_WIFI_DATA *bsw_wifi, const BSUBWAY_WIFI_PLAYER* dat, u8 rank, u8 room )
{
  GFL_STD_MemCopy( dat, bsw_wifi->player,
    sizeof(BSUBWAY_WIFI_PLAYER)*BSUBWAY_STOCK_WIFI_PLAYER_MAX );

#ifdef DEBUG_ONLY_FOR_tomoya_takahashi
  {
    int i;

    for( i=0; i<BSUBWAY_STOCK_WIFI_PLAYER_MAX; i++ ){
      TOMOYA_Printf( "idx %d trtype %d\n", bsw_wifi->player[i].tr_type );
    }
  }
#endif
  
  //roomno��rank��ۑ�
  bsw_wifi->player_rank = rank;
  bsw_wifi->player_room = room;
  bsw_wifi->player_data_f = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �v���C���[�����N���A
 *
 *  @param  bsw_wifi    ���[�N
 */
//-----------------------------------------------------------------------------
void BSUBWAY_WIFIDATA_ClearPlayerData( BSUBWAY_WIFI_DATA *bsw_wifi )
{
  GFL_STD_MemClear( bsw_wifi->player,
    sizeof(BSUBWAY_WIFI_PLAYER)*BSUBWAY_STOCK_WIFI_PLAYER_MAX );
  bsw_wifi->player_data_f = FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �v���C���[���@�����N�擾
 */
//-----------------------------------------------------------------------------
u8 BSUBWAY_WIFIDATA_GetPlayerRank( const BSUBWAY_WIFI_DATA *bsw_wifi )
{
  GF_ASSERT( bsw_wifi->player_data_f );
  return bsw_wifi->player_rank;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �v���C���[���@����No�擾
 */
//-----------------------------------------------------------------------------
u8 BSUBWAY_WIFIDATA_GetPlayerRoomNo( const BSUBWAY_WIFI_DATA *bsw_wifi )
{
  GF_ASSERT( bsw_wifi->player_data_f );
  return bsw_wifi->player_room;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �v���C���[���@�o�g���p�p�����[�^�擾
 */
//-----------------------------------------------------------------------------
void BSUBWAY_WIFIDATA_GetBtlPlayerData( const BSUBWAY_WIFI_DATA *bsw_wifi, BSUBWAY_PARTNER_DATA *player, u8 round, HEAPID heapID )
{
  BSUBWAY_TRAINER  *tr;      //�g���[�i�[�f�[�^
  BSUBWAY_POKEMON  *poke;    //�����|�P�����f�[�^
  const BSUBWAY_WIFI_PLAYER* src;
  GFL_MSGDATA* msgdata;

  tr = &(player->bt_trd);
  poke = player->btpwd;
  src = &(bsw_wifi->player[round]);

  //�g���[�i�[�p�����[�^�擾
  tr->player_id = 0;//�T�u�E�F�C�pID�͌Œ�l @TODO
  tr->tr_type = src->tr_type;
  //NG�l�[���t���O�`�F�b�N
  if(src->ngname_f){
    // NG�l�[���̕ύX����
    DWC_TOOL_SetBadNickName( tr->name, 8, heapID );
  }else{
    GFL_STD_MemCopy(src->name,tr->name,16);
  }
  GFL_STD_MemCopy(src->appear_word,tr->appear_word,8);
  GFL_STD_MemCopy(src->win_word,tr->win_word,8);
  GFL_STD_MemCopy(src->lose_word,tr->lose_word,8);

  //�|�P�����f�[�^�擾
  MI_CpuCopy8(src->poke,poke,sizeof(BSUBWAY_POKEMON)*3);
}


//----------------------------------------------------------------------------
/**
 *  @brief  ���[�_�[���̐ݒ�
 *
 *  @param  bsw_wifi  ���[�N
 *  @param  dat       ���[�_�[���e�[�u��
 *  @param  rank      �����N
 *  @param  room      �����i���o�[
 */
//-----------------------------------------------------------------------------
void BSUBWAY_WIFIDATA_SetLeaderData( BSUBWAY_WIFI_DATA *bsw_wifi, const BSUBWAY_LEADER_DATA* dat, u8 rank, u8 room )
{
  GFL_STD_MemCopy( dat, &bsw_wifi->leader,
    sizeof(BSUBWAY_LEADER_DATA)*BSUBWAY_STOCK_WIFI_LEADER_MAX );
  
  //roomno��rank��ۑ�
  bsw_wifi->leader_rank = rank;
  bsw_wifi->leader_room = room;
  bsw_wifi->leader_data_f = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�_�[���̃N���A
 *
 *  @param  bsw_wifi  ���[�N
 */
//-----------------------------------------------------------------------------
void BSUBWAY_WIFIDATA_ClearLeaderData( BSUBWAY_WIFI_DATA *bsw_wifi )
{
  MI_CpuClear8(&bsw_wifi->leader,
    sizeof(BSUBWAY_LEADER_DATA)*BSUBWAY_STOCK_WIFI_LEADER_MAX);
  bsw_wifi->leader_data_f = FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�_�[���̃����N�擾
 *
 *  @param  bsw_wifi  ���[�N
 *
 *  @return �����N
 */
//-----------------------------------------------------------------------------
u8 BSUBWAY_WIFIDATA_GetLeaderRank( const BSUBWAY_WIFI_DATA *bsw_wifi )
{
  GF_ASSERT( bsw_wifi->leader_data_f );
  return bsw_wifi->leader_rank;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�_�[���̕����i���o�[�擾
 *
 *  @param  bsw_wifi  ���[�N
 *  
 *  @return �����i���o�[
 */
//-----------------------------------------------------------------------------
u8 BSUBWAY_WIFIDATA_GetLeaderRoomNo( const BSUBWAY_WIFI_DATA *bsw_wifi )
{
  GF_ASSERT( bsw_wifi->leader_data_f );
  return bsw_wifi->leader_room;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�_�[���̎擾
 *
 *  @param  bsw_wifi  ���[�N
 *  @param  heapID    �q�[�vID
 *
 *  @return �������m�ۂ��ď����R�s�[�������[�N
 */
//-----------------------------------------------------------------------------
BSUBWAY_LEADER_DATA* BSUBWAY_WIFIDATA_GetLeaderDataAlloc( const BSUBWAY_WIFI_DATA *bsw_wifi, HEAPID heapID )
{
  BSUBWAY_LEADER_DATA* p_dat;

  p_dat = GFL_HEAP_AllocClearMemory( heapID, sizeof(BSUBWAY_LEADER_DATA)*BSUBWAY_STOCK_WIFI_LEADER_MAX );

  GFL_STD_MemCopy(bsw_wifi->leader, p_dat, sizeof(BSUBWAY_LEADER_DATA)*BSUBWAY_STOCK_WIFI_LEADER_MAX);
  
  return p_dat;
}

//----------------------------------------------------------------------------------
/**
 * @brief ���g���[�i�[�f�[�^�̊i�[����Ă���l����Ԃ�
 *
 * @param   bsw_leader    
 *
 * @retval  int   �g���[�i�[�f�[�^�̓����Ă���l���i�ő�R�O�l�j
 */
//----------------------------------------------------------------------------------
int BSUBWAY_LEADERDATA_GetDataNum( const BSUBWAY_LEADER_DATA *bsw_leader )
{
  int i,result=0;
  for(i=0;i<BSUBWAY_STOCK_WIFI_LEADER_MAX;i++)
  {
    // ���O�̂P�����ڂ��O���ᖳ�������瑶�݂��Ă��鎖�ɂ���
    if(bsw_leader[i].name[0]!=0)
    {
      result++;
    }
  }

  return result;
}


#ifdef PM_DEBUG
// �_�~�[����ݒ�
void DEBUG_BSUBWAY_WIFIDATA_SetPlayerData( BSUBWAY_WIFI_DATA *bsw_wifi, u8 rank, u8 room )
{
  //roomno��rank��ۑ�
  bsw_wifi->player_rank = rank;
  bsw_wifi->player_room = room;
  bsw_wifi->player_data_f = TRUE;
}

void DEBUG_BSUBWAY_WIFIDATA_SetLeaderData( BSUBWAY_WIFI_DATA *bsw_wifi, u8 rank, u8 room )
{
  //roomno��rank��ۑ�
  bsw_wifi->leader_rank = rank;
  bsw_wifi->leader_room = room;
  bsw_wifi->leader_data_f = TRUE;
}
#endif



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
  sa = (playdata->round-1)*1000;
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
u16 BSUBWAY_ScoreData_DebugSetStageValue(
    BSUBWAY_SCOREWORK* dat,u16 id,u16 value)
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
  MI_CpuClear8(&dat->day,sizeof(GFDATE));
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
