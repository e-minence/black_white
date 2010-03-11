//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		livebattlematch_flow.h
 *	@brief  ���C�u���t���[
 *	@author	Toru=Nagihashi
 *	@data		2010.03.06
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
  LIVEBATTLEMATCH_FLOW_RET_CONTINUE,  //�������s��
  LIVEBATTLEMATCH_FLOW_RET_LIVEMENU,  //���C�u���j���[��
  LIVEBATTLEMATCH_FLOW_RET_BATTLE,    //�o�g����
  LIVEBATTLEMATCH_FLOW_RET_REC,       //�^���
  LIVEBATTLEMATCH_FLOW_RET_BTLREC,    //�^��Đ���
} LIVEBATTLEMATCH_FLOW_RET;

//-------------------------------------
///	�J�n���[�h
//=====================================
typedef enum
{
  LIVEBATTLEMATCH_FLOW_MODE_START,  //�ŏ�����J�n
  LIVEBATTLEMATCH_FLOW_MODE_MENU,    //���C�u���C�����j���[����J�n
  LIVEBATTLEMATCH_FLOW_MODE_BTL,    //�o�g���ォ��J�n
  LIVEBATTLEMATCH_FLOW_MODE_REC,    //�^��ォ��J�n

} LIVEBATTLEMATCH_FLOW_MODE;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�n���h��
//=====================================
typedef struct _LIVEBATTLEMATCH_FLOW_WORK LIVEBATTLEMATCH_FLOW_WORK;


//-------------------------------------
///	����
//=====================================
typedef struct 
{
  LIVEBATTLEMATCH_FLOW_MODE     mode;         //[in ]�J�n�̃t���[
  WBM_TEXT_WORK                 *p_text;      //[in ]�e�L�X�g�\�����W���[��
  GFL_FONT                      *p_font;      //[in ]�t�H���g
  PRINT_QUE                     *p_que;       //[in ]�v�����g�L���[
  WIFIBATTLEMATCH_VIEW_RESOURCE *p_view;      //[in ]���\�[�X���ʃ��W���[��
  WIFIBATTLEMATCH_GRAPHIC_WORK  *p_graphic;   //[in ]�`�拤�ʃ��W���[��
  GAMEDATA                      *p_gamedata;  //[in ]�Q�[���f�[�^

   //�ȉ��A[in ]�풓�f�[�^�u����
  WIFIBATTLEMATCH_ENEMYDATA     *p_player_data;
  WIFIBATTLEMATCH_ENEMYDATA     *p_enemy_data; 

} LIVEBATTLEMATCH_FLOW_PARAM;

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
extern LIVEBATTLEMATCH_FLOW_WORK *LIVEBATTLEMATCH_FLOW_Init( const LIVEBATTLEMATCH_FLOW_PARAM *cp_param, HEAPID heapID );
extern void LIVEBATTLEMATCH_FLOW_Exit( LIVEBATTLEMATCH_FLOW_WORK *p_wk );
extern void LIVEBATTLEMATCH_FLOW_Main( LIVEBATTLEMATCH_FLOW_WORK *p_wk );

extern LIVEBATTLEMATCH_FLOW_RET LIVEBATTLEMATCH_FLOW_IsEnd( const LIVEBATTLEMATCH_FLOW_WORK *cp_wk );
