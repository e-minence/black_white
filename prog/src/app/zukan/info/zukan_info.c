//============================================================================
/**
 *  @file   zukan_info.c
 *  @brief  �}�ӏ��
 *  @author Koji Kawada
 *  @data   2009.12.03
 *  @note   infowin.c, touchbar.c, ui_template.c���Q�l�ɂ��č쐬���܂����B
 *
 *  ���W���[�����FZUKAN_INFO
 */
//============================================================================
// lib
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
















#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_msg.h"
#define PSTATUS_BG_PLT_FONT (4)
enum
{
  ZUKAN_INFO_MSG_TOROKU,
  ZUKAN_INFO_MSG_NAME,
  ZUKAN_INFO_MSG_KIND,
  ZUKAN_INFO_MSG_HEIGHT,
  ZUKAN_INFO_MSG_WEIGHT,
  ZUKAN_INFO_MSG_EXPLAIN,

  ZUKAN_INFO_MSG_MAX,
};

#include "ui/ui_easy_clwk.h"
#define PLTID_OBJ_TYPEICON_M (3)  // 3�{�g�p

#include "system/poke2dgra.h"
#define PLTID_OBJ_POKE_M (12)

#include "poke_tool/pokefoot.h"
#define PLTID_OBJ_POKEFOOT_M (13)
/*
make_prog_files
��
#�풓�v���O����
SRCS += $(SOUNDSRC) \
        ...
        poke_tool/pokefoot.c \
        ...
��ǉ������B
*/
/*
arc_tool.lst
��
ARCID_POKEFOOT_GRA
��ǉ������B
*/









// �A�[�J�C�u
#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"
#include "zukan_gra.naix"

// ���b�Z�[�W
#include "msg/msg_pokestatus.h"

// zukan_info
#include "zukan_info.h"

//=============================================================================
/**
 *  �萔��`
 */
//=============================================================================
#define ZUKAN_INFO_BG_PAL_LINE_SIZE (0x20)
#define ZUKAN_INFO_BG_PAL_NUM       (3)
#define ZUKAN_INFO_BACK_BG_PAL_NO   (0)
#define ZUKAN_INFO_FORE_BG_PAL_NO_D (1)
#define ZUKAN_INFO_FORE_BG_PAL_NO_L (2)
#define ZUKAN_INFO_BG_CHARA_SIZE    ( 32 * 4 * GFL_BG_1CHRDATASIZ )
#define ZUKAN_INFO_BG_SCREEN_W      (32)
#define ZUKAN_INFO_BG_SCREEN_H      (24)
#define ZUKAN_INFO_BG_SCREEN_SIZE   ( ZUKAN_INFO_BG_SCREEN_W * ZUKAN_INFO_BG_SCREEN_H * GFL_BG_1SCRDATASIZ )
#define ZUKAN_INFO_BAR_SCREEN_X     (0)
#define ZUKAN_INFO_BAR_SCREEN_Y     (0)
#define ZUKAN_INFO_BAR_SCREEN_W     (32)
#define ZUKAN_INFO_BAR_SCREEN_H     (3)

#define ZUKAN_INFO_FPS (60)
#define ZUKAN_INFO_BAR_BLINK_COUNT_MAX ( ZUKAN_INFO_FPS / 2 )
#define ZUKAN_INFO_BACK_BG_SCREEN_SCROLL_X_VALUE (1)
#define ZUKAN_INFO_VBLANK_TCB_PRI (1)

#define ZUKAN_INFO_BG_UPDATE_BIT_RESET          (0)
#define ZUKAN_INFO_FORE_BG_UPDATE_BIT_BAR_BLINK (1<<0)
#define ZUKAN_INFO_BACK_BG_UPDATE_BIT_SCROLL    (1<<0)




#define TEST_MSG
//#define UI_TEST_TYPEICON
#define TEST_TYPEICON_TWICE
#define UI_TEST_POKE2D
#define TEST_POKEFOOT



//=============================================================================
/**
 *  �\���̐錾
 */
//=============================================================================
struct _ZUKAN_INFO_WORK
{
  u32 bg_pal_no;  ///< �g�p�J�n�p���b�g�ԍ�

  // fore = setumei, back = base
  GFL_ARCUTIL_TRANSINFO fore_bg_chara_info;  ///< fore��BG�L�����̈�
  GFL_ARCUTIL_TRANSINFO back_bg_chara_info;  ///< back��BG�L�����̈�
  u8                    fore_bg_frame;       ///< fore��BG�t���[��
  u8                    back_bg_frame;       ///< back��BG�t���[��
  u8                    fore_bg_update;  ///< fore��BG�X�V�r�b�g�t���O
  u8                    back_bg_update;  ///< back��BG�X�V�r�b�g�t���O
 
  GFL_TCB* vblank_tcb;  ///< VBlank����TCB

  u32 bar_blink_pal_no;  ///< �o�[�̃p���b�g�ԍ�
  u8  bar_blink_count;   ///< �o�[�̓_�ŃJ�E���g

