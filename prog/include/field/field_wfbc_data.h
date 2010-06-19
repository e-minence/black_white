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

#include <net.h>

#include "system/vm.h"

#include "gamesystem/gamedata_def.h"  //GAMEDATA
#include "field/intrude_common.h"
#include "field/field_wfbc_people_def.h"
#include "field/fldmmdl.h"
#include "field/intrude_common.h"

#include "field/field_status_local.h"
#include "field/scrcmd_shop.h"


#include "savedata/mystatus.h"

#include "buflen.h"



//-------------------------------------
///	�f�o�b�N�p�錾
//=====================================
#ifdef PM_DEBUG

#define FIELD_WFBC_MAKE_MAPDATA_DEBUG

#endif



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
#define FIELD_WFBC_ONEDAY_MSK_DUMMYMODE_INIT  ( FIELD_WFBC_ONEDAY_MSK_BATTLE )  // �l���������O�Ȃ̂ɓ��삷��ꍇ�̏�����

// ON
#define FIELD_WFBC_ONEDAY_MSK_ON( val, msk )  ( (val) | (msk) )
// OFF
#define FIELD_WFBC_ONEDAY_MSK_OFF( val, msk )  ( (val) & ~(msk) )
// CHECK
#define FIELD_WFBC_ONEDAY_MSK_CHECK( val, msk )  ( (val) & (msk) )


//-------------------------------------
///	OBJID����
//=====================================
#define FIELD_WFBC_OBJID_START  ( 128 )
#define FIELD_WFBC_OBJID_SET(x)  ( (x) + FIELD_WFBC_OBJID_START )
#define FIELD_WFBC_OBJID_GET(x)  ( (x) - FIELD_WFBC_OBJID_START )

#define FIELD_WFBC_ITEM_OBJID_START  ( 160 )


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
#define FIELD_WFBC_INIT_BACK_PEOPLE_NUM ( 3 ) // �������ł̐l�̐���ύX



//-------------------------------------
/// �@��
//=====================================
#define FIELD_WFBC_MOOD_MAX     ( 100 ) // �ő�l
#define FIELD_WFBC_MOOD_ADD_INTOWN  ( 3 ) // �X�ɖK���Ƒ������l
#define FIELD_WFBC_MOOD_ADD_TALK    ( 10 )  // �b�������1��1�񑫂����l
#define FIELD_WFBC_MOOD_SUB         ( -5 ) // 1��1�񌸂�l

#define FIELD_WFBC_MOOD_TAKES       (20) // �A��Ă����Ăق����Ƃ����l

#define FIELD_WFBC_MOOD_SUB_DAY_MAX ( FIELD_WFBC_MOOD_MAX / MATH_ABS(FIELD_WFBC_MOOD_SUB) + 1 ) // �������ɂ��̍ő�l + 1�͊���؂�Ȃ��Ƃ��̗\��


//-------------------------------------
///	Shop��
//=====================================
#define FIELD_WFBC_SHOP_MAX ( 5 )
#define FIELD_WFBC_SHOP_ONE_PEOPLE ( 2 )  // 1�V���b�v�ŎQ�Ƃ���l����

//-------------------------------------
///	�z�u�A�C�e���@�s��l
//=====================================
#define FIELD_WFBC_ITEM_NONE  (0xffff)

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
  u8 data_in:4;   // �l�̗L�� TRUE FALSE
  u8 parent_in:4; // �e���̗L�� TRUE FALSE
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

// WFBC �p���X�@�J�n
extern void GAMEDATA_SetUpPalaceWFBCCoreData( GAMEDATA * gamedata, const FIELD_WFBC_CORE* cp_core );
extern void GAMEDATA_ClearPalaceWFBCCoreData( GAMEDATA * gamedata );


// 1���̐؂�ւ��Ǘ�
extern void FIELD_WFBC_CORE_CalcOneDataStart( GAMEDATA * gamedata, s32 diff_day, HEAPID heapID );

