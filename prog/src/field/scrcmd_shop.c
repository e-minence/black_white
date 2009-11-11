//////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �X�N���v�g�R�}���h�F�V���b�v�֘A
 * @file   scrcmd_shop.c
 * @author iwasawa, mori
 * @date   2009.10.20
 *
 */
//////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "system/vm_cmd.h"
#include "scrcmd.h"
#include "system/main.h"
#include "system/bmp_menuwork.h"
#include "system/bmp_menulist.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menu.h"
#include "sound/pm_sndsys.h"
#include "item/item.h"    // ITEM_GetIndex
#include "item_icon.naix"
#include "message.naix"
#include "message.naix"
#include "script_message.naix"

#include "gamesystem/game_data.h"
#include "gamesystem/msgspeed.h"
#include "item/itemsym.h"
#include "script_local.h"
//#include "script.h"     // SCRIPT_SetSubProcWorkPointerAdrs
#include "app/p_status.h"
#include "event_fieldmap_control.h"
#include "scrcmd_shop.h"
#include "fieldmap.h"
#include "script_def.h"
#include "field_camera.h"
#include "field/field_msgbg.h"
#include "msg/script/msg_shop.h"

#include "shop_data.cdat" // shop_data_table, shop_itemnum_table

#include "arc/shop_gra.naix"

#ifdef PM_DEBUG
#define SHOP_DEBUG_COMMAND  // LR�ł�������ɓ���BSELECT�Ńo�b�W�W�̃V���b�v�ɂȂ�
#endif

//====================================================================
// ���萔�錾
//====================================================================

#define SHOP_BUY_OBJ_RES_NUM   ( 3 )

// BMPWIN
enum{
  SHOP_BUY_BMPWIN_OKODUKAI=0,
  SHOP_BUY_BMPWIN_MONEY,
  SHOP_BUY_BMPWIN_LIST,
  SHOP_BUY_BMPWIN_NUM,
  SHOP_BUY_BMPWIN_PRICE,
  SHOP_BUY_BMPWIN_ITEMINFO,
  SHOP_BUY_BMPWIN_TALKMSG,
  SHOP_BUY_BMPWIN_MAX,
};

// OBJ�p
enum{
  SHOP_OBJ_CURSOR=0,
  SHOP_OBJ_ARROW_UP,
  SHOP_OBJ_ARROW_DOWN,
  SHOP_OBJ_ARROW_UPDOWN,
  SHOP_OBJ_ITEM,
  SHOP_OBJ_MAX,
};

// ���j���[�E�C���h�E�g�̃p���b�g��]������ʒu
#define MENU_WINDOW_PAL_OFFSET  ( 9 )

// �������p�o�b�t�@�̒���
#define SHOP_MYGOLD_STR_MAX   ( 10 )

// ��������o�b�t�@�̒���
#define SHOP_ITEMNUM_STR_MAX   ( 20 )

// ��������o�b�t�@�̒���
#define SHOP_STRING_MAX   ( 200 )

// �X�N���[���N���A�֐��Ŏg�p�����`
#define SCREEN_SUBMENU_AND_INFOWIN  ( 0 )
#define SCREEN_SUBMENU_ONLY         ( 1 )


//====================================================================
// ���\���̐錾�P
//====================================================================

// �C�x���g�p�\����
typedef struct _SHOP_EVENT_WORK{
  void* bag_param;
  u16*  ret_work;
}SHOP_EVENT_WORK;

// �V���b�v�����A�v������p�\����
typedef struct{
  u8      seq;     // �����A�v���V�[�P���X����p
  u8      next;    // ���̃V�[�P���X�\��
  u16     wait;    // �E�F�C�g
  HEAPID  heapId;  // �q�[�vID
  GFL_FONT *font;
  
  MYSTATUS  *mystatus;
  MYITEM    *myitem;  

  GFL_BMPWIN        *win[SHOP_BUY_BMPWIN_MAX];  // BMPWIN
  BMP_MENULIST_DATA *list;            // ���������X�g�f�[�^
  BMPMENU_WORK      *yesnoWork;
  GFL_MSGDATA       *shopMsgData;     // �V���b�v���MSGDATA
  GFL_MSGDATA       *itemInfoMsgData; // �ǂ�����MSGDATA
  WORDSET           *wordSet;         // ������W�J���[�N
  STRBUF            *priceStr;       
  STRBUF            *expandBuf;       
  BMPMENULIST_WORK  *menuList;        // BMPMENULIST
  PRINT_UTIL         printUtil;       
  PRINT_QUE         *printQue;
  GFL_TCBLSYS       *pMsgTcblSys;  // ���b�Z�[�W�\���p�^�X�NSYS
  PRINT_STREAM      *printStream;
  
  GFL_CLUNIT        *ClactUnit;                // �Z���A�N�^�[���j�b�g
  GFL_CLWK*         ClactWork[SHOP_OBJ_MAX];   // OBJ���[�N�|�C���^
  u32               clact_res[3][SHOP_BUY_OBJ_RES_NUM]; // OBJ���\�[�X�n���h��

  ARCHANDLE         *itemiconHandle;    // �A�C�e���A�C�R���n���h��

  u16               selectitem;
  u16               price;
  u32               buy_max;     // �w���ő吔
  s16               item_multi;
  s16               etc;
} SHOP_BUY_APP_WORK;

// �V���b�v�����N�����\����
typedef struct {
  u8 seq;       // �V�[�P���X���䃏�[�N
  u8 wait;      // �E�F�C�g�p
  u8 shopType;  // �V���b�v�^�C�v(SHOP_TYPE_NORMAL, SHOP_TYPE_FIX, etc..)
  u8 shopId;    // �V���b�v�̏ꏊID�ishop_data.cdat�Q�ƁBSHOP_TYOE_FIX�̍ۂɎg�p����)
  
  SHOP_BUY_APP_WORK wk;
  
} SHOP_BUY_CALL_WORK;


typedef struct {
  int frame;
  u8  x,y,w,h;
  u16 pal,chara;
}BMPWIN_DAT;


//====================================================================
// ���v���g�^�C�v�錾
//====================================================================

static BOOL EvShopBuyWait( VMHANDLE *core, void *wk );
static BOOL ShopCallFunc( GAMESYS_WORK *gsys, SHOP_BUY_APP_WORK *wk, int type, int id );
static void init_work( SHOP_BUY_APP_WORK *wk );
static void exit_work( SHOP_BUY_APP_WORK *wk );
static void shop_item_set( SHOP_BUY_APP_WORK *wk, int type, int id, int badge );
static void shop_item_release( SHOP_BUY_APP_WORK *wk );
static void bg_trans( SHOP_BUY_APP_WORK *wk );
static void bg_clear( SHOP_BUY_APP_WORK *wk );
static void obj_init( SHOP_BUY_APP_WORK *wk );
static void obj_del( SHOP_BUY_APP_WORK *wk );
static void itemicon_resource_change( SHOP_BUY_APP_WORK *wk, u16 itemno );
static void bmpwin_init( SHOP_BUY_APP_WORK *wk );
static void bmpwin_exit( SHOP_BUY_APP_WORK *wk );
static void print_mygold( SHOP_BUY_APP_WORK *wk );
static void print_carry_item( SHOP_BUY_APP_WORK *wk, u16 itemno );
static void print_iteminfo( SHOP_BUY_APP_WORK *wk, u16 itemno );
static void bmpwin_list_init( SHOP_BUY_APP_WORK *wk );
static void bmpwin_list_exit( SHOP_BUY_APP_WORK *wk );
static void line_callback(BMPMENULIST_WORK * wk, u32 param, u8 y );
static void move_callback( BMPMENULIST_WORK * wk, u32 param, u8 mode);
static void ShopPrintMsg( SHOP_BUY_APP_WORK *wk, int strId, u16 itemno );
static void ShopDecideMsg( SHOP_BUY_APP_WORK *wk, int strId, u16 itemno, u16 num );
static void print_multiitem_price( SHOP_BUY_APP_WORK *wk, u16 number, int one_price );
static  int price_key_control( SHOP_BUY_APP_WORK *wk );
static void submenu_screen_clear( int type );


