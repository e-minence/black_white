//======================================================================
/**
 * @file	script_hideitem.h
 * @brief	�X�N���v�g����F�B���A�C�e���֘A
 * @date	09.09.13
 * 
 * �B���A�C�e���֘A
 */
//======================================================================
#pragma once

#define HIDE_LIST_SX				(7)		//�����͈�
#define HIDE_LIST_SZ				(7)		//�����͈�(���g�p)
#define HIDE_LIST_SZ_2				(6)		//�����͈�(���g�p)
#define HIDE_LIST_TOP				(7)		//�����͈�(��l�������ʏ�)
#define HIDE_LIST_BOTTOM			(6)		//�����͈�(��l�������ʉ�)
#define HIDE_LIST_RESPONSE_NONE		(0xff)	//�I���R�[�h

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A�B���A�C�e���t���O�i���o�[���擾
 *
 * @param   scr_id		�X�N���v�gID
 *
 * @retval  "�t���O�i���o�["
 */
//--------------------------------------------------------------
//extern u16 GetHideItemFlagNoByScriptId( u16 scr_id );
extern u16 SCRIPT_GetHideItemFlagNoByScriptID( u16 scr_id );

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A�B���A�C�e���t���O�C���f�b�N�X���擾
 *
 * @param   scr_id		�X�N���v�gID
 *
 * @retval  "�t���O�C���f�b�N�X"
 */
//--------------------------------------------------------------
extern u16 GetHideItemFlagIndexByScriptId( u16 scr_id );

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A�B���A�C�e���̔������擾
 *
 * @param   scr_id		�X�N���v�gID
 *
 * @retval  "����"
 *
 * �B���A�C�e���̃X�N���v�g����������ĂԂ悤�ɂ���I
 */
//--------------------------------------------------------------
extern u8 GetHideItemResponseByScriptId( u16 scr_id );

//--------------------------------------------------------------
/**
 * ��ʓ��ɂ���B���A�C�e�����������Ċm�ۂ������X�g�ɓo�^
 *
 * @param   heapid		�q�[�vID
 *
 * @retval  "���X�g�̃A�h���X"
 *
 * ���������Y�ꂸ�ɁI
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
extern HIDE_ITEM_LIST * HideItem_CreateList( FLDCOMMON_WORK * fsys, int heapid );
#endif

//--------------------------------------------------------------
/**
 * 0���ŉB���t���O����������
 *
 *
 * @retval  none
 */
//--------------------------------------------------------------
//extern void HideItemFlagOneDayClear( FLDCOMMON_WORK* fsys );