  u8 padding[3];





#ifdef TEST_MSG  // prog/src/app/p_status/p_sta_local_def.h, p_sta_sys.c  // que��Proc�ɂ܂�����
  //MSG�n
  GFL_MSGDATA* msg_handle;
  GFL_FONT*    font_handle;
  GFL_BMPWIN*  bmpwin[ZUKAN_INFO_MSG_MAX];
  u8           msg_bg_frame;  ///< msg��BG�t���[��
  PRINT_QUE*   print_que;  ///< �|�C���^��ێ����Ă��邾���B�����ƍ폜�͑��̂Ƃ���ōs���ĉ������B
  HEAPID       heap_id;  ///< �l��ێ����Ă��邾���B�����ƍ폜�͑��̂Ƃ���ōs���ĉ������B
#endif //TEST_MSG




#ifdef UI_TEST_TYPEICON
	//���ށA�Z�A�C�R��
  UI_EASY_CLWK_RES      clres_type_icon;
  GFL_CLWK                  *clwk_type_icon;
#endif //UI_TEST_TYPEICON

#ifdef TEST_TYPEICON_TWICE
  u32       typeicon_cg_idx[2];
  u32       typeicon_cl_idx;
  u32       typeicon_cean_idx;
  GFL_CLWK* typeicon_clwk[2];
#endif //TEST_TYPEICON_TWICE

#ifdef UI_TEST_POKE2D
	u32												ncg_poke2d;
	u32												ncl_poke2d;
	u32												nce_poke2d;
	GFL_CLWK									*clwk_poke2d;
#endif //UI_TEST_POKE2D





#ifdef TEST_POKEFOOT
  UI_EASY_CLWK_RES clres_pokefoot;
  GFL_CLWK* clwk_pokefoot;
#endif //TEST_POKEFOOT




};

//=============================================================================
/**
 *  
 */
//=============================================================================
static void Zukan_Info_VBlankFunc( GFL_TCB* tcb, void* wk );
static void Zukan_Info_ChangeBarPal( ZUKAN_INFO_WORK* work );





#ifdef TEST_MSG  // PSTATUS_InitMessage
static void Zukan_Info_CreateMessage( ZUKAN_INFO_WORK* work, HEAPID a_heap_id, PRINT_QUE* a_print_que );
static void Zukan_Info_DeleteMessage( ZUKAN_INFO_WORK* work );
#endif

#ifdef UI_TEST_TYPEICON
//-------------------------------------
///	���ށA�Z�A�C�R��
//=====================================
static void UITemplate_TYPEICON_CreateCLWK( ZUKAN_INFO_WORK *wk, PokeType type, GFL_CLUNIT *unit, HEAPID heapID );
static void UITemplate_TYPEICON_DeleteCLWK( ZUKAN_INFO_WORK *wk );
#endif //UI_TEST_TYPEICON

#ifdef TEST_TYPEICON_TWICE
static void Zukan_Info_CreateTypeicon( ZUKAN_INFO_WORK* work, PokeType type1, PokeType type2, GFL_CLUNIT* clunit, HEAPID heap_id );
static void Zukan_Info_DeleteTypeicon( ZUKAN_INFO_WORK* work );
#endif //TEST_TYPEICON_TWICE

#ifdef UI_TEST_POKE2D
//-------------------------------------
///	�|�P����OBJ,BG�ǂ݂���
//=====================================
static void UITemplate_POKE2D_LoadResourceOBJ( ZUKAN_INFO_WORK *wk, HEAPID heapID );
static void UITemplate_POKE2D_UnLoadResourceOBJ( ZUKAN_INFO_WORK *wk );
static void UITemplate_POKE2D_CreateCLWK( ZUKAN_INFO_WORK *wk, GFL_CLUNIT *clunit, HEAPID heapID );
static void UITemplate_POKE2D_DeleteCLWK( ZUKAN_INFO_WORK *wk );
#endif //UI_TEST_POKE2D




#ifdef TEST_POKEFOOT
static void Zukan_Info_CreatePokefoot( ZUKAN_INFO_WORK* work, s32 monsno, GFL_CLUNIT* unit, HEAPID heap_id );
static void Zukan_Info_DeletePokefoot( ZUKAN_INFO_WORK* work );
#endif //TEST_POKEFOOT






//=============================================================================
/**
 *  �O�����J�֐���`
 */