// BMPWIN��`�e�[�u��
#include "wbshopbmp.h"    // Window0BmpDataTable

//--------------------------------------------------------------------
/**
 * @brief �V���b�v�w���C�x���g�Ăяo�� 
 *
 * @param core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdCallShopProcBuy( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*      work = (SCRCMD_WORK*)wk;
  u16               shop_id = SCRCMD_GetVMWorkValue( core, work );
  u16*              ret_work = SCRCMD_GetVMWork( core, work );
  GAMESYS_WORK*     gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*         gamedata = GAMESYSTEM_GetGameData( gsys );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  void** scr_subproc_work = SCRIPT_SetSubProcWorkPointerAdrs( sc );
  FLDMSGBG *fldmsg = FIELDMAP_GetFldMsgBG( fieldmap );
  

  // �V���b�v�p���[�N�m��
  SHOP_BUY_CALL_WORK* sbw = GFL_HEAP_AllocClearMemory(HEAPID_PROC,sizeof(SHOP_BUY_CALL_WORK));
  *scr_subproc_work = sbw;
  sbw->wk.font      = FLDMSGBG_GetFontHandle( fldmsg );
  sbw->wk.mystatus  = GAMEDATA_GetMyStatus( gamedata );
  sbw->wk.heapId    = HEAPID_FIELDMAP;
  sbw->wk.myitem    = GAMEDATA_GetMyItem( gamedata );

  if(shop_id == SCR_SHOPID_NULL){
    //�ϓ��V���b�v�Ăяo�� 
    sbw->shopType = shop_id;
  }else{
    //�Œ�V���b�v�Ăяo��
    sbw->shopType = shop_id;
    sbw->shopId   = shop_id;
  }

  // �V���b�v�Ăяo��
  VMCMD_SetWait( core, EvShopBuyWait );

  /*
    �C�x���g�I������ ret_work �ɏI�����[�h��Ԃ��Ă�������
    SCR_PROC_RETMODE_EXIT �ꔭ�I��
    SCR_PROC_RETMODE_NORMAL �p��
  */
  return VMCMD_RESULT_SUSPEND;
}


enum{
  SHOP_BUY_CALL_CAMERATRACE_STOP=0,
  SHOP_BUY_CALL_CAMERATRACE_STOP_WAIT,
  SHOP_BUY_CALL_CAMERA_MOVE,
  SHOP_BUY_CALL_CAMERA_MOVE_WAIT,
  SHOP_BUY_CALL,
  SHOP_BUY_CALL_CAMERA_RETURN,
  SHOP_BUY_CALL_CAMERA_RETURN_WAIT,
  SHOP_BUY_CALL_END,
};

//----------------------------------------------------------------------------------
/**
 * @brief �V���b�v�����T�u�V�[�P���X
 *
 * @param   core  SCRCMD_WORK
 * @param   wk    SHOP_BUY_CALL_WORK
 *
 * @retval  BOOL  �I��:TRUE   �҂�:FALSE
 */
//----------------------------------------------------------------------------------
static BOOL EvShopBuyWait( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  void **scr_subproc_work = SCRIPT_SetSubProcWorkPointerAdrs( sc );
  SHOP_BUY_CALL_WORK *sbw = *scr_subproc_work;
  GAMESYS_WORK*     gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FIELD_CAMERA *fieldcamera = FIELDMAP_GetFieldCamera( fieldmap );


  // 
  switch(sbw->seq)
  {
    case SHOP_BUY_CALL_CAMERATRACE_STOP:
      FIELD_CAMERA_StopTraceRequest( fieldcamera );
      sbw->seq++;
      break;
    case SHOP_BUY_CALL_CAMERATRACE_STOP_WAIT:
      if(FIELD_CAMERA_CheckTrace(fieldcamera)==FALSE){
        sbw->seq++;
      }
      break;
    case SHOP_BUY_CALL_CAMERA_MOVE:
    
      // �J�����ړ�
      OS_Printf("�V���b�v�N��\n");
      FIELD_CAMERA_SetRecvCamParam( fieldcamera );
      {
        FLD_CAM_MV_PARAM param;
        param.Core.Shift.x = FX32_ONE*16*6;
        param.Core.Shift.y = -FX32_ONE*8*7;
        param.Core.Shift.z = 0;
  
        param.Chk.Shift    = TRUE;
        param.Chk.Pitch    = FALSE;
        param.Chk.Yaw   = FALSE;
        param.Chk.Dist   = FALSE;
        param.Chk.Fovy = FALSE;
        param.Chk.Pos   = FALSE;
        FIELD_CAMERA_SetLinerParam( fieldcamera, &param, 10 );
      }
      sbw->seq++;
      break;
    case SHOP_BUY_CALL_CAMERA_MOVE_WAIT:
      // �J�����ړ��I���҂�
      if(!FIELD_CAMERA_CheckMvFunc(fieldcamera)){
        sbw->seq++;
        sbw->wk.seq = 0;
        FLDMSGBG_SetBlendAlpha( FALSE );  // ���b�Z�[�W�E�C���h�E������OFF
      }
      break;
    case SHOP_BUY_CALL:
      // �V���b�v���C������
      if(ShopCallFunc( gsys, &sbw->wk, sbw->shopType, sbw->shopId  )){
        sbw->seq++;
      }
      break;
    case SHOP_BUY_CALL_CAMERA_RETURN:
      // �ړ��J�����߂�����
      {
        FLD_CAM_MV_PARAM_CHK chk = {TRUE, FALSE,FALSE,FALSE,FALSE,};
        FIELD_CAMERA_RecvLinerParam(fieldcamera, &chk, 8);
      }
      sbw->seq++;
      sbw->wait=0;
      break;
    case SHOP_BUY_CALL_CAMERA_RETURN_WAIT:
      // �J�����ړ��I���҂�
      if(!FIELD_CAMERA_CheckMvFunc(fieldcamera)){
        FIELD_CAMERA_RestartTrace( fieldcamera );
        FIELD_CAMERA_ClearRecvCamParam(fieldcamera);
        FLDMSGBG_SetBlendAlpha( TRUE );  // ���b�Z�[�W�E�C���h�E������ON
        sbw->seq++;
      }
      break;
    case SHOP_BUY_CALL_END:
      GFL_HEAP_FreeMemory(*scr_subproc_work);
      return TRUE;
      break;
  }
  
  return FALSE;
}

// �����܂ł��Ăяo�����B

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

static const BMPWIN_YESNO_DAT yesno_data= {
  GFL_BG_FRAME1_M,
  24, 13, 
  11, 1+24
};

// �V���b�v������ʑJ�ڗp��`
enum{
  SHOPBUY_SEQ_INIT=0,
  SHOPBUY_SEQ_MAIN,
  SHOPBUY_SEQ_SELECT,
  SHOPBUY_SEQ_DECIDE_NUM,
  SHOPBUY_SEQ_BACK,
  SHOPBUY_SEQ_YESNO,
  SHOPBUY_SEQ_SELECT_YESNO_WAIT,
  SHOPBUY_SEQ_BUY,
  SHOPBUY_SEQ_JUDGE_PREMIER_BALL,
  SHOPBUY_SEQ_MSG_WAIT,
  SHOPBUY_SEQ_END,
};

//----------------------------------------------------------------------------------
/**
 * @brief �V���b�v��ʏ�����
 *
 * @param   gsys    
 * @param   wk    
 * @param   type    
 * @param   id    
 */
//----------------------------------------------------------------------------------
static void shop_call_init( GAMESYS_WORK *gsys, SHOP_BUY_APP_WORK *wk, int type, int id )
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
  MYSTATUS *my       = GAMEDATA_GetMyStatus(gamedata);

  // ���[�N������
  init_work( wk );

  // �i�����ǂݍ���
#ifdef SHOP_DEBUG_COMMAND
  if(GFL_UI_KEY_GetCont()&PAD_BUTTON_SELECT){
    shop_item_set( wk, type, id, 8);
  }else{
    shop_item_set( wk, type, id, MyStatus_GetBadgeCount(my));
  }
