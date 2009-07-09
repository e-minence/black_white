//============================================================================================
/**
 * @file	btlv_input.c
 * @brief	�퓬�����
 * @author	soga
 * @date	2009.06.29
 */
//============================================================================================
#include <gflib.h>

#include "btlv_effect.h"
#include "print/printsys.h"
#include "system/palanm.h"
#include "system/bmp_oam.h"
#include "infowin/infowin.h"

#include "btlv_input.h"
#include "data/btlv_input.cdat"

#include "arc_def.h"
#include "battle/battgra_wb.naix"

//============================================================================================
/**
 *	�萔�錾
 */
//============================================================================================

#define BTLV_INPUT_TCB_MAX ( 8 )    //�g�p����TCB��MAX
#define BG_CLEAR_CODE     (0x8000 / 0x20 - 1)   ///BG�X�N���[���̃N���A�R�[�h�i�L�����N�^�̈�Ԍ����w��j

#define INFOWIN_PAL_NO  ( 0x0f )  //���X�e�[�^�X�o�[�p���b�g

//TCB_TransformStandby2Command
#define TTS2C_SCROLL_COUNT ( 8 )
#define TTS2C_START_SCALE ( FX32_ONE * 3 )
#define TTS2C_END_SCALE ( FX32_ONE )
#define TTS2C_START_SCROLL_X ( 0 )
#define TTS2C_START_SCROLL_Y ( 0x1c0 )
#define TTS2C_SCALE_SPEED ( ( FX32_ONE * 2 ) / TTS2C_SCROLL_COUNT )
#define TTS2C_SCROLL_SPEED ( 64 / TTS2C_SCROLL_COUNT )
#define TTS2C_FRAME1_SCROLL_X ( 0 )
#define TTS2C_FRAME1_SCROLL_Y ( 0 )

//TCB_TransformCommand2Waza
#define TTC2W_SCROLL_COUNT ( 8 )
#define TTC2W_START_SCROLL_X ( 256 )
#define TTC2W_START_SCROLL_Y ( 0x1c0 )
#define TTC2W_SCROLL_SPEED ( 64 / TTC2W_SCROLL_COUNT )

//TCB_TransformWaza2Command
#define TTW2C_SCROLL_COUNT ( 8 )
#define TTW2C_START_SCROLL_X ( 0 )
#define TTW2C_START_SCROLL_Y ( 0x1c0 )
#define TTW2C_SCROLL_SPEED ( 64 / TTW2C_SCROLL_COUNT )

//TCB_TransformCommand2Standby
#define TTC2S_SCALE_COUNT ( 8 )
#define TTC2S_SCALE_SPEED ( ( FX32_ONE * 2 ) / TTC2S_SCALE_COUNT )
#define TTC2S_START_SCALE ( FX32_ONE )
#define TTC2S_END_SCALE   ( FX32_ONE * 3 )

//TCB_SCREEN_ANIME�p�̃X�N���[���l
enum{ 
  TSA_SCROLL_X = 0,
  TSA_SCROLL_Y,

  TSA_SCROLL_X0 = 0,
  TSA_SCROLL_Y0 = 0,
  TSA_SCROLL_X1 = 256,
  TSA_SCROLL_Y1 = 0,
  TSA_SCROLL_X2 = 0,
  TSA_SCROLL_Y2 = 192,
  TSA_SCROLL_X3 = 256,
  TSA_SCROLL_Y3 = 192,

  TSA_WAIT = 2,
};

typedef enum
{ 
  SCREEN_ANIME_DIR_FORWARD = 0,
  SCREEN_ANIME_DIR_BACKWARD,
}SCREEN_ANIME_DIR;

//TCB_BUTTON�`�n��`
enum{ 
  BUTTON_ANIME_MAX = 6,     //3��3���̍U���ΏۑI�����}�b�N�X�̂͂�

  BUTTON2_APPEAR_ANIME = 8,
  BUTTON2_VANISH_ANIME = 9,

  BUTTON4_APPEAR_ANIME = 8,
  BUTTON4_VANISH_ANIME = 9,

  BUTTON6_APPEAR_ANIME = 8,
  BUTTON6_VANISH_ANIME = 9,

  //BUTTON_TYPE_YES_NO
  BUTTON2_X1 = 64,
  BUTTON2_Y1 = 56,
  BUTTON2_X2 = 128 + 64,
  BUTTON2_Y2 = 56,

  //BUTTON_TYPE_WAZA & DIR_4
  BUTTON4_X1 = 64,
  BUTTON4_Y1 = 56,
  BUTTON4_X2 = 128 + 64,
  BUTTON4_Y2 = 56,
  BUTTON4_X3 = 64,
  BUTTON4_Y3 = 64 + 40,
  BUTTON4_X4 = 128 + 64,
  BUTTON4_Y4 = 64 + 40,

