//=============================================================================
/**
 * @file	union_beacon_tool.H
 * @brief	���j�I�����[�������ڐݒ菈��
 * @author	Akito Mori
 * @date    	2006.03.16
 */
//=============================================================================
#pragma once

#include "print/wordset.h"


enum UNION_VIEW_INFO {
  UNIONVIEW_OBJCODE,  // �t�B�[���hOBJ�R�[�h
  UNIONVIEW_TRTYPE,		// �g���[�i�[�O���t�B�b�N�̂h�c�i�o�g����J�[�h���̃O���t�B�b�N�j
  UNIONVIEW_MSGTYPE,	// �g���[�i�[�^�C�v�̃��b�Z�[�W�m�n(msg_trtype_name_dat)

  UNIONVIEW_ICONINDEX,	// �Q�c�摜�p(�l���n�a�i�j�̃e�[�u���̏ꏊ�i0-18)
};

// �g���[�i�[VIEWTYPE�Ɛ��ʂ�����������o��
extern int  UnionView_GetTrainerInfo( int view_type, int sex, int info );

extern int UnionView_GetTrainerTypeIndex( u32 id, int sex, u32 select );
extern int  UnionView_GetTrainerType( u32 id, int sex, u32 select );
extern void UnionView_SetUpTrainerTypeSelect( u32 id, int sex, WORDSET *wordset );
extern int UnionView_GetCharaNo( int sex, int view_type );
extern int UnionView_GetObjCode(int view_index);
extern int UnionView_GetTrType(int view_index);
extern u16 *UnionView_GetPalNo( u16 * table, int sex, int view_type );
extern u16 *UnionView_PalleteTableAlloc( int heapID );

//GS��poke_tool����ڐA

#define	TR_PARA_BACK		(0)		///<�w��
#define	TR_PARA_FRONT		(2)		///<����

//TrCLACTGraDataGet�Ŏg�p�����\����
typedef struct{
	int	arcID;		//�g���[�i�[�O���t�B�b�N�f�[�^��ArcID
	int	ncgrID;		//�g���[�i�[�O���t�B�b�N�f�[�^�̃L����ID
	int	nclrID;		//�g���[�i�[�O���t�B�b�N�f�[�^�̃p���b�gID
	int	ncerID;		//�g���[�i�[�O���t�B�b�N�f�[�^�̃Z��ID
	int	nanrID;		//�g���[�i�[�O���t�B�b�N�f�[�^�̃Z���A�j��ID
	int	ncbrID;		//�g���[�i�[�O���t�B�b�N�f�[�^�̃L����ID�i�\�t�g�X�v���C�g�p�j
}TR_CLACT_GRA;

extern void	TrCLACTGraDataGet(int trtype,int dir,TR_CLACT_GRA *tcg);
extern void	TrCLACTGraDataGetEx(int trtype,int dir,BOOL non_throw,TR_CLACT_GRA *tcg);

