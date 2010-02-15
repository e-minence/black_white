/////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �A���P�[�g�̉񓚃C���f�b�N�X�Q�Ɗ֐�
 * @file   questionnaire_index.h
 * @author obata
 * @date   2010.02.15
 */
/////////////////////////////////////////////////////////////////////////////
#pragma once


// �񓚑I�����̐�
extern u8 QUESTIONNAIRE_INDEX_GetAnswerNum( u8 questionID );

// �񓚂�\������̂ɕK�v�ȃr�b�g��
extern u8 QUESTIONNAIRE_INDEX_GetBitCount( u8 questionID );

// �Z�[�u�f�[�^ �I�t�Z�b�g ( �񓚑I�����̐� )
extern u8 QUESTIONNAIRE_INDEX_GetAnswerNumOffset( u8 questionID );

// �Z�[�u�f�[�^ �I�t�Z�b�g ( �r�b�g�� )
extern u8 QUESTIONNAIRE_INDEX_GetBitCountOffset( u8 questionID );