#else
    shop_item_set( wk, type, id, MyStatus_GetBadgeCount(my));
#endif

  bg_trans( wk );     // BG�]��
  bmpwin_init( wk );  // BMPWIN������
  obj_init( wk );     // OAM������
  print_mygold(wk);   // �������\��
  print_iteminfo( wk, 1 );

  bmpwin_list_init( wk ); // ���i���X�g�\��
 
}

//----------------------------------------------------------------------------------
/**
 * @brief �ǂ��������̍ő吔��Ԃ�
 *
 * @param   item  
 *
 * @retval  int   �A�C�e���i���o�[
 */
//----------------------------------------------------------------------------------
static int get_item_max( int item )
{
  // 
  if(item>=ITEM_WAZAMASIN01 && item < ITEM_HIDENMASIN01){
    return 99;
  }
  
  return 999;
}

//----------------------------------------------------------------------------------
/**
 * @brief ����I�����ɉ������邩�ŏ����𕪊򂳂���
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int can_player_buy_item( SHOP_BUY_APP_WORK *wk )
{
  int gold = MyStatus_GetGold(wk->mystatus);
  int num  = MYITEM_GetItemNum( wk->myitem, wk->selectitem, wk->heapId );

  // ��������Ȃ��ł���I
  if(MYITEM_AddCheck( wk->myitem, wk->selectitem, 1, wk->heapId )==FALSE)
  {
    ShopPrintMsg( wk, mes_shop_02_05, wk->selectitem );
    wk->seq  = SHOPBUY_SEQ_MSG_WAIT;
    wk->next = SHOPBUY_SEQ_BACK;

  // �u�����܂����I�v
  }else if(gold<wk->price)
  {
    ShopPrintMsg( wk, mes_shop_02_01, wk->selectitem );
    wk->seq  = SHOPBUY_SEQ_MSG_WAIT;
    wk->next = SHOPBUY_SEQ_BACK;

  // �����������̂ł��̂܂܁u�͂��E�������v
  }else if(gold>=wk->price && gold<(wk->price*2))
  {
    ShopDecideMsg(wk,  mes_shop_02_03, wk->selectitem, wk->item_multi );
    wk->seq  = SHOPBUY_SEQ_MSG_WAIT;
    wk->next = SHOPBUY_SEQ_YESNO;

  // ����������̂Ō��I����
  }else{
  
    ShopPrintMsg( wk, mes_shop_02_02, wk->selectitem );
    wk->seq  = SHOPBUY_SEQ_MSG_WAIT;
    wk->next = SHOPBUY_SEQ_SELECT;
    wk->buy_max = gold/wk->price;
    if(wk->buy_max>99){   // ������ő吔�͂X�X��
      wk->buy_max = 99;
     }
     // ���Ă�ő吔���Z�o
    if(wk->buy_max+num > MYITEM_GetItemMax( wk->selectitem ) ){
      wk->buy_max = MYITEM_GetItemMax( wk->selectitem ) - num;
    }
  }
  return 1;
}

//----------------------------------------------------------------------------------
/**
 * @brief 
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static void shop_main( SHOP_BUY_APP_WORK *wk )
{
  u32 ret = BmpMenuList_Main( wk->menuList );
  if(ret!=BMPMENULIST_NULL)
  {
    if(ret==BMPMENULIST_CANCEL)
    {
      wk->seq = SHOPBUY_SEQ_END;
    } else {
      wk->selectitem = ret;
      wk->price      = ITEM_GetParam( ret, ITEM_PRM_PRICE, wk->heapId );
      wk->item_multi = 1;
      // �w���ł��邩����i�����Ȃ��E�������E����������j
      can_player_buy_item( wk );
    }
  }

}

//----------------------------------------------------------------------------------
/**
 * @brief �V���b�v�������C�����[�`��
 *
 * @param   gsys  GAMESYS_WORK
 * @param   wk    SHOP_BUY_APP_WORK
 * @param   type  �V���b�v�^�C�v(SHOP_TYPE_NORMAL or SHOP_TYPE_FIX)
 * @param   id    �Œ�V���b�v�̍ۂ�ID�ishop_data.cdat�Q��)
 *
 * @retval  BOOL  �p��:FALSE  �I��:TRUE
 */
//----------------------------------------------------------------------------------
static BOOL ShopCallFunc( GAMESYS_WORK *gsys, SHOP_BUY_APP_WORK *wk, int type, int id )
{
  u32 ret;
  
  switch( wk->seq ){
  case SHOPBUY_SEQ_INIT:  // ������
    shop_call_init(gsys, wk, type, id);
    
    wk->seq++;
    break;
  case SHOPBUY_SEQ_MAIN:
    shop_main(wk);
    break;
  case SHOPBUY_SEQ_SELECT:
    // ����������̂Ō��I����
    print_carry_item( wk, wk->selectitem );
    print_multiitem_price( wk, wk->item_multi, wk->price );
    GFL_CLACT_WK_SetDrawEnable( wk->ClactWork[SHOP_OBJ_ARROW_UPDOWN], TRUE );
    wk->seq = SHOPBUY_SEQ_DECIDE_NUM;

    break;
  case SHOPBUY_SEQ_DECIDE_NUM:
    wk->seq = price_key_control( wk );
    break;
  case SHOPBUY_SEQ_BACK:    // �߂�
    submenu_screen_clear( SCREEN_SUBMENU_AND_INFOWIN );
    GFL_BMPWIN_MakeScreen( wk->win[SHOP_BUY_BMPWIN_LIST] );
    GFL_BMPWIN_MakeScreen( wk->win[SHOP_BUY_BMPWIN_ITEMINFO] );
    print_mygold( wk );
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_M );
    GFL_CLACT_WK_SetDrawEnable( wk->ClactWork[SHOP_OBJ_ARROW_UPDOWN], FALSE );
    wk->seq = SHOPBUY_SEQ_MAIN;
    break;
  case SHOPBUY_SEQ_YESNO:
    submenu_screen_clear( SCREEN_SUBMENU_ONLY );
    GFL_BMPWIN_MakeScreen( wk->win[SHOP_BUY_BMPWIN_LIST] );
    GFL_CLACT_WK_SetDrawEnable( wk->ClactWork[SHOP_OBJ_ARROW_UPDOWN], FALSE );
    wk->yesnoWork = BmpMenu_YesNoSelectInit(  &yesno_data, 1, MENU_WINDOW_PAL_OFFSET, 0, wk->heapId );
    wk->seq = SHOPBUY_SEQ_SELECT_YESNO_WAIT;
    break;
  case SHOPBUY_SEQ_SELECT_YESNO_WAIT:
    ret = BmpMenu_YesNoSelectMain( wk->yesnoWork );
    if(ret!=BMPMENU_NULL){
      if(ret==0){
        wk->seq = SHOPBUY_SEQ_BUY;
      }else if(ret==BMPMENU_CANCEL){
        wk->seq = SHOPBUY_SEQ_BACK;
      }
    }
    break;
  case SHOPBUY_SEQ_BUY:
    PMSND_PlaySE( SEQ_SE_SYS_22 );
    ShopPrintMsg( wk, mes_shop_02_04, wk->selectitem );
    Mystatus_SubGold( wk->mystatus, wk->price*wk->item_multi);
    MYITEM_AddItem( wk->myitem, wk->selectitem, wk->item_multi, wk->heapId);
    wk->seq  = SHOPBUY_SEQ_MSG_WAIT;
    wk->next = SHOPBUY_SEQ_JUDGE_PREMIER_BALL;
    break;

  // �v���~�A�{�[������
  case SHOPBUY_SEQ_JUDGE_PREMIER_BALL:
    if( wk->selectitem==ITEM_MONSUTAABOORU && wk->item_multi>=10){
      ShopPrintMsg( wk, mes_shop_02_08, wk->selectitem );
      MYITEM_AddItem( wk->myitem, ITEM_PUREMIABOORU, 1, wk->heapId);
      wk->seq  = SHOPBUY_SEQ_MSG_WAIT;
      wk->next = SHOPBUY_SEQ_BACK;
    }else{
      wk->seq  = SHOPBUY_SEQ_BACK;
    }
    break;

  case SHOPBUY_SEQ_MSG_WAIT:    // ���b�Z�[�W�\���҂�
    {
      
      PRINTSTREAM_STATE state = PRINTSYS_PrintStreamGetState( wk->printStream );
      if( state == PRINTSTREAM_STATE_DONE ){
        PRINTSYS_PrintStreamDelete( wk->printStream );
        wk->seq = wk->next;
      }else if( state == PRINTSTREAM_STATE_RUNNING){
        if(GFL_UI_KEY_GetCont()&PAD_BUTTON_DECIDE){
          PRINTSYS_PrintStreamShortWait( wk->printStream, 0 );
        }
      }else if( state == PRINTSTREAM_STATE_PAUSE ){
        if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_DECIDE){
          PRINTSYS_PrintStreamReleasePause( wk->printStream );
          PMSND_PlaySE( SEQ_SE_SELECT1 );
        }
      }
    }
    break;

  case SHOPBUY_SEQ_END:
    bmpwin_list_exit(wk);   // BMPLIST���
    obj_del( wk );          // OBJ�폜
    bmpwin_exit( wk );      // BMPWIN���
    bg_clear(wk);           // BG����
    
    // �i�����f�[�^�폜
    shop_item_release( wk);

    // ���[�N���
    exit_work(wk);
    return TRUE;
    break;
  }

  PRINTSYS_QUE_Main( wk->printQue );
  PRINT_UTIL_Trans( &wk->printUtil, wk->printQue );

  GFL_TCBL_Main( wk->pMsgTcblSys );
  
  // �p��
  return FALSE;
}




