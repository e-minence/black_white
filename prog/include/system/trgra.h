//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		trgra.h
 *	@brief	�g���[�i�[�O���t�B�b�N�f�ނ̎擾
 *	@author	HisashiSogabe
 *	@date		2010.01.22
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

//-------------------------------------
///	���\�[�X
//=====================================
#define TRGRA_TRAINER_CHARA_WIDTH		( 10 )	//�|�P�����G�̃L������
#define TRGRA_TRAINER_CHARA_HEIGHT	( 10 )	//�|�P�����G�̃L��������

//�g���[�i�[�̃L�����T�C�Y
#define TRGRA_TRAINER_CHARA_SIZE		( TRGRA_TRAINER_CHARA_WIDTH * TRGRA_TRAINER_CHARA_HEIGHT * GFL_BG_1CHRDATASIZ )

#define TRGRA_TRAINER_PLT_NUM				( 1 )	                            //�g���[�i�[��l���̃p���b�g�g�p�{��
#define TRGRA_TRAINER_PLT_SIZE			( TRGRA_TRAINER_PLT_NUM * 0x20 )	//�g���[�i�[��l���̃p���b�g�T�C�Y


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
extern ARCID TRGRA_GetArcID( int dir );
//NCGR
extern ARCDATID TRGRA_GetCgrArcIndex( int tr_type, int dir );
//NCBR
extern ARCDATID TRGRA_GetCbrArcIndex( int tr_type, int dir );
//NCLR
extern ARCDATID TRGRA_GetPalArcIndex( int tr_type, int dir );
//NCER	-> �}���`�Z���p�̃p�[�c�ł��B
extern ARCDATID TRGRA_GetCelArcIndex( int tr_type, int dir );
//NANR
extern ARCDATID TRGRA_GetAnmArcIndex( int tr_type, int dir );
//NMCR
extern ARCDATID TRGRA_GetMCelArcIndex( int tr_type, int dir );
//NMAR
extern ARCDATID TRGRA_GetMAnmArcIndex( int tr_type, int dir );
//NCEC MCSS�p�t�@�C��
extern ARCDATID TRGRA_GetNcecArcIndex( int tr_type, int dir );
