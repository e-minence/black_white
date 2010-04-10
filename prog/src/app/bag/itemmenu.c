//============================================================================================
/**
 * @file    itemmenu.c
 * @brief   �A�C�e�����j���[
 * @author  k.ohno
 * @author  genya hosaka (���p)
 * @date    2009.06.30
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
#include "gamesystem/game_beacon.h"
#include "system/bmp_winframe.h"

#include "message.naix"
#include "bag.naix"
#include "msg/msg_d_field.h"
#include "msg/msg_bag.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "font/font.naix" //NARC_font_large_gftr
#include "sound/pm_sndsys.h"
#include "system/wipe.h"
#include "ui/touchbar.h"

#include "itemmenu.h"
#include "itemmenu_local.h"

#include "app/itemuse.h"
#include "savedata/mystatus.h"
#include "savedata/encount_sv.h"

#include "system/main.h"      //GFL_HEAPID_APP�Q��
#include "system/scroll_bar.h"

#include "../../field/eventwork.h"  // GAMEDATA_GetEventWork
#include "../../../resource/fldmapdata/flagwork/work_define.h"  // WK_SYS_N01R0502_ITEMUSE

//============================================================================================
//============================================================================================

#define _1CHAR (8)
#define BAG_HEAPSIZE ( 0x34000 ) // 0x28000

//=============================================================================
// �ڎ�
//=============================================================================
//  __TECNIQUE  �Z�}�V�� �V�[�P���X
//  __TRASH     �̂Ă� �V�[�P���X
//  __SELL      ���� �V�[�P���X


//=============================================================================
// �f�[�^�e�[�u��
//=============================================================================
//�Ȃ�т� savedata/myitem_savedata.h�ɏ���
// �R�[���o�b�N��_BttnCallBack
static const GFL_UI_TP_HITTBL bttndata[] = {  //�㉺���E

#include "bag_btnpos.h"  //�o�b�O�̊G�̃^�b�`�G���A

  { 21*_1CHAR,  24*_1CHAR-1,   0*_1CHAR,  3*_1CHAR-1 },  //��
  { 21*_1CHAR,  24*_1CHAR-1,  15*_1CHAR, 18*_1CHAR-1 },  //�E

  { 21*_1CHAR,  24*_1CHAR-1,  19*_1CHAR, 22*_1CHAR-1 },  //�\�[�g
  { 21*_1CHAR,  24*_1CHAR-1,  22*_1CHAR, 24*_1CHAR-1 },  //�`�F�b�N�{�^��

  { TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, _BAR_CELL_CURSOR_EXIT, _BAR_CELL_CURSOR_EXIT+TOUCHBAR_ICON_WIDTH-1 },  //x
  { TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, _BAR_CELL_CURSOR_RETURN, _BAR_CELL_CURSOR_RETURN+TOUCHBAR_ICON_WIDTH-1 },  //���^�[��

  { 1*_1CHAR+4,   4*_1CHAR+3,  18*_1CHAR, 30*_1CHAR-1 },  //�A�C�e���ꗗ�G���A1
  { 4*_1CHAR+4,   7*_1CHAR+3,  18*_1CHAR, 30*_1CHAR-1 },  //�A�C�e���ꗗ�G���A2
  { 7*_1CHAR+4,  11*_1CHAR+3,  18*_1CHAR, 30*_1CHAR-1 },  //�A�C�e���ꗗ�G���A3
  {11*_1CHAR+4,  14*_1CHAR+3,  18*_1CHAR, 30*_1CHAR-1 },  //�A�C�e���ꗗ�G���A4
  {14*_1CHAR+4,  17*_1CHAR+3,  18*_1CHAR, 30*_1CHAR-1 },  //�A�C�e���ꗗ�G���A5
  {17*_1CHAR+4,  20*_1CHAR+3,  18*_1CHAR, 30*_1CHAR-1 },  //�A�C�e���ꗗ�G���A6

  { 1*_1CHAR+4,   4*_1CHAR+3,  15*_1CHAR, 18*_1CHAR-1 },  //�������Ȃ��̃`�F�b�N�G���A1
  { 4*_1CHAR+4,   7*_1CHAR+3,  15*_1CHAR, 18*_1CHAR-1 },  //�������Ȃ��̃`�F�b�N�G���A2
  { 7*_1CHAR+4,  11*_1CHAR+3,  15*_1CHAR, 18*_1CHAR-1 },  //�������Ȃ��̃`�F�b�N�G���A3
  {11*_1CHAR+4,  14*_1CHAR+3,  15*_1CHAR, 18*_1CHAR-1 },  //�������Ȃ��̃`�F�b�N�G���A4
  {14*_1CHAR+4,  17*_1CHAR+3,  15*_1CHAR, 18*_1CHAR-1 },  //�������Ȃ��̃`�F�b�N�G���A5
  {17*_1CHAR+4,  20*_1CHAR+3,  15*_1CHAR, 18*_1CHAR-1 },  //�������Ȃ��̃`�F�b�N�G���A6

  {GFL_UI_TP_HIT_END,0,0,0},     //�I���f�[�^
};



//-----------------------------------------------
//static ��`
//-----------------------------------------------
static void _changeState(FIELD_ITEMMENU_WORK* pWork,StateFunc state);
static void _changeStateDebug(FIELD_ITEMMENU_WORK* pWork,StateFunc state, int line);
static void _windowCreate(FIELD_ITEMMENU_WORK* pWork);
static void _windowRewrite(FIELD_ITEMMENU_WORK* pWork);
ITEM_ST* ITEMMENU_GetItem(FIELD_ITEMMENU_WORK* pWork, int no);
static void _ItemChangeSingle(FIELD_ITEMMENU_WORK* pWork, int no1, int no2 );
static void _ItemChange(FIELD_ITEMMENU_WORK* pWork, int no1, int no2 );
static void _pocketCursorChange(FIELD_ITEMMENU_WORK* pWork,int oldpocket, int newpocket);
int ITEMMENU_GetItemIndex(FIELD_ITEMMENU_WORK* pWork);
static BOOL _posplus( FIELD_ITEMMENU_WORK * pWork, int length, BOOL loop );
static BOOL _posminus( FIELD_ITEMMENU_WORK * pWork, int length, BOOL loop );
static BOOL _itemScrollCheck(FIELD_ITEMMENU_WORK* pWork);
static BOOL _keyChangeItemCheck(FIELD_ITEMMENU_WORK* pWork);
static BOOL _keyMoveCheck(FIELD_ITEMMENU_WORK* pWork);
static BOOL _itemMovePositionTouchItem(FIELD_ITEMMENU_WORK* pWork);
static void _itemMovePosition(FIELD_ITEMMENU_WORK* pWork);
static void _itemInnerUse( FIELD_ITEMMENU_WORK* pWork );
static void _itemInnerUseWait( FIELD_ITEMMENU_WORK* pWork );
static void _itemInnerUseError( FIELD_ITEMMENU_WORK* pWork );
static void _itemInnerUseRightStone( FIELD_ITEMMENU_WORK* pWork );
static void _itemInnerUseRightStoneWait( FIELD_ITEMMENU_WORK* pWork );
static void _itemSelectWait(FIELD_ITEMMENU_WORK* pWork);
static void _itemSelectState(FIELD_ITEMMENU_WORK* pWork);
static void _itemKindSelectMenu(FIELD_ITEMMENU_WORK* pWork);
static void _itemTecniqueUseYesNo(FIELD_ITEMMENU_WORK* pWork);
static void _itemTecniqueUseWait(FIELD_ITEMMENU_WORK* pWork);
static void _itemTecniqueUseInit(FIELD_ITEMMENU_WORK* pWork);
static void _itemTrashEndWait(FIELD_ITEMMENU_WORK* pWork);
static void _itemTrashYesWait(FIELD_ITEMMENU_WORK* pWork);
static void _itemTrashYesNoWait(FIELD_ITEMMENU_WORK* pWork);
static void _itemTrashYesNo(FIELD_ITEMMENU_WORK* pWork);
static void _itemTrashWait(FIELD_ITEMMENU_WORK* pWork);
static void _itemTrash(FIELD_ITEMMENU_WORK* pWork);
static void _itemSellInit( FIELD_ITEMMENU_WORK* pWork );
static void _itemSellInputWait( FIELD_ITEMMENU_WORK* pWork );
static void _itemSellYesnoInit( FIELD_ITEMMENU_WORK* pWork );
static void _itemSellYesnoInput( FIELD_ITEMMENU_WORK* pWork );
static void _itemSellEndMsgWait( FIELD_ITEMMENU_WORK* pWork );
static void _itemSellExit( FIELD_ITEMMENU_WORK* pWork );
static void ITEM_Sub( FIELD_ITEMMENU_WORK* pWork, int sub_num );
static void InputNum_Start( FIELD_ITEMMENU_WORK* pWork, BAG_INPUT_MODE mode );
static void InputNum_Exit( FIELD_ITEMMENU_WORK* pWork );
static BOOL InputNum_Main( FIELD_ITEMMENU_WORK* pWork );
//static void InputNum_ButtonState( FIELD_ITEMMENU_WORK* pWork, BOOL on_off );
static void SORT_Type( FIELD_ITEMMENU_WORK* pWork );
//static void SORT_ABC( FIELD_ITEMMENU_WORK* pWork );
static u16 SORT_GetABCPrio( u16 item_no );
static void SORT_Button( FIELD_ITEMMENU_WORK* pWork );
static void SORT_ModeReset( FIELD_ITEMMENU_WORK* pWork );
//static void SORT_Draw( FIELD_ITEMMENU_WORK* pWork );
static void KTST_SetDraw( FIELD_ITEMMENU_WORK* pWork, BOOL on_off );
static void SHORTCUT_SetEventItem( FIELD_ITEMMENU_WORK* pWork, int pos );
static void SHORTCUT_SetPocket( FIELD_ITEMMENU_WORK* pWork );
static void BTN_DrawCheckBox( FIELD_ITEMMENU_WORK* pWork );
//static void BAG_ItemUseErrorMsgSet( MYSTATUS * myst, STRBUF * buf, u16 item, u32 err, FIELD_ITEMMENU_WORK * pWork );
//u8 Bag_TalkMsgPrint( BAG_WORK * pWork, int type );
//static int Bag_MenuUse( FIELD_ITEMMENU_WORK * pWork );
static BOOL BAGMAIN_NPlanterUseCheck( u16 pocket, u16 item );
static void ItemMenuMake( FIELD_ITEMMENU_WORK * pWork, u8* tbl );
static void _itemUseWindowRewrite(FIELD_ITEMMENU_WORK* pWork);
int ITEMMENU_GetPosCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no);
BOOL ITEMMENU_AddCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no);
void ITEMMENU_RemoveCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no);
static void _BttnCallBack( u32 bttnid, u32 event, void* p_work );
static void _VBlank( GFL_TCB *tcb, void *work );
static void _startState(FIELD_ITEMMENU_WORK* pWork);

#ifdef PM_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeStateDebug(pWork ,state, __LINE__)
#else  //_NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeState(pWork ,state)
#endif //_NET_DEBUG

static void ChangeStateItemKindSelectItemMenu( FIELD_ITEMMENU_WORK * wk );
static void ChangeStateItemMovePosition( FIELD_ITEMMENU_WORK * wk );
static void InitBlinkPalAnm( FIELD_ITEMMENU_WORK * wk );
static void ExitBlinkPalAnm( FIELD_ITEMMENU_WORK * wk );
static void _listSelectAnime( FIELD_ITEMMENU_WORK * wk );
static void SetEndButtonAnime( FIELD_ITEMMENU_WORK * wk, u8 type, StateFunc * next );
static void _endButtonAnime( FIELD_ITEMMENU_WORK * wk );
static int CheckNumSelTouch(void);
static void _itemTrashCancel( FIELD_ITEMMENU_WORK * wk );
static void _itemSellInputCancel( FIELD_ITEMMENU_WORK * wk );
static void InitPaletteAnime( FIELD_ITEMMENU_WORK * wk );
static void ExitPaletteAnime( FIELD_ITEMMENU_WORK * wk );
static void _sortMessageSet( FIELD_ITEMMENU_WORK * wk );
static void _sortMessageWait( FIELD_ITEMMENU_WORK * wk );
static BOOL CheckEventItemUse( FIELD_ITEMMENU_WORK * wk, u8 pocket );


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
//  GFL_UI_KEY_SetRepeatSpeed(8, 15);  //���ɖ߂�
  GFL_UI_KEY_SetRepeatSpeed( pWork->key_repeat_speed, pWork->key_repeat_wait );
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
  OS_TPrintf("BAG STATE: %d\n",line);
  _changeState(pWork, state);
}
#endif


#define FBMP_COL_NULL   (0)
#define FBMP_COL_BLACK    (1)
#define FBMP_COL_BLK_SDW  (2)
#define FBMP_COL_RED    (3)
#define FBMP_COL_RED_SDW  (4)
#define FBMP_COL_GREEN    (5)
#define FBMP_COL_GRN_SDW  (6)
#define FBMP_COL_BLUE   (7)
#define FBMP_COL_BLU_SDW  (8)
#define FBMP_COL_PINK   (9)
#define FBMP_COL_PNK_SDW  (10)
#define FBMP_COL_WHITE    (15)
#define _BMPMENULIST_FONTSIZE   (16)




///�I�����j���[�̃��X�g
static BMPMENULIST_HEADER _itemMenuListHeader = {
  NULL,     // �\�������f�[�^�|�C���^
  NULL,         // �J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
  NULL,         // ���\�����Ƃ̃R�[���o�b�N�֐�
  NULL,                      // GF_BGL_BMPWIN* win
  9,// ���X�g���ڐ�
  9,// �\���ő區�ڐ�
  0,            // ���x���\���w���W
  16,           // ���ڕ\���w���W
  0,            // �J�[�\���\���w���W
  0,            // �\���x���W
  FBMP_COL_BLACK,     // �����F
  FBMP_COL_WHITE,     // �w�i�F
  FBMP_COL_BLK_SDW,   // �����e�F
  0,            // �����Ԋu�w
  18,           // �����Ԋu�x
  BMPMENULIST_NO_SKIP,    // �y�[�W�X�L�b�v�^�C�v
  0,//FONT_SYSTEM,        // �����w��
  0,            // �a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)
  NULL,                   // work
  _BMPMENULIST_FONTSIZE,      //�����T�C�YX(�h�b�g
  _BMPMENULIST_FONTSIZE,      //�����T�C�YY(�h�b�g
  NULL,   //�\���Ɏg�p���郁�b�Z�[�W�o�b�t
  NULL,   //�\���Ɏg�p����v�����g���[�e�B
  NULL,   //�\���Ɏg�p����v�����g�L���[
  NULL,   //�\���Ɏg�p����t�H���g�n���h��
};

//-----------------------------------------------------------------------------
/**
 *  @brief  �E�B���h�E�`�揉����
 *
 *  @param  pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _windowCreate(FIELD_ITEMMENU_WORK* pWork)
{
  _windowRewrite(pWork);
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �E�B���h�E�ĕ`��
 *
 *  @param  pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _windowRewrite(FIELD_ITEMMENU_WORK* pWork)
{
  // �L�[���쎞�̂݁A���ʂ��X�V
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY )
  {
    ITEMDISP_upMessageRewrite(pWork);
  }
  else
  {
    ITEMDISP_upMessageClean(pWork);
  }

  ITEMDISP_CellMessagePrint(pWork);
  BTN_DrawCheckBox( pWork );
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

//-----------------------------------------------------------------------------
/**
 *  @brief  �A�C�e���̔��l���擾
 *
 *  @param  int item_no ����A�C�e��NO
 *  @param  input_num   �����
 *  @param  heapID      �q�[�vID
 *
 *  @retval s32 ���l
 */
