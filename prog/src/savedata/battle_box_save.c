//============================================================================================
/**
 * @file	battle_box_save.c
 * @brief	�o�g���{�b�N�X�Z�[�u�f�[�^
 * @author	Nobuhiko Ariizumi
 * @date	2009/09/25
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"
#include "savedata/battle_box_save.h"
#include "savedata/save_tbl.h"
#include "poke_tool/poke_tool_def.h"
#include "msg/msg_boxmenu.h"
#include "arc_def.h"
#include "message.naix"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

struct _BATTLE_BOX_SAVE
{
  POKEMON_PASO_PARAM ppp[BATTLE_BOX_PARTY_NUM][BATTLE_BOX_PARTY_MEMBER];
  STRCODE name[BATTLE_BOX_PARTY_NUM][BATTLE_BOX_NAME_BUFSIZE];
  u16 lockFlg;  //�o�g���{�b�N�X�����b�N����Ă��邩�ǂ���
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
int BATTLE_BOX_SAVE_GetWorkSize(void)
{
  return sizeof( BATTLE_BOX_SAVE );
}

void BATTLE_BOX_SAVE_InitWork(BATTLE_BOX_SAVE *btlBoxSave)
{
  u32 i,p;
	GFL_MSGDATA*  msgman;

  GFL_STD_MemClear( btlBoxSave, sizeof(BATTLE_BOX_SAVE) );

  for(i = 0; i < BATTLE_BOX_PARTY_NUM; i++)
  {
    for(p = 0; p < BATTLE_BOX_PARTY_MEMBER; p++)
    {
      PPP_Clear( &(btlBoxSave->ppp[i][p]) );
    }
  }
  // �f�t�H���g�{�b�N�X���Z�b�g
  // �{�b�N�X�Ɠ������̂��Z�b�g
  msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_boxmenu_dat, GFL_HEAPID_APP );
  if( msgman )
  {
    for(i = 0 ; i < BATTLE_BOX_PARTY_NUM; i++)
    {
      GFL_MSG_GetStringRaw( msgman, mes_boxmenu_02_02+i, btlBoxSave->name[i], BATTLE_BOX_NAME_BUFSIZE );
    }
    GFL_MSG_Delete( msgman );
  }
}

//----------------------------------------------------------
//	�f�[�^����̂��߂̊֐�
//----------------------------------------------------------
BATTLE_BOX_SAVE* BATTLE_BOX_SAVE_GetBattleBoxSave( SAVE_CONTROL_WORK *sv )
{
  return (BATTLE_BOX_SAVE*)SaveControl_DataPtrGet( sv , GMDATA_ID_BATTLE_BOX );
}


//======================================================================
//	�ݒ�
//======================================================================

//----------------------------------------------------------
//  PPP�Z�b�g
//----------------------------------------------------------
void BATTLE_BOX_SAVE_SetPPP( BATTLE_BOX_SAVE * btlBoxSave, const u32 boxIdx, const u32 idx, const POKEMON_PASO_PARAM * ppp )
{
	btlBoxSave->ppp[boxIdx][idx] = *ppp;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�o�g���{�b�N�X�̃Z�[�u�f�[�^����POEPARTY���쐬
 *
 * @param		sv				�o�g���{�b�N�X�̃Z�[�u�f�[�^
 * @param		heapID		�q�[�v�h�c
 *
 * @return	�쐬�����f�[�^
 */