  //BUTTON_TYPE_DIR_6
  BUTTON6_X1 = 64,
  BUTTON6_Y1 = 56,
  BUTTON6_X2 = 128 + 64,
  BUTTON6_Y2 = 56,
  BUTTON6_X3 = 64,
  BUTTON6_Y3 = 64 + 40,
  BUTTON6_X4 = 128 + 64,
  BUTTON6_Y4 = 64 + 40,
  BUTTON6_X5 = 64,
  BUTTON6_Y5 = 64 + 40,
  BUTTON6_X6 = 128 + 64,
  BUTTON6_Y6 = 64 + 40,
};

typedef enum
{ 
  BUTTON_TYPE_WAZA = 0,
  BUTTON_TYPE_DIR_4 = 0,
  BUTTON_TYPE_DIR_6 = 1,
  BUTTON_TYPE_YES_NO = 2,
  BUTTON_TYPE_MAX = 3,
}BUTTON_TYPE;

typedef enum
{ 
  BUTTON_ANIME_TYPE_APPEAR = 0,
  BUTTON_ANIME_TYPE_VANISH,
  BUTTON_ANIME_TYPE_MAX,
}BUTTON_ANIME_TYPE;

typedef struct
{ 
  const GFL_CLACTPOS  pos[ BUTTON_ANIME_MAX ];          //���W
  int                 anm_no[ BUTTON_ANIME_TYPE_MAX ];  //�A�j���[�V�����i���o�[
}BUTTON_ANIME_PARAM;

//============================================================================================
/**
 *	�\���̐錾
 */
//============================================================================================

struct _BTLV_INPUT_WORK
{
  GFL_TCBSYS*         tcbsys;
  void*               tcbwork;
  ARCHANDLE*          handle;
  BTLV_INPUT_TYPE     type;
  BTLV_INPUT_SCRTYPE  scr_type;
  u32                 tcb_execute_flag  :1;
  u32                 tcb_execute_count :3;
  u32                                   :28;

  //OBJ���\�[�X
  u32                 objcharID;
  u32                 objplttID;
  u32                 objcellID;

  //�Z�^�C�v�A�C�R��OBJ
  GFL_CLUNIT*         wazatype_clunit;
  GFL_CLWK*           wazatype_wk[ PTL_WAZA_MAX ];

  //�t�H���g
  GFL_FONT*           font;

	HEAPID              heapID;
};

typedef struct
{ 
  BTLV_INPUT_WORK*  biw;
  int               seq_no;
}TCB_TRANSFORM_WORK;

typedef struct
{ 
  BTLV_INPUT_WORK*  biw;
  fx32              start_scale;
  fx32              end_scale;
  fx32              scale_speed;
}TCB_SCALE_UP;

typedef struct
{ 
  BTLV_INPUT_WORK*  biw;
  int               scroll_y;
  int               scroll_speed;
  int               scroll_count;
}TCB_SCROLL_UP;

typedef struct
{ 
  BTLV_INPUT_WORK*  biw;
  int               count;
  SCREEN_ANIME_DIR  dir;
  int               wait;
}TCB_SCREEN_ANIME;

typedef struct
{ 
  BTLV_INPUT_WORK*  biw;
  GFL_CLUNIT*       clunit;
  GFL_CLWK*         clwk[ BUTTON_ANIME_MAX ];
}TCB_BUTTON_ANIME;

//============================================================================================
/**
 *	�v���g�^�C�v�錾
 */
//============================================================================================
static  void  BTLV_INPUT_LoadResource( BTLV_INPUT_WORK* biw );
static  void  FontLenGet( const STRBUF *str, GFL_FONT *font, int *ret_dot_len, int *ret_char_len );
static  void  TCB_TransformStandby2Command( GFL_TCB* tcb, void* work );
static  void  TCB_TransformCommand2Waza( GFL_TCB* tcb, void* work );
static  void  TCB_TransformWaza2Command( GFL_TCB* tcb, void* work );
static  void  TCB_TransformWaza2Dir( GFL_TCB* tcb, void* work );
static  void  TCB_TransformCommand2Standby( GFL_TCB* tcb, void* work );
static  void  TCB_TransformWaza2Standby( GFL_TCB* tcb, void* work );

