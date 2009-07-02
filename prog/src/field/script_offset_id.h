//======================================================================
/**
 * @file  scr_offset.h
 * @bfief  �X�N���v�gID�̃I�t�Z�b�g��`
 * @author  Satoshi Nohara
 * @date  06.05.26
 */
//======================================================================
#ifndef SCR_OFFSET_H
#define SCR_OFFSET_H

//======================================================================
//  �X�N���v�gID�I�t�Z�b�g
//
//  ��`�ǉ����[��
//  #define ID_"�X�N���v�g�t�@�C����"_OFFSET
//  #define ID_"�X�N���v�g�t�@�C����"_OFFSET_END
//
//  script.c�̓ǂݍ��ރX�N���v�g�t�@�C���A
//  ���b�Z�[�W�t�@�C����Ή������ĉ������B
//======================================================================
//�_�~�[�X�N���v�gID
//#define SCRID_NULL                 (0)

//�J�n�X�N���v�gID�̃I�t�Z�b�g
#define ID_START_SCR_OFFSET          (1)

//���ʃX�N���v�gID�̃I�t�Z�b�g
#define ID_COMMON_SCR_OFFSET         (2000)
//���ʃX�N���v�gID�̃I�t�Z�b�g�I��
#define ID_COMMON_SCR_OFFSET_END     (2499)

//�g���[�i�[�X�N���v�gID�̃I�t�Z�b�g
#define ID_TRAINER_OFFSET            (3000)
//�g���[�i�[�X�N���v�gID�̃I�t�Z�b�g�I��
#define ID_TRAINER_OFFSET_END        (4999)

//2vs2�g���[�i�[�X�N���v�gID�̃I�t�Z�b�g
#define ID_TRAINER_2VS2_OFFSET       (5000)
//2vs2�g���[�i�[�X�N���v�gID�̃I�t�Z�b�g�I��
#define ID_TRAINER_2VS2_OFFSET_END   (6999)

//�B���A�C�e���X�N���v�gID�̃I�t�Z�b�g
#define ID_HIDE_ITEM_OFFSET          (8000)
//�B���A�C�e���X�N���v�gID�̃I�t�Z�b�g�I��
#define ID_HIDE_ITEM_OFFSET_END      (8799)

#endif  /* SCR_OFFSET_H */
