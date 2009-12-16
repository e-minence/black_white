//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_wfbc_data.h
 *	@brief  White Forest  Black City ��{�f�[�^
 *	@author	tomoya takahashi
 *	@date		2009.11.10
 *
 *	���W���[�����FFIELD_WFBC_CORE
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "gamesystem/gamedata_def.h"  //GAMEDATA
#include "field/intrude_common.h"
#include "field/field_wfbc_people_def.h"
#include "field/fldmmdl.h"

#include "savedata/mystatus.h"

#include "buflen.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	1���̐����}�X�N
// 1�`8 bit�܂�
//=====================================
typedef enum {
  FIELD_WFBC_ONEDAY_MSK_BATTLE = 1<<0,    ///<�o�g���\
  FIELD_WFBC_ONEDAY_MSK_INTOWN = 1<<1,    ///<�X�ɓ������Ƃ��̉��Z�\
  FIELD_WFBC_ONEDAY_MSK_TALK   = 1<<2,    ///<��b�̌�̉��Z�\
  
} FIELD_WFBC_ONEDAY_MSK;

// 1���̃t���O�����l
#define FIELD_WFBC_ONEDAY_MSK_INIT  ( FIELD_WFBC_ONEDAY_MSK_BATTLE | FIELD_WFBC_ONEDAY_MSK_INTOWN | FIELD_WFBC_ONEDAY_MSK_TALK )

// ON
#define FIELD_WFBC_ONEDAY_MSK_ON( val, msk )  ( (val) | (msk) )
// OFF
#define FIELD_WFBC_ONEDAY_MSK_OFF( val, msk )  ( (val) & ~(msk) )
// CHECK
#define FIELD_WFBC_ONEDAY_MSK_CHECK( val, msk )  ( (val) & (msk) )

//-------------------------------------
///	GAMEDATA�@�X���C���f�b�N�X
//=====================================
typedef enum {
  GAMEDATA_WFBC_ID_COMM = 0,    // �p���X�i�����WFBC���[�N
  GAMEDATA_WFBC_ID_MINE,        // ������WFBC���[�N
  GAMEDATA_WFBC_ID_MAX,
} GAMEDATA_WFBC_ID;


//-------------------------------------
///	�X�̎��
//=====================================
typedef enum
{
  FIELD_WFBC_CORE_TYPE_BLACK_CITY,    // �u���b�N�V�e�B
  FIELD_WFBC_CORE_TYPE_WHITE_FOREST,  // �z���C�g�t�H���X�g

  FIELD_WFBC_CORE_TYPE_MAX,  // �X�̐�
} FIELD_WFBC_CORE_TYPE;


//-------------------------------------
///	�L������l���̐��@�ő吔
//=====================================
#define FIELD_WFBC_PEOPLE_MAX ( 10 )
#define FIELD_WFBC_INIT_PEOPLE_NUM ( 10 ) // �������ł̐l�̐���ύX


//-------------------------------------
/// �@��
//=====================================
#define FIELD_WFBC_MOOD_MAX     ( 100 ) // �ő�l
#define FIELD_WFBC_MOOD_ADD_INTOWN  ( 3 ) // �X�ɖK���Ƒ������l
#define FIELD_WFBC_MOOD_ADD_TALK    ( 10 )  // �b�������1��1�񑫂����l
#define FIELD_WFBC_MOOD_SUB         ( -5 ) // 1��1�񌸂�l

#define FIELD_WFBC_MOOD_SUB_DAY_MAX ( FIELD_WFBC_MOOD_MAX / MATH_ABS(FIELD_WFBC_MOOD_SUB) + 1 ) // �������ɂ��̍ő�l + 1�͊���؂�Ȃ��Ƃ��̗\��

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	WFBC�}�b�vCORE�f�[�^
//=====================================
// �l��CORE�f�[�^
typedef struct 
{
  u8 data_in;   // �l�̗L�� TRUE FALSE
  u8 npc_id;    // �l������肷�邽�߂�NPCID
  u8 mood;      // �@��
  u8 one_day_msk;// 1���̐����}�X�N FIELD_WFBC_ONEDAY_MSK
  

  // �X�ɁA�ʐM�Ȃǂň����p���������Ă���
  // �P�O�̐e���
	STRCODE parent[PERSON_NAME_SIZE + EOM_SIZE];		// 16
  u32     parent_id;  // 4


} FIELD_WFBC_CORE_PEOPLE;

// �S�̂�CORE�f�[�^ 
typedef struct 
{
  u16 data_in;  // TRUE FALSE
  u16 type;     // FIELD_WFBC_CORE_TYPE
  u32 random_no;// �X�̃����_����
  FIELD_WFBC_CORE_PEOPLE people[FIELD_WFBC_PEOPLE_MAX];
  FIELD_WFBC_CORE_PEOPLE back_people[FIELD_WFBC_PEOPLE_MAX];
} FIELD_WFBC_CORE;


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------
//  GAMEDATA����̎擾�֐��S
//----------------------------------------------------------
extern FIELD_WFBC_CORE* GAMEDATA_GetMyWFBCCoreData( GAMEDATA * gamedata );
extern FIELD_WFBC_CORE* GAMEDATA_GetWFBCCoreData( GAMEDATA * gamedata, GAMEDATA_WFBC_ID id );



//-------------------------------------
///	FIELD_WFBC_CORE�p�֐�
//=====================================
//���[�N�̑S�N���A
extern void FIELD_WFBC_CORE_Clear( FIELD_WFBC_CORE* p_wk );
extern void FIELD_WFBC_CORE_ClearNormal( FIELD_WFBC_CORE* p_wk );
extern void FIELD_WFBC_CORE_ClearBack( FIELD_WFBC_CORE* p_wk );
//�X�̏����Z�b�g�A�b�v
extern void FIELD_WFBC_CORE_SetUp( FIELD_WFBC_CORE* p_wk, const MYSTATUS* cp_mystatus, HEAPID heapID );
//�������`�F�b�N
extern BOOL FIELD_WFBC_CORE_IsConfomity( const FIELD_WFBC_CORE* cp_wk );
//�f�[�^�̒���  �s���f�[�^�̏ꍇ�A����ȏ��ɏ��������܂��B
extern void FIELD_WFBC_CORE_Management( FIELD_WFBC_CORE* p_wk );
// �f�[�^�̗L��   �s���f�[�^�̏ꍇ�AFALSE��Ԃ��܂��B
extern BOOL FIELD_WFBC_CORE_IsInData( const FIELD_WFBC_CORE* cp_wk );
// �l�̐���Ԃ�
extern u32 FIELD_WFBC_CORE_GetPeopleNum( const FIELD_WFBC_CORE* cp_wk, u32 mapmode );
// �@���l�Ń\�[�g����
// �\�[�g�Ɏg�p����e���|�������[�N�𐶐����邽�߂�heapID�ł��B
extern void FIELD_WFBC_CORE_SortData( FIELD_WFBC_CORE* p_wk, HEAPID heapID );
// 1���̐؂�ւ��Ǘ�
extern void FIELD_WFBC_CORE_CalcOneDataStart( FIELD_WFBC_CORE* p_wk, s32 diff_day );
// �X�ɓ������I�v�Z
extern void FIELD_WFBC_CORE_CalcMoodInTown( FIELD_WFBC_CORE* p_wk );
// �l�𑫂�����
extern void FIELD_WFBC_CORE_AddPeople( FIELD_WFBC_CORE* p_wk, const FIELD_WFBC_CORE_PEOPLE* cp_people );
// �l��T��
extern FIELD_WFBC_CORE_PEOPLE* FIELD_WFBC_CORE_GetNpcIDPeople( FIELD_WFBC_CORE* p_wk, u32 npc_id );
// �f�[�^����AMMDL�w�b�_�[�𐶐�
// mapmode == field_status_local.h MAPMODE
// �߂�l�́AGFL_HEAP_Free�����Ă��������B
extern MMDL_HEADER* FIELD_WFBC_CORE_MMDLHeaderCreateHeapLo( const FIELD_WFBC_CORE* cp_wk, u32 mapmode, HEAPID heapID );

//-------------------------------------
///	FIELD_WFBC_CORE_PEOPLE�p�֐�
//=====================================
//���[�N�̃N���A
extern void FIELD_WFBC_CORE_PEOPLE_Clear( FIELD_WFBC_CORE_PEOPLE* p_wk );
//�������`�F�b�N 
extern BOOL FIELD_WFBC_CORE_PEOPLE_IsConfomity( const FIELD_WFBC_CORE_PEOPLE* cp_wk );
//�f�[�^�̒���  �s���f�[�^�̏ꍇ�A����ȏ��ɏ��������܂��B
extern void FIELD_WFBC_CORE_PEOPLE_Management( FIELD_WFBC_CORE_PEOPLE* p_wk );
// �f�[�^�̗L��   �s���f�[�^�̏ꍇ�AFALSE��Ԃ��܂��B
extern BOOL FIELD_WFBC_CORE_PEOPLE_IsInData( const FIELD_WFBC_CORE_PEOPLE* cp_wk );

// �b���������v�Z�I
extern void FIELD_WFBC_CORE_PEOPLE_CalcTalk( FIELD_WFBC_CORE_PEOPLE* p_wk );

// ���ɃA�N�Z�X
extern void FIELD_WFBC_CORE_PEOPLE_SetParentData( FIELD_WFBC_CORE_PEOPLE* p_wk, const MYSTATUS* cp_mystatus );
extern void FIELD_WFBC_CORE_PEOPLE_GetParentName( const FIELD_WFBC_CORE_PEOPLE* cp_wk, STRCODE* p_buff );
extern u32 FIELD_WFBC_CORE_PEOPLE_GetParentID( const FIELD_WFBC_CORE_PEOPLE* cp_wk );
extern u32 FIELD_WFBC_CORE_PEOPLE_GetNpcID( const FIELD_WFBC_CORE_PEOPLE* cp_wk );








//-----------------------------------------------------------------------------
/**
 *					WFBC�l�����ւ̃A�N�Z�X
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
// �G���J�E���g�|�P�����̐�
typedef enum
{
  FIELD_WFBC_PEOPLE_ENC_POKE_GRASS_LOW,
  FIELD_WFBC_PEOPLE_ENC_POKE_GRASS_HIGH,
  FIELD_WFBC_PEOPLE_ENC_POKE_WATER,
  FIELD_WFBC_PEOPLE_ENC_POKE_MAX,
  
} FIELD_WFBC_PEOPLE_ENC_POKE_TYPE;

// ���b�Z�[�W�̐�
#define FIELD_WFBC_PEOPLE_WF_MESSAGE_MAX  (3)
#define FIELD_WFBC_PEOPLE_BC_MESSAGE_MAX  (4)

// �G���J�E���g���I�m���@�ő�l
#define FIELD_WFBC_PEOPLE_ENC_POKE_PERCENT_MAX  ( 100 )

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�l�����
// 44�o�C�g
//=====================================
typedef struct 
{
  u16     objid;    // �I�u�W�F�R�[�h
  u16     trtype;   // �g���[�i�[�^�C�v
  u16     mood_wf;  // �@���l�@WF
  u16     mood_bc;  // �@���l  BC
  // �G���J�E���g���
  u16     enc_monsno[FIELD_WFBC_PEOPLE_ENC_POKE_MAX+1]; //+1�p�f�B���O 8byte
  u8      enc_lv[FIELD_WFBC_PEOPLE_ENC_POKE_MAX+1];       // 4byte
  u8      enc_percent[FIELD_WFBC_PEOPLE_ENC_POKE_MAX+1];  // 4byte
  // �o�g���|�P�������
  u32     btl_trdata;   // 4byte
  // SCRIPT
  u16     script_wf; 
  u16     script_bc; 
  // ����
  u16     goods_wf;
  u16     goods_wf_percent;
  u16     goods_bc;
  u16     goods_bc_money;
  // �o���m��
  u16     hit_percent;
  // �u���b�N�v�Z�␳�l
  u16     block_param;
} FIELD_WFBC_PEOPLE_DATA;

//-------------------------------------
///	�l����񃍁[�_�[
//=====================================
typedef struct _FIELD_WFBC_PEOPLE_DATA_LOAD FIELD_WFBC_PEOPLE_DATA_LOAD;


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

extern FIELD_WFBC_PEOPLE_DATA_LOAD* FIELD_WFBC_PEOPLE_DATA_Create( u32 npc_id, HEAPID heapID );
extern void FIELD_WFBC_PEOPLE_DATA_Delete( FIELD_WFBC_PEOPLE_DATA_LOAD* p_wk );

// �ēǂݍ���
extern void FIELD_WFBC_PEOPLE_DATA_Load( FIELD_WFBC_PEOPLE_DATA_LOAD* p_wk, u32 npc_id );

// ���̎擾
extern const FIELD_WFBC_PEOPLE_DATA* FIELD_WFBC_PEOPLE_DATA_GetData( const FIELD_WFBC_PEOPLE_DATA_LOAD* cp_wk );
extern u32 FIELD_WFBC_PEOPLE_DATA_GetLoadNpcID( const FIELD_WFBC_PEOPLE_DATA_LOAD* cp_wk );


//-----------------------------------------------------------------------------
/**
 *					WFBC�l���ʒu���ւ̃A�N�Z�X
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�l���z�u���
//=====================================
typedef struct
{
  u8 gx;
  u8 gz;
} FIELD_WFBC_CORE_PEOPLE_POS;


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

extern FIELD_WFBC_CORE_PEOPLE_POS* FIELD_WFBC_PEOPLE_POS_Create( FIELD_WFBC_PEOPLE_DATA_LOAD* p_loader, FIELD_WFBC_CORE_TYPE type, HEAPID heapID );
extern void FIELD_WFBC_PEOPLE_POS_Delete( FIELD_WFBC_CORE_PEOPLE_POS* p_wk );

extern const FIELD_WFBC_CORE_PEOPLE_POS* FIELD_WFBC_PEOPLE_POS_GetIndexData( const FIELD_WFBC_CORE_PEOPLE_POS* cp_wk, u32 index );





#ifdef _cplusplus
}	// extern "C"{
#endif


