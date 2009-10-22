//======================================================================
/**
 * @file	field_player_code.h
 * @date	2008.9.29
 * @brief	�t�B�[���h�v���C���[�@�X�N���v�g������Q�Ƃ���l
 * @note �A�Z���u���Q�Ƃ��L�邽��define�ȊO�͋֎~�B
 */
//======================================================================
#pragma once

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
/// FIELD_PLAYER_REQBIT
//--------------------------------------------------------------
///���@���Q�����s�ɕύX
#define FIELD_PLAYER_REQBIT_NORMAL (1<<0)
///���@�����]�ԂɕύX
#define FIELD_PLAYER_REQBIT_CYCLE  (1<<1)
///���@��g����ԂɕύX
#define FIELD_PLAYER_REQBIT_SWIM   (1<<2)
///����`�Ԃɂ��킹���\���ɂ���
#define FIELD_PLAYER_REQBIT_MOVE_FORM_TO_DRAW_FORM (1<<3)
///�A�C�e���Q�b�g���@�\���ɂ���
#define FIELD_PLAYER_REQBIT_ITEMGET (1<<4)
///���|�[�g���@�\���ɂ���
#define FIELD_PLAYER_REQBIT_REPORT (1<<5)
///PC�a�����@�\���ɂ���
#define FIELD_PLAYER_REQBIT_PC_AZUKE (1<<6)
///�r�b�g�ő�
#define FIELD_PLAYER_REQBIT_MAX (7)