// WFBC�]�[���`�F���W�ł́A�l�n�n�c�`�F�b�N
extern void FIELD_WFBC_CORE_CalcZoneChange( GAMEDATA * gamedata );

//----------------------------------------------------------
//  ZONEDATA�̑���    �w�풓�x
//----------------------------------------------------------
extern void FIELD_WFBC_CORE_SetUpZoneData( const FIELD_WFBC_CORE* cp_wk );


//-------------------------------------
///	MMDL���[�N����AWFBC�@NPCID���擾
//=====================================
extern u32 FIELD_WFBC_CORE_GetMMdlNpcID( const MMDL* cp_mmdl );


//-------------------------------------
///	FIELD_WFBC_CORE�p�֐�
//=====================================
//���[�N�̑S�N���A
extern void FIELD_WFBC_CORE_Clear( FIELD_WFBC_CORE* p_wk ); // �w�풓�x
extern void FIELD_WFBC_CORE_ClearNormal( FIELD_WFBC_CORE* p_wk );
extern void FIELD_WFBC_CORE_ClearBack( FIELD_WFBC_CORE* p_wk );
//�X�̏����Z�b�g�A�b�v
extern void FIELD_WFBC_CORE_SetUp( FIELD_WFBC_CORE* p_wk, const MYSTATUS* cp_mystatus, HEAPID tmp_heapID );
//�������`�F�b�N
extern BOOL FIELD_WFBC_CORE_IsConfomity( const FIELD_WFBC_CORE* cp_wk );
//�f�[�^�̒���  �s���f�[�^�̏ꍇ�A����ȏ��ɏ��������܂��B
extern void FIELD_WFBC_CORE_Management( FIELD_WFBC_CORE* p_wk );
// �f�[�^�̗L��   �s���f�[�^�̏ꍇ�AFALSE��Ԃ��܂��B
extern BOOL FIELD_WFBC_CORE_IsInData( const FIELD_WFBC_CORE* cp_wk );
// �l�̐���Ԃ�
extern u32 FIELD_WFBC_CORE_GetPeopleNum( const FIELD_WFBC_CORE* cp_wk, MAPMODE mapmode );
// �@���l�Ń\�[�g����
// �\�[�g�Ɏg�p����e���|�������[�N�𐶐����邽�߂�heapID�ł��B
extern void FIELD_WFBC_CORE_SortData( FIELD_WFBC_CORE* p_wk, HEAPID heapID );
// �󂢂Ă��郏�[�N���߂鏈��
extern void FIELD_WFBC_CORE_PackPeopleArray( FIELD_WFBC_CORE* p_wk, MAPMODE mapmode );   // �w�풓�x
// �X�ɓ������I�v�Z
extern void FIELD_WFBC_CORE_CalcMoodInTown( FIELD_WFBC_CORE* p_wk );
// �l�𑫂�����
extern void FIELD_WFBC_CORE_AddPeople( FIELD_WFBC_CORE* p_wk, const MYSTATUS* cp_mystatus, const FIELD_WFBC_CORE_PEOPLE* cp_people );
// �l��T�� �풓�V�X�e��
extern FIELD_WFBC_CORE_PEOPLE* FIELD_WFBC_CORE_GetNpcIDPeople( FIELD_WFBC_CORE* p_wk, u32 npc_id, MAPMODE mapmode );
extern BOOL FIELD_WFBC_CORE_IsOnNpcIDPeople( const FIELD_WFBC_CORE* cp_wk, u32 npc_id, MAPMODE mapmode );
// �f�[�^����AMMDL�w�b�_�[�𐶐�
// mapmode == field_status_local.h MAPMODE
// �߂�l�́AGFL_HEAP_Free�����Ă��������B
extern MMDL_HEADER* FIELD_WFBC_CORE_MMDLHeaderCreateHeapLo( const FIELD_WFBC_CORE* cp_wk, MAPMODE mapmode, HEAPID heapID );