static  void  SetupScaleChange( BTLV_INPUT_WORK* biw, fx32 start_scale, fx32 end_scale, fx32 scale_speed );
static  void  TCB_ScaleChange( GFL_TCB* tcb, void* work );
static  void  SetupScrollUp( BTLV_INPUT_WORK* biw, int scroll_x, int scroll_y, int scroll_speed, int scroll_count );
static  void  TCB_ScrollUp( GFL_TCB* tcb, void* work );
static  void  SetupScreenAnime( BTLV_INPUT_WORK* biw, int index, SCREEN_ANIME_DIR dir );
static  void  TCB_ScreenAnime( GFL_TCB* tcb, void* work );
static  void  SetupButtonAnime( BTLV_INPUT_WORK* biw, BUTTON_TYPE type, BUTTON_ANIME_TYPE anm_type );
static  void  TCB_ButtonAnime( GFL_TCB* tcb, void* work );

//============================================================================================
/**
 *  @brief  �V�X�e��������
 *
 *  @param[in]  type    �C���^�[�t�F�[�X�^�C�v
 *  @param[in]  font    �g�p����t�H���g
 *  @param[in]  heapID  �q�[�vID
 *
 *  @retval �V�X�e���Ǘ��\���̂̃|�C���^
 */
//============================================================================================
BTLV_INPUT_WORK*  BTLV_INPUT_Init( BTLV_INPUT_TYPE type, GFL_FONT* font, HEAPID heapID )
{
	BTLV_INPUT_WORK *biw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_INPUT_WORK ) );

	biw->heapID = heapID;

  biw->handle   = GFL_ARC_OpenDataHandle( ARCID_BATTGRA, biw->heapID );
  biw->tcbwork  = GFL_HEAP_AllocClearMemory( biw->heapID, GFL_TCB_CalcSystemWorkSize( BTLV_INPUT_TCB_MAX ) );
  biw->tcbsys   = GFL_TCB_Init( BTLV_INPUT_TCB_MAX, biw->tcbwork );

  biw->font = font;

  biw->wazatype_clunit = GFL_CLACT_UNIT_Create( PTL_WAZA_MAX, 0, biw->heapID );

  BTLV_INPUT_SetFrame();
  BTLV_INPUT_LoadResource( biw );
  BTLV_INPUT_CreateScreen( biw, BTLV_INPUT_SCRTYPE_STANDBY );

  //���X�e�[�^�X�o�[������
  INFOWIN_Init( GFL_BG_FRAME2_S, INFOWIN_PAL_NO, NULL, biw->heapID );

	return biw;
}

//============================================================================================
/**
 *  @brief  �V�X�e���I��
 *
 *  @param[in]  biw �V�X�e���Ǘ��\���̂̃|�C���^
 */
//============================================================================================
void	BTLV_INPUT_Exit( BTLV_INPUT_WORK* biw )
{
  GFL_CLGRP_CGR_Release( biw->objcharID );
  GFL_CLGRP_CELLANIM_Release( biw->objcellID );
  GFL_CLGRP_PLTT_Release( biw->objplttID );
  GFL_TCB_Exit( biw->tcbsys );
  GFL_HEAP_FreeMemory( biw->tcbwork );
  GFL_ARC_CloseDataHandle( biw->handle );
	GFL_HEAP_FreeMemory( biw );
}

//============================================================================================
/**
 *  @brief  �V�X�e�����C�����[�v
 *
 *  @param[in]  biw �V�X�e���Ǘ��\���̂̃|�C���^
 */
//============================================================================================
void	BTLV_INPUT_Main( BTLV_INPUT_WORK* biw )
{
  GFL_TCB_Main( biw->tcbsys );
  INFOWIN_Update();
}

//============================================================================================
/**
 *	@brief  �����BG�t���[���ݒ�
 */
//============================================================================================
void BTLV_INPUT_SetFrame( void )
{ 
  int i;

  for( i = 0 ; i < NELEMS( bibf ) ; i++ )
  {
    GFL_BG_SetBGControl( GFL_BG_FRAME0_S + i, &bibf[ i ].bgcnt_table, bibf[ i ].bgcnt_mode );
    GFL_BG_ClearScreenCode( GFL_BG_FRAME0_S + i, BG_CLEAR_CODE );
    GFL_BG_SetScroll( GFL_BG_FRAME0_S + i, GFL_BG_SCROLL_X_SET, 0 );
    GFL_BG_SetScroll( GFL_BG_FRAME0_S + i, GFL_BG_SCROLL_Y_SET, 0 );
  }
}

//============================================================================================
/**
 *	@brief  �����BG�t���[���ݒ���
 */
//============================================================================================
void BTLV_INPUT_FreeFrame( void )
{
  int i;

  for( i = 0 ; i < NELEMS( bibf ) ; i++ ){
    GFL_BG_SetVisible( GFL_BG_FRAME0_S + i, VISIBLE_OFF );
    GFL_BG_FreeBGControl( GFL_BG_FRAME0_S + i );
  }
}

