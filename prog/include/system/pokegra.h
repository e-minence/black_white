//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		pokegra.h
 *	@brief	�|�P�����O���t�B�b�N�f�ނ̎擾
 *	@author	Toru=Nagihashi
 *	@date		2009.10.05
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
//���C�u����
#include <gflib.h>
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define	POKEGRA_DIR_FRONT	( 0 )	//����
#define	POKEGRA_DIR_BACK	( 1 )	//�w��

//�|�P������̂��\������MCSS�p�t�@�C���̍\��
enum{
	POKEGRA_FRONT_M_NCGR = 0,
	POKEGRA_FRONT_F_NCGR,
	POKEGRA_FRONT_M_NCBR,
	POKEGRA_FRONT_F_NCBR,
	POKEGRA_FRONT_NCER,
	POKEGRA_FRONT_NANR,
	POKEGRA_FRONT_NMCR,
	POKEGRA_FRONT_NMAR,
	POKEGRA_FRONT_NCEC,
	POKEGRA_BACK_M_NCGR,
	POKEGRA_BACK_F_NCGR,
	POKEGRA_BACK_M_NCBR,
	POKEGRA_BACK_F_NCBR,
	POKEGRA_BACK_NCER,
	POKEGRA_BACK_NANR,
	POKEGRA_BACK_NMCR,
	POKEGRA_BACK_NMAR,
	POKEGRA_BACK_NCEC,
	POKEGRA_NORMAL_NCLR,
	POKEGRA_RARE_NCLR,

	POKEGRA_FILE_MAX,			//�|�P������̂��\������MCSS�p�t�@�C���̑���

	POKEGRA_M_NCGR = 0,
	POKEGRA_F_NCGR,
	POKEGRA_M_NCBR,
	POKEGRA_F_NCBR,
	POKEGRA_NCER,
	POKEGRA_NANR,
	POKEGRA_NMCR,
	POKEGRA_NMAR,
	POKEGRA_NCEC,

  POKEGRA_MAX,

  POKEGRA_PLTT_ONLY_MAX = 2,  //�t�H�����Ⴂ�̃f�[�^���p���b�g�݂̂̎��̑���
};

//-------------------------------------
///	���\�[�X
//=====================================
#define POKEGRA_POKEMON_CHARA_WIDTH		(12)	//�|�P�����G�̃L������
#define POKEGRA_POKEMON_CHARA_HEIGHT	(12)	//�|�P�����G�̃L��������

#define POKEGRA_POKEMON_CHARA_SIZE		(POKEGRA_POKEMON_CHARA_WIDTH*POKEGRA_POKEMON_CHARA_HEIGHT*GFL_BG_1CHRDATASIZ)	//�|�P�����̃L�����T�C�Y

#define POKEGRA_POKEMON_PLT_NUM				(1)	//�|�P����1�̕��̃p���b�g�g�p�{��
#define POKEGRA_POKEMON_PLT_SIZE			(POKEGRA_POKEMON_PLT_NUM*0x20)	//�|�P����1�̕��̃p���b�g�T�C�Y

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================

//-------------------------------------
///	�A�[�J�C�u�擾
//=====================================
extern ARCID POKEGRA_GetArcID( void );

//-------------------------------------
///	���\�[�X�擾
//=====================================
//NCGR
extern ARCDATID POKEGRA_GetCgrArcIndex( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg );
//NCBR
extern ARCDATID POKEGRA_GetCbrArcIndex( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg );
//NCLR
extern ARCDATID POKEGRA_GetPalArcIndex( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg );
//NCER	-> �}���`�Z���p�̃p�[�c�ł��B
extern ARCDATID POKEGRA_GetCelArcIndex( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg );
//NANR
extern ARCDATID POKEGRA_GetAnmArcIndex( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg );
//NMCR
extern ARCDATID POKEGRA_GetMCelArcIndex( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg );
//NMAR
extern ARCDATID POKEGRA_GetMAnmArcIndex( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg );
//NCEC MCSS�p�t�@�C��
extern ARCDATID POKEGRA_GetNcecArcIndex( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg );


//-------------------------------------
///	���H
//=====================================
//�ȉ��A2D�p�L�����̓Z�������̂��߁A���ёւ�����Ă���̂ŁA
//�����BG�Ŏg����悤�ɕ��ђ����֐�
extern void POKEGRA_SortBGCharacter( NNSG2dCharacterData *p_chara, HEAPID heapID );
//���ђ��������̂����Ƃ̕��тɖ߂��֐�
extern void POKEGRA_SortOBJCharacter( NNSG2dCharacterData *p_chara, HEAPID heapID );
//�p�b�`�[���̂Ԃ����L�����ɏ������ށiBG�L�������W�̏ꍇ�j
extern void POKEGRA_MakePattiiruBuchi( NNSG2dCharacterData *p_chara, u32 personal_rnd );
