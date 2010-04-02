//============================================================================
/**
 *  @file   shinka_demo.h
 *  @brief  �i���f��
 *  @author Koji Kawada
 *  @data   2010.01.13
 *  @note   
 *
 *  ���W���[�����FSHINKADEMO
 */
//============================================================================
#pragma once

#include "poke_tool/pokeparty.h"
#include "gamesystem/gamedata_def.h"

// �I�[�o�[���C
FS_EXTERN_OVERLAY(shinka_demo);


//=============================================================================
/**
 *  PROC
 */
//=============================================================================
extern  const GFL_PROC_DATA   ShinkaDemoProcData;


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// PROC �p�����[�^
//=====================================
typedef struct _SHINKA_DEMO_PARAM
{
  GAMEDATA*         gamedata;           ///< [in,out]  GAMEDATA
  const POKEPARTY*  ppt;                ///< [in]      POKEPARTY�\����
  u16               after_mons_no;      ///< [in]      �i����̃|�P�����i���o�[
  u8                shinka_pos;         ///< [in]      �i������|�P������POKEPARTY���̃C���f�b�N�X
  u8                shinka_cond;        ///< [in]      �i�������i�k�P�j���`�F�b�N�Ɏg�p�j
  BOOL              b_field;            ///< [in]      �t�B�[���h����Ăяo���Ƃ���TRUE�A�o�g����ɌĂяo���Ƃ���FALSE
  BOOL              b_enable_cancel;    ///< [in]      �i���L�����Z���ł���Ƃ���TRUE�A�ł��Ȃ��Ƃ���FALSE
}
SHINKA_DEMO_PARAM;


//=============================================================================
/**
*  �֐��̃v���g�^�C�v�錾
*/
//=============================================================================

#if 0

//-------------------------------------
/// �i���f���p�p�����[�^�\���̐���
//=====================================
extern  SHINKA_DEMO_PARAM*  SHINKADEMO_AllocParam(
                                HEAPID heapID, GAMEDATA* gamedata, const POKEPARTY* ppt,
                                u16 after_mons_no, u8 pos, u8 cond, BOOL b_field );

//-------------------------------------
/// �i���f���p�p�����[�^���[�N���
//=====================================
extern  void                SHINKADEMO_FreeParam( SHINKA_DEMO_PARAM* sdp );

//-------------------------------------
/// �i���f���p�p�����[�^��ݒ肷��
//=====================================
extern  void                SHINKADEMO_InitParam(
                                SHINKA_DEMO_PARAM* param,
                                GAMEDATA* gamedata, const POKEPARTY* ppt,
                                u16 after_mons_no, u8 pos, u8 cond, BOOL b_field, BOOL b_enable_cancel );

#endif