//============================================================================================
/**
 *	@brief  ����ʐ���
 *
 *  @param[in]  biw   �V�X�e���Ǘ��\���̂̃|�C���^
 *	@param[in]  type  ��������X�N���[���^�C�v
 */
//============================================================================================
void BTLV_INPUT_CreateScreen( BTLV_INPUT_WORK* biw, BTLV_INPUT_SCRTYPE type )
{
  switch( type ){ 
  case BTLV_INPUT_SCRTYPE_STANDBY:
    if( biw->scr_type == BTLV_INPUT_SCRTYPE_STANDBY )
    { 
      MtxFx22 mtx;

      MTX_Scale22( &mtx, FX32_ONE * 3, FX32_ONE * 3 );
      GFL_BG_SetAffineScroll( GFL_BG_FRAME3_S, GFL_BG_SCROLL_X_SET, 128, &mtx, 256, 256 );
      GFL_BG_SetAffineScroll( GFL_BG_FRAME3_S, GFL_BG_SCROLL_Y_SET, 128 + 40, &mtx, 256, 256 );

      GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
      GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
      GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_ON );
      GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_ON );
    }
    else
    { 
      TCB_TRANSFORM_WORK* ttw = GFL_HEAP_AllocClearMemory( biw->heapID, sizeof( TCB_TRANSFORM_WORK ) );
      biw->tcb_execute_flag = 1;
      ttw->biw = biw;

      if( biw->scr_type == BTLV_INPUT_SCRTYPE_COMMAND )
      { 
        GFL_TCB_AddTask( biw->tcbsys, TCB_TransformCommand2Standby, ttw, 1 );
      }
      else
      {
        GFL_TCB_AddTask( biw->tcbsys, TCB_TransformWaza2Standby, ttw, 1 );
      }
    }
    break;
  case BTLV_INPUT_SCRTYPE_COMMAND:
    { 
      TCB_TRANSFORM_WORK* ttw = GFL_HEAP_AllocClearMemory( biw->heapID, sizeof( TCB_TRANSFORM_WORK ) );
      biw->tcb_execute_flag = 1;
      ttw->biw = biw;

      if( biw->scr_type == BTLV_INPUT_SCRTYPE_WAZA )
      { 
        GFL_TCB_AddTask( biw->tcbsys, TCB_TransformWaza2Command, ttw, 1 );
      }
      else
      {
        GFL_TCB_AddTask( biw->tcbsys, TCB_TransformStandby2Command, ttw, 1 );
      }
    }
    break;
  case BTLV_INPUT_SCRTYPE_WAZA:
    { 
      TCB_TRANSFORM_WORK* ttw = GFL_HEAP_AllocClearMemory( biw->heapID, sizeof( TCB_TRANSFORM_WORK ) );
      biw->tcb_execute_flag = 1;
      ttw->biw = biw;
      GFL_TCB_AddTask( biw->tcbsys, TCB_TransformCommand2Waza, ttw, 1 );
    }
    break;
  case BTLV_INPUT_SCRTYPE_DIR:
    break;
  case BTLV_INPUT_SCRTYPE_YES_NO:
    break;
  case BTLV_INPUT_SCRTYPE_ROTATE:
    break;
  }
  biw->scr_type = type;
}

//============================================================================================
/**
 *	@brief  ����ʃ��\�[�X���[�h
 *
 *	@param[in]  biw  ����ʊǗ��\����
 */
//============================================================================================
static  void  BTLV_INPUT_LoadResource( BTLV_INPUT_WORK* biw )
{ 
//	NARC_battgra_wb_battle_w_bg1b_NSCR = 18,
  GFL_ARCHDL_UTIL_TransVramBgCharacter( biw->handle, NARC_battgra_wb_battle_w_bg_NCGR,
                                        GFL_BG_FRAME0_S, 0, 0, FALSE, biw->heapID );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( biw->handle, NARC_battgra_wb_battle_w_bg3_NCGR,
                                        GFL_BG_FRAME3_S, 0, 0x8000, FALSE, biw->heapID );
  GFL_ARCHDL_UTIL_TransVramScreen( biw->handle, NARC_battgra_wb_battle_w_bg0_NSCR,
                                   GFL_BG_FRAME0_S, 0, 0, FALSE, biw->heapID );
  GFL_ARCHDL_UTIL_TransVramScreen( biw->handle, NARC_battgra_wb_battle_w_bg1a_NSCR,
                                   GFL_BG_FRAME1_S, 0, 0, FALSE, biw->heapID );
//  GFL_ARCHDL_UTIL_TransVramScreen( biw->handle, NARC_battgra_wb_battle_w_bg2_NSCR,
 //                                  GFL_BG_FRAME2_S, 0, 0, FALSE, biw->heapID );
  GFL_ARCHDL_UTIL_TransVramScreen( biw->handle, NARC_battgra_wb_battle_w_bg3_NSCR,
                                   GFL_BG_FRAME3_S, 0, 0, FALSE, biw->heapID );
  PaletteWorkSet_ArcHandle( BTLV_EFFECT_GetPfd(), biw->handle, NARC_battgra_wb_battle_w_bg_NCLR,
                            biw->heapID, FADE_SUB_BG, 0x1e0, 0 );

  biw->objcharID = GFL_CLGRP_CGR_Register( biw->handle, NARC_battgra_wb_battle_w_obj_NCGR, FALSE,
                                           CLSYS_DRAW_SUB, biw->heapID );
  biw->objcellID = GFL_CLGRP_CELLANIM_Register( biw->handle, NARC_battgra_wb_battle_w_obj_NCER,
                                                NARC_battgra_wb_battle_w_obj_NANR, biw->heapID );
  biw->objplttID = GFL_CLGRP_PLTT_Register( biw->handle, NARC_battgra_wb_battle_w_obj_NCLR, CLSYS_DRAW_SUB, 0, biw->heapID );
  PaletteWorkSet_VramCopy( BTLV_EFFECT_GetPfd(), FADE_SUB_OBJ,
                           GFL_CLGRP_PLTT_GetAddr( biw->objplttID, CLSYS_DRAW_SUB ) / 2, 0x20 * 3 );
}

