///////////////////////////////////////////////////////////////////
/**
 * @brief  �A���P�[�g�񓚃C���f�b�N�X
 * @file   questionnaire_index.h
 * @author obata
 * @date   2010.02.15
 */
///////////////////////////////////////////////////////////////////
#pragma once


//=================================================================
// ���񓚃��[�N���� ����̎���ɑ΂���񓚂ɃA�N�Z�X���邽�߂̏��
//=================================================================
typedef struct
{
  u8 answerNum;        // �񓚑I�����̐�
  u8 bitCount;         // �񓚂�\������̂ɕK�v�ȃr�b�g��
  u8 answerNumOffset;  // ���ڐ��C���f�b�N�X
  u8 bitCountOffset;   // bit���C���f�b�N�X

} QUESTIONNAIRE_INDEX;


//=================================================================
// �����擾�֐�
//=================================================================

// �񓚑I�����̐�
extern u8 QUESTIONNAIRE_INDEX_GetAnswerNum( u8 questionID );

// �񓚂�\������̂ɕK�v�ȃr�b�g��
extern u8 QUESTIONNAIRE_INDEX_GetBitCount( u8 questionID );

// �Z�[�u�f�[�^ �I�t�Z�b�g ( �񓚑I�����̐� )
extern u8 QUESTIONNAIRE_INDEX_GetAnswerNumOffset( u8 questionID );

// �Z�[�u�f�[�^ �I�t�Z�b�g ( �r�b�g�� )
extern u8 QUESTIONNAIRE_INDEX_GetBitCountOffset( u8 questionID );


//=================================================================
// ���c�[���֐�
//=================================================================

// �w�肵���񓚂��������鎿���ID���擾����
extern u8 GetQuestionID_byAnswerID( u8 answerID );

// �w�肵���񓚂�, �������鎿����̉��Ԗڂ̉񓚂Ȃ̂����擾����
extern u8 GetAnswerIndex_atQuestion( u8 answerID );
