//============================================================================================
/**
 * @file	  itemmenu.h
 * @brief	  �A�C�e�����j���[
 * @author	k.ohno
 * @date	  2009.06.30
 */
//============================================================================================


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
#include "bag.naix"
#include "msg/msg_d_field.h"
#include "msg/msg_bag.h"
#include "msg/msg_itempocket.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "field/fieldmap.h"
#include "font/font.naix" //NARC_font_large_nftr
#include "sound/pm_sndsys.h"
#include "system/wipe.h"

#include "itemmenu.h"
#include "itemmenu_local.h"
#include "app/itemuse.h"
#include "savedata/mystatus.h"

//------------------------------------------------------------------
//------------------------------------------------------------------
#include "system/main.h"			//GFL_HEAPID_APP�Q��

//============================================================================================
//============================================================================================

#define _1CHAR (8)


//�Ȃ�т� savedata/myitem_savedata.h�ɏ���
// �R�[���o�b�N��_BttnCallBack
static const GFL_UI_TP_HITTBL bttndata[] = {  //�㉺���E

#include "bag_btnpos.h"  //�o�b�O�̊G�̃^�b�`�G���A

  {	21*_1CHAR,  24*_1CHAR,   1*_1CHAR,  3*_1CHAR },  //��
  {	21*_1CHAR,  24*_1CHAR,  15*_1CHAR, 18*_1CHAR },  //�E

  {	21*_1CHAR,  24*_1CHAR,  26*_1CHAR, 28*_1CHAR },  //x
  {	21*_1CHAR,  24*_1CHAR,  30*_1CHAR, 32*_1CHAR },  //���^�[��

  {	1*_1CHAR+4,   4*_1CHAR+3,  18*_1CHAR, 28*_1CHAR },  //�A�C�e���ꗗ�G���A1
  {	4*_1CHAR+4,   7*_1CHAR+3,  18*_1CHAR, 28*_1CHAR },  //�A�C�e���ꗗ�G���A2
  {	7*_1CHAR+4,  11*_1CHAR+3,  18*_1CHAR, 28*_1CHAR },  //�A�C�e���ꗗ�G���A3
  {11*_1CHAR+4,  14*_1CHAR+3,  18*_1CHAR, 28*_1CHAR },  //�A�C�e���ꗗ�G���A4
  {14*_1CHAR+4,  17*_1CHAR+3,  18*_1CHAR, 28*_1CHAR },  //�A�C�e���ꗗ�G���A5
  {17*_1CHAR+4,  20*_1CHAR+3,  18*_1CHAR, 28*_1CHAR },  //�A�C�e���ꗗ�G���A6

  {	1*_1CHAR+4,   4*_1CHAR+3,  15*_1CHAR, 18*_1CHAR },  //�������Ȃ��̃`�F�b�N�G���A1
  {	4*_1CHAR+4,   7*_1CHAR+3,  15*_1CHAR, 18*_1CHAR },  //�������Ȃ��̃`�F�b�N�G���A2
  {	7*_1CHAR+4,  11*_1CHAR+3,  15*_1CHAR, 18*_1CHAR },  //�������Ȃ��̃`�F�b�N�G���A3
  {11*_1CHAR+4,  14*_1CHAR+3,  15*_1CHAR, 18*_1CHAR },  //�������Ȃ��̃`�F�b�N�G���A4
  {14*_1CHAR+4,  17*_1CHAR+3,  15*_1CHAR, 18*_1CHAR },  //�������Ȃ��̃`�F�b�N�G���A5
  {17*_1CHAR+4,  20*_1CHAR+3,  15*_1CHAR, 18*_1CHAR },  //�������Ȃ��̃`�F�b�N�G���A6

  {GFL_UI_TP_HIT_END,0,0,0},		 //�I���f�[�^
};



//-----------------------------------------------
//static ��`
//-----------------------------------------------


static void _changeState(FIELD_ITEMMENU_WORK* pWork,StateFunc* state);
static void _changeStateDebug(FIELD_ITEMMENU_WORK* pWork,StateFunc* state, int line);
static void _itemNumSelectMenu(FIELD_ITEMMENU_WORK* pWork);
static void _lineCallback(BMPMENULIST_WORK * mpWork,u32 param,u8 y);
static void _windowRewrite(FIELD_ITEMMENU_WORK* pWork);
static void _itemUseWindowRewrite(FIELD_ITEMMENU_WORK* pWork);
static void _itemUseMenu(FIELD_ITEMMENU_WORK* pWork);
static void _itemKindSelectMenu(FIELD_ITEMMENU_WORK* pWork);


