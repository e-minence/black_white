//============================================================================
/**
 *  @file   zukan_detail_headbar.c
 *  @brief  �}�ӏڍ׉�ʋ��ʂ̃^�C�g���o�[
 *  @author Koji Kawada
 *  @data   2010.02.02
 *  @note   
 *  ���W���[�����FZUKAN_DETAIL_HEADBAR
 */
//============================================================================
// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/bmp_winframe.h"
#include "print/gf_font.h"
#include "print/printsys.h"

#include "zukan_detail_def.h"
#include "zukan_detail_headbar.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_zkn.h"
#include "zukan_gra.naix"

// �T�E���h

// �I�[�o�[���C


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
// �X�N���[��
#define SCROLL_Y_DISAPPEAR   (24)
#define SCROLL_Y_APPEAR      (0)
#define SCROLL_VALUE         (3)  // �ړ���
#define SCROLL_WAIT          (0)  // 0=���t���[���ړ�; 1=�ړ��������1�t���[���҂�; 2=�ړ��������2�t���[���҂�;


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
struct _ZUKAN_DETAIL_HEADBAR_WORK
{
  // ���̂Ƃ��납��ؗp�������
  HEAPID                      heap_id;
  GFL_FONT*                   font;
  PRINT_QUE*                  print_que;

  // �����ō쐬�������
  // ���
  ZUKAN_DETAIL_HEADBAR_STATE  state;
  ZUKAN_DETAIL_HEADBAR_TYPE   type;
  u8                          scroll_wait_count;

  // �^�C�g���o�[
  GFL_BMPWIN*                 dummy_bmpwin;  // ���̂��L�����̈ʒu�����܂��I�t�Z�b�g����Ȃ�
  GFL_BMP_DATA*               head_bmp_data;
  u32                         head_bmp_pos;
  u32                         head_bmp_size; 
  GFL_MSGDATA*                head_msgdata;

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;
};


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Zukan_Detail_Headbar_VBlankFunc( GFL_TCB* tcb, void* wk );

// �^�C�g���o�[
static void Zukan_Detail_Headbar_CreateBase( ZUKAN_DETAIL_HEADBAR_WORK* work );
static void Zukan_Detail_Headbar_DeleteBase( ZUKAN_DETAIL_HEADBAR_WORK* work );
static void Zukan_Detail_Headbar_Create( ZUKAN_DETAIL_HEADBAR_WORK* work );
static void Zukan_Detail_Headbar_Delete( ZUKAN_DETAIL_HEADBAR_WORK* work );


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
ZUKAN_DETAIL_HEADBAR_WORK* ZUKAN_DETAIL_HEADBAR_Init(
                               HEAPID      heap_id,
                               GFL_FONT*   font,
                               PRINT_QUE*  print_que )
{
  ZUKAN_DETAIL_HEADBAR_WORK*   work;

  // ���[�N����
  work = GFL_HEAP_AllocClearMemory( heap_id, sizeof(ZUKAN_DETAIL_HEADBAR_WORK) );

  // ������
  work->heap_id     = heap_id;
  work->font        = font;
  work->print_que   = print_que;

  // ���
  work->state = ZUKAN_DETAIL_HEADBAR_STATE_DISAPPEAR;
  work->type = ZUKAN_DETAIL_HEADBAR_TYPE_NONE;

  // �����X�N���[���ʒu
  GFL_BG_SetScroll( ZKNDTL_BG_FRAME_S_HEADBAR, GFL_BG_SCROLL_Y_SET, SCROLL_Y_DISAPPEAR );

  // �v���C�I���e�B
  GFL_BG_SetPriority( ZKNDTL_BG_FRAME_S_HEADBAR, ZKNDTL_BG_FRAME_PRI_S_HEADBAR );

  // �^�C�g���o�[
  Zukan_Detail_Headbar_CreateBase( work );
  Zukan_Detail_Headbar_Create( work );

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Zukan_Detail_Headbar_VBlankFunc, work, 1 );

  return work;
}

