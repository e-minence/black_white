//============================================================================
/**
 *  @file   manual_common.h
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *
 *  ���W���[�����FMANUAL_COMMON
 */
//============================================================================
#pragma once


// �C���N���[�h
#include <gflib.h>

#include "gamesystem/gamedata_def.h"

#include "manual_data.h"
#include "manual_touchbar.h"


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
typedef struct
{
  // MANUAL���Ă񂾏ꏊ����̎؂蕨
  GAMEDATA*                   gamedata;
 
  // MANUAL�Ő�����������
  HEAPID                      heap_id;
  
  // �O���t�B�b�N�A�t�H���g�Ȃ�
  MANUAL_GRAPHIC_WORK*        graphic;
  GFL_FONT*                   font;
  PRINT_QUE*                  print_que_title;  // �^�C�g����print_que
  PRINT_QUE*                  print_que_main;   // ���X�g��{����print_que
  PRINT_QUE*                  print_que_tb;     // �^�b�`�o�[��print_que

  // ���b�Z�[�W
  GFL_MSGDATA*                msgdata_system;  // �V�X�e���̃��b�Z�[�W
  GFL_MSGDATA*                msgdata_main;    // �J�e�S����^�C�g���A�{���̃��b�Z�[�W

  // �t�@�C���n���h��
  ARCHANDLE*                  handle_system;   // �V�X�e���̃t�@�C���n���h��
  ARCHANDLE*                  handle_explain;  // ������ʂ̃t�@�C���n���h��

  // �}�j���A���̃f�[�^
  MANUAL_DATA_WORK*           data_wk;

  // ���ʂɕ\�����Ă���_�C���N�g�J���[�摜��ID
  u16                         bg_m_dcbmp_id;  // MANUAL_BG_M_DCBMP_NO_IMAGE�̂Ƃ��摜�t�@�C����\�����Ă��炸�A�^����

  // �}�j���A���^�b�`�o�[
  MANUAL_TOUCHBAR_WORK*       mtb_wk;  // MANUAL_TOUCHBAR_Main�͊e��ʂŌĂ�ŉ�����
}
MANUAL_COMMON_WORK;


//=============================================================================
/**
*  �֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// ����������
extern  MANUAL_COMMON_WORK*  MANUAL_COMMON_Init(
    GAMEDATA*  gamedata,
    HEAPID     heap_id
);
// �I������
extern  void  MANUAL_COMMON_Exit(
    MANUAL_COMMON_WORK*  work
);
// �又��
extern  void  MANUAL_COMMON_Main(
    MANUAL_COMMON_WORK*  work
);

// ���ʂɃ_�C���N�g�J���[�摜��\������
extern  void  MANUAL_COMMON_DrawBgMDcbmp(
    MANUAL_COMMON_WORK*  work,
    u16                  bg_m_dcbmp_id
);