// �o�b�W�̐��ŃV���b�v��ς���ׂ̃e�[�u��
static const u8 normal_shop_offset[]={
 0,1,1,2,2,3,3,4,5,
};


//----------------------------------------------------------------------------------
/**
 * @brief �V���b�v�A�C�e���o�^
 *
 * @param   wk    SHOP_BUY_APP_WORK
 * @param   type  �V���b�v�^�C�v(SCR_SHOPID_NULL,SCR_SHOPID_FIX)
 * @param   id    �Œ�V���b�v�̍ۂ�ID�ishop_data.cdat�Q��)
 * @param   badge �o�b�W�擾����(SCR_SHOPID_NULL�̍ۂɎg�p�j
 */
//----------------------------------------------------------------------------------
static void shop_item_set( SHOP_BUY_APP_WORK *wk, int type, int id, int badge )
{
  int i,offset,itemnum;
  GFL_MSGDATA *itemMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                                             NARC_message_itemname_dat, wk->heapId );


  // �ϓ��V���b�v
  if(type==SCR_SHOPID_NULL)
  {
    offset = normal_shop_offset[badge];
  // �Œ胊�X�g�V���b�v
  } else {
    offset = id;
  }

  // �ǂ������o�^�EBMPMENULIST�f�[�^�쐬
  itemnum = shop_itemnum_table[offset];
  wk->list = BmpMenuWork_ListCreate( itemnum, wk->heapId );
  OS_Printf("type=%d, offset=%d, badge=%d,itemnum=%d\n", type, offset, badge, itemnum);
  for(i=0;i<(itemnum-1);i++)
  {
      OS_Printf("strId=%d\n", shop_data_table[offset][i]);
      BmpMenuWork_ListAddArchiveString( &wk->list[i], itemMsgData, 
                                        shop_data_table[offset][i], 
                                        shop_data_table[offset][i], 
                                        wk->heapId );
  }
  // �u��߂�v�o�^
  BmpMenuWork_ListAddArchiveString( &wk->list[i], wk->shopMsgData, 
                                    mes_shop_02_06, 
                                    BMPMENULIST_CANCEL, 
                                    wk->heapId );


  GFL_MSG_Delete( itemMsgData );
}


//----------------------------------------------------------------------------------
/**
 * @brief �V���b�v�A�C�e�������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void shop_item_release( SHOP_BUY_APP_WORK *wk )
{

  // ���j���[���X�g���
  BmpMenuWork_ListDelete( wk->list );

}

//----------------------------------------------------------------------------------
/**
 * @brief ���[�N������
 *
 * @param   wk    SHOP_BUY_APP_WORK
 */
//----------------------------------------------------------------------------------
static void init_work( SHOP_BUY_APP_WORK *wk )
{
  // ���b�Z�[�W�f�[�^�m��

  wk->shopMsgData     = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE, 
                                             NARC_script_message_shop_dat, wk->heapId );
  wk->itemInfoMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                                             NARC_message_iteminfo_dat, wk->heapId );

  wk->wordSet   = WORDSET_Create( wk->heapId );

  // �������~
  wk->priceStr = GFL_MSG_CreateString( wk->shopMsgData, mes_shop_02_07 );

  // �W�J�p�o�b�t�@
  wk->expandBuf = GFL_STR_CreateBuffer( SHOP_STRING_MAX, wk->heapId );

  // ��b�E�C���h�E�p�^�X�N�o�^
  wk->pMsgTcblSys = GFL_TCBL_Init( wk->heapId, wk->heapId , 32 , 32 );

#ifdef SHOP_DEBUG_COMMAND

  if(GFL_UI_KEY_GetCont()&PAD_BUTTON_L){
    MyStatus_SetGold(wk->mystatus, 999999);
  }else if(GFL_UI_KEY_GetCont()&PAD_BUTTON_R){
    MyStatus_SetGold(wk->mystatus, 5000);
  }
#endif
}



