//======================================================================
/**
 * @file  field_comm_data.c
 * @brief �ʐM�ŋ��L����f�[�^(�e�L�����̍��W�Ƃ�)
 * @author  ariizumi
 * @data  08/11/13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "gamesystem/playerwork.h"
#include "test/ariizumi/ari_debug.h"
#include "app/trainer_card.h"
#include "field_comm_main.h"
#include "field_comm_data.h"
#include "system/main.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  enum
//======================================================================

//======================================================================
//  typedef struct
//======================================================================
//�L�����l�̃f�[�^
typedef struct
{
  BOOL  isExist_; //�L���������݂���
  BOOL  isValid_; //�f�[�^���L��
  F_COMM_CHARA_STATE  state_;
  F_COMM_TALK_STATE talkState_;
  PLAYER_WORK plWork_;
}FIELD_COMM_CHARA_DATA;

//��b��̍s���p�̃f�[�^�Ǘ�
typedef struct
{
  F_COMM_USERDATA_TYPE type_;
  void* selfData_;    //���g�̃f�[�^(���M�o�b�t�@
  void* partnerData_; //����̃f�[�^(��M�o�b�t�@
}FIELD_COMM_USERDATA;

struct _FIELD_COMM_DATA
{
  HEAPID heapID_;
  u8  backupCommState_; //FieldCommFunc�̃X�e�[�g�̃o�b�N�A�b�v
  FIELD_COMM_CHARA_DATA selfData_;  //���g�̃f�[�^�B
  FIELD_COMM_CHARA_DATA charaData_[FIELD_COMM_MEMBER_MAX];

  FIELD_COMM_USERDATA userData_;
};

//�����ēƗ����Ċm�ۂ�����
//FIELD_COMM_DATA *commData = NULL;

//======================================================================
//  proto
//======================================================================
FIELD_COMM_DATA * FIELD_COMM_DATA_InitSystem( HEAPID heapID );
void  FIELD_COMM_DATA_TermSystem( FIELD_COMM_DATA *comm_data );

void  FIELD_COMM_DATA_SetFieldCommMode( FIELD_COMM_DATA *comm_data, const u8 state );
const u8 FIELD_COMM_DATA_GetFieldCommMode(FIELD_COMM_DATA *comm_data);

void  FIELD_COMM_DATA_SetSelfData_PlayerWork( const PLAYER_WORK *plWork );
void  FIELD_COMM_DATA_SetSelfDataPos( const ZONEID zoneID , const VecFx32 *pos , const u16 *dir );
PLAYER_WORK* FIELD_COMM_DATA_GetSelfData_PlayerWork(FIELD_COMM_DATA *commData);

static void FIELD_COMM_DATA_InitOneCharaData( FIELD_COMM_CHARA_DATA *charaData );
static FIELD_COMM_CHARA_DATA* FIELD_COMM_DATA_GetCharaDataWork( FIELD_COMM_DATA *commData, const u8 idx );

//���[�U�[�f�[�^�֘A
const u32 FIELD_COMM_DATA_GetUserDataSize( const F_COMM_USERDATA_TYPE type );
const F_COMM_USERDATA_TYPE  FIELD_COMM_DATA_GetUserDataType_From_Action( const F_COMM_ACTION_LIST action );

//--------------------------------------------------------------
//  �ʐM�p�f�[�^�Ǘ�������
//--------------------------------------------------------------
FIELD_COMM_DATA * FIELD_COMM_DATA_InitSystem( HEAPID heapID )
{
  FIELD_COMM_DATA *comm_data;
  int i;

  //�ŏ��̓q�[�v�̊m�ۂƑS������
  comm_data = GFL_HEAP_AllocClearMemory( heapID , sizeof(FIELD_COMM_DATA) );
  comm_data->heapID_ = heapID;
  FIELD_COMM_DATA_InitOneCharaData( &comm_data->selfData_ );
  for( i=0;i<FIELD_COMM_MEMBER_MAX;i++ )
  {
    FIELD_COMM_DATA_InitOneCharaData( &comm_data->charaData_[i] );
  }
  comm_data->userData_.type_ = FCUT_NO_INIT;
  comm_data->userData_.selfData_ = NULL;
  comm_data->userData_.partnerData_ = NULL;

  return comm_data;
}

//--------------------------------------------------------------
//  �f�[�^�����łɂ���Ƃ���exist�����؂��Ă���(COMM_ACTOR�Đ����̂���
//--------------------------------------------------------------
void FIELD_COMM_DATA_DataReset(FIELD_COMM_DATA *comm_data)
{
  int i;

  if(comm_data == NULL){
    return;
  }

  //�f�[�^�����łɂ���Ƃ���exist�����؂��Ă���(COMM_ACTOR�Đ����̂���
  for( i=0;i<FIELD_COMM_MEMBER_MAX;i++ )
  {
    comm_data->charaData_[i].isExist_ = FALSE;
  }
}

//--------------------------------------------------------------
//  �ʐM�p�f�[�^�Ǘ�������
//--------------------------------------------------------------
void  FIELD_COMM_DATA_TermSystem( FIELD_COMM_DATA *comm_data )
{
  GF_ASSERT( comm_data != NULL );
  GFL_HEAP_FreeMemory( comm_data );
}

//--------------------------------------------------------------
//  FieldCommFunc�̃X�e�[�g�̃o�b�N�A�b�v
//--------------------------------------------------------------
void  FIELD_COMM_DATA_SetFieldCommMode( FIELD_COMM_DATA *comm_data, const u8 state )
{
  GF_ASSERT( comm_data != NULL );
  comm_data->backupCommState_ = state;
}
const u8 FIELD_COMM_DATA_GetFieldCommMode(FIELD_COMM_DATA *comm_data)
{
  GF_ASSERT( comm_data != NULL );
  return comm_data->backupCommState_;
}

//--------------------------------------------------------------
//  �����̃f�[�^�Z�b�g
//--------------------------------------------------------------
void  FIELD_COMM_DATA_SetSelfData_Pos( FIELD_COMM_DATA *comm_data, const ZONEID *zoneID , const VecFx32 *pos , const u16 *dir )
{
  GF_ASSERT( comm_data != NULL );
  PLAYERWORK_setZoneID( &comm_data->selfData_.plWork_ , *zoneID );
  PLAYERWORK_setPosition( &comm_data->selfData_.plWork_ , pos );
  PLAYERWORK_setDirection( &comm_data->selfData_.plWork_ , *dir );
  comm_data->selfData_.isExist_ = TRUE;
  comm_data->selfData_.isValid_ = TRUE;
}

//--------------------------------------------------------------
//  �����̃f�[�^�擾
//--------------------------------------------------------------
PLAYER_WORK*  FIELD_COMM_DATA_GetSelfData_PlayerWork( FIELD_COMM_DATA *commData )
{
  return FIELD_COMM_DATA_GetCharaData_PlayerWork( commData, FCD_SELF_INDEX );
//  GF_ASSERT( commData != NULL );
//  return &commData->selfData_.plWork_;
}

//--------------------------------------------------------------
//  �w��ԍ��̃t���O�擾�E�ݒ�
//--------------------------------------------------------------
void  FIELD_COMM_DATA_SetCharaData_IsExist( FIELD_COMM_DATA *commData, const u8 idx , const BOOL flg)
{
  FIELD_COMM_CHARA_DATA *pData = FIELD_COMM_DATA_GetCharaDataWork(commData, idx);
  pData->isExist_ = flg;
}
void  FIELD_COMM_DATA_SetCharaData_IsValid( FIELD_COMM_DATA *commData, const u8 idx , const BOOL flg)
{
  FIELD_COMM_CHARA_DATA *pData = FIELD_COMM_DATA_GetCharaDataWork(commData, idx);
  pData->isValid_ = flg;
}
const BOOL  FIELD_COMM_DATA_GetCharaData_IsExist( FIELD_COMM_DATA *commData, const u8 idx )
{
  FIELD_COMM_CHARA_DATA *pData = FIELD_COMM_DATA_GetCharaDataWork(commData, idx);
  return pData->isExist_;
}
const BOOL  FIELD_COMM_DATA_GetCharaData_IsValid( FIELD_COMM_DATA *commData, const u8 idx )
{
  FIELD_COMM_CHARA_DATA *pData = FIELD_COMM_DATA_GetCharaDataWork(commData, idx);
  return pData->isValid_;
}

//--------------------------------------------------------------
//  ��b�n�F��b�X�e�[�g�̎擾�E�ݒ�
//--------------------------------------------------------------
void  FIELD_COMM_DATA_SetTalkState( FIELD_COMM_DATA *commData, const u8 idx , const F_COMM_TALK_STATE state )
{
  FIELD_COMM_CHARA_DATA *pData = FIELD_COMM_DATA_GetCharaDataWork(commData, idx);
  pData->talkState_ = state;
}
const F_COMM_TALK_STATE FIELD_COMM_DATA_GetTalkState( FIELD_COMM_DATA *commData, const u8 idx )
{
  FIELD_COMM_CHARA_DATA *pData = FIELD_COMM_DATA_GetCharaDataWork(commData, idx);
  return pData->talkState_;
}

//--------------------------------------------------------------
//  �L�����̈ړ���(��~���͌����W)�𒲂ׂ�
//  @return �ړ����ł����TRUE
//--------------------------------------------------------------
const BOOL  FIELD_COMM_DATA_GetGridPos_AfterMove( FIELD_COMM_DATA *commData, const u8 idx , int *posX , int *posZ )
{
  FIELD_COMM_CHARA_DATA *pData = FIELD_COMM_DATA_GetCharaDataWork(commData, idx);
  if( pData->isExist_ == FALSE )
  {
    //���Ȃ���
    *posX = -1;
    *posZ = -1;
    return FALSE;
  }
  {
    PLAYER_WORK *plWork = &pData->plWork_;
    //FIXME:�������O���b�h�T�C�Y�擾����H
    const u8 gridSize = 16;
    const int gridX = F32_CONST( plWork->position.x )+8;
    const int gridZ = F32_CONST( plWork->position.z )+8;
    u8  modSize;  //���܂�̗�
    *posX = gridX/gridSize;
    *posZ = gridZ/gridSize;
    //�ǂꂾ�����S�ʒu���炸��Ă邩�H XZ��������Ă��邱�Ƃ͖����̂ŕЕ��������
    switch( plWork->direction )
    {
    case COMMDIR_UP:
    case COMMDIR_DOWN:
      modSize = gridZ - (*posZ*gridSize);
      break;
    case COMMDIR_LEFT:
    case COMMDIR_RIGHT:
      modSize = gridX - (*posX*gridSize);
      break;
    }
    //�ړ����ĂȂ��̂�OK
    if( modSize == 0 )
      return FALSE;
    switch( plWork->direction )
    {
    case COMMDIR_UP:
      //�����Ȃ�
      break;
    case COMMDIR_DOWN:
      *posZ += 1;
      break;
    case COMMDIR_LEFT:
      //�����Ȃ�
      break;
    case COMMDIR_RIGHT:
      *posX += 1;
      break;
    }
    return TRUE;
  }
}

//--------------------------------------------------------------
//  �w��ԍ��̃X�e�[�^�X�擾
//--------------------------------------------------------------
const F_COMM_CHARA_STATE FIELD_COMM_DATA_GetCharaData_State( FIELD_COMM_DATA *commData, const u8 idx )
{
  FIELD_COMM_CHARA_DATA *pData = FIELD_COMM_DATA_GetCharaDataWork(commData, idx);
  return pData->state_;
}
void FIELD_COMM_DATA_SetCharaData_State( FIELD_COMM_DATA *commData, const u8 idx , const F_COMM_CHARA_STATE state )
{
  FIELD_COMM_CHARA_DATA *pData = FIELD_COMM_DATA_GetCharaDataWork(commData, idx);
  pData->state_ = state;
}

//--------------------------------------------------------------
//  �w��ԍ��̃f�[�^�擾
//--------------------------------------------------------------
PLAYER_WORK*  FIELD_COMM_DATA_GetCharaData_PlayerWork( FIELD_COMM_DATA *commData, const u8 idx )
{
  FIELD_COMM_CHARA_DATA *pData = FIELD_COMM_DATA_GetCharaDataWork(commData, idx);
  return &pData->plWork_;
}

//--------------------------------------------------------------
//  �ʐM�p�f�[�^�Ǘ�������
//--------------------------------------------------------------
static void FIELD_COMM_DATA_InitOneCharaData( FIELD_COMM_CHARA_DATA *charaData )
{
  charaData->isExist_ = FALSE;
  charaData->isValid_ = FALSE;
  charaData->state_ = FCCS_NONE;
  charaData->talkState_ = FCTS_NONE;
  PLAYERWORK_init( &charaData->plWork_ );
}

//--------------------------------------------------------------
//  �f�[�^�擾�p�@���L�����E���L�����Ǘ�
//--------------------------------------------------------------
static FIELD_COMM_CHARA_DATA* FIELD_COMM_DATA_GetCharaDataWork( FIELD_COMM_DATA *commData, const u8 idx )
{
  GF_ASSERT( commData != NULL );
  if( idx == FCD_SELF_INDEX )
    return &commData->selfData_;
  GF_ASSERT( idx < FIELD_COMM_MEMBER_MAX );
  return &commData->charaData_[idx];
}

//--------------------------------------------------------------
//���[�U�[�f�[�^�֘A
//--------------------------------------------------------------
void  FIELD_COMM_DATA_CreateUserData( FIELD_COMM_DATA *commData, const F_COMM_USERDATA_TYPE type )
{
  GF_ASSERT( commData != NULL );
  GF_ASSERT( commData->userData_.type_ == FCUT_NO_INIT );

  commData->userData_.type_ = type;
  commData->userData_.selfData_ = GFL_HEAP_AllocClearMemory( commData->heapID_ , FIELD_COMM_DATA_GetUserDataSize( type ));
  commData->userData_.partnerData_ = GFL_HEAP_AllocClearMemory( commData->heapID_ , FIELD_COMM_DATA_GetUserDataSize( type ));
}

void  FIELD_COMM_DATA_DeleteUserData(FIELD_COMM_DATA *commData)
{
  GF_ASSERT( commData != NULL );
  GF_ASSERT( commData->userData_.type_ < FCUT_MAX );

  GFL_HEAP_FreeMemory( commData->userData_.selfData_ );
  GFL_HEAP_FreeMemory( commData->userData_.partnerData_ );
  commData->userData_.type_ = FCUT_NO_INIT;
}

const u32 FIELD_COMM_DATA_GetUserDataSize( const F_COMM_USERDATA_TYPE type )
{
  static const u32 UserDataSizeTable[FCUT_MAX] =
  {
    sizeof(TR_CARD_DATA),
    0x400,
  };
  return UserDataSizeTable[type];
}
//--------------------------------------------------------------
//  �s���ɑΉ������f�[�^�̌^��Ԃ�
//--------------------------------------------------------------
const F_COMM_USERDATA_TYPE  FIELD_COMM_DATA_GetUserDataType_From_Action( const F_COMM_ACTION_LIST action )
{
  static const F_COMM_USERDATA_TYPE UserDataTypeTable[FCAL_MAX] =
  {
    FCUT_TRAINERCARD,
    FCUT_BATTLE,
    FCUT_MAX
  };
  return UserDataTypeTable[action];
}

void* FIELD_COMM_DATA_GetSelfUserData( FIELD_COMM_DATA *commData, const F_COMM_USERDATA_TYPE type )
{
  GF_ASSERT( commData != NULL );
  //��M�o�b�t�@�Ń^�C�v�̎w�肪�ł��Ȃ��̂�FCUT_MAX�ŏ��u�B
  GF_ASSERT( commData->userData_.type_ == type || type == FCUT_MAX );
  return commData->userData_.selfData_;
}

void* FIELD_COMM_DATA_GetPartnerUserData( FIELD_COMM_DATA *commData, const F_COMM_USERDATA_TYPE type )
{
  GF_ASSERT( commData != NULL );
  //��M�o�b�t�@�Ń^�C�v�̎w�肪�ł��Ȃ��̂�FCUT_MAX�ŏ��u�B
  GF_ASSERT( commData->userData_.type_ == type || type == FCUT_MAX );
  return commData->userData_.partnerData_;
}

F_COMM_USERDATA_TYPE FIELD_COMM_DATA_GetUserDataType( FIELD_COMM_DATA *commData )
{
  if( commData == NULL )
  {
    return FCUT_NO_INIT;
  }
  return commData->userData_.type_;
}