//-----------------------------------------------------------------------------
s32 ITEMMENU_SellPrice( int item_no, int input_num, HEAPID heapID )
{
  s32 val;

  val = ITEM_GetParam( item_no, ITEM_PRM_PRICE, heapID );

  // ����l�͔���
  val /= 2;

  // ��
  val *= input_num;

  return val;
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

//-----------------------------------------------------------------------------
/**
 *  @brief  �A�C�e���������W�X�g
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *  @param  bufID
 *  @param  itemID 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
void ITEMMENU_WordsetItemName( FIELD_ITEMMENU_WORK* pWork, u32 bufID, u32 itemID )
{
    GFL_MSG_GetString( pWork->MsgManagerItemName, itemID, pWork->pItemNameBuf );
    WORDSET_RegisterWord( pWork->WordSet, bufID, pWork->pItemNameBuf, 0, TRUE, PM_LANG );
}


//-----------------------------------------------------------------------------
/**
 *  @brief  �A�C�e���f�[�^����
 *
 *  @param  pWork
 *  @param  no1
 *  @param  no2
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _ItemChangeSingle(FIELD_ITEMMENU_WORK* pWork, int no1, int no2 )
{
  ITEM_ST temp;
  GFL_STD_MemCopy(&pWork->ScrollItem[no1], &temp, sizeof(ITEM_ST));
  GFL_STD_MemCopy(&pWork->ScrollItem[no2],&pWork->ScrollItem[no1], sizeof(ITEM_ST));
  GFL_STD_MemCopy(&temp, &pWork->ScrollItem[no2], sizeof(ITEM_ST));
}


//-----------------------------------------------------------------------------
/**
 *  @brief  �A�C�e����������
 *
 *  @param  pWork
 *  @param  no1
 *  @param  no2
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
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
  ITEMDISP_ScrollCursorChangePos( pWork );
  ITEMDISP_PocketMessage(pWork, newpocket);
  ITEMDISP_ChangePocketCell( pWork,newpocket );
  ITEMDISP_ItemInfoWindowChange(pWork,newpocket);
}
//-----------------------------------------------------------------------------
/**
 *  @brief  ���L�[�������ꂽ���̏���
 *
 *  @param  pWork
 *  @param  length
 *
 *  @retval none
 */
//-----------------------------------------------------------------------------
static BOOL _posplus( FIELD_ITEMMENU_WORK * pWork, int length, BOOL loop )
{
  int tmp_cursor, tmp_list;

  // �A�C�e���Ȃ�
  if( length == 0 ){ return FALSE; }

  tmp_cursor = pWork->curpos;
  tmp_list   = pWork->oamlistpos;

/*
  if((pWork->curpos==4) && ((pWork->oamlistpos+7) < length)){
    //�J�[�\���͂��̂܂܂Ń��X�g���ړ�
    pWork->oamlistpos++;
  }
  else if((pWork->curpos==4) && ((pWork->curpos+1) < length)){
    //���X�g�̏I�[�܂ŗ����̂ŃJ�[�\�����ړ�
    pWork->curpos++;
  }
  else if((pWork->curpos!=5) && ((pWork->curpos+1) < length)){
    //���X�g�̏I�[�܂ŗ����̂ŃJ�[�\�����ړ�
    pWork->curpos++;
  }else{
    // ���[�v
    if( loop == TRUE ){
      pWork->oamlistpos = -1;
      pWork->curpos = 0;
    }
  }
*/
  // �J�[�\���͂��̂܂܂Ń��X�g���ړ�
  if( pWork->curpos == 5 && ( pWork->oamlistpos + 7 ) < length ){
    pWork->oamlistpos++;
  // ���X�g�̏I�[�܂ŗ����̂ŃJ�[�\�����ړ�
  }else if( pWork->curpos != 5 && ( pWork->curpos + 1 ) < length ){
    pWork->curpos++;
  }else{
    // ���[�v
    if( loop == TRUE ){
      pWork->oamlistpos = -1;
      pWork->curpos = 0;
    }
  }

  if( tmp_cursor != pWork->curpos || tmp_list != pWork->oamlistpos ){
    return TRUE;
  }
  return FALSE;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  ��L�[�������ꂽ���̏���
 *
 *  @param  pWork
 *  @param  length
 *
 *  @retval none
 */
//-----------------------------------------------------------------------------
static BOOL _posminus( FIELD_ITEMMENU_WORK * pWork, int length, BOOL loop )
{
  int tmp_cursor, tmp_list;

  // �A�C�e���Ȃ�
  if( length == 0 ){ return FALSE; }

  tmp_cursor = pWork->curpos;
  tmp_list   = pWork->oamlistpos;

/*
  if((pWork->curpos==1) && (pWork->oamlistpos!=-1)){
    //�J�[�\���͂��̂܂܂Ń��X�g���ړ�
    pWork->oamlistpos--;
  }
  else if((pWork->curpos==1)){
    //���X�g�̏I�[�܂ŗ����̂ŃJ�[�\�����ړ�
    pWork->curpos--;
  }
  else if(pWork->curpos != 0){
    pWork->curpos--;
  }else{
    // ���[�v
    if( loop == TRUE ){
      pWork->oamlistpos = length - 7;
      if( pWork->oamlistpos < -1 ){
        pWork->oamlistpos = -1;
      }
      pWork->curpos = 5;
      if( pWork->curpos >= length ){
        pWork->curpos = length-1;
      }
    }
  }
*/
  // �J�[�\���͂��̂܂܂Ń��X�g���ړ�
  if( pWork->curpos == 0 && pWork->oamlistpos != -1 ){
    pWork->oamlistpos--;
  // ���X�g�̏I�[�܂ŗ����̂ŃJ�[�\�����ړ�
  }else if( pWork->curpos != 0 ){
    pWork->curpos--;
  }else{
    // ���[�v
    if( loop == TRUE ){
      pWork->oamlistpos = length - 7;
      if( pWork->oamlistpos < -1 ){
        pWork->oamlistpos = -1;
      }
      pWork->curpos = 5;
      if( pWork->curpos >= length ){
        pWork->curpos = length-1;
      }
    }
  }

  if( tmp_cursor != pWork->curpos || tmp_list != pWork->oamlistpos ){
    return TRUE;
  }
  return FALSE;
}


//------------------------------------------------------------------------------
/**
 * @brief   �X�N���[���̏���
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _itemScrollCheck(FIELD_ITEMMENU_WORK* pWork)
{
  u32 x, y;
  int length = ITEMMENU_GetItemPocketNumber(pWork);

  if( length >= ITEMMENU_SCROLLBAR_ENABLE_NUM && GFL_UI_TP_GetPointCont(&x, &y) == TRUE ){
    // �͈͔���
    if( (x >= (32*8)) || (x <= (30*8)) ){
      return FALSE;
    }
    // ����`�F�b�N
    if( pWork->scrollMode == FALSE ){
      if( GFL_UI_TP_GetTrg() == TRUE ){
        if( y < _SCROLL_TOP_Y || y > _SCROLL_BOTTOM_Y ){
          return FALSE;
        }
        pWork->scrollMode = TRUE;
      }else{
        return FALSE;
      }
    }

    // �x���W�␳
    if( y < _SCROLL_TOP_Y ){
      y = _SCROLL_TOP_Y;
    }else if( y > _SCROLL_BOTTOM_Y ){
      y = _SCROLL_BOTTOM_Y;
    }

    // �^�b�`���
    KTST_SetDraw( pWork, FALSE );

    {
      int tmp_scr = pWork->oamlistpos;

      pWork->oamlistpos = SCROLLBAR_GetCount( length+1-6, y, _SCROLL_TOP_Y, _SCROLL_BOTTOM_Y, 0 ) - 1;

      ITEMDISP_ScrollCursorMove( pWork );   // �X�N���[���o�[OAM���W��ύX

      if( pWork->oamlistpos != tmp_scr ){
        PMSND_PlaySE( SE_BAG_SLIDE );
        return TRUE;
      }
    }
  }

  return FALSE;
}


//------------------------------------------------------------------------------
/**
 * @brief   �L�[�̓����̏���(�A�C�e���ړ����[�h)
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

    if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN ){
      bChange = _posplus( pWork, length, TRUE );
    }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP ){
      bChange = _posminus( pWork, length, TRUE );
    }else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_R ){
      u32 i;
      for( i=0; i<6; i++ ){
        if( _posplus( pWork, length, FALSE ) == FALSE ){
          break;
        }
      }
      if( i != 0 ){ bChange = TRUE; }
    }else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_L ){
      u32 i;
      for( i=0; i<6; i++ ){
        if( _posminus( pWork, length, FALSE ) == FALSE ){
          break;
        }
      }
      if( i != 0 ){ bChange = TRUE; }
    }
  }
  if(bChange){
    int newno = ITEMMENU_GetItemIndex(pWork);
    PMSND_PlaySE( SE_BAG_CURSOR_MOVE );
    _ItemChange(pWork, nowno, newno);
  }
  return bChange;
}



//------------------------------------------------------------------------------
/**
 * @brief   �L�[�̓����̏���
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _keyMoveCheck(FIELD_ITEMMENU_WORK* pWork)
{
  BOOL bChange = FALSE;
  {
    int pos = pWork->curpos;
    int length = ITEMMENU_GetItemPocketNumber( pWork);

    if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN ){
      bChange = _posplus( pWork, length, TRUE );
    }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP ){
      bChange = _posminus( pWork, length, TRUE );
    }else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_R ){
      u32 i;
      for( i=0; i<6; i++ ){
        if( _posplus( pWork, length, FALSE ) == FALSE ){
          break;
        }
      }
      if( i != 0 ){ bChange = TRUE; }
    }else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_L ){
      u32 i;
      for( i=0; i<6; i++ ){
        if( _posminus( pWork, length, FALSE ) == FALSE ){
          break;
        }
      }
      if( i != 0 ){ bChange = TRUE; }
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
#define ITEM_MOVE_LX    ( 18*8 )      // �A�C�e���ړ��^�b�`�L���͈́F��
#define ITEM_MOVE_RX    ( 29*8-1 )    // �A�C�e���ړ��^�b�`�L���͈́F�E
#define ITEM_MOVE_UY    ( 12 )        // �A�C�e���ړ��^�b�`�L���͈́F��
#define ITEM_MOVE_DY    ( 156-1 )     // �A�C�e���ړ��^�b�`�L���͈́F��
#define ITEM_MOVE_SY    ( 24 )        // ���X�g�P���ڃT�C�Y

static BOOL _itemMovePositionTouchItem(FIELD_ITEMMENU_WORK* pWork)
{
  u32 x, y;

  if( GFL_UI_TP_GetPointCont( &x, &y ) == TRUE ){
    int length;
    int now_pos, new_pos;
    u32 tp;
    u32 cnt;
    u32 i;

    // �͈̓`�F�b�N
    if( !( x >= ITEM_MOVE_LX && x <= ITEM_MOVE_RX && y >= ITEM_MOVE_UY && y <= ITEM_MOVE_DY ) ){
      return FALSE;
    }

    tp = ( y - ITEM_MOVE_UY ) / ITEM_MOVE_SY;         // �^�b�`�ʒu

    // ���݂̃J�[�\���ʒu
    if( pWork->curpos == tp ){
      return FALSE;
    }

    length  = ITEMMENU_GetItemPocketNumber( pWork );  // �A�C�e����
    now_pos = ITEMMENU_GetItemIndex( pWork );         // ���݂̃J�[�\���ʒu�̃A�C�e��
    cnt     = GFL_STD_Abs( pWork->curpos - tp );      // �J�[�\���ړ��J�E���^

    // ����
    if( pWork->curpos < tp ){
      for( i=0; i<cnt; i++ ){
        if( _posplus( pWork, length, FALSE ) == FALSE ){
          break;
        }
      }
    // ���
    }else if( pWork->curpos > tp ){
      for( i=0; i<cnt; i++ ){
        if( _posminus( pWork, length, FALSE ) == FALSE ){
          break;
        }
      }
    }
    // �ړ��ł��Ȃ�����
    if( i == 0 ){ return FALSE; }

    new_pos = ITEMMENU_GetItemIndex( pWork );     // �ړ���̃J�[�\���ʒu�̃A�C�e��
    _ItemChange( pWork, now_pos, new_pos );       // �A�C�e������ւ�

    KTST_SetDraw( pWork, FALSE );

    return TRUE;
  }

  return FALSE;
}

// �A�C�e���ړ��̃L�����Z���`�F�b�N
static BOOL _itemMoveCancelTouchCheck(void)
{
  u32 x, y;

  if( GFL_UI_TP_GetPointTrg( &x, &y ) == TRUE ){
    if( x >= _BAR_CELL_CURSOR_RETURN && x <= (_BAR_CELL_CURSOR_RETURN+TOUCHBAR_ICON_WIDTH-1) &&
        y >= TOUCHBAR_ICON_Y && y <= (TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1) ){
      return TRUE;
    }
  }
  return FALSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e���ړ����[�h���̃L�[�̏���
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _itemMoveCancel(FIELD_ITEMMENU_WORK* pWork)
{
  ITEMDISP_ChangeMoveModeButton( pWork, TRUE );
//  _CHANGE_STATE( pWork, _itemKindSelectMenu );
  ChangeStateItemKindSelectItemMenu( pWork );
}

static void _itemMovePosition(FIELD_ITEMMENU_WORK* pWork)
{
  int now_pos, new_pos;

  BLINKPALANM_Main( pWork->blwk );

//  GFL_UI_KEY_SetRepeatSpeed(1, 6);

  // �X�N���[�����t���O�N���A
  if( GFL_UI_TP_GetCont() == FALSE ){
    pWork->scrollMode = FALSE;
    if( pWork->moveDrag == TRUE ){
      PMSND_PlaySE( SE_BAG_DECIDE );
      pWork->moveDrag = FALSE;
      MYITEM_ITEM_STCopy(pWork->pMyItem, pWork->ScrollItem, pWork->pocketno, FALSE);
      GFL_CLACT_WK_SetAnmSeq( pWork->clwkCur, 1 );
      ITEMDISP_ChangeMoveModeButton( pWork, TRUE );
      pWork->moveMode = FALSE;
      KTST_SetDraw( pWork, FALSE );
//      _CHANGE_STATE( pWork, _itemKindSelectMenu );
      ChangeStateItemKindSelectItemMenu( pWork );
      return;
    }
  }

  // ����
  if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE|PAD_BUTTON_SELECT) ){
    PMSND_PlaySE( SE_BAG_DECIDE );
    MYITEM_ITEM_STCopy(pWork->pMyItem, pWork->ScrollItem, pWork->pocketno, FALSE);
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkCur, 1 );
    ITEMDISP_ChangeMoveModeButton( pWork, TRUE );
    pWork->moveMode = FALSE;
    KTST_SetDraw( pWork, TRUE );
//    _CHANGE_STATE( pWork, _itemKindSelectMenu );
    ChangeStateItemKindSelectItemMenu( pWork );
    return;
  }
  // �L�����Z���i�L�[�j
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
    PMSND_PlaySE( SE_BAG_CANCEL );
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkCur, 1 );
    pWork->moveMode = FALSE;
    KTST_SetDraw( pWork, TRUE );
//    _CHANGE_STATE( pWork, _itemMoveCancel );
    _windowRewrite( pWork );
    SetEndButtonAnime( pWork, 0, _itemMoveCancel );
    _CHANGE_STATE( pWork, _endButtonAnime );
    return;
  }
  // �L�����Z���i�^�b�`�j
  if( _itemMoveCancelTouchCheck() == TRUE ){
    PMSND_PlaySE( SE_BAG_CANCEL );
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkCur, 1 );
    pWork->moveMode = FALSE;
    KTST_SetDraw( pWork, FALSE );
//    _CHANGE_STATE( pWork, _itemMoveCancel );
    SetEndButtonAnime( pWork, 0, _itemMoveCancel );
    _CHANGE_STATE( pWork, _endButtonAnime );
    return;
  }

  // �X�N���[���o�[�̑���
  now_pos = ITEMMENU_GetItemIndex( pWork );
  if( _itemScrollCheck(pWork) ){
    new_pos = ITEMMENU_GetItemIndex( pWork );
    _ItemChange( pWork, now_pos, new_pos );
    _windowRewrite( pWork );
  // ���X�g�^�b�`����
  }else if( _itemMovePositionTouchItem(pWork) ){
    pWork->moveDrag = TRUE;
    ITEMDISP_ScrollCursorChangePos( pWork );
    _windowRewrite(pWork);
  // �L�[����
  }else if( _keyChangeItemCheck(pWork) ){ 
    ITEMDISP_ScrollCursorChangePos( pWork );
    _windowRewrite(pWork);
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �X�v���[�g�p�`�F�b�N
 *
 *  @param  u16 item_id
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL _check_spray( u16 item_id )
{
  return ( item_id == ITEM_GOORUDOSUPUREE ||
      item_id == ITEM_SIRUBAASUPUREE ||
      item_id == ITEM_MUSIYOKESUPUREE  );
}

enum{
  ITEM_USE_NONE=0,          // �ʏ�g�p
  INNER_USE_BAG,            // �g���ƃo�b�O�̒��Ń��b�Z�[�W���łăo�b�O�I���ɖ߂�
  INNER_USE_AFTER_OUTBAG,   // �g���ƃo�b�O�̒��Ń��b�Z�[�W���łăo�b�O���I��������
};

//-----------------------------------------------------------------------------
/**
 *  @brief  �o�b�O���Ŏg���A�C�e��������
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval TURE:�o�b�O���Ŏg���A�C�e��
 */
//-----------------------------------------------------------------------------
static int BAG_IsInnerItem( u16 item_id )
{
  // �X�v���[
  if(  _check_spray( item_id ) )
  {
    return INNER_USE_BAG;
  } // �_�[�N�X�g�[���E���C�g�X�g�[������
  else if( item_id==ITEM_DAAKUSUTOON || item_id==ITEM_RAITOSUTOON){
    return INNER_USE_AFTER_OUTBAG;
  }
  return FALSE;
}



//-----------------------------------------------------------------------------
/**
 *  @brief  �o�b�O���Ŏg���A�C�e������
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _itemInnerUse( FIELD_ITEMMENU_WORK* pWork )
{
  // �X�v���[
  if( _check_spray( pWork->ret_item ) )
  {
    SAVE_CONTROL_WORK* save = GAMEDATA_GetSaveControlWork( pWork->gamedata );
    ENC_SV_PTR encsv = EncDataSave_GetSaveDataPtr( save );

    // �g�p�`�F�b�N
    if( EncDataSave_CanUseSpray( encsv ) )
    {
      u8 count = ITEM_GetParam( pWork->ret_item, ITEM_PRM_ATTACK, pWork->heapID );

      // �X�v���[����
      EncDataSave_SetSprayCnt( encsv, count );

      // �A�C�e�������炷
      ITEM_Sub( pWork, 1 );

      // ������
      GFL_MSG_GetString( pWork->MsgManager, msg_bag_066, pWork->pStrBuf );
      WORDSET_RegisterPlayerName(pWork->WordSet, 0,  pWork->mystatus );
      ITEMMENU_WordsetItemName( pWork, 1,  pWork->ret_item );
      WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
      ITEMDISP_ItemInfoWindowDispEx( pWork, TRUE );

      // �X�v���[������������Ȃ��̂łƂ肠�����e�L�g�[�ȉ�
      PMSND_PlaySE( SE_BAG_SPRAY );
    }
    else
    {
      // �����Ȃ�����
      GFL_MSG_GetString( pWork->MsgManager, msg_bag_067, pWork->pStrBuf );
      WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
      ITEMDISP_ItemInfoWindowDispEx( pWork, TRUE );
    }

    _CHANGE_STATE(pWork,_itemInnerUseWait);
  }
  else
  {
    GF_ASSERT(0);
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �A�C�e���g�p���b�Z�[�W�E�F�C�g
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _itemInnerUseWait( FIELD_ITEMMENU_WORK* pWork )
{
  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }

  // ���͑҂�
  if( ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ) || GFL_UI_TP_GetTrg() )
  {
    GFL_BG_ClearScreen(GFL_BG_FRAME3_M);

    // �ĕ`��
    _windowRewrite(pWork);

    GFL_CLACT_WK_SetAutoAnmFlag( pWork->clwkScroll, TRUE );
    ITEMDISP_ChangeActive( pWork, TRUE );
//    _CHANGE_STATE(pWork,_itemKindSelectMenu);
    ChangeStateItemKindSelectItemMenu( pWork );
  }
}


// N�̑O�Łu�ق烉�C�g�X�g�[�����g���Ă݂��Ă�v
#define N01R0502_START     ( 1 )
// ���C�g�X�g�[�����g����
#define N01R0502_CHANGE    ( 2 )

//-----------------------------------------------------------------------------
/**
 *  @brief  �o�b�O���Ŏg���A�C�e������
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _itemInnerUseRightStone( FIELD_ITEMMENU_WORK* pWork )
{
  // �X�v���[
  if( pWork->ret_item ==ITEM_DAAKUSUTOON || pWork->ret_item==ITEM_RAITOSUTOON )
  {
    SAVE_CONTROL_WORK* save = GAMEDATA_GetSaveControlWork( pWork->gamedata );
    ENC_SV_PTR encsv = EncDataSave_GetSaveDataPtr( save );
    EVENTWORK *evwork = GAMEDATA_GetEventWork(pWork->gamedata);

    // �g�p�`�F�b�N(N�̑O�Ń��C�g�X�g�[�����g���V�`���G�[�V�������H)
    if( *EVENTWORK_GetEventWorkAdrs(evwork,WK_SYS_N01R0502_ITEMUSE)==N01R0502_START )
    {
      // �g�p���b�Z�[�W�u�Ȃɂ����������߂��Ȃ��v
      if(pWork->ret_item==ITEM_DAAKUSUTOON){
        GFL_MSG_GetString( pWork->MsgManager, msg_bag_n_001w, pWork->pStrBuf );
      }else{
        GFL_MSG_GetString( pWork->MsgManager, msg_bag_n_001b, pWork->pStrBuf );
      }
      WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
      ITEMDISP_ItemInfoWindowDispEx( pWork, TRUE );

      // ������������Ȃ��̂łƂ肠�����e�L�g�[�ȉ�
      PMSND_PlaySE( SE_BAG_RAITOSUTOON );
      _CHANGE_STATE(pWork,_itemInnerUseRightStoneWait);
      
    }
    else
    {
      // �����Ȃ������̂Łu�A�����M�́@���Ƃ΁c�@�����ǂ�������v
      GFL_MSG_GetString( pWork->MsgManager, msg_bag_057, pWork->pStrBuf );
      WORDSET_RegisterPlayerName(pWork->WordSet, 0,  pWork->mystatus );
      WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
      ITEMDISP_ItemInfoWindowDispEx( pWork, TRUE );
      _CHANGE_STATE(pWork,_itemInnerUseWait);
    }

  }
  else
  {
    GF_ASSERT(0);
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���C�g�X�g�[���E�u���b�N�X�g�[����p���b�Z�[�W�E�F�C�g�i�ƌ㏈���j
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _itemInnerUseRightStoneWait( FIELD_ITEMMENU_WORK* pWork )
{
  EVENTWORK *evwork = GAMEDATA_GetEventWork(pWork->gamedata);
  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }

  // ���͑҂�
  if( ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ) || GFL_UI_TP_GetTrg() )
  {
    GFL_BG_ClearScreen(GFL_BG_FRAME3_M);

    // �ĕ`��
    _windowRewrite(pWork);

    pWork->ret_code = BAG_NEXTPROC_EXIT;
    _CHANGE_STATE(pWork,NULL);

    // �C�x���g���[�N�̏�������
    *EVENTWORK_GetEventWorkAdrs(evwork,WK_SYS_N01R0502_ITEMUSE) = N01R0502_CHANGE;
    
  }
}



//----------------------------------------------------------------------------------
/**
 * @brief ����g���Ȃ������ۂ̃��b�Z�[�W�\���i����ExpandStr�ɏ�������Ă���Ƃ��āj
 *
 * @param   pWork   
 */
//----------------------------------------------------------------------------------
static void _itemInnerUseError( FIELD_ITEMMENU_WORK* pWork )
{
  
  ITEMDISP_ItemInfoWindowDispEx( pWork, TRUE );
  _CHANGE_STATE(pWork,_itemInnerUseWait);

}

static int _check_Cycle( FIELD_ITEMMENU_WORK *pWork );
static int _check_MailView( FIELD_ITEMMENU_WORK *pWork );
static int _check_BattleRecorder( FIELD_ITEMMENU_WORK *pWork );
static int _check_Turizao( FIELD_ITEMMENU_WORK *pWork );
static int _check_AmaiMitu( FIELD_ITEMMENU_WORK *pWork );
static int _check_TomodatiTechou( FIELD_ITEMMENU_WORK *pWork );
static int _check_TownMap( FIELD_ITEMMENU_WORK *pWork );
static int _check_Cycle( FIELD_ITEMMENU_WORK *pWork );
static int _check_DowsingMachine( FIELD_ITEMMENU_WORK *pWork );
static int _check_AnanukenoHimo( FIELD_ITEMMENU_WORK *pWork );

// �A�����M���m�̂ǂ����g���Ȃ��惁�b�Z�[�W
#define MSGID_ITEMUSE_ERROR_ARARAGI   ( msg_bag_059 )
#define MSGID_ITEMUSE_CYCLE_NOSTOP    ( msg_bag_058 )

//----------------------------------------------------------------------------------
/**
 * @brief �y����g�p�\�`�F�b�N�E�����z���]��
 *
 * @param   pWork   
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int _check_Cycle( FIELD_ITEMMENU_WORK *pWork )
{

  if(BAG_MENU_TSUKAU==pWork->ret_code2){
    if(ITEMUSE_GetItemUseCheck( pWork->icwk, ITEMCHECK_CYCLE)){
      pWork->ret_code = BAG_NEXTPROC_RIDECYCLE;  //�̂�
      _CHANGE_STATE(pWork,NULL);
      return TRUE;
    }else{
      // �̂�Ȃ�����
      GFL_MSG_GetString( pWork->MsgManager, MSGID_ITEMUSE_ERROR_ARARAGI, pWork->pStrBuf );
      WORDSET_RegisterPlayerName( pWork->WordSet, 0, pWork->mystatus );
      WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
      // ������̂�pExpStrBuf�ɗp�ӂ��Ă����J�ڐ�ŕ\��������
      _CHANGE_STATE(pWork,_itemInnerUseError);
      return FALSE;
    }
  }else if(BAG_MENU_ORIRU==pWork->ret_code2){
    if(ITEMUSE_GetItemUseCheck( pWork->icwk, ITEMCHECK_CYCLE)){
      pWork->ret_code = BAG_NEXTPROC_DROPCYCLE;  //�����
      _CHANGE_STATE(pWork,NULL);
      return TRUE;
    }else{
      // �����Ȃ�����
      GFL_MSG_GetString( pWork->MsgManager, MSGID_ITEMUSE_CYCLE_NOSTOP, pWork->pStrBuf );
      WORDSET_RegisterPlayerName( pWork->WordSet, 0, pWork->mystatus );
      WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
      // ������̂�pExpStrBuf�ɗp�ӂ��Ă����J�ڐ�ŕ\��������
      _CHANGE_STATE(pWork,_itemInnerUseError);
      return FALSE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------------
/**
 * @brief �y����g�p�\�`�F�b�N�E�����z�^�E���}�b�v
 */
//----------------------------------------------------------------------------------
static int _check_TownMap( FIELD_ITEMMENU_WORK *pWork )
{
  if(ITEMUSE_GetItemUseCheck( pWork->icwk, ITEMCHECK_TOWNMAP)){
    pWork->ret_code = BAG_NEXTPROC_TOWNMAP;  //�^�E���}�b�v
    _CHANGE_STATE(pWork,NULL);
    return TRUE;
  }else{
    // �g���Ȃ�
    GFL_MSG_GetString( pWork->MsgManager, MSGID_ITEMUSE_ERROR_ARARAGI, pWork->pStrBuf );
    WORDSET_RegisterPlayerName( pWork->WordSet, 0, pWork->mystatus );
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
    // ������̂�pExpStrBuf�ɗp�ӂ��Ă����J�ڐ�ŕ\��������
    _CHANGE_STATE(pWork,_itemInnerUseError);
    return FALSE;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * @brief �y����g�p�\�`�F�b�N�E�����z�Ƃ������蒠
 */
//----------------------------------------------------------------------------------
static int _check_TomodatiTechou( FIELD_ITEMMENU_WORK *pWork )
{
  if(ITEMUSE_GetItemUseCheck( pWork->icwk, ITEMCHECK_WIFINOTE)){
    pWork->ret_code = BAG_NEXTPROC_FRIENDNOTE;  //�^�E���}�b�v
    _CHANGE_STATE(pWork,NULL);
    return TRUE;
  }else{
    // �g���Ȃ�
    GFL_MSG_GetString( pWork->MsgManager, MSGID_ITEMUSE_ERROR_ARARAGI, pWork->pStrBuf );
    WORDSET_RegisterPlayerName( pWork->WordSet, 0, pWork->mystatus );
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
    // ������̂�pExpStrBuf�ɗp�ӂ��Ă����J�ڐ�ŕ\��������
    _CHANGE_STATE(pWork,_itemInnerUseError);
    return FALSE;
  }
  return FALSE;
  
}
//----------------------------------------------------------------------------------
/**
 * @brief �y����g�p�\�`�F�b�N�E�����z�o�g�����R�[�_�[
 */
//----------------------------------------------------------------------------------
static int _check_BattleRecorder( FIELD_ITEMMENU_WORK *pWork )
{
  
  if(ITEMUSE_GetItemUseCheck( pWork->icwk, ITEMCHECK_BATTLE_RECORDER)){
    pWork->ret_code = BAG_NEXTPROC_BATTLERECORDER;  //�o�g�����R�[�_�[
    _CHANGE_STATE(pWork,NULL);
    return TRUE;
  }else{
    // �g���Ȃ�
    GFL_MSG_GetString( pWork->MsgManager, MSGID_ITEMUSE_ERROR_ARARAGI, pWork->pStrBuf );
    WORDSET_RegisterPlayerName( pWork->WordSet, 0, pWork->mystatus );
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
    // ������̂�pExpStrBuf�ɗp�ӂ��Ă����J�ڐ�ŕ\��������
    _CHANGE_STATE(pWork,_itemInnerUseError);
    return FALSE;
  
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * @brief �y����g�p�\�`�F�b�N�E�����z���܂��~�c
 */
//----------------------------------------------------------------------------------
static int _check_AmaiMitu( FIELD_ITEMMENU_WORK *pWork )
{
  if(ITEMUSE_GetItemUseCheck( pWork->icwk, ITEMCHECK_AMAIMITU)){
      pWork->ret_code = BAG_NEXTPROC_AMAIMITU;  //���܂��~�c
    _CHANGE_STATE(pWork,NULL);
    return TRUE;
  }else{
    // �g���Ȃ�
    GFL_MSG_GetString( pWork->MsgManager, MSGID_ITEMUSE_ERROR_ARARAGI, pWork->pStrBuf );
    WORDSET_RegisterPlayerName( pWork->WordSet, 0, pWork->mystatus );
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
    // ������̂�pExpStrBuf�ɗp�ӂ��Ă����J�ڐ�ŕ\��������
    _CHANGE_STATE(pWork,_itemInnerUseError);
    return FALSE;
  
  }
  return FALSE;
  
}
//----------------------------------------------------------------------------------
/**
 * @brief �y����g�p�\�`�F�b�N�E�����z�肴��
 */
//----------------------------------------------------------------------------------
static int _check_Turizao( FIELD_ITEMMENU_WORK *pWork )
{
  if(ITEMUSE_GetItemUseCheck( pWork->icwk, ITEMCHECK_TURIZAO)){
      pWork->ret_code = BAG_NEXTPROC_TURIZAO;  //�肴��
    _CHANGE_STATE(pWork,NULL);
    return TRUE;
  }else{
    // �̂�Ȃ�����
    GFL_MSG_GetString( pWork->MsgManager, MSGID_ITEMUSE_ERROR_ARARAGI, pWork->pStrBuf );
    WORDSET_RegisterPlayerName( pWork->WordSet, 0, pWork->mystatus );
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
    // ������̂�pExpStrBuf�ɗp�ӂ��Ă����J�ڐ�ŕ\��������
    _CHANGE_STATE(pWork,_itemInnerUseError);
    return FALSE;
  }
  return FALSE;
  
}
//----------------------------------------------------------------------------------
/**
 * @brief �y����g�p�\�`�F�b�N�E�����z���[��
 */
//----------------------------------------------------------------------------------
static int _check_MailView( FIELD_ITEMMENU_WORK *pWork )
{
  if(pWork->ret_code2==BAG_MENU_MIRU){
    if(ITEMUSE_GetItemUseCheck( pWork->icwk, ITEMCHECK_MAIL)){
        pWork->ret_code = BAG_NEXTPROC_MAILVIEW;  //���[���{��
      _CHANGE_STATE(pWork,NULL);
      return TRUE;
    }else{
      // �g���Ȃ�
      GFL_MSG_GetString( pWork->MsgManager, MSGID_ITEMUSE_ERROR_ARARAGI, pWork->pStrBuf );
      WORDSET_RegisterPlayerName( pWork->WordSet, 0, pWork->mystatus );
      WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
      // ������̂�pExpStrBuf�ɗp�ӂ��Ă����J�ڐ�ŕ\��������
      _CHANGE_STATE(pWork,_itemInnerUseError);
      return FALSE;
    
    }
  }
  return FALSE;
  
}


//----------------------------------------------------------------------------------
/**
 * @brief �y����g�p�\�`�F�b�N�E�����z�_�E�W���O�}�V��
 */
//----------------------------------------------------------------------------------
static int _check_DowsingMachine( FIELD_ITEMMENU_WORK* pWork )
{
  if( BAG_MENU_TSUKAU == pWork->ret_code2 )
  {
    if( ITEMUSE_GetItemUseCheck( pWork->icwk, ITEMCHECK_DOWSINGMACHINE ) )
    {
      pWork->ret_code = BAG_NEXTPROC_DOWSINGMACHINE;  // �o�b�O�����j���[�����āA�_�E�W���O�}�V�����g��
      _CHANGE_STATE( pWork, NULL );
      return TRUE;
    }
    else
    {
      // �g���Ȃ�
      GFL_MSG_GetString( pWork->MsgManager, MSGID_ITEMUSE_ERROR_ARARAGI, pWork->pStrBuf );
      WORDSET_RegisterPlayerName( pWork->WordSet, 0, pWork->mystatus );
      WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
      // ������̂�pExpStrBuf�ɗp�ӂ��Ă����J�ڐ�ŕ\��������
      _CHANGE_STATE(pWork,_itemInnerUseError);
      return FALSE;
    }
  }
  return FALSE;
}


//----------------------------------------------------------------------------------
/**
 * @brief ���Ȃʂ��̃q���`�F�b�N
 *
 * @param   pWork   
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int _check_AnanukenoHimo( FIELD_ITEMMENU_WORK *pWork )
{
  if(BAG_MENU_TSUKAU == pWork->ret_code2){
    if( ITEMUSE_GetItemUseCheck( pWork->icwk, ITEMCHECK_ANANUKENOHIMO ) )
    {
      pWork->ret_code = BAG_NEXTPROC_ANANUKENOHIMO;  // �o�b�O�����j���[�����āA�_�E�W���O�}�V�����g��
      _CHANGE_STATE( pWork, NULL );
      return TRUE;
    }
    else
    { // �g���Ȃ�
      GFL_MSG_GetString( pWork->MsgManager, MSGID_ITEMUSE_ERROR_ARARAGI, pWork->pStrBuf );
      WORDSET_RegisterPlayerName( pWork->WordSet, 0, pWork->mystatus );
      WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
      // ������̂�pExpStrBuf�ɗp�ӂ��Ă����J�ڐ�ŕ\��������
      _CHANGE_STATE(pWork,_itemInnerUseError);
      return FALSE;
    }
  }
  return FALSE;
}


//----------------------------------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------------------------------
typedef struct{
  u16 item;
  u16 num;
  int (*check)(FIELD_ITEMMENU_WORK   *pWork);
} ITEMUSE_FUNC_DATA ;

static const ITEMUSE_FUNC_DATA ItemUseFuncTable[]={
  { // ���]�ԏ��E�~���`�F�b�N
    ITEM_ZITENSYA,  1,
    _check_Cycle,
  },
  { // �^�E���}�b�v
    ITEM_TAUNMAPPU,  1,
    _check_TownMap,
  },
  { // �Ƃ������蒠
    ITEM_TOMODATITETYOU,  1,
    _check_TomodatiTechou,
  },
  { // ���܂��~�c
    ITEM_AMAIMITU,  1,
    _check_AmaiMitu,
  },
  { // �肴��
    ITEM_SUGOITURIZAO,  1,
    _check_Turizao,
  },
  { // �o�g�����R�[�_�[
    ITEM_BATORUREKOODAA,  1,
    _check_BattleRecorder,
  },
  { // �_�E�W���O�}�V��
    ITEM_DAUZINGUMASIN,  1,
    _check_DowsingMachine,
  },
  { // ���Ȃʂ��̃q��
    ITEM_ANANUKENOHIMO,  1,
    _check_AnanukenoHimo,
  },
};

#define ITEMUSE_MAX (NELEMS(ItemUseFuncTable))

//----------------------------------------------------------------------------------
/**
 * @brief 
 *
 * @param   item    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int _hit_item( u16 item )
{
  int i;
  for(i=0;i<ITEMUSE_MAX;i++){
    const ITEMUSE_FUNC_DATA *dat = &ItemUseFuncTable[i];
//    OS_Printf("search=%d, num=%d, item=%d\n", dat->item, dat->num, item);
    if(item>=dat->item && item<(dat->item+dat->num)){
      return i;
    }
  }
  return -1;
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

    switch(pWork->ret_code2){
     // ------------����--------------
    case BAG_MENU_TSUKAU:  
      ITEMDISP_ChangeRetButtonActive( pWork, FALSE );   // �߂�{�^�����p�b�V�u��

      if( pWork->pocketno != BAG_POKE_EVENT){ //��؂Ȃ��̈ȊO�̓r�[�R�����΂�
        GAMEBEACON_Set_UseItem( pWork->ret_item );
      }
      if((pWork->pocketno==BAG_POKE_WAZA)){         //�Z�}�V��
        _CHANGE_STATE(pWork,_itemTecniqueUseInit);
      }else if(1==ITEM_GetParam( pWork->ret_item, ITEM_PRM_EVOLUTION, pWork->heapID )){
        pWork->ret_code = BAG_NEXTPROC_EVOLUTION;   //�i���A�C�e��
        _CHANGE_STATE(pWork,NULL);
      }else if(_hit_item(pWork->ret_item)>=0){    // �ǂ����g�p�`�F�b�N�𑝂₹��\��
        ItemUseFuncTable[_hit_item(pWork->ret_item)].check( pWork );
      }else {
        // �o�b�O���Ŏg���A�C�e������
        int ret = BAG_IsInnerItem( pWork->ret_item );
        if( ret==INNER_USE_BAG )
        {
          _CHANGE_STATE(pWork,_itemInnerUse); //���b�Z�[�W�E�F�C�g���Ēʏ탁�j���[�ɂ��ǂ�
        }
        else if(ret==INNER_USE_AFTER_OUTBAG)
        {
          _CHANGE_STATE(pWork,_itemInnerUseRightStone);
        }
        else
        {
          // �ʏ�́u�����v��WB�ł̓o�b�O�𔲂�����Ɏg������
          pWork->ret_code = BAG_NEXTPROC_ITEMUSE;
          _CHANGE_STATE(pWork,NULL);
        }
      }
      break;
    // ----------�����---------------
    case BAG_MENU_ORIRU:
      OS_Printf("������I�����ꂽ\n");
      ITEMDISP_ChangeRetButtonActive( pWork, FALSE );   // �߂�{�^�����p�b�V�u��
      if(ITEMUSE_GetItemUseCheck( pWork->icwk, ITEMCHECK_CYCLE)){
        pWork->ret_code = BAG_NEXTPROC_DROPCYCLE;  //�����
        _CHANGE_STATE(pWork,NULL);
      }else{
        // �����Ȃ�����
        GFL_MSG_GetString( pWork->MsgManager, MSGID_ITEMUSE_CYCLE_NOSTOP, pWork->pStrBuf );
        WORDSET_RegisterPlayerName( pWork->WordSet, 0, pWork->mystatus );
        WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
        // ������̂�pExpStrBuf�ɗp�ӂ��Ă����J�ڐ�ŕ\��������
        _CHANGE_STATE(pWork,_itemInnerUseError);
      }
      break;
    // ----------���Ă�---------------
    case BAG_MENU_SUTERU:   
      _CHANGE_STATE(pWork,_itemTrash);
      break;
    // ----------�Ƃ��낭------------
    // ----------��������------------
    case BAG_MENU_TOUROKU:  
    case BAG_MENU_KAIZYO:   
      SHORTCUT_SetEventItem( pWork, pWork->curpos );
      ITEMDISP_ChangeActive( pWork, TRUE );
      GFL_CLACT_WK_SetAutoAnmFlag( pWork->clwkScroll, TRUE );
//      _CHANGE_STATE(pWork, _itemKindSelectMenu);
      ChangeStateItemKindSelectItemMenu( pWork );
      break;
    // ----------�݂�------------
    case BAG_MENU_MIRU:
      ITEMDISP_ChangeRetButtonActive( pWork, FALSE );   // �߂�{�^�����p�b�V�u��
      if(pWork->ret_item>=ITEM_HAZIMETEMEERU && pWork->ret_item<=ITEM_BURIZZIMEERUw){
        pWork->ret_code = BAG_NEXTPROC_MAILVIEW;  //���[���{��
        _CHANGE_STATE(pWork,NULL);
      }else{
        GF_ASSERT(0);
      }
      
      break;
    // ----------��߂�------------
    case BAG_MENU_YAMERU:
      // �^�b�`��ԂŔ������̂Ȃ�N���A
      if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ) {
        ITEMDISP_upMessageClean( pWork ); // �N���A
      }
      // �^�b�`�J�ڂȂ��\����
      KTST_SetDraw( pWork, (GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY) );
      ITEMDISP_ChangeActive( pWork, TRUE );
      GFL_CLACT_WK_SetAutoAnmFlag( pWork->clwkScroll, TRUE );
//      _CHANGE_STATE(pWork, _itemKindSelectMenu);
      ChangeStateItemKindSelectItemMenu( pWork );
      break;
    // ----------����------------
    case BAG_MENU_URU:
      pWork->ret_code = BAG_NEXTPROC_RETURN;
      _CHANGE_STATE(pWork,NULL);
      break;
    // ----------��������------------
    case BAG_MENU_MOTASERU:
      //@TODO check!!!!
      // �|�P�������X�g �A�C�e�����������鏈��
      //���X�g���痈�����ǂ����̕�������܂����B Ariizumi 100222
      if(pWork->mode == BAG_MODE_POKELIST)
      {
        pWork->ret_code = BAG_NEXTPROC_ITEMEQUIP;
      }
      else
      {
        pWork->ret_code = BAG_NEXTPROC_HAVE;
      }
      ITEMDISP_ChangeRetButtonActive( pWork, FALSE );   // �߂�{�^�����p�b�V�u��
      _CHANGE_STATE(pWork,NULL);
      break;
    default:
      GF_ASSERT(0 && "�Y�����鋓�����Ȃ�");
    }
    bClear = TRUE;
  }

  // �I���I���������烁�j���[�ʂ̏���
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

  // ���艹
//  PMSND_PlaySE( SE_BAG_DECIDE );

  //@TODO ��肭�����������
#if 0
  if(pWork->mode == BAG_MODE_POKELIST)
  {
    // �|�P�������X�g �A�C�e�����������鏈��
    pWork->ret_code = BAG_NEXTPROC_ITEMEQUIP;
    _CHANGE_STATE(pWork,NULL);
  }
  else 
#endif
  // �I���݂̂̏ꍇ
  if( pWork->mode == BAG_MODE_SELECT ){
    pWork->ret_code = BAG_NEXTPROC_RETURN;
    _CHANGE_STATE(pWork,NULL);
  // ����
  }else if( pWork->mode == BAG_MODE_SELL ){
    _CHANGE_STATE(pWork,_itemSellInit);
  }else{
    _itemUseWindowRewrite(pWork);
    ITEMDISP_ItemInfoMessageMake( pWork,pWork->ret_item );
    ITEMDISP_ItemInfoWindowDisp( pWork );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->clwkScroll, FALSE );
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
  u32 ret=0;
  BOOL bChange=FALSE;

  // �X�N���[�����t���O�N���A
  if( GFL_UI_TP_GetCont() == FALSE ){
    pWork->scrollMode = FALSE;
  }

//  GFL_UI_KEY_SetRepeatSpeed(1, 6);

  GFL_BMN_Main( pWork->pButton );

  BLINKPALANM_Main( pWork->blwk );

  if(pWork->state == _endButtonAnime){
    // �I�����̓L�[����
    return;
  }

  // �L�����Z��
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
    pWork->ret_code = BAG_NEXTPROC_RETURN;
    pWork->ret_item = ITEM_DUMMY_DATA;
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
//    _CHANGE_STATE(pWork,NULL);
    SetEndButtonAnime( pWork, 0, NULL );
    _CHANGE_STATE( pWork, _endButtonAnime );
    return;
  // �����I��
  }else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
    pWork->ret_code = BAG_NEXTPROC_EXIT;
    pWork->ret_item = ITEM_DUMMY_DATA;
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
//    _CHANGE_STATE(pWork,NULL);
    SetEndButtonAnime( pWork, 1, NULL );
    _CHANGE_STATE( pWork, _endButtonAnime );
    return;
  // �o�^
  }else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ){
    if( pWork->pocketno == BAG_POKE_EVENT ){
      SHORTCUT_SetEventItem( pWork, pWork->curpos );
    }else{
      SHORTCUT_SetPocket( pWork );
    }
    if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
      ITEMDISP_upMessageRewrite(pWork); // ���ʕ\��
      KTST_SetDraw( pWork, TRUE );
    }
    return;
  // �\�[�g
  }else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START ){
    if( pWork->pocketno != BAG_POKE_WAZA && pWork->pocketno != BAG_POKE_NUTS ){
      SORT_Button( pWork );
      KTST_SetDraw( pWork, TRUE );
      _windowRewrite(pWork);
      _CHANGE_STATE( pWork, _sortMessageSet );
      return;
    }
  }

  // �J�[�\���Ȃ��̏�Ԃ�����͂��������ꍇ�A�J�[�\����\�����Ĕ�����
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  {
    if( GFL_UI_KEY_GetTrg() )
    {
      PMSND_PlaySE( SE_BAG_CURSOR_MOVE );
      ITEMDISP_upMessageRewrite(pWork); // ���ʕ\��
      KTST_SetDraw( pWork, TRUE );
      return;
    }
  }

  // ����
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE )
  {
    // �A�C�e�����݃`�F�b�N
    if( ITEMMENU_GetItemPocketNumber( pWork ) > 0 )
    {
      // �A�C�e���ɑ΂��铮��I����ʂ�
//      _CHANGE_STATE(pWork,_itemSelectState);
      _CHANGE_STATE( pWork, _listSelectAnime );
      return;
    }
  }
  // ���ёւ�
  else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
  {
    GFL_STD_MemClear( pWork->ScrollItem, sizeof( pWork->ScrollItem ) );
    MYITEM_ITEM_STCopy( pWork->pMyItem, pWork->ScrollItem, pWork->pocketno, TRUE );  //�擾
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkCur , 2 );
    ITEMDISP_ChangeMoveModeButton( pWork, FALSE );
    pWork->moveMode = TRUE;
//    _CHANGE_STATE(pWork,_itemMovePosition);
    ChangeStateItemMovePosition( pWork );
    return;
  }
  // �^�b�`�X�N���[��
  else if( _itemScrollCheck(pWork) )
  {
    bChange = TRUE;
  }
  // �L�[����
  else if( _keyMoveCheck(pWork) )
  {
    ITEMDISP_ScrollCursorChangePos( pWork );
    bChange = TRUE;
  }

  {
    int oldpocket = pWork->pocketno;

    if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT ){
      pWork->pocketno++;
      if( pWork->pocketno >= BAG_POKE_MAX ){
        pWork->pocketno = 0;
      }
      GFL_CLACT_WK_SetAnmSeq( pWork->clwkBarIcon[BAR_ICON_RIGHT], APP_COMMON_BARICON_CURSOR_RIGHT_ON );
    }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT ){
      pWork->pocketno--;
      if( pWork->pocketno < 0 ){
        pWork->pocketno = BAG_POKE_MAX-1;
      }
      GFL_CLACT_WK_SetAnmSeq( pWork->clwkBarIcon[BAR_ICON_LEFT], APP_COMMON_BARICON_CURSOR_LEFT_ON );
    }
    if( oldpocket != pWork->pocketno ){
      PMSND_PlaySE( SE_BAG_POCKET_MOVE );
      // �|�P�b�g�J�[�\���ړ�
      _pocketCursorChange( pWork, oldpocket, pWork->pocketno );
      // �\�[�g�{�^���\���ؑ�
      SORT_ModeReset( pWork );
      BTN_DrawCheckBox( pWork );
      bChange = TRUE;
    }
  }

  if(bChange){
    _windowRewrite(pWork);
  }
}

//=============================================================================
//
//
//  __TECNIQUE �Z�}�V�� �V�[�P���X
//
//
//=============================================================================

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
      GFL_CLACT_WK_SetAutoAnmFlag( pWork->clwkScroll, TRUE );
      ITEMDISP_ChangeActive( pWork, TRUE );
      if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ) {
        ITEMDISP_upMessageClean( pWork );
        KTST_SetDraw( pWork, FALSE );
      }else{
        KTST_SetDraw( pWork, TRUE );
      }
//      _CHANGE_STATE(pWork,_itemKindSelectMenu);
      ChangeStateItemKindSelectItemMenu( pWork );
    }
    // YESNO�̌�n��
    ITEMDISP_YesNoExit(pWork);
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
  PMSND_PlaySE( SE_BAG_WAZA );

  GFL_MSG_GetString( pWork->MsgManager, msg_bag_065, pWork->pStrBuf );
  WORDSET_RegisterWazaName(pWork->WordSet, 0, ITEM_GetWazaNo( pWork->ret_item ));
  WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
  ITEMDISP_ItemInfoWindowDispEx( pWork, TRUE );
  _CHANGE_STATE(pWork,_itemTecniqueUseWait);
}

//=============================================================================
//
//
//  __TRASH �̂Ă� �V�[�P���X
//
//
//=============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e�����̂Ă�ׂ̑��� �͂��܂�
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTrashEndWait(FIELD_ITEMMENU_WORK* pWork)
{
  if( ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ) || GFL_UI_TP_GetTrg() )
  {
    // �ĕ`��
    GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
    _windowRewrite(pWork);

//    InputNum_ButtonState( pWork, TRUE );

    GFL_CLACT_WK_SetAutoAnmFlag( pWork->clwkScroll, TRUE );
    ITEMDISP_ChangeActive( pWork, TRUE );
//    _CHANGE_STATE(pWork,_itemKindSelectMenu);
    ChangeStateItemKindSelectItemMenu( pWork );
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

    // YESNO�̌�n��
    ITEMDISP_YesNoExit(pWork);

    GFL_BG_ClearScreen(GFL_BG_FRAME3_M);

    if(selectno==0){
      // �̂Ă鉹
      PMSND_PlaySE( SE_BAG_TRASH );

      // �A�C�e�������炷
      ITEM_Sub( pWork, pWork->InputNum );

      GFL_MSG_GetString( pWork->MsgManager, msg_bag_055, pWork->pStrBuf );
      ITEMMENU_WordsetItemName( pWork, 0,  pWork->ret_item );
      WORDSET_RegisterNumber(pWork->WordSet, 1, pWork->InputNum,
                             3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
      WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
      
      ITEMDISP_ItemInfoWindowDisp( pWork );

      _CHANGE_STATE(pWork,_itemTrashYesWait);
    }
    else{
//      InputNum_ButtonState( pWork, TRUE );
      GFL_CLACT_WK_SetAutoAnmFlag( pWork->clwkScroll, TRUE );
      ITEMDISP_ChangeActive( pWork, TRUE );
      KTST_SetDraw( pWork, GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY );
//      _CHANGE_STATE(pWork,_itemKindSelectMenu);
      ChangeStateItemKindSelectItemMenu( pWork );
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
 * @brief   �A�C�e�����̂Ă�ׂ̕\��
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTrash(FIELD_ITEMMENU_WORK* pWork)
{
  pWork->InputNum = 1;  //������

//  InputNum_ButtonState( pWork, FALSE );

  // ���l���͊J�n
  InputNum_Start( pWork, BAG_INPUT_MODE_TRASH );

  GFL_MSG_GetString( pWork->MsgManager, msg_bag_054, pWork->pStrBuf );
  ITEMMENU_WordsetItemName( pWork, 0,  pWork->ret_item );
  WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
  ITEMDISP_ItemInfoWindowDisp( pWork );

  _CHANGE_STATE(pWork,_itemTrashWait);
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e�����̂Ă�ׂ̑���
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTrashWait(FIELD_ITEMMENU_WORK* pWork)
{
  int ret;

  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }

  // ���l���͎又��
  if( InputNum_Main( pWork ) == TRUE ){
    return;
  }

  ret = CheckNumSelTouch();
  if( ret == GFL_UI_TP_HIT_NONE ){
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
      GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
      ret = 0;
    }else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
      GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
      ret = 1;
    }
  }

  // ����
  if( ret == 0 ){
    PMSND_PlaySE( SE_BAG_DECIDE );

    InputNum_Exit( pWork );

    GFL_MSG_GetString( pWork->MsgManager, msg_bag_056, pWork->pStrBuf );
    ITEMMENU_WordsetItemName( pWork, 0,  pWork->ret_item );
    WORDSET_RegisterNumber(pWork->WordSet, 1, pWork->InputNum,
                           3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
    ITEMDISP_ItemInfoWindowDisp( pWork );

    _CHANGE_STATE(pWork,_itemTrashYesNo);
  // �L�����Z��
  }else if( ret == 1 ){
    PMSND_PlaySE( SE_BAG_CANCEL );
    SetEndButtonAnime( pWork, 0, _itemTrashCancel );
    _CHANGE_STATE( pWork, _endButtonAnime );
  }
}

//=============================================================================
//
//
//  __SELL ���� �V�[�P���X
//
//
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *  @brief  ����V�[�P���X ������
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _itemSellInit( FIELD_ITEMMENU_WORK* pWork )
{
//  InputNum_ButtonState( pWork, FALSE );

  GFL_CLACT_WK_SetAutoAnmFlag( pWork->clwkScroll, FALSE );
  ITEMDISP_ChangeActive( pWork, FALSE );

  // �����Ȃ����̔���
  {
    s32 val;
    s32 event;

    val   = ITEM_GetParam( pWork->ret_item, ITEM_PRM_PRICE, pWork->heapID );
    event = ITEM_GetParam( pWork->ret_item, ITEM_PRM_EVENT, pWork->heapID );

    // ���l��0�A�������͏d�v�t���O��ON�̎��͔���Ȃ�
    if( val == 0 || event )
    {
      // �u���������@�����Ƃ邱�Ƃ́@�ł��܂���v
      GFL_MSG_GetString( pWork->MsgManager, mes_shop_093, pWork->pStrBuf );
      ITEMMENU_WordsetItemName( pWork, 0,  pWork->ret_item );
      WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf );
      ITEMDISP_ItemInfoWindowDisp( pWork );

      ITEMDISP_ChangeRetButtonActive( pWork, FALSE );

      _CHANGE_STATE( pWork, _itemSellEndMsgWait );
      return;
    }
  }

  pWork->InputNum = 1;  //������

  // �����Â����\���J�n
  ITEMDISP_GoldDispIn( pWork );

  // �A�C�e�����ЂƂ����Ȃ��ꍇ�A���͂��X�L�b�v
  {
    ITEM_ST * item = ITEMMENU_GetItem( pWork,ITEMMENU_GetItemIndex(pWork) );

    HOSAKA_Printf("item->no:%d \n", item->no );

    if( item->no == 1 )
    {
      _CHANGE_STATE( pWork, _itemSellYesnoInit );
      return;
    }
  }

  // ���l���͊J�n
  InputNum_Start( pWork, BAG_INPUT_MODE_SELL );

  // �u���������@�����@����܂����H�v
  GFL_MSG_GetString( pWork->MsgManager, mes_shop_094, pWork->pStrBuf );
  ITEMMENU_WordsetItemName( pWork, 0,  pWork->ret_item );
  WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
  ITEMDISP_ItemInfoWindowDisp( pWork );

  _CHANGE_STATE( pWork, _itemSellInputWait );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ����V�[�P���X ���͑҂�
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _itemSellInputWait( FIELD_ITEMMENU_WORK* pWork )
{
  int ret;

  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }

  // ����V�[�P���X���͏���
  if( InputNum_Main( pWork ) == TRUE ){
    return;
  }

  // @TODO �o�[�A�C�R���Ȃǃ^�b�`�Ή����܂߂�̂Ń��b�v����
  // �����I��

  ret = CheckNumSelTouch();
  if( ret == GFL_UI_TP_HIT_NONE ){
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
      GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
      ret = 0;
    }else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
      GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
      ret = 1;
    }
  }

   // ����
  if( ret == 0 ){
    PMSND_PlaySE( SE_BAG_DECIDE );

    // ���l���͏I��
    InputNum_Exit( pWork );

    _CHANGE_STATE( pWork, _itemSellYesnoInit );
  // �L�����Z��
  }else if( ret == 1 ){
    PMSND_PlaySE( SE_BAG_CANCEL );
    SetEndButtonAnime( pWork, 0, _itemSellInputCancel );
    _CHANGE_STATE( pWork, _endButtonAnime );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ����V�[�P���X YESNO�J�n
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _itemSellYesnoInit( FIELD_ITEMMENU_WORK* pWork )
{
  // YESNO�J�n
  ITEMDISP_YesNoStart(pWork);

  // ����ł����� XXXXXX�~�� ���Ђ��Ƃ�@�������܂��傤
  {
    s32 val;

    // ���l���擾
    val = ITEMMENU_SellPrice( pWork->ret_item, pWork->InputNum, pWork->heapID );

    GFL_MSG_GetString( pWork->MsgManager, mes_shop_095, pWork->pStrBuf );
    WORDSET_RegisterNumber(pWork->WordSet, 0, val,
                             7, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
    ITEMDISP_ItemInfoWindowDisp( pWork );
  }

    _CHANGE_STATE( pWork, _itemSellYesnoInput );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ����V�[�P���X YESNO�I���܂�
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _itemSellYesnoInput( FIELD_ITEMMENU_WORK* pWork )
{
  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }

  if(APP_TASKMENU_IsFinish(pWork->pAppTask))
  {
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);

    // YESNO�̌�n��
    ITEMDISP_YesNoExit(pWork);

    switch( selectno )
    {
      case 0 :
        // YES
        {
          s32 val;

          // ���l���擾
          val = ITEMMENU_SellPrice( pWork->ret_item, pWork->InputNum, pWork->heapID );

          // �A�C�e�������炷
          ITEM_Sub( pWork, pWork->InputNum );

          // ����������
          MISC_AddGold( GAMEDATA_GetMiscWork(pWork->gamedata), val );

          // ���p��
          PMSND_PlaySE( SE_BAG_SELL );

          // �^�X�N�o�[�����Â����\�����Ȃ���
          ITEMDISP_GoldDispWrite( pWork );

          // �ĕ`��
          _windowRewrite(pWork);

          // �u���������@�킽���� XXXXXX�~ �����Ƃ����v
          GFL_MSG_GetString( pWork->MsgManager, mes_shop_096, pWork->pStrBuf );
          ITEMMENU_WordsetItemName( pWork, 0,  pWork->ret_item );
          WORDSET_RegisterNumber(pWork->WordSet, 1, val, 7, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
          WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
          ITEMDISP_ItemInfoWindowDisp( pWork );

          _CHANGE_STATE( pWork, _itemSellEndMsgWait );
        }
        break;

      case 1 :
        // NO
        KTST_SetDraw( pWork, GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY );
        _CHANGE_STATE( pWork, _itemSellExit );
        break;

      default : GF_ASSERT(0);
    }
  }

}

//-----------------------------------------------------------------------------
/**
 *  @brief  ����V�[�P���X ��������̃��b�Z�[�W�E�B�G�g
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _itemSellEndMsgWait( FIELD_ITEMMENU_WORK* pWork )
{
  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }

  // ���͑҂�
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
    KTST_SetDraw( pWork, TRUE );
    _CHANGE_STATE( pWork, _itemSellExit );
  }else if( GFL_UI_TP_GetTrg() ){
    KTST_SetDraw( pWork, FALSE );
    _CHANGE_STATE( pWork, _itemSellExit );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ����V�[�P���X �I��
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _itemSellExit( FIELD_ITEMMENU_WORK* pWork )
{
  GFL_BG_ClearScreen(GFL_BG_FRAME3_M);

  // �����Â����\���I��
  ITEMDISP_GoldDispOut( pWork );

//  InputNum_ButtonState( pWork, TRUE );

  GFL_CLACT_WK_SetAutoAnmFlag( pWork->clwkScroll, TRUE );
  ITEMDISP_ChangeActive( pWork, TRUE );
//  _CHANGE_STATE( pWork, _itemKindSelectMenu );
  ChangeStateItemKindSelectItemMenu( pWork );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �̂Ă鏈��  �A�C�e�����폜
 *  @retval none
 */
//-----------------------------------------------------------------------------
static void ITEM_Sub( FIELD_ITEMMENU_WORK* pWork, int sub_num )
{
  BOOL rslt;
  ITEM_ST * item = ITEMMENU_GetItem( pWork,ITEMMENU_GetItemIndex(pWork) ); // �I�𒆂̃A�C�e��

  // �莝������A�C�e���폜
  rslt = MYITEM_SubItem(pWork->pMyItem, pWork->ret_item, sub_num, pWork->heapID );

  GF_ASSERT( rslt == TRUE );
  
  // �A�C�e�����Ȃ��Ȃ����ꍇ
  if( MYITEM_GetItemNum( pWork->pMyItem, pWork->ret_item, pWork->heapID ) == 0 )
  {
    int length = ITEMMENU_GetItemPocketNumber( pWork);

    // �X�N���[���o�[�������Ȃ�
    if( length < ITEMMENU_SCROLLBAR_ENABLE_NUM )
    {
      // �J�[�\�����w�����W��������
      pWork->curpos = MATH_IMax( 0, pWork->curpos-1 );
    }
    else
    {
      HOSAKA_Printf("pre oamlistpos=%d\n", pWork->oamlistpos);
      // ���X�g��������
      pWork->oamlistpos = MATH_IMax( pWork->oamlistpos-1 , -1 );
      pWork->bChange = TRUE;
      
      HOSAKA_Printf("oamlistpos=%d\n", pWork->oamlistpos);
    }
  }
}    

//-----------------------------------------------------------------------------
/**
 *  @brief  ���l���� �J�n
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void InputNum_Start( FIELD_ITEMMENU_WORK* pWork, BAG_INPUT_MODE mode )
{
  pWork->InputMode = mode;

  ITEMDISP_NumFrameDisp( pWork );
  ITEMDISP_InputNumDisp( pWork, pWork->InputNum );

  // �A�C�R���\��
  GFL_CLACT_WK_SetDrawEnable( pWork->clwkBarIcon[ BAR_ICON_INPUT_U ], TRUE );
  GFL_CLACT_WK_SetDrawEnable( pWork->clwkBarIcon[ BAR_ICON_INPUT_D ], TRUE );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���l���� �I��
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void InputNum_Exit( FIELD_ITEMMENU_WORK* pWork )
{
  // ���l���̓t���[���N���A
  GFL_BG_ClearScreen(GFL_BG_FRAME3_M);

  // �A�C�R����\��
  GFL_CLACT_WK_SetDrawEnable( pWork->clwkBarIcon[ BAR_ICON_INPUT_U ], FALSE );
  GFL_CLACT_WK_SetDrawEnable( pWork->clwkBarIcon[ BAR_ICON_INPUT_D ], FALSE );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���l���� �又��
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL InputNum_Main( FIELD_ITEMMENU_WORK* pWork )
{
  int tp_result;
  int backup = pWork->InputNum;
  int anm_type;
  BOOL  ret = FALSE;
  ITEM_ST * item = ITEMMENU_GetItem( pWork,ITEMMENU_GetItemIndex(pWork) ); // �I�𒆂̃A�C�e��

  const GFL_UI_TP_HITTBL tp_tbl[] =
  {
    {
      // ��{�^��
      8*11+4, 8*11+8*3+4,
      8*29+2, 8*29+8*3,
    },
    {
      // ���{�^��
      8*14+4, 8*14+8*3+4,
      8*29+2, 8*29+8*3,
    },
    {
      //�I���f�[�^
      GFL_UI_TP_HIT_END,0,0,0,
    }
  };

  //@TODO BTN/TP
//  GFL_BMN_Main( pWork->pButton );

  // �^�b�`����
  tp_result = GFL_UI_TP_HitCont( tp_tbl );

  if( tp_result != GFL_UI_TP_HIT_NONE )
  {
    pWork->countTouch++;
    ret = TRUE;
  }
  else
  {
    pWork->countTouch = 0;
  }
  

  if( pWork->countTouch == 1 || (pWork->countTouch > BAG_UI_TP_CUR_REPEAT_SYNC /*&& (pWork->countTouch % 8) == 0*/ ) )
  {
    switch( tp_result )
    {
    case 0 :
      if( pWork->countTouch > BAG_UI_TP_CUR_HIGHSPEED_SYNC )
      {
        pWork->InputNum += 10;
      }
      else
      {
        pWork->InputNum++;
      }
      anm_type = 0;
      break;

    case 1 :
      if( pWork->countTouch > BAG_UI_TP_CUR_HIGHSPEED_SYNC )
      {
        pWork->InputNum -= 10;
      }
      else
      {
        pWork->InputNum--;
      }
      anm_type = 1;
      break;

    default : GF_ASSERT(0);

    }
  }

  // �L�[����
  if( tp_result == GFL_UI_TP_HIT_NONE )
  {
    if(GFL_UI_KEY_GetRepeat() & PAD_KEY_UP){
      pWork->InputNum++;
      anm_type = 0;
      ret = TRUE;
    }
    else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN){
      pWork->InputNum--;
      anm_type = 1;
      ret = TRUE;
    }
    else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT){
      pWork->InputNum += 10;
      anm_type = 0;
      ret = TRUE;
    }
    else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT){
      pWork->InputNum -= 10;
      anm_type = 1;
      ret = TRUE;
    }
  }

  // ��荞��
  if(item->no < pWork->InputNum)
  {
    pWork->InputNum = 1;
  }
  else if(pWork->InputNum < 1)
  {
    pWork->InputNum = item->no;
  }
  
  // �\���X�V
  if(pWork->InputNum != backup)
  {
    // �ړ���
    PMSND_PlaySE( SE_BAG_CURSOR_MOVE );

    // �{�^���A�j��
    if( anm_type == 0 ){
      GFL_CLACT_WK_SetAnmSeq( pWork->clwkBarIcon[ BAR_ICON_INPUT_U ], 11 );
    }else{
      GFL_CLACT_WK_SetAnmSeq( pWork->clwkBarIcon[ BAR_ICON_INPUT_D ], 10 );
    }

    ITEMDISP_InputNumDisp(pWork,pWork->InputNum);
  }

  return ret;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �{�^���̏�ԕω�
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *  @param  on_off
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
/*
static void InputNum_ButtonState( FIELD_ITEMMENU_WORK* pWork, BOOL on_off )
{
  if( on_off )
  {
    // ������\��

    // �\�[�g�{�^��
    SORT_ModeReset( pWork );
    // �o�[�A�C�R��
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkBarIcon[ BAR_ICON_LEFT ],  BAR_ICON_ANM_LEFT );
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkBarIcon[ BAR_ICON_RIGHT ], BAR_ICON_ANM_RIGHT );
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkBarIcon[ BAR_ICON_EXIT ],  BAR_ICON_ANM_EXIT );
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkBarIcon[ BAR_ICON_CHECK_BOX ],  BAR_ICON_ANM_CHECKBOX );
  }
  else
  {
    // �����Ȃ��\��

    // �\�[�g�{�^��
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkSort , 4 );
    // �o�[�A�C�R���e��
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkBarIcon[ BAR_ICON_LEFT ],      BAR_ICON_ANM_LEFT_OFF );
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkBarIcon[ BAR_ICON_RIGHT ],     BAR_ICON_ANM_RIGHT_OFF );
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkBarIcon[ BAR_ICON_EXIT ],      BAR_ICON_ANM_EXIT_OFF );
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkBarIcon[ BAR_ICON_CHECK_BOX ], BAR_ICON_ANM_CHECKBOX_OFF );
  }
}
*/

#include "item/itemtype_def.h"
//--------------------------------------------------------------
/// ��ޕʃ\�[�g�p���[�N
//==============================================================
typedef struct {
  ITEM_ST st;
  u32 type;
} ITEM_SORTDATA_TYPE;

//-----------------------------------------------------------------------------
/**
 *  @brief  MATH_QSort�p�֐� �|�P�b�g���̃A�C�e����ރ\�[�g
 *
 *  @param  elem1
 *  @param  elem2
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static s32 QSort_Type( void * elem1, void * elem2 )
{
  ITEM_SORTDATA_TYPE * p1 = (ITEM_SORTDATA_TYPE *)elem1;
  ITEM_SORTDATA_TYPE * p2 = (ITEM_SORTDATA_TYPE *)elem2;

  if( p1->type == p2->type ){
    return 0;
  }else if( p1->type > p2->type ){
    return 1;
  }
  return -1;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  MATH_QSort�p�֐� �|�P�b�g���̃A�C�e�������\�[�g
 *
 *  @param  void* elem1
 *  @param  elem2
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
/*
static s32 QSort_ABC( void* elem1, void* elem2 )
{
  ITEM_ST* p1 = (ITEM_ST*) elem1;
  ITEM_ST* p2 = (ITEM_ST*) elem2;
  u16 prio1 = SORT_GetABCPrio( p1->id );
  u16 prio2 = SORT_GetABCPrio( p2->id );

  if( prio1 == prio2 )
  {
    return 0;
  }
  else if( prio1 > prio2 )
  {
    return 1;
  }
  else
  {
    return -1;
  }
}
*/

//-----------------------------------------------------------------------------
/**
 *  @brief  �|�P�b�g���̃A�C�e������ʃ\�[�g
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void SORT_Type( FIELD_ITEMMENU_WORK* pWork )
{
  int i;
  int length;
  ITEM_SORTDATA_TYPE * sort;
  ITEM_ST * item;

  // �������m��
  sort = GFL_HEAP_AllocMemory( pWork->heapID, sizeof(ITEM_SORTDATA_TYPE)*BAG_MYITEM_MAX );
  item = GFL_HEAP_AllocMemory( pWork->heapID, sizeof(ITEM_ST)*BAG_MYITEM_MAX );

  // �擾
  MYITEM_ITEM_STCopy( pWork->pMyItem, item, pWork->pocketno, TRUE );
  length = ITEMMENU_GetItemPocketNumber( pWork );

  // �\�[�g�p�f�[�^����
  // �J�e�S�� << 28 + �\�[�g�ԍ� << 16 + �A�C�e���ԍ�
  for( i=0; i<length; i++ ){
    void * dat = ITEM_GetItemArcData( item[i].id, ITEM_GET_DATA, pWork->heapID );
    sort[i].type = (ITEM_GetBufParam(dat,ITEM_PRM_ITEM_TYPE)<<28) + (ITEM_GetBufParam(dat,ITEM_PRM_SORT_NUMBER)<<16) + item[i].id;
    sort[i].st   = item[i];
    GFL_HEAP_FreeMemory( dat );
  }

  MATH_QSort( (void*)sort, length, sizeof(ITEM_SORTDATA_TYPE), QSort_Type, NULL );

  // �Z�[�u�p�f�[�^�ɐ��`
  for( i=0; i<length; i++ ){
    item[i] = sort[i].st;
  }

  // �ۑ�
  MYITEM_ITEM_STCopy( pWork->pMyItem, item, pWork->pocketno, FALSE );

  // ���������
  GFL_HEAP_FreeMemory( item );
  GFL_HEAP_FreeMemory( sort );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �|�P�b�g���̃A�C�e����ABC�\�[�g
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
/*
static void SORT_ABC( FIELD_ITEMMENU_WORK* pWork )
{
  int i;
  int length;
  ITEM_ST item[ BAG_MYITEM_MAX ];

  //�擾
  MYITEM_ITEM_STCopy(pWork->pMyItem, item, pWork->pocketno, TRUE);
  length = ITEMMENU_GetItemPocketNumber( pWork );

  HOSAKA_Printf("ABC�\�[�g length=%d \n", length );

#ifdef PM_DEBUG
  {
    OS_TPrintf("=============================\n");
    OS_TPrintf(" SORT BEFORE \n");
    OS_TPrintf("=============================\n");

    for( i=0; i<length; i++ )
    {
      OS_TPrintf("[%d] id=%d prio=%d \n",i, item[i].id, SORT_GetABCPrio( item[i].id ) );
    }
  }
#endif

  // �������X�^�b�N���g���؂�����o�b�t�@�m�ۂőΏ�����
  MATH_QSort( (void*)item, length, sizeof(ITEM_ST), QSort_ABC, NULL );

#ifdef PM_DEBUG
  {
    OS_TPrintf("=============================\n");
    OS_TPrintf(" SORT AFTER \n");
    OS_TPrintf("=============================\n");

    for( i=0; i<length; i++ )
    {
      OS_TPrintf("[%d] id=%d prio=%d \n",i, item[i].id, SORT_GetABCPrio( item[i].id ) );
    }
  }
#endif

  // �ۑ�
  MYITEM_ITEM_STCopy(pWork->pMyItem, item, pWork->pocketno, FALSE);
}
*/

//#include "itemsort_abc_def.h"
//-----------------------------------------------------------------------------
/**
 *  @brief  50���\�[�g�p�v���C�I���e�B���擾
 *
 *  @param  u16 item_no
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
/*
static u16 SORT_GetABCPrio( u16 item_no )
{
  GF_ASSERT( item_no-1 < ITEM_DATA_MAX && item_no != ITEM_DUMMY_DATA );

  // itemsort_abc_def.h �Œ�`
  return ItemSortByAbc[ item_no-1 ];
}
*/

//-----------------------------------------------------------------------------
/**
 *  @brief  �\�[�g�{�^������
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void SORT_Button( FIELD_ITEMMENU_WORK* pWork )
{
  PMSND_PlaySE( SE_BAG_SORT );
  GFL_CLACT_WK_SetAnmSeq( pWork->clwkSort, 2 );


/*
  if( pWork->sort_mode == 0 ){
    SORT_ABC( pWork );
  }else if( pWork->sort_mode == 1 ){
    SORT_Type( pWork );
  }
*/

/*
  // ��؂Ȃ��̂�ABC�\�[�g�̂�
  if( pWork->pocketno != BAG_POKE_EVENT )
  {
    // ���[�h�`�F���W
    pWork->sort_mode ^= 1;
  }
*/


}

static void SORT_Main( FIELD_ITEMMENU_WORK* pWork )
{
  SORT_Type( pWork );
  // �ĕ`��
  _windowRewrite( pWork );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �\�[�g ���[�h�����Z�b�g�i�|�P�b�g�ύX���Ȃǁj
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void SORT_ModeReset( FIELD_ITEMMENU_WORK* pWork )
{
//  pWork->sort_mode = 0;

  HOSAKA_Printf("pocketno:%d\n", pWork->pocketno);

  ITEMDISP_ChangeSortButton( pWork );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �\�[�g �A�C�R���Ď�
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
/*
static void SORT_Draw( FIELD_ITEMMENU_WORK* pWork )
{
  if( GFL_CLACT_WK_CheckAnmActive( pWork->clwkSort ) == FALSE ){
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkSort, 0 );
  }
}
*/

//-----------------------------------------------------------------------------
/**
 *  @brief  �L�[�X�e�[�^�X�ɂ��\���ؑ�
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *  @param  on_off TRUE:�\��(�L�[����) FALSE:��\���i�^�b�`����j
 *
 *  @retval none
 */
//-----------------------------------------------------------------------------
static void KTST_SetDraw( FIELD_ITEMMENU_WORK* pWork, BOOL on_off )
{
  // �ړ����[�h���͋����I�ɃL�[����ɂ���
  if( pWork->moveMode == TRUE ){
    on_off = TRUE;
  }

  if( on_off )
  {
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
  }
  else
  {
    GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
  }

  // �A�C�e��0�͋����I�ɃJ�[�\�����o���Ȃ�
  if( ITEMMENU_GetItemPocketNumber(pWork) == 0 )
  {
    on_off = FALSE;
  }

  if( on_off == FALSE ){
    ITEMDISP_ChangeCursorPosPalette( pWork, 0 );
  }else{
    ITEMDISP_ChangeCursorPosPalette( pWork, 1 );
    BLINKPALANM_InitAnimeCount( pWork->blwk );
  }

  // ���ʕ\���؂�ւ�
  ITEMDISP_ItemInfoVanishSet( pWork, on_off );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �V���[�g�J�b�g�ݒ�
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *  @param  pos �\����̈ʒu
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void SHORTCUT_SetEventItem( FIELD_ITEMMENU_WORK* pWork, int pos )
{
  // �\����̈ʒu������̂̈ʒu�֍��W�ϊ�
  pos += pWork->oamlistpos + 1;

  if(pWork->pocketno == BAG_POKE_EVENT){
    int length = ITEMMENU_GetItemPocketNumber( pWork );
    ITEM_ST * item = ITEMMENU_GetItem( pWork, pos );
    SHORTCUT_ID shortcut_id = SHORTCUT_DATA_GetItemToShortcutID( item->id );

    if(length <= pos){
      return;
    }

    // �o�^�\���`�F�b�N
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
      GAMEDATA_SetShortCut( pWork->gamedata, shortcut_id, FALSE );
      ITEMMENU_RemoveCnvButtonItem(pWork,item->id);
    }
    else{
      GAMEDATA_SetShortCut( pWork->gamedata, shortcut_id, TRUE );
      ITEMMENU_AddCnvButtonItem(pWork,item->id);
    }

    PMSND_PlaySE( SE_BAG_REGIST_Y ); // �o�^��

    _windowRewrite(pWork);
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �|�P�b�g�ʒu����V���[�g�J�b�gID���擾
 *
 *  @param  int pocketno
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static SHORTCUT_ID SHORTCUT_GetPocket( int pocketno )
{
  return ITEMDISP_GetPocketShortcut( pocketno );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �|�P�b�g��Y�{�^���o�^
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval none
 */
//-----------------------------------------------------------------------------
static void SHORTCUT_SetPocket( FIELD_ITEMMENU_WORK* pWork )
{
  SHORTCUT_ID id;
  BOOL is_active;

  id = SHORTCUT_GetPocket( pWork->pocketno );
  is_active = GAMEDATA_GetShortCut( pWork->gamedata, id );
  is_active ^= 1; ///< ���]
  GAMEDATA_SetShortCut( pWork->gamedata, id, is_active );

  PMSND_PlaySE( SE_BAG_REGIST_Y );

  BTN_DrawCheckBox( pWork );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �`�F�b�N�{�b�N�X�`��
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void BTN_DrawCheckBox( FIELD_ITEMMENU_WORK* pWork )
{
  SHORTCUT_ID id;
  BOOL is_active;
  id = SHORTCUT_GetPocket( pWork->pocketno );
  is_active = GAMEDATA_GetShortCut( pWork->gamedata, id );

  GFL_CLACT_WK_SetAnmSeq( pWork->clwkBarIcon[ BAR_ICON_CHECK_BOX ], 6 + is_active );
}

#if 0
//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�G���[���b�Z�[�W�Z�b�g
 *
 * @param myst  �v���[���[�f�[�^
 * @param buf   ���b�Z�[�W�W�J�ꏊ
 * @param item  �A�C�e���ԍ�
 * @param err   �G���[ID
 * @param heap  �q�[�vID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BAG_ItemUseErrorMsgSet( MYSTATUS * myst, STRBUF * buf, u16 item, u32 err, FIELD_ITEMMENU_WORK * pWork )
{
  WORDSET * wset;
  STRBUF * str;

  switch( err ){
  case ITEMCHECK_ERR_CYCLE_OFF:   // ���]�Ԃ��~���Ȃ�
    GFL_MSG_GetString( pWork->MsgManager, msg_bag_058, buf );
    break;
  case ITEMCHECK_ERR_COMPANION:   // �g�p�s�E�A�����
    GFL_MSG_GetString( pWork->MsgManager, msg_bag_err_pair, buf );
    break;
  case ITEMCHECK_ERR_DISGUISE:    // �g�p�s�E���P�b�g�c�ϑ���
    GFL_MSG_GetString( pWork->MsgManager, msg_bag_err_disguise, buf );
    break;
  default:              // �g�p�s�E���m�̌��t
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
 * @param pWork   �o�b�O��ʃ��[�N
 * @param type  BAG_TALK_FULL=��ʈ�t, BAG_TALK_MINI=���2/3
 *
 * @return  ���b�Z�[�WID
 */
//--------------------------------------------------------------------------------------------
u8 Bag_TalkMsgPrint( BAG_WORK * pWork, int type )
{
  u8  idx;
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

  idx = GF_STR_PrintSimple( win, FONT_TALK,pWork->expb, 0, 0,
                            MSGSPEED_GetWait, BAG_TalkMsgCallBack );

  return idx;
}

//--------------------------------------------------------------------------------------------
/**
 * ���j���[�F����
 *
 * @param pWork   �o�b�O��ʂ̃��[�N
 *
 * @return  �ڍs���郁�C���V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int Bag_MenuUse( FIELD_ITEMMENU_WORK * pWork )
{
  ITEMCHECK_FUNC  check;
  s32 id;

  // �ǂ����g�p�����擾
  id    = ITEM_GetParam( pWork->ret_item, ITEM_PRM_FIELD, pWork->heapID );
  // �g���`�F�b�N������֐�
  check = (ITEMCHECK_FUNC)ITEMUSE_GetFunc( ITEMUSE_PRM_CHECKFUNC, id );

  // �ǂ����g�p�`�F�b�N
  if( check != NULL ){
    u32 ret = check( &pWork->icpWork );
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
 * @param pWork   �o�b�O��ʂ̃��[�N
 * @return  none
 */
//--------------------------------------------------------------------------------------------

#if 0

int (*MenuFuncTbl[])( FIELD_ITEMMENU_WORK *pWork)={
  //Bag_MenuUse,    // ����
  //Bag_MenuUse,    // �����
  //Bag_MenuUse,    // �݂�
  //Bag_MenuUse,    // ���߂�
  //Bag_MenuUse,    // �Ђ炭
  //Bag_MenuSub,    // ���Ă�
  //Bag_MenuCnvSet,   // �Ƃ��낭
  //Bag_MenuCnvDel,   // ��������
  //Bag_MenuSet,    // ��������
  //Bag_MenuTag,    // �^�O���݂�
  //Bag_MenuKettei,   // �����Ă�
  NULL,       // ��߂�
  //Bag_MenuItemMove, // ���ǂ�
  NULL,       // ����
  //Bag_MenuPlanterUse, // �����i�؂̎��v�����^�[�p�j
  //Bag_MenuUse,    // �Ƃ߂�
};

#endif

#if 1

//@TODO �s�v�H
//-----------------------------------------------------------------------------
/**
 *  @brief  ���̂݃v�����^�[����
 *
 *  @param  u16 pocket
 *  @param  item 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL BAGMAIN_NPlanterUseCheck( u16 pocket, u16 item )
{
  if( pocket == BAG_POKE_NUTS ||
      ( item >= COMPOST_START_ITEMNUM && item < (COMPOST_START_ITEMNUM+ITEM_COMPOST_MAX) ) ){
    return TRUE;
  }
  return FALSE;
}


//----------------------------------------------------------------------------------
/**
 * @brief �u�����v�u�����v�u�݂�v�u�Ђ炭�v�u���߂�v�u�Ƃ߂�v���ڒǉ�
 *
 * @param   pWork   
 * @param   itemdata    
 * @param   item    
 * @param   tbl     ���j���[���ڃe�[�u��
 */
//----------------------------------------------------------------------------------
static void _tsukau_check( FIELD_ITEMMENU_WORK *pWork, void *itemdata, ITEM_ST * item, u8 *tbl )
{
  if( ITEM_GetBufParam( itemdata,  ITEM_PRM_FIELD ) != 0 )
  {
    if( ITEM_CheckMail( item->id ) == TRUE )
    {
      tbl[BAG_MENU_USE] = BAG_MENU_MIRU;
    }
    else if( item->id == ITEM_ZITENSYA && pWork->cycle_flg == 1 )
    {
      tbl[BAG_MENU_USE] = BAG_MENU_ORIRU;
    }
    else
    {
      tbl[BAG_MENU_USE] = BAG_MENU_TSUKAU;
    }
  }

  // �_�[�N�X�g�[���E���C�g�X�g�[���͓���ȏ󋵂Ŏg�p�\�ɂȂ�
  if( (item->id == ITEM_RAITOSUTOON || item->id == ITEM_DAAKUSUTOON))
  {
    tbl[BAG_MENU_USE] = BAG_MENU_TSUKAU;
  }

}

//-----------------------------------------------------------------------------
/**
 *  @brief  �A�C�e���I�����̃��j���[���X�g�𐶐�
 *
 *  @param  FIELD_ITEMMENU_WORK * pWork
 *  @param  tbl 
 *
 *  @retval none
 */
//-----------------------------------------------------------------------------
static void ItemMenuMake( FIELD_ITEMMENU_WORK * pWork, u8* tbl )
{
  int  i;
  void * itemdata;
  u8  pocket;
  ITEM_ST * item = ITEMMENU_GetItem( pWork,ITEMMENU_GetItemIndex(pWork) );

  if((item==NULL) || (item->id==ITEM_DUMMY_DATA)){
    return;
  }

  itemdata = ITEM_GetItemArcData( item->id, ITEM_GET_DATA, pWork->heapID );
  pocket   = pWork->pocketno;

#if 0
  for(i=0;i<BAG_MENUTBL_MAX;i++){
    pWork->menu_func[i] = NULL;
  }
#endif

  // �|�P�������X�g�ł́u��������v�̂�
  if( pWork->mode == BAG_MODE_POKELIST ){
    // ��������
    if( ITEM_GetBufParam( itemdata, ITEM_PRM_EVENT ) == 0 ){
      // ���������Ȃ��̂͒e��
      if( ITEM_CheckPokeAdd( item->id ) == TRUE ){
        // ������������A�C�e��������
        tbl[BAG_MENU_GIVE] = BAG_MENU_MOTASERU; 
      }
    }
  }else{
    //�u�����v�Ȃ�
    _tsukau_check( pWork, itemdata, item, tbl );
    // ����̏ꏊ�ł́u�݂�v�̂�
    if( CheckEventItemUse( pWork, pocket ) == FALSE ){
      if( tbl[BAG_MENU_USE] != BAG_MENU_MIRU ){
        tbl[BAG_MENU_USE] = 255;
      }
    }

    // ��������
    // ���Ă�
    if( ITEM_GetBufParam( itemdata, ITEM_PRM_EVENT ) == 0 ){
      // ���������Ȃ��̂͒e��
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

  // ��߂�
  tbl[BAG_MENU_CANCEL] = BAG_MENU_YAMERU;

  OS_Printf("pocket=%d\n",   pocket);
  OS_Printf("tbl[BAG_MENU_USE]=%d\n",   tbl[BAG_MENU_USE]);
  OS_Printf("tbl[BAG_MENU_GIVE]=%d\n",  tbl[BAG_MENU_GIVE]);
  OS_Printf("tbl[BAG_MENU_SUB]=%d\n",   tbl[BAG_MENU_SUB]);

#if 0
  for(i=0;i<BAG_MENUTBL_MAX;i++){
    if(tbl[i]!=255){
      pWork->menu_func[i] = MenuFuncTbl[tbl[i]];
    }
    OS_Printf("menu_func[%d]=%08x\n", i,pWork->menu_func[i]);
  }
#endif

  GFL_HEAP_FreeMemory( itemdata );
}

#endif


//-----------------------------------------------------------------------------
/**
 *  @brief  �A�C�e���g�p�E�B���h�E�`��
 *
 *  @param  pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _itemUseWindowRewrite(FIELD_ITEMMENU_WORK* pWork)
{
  int strtbl[]= {
    msg_bag_001,   ///< ����
    msg_bag_007,   ///< �����
    msg_bag_017,   ///< �݂�
    msg_bag_002,   ///< ���Ă�
    msg_bag_003,   ///< �Ƃ��낭
    msg_bag_019,   ///< ��������
    msg_bag_004,   ///< ��������
    msg_bag_006,   ///< �����Ă�
    msg_bag_009,   ///< ��߂�
    mes_shop_103,  ///< ����
    msg_bag_001,   ///< ����
  };
  u8  tbl[BAG_MENUTBL_MAX] = { 255, 255, 255, 255 };
    
  int stringbuff[BAG_MENUTBL_MAX];
  int i, j;

  ItemMenuMake( pWork, tbl );

  for( i=0,j=0; i<BAG_MENUTBL_MAX; i++ ){
    if( tbl[i] == 255 ){ continue; }
    pWork->submenuList[j] = tbl[i];
    stringbuff[j] = strtbl[tbl[i]];
    j++;
  }
  ITEMDISP_MenuWinDisp( pWork, stringbuff, j );
}


//----------------------------------------------------------------------------
/**
 *  @brief  �֗��o�^����Ă����珇�Ԃ�Ԃ�
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
 *  @brief  �`�F�b�N��ǉ�
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
 *  @brief  �`�F�b�N������
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
 *  @brief  �{�^���C�x���g�R�[���o�b�N
 *
 *  @param  bttnid    �{�^��ID
 *  @param  event   �C�x���g���
 *  @param  p_work    ���[�N
 */
//-----------------------------------------------------------------------------
static void _BttnCallBack( u32 bttnid, u32 event, void* p_work )
{
  FIELD_ITEMMENU_WORK* pWork = p_work;
  int pocketno = -1;
  int length = ITEMMENU_GetItemPocketNumber(pWork);

  if(event!=GFL_BMN_EVENT_TOUCH){
    // �^�b�`�����u�ԂłȂ���Δ�����
    return;
  }

  // �o�b�O
  if(BAG_POKE_MAX > bttnid){
    int id2no[]={1,2,3,4,0};
    pocketno = id2no[bttnid];  //�ǂ����̂����肪�L���ׁA���Ԃ��Ō�ɂ��Ă���
    KTST_SetDraw( pWork, FALSE );
  // �|�P�b�g�؂�ւ����{�^��
  }else if(BUTTONID_LEFT == bttnid){
    pocketno = pWork->pocketno;
    pocketno--;
    if(pocketno < 0){
      pocketno = BAG_POKE_MAX-1;
    }
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkBarIcon[BAR_ICON_LEFT], APP_COMMON_BARICON_CURSOR_LEFT_ON );
  // �|�P�b�g�؂�ւ��E�{�^��
  }else if(BUTTONID_RIGHT == bttnid){
    pocketno = pWork->pocketno;
    pocketno++;
    if(pocketno >= BAG_POKE_MAX){
      pocketno = 0;
    }
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkBarIcon[BAR_ICON_RIGHT], APP_COMMON_BARICON_CURSOR_RIGHT_ON );
  // �\�[�g�{�^��
  }else if(BUTTONID_SORT == bttnid){
    // �Z�}�V���E�؂̎��͏����Ȃ�
    if( pWork->pocketno != BAG_POKE_WAZA && pWork->pocketno != BAG_POKE_NUTS ){
      SORT_Button( pWork );
      KTST_SetDraw( pWork, FALSE );
      _windowRewrite(pWork);
      _CHANGE_STATE( pWork, _sortMessageSet );
    }
  // �x�o�^�{�^��
  }else if(BUTTONID_CHECKBOX == bttnid){
    SHORTCUT_SetPocket( pWork );
    KTST_SetDraw( pWork, FALSE );
    _windowRewrite(pWork);
  // �I���{�^��
  }else if(BUTTONID_EXIT == bttnid){
    pWork->ret_code = BAG_NEXTPROC_EXIT;
    SetEndButtonAnime( pWork, 1, NULL );
    _CHANGE_STATE( pWork, _endButtonAnime );
  // �߂�{�^��
  }else if(BUTTONID_RETURN == bttnid){
    pWork->ret_code = BAG_NEXTPROC_RETURN;
    SetEndButtonAnime( pWork, 0, NULL );
    _CHANGE_STATE( pWork, _endButtonAnime );
  // ���X�g
  }else if((bttnid >= BUTTONID_ITEM_AREA) && (bttnid < BUTTONID_CHECK_AREA)){
    // �A�C�e���I���V�[�P���X�ł̑���
    if( pWork->state == _itemKindSelectMenu ){
      int backup = pWork->curpos;
      int nowno = ITEMMENU_GetItemIndex(pWork);

      // �A�C�e�����Ȃ������ꍇ
      if( ITEMMENU_GetItemPocketNumber( pWork ) == 0 )
      {
        // �����Ȃ�
        return;
      }

      if(pWork->curpos != (bttnid-BUTTONID_ITEM_AREA))
      {
        pWork->curpos = bttnid - BUTTONID_ITEM_AREA;
        if( length <= ITEMMENU_GetItemIndex(pWork) )
        {
          pWork->curpos = backup;
          return;
        }
      }

//    ITEMDISP_ScrollCursorChangePos( pWork );
      _ItemChange(pWork, nowno, ITEMMENU_GetItemIndex(pWork));

      // �^�b�`�ʒm
      KTST_SetDraw( pWork, FALSE ); 
      _windowRewrite(pWork);

      // ���ʕ\��
      ITEMDISP_upMessageRewrite(pWork);

//      _CHANGE_STATE(pWork,_itemSelectState);
      _CHANGE_STATE( pWork, _listSelectAnime );
    }
    return;
  // �`�F�b�N�{�b�N�X
  }else if(bttnid >= BUTTONID_CHECK_AREA){
    int no = bttnid - BUTTONID_CHECK_AREA;
    SHORTCUT_SetEventItem( pWork, no );
  }

  if(pocketno != -1){
    // �|�P�b�g�J�[�\���ړ�
    _pocketCursorChange(pWork, pWork->pocketno, pocketno);
    pWork->pocketno = pocketno;
    // �\�[�g�{�^���\���ؑ�
    SORT_ModeReset( pWork );

    KTST_SetDraw( pWork, FALSE );
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
static void _VBlank( GFL_TCB *tcb, void *work )
{
  FIELD_ITEMMENU_WORK* pWork = work;
  
  // ���ʁA�\����\���ؑ�
  if( pWork->bDispUpReq == 1 ){
    ITEMDISP_upMessageSetDispVBlank( pWork, TRUE );
    pWork->bDispUpReq = 0;
  }else if( pWork->bDispUpReq == 2 ){
    ITEMDISP_upMessageSetDispVBlank( pWork, FALSE );
    pWork->bDispUpReq = 0;
  }

  PaletteFadeTrans( pWork->pfd );

  GFL_CLACT_SYS_VBlankFunc(); //�Z���A�N�^�[VBlank

  // �Z�����X�g�X�V
  if(pWork->bChange)
  {
    ITEMDISP_CellVramTrans(pWork);
    pWork->bChange = FALSE;
  }

}


//-----------------------------------------------------------------------------
/**
 *  @brief  �A�C�e���I�����b�Z�[�W
 *
 *  @param  pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _startState(FIELD_ITEMMENU_WORK* pWork)
{
  BOOL  wipe, obj;

  wipe = WIPE_SYS_EndCheck();
  obj  = ITEMDISP_InitBagMode( pWork );

  if( wipe == TRUE && obj == FALSE ){
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->clwkScroll, TRUE );
//    _CHANGE_STATE(pWork, _itemKindSelectMenu);
    ChangeStateItemKindSelectItemMenu( pWork );
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
  BAG_PARAM* pParam = ppWork;
  FIELD_ITEMMENU_WORK* pWork;

//  GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0);

  // �q�[�v����
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_ITEMMENU, BAG_HEAPSIZE );
  pWork = GFL_PROC_AllocWork( proc, sizeof(FIELD_ITEMMENU_WORK), HEAPID_ITEMMENU );
  GFL_STD_MemClear( pWork, sizeof(FIELD_ITEMMENU_WORK) );

  //-------------------------------------
  // �����o������
  //-------------------------------------
  GFL_UI_KEY_GetRepeatSpeed( &pWork->key_repeat_speed, &pWork->key_repeat_wait );

  pWork->heapID = HEAPID_ITEMMENU;

  pWork->active = TRUE;

  // �p�����[�^����擾
  pWork->gamedata   = pParam->p_gamedata;
  pWork->mystatus   = pParam->p_mystatus;
  pWork->config     = pParam->p_config;
  pWork->pBagCursor = pParam->p_bagcursor;
  pWork->pMyItem    = pParam->p_myitem;
  pWork->icwk       = &pParam->icwk;
  pWork->mode       = pParam->mode;
  pWork->cycle_flg  = pParam->cycle_flg;

  // �A�C�e���j���f�[�^���C��
  MYITEM_CheckSafety( pWork->pMyItem );

  HOSAKA_Printf("bag mode =%d \n", pWork->mode );

// @TODO �ۍ�̂݃f�o�b�O�����Ȃ���N���ŃV���b�v����N���������Ƃɂ���
#ifdef DEBUG_ONLY_FOR_genya_hosaka
  if( (GFL_UI_KEY_GetCont() & PAD_BUTTON_DEBUG) )
  {
    pWork->mode = BAG_MODE_SELL;
    HOSAKA_Printf("hook sell\n");
  }
#endif

//  pWork->pBagCursor = GAMEDATA_GetBagCursor(GAMESYSTEM_GetGameData(pWork->gsys));
//  pWork->pMyItem = GAMEDATA_GetMyItem(GAMESYSTEM_GetGameData(pWork->gsys));

  // ���̑�
  pWork->pocketno = MYITEM_FieldBagPocketGet(pWork->pBagCursor);

  {
    s16 cur,scr;
    MYITEM_FieldBagCursorGet(pWork->pBagCursor, pWork->pocketno, &cur, &scr );
    pWork->curpos = cur;
    pWork->oamlistpos = scr - 1;
  }

  // �����񏉊���
  pWork->MsgManager = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
                                      NARC_message_bag_dat, pWork->heapID );

  pWork->MsgManagerItemInfo = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
                                              NARC_message_iteminfo_dat, pWork->heapID );

  pWork->MsgManagerPocket = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
                                            NARC_message_itempocket_dat, pWork->heapID );
  
  pWork->MsgManagerItemName = GFL_MSG_Create( GFL_MSG_LOAD_FAST, ARCID_MESSAGE,
                                            NARC_message_itemname_dat, pWork->heapID );


  pWork->pStrBuf = GFL_STR_CreateBuffer(200,pWork->heapID);
  pWork->pExpStrBuf = GFL_STR_CreateBuffer(200,pWork->heapID);
  pWork->pItemNameBuf = GFL_STR_CreateBuffer(64,pWork->heapID);
  pWork->WordSet    = WORDSET_Create( pWork->heapID );
  pWork->SysMsgQue  = PRINTSYS_QUE_Create( pWork->heapID );
  pWork->MsgCursorWork =  APP_KEYCURSOR_Create( 15, TRUE, TRUE, pWork->heapID );
  pWork->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr ,
                                       GFL_FONT_LOADTYPE_FILE , FALSE , pWork->heapID );
 
  // �O���t�B�b�N������
  ITEMDISP_graphicInit(pWork);

  ITEMDISP_upMessageCreate(pWork);
  ITEMDISP_CellResourceCreate(pWork);
  ITEMDISP_InitPocketCell( pWork );

  _windowCreate(pWork);
  ITEMDISP_CellCreate(pWork);

  pWork->pButton = GFL_BMN_Create( bttndata, _BttnCallBack, pWork,  pWork->heapID );

  InitBlinkPalAnm( pWork );
  InitPaletteAnime( pWork );

  // ���C�v�J�n
  WIPE_SYS_Start(
    WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
    WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, pWork->heapID );

  pWork->g3dVintr = GFUser_VIntr_CreateTCB( _VBlank, (void*)pWork, 0 );

  pWork->pMsgTcblSys = GFL_TCBL_Init( pWork->heapID , pWork->heapID , 1 , 0 );

  //  pWork->pTouchSWSys = TOUCH_SW_AllocWork(pWork->heapID);

  ITEMDISP_BarMessageCreate( pWork );

  pWork->pAppTaskRes = APP_TASKMENU_RES_Create( GFL_BG_FRAME3_M, _SUBLIST_NORMAL_PAL, pWork->fontHandle, pWork->SysMsgQue, pWork->heapID );

  // �l�b�g���[�N������
  GFL_NET_WirelessIconEasy_HoldLCD( TRUE, pWork->heapID );
  GFL_NET_ChangeIconPosition(256-16, 0);
  GFL_NET_ReloadIcon();
  
  // ������
  KTST_SetDraw( pWork ,GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY );
  
  ITEMDISP_SetVisible();

  PMSND_PlaySE( SE_BAG_SLIDE_IN );
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
  FIELD_ITEMMENU_WORK* pWork = mypWork;
  StateFunc* state = pWork->state;

  if(state == NULL)
  {
    // ���C�v �u���b�N�A�E�g�J�n
    WIPE_SYS_Start(
      WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
      WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, pWork->heapID );

    return GFL_PROC_RES_FINISH;
  }
/*
  else if( WIPE_SYS_EndCheck() != TRUE )
  {
    return GFL_PROC_RES_CONTINUE;
  }
*/

  state(pWork);
  _dispMain(pWork);
//  SORT_Draw(pWork);

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
  FIELD_ITEMMENU_WORK* pWork = mypWork;

  if (WIPE_SYS_EndCheck() != TRUE) {
    return GFL_PROC_RES_CONTINUE;
  }

  // GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0);
  GFL_TCB_DeleteTask( pWork->g3dVintr );

  MYITEM_FieldBagPocketSet(pWork->pBagCursor, pWork->pocketno);
  MYITEM_FieldBagCursorSet(pWork->pBagCursor, pWork->pocketno, pWork->curpos, pWork->oamlistpos+1 );

  APP_TASKMENU_RES_Delete( pWork->pAppTaskRes );

  ITEMDISP_upMessageDelete(pWork);
  ITEMDISP_graphicDelete(pWork);

  //  TOUCH_SW_FreeWork(pWork->pTouchSWSys);

  GFL_TCBL_Exit(pWork->pMsgTcblSys);// = GFL_TCBL_Init( work->heapId , work->heapId , 1 , 0 );


  GFL_MSG_Delete(pWork->MsgManager);
  GFL_MSG_Delete(pWork->MsgManagerItemInfo);
  GFL_MSG_Delete(pWork->MsgManagerPocket);
  GFL_MSG_Delete(pWork->MsgManagerItemName);

  GFL_STR_DeleteBuffer(pWork->pStrBuf);
  GFL_STR_DeleteBuffer(pWork->pExpStrBuf);
  GFL_STR_DeleteBuffer(pWork->pItemNameBuf);
  WORDSET_Delete(pWork->WordSet);

  ITEMDISP_BarMessageDelete( pWork );

  if(pWork->itemInfoDispWin){

    GFL_BG_FreeCharacterArea(GFL_BG_FRAME3_M,
                             GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar),
                             GFL_ARCUTIL_TRANSINFO_GetSize(pWork->bgchar));

    GFL_BMPWIN_Delete(pWork->itemInfoDispWin);
  }

  GFL_BG_FillCharacterRelease(GFL_BG_FRAME3_M,1,0);

  GFL_FONT_Delete(pWork->fontHandle);
  APP_KEYCURSOR_Delete(pWork->MsgCursorWork);
  PRINTSYS_QUE_Clear(pWork->SysMsgQue);
  PRINTSYS_QUE_Delete(pWork->SysMsgQue);
  ExitPaletteAnime( pWork );
  ExitBlinkPalAnm( pWork );
  GFL_BMN_Delete(pWork->pButton);
  //  ITEMDISP_ListPlateDelete(pWork);

  GFL_BMPWIN_Exit();
  GFL_BG_Exit();

  // �o�̓p�����[�^��ݒ�
  {
    BAG_PARAM* pParam = ppWork;

    pParam->next_proc = pWork->ret_code;
    pParam->ret_item  = pWork->ret_item;
  }

  GFL_UI_KEY_SetRepeatSpeed( pWork->key_repeat_speed, pWork->key_repeat_wait );

  GFL_PROC_FreeWork( proc );

  GFL_HEAP_DeleteHeap(  HEAPID_ITEMMENU );

  return GFL_PROC_RES_FINISH;

}



// �v���Z�X��`�f�[�^
const GFL_PROC_DATA ItemMenuProcData = {
  FieldItemMenuProc_Init,
  FieldItemMenuProc_Main,
  FieldItemMenuProc_End,
};





//--------------------------------------------------------------------------------------------
//  ���������璆���ǉ���
//--------------------------------------------------------------------------------------------
static void ChangeStateItemKindSelectItemMenu( FIELD_ITEMMENU_WORK * wk )
{
  _CHANGE_STATE( wk, _itemKindSelectMenu );
  GFL_UI_KEY_SetRepeatSpeed( 3, 6 );
}

static void ChangeStateItemMovePosition( FIELD_ITEMMENU_WORK * wk )
{
  _CHANGE_STATE( wk, _itemMovePosition );
  GFL_UI_KEY_SetRepeatSpeed( 3, 6 );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   ���X�g�J�[�\���_�ŃA�j��
 */
//--------------------------------------------------------------------------------------------
static void InitBlinkPalAnm( FIELD_ITEMMENU_WORK * wk )
{
  ARCHANDLE * ah;

  wk->blwk = BLINKPALANM_Create( (_PAL_WIN01_CELL+1)*16, 16, BLINKPALANM_MODE_MAIN_OBJ, wk->heapID );

  ah = GFL_ARC_OpenDataHandle( ARCID_BAG, wk->heapID );
  BLINKPALANM_SetPalBufferArcHDL( wk->blwk, ah, NARC_bag_bag_win01_d_NCLR, 16, 32 );
  GFL_ARC_CloseDataHandle( ah );
}

static void ExitBlinkPalAnm( FIELD_ITEMMENU_WORK * wk )
{
  BLINKPALANM_Exit( wk->blwk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ���X�g�I���A�j��
 */
//--------------------------------------------------------------------------------------------
static void _listSelectAnime( FIELD_ITEMMENU_WORK * wk )
{
  switch( wk->tmpSeq ){
  case 0:
    PMSND_PlaySE( SE_BAG_DECIDE );
  case 3:
    ITEMDISP_ChangeCursorPosPalette( wk, 0 );
    wk->tmpSeq++;
    break;

  case 1:
  case 4:
  case 6:
    wk->tmpCnt++;
    if( wk->tmpCnt == 4 ){
      wk->tmpCnt = 0;
      wk->tmpSeq++;
    }
    break;

  case 2:
  case 5:
    ITEMDISP_ChangeCursorPosPalette( wk, 2 );
    wk->tmpSeq++;
    break;

  case 7:
    wk->tmpSeq = 0;
    // ���j���[�I�������l���ď��������Ă���
    ITEMDISP_ChangeCursorPosPalette( wk, 1 );
    BLINKPALANM_InitAnimeCount( wk->blwk );
    BLINKPALANM_Main( wk->blwk );
    _CHANGE_STATE( wk, _itemSelectState );
    break;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �^�b�`�o�[�{�^���A�j��
 */
//--------------------------------------------------------------------------------------------
static void SetEndButtonAnime( FIELD_ITEMMENU_WORK * wk, u8 type, StateFunc * next )
{
  u32 idx;
  u32 anm;

  if( type == 0 ){
    PMSND_PlaySE( SE_BAG_CANCEL );
    idx = BAR_ICON_RETURN;
    anm = APP_COMMON_BARICON_RETURN_ON;
  }else{
    PMSND_PlaySE( SE_BAG_CLOSE );
    idx = BAR_ICON_EXIT;
    anm = APP_COMMON_BARICON_EXIT_ON;
  }
  GFL_CLACT_WK_SetAnmFrame( wk->clwkBarIcon[idx], 0 );
  GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[idx], anm );
  GFL_CLACT_WK_SetAutoAnmFlag( wk->clwkBarIcon[idx], TRUE );
  wk->tmpSeq = type;
  wk->chgState = next;
}

static void _endButtonAnime( FIELD_ITEMMENU_WORK * wk )
{
  u32 idx;

  if( wk->tmpSeq == 0 ){
    idx = BAR_ICON_RETURN;
  }else{
    idx = BAR_ICON_EXIT;
  }

  if( GFL_CLACT_WK_CheckAnmActive( wk->clwkBarIcon[idx] ) == FALSE ){
    _CHANGE_STATE( wk, wk->chgState );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �̂Ă�E����̃^�b�`�`�F�b�N
 */
//--------------------------------------------------------------------------------------------

static int CheckNumSelTouch(void)
{
  static const GFL_UI_TP_HITTBL tbl[] =
  {
    { _WINNUM_SCR_INITY*8, _WINNUM_SCR_TP_END_Y*8-1, _WINNUM_SCR_INITX*8, _WINNUM_SCR_TP_END_X*8-1 },
    { TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, _BAR_CELL_CURSOR_RETURN, _BAR_CELL_CURSOR_RETURN+TOUCHBAR_ICON_WIDTH-1 },  //���^�[��
    { GFL_UI_TP_HIT_END, 0, 0, 0 },
  };
  int ret = GFL_UI_TP_HitTrg( tbl );

  if( ret != GFL_UI_TP_HIT_NONE ){
    GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
  }
  return ret;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   �̂Ă�L�����Z������
 */
//--------------------------------------------------------------------------------------------

static void _itemTrashCancel( FIELD_ITEMMENU_WORK * wk )
{
  InputNum_Exit( wk );

//  InputNum_ButtonState( wk, TRUE );

  GFL_CLACT_WK_SetAutoAnmFlag( wk->clwkScroll, TRUE );
  ITEMDISP_ChangeActive( wk, TRUE );
  KTST_SetDraw( wk, GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY );
//  _CHANGE_STATE( wk, _itemKindSelectMenu );
  ChangeStateItemKindSelectItemMenu( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ����L�����Z������
 */
//--------------------------------------------------------------------------------------------

static void _itemSellInputCancel( FIELD_ITEMMENU_WORK * wk )
{
  InputNum_Exit( wk );

  GFL_CLACT_WK_SetAutoAnmFlag( wk->clwkScroll, TRUE );
  ITEMDISP_ChangeActive( wk, TRUE );
  KTST_SetDraw( wk, GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY );

  _CHANGE_STATE( wk, _itemSellExit );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   �p���b�g�A�j��
 */
//--------------------------------------------------------------------------------------------

static void InitPaletteAnime( FIELD_ITEMMENU_WORK * wk )
{
  wk->pfd = PaletteFadeInit( wk->heapID );
  PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_OBJ, FADE_PAL_ALL_SIZE, wk->heapID );
  PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_BG, FADE_PAL_ONE_SIZE, wk->heapID );
  PaletteWorkSet_VramCopy( wk->pfd, FADE_MAIN_BG, 0, FADE_PAL_ONE_SIZE );
}

static void ExitPaletteAnime( FIELD_ITEMMENU_WORK * wk )
{
  PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_BG );
  PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_OBJ );
  PaletteFadeFree( wk->pfd );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �\�[�g���b�Z�[�W�\��
 */
//--------------------------------------------------------------------------------------------
static void _sortMessageSet( FIELD_ITEMMENU_WORK * wk )
{
  if( GFL_CLACT_WK_CheckAnmActive( wk->clwkSort ) == FALSE ){
    GFL_CLACT_WK_SetAnmSeq( wk->clwkSort, 0 );
    SORT_Main( wk );
    GFL_CLACT_WK_SetAutoAnmFlag( wk->clwkScroll, FALSE );
    GFL_MSG_GetString( wk->MsgManager, mes_bag_113, wk->pExpStrBuf );
    ITEMDISP_ItemInfoWindowDispEx( wk, FALSE );
    ITEMDISP_ChangeRetButtonActive( wk, FALSE );
    ITEMDISP_ChangeActive( wk, FALSE );
    _CHANGE_STATE( wk, _sortMessageWait );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �\�[�g���b�Z�[�W�E�F�C�g
 */
//--------------------------------------------------------------------------------------------
static void _sortMessageWait( FIELD_ITEMMENU_WORK * wk )
{
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
    KTST_SetDraw( wk, TRUE );
  }else if( GFL_UI_TP_GetTrg() ){
    KTST_SetDraw( wk, FALSE );
  }else{
    return;
  }

  _windowRewrite( wk );
  ITEMDISP_ListPlateClear( wk );
  GFL_CLACT_WK_SetAutoAnmFlag( wk->clwkScroll, TRUE );
  ITEMDISP_ChangeActive( wk, TRUE );
//  _CHANGE_STATE( wk, _itemKindSelectMenu );
  ChangeStateItemKindSelectItemMenu( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �A�C�e���g�p�ꏊ�`�F�b�N
 */
//--------------------------------------------------------------------------------------------
static BOOL CheckEventItemUse( FIELD_ITEMMENU_WORK * wk, u8 pocket )
{
  // �R���V�A���E���j�I�����[��
  if( wk->mode == BAG_MODE_COLOSSEUM || wk->mode == BAG_MODE_UNION ){
    return FALSE;
  }
  // ���t�B�[���h�Łu�������Ȃ��́v�|�P�b�g
  if( GAMEDATA_GetIntrudeReverseArea( wk->gamedata ) == TRUE && pocket == BAG_POKE_EVENT ){
    return FALSE;
  }
  return TRUE;
}