//----------------------------------------------------------------------------------
/**
 * @brief ���[�N���
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void exit_work( SHOP_BUY_APP_WORK *wk )
{
  GFL_TCBL_Exit( wk->pMsgTcblSys );

  WORDSET_Delete(wk->wordSet);
  
  GFL_STR_DeleteBuffer( wk->expandBuf );
  GFL_STR_DeleteBuffer( wk->priceStr );

  // ���b�Z�[�W�f�[�^���
  GFL_MSG_Delete( wk->shopMsgData );
  GFL_MSG_Delete( wk->itemInfoMsgData );

}

// �p���b�g�]���{��(byte�P�ʁj
#define SHOP_BG_PLTT_NUM  ( 32*3 )

//----------------------------------------------------------------------------------
/**
 * @brief BG�]��
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void bg_trans( SHOP_BUY_APP_WORK *wk )
{
  // �V���b�v��ʃ��\�[�X
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_SHOP_GRA, wk->heapId );
  
    // MAIN BG��Pltt
    GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_shop_gra_shop_bg_NCLR, PALTYPE_MAIN_BG, 
                                      0, SHOP_BG_PLTT_NUM, wk->heapId );
    // BG2��Char(�w�i�j
    GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_shop_gra_shop_bg_NCGR, GFL_BG_FRAME2_M, 
                                      0, 0, 0, wk->heapId);
    // BG2��Screen(�w�i�j
    GFL_ARCHDL_UTIL_TransVramScreen( handle, NARC_shop_gra_shop_bg1_NSCR, GFL_BG_FRAME2_M, 
                                     0, 0, 0, wk->heapId);
  
    GFL_ARC_CloseDataHandle( handle );
  }
  
  // �E�C���h�E�g
  BmpWinFrame_GraphicSet( GFL_BG_FRAME1_M, 1, MENU_WINDOW_PAL_OFFSET, MENU_TYPE_SYSTEM, wk->heapId );

  // �e�L�X�g�ʏ���
  GFL_BG_ClearScreen( GFL_BG_FRAME1_M );



  // BG2�ʂ̃v���C�I���e�B���P�ɕύX
  GFL_BG_SetPriority( GFL_BG_FRAME2_M, 1 );

}


//----------------------------------------------------------------------------------
/**
 * @brief BG����
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void bg_clear( SHOP_BUY_APP_WORK *wk )
{
  // �V���b�vBG�ʏ���
  GFL_BG_ClearScreen( GFL_BG_FRAME1_M );
  GFL_BG_ClearScreen( GFL_BG_FRAME2_M );

  // �V���b�v�p�ɕύX���Ă���BG2�ʂ̃v���C�I���e�B��0�ɖ߂�
  GFL_BG_SetPriority( GFL_BG_FRAME2_M, 0 );

}

#define SHOP_BUY_OBJ_BG_PRI ( 1 )

enum{
  SHOP_OBJ_RES_PLTT=0,
  SHOP_OBJ_RES_CHAR,
  SHOP_OBJ_RES_CELL,
};

static const GFL_CLWK_DATA shop_obj_param[]={
  { 172,  22, 0, 0, 1 },// �J�[�\��
  { 172,  92, 0, 0, 0 },// ����
  { 172, 132, 1, 0, 0 },// �����
  { 243, 128, 2, 0, 0 },// �㉺�J�[�\��
  {  22, 172, 0, 0, 1 },// �ǂ����A�C�R��
  // ��,�����W,�A�j���[�V�����V�[�P���X,�\�t�g�D�揇��  0>0xff, BG�D�揇��
};


//----------------------------------------------------------------------------------
/**
 * @brief �V���b�v���OBJ���\�[�X�ǂݍ���
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void shop_resource_load( SHOP_BUY_APP_WORK *wk )
{
  ARCHANDLE *handle = GFL_ARC_OpenDataHandle( ARCID_SHOP_GRA, wk->heapId );

  // �p���b�g
  wk->clact_res[0][ SHOP_OBJ_RES_PLTT ] = GFL_CLGRP_PLTT_RegisterEx( 
        handle, NARC_shop_gra_shop_obj_NCLR, CLSYS_DRAW_MAIN, 
        0, 0, 1, wk->heapId );

  // �L����( ��� )
  wk->clact_res[0][ SHOP_OBJ_RES_CHAR ] = GFL_CLGRP_CGR_Register( 
        handle, NARC_shop_gra_shop_sel_NCGR, 0, CLSYS_DRAW_MAIN, wk->heapId );
  // �L����( �J�[�\�� )
  wk->clact_res[1][ SHOP_OBJ_RES_CHAR ] = GFL_CLGRP_CGR_Register( 
        handle, NARC_shop_gra_shop_arw_NCGR, 0, CLSYS_DRAW_MAIN, wk->heapId );

  // �Z���i���j
  wk->clact_res[0][ SHOP_OBJ_RES_CELL ] = GFL_CLGRP_CELLANIM_Register( 
        handle, NARC_shop_gra_shop_sel_NCER, NARC_shop_gra_shop_sel_NANR, wk->heapId );
  // �Z���i�J�[�\���j
  wk->clact_res[1][ SHOP_OBJ_RES_CELL ] = GFL_CLGRP_CELLANIM_Register( 
        handle, NARC_shop_gra_shop_arw_NCER, NARC_shop_gra_shop_arw_NANR, wk->heapId );

  GFL_ARC_CloseDataHandle( handle );
}


//----------------------------------------------------------------------------------
/**
 * @brief �ǂ����A�C�R���ǂݍ���
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void itemicon_resource_load( SHOP_BUY_APP_WORK *wk )
{
  // �p���b�g
  wk->clact_res[2][ SHOP_OBJ_RES_PLTT ] = GFL_CLGRP_PLTT_RegisterEx( 
        wk->itemiconHandle, ITEM_GetIndex( 1, ITEM_GET_ICON_PAL ), CLSYS_DRAW_MAIN, 
        32, 0, 1, wk->heapId );

  // �L����
  wk->clact_res[2][ SHOP_OBJ_RES_CHAR ] = GFL_CLGRP_CGR_Register( 
        wk->itemiconHandle, ITEM_GetIndex( 1, ITEM_GET_ICON_CGX ), 0, CLSYS_DRAW_MAIN, wk->heapId );

  // �Z��
  wk->clact_res[2][ SHOP_OBJ_RES_CELL ] = GFL_CLGRP_CELLANIM_Register( 
        wk->itemiconHandle, NARC_item_icon_itemicon_NCER, NARC_item_icon_itemicon_NANR, wk->heapId );
 
}

//----------------------------------------------------------------------------------
/**
 * @brief �ǂ����A�C�R�����\�[�X�����ւ�
 *
 * @param   wk      
 * @param   itemno  �ǂ���NO
 */
//----------------------------------------------------------------------------------
static void itemicon_resource_change( SHOP_BUY_APP_WORK *wk, u16 itemno )
{
  NNSG2dCharacterData* chara;
  NNSG2dPaletteData*   pltt;
  void *chara_buf,*pltt_buf;
  
  if(itemno<=ITEM_DATA_MAX){
    pltt_buf = GFL_ARCHDL_UTIL_LoadPalette( wk->itemiconHandle, 
                  ITEM_GetIndex( itemno, ITEM_GET_ICON_PAL ), &pltt, wk->heapId );
    chara_buf = GFL_ARCHDL_UTIL_LoadOBJCharacter( wk->itemiconHandle, 
                  ITEM_GetIndex( itemno, ITEM_GET_ICON_CGX ), 0, &chara, wk->heapId );
  
    GFL_CLGRP_PLTT_Replace( wk->clact_res[2][ SHOP_OBJ_RES_PLTT ], pltt, 1 );
    GFL_CLGRP_CGR_Replace( wk->clact_res[2][ SHOP_OBJ_RES_CHAR ], chara );
  
    GFL_HEAP_FreeMemory( pltt_buf );
    GFL_HEAP_FreeMemory( chara_buf );
    GFL_CLACT_WK_SetDrawEnable( wk->ClactWork[SHOP_OBJ_ITEM], TRUE );
  }else{
    GFL_CLACT_WK_SetDrawEnable( wk->ClactWork[SHOP_OBJ_ITEM], FALSE );
   
  }
}


