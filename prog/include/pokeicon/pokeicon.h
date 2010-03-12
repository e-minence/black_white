//==============================================================================
/**
 * @file	pokeicon.h
 * @brief	�|�P�����A�C�R���̃w�b�_
 * @author	matsuda
 * @date	2008.11.26(��)
 */
//==============================================================================
#ifndef __POKEICON_H__
#define __POKEICON_H__

#include "poke_tool/poke_tool.h"
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����

#define GENDER_VER

#else                    //DL�q�@������

//#define GENDER_VER

#endif //MULTI_BOOT_MAKE


//==============================================================================
//	�萔��`
//==============================================================================
// �A�j�����
enum {
	POKEICON_ANM_DEATH = 0,		// �m��
	POKEICON_ANM_HPMAX,			// HP MAX
	POKEICON_ANM_HPGREEN,		// HP ��
	POKEICON_ANM_HPYERROW,		// HP ��
	POKEICON_ANM_HPRED,			// HP ��
	POKEICON_ANM_STCHG,			// ��Ԉُ�
};

///�A�C�R���Ŏg�p����p���b�g��
#define	POKEICON_PAL_MAX	( 3 )

/// �A�C�R���̉��T�C�Y
#define POKEICON_SIZE_X		(32)
/// �A�C�R���̏c�T�C�Y
#define POKEICON_SIZE_Y		(32)

///�|�P�����A�C�R����CGX�f�[�^�T�C�Y(1�̕�) ���A�j������
#define POKEICON_SIZE_CGX	(0x20*16 * 2)


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern u32 POKEICON_GetCgxArcIndex( const POKEMON_PASO_PARAM* ppp );
#ifdef GENDER_VER
extern u32 POKEICON_GetCgxArcIndexByMonsNumber( u32 mons, u32 form_no, u32 sex, BOOL egg );
extern const u8 POKEICON_GetPalNum( u32 mons, u32 form, u32 sex, BOOL egg );
#else
extern u32 POKEICON_GetCgxArcIndexByMonsNumber( u32 mons, u32 form_no, BOOL egg );
extern const u8 POKEICON_GetPalNum( u32 mons, u32 form, BOOL egg );
#endif
extern const u8 POKEICON_GetPalNumByPPP( const POKEMON_PASO_PARAM * ppp );
extern u8 POKEICON_GetPalNumGetByPPP( const POKEMON_PASO_PARAM * ppp );
extern u32 POKEICON_GetPalArcIndex(void);
extern u32 POKEICON_GetCellArcIndex(void);
extern u32 POKEICON_GetCellSubArcIndex(void);
extern u32 POKEICON_GetAnmArcIndex(void);
extern u32 POKEICON_GetAnmSubArcIndex(void);

#endif	//__POKEICON_H__