//=============================================================================
//-------------------------------------
/// ������
//=====================================
ZUKAN_INFO_WORK* ZUKAN_INFO_Init( HEAPID heap_id, u32 a_bg_pal_no,
                                  u8 a_fore_bg_frame, u8 a_back_bg_frame,
                                  GFL_CLUNIT *clunit,
                                  u8 a_msg_bg_frame, PRINT_QUE* a_print_que )
{
  ZUKAN_INFO_WORK* work;

  // ���[�N�����A������
  {
    u32 size = sizeof(ZUKAN_INFO_WORK);
    work = GFL_HEAP_AllocMemory( heap_id, size );
    GFL_STD_MemClear( work, size );

    work->bg_pal_no = a_bg_pal_no;
    work->fore_bg_frame = a_fore_bg_frame;
    work->back_bg_frame = a_back_bg_frame;
    
    work->bar_blink_count = ZUKAN_INFO_BAR_BLINK_COUNT_MAX;
    work->bar_blink_pal_no = ZUKAN_INFO_FORE_BG_PAL_NO_D;

    work->fore_bg_update = ZUKAN_INFO_BG_UPDATE_BIT_RESET;
    work->back_bg_update = ZUKAN_INFO_BG_UPDATE_BIT_RESET;
  }

  // ���\�[�X�ǂݍ���
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, heap_id );
    PALTYPE bg_paltype = ( a_fore_bg_frame < GFL_BG_FRAME0_S ) ?
                         ( PALTYPE_MAIN_BG ) : ( PALTYPE_SUB_BG );  // a_fore_bg_frame����bg_paltype�����߂�
    
    GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_zukan_gra_info_zukan_bgu_NCLR, bg_paltype,
                                      work->bg_pal_no * ZUKAN_INFO_BG_PAL_LINE_SIZE, ZUKAN_INFO_BG_PAL_NUM * ZUKAN_INFO_BG_PAL_LINE_SIZE,
                                      heap_id );

    // fore
    work->fore_bg_chara_info = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( handle, NARC_zukan_gra_info_zukan_bgu_NCGR, work->fore_bg_frame,
                                                                            ZUKAN_INFO_BG_CHARA_SIZE, FALSE, heap_id );
    GF_ASSERT_MSG( work->fore_bg_chara_info != GFL_ARCUTIL_TRANSINFO_FAIL, "ZUKAN_INFO : BG�L�����̈悪����܂���ł����B\n" );
    
    GFL_ARCHDL_UTIL_TransVramScreen( handle, NARC_zukan_gra_info_setumei_bgu_NSCR, work->fore_bg_frame,
                                     GFL_ARCUTIL_TRANSINFO_GetPos( work->fore_bg_chara_info ), ZUKAN_INFO_BG_SCREEN_SIZE, FALSE, heap_id );
    //GFL_BG_ChangeScreenPalette( work->fore_bg_frame, 0, 0, ZUKAN_INFO_BG_SCREEN_W, ZUKAN_INFO_BG_SCREEN_H,
    //                            work->bg_pal_no + ZUKAN_INFO_FORE_BG_PAL_NO_D );

    //Zukan_Info_ChangeBarPal( work );

    // back
    work->back_bg_chara_info = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( handle, NARC_zukan_gra_info_zukan_bgu_NCGR, work->back_bg_frame,
                                                                            ZUKAN_INFO_BG_CHARA_SIZE, FALSE, heap_id );
    GF_ASSERT_MSG( work->back_bg_chara_info != GFL_ARCUTIL_TRANSINFO_FAIL, "ZUKAN_INFO : BG�L�����̈悪����܂���ł����B\n" );
        
    GFL_ARCHDL_UTIL_TransVramScreen( handle, NARC_zukan_gra_info_base_bgu_NSCR, work->back_bg_frame,
                                     GFL_ARCUTIL_TRANSINFO_GetPos( work->back_bg_chara_info ), ZUKAN_INFO_BG_SCREEN_SIZE, FALSE, heap_id );
    //GFL_BG_ChangeScreenPalette( work->back_bg_frame, 0, 0, ZUKAN_INFO_BG_SCREEN_W, ZUKAN_INFO_BG_SCREEN_H,
    //                            work->bg_pal_no + ZUKAN_INFO_BACK_BG_PAL_NO );
    
    GFL_ARC_CloseDataHandle( handle );
  }

  work->vblank_tcb = GFUser_VIntr_CreateTCB( Zukan_Info_VBlankFunc, work, ZUKAN_INFO_VBLANK_TCB_PRI );

  GFL_BG_LoadScreenReq( work->fore_bg_frame );
  GFL_BG_LoadScreenReq( work->back_bg_frame );





#ifdef TEST_MSG
  {
    work->msg_bg_frame = a_msg_bg_frame;

    GFL_BG_SetPriority( work->msg_bg_frame, 0 );  // �őO��
    GFL_BG_SetPriority( work->fore_bg_frame, 1 );
    GFL_BG_SetPriority( work->back_bg_frame, 2 );
    GFL_BG_SetPriority( 0, 3 );  // �c�����ʂ͍Ŕw�ʂɂ���

    Zukan_Info_CreateMessage( work, heap_id, a_print_que );
  }
#endif //TEST_MSG

#ifdef UI_TEST_TYPEICON
	//�����A�C�R���̓ǂݍ���
	{	
		UITemplate_TYPEICON_CreateCLWK( work, POKETYPE_KUSA, clunit, heap_id );
	}
#endif //UI_TEST_TYPEICON

#ifdef TEST_TYPEICON_TWICE
  Zukan_Info_CreateTypeicon( work, POKETYPE_KUSA, POKETYPE_KAKUTOU, clunit, heap_id ); 
#endif //TEST_TYPEICON_TWICE

#ifdef UI_TEST_POKE2D
	//�|�P����OBJ,BG�ǂ݂���
	{	
		UITemplate_POKE2D_LoadResourceOBJ( work, heap_id );
		UITemplate_POKE2D_CreateCLWK( work, clunit, heap_id );
	}
#endif //UI_TEST_POKE2D

#ifdef TEST_POKEFOOT
  Zukan_Info_CreatePokefoot( work, MONSNO_HUSIGIBANA, clunit, heap_id );
#endif //TEST_POKEFOOT




  return work;
}