#ifdef PM_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeStateDebug(pWork ,state, __LINE__)
#else  //_NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeState(pWork ,state)
#endif //_NET_DEBUG

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _changeState(FIELD_ITEMMENU_WORK* pWork,StateFunc state)
{
  pWork->state = state;
  GFL_UI_KEY_SetRepeatSpeed(8, 15);  //���ɖ߂�
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @retval  none
 */
//------------------------------------------------------------------------------
#ifdef PM_DEBUG
static void _changeStateDebug(FIELD_ITEMMENU_WORK* pWork,StateFunc state, int line)
{
  OS_Printf("BAG STATE: %d\n",line);
  _changeState(pWork, state);
}
#endif


#define	FBMP_COL_NULL		(0)
#define	FBMP_COL_BLACK		(1)
#define	FBMP_COL_BLK_SDW	(2)
#define	FBMP_COL_RED		(3)
#define	FBMP_COL_RED_SDW	(4)
#define	FBMP_COL_GREEN		(5)
#define	FBMP_COL_GRN_SDW	(6)
#define	FBMP_COL_BLUE		(7)
#define	FBMP_COL_BLU_SDW	(8)
#define	FBMP_COL_PINK		(9)
#define	FBMP_COL_PNK_SDW	(10)
#define	FBMP_COL_WHITE		(15)
#define _BMPMENULIST_FONTSIZE   (16)




///�I�����j���[�̃��X�g
static BMPMENULIST_HEADER _itemMenuListHeader = {
  NULL,			// �\�������f�[�^�|�C���^
  NULL,					// �J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
  NULL,					// ���\�����Ƃ̃R�[���o�b�N�֐�
  NULL,                      // GF_BGL_BMPWIN* win
  9,// ���X�g���ڐ�
  9,// �\���ő區�ڐ�
  0,						// ���x���\���w���W
  16,						// ���ڕ\���w���W
  0,						// �J�[�\���\���w���W
  0,						// �\���x���W
  FBMP_COL_BLACK,			// �����F
  FBMP_COL_WHITE,			// �w�i�F
  FBMP_COL_BLK_SDW,		// �����e�F
  0,						// �����Ԋu�w
  18,						// �����Ԋu�x
  BMPMENULIST_NO_SKIP,		// �y�[�W�X�L�b�v�^�C�v
  0,//FONT_SYSTEM,				// �����w��
  0,						// �a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)
  NULL,                   // work
  _BMPMENULIST_FONTSIZE,			//�����T�C�YX(�h�b�g
  _BMPMENULIST_FONTSIZE,			//�����T�C�YY(�h�b�g
  NULL,		//�\���Ɏg�p���郁�b�Z�[�W�o�b�t
  NULL,		//�\���Ɏg�p����v�����g���[�e�B
  NULL,		//�\���Ɏg�p����v�����g�L���[
  NULL,		//�\���Ɏg�p����t�H���g�n���h��
};

static void _windowCreate(FIELD_ITEMMENU_WORK* pWork)
{
  _windowRewrite(pWork);

}


static void _windowRewrite(FIELD_ITEMMENU_WORK* pWork)
{

  ITEMDISP_upMessageRewrite(pWork);
  ITEMDISP_WazaInfoWindowChange(pWork);
  ITEMDISP_CellMessagePrint(pWork);
  pWork->bChange = TRUE;


}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�b�g�ɂ���A�C�e����Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------

ITEM_ST* ITEMMENU_GetItem(FIELD_ITEMMENU_WORK* pWork, int no)
{
  ITEM_ST * item;

  if(pWork->moveMode){
    item = &pWork->ScrollItem[no];
  }
  else{
    item = MYITEM_PosItemGet( pWork->pMyItem, pWork->pocketno, no );
  }
  return item;
}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�b�g�ɂ���A�C�e���̑�����Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------

int ITEMMENU_GetItemPocketNumber(FIELD_ITEMMENU_WORK* pWork)
{
  int length = 0;

  if(pWork->moveMode){
    length = MYITEM_GetItemThisPocketNumber(pWork->ScrollItem, BAG_MYITEM_MAX);

  }
  else{
    length = MYITEM_GetItemPocketNumber( pWork->pMyItem, pWork->pocketno);
  }
  return length;
}


static void _ItemChangeSingle(FIELD_ITEMMENU_WORK* pWork, int no1, int no2 )
{
  ITEM_ST temp;
  GFL_STD_MemCopy(&pWork->ScrollItem[no1], &temp, sizeof(ITEM_ST));
  GFL_STD_MemCopy(&pWork->ScrollItem[no2],&pWork->ScrollItem[no1], sizeof(ITEM_ST));
  GFL_STD_MemCopy(&temp, &pWork->ScrollItem[no2], sizeof(ITEM_ST));
}


static void _ItemChange(FIELD_ITEMMENU_WORK* pWork, int no1, int no2 )
{
  int i;

  if(no1 == no2){
    return;
  }
  else if(no1 < no2){
    for(i = no1; i < no2 ; i++ ){
      _ItemChangeSingle(pWork, i, i+1);
    }
  }
  else{
    for(i = no1; i > no2 ; i-- ){
      _ItemChangeSingle(pWork, i, i-1);
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�b�g���̕\��
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _pocketMessageDisp(FIELD_ITEMMENU_WORK* pWork,int newpocket)
{
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->pocketNameWin), 0 );
  GFL_FONTSYS_SetColor( 0xf, 0xe, 0 );
  GFL_MSG_GetString(pWork->MsgManagerPocket, msg_pocket_001+newpocket, pWork->pStrBuf );

  {  //�Z���^�����O
    u32 dot =PRINTSYS_GetStrWidth(pWork->pStrBuf, pWork->fontHandle, 0);
    dot = (80 - dot )/2;
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->pocketNameWin), dot, 4, pWork->pStrBuf, pWork->fontHandle);
  }
  GFL_BMPWIN_TransVramCharacter(pWork->pocketNameWin);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_M);
}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�b�g�ɉ������J�[�\���ʒu���o���Ă��� �V�����|�P�b�g�ł̃J�[�\���ʒu�������o��
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _pocketCursorChange(FIELD_ITEMMENU_WORK* pWork,int oldpocket, int newpocket)
{
  s16 cur,scr;

  MYITEM_FieldBagPocketSet(pWork->pBagCursor, oldpocket);
  MYITEM_FieldBagCursorSet(pWork->pBagCursor, oldpocket, pWork->curpos, pWork->oamlistpos+1 );
  MYITEM_FieldBagCursorGet(pWork->pBagCursor, newpocket, &cur, &scr );
  pWork->curpos = cur;
  pWork->oamlistpos = scr - 1;
  ITEMDISP_scrollCursorChangePos(pWork, ITEMMENU_GetItemIndex(pWork));
  _pocketMessageDisp(pWork, newpocket);
  ITEMDISP_ChangePocketCell( pWork,newpocket );
  ITEMDISP_ItemInfoWindowChange(pWork,newpocket);


}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e���C���f�b�N�X���J�[�\���ƃ��X�g�̈ʒu����v�Z
 * @retval  none
 */
//------------------------------------------------------------------------------

int ITEMMENU_GetItemIndex(FIELD_ITEMMENU_WORK* pWork)
{
  return pWork->curpos + pWork->oamlistpos + 1;
}

//------------------------------------------------------------------------------
/**
 * @brief   �L�[�����������ꂽ�Ƃ��̏���
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _posplus(FIELD_ITEMMENU_WORK* pWork, int length)
{
  BOOL bChange = FALSE;

  if((pWork->curpos==4) && ((pWork->oamlistpos+7) < length)){
    //�J�[�\���͂��̂܂܂Ń��X�g���ړ�
    pWork->oamlistpos++;
    bChange = TRUE;
  }
  else if((pWork->curpos==4) && ((pWork->curpos+1) < length)){
    //���X�g�̏I�[�܂ŗ����̂ŃJ�[�\�����ړ�
    pWork->curpos++;
    bChange = TRUE;
  }
  else if((pWork->curpos!=5) && ((pWork->curpos+1) < length)){
    pWork->curpos++;
    bChange = TRUE;
  }
  return bChange;
}


static BOOL _posminus(FIELD_ITEMMENU_WORK* pWork, int length)
{
  BOOL bChange = FALSE;

  if((pWork->curpos==1) && (pWork->oamlistpos!=-1)){
    //�J�[�\���͂��̂܂܂Ń��X�g���ړ�
    pWork->oamlistpos--;
    bChange = TRUE;
  }
  else if((pWork->curpos==1)){
    //���X�g�̏I�[�܂ŗ����̂ŃJ�[�\�����ړ�
    pWork->curpos--;
    bChange = TRUE;
  }
  else if(pWork->curpos != 0){
    pWork->curpos--;
    bChange = TRUE;
  }
  return bChange;
}


//------------------------------------------------------------------------------
/**
 * @brief   �X�N���[���̏���
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _itemScrollCheck(FIELD_ITEMMENU_WORK* pWork)
{
  u32 x,y,i;
  int ymax = _SCROLL_BOTTOM_Y - _SCROLL_TOP_Y;

  if(GFL_UI_TP_GetPointCont(&x, &y) == TRUE){
    if((y <= _SCROLL_TOP_Y) || (y >= _SCROLL_BOTTOM_Y)){
      return FALSE;
    }
    if((x >= (32*8)) || (x <= (28*8)) ){
      return FALSE;
    }
    {
      int length = ITEMMENU_GetItemPocketNumber( pWork);
      int num = (length * (y-_SCROLL_TOP_Y)) / ymax;

      pWork->curpos = 0;
      pWork->oamlistpos = -1;
      for(i = 0 ; i < num ; i++){
        _posplus(pWork, length);
      }
    }
    return TRUE;
  }
  return FALSE;
}


//------------------------------------------------------------------------------
/**
 * @brief   �L�[�̓����̏���
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _keyChangeItemCheck(FIELD_ITEMMENU_WORK* pWork)
{
  BOOL bChange = FALSE;
  int nowno = ITEMMENU_GetItemIndex(pWork);
  {
    int pos = pWork->curpos;
    int length = ITEMMENU_GetItemPocketNumber( pWork);

    if(GFL_UI_KEY_GetRepeat()== PAD_KEY_DOWN){
      bChange = _posplus(pWork, length);
    }
    if(GFL_UI_KEY_GetRepeat()== PAD_KEY_UP){
      bChange = _posminus(pWork, length);
    }
  }
  if(bChange){
    int newno = ITEMMENU_GetItemIndex(pWork);
    _ItemChange(pWork, nowno, newno);
  }
  return bChange;
}



//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e���������̃L�[�̓����̏���
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _keyMoveCheck(FIELD_ITEMMENU_WORK* pWork)
{
  BOOL bChange = FALSE;
  {
    int pos = pWork->curpos;
    int length = ITEMMENU_GetItemPocketNumber( pWork);

    if(GFL_UI_KEY_GetRepeat()== PAD_KEY_DOWN){
      bChange = _posplus(pWork, length);
    }
    if(GFL_UI_KEY_GetRepeat()== PAD_KEY_UP){
      bChange = _posminus(pWork, length);
    }
  }
  return bChange;
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e���ړ����[�h���̃A�C�e�������̃^�b�`����
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _itemMovePositionTouchItem(FIELD_ITEMMENU_WORK* pWork)
{
  int nowno = ITEMMENU_GetItemIndex(pWork);
  u32 x,y,i;
  int ymax = _SCROLL_BOTTOM_Y - _SCROLL_TOP_Y;

  if(GFL_UI_TP_GetPointCont(&x, &y) == TRUE){
    if((y <= _SCROLL_TOP_Y)  || (y >= _SCROLL_BOTTOM_Y)){
      return FALSE;
    }
    if((x >= (30*8)) || (x <= (18*8)) ){
      return FALSE;
    }
    {
      int length = ITEMMENU_GetItemPocketNumber( pWork);
      BOOL bChange=FALSE;
      int num = (6 * (y-_SCROLL_TOP_Y)) / ymax;  //�J�[�\�����ړ�����������

      if(pWork->curpos == num){
        return FALSE;
      }
      else if(pWork->curpos < num){
        for(i = pWork->curpos ; i < num; i++){
          bChange += _posplus(pWork, length);
        }
      }
      else{
        for(i = pWork->curpos ; i > num; i--){
          bChange += _posminus(pWork, length);
        }
      }
    }
    if(0){
      int length = ITEMMENU_GetItemPocketNumber( pWork);
      int num = (length * (y-_SCROLL_TOP_Y)) / ymax;

      pWork->curpos = 0;
      pWork->oamlistpos = -1;
      for(i = 0 ; i < num ; i++){
        _posplus(pWork, length);
      }
    }
    {
      int newno = ITEMMENU_GetItemIndex(pWork);
      _ItemChange(pWork, nowno, newno);
    }
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e���ړ����[�h���̃L�[�̏���
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemMovePosition(FIELD_ITEMMENU_WORK* pWork)
{
  BOOL bChange=FALSE;
  u32 trg = GFL_UI_KEY_GetTrg();

 	GFL_UI_KEY_SetRepeatSpeed(1, 6);

  if((trg == PAD_BUTTON_A) || (trg == PAD_BUTTON_SELECT)){  //���f
    MYITEM_ITEM_STCopy(pWork->pMyItem, pWork->ScrollItem, pWork->pocketno, FALSE);
  }
  if((trg == PAD_BUTTON_B) || (trg == PAD_BUTTON_A) || (trg == PAD_BUTTON_SELECT) ){  //�߂�
    pWork->moveMode = FALSE;
    _CHANGE_STATE(pWork,_itemKindSelectMenu);
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkCur , 1 );
    return;
  }

  if( _itemScrollCheck(pWork) ){   // �X�N���[���o�[�̑���
    ITEMDISP_scrollCursorMove(pWork);
    bChange = TRUE;
  }
  else if(_itemMovePositionTouchItem(pWork)){
    ITEMDISP_scrollCursorChangePos(pWork, ITEMMENU_GetItemIndex(pWork));
    bChange = TRUE;
  }
  else if(_keyChangeItemCheck(pWork)){   // �L�[�̑���
    ITEMDISP_scrollCursorChangePos(pWork, ITEMMENU_GetItemIndex(pWork));
    bChange = TRUE;
  }
  if(bChange){
    _windowRewrite(pWork);
  }
}
//------------------------------------------------------------------------------
/**
 * @brief   �Z�}�V���m�F�� �͂������܂�
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTecniqueUseYesNo(FIELD_ITEMMENU_WORK* pWork)
{
  u32 ret = 0;//= TOUCH_SW_Main( pWork->pTouchSWSys );

  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);

    if(selectno==0){
      ITEMDISP_ListPlateClear( pWork );
      pWork->ret_code = BAG_NEXTPROC_WAZASET;
      _CHANGE_STATE(pWork,NULL);
    }
    else{
      ITEMDISP_ListPlateClear( pWork );
      GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
      _CHANGE_STATE(pWork,_itemKindSelectMenu);
    }
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
  }

}



//------------------------------------------------------------------------------
/**
 * @brief   �Z�}�V���m�F��
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTecniqueUseWait(FIELD_ITEMMENU_WORK* pWork)
{
  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }

  ITEMDISP_YesNoStart(pWork);

  _CHANGE_STATE(pWork,_itemTecniqueUseYesNo);

}

//------------------------------------------------------------------------------
/**
 * @brief   �Z�}�V���N��
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTecniqueUseInit(FIELD_ITEMMENU_WORK* pWork)
{

  GFL_MSG_GetString( pWork->MsgManager, msg_bag_065, pWork->pStrBuf );
  WORDSET_RegisterWazaName(pWork->WordSet, 0, ITEM_GetWazaNo( pWork->ret_item ));
  WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
  ITEMDISP_ItemInfoWindowDisp( pWork );
  _CHANGE_STATE(pWork,_itemTecniqueUseWait);
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e�����̂Ă�ׂ̑��� �͂��܂�
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTrashEndWait(FIELD_ITEMMENU_WORK* pWork)
{
  if(PAD_BUTTON_DECIDE == GFL_UI_KEY_GetTrg()){
    GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
    _CHANGE_STATE(pWork,_itemKindSelectMenu);
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e�����̂Ă�ׂ̑��� �͂��܂�
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTrashYesWait(FIELD_ITEMMENU_WORK* pWork)
{
  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }
  _windowRewrite(pWork);
  _CHANGE_STATE(pWork,_itemTrashEndWait);



}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e�����̂Ă�ׂ̑��� �͂��������܂�
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTrashYesNoWait(FIELD_ITEMMENU_WORK* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    GFL_BG_ClearScreen(GFL_BG_FRAME3_M);

    if(selectno==0){
      //������
      MYITEM_SubItem(pWork->pMyItem, pWork->ret_item, pWork->trashNum, pWork->heapID );

      GFL_MSG_GetString( pWork->MsgManager, msg_bag_055, pWork->pStrBuf );
      WORDSET_RegisterItemName(pWork->WordSet, 0,  pWork->ret_item );
      WORDSET_RegisterNumber(pWork->WordSet, 1, pWork->trashNum,
                             3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
      WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
      ITEMDISP_ItemInfoWindowDisp( pWork );

      // �ĕ`��---



      _CHANGE_STATE(pWork,_itemTrashYesWait);
    }
    else{
      _CHANGE_STATE(pWork,_itemKindSelectMenu);
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e�����̂Ă�ׂ̑���
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTrashYesNo(FIELD_ITEMMENU_WORK* pWork)
{
  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }
  ITEMDISP_YesNoStart(pWork);
  _CHANGE_STATE(pWork,_itemTrashYesNoWait);
}


//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e�����̂Ă�ׂ̑���
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTrashWait(FIELD_ITEMMENU_WORK* pWork)
{
  ITEM_ST * item = ITEMMENU_GetItem( pWork,ITEMMENU_GetItemIndex(pWork) );
  int backup = pWork->trashNum;

  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }
  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE){
    GFL_BG_ClearScreen(GFL_BG_FRAME3_M);

    GFL_MSG_GetString( pWork->MsgManager, msg_bag_056, pWork->pStrBuf );
    WORDSET_RegisterItemName(pWork->WordSet, 0,  pWork->ret_item );
    WORDSET_RegisterNumber(pWork->WordSet, 1, pWork->trashNum,
                           3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
    ITEMDISP_ItemInfoWindowDisp( pWork );
    _CHANGE_STATE(pWork,_itemTrashYesNo);
  }
  else if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_CANCEL){
    GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
    _CHANGE_STATE(pWork,_itemKindSelectMenu);
  }

  if(GFL_UI_KEY_GetRepeat() == PAD_KEY_UP){
    pWork->trashNum++;
  }
  else if(GFL_UI_KEY_GetRepeat() == PAD_KEY_DOWN){
    pWork->trashNum--;
  }
  else if(GFL_UI_KEY_GetRepeat() == PAD_KEY_RIGHT){
    pWork->trashNum += 10;
  }
  else if(GFL_UI_KEY_GetRepeat() == PAD_KEY_LEFT){
    pWork->trashNum -= 10;
  }


  if(item->no < pWork->trashNum){
    pWork->trashNum = item->no;
  }
  else if(pWork->trashNum < 1){
    pWork->trashNum = 1;
  }
  if(pWork->trashNum != backup){
    ITEMDISP_TrashNumDisp(pWork,pWork->trashNum);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e�����̂Ă�ׂ̕\��
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTrash(FIELD_ITEMMENU_WORK* pWork)
{
  pWork->trashNum = 1;  //������

  ITEMDISP_NumFrameDisp(pWork);

  ITEMDISP_TrashNumDisp(pWork,pWork->trashNum);

  GFL_MSG_GetString( pWork->MsgManager, msg_bag_054, pWork->pStrBuf );
  WORDSET_RegisterItemName(pWork->WordSet, 0,  pWork->ret_item );
  WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
  ITEMDISP_ItemInfoWindowDisp( pWork );

  _CHANGE_STATE(pWork,_itemTrashWait);
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e����I�����Ăǂ�����������s���������Ă���Ƃ���
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemSelectWait(FIELD_ITEMMENU_WORK* pWork)
{
  BOOL bClear=FALSE;

  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    pWork->ret_code2 = pWork->submenuList[selectno];
    OS_Printf("ret_code2 %d %d\n", pWork->ret_code2,selectno);

    if((BAG_MENU_TSUKAU==pWork->ret_code2)&&(pWork->pocketno==BAG_POKE_WAZA)){ //�Z�}�V��
      _CHANGE_STATE(pWork,_itemTecniqueUseInit);
    }
    else if((BAG_MENU_TSUKAU==pWork->ret_code2)&&( 1==ITEM_GetParam( pWork->ret_item, ITEM_PRM_EVOLUTION, pWork->heapID )  )){
      pWork->ret_code = BAG_NEXTPROC_EVOLUTION;  //�i��
      _CHANGE_STATE(pWork,NULL);
    }
    else if((ITEM_ZITENSYA == pWork->ret_item) && (BAG_MENU_TSUKAU==pWork->ret_code2)){
      pWork->ret_code = BAG_NEXTPROC_RIDECYCLE;  //�̂�
      _CHANGE_STATE(pWork,NULL);
    }
    else if((ITEM_ZITENSYA == pWork->ret_item) && (BAG_MENU_ORIRU==pWork->ret_code2)){
      pWork->ret_code = BAG_NEXTPROC_DROPCYCLE;  //�����
      _CHANGE_STATE(pWork,NULL);
    }
    else if(BAG_MENU_SUTERU==pWork->ret_code2){  //���Ă�

      _CHANGE_STATE(pWork,_itemTrash);
    }
    else if(pWork->ret_item == ITEM_TAUNMAPPU){
      pWork->ret_code = BAG_NEXTPROC_TOWNMAP;  //�^�E���}�b�v
      _CHANGE_STATE(pWork,NULL);
    }
    else if(pWork->ret_item == ITEM_TOMODATITETYOU){
      pWork->ret_code = BAG_NEXTPROC_FRIENDNOTE;  //�Ƃ������蒠
      _CHANGE_STATE(pWork,NULL);
    }
    else if(BAG_MENU_YAMERU==pWork->ret_code2){  //��߂�
      _CHANGE_STATE(pWork,_itemKindSelectMenu);
    }
    else if(BAG_MENU_TSUKAU==pWork->ret_code2){
      pWork->ret_code = BAG_NEXTPROC_ITEMUSE;  //����
      _CHANGE_STATE(pWork,NULL);
    }
    else{
      pWork->ret_code = BAG_NEXTPROC_HAVE;
      _CHANGE_STATE(pWork,NULL);
    }
    bClear = TRUE;
  }
  else if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_B){
    bClear = TRUE;
    _CHANGE_STATE(pWork,_itemKindSelectMenu);
  }
  if(bClear){
    ITEMDISP_ListPlateClear( pWork );
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
  }

}



//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e����I�����Ăǂ�����������s���������Ă���Ƃ���
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemSelectState(FIELD_ITEMMENU_WORK* pWork)
{
  {  //�I�񂾃A�C�e���̃Z�b�g
    ITEM_ST * item = ITEMMENU_GetItem( pWork,ITEMMENU_GetItemIndex(pWork) );
    pWork->ret_item = ITEM_DUMMY_DATA;
    if(item){
      pWork->ret_item = item->id;
    }
  }
  if(pWork->mode == BAG_MODE_POKELIST){
    //    pWork->ret_code = BAG_NEXTPROC_ITEMHAVE_RET;
    pWork->ret_code = BAG_NEXTPROC_ITEMEQUIP;
    _CHANGE_STATE(pWork,NULL);
  }
  else{
    _itemUseWindowRewrite(pWork);
    ITEMDISP_ItemInfoMessageMake( pWork,pWork->ret_item );
    ITEMDISP_ItemInfoWindowDisp( pWork );
    _CHANGE_STATE(pWork,_itemSelectWait);
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   �L�[�̏���
 * @retval  none
 */
//------------------------------------------------------------------------------


//�J�[�\���̈ʒu
//OAMLIST �� �擪�ʒu -1�͕\�����Ȃ�



static void _itemKindSelectMenu(FIELD_ITEMMENU_WORK* pWork)
{
  u32	ret=0;
  BOOL bChange=FALSE;

 	GFL_UI_KEY_SetRepeatSpeed(1, 6);

  GFL_BMN_Main( pWork->pButton );
  if(pWork->state == NULL){
    return;
  }

  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE){
    _CHANGE_STATE(pWork,_itemSelectState);
    return;
  }
  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_B){
    pWork->ret_code = BAG_NEXTPROC_RETURN;
    _CHANGE_STATE(pWork,NULL);
    return;
  }
  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_SELECT){
    GFL_STD_MemClear(pWork->ScrollItem, sizeof(pWork->ScrollItem));
    MYITEM_ITEM_STCopy(pWork->pMyItem, pWork->ScrollItem, pWork->pocketno, TRUE);  //�擾
    pWork->moveMode = TRUE;
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkCur , 2 );

    _CHANGE_STATE(pWork,_itemMovePosition);
    return;
  }

  if( _itemScrollCheck(pWork) ){
    ITEMDISP_scrollCursorMove(pWork);
    bChange = TRUE;
  }
  else if(_keyMoveCheck(pWork)){
    ITEMDISP_scrollCursorChangePos(pWork, ITEMMENU_GetItemIndex(pWork));
    bChange = TRUE;
  }
  {
    int oldpocket = pWork->pocketno;
    if(GFL_UI_KEY_GetTrg() == PAD_KEY_RIGHT){
      pWork->pocketno++;
    }
    if(GFL_UI_KEY_GetTrg() == PAD_KEY_LEFT){
      pWork->pocketno--;
    }
    if(pWork->pocketno >= BAG_POKE_MAX){
      pWork->pocketno = 0;
    }
    if(pWork->pocketno < 0){
      pWork->pocketno = BAG_POKE_MAX-1;
    }
    if(oldpocket != pWork->pocketno){
      _pocketCursorChange(pWork, oldpocket, pWork->pocketno);
      bChange = TRUE;
    }
  }

  if(bChange){
    _windowRewrite(pWork);
  }

}

