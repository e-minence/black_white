/**
 *  @file   beacon_view_local.h
 *  @brief  ����Ⴂ�ʐM��ʃ��[�J���w�b�_
 *  @author Miyuki Iwasawa
 *  @date   10.01.19
 */

#pragma once

#include "gamesystem/game_beacon.h"
#include "gamesystem/game_beacon_types.h"
#include "gamesystem/game_beacon_accessor.h"
#include "gamesystem/beacon_status.h"
#include "gamesystem/g_power.h"
#include "net/wih_dwc.h"
#include "field/field_comm/intrude_work.h"
#include "field/fld_vreq.h"
#include "system/palanm.h"
#include "system/gfl_use.h"
#include "system/bmp_oam.h"
#include "savedata/intrude_save.h"
#include "savedata/misc.h"
#include "savedata/myitem_savedata.h"
#include "app/app_taskmenu.h"
#include "app/app_keycursor.h"
#include "item/itemsym.h"
#include "msg/msg_beacon_status.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "beacon_view_def.h"

//==============================================================================
//  �\���̒�`
//==============================================================================

//�I�u�W�F���\�[�X�Ǘ�
typedef struct _OBJ_RES{
  u32   num;
  u32*  tbl;
}OBJ_RES;

typedef struct _OBJ_RES_SRC{
  u16 num;
  u16 srcID;
}OBJ_RES_SRC;

//�I�u�W�F���\�[�X�Ǘ��\����
typedef struct _OBJ_RES_TBL{
  OBJ_RES res[OBJ_RES_MAX];
}OBJ_RES_TBL;

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

// Bmp�E�B���h�E
typedef struct{
  GFL_BMPWIN*   win;
  GFL_BMP_DATA* bmp;
  PRINT_UTIL  putil;
  u8          frm;
}BMP_WIN;

// OAM�t�H���g���[�N
typedef struct {
	BMPOAM_ACT_PTR oam;
	GFL_BMP_DATA * bmp;
}FONT_OAM;

//TaskMenu���[�N
typedef struct {
  APP_TASKMENU_WIN_WORK *work;
  APP_TASKMENU_ITEMWORK item;
}TMENU_ITEM;

//�p�l���Ǘ��\����
typedef struct _PANEL_WORK{
  u8  id; //�p�l��ID
  u8  data_idx; //�f�[�^�z��Q�Ǝ�index

  u8  n_line; //���݂̃��C��
  u8  rank; //�����N
  u8  sex;  //����
  u8  timeout_f;  //�^�C���A�E�g�t���O
  u16 tr_id;  //�g���[�i�[ID
  s16 px;
  s16 py;
  GFL_CLWK* cPanel;
  GFL_CLWK* cUnion;
  GFL_CLWK* cIcon;
  FONT_OAM  msgOam;
	STRBUF* str;
	STRBUF* name;

  const PRINT_QUE* que;
  GFL_TCBL* tcb_print;
  GFL_TCBL* tcb_icon;
}PANEL_WORK;

/*
 *  @brief  �|�b�v�A�b�v����
 */
typedef struct _LOG_CTRL{
  u8  max;  //���O��
  u8  view_top;
  u8  view_max;
  u8  target;

  u8  g_power;
  u8  mine_power_f;
}LOG_CTRL;

/*
 *  @brief  �Z�[�u�f�[�^���p�����[�^
 */
typedef struct _MY_DATA{
  GPOWER_ID power;
  u32       tr_id;
  u8        sex;
  u8        union_char;
  u8        pm_version;  ///<PM_VERSION
}MY_DATA;