//-------------------------------------
/// �j��
//=====================================
void ZUKAN_INFO_Exit( ZUKAN_INFO_WORK* work )
{




#ifdef TEST_POKEFOOT
  Zukan_Info_DeletePokefoot( work );
#endif //TEST_POKEFOOT

#ifdef UI_TEST_POKE2D
	UITemplate_POKE2D_DeleteCLWK( work );
	UITemplate_POKE2D_UnLoadResourceOBJ( work );
#endif //UI_TEST_POKE2D

#ifdef TEST_TYPEICON_TWICE
  Zukan_Info_DeleteTypeicon( work ); 
#endif //TEST_TYPEICON_TWICE

#ifdef UI_TEST_TYPEICON
	//�����A�C�R���̔j��
	UITemplate_TYPEICON_DeleteCLWK( work );
#endif //UI_TEST_TYPEICON

#ifdef TEST_MSG
  {
    Zukan_Info_DeleteMessage( work );
  }
#endif //TEST_MSG






  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �ǂݍ��񂾃��\�[�X�̔j��
  {
    // back
    GFL_BG_FreeCharacterArea( work->back_bg_frame, GFL_ARCUTIL_TRANSINFO_GetPos( work->back_bg_chara_info ),
                              GFL_ARCUTIL_TRANSINFO_GetSize( work->back_bg_chara_info ) );
      
    // fore
    GFL_BG_FreeCharacterArea( work->fore_bg_frame, GFL_ARCUTIL_TRANSINFO_GetPos( work->fore_bg_chara_info ),
                              GFL_ARCUTIL_TRANSINFO_GetSize( work->fore_bg_chara_info ) );
  }

  // ���[�N�j��
  {
    GFL_HEAP_FreeMemory( work );
  }
}

//-------------------------------------
/// ���C��
//=====================================
void ZUKAN_INFO_Main( ZUKAN_INFO_WORK* work )
{
  {
    work->bar_blink_count--;
    if( work->bar_blink_count == 0 )
    {
      work->bar_blink_pal_no = ( work->bar_blink_pal_no == ZUKAN_INFO_FORE_BG_PAL_NO_D ) ?
                               ZUKAN_INFO_FORE_BG_PAL_NO_L : ZUKAN_INFO_FORE_BG_PAL_NO_D;
      work->bar_blink_count = ZUKAN_INFO_BAR_BLINK_COUNT_MAX;
    
      work->fore_bg_update |= ZUKAN_INFO_FORE_BG_UPDATE_BIT_BAR_BLINK; 
    }
  }

  {
    work->back_bg_update |= ZUKAN_INFO_BACK_BG_UPDATE_BIT_SCROLL;
  }
}

//=============================================================================
/**
 *  
 */
//=============================================================================
static void Zukan_Info_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  ZUKAN_INFO_WORK* work = (ZUKAN_INFO_WORK*)wk;
  BOOL fore_bg_load = FALSE;

  if( work->fore_bg_update & ZUKAN_INFO_FORE_BG_UPDATE_BIT_BAR_BLINK )
  {
    //Zukan_Info_ChangeBarPal( work );
    fore_bg_load = TRUE;
  }

  if( fore_bg_load )
  {
    GFL_BG_LoadScreenReq( work->fore_bg_frame );
  }

  if( work->back_bg_update & ZUKAN_INFO_BACK_BG_UPDATE_BIT_SCROLL )
  {
    GFL_BG_SetScrollReq( work->back_bg_frame, GFL_BG_SCROLL_X_INC, ZUKAN_INFO_BACK_BG_SCREEN_SCROLL_X_VALUE );
  }

  work->fore_bg_update = ZUKAN_INFO_BG_UPDATE_BIT_RESET;
  work->back_bg_update = ZUKAN_INFO_BG_UPDATE_BIT_RESET;
}

static void Zukan_Info_ChangeBarPal( ZUKAN_INFO_WORK* work )
{
  GFL_BG_ChangeScreenPalette( work->fore_bg_frame,
                              ZUKAN_INFO_BAR_SCREEN_X, ZUKAN_INFO_BAR_SCREEN_X, ZUKAN_INFO_BAR_SCREEN_W, ZUKAN_INFO_BAR_SCREEN_H,
                              work->bg_pal_no + work->bar_blink_pal_no );
}




















#ifdef TEST_MSG  // PSTATUS_InitMessage


void PSTATUS_UTIL_DrawStrFuncBmp( ZUKAN_INFO_WORK *work , GFL_BMP_DATA *bmpData , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col );
void PSTATUS_UTIL_DrawStrFunc( ZUKAN_INFO_WORK *work , GFL_BMPWIN *bmpWin , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col );
void PSTATUS_UTIL_DrawStrFuncRight( ZUKAN_INFO_WORK *work , GFL_BMPWIN *bmpWin , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col );
void PSTATUS_UTIL_DrawValueStrFuncBmp( ZUKAN_INFO_WORK *work , GFL_BMP_DATA *bmpData , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col );
void PSTATUS_UTIL_DrawValueStrFunc( ZUKAN_INFO_WORK *work , GFL_BMPWIN *bmpWin , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col );
void PSTATUS_UTIL_DrawValueStrFuncRightBmp( ZUKAN_INFO_WORK *work , GFL_BMP_DATA *bmpData ,
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col );
void PSTATUS_UTIL_DrawValueStrFuncRight( ZUKAN_INFO_WORK *work , GFL_BMPWIN *bmpWin , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col );


//--------------------------------------------------------------
//	�����̕`��(bmp
//--------------------------------------------------------------
void PSTATUS_UTIL_DrawStrFuncBmp( ZUKAN_INFO_WORK *work , GFL_BMP_DATA *bmpData , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  srcStr = GFL_MSG_CreateString( work->msg_handle , strId ); 
  PRINTSYS_PrintQueColor( work->print_que , bmpData , 
          posX , posY , srcStr , work->font_handle , col );
  GFL_STR_DeleteBuffer( srcStr );
}

