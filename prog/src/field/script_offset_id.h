//======================================================================
/**
 * @file	scr_offset.h
 * @bfief	�X�N���v�gID�̃I�t�Z�b�g��`
 * @author	Satoshi Nohara
 * @date	06.05.26
 */
//======================================================================
#ifndef SCR_OFFSET_H
#define SCR_OFFSET_H

//======================================================================
//	�X�N���v�gID�I�t�Z�b�g
//
//	��`�ǉ����[��
//	#define ID_"�X�N���v�g�t�@�C����"_OFFSET
//	#define ID_"�X�N���v�g�t�@�C����"_OFFSET_END
//
//	script.c�̓ǂݍ��ރX�N���v�g�t�@�C���A
//	���b�Z�[�W�t�@�C����Ή������ĉ������B
//======================================================================
//#define	SCRID_NULL				(0)		//�_�~�[�X�N���v�gID
#define	ID_START_SCR_OFFSET			(1)		//�J�n�X�N���v�gID�̃I�t�Z�b�g
#define	ID_COMMON_SCR_OFFSET		(2000)	//���ʃX�N���v�gID�̃I�t�Z�b�g
#define	ID_COMMON_SCR_OFFSET_END	(2499)	//���ʃX�N���v�gID�̃I�t�Z�b�g�I��

#define	ID_HIDE_ITEM_OFFSET (8000) //�B���A�C�e���X�N���v�gID�̃I�t�Z�b�g

#endif	/* SCR_OFFSET_H */
