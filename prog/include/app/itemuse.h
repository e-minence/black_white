//============================================================================================
/**
 * @file	itemuse.h
 * @brief	�t�B�[���h�A�C�e���g�p����
 * @author	Hiroyuki Nakamura
 * @date	05.12.12
 */
//============================================================================================

#pragma once


#include "gamesystem/gamesystem.h"

//============================================================================================
//	�萔��`
//============================================================================================
// �A�C�e���g�p���[�N
typedef struct _ITEMUSE_WORK ITEMUSE_WORK;

// �A�C�e���g�p�`�F�b�N���[�N
typedef struct _ITEMCHECK_WORK ITEMCHECK_WORK;

// �A�C�e���g�p�`�F�b�N�֐�
typedef u32 (*ITEMCHECK_FUNC)(const ITEMCHECK_WORK*);

// �A�C�e���g�p�֐�
typedef void (*ITEMUSE_FUNC)(ITEMUSE_WORK*, const ITEMCHECK_WORK *);

// �A�C�e���g�p�֐��擾�p�����[�^
enum {
	ITEMUSE_PRM_USEFUNC = 0,	// �o�b�O����̎g�p�֐�
	ITEMUSE_PRM_CNVFUNC,		// �֗��{�^������̎g�p�֐�
	ITEMUSE_PRM_CHECKFUNC		// �`�F�b�N�֐�
};

// �A�C�e���g�p�`�F�b�N�̖߂�l
enum {
	ITEMCHECK_TRUE = 0,				// �g�p�\

	ITEMCHECK_ERR_CYCLE_OFF,		// ���]�Ԃ��~���Ȃ�
	ITEMCHECK_ERR_COMPANION,		// �g�p�s�E�A�����
	ITEMCHECK_ERR_DISGUISE,			// �g�p�s�E���P�b�g�c�ϑ���

	ITEMCHECK_FALSE = 0xffffffff	// �g�p�s�E���m�̌��t
};


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �g�p�֐��擾
 *
 * @param	prm		�擾�p�����[�^
 * @param	id		�֐�ID
 *
 * @return	�g�p�֐�
 */
//--------------------------------------------------------------------------------------------
extern u32 ItemUse_FuncGet( u16 prm, u16 id );

//--------------------------------------------------------------------------------------------
/**
 * �g�p�`�F�b�N���[�N�쐬
 *
 * @param	fsys	�t�B�[���h���[�N
 * @param	id		�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ItemUse_CheckWorkMake( GAMESYS_WORK * fsys, ITEMCHECK_WORK * icwk );

//--------------------------------------------------------------------------------------------
/**
 * ���̂ݎg�p�^�C�v�擾
 *
 * @param	iuwk	�A�C�e���g�p���[�N
 *
 * @retval	"TRUE = ���߂�"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
//extern BOOL ItemUse_KinomiTypeCheck( const ITEMCHECK_WORK * icwk );

//--------------------------------------------------------------------------------------------
/**
 * �֗��{�^���N������
 *
 * @param	repw	�t�B�[���h���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern int FieldConvButtonEventInit( GAMESYS_WORK * repw, int no );