//--------------------------------------------------------------
/**
 * @brief ������̒������擾����
 *
 * @param[in] str           ������ւ̃|�C���^
 * @param[in] font          �t�H���g�^�C�v
 * @param[in] ret_dot_len   �h�b�g�������
 * @param[in] ret_char_len  �L�����������
 */
//--------------------------------------------------------------
static void FontLenGet( const STRBUF *str, GFL_FONT *font, int *ret_dot_len, int *ret_char_len )
{
  int dot_len, char_len;

  //������̃h�b�g������A�g�p����L���������Z�o����
  dot_len = PRINTSYS_GetStrWidth( str, font, 0 );
  char_len = dot_len / 8;
  if( FX_ModS32( dot_len, 8 ) != 0 ){
    char_len++;
  }

  *ret_dot_len = dot_len;
  *ret_char_len = char_len;
}

//============================================================================================
/**
 *	@brief  ����ʕό`�^�X�N�i�ҋ@���R�}���h�I���j
 */
//============================================================================================
static  void  TCB_TransformStandby2Command( GFL_TCB* tcb, void* work )
{ 
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){ 
  case 0:
    GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TTS2C_FRAME1_SCROLL_X );
    GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TTS2C_FRAME1_SCROLL_Y );
    SetupScaleChange( ttw->biw, TTS2C_START_SCALE, TTS2C_END_SCALE, -TTS2C_SCALE_SPEED );
    SetupScrollUp( ttw->biw, TTS2C_START_SCROLL_X, TTS2C_START_SCROLL_Y, TTS2C_SCROLL_SPEED, TTS2C_SCROLL_COUNT );
    GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_ON );
    ttw->seq_no++;
    break;
  case 1:
    if( ttw->biw->tcb_execute_count == 0 )
    { 
      GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
      GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
      GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
      ttw->biw->tcb_execute_flag = 0;
      GFL_HEAP_FreeMemory( ttw );
      GFL_TCB_DeleteTask( tcb );
    }
    break;
  }
}

//============================================================================================
/**
 *	@brief  ����ʕό`�^�X�N�i�R�}���h�I�����Z�I���j
 */
//============================================================================================
static  void  TCB_TransformCommand2Waza( GFL_TCB* tcb, void* work )
{ 
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){ 
  case 0:
    SetupScrollUp( ttw->biw, TTC2W_START_SCROLL_X, TTC2W_START_SCROLL_Y, TTC2W_SCROLL_SPEED, TTC2W_SCROLL_COUNT );
    SetupScreenAnime( ttw->biw, 0, SCREEN_ANIME_DIR_FORWARD );
    SetupButtonAnime( ttw->biw, BUTTON_TYPE_WAZA, BUTTON_ANIME_TYPE_APPEAR );
    GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
    ttw->seq_no++;
    break;
  case 1:
    if( ttw->biw->tcb_execute_count == 0 )
    { 
      GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TSA_SCROLL_X3 );
      GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TSA_SCROLL_Y3 );
      ttw->biw->tcb_execute_flag = 0;
      GFL_HEAP_FreeMemory( ttw );
      GFL_TCB_DeleteTask( tcb );
    }
    break;
  }
}

//============================================================================================
/**
 *	@brief  ����ʕό`�^�X�N�i�Z�I�����R�}���h�I���j
 */
