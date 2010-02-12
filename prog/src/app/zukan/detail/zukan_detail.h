//============================================================================
/**
 *  @file   zukan_detail.h
 *  @brief  �}�ӏڍ׉�ʋ���
 *  @author Koji Kawada
 *  @data   2010.02.02
 *  @note   
 *
 *  ���W���[�����FZUKAN_DETAIL
 */
//============================================================================
#pragma once


// �C���N���[�h
#include <gflib.h>

#include "gamesystem/gamedata_def.h"


// �I�[�o�[���C
FS_EXTERN_OVERLAY(zukan_detail);


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
typedef enum
{
  ZUKAN_DETAIL_TYPE_NONE,
  ZUKAN_DETAIL_TYPE_INFO,
  ZUKAN_DETAIL_TYPE_MAP,
  ZUKAN_DETAIL_TYPE_VOICE,
  ZUKAN_DETAIL_TYPE_FORM,
}
ZUKAN_DETAIL_TYPE;

typedef enum
{
  ZUKAN_DETAIL_RET_CLOSE,    // �~�{�^��
  ZUKAN_DETAIL_RET_RETURN,   // �����{�^��
}
ZUKAN_DETAIL_RETURN;


//=============================================================================
/**
 *  PROC
 */
//=============================================================================
extern const GFL_PROC_DATA    ZUKAN_DETAIL_ProcData;


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// PROC �p�����[�^
//=====================================
typedef struct _ZUKAN_DETAIL_PARAM
{
  GAMEDATA*            gamedata;           ///< [in,out]  GAMEDATA
  ZUKAN_DETAIL_TYPE    type;               ///< [in]      �\������ڍ׉��
  u16*                 list;  // �|�P������monsno�̃��X�g���ȁH
  u16                  num;   // list�̗v�f��
  u16                  no;    // list�̉��Ԗڂɂ���|�P������\�����邩
  ZUKAN_DETAIL_RETURN  ret;                ///< [out]     ���̎w��
}
ZUKAN_DETAIL_PARAM;


//=============================================================================
/**
*  �֐��̃v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
/// �p�����[�^����
//=====================================
extern ZUKAN_DETAIL_PARAM*  ZUKAN_DETAIL_AllocParam(
                                HEAPID               heap_id,
                                GAMEDATA*            gamedata,
                                ZUKAN_DETAIL_TYPE    type,
                                u16*                 list,
                                u16                  num,
                                u16                  no );

//-------------------------------------
/// �p�����[�^���
//=====================================
extern void             ZUKAN_DETAIL_FreeParam(
                            ZUKAN_DETAIL_PARAM*  param );

//-------------------------------------
/// �p�����[�^��ݒ肷��
//=====================================
extern void             ZUKAN_DETAIL_InitParam(
                            ZUKAN_DETAIL_PARAM*  param,
                            GAMEDATA*            gamedata,
                            ZUKAN_DETAIL_TYPE    type,
                            u16*                 list,
                            u16                  num,
                            u16                  no );