///����Ⴂ�ʐM��ԎQ�Ɖ�ʊǗ����[�N
typedef struct _BEACON_VIEW{
  GAMESYS_WORK *gsys;
  GAMEDATA* gdata;
  FIELDMAP_WORK* fieldWork; 
  FIELD_SUBSCREEN_WORK *subscreen;
  FLD_VREQ* fld_vreq;
  MISC* misc_sv;
  MYITEM_PTR item_sv;
  GAME_COMM_SYS_PTR game_comm;
  WIFI_LIST*  wifi_list;
  POWER_CONV_DATA* gpower_data;

  ////////////////////////////////////////
  MY_DATA               my_data;
  
  BEACON_STATUS* b_status;
  GAMEBEACON_INFO_TBL*  infoStack;  //�X�^�b�N���[�N
  GAMEBEACON_INFO_TBL*  infoLog;    //���O�e�[�u��
  GAMEBEACON_INFO*      tmpInfo;
  GAMEBEACON_INFO*      tmpInfo2;
  u16                   tmpTime;
  u16                   newLogTime;
  GAMEBEACON_INFO*      newLogInfo;
  PANEL_WORK*           newLogPanel;

  u8                    newLogOfs;
  u8                    newLogIdx;
  u8                    newLogWait;
  u8                    old_list_max;
  BOOL                  first_entry_f;

  int       active;
  BOOL      my_power_f;
  int       seq;
  int       sub_seq;
  u16       event_id;
  u16       event_reserve_f;

  int       io_interval;      ///<�X�^�b�N�`�F�b�N�C���^�[�o���J�E���^
  u8        msg_spd;            ///<���b�Z�[�W�X�s�[�h
  u8        gpower_check_req;   ///<��������G�p���[�m�F�C�x���g���N�G�X�g�t���O
  u8        init_f;             ///<���������t���O
  LOG_CTRL  ctrl;

  //���X�g�A���X�N���[���R���g���[��
  u8          scr_repeat_f;       //���s�[�g���t���O
  u8          scr_repeat_end;     //���s�[�g�I���t���O
  u8          scr_repeat_ret_seq; //���s�[�g�I����̖߂��seq
  SCROLL_DIR  scr_repeat_dir; //�X�N���[�������ۑ�
  u32         scr_repeat_ct;  //���s�[�g�J�E���^

  ///����G�p���[�������N�G�X�g�`�F�b�N
  u8        sp_gpower_req_ct;
  u8        sp_gpower_req[ SP_GPOWER_REQ_MAX ];

  ////////////////////////////////////////
  HEAPID      heapID;
  HEAPID      tmpHeapID;
  HEAPID      heap_sID;
  ARCHANDLE*  arc_handle;

  GFL_TCBLSYS*  pTcbSys;
  GFL_TCB*      tcbVIntr;

  int           eff_task_ct;  //�G�t�F�N�g�^�X�N��

  PALETTE_FADE_PTR pfd;

  GFL_FONT *fontHandle;
  PRINT_QUE *printQue;
  APP_TASKMENU_RES* menuRes;
  APP_KEYCURSOR_WORK* key_cursor;
  TMENU_ITEM tmenu[TMENU_YN_MAX];
  TMENU_ITEM tmenu_check[TMENU_CHECK_MAX];

  WORDSET *wordset;
  GFL_MSGDATA *mm_status;
  STRBUF *str_tmp;
  STRBUF *str_expand;
  STRBUF *str_popup;  //�|�b�v�A�b�v�p�e���|����

  BMPOAM_SYS_PTR bmpOam;
  GFL_CLUNIT* cellUnit;
  GFL_CLSYS_REND* cellRender;
  GFL_CLACTPOS  cellSurfacePos;
  OBJ_RES_TBL objResNormal;
  OBJ_RES_TBL objResUnion;
  OBJ_RES_TBL objResIcon;
  
  BMP_WIN    win[ WIN_MAX ];
 
  PANEL_WORK  panel[PANEL_MAX];
  GFL_CLWK*   pAct[ACT_MAX];
  u32 log_count;

  //���\�[�X
  RES2D_CHAR  resCharIcon[BEACON_ICON_MAX];
  RES2D_CHAR  resCharUnion[UNION_CHAR_MAX];
  RES2D_PLTT  resPlttUnion;
  RES2D_PLTT  resPlttPanel;

}BEACON_VIEW;

/*
 *  @brief  �C�x���g���N�G�X�g
 */
extern void BEACON_VIEW_SUB_EventReserve( BEACON_VIEW_PTR wk, BEACON_DETAIL_EVENT ev_id);

/*
 *  @brief  �C�x���g���N�G�X�g
 */
extern void BEACON_VIEW_SUB_EventRequest( BEACON_VIEW_PTR wk, BEACON_DETAIL_EVENT ev_id);

/*
 *  @brief  �C�x���g���N�G�X�g���Z�b�g
 */
extern void BEACON_VIEW_SUB_EventReserveReset( BEACON_VIEW_PTR wk );