//============================================================================================
static  void  TCB_TransformWaza2Command( GFL_TCB* tcb, void* work )
{ 
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){ 
  case 0:
    SetupScrollUp( ttw->biw, TTW2C_START_SCROLL_X, TTW2C_START_SCROLL_Y, TTW2C_SCROLL_SPEED, TTW2C_SCROLL_COUNT );
    SetupScreenAnime( ttw->biw, 0, SCREEN_ANIME_DIR_BACKWARD );
    SetupButtonAnime( ttw->biw, BUTTON_TYPE_WAZA, BUTTON_ANIME_TYPE_VANISH );
    GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
    ttw->seq_no++;
    break;
  case 1:
    if( ttw->biw->tcb_execute_count == 0 )
    { 
      GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TSA_SCROLL_X0 );
      GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TSA_SCROLL_Y0 );
      ttw->biw->tcb_execute_flag = 0;
      GFL_HEAP_FreeMemory( ttw );
      GFL_TCB_DeleteTask( tcb );
    }
    break;
  }
}

static  void  TCB_TransformWaza2Dir( GFL_TCB* tcb, void* work )
{ 

}

//============================================================================================
/**
 *	@brief  ����ʕό`�^�X�N�i�R�}���h�I�����X�^���o�C�j
 */
//============================================================================================
static  void  TCB_TransformCommand2Standby( GFL_TCB* tcb, void* work )
{ 
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){ 
  case 0:
    SetupScaleChange( ttw->biw, TTC2S_START_SCALE, TTC2S_END_SCALE, TTC2S_SCALE_SPEED );
    GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_ON );
    ttw->seq_no++;
    break;
  case 1:
    if( ttw->biw->tcb_execute_count == 0 )
    { 
      ttw->biw->tcb_execute_flag = 0;
      GFL_HEAP_FreeMemory( ttw );
      GFL_TCB_DeleteTask( tcb );
    }
    break;
  }
}

//============================================================================================
/**
 *	@brief  ����ʕό`�^�X�N�i�Z�I�����X�^���o�C�j
 */
//============================================================================================
static  void  TCB_TransformWaza2Standby( GFL_TCB* tcb, void* work )
{ 
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){ 
  case 0:
    SetupScreenAnime( ttw->biw, 0, SCREEN_ANIME_DIR_BACKWARD );
    SetupButtonAnime( ttw->biw, BUTTON_TYPE_WAZA, BUTTON_ANIME_TYPE_VANISH );
    GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
    ttw->seq_no++;
    break;
  case 1:
    if( ttw->biw->tcb_execute_count == 0 )
    { 
      SetupScaleChange( ttw->biw, TTC2S_START_SCALE, TTC2S_END_SCALE, TTC2S_SCALE_SPEED );
      GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
      GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
      GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_ON );
      ttw->seq_no++;
    }
    break;
  case 2:
    if( ttw->biw->tcb_execute_count == 0 )
    { 
      ttw->biw->tcb_execute_flag = 0;
      GFL_HEAP_FreeMemory( ttw );
      GFL_TCB_DeleteTask( tcb );
    }
    break;
  }
}

//============================================================================================
/**
 *	@brief  �X�P�[���ύX�����Z�b�g�A�b�v
 *
 *  @param[in]  biw           �V�X�e���Ǘ��\���̂̃|�C���^
 *  @param[in]  start_scale   �X�P�[�������l
 *  @param[in]  end_scale     �ŏI�I�ȃX�P�[���l
 *  @param[in]  scale_speed   �X�P�[���ύX���x
 */
//============================================================================================
static  void  SetupScaleChange( BTLV_INPUT_WORK* biw, fx32 start_scale, fx32 end_scale, fx32 scale_speed )
{ 
  TCB_SCALE_UP* tsu = GFL_HEAP_AllocMemory( biw->heapID, sizeof( TCB_SCALE_UP ) );

  tsu->biw          = biw;
  tsu->start_scale  = start_scale;
  tsu->end_scale    = end_scale;
  tsu->scale_speed  = scale_speed;

  GFL_TCB_AddTask( biw->tcbsys, TCB_ScaleChange, tsu, 0 );

  biw->tcb_execute_count++;
}

//============================================================================================
/**
 *	@brief  �X�P�[���ύX�����^�X�N
 */
//============================================================================================
static  void  TCB_ScaleChange( GFL_TCB* tcb, void* work )
{ 
  TCB_SCALE_UP* tsu = ( TCB_SCALE_UP * )work;
  MtxFx22 mtx;

  tsu->start_scale += tsu->scale_speed;

  MTX_Scale22( &mtx, tsu->start_scale, tsu->start_scale );
  GFL_BG_SetAffineScroll( GFL_BG_FRAME3_S, GFL_BG_SCROLL_X_SET, 128, &mtx, 256, 256 );
  GFL_BG_SetAffineScroll( GFL_BG_FRAME3_S, GFL_BG_SCROLL_Y_SET, 128 + 40, &mtx, 256, 256 );
  if( tsu->start_scale == tsu->end_scale )
  { 
    tsu->biw->tcb_execute_count--;
    GFL_HEAP_FreeMemory( tsu );
    GFL_TCB_DeleteTask( tcb );
  }
}

