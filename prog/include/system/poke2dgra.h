//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		poke2dgra.h
 *	@brief	�|�P����OBJ,BG�O���t�B�b�N
 *	@author	Toru=Nagihashi
 *	@date		2009.10.05
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "poke_tool/poke_tool.h"
#include "system/pokegra.h"
#include "app/app_menu_common.h"
#include "poke_tool/monsno_def.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�L�����T�C�Y
//=====================================
#define POKE2DGRA_POKEMON_CHARA_WIDTH		(12)	//�|�P�����G�̃L������
#define POKE2DGRA_POKEMON_CHARA_HEIGHT	(12)	//�|�P�����G�̃L��������

#define POKE2DGRA_POKEMON_CHARA_SIZE		(POKE2DGRA_POKEMON_CHARA_WIDTH*POKE2DGRA_POKEMON_CHARA_HEIGHT*GFL_BG_1CHRDATASIZ)	//�|�P�����̃L�����T�C�Y


//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//-------------------------------------
///	�L�����f�[�^�ǂݍ���  
//  �i�����Ńp�b�`�[���̂Ԃ������������Ȃ��Ă��܂�
//  �@�������L�����f�[�^�͂PD�}�b�s���O��8x8,4x8,8x4,4x4���ɂȂ��ł���̂ŁA
//    2D�L�����Ƃ��Ďg�������ꍇ�Apokegra.h�̉��H�֐����g���Ă��������j
//=====================================
extern void* POKE2DGRA_LoadCharacterPPP( NNSG2dCharacterData **ncg_data, const POKEMON_PASO_PARAM* ppp, int dir, HEAPID heapID );
extern void* POKE2DGRA_LoadCharacter( NNSG2dCharacterData **ncg_data, int mons_no, int form_no, int sex, int rare, int dir, BOOL egg, u32 personal_rnd, HEAPID heapID );
//-------------------------------------
///	BG�֌W
//=====================================
//�����X�^�[�ԍ�������ǂޏꍇ
extern void POKE2DGRA_BG_TransResource( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg, u32 frm, u32 personal_rnd, u32 chr_offs, u32 plt_offs, HEAPID heapID );
extern GFL_ARCUTIL_TRANSINFO POKE2DGRA_BG_TransResourceByAreaMan( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg, u32 frm, u32 plt_offs, HEAPID heapID );
//PPP����ǂޏꍇ
extern void POKE2DGRA_BG_TransResourcePPP( const POKEMON_PASO_PARAM* ppp, int dir, u32 frm, u32 chr_offs, u32 plt_offs, HEAPID heapID );
extern GFL_ARCUTIL_TRANSINFO POKE2DGRA_BG_TransResourceByAreaManPPP( const POKEMON_PASO_PARAM* ppp, int dir, u32 frm, u32 plt_offs, HEAPID heapID );
//�X�N���[���\��t��
extern void POKE2DGRA_BG_WriteScreen( u32 frm, u32 chr_offs, u32 pal_offs, u16 x, u16 y );
//-------------------------------------
///	OBJ�֌W
//=====================================
//�|�P�O���̃A�[�N�n���h��
extern ARCHANDLE *POKE2DGRA_OpenHandle( HEAPID heapID );
//�����X�^�[�ԍ�������ǂޏꍇ
extern u32 POKE2DGRA_OBJ_PLTT_Register( ARCHANDLE *p_handle, int mons_no, int form_no, int sex, int rare, int dir, BOOL egg, CLSYS_DRAW_TYPE vramType, u16 byteOffs, HEAPID heapID );
extern u32 POKE2DGRA_OBJ_CGR_Register( ARCHANDLE *p_handle, int mons_no, int form_no, int sex, int rare, int dir, BOOL egg, u32 personal_rnd, CLSYS_DRAW_TYPE vramType, HEAPID heapID );
extern u32 POKE2DGRA_OBJ_CELLANM_Register( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg, APP_COMMON_MAPPING mapping, CLSYS_DRAW_TYPE vramType, HEAPID heapID );
//PPP����ǂޏꍇ
extern u32 POKE2DGRA_OBJ_PLTT_RegisterPPP( ARCHANDLE *p_handle, const POKEMON_PASO_PARAM* ppp, int dir, CLSYS_DRAW_TYPE vramType, u16 byteOffs, HEAPID heapID );
extern u32 POKE2DGRA_OBJ_CGR_RegisterPPP( ARCHANDLE *p_handle, const POKEMON_PASO_PARAM* ppp, int dir, CLSYS_DRAW_TYPE vramType, HEAPID heapID );
extern u32 POKE2DGRA_OBJ_CELLANM_RegisterPPP( const POKEMON_PASO_PARAM* ppp, int dir, APP_COMMON_MAPPING mapping, CLSYS_DRAW_TYPE vramType, HEAPID heapID );