//--------------------------------------------------------------
//	�����̕`��
//--------------------------------------------------------------
void PSTATUS_UTIL_DrawStrFunc( ZUKAN_INFO_WORK *work , GFL_BMPWIN *bmpWin , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  PSTATUS_UTIL_DrawStrFuncBmp( work , GFL_BMPWIN_GetBmp( bmpWin ) , strId , posX , posY , col );
}

//--------------------------------------------------------------
//	�����̕`��(�E��
//--------------------------------------------------------------
void PSTATUS_UTIL_DrawStrFuncRight( ZUKAN_INFO_WORK *work , GFL_BMPWIN *bmpWin , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  u32 width;
  srcStr = GFL_MSG_CreateString( work->msg_handle , strId ); 
  width = PRINTSYS_GetStrWidth( srcStr , work->font_handle , 0 );
  PRINTSYS_PrintQueColor( work->print_que , GFL_BMPWIN_GetBmp( bmpWin ) , 
          posX-width , posY , srcStr , work->font_handle , col );
  GFL_STR_DeleteBuffer( srcStr );
}

//--------------------------------------------------------------
//	�����̕`��(�l�p bmp
//--------------------------------------------------------------
void PSTATUS_UTIL_DrawValueStrFuncBmp( ZUKAN_INFO_WORK *work , GFL_BMP_DATA *bmpData , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  STRBUF *dstStr = GFL_STR_CreateBuffer( 16, work->heap_id );

  srcStr = GFL_MSG_CreateString( work->msg_handle , strId ); 
  WORDSET_ExpandStr( wordSet , dstStr , srcStr );
  PRINTSYS_PrintQueColor( work->print_que , bmpData , 
          posX , posY , dstStr , work->font_handle , col );

  GFL_STR_DeleteBuffer( srcStr );
  GFL_STR_DeleteBuffer( dstStr );
}
//--------------------------------------------------------------
//	�����̕`��(�l�p
//--------------------------------------------------------------
void PSTATUS_UTIL_DrawValueStrFunc( ZUKAN_INFO_WORK *work , GFL_BMPWIN *bmpWin , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  PSTATUS_UTIL_DrawValueStrFuncBmp( work , GFL_BMPWIN_GetBmp( bmpWin ) , wordSet ,
                                    strId , posX , posY , col );
}

//--------------------------------------------------------------
//	�����̕`��(�l�p bmp
//--------------------------------------------------------------
void PSTATUS_UTIL_DrawValueStrFuncRightBmp( ZUKAN_INFO_WORK *work , GFL_BMP_DATA *bmpData ,
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  STRBUF *dstStr = GFL_STR_CreateBuffer( 32, work->heap_id );
  u32 width;

  srcStr = GFL_MSG_CreateString( work->msg_handle , strId ); 
  WORDSET_ExpandStr( wordSet , dstStr , srcStr );
  width = PRINTSYS_GetStrWidth( dstStr , work->font_handle , 0 );
  PRINTSYS_PrintQueColor( work->print_que , bmpData , 
          posX-width , posY , dstStr , work->font_handle , col );

  GFL_STR_DeleteBuffer( srcStr );
  GFL_STR_DeleteBuffer( dstStr );
}

//--------------------------------------------------------------
//	�����̕`��(�l�p
//--------------------------------------------------------------
void PSTATUS_UTIL_DrawValueStrFuncRight( ZUKAN_INFO_WORK *work , GFL_BMPWIN *bmpWin , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  PSTATUS_UTIL_DrawValueStrFuncRightBmp( work , GFL_BMPWIN_GetBmp( bmpWin ) , wordSet ,
                                         strId , posX , posY , col );
}

