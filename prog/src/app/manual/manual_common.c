//============================================================================
/**
 *  @file   manual_common.c
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *  ���W���[�����FMANUAL_COMMON
 */
//============================================================================
// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "gamesystem/gamedata_def.h"
#include "gamesystem/game_data.h"
#include "print/gf_font.h"
#include "print/printsys.h"

#include "manual_graphic.h"
#include "manual_def.h"
#include "manual_data.h"
#include "manual_touchbar.h"
#include "manual_common.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_manual.h"


// �T�E���h

// �I�[�o�[���C


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// ���C��BG�̐ݒ�
static void Manual_Common_MainBgInit( MANUAL_COMMON_WORK* work );
static void Manual_Common_MainBgExit( MANUAL_COMMON_WORK* work );


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
// ����������
MANUAL_COMMON_WORK*  MANUAL_COMMON_Init(
    GAMEDATA*  gamedata,
    HEAPID     heap_id
)
{
  // ���[�N
  MANUAL_COMMON_WORK*  work  = GFL_HEAP_AllocClearMemory( heap_id, sizeof(MANUAL_COMMON_WORK) );

  // MANUAL���Ă񂾏ꏊ����̎؂蕨
  {
    work->gamedata = gamedata;
  }

  // MANUAL�Ő�����������
  {
    work->heap_id = heap_id;
  }

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    work->graphic       = MANUAL_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, work->heap_id );
    work->font          = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, work->heap_id );
    
    work->print_que_title  = PRINTSYS_QUE_Create( work->heap_id );
    work->print_que_main   = PRINTSYS_QUE_Create( work->heap_id );
    work->print_que_tb     = PRINTSYS_QUE_Create( work->heap_id );
  }

  // ���b�Z�[�W
  {
    work->msgdata_system = GFL_MSG_Create(
                               GFL_MSG_LOAD_NORMAL,
                               ARCID_MESSAGE,
                               NARC_message_manual_dat,
                               work->heap_id );
    work->msgdata_main   = GFL_MSG_Create(
                               GFL_MSG_LOAD_NORMAL,
                               ARCID_MESSAGE,
                               NARC_message_manual_text_dat,
                               work->heap_id );
  }

  // �t�@�C���n���h��
  {
    work->handle_system  = GFL_ARC_OpenDataHandle( ARCID_MANUAL, work->heap_id );
    work->handle_explain = GFL_ARC_OpenDataHandle( ARCID_MANUAL_IMAGE, work->heap_id );
  }

  // �p���b�g
  {
    // �T�uBG�p���b�g
    // �e�L�X�g
    GFL_ARC_UTIL_TransVramPaletteEx(
        ARCID_FONT,
        NARC_font_default_nclr,
        PALTYPE_SUB_BG,
        0,
        BG_PAL_POS_S_TEXT * 0x20,
        BG_PAL_NUM_S_TEXT * 0x20,
        work->heap_id );
  }

  // �}�j���A���f�[�^
  {
    work->data_wk = MANUAL_DATA_Load( work->handle_system, work->heap_id ); 
  }

  // ���ʂɃ_�C���N�g�J���[�摜��\������
  {
    // ���C��BG�̐ݒ�
    Manual_Common_MainBgInit( work );

    // ���ʂɕ\�����Ă���_�C���N�g�J���[�摜��ID
    work->bg_m_dcbmp_id = MANUAL_BG_M_DCBMP_NO_IMAGE;  // �ŏ��ɕ\������摜��ID�ƈقȂ�΂ǂ�Ȓl�ł��悢

    // ���ʂɃ_�C���N�g�J���[�摜��\������
    MANUAL_COMMON_DrawBgMDcbmp( work, MANUAL_DATA_ImageIdGetNoImage( work->data_wk ) );
  }

  // �Ăяo�����֐��̒���MANUAL_COMMON_WORK���g�p������̂́AMANUAL_COMMON_Init��1�ԍŌ�ɌĂяo�����ƁB
  // �}�j���A���^�b�`�o�[
  {
    work->mtb_wk = MANUAL_TOUCHBAR_Init( work );
  }

  return  work;
}