//----------------------------------------------------------------------------------
/**
 * @brief OBJ�\���o�^
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void obj_set( SHOP_BUY_APP_WORK *wk )
{
  // �J�[�\��
  wk->ClactWork[SHOP_OBJ_CURSOR] = GFL_CLACT_WK_Create( wk->ClactUnit, 
      wk->clact_res[0][ SHOP_OBJ_RES_CHAR ], 
      wk->clact_res[0][ SHOP_OBJ_RES_PLTT ], 
      wk->clact_res[0][ SHOP_OBJ_RES_CELL ], 
      &shop_obj_param[0], CLSYS_DEFREND_MAIN, wk->heapId );

  // ����
  wk->ClactWork[SHOP_OBJ_ARROW_UP] = GFL_CLACT_WK_Create( wk->ClactUnit, 
      wk->clact_res[1][ SHOP_OBJ_RES_CHAR ], 
      wk->clact_res[0][ SHOP_OBJ_RES_PLTT ], 
      wk->clact_res[1][ SHOP_OBJ_RES_CELL ], 
      &shop_obj_param[1], CLSYS_DEFREND_MAIN, wk->heapId );

  // ���
  wk->ClactWork[SHOP_OBJ_ARROW_DOWN] = GFL_CLACT_WK_Create( wk->ClactUnit, 
      wk->clact_res[1][ SHOP_OBJ_RES_CHAR ], 
      wk->clact_res[0][ SHOP_OBJ_RES_PLTT ], 
      wk->clact_res[1][ SHOP_OBJ_RES_CELL ], 
      &shop_obj_param[2], CLSYS_DEFREND_MAIN, wk->heapId );

  // ���㉺
  wk->ClactWork[SHOP_OBJ_ARROW_UPDOWN] = GFL_CLACT_WK_Create( wk->ClactUnit, 
      wk->clact_res[1][ SHOP_OBJ_RES_CHAR ], 
      wk->clact_res[0][ SHOP_OBJ_RES_PLTT ], 
      wk->clact_res[1][ SHOP_OBJ_RES_CELL ], 
      &shop_obj_param[3], CLSYS_DEFREND_MAIN, wk->heapId );

  GFL_CLACT_WK_SetAutoAnmFlag( wk->ClactWork[SHOP_OBJ_ARROW_UPDOWN], TRUE );

  // �ǂ����A�C�R��
  wk->ClactWork[SHOP_OBJ_ITEM] = GFL_CLACT_WK_Create( wk->ClactUnit, 
      wk->clact_res[2][ SHOP_OBJ_RES_CHAR ], 
      wk->clact_res[2][ SHOP_OBJ_RES_PLTT ], 
      wk->clact_res[2][ SHOP_OBJ_RES_CELL ], 
      &shop_obj_param[4], CLSYS_DEFREND_MAIN, wk->heapId );

  // ���͔�\��
  GFL_CLACT_WK_SetDrawEnable( wk->ClactWork[SHOP_OBJ_ARROW_UP],   FALSE );
  GFL_CLACT_WK_SetDrawEnable( wk->ClactWork[SHOP_OBJ_ARROW_DOWN], FALSE );
  GFL_CLACT_WK_SetDrawEnable( wk->ClactWork[SHOP_OBJ_ARROW_UPDOWN], FALSE );

}

//----------------------------------------------------------------------------------
/**
 * @brief OBJ������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void obj_init( SHOP_BUY_APP_WORK *wk )
{
  // CLACT_UNIT�쐬
  wk->ClactUnit = GFL_CLACT_UNIT_Create( SHOP_OBJ_MAX, 
                                         SHOP_BUY_OBJ_BG_PRI, wk->heapId );
  // �V�X�e���ɘA��
  GFL_CLACT_UNIT_SetDrawEnable( wk->ClactUnit, TRUE );

  // �V���b�v���OBJ���\�[�X�ǂݍ���
  shop_resource_load( wk );

  // �A�C�e���A�C�R���̃t�@�C���n���h���J�����ςȂ�(obj_end�ŉ��)
  wk->itemiconHandle = GFL_ARC_OpenDataHandle( ARCID_ITEMICON, wk->heapId );

  // ����A�C�R�����\�[�X�ǂݍ���
  itemicon_resource_load( wk );

  // OBJ�o�^
  obj_set( wk );
  
}



//----------------------------------------------------------------------------------
/**
 * @brief OBJ�폜
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void obj_del( SHOP_BUY_APP_WORK *wk )
{

  // OBJ���\�[�X�S���
  GFL_CLGRP_CELLANIM_Release( wk->clact_res[2][ SHOP_OBJ_RES_CELL ] );
  GFL_CLGRP_CELLANIM_Release( wk->clact_res[1][ SHOP_OBJ_RES_CELL ] );
  GFL_CLGRP_CELLANIM_Release( wk->clact_res[0][ SHOP_OBJ_RES_CELL ] );

  GFL_CLGRP_CGR_Release( wk->clact_res[2][ SHOP_OBJ_RES_CHAR ] );
  GFL_CLGRP_CGR_Release( wk->clact_res[1][ SHOP_OBJ_RES_CHAR ] );
  GFL_CLGRP_CGR_Release( wk->clact_res[0][ SHOP_OBJ_RES_CHAR ] );

  GFL_CLGRP_PLTT_Release( wk->clact_res[2][ SHOP_OBJ_RES_PLTT ] );
  GFL_CLGRP_PLTT_Release( wk->clact_res[0][ SHOP_OBJ_RES_PLTT ] );

  // �ǂ����A�C�R���n���h������
  GFL_ARC_CloseDataHandle( wk->itemiconHandle );

  // �V���b�v�pCLUNIT���
  GFL_CLACT_UNIT_Delete( wk->ClactUnit );

}


//----------------------------------------------------------------------------------
/**
 * @brief BMPWIN������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void bmpwin_init( SHOP_BUY_APP_WORK *wk )
{
  int i;
  const BMPWIN_DAT *dat;

  for(i=0;i<SHOP_BUY_BMPWIN_MAX;i++){
    dat = Window0BmpDataTable[i];
    wk->win[i] = GFL_BMPWIN_Create( dat->frame, dat->x, dat->y, dat->w, dat->h, 
                        dat->pal, GFL_BMP_CHRAREA_GET_B );
  }

  {
    // �����Â���
    STRBUF *str = GFL_MSG_CreateString( wk->shopMsgData, mes_shop_05_01 );
    PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp( wk->win[SHOP_BUY_BMPWIN_OKODUKAI]), 
                                            0, 0, str, wk->font, 
                                            PRINTSYS_LSB_Make(15,2,15) );
    GFL_STR_DeleteBuffer( str );
  }

  // PRINTUTIL�Ɗ֘A�t��
  PRINT_UTIL_Setup( &wk->printUtil, wk->win[SHOP_BUY_BMPWIN_LIST] );
  wk->printQue = PRINTSYS_QUE_Create( wk->heapId );

  GFL_BMPWIN_MakeTransWindow( wk->win[SHOP_BUY_BMPWIN_OKODUKAI] );
  GFL_BMPWIN_MakeTransWindow( wk->win[SHOP_BUY_BMPWIN_MONEY] );
}

//----------------------------------------------------------------------------------
/**
 * @brief BMPWIN���
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void bmpwin_exit( SHOP_BUY_APP_WORK *wk )
{
  int i;

  PRINTSYS_QUE_Delete( wk->printQue );

  for(i=0;i<SHOP_BUY_BMPWIN_MAX;i++){
    GFL_BMPWIN_Delete( wk->win[i] );
  }
}


//----------------------------------------------------------------------------------
/**
 * @brief �������\��
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void print_mygold( SHOP_BUY_APP_WORK *wk )
{
  STRBUF *str    = GFL_MSG_CreateString( wk->shopMsgData, mes_shop_05_02 );
  STRBUF *expand = GFL_STR_CreateBuffer( SHOP_MYGOLD_STR_MAX, wk->heapId );

  // �c���擾
  WORDSET_RegisterNumber( wk->wordSet, 0, MyStatus_GetGold(wk->mystatus), 6, 
                          STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
  WORDSET_ExpandStr( wk->wordSet, expand, str );

  // �`��
  {
    GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( wk->win[SHOP_BUY_BMPWIN_MONEY]);
    GFL_BMP_Clear( bmp, 0 );
    PRINTSYS_PrintColor( bmp,  0, 0, expand, wk->font, PRINTSYS_LSB_Make(15,2,15) );
  
  }

  GFL_BMPWIN_MakeTransWindow( wk->win[SHOP_BUY_BMPWIN_MONEY] );

  GFL_STR_DeleteBuffer( expand );
  GFL_STR_DeleteBuffer( str );
}


//----------------------------------------------------------------------------------
/**
 * @brief �w��̃A�C�e�����������Ă��邩�\������
 *
 * @param   wk      
 * @param   itemno  �ǂ���NO
 */
