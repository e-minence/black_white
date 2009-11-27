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
#define FIELD_WFBC_PEOPLE_MAX ( 20 )
#define FIELD_WFBC_INIT_PEOPLE_NUM ( 10 ) // �������ł̐l�̐���ύX


//-------------------------------------
/// �@��
//=====================================
#define FIELD_WFBC_MOOD_DEFAULT ( 50 )  // @TODO ��X�́A�G�N�Z���f�[�^����
#define FIELD_WFBC_MOOD_MAX     ( 100 ) // �ő�l
#define FIELD_WFBC_MOOD_ADD_INTOWN  ( 3 ) // �X�ɖK���Ƒ������l
#define FIELD_WFBC_MOOD_ADD_TALK    ( 10 )  // �b�������1��1�񑫂����l
#define FIELD_WFBC_MOOD_SUB         ( -5 ) // 1��1�񌸂�l

//-------------------------------------
///	NPCID�̍ő�l
//=====================================
#define FIELD_WFBC_NPCID_MAX  ( 50 )

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
//���[�N�̃N���A
extern void FIELD_WFBC_CORE_Crear( FIELD_WFBC_CORE* p_wk );
//�X�̏����Z�b�g�A�b�v
extern void FIELD_WFBC_CORE_SetUp( FIELD_WFBC_CORE* p_wk, const MYSTATUS* cp_mystatus );
//�������`�F�b�N
extern BOOL FIELD_WFBC_CORE_IsConfomity( const FIELD_WFBC_CORE* cp_wk );
//�f�[�^�̒���  �s���f�[�^�̏ꍇ�A����ȏ��ɏ��������܂��B
extern void FIELD_WFBC_CORE_Management( FIELD_WFBC_CORE* p_wk );
// �f�[�^�̗L��   �s���f�[�^�̏ꍇ�AFALSE��Ԃ��܂��B
extern BOOL FIELD_WFBC_CORE_IsInData( const FIELD_WFBC_CORE* cp_wk );
// �l�̐���Ԃ�
extern u32 FIELD_WFBC_CORE_GetPeopleNum( const FIELD_WFBC_CORE* cp_wk );
// �@���l�Ń\�[�g����
// �\�[�g�Ɏg�p����e���|�������[�N�𐶐����邽�߂�heapID�ł��B
extern void FIELD_WFBC_CORE_SortData( FIELD_WFBC_CORE* p_wk, HEAPID heapID );
// 1���̐؂�ւ��Ǘ�
extern void FIELD_WFBC_CORE_CalcOneDataStart( FIELD_WFBC_CORE* p_wk );
// �X�ɓ������I�v�Z
extern void FIELD_WFBC_CORE_CalcMoodInTown( FIELD_WFBC_CORE* p_wk );
// �l�𑫂�����
extern void FIELD_WFBC_CORE_AddPeople( FIELD_WFBC_CORE* p_wk, const FIELD_WFBC_CORE_PEOPLE* cp_people );
// �l��T��
extern FIELD_WFBC_CORE_PEOPLE* FIELD_WFBC_CORE_GetNpcIDPeople( FIELD_WFBC_CORE* p_wk, u32 npc_id );

//-------------------------------------
///	FIELD_WFBC_CORE_PEOPLE�p�֐�
//=====================================
//���[�N�̃N���A
extern void FIELD_WFBC_CORE_PEOPLE_Crear( FIELD_WFBC_CORE_PEOPLE* p_wk );
//�������`�F�b�N 
//@TODO�@�ŏI�I�ɂ́A�Ή�����l�̏���������āA�����������킹��
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

#ifdef _cplusplus
}	// extern "C"{
#endif


