//======================================================================
/**
 * @file  bmp_menu.c
 * @brief BMP���j���[����
 * @author  Hiroyuki Nakamura
 * @date  2004.11.10
 *
 *  �E���j���[���X�N���[������Ȃ�BMPLIST���g�p���邱��
 *  �E�c�A���̃��j���[�ɑΉ�
 *  �E2008.09 DP����WB�ֈڐA
 */
//======================================================================
#include <gflib.h>

#include "include/system/gfl_use.h"
#include "arc/arc_def.h"
#include "system/bmp_winframe.h"
#include "system/bmp_cursor.h"
#include "system/bmp_menu.h"
#include "system/bmp_menu.h"

#include "message.naix"
#include "msg/msg_yesnomenu.h"
#include "font/font.naix"
#include "sound/pm_sndsys.h"

//======================================================================
//  �V���{����`
//======================================================================
enum {
  MV_UP = 0,
  MV_DOWN,
  MV_LEFT,
  MV_RIGHT
};

struct _BMPMENU_WORK {
  BMPMENU_HEADER  hed;
  BMPCURSOR *cursor;

  u32 cancel;

  u8  index;
  u8  cur_pos;
  u8  len;
  u8  px;

  u8  py;
  u8  sx;
  u8  sy;
  u8  mv;

  HEAPID  heap_id;  // �������擾���[�h
  u8  dmy[2];   // �r�b�g�]��
};

//======================================================================
//  �v���g�^�C�v�錾
//======================================================================
#if 0
static void BmpMenuWinWriteMain(
  GF_BGL_INI * ini, u8 frm, u8 px, u8 py, u8 sx, u8 sy, u8 pal, u16 cgx );
static void BmpTalkWinWriteMain(
  GF_BGL_INI * ini, u8 frm, u8 px, u8 py, u8 sx, u8 sy, u8 pal, u16 cgx );
#endif

static BOOL BmpMenuCursorMove( BMPMENU_WORK * mw, u8 mv, u16 se );
static u8 BmpMenuCursorMoveCheck( BMPMENU_WORK * mw, u8 mv );
static u8 BmpMenuStrLen( BMPMENU_WORK * buf );
static void BmpMenuStrPut( BMPMENU_WORK * mw );
static void BmpMenuCursorPut( BMPMENU_WORK * mw );
static void CursorWritePosGet( BMPMENU_WORK * mw, u8 * x, u8 * y, u8 pos );

#if 0
static void BmpWinOn( GFL_BMPWIN *bmpwin );
#endif

//======================================================================
//  BMP���j���[
//======================================================================
//--------------------------------------------------------------
/**
 * BMP���j���[�o�^�i�X�N���[���]�����Ȃ��j
 * @param dat     �w�b�_�f�[�^
 * @param px      ���ڕ\��X���W
 * @param py      ���ڕ\��Y���W
 * @param pos     �����J�[�\���ʒu
 * @aram  heap_id   �������擾���[�h
 * @param cancel    �L�����Z���{�^��
 * @return  BMP���j���[���[�N
 * @li  BMP���X�g���[�N��sys_AllocMemory�Ŋm��
 */
//--------------------------------------------------------------
BMPMENU_WORK * BmpMenu_AddNoTrans( const BMPMENU_HEADER *dat,
                                   u8 px, u8 py, u8 pos, HEAPID heap_id, u32 cancel )
{
  BMPMENU_WORK *mw;

  mw = (BMPMENU_WORK *)GFL_HEAP_AllocClearMemory(
    heap_id, sizeof(BMPMENU_WORK) );

  mw->hed     = *dat;
  mw->cursor  = BmpCursor_Create( heap_id );
  mw->cancel  = cancel;
  mw->cur_pos = pos;
  mw->len     = BmpMenuStrLen( mw );
  mw->heap_id = heap_id;

  mw->px = px;
  mw->py = py;

  BmpCursor_SetCursorBitmap( mw->cursor,  heap_id );

#if 0
  mw->sx = FontHeaderGet( dat->font, FONT_HEADER_SIZE_X )
    + FontHeaderGet( dat->font, FONT_HEADER_SPACE_X );
  mw->sy = FontHeaderGet( dat->font, FONT_HEADER_SIZE_Y )
    + FontHeaderGet( dat->font, FONT_HEADER_SPACE_Y );
#else
  mw->sx = mw->hed.font_size_x;
  mw->sy = mw->hed.font_size_y;
#endif

  BmpMenuStrPut( mw );
  BmpMenuCursorPut( mw );

  return mw;
}