//----------------------------------------------------------------------------------
static void print_carry_item( SHOP_BUY_APP_WORK *wk, u16 itemno )
{
  STRBUF *str    = GFL_MSG_CreateString( wk->shopMsgData, mes_shop_06_01 );
  STRBUF *expand = GFL_STR_CreateBuffer( SHOP_ITEMNUM_STR_MAX, wk->heapId );


  BmpWinFrame_Write( wk->win[SHOP_BUY_BMPWIN_NUM], WINDOW_TRANS_OFF, 
                      1, MENU_WINDOW_PAL_OFFSET );

  // ���o�^
  WORDSET_RegisterNumber( wk->wordSet, 0, MYITEM_GetItemNum(wk->myitem, itemno, wk->heapId), 
                          3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
  WORDSET_ExpandStr( wk->wordSet, expand, str );

  // �`��
  {
    GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( wk->win[SHOP_BUY_BMPWIN_NUM]);
    GFL_BMP_Clear( bmp, 15 );
    PRINTSYS_PrintColor( bmp,  0, 0, expand, wk->font, PRINTSYS_LSB_Make(1,2,15) );
  
  }

  GFL_BMPWIN_MakeTransWindow( wk->win[SHOP_BUY_BMPWIN_NUM] );

  GFL_STR_DeleteBuffer( expand );
  GFL_STR_DeleteBuffer( str );

}

//----------------------------------------------------------------------------------
/**
 * @brief �ǂ��������\��
 *
 * @param   wk    
 * @param   itemno    
 */
//----------------------------------------------------------------------------------
static void print_iteminfo( SHOP_BUY_APP_WORK *wk, u16 itemno )
{
  STRBUF *str;    

  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( wk->win[SHOP_BUY_BMPWIN_ITEMINFO]);
  GFL_BMP_Clear( bmp, 0 );

  // �`��(���݂��Ȃ��ꍇ�̓N���A)
  if(itemno<=ITEM_DATA_MAX){
    str = GFL_MSG_CreateString( wk->itemInfoMsgData, itemno );
    PRINTSYS_PrintColor( bmp,  0, 0, str, wk->font, PRINTSYS_LSB_Make(15,2,0) );
    GFL_STR_DeleteBuffer( str );
  }

  GFL_BMPWIN_MakeTransWindow( wk->win[SHOP_BUY_BMPWIN_ITEMINFO] );

}


//----------------------------------------------------------------------------------
/**
 * @brief �u�����@�H�H�H�H�H�H�~�v�\��
 *
 * @param   wk          
 * @param   number  �ǂ����̐�
 * @param   one_price   �����̔��l
 */
//----------------------------------------------------------------------------------
static void print_multiitem_price( SHOP_BUY_APP_WORK *wk, u16 number, int one_price )
{

  STRBUF *kake_str = GFL_MSG_CreateString( wk->shopMsgData, mes_shop_07_01 );
  STRBUF *yen_str   = GFL_MSG_CreateString( wk->shopMsgData, mes_shop_07_02 );
  STRBUF *expand = GFL_STR_CreateBuffer( SHOP_ITEMNUM_STR_MAX, wk->heapId );

  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( wk->win[SHOP_BUY_BMPWIN_PRICE]);
  GFL_BMP_Clear( bmp, 15 );


  BmpWinFrame_Write( wk->win[SHOP_BUY_BMPWIN_PRICE], WINDOW_TRANS_OFF, 
                      1, MENU_WINDOW_PAL_OFFSET );

  // �u���H�H�H�v
  WORDSET_RegisterNumber( wk->wordSet, 0, number, 
                          2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
  WORDSET_ExpandStr( wk->wordSet, expand, kake_str );

  PRINTSYS_PrintColor( bmp,  0, 0, expand, wk->font, PRINTSYS_LSB_Make(1,2,15) );

  // �u�H�H�H�~�v
  WORDSET_RegisterNumber( wk->wordSet, 0, number*one_price, 
                          6, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
  WORDSET_ExpandStr( wk->wordSet, expand, yen_str );

  PRINTSYS_PrintColor( bmp,  13*3+2, 0, expand, wk->font, PRINTSYS_LSB_Make(1,2,15) );
  

  GFL_BMPWIN_MakeTransWindow( wk->win[SHOP_BUY_BMPWIN_PRICE] );

  GFL_STR_DeleteBuffer( expand );
  GFL_STR_DeleteBuffer( yen_str );
  GFL_STR_DeleteBuffer( kake_str );

  
}

//----------------------------------------------------------------------------------
/**
 * @brief BMPWIN���X�g������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void bmpwin_list_init( SHOP_BUY_APP_WORK *wk )
{
  BMPMENULIST_HEADER header;

  
  header.list = wk->list;   //�\�������f�[�^�|�C���^
                            //�J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
  header.call_back = move_callback;  // void  (*call_back)(BMPMENULIST_WORK * wk,u32 param,u8 mode);
                            //���\�����Ƃ̃R�[���o�b�N�֐�
  header.icon      = line_callback;  // void  (*icon)(BMPMENULIST_WORK * wk,u32 param,u8 y);

  header.win       = wk->win[SHOP_BUY_BMPWIN_LIST];
  
  header.count     = BmpMenuWork_GetListMax(wk->list);    //���X�g���ڐ�
  header.line      = 7; //�\���ő區�ڐ�
  header.rabel_x   = 0; //���x���\���w���W
  header.data_x    = 0; //���ڕ\���w���W
  header.cursor_x  = 0; //�J�[�\���\���w���W
  header.line_y    = 8; //�\���x���W
  header.f_col     = 12; //�\�������F
  header.b_col     = 0;  //�\���w�i�F
  header.s_col     = 13; //�\�������e�F
  header.msg_spc   = 0;  //�����Ԋu�w
  header.line_spc  = 0; //�����Ԋu�x
  header.page_skip = BMPMENULIST_NO_SKIP; //�y�[�W�X�L�b�v�^�C�v
  header.font      = 0; //�����w��(�{����u8�����ǂ���Ȃɍ��Ȃ��Ǝv���̂�)
  header.c_disp_f  = 1; //�a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)
  header.work      = wk;// callback���ɎQ�Ƃ��郏�[�N
  
  header.font_size_x  = 12;   //�����T�C�YX(�h�b�g
  header.font_size_y  = 16;   //�����T�C�YY(�h�b�g
  header.msgdata      = NULL; //�\���Ɏg�p���郁�b�Z�[�W�o�b�t�@
  header.print_util   = &wk->printUtil; //�\���Ɏg�p����v�����g���[�e�B���e�B
  header.print_que    = wk->printQue; //�\���Ɏg�p����v�����g�L���[
  header.font_handle  = wk->font; //�\���Ɏg�p����t�H���g�n���h��
  

   wk->menuList = BmpMenuList_Set( &header, 0, 0, wk->heapId );

  GFL_BMPWIN_MakeTransWindow( wk->win[SHOP_BUY_BMPWIN_LIST] );
  
}

//----------------------------------------------------------------------------------
/**
 * @brief �P�s�\���R�[���o�b�N�i��ɒl�i�\������)
 *
 * @param   wk      
 * @param   param   �ǂ���NO
 * @param   y       �\��Y���W
 */
//----------------------------------------------------------------------------------
static void line_callback(BMPMENULIST_WORK * wk, u32 param, u8 y )
{
  SHOP_BUY_APP_WORK *sbw = BmpMenuList_GetWorkPtr( wk );

  // �u��߂�v�ȊO�ɂ͒l�i��\��
  if(param!=BMPMENULIST_CANCEL){
    WORDSET_RegisterNumber( sbw->wordSet, 1, ITEM_GetParam( param, ITEM_PRM_PRICE, sbw->heapId ), 
                            4, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );  
    WORDSET_ExpandStr( sbw->wordSet, sbw->expandBuf, sbw->priceStr );
  
    PRINTSYS_PrintQueColor( sbw->printQue, GFL_BMPWIN_GetBmp( sbw->win[SHOP_BUY_BMPWIN_LIST]), 
                          14*8, y, sbw->expandBuf, sbw->font, PRINTSYS_LSB_Make(12,13,0) );
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ړ����̃R�[���o�b�N
 *
 * @param   wk      
 * @param   param   
 * @param   mode    
 */
//----------------------------------------------------------------------------------
static void move_callback( BMPMENULIST_WORK * wk, u32 param, u8 mode)
{
  SHOP_BUY_APP_WORK *sbw = BmpMenuList_GetWorkPtr( wk );
  GFL_CLACTPOS clpos;
  u16 list, cursor;
  
  // �J�[�\���ʒu�擾
  BmpMenuList_PosGet( wk, &list, &cursor );

  clpos.x = shop_obj_param[0].pos_x; clpos.y = shop_obj_param[0].pos_y+16*cursor;

  GFL_CLACT_WK_SetPos( sbw->ClactWork[SHOP_OBJ_CURSOR], &clpos, CLSYS_DEFREND_MAIN );
  
  // �ǂ���������������
  print_iteminfo( sbw, param );
  itemicon_resource_change( sbw, param );

}

//----------------------------------------------------------------------------------
/**
 * @brief BMPLIST�I���E�������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void bmpwin_list_exit( SHOP_BUY_APP_WORK *wk )
{
  u16 list, cursor;
  
  // 
  BmpMenuList_Exit( wk->menuList, &list, &cursor );

}


//----------------------------------------------------------------------------------
/**
 * @brief �v�����g�R�[���o�b�N
 *
 * @param   param   
 *
 * @retval  BOOL    
 */
//----------------------------------------------------------------------------------
static BOOL printBuyMsgCallBack( u32 param )
{
  if(param==1){
//    PMSND_PlaySE( SEQ_SE_SYS_22 );
  }

  return FALSE;
}


//----------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�\��
 *
 * @param   wk    
 * @param   itemno    
 */
//----------------------------------------------------------------------------------
static void ShopPrintMsg( SHOP_BUY_APP_WORK *wk, int strId, u16 itemno )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[SHOP_BUY_BMPWIN_TALKMSG]), 15 );
  BmpWinFrame_Write( wk->win[SHOP_BUY_BMPWIN_TALKMSG], WINDOW_TRANS_ON, 
                    1, MENU_WINDOW_PAL_OFFSET );

  WORDSET_RegisterItemName( wk->wordSet, 0, itemno );
  WORDSET_RegisterItemPocketName( wk->wordSet, 1, 
                                    ITEM_GetParam( itemno, ITEM_PRM_POCKET, wk->heapId ) );
  {
    STRBUF *str    = GFL_MSG_CreateString( wk->shopMsgData, strId );
    WORDSET_ExpandStr( wk->wordSet, wk->expandBuf, str );
    GFL_STR_DeleteBuffer( str );
  }
  
  wk->printStream = PRINTSYS_PrintStreamCallBack(
    wk->win[SHOP_BUY_BMPWIN_TALKMSG], 0, 0, wk->expandBuf, wk->font,
    MSGSPEED_GetWait(), wk->pMsgTcblSys,
    0, wk->heapId, 0xffff, printBuyMsgCallBack );

  GFL_BMPWIN_MakeTransWindow(wk->win[SHOP_BUY_BMPWIN_TALKMSG]);

}

//----------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�\��
 *
 * @param   wk    
 * @param   itemno    
 */
//----------------------------------------------------------------------------------
static void ShopDecideMsg( SHOP_BUY_APP_WORK *wk, int strId, u16 itemno, u16 num )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[SHOP_BUY_BMPWIN_TALKMSG]), 15 );
  BmpWinFrame_Write( wk->win[SHOP_BUY_BMPWIN_TALKMSG], WINDOW_TRANS_ON, 
                    1, MENU_WINDOW_PAL_OFFSET );

  WORDSET_RegisterItemName( wk->wordSet, 0, itemno );
  WORDSET_RegisterNumber( wk->wordSet, 1, num, 
                            2, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );  

  WORDSET_RegisterNumber( wk->wordSet, 2, num*ITEM_GetParam( itemno, ITEM_PRM_PRICE, wk->heapId ),
                            6, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );  

  {
    STRBUF *str    = GFL_MSG_CreateString( wk->shopMsgData, strId );
    WORDSET_ExpandStr( wk->wordSet, wk->expandBuf, str );
    GFL_STR_DeleteBuffer( str );
  }
  
  wk->printStream = PRINTSYS_PrintStreamCallBack(
    wk->win[SHOP_BUY_BMPWIN_TALKMSG], 0, 0, wk->expandBuf, wk->font,
    MSGSPEED_GetWait(), wk->pMsgTcblSys,
    0, wk->heapId, 0xffff, NULL );

  GFL_BMPWIN_MakeTransWindow(wk->win[SHOP_BUY_BMPWIN_TALKMSG]);

}