static void Zukan_Info_CreateMessage( ZUKAN_INFO_WORK* work, HEAPID a_heap_id, PRINT_QUE* a_print_que )
{
  work->font_handle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, a_heap_id );
  work->msg_handle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_pokestatus_dat, a_heap_id );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                 PSTATUS_BG_PLT_FONT * ZUKAN_INFO_BG_PAL_LINE_SIZE, 1 * ZUKAN_INFO_BG_PAL_LINE_SIZE, a_heap_id );

  {
    s32 i;

    // �L�����N�^�[�P�ʂ�X���W�AY���W�AX�T�C�Y�AY�T�C�Y���w�肷��
    const u8 pos_siz[ZUKAN_INFO_MSG_MAX][4] =
    {
      {  0,  0, 32,  3 },
      { 16,  4, 16,  2 },
      { 16,  6, 16,  2 },
      { 17, 12, 15,  2 },
      { 17, 14, 15,  2 },
      {  0, 16, 32,  8 },
    };

    work->print_que = a_print_que;
    work->heap_id = a_heap_id;

    for( i=0; i<ZUKAN_INFO_MSG_MAX; i++ )
    {
      // bmpwin�̃r�b�g�}�b�v�𓧖��F�œh��ׂ��Ă������ق��������H
      work->bmpwin[i] = GFL_BMPWIN_Create( work->msg_bg_frame,
                                           pos_siz[i][0], pos_siz[i][1], pos_siz[i][2], pos_siz[i][3],
                                           PSTATUS_BG_PLT_FONT, GFL_BMP_CHRAREA_GET_B );
    }

    PSTATUS_UTIL_DrawStrFunc( work, work->bmpwin[ZUKAN_INFO_MSG_TOROKU], mes_status_02_02,
                              32, 4, PRINTSYS_LSB_Make(0xF,2,0) );

    {
      WORDSET* wordset = WORDSET_Create( work->heap_id );  // WORDSET_RegisterPokeMonsName, WORDSET_RegisterPokeMonsNamePPP
      u32 monno = MONSNO_HUSIGIBANA;  // PPP_Get( ppp, ID_PARA_monno, NULL );  // POKEMON_PASO_PARAM* ppp
      WORDSET_RegisterNumber( wordset, 0, monno, 3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
      PSTATUS_UTIL_DrawValueStrFunc( work, work->bmpwin[ZUKAN_INFO_MSG_NAME], wordset, mes_status_02_03,
                                     4, 4, PRINTSYS_LSB_Make(1,2,0) );
      WORDSET_Delete( wordset );
    }
    {
      STRBUF* str_src;
      u32 monno = MONSNO_HUSIGIBANA;  // PPP_Get( ppp, ID_PARA_monno, NULL );
      str_src = GFL_MSG_CreateString( GlobalMsg_PokeName, monno );
      PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp( work->bmpwin[ZUKAN_INFO_MSG_NAME] ),
                              44, 4, str_src, work->font_handle, PRINTSYS_LSB_Make(1,2,0) );
      GFL_STR_DeleteBuffer( str_src );
    }

    PSTATUS_UTIL_DrawStrFunc( work, work->bmpwin[ZUKAN_INFO_MSG_KIND], mes_status_02_13,  // zkn_type.gmm
                              4, 4, PRINTSYS_LSB_Make(1,2,0) );
   
    PSTATUS_UTIL_DrawStrFunc( work, work->bmpwin[ZUKAN_INFO_MSG_HEIGHT], mes_status_02_04,
                              8, 4, PRINTSYS_LSB_Make(1,2,0) );
    {
      WORDSET* wordset = WORDSET_Create( work->heap_id );
      WORDSET_RegisterNumber( wordset, 0, 43210, 5, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
      PSTATUS_UTIL_DrawValueStrFunc( work, work->bmpwin[ZUKAN_INFO_MSG_HEIGHT], wordset, mes_status_02_10,
                                     44, 4, PRINTSYS_LSB_Make(1,2,0) );
      WORDSET_Delete( wordset );
    }
    PSTATUS_UTIL_DrawStrFunc( work, work->bmpwin[ZUKAN_INFO_MSG_HEIGHT], mes_status_01_02,
                              80, 4, PRINTSYS_LSB_Make(1,2,0) );

    PSTATUS_UTIL_DrawStrFunc( work, work->bmpwin[ZUKAN_INFO_MSG_WEIGHT], mes_status_02_04,
                              8, 4, PRINTSYS_LSB_Make(1,2,0) );
    {
      WORDSET* wordset = WORDSET_Create( work->heap_id );
      WORDSET_RegisterNumber( wordset, 0, 123, 5, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
      PSTATUS_UTIL_DrawValueStrFunc( work, work->bmpwin[ZUKAN_INFO_MSG_WEIGHT], wordset, mes_status_02_10,
                                     44, 4, PRINTSYS_LSB_Make(1,2,0) );
      WORDSET_Delete( wordset );
    }
    PSTATUS_UTIL_DrawStrFunc( work, work->bmpwin[ZUKAN_INFO_MSG_WEIGHT], mes_status_01_07,
                              80, 4, PRINTSYS_LSB_Make(1,2,0) );
 
    PSTATUS_UTIL_DrawStrFunc( work, work->bmpwin[ZUKAN_INFO_MSG_EXPLAIN], mes_status_06_30,
                              28, 12, PRINTSYS_LSB_Make(1,2,0) );

    for( i=0; i<ZUKAN_INFO_MSG_MAX; i++ )
    {
      GFL_BMPWIN_MakeTransWindow_VBlank( work->bmpwin[i] );
    }
  }
}

static void Zukan_Info_DeleteMessage( ZUKAN_INFO_WORK* work )
{
  s32 i;
  for( i=0; i<ZUKAN_INFO_MSG_MAX; i++ )
  {
    GFL_BMPWIN_Delete( work->bmpwin[i] );
  }
  GFL_MSG_Delete( work->msg_handle );
  GFL_FONT_Delete( work->font_handle );
}
#endif //TEST_MSG







#ifdef UI_TEST_TYPEICON
//=============================================================================
/**
 *	TYPEICON
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�����A�^�C�v�A�C�R���쐬
 *
 *	@param	UI_TEST_MAIN_WORK *wk ���[�N
 *	@param	PokeType					�^�C�v
 *	@param	GFL_CLUNIT *unit	CLUNIT  ���j�b�g
 *	@param	heapID						�q�[�vID
 *
 *	@return	void
 */
//-----------------------------------------------------------------------------
static void UITemplate_TYPEICON_CreateCLWK( ZUKAN_INFO_WORK *wk, PokeType type, GFL_CLUNIT *unit, HEAPID heapID )
{	
  UI_EASY_CLWK_RES_PARAM prm;

  prm.draw_type = CLSYS_DRAW_MAIN;
  prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NONE;
  prm.arc_id    = APP_COMMON_GetArcId();
  prm.pltt_id   = APP_COMMON_GetPokeTypePltArcIdx();
  prm.ncg_id    = APP_COMMON_GetPokeTypeCharArcIdx(type);
  prm.cell_id   = APP_COMMON_GetPokeTypeCellArcIdx( APP_COMMON_MAPPING_128K );
  prm.anm_id    = APP_COMMON_GetPokeTypeAnimeArcIdx( APP_COMMON_MAPPING_128K );
  prm.pltt_line = PLTID_OBJ_TYPEICON_M;
  prm.pltt_src_ofs  = 0;
  prm.pltt_src_num = 3;
  
  UI_EASY_CLWK_LoadResource( &wk->clres_type_icon, &prm, unit, heapID );

  wk->clwk_type_icon = UI_EASY_CLWK_CreateCLWK( &wk->clres_type_icon, unit, 8*20, 8*10, 0, heapID );//8*24, 8*10
		
  GFL_CLACT_WK_SetPlttOffs( wk->clwk_type_icon, APP_COMMON_GetPokeTypePltOffset(type),
				CLWK_PLTTOFFS_MODE_PLTT_TOP );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����A�^�C�v�A�C�R���j��
 *
 *	@param	UI_TEST_MAIN_WORK *wk ���[�N
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void UITemplate_TYPEICON_DeleteCLWK( ZUKAN_INFO_WORK *wk )
{	
	//CLWK�j��
	GFL_CLACT_WK_Remove( wk->clwk_type_icon );
	//���\�[�X�j��
  UI_EASY_CLWK_UnLoadResource( &wk->clres_type_icon );
}

#endif //UI_TEST_TYPEICON








#ifdef TEST_TYPEICON_TWICE
static void Zukan_Info_CreateTypeicon( ZUKAN_INFO_WORK* work, PokeType type1, PokeType type2, GFL_CLUNIT* clunit, HEAPID heap_id )
{
  s32 i;
  PokeType type[2];
  GFL_CLWK_DATA data[2] =
  {
    { 8*20, 8*10, 0, 0, 0 },
    { 8*24, 8*10, 0, 0, 0 },
  };

  type[0] = type1;
  type[1] = type2;

  // ���\�[�X�ǂݍ���
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heap_id );

    for( i=0; i<2; i++ )
    {
      work->typeicon_cg_idx[i] = GFL_CLGRP_REGISTER_FAILED;
      if( type[i] == POKETYPE_NULL )
      {
        continue;
      }
      work->typeicon_cg_idx[i] = GFL_CLGRP_CGR_Register( handle,
                                                         APP_COMMON_GetPokeTypeCharArcIdx(type[i]),
                                                         FALSE, CLSYS_DRAW_MAIN, heap_id );
    }

    work->typeicon_cl_idx = GFL_CLGRP_PLTT_RegisterEx( handle,
                                                       APP_COMMON_GetPokeTypePltArcIdx(),
                                                       CLSYS_DRAW_MAIN,
                                                       PLTID_OBJ_TYPEICON_M * 0x20,
                                                       0, 3, heap_id );

    work->typeicon_cean_idx = GFL_CLGRP_CELLANIM_Register( handle,
                                                           APP_COMMON_GetPokeTypeCellArcIdx( APP_COMMON_MAPPING_128K ),
                                                           APP_COMMON_GetPokeTypeAnimeArcIdx( APP_COMMON_MAPPING_128K ),
                                                           heap_id );
  
    GFL_ARC_CloseDataHandle( handle );
  }

  // CLWK�쐬
  {
    for( i=0; i<2; i++ )
    {
      if( work->typeicon_cg_idx[i] == GFL_CLGRP_REGISTER_FAILED )
      {
        continue;
      }
      work->typeicon_clwk[i] = GFL_CLACT_WK_Create( clunit,
                                                    work->typeicon_cg_idx[i],
                                                    work->typeicon_cl_idx,
                                                    work->typeicon_cean_idx,
                                                    &(data[i]),
                                                    CLSYS_DEFREND_MAIN, heap_id );
      GFL_CLACT_WK_SetPlttOffs( work->typeicon_clwk[i], APP_COMMON_GetPokeTypePltOffset(type[i]),
                                CLWK_PLTTOFFS_MODE_PLTT_TOP );
    }
  }
}
static void Zukan_Info_DeleteTypeicon( ZUKAN_INFO_WORK* work )
{
  s32 i=0;

  // CLWK�j��
  for( i=0; i<2; i++ )
  {
    if( work->typeicon_cg_idx[i] == GFL_CLGRP_REGISTER_FAILED )
    {
      continue;
    }
    GFL_CLACT_WK_Remove( work->typeicon_clwk[i] );
  }

  // ���\�[�X�j��
  GFL_CLGRP_PLTT_Release( work->typeicon_cl_idx );
  for( i=0; i<2; i++ )
  {
    if( work->typeicon_cg_idx[i] == GFL_CLGRP_REGISTER_FAILED )
    {
      continue;
    }
    GFL_CLGRP_CGR_Release( work->typeicon_cg_idx[i] );
  }
  GFL_CLGRP_CELLANIM_Release( work->typeicon_cean_idx );
}
#endif //TEST_TYPEICON_TWICE








#ifdef UI_TEST_POKE2D
//=============================================================================
/**
 *	�|�P����OBJ,BG�ǂ݂���
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�ǂ݂���
 *
 *	@param	UI_TEST_MAIN_WORK *wk	���[�N
 *	@param	heapID										�q�[�vID
 */
//-----------------------------------------------------------------------------
static void UITemplate_POKE2D_LoadResourceOBJ( ZUKAN_INFO_WORK *wk, HEAPID heapID )
{	
	POKEMON_PASO_PARAM	*ppp;
	ARCHANDLE						*handle;

	//PPP�쐬
	ppp	= (POKEMON_PASO_PARAM	*)PP_Create( MONSNO_HUSIGIBANA, 0, 0, heapID );

	//�n���h��
	handle	= POKE2DGRA_OpenHandle( heapID );
	//���\�[�X�ǂ݂���
	wk->ncg_poke2d	= POKE2DGRA_OBJ_CGR_RegisterPPP( handle, ppp, POKEGRA_DIR_FRONT, CLSYS_DRAW_MAIN, heapID );
	wk->ncl_poke2d	= POKE2DGRA_OBJ_PLTT_RegisterPPP( handle, ppp, POKEGRA_DIR_FRONT ,CLSYS_DRAW_MAIN,  PLTID_OBJ_POKE_M*0x20,  heapID );
	wk->nce_poke2d	= POKE2DGRA_OBJ_CELLANM_RegisterPPP( ppp, POKEGRA_DIR_FRONT, APP_COMMON_MAPPING_128K, CLSYS_DRAW_MAIN, heapID );
	GFL_ARC_CloseDataHandle( handle );

	//PP�j��
	GFL_HEAP_FreeMemory( ppp );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�j��
 *
 *	@param	UI_TEST_MAIN_WORK *wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void UITemplate_POKE2D_UnLoadResourceOBJ( ZUKAN_INFO_WORK *wk )
{	
	//���\�[�X�j��
	GFL_CLGRP_PLTT_Release( wk->ncl_poke2d );
	GFL_CLGRP_CGR_Release( wk->ncg_poke2d );
	GFL_CLGRP_CELLANIM_Release( wk->nce_poke2d );

}
//----------------------------------------------------------------------------
/**
 *	@brief	CLWK�쐬
 *
 *	@param	UI_TEST_MAIN_WORK *wk	���[�N
 *	@param	*clunit	���j�b�g
 *	@param	heapID	�q�[�vID
 */
//-----------------------------------------------------------------------------
static void UITemplate_POKE2D_CreateCLWK( ZUKAN_INFO_WORK *wk, GFL_CLUNIT *clunit, HEAPID heapID )
{	
	GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
		cldata.pos_x	= 8*6;
		cldata.pos_y	= 8*(3+6);
		wk->clwk_poke2d	= GFL_CLACT_WK_Create( clunit, 
				wk->ncg_poke2d,
				wk->ncl_poke2d,
				wk->nce_poke2d,
				&cldata, 
				CLSYS_DEFREND_MAIN, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	CLWK�j��
 *
 *	@param	UI_TEST_MAIN_WORK *wk ���[�N
 */
//-----------------------------------------------------------------------------
static void UITemplate_POKE2D_DeleteCLWK( ZUKAN_INFO_WORK *wk )
{	
	GFL_CLACT_WK_Remove( wk->clwk_poke2d );
}
#endif //UI_TEST_POKE2D




/*
�}�b�s���O���[�h��1D���[�h(128K)�Ƃ��Ă���̂����A
CGR charData�̃}�b�s���O���[�h�����W�X�^�̒l�ɏ��������܂����Bregister[0x200010] resource=[0x10]
CGR charData�̃}�b�s���O���[�h�����W�X�^�̒l�ɏ��������܂����Bregister[0x200010] resource=[0x0]
�ƕ\�������B����͕\������邾���ŁAASSERT�Ƃ����킯�ł͂Ȃ��̂ŋC�ɂ��Ȃ��Ă������ȁB
���Ȃ݂ɂ����\�����Ă���̂́A�^�C�v�A�C�R���ƃ|�P����2D�ł���A�|�P�������Ղ͊֌W�Ȃ��B
*/
#ifdef TEST_POKEFOOT
static void Zukan_Info_CreatePokefoot( ZUKAN_INFO_WORK* work, s32 monsno, GFL_CLUNIT* unit, HEAPID heap_id )
{
  UI_EASY_CLWK_RES_PARAM prm;

  prm.draw_type = CLSYS_DRAW_MAIN;
  prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NCGR;  // NCLR�񈳏k�ANCGR���k�ANCER���k�ANANR���k�Ȃ̂ŁANCGR�̈��k�ɂ����Ή����Ă��Ȃ����̃t���O�ł͂��܂������Ȃ� �� NCLR�񈳏k�ANCGR���k�ANCER�񈳏k�ANANR�����k�ɕύX����
  prm.arc_id    = PokeFootArcFileGet();
  prm.pltt_id   = PokeFootPlttDataIdxGet();
  prm.ncg_id    = PokeFootCharDataIdxGet(monsno);
  prm.cell_id   = PokeFootCellDataIdxGet();
  prm.anm_id    = PokeFootCellAnmDataIdxGet();
  prm.pltt_line = PLTID_OBJ_POKEFOOT_M;
  prm.pltt_src_ofs = 0;
  prm.pltt_src_num = 1;
  
  UI_EASY_CLWK_LoadResource( &work->clres_pokefoot, &prm, unit, heap_id );

  work->clwk_pokefoot = UI_EASY_CLWK_CreateCLWK( &work->clres_pokefoot, unit, 8*15, 8*11, 0, heap_id );
}
static void Zukan_Info_DeletePokefoot( ZUKAN_INFO_WORK* work )
{	
	//CLWK�j��
	GFL_CLACT_WK_Remove( work->clwk_pokefoot );
	//���\�[�X�j��
  UI_EASY_CLWK_UnLoadResource( &work->clres_pokefoot );
}
#endif //TEST_POKEFOOT

