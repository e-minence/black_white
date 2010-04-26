//============================================================================
/**
 *  @file   zukan_info.h
 *  @brief  �}�ӏ��
 *  @author Koji Kawada
 *  @data   2009.12.03
 *  @note   infowin.h, touchbar.h, ui_template.h���Q�l�ɂ��č쐬���܂����B
 *
 *  ���W���[�����FZUKAN_INFO
 */
//============================================================================
#pragma once

// lib
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

// system
#include "gamesystem/gamesystem.h"
#include "print/printsys.h"
#include "print/gf_font.h"


// �I�[�o�[���C
FS_EXTERN_OVERLAY(zukan_info);


//=============================================================================
/**
 *  �萔��`
 */
//=============================================================================
//-------------------------------------
/// �\����
//=====================================
typedef enum
{
  ZUKAN_INFO_DISP_M,     ///< ���C��
  ZUKAN_INFO_DISP_S,     ///< �T�u
}
ZUKAN_INFO_DISP;

//-------------------------------------
/// �N�����@
//=====================================
typedef enum
{
  ZUKAN_INFO_LAUNCH_TOROKU,       ///< �}�ӓo�^
  ZUKAN_INFO_LAUNCH_NICKNAME,     ///< �j�b�N�l�[�������m�F
  ZUKAN_INFO_LAUNCH_LIST,         ///< �}�ӂ̃��X�g����
}
ZUKAN_INFO_LAUNCH;

//-------------------------------------
/// ����
//=====================================
typedef enum
{
  ZUKAN_INFO_LANG_E,
  ZUKAN_INFO_LANG_FRA,
  ZUKAN_INFO_LANG_GER,
  ZUKAN_INFO_LANG_ITA,
  ZUKAN_INFO_LANG_SPA,
  ZUKAN_INFO_LANG_KOR,
  ZUKAN_INFO_LANG_MAX,

  ZUKAN_INFO_LANG_NONE   = ZUKAN_INFO_LANG_MAX,  // ���{��̂Ђ炪�Ȃ�����
}
ZUKAN_INFO_LANG;


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
typedef struct _ZUKAN_INFO_WORK ZUKAN_INFO_WORK;

//=============================================================================
/**
 *  �֐��錾
 */
//=============================================================================
//-------------------------------------
/// ����������
//=====================================
extern ZUKAN_INFO_WORK* ZUKAN_INFO_Init(
                            HEAPID                  a_heap_id,
                            const POKEMON_PARAM*    a_pp,
                            BOOL                    a_zenkoku_flag,
                            BOOL                    a_get_flag,
                            ZUKAN_INFO_LAUNCH       a_launch,
                            ZUKAN_INFO_DISP         a_disp,
                            u8                      a_bg_priority,
                            GFL_CLUNIT*             a_clunit,
                            GFL_FONT*               a_font,
                            PRINT_QUE*              a_print_que );  // �Ǝ���print_que�𐶐����邱�Ƃɂ����̂Ŏg�p���Ȃ�

extern ZUKAN_INFO_WORK* ZUKAN_INFO_InitFromMonsno(
                            HEAPID                 a_heap_id,
                            u16                    a_monsno,
                            u16                    a_formno,
                            u16                    a_sex,
                            u16                    a_rare,
                            u32                    a_personal_rnd,
                            BOOL                   a_zenkoku_flag,
                            BOOL                   a_get_flag,
                            ZUKAN_INFO_LAUNCH      a_launch,
                            ZUKAN_INFO_DISP        a_disp,
                            u8                     a_bg_priority,
                            GFL_CLUNIT*            a_clunit,
                            GFL_FONT*              a_font,
                            PRINT_QUE*             a_print_que );  // �Ǝ���print_que�𐶐����邱�Ƃɂ����̂Ŏg�p���Ȃ�

//-------------------------------------
/// �I������
//=====================================
extern void ZUKAN_INFO_Exit( ZUKAN_INFO_WORK* work );

//-------------------------------------
/// �又��
//=====================================
extern void ZUKAN_INFO_Main( ZUKAN_INFO_WORK* work );

//-------------------------------------
/// �|�P����2D�ȊO�̂��̂�j������
//=====================================
extern void ZUKAN_INFO_DeleteOthers( ZUKAN_INFO_WORK* work );

//-------------------------------------
/// �|�P����2D�̈ړ��J�n���t���O�𗧂Ă�
//=====================================
extern void ZUKAN_INFO_Move( ZUKAN_INFO_WORK* work );

//-------------------------------------
/// �|�P�����̖����t���O�𗧂Ă�
//=====================================
extern void ZUKAN_INFO_Cry( ZUKAN_INFO_WORK* work );

//-------------------------------------
/// �|�P����2D���ړ������ۂ�
//=====================================
extern BOOL ZUKAN_INFO_IsMove( ZUKAN_INFO_WORK* work );

//-------------------------------------
/// �|�P����2D�������ł����Ƃ��Ă��邩�ۂ�
//=====================================
extern BOOL ZUKAN_INFO_IsCenterStill( ZUKAN_INFO_WORK* work );

//-------------------------------------
/// �������J�n������
//=====================================
extern void ZUKAN_INFO_Start( ZUKAN_INFO_WORK* work );

//-------------------------------------
/// �|�P������ύX����
//=====================================
extern void ZUKAN_INFO_ChangePoke(
                ZUKAN_INFO_WORK* work,
                u16              monsno,
                u16              formno,
                u16              sex,
                u16              rare,
                u32              personal_rnd,
                BOOL             get_flag );

//-------------------------------------
/// �����ύX����
//=====================================
extern void ZUKAN_INFO_ChangeLang(
                ZUKAN_INFO_WORK* work,
                ZUKAN_INFO_LANG lang );

//-------------------------------------
/// �|�P�����ƌ����ύX����
//=====================================
extern void ZUKAN_INFO_ChangePokeAndLang(
                ZUKAN_INFO_WORK* work,
                u16              monsno,
                u16              formno,
                u16              sex,
                u16              rare,
                u32              personal_rnd,
                BOOL             get_flag,
                ZUKAN_INFO_LANG  lang );

//-------------------------------------
/// �����\�����Ȃ��Ƃ��p�̔w�i�݂̂�\������
//=====================================
extern void ZUKAN_INFO_DisplayBackNone( ZUKAN_INFO_WORK* work );
//-------------------------------------
/// �ʏ�̕\���ɂ���
//=====================================
extern void ZUKAN_INFO_DisplayNormal( ZUKAN_INFO_WORK* work );