// �f�[�^����V���b�v���𐶐�
extern void FIELD_WFBC_CORE_SetShopData( const FIELD_WFBC_CORE* cp_wk, SHOP_ITEM* p_buff, u16* p_num, u32 shop_idx, HEAPID heapID );


#ifdef PM_DEBUG
// �܂��g�p���Ă��Ȃ�NPC��random�Ŏ擾
extern u32 FIELD_WFBC_CORE_DEBUG_GetRandomNpcID( const FIELD_WFBC_CORE* cp_wk );
#endif

//-------------------------------------
///	FIELD_WFBC_CORE_PEOPLE�p�֐�
//=====================================
//���[�N�̃N���A
extern void FIELD_WFBC_CORE_PEOPLE_Clear( FIELD_WFBC_CORE_PEOPLE* p_wk ); // �w�풓�x

//�������`�F�b�N �@�풓�V�X�e��
extern BOOL FIELD_WFBC_CORE_PEOPLE_IsConfomity( const FIELD_WFBC_CORE_PEOPLE* cp_wk );
//�f�[�^�̒���  �s���f�[�^�̏ꍇ�A����ȏ��ɏ��������܂��B�@�풓�V�X�e��
extern void FIELD_WFBC_CORE_PEOPLE_Management( FIELD_WFBC_CORE_PEOPLE* p_wk );
// �f�[�^�̗L��   �s���f�[�^�̏ꍇ�AFALSE��Ԃ��܂��B �풓�V�X�e��
extern BOOL FIELD_WFBC_CORE_PEOPLE_IsInData( const FIELD_WFBC_CORE_PEOPLE* cp_wk );

// �b���������v�Z�I
extern void FIELD_WFBC_CORE_PEOPLE_CalcTalk( FIELD_WFBC_CORE_PEOPLE* p_wk );

// �o�g�������I�ݒ�
extern void FIELD_WFBC_CORE_PEOPLE_SetBattle( FIELD_WFBC_CORE_PEOPLE* p_wk );
extern BOOL FIELD_WFBC_CORE_PEOPLE_IsBattle( const FIELD_WFBC_CORE_PEOPLE* cp_wk );
 
// ���ɃA�N�Z�X
extern void FIELD_WFBC_CORE_PEOPLE_SetParentData( FIELD_WFBC_CORE_PEOPLE* p_wk, const MYSTATUS* cp_mystatus );
extern void FIELD_WFBC_CORE_PEOPLE_GetParentName( const FIELD_WFBC_CORE_PEOPLE* cp_wk, STRCODE* p_buff );
extern BOOL FIELD_WFBC_CORE_PEOPLE_IsParentIn( const FIELD_WFBC_CORE_PEOPLE* cp_wk );
extern u32 FIELD_WFBC_CORE_PEOPLE_GetParentID( const FIELD_WFBC_CORE_PEOPLE* cp_wk );
extern u32 FIELD_WFBC_CORE_PEOPLE_GetNpcID( const FIELD_WFBC_CORE_PEOPLE* cp_wk );
extern BOOL FIELD_WFBC_CORE_PEOPLE_IsMoodTakes( const FIELD_WFBC_CORE_PEOPLE* cp_wk );









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
  u16     script_plcw; 
  u16     script_plc; 
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

extern const FIELD_WFBC_CORE_PEOPLE_POS* FIELD_WFBC_PEOPLE_POS_GetIndexItemPos( const FIELD_WFBC_CORE_PEOPLE_POS* cp_wk, u32 index );