//--------------------------------------------------------------
/**
 * BMP���j���[�o�^�i�L�����Z���{�^���w��j
 *
 * @param dat     �w�b�_�f�[�^
 * @param px      ���ڕ\��X���W
 * @param py      ���ڕ\��Y���W
 * @param pos     �����J�[�\���ʒu
 * @aram  mode    �������擾���[�h
 * @param cancel    �L�����Z���{�^��
 *
 * @return  BMP���j���[���[�N
 *
 * @li  BMP���X�g���[�N��sys_AllocMemory�Ŋm��
 */
//--------------------------------------------------------------
BMPMENU_WORK * BmpMenu_AddEx( const BMPMENU_HEADER * dat,
                              u8 px, u8 py, u8 pos, HEAPID heap_id, u32 cancel )
{
  BMPMENU_WORK *mw;
  mw = BmpMenu_AddNoTrans( dat, px, py, pos, heap_id, cancel );
  //  BmpWinOn( mw->hed.win );
  return mw;
}

//--------------------------------------------------------------
/**
 * BMP���j���[�o�^�i�ȈՔŁj
 *
 * @param dat     �w�b�_�f�[�^
 * @param pos     �����J�[�\���ʒu
 * @aram  mode    �������擾���[�h
 *
 * @return  BMP���j���[���[�N
 *
 * @li  BMP���X�g���[�N��sys_AllocMemory�Ŋm��
 * @li  B�{�^���L�����Z��
 */
//--------------------------------------------------------------
BMPMENU_WORK * BmpMenu_Add( const BMPMENU_HEADER * dat, u8 pos, HEAPID heap_id )
{
#if 0
  return BmpMenu_AddEx(
    dat, FontHeaderGet(dat->font,FONT_HEADER_SIZE_X), 0,
    pos, mode, PAD_BUTTON_CANCEL );
#else
  return BmpMenu_AddEx(
    dat, dat->font_size_x, 0, pos, heap_id, PAD_BUTTON_CANCEL );
#endif
}

//--------------------------------------------------------------
/**
 * BMP���j���[�j��
 *
 * @param mw    BMP���j���[���[�N
 * @param backup  �J�[�\���ʒu�ۑ��ꏊ
 *
 * @return  none
 */
//--------------------------------------------------------------
void BmpMenu_Exit( BMPMENU_WORK *mw, u8 * backup )
{
  if( backup != NULL ){
    *backup = mw->cur_pos;
  }

  BmpCursor_Delete( mw->cursor );
  GFL_HEAP_FreeMemory( mw );
}

//--------------------------------------------------------------
/**
 * ���j���[����֐�
 *
 * @param mw    BMP���j���[���[�N
 *
 * @return  ���쌋��
 */
//--------------------------------------------------------------
u32 BmpMenu_Main( BMPMENU_WORK * mw )
{
  int trg = GFL_UI_KEY_GetTrg();
  mw->mv = BMPMENU_MOVE_NONE;

  if( trg & PAD_BUTTON_DECIDE ){
    PMSND_PlaySystemSE( SEQ_SE_DECIDE1 );
    return mw->hed.menu[ mw->cur_pos ].param;
  }

  if( trg & mw->cancel ){
    PMSND_PlaySystemSE( SEQ_SE_CANCEL1 );
    return BMPMENU_CANCEL;
  }
  if( trg & PAD_KEY_UP ){
    if( BmpMenuCursorMove( mw, MV_UP, SEQ_SE_SELECT1 ) == TRUE ){
      mw->mv = BMPMENU_MOVE_UP;
    }
    return BMPMENU_NULL;
  }
  if( trg & PAD_KEY_DOWN ){
    if( BmpMenuCursorMove( mw, MV_DOWN, SEQ_SE_SELECT1 ) == TRUE ){
      mw->mv = BMPMENU_MOVE_DOWN;
    }
    return BMPMENU_NULL;
  }
  if( trg & PAD_KEY_LEFT ){
    if( BmpMenuCursorMove( mw, MV_LEFT, SEQ_SE_SELECT1 ) == TRUE ){
      mw->mv = BMPMENU_MOVE_LEFT;
    }
    return BMPMENU_NULL;
  }
  if( trg & PAD_KEY_RIGHT ){
    if( BmpMenuCursorMove( mw, MV_RIGHT, SEQ_SE_SELECT1 ) == TRUE ){
      mw->mv = BMPMENU_MOVE_RIGHT;
    }
    return BMPMENU_NULL;
  }

  return BMPMENU_NULL;
}

