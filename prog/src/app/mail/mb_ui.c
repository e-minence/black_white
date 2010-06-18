//============================================================================================
/**
 * @file  mb_ui.c
 * @brief ���[���{�b�N�X��� �C���^�[�t�F�[�X�֘A
 * @author  Hiroyuki Nakamura
 * @date  09.01.31
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"
#include "mb_main.h"
#include "mb_obj.h"
#include "mb_ui.h"


//============================================================================================
//  �萔��`
//============================================================================================


//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================
static void CursorMoveCallBack_On( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Off( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Move( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Touch( void * work, int now_pos, int old_pos );


//============================================================================================
//  �O���[�o���ϐ�
//============================================================================================

// ���[���I��
static const CURSORMOVE_DATA MailSelTbl[] =
{
  {  64,  16, 0, 0, 0,  2, 0, 1, {   0,  31,   8, 119 },},    // 00: ���[���P
  { 192,  16, 0, 0, 1,  3, 0, 1, {   0,  31, 136, 244 },},    // 01: ���[���Q
  {  64,  48, 0, 0, 0,  4, 2, 3, {  32,  63,   8, 119 },},    // 02: ���[���R
  { 192,  48, 0, 0, 1,  5, 2, 3, {  32,  63, 136, 244 },},    // 03: ���[���S
  {  64,  80, 0, 0, 2,  6, 4, 5, {  64,  95,   8, 119 },},    // 04: ���[���T
  { 192,  80, 0, 0, 3,  7, 4, 5, {  64,  95, 136, 244 },},    // 05: ���[���U
  {  64, 112, 0, 0, 4,  8, 6, 7, {  96, 127,   8, 119 },},    // 06: ���[���V
  { 192, 112, 0, 0, 5,  9, 6, 7, {  96, 127, 136, 244 },},    // 07: ���[���W
  {  64, 144, 0, 0, 6,  8, 8, 9, { 128, 159,   8, 119 },},    // 08: ���[���X
  { 192, 144, 0, 0, 7,  9, 8, 9, { 128, 159, 136, 244 },},    // 09: ���[���P�O
  {   0,   0, 0, 0, 0,  0, 0, 0, { GFL_UI_TP_HIT_END, 0, 0, 0 },},

};
static const CURSORMOVE_CALLBACK MailSelCallBack = {
  CursorMoveCallBack_On,
  CursorMoveCallBack_Off,
  CursorMoveCallBack_Move,
  CursorMoveCallBack_Touch
};

// ���[���I���y�[�W�؂�ւ�
static const GFL_UI_TP_HITTBL TouchButtonHitTbl[] =
{
  { 168, 191,   8,  8+24 },   //  �����
  { 168, 191,  32,  32+24 },  //  �E���
  { 168, 191, 232,  255 },    //  ��߂�
  { 168, 191, 194,  231 },    //  �u�~�v
  { GFL_UI_TP_HIT_END, 0, 0, 0 }
};

//�u���[������ށv
static const GFL_UI_TP_HITTBL MailReadHitTbl[] =
{
  { 160, 191, 230, 255 },    // 0: ��߂�
  { GFL_UI_TP_HIT_END, 0, 0, 0 }
};


//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�؂�ւ����^�b�`�`�F�b�N
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ����
 */
//--------------------------------------------------------------------------------------------
int MBUI_MailboxTouchButtonCheck( MAILBOX_SYS_WORK * syswk )
{
  return GFL_UI_TP_HitTrg( TouchButtonHitTbl );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ�������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBUI_CursorMoveInit( MAILBOX_SYS_WORK * syswk )
{
  int pos;

  if( syswk->sel_page > syswk->app->page_max ){
    syswk->sel_page = syswk->app->page_max;
    pos = 0;
  }else{
    pos = syswk->sel_pos;
  }

  syswk->app->cmwk = CURSORMOVE_Create(
              MailSelTbl,
              &MailSelCallBack,
              syswk,
              TRUE,
              pos,
              HEAPID_MAILBOX_APP );

  MBUI_MailSelCurMove( syswk, pos );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ��폜
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBUI_CursorMoveExit( MAILBOX_SYS_WORK * syswk )
{
  CURSORMOVE_Exit( syswk->app->cmwk );
}

//--------------------------------------------------------------------------------------------
/**
 * �R�[���o�b�N�֐��F�J�[�\���\��
 *
 * @param work    ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param now_pos   ���݂̈ʒu
 * @param old_pos   �O��̈ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveCallBack_On( void * work, int now_pos, int old_pos )
{
}

//--------------------------------------------------------------------------------------------
/**
 * �R�[���o�b�N�֐��F�J�[�\����\��
 *
 * @param work    ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param now_pos   ���݂̈ʒu
 * @param old_pos   �O��̈ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveCallBack_Off( void * work, int now_pos, int old_pos )
{
}

//--------------------------------------------------------------------------------------------
/**
 * �R�[���o�b�N�֐��F�J�[�\���ړ�
 *
 * @param work    ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param now_pos   ���݂̈ʒu
 * @param old_pos   �O��̈ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveCallBack_Move( void * work, int now_pos, int old_pos )
{
  MBUI_MailSelCurMove( work, now_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * �R�[���o�b�N�֐��F�^�b�`
 *
 * @param work    ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param now_pos   ���݂̈ʒu
 * @param old_pos   �O��̈ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveCallBack_Touch( void * work, int now_pos, int old_pos )
{
  MBUI_MailSelCurMove( work, now_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * ���[���I���̃J�[�\���ʒu�Z�b�g
 *
 * @param work    ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param pos     �ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBUI_MailSelCurMove( MAILBOX_SYS_WORK * syswk, int pos )
{
  const CURSORMOVE_DATA *cd = CURSORMOVE_GetMoveData( syswk->app->cmwk, pos );

  MBOBJ_PosSet( syswk->app, MBMAIN_OBJ_CURSOR, cd->px, cd->py );

  if( pos == MBUI_MAILSEL_RETURN ){
    MBOBJ_AnmSet( syswk->app, MBMAIN_OBJ_CURSOR, 4 );
  }else{
    MBOBJ_AnmSet( syswk->app, MBMAIN_OBJ_CURSOR, 5 );
  }
}


//--------------------------------------------------------------------------------------------
/**
 * �u���[������ށv��ʃL�[�E�^�b�`�`�F�b�N
 *
 * @param none
 *
 * @retval  "TRUE = �I��"
 * @retval  "FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL MBUI_MailReadCheck(void)
{
  if( GFL_UI_TP_HitTrg( MailReadHitTbl ) != GFL_UI_TP_HIT_NONE ){
    return TRUE;
  }

  if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE) ){
    return TRUE;
  }

  return FALSE;
}

//=============================================================================================
/**
 * @brief ���ڏI���{�^���`�F�b�N
 *
 * @param   none    
 *
 * @retval  BOOL    TRUE:X_BUTTON�������� FALSE:�����ĂȂ�
 */
//=============================================================================================
BOOL MBUI_EndButtonCheck( void )
{
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
    return TRUE;
  }
  
  return FALSE;
}

