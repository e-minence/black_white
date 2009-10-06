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
 *					�v���g�^�C�v�錾
*/
//=============================================================================
extern ARCID POKEGRA_GetArcID( void );
//NCGR
extern ARCDATID POKEGRA_GetCgrArcIndex( int mons_no, int form_no, int sex, int rare, int dir );
//NCBR
extern ARCDATID POKEGRA_GetCbrArcIndex( int mons_no, int form_no, int sex, int rare, int dir );
//NCLR
extern ARCDATID POKEGRA_GetPalArcIndex( int mons_no, int form_no, int sex, int rare, int dir );
//NCER	-> �}���`�Z���p�̃p�[�c�ł��B
extern ARCDATID POKEGRA_GetCelArcIndex( int mons_no, int form_no, int sex, int rare, int dir );
//NANR
extern ARCDATID POKEGRA_GetAnmArcIndex( int mons_no, int form_no, int sex, int rare, int dir );
//NMCR
extern ARCDATID POKEGRA_GetMCelArcIndex( int mons_no, int form_no, int sex, int rare, int dir );
//NMAR
extern ARCDATID POKEGRA_GetMAnmArcIndex( int mons_no, int form_no, int sex, int rare, int dir );
//NCEC MCSS�p�t�@�C��
extern ARCDATID POKEGRA_GetNcecArcIndex( int mons_no, int form_no, int sex, int rare, int dir );