//--------------------------------------------------------------
/**
 * ���j���[����֐��i�\���L�[��SE���w��ł���j
 *
 * @param mw    BMP���j���[���[�N
 * @param key_se  �\���L�[��SE
 *
 * @return  ���쌋��
 */
//--------------------------------------------------------------
u32 BmpMenu_MainSE( BMPMENU_WORK * mw, u16 key_se )
{
  int trg = GFL_UI_KEY_GetTrg();
  mw->mv = BMPMENU_MOVE_NONE;

  if( trg & PAD_BUTTON_DECIDE ){
    PMSND_PlaySystemSE( SEQ_SE_DECIDE1 );
    return mw->hed.menu[ mw->cur_pos ].param;
  }
  if( trg & mw->cancel ){
    PMSND_PlaySystemSE( SEQ_SE_CANCEL1 );
    return BMPMENU_CANCEL;
  }
  if( trg & PAD_KEY_UP ){
    if( BmpMenuCursorMove( mw, MV_UP, key_se ) == TRUE ){
      mw->mv = BMPMENU_MOVE_UP;
    }
    return BMPMENU_NULL;
  }
  if( trg & PAD_KEY_DOWN ){
    if( BmpMenuCursorMove( mw, MV_DOWN, key_se ) == TRUE ){
      mw->mv = BMPMENU_MOVE_DOWN;
    }
    return BMPMENU_NULL;
  }
  if( trg & PAD_KEY_LEFT ){
    if( BmpMenuCursorMove( mw, MV_LEFT, key_se ) == TRUE ){
      mw->mv = BMPMENU_MOVE_LEFT;
    }
    return BMPMENU_NULL;
  }
  if( trg & PAD_KEY_RIGHT ){
    if( BmpMenuCursorMove( mw, MV_RIGHT, key_se ) == TRUE ){
      mw->mv = BMPMENU_MOVE_RIGHT;
    }
    return BMPMENU_NULL;
  }

  return BMPMENU_NULL;
}

//--------------------------------------------------------------
/**
 * ���j���[�O���R���g���[��
 *
 * @param mw    BMP���j���[���[�N
 * @param prm   �R���g���[���p�����[�^
 *
 * @return  ���쌋��
 */
//--------------------------------------------------------------
u32 BmpMenu_MainOutControl( BMPMENU_WORK * mw, u8 prm )
{
  switch( prm ){
  case BMPMENU_CNTROL_DECIDE:   // ����
    PMSND_PlaySystemSE( SEQ_SE_DECIDE1 );
    return mw->hed.menu[ mw->cur_pos ].param;

  case BMPMENU_CNTROL_CANCEL:   // �L�����Z��
    PMSND_PlaySystemSE( SEQ_SE_CANCEL1 );
    return BMPMENU_CANCEL;

  case BMPMENU_CNTROL_UP:     // ��
    BmpMenuCursorMove( mw, MV_UP, SEQ_SE_SELECT1 );
    return BMPMENU_NULL;

  case BMPMENU_CNTROL_DOWN:   // ��
    BmpMenuCursorMove( mw, MV_DOWN, SEQ_SE_SELECT1 );
    return BMPMENU_NULL;

  case BMPMENU_CNTROL_LEFT:   // ��
    BmpMenuCursorMove( mw, MV_LEFT, SEQ_SE_SELECT1 );
    return BMPMENU_NULL;

  case BMPMENU_CNTROL_RIGHT:    // �E
    BmpMenuCursorMove( mw, MV_RIGHT, SEQ_SE_SELECT1 );
    return BMPMENU_NULL;
  }

  return BMPMENU_NULL;
}

