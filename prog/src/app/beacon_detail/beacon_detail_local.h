//=============================================================================
/**
 *
 *	@file		beacon_detail_local.h
 *	@brief  ����Ⴂ�ڍ׉�ʃ��[�J���w�b�_
 *	@author	Miyuki Iwasawa
 *	@data		2010.02.01
 *
 */
//=============================================================================

#pragma once

#include "gamesystem/game_beacon.h"
#include "gamesystem/game_beacon_accessor.h"
#include "gamesystem/game_beacon_types.h"
#include "gamesystem/beacon_status.h"
#include "gamesystem/msgspeed.h"
#include "tr_tool/trtype_def.h"
#include "system/tr2dgra.h"
#include "net_app/union/union_beacon_tool.h"

#include "beacon_detail_def.h"

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//�L�����N�^�[���\�[�X
typedef struct _RES2D_CHAR{
  NNSG2dCharacterData* p_char;
  void* buf;
}RES2D_CHAR;

//�p���b�g���\�[�X
typedef struct _RES2D_PLTT{
  NNSG2dPaletteData* p_pltt;
  u16* dat;
  void* buf;
}RES2D_PLTT;

//�I�u�W�F���\�[�X��`
typedef struct _OBJ_RES_SRC{
  u8  type, pltt_ofs, pltt_siz;
  u16 pltt_id;
  u16 cgr_id;
  u16 cell_id;
}OBJ_RES_SRC;

//�I�u�W�F���\�[�X�Ǘ��\����
typedef struct _OBJ_RES_TBL{
  u32 pltt;
  u32 cgr;
  u32 cell;
}OBJ_RES_TBL;


// Bmp�E�B���h�E
typedef struct{
  GFL_BMPWIN *win;
  GFL_BMP_DATA* bmp;
  PRINT_UTIL putil;
}BMP_WIN;

typedef struct _BEACON_WIN{
  BMP_WIN prof[BEACON_PROF_MAX];
  BMP_WIN record;
  GFL_BMPWIN* pms;
  GFL_CLWK* cRank;
  GFL_CLWK* cTrainer;
  u8  union_char;
  u8  rank;
  u8  frame;
}BEACON_WIN;

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
typedef struct 
{
  HEAPID heapID;
  HEAPID tmpHeapID;
  int                   seq;
	int										sub_seq;
  int                   eff_task_ct;

  BEACON_DETAIL_PARAM*  param;
  GFL_TCBLSYS*  pTcbSys;
  
  BEACON_STATUS* b_status;
  GAMEBEACON_INFO_TBL*  infoLog;    //���O�e�[�u��
  GAMEBEACON_INFO*      tmpInfo;
  u16                   tmpTime;
//  MY_DATA               my_data;
  u8                    flip_sw;
  u8                    list_max;
  u8                    list_top;
  u8                    list[GAMEBEACON_INFO_TBL_MAX];

  u8                    msg_spd;

	//�`��ݒ�
	BEACON_DETAIL_GRAPHIC_WORK	*graphic;
  ARCHANDLE* handle;
  ARCHANDLE* h_trgra;
  OBJ_RES_TBL objResNormal;
  OBJ_RES_TBL objResTrainer[BEACON_WIN_MAX];
  OBJ_RES_TBL objResUnion;
  
  RES2D_CHAR  resCharUnion[UNION_CHAR_MAX];
  RES2D_PLTT  resPlttUnion;

	//�^�b�`�o�[
	TOUCHBAR_WORK							*touchbar;

	//�t�H���g
	GFL_FONT									*font;

	//�v�����g�L���[
	PRINT_QUE									*print_que;
	GFL_MSGDATA								*msg;
  WORDSET                   *wset;
  STRBUF *str_tmp;
  STRBUF *str_expand;
  STRBUF *str_popup;  //�|�b�v�A�b�v�p�e���|����

#ifdef	BEACON_DETAIL_PRINT_TOOL
	//�v�����g���[�e�B���e�B
	PRINT_UTIL								print_util;
#endif	//BEACON_DETAIL_PRINT_TOOL

  BMP_WIN         win_popup;
  PMS_DRAW_WORK*  pms_draw;
  BEACON_WIN      beacon_win[BEACON_WIN_MAX];

} BEACON_DETAIL_WORK;



