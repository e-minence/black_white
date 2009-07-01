//======================================================================
/**
 * @file	  itemmenu.h
 * @brief	  �A�C�e�����j���[
 * @author	k.ohno
 * @date	  2009.06.30
 */
//======================================================================

#pragma once

#include <gflib.h>
#include "net/network_define.h"
#include "arc_def.h"

#include "item/item.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"
#include "system/bmp_winframe.h"

#include "message.naix"
#include "msg/msg_d_field.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "field/fieldmap.h"
#include "font/font.naix" //NARC_font_large_nftr
#include "sound/pm_sndsys.h"

extern const GFL_PROC_DATA ItemMenuProcData;

typedef struct _DEBUGITEM_PARAM FIELD_ITEMMENU_WORK;
typedef void (StateFunc)(FIELD_ITEMMENU_WORK* wk);

struct _DEBUGITEM_PARAM {
  StateFunc* state;      ///< �n���h���̃v���O�������
  GMEVENT * event;
  GAMESYS_WORK * gsys;
  FIELD_MAIN_WORK * fieldmap;
  SAVE_CONTROL_WORK *ctrl;

	GFL_BMPWIN* win;
 // BMPMENULIST_WORK* lw;		// BMP���j���[���[�N
//  BMP_MENULIST_DATA*   menulist;
  GFL_MSGDATA *MsgManager;			// ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[
  WORDSET			*WordSet;								// ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[
  STRBUF*  pStrBuf;
	STRBUF*  pExpStrBuf;
  GFL_FONT 			*fontHandle;
	int itemid;
	int itemnum;
	int curpos;
	HEAPID heapID;
	u32 bgchar;
};