#if 0
//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�G���[���b�Z�[�W�Z�b�g
 *
 * @param	myst	�v���[���[�f�[�^
 * @param	buf		���b�Z�[�W�W�J�ꏊ
 * @param	item	�A�C�e���ԍ�
 * @param	err		�G���[ID
 * @param	heap	�q�[�vID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BAG_ItemUseErrorMsgSet( MYSTATUS * myst, STRBUF * buf, u16 item, u32 err, FIELD_ITEMMENU_WORK * pWork )
{
  WORDSET * wset;
  STRBUF * str;

  switch( err ){
  case ITEMCHECK_ERR_CYCLE_OFF:		// ���]�Ԃ��~���Ȃ�
    GFL_MSG_GetString( pWork->MsgManager, msg_bag_058, buf );
    break;
  case ITEMCHECK_ERR_COMPANION:		// �g�p�s�E�A�����
    GFL_MSG_GetString( pWork->MsgManager, msg_bag_err_pair, buf );
    break;
  case ITEMCHECK_ERR_DISGUISE:		// �g�p�s�E���P�b�g�c�ϑ���
    GFL_MSG_GetString( pWork->MsgManager, msg_bag_err_disguise, buf );
    break;
  default:							// �g�p�s�E���m�̌��t
    wset = WORDSET_Create( pWork->heapID );
    str  = GFL_MSG_CreateString( pWork->MsgManager, msg_bag_059 );
    WORDSET_RegisterPlayerName( wset, 0, myst );
    WORDSET_ExpandStr( wset, buf, str );
    GFL_STR_DeleteBuffer( str );
    WORDSET_Delete( wset );
  }
}


//--------------------------------------------------------------------------------------------
/**
 * ��b�i���j���b�Z�[�W�\��
 *
 * @param	pWork		�o�b�O��ʃ��[�N
 * @param	type	BAG_TALK_FULL=��ʈ�t, BAG_TALK_MINI=���2/3
 *
 * @return	���b�Z�[�WID
 */
//--------------------------------------------------------------------------------------------
u8 Bag_TalkMsgPrint( BAG_WORK * pWork, int type )
{
  u8	idx;
  GF_BGL_BMPWIN *win;
  if(type==BAG_TALK_FULL){
    win = &pWork->win[WIN_TALK];
  }else{
    // �Ăяo���Ȃ���������
    GF_ASSERT( pWork->sub_win[ADD_WIN_MINITALK].ini!=NULL );
    win = &pWork->sub_win[ADD_WIN_MINITALK];
  }

  GF_BGL_BmpWinDataFill( win, 15 );
  BmpTalkWinWrite( win, WINDOW_TRANS_OFF, TALK_SUB_WIN_CGX_NUM, TALKWIN_PAL );
  GF_BGL_BmpWinOnVReq( win );

  MsgPrintSkipFlagSet( MSG_SKIP_ON );
  MsgPrintAutoFlagSet( MSG_AUTO_OFF );

  idx = GF_STR_PrintSimple(	win, FONT_TALK,pWork->expb, 0, 0,
                            CONFIG_GetMsgPrintSpeed( pWork->cfg ), BAG_TalkMsgCallBack );

  return idx;
}

//--------------------------------------------------------------------------------------------
/**
 * ���j���[�F����
 *
 * @param	pWork		�o�b�O��ʂ̃��[�N
 *
 * @return	�ڍs���郁�C���V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int Bag_MenuUse( FIELD_ITEMMENU_WORK * pWork )
{
  ITEMCHECK_FUNC	check;
  s32	id;

  // �ǂ����g�p�����擾
  id    = ITEM_GetParam( pWork->ret_item, ITEM_PRM_FIELD, pWork->heapID );
  // �g���`�F�b�N������֐�
  check = (ITEMCHECK_FUNC)ITEMUSE_GetFunc( ITEMUSE_PRM_CHECKFUNC, id );

  // �ǂ����g�p�`�F�b�N
  if( check != NULL ){
    u32	ret = check( &pWork->icpWork );
    if( ret != ITEMCHECK_TRUE ){
      BAG_ItemUseErrorMsgSet( pWork->mystatus, pWork->expb, pWork->ret_item, ret, pWork );
      pWork->midx = Bag_TalkMsgPrint( pWork, BAG_TALK_FULL );
      return SEQ_ITEM_ERR_WAIT;
    }
  }

  return BAG_ItemUse( pWork );
}
#endif



//--------------------------------------------------------------------------------------------
/**
 * Use���j���[�쐬
 * @param	pWork		�o�b�O��ʂ̃��[�N
 * @return	none
 */
//--------------------------------------------------------------------------------------------
#if 1

int (*MenuFuncTbl[])( FIELD_ITEMMENU_WORK *pWork)={
  //Bag_MenuUse,		// ����
  //Bag_MenuUse,		// �����
  //Bag_MenuUse,		// �݂�
  //Bag_MenuUse,		// ���߂�
  //Bag_MenuUse,		// �Ђ炭
  //Bag_MenuSub,		// ���Ă�
  //Bag_MenuCnvSet,		// �Ƃ��낭
  //Bag_MenuCnvDel,		// ��������
  //Bag_MenuSet,		// ��������
  //Bag_MenuTag,		// �^�O���݂�
  //Bag_MenuKettei,		// �����Ă�
  NULL,				// ��߂�
  //Bag_MenuItemMove,	// ���ǂ�
  NULL,				// ����
  //Bag_MenuPlanterUse,	// �����i�؂̎��v�����^�[�p�j
  //Bag_MenuUse,		// �Ƃ߂�
};

static BOOL BAGMAIN_NPlanterUseCheck( u16 pocket, u16 item )
{
  if( pocket == BAG_POKE_NUTS ||
      ( item >= COMPOST_START_ITEMNUM && item < (COMPOST_START_ITEMNUM+ITEM_COMPOST_MAX) ) ){
    return TRUE;
  }
  return FALSE;
}

static void ItemMenuMake( FIELD_ITEMMENU_WORK * pWork, u8* tbl )
{
  int	 i;
  void * itemdata;
  u8	pocket;
  ITEM_ST * item = ITEMMENU_GetItem( pWork,ITEMMENU_GetItemIndex(pWork) );
  if((item==NULL) || (item->id==ITEM_DUMMY_DATA)){
    return;
  }

  itemdata = ITEM_GetItemArcData( item->id, ITEM_GET_DATA, pWork->heapID );
  pocket   = pWork->pocketno;

  for(i=0;i<BAG_MENUTBL_MAX;i++){
    pWork->menu_func[i] = NULL;
  }

  // �t�B�[���h
  if( pWork->mode == BAG_MODE_FIELD ){
    // �R���V�A���E���j�I�����[���ł́u�݂�v�̂�
    if( pWork->mode == BAG_MODE_COLOSSEUM || pWork->mode == BAG_MODE_UNION )
    {
      tbl[BAG_MENU_USE] = BAG_MENU_MIRU;
    }
    else
    {
      // ����
      // �����
      // �݂�
      // �Ђ炭
      // ���߂�
      // �Ƃ߂�
      if( ITEM_GetBufParam( itemdata,  ITEM_PRM_FIELD ) != 0 )  //@@OO �A�C�e���R���o�[�^�����������̂����ʂ��Ⴄ
      {
        if( item->id == ITEM_ZITENSYA && pWork->cycle_flg == 1 )
        {
          tbl[BAG_MENU_USE] = BAG_MENU_ORIRU;
        }
        else if( item->id == ITEM_POFINKEESU )
        {
          tbl[BAG_MENU_USE] = BAG_MENU_HIRAKU;
        }
        //			else if( pWork->ret_item == ITEM_gbPUREIYAA && Snd_GameBoyFlagCheck() == 1 )
        //		{
        //		tbl[BAG_MENU_USE] = BAG_MENU_TOMERU;
        //}
        else
        {
          tbl[BAG_MENU_USE] = BAG_MENU_TSUKAU;
        }
      }
    }
    // ��������
    // ���Ă�
    if( ITEM_GetBufParam( itemdata, ITEM_PRM_EVENT ) == 0 ){
      if( ITEM_CheckPokeAdd( item->id ) == TRUE ){
        tbl[BAG_MENU_GIVE] = BAG_MENU_MOTASERU;
      }
      if( pocket != BAG_POKE_WAZA ){
        // �Z�}�V������Ȃ��ꍇ�͎̂Ă��o�^
        tbl[BAG_MENU_SUB] = BAG_MENU_SUTERU;
      }
    }
    // �Ƃ��낭
    // ��������
    if( ITEM_GetBufParam( itemdata, ITEM_PRM_CNV ) != 0 ){
      if( ITEMMENU_GetPosCnvButtonItem( pWork, item->id  ) != -1 ){
        tbl[BAG_MENU_SUB] = BAG_MENU_KAIZYO;
      }else{
        tbl[BAG_MENU_SUB] = BAG_MENU_TOUROKU;
      }
    }
  }
  // �؂̎��v�����^�[
  else if( pWork->mode == BAG_MODE_N_PLANTER ){
    if( BAGMAIN_NPlanterUseCheck( pocket, item->id ) == TRUE ){
      tbl[BAG_MENU_USE] = BAG_MENU_TSUKAU_NP;
    }
  }

  // ��߂�
  tbl[BAG_MENU_CANCEL] = BAG_MENU_YAMERU;

  OS_Printf("pocket=%d\n",   pocket);
  OS_Printf("tbl[BAG_MENU_USE]=%d\n",   tbl[BAG_MENU_USE]);
  OS_Printf("tbl[BAG_MENU_GIVE]=%d\n",  tbl[BAG_MENU_GIVE]);
  OS_Printf("tbl[BAG_MENU_SUB]=%d\n",   tbl[BAG_MENU_SUB]);
  OS_Printf("tbl[BAG_MENU_MOVE]=%d\n",   tbl[BAG_MENU_ITEMMOVE]);

  for(i=0;i<BAG_MENUTBL_MAX;i++){
    if(tbl[i]!=255){
      pWork->menu_func[i] = MenuFuncTbl[tbl[i]];
    }
    OS_Printf("menu_func[%d]=%08x\n", i,pWork->menu_func[i]);
  }


  GFL_HEAP_FreeMemory( itemdata );
}

#endif


static void _itemUseWindowRewrite(FIELD_ITEMMENU_WORK* pWork)
{
  BMPMENULIST_HEADER list_h = _itemMenuListHeader;
  int length,i,j;


  length = BAG_MENUTBL_MAX;


  {
    u8	tbl[BAG_MENUTBL_MAX]={255, 255, 255, 255, 255};
    int strtbl[]={msg_bag_001,msg_bag_007,msg_bag_017,mes_bag_104,mes_bag_105,
      msg_bag_002,msg_bag_003,msg_bag_019,msg_bag_004,msg_bag_005,msg_bag_006,
      msg_bag_009, msg_bag_094, mes_shop_103,msg_bag_001,msg_bag_menu_ex_01};
    int stringbuff[BAG_MENUTBL_MAX];

    ItemMenuMake(pWork, tbl);

    for(i=0,j=0; i< BAG_MENUTBL_MAX ; i++){
      if(tbl[i]==255){
        continue;
      }
      pWork->submenuList[j]=tbl[i];
      stringbuff[j] = strtbl[tbl[i]];
      j++;
    }
    ITEMDISP_MenuWinDisp(pWork,stringbuff,j);


  }
}


//----------------------------------------------------------------------------
/**
 *	@brief	�֗��o�^����Ă����珇�Ԃ�Ԃ�
 */
//-----------------------------------------------------------------------------

int ITEMMENU_GetPosCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no)
{
  int i;
  for(i = 0 ; i < DUMMY_SHORTCUT_MAX ; i++){
    if(no == MYITEM_CnvButtonItemGet( pWork->pMyItem, i)){
      return i;
    }
  }
  return -1;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`�F�b�N��ǉ�
 */
//-----------------------------------------------------------------------------

BOOL ITEMMENU_AddCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no)
{
  int i;
  for(i = 0 ; i < DUMMY_SHORTCUT_MAX ; i++){
    if(0 == MYITEM_CnvButtonItemGet( pWork->pMyItem, i)){
      MYITEM_CnvButtonItemSet( pWork->pMyItem, i, no);
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`�F�b�N������
 */
//-----------------------------------------------------------------------------

void ITEMMENU_RemoveCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no)
{
  int i=ITEMMENU_GetPosCnvButtonItem(pWork,no);

  if(i!=-1){
    MYITEM_CnvButtonItemSet( pWork->pMyItem, i, 0);
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���C�x���g�R�[���o�b�N
 *
 *	@param	bttnid		�{�^��ID
 *	@param	event		�C�x���g���
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------
static void _BttnCallBack( u32 bttnid, u32 event, void* p_work )
{
  FIELD_ITEMMENU_WORK* pWork = p_work;
  int pocketno = -1;
  int length = ITEMMENU_GetItemPocketNumber(pWork);

  if(event!=GFL_BMN_EVENT_TOUCH){
    return;
  }
  if(BAG_POKE_MAX > bttnid){
    int id2no[]={1,2,3,4,0};
    pocketno = id2no[bttnid];  //�ǂ����̂����肪�L���ׁA���Ԃ��Ō�ɂ��Ă���
  }
  else if(BUTTONID_LEFT == bttnid){
    pocketno = pWork->pocketno;
    pocketno--;
    if(pocketno < 0){
      pocketno = BAG_POKE_MAX-1;
    }
  }
  else if(BUTTONID_RIGHT == bttnid){
    pocketno = pWork->pocketno;
    pocketno++;
    if(pocketno >= BAG_POKE_MAX){
      pocketno = 0;
    }
  }
  else if(BUTTONID_EXIT == bttnid){
    pWork->ret_code = BAG_NEXTPROC_EXIT;
    _CHANGE_STATE(pWork, NULL);
  }
  else if(BUTTONID_RETURN == bttnid){
    pWork->ret_code = BAG_NEXTPROC_RETURN;
    _CHANGE_STATE(pWork, NULL);
  }
  else if((bttnid >= BUTTONID_ITEM_AREA) && (bttnid < BUTTONID_CHECK_AREA)){
    if(_itemKindSelectMenu == pWork->state){
      int backup = pWork->curpos;
      int nowno = ITEMMENU_GetItemIndex(pWork);

      if(pWork->curpos != (bttnid-BUTTONID_ITEM_AREA)){
        pWork->curpos = bttnid - BUTTONID_ITEM_AREA;
        if(length <= ITEMMENU_GetItemIndex(pWork)){
          pWork->curpos = backup;
          return;
        }
      }
      _ItemChange(pWork, nowno, ITEMMENU_GetItemIndex(pWork));
      //      ITEMDISP_scrollCursorChangePos(pWork, ITEMMENU_GetItemIndex(pWork));
      _windowRewrite(pWork);
      _CHANGE_STATE(pWork,_itemSelectState);
    }
    return;
  }
  else if(bttnid >= BUTTONID_CHECK_AREA){
    if(pWork->pocketno == BAG_POKE_EVENT){
      int no = bttnid - BUTTONID_CHECK_AREA + pWork->oamlistpos + 1;
      ITEM_ST * item = ITEMMENU_GetItem( pWork, no );
      if(length <= no){
        return;
      }
      {
        void * itemdata;
        int ret;
        itemdata = ITEM_GetItemArcData( item->id, ITEM_GET_DATA, pWork->heapID );
        ret = ITEM_GetBufParam( itemdata, ITEM_PRM_CNV );
        GFL_HEAP_FreeMemory( itemdata );
        if( ret == 0 ){
          return;
        }
      }



      if(ITEMMENU_GetPosCnvButtonItem(pWork,item->id)!=-1){//�`�F�b�N��
        ITEMMENU_RemoveCnvButtonItem(pWork,item->id);
      }
      else{
        ITEMMENU_AddCnvButtonItem(pWork,item->id);
      }
      _windowRewrite(pWork);
    }
  }

  if(pocketno != -1){
    _pocketCursorChange(pWork, pWork->pocketno, pocketno);
    pWork->pocketno = pocketno;
    _windowRewrite(pWork);
  }


}

//--------------------------------------------------------------
/**
 * G3D VBlank����
 * @param TCB GFL_TCB
 * @param work tcb work
 * @retval nothing
 */
//--------------------------------------------------------------
static void	_VBlank( GFL_TCB *tcb, void *work )
{
  FIELD_ITEMMENU_WORK* pWork = work;

  GFL_CLACT_SYS_VBlankFunc();	//�Z���A�N�^�[VBlank
  if(pWork->bChange){
    ITEMDISP_CellVramTrans(pWork);
    pWork->bChange = FALSE;
  }
}


static void _startState(FIELD_ITEMMENU_WORK* pWork)
{
  ITEMDISP_SetVisible();
  if (GFL_FADE_CheckFade() == FALSE) {
    _CHANGE_STATE(pWork, _itemKindSelectMenu);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �v���Z�X������
 * @retval  none
 */
//------------------------------------------------------------------------------

static GFL_PROC_RESULT FieldItemMenuProc_Init( GFL_PROC * proc, int * seq, void * ppWork, void * mypWork )
{
  FIELD_ITEMMENU_WORK* pWork = ppWork;

  GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0);

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_ITEMMENU, 0x28000 );
  pWork->heapID = HEAPID_ITEMMENU;
  pWork->pBagCursor = GAMEDATA_GetBagCursor(GAMESYSTEM_GetGameData(pWork->gsys));

  pWork->pocketno = MYITEM_FieldBagPocketGet(pWork->pBagCursor);
  {
    s16 cur,scr;
    MYITEM_FieldBagCursorGet(pWork->pBagCursor, pWork->pocketno, &cur, &scr );
    pWork->curpos = cur;
    pWork->oamlistpos = scr - 1;
  }
  ITEMDISP_graphicInit(pWork);

  pWork->pMyItem = GAMEDATA_GetMyItem(GAMESYSTEM_GetGameData(pWork->gsys));
  pWork->MsgManager = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
                                      NARC_message_bag_dat, pWork->heapID );

  pWork->MsgManagerItemInfo = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
                                              NARC_message_iteminfo_dat, pWork->heapID );

  pWork->MsgManagerPocket = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
                                            NARC_message_itempocket_dat, pWork->heapID );

  pWork->pStrBuf = GFL_STR_CreateBuffer(200,pWork->heapID);
  pWork->pExpStrBuf = GFL_STR_CreateBuffer(200,pWork->heapID);
  pWork->WordSet    = WORDSET_Create( pWork->heapID );
  pWork->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_nftr ,
                                       GFL_FONT_LOADTYPE_FILE , FALSE , pWork->heapID );

  ITEMDISP_upMessageCreate(pWork);
  ITEMDISP_CellResourceCreate(pWork);
  ITEMDISP_InitPocketCell( pWork );

  _windowCreate(pWork);
  ITEMDISP_CellCreate(pWork);

  pWork->pButton = GFL_BMN_Create( bttndata, _BttnCallBack, pWork,  pWork->heapID );

  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN ,
                  WIPE_FADE_BLACK , FLD_SUBSCR_FADE_DIV , FLD_SUBSCR_FADE_SYNC , pWork->heapID );

  pWork->g3dVintr = GFUser_VIntr_CreateTCB( _VBlank, (void*)pWork, 0 );

  pWork->pMsgTcblSys = GFL_TCBL_Init( pWork->heapID , pWork->heapID , 1 , 0 );

  //  pWork->pTouchSWSys = TOUCH_SW_AllocWork(pWork->heapID);

  pWork->pocketNameWin = GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M,
    _POCKETNAME_DISP_INITX, _POCKETNAME_DISP_INITY,
    _POCKETNAME_DISP_SIZEX, _POCKETNAME_DISP_SIZEY,
    _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_B );
  GFL_BMPWIN_MakeScreen( pWork->pocketNameWin );
  _pocketMessageDisp(pWork, pWork->pocketno);
  ITEMDISP_ChangePocketCell( pWork, pWork->pocketno );

  pWork->count=2;
  _CHANGE_STATE(pWork, _startState);
  return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   �v���Z�XMain
 * @retval  none
 */
//------------------------------------------------------------------------------

static GFL_PROC_RESULT FieldItemMenuProc_Main( GFL_PROC * proc, int * seq, void * ppWork, void * mypWork )
{
  FIELD_ITEMMENU_WORK* pWork = ppWork;
  StateFunc* state = pWork->state;

  if(state == NULL){
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0);
    return GFL_PROC_RES_FINISH;
  }
  if( WIPE_SYS_EndCheck() != TRUE ){
    return GFL_PROC_RES_CONTINUE;
  }
  state(pWork);
  _dispMain(pWork);

  PRINTSYS_QUE_Main(pWork->SysMsgQue);


  GFL_TCBL_Main( pWork->pMsgTcblSys );

  return GFL_PROC_RES_CONTINUE;

}

//------------------------------------------------------------------------------
/**
 * @brief   �v���Z�XEnd
 * @retval  none
 */
//------------------------------------------------------------------------------

static GFL_PROC_RESULT FieldItemMenuProc_End( GFL_PROC * proc, int * seq, void * ppWork, void * mypWork )
{
  FIELD_ITEMMENU_WORK* pWork = ppWork;

  if (GFL_FADE_CheckFade() == TRUE) {
    return GFL_PROC_RES_CONTINUE;
  }

  // GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0);
  GFL_TCB_DeleteTask( pWork->g3dVintr );

  MYITEM_FieldBagPocketSet(pWork->pBagCursor, pWork->pocketno);
  MYITEM_FieldBagCursorSet(pWork->pBagCursor, pWork->pocketno, pWork->curpos, pWork->oamlistpos+1 );

  ITEMDISP_upMessageDelete(pWork);
  ITEMDISP_graphicDelete(pWork);

  //  TOUCH_SW_FreeWork(pWork->pTouchSWSys);

  GFL_TCBL_Exit(pWork->pMsgTcblSys);// = GFL_TCBL_Init( work->heapId , work->heapId , 1 , 0 );


  GFL_MSG_Delete(pWork->MsgManager);
  GFL_MSG_Delete(pWork->MsgManagerItemInfo);
  GFL_MSG_Delete(pWork->MsgManagerPocket);

  GFL_STR_DeleteBuffer(pWork->pStrBuf);
  GFL_STR_DeleteBuffer(pWork->pExpStrBuf);
  WORDSET_Delete(pWork->WordSet);


  GFL_BMPWIN_Delete(pWork->pocketNameWin);


  if(pWork->itemInfoDispWin){

    GFL_BG_FreeCharacterArea(GFL_BG_FRAME3_M,
                             GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar),
                             GFL_ARCUTIL_TRANSINFO_GetSize(pWork->bgchar));

    GFL_BMPWIN_Delete(pWork->itemInfoDispWin);
  }

  GFL_BG_FillCharacterRelease(GFL_BG_FRAME3_M,1,0);

  GFL_FONT_Delete(pWork->fontHandle);
  PRINTSYS_QUE_Clear(pWork->SysMsgQue);
  PRINTSYS_QUE_Delete(pWork->SysMsgQue);
  GFL_BMN_Delete(pWork->pButton);
  //  ITEMDISP_ListPlateDelete(pWork);

  GFL_BMPWIN_Exit();
  GFL_BG_Exit();


  GFL_HEAP_DeleteHeap(  HEAPID_ITEMMENU );

  return GFL_PROC_RES_FINISH;

}



// �v���Z�X��`�f�[�^
const GFL_PROC_DATA ItemMenuProcData = {
  FieldItemMenuProc_Init,
  FieldItemMenuProc_Main,
  FieldItemMenuProc_End,
};