//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void ZUKAN_DETAIL_HEADBAR_Exit( ZUKAN_DETAIL_HEADBAR_WORK* work )
{
  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �^�C�g���o�[
  Zukan_Detail_Headbar_Delete( work );
  Zukan_Detail_Headbar_DeleteBase( work );

  // ���[�N�j��
  GFL_HEAP_FreeMemory( work );
}

//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void ZUKAN_DETAIL_HEADBAR_Main( ZUKAN_DETAIL_HEADBAR_WORK* work )
{
  switch( work->state )
  {
  case ZUKAN_DETAIL_HEADBAR_STATE_DISAPPEAR:
    {
    }
    break;
  case ZUKAN_DETAIL_HEADBAR_STATE_DISAPPEAR_TO_APPEAR:
    {
      if( work->scroll_wait_count == 0 )
      {
        if( GFL_BG_GetScrollY( ZKNDTL_BG_FRAME_S_HEADBAR ) <= SCROLL_Y_APPEAR )
        {
          GFL_BG_SetScrollReq( ZKNDTL_BG_FRAME_S_HEADBAR, GFL_BG_SCROLL_Y_SET, SCROLL_Y_APPEAR );
          work->state = ZUKAN_DETAIL_HEADBAR_STATE_APPEAR;
        }
        else
        {
          GFL_BG_SetScrollReq( ZKNDTL_BG_FRAME_S_HEADBAR, GFL_BG_SCROLL_Y_DEC, SCROLL_VALUE );
          work->scroll_wait_count = SCROLL_WAIT;
        }
      }
      else
      {
        work->scroll_wait_count--;
      }
    }
    break;
  case ZUKAN_DETAIL_HEADBAR_STATE_APPEAR:
    {
    }
    break;
  case ZUKAN_DETAIL_HEADBAR_STATE_APPEAR_TO_DISAPPEAR:
    {
      if( work->scroll_wait_count == 0 )
      {
        if( GFL_BG_GetScrollY( ZKNDTL_BG_FRAME_S_HEADBAR ) >= SCROLL_Y_DISAPPEAR )
        {
          GFL_BG_SetScrollReq( ZKNDTL_BG_FRAME_S_HEADBAR, GFL_BG_SCROLL_Y_SET, SCROLL_Y_DISAPPEAR );
          work->state = ZUKAN_DETAIL_HEADBAR_STATE_DISAPPEAR;
        }
        else
        {
          GFL_BG_SetScrollReq( ZKNDTL_BG_FRAME_S_HEADBAR, GFL_BG_SCROLL_Y_INC, SCROLL_VALUE );
          work->scroll_wait_count = SCROLL_WAIT;
        }
      }
      else
      {
        work->scroll_wait_count--;
      }
    }
    break;
  }
}

//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void ZUKAN_DETAIL_HEADBAR_SetType(
                   ZUKAN_DETAIL_HEADBAR_WORK* work,
                   ZUKAN_DETAIL_HEADBAR_TYPE type )
{
  work->type = type;
  Zukan_Detail_Headbar_Delete( work );
  Zukan_Detail_Headbar_Create( work );
}

//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
ZUKAN_DETAIL_HEADBAR_STATE ZUKAN_DETAIL_HEADBAR_GetState( ZUKAN_DETAIL_HEADBAR_WORK* work )
{
  return work->state;
}

//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void ZUKAN_DETAIL_HEADBAR_Appear( ZUKAN_DETAIL_HEADBAR_WORK* work )
{
  if( work->state != ZUKAN_DETAIL_HEADBAR_STATE_APPEAR )
  {
    work->state = ZUKAN_DETAIL_HEADBAR_STATE_DISAPPEAR_TO_APPEAR;
    work->scroll_wait_count = 0;
  }
}

//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void ZUKAN_DETAIL_HEADBAR_Disappear( ZUKAN_DETAIL_HEADBAR_WORK* work )
{
  if( work->state != ZUKAN_DETAIL_HEADBAR_STATE_DISAPPEAR )
  {
    work->state = ZUKAN_DETAIL_HEADBAR_STATE_APPEAR_TO_DISAPPEAR;
    work->scroll_wait_count = 0;
  }
}


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// VBlank�֐�
//=====================================
static void Zukan_Detail_Headbar_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  ZUKAN_DETAIL_HEADBAR_WORK* work = (ZUKAN_DETAIL_HEADBAR_WORK*)wk;
}

