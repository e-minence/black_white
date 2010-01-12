//============================================================================
/**
 *
 *  @file   bag.h
 *  @brief  �o�b�O
 *  @author hosaka genya
 *  @data   2009.10.21 �o�b�O�̋N���p�����[�^��؂�o��
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
 *                �萔��`
 */
//=============================================================================

//--------------------------------------------------------------
/// �o�b�O�N�����[�h
//==============================================================
typedef enum {
  BAG_MODE_FIELD,
  BAG_MODE_UNION,
  BAG_MODE_POKELIST,
  BAG_MODE_COLOSSEUM,
  BAG_MODE_N_PLANTER,
  BAG_MODE_SELL,       // �u����v�V�[�P���X
} BAG_MODE;


// �t�B�[���h�Ŏg�p�E�s�g�p�`�F�b�N�̎��W�Ɏg�p����ID
// ���V�����g�p���铹��������ꍇ�͂���enum��`�ɐV�������O��ǉ�����
//   ���Abag_param.h��BAG_SetItemUseCheck�֐��œ���g�p�E�s�g�p�̏�Ԃ��i�[����K�v����
typedef enum {
  ITEMCHECK_CYCLE_RIDE=0, // ���]�Ԃ̂�
  ITEMCHECK_CYCLE_STOP,   // ���]�Ԃ����
  ITEMCHECK_TOWNMAP,      // �^�E���}�b�v
  ITEMCHECK_WIFINOTE,     // �Ƃ������蒠
  ITEMCHECK_ANANUKENOHIMO,// ���Ȃʂ��̃q��
  ITEMCHECK_AMAIMITU,     // ���܂��~�c
  ITEMCHECK_TURIZAO,      // �肴��
  ITEMCHECK_BATTLE_RECORDER,  // �o�g�����R�[�_�[
  ITEMCHECK_MAIL,         // ���[��
  ITEMCHECK_SPLAY,        // �X�v���[
  ITEMCHECK_MAX,

} ITEMCHECK_ENABLE ;



//=============================================================================
/**
 *                �\���̒�`
 */
//=============================================================================

//--------------------------------------------------------------
/// �o�b�O�N���p�����[�^
//==============================================================
typedef struct {
  // [IN] ���������ɊO������󂯓n����郁���o
//  GMEVENT           * p_event;
//  FIELDMAP_WORK     * p_fieldmap;
  GAMEDATA          * p_gamedata;   ///< Y�{�^���o�^ + �o�b�O���g�p�A�C�e���i�X�v���[�Ȃǁj �Ɏg�p
  CONFIG            * p_config;    
  MYSTATUS          * p_mystatus;
  BAG_CURSOR        * p_bagcursor;
  MYITEM_PTR          p_myitem;
  ITEMCHECK_WORK      icwk;         ///< �A�C�e���`�F�b�N���[�N FMENU_EVENT_WORK����R�s�[�B�u�����v���o�Ȃ���Ԃł���Εs�K�v
  BAG_MODE            mode;         ///< �o�b�O�Ăяo�����[�h
  BOOL                cycle_flg;    ///< ���]�Ԃ��ǂ����H
  u32                 item_enable;  ///< 1bit�P�ʂœ���g���邩�Ƃ�������񂪊i�[����Ă���(itemmenu.h�Q�Ɓj
  // [OUT] �O���ɕԂ������o
  enum BAG_NEXTPROC_ENUM   next_proc; ///< �o�b�O���甲����Ƃ��̓���
  int ret_item; ///< �I�������A�C�e��
  // �Ȃ�
} BAG_PARAM;

//=============================================================================
/**
 *                EXTERN�錾
 */
//=============================================================================

// PROC��`
extern const GFL_PROC_DATA ItemMenuProcData;
FS_EXTERN_OVERLAY( bag );

//-----------------------------------------------------------------------------
/**
 *  @brief  �o�b�O�p�����[�^����
 *
 *  @param  GAMEDATA* gmData  �Q�[���f�[�^
 *  @param  ITEMCHECK_WORK* icwk �A�C�e���`�F�b�N���[�N
 *  @param  mode �o�b�O�N�����[�h
 *  @param  heap_id �q�[�vID
 *
 *  @retval BAG_PARAM* �o�b�O�p�����[�^(ALLOC�ς�)
 */
//-----------------------------------------------------------------------------
extern BAG_PARAM* BAG_CreateParam( GAMEDATA* gmData, const ITEMCHECK_WORK* icwk, BAG_MODE mode, HEAPID heap_id );

//=============================================================================================
/**
 * @brief �w��̓���g���邩���E�s�������i�[����(itemmenu.h����enum��`ITEMCHECK_ENABLE)
 *
 * @param   param   BAG_PARAM�̃|�C���^
 * @param   type    �g������̎�ށiitemmenu.h����enum��`ITEMCHECK_ENABLE����w��j
 * @param   check   0:�g���Ȃ�  1:�g����
 *
 * @retval  none
 */
//=============================================================================================
extern void BAG_SetItemUseCheck( BAG_PARAM* param, ITEMCHECK_ENABLE type, BOOL check );