//============================================================================================
/**
 *	@brief  �X�N���[���X�N���[�������Z�b�g�A�b�v
 *
 *  @param[in]  scroll_x      ����X���W
 *  @param[in]  scroll_y      ����Y���W
 *  @param[in]  scroll_speed  �X�N���[���X�s�[�h
 *  @param[in]  scroll_count  �X�N���[���J�E���g
 */
//============================================================================================
static  void  SetupScrollUp( BTLV_INPUT_WORK* biw, int scroll_x, int scroll_y, int scroll_speed, int scroll_count )
{ 
  TCB_SCROLL_UP* tsu = GFL_HEAP_AllocMemory( biw->heapID, sizeof( TCB_SCROLL_UP ) );

  GFL_BG_SetScroll( GFL_BG_FRAME0_S, GFL_BG_SCROLL_X_SET, scroll_x );
  GFL_BG_SetScroll( GFL_BG_FRAME0_S, GFL_BG_SCROLL_Y_SET, scroll_y );

  tsu->biw          = biw;
  tsu->scroll_y     = scroll_y;
  tsu->scroll_speed = scroll_speed;
  tsu->scroll_count = scroll_count;

  GFL_TCB_AddTask( biw->tcbsys, TCB_ScrollUp, tsu, 0 );

  biw->tcb_execute_count++;
}

//============================================================================================
/**
 *	@brief  �X�N���[���X�N���[�������^�X�N
 */
//============================================================================================
static  void  TCB_ScrollUp( GFL_TCB* tcb, void* work )
{ 
  TCB_SCROLL_UP* tsu = ( TCB_SCROLL_UP * )work;

  tsu->scroll_y += tsu->scroll_speed;

  GFL_BG_SetScroll( GFL_BG_FRAME0_S, GFL_BG_SCROLL_Y_SET, tsu->scroll_y );

  if( --tsu->scroll_count == 0 )
  { 
    tsu->biw->tcb_execute_count--;
    GFL_HEAP_FreeMemory( tsu );
    GFL_TCB_DeleteTask( tcb );
  }
}

//============================================================================================
/**
 *	@brief  �X�N���[���A�j�������Z�b�g�A�b�v
 *
 *	@param[in]  index �ǂݍ��ރX�N���[�����\�[�X�C���f�b�N�X
 *	@param[in]  dir   �A�j���[�V�����̌���
 */
//============================================================================================
static  void  SetupScreenAnime( BTLV_INPUT_WORK* biw, int index, SCREEN_ANIME_DIR dir )
{ 
  TCB_SCREEN_ANIME* tsa = GFL_HEAP_AllocMemory( biw->heapID, sizeof( TCB_SCREEN_ANIME ) );

  tsa->biw    = biw;
  tsa->count  = 0;
  tsa->dir    = dir;
  tsa->wait   = TSA_WAIT;

  //index����\������X�N���[����ǂݑւ���\��

  GFL_TCB_AddTask( biw->tcbsys, TCB_ScreenAnime, tsa, 0 );

  biw->tcb_execute_count++;
}

//============================================================================================
/**
 *	@brief  �X�N���[���A�j�������^�X�N
 */
//============================================================================================
static  void  TCB_ScreenAnime( GFL_TCB* tcb, void* work )
{ 
  TCB_SCREEN_ANIME* tsa = ( TCB_SCREEN_ANIME * )work;
  static  const int TSA_scroll_table[ 2 ][ 2 ][ 2 ] = { 
    { 
      { TSA_SCROLL_X1, TSA_SCROLL_Y1 },
      { TSA_SCROLL_X2, TSA_SCROLL_Y2 },
    },
    { 
      { TSA_SCROLL_X2, TSA_SCROLL_Y2 },
      { TSA_SCROLL_X1, TSA_SCROLL_Y1 },
    }
  };

  if( --tsa->wait )
  { 
    return;
  }

  tsa->wait = TSA_WAIT;

  GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TSA_scroll_table[ tsa->dir ][ tsa->count ][ TSA_SCROLL_X ] );
  GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TSA_scroll_table[ tsa->dir ][ tsa->count++ ][ TSA_SCROLL_Y ] );

  if( tsa->count >= 2 )
  { 
    tsa->biw->tcb_execute_count--;
    GFL_HEAP_FreeMemory( tsa );
    GFL_TCB_DeleteTask( tcb );
  }
}