#define APP_NUMSEL_NONE ( 0 )
#define APP_NUMSEL_UP   ( 1 )
#define APP_NUMSEL_DOWN ( 2 )
//==============================================================================
/**
 * @brief   ���l�ɎZ�o��max,min�ɑ΂��ĉ�荞�݌v�Z���s��
 *
 * @param   num   ���l���[�N�ւ̃|�C���^
 * @param   max   ���l�ő�
 * @param   calc  +1,-1,+10,-10�̂ݎ󂯕t����
 *
 * @retval  u8    �v�Z����
 */
//==============================================================================
static u8 NumArroundCheck( s16 *num, u16 max, int calc )
{
  s16 tmp;

  tmp = *num;

  switch(calc){
  case -1:
    *num -= 1;
    if( *num <= 0 ){ *num = max; }
    if( *num == tmp ){ return APP_NUMSEL_NONE; }
    return APP_NUMSEL_DOWN;
    break;
  case -10:
    *num -= 10;
    if( *num <= 0 ){ *num = 1; }
    if( *num == tmp ){ return APP_NUMSEL_NONE; }
    return APP_NUMSEL_DOWN;
    break;
  case 1:
    *num += 1;
    if( *num > max ){ *num = 1; }
    if( *num == tmp ){ return APP_NUMSEL_NONE; }
    return APP_NUMSEL_UP;
    break;
  case 10:
    *num += 10;
    if( *num > max ){ *num = max; }
    if( *num == tmp ){ return APP_NUMSEL_NONE; }
    return APP_NUMSEL_UP;
    break;
  }
  return APP_NUMSEL_NONE;
}



//----------------------------------------------------------------------------------
/**
 * @brief ���I�����̃L�[����
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int price_key_control( SHOP_BUY_APP_WORK *wk )
{
  if(GFL_UI_KEY_GetRepeat() & PAD_KEY_UP){
    if(NumArroundCheck( &wk->item_multi, wk->buy_max, 1 )!=APP_NUMSEL_NONE){
      PMSND_PlaySE( SEQ_SE_SELECT1 );
      print_multiitem_price( wk, wk->item_multi, wk->price );
    }
  }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN){
    if(NumArroundCheck( &wk->item_multi, wk->buy_max, -1 )!=APP_NUMSEL_NONE){
      PMSND_PlaySE( SEQ_SE_SELECT1 );
      print_multiitem_price( wk, wk->item_multi, wk->price );
    }
  }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT){
    if(NumArroundCheck( &wk->item_multi, wk->buy_max, -10 )!=APP_NUMSEL_NONE){
      PMSND_PlaySE( SEQ_SE_SELECT1 );
      print_multiitem_price( wk, wk->item_multi, wk->price );
    }
  }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT){
    if(NumArroundCheck( &wk->item_multi, wk->buy_max,  10 )!=APP_NUMSEL_NONE){
      PMSND_PlaySE( SEQ_SE_SELECT1 );
      print_multiitem_price( wk, wk->item_multi, wk->price );
    }
  }
  if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_DECIDE){
      PMSND_PlaySE( SEQ_SE_DECIDE1 );
      ShopDecideMsg( wk, mes_shop_02_03, wk->selectitem, wk->item_multi );
      wk->next = SHOPBUY_SEQ_YESNO;
      return SHOPBUY_SEQ_MSG_WAIT;
  }else if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_CANCEL){
      PMSND_PlaySE( SEQ_SE_CANCEL1 );
    return SHOPBUY_SEQ_BACK;
  }

  return SHOPBUY_SEQ_DECIDE_NUM;
}


//----------------------------------------------------------------------------------
/**
 * @brief �T�u���j���[�̃X�N���[���N���A
 *
 * @param   type    
 */
//----------------------------------------------------------------------------------
static void submenu_screen_clear( int type )
{
  static const clear_rect[][4]={
    { 0,12,32,12 },   // SCREEN_SUBMENU_AND_INFOWIN
    { 0,12,32, 6 },   // SCREEN_SUBMENU_ONLY
  };

  // �w��͈̔͂̃X�N���[���N���A
  GFL_BG_FillScreen( GFL_BG_FRAME1_M, 0, 
    clear_rect[type][0], clear_rect[type][1],
    clear_rect[type][2], clear_rect[type][3], 0  );

}


/*

  BmpWinFrame_Write( wk->win[SHOP_BUY_BMPWIN_PRICE], WINDOW_TRANS_ON, 
                      1, MENU_WINDOW_PAL_OFFSET );


field/scrcmd_menuwin.c

252�s�ڂ�����

EvCmdTalkMsg()��TalkMsgWait
*/