// �I������
void  MANUAL_COMMON_Exit(
    MANUAL_COMMON_WORK*  work
)
{
  // �}�j���A���^�b�`�o�[
  {
    MANUAL_TOUCHBAR_Exit( work->mtb_wk );
  }

  // ���ʂɃ_�C���N�g�J���[�摜��\������
  {
    // ���ʂɃ_�C���N�g�J���[�摜��\������
    MANUAL_COMMON_DrawBgMDcbmp( work, MANUAL_BG_M_DCBMP_NO_IMAGE );

    // ���C��BG�̐ݒ�
    Manual_Common_MainBgExit( work );
  }

  // �}�j���A���f�[�^
  {
    MANUAL_DATA_Unload( work->data_wk ); 
  }

  // �t�@�C���n���h��
  {
    GFL_ARC_CloseDataHandle( work->handle_system );
    GFL_ARC_CloseDataHandle( work->handle_explain );
  }

  // ���b�Z�[�W
  {
    GFL_MSG_Delete( work->msgdata_system );
    GFL_MSG_Delete( work->msgdata_main );
  }

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    PRINTSYS_QUE_Clear( work->print_que_title );
    PRINTSYS_QUE_Delete( work->print_que_title );
    PRINTSYS_QUE_Clear( work->print_que_main );
    PRINTSYS_QUE_Delete( work->print_que_main );
    PRINTSYS_QUE_Clear( work->print_que_tb );
    PRINTSYS_QUE_Delete( work->print_que_tb );
    
    GFL_FONT_Delete( work->font );
    MANUAL_GRAPHIC_Exit( work->graphic );
  }

  // ���[�N
  GFL_HEAP_FreeMemory( work );
}

// �又��
void  MANUAL_COMMON_Main(
    MANUAL_COMMON_WORK*  work
)
{
  // print_que
  PRINTSYS_QUE_Main( work->print_que_title );
  PRINTSYS_QUE_Main( work->print_que_main );
  PRINTSYS_QUE_Main( work->print_que_tb );

  // 2D�`��
  MANUAL_GRAPHIC_2D_Draw( work->graphic );

  // 3D�`��
  //MANUAL_GRAPHIC_3D_StartDraw( work->graphic );
  //MANUAL_GRAPHIC_3D_EndDraw( work->graphic );
}

// ���ʂɃ_�C���N�g�J���[�摜��\������
void  MANUAL_COMMON_DrawBgMDcbmp(
    MANUAL_COMMON_WORK*  work,
    u16                  bg_m_dcbmp_id
)
{
  if( work->bg_m_dcbmp_id != bg_m_dcbmp_id )  // �ύX����K�v������Ƃ�

  {
    u32  image_size = 256*192*2;
    u32  size;
    u16* buf;
    
    work->bg_m_dcbmp_id = bg_m_dcbmp_id;

    if( work->bg_m_dcbmp_id == MANUAL_BG_M_DCBMP_NO_IMAGE )
    {
      buf = GFL_HEAP_AllocClearMemory( work->heap_id, image_size );
      size = image_size;
    }
    else
    {
      buf = GFL_ARCHDL_UTIL_LoadEx( work->handle_explain, work->bg_m_dcbmp_id, TRUE, work->heap_id, &size );
    }

    DC_FlushRange( buf, size );
    GX_LoadBG2Bmp( buf, 0, image_size );
    GFL_HEAP_FreeMemory( buf );
  }
}


//------------------------------------------------------------------
/**
 *  @brief           
 *
 *  @param[in]       
 *  @param[in,out]       
 *
 *  @retval          
 */
//------------------------------------------------------------------

//-------------------------------------
/// 
//=====================================


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// ���C��BG�̐ݒ�
//=====================================
static void Manual_Common_MainBgInit( MANUAL_COMMON_WORK* work )
{
  {
    G2_SetBG2ControlDCBmp( GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000 );
    G2_SetBG2Priority( BG_FRAME_PRI_M_PIC );
    G2_BG0Mosaic( FALSE );
  }

  GFL_BG_SetVisible( BG_FRAME_M_PIC, VISIBLE_ON );
}
static void Manual_Common_MainBgExit( MANUAL_COMMON_WORK* work )
{
  // �������Ȃ�
}