//--------------------------------------------------------------
/**
 * �J�[�\���ʒu�擾
 *
 * @param mw    BMP���j���[���[�N
 *
 * @return  �J�[�\���ʒu
 */
//--------------------------------------------------------------
u8 BmpMenu_CursorPosGet( BMPMENU_WORK * mw )
{
  return mw->cur_pos;
}

//--------------------------------------------------------------------------------------------
/**
 * �ړ������擾
 *
 * @param mw    BMP���j���[���[�N
 *
 * @return  �ړ������擾
 */
//--------------------------------------------------------------------------------------------
u8 BmpMenu_MoveSiteGet( BMPMENU_WORK * mw )
{
  return mw->mv;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ�
 *
 * @param mw    BMP���j���[���[�N
 * @param mv    �ړ�����
 * @param se    SE
 *
 * @retval  "TRUE = �ړ�����"
 * @retval  "FALSE = �ړ����ĂȂ�"
 */
//--------------------------------------------------------------------------------------------
static BOOL BmpMenuCursorMove( BMPMENU_WORK * mw, u8 mv, u16 se )
{
  u8  old = mw->cur_pos;

  if( BmpMenuCursorMoveCheck( mw, mv ) == FALSE ){
    return FALSE;
  }

  {
    u8  px, py;
    u8  col;
#if 0 //old dp
    col = FontHeaderGet( mw->hed.font, FONT_HEADER_B_COLOR );
#else
    {
      u8 l,s;
      GFL_FONTSYS_GetColor( &l, &s, &col );
    }
#endif
    CursorWritePosGet( mw, &px, &py, old );

#if 0 //old dp
    GF_BGL_BmpWinFill( mw->hed.win, col, px, py, 8, mw->sy );
#else
    //    GFL_BMP_Fill(
    //      GFL_BMPWIN_GetBmp(mw->hed.win), px, py, mw->sx, mw->sy, col );
    GFL_BMP_Fill(
      GFL_BMPWIN_GetBmp(mw->hed.win), px, py, mw->sx, mw->sy, 0xff );
#endif
  }

  BmpMenuCursorPut( mw );
  PMSND_PlaySystemSE( se );
  return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ��`�F�b�N
 *
 * @param mw    BMP���j���[���[�N
 * @param mv    �ړ�����
 *
 * @retval  "TRUE = �ړ�����"
 * @retval  "FALSE = �ړ����ĂȂ�"
 */
//--------------------------------------------------------------------------------------------
static u8 BmpMenuCursorMoveCheck( BMPMENU_WORK * mw, u8 mv )
{
  s8  new_pos;

  if( mv == MV_UP ){
    if( mw->hed.y_max <= 1 ){ return FALSE; }
    if( ( mw->cur_pos % mw->hed.y_max ) == 0 ){
      if( mw->hed.loop_f == 0 ){ return FALSE; }
      //      mw->cur_pos += ( mw->hed.y_max - 1 );
      new_pos = mw->cur_pos + ( mw->hed.y_max - 1 );
    }else{
      //      mw->cur_pos -= 1;
      //      return TRUE;
      new_pos = mw->cur_pos - 1;
    }
  }else if( mv == MV_DOWN ){
    if( mw->hed.y_max <= 1 ){ return FALSE; }
    if( ( mw->cur_pos % mw->hed.y_max ) == ( mw->hed.y_max - 1 ) ){
      if( mw->hed.loop_f == 0 ){ return FALSE; }
      //      mw->cur_pos -= ( mw->hed.y_max - 1 );
      new_pos = mw->cur_pos - ( mw->hed.y_max - 1 );
    }else{
      //      mw->cur_pos += 1;
      //      return TRUE;
      new_pos = mw->cur_pos + 1;
    }
  }else if( mv == MV_LEFT ){
    if( mw->hed.x_max <= 1 ){ return FALSE; }
    if( mw->cur_pos < mw->hed.y_max ){
      if( mw->hed.loop_f == 0 ){ return FALSE; }
      //      mw->cur_pos += ( mw->hed.y_max * ( mw->hed.x_max - 1 ) );
      new_pos = mw->cur_pos + ( mw->hed.y_max * ( mw->hed.x_max - 1 ) );
    }else{
      //      mw->cur_pos -= mw->hed.y_max;
      //      return TRUE;
      new_pos = mw->cur_pos - mw->hed.y_max;
    }
  }else{
    if( mw->hed.x_max <= 1 ){ return FALSE; }
    if( mw->cur_pos >= ( mw->hed.y_max * ( mw->hed.x_max - 1 ) ) ){
      if( mw->hed.loop_f == 0 ){ return FALSE; }
      //      mw->cur_pos %= mw->hed.y_max;
      new_pos = mw->cur_pos % mw->hed.y_max;
    }else{
      //      mw->cur_pos += mw->hed.y_max;
      //      return TRUE;
      new_pos = mw->cur_pos + mw->hed.y_max;
    }
  }

  if( mw->hed.menu[ new_pos ].param == BMPMENU_DUMMY ){
    return FALSE;
  }
  mw->cur_pos = new_pos;
  return TRUE;
}


//--------------------------------------------------------------------------------------------
/**
 * �Œ����ڂ̕��������擾
 *
 * @param buf   BMP���j���[���[�N
 *
 * @return  �Œ����ڂ̕�����
 */
//--------------------------------------------------------------------------------------------
static u8 BmpMenuStrLen( BMPMENU_WORK * buf )
{
  u8  len = 0;
  u8  i, j;

  for( i=0; i<buf->hed.x_max*buf->hed.y_max; i++ ){
#if 0 //old_dp
    j = FontProc_GetPrintStrWidth(
      buf->hed.font, buf->hed.menu[i].str, 0 );
#else
    j = PRINTSYS_GetStrWidth(
      buf->hed.menu[i].str, buf->hed.font_handle, 0 );
#endif
    if( len < j ){ len = j; }
  }

  return len;
}

//--------------------------------------------------------------------------------------------
/**
 * ���j���[���ڕ\��
 *
 * @param mw    BMP���j���[���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BmpMenuStrPut( BMPMENU_WORK * mw )
{
  const void* str;
  u8  px, py, plus;
  u8  i, j;

#if 0 //old dp
  GF_BGL_BmpWinDataFill(
    mw->hed.win, FontHeaderGet( mw->hed.font, FONT_HEADER_B_COLOR ) );

  px = mw->px;
  //  plus = px * ( mw->len + 2 );
  plus = mw->len + mw->sx * 2;
  for( i=0; i<mw->hed.x_max; i++ ){
    for( j=0; j<mw->hed.y_max; j++ ){
      str = mw->hed.menu[i*mw->hed.y_max+j].str;
      py  = ( mw->sy + mw->hed.line_spc ) * j + mw->py;
      GF_STR_PrintSimple( mw->hed.win, mw->hed.font, str, px, py, MSG_NO_PUT, NULL );
    }
    px += plus;
  }
#else
  {
    //    u8 l,s,b;
    //    Gfl_FONTSYS_GetColor( &l, &s, &b );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(mw->hed.win), 0xff );
  }

  px = mw->px;
  //  plus = px * ( mw->len + 2 );
  plus = mw->len + mw->sx * 2;

  for( i=0; i<mw->hed.x_max; i++ ){
    for( j=0; j<mw->hed.y_max; j++ ){
      str = mw->hed.menu[i*mw->hed.y_max+j].str;
      py  = ( mw->sy + mw->hed.line_spc ) * j + mw->py;
      PRINT_UTIL_Print(
        mw->hed.print_util, mw->hed.print_que,
        px, py, str, mw->hed.font_handle );
    }
    px += plus;
  }
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���\��
 *
 * @param mw    BMP���j���[���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BmpMenuCursorPut( BMPMENU_WORK * mw )
{
  u8  px, py;

  if( mw->hed.c_disp_f == 1 ){ return; }

  CursorWritePosGet( mw, &px, &py, mw->cur_pos );
  BmpCursor_Print( mw->cursor, px, py,
                   mw->hed.print_util, mw->hed.print_que, mw->hed.font_handle );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���\���ʒu�擾
 *
 * @param buf   BMP���j���[�f�[�^
 * @param x   �\��X���W�擾�ꏊ
 * @param y   �\��Y���W�擾�ꏊ
 * @param pos   �J�[�\���ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void CursorWritePosGet( BMPMENU_WORK * mw, u8 * x, u8 * y, u8 pos )
{
  //  *x = ( pos / mw->hed.y_max ) * ( mw->len + 2 ) * mw->sx;
  *x = ( pos / mw->hed.y_max ) * ( mw->len + mw->sx * 2 );
  *y = ( pos % mw->hed.y_max ) * ( mw->sy + mw->hed.line_spc ) + mw->py;
}

#if 1  //YESNOWINDOW
//============================================================================================
//  �͂��E����������
//============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * �͂��E�������E�B���h�E�Z�b�g�i�J�[�\���ʒu�w��j
 *
 * @param data  �E�B���h�E�f�[�^
 * @param cgx   �E�B���h�E�L�����ʒu
 * @param pal   �E�B���h�E�p���b�g�ԍ�
 * @param pos   �����J�[�\���ʒu
 * @param heap  �q�[�vID
 *
 * @return  BMP���j���[���[�N
 *
 * @li  BMP�E�B���h�E��BMP���j���[���[�N��Alloc�Ŏ擾���Ă���
 */
//--------------------------------------------------------------------------------------------
BMPMENU_WORK * BmpMenu_YesNoSelectInit( const BMPWIN_YESNO_DAT *data, u16 cgx, u8 pal, u8 pos, HEAPID heap )
{
  BMPMENU_HEADER hed;
  GFL_MSGDATA * man;
  BMP_MENULIST_DATA * ld;
  BMPMENU_WORK* pWk;


  man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE , NARC_message_yesnomenu_dat, heap );
  ld  = BmpMenuWork_ListCreate( 2, heap );
  BmpMenuWork_ListAddArchiveString( ld, man, msgid_yesno_yes, 0,heap );
  BmpMenuWork_ListAddArchiveString( ld, man, msgid_yesno_no, BMPMENU_CANCEL,heap );
  GFL_MSG_Delete( man );

  GFL_STD_MemClear(&hed,sizeof(BMPMENU_HEADER));

  hed.menu     = ld;
  hed.win      = GFL_BMPWIN_Create( data->frmnum , data->pos_x, data->pos_y, 7, 4, data->palnum, GFL_BMP_CHRAREA_GET_B );
  hed.x_max    = 1;
  hed.y_max    = 2;
  hed.line_spc = 1;
  hed.c_disp_f = 0;
  hed.loop_f = 0;
  hed.print_util = GFL_HEAP_AllocClearMemory(heap,sizeof(PRINT_UTIL));

  hed.font_handle = GFL_FONT_Create(ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, heap );
  hed.font_size_y = GFL_FONT_GetLineHeight(hed.font_handle);
  hed.font_size_x = hed.font_size_y;
  PRINT_UTIL_Setup(hed.print_util, hed.win);
  hed.print_que = PRINTSYS_QUE_Create( heap );
  
  hed.b_trans   = FALSE;

//  GFL_MSGDATA *msgdata; //�\���Ɏg�p���郁�b�Z�[�W�o�b�t�@

  //  GFL_BG_BmpWinAddEx( ini, hed.win, data );
  //BmpMenuWinWrite( hed.win, WINDOW_TRANS_OFF, cgx, pal );
  pWk = BmpMenu_AddEx( &hed, 14, 0, pos, heap, PAD_BUTTON_CANCEL );
  GFL_BMPWIN_MakeScreen(hed.win);
  BmpWinFrame_Write( hed.win, WINDOW_TRANS_ON, cgx, pal );

  GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(hed.win) );
  GFL_BMPWIN_TransVramCharacter( hed.win );
  return pWk;

}


#if 0
static BMPMENU_WORK * BmpMenu_YesNoSelectInitCore
  ( GFL_BMPWIN* bmpwin, u16 cgx, u8 pal, u8 pos, HEAPID heap )
{
  BMPMENU_HEADER hed;
  GFL_MSGDATA * man;
  BMP_MENULIST_DATA * ld;
  BMPMENU_WORK* pWk;

  man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE , NARC_message_yesnomenu_dat, heap );
  ld  = BmpMenuWork_ListCreate( 2, heap );
  BmpMenuWork_ListAddArchiveString( ld, man, msgid_yesno_yes, 0,heap );
  BmpMenuWork_ListAddArchiveString( ld, man, msgid_yesno_no, BMPMENU_CANCEL,heap );
  GFL_MSG_Delete( man );

  GFL_STD_MemClear(&hed,sizeof(BMPMENU_HEADER));

  hed.menu     = ld;
  hed.win      = bmpwin;
  hed.x_max    = 1;
  hed.y_max    = 2;
  hed.line_spc = 1;
  hed.c_disp_f = 0;
  hed.loop_f = 0;
  hed.print_util = GFL_HEAP_AllocClearMemory(heap,sizeof(PRINT_UTIL));

  hed.font_handle = GFL_FONT_Create
                (ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, heap );
  hed.font_size_y = GFL_FONT_GetLineHeight(hed.font_handle);
  hed.font_size_x = 2+8;//hed.font_size_y; //�����̒l���ƕ������[���J�[�\���ړ��ŏ����Ă��܂�
    // 2+8�̗��R(define�ɂ����ق�������) 
    // prog/src/system/bmp_cursor.c
    // void BmpCursor_Print( const BMPCURSOR *bmpCursor, u32 x, u32 y,
		//                 PRINT_UTIL *printUtil, PRINT_QUE *printQue, GFL_FONT *fontHandle )
    // GFL_BMP_Print( bmpCursor->bmp, bmp, 0, 0, x+2, y+2, 8, 8, 0x0f );
  PRINT_UTIL_Setup(hed.print_util, hed.win);
  hed.print_que = PRINTSYS_QUE_Create( heap );
//  GFL_MSGDATA *msgdata; //�\���Ɏg�p���郁�b�Z�[�W�o�b�t�@

  //  GFL_BG_BmpWinAddEx( ini, hed.win, data );
  //BmpMenuWinWrite( hed.win, WINDOW_TRANS_OFF, cgx, pal );
  pWk = BmpMenu_AddEx( &hed, 14, 0, pos, heap, PAD_BUTTON_CANCEL );
  GFL_BMPWIN_MakeScreen(hed.win);
  BmpWinFrame_Write( hed.win, WINDOW_TRANS_ON, cgx, pal );

  GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(hed.win) );
  GFL_BMPWIN_TransVramCharacter( hed.win );

  return pWk;
}


BMPMENU_WORK * BmpMenu_YesNoSelectInit
  ( const BMPWIN_YESNO_DAT *data, u16 winframecgx, u8 winframepal, u8 pos, HEAPID heap )
{
  GFL_BMPWIN* bmpwin = GFL_BMPWIN_Create
    ( data->frmnum , data->pos_x, data->pos_y, 7, 4, data->palnum, GFL_BMP_CHRAREA_GET_B );
  return BmpMenu_YesNoSelectInitCore(bmpwin, winframecgx, winframepal, pos, heap);
}

BMPMENU_WORK * BmpMenu_YesNoSelectInitFixPos
  ( const BMPWIN_YESNO_DAT *data, u16 winframecgx, u8 winframepal, u8 pos, HEAPID heap )
{
  GFL_BMPWIN* bmpwin = GFL_BMPWIN_CreateFixPos
    ( data->frmnum , data->pos_x, data->pos_y, 7, 4, data->palnum, data->chrnum );
  return BmpMenu_YesNoSelectInitCore(bmpwin, winframecgx, winframepal, pos, heap);
}
#endif

//--------------------------------------------------------------------------------------------
/**
 * �͂��E�������I���E�B���h�E�̐���
 *
 * @param ini   BGL�f�[�^
 * @param heap  �q�[�vID
 *
 * @retval  "BMPMENU_NULL �I������Ă��Ȃ�"
 * @retval  "0        �͂���I��"
 * @retval  "BMPMENU_CANCEL ������or�L�����Z��"
 */
//--------------------------------------------------------------------------------------------
u32 BmpMenu_YesNoSelectMain( BMPMENU_WORK * mw )
{
  //  u32 ret = BmpMenu_MainSE( mw,SE_DECIDE );
  u32 ret = BmpMenu_Main( mw );

  BOOL b_finish = PRINTSYS_QUE_Main(mw->hed.print_que);
  
  if( (!(mw->hed.b_trans)) && b_finish )
  {
    GFL_BMPWIN_TransVramCharacter( mw->hed.win );
    mw->hed.b_trans = TRUE;
  }

  if( ret != BMPMENU_NULL ){
    BmpMenu_YesNoMenuExit( mw );
  }
  return  ret;
}


//--------------------------------------------------------------------------------------------
/**
 * �͂��E�������I���E�B���h�E�O���R���g���[��
 *
 * @param ini   BGL�f�[�^
 * @param prm   �R���g���[���p�����[�^
 * @param heap  �q�[�vID
 *
 * @retval  "BMPMENU_NULL �I������Ă��Ȃ�"
 * @retval  "0        �͂���I��"
 * @retval  "BMPMENU_CANCEL ������or�L�����Z��"
 */
//--------------------------------------------------------------------------------------------
#if 0
u32 BmpYesNoSelectMainOutControl( BMPMENU_WORK * mw, u8 prm, HEAPID heap )
{
  u32 ret = BmpMenuMainOutControl( mw, prm );

  if( ret != BMPMENU_NULL ){
    BmpYesNoWinDel( mw, heap );
  }
  return  ret;
}
#endif

//--------------------------------------------------------------------------------------------
/**
 * �͂��E�������E�B���h�E�폜
 *
 * @param ini   BGL�f�[�^
 * @param heap  �q�[�vID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BmpMenu_YesNoMenuExit( BMPMENU_WORK * mw )
{
  PRINTSYS_QUE_Clear( mw->hed.print_que );
  PRINTSYS_QUE_Delete( mw->hed.print_que );
  GFL_HEAP_FreeMemory( mw->hed.print_util );

  BmpWinFrame_Clear( mw->hed.win, WINDOW_TRANS_ON );
  GFL_FONT_Delete( mw->hed.font_handle );
  BmpMenuWork_ListDelete( (BMPMENU_DATA*)mw->hed.menu );

  GFL_BMPWIN_Delete( mw->hed.win );
  BmpMenu_Exit( mw, NULL );
}


//------------------------------------------------------------------
/**
 * �E�B���h�E�ɃJ�[�\���摜��`��
 *
 * @param   win   �E�B���h�E
 * @param   x   �`��w���W�i�h�b�g�j
 * @param   y   �`��x���W�i�h�b�g�j
 *
 */
//------------------------------------------------------------------
void BmpWin_DrawCursorImage(GFL_BMPWIN* win, u32 x, u32 y)
{
#if 0
  static const u8 CursorBitmapImage[] = {
    0xff,0xff,0xff,0x00,
    0xff,0xff,0xff,0x00,
    0x21,0xff,0xff,0x00,
    0x11,0xf2,0xff,0x00,
    0x11,0x21,0xff,0x00,
    0x11,0x11,0xf2,0x00,
    0x11,0x11,0x21,0x00,
    0x11,0x11,0x22,0x00,

    0x11,0x21,0xf2,0x00,
    0x11,0x22,0xff,0x00,
    0x21,0xf2,0xff,0x00,
    0x22,0xff,0xff,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
  };

  GFL_BMP_Print( win, (void*)CursorBitmapImage, 0, 0, 8, 16, x, y, 8, 16 );
#endif
}
#endif //YESNOWINDOW

//======================================================================
//
//======================================================================
//--------------------------------------------------------------
//  bitmap on
//  GF_BGL_BmpWinOn()
//--------------------------------------------------------------
#if 0
static void BmpWinOn( GFL_BMPWIN *bmpwin )
{
  GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(bmpwin) );
  GFL_BMPWIN_TransVramCharacter( bmpwin );
}
#endif

//--------------------------------------------------------------
/**
 * �J�[�\��������Z�b�g
 * @param mw  BMPMENU_WORK
 * @param strID ������ID
 * @retval  nothing
 */
//--------------------------------------------------------------
void BmpMenu_SetCursorString( BMPMENU_WORK *mw, u32 strID )
{
  BmpCursor_SetCursorFontMsg( mw->cursor, mw->hed.msgdata, strID );
  BmpMenuCursorPut( mw );
}

//�֐��ǉ����܂��� Ari081015
//--------------------------------------------------------------
/**
 * ���j���[�ĕ`��
 * @param mw  BMPMENU_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
void BmpMenu_RedrawString( BMPMENU_WORK *mw )
{
  BmpMenuStrPut( mw );
  BmpMenuCursorPut( mw );
}
