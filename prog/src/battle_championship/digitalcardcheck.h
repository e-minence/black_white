//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		digitalcardcheck.h
 *	@brief	�f�W�^���I��؊m�F���
 *	@author	Toru=Nagihashi
 *	@data		2010.1.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "gamesystem/game_data.h"
#include "savedata/regulation.h"

//WIFIBATTLEMATCH�̃��W���[��
#include "net_app/wifi_match/wifibattlematch_util.h"
#include "net_app/wifi_match/wifibattlematch_graphic.h"
#include "net_app/wifi_match/wifibattlematch_view.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\����
*/
//=============================================================================
//-------------------------------------
///	�n���h��
//=====================================
typedef struct _DIGITALCARD_CHECK_WORK DIGITALCARD_CHECK_WORK;

//-------------------------------------
///	����
//=====================================
typedef struct 
{
  REGULATION_CARD_TYPE          type;         //[in ]�m�F����I��؂̃^�C�v
  WBM_TEXT_WORK                 *p_text;      //[in ]�e�L�X�g�\�����W���[��
  GFL_FONT                      *p_font;      //[in ]�t�H���g
  GFL_MSGDATA                   *p_msg;       //[in ]���b�Z�[�W
  PRINT_QUE                     *p_que;       //[in ]�v�����g�L���[
  WIFIBATTLEMATCH_VIEW_RESOURCE *p_view;      //[in ]���\�[�X���ʃ��W���[��
  WIFIBATTLEMATCH_GRAPHIC_WORK  *p_graphic;   //[in ]�`�拤�ʃ��W���[��
  GAMEDATA                      *p_gamedata;  //[in ]�Q�[���f�[�^
} DIGITALCARD_CHECK_PARAM;

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
extern DIGITALCARD_CHECK_WORK *DIGITALCARD_CHECK_Init( const DIGITALCARD_CHECK_PARAM *cp_param, HEAPID heapID );
extern void DIGITALCARD_CHECK_Main( DIGITALCARD_CHECK_WORK *p_wk );
extern void DIGITALCARD_CHECK_Exit( DIGITALCARD_CHECK_WORK *p_wk );

extern BOOL DIGITALCARD_CHECK_IsEnd( const DIGITALCARD_CHECK_WORK *cp_wk );