//-----------------------------------------------------------------------------
/**
 *					WFBC�A�C�e���z�u���
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	WFBC�A�C�e���z�u���
//=====================================
typedef struct {
  u16 scr_item[ FIELD_WFBC_PEOPLE_MAX ];  // idx�ʒu�̃A�C�e���X�N���v�gID
} FIELD_WFBC_CORE_ITEM;

//-------------------------------------
///	GAMEDATA����擾
//=====================================
extern FIELD_WFBC_CORE_ITEM* GAMEDATA_GetWFBCItemData( GAMEDATA * gamedata );


//-------------------------------------
///	�S�̃N���A
// �풓
//=====================================
extern void WFBC_CORE_ITEM_ClaerAll( FIELD_WFBC_CORE_ITEM* p_wk );

//-------------------------------------
///	�A�C�e����
//=====================================
extern u32 WFBC_CORE_ITEM_GetNum( const FIELD_WFBC_CORE_ITEM* cp_wk );

//-------------------------------------
///	�A�C�e���z�u����ݒ�
//=====================================
extern BOOL FIELD_WFBC_CORE_ITEM_SetItemData( FIELD_WFBC_CORE_ITEM* p_wk, u16 scr_item, u32 idx );
extern void FIELD_WFBC_CORE_ITEM_ClearItemData( FIELD_WFBC_CORE_ITEM* p_wk, u32 idx );

//-------------------------------------
///	�A�C�e���z�u�����擾
//=====================================
extern u16 FIELD_WFBC_CORE_ITEM_GetItemData( const FIELD_WFBC_CORE_ITEM* cp_wk, u32 idx );
extern BOOL FIELD_WFBC_CORE_ITEM_IsInItemData( const FIELD_WFBC_CORE_ITEM* cp_wk, u32 idx );

//-------------------------------------
///	�f�[�^����AMMDL�w�b�_�[�𐶐�
//=====================================
extern MMDL_HEADER* FIELD_WFBC_CORE_ITEM_MMDLHeaderCreateHeapLo( const FIELD_WFBC_CORE_ITEM* cp_wk, MAPMODE mapmode, FIELD_WFBC_CORE_TYPE type, HEAPID heapID );






//-----------------------------------------------------------------------------
/**
 *					WFBC�C�x���g�Ǘ����[�N
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	BC�C�x���g�萔
//=====================================
#define FIELD_WFBC_EVENT_NPC_WIN_TARGET_INIT (10)
#define FIELD_WFBC_EVENT_NPC_WIN_TARGET_ADD (10)
#define FIELD_WFBC_EVENT_NPC_WIN_TARGET_MAX (10000)

//-------------------------------------
///	�C�x���g���[�N
//=====================================
typedef union {
  
  // BC�p���[�N
  struct
  {
    u16 bc_npc_win_count;     // �{�X�C�x���g�@BC�̐l�ɏ�������
    u16 bc_npc_win_target;    // �{�X�C�x���g�@BC�̐l�̏����ڕW��
  };

  // WF�p���[�N
  struct
  {
    u16 wf_poke_catch_item;   // �����C�x���g�@���炦��A�C�e��
    u16 wf_poke_catch_monsno; // �����C�x���g�@�����Ă��Ă��炢�����|�P�����i���o�[
  };
} FIELD_WFBC_EVENT;

//-------------------------------------
///	GAMEDATA����擾
//=====================================
extern FIELD_WFBC_EVENT* GAMEDATA_GetWFBCEventData( GAMEDATA * gamedata );


//-------------------------------------
///	���[�N����
//  �풓
//=====================================
extern void FIELD_WFBC_EVENT_Clear( FIELD_WFBC_EVENT* p_wk );
extern void FIELD_WFBC_EVENT_Init( FIELD_WFBC_EVENT* p_wk, const FIELD_WFBC_CORE* cp_core, HEAPID heapID );

// BC
extern u16 FIELD_WFBC_EVENT_GetBCNpcWinCount( const FIELD_WFBC_EVENT* cp_wk );
extern void FIELD_WFBC_EVENT_AddBCNpcWinCount( FIELD_WFBC_EVENT* p_wk );
extern u16 FIELD_WFBC_EVENT_GetBCNpcWinTarget( const FIELD_WFBC_EVENT* cp_wk );
extern void FIELD_WFBC_EVENT_AddBCNpcWinTarget( FIELD_WFBC_EVENT* p_wk );

// WF
extern u16 FIELD_WFBC_EVENT_GetWFPokeCatchEventMonsNo( const FIELD_WFBC_EVENT* cp_wk );
extern void FIELD_WFBC_EVENT_SetWFPokeCatchEventMonsNo( FIELD_WFBC_EVENT* p_wk, u16 mons_no );
extern u16 FIELD_WFBC_EVENT_GetWFPokeCatchEventItem( const FIELD_WFBC_EVENT* cp_wk );
extern void FIELD_WFBC_EVENT_SetWFPokeCatchEventItem( FIELD_WFBC_EVENT* p_wk, u16 item );


//-----------------------------------------------------------------------------
/**
 *					�ʐM���
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	NPC_ID����̏�Ԏ擾���N�G�X�g�^�C�v
//=====================================
typedef enum 
{
  FIELD_WFBC_COMM_NPC_REQ_THERE,      // ���݊m�F
  FIELD_WFBC_COMM_NPC_REQ_WISH_TAKE,  // �A��čs������
  FIELD_WFBC_COMM_NPC_REQ_TAKE,       // �A��Ă�����

  FIELD_WFBC_COMM_NPC_REQ_TYPE_NUM,   // �V�X�e�����Ŏg�p
} FIELD_WFBC_COMM_NPC_REQ_TYPE;


//-------------------------------------
///	NPC_ID����̏�� �����^�C�v
//=====================================
typedef enum 
{
  FIELD_WFBC_COMM_NPC_ANS_ON,         // ���܂��B
  FIELD_WFBC_COMM_NPC_ANS_OFF,        // ���܂���B
  FIELD_WFBC_COMM_NPC_ANS_TAKE_OK,    // ��Ă����Ă��������B
  FIELD_WFBC_COMM_NPC_ANS_TAKE_NG,    // ��Ă����܂���B

  FIELD_WFBC_COMM_NPC_ANS_TYPE_NUM,   // �V�X�e�����Ŏg�p
} FIELD_WFBC_COMM_NPC_ANS_TYPE;


//-------------------------------------
///	�q�[���e  NPC_ID�֘A�@���N�G�X�g
//=====================================
typedef struct {
  int net_id;   // ���N�G�X�g���o���Ă���̂͒N���H
  u16 npc_id;
  u16 req_type; // FIELD_WFBC_COMM_NPC_REQ_TYPE
} FIELD_WFBC_COMM_NPC_REQ;

//-------------------------------------
///	�e�[���q  NPC_ID�֘A�@�A���T�[
//=====================================
typedef struct {
  int net_id;   // �N�ɑ΂���ԓ����H
  u16 npc_id;
  u16 ans_type; // FIELD_WFBC_COMM_NPC_ANS_TYPE
} FIELD_WFBC_COMM_NPC_ANS;

//-------------------------------------
///	�ʐM�p���
//=====================================
typedef struct {
  u16 netID;
  
  u16 buff_msk;

  u16 intrudeNetID;
  
  // �ʐM�o�b�t�@
  FIELD_WFBC_COMM_NPC_REQ recv_req_que[ FIELD_COMM_MEMBER_MAX ];
  FIELD_WFBC_COMM_NPC_ANS recv_ans;
  
  FIELD_WFBC_COMM_NPC_REQ send_req;
  FIELD_WFBC_COMM_NPC_ANS send_ans_que[ FIELD_COMM_MEMBER_MAX ];
} WFBC_COMM_DATA;

//-------------------------------------
///	�����Ǘ�
//=====================================
extern void FIELD_WFBC_COMM_DATA_Init( WFBC_COMM_DATA* p_wk );
extern void FIELD_WFBC_COMM_DATA_Exit( WFBC_COMM_DATA* p_wk );
extern void FIELD_WFBC_COMM_DATA_Oya_Main( WFBC_COMM_DATA* p_wk, FIELD_WFBC_CORE* p_mywfbc, u8 member_bit );
extern void FIELD_WFBC_COMM_DATA_Ko_ChangeNpc( WFBC_COMM_DATA* p_wk, FIELD_WFBC_CORE* p_mywfbc, FIELD_WFBC_CORE* p_oyawfbc, const MYSTATUS* cp_mystatus, u16 npc_id, HEAPID heapID );


extern void FIELD_WFBC_COMM_DATA_SetIntrudeNetID( WFBC_COMM_DATA* p_wk, NetID intrudeNetID );

//-------------------------------------
///	�ʐM�p����
//    ��M�̂Ȃ���
//      ��M���������ȉ��̊֐��ɓn�������ł��B
//        FIELD_WFBC_COMM_DATA_SetRecvCommAnsData
//        FIELD_WFBC_COMM_DATA_SetRecvCommReqData
//    ���M�̗���
//      �ȉ��̊֐��ŁA���M��񂪂��邩�`�F�b�N���A���M���Ă��������B
//        FIELD_WFBC_COMM_DATA_GetSendCommAnsData
//        FIELD_WFBC_COMM_DATA_GetSendCommAnsData
//      ���M������������A���M�����N���A���Ă��������B
//        FIELD_WFBC_COMM_DATA_ClearSendCommAnsData
//        FIELD_WFBC_COMM_DATA_ClearSendCommReqData
//
//    �e�Ƀ��N�G�X�g���o���B������҂�
//      �܂��A�����o�b�t�@���N���A���܂��B
//        FIELD_WFBC_COMM_DATA_ClearReqAnsData
//      ���N�G�X�g����ݒ肵�܂��B
//        FIELD_WFBC_COMM_DATA_SetReqData
//      ������҂��܂��B
//        FIELD_WFBC_COMM_DATA_WaitAnsData
//      ���������擾���܂��B
//        FIELD_WFBC_GetAnsData
//      
//=====================================
// ��M���̐ݒ�
extern void FIELD_WFBC_COMM_DATA_SetRecvCommAnsData( WFBC_COMM_DATA* p_wk, const FIELD_WFBC_COMM_NPC_ANS* cp_ans );
extern void FIELD_WFBC_COMM_DATA_SetRecvCommReqData( WFBC_COMM_DATA* p_wk, u16 netID, const FIELD_WFBC_COMM_NPC_REQ* cp_req );

// ���M���̎擾
extern BOOL FIELD_WFBC_COMM_DATA_GetSendCommAnsData( const WFBC_COMM_DATA* cp_wk, u16 netID, FIELD_WFBC_COMM_NPC_ANS* p_ans ); // 0�`3���[�v�ł܂킵�āA���t���[���`�F�b�N���Ă��������B
extern BOOL FIELD_WFBC_COMM_DATA_GetSendCommReqData( const WFBC_COMM_DATA* cp_wk, FIELD_WFBC_COMM_NPC_REQ* p_req, NetID* p_send_netID );

// ���M���������f�[�^�͔j�����Ă��������B
extern void FIELD_WFBC_COMM_DATA_ClearSendCommAnsData( WFBC_COMM_DATA* p_wk, u16 netID );
extern void FIELD_WFBC_COMM_DATA_ClearSendCommReqData( WFBC_COMM_DATA* p_wk );

// ���N�G�X�g���̐ݒ�
extern void FIELD_WFBC_COMM_DATA_SetReqData( WFBC_COMM_DATA* p_wk, u16 npc_id, FIELD_WFBC_COMM_NPC_REQ_TYPE req_type );
extern BOOL FIELD_WFBC_COMM_DATA_WaitAnsData( const WFBC_COMM_DATA* cp_wk, u16 npc_id );
extern FIELD_WFBC_COMM_NPC_ANS_TYPE FIELD_WFBC_GetAnsData( const WFBC_COMM_DATA* cp_wk );

// ���N�G�X�g���o���O�ɌĂ�ł�������
extern void FIELD_WFBC_COMM_DATA_ClearReqAnsData( WFBC_COMM_DATA* p_wk );


#ifdef _cplusplus
}	// extern "C"{
#endif