//============================================================================================
/**
 *	@brief  �{�^���A�j�������Z�b�g�A�b�v
 *
 *	@param[in]  type      �{�^���^�C�v
 *	@param[in]  anm_type  �{�^���A�j���^�C�v
 */
//============================================================================================
static  void  SetupButtonAnime( BTLV_INPUT_WORK* biw, BUTTON_TYPE type, BUTTON_ANIME_TYPE anm_type )
{ 
  TCB_BUTTON_ANIME* tba = GFL_HEAP_AllocMemory( biw->heapID, sizeof( TCB_BUTTON_ANIME ) );
  static const GFL_CLWK_DATA button = {
    0, 0,     //x, y
    0, 0, 2,  //�A�j���ԍ��A�D�揇�ʁABG�v���C�I���e�B
  };
  static  const BUTTON_ANIME_PARAM bap[ BUTTON_TYPE_MAX ] = { 
    { 
      //BUTTON_TYPE_WAZA & BUTTON_TYPE_DIR_4
      { 
        { BUTTON4_X1, BUTTON4_Y1 },
        { BUTTON4_X2, BUTTON4_Y2 },
        { BUTTON4_X3, BUTTON4_Y3 },
        { BUTTON4_X4, BUTTON4_Y4 },
        { -1, -1 },
        { -1, -1 },
      },
      { BUTTON4_APPEAR_ANIME, BUTTON4_VANISH_ANIME },
    },
    { 
      //BUTTON_TYPE_DIR_6
      { 
        { BUTTON6_X1, BUTTON6_Y1 },
        { BUTTON6_X2, BUTTON6_Y2 },
        { BUTTON6_X3, BUTTON6_Y3 },
        { BUTTON6_X4, BUTTON6_Y4 },
        { BUTTON6_X5, BUTTON6_Y5 },
        { BUTTON6_X6, BUTTON6_Y6 },
      },
      { BUTTON6_APPEAR_ANIME, BUTTON6_VANISH_ANIME },
    },
    { 
      //BUTTON_TYPE_YES_NO
      { 
        { BUTTON2_X1, BUTTON2_Y1 },
        { BUTTON2_X2, BUTTON2_Y2 },
        { -1, -1 },
        { -1, -1 },
        { -1, -1 },
        { -1, -1 },
      },
      { BUTTON2_APPEAR_ANIME, BUTTON2_VANISH_ANIME },
    },
  };
  int i;

  tba->biw = biw;
  tba->clunit = GFL_CLACT_UNIT_Create( BUTTON_ANIME_MAX, 0, biw->heapID );

  for( i = 0 ; i < BUTTON_ANIME_MAX ; i++ )
  { 
    if( bap[ type ].pos[ i ].x == -1 )
    { 
      tba->clwk[ i ] = NULL;
      continue;
    }
    tba->clwk[ i ] = GFL_CLACT_WK_Create( tba->clunit, biw->objcharID, biw->objplttID, biw->objcellID,
                                          &button, CLSYS_DEFREND_SUB, biw->heapID );
    GFL_CLACT_WK_SetPos( tba->clwk[ i ], &bap[ type ].pos[ i ], CLSYS_DEFREND_SUB );
    GFL_CLACT_WK_SetAffineParam( tba->clwk[ i ], CLSYS_AFFINETYPE_DOUBLE );
    GFL_CLACT_WK_SetAutoAnmFlag( tba->clwk[ i ], TRUE );
    GFL_CLACT_WK_SetAnmSeq( tba->clwk[ i ], bap[ type ].anm_no[ anm_type ] );
  }

  GFL_TCB_AddTask( biw->tcbsys, TCB_ButtonAnime, tba, 0 );

  biw->tcb_execute_count++;
}

//============================================================================================
/**
 *	@brief  �{�^���A�j�������^�X�N
 */
//============================================================================================
static  void  TCB_ButtonAnime( GFL_TCB* tcb, void* work )
{ 
  TCB_BUTTON_ANIME* tba = ( TCB_BUTTON_ANIME * )work;
  int i;

  for( i = 0 ; i < BUTTON_ANIME_MAX ; i++ )
  { 
    if( tba->clwk[ i ] )
    { 
      if( GFL_CLACT_WK_CheckAnmActive( tba->clwk[ i ] ) )
      { 
        return;
      }
    }
  }
  for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
  { 
    if( tba->clwk[ i ] )
    { 
      GFL_CLACT_WK_Remove( tba->clwk[ i ] );
    }
  }
  tba->biw->tcb_execute_count--;
  GFL_HEAP_FreeMemory( tba );
  GFL_TCB_DeleteTask( tcb );
}

