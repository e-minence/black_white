//============================================================================
/**
 *
 *	@file		bag.h
 *	@brief  �o�b�O
 *	@author	hosaka genya
 *	@data		2009.10.21 �o�b�O�̋N���p�����[�^��؂�o��
 *
 */
//============================================================================
#pragma once

#include <gflib.h>

#include "field/fieldmap_proc.h"

#include "system/main.h"
#include "gamesystem/game_data.h"
#include "savedata/config.h"
#include "app/itemuse.h"

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//--------------------------------------------------------------
///	�o�b�O�N�����[�h
//==============================================================
typedef enum {
  BAG_MODE_FIELD,
  BAG_MODE_UNION,
  BAG_MODE_POKELIST,
  BAG_MODE_COLOSSEUM,
  BAG_MODE_N_PLANTER,
  BAG_MODE_SELL,       // �u����v�V�[�P���X
} BAG_MODE;

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//--------------------------------------------------------------
///	�o�b�O�N���p�����[�^
//==============================================================
typedef struct {
  // [IN] ���������ɊO������󂯓n����郁���o
//  GMEVENT           * p_event;
//  FIELDMAP_WORK     * p_fieldmap;
  GAMEDATA          * p_gamedata;   ///< Y�{�^���o�^�Ɏg�p
  CONFIG            * p_config;    
  MYSTATUS          * p_mystatus;
  BAG_CURSOR        * p_bagcursor;
  MYITEM_PTR          p_myitem;
  ITEMCHECK_WORK      icwk;         ///< �A�C�e���`�F�b�N���[�N FMENU_EVENT_WORK����R�s�[�B�u�����v���o�Ȃ���Ԃł���Εs�K�v
  BAG_MODE            mode;         ///< �o�b�O�Ăяo�����[�h
  BOOL                cycle_flg;    ///< ���]�Ԃ��ǂ����H
  // [OUT] �O���ɕԂ������o
  enum BAG_NEXTPROC_ENUM   next_proc; ///< �o�b�O���甲����Ƃ��̓���
  int ret_item; ///< �I�������A�C�e��
  // �Ȃ�
} BAG_PARAM;

//=============================================================================
/**
 *								EXTERN�錾
 */
//=============================================================================

// PROC��`
extern const GFL_PROC_DATA ItemMenuProcData;
FS_EXTERN_OVERLAY( bag );

//-----------------------------------------------------------------------------
/**
 *	@brief  �o�b�O�p�����[�^����
 *
 *	@param	GAMEDATA* gmData  �Q�[���f�[�^
 *	@param	ITEMCHECK_WORK* icwk �A�C�e���`�F�b�N���[�N
 *	@param	mode �o�b�O�N�����[�h
 *
 *	@retval BAG_PARAM* �o�b�O�p�����[�^(ALLOC�ς�)
 */
//-----------------------------------------------------------------------------
extern BAG_PARAM* BAG_CreateParam( GAMEDATA* gmData, const ITEMCHECK_WORK* icwk, BAG_MODE mode );