//-------------------------------------
/// �^�C�g���o�[
//=====================================
static void Zukan_Detail_Headbar_CreateBase( ZUKAN_DETAIL_HEADBAR_WORK* work )
{
  // BG�t���[���̃X�N���[���̋󂢂Ă���ӏ��ɉ����\��������Ȃ��悤�ɂ��Ă���
//  work->dummy_bmpwin = GFL_BMPWIN_Create( ZKNDTL_BG_FRAME_S_HEADBAR, 0, 0, 1, 1,
  work->dummy_bmpwin = GFL_BMPWIN_Create( ZKNDTL_BG_FRAME_S_HEADBAR, 0, 0, 16, 1,
                            ZKNDTL_BG_PAL_POS_S_HEADBAR, GFL_BMP_CHRAREA_GET_F );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->dummy_bmpwin), 0 );
  GFL_BMPWIN_TransVramCharacter(work->dummy_bmpwin);
  // ���̂��L�����̈ʒu�����܂��I�t�Z�b�g����Ȃ�

  // BG�L����
  {
    work->head_bmp_data = GFL_BMP_LoadCharacter( ARCID_ZUKAN_GRA, NARC_zukan_gra_info_infotitle_bgu_NCGR, 0, work->heap_id );
    work->head_bmp_size = GFL_BMP_GetBmpDataSize( work->head_bmp_data );
    work->head_bmp_pos  = GFL_BG_LoadCharacterAreaMan(
                              ZKNDTL_BG_FRAME_S_HEADBAR,
                              GFL_BMP_GetCharacterAdrs( work->head_bmp_data ),
                              work->head_bmp_size );
    GF_ASSERT_MSG( work->head_bmp_pos != AREAMAN_POS_NOTFOUND, "ZUKAN_DETAIL_HEADBAR : BG�L�����̈悪����܂���ł����B\n" );
  }

  // BG�X�N���[��
  {
    //GFL_ARCHDL_UTIL_TransVramScreenCharOfsVBlank
    GFL_ARC_UTIL_TransVramScreenCharOfs(
        ARCID_ZUKAN_GRA,
        NARC_zukan_gra_info_infotitle_bgu_NSCR,
        ZKNDTL_BG_FRAME_S_HEADBAR,
//        work->head_bmp_pos,    // ����������    // ���̂��L�����̈ʒu�����܂��I�t�Z�b�g����Ȃ�
//        0,                     // �������悤��  // ���̂��L�����̈ʒu�����܂��I�t�Z�b�g����Ȃ�
        0,
        work->head_bmp_pos,
        32 * 3 * GFL_BG_1SCRDATASIZ,
        FALSE,
        work->heap_id );
    GFL_BG_ChangeScreenPalette( ZKNDTL_BG_FRAME_S_HEADBAR, 0, 0, 32, 24, ZKNDTL_BG_PAL_POS_S_HEADBAR );
  }

  // ���b�Z�[�W
  work->head_msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL,
                           ARCID_MESSAGE, NARC_message_zkn_dat,
                           work->heap_id );
}
static void Zukan_Detail_Headbar_DeleteBase( ZUKAN_DETAIL_HEADBAR_WORK* work )
{
  GFL_MSG_Delete( work->head_msgdata );

  GFL_BG_FreeCharacterArea( ZKNDTL_BG_FRAME_S_HEADBAR,
                            work->head_bmp_pos,
                            work->head_bmp_size );
  GFL_BMP_Delete( work->head_bmp_data );

  GFL_BMPWIN_Delete( work->dummy_bmpwin );  // ���̂��L�����̈ʒu�����܂��I�t�Z�b�g����Ȃ�
}
static void Zukan_Detail_Headbar_Create( ZUKAN_DETAIL_HEADBAR_WORK* work )
{
  if( !( 0 <= work->type && work->type < ZUKAN_DETAIL_HEADBAR_TYPE_MAX ) ) return;

  // BG�p���b�g
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, work->heap_id );

    u32 pal_ofs[ZUKAN_DETAIL_HEADBAR_TYPE_MAX] =
    {
      0,
      2,
      1,
      3,
      3,
    };

    PALTYPE paltype = ( ZKNDTL_BG_FRAME_S_HEADBAR < GFL_BG_FRAME0_S ) ? (PALTYPE_MAIN_BG) : (PALTYPE_SUB_BG);
    
    GFL_ARCHDL_UTIL_TransVramPaletteEx(
       handle,
       NARC_zukan_gra_info_infotitle_bgu_NCLR,
       paltype,
       pal_ofs[work->type] * 0x20,
       ZKNDTL_BG_PAL_POS_S_HEADBAR * 0x20,
       ZKNDTL_BG_PAL_NUM_S_HEADBAR * 0x20,
       work->heap_id );
    
    GFL_ARC_CloseDataHandle( handle );
  }

  // BG�L����
  {
    GFL_BMP_DATA* original_bmp_data = GFL_BMP_LoadCharacter( ARCID_ZUKAN_GRA, NARC_zukan_gra_info_infotitle_bgu_NCGR, 0, work->heap_id );
    
    // �㏑�����ď�����Ă��镶��������
    GFL_BMP_Copy( original_bmp_data, work->head_bmp_data );

    GFL_BMP_Delete( original_bmp_data );
  }

  // �����`��
  {
    STRBUF* strbuf = GFL_MSG_CreateString( work->head_msgdata, ZKN_DETAIL_HEAD_01 + work->type );
    PRINTSYS_PrintQueColor( work->print_que, work->head_bmp_data,
        24, 5, strbuf, work->font, PRINTSYS_LSB_Make(15,14,0) );
    GFL_STR_DeleteBuffer( strbuf );
  }

  // BG�L�����𓯂��ꏊ�ɓ]������
  GFL_BG_LoadCharacter(
      ZKNDTL_BG_FRAME_S_HEADBAR,
      GFL_BMP_GetCharacterAdrs( work->head_bmp_data ),
      work->head_bmp_size,
      work->head_bmp_pos );

  // �X�N���[���]��
	GFL_BG_LoadScreenV_Req( ZKNDTL_BG_FRAME_S_HEADBAR );
}
static void Zukan_Detail_Headbar_Delete( ZUKAN_DETAIL_HEADBAR_WORK* work )
{
  // �������Ȃ�
}

