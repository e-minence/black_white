
//============================================================================================
/**
 * @file	pokeparty_local.h
 * @author	sogabe
 * @date	2007.03.14
 * @brief	�|�P�����莝���f�[�^�^
 *
 * �Z�[�u�f�[�^�֘A�̋��L�w�b�_�B�O�����J�͂��Ȃ��B
 */
//============================================================================================
#ifndef	__POKEPARTY_LOCAL_H__
#define	__POKEPARTY_LOCAL_H__

#include	"poke_tool_def.h"
#include	"pm_define.h"

//----------------------------------------------------------
/**
 * @brief	�|�P�����p�[�e�B�[�\���̂̒�`
 *
 * �ő��6�̂܂ł̃|�P�������܂Ƃ߂ĕێ��ł���悤�ɂȂ��Ă���B
 */
//----------------------------------------------------------
struct _POKEPARTY {
	///	�ێ��ł���|�P�������̍ő�
	int PokeCountMax;
	///	���ݕێ����Ă���|�P������
	int PokeCount;
	///	�|�P�����f�[�^
	struct pokemon_param member[TEMOTI_POKEMAX];
	
	//�X�e�[�^�X��ʂőO������������ۑ�����(�r�b�g�Ǘ�
	u8 statusView;
};

#endif	/* __POKEPARTY_LOCAL_H__ */