//--------------------------------------------------------------------------------------------
POKEPARTY * BATTLE_BOX_SAVE_MakePokeParty( BATTLE_BOX_SAVE * sv, HEAPID heapID )
{
	POKEMON_PASO_PARAM * ppp;
	POKEMON_PARAM * pp;
	POKEPARTY * party;
	u32	i;

	party = PokeParty_AllocPartyWork( heapID );
	PokeParty_Init( party, TEMOTI_POKEMAX );

	for( i=0; i<TEMOTI_POKEMAX; i++ ){
		ppp = BATTLE_BOX_SAVE_GetPPP( sv, 0, i );
		if( PPP_Get( ppp, ID_PARA_poke_exist, NULL ) == 0 ){
			break;
		}
		pp = PP_CreateByPPP( ppp, GFL_HEAP_LOWID(heapID) );
		PokeParty_Add( party, pp );
		GFL_HEAP_FreeMemory( pp );
	}

	return party;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		POEPARTY���o�g���{�b�N�X�̃Z�[�u�f�[�^�ɐݒ�
 *
 * @param		sv				�o�g���{�b�N�X�̃Z�[�u�f�[�^
 * @param		party			POKEPARTY
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BATTLE_BOX_SAVE_SetPokeParty( BATTLE_BOX_SAVE * sv, POKEPARTY * party )
{
	POKEMON_PARAM * pp;
	u32	i;

	// �Z�[�u�f�[�^������
	for( i=0; i<BATTLE_BOX_PARTY_MEMBER; i++ ){
		PPP_Clear( &sv->ppp[0][i] );
	}

	for( i=0; i<PokeParty_GetPokeCount(party); i++ ){
		pp = PokeParty_GetMemberPointer( party, i );
		BATTLE_BOX_SAVE_SetPPP( sv, 0, i, PP_GetPPPPointerConst(pp) );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�o�g���{�b�N�X�Ƀ|�P�����������Ă��邩�ǂ���
 * @param		sv				�o�g���{�b�N�X�̃Z�[�u�f�[�^
 * @return	������TRUE
 */
//--------------------------------------------------------------------------------------------
BOOL BATTLE_BOX_SAVE_IsIn( BATTLE_BOX_SAVE * sv )
{
	POKEMON_PASO_PARAM * ppp;
	POKEMON_PARAM * pp;

  ppp = BATTLE_BOX_SAVE_GetPPP( sv, 0, 0 );
  if( PPP_Get( ppp, ID_PARA_poke_exist, NULL ) == 0 ){
    return FALSE;
  }
	return TRUE;
}


//----------------------------------------------------------
//  �o�g���{�b�N�X�����b�N����
//----------------------------------------------------------
void BATTLE_BOX_SAVE_OnLockFlg( BATTLE_BOX_SAVE *btlBoxSave,BATTLE_BOX_LOCK_BIT flg )
{
  btlBoxSave->lockFlg |= flg;
}
void BATTLE_BOX_SAVE_OffLockFlg( BATTLE_BOX_SAVE *btlBoxSave,BATTLE_BOX_LOCK_BIT flg )
{
  btlBoxSave->lockFlg &= ~flg;
}
   
//======================================================================
//	�擾�n
//======================================================================

//----------------------------------------------------------
//  PPP�擾
//----------------------------------------------------------
POKEMON_PASO_PARAM* BATTLE_BOX_SAVE_GetPPP( BATTLE_BOX_SAVE *btlBoxSave , const u32 boxIdx , const u32 idx )
{
  return &btlBoxSave->ppp[boxIdx][idx];
}

//----------------------------------------------------------
//  �{�b�N�X���擾
//----------------------------------------------------------
STRCODE* BATTLE_BOX_SAVE_GetBoxName( BATTLE_BOX_SAVE *btlBoxSave , const u32 boxIdx )
{
  return btlBoxSave->name[boxIdx];
}

//----------------------------------------------------------
//  ���b�N����Ă��邩�ǂ���
//----------------------------------------------------------
BOOL BATTLE_BOX_SAVE_GetLockFlg( BATTLE_BOX_SAVE *btlBoxSave )
{
  return btlBoxSave->lockFlg != BATTLE_BOX_LOCK_BIT_NONE;
}
BOOL BATTLE_BOX_SAVE_GetLockType( const BATTLE_BOX_SAVE *btlBoxSave, BATTLE_BOX_LOCK_BIT flg )
{ 
  return (btlBoxSave->lockFlg & flg) != 0;
}
