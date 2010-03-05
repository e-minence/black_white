//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		battle_championship_flow.h
 *	@brief  ���C�����j���[  �t���[
 *	@author	Toru=Nagihashi
 *	@date		2010.03.04
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
//���C�u����
#include <gflib.h>
//WIFIBATTLEMATCH�̃��W���[��
#include "net_app/wifi_match/wifibattlematch_util.h"
#include "net_app/wifi_match/wifibattlematch_graphic.h"
#include "net_app/wifi_match/wifibattlematch_view.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�I�����[�h
//=====================================
typedef enum
{
  BATTLE_CHAMPIONSHIP_FLOW_RET_CONTINUE,  //�������s��
  BATTLE_CHAMPIONSHIP_FLOW_RET_TITLE,     //�I�����ă^�C�g����
  BATTLE_CHAMPIONSHIP_FLOW_RET_WIFICUP,   //�I������WIFI����
  BATTLE_CHAMPIONSHIP_FLOW_RET_LIVECUP,   //���C�u����
} BATTLE_CHAMPIONSHIP_FLOW_RET;

//-------------------------------------
///	�J�n���[�h
//=====================================
typedef enum
{
  BATTLE_CHAMPIONSHIP_FLOW_MODE_MAINMENU,   //���C�����j���[����J�n
  BATTLE_CHAMPIONSHIP_FLOW_MODE_WIFIMENU,   //WIFI���j���[����J�n
  BATTLE_CHAMPIONSHIP_FLOW_MODE_LIVEMENU,   //LIVE���j���[����J�n
} BATTLE_CHAMPIONSHIP_FLOW_MODE;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�n���h��
//=====================================
typedef struct _BATTLE_CHAMPIONSHIP_FLOW_WORK BATTLE_CHAMPIONSHIP_FLOW_WORK;

//-------------------------------------
///	����
//=====================================
typedef struct 
{
  BATTLE_CHAMPIONSHIP_FLOW_MODE mode;         //[in ]�J�n�̃t���[
  WBM_TEXT_WORK                 *p_text;      //[in ]�e�L�X�g�\�����W���[��
  GFL_FONT                      *p_font;      //[in ]�t�H���g
  GFL_MSGDATA                   *p_msg;       //[in ]���b�Z�[�W
  PRINT_QUE                     *p_que;       //[in ]�v�����g�L���[
  WIFIBATTLEMATCH_VIEW_RESOURCE *p_view;      //[in ]���\�[�X���ʃ��W���[��
  WIFIBATTLEMATCH_GRAPHIC_WORK  *p_graphic;   //[in ]�`�拤�ʃ��W���[��
  GAMEDATA                      *p_gamedata;  //[in ]�Q�[���f�[�^
} BATTLE_CHAMPIONSHIP_FLOW_PARAM;

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
extern BATTLE_CHAMPIONSHIP_FLOW_WORK *BATTLE_CHAMPIONSHIP_FLOW_Init( const BATTLE_CHAMPIONSHIP_FLOW_PARAM *cp_param, HEAPID heapID );
extern void BATTLE_CHAMPIONSHIP_FLOW_Main( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk );
extern void BATTLE_CHAMPIONSHIP_FLOW_Exit( BATTLE_CHAMPIONSHIP_FLOW_WORK *p_wk );

extern BATTLE_CHAMPIONSHIP_FLOW_RET BATTLE_CHAMPIONSHIP_FLOW_IsEnd( const BATTLE_CHAMPIONSHIP_FLOW_WORK *cp_wk );
