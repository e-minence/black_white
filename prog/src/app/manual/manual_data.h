//============================================================================
/**
 *  @file   manual_data.h
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *
 *  ���W���[�����FMANUAL_DATA
 */
//============================================================================
#pragma once


// �C���N���[�h
#include <gflib.h>

#include "gamesystem/gamedata_def.h"


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
typedef struct  _MANUAL_DATA_WORK  MANUAL_DATA_WORK;


//=============================================================================
/**
*  �֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// Load����
extern  MANUAL_DATA_WORK*  MANUAL_DATA_Load( ARCHANDLE* handle, HEAPID heap_id );
// Unload����
extern  void               MANUAL_DATA_Unload( MANUAL_DATA_WORK* work );

// �^�C�g���t�@�C��
extern  u16  MANUAL_DATA_TitleGetTitleGmmId( MANUAL_DATA_WORK* work, u16 title_idx );
                                          // title_idx�͑S�^�C�g���ɂ�����o�Ȕԍ�
                                          // title_idx�^�C�g����gmm��ID��߂�l�Ƃ��ĕԂ�
extern  u16  MANUAL_DATA_TitleGetOpenFlag( MANUAL_DATA_WORK* work, u16 title_idx );
extern  u16  MANUAL_DATA_TitleGetReadFlag( MANUAL_DATA_WORK* work, u16 title_idx );
extern  u16  MANUAL_DATA_TitleGetPageNum( MANUAL_DATA_WORK* work, u16 title_idx );
extern  u16  MANUAL_DATA_TitleGetPageGmmId( MANUAL_DATA_WORK* work, u16 title_idx, u16 page_order );
                                          // title_idx�͑S�^�C�g���ɂ�����o�Ȕԍ�
                                          // title_idx�^�C�g�����ɂ�����page_order�Ԗڂ̃y�[�W�́Agmm��ID��߂�l�Ƃ��ĕԂ�
extern  u16  MANUAL_DATA_TitleGetPageImageId( MANUAL_DATA_WORK* work, u16 title_idx, u16 page_order );
                                          // title_idx�͑S�^�C�g���ɂ�����o�Ȕԍ�
                                          // title_idx�^�C�g�����ɂ�����page_order�Ԗڂ̃y�[�W�́A�摜��ID��߂�l�Ƃ��ĕԂ�

// �^�C�g���J�n�ꏊ�t�@�C��
extern  u16  MANUAL_DATA_TitleRefGetTotalTitleNum( MANUAL_DATA_WORK* work );

// �J�e�S���t�@�C��
extern  u16  MANUAL_DATA_CateGetCateGmmId( MANUAL_DATA_WORK* work, u16 cate_idx );
                                          // cate_idx�͑S�J�e�S���ɂ�����o�Ȕԍ�
                                          // cate_idx�J�e�S����gmm��ID��߂�l�Ƃ��ĕԂ�
extern  u16  MANUAL_DATA_CateGetTitleNum( MANUAL_DATA_WORK* work, u16 cate_idx );
                                          // cate_idx�͑S�J�e�S���ɂ�����o�Ȕԍ�
                                          // cate_idx�J�e�S�����ɂ�����^�C�g���̌���߂�l�Ƃ��ĕԂ�
extern  u16  MANUAL_DATA_CateGetTitleIdx( MANUAL_DATA_WORK* work, u16 cate_idx, u16 title_order );
                                          // cate_idx�͑S�J�e�S���ɂ�����o�Ȕԍ�
                                          // cate_idx�J�e�S�����ɂ�����title_order�Ԗڂ̃^�C�g���́A�S�^�C�g���ɂ�����o�Ȕԍ���߂�l�Ƃ��ĕԂ�

// �J�e�S���J�n�ꏊ�t�@�C��
extern  u16  MANUAL_DATA_CateRefGetTotalCateNum( MANUAL_DATA_WORK* work );

// �I�[�v���t���O
extern  BOOL MANUAL_DATA_OpenFlagIsOpen( MANUAL_DATA_WORK* work, u16 open_flag, GAMEDATA* gamedata );

// ���[�h�t���O
extern  BOOL MANUAL_DATA_ReadFlagIsRead( MANUAL_DATA_WORK* work, u16 read_flag, GAMEDATA* gamedata );
extern  void MANUAL_DATA_ReadFlagSetRead( MANUAL_DATA_WORK* work, u16 read_flag, GAMEDATA* gamedata );

// �摜��ID
extern  BOOL MANUAL_DATA_ImageIdIsValid( MANUAL_DATA_WORK* work, u16 image_id );  // �摜��ID���L��(�摜����)������(�摜�Ȃ�)����Ԃ�(�L���̂Ƃ�TRUE)
extern  u16  MANUAL_DATA_ImageIdGetNoImage( MANUAL_DATA_WORK* work );  // �摜�Ȃ��̂Ƃ��ɕ\������摜��ID�𓾂�

