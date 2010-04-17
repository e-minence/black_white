//============================================================================================
/**
 * @file    box2_main.c
 * @brief   �{�b�N�X��� ���C������
 * @author  Hiroyuki Nakamura
 * @date    09.10.05
 *
 *  ���W���[�����FBOX2MAIN
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "message.naix"
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/bmp_winframe.h"
#include "sound/pm_sndsys.h"
#include "savedata/perapvoice.h"
#include "savedata/zukan_savedata.h"
#include "font/font.naix"
#include "poke_tool/monsno_def.h"
#include "poke_tool/pokerus.h"
#include "item/item.h"

#include "msg/msg_boxmenu.h"

#include "app/bag.h"
#include "app/p_status.h"
#include "app/name_input.h"
#include "app/app_menu_common.h"
#include "app_menu_common.naix"

#include "box2_main.h"
#include "box2_bmp.h"
#include "box2_obj.h"
#include "box2_seq.h"
#include "box2_ui.h"
#include "box2_bgwfrm.h"
#include "box2_snd_def.h"
#include "box_gra.naix"


//============================================================================================
//  �萔��`
//============================================================================================
#define EXP_BUF_SIZE  ( 1024 )  // �e���|�������b�Z�[�W�o�b�t�@�T�C�Y

#define BLEND_EV1   ( 6 )       // �u�����h�W��
#define BLEND_EV2   ( 10 )      // �u�����h�W��

// �ǎ�
#define WALL_PY         ( 1 )                             // �ǎ��x�\�����W
#define WALL_SX         ( 21 )                            // �ǎ��w�T�C�Y
#define WALL_SY         ( 20 )                            // �ǎ��x�T�C�Y
#define WALL_CHR_SIZ    ( WALL_SX * WALL_SY )             // �ǎ��L������
#define WALL_CGX_POS1   ( 1024 - WALL_CHR_SIZ )           // �ǎ��L�����]���ʒu�P
#define WALL_CGX_POS2   ( WALL_CGX_POS1 - WALL_CHR_SIZ )  // �ǎ��L�����]���ʒu�Q
#define WALL_PUT_SPACE  ( WALL_SX + 2 )                   // �ǎ��z�u�Ԋu
#define WALL_SPACE_CHR  ( (BOX2MAIN_BG_PAL_WALL1<<12)+WALL_CGX_POS1+0x2b )  // �ǎ����ԃL����
#define WALL_SPTAG_CHR  ( (BOX2MAIN_BG_PAL_WALL1<<12)+WALL_CGX_POS1+0x54 )  // �ǎ����ԃL����
#define WALL_TITLE_POS  ( WALL_SX )                       // �{�b�N�X���\���L�����ʒu
#define WALL_TITLE_SY   ( 3 )                             // �{�b�N�X���\���x�T�C�Y

// �G���A�`�F�b�N�f�[�^
typedef struct {
  u8  lx;   // ���w���W
  u8  rx;   // �E�w���W
  u8  ty;   // ��x���W
  u8  by;   // ���x���W
}AREA_CHECK;

#define TRAYAREA_LX   ( 8 )   // �g���C�G���A���w���W
#define TRAYAREA_RX   ( 160-1 ) // �g���C�G���A�E�w���W
#define TRAYAREA_UY   ( 40 )    // �g���C�G���A��x���W
#define TRAYAREA_DY   ( 160-1 ) // �g���C�G���A���x���W

#define TRAYGRID_LX   ( 12 )    // �g���C�̍��[�O���b�h���W
#define TRAYGRID_RX   ( 156 )   // �g���C�̉E�[�O���b�h���W
#define TRAYGRID_SX   ( 24 )    // �g���C�̃O���b�h�w�T�C�Y
#define TRAYGRID_SY   ( 24 )    // �g���C�̃O���b�h�x�T�C�Y

#define PARTYAREA_SX  ( 32 )    // �莝���G���A�̂w�T�C�Y
#define PARTYAREA_SY  ( 24 )    // �莝���G���A�̂x�T�C�Y

// �|�P�����A�C�R���ړ�
#define POKEICON_MOVE_CNT ( 8 ) // �|�P�����A�C�R������t���[����

// �|�P�����𓦂���
#define POKE_FREE_CHECK_ONECE ( 15 )                                            // 1sync�Ƀ`�F�b�N����|�P������
#define POKE_FREE_CHECK_BOX   ( BOX_MAX_POS*BOX_MAX_TRAY )                      // �{�b�N�X�|�P������
#define POKE_FREE_CHECK_MAX   ( POKE_FREE_CHECK_BOX+BOX2OBJ_POKEICON_MINE_MAX ) // �`�F�b�N����|�P�����̑���

/*
// �}�[�L���O
#define MARKING_FRM_SX  ( 11 )            // �}�[�L���O�t���[���w�T�C�Y
#define MARKING_FRM_SY  ( 18 )            // �}�[�L���O�t���[���x�T�C�Y
#define MARKING_FRM_PX  ( 32 - MARKING_FRM_SX )   // �}�[�L���O�t���[���w�\�����W
#define MARKING_FRM_PY  ( 26 )            // �}�[�L���O�t���[���w�\�����W

#define MARKING_ON_CHR  ( 0x2b )          // �}�[�L���O�L�����J�n�ʒu ( ON )
#define MARKING_OFF_CHR ( 0x0b )          // �}�[�L���O�L�����J�n�ʒu ( OFF )

// �莝���|�P�����t���[��
#define PARTYPOKE_FRM_SX  ( 11 )    // �莝���|�P�����t���[���w�T�C�Y
#define PARTYPOKE_FRM_SY  ( 18 )    // �莝���|�P�����t���[���x�T�C�Y
#define PARTYPOKE_FRM_PX  ( 2 )   // �莝���|�P�����t���[���w�\�����W
#define PARTYPOKE_FRM_PY  ( 21 )    // �莝���|�P�����t���[���x�\�����W

// �{�b�N�X�ړ��t���[��
#define BOXMOVE_FRM_SX    ( 32 )    // �{�b�N�X�ړ��t���[���w�T�C�Y
#define BOXMOVE_FRM_SY    ( 6 )   // �{�b�N�X�ړ��t���[���x�T�C�Y
#define BOXMV_PTOUT_FRM_SX  ( 32 )    // �{�b�N�X�ړ��t���[���w�T�C�Y
#define BOXMV_PTOUT_FRM_SY  ( 7 )   // �{�b�N�X�ړ��t���[���x�T�C�Y

// �ǎ��ύX�t���[��
#define WPCHG_FRM_SX    ( 32 )    // �{�b�N�X�ړ��t���[���w�T�C�Y
#define WPCHG_FRM_SY    ( 7 )   // �{�b�N�X�ړ��t���[���x�T�C�Y
*/


/*
#define POKEMENU_SCROLL_SPD ( 8 ) // �|�P�������j���[�X�N���[�����x
#define POKEMENU_SCROLL_CNT ( 11 )  // �|�P�������j���[�X�N���[���J�E���g

#define MARKIN_FRM_SPD    ( 8 ) // �}�[�L���O�t���[���X�N���[�����x
#define MARKIN_FRM_CNT    ( 18 )  // �}�[�L���O�t���[���X�N���[���J�E���g

#define BOXPARTY_BTN_SPD  ( 8 ) // �u�Ă����|�P�����v�u�|�P�������ǂ��v�{�^���X�N���[�����x
#define BOXPARTY_BTN_CNT  ( 3 ) // �u�Ă����|�P�����v�u�|�P�������ǂ��v�{�^���X�N���[���J�E���g

#define PARTYPOKE_FRM_SPD ( BOX2MAIN_PARTYPOKE_FRM_SPD )  // �莝���|�P�����t���[���X�N���[�����x
#define PARTYPOKE_FRM_CNT ( BOX2MAIN_PARTYPOKE_FRM_CNT )  // �莝���|�P�����t���[���X�N���[���J�E���g
*/

// �A�C�e���A�C�R���ړ�
#define ITEMICON_MOVE_CNT ( 8 ) // �A�C�e���A�C�R������t���[����

/*
// BGWIN�t���[���f�[�^
#define SUBDISP_ITEM_FRM_SX   ( 32 )
#define SUBDISP_ITEM_FRM_SY   ( 9 )
#define SUBDISP_ITEM_FRM_IN_PX  ( 0 )
#define SUBDISP_ITEM_FRM_IN_PY  ( 24 )
#define SUBDISP_WAZA_FRM_SX   ( 12 )
#define SUBDISP_WAZA_FRM_SY   ( 10 )
#define SUBDISP_WAZA_FRM_IN_PX  ( 32 )
#define SUBDISP_WAZA_FRM_IN_PY  ( 12 )


// �}�[�L���O�t���[���f�[�^
#define WINFRM_MARK_PX      ( 21 )
#define WINFRM_MARK_PY      ( 6 )
#define WINFRM_MARK_IN_START_PY ( 24 )
#define WINFRM_MARK_MV_CNT    ( WINFRM_MARK_IN_START_PY - WINFRM_MARK_PY )

// �莝���|�P�����t���[���f�[�^
#define WINFRM_PARTYPOKE_LX ( 2 )
#define WINFRM_PARTYPOKE_RX ( 21 )
#define WINFRM_PARTYPOKE_PY ( 6 )
#define WINFRM_PARTYPOKE_INIT_PY  ( 24 )
#define WINFRM_PARTYPOKE_RET_PX   ( 24 )
#define WINFRM_PARTYPOKE_RET_PY   ( 15 )



// �|�P�����I�����j���[�t���[���f�[�^
#define WINFRM_MENU_INIT_PX ( 32 )    // �\���w���W
#define WINFRM_MENU_PX    ( 21 )    // �\���w���W
#define WINFRM_MENU_PY    ( 5 )   // �\���x���W
#define WINFRM_MENU_SY    ( 3 )   // �\���x�T�C�Y


//�u�{�b�N�X�����肩����v�{�^���t���[���f�[�^
#define WINFRM_BOXCHG_BTN_PX  ( 0 )
#define WINFRM_BOXCHG_BTN_PY  ( 21 )
#define WINFRM_BOXCHG_BTN_INIT_PY ( 24 )

//�u�x�悤�����݂�v�{�^���t���[���f�[�^
#define WINFRM_Y_ST_BTN_PX    ( 0 )
#define WINFRM_Y_ST_BTN_PY    ( 21 )
#define WINFRM_Y_ST_BTN_INIT_PY ( 24 )
*/


// ���ʃ}�[�L���O�\�����W
#define SUBDISP_MARK_PX       ( 25 )
#define SUBDISP_MARK_PY       ( 13 )
#define SUBDISP_MARK_ON_CHAR  ( 0x3a )
#define SUBDISP_MARK_OFF_CHAR ( 0x1a )
#define SUBDISP_MARK_PAL      ( 0 )

// VBLANK FUNC�F�擾�|�P�����ړ��V�[�P���X
enum {
  SEIRI_SEQ_MENU_OUT = 0,     // ���j���[�A�E�g
  SEIRI_SEQ_ICON_GET,         // �A�C�R���擾��
  SEIRI_SEQ_ICON_PUT,         // �A�C�R���z�u
  SEIRI_SEQ_ICON_PUT_END,     // �A�C�R���z�u�I��
  SEIRI_SEQ_CANCEL_SCROLL_L,  // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i���j
  SEIRI_SEQ_CANCEL_SCROLL_R,  // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i�E�j
  SEIRI_SEQ_SCROLL_L,         // �g���C�X�N���[���i���j
  SEIRI_SEQ_SCROLL_R,         // �g���C�X�N���[���i�E�j
  SEIRI_SEQ_TI_SCROLL_U,      // �g���C�A�C�R���X�N���[���i��j
  SEIRI_SEQ_TI_SCROLL_D,      // �g���C�A�C�R���X�N���[���i���j
  SEIRI_SEQ_TRAYFRM_IN,       // �g���C�t���[���C��
  SEIRI_SEQ_TRAYFRM_OUT,      // �g���C�t���[���A�E�g
  SEIRI_SEQ_PARTYFRM_IN,      // �p�[�e�B�t���[���C��
  SEIRI_SEQ_PARTYFRM_OUT,     // �p�[�e�B�t���[���A�E�g
  SEIRI_SEQ_CANCEL_TRAYFRM_OUT,   // �g���C�t���[���A�E�g�i�L�����Z�������j
  SEIRI_SEQ_CANCEL_PARTYFRM_IN,   // �p�[�e�B�t���[���C���i�L�����Z�������j
  SEIRI_SEQ_END,              // �I��
};

//�u��������v���[�h�̗a����{�b�N�X�I���t���[���f�[�^
#define WINFRM_PARTYOUT_PX  ( 0 )
#define WINFRM_PARTYOUT_PY  ( 6 )
//#define PTOUT_PUT_CNT_MAX ( 4 )

// VBLANK FUNC�F�擾�|�P�����ړ��V�[�P���X�i�|�P��������������j
enum {
//  AZUKERU_SEQ_EXITPRM_INIT = 0, //�u���ǂ�v�{�^�����쏉����
  AZUKERU_SEQ_MENU_OUT = 0,   // ���j���[�A�E�g
  AZUKERU_SEQ_ICON_GET,       // �A�C�R���擾��

  AZUKERU_SEQ_SCROLL_L,       // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i���j
  AZUKERU_SEQ_SCROLL_R,       // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i�E�j
  AZUKERU_SEQ_MOVE_CANCEL1,   // �ړ��L�����Z���P
  AZUKERU_SEQ_MOVE_CANCEL2,   // �ړ��L�����Z���Q

  AZUKERU_SEQ_MOVE_ENTER1,    // �ړ����s�P
  AZUKERU_SEQ_MOVE_ENTER2,    // �ړ����s�Q

  AZUKERU_SEQ_END,        // �I��
};

// VBLANK FUNC�F�擾�|�P�����ړ��V�[�P���X�i�|�P��������Ă����j
enum {
  TSURETEIKU_SEQ_MENU_OUT = 0,    // ���j���[�A�E�g
  TSURETEIKU_SEQ_ICON_GET,        // �A�C�R���擾��

  TSURETEIKU_SEQ_MOVE_CANCEL1,    // �ړ��L�����Z���P
  TSURETEIKU_SEQ_MOVE_CANCEL2,    // �ړ��L�����Z���Q
  TSURETEIKU_SEQ_MOVE_CANCEL3,    // �ړ��L�����Z���R

  TSURETEIKU_SEQ_MOVE_ENTER1,   // �ړ����s�P
  TSURETEIKU_SEQ_MOVE_ENTER2,   // �ړ����s�Q
//  TSURETEIKU_SEQ_MOVE_ENTER3,   // �ړ����s�R

  TSURETEIKU_SEQ_END,       // �I��
};

// ��J�[�\���Ń|�P�����擾����Ƃ��̃f�[�^
#define HANDCURSOR_MOVE_CNT   ( 4 )
#define HANDCURSOR_MOVE_DOT   ( 2 )

// ���O���͉�ʗp���[�N
typedef struct {
  NAMEIN_PARAM * prm;
  STRBUF * name;
}BOX_NAMEIN_WORK;

// �h���b�O���Ɏ��̃g���C�X�N���[�����\�ɂ���܂ł̃E�F�C�g
#define TRAY_SCROLL_NEXT_WAIT   ( 16 )


//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================
static void VBlankTask( GFL_TCB * tcb, void * work );
static u32 MoveBoxGet( BOX2_SYS_WORK * syswk, u32 pos );
static void PutSubDispPokeMark( BOX2_APP_WORK * appwk, BOX2_POKEINFO_DATA * info );
static void SubDispMarkingOff( BOX2_APP_WORK * appwk );

static void MoveGetItemIcon( BOX2_APP_WORK * appwk, u32 x, u32 y );


//============================================================================================
//  �O���[�o��
//============================================================================================

// VRAM����U��
static const GFL_DISP_VRAM VramTbl = {
  GX_VRAM_BG_128_A,             // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,       // ���C��2D�G���W����BG�g���p���b�g

  GX_VRAM_SUB_BG_128_C,         // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g

  GX_VRAM_OBJ_128_B,            // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,      // ���C��2D�G���W����OBJ�g���p���b�g

  GX_VRAM_SUB_OBJ_128_D,          // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,    // �T�u2D�G���W����OBJ�g���p���b�g

  GX_VRAM_TEX_NONE,             // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_NONE,         // �e�N�X�`���p���b�g�X���b�g

  GX_OBJVRAMMODE_CHAR_1D_128K,  // ���C��OBJ�}�b�s���O���[�h
  GX_OBJVRAMMODE_CHAR_1D_128K   // �T�uOBJ�}�b�s���O���[�h
};

// �g���C�G���A�͈̓f�[�^
static const AREA_CHECK TrayPokeArea = { TRAYAREA_LX, TRAYAREA_RX, TRAYAREA_UY, TRAYAREA_DY };

// �莝���G���A�͈̓f�[�^�i���j
static const AREA_CHECK PartyPokeAreaLeft[BOX2OBJ_POKEICON_MINE_MAX] =
{
  { 26, 26+PARTYAREA_SX-1,  58,  58+PARTYAREA_SY-1 }, { 62, 62+PARTYAREA_SX-1,  66,  66+PARTYAREA_SY-1 },
  { 26, 26+PARTYAREA_SX-1,  90,  90+PARTYAREA_SY-1 }, { 62, 62+PARTYAREA_SX-1,  98,  98+PARTYAREA_SY-1 },
  { 26, 26+PARTYAREA_SX-1, 122, 122+PARTYAREA_SY-1 }, { 62, 62+PARTYAREA_SX-1, 130, 130+PARTYAREA_SY-1 },
};

// �莝���G���A�͈̓f�[�^�i�E�j
static const AREA_CHECK PartyPokeAreaRight[BOX2OBJ_POKEICON_MINE_MAX] =
{
  { 178, 178+PARTYAREA_SX-1,  58,  58+PARTYAREA_SY-1 }, { 214, 214+PARTYAREA_SX-1,  66,  66+PARTYAREA_SY-1 },
  { 178, 178+PARTYAREA_SX-1,  90,  90+PARTYAREA_SY-1 }, { 214, 214+PARTYAREA_SX-1,  98,  98+PARTYAREA_SY-1 },
  { 178, 178+PARTYAREA_SX-1, 122, 122+PARTYAREA_SY-1 }, { 214, 214+PARTYAREA_SX-1, 130, 130+PARTYAREA_SY-1 },
};

// �{�b�N�X�ړ��g���C�G���A�f�[�^
static const AREA_CHECK BoxMoveTrayArea[BOX2OBJ_TRAYICON_MAX] =
{
  {  31,  54, 15, 38 }, {  65,  88, 15, 38 }, {  99, 122, 15, 38 },
  { 133, 156, 15, 38 }, { 167, 190, 15, 38 }, { 201, 224, 15, 38 },
};




//--------------------------------------------------------------------------------------------
/**
 * @brief   VBLANK�֐��ݒ�
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_InitVBlank( BOX2_SYS_WORK * syswk )
{
  syswk->app->vtask = GFUser_VIntr_CreateTCB( VBlankTask, syswk, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   VBLANK�֐��폜
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_ExitVBlank( BOX2_SYS_WORK * syswk )
{
  GFL_TCB_DeleteTask( syswk->app->vtask );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   VBLANK����
 *
 * @param   tcb     GFL_TCB
 * @param   work    �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void VBlankTask( GFL_TCB * tcb, void * work )
{
  BOX2_SYS_WORK * syswk = work;

  GFL_BG_VBlankFunc();
  GFL_CLACT_SYS_VBlankFunc();

  PaletteFadeTrans( syswk->app->pfd );

  OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK���ŌĂ΂��֐���ݒ�
 *
 * @param appwk �{�b�N�X��ʃA�v�����[�N
 * @param func  �֐��|�C���^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_VFuncSet( BOX2_APP_WORK * appwk, void * func )
{
  appwk->vfunk.seq  = 0;
  appwk->vfunk.cnt  = 0;
  appwk->vfunk.func = func;
  appwk->vfunk.freq = NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK���ŌĂ΂��֐���\��
 *
 * @param appwk �{�b�N�X��ʃA�v�����[�N
 * @param func  �֐��|�C���^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_VFuncReq( BOX2_APP_WORK * appwk, void * func )
{
  appwk->vfunk.freq = func;
}

//--------------------------------------------------------------------------------------------
/**
 * �\�񂵂�VBLANK�֐����Z�b�g
 *
 * @param appwk �{�b�N�X��ʃA�v�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_VFuncReqSet( BOX2_APP_WORK * appwk )
{
  BOX2MAIN_VFuncSet( appwk, appwk->vfunk.freq );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   VRAM�ݒ�
 *
 * @param   none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_VramBankSet(void)
{
  GFL_DISP_SetBank( &VramTbl );
}

const GFL_DISP_VRAM * BOX2MAIN_GetVramBankData(void)
{
  return &VramTbl;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   �a�f������
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_BgInit( BOX2_SYS_WORK * syswk )
{
  GFL_BG_Init( HEAPID_BOX_APP );

  { // BG SYSTEM
    GFL_BG_SYS_HEADER sysh = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    GFL_BG_SetBGMode( &sysh );
  }

  { // ���C����ʁF����
    GFL_BG_BGCNT_HEADER cnth= {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x18000, 0x8000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &cnth, GFL_BG_MODE_TEXT );
  }
  { // ���C����ʁF�{�^����
    GFL_BG_BGCNT_HEADER cnth= {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000, 0x8000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &cnth, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen( GFL_BG_FRAME1_M );
    GFL_BG_SetClearCharacter( GFL_BG_FRAME1_M, 0x20, 0, HEAPID_BOX_APP );
  }
  { // ���C����ʁF�w�i
    GFL_BG_BGCNT_HEADER cnth= {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x08000, 0x8000,
      GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &cnth, GFL_BG_MODE_TEXT );
  }
  { // ���C����ʁF�ǎ���
    GFL_BG_BGCNT_HEADER cnth= {
      0, 0, 0x1000, 0, GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x00000,  0x8000,
      GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &cnth, GFL_BG_MODE_TEXT );
  }

  { // �T�u��ʁF����
    GFL_BG_BGCNT_HEADER cnth= {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x10000, 0x8000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME0_S, &cnth, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen( GFL_BG_FRAME0_S );
    GFL_BG_SetClearCharacter( GFL_BG_FRAME0_S, 0x20, 0, HEAPID_BOX_APP );
  }
  { // �T�u��ʁF�w�i
    GFL_BG_BGCNT_HEADER cnth= {
      0, 0, 0x1000, 0, GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x00000, 0x8000,
      GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &cnth, GFL_BG_MODE_TEXT );
  }
  { // �T�u��ʁF�E�B���h�E�i�Z�E�A�C�e���j
    GFL_BG_BGCNT_HEADER cnth= {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x18000, 0x8000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME2_S, &cnth, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen( GFL_BG_FRAME2_S );
    GFL_BG_SetClearCharacter( GFL_BG_FRAME2_S, 0x20, 0, HEAPID_BOX_APP );
  }

  GFL_DISP_GX_SetVisibleControl(
    GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl(
    GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2, VISIBLE_ON );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �a�f���
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_BgExit( BOX2_SYS_WORK * syswk )
{
  GFL_DISP_GX_SetVisibleControl(
    GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_OFF );
  GFL_DISP_GXS_SetVisibleControl(
    GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2, VISIBLE_OFF );

  GFL_BG_FreeBGControl( GFL_BG_FRAME2_S );
  GFL_BG_FreeBGControl( GFL_BG_FRAME1_S );
  GFL_BG_FreeBGControl( GFL_BG_FRAME0_S );
  GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );
  GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
  GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
  GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );

  GFL_BG_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �a�f�O���t�B�b�N�ǂݍ���
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_BgGraphicLoad( BOX2_SYS_WORK * syswk )
{
  ARCHANDLE * ah;
  GFL_ARCUTIL_TRANSINFO  info;

  ah = GFL_ARC_OpenDataHandle( ARCID_BOX2_GRA, HEAPID_BOX_APP );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
    ah, NARC_box_gra_box_m_bg1_lz_NCGR, GFL_BG_FRAME1_M, 0, 0, TRUE, HEAPID_BOX_APP );

//  GFL_ARCHDL_UTIL_TransVramBgCharacter(
  GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(
    ah, NARC_box_gra_box_m_bg2_lz_NCGR,
    GFL_BG_FRAME2_M, 0, TRUE, HEAPID_BOX_APP );
  GFL_ARCHDL_UTIL_TransVramScreen(
    ah, NARC_box_gra_box_m_bg2_lz_NSCR,
    GFL_BG_FRAME2_M, 0, 0, TRUE, HEAPID_BOX_APP );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
    ah, NARC_box_gra_box_m_bg3_lz_NCGR,
    GFL_BG_FRAME3_M, 0, 0, TRUE, HEAPID_BOX_APP );
  GFL_ARCHDL_UTIL_TransVramScreen(
    ah, NARC_box_gra_box_m_bg3_lz_NSCR,
    GFL_BG_FRAME3_M, 0, 0, TRUE, HEAPID_BOX_APP );

  GFL_ARCHDL_UTIL_TransVramPalette(
    ah, NARC_box_gra_box_m_bg_NCLR, PALTYPE_MAIN_BG, 0, 0x20*4, HEAPID_BOX_APP );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
    ah, NARC_box_gra_selwin_lz_NCGR, GFL_BG_FRAME1_M,
    BOX2MAIN_SELWIN_CGX_POS, BOX2MAIN_SELWIN_CGX_SIZ*0x20*2, TRUE, HEAPID_BOX_APP );

  GFL_ARCHDL_UTIL_TransVramPalette(
    ah, NARC_box_gra_selwin_NCLR, PALTYPE_MAIN_BG,
    BOX2MAIN_BG_PAL_SELWIN*0x20, 0x40, HEAPID_BOX_APP );
  {
    NNSG2dPaletteData * pal;
    void * buf;
    u16 * dat;
    buf = GFL_ARC_UTIL_LoadPalette(
            APP_COMMON_GetArcId(), NARC_app_menu_common_task_menu_NCLR, &pal, HEAPID_BOX_APP_L );
    dat = (u16 *)pal->pRawData;
    GFL_BG_LoadPalette( GFL_BG_FRAME0_M, &dat[16*1+10], 2*2, BOX2MAIN_BG_PAL_SELWIN*0x20+10*2 );
    GFL_BG_LoadPalette( GFL_BG_FRAME0_M, &dat[16*0+10], 2*2, (BOX2MAIN_BG_PAL_SELWIN+1)*0x20+10*2 );
    GFL_HEAP_FreeMemory( buf );
  }

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
    ah, NARC_box_gra_box_s_bg3_lz_NCGR,
    GFL_BG_FRAME1_S, 0, 0, TRUE, HEAPID_BOX_APP );
  GFL_ARCHDL_UTIL_TransVramScreen(
    ah, NARC_box_gra_box_s_bg3_lz_NSCR,
    GFL_BG_FRAME1_S, 0, 0, TRUE, HEAPID_BOX_APP );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
    ah, NARC_box_gra_box_s_bg3_lz_NCGR,
    GFL_BG_FRAME2_S, 0, 0, TRUE, HEAPID_BOX_APP );

  GFL_ARCHDL_UTIL_TransVramPalette(
    ah, NARC_box_gra_box_s_bg_NCLR, PALTYPE_SUB_BG, 0, 0, HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );

  // �^�b�`�o�[
  ah = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_BOX_APP );

  GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(
    ah, APP_COMMON_GetBarCharArcIdx(), GFL_BG_FRAME0_M, 0, FALSE, HEAPID_BOX_APP );
  GFL_ARCHDL_UTIL_TransVramPalette(
    ah, APP_COMMON_GetBarPltArcIdx(), PALTYPE_MAIN_BG,
    BOX2MAIN_BG_PAL_TOUCH_BAR*0x20, 0x20, HEAPID_BOX_APP );
  GFL_ARCHDL_UTIL_TransVramScreen(
    ah, APP_COMMON_GetBarScrnArcIdx(),
    GFL_BG_FRAME0_M, 0, 0, FALSE, HEAPID_BOX_APP );
  GFL_BG_ChangeScreenPalette(
    GFL_BG_FRAME0_M, BOX2MAIN_TOUCH_BAR_PX, BOX2MAIN_TOUCH_BAR_PY,
    BOX2MAIN_TOUCH_BAR_SX, BOX2MAIN_TOUCH_BAR_SY, BOX2MAIN_BG_PAL_TOUCH_BAR );
  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME0_M );

  info = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(
          ah, APP_COMMON_GetBarCharArcIdx(), GFL_BG_FRAME2_M, 0, FALSE, HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );

  { // �^�b�`�o�[��BG2�ɓW�J
    u16 * scr0;
    u16 * scr2;
    u32 i;

    scr0 = GFL_BG_GetScreenBufferAdrs( GFL_BG_FRAME0_M );
    scr2 = GFL_BG_GetScreenBufferAdrs( GFL_BG_FRAME2_M );

    scr0 = &scr0[ BOX2MAIN_TOUCH_BAR_PY*BOX2MAIN_TOUCH_BAR_SX+BOX2MAIN_TOUCH_BAR_PX ];
    scr2 = &scr2[ BOX2MAIN_TOUCH_BAR_PY*BOX2MAIN_TOUCH_BAR_SX+BOX2MAIN_TOUCH_BAR_PX ];

    for( i=0; i<BOX2MAIN_TOUCH_BAR_SX*BOX2MAIN_TOUCH_BAR_SY; i++ ){
      scr2[i] = scr0[i] + info;
    }

    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_M );
  }


  syswk->app->syswinInfo = BmpWinFrame_GraphicSetAreaMan(
                            GFL_BG_FRAME0_M,
                            BOX2MAIN_BG_PAL_SYSWIN,
                            MENU_TYPE_SYSTEM,
                            HEAPID_BOX_APP );

  // �t�H���g�p���b�g
  GFL_ARC_UTIL_TransVramPalette(
    ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
    BOX2MAIN_BG_PAL_SYSFNT*0x20, 0x20, HEAPID_BOX_APP );
  GFL_ARC_UTIL_TransVramPalette(
    ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
    BOX2MAIN_BG_PAL_SYSFNT_S*0x20, 0x20, HEAPID_BOX_APP );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �p���b�g�t�F�[�h������
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_PaletteFadeInit( BOX2_SYS_WORK * syswk )
{
  syswk->app->pfd = PaletteFadeInit( HEAPID_BOX_APP );
  PaletteFadeWorkAllocSet( syswk->app->pfd, FADE_MAIN_BG, FADE_PAL_ALL_SIZE, HEAPID_BOX_APP );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �p���b�g�t�F�[�h���
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_PaletteFadeExit( BOX2_SYS_WORK * syswk )
{
  PaletteFadeWorkAllocFree( syswk->app->pfd, FADE_MAIN_BG );
  PaletteFadeFree( syswk->app->pfd );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  �A���t�@�u�����h�ݒ�
 *
 * @param   flg   TRUE = �L��, FALSE = ����
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_SetBlendAlpha( BOOL flg )
{
  if( flg == TRUE ){
    G2_SetBlendAlpha(
      GX_BLEND_PLANEMASK_NONE, 
      GX_BLEND_PLANEMASK_BG1 |
      GX_BLEND_PLANEMASK_BG3 |
      GX_BLEND_PLANEMASK_BD,
      BLEND_EV1,
      BLEND_EV2 );
  }else{
    G2_BlendNone();
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�֘A������
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_MsgInit( BOX2_SYS_WORK * syswk )
{
  syswk->app->mman = GFL_MSG_Create(
                        GFL_MSG_LOAD_NORMAL,
                        ARCID_MESSAGE, NARC_message_boxmenu_dat, HEAPID_BOX_APP );
  syswk->app->font = GFL_FONT_Create(
                        ARCID_FONT, NARC_font_large_gftr,
                        GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_BOX_APP );
  syswk->app->nfnt = GFL_FONT_Create(
                        ARCID_FONT, NARC_font_num_gftr,
                        GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_BOX_APP );
  syswk->app->wset = WORDSET_Create( HEAPID_BOX_APP );
  syswk->app->que  = PRINTSYS_QUE_Create( HEAPID_BOX_APP );
  syswk->app->exp  = GFL_STR_CreateBuffer( EXP_BUF_SIZE, HEAPID_BOX_APP );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�֘A���
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_MsgExit( BOX2_SYS_WORK * syswk )
{
  GFL_STR_DeleteBuffer( syswk->app->exp );
  PRINTSYS_QUE_Delete( syswk->app->que );
  WORDSET_Delete( syswk->app->wset );
  GFL_FONT_Delete( syswk->app->nfnt );
  GFL_FONT_Delete( syswk->app->font );
  GFL_MSG_Delete( syswk->app->mman );
}

//--------------------------------------------------------------------------------------------
/**
 * �͂��E����������������
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_YesNoWinInit( BOX2_SYS_WORK * syswk )
{
//  syswk->app->tsw = TOUCH_SW_AllocWork( HEAPID_BOX_APP );

  syswk->app->ynList[0].str      = GFL_MSG_CreateString( syswk->app->mman, mes_box_yes );
  syswk->app->ynList[0].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  syswk->app->ynList[0].type     = APP_TASKMENU_WIN_TYPE_NORMAL;

  syswk->app->ynList[1].str      = GFL_MSG_CreateString( syswk->app->mman, mes_box_no );
  syswk->app->ynList[1].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  syswk->app->ynList[1].type     = APP_TASKMENU_WIN_TYPE_NORMAL;

  syswk->app->ynRes = APP_TASKMENU_RES_Create(
                        GFL_BG_FRAME0_M, BOX2MAIN_BG_PAL_YNWIN,
                        syswk->app->font, syswk->app->que, HEAPID_BOX_APP );
}

//--------------------------------------------------------------------------------------------
/**
 * �͂��E�������������
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_YesNoWinExit( BOX2_SYS_WORK * syswk )
{
  APP_TASKMENU_RES_Delete( syswk->app->ynRes );

  GFL_STR_DeleteBuffer( syswk->app->ynList[1].str );
  GFL_STR_DeleteBuffer( syswk->app->ynList[0].str );

//  TOUCH_SW_FreeWork( syswk->app->tsw );
}

//--------------------------------------------------------------------------------------------
/**
 * �͂��E�������Z�b�g
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param pos   �J�[�\���ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_YesNoWinSet( BOX2_SYS_WORK * syswk, u32 pos )
{
  APP_TASKMENU_INITWORK wk;

  wk.heapId   = HEAPID_BOX_APP;
  wk.itemNum  = 2;
  wk.itemWork = syswk->app->ynList;
  wk.posType  = ATPT_RIGHT_DOWN;
  wk.charPosX = 32;
  wk.charPosY = 18;
  wk.w        = APP_TASKMENU_PLATE_WIDTH_YN_WIN;
  wk.h        = APP_TASKMENU_PLATE_HEIGHT_YN_WIN;

  // �J�[�\���̏�����Ԃ�ݒ�
  if( CURSORMOVE_CursorOnOffGet( syswk->app->cmwk ) == TRUE ){
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
  }else{
    GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
  }

  syswk->app->ynWork = APP_TASKMENU_OpenMenu( &wk, syswk->app->ynRes );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�^���A�j������
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "TRUE = �A�j����"
 * @retval  "FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2MAIN_ButtonAnmMain( BOX2_SYS_WORK * syswk )
{
  BUTTON_ANM_WORK * bawk = &syswk->app->bawk;

  switch( bawk->btn_seq ){
  case 0:
    if( bawk->btn_mode == BOX2MAIN_BTN_ANM_MODE_OBJ ){
      BOX2OBJ_AutoAnmSet( syswk->app, bawk->btn_id, bawk->btn_pal1 );
    }else{
      GFL_BG_ChangeScreenPalette(
        bawk->btn_id, bawk->btn_px, bawk->btn_py, bawk->btn_sx, bawk->btn_sy, bawk->btn_pal1 );
      GFL_BG_LoadScreenV_Req( bawk->btn_id );
    }
    bawk->btn_seq++;
    break;

  case 1:
    if( bawk->btn_mode == BOX2MAIN_BTN_ANM_MODE_OBJ ){
      if( BOX2OBJ_AnmCheck( syswk->app, bawk->btn_id ) == FALSE ){
        return FALSE;
      }
    }else{
      bawk->btn_cnt++;
      if( bawk->btn_cnt != 4 ){ break; }
      GFL_BG_ChangeScreenPalette(
        bawk->btn_id, bawk->btn_px, bawk->btn_py, bawk->btn_sx, bawk->btn_sy, bawk->btn_pal2 );
      GFL_BG_LoadScreenV_Req( bawk->btn_id );
      bawk->btn_cnt = 0;
      bawk->btn_seq++;
    }
    break;

  case 2:
    bawk->btn_cnt++;
    if( bawk->btn_cnt == 2 ){
      return FALSE;
    }
  }

  return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �a�f�{�^���A�j���Z�b�g
 *
 * @param   syswk     �{�b�N�X��ʃV�X�e�����[�N
 * @param   wfrmID    �a�f�E�B���h�E�t���[���h�c
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_InitBgButtonAnm( BOX2_SYS_WORK * syswk, u32 wfrmID )
{
  u16 sx, sy;
  s8  px, py;

  BGWINFRM_PosGet( syswk->app->wfrm, wfrmID, &px, &py );
  BGWINFRM_SizeGet( syswk->app->wfrm, wfrmID, &sx, &sy );

  syswk->app->bawk.btn_mode = BOX2MAIN_BTN_ANM_MODE_BG;
  syswk->app->bawk.btn_id   = BGWINFRM_BGFrameGet( syswk->app->wfrm, wfrmID );
  syswk->app->bawk.btn_pal1 = BOX2MAIN_BG_PAL_SELWIN + 1;
  syswk->app->bawk.btn_pal2 = BOX2MAIN_BG_PAL_SELWIN;
  syswk->app->bawk.btn_seq  = 0;
  syswk->app->bawk.btn_cnt  = 0;

  syswk->app->bawk.btn_px   = px;
  syswk->app->bawk.btn_py   = py;
  syswk->app->bawk.btn_sx   = sx;
  syswk->app->bawk.btn_sy   = sy;
}


void BOX2MAIN_ResetTouchBar( BOX2_SYS_WORK * syswk )
{
  BOX2OBJ_SetTouchBarButton(
    syswk, syswk->tb_ret_btn, syswk->tb_exit_btn, syswk->tb_status_btn );
  BOX2BGWFRM_PutTouchBar( syswk->app->wfrm );
}



void BOX2MAIN_InitTrayScroll( BOX2_SYS_WORK * syswk, u32 mvID )
{
  if( mvID == BOX2MAIN_TRAY_SCROLL_L ){
    if( syswk->tray == 0 ){
      syswk->tray = syswk->trayMax-1;
    }else{
      syswk->tray--;
    }
    BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_L_ARROW, BOX2OBJ_ANM_L_ARROW_ON );
  }else{
    if( syswk->tray == (syswk->trayMax-1) ){
      syswk->tray = 0;
    }else{
      syswk->tray++;
    }
    BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_R_ARROW, BOX2OBJ_ANM_R_ARROW_ON );
  }
  BOX2OBJ_PokeIconBufLoad( syswk, syswk->tray );
  BOX2MAIN_WallPaperSet( syswk, BOX2MAIN_GetWallPaperNumber(syswk,syswk->tray), mvID );
}

void BOX2MAIN_InitTrayIconScroll( BOX2_SYS_WORK * syswk, s32 mv )
{
  syswk->box_mv_pos = BOX2MAIN_GetBoxMoveTrayNum( syswk, mv );
  BOX2OBJ_InitTrayCursorScroll( syswk, mv );
  BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
}

void BOX2MAIN_InitBoxMoveFrameScroll( BOX2_SYS_WORK * syswk )
{
  BOX2OBJ_BoxMoveObjInit( syswk );
//  BOX2OBJ_BoxMoveCursorSet( syswk );
  BOX2OBJ_InitBoxMoveCursorPos( syswk->app );
//  BOX2OBJ_BoxMoveNameSet( syswk );
  BOX2BMP_BoxMoveNameWrite( syswk, 0 );
  BOX2BGWFRM_BoxMoveFrmInSet( syswk->app->wfrm );
}


u32 BOX2MAIN_GetTrayScrollDir( BOX2_SYS_WORK * syswk, u32 now, u32 chg )
{
  u32 abs = GFL_STD_Abs( now - chg );

  if( now > chg ){
    if( abs >= syswk->trayMax/2 ){
      return BOX2MAIN_TRAY_SCROLL_R;
    }else{
      return BOX2MAIN_TRAY_SCROLL_L;
    }
  }else{
    if( abs >= syswk->trayMax/2 ){
      return BOX2MAIN_TRAY_SCROLL_L;
    }else{
      return BOX2MAIN_TRAY_SCROLL_R;
    }
  }
  return BOX2MAIN_TRAY_SCROLL_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ��p�{�^�����������Ƃ��̃J�[�\���\���؂�ւ�
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_ChgCursorButton( BOX2_SYS_WORK * syswk )
{
  GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
  if( CURSORMOVE_CursorOnOffGet( syswk->app->cmwk ) == FALSE ){
    CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
  }
}


//============================================================================================
//  �|�P�����f�[�^
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief   �|�P�����f�[�^�擾
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param   pos     �ʒu
 * @param   tray    �g���C
 * @param   prm     �擾�p�����[�^
 * @param   buf     �擾�o�b�t�@
 *
 * @return  �擾�f�[�^
 */
//--------------------------------------------------------------------------------------------
u32 BOX2MAIN_PokeParaGet( BOX2_SYS_WORK * syswk, u16 pos, u16 tray, int prm, void * buf )
{
  // �莝��
  if( pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
    if( pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
      pos -= BOX2OBJ_POKEICON_TRAY_MAX;
    }
    if( PokeParty_GetPokeCount( syswk->dat->pokeparty ) > pos ){
      POKEMON_PARAM * pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, pos );
      return PP_Get( pp, prm, buf );
    }
  // �g���C
  }else if( pos != BOX2MAIN_GETPOS_NONE ){
    POKEMON_PASO_PARAM * ppp = BOXDAT_GetPokeDataAddress( syswk->dat->sv_box, tray, pos );
    if( ppp != NULL ){
      return PPP_Get( ppp, prm, buf );
    }
  }

  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �|�P�����f�[�^�ݒ�
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param   pos     �ʒu
 * @param   prm     �ݒ�p�����[�^
 * @param   buf     �ݒ�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_PokeParaPut( BOX2_SYS_WORK * syswk, u32 pos, u32 tray, int prm, u32 buf )
{
  POKEMON_PASO_PARAM * ppp = BOX2MAIN_PPPGet( syswk, tray, pos );

  if( ppp == NULL ){
    return;
  }
  PPP_Put( ppp, prm, buf );
/*
  if( pos < BOX2OBJ_POKEICON_TRAY_MAX ){
    BOXDAT_SetTrayUseBit( syswk->dat->sv_box, tray );
  }
*/
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   POKEMON_PASO_PARAM�擾
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param   tray    �g���C�ԍ�
 * @param   pos     �ʒu
 *
 * @return  POKEMON_PASO_PARAM
 */
//--------------------------------------------------------------------------------------------
POKEMON_PASO_PARAM * BOX2MAIN_PPPGet( BOX2_SYS_WORK * syswk, u32 tray, u32 pos )
{
  POKEMON_PASO_PARAM * ppp;

  // �莝��
  if( tray == BOX2MAIN_PPP_GET_MINE || pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
    if( pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
      pos -= BOX2OBJ_POKEICON_TRAY_MAX;
    }
    if( PokeParty_GetPokeCount( syswk->dat->pokeparty ) > pos ){
      return PP_GetPPPPointer( PokeParty_GetMemberPointer(syswk->dat->pokeparty,pos) );
    }
  // �g���C
  }else if( pos != BOX2MAIN_GETPOS_NONE ){
    return BOXDAT_GetPokeDataAddress( syswk->dat->sv_box, tray, pos );
  }

  return NULL;
}

// �|�P�����f�[�^�擾
/*
void BOX2MAIN_GetPokeData( BOX2_SYS_WORK * syswk, u32 tray, u32 pos )
{
  // �g���C
  if( pos < BOX2OBJ_POKEICON_TRAY_MAX ){
    syswk->getPP = PP_CreateByPPP( BOXDAT_GetPokeDataAddress(syswk->dat->sv_box,tray,pos), HEAPID_BOX_SYS );
    BOXDAT_ClearPokemon( syswk->dat->sv_box, tray, pos );
  // �莝��
  }else{
    POKEMON_PARAM * pp;
    pos -= BOX2OBJ_POKEICON_TRAY_MAX;
    pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, pos );
    syswk->getPP = PP_CreateByPPP( PP_GetPPPPointerConst(pp), HEAPID_BOX_SYS );
    PokeParty_Delete( syswk->dat->pokeparty, pos );
  }
}
*/

// �擾�����|�P�����f�[�^��z�u
/*
void BOX2MAIN_PutPokeData( BOX2_SYS_WORK * syswk, u32 tray, u32 pos )
{
  POKEMON_PARAM * pp;

  // �g���C
  if( pos < BOX2OBJ_POKEICON_TRAY_MAX ){
    if( BOX2MAIN_PokeParaGet( syswk, pos, tray, ID_PARA_poke_exist, NULL ) == 0 ){
      BOXDAT_PutPokemonPos( syswk->dat->sv_box, tray, pos, PP_GetPPPPointer(syswk->getPP) );
      GFL_HEAP_FreeMemory( syswk->getPP );
    }else{
      pp = PP_CreateByPPP( BOXDAT_GetPokeDataAddress(syswk->dat->sv_box,tray,pos), HEAPID_BOX_APP );
      BOXDAT_PutPokemonPos( syswk->dat->sv_box, tray, pos, PP_GetPPPPointer(syswk->getPP) );
      GFL_HEAP_FreeMemory( syswk->getPP );
      syswk->getPP = PP_CreateByPPP( PP_GetPPPPointerConst(pp), HEAPID_BOX_SYS );
      GFL_HEAP_FreeMemory( pp );
    }
  // �莝��
  }else if( pos < BOX2OBJ_POKEICON_PUT_MAX ){
    int max = PokeParty_GetPokeCount( syswk->dat->pokeparty );
    pos -= BOX2OBJ_POKEICON_TRAY_MAX;
    if( pos < max ){
      pp = PP_CreateByPPP( PP_GetPPPPointerConst(PokeParty_GetMemberPointer(syswk->dat->pokeparty,pos)), HEAPID_BOX_APP );
      PokeParty_SetMemberData( syswk->dat->pokeparty, pos, syswk->getPP );
      GFL_HEAP_FreeMemory( syswk->getPP );
      syswk->getPP = PP_CreateByPPP( PP_GetPPPPointerConst(pp), HEAPID_BOX_SYS );
      GFL_HEAP_FreeMemory( pp );
    }else{
      PokeParty_Add( syswk->dat->pokeparty, syswk->getPP );
      GFL_HEAP_FreeMemory( syswk->getPP );
    }
  // ���g���C
  }else{
    pos = syswk->box_mv_pos + pos - BOX2OBJ_POKEICON_PUT_MAX;
    if( pos >= syswk->trayMax ){
      pos -= syswk->trayMax;
    }
    BOXDAT_PutPokemonBox( syswk->dat->sv_box, pos, PP_GetPPPPointer(syswk->getPP) );
    GFL_HEAP_FreeMemory( syswk->getPP );
  }
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����f�[�^�N���A
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param tray  �g���C�ԍ�
 * @param pos   �ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_PokeDataClear( BOX2_SYS_WORK * syswk, u32 tray, u32 pos )
{
  if( pos < BOX2OBJ_POKEICON_TRAY_MAX ){
    BOXDAT_ClearPokemon( syswk->dat->sv_box, tray, pos );
  }else{
    pos -= BOX2OBJ_POKEICON_TRAY_MAX;
    PokeParty_Delete( syswk->dat->pokeparty, pos );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�Ǝ莝���̃f�[�^�����ւ���
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param box   �{�b�N�X�|�P�����̈ړ��f�[�^
 * @param party �莝���|�P�����̈ړ��f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeDataChangeBoxParty(
        BOX2_SYS_WORK * syswk, BOX2MAIN_POKEMOVE_DATA * box, BOX2MAIN_POKEMOVE_DATA * party )
{
  POKEMON_PARAM * box_pp;
  POKEMON_PARAM * party_pp;
  u16 tray;
  u16 party_pos;

  // �{�b�N�X����擾�����ꍇ
  if( box->df_pos == BOX2OBJ_POKEICON_GET_POS ){
    tray = syswk->get_tray;
  }else{
    tray = syswk->tray;
  }

  // �{�b�N�X�f�[�^�ޔ�
  box_pp = PP_CreateByPPP( BOX2MAIN_PPPGet(syswk,tray,box->gt_pos), HEAPID_BOX_APP_L );

  // �莝���f�[�^���{�b�N�X�Ɉړ�
  party_pos = party->gt_pos - BOX2OBJ_POKEICON_TRAY_MAX;
  party_pp  = PokeParty_GetMemberPointer( syswk->dat->pokeparty, party_pos );
//  BOXDAT_PutPokemonPos( syswk->dat->sv_box, tray, party->mv_pos, PP_GetPPPPointerConst(party_pp) );
  BOXDAT_PutPokemonPos( syswk->dat->sv_box, tray, box->gt_pos, PP_GetPPPPointerConst(party_pp) );

  // �{�b�N�X�f�[�^���莝���Ɉړ�
  PokeParty_SetMemberData( syswk->dat->pokeparty, party_pos, box_pp );

  GFL_HEAP_FreeMemory( box_pp );

  // �g���C�A�C�R���X�V
  BOX2OBJ_TrayIconCgxMake( syswk, tray );
//  BOX2OBJ_TrayIconCgxTrans( syswk, tray );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X����莝���ֈړ�
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param box   �{�b�N�X�|�P�����̈ړ��f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeDataMoveBOXtoPARTY( BOX2_SYS_WORK * syswk, BOX2MAIN_POKEMOVE_DATA * box )
{
  POKEMON_PARAM * pp;

  // �{�b�N�X�f�[�^����
  pp = PP_CreateByPPP( BOX2MAIN_PPPGet(syswk,syswk->get_tray,box->gt_pos), HEAPID_BOX_APP_L );

  // �莝����
  PokeParty_Add( syswk->dat->pokeparty, pp );
  // �{�b�N�X�f�[�^���폜
  BOX2MAIN_PokeDataClear( syswk, syswk->get_tray, box->gt_pos );

  GFL_HEAP_FreeMemory( pp );

  // �g���C�A�C�R���X�V
  BOX2OBJ_TrayIconCgxMake( syswk, syswk->get_tray );
//  BOX2OBJ_TrayIconCgxTrans( syswk, syswk->get_tray );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝������{�b�N�X�ֈړ�
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param party �莝���|�P�����̈ړ��f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeDataMovePARTYtoBOX( BOX2_SYS_WORK * syswk, BOX2MAIN_POKEMOVE_DATA * party )
{
  POKEMON_PARAM * pp;
  u32 party_pos;

  // �莝���f�[�^����
  party_pos = party->gt_pos - BOX2OBJ_POKEICON_TRAY_MAX;
  pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, party_pos );

  // �{�b�N�X��
  BOXDAT_PutPokemonPos(
    syswk->dat->sv_box, syswk->tray, party->mv_pos, PP_GetPPPPointerConst(pp) );
  // �莝���f�[�^�폜
  BOX2MAIN_PokeDataClear( syswk, syswk->tray, party->gt_pos );

  // �g���C�A�C�R���X�V
  BOX2OBJ_TrayIconCgxMake( syswk, syswk->tray );
//  BOX2OBJ_TrayIconCgxTrans( syswk, syswk->tray );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝�����m�̓���ւ�
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param party �擾�莝���|�P�����̈ړ��f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeDataChangeParty( BOX2_SYS_WORK * syswk, BOX2MAIN_POKEMOVE_DATA * party )
{
/*
  u32 pos1, pos2;
  PM_CONDITION  cond1;
  PM_CONDITION  cond2;

  pos1 = party->df_pos - BOX2OBJ_POKEICON_TRAY_MAX;
  pos2 = party->mv_pos - BOX2OBJ_POKEICON_TRAY_MAX;

  // �f�[�^�擾
  PokeCopyPPtoPP( PokeParty_GetMemberPointer(syswk->dat->pokeparty,pos1), party->pp );
  PokeParty_GetPokeCondition( syswk->dat->pokeparty, &cond1,pos1 );
  PokeParty_GetPokeCondition( syswk->dat->pokeparty, &cond2,pos2 );
  // �f�[�^�}��
  PokeParty_SetMemberData( syswk->dat->pokeparty, pos1, PokeParty_GetMemberPointer(syswk->dat->pokeparty,pos2) );
  PokeParty_SetMemberData( syswk->dat->pokeparty, pos2, party->pp );
  //�����o�[����(SetMemberData)�ŃR���f�B�V������������Ă��܂��̂ŁA�R���f�B�V����������ւ���
  PokeParty_SetPokeCondition( syswk->dat->pokeparty, &cond2,pos1 );
  PokeParty_SetPokeCondition( syswk->dat->pokeparty, &cond1,pos2 );
*/
  PokeParty_ExchangePosition(
    syswk->dat->pokeparty,
    party->gt_pos - BOX2OBJ_POKEICON_TRAY_MAX,
    party->mv_pos - BOX2OBJ_POKEICON_TRAY_MAX,
    HEAPID_BOX_APP );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���ړ�
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param party �莝���|�P�����̈ړ��f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeDataMoveParty( BOX2_SYS_WORK * syswk, BOX2MAIN_POKEMOVE_DATA * party )
{
/*
  u32 pos;
  PM_CONDITION  cond;

  pos = party->df_pos - BOX2OBJ_POKEICON_TRAY_MAX;

  PokeCopyPPtoPP( PokeParty_GetMemberPointer(syswk->dat->pokeparty,pos), party->pp );
  PokeParty_GetPokeCondition( syswk->dat->pokeparty, &cond, pos );
  BOX2MAIN_PokeDataClear( syswk, syswk->tray, party->df_pos );
  PokeParty_Add( syswk->dat->pokeparty, party->pp );
  //�R���f�B�V�������Đݒ� BTS4981�̃o�O�C��
  {
    int pos_tail  = PokeParty_GetPokeCount(syswk->dat->pokeparty)-1;//����
    PokeParty_SetPokeCondition( syswk->dat->pokeparty, &cond, pos_tail );
  }
*/
  u32 pos;
  u32 i;

  pos = party->gt_pos - BOX2OBJ_POKEICON_TRAY_MAX;

  for( i=pos; i<PokeParty_GetPokeCount(syswk->dat->pokeparty)-1; i++ ){
    PokeParty_ExchangePosition( syswk->dat->pokeparty, i, i+1, HEAPID_BOX_APP );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�̃f�[�^�𑼂̃{�b�N�X��
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param box   �{�b�N�X�|�P�����̈ړ��f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeDataBoxMoveBox( BOX2_SYS_WORK * syswk, u32 getTray, u32 getPos, u32 putPos )
{
  POKEMON_PASO_PARAM * ppp;
  u32 tray;

  ppp  = BOX2MAIN_PPPGet( syswk, getTray, getPos );
  tray = MoveBoxGet( syswk, putPos );
  BOXDAT_PutPokemonBox( syswk->dat->sv_box, tray, ppp );
  BOXDAT_ClearPokemon( syswk->dat->sv_box, getTray, getPos );

  // �g���C�A�C�R���X�V
  BOX2OBJ_TrayIconCgxMake( syswk, getTray );
  BOX2OBJ_TrayIconCgxTrans( syswk, getTray );
  BOX2OBJ_TrayIconCgxMake( syswk, tray );
  BOX2OBJ_TrayIconCgxTrans( syswk, tray );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝�����w��̃{�b�N�X��
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param party �莝���|�P�����̈ړ��f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeDataBoxMoveParty( BOX2_SYS_WORK * syswk, u32 getPos, u32 putPos )
{
  POKEMON_PARAM * pp;
  u32 tray;

  tray = MoveBoxGet( syswk, putPos );

  pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, getPos-BOX2OBJ_POKEICON_TRAY_MAX );

  // �V�F�C�~�Ȃ�t�H�����`�F���W
  if( PP_Get( pp, ID_PARA_monsno, NULL ) == MONSNO_SHEIMI ){
    PP_ChangeFormNo( pp, FORMNO_SHEIMI_LAND );
  }

  BOXDAT_PutPokemonBox( syswk->dat->sv_box, tray, PP_GetPPPPointerConst(pp) );
  BOX2MAIN_PokeDataClear( syswk, syswk->tray, getPos );

  // �g���C�A�C�R���X�V
  BOX2OBJ_TrayIconCgxMake( syswk, tray );
  BOX2OBJ_TrayIconCgxTrans( syswk, tray );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����f�[�^�ړ�
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_PokeDataMove( BOX2_SYS_WORK * syswk )
{
  BOX2MAIN_POKEMOVE_WORK * work;
  BOX2MAIN_POKEMOVE_DATA * dat;
  u16 ppcnt;
  u8  tray;
//  u32 i;

  work = syswk->app->vfunk.work;
  dat  = work->dat;
  tray = syswk->get_tray;

//  syswk->get_tray = BOX2MAIN_GETPOS_NONE;

  // �z�u�G���A�O or �����ʒu
  if( work->get_pos == BOX2MAIN_GETPOS_NONE ){
    return;
  }

  ppcnt = PokeParty_GetPokeCount( syswk->dat->pokeparty );

/*
  if( ( work->put_pos & BOX2MAIN_BOXMOVE_FLG ) != 0 ){
    u32 tray = MoveBoxGet( syswk, work->put_pos );
    if( work->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
      PokeDataBoxMoveBox( syswk, &dat[0] );
    }else{
      PokeDataBoxMoveParty( syswk, &dat[work->get_pos-BOX2OBJ_POKEICON_TRAY_MAX] );
    }
    return;
  }
*/
  // �g���C�A�C�R����
  if( work->put_pos >= BOX2OBJ_POKEICON_PUT_MAX ){
    if( work->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
      PokeDataBoxMoveBox( syswk, syswk->get_tray, work->get_pos, work->put_pos-BOX2OBJ_POKEICON_PUT_MAX );
    }else{
      PokeDataBoxMoveParty( syswk, work->get_pos, work->put_pos-BOX2OBJ_POKEICON_PUT_MAX );
    }
    return;
  }

  if( work->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
    if( work->put_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
      // �{�b�N�X���m�̓���ւ�
      BOXDAT_ChangePokeData( syswk->dat->sv_box, syswk->get_tray, work->get_pos, syswk->tray, work->put_pos );
      // �g���C�A�C�R���X�V
      BOX2OBJ_TrayIconCgxMake( syswk, syswk->tray );
      BOX2OBJ_TrayIconCgxTrans( syswk, syswk->tray );
      if( syswk->tray != syswk->get_tray ){
        BOX2OBJ_TrayIconCgxMake( syswk, syswk->get_tray );
        BOX2OBJ_TrayIconCgxTrans( syswk, syswk->get_tray );
      }
    }else{
      if( ( work->put_pos - BOX2OBJ_POKEICON_TRAY_MAX ) < ppcnt ){
        // �{�b�N�X�E�莝���Ԃ̓���ւ�
        PokeDataChangeBoxParty( syswk, &dat[0], &dat[1] );
      }else{
        // �{�b�N�X����莝���ֈړ�
        PokeDataMoveBOXtoPARTY( syswk, &dat[0] );
      }
    }
  }else{
    if( work->put_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
      // ���݃`�F�b�N
      if( BOX2MAIN_PokeParaGet( syswk, work->put_pos, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
        // �莝���E�{�b�N�X�Ԃ̓���ւ�
        PokeDataChangeBoxParty( syswk, &dat[1], &dat[0] );
      }else{
        // �莝������{�b�N�X�ֈړ�
        PokeDataMovePARTYtoBOX( syswk, &dat[work->get_pos-BOX2OBJ_POKEICON_TRAY_MAX] );
      }
    }else{
      if( ( work->put_pos - BOX2OBJ_POKEICON_TRAY_MAX ) < ppcnt ){
        // �莝�����m�̓���ւ�
        PokeDataChangeParty( syswk, &dat[0] );
      }else{
        // �莝�����ړ�
        PokeDataMoveParty( syswk, &dat[work->get_pos-BOX2OBJ_POKEICON_TRAY_MAX] );
      }
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �ړ�����{�b�N�X���擾
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param pos   �\���{�b�N�X�ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static u32 MoveBoxGet( BOX2_SYS_WORK * syswk, u32 pos )
{
//  return ( BOX2MAIN_MV_TRAY_DEF(syswk->box_mv_pos) + (pos&0x7f) );
  pos += syswk->box_mv_pos;
  if( pos >= syswk->trayMax ){
    pos -= syswk->trayMax;
  }
  return pos;
}

//--------------------------------------------------------------------------------------------
/**
 * �w��ʒu�ȊO�ɐ킦��|�P���������邩
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param pos   �ΏۊO�̃|�P�����ʒu
 *
 * @retval  "TRUE = ����"
 * @retval  "FALSE = ���Ȃ�"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2MAIN_BattlePokeCheck( BOX2_SYS_WORK * syswk, u32 pos )
{
  POKEMON_PARAM * pp;
  u32 i;

  for( i=0; i<PokeParty_GetPokeCount(syswk->dat->pokeparty); i++ ){
    if( i == pos ){ continue; }
    pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, i );
    if( PP_Get( pp, ID_PARA_tamago_flag, NULL ) == 0 &&
      PP_Get( pp, ID_PARA_hp, NULL ) != 0 ){
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �t�H�����`�F���W
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param   ppp     POKEMON_PASO_PARAM
 * @param   form    �t�H�����ԍ�
 *
 * @retval  none
 */
//--------------------------------------------------------------------------------------------
static void ChangePokeForm( BOX2_SYS_WORK * syswk, POKEMON_PASO_PARAM * ppp, u16 form )
{
  POKEMON_PARAM * pp;

  PPP_ChangeFormNo( ppp, form );
  pp = PP_CreateByPPP( ppp, HEAPID_BOX_APP );
  ZUKANSAVE_SetPokeSee( GAMEDATA_GetZukanSave(syswk->dat->gamedata), pp );
  GFL_HEAP_FreeMemory( pp );
}

//--------------------------------------------------------------------------------------------
/**
 * �������ύX�ɂ��t�H�����`�F���W
 *
 * @param ppp   POKEMON_PASO_PARAM
 *
 * @retval  "TRUE = �t�H�����`�F���W"
 * @retval  "FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2MAIN_PokeItemFormChange( BOX2_SYS_WORK * syswk, POKEMON_PASO_PARAM * ppp )
{
  u16 bf, af;
  u16 mons;
  u16 item;

  mons = PPP_Get( ppp, ID_PARA_monsno, NULL );

  // �A���Z�E�X
  if( mons == MONSNO_ARUSEUSU ){
    item = PPP_Get( ppp, ID_PARA_item, NULL );
    bf = PPP_Get( ppp, ID_PARA_form_no, NULL );
    af = POKETOOL_GetPokeTypeFromItem( item );
    if( bf != af ){
//      PPP_ChangeFormNo( ppp, af );
      ChangePokeForm( syswk, ppp, af );
    }
    if( bf != af ){ return TRUE; }
  // �M���e�B�i
  }else if( mons == MONSNO_GIRATHINA ){
    item = PPP_Get( ppp, ID_PARA_item, NULL );
    bf = PPP_Get( ppp, ID_PARA_form_no, NULL );
    af = bf;
    if( bf == FORMNO_GIRATHINA_ANOTHER ){
      if( item == ITEM_HAKKINDAMA ){
//        PPP_ChangeFormNo( ppp, FORMNO_GIRATHINA_ORIGIN );
        ChangePokeForm( syswk, ppp, FORMNO_GIRATHINA_ORIGIN );
        af = FORMNO_GIRATHINA_ORIGIN;
      }
    }else{
      if( item != ITEM_HAKKINDAMA ){
//        PPP_ChangeFormNo( ppp, FORMNO_GIRATHINA_ANOTHER );
        ChangePokeForm( syswk, ppp, FORMNO_GIRATHINA_ANOTHER );
        af = FORMNO_GIRATHINA_ANOTHER;
      }
    }
    if( bf != af ){ return TRUE; }
  }

  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �莝������{�b�N�X�Ɉړ��������Ƃ��̃V�F�C�~�̃t�H�����`�F���W
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param b_pos �ړ��O�̈ʒu
 * @param a_pos �ړ���̈ʒu
 *
 * @return  none
 *
 *  �f�[�^�͓���ւ������ƂɌĂ�
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_FormChangeSheimi( BOX2_SYS_WORK * syswk, u32 b_pos, u32 a_pos )
{
  POKEMON_PASO_PARAM * ppp;
  u32 pos;

  // �{�b�N�X���m�A�莝�����m�̏ꍇ
  if( ( b_pos < BOX2OBJ_POKEICON_TRAY_MAX && a_pos < BOX2OBJ_POKEICON_TRAY_MAX ) ||
    ( b_pos >= BOX2OBJ_POKEICON_TRAY_MAX && a_pos >= BOX2OBJ_POKEICON_TRAY_MAX ) ){
    return;
  }

  // �{�b�N�X�z�u�ʒu
  if( b_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
    pos = b_pos;
  }else{
    pos = a_pos;
  }

  // �V�F�C�~�ȊO
  if( BOX2MAIN_PokeParaGet( syswk, pos, syswk->tray, ID_PARA_monsno, NULL ) != MONSNO_SHEIMI ){
    return;
  }
  // �����h�t�H����
  if( BOX2MAIN_PokeParaGet( syswk, pos, syswk->tray, ID_PARA_form_no, NULL ) == FORMNO_SHEIMI_LAND ){
    return;
  }

  ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, pos );

//  PPP_ChangeFormNo( ppp, FORMNO_SHEIMI_LAND );
  ChangePokeForm( syswk, ppp, FORMNO_SHEIMI_LAND );

  // �|�P�����A�C�R���؂�ւ�
  BOX2OBJ_PokeIconChange( syswk, syswk->tray, pos, syswk->app->pokeicon_id[pos] );

  // ���ʕ\���ؑ�
  if( syswk->get_pos == pos ){
    BOX2MAIN_PokeInfoPut( syswk, pos );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �n���}�Ӕԍ��e�[�u���ǂݍ���
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_LoadLocalNoList( BOX2_SYS_WORK * syswk )
{
  syswk->app->localNo     = POKE_PERSONAL_GetZenkokuToChihouArray( HEAPID_BOX_APP, NULL );
  syswk->app->zenkokuFlag = ZUKANSAVE_GetZenkokuZukanFlag(
                              GAMEDATA_GetZukanSave(syswk->dat->gamedata));
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �n���}�Ӕԍ��e�[�u�����
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_FreeLocalNoList( BOX2_SYS_WORK * syswk )
{
  GFL_HEAP_FreeMemory( syswk->app->localNo );
}


//============================================================================================
//  �|�P�����A�C�R��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R������ւ�����
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "TRUE = ���쒆"
 * @retval  "FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
static BOOL PokeIconObjMove( BOX2_SYS_WORK * syswk )
{
  BOX2MAIN_POKEMOVE_WORK * work;
  BOX2MAIN_POKEMOVE_DATA * dat;
  u32 i;
  u32 mvID;
  s16 px, py;

  work = syswk->app->vfunk.work;
  dat  = work->dat;

  if( work->cnt == POKEICON_MOVE_CNT ){
    for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
      if( dat[i].flg == 0 ){ continue; }
//      BOX2OBJ_PokeIconPriChg( syswk->app, dat[i].df_pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
      BOX2OBJ_PokeIconPriChg2( syswk->app, dat[i].df_pos, dat[i].mv_pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
      mvID = syswk->app->pokeicon_id[dat[i].df_pos];
      if( dat[i].mv_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
        BOX2OBJ_PokeIconDefaultPosGet( dat[i].mv_pos, &px, &py, syswk->move_mode );
      }else if( dat[i].mv_pos < BOX2OBJ_POKEICON_PUT_MAX ){
        BOX2OBJ_PokeIconDefaultPosGet( dat[i].mv_pos, &px, &py, syswk->move_mode );
        if( BOX2BGWFRM_CheckPartyPokeFrameRight(syswk->app->wfrm) == FALSE &&
            BOX2BGWFRM_CheckPartyPokeFrameLeft(syswk->app->wfrm) == FALSE ){
          py += 192;
        }
      }else{
        if( dat[i].df_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
          BOX2OBJ_PokeIconDefaultPosGet( BOX2OBJ_POKEICON_MAX-1, &px, &py, syswk->move_mode );
        }else{
          BOX2OBJ_PokeIconDefaultPosGet( dat[i].df_pos, &px, &py, syswk->move_mode );
        }
        BOX2OBJ_Vanish( syswk->app, mvID, FALSE );
      }
      BOX2OBJ_SetPos( syswk->app, mvID, px, py, CLSYS_DEFREND_MAIN );
      if( dat[i].flg == 2 ){
        BOX2OBJ_Vanish( syswk->app, mvID, FALSE );
      }
    }
    BOX2OBJ_ChgPokeCursorPriority( syswk, syswk->get_pos );
    BOX2OBJ_PokeCursorMove( syswk->app, BOX2OBJ_POKEICON_GET_POS );
    return FALSE;
  }else{
    work->cnt++;
    for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
//      if( dat[i].flg != 1 ){ continue; }
      if( dat[i].flg == 0 ){ continue; }
      mvID = syswk->app->pokeicon_id[dat[i].df_pos];
      px = dat[i].dx + ( ( dat[i].mx * work->cnt ) >> 16 ) * dat[i].vx;
      py = dat[i].dy + ( ( dat[i].my * work->cnt ) >> 16 ) * dat[i].vy;
      BOX2OBJ_SetPos( syswk->app, mvID, px, py, CLSYS_DEFREND_MAIN );
      BOX2OBJ_PokeIconPriChg( syswk->app, dat[i].df_pos, BOX2OBJ_POKEICON_PRI_CHG_GET );
    }
    BOX2OBJ_ChgPokeCursorPriority( syswk, syswk->get_pos );
    BOX2OBJ_PokeCursorMove( syswk->app, BOX2OBJ_POKEICON_GET_POS );
  }
  return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���ړ�����i�h���b�v�j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "TRUE = ���쒆"
 * @retval  "FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
static BOOL PokeIconObjDrop( BOX2_SYS_WORK * syswk )
{
  BOX2MAIN_POKEMOVE_WORK * work;
  BOX2MAIN_POKEMOVE_DATA * dat;
  u32 i;
  u32 mvID;
  s16 px, py;

  work = syswk->app->vfunk.work;
  dat  = work->dat;

  if( work->cnt == POKEICON_MOVE_CNT ){
    for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
      if( dat[i].flg == 0 ){ continue; }

      mvID = syswk->app->pokeicon_id[dat[i].df_pos];
      BOX2OBJ_PokeIconDefaultPosGet( dat[i].mv_pos, &px, &py, syswk->move_mode );
//      if( syswk->get_pos != dat[i].mv_pos ){
      if( dat[i].mv_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
        py = py + BOX2MAIN_PARTYPOKE_FRM_SPD * BOX2MAIN_PARTYPOKE_FRM_CNT;
      }
      BOX2OBJ_SetPos( syswk->app, mvID, px, py, CLSYS_DEFREND_MAIN );
    }
    BOX2OBJ_PokeCursorMove( syswk->app, BOX2OBJ_POKEICON_GET_POS );
    return FALSE;
/*
    for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
      if( dat[i].flg == 0 ){ continue; }
//      BOX2OBJ_PokeIconPriChg( syswk->app, dat[i].df_pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
      BOX2OBJ_PokeIconPriChg2( syswk->app, dat[i].df_pos, dat[i].mv_pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
      mvID = syswk->app->pokeicon_id[dat[i].df_pos];
      if( dat[i].mv_pos < BOX2OBJ_POKEICON_PUT_MAX ){
        BOX2OBJ_PokeIconDefaultPosGet( dat[i].mv_pos, &px, &py, syswk->move_mode );
      }else{
        if( dat[i].df_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
          BOX2OBJ_PokeIconDefaultPosGet( BOX2OBJ_POKEICON_MAX-1, &px, &py, syswk->move_mode );
        }else{
          BOX2OBJ_PokeIconDefaultPosGet( dat[i].df_pos, &px, &py, syswk->move_mode );
        }
        BOX2OBJ_Vanish( syswk->app, mvID, FALSE );
      }
      BOX2OBJ_SetPos( syswk->app, mvID, px, py, CLSYS_DEFREND_MAIN );
      if( dat[i].flg == 2 ){
        BOX2OBJ_Vanish( syswk->app, mvID, FALSE );
      }
    }
*/
  }else{
    work->cnt++;
    for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
      if( dat[i].flg == 0 ){ continue; }
//      if( syswk->get_pos != dat[i].mv_pos ){ continue; }

      mvID = syswk->app->pokeicon_id[dat[i].df_pos];
      px = dat[i].dx + ( ( dat[i].mx * work->cnt ) >> 16 ) * dat[i].vx;
      py = dat[i].dy + ( ( dat[i].my * work->cnt ) >> 16 ) * dat[i].vy;
      BOX2OBJ_SetPos( syswk->app, mvID, px, py, CLSYS_DEFREND_MAIN );
      BOX2OBJ_PokeIconPriChg( syswk->app, dat[i].df_pos, BOX2OBJ_POKEICON_PRI_CHG_GET );
      break;
    }
    BOX2OBJ_ChgPokeCursorPriority( syswk, syswk->get_pos );
    BOX2OBJ_PokeCursorMove( syswk->app, BOX2OBJ_POKEICON_GET_POS );
  }
  return TRUE;

/*
  if( work->cnt == POKEICON_MOVE_CNT ){
    for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
      if( dat[i].flg == 0 ){ continue; }
//      BOX2OBJ_PokeIconPriChg( syswk->app, dat[i].df_pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
      BOX2OBJ_PokeIconPriChg2( syswk->app, dat[i].df_pos, dat[i].mv_pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
      mvID = syswk->app->pokeicon_id[dat[i].df_pos];
      if( dat[i].mv_pos < BOX2OBJ_POKEICON_PUT_MAX ){
        BOX2OBJ_PokeIconDefaultPosGet( dat[i].mv_pos, &px, &py, syswk->move_mode );
      }else{
        if( dat[i].df_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
          BOX2OBJ_PokeIconDefaultPosGet( BOX2OBJ_POKEICON_MAX-1, &px, &py, syswk->move_mode );
        }else{
          BOX2OBJ_PokeIconDefaultPosGet( dat[i].df_pos, &px, &py, syswk->move_mode );
        }
        BOX2OBJ_Vanish( syswk->app, mvID, FALSE );
      }
      BOX2OBJ_SetPos( syswk->app, mvID, px, py, CLSYS_DEFREND_MAIN );
      if( dat[i].flg == 2 ){
        BOX2OBJ_Vanish( syswk->app, mvID, FALSE );
      }
    }
    return FALSE;
  }else{
    work->cnt++;
    for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
      if( dat[i].flg != 1 ){ continue; }
      mvID = syswk->app->pokeicon_id[dat[i].df_pos];
      px = dat[i].dx + ( ( dat[i].mx * work->cnt ) >> 16 ) * dat[i].vx;
      py = dat[i].dy + ( ( dat[i].my * work->cnt ) >> 16 ) * dat[i].vy;
      BOX2OBJ_SetPos( syswk->app, mvID, px, py, CLSYS_DEFREND_MAIN );
      BOX2OBJ_PokeIconPriChg( syswk->app, dat[i].df_pos, BOX2OBJ_POKEICON_PRI_CHG_GET );
    }
  }
  return TRUE;
*/

}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R������ւ�����i�h���b�v�j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "TRUE = ���쒆"
 * @retval  "FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
#if 0
static BOOL PokeIconObjDropChange( BOX2_SYS_WORK * syswk )
{
  BOX2MAIN_POKEMOVE_WORK * work;
  BOX2MAIN_POKEMOVE_DATA * dat;
  u32 i;
  u32 mvID;
  s16 px, py;

  work = syswk->app->vfunk.work;
  dat  = work->dat;

  if( work->cnt == POKEICON_MOVE_CNT ){
    for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
      if( dat[i].flg == 0 ){ continue; }
      mvID = syswk->app->pokeicon_id[dat[i].df_pos];

      BOX2OBJ_PokeIconDefaultPosGet( dat[i].mv_pos, &px, &py, syswk->move_mode );
      if( BOX2BGWFRM_CheckPartyPokeFrameRight(syswk->app->wfrm) == FALSE &&
          BOX2BGWFRM_CheckPartyPokeFrameLeft(syswk->app->wfrm) == FALSE ){
        py += 192;
      }
      BOX2OBJ_SetPos( syswk->app, mvID, px, py, CLSYS_DEFREND_MAIN );
    }
    return FALSE;
  }else{
    work->cnt++;
    for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
      if( dat[i].flg != 1 ){ continue; }
      mvID = syswk->app->pokeicon_id[dat[i].df_pos];
      px = dat[i].dx + ( ( dat[i].mx * work->cnt ) >> 16 ) * dat[i].vx;
      py = dat[i].dy + ( ( dat[i].my * work->cnt ) >> 16 ) * dat[i].vy;
      BOX2OBJ_SetPos( syswk->app, mvID, px, py, CLSYS_DEFREND_MAIN );
      BOX2OBJ_PokeIconPriChgDrop( syswk->app, dat[i].df_pos, dat[i].mv_pos );
    }
    BOX2OBJ_ChgPokeCursorPriority( syswk, syswk->get_pos );
  }
  return TRUE;

/*
  BOX2MAIN_POKEMOVE_WORK * work;
  BOX2MAIN_POKEMOVE_DATA * dat;
  u32 i;
  u32 mvID;
  s16 px, py;

  work = syswk->app->vfunk.work;
  dat  = work->dat;

  if( work->cnt == POKEICON_MOVE_CNT ){
    for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
      if( dat[i].flg == 0 ){ continue; }
      BOX2OBJ_PokeIconPriChg( syswk->app, dat[i].df_pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
      mvID = syswk->app->pokeicon_id[dat[i].df_pos];
      if( dat[i].mv_pos < BOX2OBJ_POKEICON_PUT_MAX ){
        BOX2OBJ_PokeIconDefaultPosGet( dat[i].mv_pos, &px, &py, syswk->move_mode );
      }else{
        if( dat[i].df_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
          BOX2OBJ_PokeIconDefaultPosGet( BOX2OBJ_POKEICON_MAX-1, &px, &py, syswk->move_mode );
        }else{
          BOX2OBJ_PokeIconDefaultPosGet( dat[i].df_pos, &px, &py, syswk->move_mode );
        }
        BOX2OBJ_Vanish( syswk->app, mvID, FALSE );
      }
      BOX2OBJ_SetPos( syswk->app, mvID, px, py, CLSYS_DEFREND_MAIN );
      if( dat[i].flg == 2 ){
        BOX2OBJ_Vanish( syswk->app, mvID, FALSE );
      }
    }
    return FALSE;
  }else{
    work->cnt++;
    for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
      if( dat[i].flg != 1 ){ continue; }
      mvID = syswk->app->pokeicon_id[dat[i].df_pos];
      px = dat[i].dx + ( ( dat[i].mx * work->cnt ) >> 16 ) * dat[i].vx;
      py = dat[i].dy + ( ( dat[i].my * work->cnt ) >> 16 ) * dat[i].vy;
      BOX2OBJ_SetPos( syswk->app, mvID, px, py, CLSYS_DEFREND_MAIN );
      BOX2OBJ_PokeIconPriChg( syswk->app, dat[i].df_pos, BOX2OBJ_POKEICON_PRI_CHG_GET );
    }
  }
  return TRUE;
*/

}
#endif

//--------------------------------------------------------------------------------------------
/**
 * �擾�|�P�������w��ꏊ�Ɉړ��ł��邩�`�F�b�N
 *
 * @param syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param get_pos   �擾�ꏊ
 * @param put_pos   �z�u�ʒu
 *
 * @retval  "TRUE = ��"
 * @retval  "FALSE = �s��"
 */
//--------------------------------------------------------------------------------------------
static BOOL PartyMoveCheck( BOX2_SYS_WORK * syswk, u32 get_pos, u32 put_pos )
{
  u32 exist;
  u32 tray;

  BOX2MAIN_PokeParaGet( syswk, put_pos, syswk->tray, ID_PARA_poke_exist, NULL );

/*
  if( ( put_pos & BOX2MAIN_BOXMOVE_FLG ) != 0 ){
    tray = MoveBoxGet( syswk, put_pos );
    if( tray == syswk->tray || BOXDAT_GetPokeExistCount( syswk->dat->sv_box, tray ) == BOX_MAX_POS ){
      return FALSE;
    }
    put_pos ^= BOX2MAIN_BOXMOVE_FLG;
    exist = 0;
  }else{
    tray  = syswk->tray;
    exist = BOX2MAIN_PokeParaGet( syswk, put_pos, syswk->tray, ID_PARA_poke_exist, NULL );
  }
*/
  // �g���C�A�C�R���֔z�u
  if( put_pos >= BOX2OBJ_POKEICON_PUT_MAX ){
    tray = MoveBoxGet( syswk, put_pos-BOX2OBJ_POKEICON_PUT_MAX );
    if( tray == syswk->tray || BOXDAT_GetPokeExistCount( syswk->dat->sv_box, tray ) == BOX_MAX_POS ){
      syswk->app->mv_err_code = BOX2MAIN_ERR_CODE_BOXMAX;
      return FALSE;
    }
    exist = 0;
  }else{
    exist = BOX2MAIN_PokeParaGet( syswk, put_pos, syswk->tray, ID_PARA_poke_exist, NULL );
  }

  // �o�g���{�b�N�X��p����
  if( syswk->dat->callMode == BOX_MODE_BATTLE ){
    // �o�g���{�b�N�X�����b�N����Ă���@�����ۂ͂����ɂ͂��Ȃ�
    if( syswk->dat->bbRockFlg == TRUE ){
      syswk->app->mv_err_code = BOX2MAIN_ERR_CODE_ROCK;
      return FALSE;
    }
    // �^�}�S���g���C����莝���ɔz�u���悤�Ƃ����Ƃ�
    if( get_pos < BOX2OBJ_POKEICON_TRAY_MAX &&
        put_pos >= BOX2OBJ_POKEICON_TRAY_MAX && put_pos < BOX2OBJ_POKEICON_PUT_MAX &&
        BOX2MAIN_PokeParaGet( syswk, get_pos, syswk->tray, ID_PARA_tamago_flag, NULL ) != 0 ){
      syswk->app->mv_err_code = BOX2MAIN_ERR_CODE_EGG;
      return FALSE;
    }
    // �p�[�e�B�擾���ɔz�u�ʒu�̃g���C�|�P�������^�}�S�Ȃ�
    if( get_pos >= BOX2OBJ_POKEICON_TRAY_MAX && get_pos < BOX2OBJ_POKEICON_PUT_MAX &&
        put_pos < BOX2OBJ_POKEICON_TRAY_MAX && exist != 0 &&
        BOX2MAIN_PokeParaGet( syswk, put_pos, syswk->tray, ID_PARA_tamago_flag, NULL ) != 0 ){
      syswk->app->mv_err_code = BOX2MAIN_ERR_CODE_EGG;
      return FALSE;
    }
  }

  // �莝������
  if( get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
    // �킦��|�P�������擾�����|�P�����̂�
    if( BOX2MAIN_BattlePokeCheck( syswk, get_pos-BOX2OBJ_POKEICON_TRAY_MAX ) == FALSE ){
      // �z�u�ʒu�Ƀ|�P���������Ȃ�
      if( exist == 0 ){
        // �z�u�ʒu���g���C���Ⴄ�g���C
        if( put_pos < BOX2OBJ_POKEICON_TRAY_MAX || put_pos >= BOX2OBJ_POKEICON_PUT_MAX ){
          if( syswk->dat->callMode != BOX_MODE_BATTLE ){
            syswk->app->mv_err_code = BOX2MAIN_ERR_CODE_BATTLE;
            return FALSE;
          }
        }
      // �z�u�ʒu�Ƀ|�P����������
      }else{
        // �z�u�ʒu���g���C�Ń|�P�������^�}�S
        if( BOX2MAIN_PokeParaGet( syswk, put_pos, syswk->tray, ID_PARA_tamago_flag, NULL ) != 0 &&
          put_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
          if( syswk->dat->callMode != BOX_MODE_BATTLE ){
            syswk->app->mv_err_code = BOX2MAIN_ERR_CODE_BATTLE;
            return FALSE;
          }
        }
      }
    }
    // �{�b�N�X�֔z�u��
    if( put_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
      POKEMON_PARAM * pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, get_pos-BOX2OBJ_POKEICON_TRAY_MAX );
      // ���[���������Ă���
      if( ITEM_CheckMail(PP_Get(pp,ID_PARA_item,NULL)) == TRUE ){
        syswk->app->mv_err_code = BOX2MAIN_ERR_CODE_MAIL;
        return FALSE;
      }
    }
  // �{�b�N�X����
  }else{
    // �莝���Ƃ��ꂩ��
    if( put_pos >= BOX2OBJ_POKEICON_TRAY_MAX && exist != 0 ){
      POKEMON_PARAM * pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, put_pos-BOX2OBJ_POKEICON_TRAY_MAX );
      // �z�u�ʒu�̃|�P���������[���������Ă���
      if( ITEM_CheckMail(PP_Get(pp,ID_PARA_item,NULL)) == TRUE ){
        syswk->app->mv_err_code = BOX2MAIN_ERR_CODE_MAIL;
        return FALSE;
      }
      // �擾�|�P�������^�}�S�ŁA�킦��|�P�������z�u�ʒu�̃|�P�����̂�
      if( BOX2MAIN_PokeParaGet( syswk, get_pos, syswk->tray, ID_PARA_tamago_flag, NULL ) != 0 &&
        BOX2MAIN_BattlePokeCheck( syswk, put_pos-BOX2OBJ_POKEICON_TRAY_MAX ) == FALSE ){
        if( syswk->dat->callMode != BOX_MODE_BATTLE ){
          syswk->app->mv_err_code = BOX2MAIN_ERR_CODE_BATTLE;
          return FALSE;
        }
      }
    }
  }

  syswk->app->mv_err_code = BOX2MAIN_ERR_CODE_NONE;

  return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�������g���C�̎w��ꏊ�Ɉړ��ł��邩�`�F�b�N�i����ւ��s�j
 *
 * @param syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param get_pos   �擾�ꏊ
 * @param put_pos   �z�u�ʒu
 *
 * @retval  "TRUE = ��"
 * @retval  "FALSE = �s��"
 */
//--------------------------------------------------------------------------------------------
static BOOL PokeDropCheck( BOX2_SYS_WORK * syswk, u32 get_pos, u32 put_pos )
{
  POKEMON_PARAM * pp;

  if( BOX2MAIN_PokeParaGet( syswk, put_pos, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
//    OS_Printf( "�u���Ȃ� : �z�u�ʒu�Ƀ|�P���������܂�\n" );
    return FALSE;
  }

  get_pos -= BOX2OBJ_POKEICON_TRAY_MAX;

  if( BOX2MAIN_BattlePokeCheck( syswk, get_pos ) == FALSE ){
//    OS_Printf( "�u���Ȃ� : �킦��|�P���������ꂾ��\n" );
    return FALSE;
  }

  pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, get_pos );

  if( ITEM_CheckMail( PP_Get(pp,ID_PARA_item,NULL) ) == TRUE ){
//    OS_Printf( "�u���Ȃ� : ���[���������Ă���\n" );
    return FALSE;
  }

  return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �擾�A�C�e�����w��ꏊ�̃|�P�����ɃZ�b�g�ł��邩�`�F�b�N
 *
 * @param syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param get_pos   �擾�ꏊ
 * @param put_pos   �z�u�ʒu
 *
 * @retval  "TRUE = ��"
 * @retval  "FALSE = �s��"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2MAIN_PokeItemMoveCheck( BOX2_SYS_WORK * syswk, u32 get_pos, u32 put_pos )
{
  u16 get_item, put_item;

  if( BOX2MAIN_PokeParaGet( syswk, put_pos, syswk->tray, ID_PARA_poke_exist, NULL ) == 0 ){
//    OS_Printf( "�u���Ȃ� : �z�u�ʒu�Ƀ|�P���������܂���\n" );
    return FALSE;
  }
  if( BOX2MAIN_PokeParaGet( syswk, put_pos, syswk->tray, ID_PARA_tamago_flag, NULL ) != 0 ){
//    OS_Printf( "�u���Ȃ� : �^�}�S�̓A�C�e�������ĂȂ�\n" );
    return FALSE;
  }

  get_item = BOX2MAIN_PokeParaGet( syswk,get_pos, syswk->tray, ID_PARA_item, NULL );

  if( ITEM_CheckMail(get_item) == TRUE ){
//    OS_Printf( "�u���Ȃ� : ���[���͈ړ��ł��Ȃ�\n" );
    return FALSE;
  }

  put_item = BOX2MAIN_PokeParaGet( syswk,put_pos, syswk->tray, ID_PARA_item, NULL );

  if( ITEM_CheckMail(put_item) == TRUE ){
//    OS_Printf( "�u���Ȃ� : ���[���������Ă���\n" );
    return FALSE;
  }

/*  ����́u�͂����񂾂܁v������������
  if( get_item == ITEM_HAKKINDAMA && 
    BOX2MAIN_PokeParaGet( syswk, put_pos, syswk->tray, ID_PARA_monsno, NULL ) != MONSNO_GIRATHINA ){
//    OS_Printf( "�u���Ȃ� : �͂����񂾂܂̓M���e�B�i�������ĂȂ�\n" );
    return FALSE;
  }

  if( put_item == ITEM_HAKKINDAMA && 
    BOX2MAIN_PokeParaGet( syswk, get_pos, syswk->tray, ID_PARA_monsno, NULL ) != MONSNO_GIRATHINA ){
//    OS_Printf( "�u���Ȃ� : �͂����񂾂܂̓M���e�B�i�������ĂȂ�\n" );
    return FALSE;
  }
*/

  return TRUE;
}


//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R������ւ��p�����[�^�쐬
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param dat   �ړ��f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconMoveParamMake( BOX2_SYS_WORK * syswk, BOX2MAIN_POKEMOVE_DATA * dat )
{
  u32 mvID;
  s16 px, py;

  mvID = syswk->app->pokeicon_id[dat->df_pos];

  BOX2OBJ_GetPos( syswk->app, mvID, &dat->dx, &dat->dy, CLSYS_DEFREND_MAIN );

  // �ʏ�
  if( dat->flg != 2 ){
//    if( ( dat->mv_pos & BOX2MAIN_BOXMOVE_FLG ) == 0 ){
    if( dat->mv_pos < BOX2OBJ_POKEICON_PUT_MAX ){
      BOX2OBJ_PokeIconDefaultPosGet( dat->mv_pos, &px, &py, syswk->move_mode );
    }else{
      BOX2OBJ_TrayIconPosGet( syswk->app, dat->mv_pos-BOX2OBJ_POKEICON_PUT_MAX, &px, &py );
    }
  // �z�u�悪�X�N���[�����ꂽ�ꏊ�̂Ƃ�
  }else{
    if( syswk->tray > syswk->get_tray ){
      // �����
      BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_L_ARROW, &px, &py, CLSYS_DEFREND_MAIN );
    }else{
      // �E���
      BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_R_ARROW, &px, &py, CLSYS_DEFREND_MAIN );
    }
  }

  if( dat->dx <= px ){
    dat->vx = 1;
    dat->mx = ( ( px - dat->dx ) << 16 ) / POKEICON_MOVE_CNT;
  }else{
    dat->vx = -1;
    dat->mx = ( ( dat->dx - px ) << 16 ) / POKEICON_MOVE_CNT;
  }
  if( dat->dy <= py ){
    dat->vy = 1;
    dat->my = ( ( py - dat->dy ) << 16 ) / POKEICON_MOVE_CNT;
  }else{
    dat->vy = -1;
    dat->my = ( ( dat->dy - py ) << 16 ) / POKEICON_MOVE_CNT;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R������ւ��p�����[�^�쐬�i�h���b�v�j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param dat   �ړ��f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconDropParamMake( BOX2_SYS_WORK * syswk, BOX2MAIN_POKEMOVE_DATA * dat )
{
  u32 dfID, mvID;
  s16 px, py;

  dfID = syswk->app->pokeicon_id[dat->df_pos];
  mvID = syswk->app->pokeicon_id[dat->mv_pos];

  BOX2OBJ_GetPos( syswk->app, dfID, &dat->dx, &dat->dy, CLSYS_DEFREND_MAIN );
  BOX2OBJ_GetPos( syswk->app, mvID, &px, &py, CLSYS_DEFREND_MAIN );

  if( dat->dx <= px ){
    dat->vx = 1;
    dat->mx = ( ( px - dat->dx ) << 16 ) / POKEICON_MOVE_CNT;
  }else{
    dat->vx = -1;
    dat->mx = ( ( dat->dx - px ) << 16 ) / POKEICON_MOVE_CNT;
  }
  if( dat->dy <= py ){
    dat->vy = 1;
    dat->my = ( ( py - dat->dy ) << 16 ) / POKEICON_MOVE_CNT;
  }else{
    dat->vy = -1;
    dat->my = ( ( dat->dy - py ) << 16 ) / POKEICON_MOVE_CNT;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R������ւ��p�����[�^�쐬�i�莝�����g���C�A�C�R���ɓ����Ƃ��j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param dat   �ړ��f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconMoveOutRangeParamMake( BOX2_SYS_WORK * syswk, BOX2MAIN_POKEMOVE_DATA * dat )
{
  u32 mvID;
  s16 px, py;

  mvID = syswk->app->pokeicon_id[dat->df_pos];

  BOX2OBJ_GetPos( syswk->app, mvID, &dat->dx, &dat->dy, CLSYS_DEFREND_MAIN );
  BOX2OBJ_PokeIconDefaultPosGet( dat->mv_pos, &px, &py, syswk->move_mode );
  py += 192;  // �P���ɉ�ʊO���Ă��Ƃ�

  if( dat->dx <= px ){
    dat->vx = 1;
    dat->mx = ( ( px - dat->dx ) << 16 ) / POKEICON_MOVE_CNT;
  }else{
    dat->vx = -1;
    dat->mx = ( ( dat->dx - px ) << 16 ) / POKEICON_MOVE_CNT;
  }
  if( dat->dy <= py ){
    dat->vy = 1;
    dat->my = ( ( py - dat->dy ) << 16 ) / POKEICON_MOVE_CNT;
  }else{
    dat->vy = -1;
    dat->my = ( ( dat->dy - py ) << 16 ) / POKEICON_MOVE_CNT;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * ����ւ��f�[�^�쐬
 *
 * @param syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param get_pos   �擾�ʒu
 * @param put_pos   �z�u�ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconChgDataMeke( BOX2_SYS_WORK * syswk, u32 get_pos, u32 put_pos )
{
  BOX2MAIN_POKEMOVE_WORK * work;
  BOX2MAIN_POKEMOVE_DATA * dat;

  work = syswk->app->vfunk.work;
  dat  = work->dat;

  dat[0].df_pos = BOX2OBJ_POKEICON_GET_POS;
//  dat[0].df_pos = get_pos;
  dat[0].mv_pos = put_pos;
  dat[0].gt_pos = get_pos;
  dat[0].flg    = 1;
  PokeIconMoveParamMake( syswk, &dat[0] );
  dat[1].df_pos = put_pos;
  dat[1].mv_pos = get_pos;
  dat[1].gt_pos = put_pos;
  dat[1].flg    = 1;
  PokeIconMoveParamMake( syswk, &dat[1] );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�R���ړ��f�[�^�쐬�i�ʏ�̓���ւ��j
 *
 * @param syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param get_pos   �擾�ʒu
 * @param put_pos   �z�u�ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconMoveBoxPartyDataMake( BOX2_SYS_WORK * syswk, u32 get_pos, u32 put_pos )
{
  BOX2MAIN_POKEMOVE_WORK * work;
  BOX2MAIN_POKEMOVE_DATA * dat;
  u32 get;
  u32 ppcnt;
  u32 i;

  work = syswk->app->vfunk.work;
  dat  = work->dat;

//  syswk->app->poke_mv_cur_flg = TRUE;

  // ������
  for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
    dat[i].df_pos = 0;
    dat[i].mv_pos = 0;
    dat[i].gt_pos = 0;
    dat[i].flg    = 0;
  }

  work->set_pos = put_pos;

  // �z�u�G���A�O
  if( put_pos == BOX2MAIN_GETPOS_NONE ){
    work->get_pos = BOX2MAIN_GETPOS_NONE;
    work->put_pos = BOX2MAIN_GETPOS_NONE;
//    dat[0].df_pos = get_pos;
    dat[0].df_pos = BOX2OBJ_POKEICON_GET_POS;
    dat[0].mv_pos = get_pos;
    dat[0].gt_pos = get_pos;
    dat[0].flg    = 1;
    if( syswk->tray == syswk->get_tray ){
      dat[1].df_pos = get_pos;
      dat[1].mv_pos = get_pos;
      dat[1].gt_pos = get_pos;
      dat[1].flg    = 1;
    }
    PokeIconMoveParamMake( syswk, &dat[0] );
    return;
  }

  // �����ʒu or �莝���P�̎��̃{�b�N�X�ւ̈ړ�
  if( ( get_pos == put_pos && ( get_pos >= BOX2OBJ_POKEICON_TRAY_MAX || syswk->tray == syswk->get_tray ) ) || // �����ʒu
      PartyMoveCheck(syswk,get_pos,put_pos) == FALSE ){           // �莝���P�̎��̃{�b�N�X�ւ̈ړ�

    work->get_pos = BOX2MAIN_GETPOS_NONE;
    work->put_pos = BOX2MAIN_GETPOS_NONE;
//    dat[0].df_pos = get_pos;
    dat[0].df_pos = BOX2OBJ_POKEICON_GET_POS;
    dat[0].mv_pos = get_pos;
    dat[0].gt_pos = get_pos;
    dat[0].flg    = 1;
    dat[1].df_pos = get_pos;
    dat[1].mv_pos = get_pos;
    dat[1].gt_pos = get_pos;
    dat[1].flg    = 1;
    PokeIconMoveParamMake( syswk, &dat[0] );
    return;
  }

  work->get_pos = get_pos;
  work->put_pos = put_pos;

  ppcnt = PokeParty_GetPokeCount( syswk->dat->pokeparty );

/*
  if( ( put_pos & BOX2MAIN_BOXMOVE_FLG ) != 0 ){
    if( get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
      dat[0].df_pos = get_pos;
      dat[0].mv_pos = put_pos;
      dat[0].flg    = 1;
      PokeIconMoveParamMake( syswk, &dat[0] );
      syswk->app->poke_mv_cur_flg = FALSE;
    }else{
      get = get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
      for( i=get+1; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
        dat[i].df_pos = i + BOX2OBJ_POKEICON_TRAY_MAX;
        dat[i].mv_pos = i - 1 + BOX2OBJ_POKEICON_TRAY_MAX;
        dat[i].flg    = 1;
        PokeIconMoveParamMake( syswk, &dat[i] );
      }
      dat[get].df_pos = get_pos;
      dat[get].mv_pos = put_pos;
      dat[get].flg    = 1;
      PokeIconMoveParamMake( syswk, &dat[get] );
      syswk->app->poke_mv_cur_flg = FALSE;
    }
    return;
  }
*/
  // �g���C�A�C�R����
  if( put_pos >= BOX2OBJ_POKEICON_PUT_MAX ){
    dat[0].df_pos = BOX2OBJ_POKEICON_GET_POS;
//    dat[0].df_pos = get_pos;
    dat[0].mv_pos = put_pos;
    dat[0].gt_pos = BOX2OBJ_POKEICON_GET_POS;
    dat[0].flg    = 1;
/*
    dat[1].df_pos = get_pos;
    dat[1].mv_pos = get_pos;
    dat[1].gt_pos = get_pos;
    dat[1].flg    = 1;
*/
/*
    dat[1].df_pos = get_pos;
    dat[1].mv_pos = BOX2OBJ_POKEICON_GET_POS;
    dat[1].flg    = 1;
*/
    PokeIconMoveParamMake( syswk, &dat[0] );
    // �莝������g���C��
    if( get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
      get = get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
      for( i=get+1; i<ppcnt; i++ ){
        dat[i].df_pos = i + BOX2OBJ_POKEICON_TRAY_MAX;
        dat[i].mv_pos = i - 1 + BOX2OBJ_POKEICON_TRAY_MAX;
        dat[i].gt_pos = i + BOX2OBJ_POKEICON_TRAY_MAX;
        dat[i].flg    = 1;
//        PokeIconMoveParamMake( syswk, &dat[i] );
        PokeIconMoveOutRangeParamMake( syswk, &dat[i] );
      }
      dat[ppcnt].df_pos = get_pos;
      dat[ppcnt].mv_pos = ppcnt - 1 + BOX2OBJ_POKEICON_TRAY_MAX;
      dat[ppcnt].gt_pos = get_pos;
      dat[ppcnt].flg    = 1;
//      PokeIconMoveParamMake( syswk, &dat[ppcnt] );
      PokeIconMoveOutRangeParamMake( syswk, &dat[ppcnt] );
    }
    return;
  }

  // �g���C���X�N���[������Ă���Ƃ�
  if( syswk->tray != syswk->get_tray && get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
    if( put_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
      if( put_pos > ppcnt+BOX2OBJ_POKEICON_TRAY_MAX ){
        put_pos = ppcnt+BOX2OBJ_POKEICON_TRAY_MAX;
      }
    }
    dat[0].df_pos = BOX2OBJ_POKEICON_GET_POS;
    dat[0].mv_pos = put_pos;
//    dat[0].gt_pos = BOX2OBJ_POKEICON_GET_POS;
//    dat[0].gt_pos = put_pos;
    dat[0].gt_pos = get_pos;
    dat[0].flg    = 1;
    dat[1].df_pos = put_pos;
    dat[1].mv_pos = put_pos;
    dat[1].gt_pos = put_pos;
    dat[1].flg    = 2;
    PokeIconMoveParamMake( syswk, &dat[0] );
    PokeIconMoveParamMake( syswk, &dat[1] );
    return;
  }

  if( get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
    if( put_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
      // ���{�b�N�X���m�̓���ւ�
      PokeIconChgDataMeke( syswk, get_pos, put_pos );
    }else{
      if( ( put_pos - BOX2OBJ_POKEICON_TRAY_MAX ) < ppcnt ){
        // ���{�b�N�X�E�莝���Ԃ̓���ւ�
        PokeIconChgDataMeke( syswk, get_pos, put_pos ); // 0=box, 1=party
      }else{
        // ���{�b�N�X����莝���ֈړ�
        PokeIconChgDataMeke( syswk, get_pos, ppcnt+BOX2OBJ_POKEICON_TRAY_MAX );
      }
    }
  }else{
    if( put_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
      // ���݃`�F�b�N
      if( BOX2MAIN_PokeParaGet( syswk, put_pos, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
        // ���莝���E�{�b�N�X�Ԃ̓���ւ�
        PokeIconChgDataMeke( syswk, get_pos, put_pos ); // 0=party, 1=box
      }else{
        // ���莝������{�b�N�X�ֈړ�
        get = get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
        for( i=get+1; i<ppcnt; i++ ){
          dat[i].df_pos = i + BOX2OBJ_POKEICON_TRAY_MAX;
          dat[i].mv_pos = i - 1 + BOX2OBJ_POKEICON_TRAY_MAX;
          dat[i].gt_pos = i + BOX2OBJ_POKEICON_TRAY_MAX;
          dat[i].flg    = 1;
          PokeIconMoveParamMake( syswk, &dat[i] );
        }
//        dat[get].df_pos = get_pos;
        dat[get].df_pos = BOX2OBJ_POKEICON_GET_POS;
        dat[get].mv_pos = put_pos;
        dat[get].gt_pos = get_pos;
        dat[get].flg    = 1;
        PokeIconMoveParamMake( syswk, &dat[get] );
        dat[ppcnt].df_pos = put_pos;
        dat[ppcnt].mv_pos = ppcnt - 1 + BOX2OBJ_POKEICON_TRAY_MAX;
        dat[ppcnt].gt_pos = put_pos;
        dat[ppcnt].flg    = 1;
        PokeIconMoveParamMake( syswk, &dat[ppcnt] );
      }
    }else{
      if( ( put_pos - BOX2OBJ_POKEICON_TRAY_MAX ) < ppcnt ){
        // ���莝�����m�̓���ւ�
        PokeIconChgDataMeke( syswk, get_pos, put_pos );
      }else{
        // ���莝�����ړ�
        get = get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
        for( i=get+1; i<ppcnt; i++ ){
          dat[i].df_pos = i + BOX2OBJ_POKEICON_TRAY_MAX;
          dat[i].mv_pos = i - 1 + BOX2OBJ_POKEICON_TRAY_MAX;
          dat[i].gt_pos = i + BOX2OBJ_POKEICON_TRAY_MAX;
          dat[i].flg    = 1;
          PokeIconMoveParamMake( syswk, &dat[i] );
        }
//        dat[get].df_pos = get_pos;
        dat[get].df_pos = BOX2OBJ_POKEICON_GET_POS;
        dat[get].mv_pos = ppcnt - 1 + BOX2OBJ_POKEICON_TRAY_MAX;
        dat[get].gt_pos = get_pos;
        dat[get].flg    = 1;
        PokeIconMoveParamMake( syswk, &dat[get] );
      }
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�R���ړ��f�[�^�쐬�i�g���C�Ƀh���b�v�j
 *
 * @param syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param get_pos   �擾�ʒu
 * @param put_pos   �z�u�ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconDropDataMake( BOX2_SYS_WORK * syswk, u32 get_pos, u32 put_pos )
{
  BOX2MAIN_POKEMOVE_WORK * work;
  BOX2MAIN_POKEMOVE_DATA * dat;
  u32 get;
  u32 ppcnt;
  u32 i;

  work = syswk->app->vfunk.work;
  dat  = work->dat;

//  syswk->app->poke_mv_cur_flg = TRUE;

  // ������
  for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
    dat[i].df_pos = 0;
    dat[i].mv_pos = 0;
    dat[i].gt_pos = 0;
    dat[i].flg    = 0;
  }

  // �z�u�G���A�O or �莝���P or �z�u�ʒu�Ƀ|�P����������
  if( put_pos == BOX2MAIN_GETPOS_NONE || PokeDropCheck(syswk,get_pos,put_pos) == FALSE ){
//    OS_Printf( "�u���Ȃ� put_pos = 0x%2x\n", put_pos );
    work->get_pos = BOX2MAIN_GETPOS_NONE;
    work->put_pos = BOX2MAIN_GETPOS_NONE;
//    dat[0].df_pos = get_pos;
    dat[0].df_pos = BOX2OBJ_POKEICON_GET_POS;
    dat[0].mv_pos = get_pos;
    dat[0].gt_pos = get_pos;
    dat[0].flg    = 1;
    dat[1].df_pos = get_pos;
    dat[1].mv_pos = get_pos;
    dat[1].gt_pos = get_pos;
    dat[1].flg    = 1;
    PokeIconMoveParamMake( syswk, &dat[0] );
    return;
  }

  work->get_pos = get_pos;
  work->put_pos = put_pos;

  ppcnt = PokeParty_GetPokeCount( syswk->dat->pokeparty );

  if( put_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
    // �莝������{�b�N�X�ֈړ�
    get = get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
    for( i=get+1; i<ppcnt; i++ ){
      dat[i].df_pos = i + BOX2OBJ_POKEICON_TRAY_MAX;
      dat[i].mv_pos = i - 1 + BOX2OBJ_POKEICON_TRAY_MAX;
      dat[i].gt_pos = i + BOX2OBJ_POKEICON_TRAY_MAX;
      dat[i].flg    = 1;
      PokeIconDropParamMake( syswk, &dat[i] );
    }
//    dat[get].df_pos = get_pos;
    dat[get].df_pos = BOX2OBJ_POKEICON_GET_POS;
    dat[get].mv_pos = put_pos;
    dat[get].gt_pos = get_pos;
    dat[get].flg    = 1;
    PokeIconDropParamMake( syswk, &dat[get] );
    dat[ppcnt].df_pos = put_pos;
    dat[ppcnt].mv_pos = ppcnt - 1 + BOX2OBJ_POKEICON_TRAY_MAX;
    dat[ppcnt].gt_pos = put_pos;
    dat[ppcnt].flg    = 1;
    PokeIconDropParamMake( syswk, &dat[ppcnt] );

//    syswk->app->poke_mv_cur_flg = FALSE;
  }


#if 0

  work->get_pos = get_pos;
  work->put_pos = put_pos;

  ppcnt = PokeParty_GetPokeCount( syswk->dat->pokeparty );


  if( get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
    if( put_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
      // ���{�b�N�X���m�̓���ւ�
      PokeIconChgDataMeke( syswk, get_pos, put_pos );
    }else{
      if( ( put_pos - BOX2OBJ_POKEICON_TRAY_MAX ) < ppcnt ){
        // ���{�b�N�X�E�莝���Ԃ̓���ւ�
        PokeIconChgDataMeke( syswk, get_pos, put_pos ); // 0=box, 1=party
      }else{
        // ���{�b�N�X����莝���ֈړ�
        PokeIconChgDataMeke( syswk, get_pos, ppcnt+BOX2OBJ_POKEICON_TRAY_MAX );
      }
    }
  }else{
    if( put_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
      // ���݃`�F�b�N
      if( BOX2MAIN_PokeParaGet( syswk, put_pos, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
        // ���莝���E�{�b�N�X�Ԃ̓���ւ�
        PokeIconChgDataMeke( syswk, get_pos, put_pos ); // 0=party, 1=box
      }else{
        // ���莝������{�b�N�X�ֈړ�
        get = get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
        for( i=get+1; i<ppcnt; i++ ){
          dat[i].df_pos = i + BOX2OBJ_POKEICON_TRAY_MAX;
          dat[i].mv_pos = i - 1 + BOX2OBJ_POKEICON_TRAY_MAX;
          dat[i].gt_pos = i + BOX2OBJ_POKEICON_TRAY_MAX;
          dat[i].flg    = 1;
          PokeIconMoveParamMake( syswk, &dat[i] );
        }
//        dat[get].df_pos = get_pos;
        dat[get].df_pos = BOX2OBJ_POKEICON_GET_POS;
        dat[get].mv_pos = put_pos;
        dat[get].gt_pos = get_pos;
        dat[get].flg    = 1;
        PokeIconMoveParamMake( syswk, &dat[get] );
        dat[ppcnt].df_pos = put_pos;
        dat[ppcnt].mv_pos = ppcnt - 1 + BOX2OBJ_POKEICON_TRAY_MAX;
        dat[ppcnt].gt_pos = put_pos;
        dat[ppcnt].flg    = 1;
        PokeIconMoveParamMake( syswk, &dat[ppcnt] );
      }
    }else{
      if( ( put_pos - BOX2OBJ_POKEICON_TRAY_MAX ) < ppcnt ){
        // ���莝�����m�̓���ւ�
        PokeIconChgDataMeke( syswk, get_pos, put_pos );
      }else{
        // ���莝�����ړ�
        get = get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
        for( i=get+1; i<ppcnt; i++ ){
          dat[i].df_pos = i + BOX2OBJ_POKEICON_TRAY_MAX;
          dat[i].mv_pos = i - 1 + BOX2OBJ_POKEICON_TRAY_MAX;
          dat[i].gt_pos = i + BOX2OBJ_POKEICON_TRAY_MAX;
          dat[i].flg    = 1;
          PokeIconMoveParamMake( syswk, &dat[i] );
        }
//        dat[get].df_pos = get_pos;
        dat[get].df_pos = BOX2OBJ_POKEICON_GET_POS;
        dat[get].mv_pos = ppcnt - 1 + BOX2OBJ_POKEICON_TRAY_MAX;
        dat[get].gt_pos = get_pos;
        dat[get].flg    = 1;
        PokeIconMoveParamMake( syswk, &dat[get] );
      }
    }
  }
#endif


}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�R���ړ��f�[�^�쐬�i�莝���Ƀh���b�v�j
 *
 * @param syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param get_pos   �擾�ʒu
 * @param put_pos   �z�u�ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconPartyDropDataMake( BOX2_SYS_WORK * syswk, u32 get_pos, u32 put_pos )
{
  BOX2MAIN_POKEMOVE_WORK * work;
  BOX2MAIN_POKEMOVE_DATA * dat;
  u32 get;
  u32 ppcnt;
  u32 i;

  work = syswk->app->vfunk.work;
  dat  = work->dat;

//  syswk->app->poke_mv_cur_flg = TRUE;

  // ������
  for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
    dat[i].df_pos = 0;
    dat[i].mv_pos = 0;
    dat[i].gt_pos = 0;
    dat[i].flg    = 0;
  }

  // �z�u�G���A�O or �z�u�ʒu�Ƀ|�P����������
  if( put_pos == BOX2MAIN_GETPOS_NONE ||
    BOX2MAIN_PokeParaGet( syswk, put_pos, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
/*
    work->get_pos = BOX2MAIN_GETPOS_NONE;
    work->put_pos = BOX2MAIN_GETPOS_NONE;
    dat[0].df_pos = get_pos;
    dat[0].mv_pos = get_pos;
    dat[0].flg    = 1;
    PokeIconMoveParamMake( syswk, &dat[0] );
    return;
*/
    work->get_pos = BOX2MAIN_GETPOS_NONE;
    work->put_pos = BOX2MAIN_GETPOS_NONE;
//    dat[0].df_pos = get_pos;
    dat[0].df_pos = BOX2OBJ_POKEICON_GET_POS;
    dat[0].mv_pos = get_pos;
    dat[0].gt_pos = get_pos;
    dat[0].flg    = 1;
    dat[1].df_pos = get_pos;
    dat[1].mv_pos = get_pos;
    dat[1].gt_pos = get_pos;
    dat[1].flg    = 1;
    PokeIconMoveParamMake( syswk, &dat[0] );
    return;
  }

  work->get_pos = get_pos;
  work->put_pos = put_pos;

  ppcnt = PokeParty_GetPokeCount( syswk->dat->pokeparty );

  PokeIconChgDataMeke( syswk, get_pos, ppcnt+BOX2OBJ_POKEICON_TRAY_MAX );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���̈ړ���̈ʒu���擾
 *
 * @param syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param get_pos   �擾�ʒu
 *
 * @retval  "BOX2MAIN_GETPOS_NONE = �ړ��Ȃ�"
 * @retval  "BOX2MAIN_GETPOS_NONE != �ړ���̈ʒu"
 */
//--------------------------------------------------------------------------------------------
static u32 PokeIconMoveAfterPosGet( BOX2_SYS_WORK * syswk, u32 get_pos )
{
  BOX2MAIN_POKEMOVE_WORK * work;
  BOX2MAIN_POKEMOVE_DATA * dat;
  u32 i;

/*
  if( syswk->app->mv_err_code != BOX2MAIN_ERR_CODE_NONE ){
    return syswk->get_pos;
  }
*/

  work = syswk->app->vfunk.work;
  dat  = work->dat;

  for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
//    if( dat[i].flg == 1 && dat[i].df_pos == get_pos ){
    if( dat[i].flg != 0 && dat[i].df_pos == get_pos ){
      return dat[i].mv_pos;
    }
  }

  return syswk->get_pos;
//  return BOX2MAIN_GETPOS_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�R���ړ��f�[�^�쐬�i�������j
 *
 * @param syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param get_pos   �擾�ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconFreeDataMake( BOX2_SYS_WORK * syswk, u32 get_pos )
{
  BOX2MAIN_POKEMOVE_WORK * work;
  BOX2MAIN_POKEMOVE_DATA * dat;
  u32 get;
  u32 i;

  work = syswk->app->vfunk.work;
  dat  = work->dat;
  get  = get_pos - BOX2OBJ_POKEICON_TRAY_MAX;

  // ������
  for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
    dat[i].df_pos = i + BOX2OBJ_POKEICON_TRAY_MAX;
    dat[i].mv_pos = i + BOX2OBJ_POKEICON_TRAY_MAX;
    dat[i].gt_pos = i + BOX2OBJ_POKEICON_TRAY_MAX;
    dat[i].flg    = 0;
  }
  dat[i].flg = 0;

  for( i=get+1; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
    dat[i].mv_pos = i - 1 + BOX2OBJ_POKEICON_TRAY_MAX;
    dat[i].flg    = 1;
    PokeIconMoveParamMake( syswk, &dat[i] );
  }
  dat[get].mv_pos = BOX2OBJ_POKEICON_MAX - 1;
  dat[get].flg    = 1;
  PokeIconMoveParamMake( syswk, &dat[get] );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R������ւ�
 *
 * @param syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param dat     �ړ��f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconBufPosChange( BOX2_SYS_WORK * syswk, BOX2MAIN_POKEMOVE_DATA * dat )
{
  u16 i;
  u16 gmp;
  u8  id[BOX2OBJ_POKEICON_MINE_MAX+1];

  gmp = 0xffff;

  // ���̈ʒu�ɂ���h�c���L��
  for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
    if( dat[i].flg != 0 ){
//      id[i] = syswk->app->pokeicon_id[ dat[i].df_pos ];
      id[i] = syswk->app->pokeicon_id[ dat[i].gt_pos ];
/*
      // �擾�A�C�R���̈ړ��ʒu��ۑ�
      if( dat[i].df_pos == BOX2OBJ_POKEICON_GET_POS ){
        gmp = dat[i].mv_pos;
      }
*/
    }
  }

/*
  // �擾�ʒu�ɂ���A�C�R�����擾
  if( gmp != 0xffff ){
    for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
      if( dat[i].flg != 0 ){
        if( gmp == dat[i].df_pos ){
          gmp = syswk->app->pokeicon_id[ dat[i].mv_pos ];
          break;
        }
      }
    }
  }
  if( i == BOX2OBJ_POKEICON_MINE_MAX+1 ){
    gmp = 0xffff;
  }
*/

  // ���ꂩ����
  for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
    if( dat[i].flg != 0 ){
/*
      if( ( dat[i].mv_pos & BOX2MAIN_BOXMOVE_FLG ) == 0 ){
        syswk->app->pokeicon_id[ dat[i].mv_pos ] = id[i];
        BOX2OBJ_PokeIconPriChg( syswk->app, dat[i].mv_pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
      }else{
        if( dat[i].df_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
          syswk->app->pokeicon_id[ BOX2OBJ_POKEICON_MAX-1 ] = id[i];
        }
        BOX2OBJ_PokeIconPriChg( syswk->app, dat[i].df_pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
      }
*/
      syswk->app->pokeicon_id[ dat[i].mv_pos ] = id[i];
//      BOX2OBJ_PokeIconPriChg( syswk->app, dat[i].mv_pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
      if( dat[i].df_pos == BOX2OBJ_POKEICON_GET_POS ){
        gmp = dat[i].mv_pos;
//        OS_Printf( "gmp = %d\n", gmp );
      }
    }
  }

  if( gmp != 0xffff ){
    BOX2OBJ_PutPokeIcon( syswk->app, gmp );
  }

  // �S�̂̃v���C�I���e�B���Đݒ肷��
  for( i=0; i<BOX2OBJ_POKEICON_PUT_MAX; i++ ){
    BOX2OBJ_PokeIconPriChg( syswk->app, i, BOX2OBJ_POKEICON_PRI_CHG_PUT );
  }
}


//============================================================================================
//  �|�P�����𓦂���
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �Z�`�F�b�N
 *
 * @param waza  �Z
 *
 * @retval  "�d�v�ȋZ�ȊO = 0xffffffff"
 * @retval  "�d�v�ȋZ = �e�[�u���ԍ�"
 */
//--------------------------------------------------------------------------------------------
static u32 PokeFreeWazaCheck( u16 waza )
{
/*
  u32 i;

  for( i=0; i<NELEMS(PokeFreeCheckWazaTable); i++ ){
    if( waza == PokeFreeCheckWazaTable[i] ){
      return i;
    }
  }
*/
  return 0xffffffff;
}

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P�����𓦂����v���[�N�쐬
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_PokeFreeCreate( BOX2_SYS_WORK * syswk )
{
  BOX2MAIN_POKEFREE_WORK * wk;
  POKEMON_PASO_PARAM * ppp;
  u32 ret;
  u32 i;

  syswk->app->seqwk = GFL_HEAP_AllocMemoryLo( HEAPID_BOX_APP, sizeof(BOX2MAIN_POKEFREE_WORK) );
  wk = syswk->app->seqwk;

  wk->clwk      = syswk->app->clwk[syswk->app->pokeicon_id[syswk->get_pos]];
  wk->check_cnt = 0;
  wk->check_flg = 0;

  ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );

  for( i=0; i<4; i++ ){
    ret = PokeFreeWazaCheck( PPP_Get(ppp,ID_PARA_waza1+i,NULL) );
    if( ret != 0xffffffff ){
      wk->check_flg |= (1<<ret);
      break;
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P�����𓦂����v���[�N���
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_PokeFreeExit( BOX2_SYS_WORK * syswk )
{
  GFL_HEAP_FreeMemory( syswk->app->seqwk );
}

//--------------------------------------------------------------------------------------------
/**
 * �������邩�ǂ����Z�`�F�b�N
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  "TRUE = �`�F�b�N��"
 * @return  "FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2MAIN_PokeFreeWazaCheck( BOX2_SYS_WORK * syswk )
{
  BOX2MAIN_POKEFREE_WORK * wk;
  POKEMON_PASO_PARAM * ppp;
  u32 tray, pos;
  u32 i, j;
  u32 ret;

  wk = syswk->app->seqwk;

  if( wk->check_cnt == POKE_FREE_CHECK_MAX ){
    return FALSE;
  }

  for( i=0; i<POKE_FREE_CHECK_ONECE; i++ ){
    if( wk->check_cnt < POKE_FREE_CHECK_BOX ){
      tray = wk->check_cnt / BOX_MAX_POS;
      pos  = wk->check_cnt % BOX_MAX_POS;
      // �������|�P�����Ɠ����Ƃ��̓`�F�b�N���Ȃ��i���݂��Ȃ����Ƃɂ���j
      if( tray == syswk->tray && pos == syswk->get_pos ){
        ppp = NULL;
      }else{
        ppp = BOX2MAIN_PPPGet( syswk, tray, pos );
      }
    }else{
      pos = wk->check_cnt - POKE_FREE_CHECK_BOX;
      // �������|�P�����Ɠ����Ƃ��̓`�F�b�N���Ȃ��i���݂��Ȃ����Ƃɂ���j
      if( pos == syswk->get_pos - BOX_MAX_POS ){
        ppp = NULL;
      }else{
        ppp = BOX2MAIN_PPPGet( syswk, BOX2MAIN_PPP_GET_MINE, pos );
      }
    }
    if( ppp != NULL ){
      if( PPP_Get( ppp, ID_PARA_poke_exist, NULL ) != 0 ){
        for( j=0; j<4; j++ ){
          ret = PokeFreeWazaCheck( PPP_Get(ppp,ID_PARA_waza1+j,NULL) );
          if( ret != 0xffffffff ){
            wk->check_flg = ( wk->check_flg & (0xff^(1<<ret)) );
          }
        }
      }
    }
    wk->check_cnt++;
    if( wk->check_cnt == POKE_FREE_CHECK_MAX ){
      return FALSE;
    }
  }
  return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �I�����Ɏ莝���Ƀy���b�v�����Ȃ��ꍇ�͂؃^�b�v�{�C�X�̑��݃t���O�𗎂Ƃ�
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_CheckPartyPerapu( BOX2_SYS_WORK * syswk )
{
  u32 i;

  // �o�g���{�b�N�X�ƐQ������ꍇ�̓`�F�b�N���Ȃ�
  if( syswk->dat->callMode == BOX_MODE_BATTLE || syswk->dat->callMode == BOX_MODE_SLEEP ){
    return;
  }

  // �莝���Ƀy���b�v�����Ȃ�������u������ׂ�v�Ř^�����������폜
  PERAPVOICE_CheckPerapInPokeParty( GAMEDATA_GetPerapVoice(syswk->dat->gamedata),
                                    syswk->dat->pokeparty );
/*
  for( i=0; i<PokeParty_GetPokeCount(syswk->dat->pokeparty); i++ ){
    POKEMON_PARAM * pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, i );
    if( PP_Get( pp, ID_PARA_monsno, NULL ) == MONSNO_PERAPPU ){
      return;
    }
  }

  {
    PERAPVOICE * sv = SaveData_GetPerapVoice( GAMEDATA_GetSaveControlWork(syswk->dat->gamedata) );
    PERAPVOICE_ClearExistFlag( sv );
  }
*/
}


//============================================================================================
//  �A�C�e���A�C�R��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R������f�[�^�쐬
 *
 * @param syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param set_pos   �z�u�ʒu�i�v���C���[���w�肵���ʒu�j
 * @param put_pos   �z�u�ʒu�i�u���Ȃ������ꍇ�̕␳����j
 * @param mode    �ړ����[�h
 * @param flg     TRUE = �^�b�`, FALSE = �L�[
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void ItemIconMoveMakeCore( BOX2_SYS_WORK * syswk, u32 set_pos, u32 put_pos, u32 mode, BOOL flg )
{
  BOX2MAIN_ITEMMOVE_WORK * mvwk;
  s16 npx, npy;
  s16 mpx, mpy;

  mvwk = syswk->app->vfunk.work;

  BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_ITEMICON, &npx, &npy, CLSYS_DEFREND_MAIN );

  BOX2OBJ_PokeIconDefaultPosGet( put_pos, &mpx, &mpy, mode );
  if( flg == TRUE ){
    mpx += 8;
    mpy += 8;
  }else{
    mpy += 4;
  }

  mvwk->put_pos = put_pos;
  mvwk->set_pos = set_pos;
  mvwk->cnt = 0;
  mvwk->mv_mode = mode;

  if( npx > mpx ){
    mvwk->mv_x = 1;
    mvwk->mx = ( ( npx - mpx ) << 8 ) / ITEMICON_MOVE_CNT;
  }else{
    mvwk->mv_x = 0;
    mvwk->mx = ( ( mpx - npx ) << 8 ) / ITEMICON_MOVE_CNT;
  }
  if( npy > mpy ){
    mvwk->mv_y = 1;
    mvwk->my = ( ( npy - mpy ) << 8 ) / ITEMICON_MOVE_CNT;
  }else{
    mvwk->mv_y = 0;
    mvwk->my = ( ( mpy - npy ) << 8 ) / ITEMICON_MOVE_CNT;
  }

  mvwk->now_x = npx << 8;
  mvwk->now_y = npy << 8;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R������f�[�^�쐬�i�^�b�`�j
 *
 * @param syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param set_pos   �z�u�ʒu�i�v���C���[���w�肵���ʒu�j
 * @param put_pos   �z�u�ʒu�i�u���Ȃ������ꍇ�̕␳����j
 * @param mode    �ړ����[�h
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void ItemIconMoveMake( BOX2_SYS_WORK * syswk, u32 set_pos, u32 put_pos, u32 mode )
{
  ItemIconMoveMakeCore( syswk, set_pos, put_pos, mode, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R������f�[�^�쐬�i�L�[�j
 *
 * @param syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param put_pos   �z�u�ʒu�i�u���Ȃ������ꍇ�̕␳����j
 * @param mode    �ړ����[�h
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void ItemIconMoveMakeHand( BOX2_SYS_WORK * syswk, u32 put_pos, u32 mode )
{
  ItemIconMoveMakeCore( syswk, syswk->app->poke_put_key, put_pos, mode, FALSE );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R���ړ�
 *
 * @param syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param flg     TRUE = �^�b�`, FALSE = �L�[
 *
 * @retval  "TRUE = �ړ���"
 * @retval  "FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
static BOOL ItemIconMoveMainCore( BOX2_SYS_WORK * syswk, BOOL flg )
{
  BOX2MAIN_ITEMMOVE_WORK * mvwk;
  s16 px, py;

  mvwk = syswk->app->vfunk.work;

  if( mvwk->cnt == ITEMICON_MOVE_CNT ){
    if( flg == TRUE ){
      BOX2OBJ_ItemIconPokePut( syswk->app, mvwk->put_pos, mvwk->mv_mode );
    }else{
      BOX2OBJ_ItemIconPokePutHand( syswk->app, mvwk->put_pos, mvwk->mv_mode );
    }
    BOX2OBJ_ItemIconCursorMove( syswk->app );
    return FALSE;
  }

  if( mvwk->mv_x == 0 ){
    mvwk->now_x += mvwk->mx;
  }else{
    mvwk->now_x -= mvwk->mx;
  }
  if( mvwk->mv_y == 0 ){
    mvwk->now_y += mvwk->my;
  }else{
    mvwk->now_y -= mvwk->my;
  }

  BOX2OBJ_ItemIconPosSet( syswk->app, (mvwk->now_x>>8), (mvwk->now_y>>8) );
  BOX2OBJ_ItemIconCursorMove( syswk->app );

  mvwk->cnt++;

  return TRUE;

}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R���ړ��i�^�b�`�j
 *
 * @param syswk   �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "TRUE = �ړ���"
 * @retval  "FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
static BOOL ItemIconMoveMain( BOX2_SYS_WORK * syswk )
{
  return ItemIconMoveMainCore( syswk, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R���ړ��i�L�[�j
 *
 * @param syswk   �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "TRUE = �ړ���"
 * @retval  "FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
static BOOL ItemIconMoveMainHand( BOX2_SYS_WORK * syswk )
{
  return ItemIconMoveMainCore( syswk, FALSE );
}


//============================================================================================
//  �g���C�I��
//============================================================================================

u8 BOX2MAIN_GetBoxMoveTrayNum( BOX2_SYS_WORK * syswk, s8 mv )
{
  s8  pos = syswk->box_mv_pos;
  
  pos += mv;
  if( pos < 0 ){
    pos += syswk->trayMax;
  }else if( pos >= syswk->trayMax ){
    pos -= syswk->trayMax;
  }
  return pos;
}

//============================================================================================
//  �ǎ�
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief   �ǎ��L�����f�[�^�ǂݍ���
 *
 * @param   syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param   id    �ǎ��ԍ�
 * @param   cgx   �]���L�����ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WallCharLoad( BOX2_SYS_WORK * syswk, u32 id, u32 cgx )
{
  NNSG2dCharacterData * chr;
  void * buf;
  u8 * title;

  buf = GFL_ARC_UTIL_LoadBGCharacter(
          ARCID_BOX2_GRA, NARC_box_gra_box_wp01_lz_NCGR+id, TRUE, &chr, HEAPID_BOX_APP );

  GFL_BG_LoadCharacter( GFL_BG_FRAME3_M, chr->pRawData, chr->szByte, cgx );

  title = chr->pRawData;

//  BOX2BMP_TrayNamePut( syswk, &title[0x20*WALL_TITLE_POS] );
/*
  BOX2BMP_TrayNamePut(
    syswk, &title[0x20*WALL_TITLE_POS], cgx+WALL_TITLE_POS, WALL_SX, WALL_TITLE_SY );
*/
  GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �ǎ��p���b�g�f�[�^�ǂݍ���
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param   id      �ǎ��ԍ�
 * @param   pal     �p���b�g�ԍ�
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WallPaletteLoad( BOX2_SYS_WORK * syswk, u32 id, u32 pal )
{
  GFL_ARC_UTIL_TransVramPalette(
    ARCID_BOX2_GRA, NARC_box_gra_box_wp01_NCLR+id,
    PALTYPE_MAIN_BG, pal*0x20, 0x20, HEAPID_BOX_APP );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �ǎ��X�N���[���f�[�^�ǂݍ���
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param   id      �ǎ��ԍ�
 * @param   px      �]���w���W
 * @param   cgx     �]���L�����ʒu
 * @param   pal     �p���b�g�ԍ�
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WallScreenLoad( BOX2_SYS_WORK * syswk, u32 id, u32 px, u32 cgx, u32 pal )
{
  NNSG2dScreenData * scrn;
  void * buf;
  u16 * raw;
  u32 x;
  u16 dat;
  u8  i, j;

  buf = GFL_ARC_UTIL_LoadScreen(
          ARCID_BOX2_GRA, NARC_box_gra_box_wp01_lz_NSCR, TRUE, &scrn, HEAPID_BOX_APP );
  raw = (u16 *)scrn->rawData;
  for( i=0; i<WALL_SY; i++ ){
    x = px;
    for( j=0; j<WALL_SX; j++ ){
      dat = ( raw[i*WALL_SX+j] & 0xfff ) + ( pal << 12 ) + cgx;
      GFL_BG_WriteScreen( GFL_BG_FRAME3_M, &dat, x, WALL_PY+i, 1, 1 );
      x++;
      if( x >= 64 ){ x = 0; }
    }
  }
  GFL_HEAP_FreeMemory( buf );

  GFL_BG_FillScreen(
    GFL_BG_FRAME3_M, WALL_SPACE_CHR, x, 0, 2, WALL_SY, GFL_BG_SCRWRT_PALIN );
  GFL_BG_FillScreen(
    GFL_BG_FRAME3_M, WALL_SPTAG_CHR, x, 5, 2, 1, GFL_BG_SCRWRT_PALIN );
}

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��O���t�B�b�N�Z�b�g
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param id    �ǎ��h�c
 * @param px    �\���w���W
 * @param cgx   �]���L�����ʒu
 * @param pal   �g�p�p���b�g
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WallGraSet( BOX2_SYS_WORK * syswk, u32 id, u32 px, u32 cgx, u32 pal )
{
  WallCharLoad( syswk, id, cgx );
  WallPaletteLoad( syswk, id, pal );
  WallScreenLoad( syswk, id, px, cgx, pal );

  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME3_M );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �ǎ��Z�b�g
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param   id      �ǎ��h�c
 * @param   mv      �X�N���[������
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_WallPaperSet( BOX2_SYS_WORK * syswk, u32 id, u32 mv )
{
  u32 chr, pal;

  if( mv == BOX2MAIN_TRAY_SCROLL_L ){
    syswk->app->wall_px -= WALL_PUT_SPACE;
    if( syswk->app->wall_px < 0 ){
      syswk->app->wall_px += 64;
    }
  }else if( mv == BOX2MAIN_TRAY_SCROLL_R ){
    syswk->app->wall_px += WALL_PUT_SPACE;
    if( syswk->app->wall_px >= 64 ){
      syswk->app->wall_px -= 64;
    }
  }

  if( syswk->app->wall_area == 0 ){
    chr = WALL_CGX_POS1;
    pal = BOX2MAIN_BG_PAL_WALL1;
  }else{
    chr = WALL_CGX_POS2;
    pal = BOX2MAIN_BG_PAL_WALL2;
  }
  syswk->app->wall_area ^= 1;

  WallGraSet( syswk, id, syswk->app->wall_px, chr, pal );

  // �{�b�N�X���ݒ�
  // ��������
  if( mv == BOX2MAIN_TRAY_SCROLL_NONE ){
    BOX2BMP_BoxNameWrite( syswk, syswk->tray, BOX2MAIN_FNTOAM_BOX_NAME1 );
    BOX2OBJ_FontOamVanish( syswk->app, BOX2MAIN_FNTOAM_BOX_NAME2, FALSE );
  // �X�N���[����
  }else{
    u32 idx;
    if( BOX2OBJ_CheckFontOamVanish( syswk->app, BOX2MAIN_FNTOAM_BOX_NAME1 ) == TRUE ){
      idx = BOX2MAIN_FNTOAM_BOX_NAME2;
    }else{
      idx = BOX2MAIN_FNTOAM_BOX_NAME1;
    }
    BOX2BMP_BoxNameWrite( syswk, syswk->tray, idx );
    BOX2OBJ_SetBoxNamePos( syswk->app, idx, mv );
    BOX2OBJ_FontOamVanish( syswk->app, idx, TRUE );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��ύX
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param id    �ǎ��ԍ�
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_WallPaperChange( BOX2_SYS_WORK * syswk, u32 id )
{
  u32 chr, pal;

  if( syswk->app->wall_area == 0 ){
    chr = WALL_CGX_POS1;
    pal = BOX2MAIN_BG_PAL_WALL1;
  }else{
    chr = WALL_CGX_POS2;
    pal = BOX2MAIN_BG_PAL_WALL2;
  }
  syswk->app->wall_area ^= 1;

  WallCharLoad( syswk, id, chr );
  PaletteWorkSet_Arc(
    syswk->app->pfd,
    ARCID_BOX2_GRA, NARC_box_gra_box_wp01_NCLR+id,
    HEAPID_BOX_APP, FADE_MAIN_BG, FADE_PAL_ONE_SIZE, pal*16 );
  WallScreenLoad( syswk, id, syswk->app->wall_px, chr, pal );

  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME3_M );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �ǎ��ԍ��擾
 *
 * @param   syswk   �{�b�N�X��ʃV�X�e�����[�N
 * @param   num     �g���C�ԍ�
 *
 * @return  �ǎ��ԍ�
 *
 *  �f�t�H���g�ǎ�����̒ʂ��ԍ��ɂ���
 */
//--------------------------------------------------------------------------------------------
u32 BOX2MAIN_GetWallPaperNumber( BOX2_SYS_WORK * syswk, u32 num )
{
/*
  u32 wp = BOXDAT_GetWallPaperNumber( syswk->dat->sv_box, num );

  if( wp >= BOX_NORMAL_WALLPAPER_MAX ){
    return ( wp - BOX_TOTAL_WALLPAPER_MAX_PL + BOX_NORMAL_WALLPAPER_MAX );
  }
  return wp;
*/
  return BOXDAT_GetWallPaperNumber( syswk->dat->sv_box, num );
}


//============================================================================================
//  �f�[�^�\���֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �\���f�[�^�쐬
 *
 * @param ppp   POKEMON_PASO_PARAM
 *
 * @return  �쐬�����f�[�^
 */
//--------------------------------------------------------------------------------------------
static BOX2_POKEINFO_DATA * PokeInfoDataMake( POKEMON_PASO_PARAM * ppp )
{
  BOX2_POKEINFO_DATA * info;
  u32 i;

  if( PPP_Get( ppp, ID_PARA_poke_exist, NULL ) != 0 ){
    info = GFL_HEAP_AllocMemory( HEAPID_BOX_APP, sizeof(BOX2_POKEINFO_DATA) );

    info->ppp = ppp;

    info->mons = PPP_Get( ppp, ID_PARA_monsno, NULL );
    info->item = PPP_Get( ppp, ID_PARA_item, NULL );
    info->rand = PPP_Get( ppp, ID_PARA_personal_rnd, NULL );

    info->type1 = PPP_Get( ppp, ID_PARA_type1, NULL );
    info->type2 = PPP_Get( ppp, ID_PARA_type2, NULL );

    info->tokusei = PPP_Get( ppp, ID_PARA_speabino, NULL );
    info->seikaku = PPP_GetSeikaku( ppp );

    info->mark   = PPP_Get( ppp, ID_PARA_mark, NULL );
    info->lv     = PPP_Get( ppp, ID_PARA_level, NULL );
    info->tamago = PPP_Get( ppp, ID_PARA_tamago_flag, NULL );
    info->sex    = PPP_GetSex( ppp );

    if( PPP_CheckRare( ppp ) == TRUE ){
      info->rare = 1;
    }else{
      info->rare = 0;
    }

    // �|�P���X
    if( POKERUS_CheckInfectPPP( ppp ) == TRUE ){
      info->pokerus = 1;
    }else if( POKERUS_CheckInfectedPPP( ppp ) == TRUE ){
      info->pokerus = 2;
    }else{
      info->pokerus = 0;
    }

/*
    // �e�X�g
    info->rare = 1;
    info->pokerus = 1;
*/

    if( info->mons != MONSNO_NIDORAN_F && info->mons != MONSNO_NIDORAN_M && info->tamago == 0 ){
      info->sex_put = 1;
    }else{
      info->sex_put = 0;
    }

    for( i=0; i<4; i++ ){
      info->waza[i] = PPP_Get( ppp, ID_PARA_waza1+i, NULL );
    }
  }else{
    info = NULL;
  }

  return info;
}

//--------------------------------------------------------------------------------------------
/**
 * �\���f�[�^���
 *
 * @param info  �\���f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeInfoDataFree( BOX2_POKEINFO_DATA * info )
{
  GFL_HEAP_FreeMemory( info );
}

//--------------------------------------------------------------------------------------------
/**
 * ���\��
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param info  �\���f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeInfoPutModeNormal( BOX2_SYS_WORK * syswk, BOX2_POKEINFO_DATA * info )
{
/*
  u32 winID;
  u32 i;

  winID = BOX2BMP_PokeSkillWrite( syswk, info );
  for( i=0; i<4; i++ ){
    BGWINFRM_BmpWinOn( syswk->app->wfrm, BOX2MAIN_WINFRM_SUBDISP, syswk->app->win[winID+i].win );
  }
  BOX2BGWFRM_SubDispWazaFrmOutPosSet( syswk->app->wfrm );
*/

  BOX2OBJ_PokeGraChange( syswk, info, BOX2OBJ_ID_POKEGRA );
//  BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_POKEGRA+(syswk->app->pokegra_swap^1), TRUE );
//  BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_POKEGRA+syswk->app->pokegra_swap, FALSE );

  BOX2OBJ_TypeIconChange( syswk->app, info );
  BOX2BMP_PokeDataPut( syswk, info );

  BOX2MAIN_SubDispMarkingChange( syswk, info->mark );
  PutSubDispPokeMark( syswk->app, info );

  GFL_BG_SetScrollReq( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * ���\���i�u�������̂������肷��v�̂Ƃ��j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param info  �\���f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
static void PokeInfoPutModeItem( BOX2_SYS_WORK * syswk, BOX2_POKEINFO_DATA * info )
{
#if 0
  if( info->item != ITEM_DUMMY_DATA ){
    u32 winID = BOX2BMP_PokeItemInfoWrite( syswk, info->item );
    BGWINFRM_BmpWinOn( syswk->app->wfrm, BOX2MAIN_WINFRM_SUBDISP, syswk->app->win[winID].win );
    BGWINFRM_BmpWinOn( syswk->app->wfrm, BOX2MAIN_WINFRM_SUBDISP, syswk->app->win[winID+1].win );
    BOX2OBJ_ItemIconChangeSub( syswk->app, info->item );
  }
  BOX2BGWFRM_SubDispItemFrmOutPosSet( syswk->app->wfrm );
#endif

  BOX2OBJ_PokeGraChange( syswk, info, BOX2OBJ_ID_POKEGRA );
//  BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_POKEGRA+(syswk->app->pokegra_swap^1), TRUE );
//  BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_POKEGRA+syswk->app->pokegra_swap, FALSE );

  BOX2OBJ_TypeIconChange( syswk->app, info );
  BOX2BMP_PokeDataPut( syswk, info );

  BOX2MAIN_SubDispMarkingChange( syswk, info->mark );
  PutSubDispPokeMark( syswk->app, info );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �w��ʒu�̃|�P�������\��
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param pos   �ʒu
 *
 * @retval  "TRUE = �\��"
 * @retval  "FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2MAIN_PokeInfoPut( BOX2_SYS_WORK * syswk, u32 pos )
{
  BOX2_POKEINFO_DATA * info;
  POKEMON_PASO_PARAM * ppp;
  BOOL  fast;

  ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, pos );

  if( ppp != NULL ){
    fast = PPP_FastModeOn( ppp );
    info = PokeInfoDataMake( ppp );

    if( info != NULL ){
/*
      if( syswk->dat->callMode == BOX_MODE_ITEM ){
        PokeInfoPutModeItem( syswk, info );
      }else{
        PokeInfoPutModeNormal( syswk, info );
      }
*/
      PokeInfoPutModeNormal( syswk, info );
      PokeInfoDataFree( info );
      PPP_FastModeOff( ppp, fast );
    }else{
      // �I�t
      BOX2MAIN_PokeInfoOff( syswk );
      PPP_FastModeOff( ppp, fast );
      return FALSE;
    }
  }else{
    // �I�t
    BOX2MAIN_PokeInfoOff( syswk );
    return FALSE;
  }

  return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * ���ʂ̈ꕔ���ĕ`��
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param pos   �ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_PokeInfoRewrite( BOX2_SYS_WORK * syswk, u32 pos )
{
  BOX2_POKEINFO_DATA * info;
  POKEMON_PASO_PARAM * ppp;
  BOOL  fast;

  ppp  = BOX2MAIN_PPPGet( syswk, syswk->tray, pos );
  fast = PPP_FastModeOn( ppp );
  info = PokeInfoDataMake( ppp );

  BOX2OBJ_PokeGraChange( syswk, info, BOX2OBJ_ID_POKEGRA );
//  BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_POKEGRA+(syswk->app->pokegra_swap^1), TRUE );
//  BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_POKEGRA+syswk->app->pokegra_swap, FALSE );

  BOX2OBJ_TypeIconChange( syswk->app, info );
  BOX2BMP_PokeDataPut( syswk, info );

  PokeInfoDataFree( info );
  PPP_FastModeOff( ppp, fast );
}

//--------------------------------------------------------------------------------------------
/**
 * ���ʕ\���I�t
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_PokeInfoOff( BOX2_SYS_WORK * syswk )
{
  u32 i;

  // �|�P�O���I�t
  if( syswk->app->clwk[BOX2OBJ_ID_POKEGRA] != NULL ){
    BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_POKEGRA, FALSE );
  }
  if( syswk->app->clwk[BOX2OBJ_ID_POKEGRA2] != NULL ){
    BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_POKEGRA2, FALSE );
  }
  
  // �^�C�v�A�C�R���I�t
  for( i=BOX2OBJ_ID_TYPEICON; i<BOX2OBJ_ID_TYPEICON+POKETYPE_MAX; i++ ){
    BOX2OBJ_Vanish( syswk->app, i, FALSE );
  }

  BOX2BMP_PokeDataOff( syswk->app );      // ������I�t
  SubDispMarkingOff( syswk->app );        // �}�[�N�I�t

  BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_RARE, FALSE );
  BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_POKERUS, FALSE );
  BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_POKERUS_ICON, FALSE );

  GFL_BG_SetScrollReq( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, 192 );

/*
  // �E�B���t���[���I�t
  if( syswk->dat->callMode == BOX_MODE_ITEM ){
    BOX2BGWFRM_SubDispItemFrmOutPosSet( syswk->app->wfrm );
  }else{
    BOX2BGWFRM_SubDispWazaFrmOutPosSet( syswk->app->wfrm );
  }
*/
}

//--------------------------------------------------------------------------------------------
/**
 * ��I���ɐ؂�ւ�
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_PokeSelectOff( BOX2_SYS_WORK * syswk )
{
  BOX2MAIN_PokeInfoOff( syswk );              // ���ʃN���A

  BOX2OBJ_PokeCursorVanish( syswk, FALSE );
  syswk->get_pos = BOX2MAIN_GETPOS_NONE;
}


//============================================================================================
//  �}�[�L���O
//============================================================================================

static void MarkingChange( BOX2_SYS_WORK * syswk, u32 id, u32 mark )
{
  u16 anm;
  u16 i;

  for( i=0; i<6; i++ ){
    if( mark & (1<<i) ){
      anm = APP_COMMON_POKE_MARK_CIRCLE_BLACK + i*2;
    }else{
      anm = APP_COMMON_POKE_MARK_CIRCLE_WHITE + i*2;
    }
    BOX2OBJ_AnmSet( syswk->app, id+i, anm );
    BOX2OBJ_Vanish( syswk->app, id+i, TRUE );
  }
}

void BOX2MAIN_MainDispMarkingChange( BOX2_SYS_WORK * syswk, u32 mark )
{
  MarkingChange( syswk, BOX2OBJ_ID_MARK1, mark );
}

//--------------------------------------------------------------------------------------------
/**
 * ���ʂ̃}�[�L���O��ύX
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 * @param maek  �}�[�N�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_SubDispMarkingChange( BOX2_SYS_WORK * syswk, u32 mark )
{
  MarkingChange( syswk, BOX2OBJ_ID_MARK1_S, mark );
}

static void SubDispMarkingOff( BOX2_APP_WORK * appwk )
{
  u32 i;

  for( i=0; i<6; i++ ){
    BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_MARK1_S+i, FALSE );
  }
}

// ���A�E�|�P���X�}�[�N�\��
static void PutSubDispPokeMark( BOX2_APP_WORK * appwk, BOX2_POKEINFO_DATA * info )
{
  if( info->rare == 0 ){
    BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_RARE, FALSE );
  }else{
    BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_RARE, TRUE );
  }

  // �|�P���X�Ɋ������Ă��Ȃ�
  if( info->pokerus == 0 ){
    BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_POKERUS, FALSE );
    BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_POKERUS_ICON, FALSE );
  // �|�P���X�Ɋ������Ă���
  }else if( info->pokerus == 1 ){
    BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_POKERUS, FALSE );
    BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_POKERUS_ICON, TRUE );
  // �|�P���X�Ɋ����������Ƃ�����
  }else{
    BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_POKERUS, TRUE );
    BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_POKERUS_ICON, FALSE );
  }
}



//============================================================================================
//  �G���A�`�F�b�N
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �G���A�`�F�b�N
 *
 * @param x   �w���W
 * @param y   �x���W
 * @param area  �`�F�b�N�e�[�u��
 *
 * @retval  "TRUE = �͈͓�"
 * @retval  "FALSE = �͈͊O"
 */
//--------------------------------------------------------------------------------------------
static BOOL AreaCheck( s16 x, s16 y, const AREA_CHECK * area )
{
  if( x >= area->lx && x <= area->rx && y >= area->ty && y < area->by ){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �g���C�G���A�`�F�b�N
 *
 * @param x   �w���W
 * @param y   �x���W
 * @param area  �`�F�b�N�e�[�u��
 *
 * @retval  "TRUE = �͈͓�"
 * @retval  "FALSE = �͈͊O"
 */
//--------------------------------------------------------------------------------------------
static u32 TrayPokePutAreaCheck( s16 x, s16 y )
{
  if( AreaCheck( x, y, &TrayPokeArea ) == TRUE ){
    if( x < TRAYGRID_LX ){
      x = 0;
    }else if( x >= TRAYGRID_RX ){
      x = BOX2OBJ_POKEICON_H_MAX - 1;
    }else{
      x = ( x - TRAYGRID_LX ) / TRAYGRID_SX;
    }
    y = ( y - TRAYAREA_UY ) / TRAYGRID_SY;
    return ( y * BOX2OBJ_POKEICON_H_MAX + x );
  }
  return BOX2MAIN_GETPOS_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���G���A�`�F�b�N
 *
 * @param x   �w���W
 * @param y   �x���W
 * @param area  �`�F�b�N�e�[�u��
 *
 * @retval  "BOX2MAIN_GETPOS_NONE = �͈͓�"
 * @retval  "����ȊO = �I���ʒu"
 */
//--------------------------------------------------------------------------------------------
static u32 PartyPokePutAreaCheck( s16 x, s16 y, const AREA_CHECK * area )
{
  u32 i;

  for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
    if( AreaCheck( x, y, &area[i] ) == TRUE ){
      return ( i + BOX2OBJ_POKEICON_TRAY_MAX );
    }
  }
  return BOX2MAIN_GETPOS_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�ړ��g���C�G���A�`�F�b�N
 *
 * @param x   �w���W
 * @param y   �x���W
 *
 * @retval  "BOX2MAIN_GETPOS_NONE = �͈͓�"
 * @retval  "����ȊO = �I���ʒu"
 */
//--------------------------------------------------------------------------------------------
static u32 BoxMovePutAreaCheck( BOX2_SYS_WORK * syswk, s16 x, s16 y )
{
/*
  u32 i;

  for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
    if( AreaCheck( x, y, &BoxMoveTrayArea[i] ) == TRUE ){
      return ( BOX2MAIN_BOXMOVE_FLG + i );
    }
  }
*/
  u32 ret = BOX2UI_HitCheckTrayIcon( syswk->app->tpx, syswk->app->tpy );

  if( ret != BOX2MAIN_GETPOS_NONE ){
    u32 pos = ret - BOX2OBJ_POKEICON_PUT_MAX;
    pos = syswk->box_mv_pos + pos;
    if( pos >= syswk->trayMax ){
      pos -= syswk->trayMax;
    }
    if( pos == syswk->tray ){
      return BOX2MAIN_GETPOS_NONE;
    }
  }

  return ret;
}



//============================================================================================
//  �T�u�v���Z�X
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X��ʌĂяo��
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  0
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_PokeStatusCall( BOX2_SYS_WORK * syswk )
{
  FS_EXTERN_OVERLAY(poke_status);

  PSTATUS_DATA * pst = GFL_HEAP_AllocMemory( HEAPID_BOX_SYS, sizeof(PSTATUS_DATA) );

  if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
    pst->ppd = BOX2MAIN_PPPGet( syswk, syswk->get_tray, 0 );
    pst->ppt = PST_PP_TYPE_POKEPASO;
    pst->max = BOX2OBJ_POKEICON_TRAY_MAX;
    pst->pos = syswk->get_pos;
  }else{
    pst->ppd = syswk->dat->pokeparty;
    pst->ppt = PST_PP_TYPE_POKEPARTY;
    pst->max = PokeParty_GetPokeCount( syswk->dat->pokeparty );
    pst->pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
  }
  pst->page       = PPT_INFO;
  pst->game_data  = syswk->dat->gamedata;
  pst->cfg        = syswk->dat->cfg;
  pst->zukan_mode = syswk->dat->zknMode;
  pst->mode       = PST_MODE_NORMAL;

  GFL_PROC_LOCAL_CallProc( syswk->localProc, FS_OVERLAY_ID(poke_status), &PokeStatus_ProcData, pst );

  syswk->subProcWork = pst;

  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X��ʏI��
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  0
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_PokeStatusExit( BOX2_SYS_WORK * syswk )
{
  PSTATUS_DATA * pst = syswk->subProcWork;

  if( syswk->poke_get_key == 0 ){
    if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
      syswk->get_pos = pst->pos;
    }else{
      syswk->get_pos = pst->pos + BOX2OBJ_POKEICON_TRAY_MAX;
    }
  }

  GFL_HEAP_FreeMemory( syswk->subProcWork );

  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * �o�b�O��ʌĂяo��
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  0
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_BagCall( BOX2_SYS_WORK * syswk )
{
  BAG_PARAM * wk = BAG_CreateParam( syswk->dat->gamedata, NULL, BAG_MODE_POKELIST, HEAPID_BOX_SYS );

  GFL_PROC_LOCAL_CallProc( syswk->localProc, FS_OVERLAY_ID(bag), &ItemMenuProcData, wk );

  syswk->subProcWork = wk;

  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * �o�b�O��ʏI��
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  0
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_BagExit( BOX2_SYS_WORK * syswk )
{
  BAG_PARAM * wk = syswk->subProcWork;

  syswk->subRet = wk->ret_item;

  GFL_HEAP_FreeMemory( syswk->subProcWork );

  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ���O���͉�ʌĂяo��
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  0
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_NameInCall( BOX2_SYS_WORK * syswk )
{
  MISC        *misc = SaveData_GetMisc( GAMEDATA_GetSaveControlWork(syswk->dat->gamedata) );
  BOX_NAMEIN_WORK * wk;

  wk = GFL_HEAP_AllocClearMemory( HEAPID_BOX_SYS, sizeof(BOX_NAMEIN_WORK) );

  wk->name = GFL_STR_CreateBuffer( BOX_TRAYNAME_BUFSIZE, HEAPID_BOX_SYS );
  BOXDAT_GetBoxName( syswk->dat->sv_box, syswk->box_mv_pos, wk->name );

  wk->prm = NAMEIN_AllocParam(
              HEAPID_BOX_SYS, NAMEIN_BOX, 0, 0, BOX_TRAYNAME_MAXLEN, wk->name, misc );
  GFL_PROC_LOCAL_CallProc( syswk->localProc, FS_OVERLAY_ID(namein), &NameInputProcData, wk->prm );

  syswk->subProcWork = wk;

  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ���O���͉�ʏI��
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  0
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_NameInExit( BOX2_SYS_WORK * syswk )
{
  BOX_NAMEIN_WORK * wk = syswk->subProcWork;

  if( wk->prm->cancel == FALSE ){
    BOXDAT_SetBoxName( syswk->dat->sv_box, syswk->box_mv_pos, wk->prm->strbuf );
  }
  syswk->subRet = wk->prm->cancel;

  NAMEIN_FreeParam( wk->prm );
  GFL_STR_DeleteBuffer( wk->name );
  GFL_HEAP_FreeMemory( syswk->subProcWork );

  return 0;
}


//============================================================================================
//  VBLANK FUNC
//============================================================================================

static void MoveGetPokeIcon( BOX2_SYS_WORK * syswk, u32 x, u32 y )
{
  BOX2OBJ_SetPos(
    syswk->app, syswk->app->pokeicon_id[BOX2OBJ_POKEICON_GET_POS], (s16)x, (s16)y-8, CLSYS_DEFREND_MAIN );
  BOX2OBJ_PokeCursorMove( syswk->app, BOX2OBJ_POKEICON_GET_POS );
  syswk->app->tpx = x;
  syswk->app->tpy = y;
}


//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �擾�|�P�����ړ��i�p�[�e�B�j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncGetPokeMoveParty( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;
  u32 put_pos;
  u32 tpx, tpy;
//  s16 ipx, ipy;
  u32 hit;
  BOOL  ret1, ret2;

  vf = &syswk->app->vfunk;

/*
  if( vf->seq == 0 ){
    BOX2OBJ_GetPos(
      syswk->app, syswk->app->pokeicon_id[syswk->get_pos], &ipx, &ipy, CLSYS_DEFREND_MAIN );
  }
*/

/*
  ret1 = 0;
  if( vf->seq != SEIRI_SEQ_PARTYFRM_IN && vf->seq != SEIRI_SEQ_PARTYFRM_OUT ){
    if( BGWINFRM_MoveCheck( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY ) == 1 ){
      ret1 = BOX2BGWFRM_PartyPokeFrameMove( syswk );
    }else{
      ret1 = 0;
    }
  }
*/
  ret1 = BOX2BGWFRM_PartyPokeFrameMove( syswk );
  ret2 = BOX2BGWFRM_PokeMenuMoveMain( syswk->app->wfrm );

  switch( vf->seq ){
  case SEIRI_SEQ_MENU_OUT:    // ���j���[�A�E�g
    if( GFL_UI_TP_GetCont() == TRUE ){
      BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
    }
    vf->seq = SEIRI_SEQ_ICON_GET;
    break;

  case SEIRI_SEQ_ICON_GET:    // �A�C�R���擾��

    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == FALSE ){
      syswk->app->mode_chg_flg = 0;
      vf->cnt = 0;

      put_pos = BOX2MAIN_GETPOS_NONE;

      // �g���C�A�C�R��
      if( BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == TRUE ){
        BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
        put_pos = BoxMovePutAreaCheck( syswk, syswk->app->tpx, syswk->app->tpy );
        if( put_pos == BOX2MAIN_GETPOS_NONE ){
          BOX2BGWFRM_BoxMoveFrmOutSet( syswk->app->wfrm );
          PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
          vf->seq = SEIRI_SEQ_CANCEL_TRAYFRM_OUT;
          break;
        }
      }

      // �莝���|�P�����i�E�j
      if( BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == TRUE ){
        put_pos = PartyPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy, PartyPokeAreaRight );
        // �Q�ƒ��̃g���C
        if( put_pos == BOX2MAIN_GETPOS_NONE ){
          put_pos = TrayPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy );
        }
      }
      // �莝���|�P�����i���j
      if( BOX2BGWFRM_CheckPartyPokeFrameLeft( syswk->app->wfrm ) == TRUE ){
        put_pos = PartyPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy, PartyPokeAreaLeft );
      }

      // �o�g���{�b�N�X�����b�N����Ă���Ƃ��͈ړ������Ȃ�
      if( syswk->dat->callMode == BOX_MODE_BATTLE && syswk->dat->bbRockFlg == TRUE ){
        put_pos = BOX2MAIN_GETPOS_NONE;
        syswk->app->mv_err_code = BOX2MAIN_ERR_CODE_ROCK;
      }

      PokeIconMoveBoxPartyDataMake( syswk, syswk->get_pos, put_pos );
      if( BOX2BGWFRM_PokeMenuOpenPutCheck( syswk->app->wfrm ) == FALSE ){
        BOX2OBJ_PokeCursorVanish( syswk, FALSE );
      }

      // �ړ���̃J�[�\���ʒu�␳
      if( put_pos != BOX2MAIN_GETPOS_NONE ){
        syswk->get_pos = PokeIconMoveAfterPosGet( syswk, BOX2OBJ_POKEICON_GET_POS );
      }

      // ���j���[�\���J�n
      if( BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == FALSE &&
          BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == FALSE &&
          ret1 == FALSE ){
        BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
      }

      vf->seq = SEIRI_SEQ_ICON_PUT;

    }else{
      if( vf->cnt != 0 ){
        vf->cnt--;
      }
      // �{�b�N�X�ړ��t���[���\���`�F�b�N
      if( ret1 == FALSE && ret2 == FALSE ){
        if( BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == FALSE &&
            BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == FALSE &&
            BOX2UI_HitCheckPartyFrameLeft() == FALSE ){
          syswk->get_tray  = BOX2MAIN_GETPOS_NONE;    // �擾�����{�b�N�X
          syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_ALL;
          BOX2BGWFRM_PartyPokeFrameRightMoveSet( syswk->app->wfrm );
          PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
          // �߂��̃V�[�P���X���u���ށv�̏����ɂ���
          BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
          BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
          BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY_MOVE, syswk->get_pos );
          if( syswk->dat->callMode == BOX_MODE_BATTLE ){
            CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_PTGT_BOXLIST );
          }
//          BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
          BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_PASSIVE );
          syswk->app->old_cur_pos = syswk->get_pos;
          syswk->app->vnext_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_DATA_CHANGE;
        }
        if( vf->cnt == 0 ){
          // �g���C�X�N���[�����q�b�g�`�F�b�N
          hit = BOX2UI_HitCheckContTrayArrow();
          if( hit == 0 ){
            PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
            BOX2MAIN_InitTrayScroll( syswk, BOX2MAIN_TRAY_SCROLL_L );
            vf->seq = SEIRI_SEQ_SCROLL_L;
          }else if( hit == 1 ){
            PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
            BOX2MAIN_InitTrayScroll( syswk, BOX2MAIN_TRAY_SCROLL_R );
            vf->seq = SEIRI_SEQ_SCROLL_R;
          }
        }
        // �{�b�N�X/�莝���؂�ւ��{�^���q�b�g�`�F�b�N
        if( syswk->dat->callMode != BOX_MODE_BATTLE ){
          if( BOX2UI_HitCheckModeChange() != GFL_UI_TP_HIT_NONE ){
            if( syswk->app->mode_chg_flg == 0 ){
              if( BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == TRUE ){
                BOX2BGWFRM_BoxMoveFrmOutSet( syswk->app->wfrm );
                BOX2MAIN_InitBgButtonAnm( syswk, BOX2MAIN_WINFRM_POKE_BTN );
                PMSND_PlaySE( SE_BOX2_DECIDE );
                PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
                syswk->app->mode_chg_flg = 1;
                vf->seq = SEIRI_SEQ_TRAYFRM_OUT;
              }
              if( BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == TRUE ){
                BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );
                BOX2MAIN_InitBgButtonAnm( syswk, BOX2MAIN_WINFRM_BOXLIST_BTN );
                PMSND_PlaySE( SE_BOX2_DECIDE );
                PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
                syswk->app->mode_chg_flg = 1;
                vf->seq = SEIRI_SEQ_PARTYFRM_OUT;
              }
            }
          }else{
            syswk->app->mode_chg_flg = 0;
          }
        }
        // �g���C�A�C�R���X�N���[���q�b�g�`�F�b�N
        if( BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == TRUE ){
          hit = BOX2UI_HitCheckContTrayIconScroll();
          if( hit == 0 ){
            PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
            BOX2MAIN_InitTrayIconScroll( syswk, -1 );
            vf->seq = SEIRI_SEQ_TI_SCROLL_U;
            vf->cnt = 0;
          }else if( hit == 1 ){
            PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
            BOX2MAIN_InitTrayIconScroll( syswk, 1 );
            vf->seq = SEIRI_SEQ_TI_SCROLL_D;
            vf->cnt = 0;
          }
        }
        // �{�b�N�X���\��
        if( BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == TRUE ){
          hit = BOX2UI_HitCheckTrayIcon( tpx, tpy );
          if( hit != BOX2MAIN_GETPOS_NONE ){
            BOX2OBJ_ChangeTrayName( syswk, hit-BOX2OBJ_POKEICON_PUT_MAX, TRUE );
          }else{
            BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
          }
        }
      }
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_ICON_PUT:  // �A�C�R���z�u
//    if( PokeIconObjDropChange( syswk ) == FALSE ){
    if( PokeIconObjMove( syswk ) == FALSE ){
      PMSND_PlaySE( SE_BOX2_POKE_PUT );
      vf->seq = SEIRI_SEQ_ICON_PUT_END;
    }
    break;

  case SEIRI_SEQ_ICON_PUT_END:
    if( ret1 == FALSE && ret2 == FALSE ){
  //    if( BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == FALSE ){
        vf->seq = SEIRI_SEQ_END;
/*
      }else{
        BOX2BGWFRM_BoxMoveFrmOutSet( syswk->app->wfrm );
        PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
        vf->seq = 0xff;
      }
*/
    }
    break;

  case SEIRI_SEQ_SCROLL_L:  // �g���C�X�N���[���i���j
    if( BOX2MAIN_VFuncTrayScrollLeft( syswk ) == 0 ){
//      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = SEIRI_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_SCROLL_R:  // �g���C�X�N���[���i�E�j
    if( BOX2MAIN_VFuncTrayScrollRight( syswk ) == 0 ){
//      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = SEIRI_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_TI_SCROLL_U:   // �g���C�A�C�R���X�N���[���i��j
    if( BOX2MAIN_VFuncTrayIconScrollDown( syswk ) == 0 ){
      vf->seq = SEIRI_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_TI_SCROLL_D:   // �g���C�A�C�R���X�N���[���i���j
    if( BOX2MAIN_VFuncTrayIconScrollUp( syswk ) == 0 ){
      vf->seq = SEIRI_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_TRAYFRM_IN:      // �g���C�t���[���C��
    if( BOX2MAIN_VFuncBoxMoveFrmIn( syswk ) == 0 ){
//      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = SEIRI_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_TRAYFRM_OUT:     // �g���C�t���[���A�E�g
    BOX2MAIN_ButtonAnmMain( syswk );
    if( BOX2MAIN_VFuncBoxMoveFrmOut( syswk ) == 0 ){
      BOX2BGWFRM_PartyPokeFrameInitPutRight( syswk->app->wfrm );
      BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
      BOX2OBJ_PartyPokeIconFrmInSet( syswk );
      BOX2BGWFRM_TemochiButtonOff( syswk->app );
      BOX2BGWFRM_BoxListButtonOn( syswk->app );
      PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
      vf->seq = SEIRI_SEQ_PARTYFRM_IN;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_PARTYFRM_IN:     // �p�[�e�B�t���[���C��
    if( ret1 == FALSE ){
//      syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_ALL;
//      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = SEIRI_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_PARTYFRM_OUT:    // �p�[�e�B�t���[���A�E�g
    BOX2MAIN_ButtonAnmMain( syswk );
    if( ret1 == FALSE ){
      BOX2MAIN_InitBoxMoveFrameScroll( syswk );
      BOX2BGWFRM_BoxListButtonOff( syswk->app );
      BOX2BGWFRM_TemochiButtonOn( syswk->app );
      PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
      vf->seq = SEIRI_SEQ_TRAYFRM_IN;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_END:       // �I��
    {
      BOX2MAIN_POKEMOVE_WORK * work = vf->work;
      PokeIconBufPosChange( syswk, work->dat );
    }
    if( BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == FALSE ){
      BOX2OBJ_PokeCursorMove( syswk->app, syswk->get_pos );
      BOX2OBJ_PokeCursorVanish( syswk, TRUE );
    }else{
//      BOX2OBJ_PokeCursorMove( syswk->app, syswk->get_pos );
      BOX2OBJ_PokeCursorVanish( syswk, FALSE );
      CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
      BOX2UI_PutHandCursor( syswk, syswk->get_pos );
      BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );
//      CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
    }
    vf->seq = 0;
    return 0;

  case SEIRI_SEQ_CANCEL_TRAYFRM_OUT:    // �g���[�t���[���A�E�g�i�L�����Z�������j
    if( BOX2MAIN_VFuncBoxMoveFrmOut( syswk ) == 0 ){
      BOX2BGWFRM_PartyPokeFrameInitPutRight( syswk->app->wfrm );
      BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
      PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
      vf->seq = SEIRI_SEQ_CANCEL_PARTYFRM_IN;
    }
    break;

  case SEIRI_SEQ_CANCEL_PARTYFRM_IN:    // �p�[�e�B�t���[���C���i�L�����Z�������j
    if( ret1 == FALSE ){
      syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_ALL;
      // �Q�ƒ��̃g���C
      put_pos = TrayPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy );
      PokeIconMoveBoxPartyDataMake( syswk, syswk->get_pos, put_pos );
      BOX2OBJ_PokeCursorVanish( syswk, FALSE );
      vf->seq = SEIRI_SEQ_ICON_PUT;
    }
    break;
/*
  case 0xff:
    if( BOX2MAIN_VFuncBoxMoveFrmOut( syswk ) == 0 ){
      BOX2BGWFRM_PartyPokeFrameInitPutRight( syswk->app->wfrm );
      BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
      PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
      vf->seq = 0xfe;
    }
    break;

  case 0xfe:
    if( ret1 == FALSE ){
      vf->seq = SEIRI_SEQ_END;
    }
    break;
*/
  }

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �擾�|�P�����ړ��i�p�[�e�B���g���C�j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncGetPokeMoveBoxParty( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;
  u32 tpx, tpy;
  u32 put_pos;
  u32 ret1, ret2;
  u32 hit;

  vf = &syswk->app->vfunk;

  ret1 = 0;
  if( vf->seq != SEIRI_SEQ_TRAYFRM_IN && vf->seq != SEIRI_SEQ_TRAYFRM_OUT ){
    if( BGWINFRM_MoveCheck( syswk->app->wfrm, BOX2MAIN_WINFRM_MOVE ) == 1 ){
      ret1 = BOX2MAIN_VFuncBoxMoveFrmIn( syswk );
    }
  }
  ret2 = BOX2BGWFRM_PokeMenuMoveMain( syswk->app->wfrm );

  switch( vf->seq ){
  case SEIRI_SEQ_MENU_OUT:    // ���j���[�A�E�g
    if( GFL_UI_TP_GetCont() == TRUE ){
      BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
    }
    vf->seq = SEIRI_SEQ_ICON_GET;
    break;

  case SEIRI_SEQ_ICON_GET:    // �A�C�R���擾��

    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == FALSE ){
      syswk->app->mode_chg_flg = 0;

      put_pos = BOX2MAIN_GETPOS_NONE;
      // �g���C�A�C�R��
      if( BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == TRUE ){
        put_pos = BoxMovePutAreaCheck( syswk, syswk->app->tpx, syswk->app->tpy );
      }
      // �莝���|�P����
      if( BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == TRUE ){
        put_pos = PartyPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy, PartyPokeAreaRight );
      }
      // �Q�ƒ��̃g���C
      if( put_pos == BOX2MAIN_GETPOS_NONE ){
        put_pos = TrayPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy );
      }
/*
      if( put_pos == BOX2MAIN_GETPOS_NONE ){
        put_pos = PartyPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy, PartyPokeAreaRight );
      }
*/
      PokeIconMoveBoxPartyDataMake( syswk, syswk->get_pos, put_pos );
      if( BOX2BGWFRM_PokeMenuOpenPutCheck( syswk->app->wfrm ) == FALSE ){
        BOX2OBJ_PokeCursorVanish( syswk, FALSE );
      }

      // �ړ���̃J�[�\���ʒu�␳
      if( put_pos != BOX2MAIN_GETPOS_NONE ){
        if( put_pos >= BOX2OBJ_POKEICON_PUT_MAX ){
          syswk->get_pos = put_pos - BOX2OBJ_POKEICON_PUT_MAX + BOX2UI_ARRANGE_PGT_TRAY2;
        }else{
          syswk->get_pos = PokeIconMoveAfterPosGet( syswk, BOX2OBJ_POKEICON_GET_POS );
        }
      }else{
        BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
      }

      // ���j���[�\���J�n
      if( BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == FALSE &&
          BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == FALSE &&
          ret1 == 0 ){
        BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
      }

      if( put_pos == BOX2MAIN_GETPOS_NONE && syswk->tray != syswk->get_tray ){
        u32 dir = BOX2MAIN_GetTrayScrollDir( syswk, syswk->tray, syswk->get_tray );
        syswk->tray = syswk->get_tray;
        BOX2OBJ_PokeIconBufLoad( syswk, syswk->tray );
        BOX2MAIN_WallPaperSet( syswk, BOX2MAIN_GetWallPaperNumber(syswk,syswk->tray), dir );
        if( dir == BOX2MAIN_TRAY_SCROLL_L ){
          vf->seq = SEIRI_SEQ_CANCEL_SCROLL_L;
        }else{
          vf->seq = SEIRI_SEQ_CANCEL_SCROLL_R;
        }
      }else{
        vf->seq = SEIRI_SEQ_ICON_PUT;
      }
      vf->cnt = 0;

    }else{
      if( vf->cnt != 0 ){
        vf->cnt--;
      }
      // �{�b�N�X�ړ��t���[���\���`�F�b�N
      if( ret1 == 0 && ret2 == 0 &&
          BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == FALSE &&
          BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == FALSE ){
        if( BOX2UI_HitCheckPosTrayPoke( tpx, tpy ) != syswk->get_pos ){
          syswk->box_mv_pos = syswk->tray;  // ���ݎQ�Ƃ��Ă���{�b�N�X
          BOX2MAIN_InitBoxMoveFrameScroll( syswk );
          PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
          // �߂��̃V�[�P���X���u���ށv�̏����ɂ���
          BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, syswk->get_pos );
          BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_PASSIVE );
          syswk->app->old_cur_pos = syswk->get_pos;
          syswk->app->vnext_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_DATA_CHANGE;
        }
      }
      if( vf->cnt == 0 ){
        // �g���C�X�N���[�����q�b�g�`�F�b�N
        hit = BOX2UI_HitCheckContTrayArrow();
        if( hit == 0 ){
          PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
          BOX2MAIN_InitTrayScroll( syswk, BOX2MAIN_TRAY_SCROLL_L );
          vf->seq = SEIRI_SEQ_SCROLL_L;
        }else if( hit == 1 ){
          PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
          BOX2MAIN_InitTrayScroll( syswk, BOX2MAIN_TRAY_SCROLL_R );
          vf->seq = SEIRI_SEQ_SCROLL_R;
        }
      }
      // �{�b�N�X/�莝���؂�ւ��{�^���q�b�g�`�F�b�N
      if( ret1 == 0 && ret2 == 0 ){
        if( BOX2UI_HitCheckModeChange() != GFL_UI_TP_HIT_NONE ){
          if( syswk->app->mode_chg_flg == 0 ){
            if( BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == TRUE ){
              BOX2BGWFRM_BoxMoveFrmOutSet( syswk->app->wfrm );
              BOX2MAIN_InitBgButtonAnm( syswk, BOX2MAIN_WINFRM_POKE_BTN );
              PMSND_PlaySE( SE_BOX2_DECIDE );
              PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
              syswk->app->mode_chg_flg = 1;
              vf->seq = SEIRI_SEQ_TRAYFRM_OUT;
            }
            if( BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == TRUE ){
              BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );
              BOX2MAIN_InitBgButtonAnm( syswk, BOX2MAIN_WINFRM_BOXLIST_BTN );
              PMSND_PlaySE( SE_BOX2_DECIDE );
              PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
              syswk->app->mode_chg_flg = 1;
              vf->seq = SEIRI_SEQ_PARTYFRM_OUT;
            }
          }
        }else{
          syswk->app->mode_chg_flg = 0;
        }
      }
      // �g���C�A�C�R���X�N���[���q�b�g�`�F�b�N
      if( BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == TRUE ){
        hit = BOX2UI_HitCheckContTrayIconScroll();
        if( hit == 0 ){
          PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
          BOX2MAIN_InitTrayIconScroll( syswk, -1 );
          vf->seq = SEIRI_SEQ_TI_SCROLL_U;
          vf->cnt = 0;
        }else if( hit == 1 ){
          PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
          BOX2MAIN_InitTrayIconScroll( syswk, 1 );
          vf->seq = SEIRI_SEQ_TI_SCROLL_D;
          vf->cnt = 0;
        }
      }

      // �{�b�N�X���\��
      if( BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == TRUE ){
        hit = BOX2UI_HitCheckTrayIcon( tpx, tpy );
        if( hit != BOX2MAIN_GETPOS_NONE ){
          BOX2OBJ_ChangeTrayName( syswk, hit-BOX2OBJ_POKEICON_PUT_MAX, TRUE );
        }else{
          BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
        }
      }
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_CANCEL_SCROLL_L:   // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i���j
    if( BOX2MAIN_VFuncTrayScrollLeft( syswk ) == 0 ){
      vf->seq = SEIRI_SEQ_ICON_PUT;
    }
    break;

  case SEIRI_SEQ_CANCEL_SCROLL_R:   // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i�E�j
    if( BOX2MAIN_VFuncTrayScrollRight( syswk ) == 0 ){
      vf->seq = SEIRI_SEQ_ICON_PUT;
    }
    break;

  case SEIRI_SEQ_ICON_PUT:  // �A�C�R���z�u
    if( PokeIconObjMove( syswk ) == FALSE ){
      PMSND_PlaySE( SE_BOX2_POKE_PUT );
      vf->seq = SEIRI_SEQ_ICON_PUT_END;
    }
    break;

  case SEIRI_SEQ_ICON_PUT_END:
    if( ret1 == 0 && ret2 == 0 ){
      vf->seq = SEIRI_SEQ_END;
    }
    break;

  case SEIRI_SEQ_SCROLL_L:  // �g���C�X�N���[���i���j
    if( BOX2MAIN_VFuncTrayScrollLeft( syswk ) == 0 ){
//      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = SEIRI_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_SCROLL_R:  // �g���C�X�N���[���i�E�j
    if( BOX2MAIN_VFuncTrayScrollRight( syswk ) == 0 ){
//      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = SEIRI_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_TI_SCROLL_U:   // �g���C�A�C�R���X�N���[���i��j
    if( BOX2MAIN_VFuncTrayIconScrollDown( syswk ) == 0 ){
      vf->seq = SEIRI_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_TI_SCROLL_D:   // �g���C�A�C�R���X�N���[���i���j
    if( BOX2MAIN_VFuncTrayIconScrollUp( syswk ) == 0 ){
      vf->seq = SEIRI_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_TRAYFRM_IN:      // �g���C�t���[���C��
    if( BOX2MAIN_VFuncBoxMoveFrmIn( syswk ) == 0 ){
      syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_TRAY;
//      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = SEIRI_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_TRAYFRM_OUT:     // �g���C�t���[���A�E�g
    BOX2MAIN_ButtonAnmMain( syswk );
    if( BOX2MAIN_VFuncBoxMoveFrmOut( syswk ) == 0 ){
      BOX2BGWFRM_PartyPokeFrameInitPutRight( syswk->app->wfrm );
      BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
      BOX2OBJ_PartyPokeIconFrmInSet( syswk );
      BOX2BGWFRM_TemochiButtonOff( syswk->app );
      BOX2BGWFRM_BoxListButtonOn( syswk->app );
      PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
      vf->seq = SEIRI_SEQ_PARTYFRM_IN;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_PARTYFRM_IN:     // �p�[�e�B�t���[���C��
    if( BOX2BGWFRM_PartyPokeFrameMove( syswk ) == FALSE ){
      syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_ALL;
//      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = SEIRI_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_PARTYFRM_OUT:    // �p�[�e�B�t���[���A�E�g
    BOX2MAIN_ButtonAnmMain( syswk );
    if( BOX2BGWFRM_PartyPokeFrameMove( syswk ) == FALSE ){
      BOX2MAIN_InitBoxMoveFrameScroll( syswk );
      BOX2BGWFRM_BoxListButtonOff( syswk->app );
      BOX2BGWFRM_TemochiButtonOn( syswk->app );
      PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
      vf->seq = SEIRI_SEQ_TRAYFRM_IN;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_END:       // �I��
    {
      BOX2MAIN_POKEMOVE_WORK * work = vf->work;
      PokeIconBufPosChange( syswk, work->dat );
    }
    if( BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == FALSE &&
        BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == FALSE ){
      BOX2OBJ_PokeCursorMove( syswk->app, syswk->get_pos );
      BOX2OBJ_PokeCursorVanish( syswk, TRUE );
    }else{
//      BOX2OBJ_PokeCursorMove( syswk->app, syswk->get_pos );
      BOX2OBJ_PokeCursorVanish( syswk, FALSE );
      CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
      BOX2UI_PutHandCursor( syswk, syswk->get_pos );
      BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );
//      CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
    }
    vf->seq = 0;
    return 0;
  }

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �莝���𓦂��������OBJ����
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncPartyPokeFreeSort( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;

  vf = &syswk->app->vfunk;

  switch( vf->seq ){
  case 0:
    PokeIconFreeDataMake( syswk, syswk->get_pos );
    vf->seq++;
  case 1:
    if( PokeIconObjMove( syswk ) == FALSE ){
      BOX2MAIN_POKEMOVE_WORK * work = vf->work;
      PokeIconBufPosChange( syswk, work->dat );
      vf->seq = 0;
      return 0;
    }
  }
  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �|�P�������莝���ɉ����铮��
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncPartyInPokeMove( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;
  u32 put_pos;

  vf = &syswk->app->vfunk;

  switch( vf->seq ){
  case 0:
    put_pos = PokeParty_GetPokeCount( syswk->dat->pokeparty ) + BOX2OBJ_POKEICON_TRAY_MAX;
    PokeIconMoveBoxPartyDataMake( syswk, syswk->get_pos, put_pos );
    BOX2OBJ_PokeCursorVanish( syswk, FALSE );
    vf->seq++;
    break;

  case 1:
    if( PokeIconObjMove( syswk ) == FALSE ){
      BOX2MAIN_POKEMOVE_WORK * work = vf->work;
      PokeIconBufPosChange( syswk, work->dat );
      PMSND_PlaySE( SE_BOX2_POKE_PUT );
      vf->seq = 0;
      return 0;
    }
  }
  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �g���C���X�N���[���i���{�^���������ꂽ�Ƃ��j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncTrayScrollLeft( BOX2_SYS_WORK * syswk )
{
  if( syswk->app->vfunk.cnt == BOX2MAIN_TRAY_SCROLL_CNT ){
    BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );
/*
    if( CURSORMOVE_CursorOnOffGet( syswk->app->cmwk ) == FALSE ){
      CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
    }
*/
    if( BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == TRUE ){
      BOX2OBJ_EndTrayCursorScroll( syswk );
    }
    syswk->app->vfunk.cnt = 0;
    return 0;
  }
  GFL_BG_SetScrollReq( GFL_BG_FRAME3_M, GFL_BG_SCROLL_X_DEC, BOX2MAIN_TRAY_SCROLL_SPD );
  BOX2OBJ_TrayPokeIconScroll( syswk, BOX2MAIN_TRAY_SCROLL_SPD );
  BOX2OBJ_BoxNameScroll( syswk->app, BOX2MAIN_TRAY_SCROLL_SPD );

  BOX2BGWFRM_PokeMenuMoveMain( syswk->app->wfrm );

  syswk->app->vfunk.cnt++;

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �g���C���X�N���[���i�E�{�^���������ꂽ�Ƃ��j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncTrayScrollRight( BOX2_SYS_WORK * syswk )
{
  if( syswk->app->vfunk.cnt == BOX2MAIN_TRAY_SCROLL_CNT ){
    BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );
/*
    if( CURSORMOVE_CursorOnOffGet( syswk->app->cmwk ) == FALSE ){
      CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
    }
*/
    if( BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == TRUE ){
      BOX2OBJ_EndTrayCursorScroll( syswk );
    }
    syswk->app->vfunk.cnt = 0;
    return 0;
  }
  GFL_BG_SetScrollReq( GFL_BG_FRAME3_M, GFL_BG_SCROLL_X_INC, BOX2MAIN_TRAY_SCROLL_SPD );
  BOX2OBJ_TrayPokeIconScroll( syswk, -BOX2MAIN_TRAY_SCROLL_SPD );
  BOX2OBJ_BoxNameScroll( syswk->app, -BOX2MAIN_TRAY_SCROLL_SPD );

  BOX2BGWFRM_PokeMenuMoveMain( syswk->app->wfrm );

  syswk->app->vfunk.cnt++;

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �|�P�������j���[����
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncPokeMenuMove( BOX2_SYS_WORK * syswk )
{
  if( BOX2BGWFRM_PokeMenuMoveMain( syswk->app->wfrm ) == FALSE ){
    return 0;
  }
  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ���j���[�E�u�Ă����|�P�����v�E�u�|�P�������ǂ��v�E�u���ǂ�v����
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
/*
int BOX2MAIN_VFuncPokeFreeMenuMove( BOX2_SYS_WORK * syswk )
{
  BOOL  menu_mv;
  u32   ret[3];

  menu_mv = BOX2BGWFRM_PokeMenuMoveMain( syswk->app->wfrm );
//  ret[0]  = BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_POKE_BTN );
//  ret[1]  = BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_MV_BTN );
//  ret[2]  = BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_RET_BTN );
  ret[0]  = 0;
  ret[1]  = 0;
  ret[2]  = 0;

  if( menu_mv == FALSE && ret[0] == 0 && ret[1] == 0 && ret[2] == 0 ){
    return 0;
  }
  return 1;
}
*/


//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �}�[�L���O�t���[������
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncMarkingFrameMove( BOX2_SYS_WORK * syswk )
{
  return BOX2BGWFRM_MarkingFrameMove( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �u�Ă����|�P�����v�u�|�P�������ǂ��v�u�Ƃ���v�u���ǂ�v�Ȃǂ̃{�^������
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
/*
int BOX2MAIN_VFuncUnderButtonMove( BOX2_SYS_WORK * syswk )
{
  u32 ret[4];

//  ret[0] = BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_POKE_BTN );
//  ret[1] = BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_MV_BTN );
//  ret[2] = BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_RET_BTN );
//  ret[3] = BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_CLOSE_BTN );
  ret[0] = 0;
  ret[1] = 0;
  ret[2] = 0;
  ret[3] = 0;

  if( ret[0] == 0 && ret[1] == 0 && ret[2] == 0 && ret[3] == 0 ){
    return 0;
  }
  return 1;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �莝���|�P�����t���[������
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncPartyFrameMove( BOX2_SYS_WORK * syswk )
{
  if( BOX2BGWFRM_PartyPokeFrameMove( syswk ) == FALSE ){
    return 0;
  }
  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �莝���|�P�����t���[�����E��
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
/*
int BOX2MAIN_VFuncPartyFrmRight( BOX2_SYS_WORK * syswk )
{
  if( BOX2BGWFRM_PartyPokeFrameMove( syswk ) == FALSE ){
    return 0;
  }

  BOX2BGWFRM_PokeMenuMoveMain( syswk->app->wfrm );

  return 1;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �{�b�N�X�I���E�B���h�E�C��
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncBoxMoveFrmIn( BOX2_SYS_WORK * syswk )
{
  int ret;

  ret = BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_MOVE );

  BOX2OBJ_BoxMoveFrmScroll( syswk, 8 );

  return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �{�b�N�X�I���E�B���h�E�A�E�g
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncBoxMoveFrmOut( BOX2_SYS_WORK * syswk )
{
  int ret;

  ret = BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_MOVE );

  BOX2OBJ_BoxMoveFrmScroll( syswk, -8 );

  return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �擾�|�P�����ړ��i�|�P��������������j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_GetPartyPokeMoveDrop( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;
  u32 tpx, tpy;
  u32 put_pos;
  BOOL  menu_mv;
  BOOL  party_mv;

  vf = &syswk->app->vfunk;

  menu_mv  = BOX2BGWFRM_PokeMenuMoveMain( syswk->app->wfrm );
  party_mv = BOX2BGWFRM_PartyPokeFrameMove( syswk );

  switch( vf->seq ){
  case AZUKERU_SEQ_MENU_OUT:    // ���j���[�A�E�g
    if( GFL_UI_TP_GetCont() == TRUE ){
      BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
    }
    vf->seq = AZUKERU_SEQ_ICON_GET;
    break;

  case AZUKERU_SEQ_ICON_GET:    // �A�C�R���擾��

    if( BOX2BGWFRM_CheckPartyPokeFrameLeft( syswk->app->wfrm ) == FALSE ){
      BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
    }

    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == FALSE ){

      put_pos = BOX2MAIN_GETPOS_NONE;
      if( BOX2BGWFRM_CheckPartyPokeFrameLeft( syswk->app->wfrm ) == FALSE && party_mv == FALSE ){
        put_pos = TrayPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy );
      }

      PokeIconDropDataMake( syswk, syswk->get_pos, put_pos );

      if( BOX2BGWFRM_PokeMenuOpenPutCheck( syswk->app->wfrm ) == FALSE ){
        BOX2OBJ_PokeCursorVanish( syswk, FALSE );
      }
      syswk->get_pos = PokeIconMoveAfterPosGet( syswk, BOX2OBJ_POKEICON_GET_POS );

      if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
        vf->seq = AZUKERU_SEQ_MOVE_ENTER1;
      }else{
        if( BOX2BGWFRM_CheckPartyPokeFrameLeft( syswk->app->wfrm ) == FALSE ){
          PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
        }
        BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
        BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
        vf->seq = AZUKERU_SEQ_MOVE_CANCEL1;
      }
      vf->cnt = 0;
      break;
    }else{
      if( vf->cnt != 0 ){
        vf->cnt--;
      }
      if( menu_mv == FALSE && party_mv == FALSE ){
        if( BOX2BGWFRM_CheckPartyPokeFrameLeft( syswk->app->wfrm ) == TRUE ){
          if( BOX2UI_HitCheckPartyFrameLeft() == FALSE ){
            syswk->get_tray  = BOX2MAIN_GETPOS_NONE;        // �擾�����{�b�N�X
            syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_TRAY;
            BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );
            PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
          }
        }else{
          if( vf->cnt == 0 ){
            // �g���C�X�N���[�����q�b�g�`�F�b�N
            u32 hit = BOX2UI_HitCheckContTrayArrow();
            if( hit == 0 ){
              PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
              BOX2MAIN_InitTrayScroll( syswk, BOX2MAIN_TRAY_SCROLL_L );
              vf->seq = AZUKERU_SEQ_SCROLL_L;
            }else if( hit == 1 ){
              PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
              BOX2MAIN_InitTrayScroll( syswk, BOX2MAIN_TRAY_SCROLL_R );
              vf->seq = AZUKERU_SEQ_SCROLL_R;
            }
          }
        }
      }
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case AZUKERU_SEQ_SCROLL_L:      // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i���j
    if( BOX2MAIN_VFuncTrayScrollLeft( syswk ) == 0 ){
//      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = AZUKERU_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case AZUKERU_SEQ_SCROLL_R:      // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i�E�j
    if( BOX2MAIN_VFuncTrayScrollRight( syswk ) == 0 ){
//      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = AZUKERU_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case AZUKERU_SEQ_MOVE_CANCEL1:    // �ړ��L�����Z���P
    if( PokeIconObjMove( syswk ) == FALSE ){
      PMSND_PlaySE( SE_BOX2_POKE_PUT );
      vf->seq = AZUKERU_SEQ_MOVE_CANCEL2;
    }
    break;

  case AZUKERU_SEQ_MOVE_CANCEL2:    // �ړ��L�����Z���Q
    if( party_mv == FALSE && menu_mv == FALSE ){
      BOX2MAIN_POKEMOVE_WORK * work = vf->work;
      PokeIconBufPosChange( syswk, work->dat );
      BOX2OBJ_PokeCursorMove( syswk->app, syswk->get_pos );
      BOX2OBJ_PokeCursorVanish( syswk, TRUE );
      vf->seq = AZUKERU_SEQ_END;
    }
    break;

  case AZUKERU_SEQ_MOVE_ENTER1: // �ړ����s�P
    if( PokeIconObjDrop( syswk ) == FALSE ){
      BOX2MAIN_POKEMOVE_WORK * work = vf->work;
      PokeIconBufPosChange( syswk, work->dat );
      BOX2MAIN_PokeInfoOff( syswk );
      PMSND_PlaySE( SE_BOX2_POKE_PUT );
      PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
      BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
      syswk->get_pos = BOX2MAIN_GETPOS_NONE;
      vf->seq = AZUKERU_SEQ_MOVE_ENTER2;
    }
    break;

  case AZUKERU_SEQ_MOVE_ENTER2: // �ړ����s�Q
    if( party_mv == FALSE ){
      vf->seq = AZUKERU_SEQ_END;
    }
    break;

  case AZUKERU_SEQ_END:     // �I��
    BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
    BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
    BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, FALSE );
//    syswk->app->party_frm = 0;
    vf->seq = 0;
    return 0;
  }

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �擾�|�P�����ړ��i�|�P��������Ă����j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_GetTrayPokeMoveDrop( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;
  u32 tpx, tpy;
  u32 put_pos;
  BOOL  menu_mv;
  BOOL  party_mv;

  vf = &syswk->app->vfunk;

  menu_mv  = BOX2BGWFRM_PokeMenuMoveMain( syswk->app->wfrm );
  party_mv = BOX2BGWFRM_PartyPokeFrameMove( syswk );

  switch( vf->seq ){
  case TSURETEIKU_SEQ_MENU_OUT:   // ���j���[�A�E�g
    if( GFL_UI_TP_GetCont() == TRUE ){
      BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
    }
    vf->seq = TSURETEIKU_SEQ_ICON_GET;
    break;

  case TSURETEIKU_SEQ_ICON_GET:   // �A�C�R���擾��
/*
    if( syswk->app->party_frm == 1 && party_mv == FALSE ){
      syswk->app->party_frm = 2;
      BOX2BMP_VBlankMsgPut( syswk, BOX2BMPWIN_ID_MSG4 );
    }
*/
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == FALSE ){

      put_pos = BOX2MAIN_GETPOS_NONE;
      if( BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == TRUE ){
        put_pos = PartyPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy, PartyPokeAreaRight );
      }
      PokeIconPartyDropDataMake( syswk, syswk->get_pos, put_pos );
//      PokeIconMoveBoxPartyDataMake( syswk, syswk->get_pos, put_pos );

      if( BOX2BGWFRM_PokeMenuOpenPutCheck( syswk->app->wfrm ) == FALSE ){
        BOX2OBJ_PokeCursorVanish( syswk, FALSE );
      }
      syswk->get_pos = PokeIconMoveAfterPosGet( syswk, BOX2OBJ_POKEICON_GET_POS );

/*
      if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
        vf->seq = TSURETEIKU_SEQ_MOVE_ENTER1;
      }else{
        if( syswk->app->party_frm != 0 ){
          BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );
        }
        vf->seq = TSURETEIKU_SEQ_MOVE_CANCEL1;
      }
*/
      if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
        vf->seq = TSURETEIKU_SEQ_MOVE_ENTER1;
      }else{
        if( party_mv == TRUE ||
            BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == TRUE ){
          PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
          BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );
        }
        vf->seq = TSURETEIKU_SEQ_MOVE_CANCEL1;
      }

    }else{
      if( menu_mv == FALSE && party_mv == FALSE &&
          BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == FALSE ){
        if( BOX2UI_HitCheckPosTrayPoke( tpx, tpy ) != syswk->get_pos ){
          PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
          BOX2BGWFRM_PartyPokeFrameInitPutRight( syswk->app->wfrm );
          BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
          BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF );
          syswk->app->old_cur_pos = syswk->get_pos;
        }
      }
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

/*
  // �ړ��L�����Z����
  case TSURETEIKU_SEQ_MOVE_CANCEL1: // �ړ��L�����Z���P
    if( party_mv == FALSE ){
      BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
//      BOX2BGWFRM_SubDispWazaFrmInSet( syswk->app );
      vf->seq = TSURETEIKU_SEQ_MOVE_CANCEL2;
    }

  case TSURETEIKU_SEQ_MOVE_CANCEL2: // �ړ��L�����Z���Q
  case TSURETEIKU_SEQ_MOVE_CANCEL3: // �ړ��L�����Z���R
    if( PokeIconObjMove( syswk ) == FALSE &&
        vf->seq == TSURETEIKU_SEQ_MOVE_CANCEL3 &&
        menu_mv == FALSE ){
      BOX2MAIN_POKEMOVE_WORK * work = vf->work;
      PokeIconBufPosChange( syswk, work->dat );
      BOX2OBJ_PokeCursorMove( syswk->app, syswk->get_pos );
      BOX2OBJ_PokeCursorVanish( syswk, TRUE );
      vf->seq = TSURETEIKU_SEQ_END;
      break;
    }
    if( vf->seq == TSURETEIKU_SEQ_MOVE_CANCEL2 ){
      vf->seq = TSURETEIKU_SEQ_MOVE_CANCEL3;
    }
    break;
*/
  // �ړ��L�����Z����
  case TSURETEIKU_SEQ_MOVE_CANCEL1: // �ړ��L�����Z���P
    if( PokeIconObjMove( syswk ) == FALSE ){
      PMSND_PlaySE( SE_BOX2_POKE_PUT );
      vf->seq = TSURETEIKU_SEQ_MOVE_CANCEL2;
    }
    if( party_mv == FALSE ){
      BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
    }
    break;

  case TSURETEIKU_SEQ_MOVE_CANCEL2: // �ړ��L�����Z���Q
    if( party_mv == FALSE ){
      BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
      vf->seq = TSURETEIKU_SEQ_MOVE_CANCEL3;
    }
    break;

  case TSURETEIKU_SEQ_MOVE_CANCEL3: // �ړ��L�����Z���R
    if( menu_mv == FALSE ){
      BOX2MAIN_POKEMOVE_WORK * work = vf->work;
      PokeIconBufPosChange( syswk, work->dat );
      BOX2OBJ_PokeCursorMove( syswk->app, syswk->get_pos );
      BOX2OBJ_PokeCursorVanish( syswk, TRUE );
      vf->seq = TSURETEIKU_SEQ_END;
      break;
    }
    break;

  // �ړ����s��
  case TSURETEIKU_SEQ_MOVE_ENTER1:  // �ړ����s�P
    if( PokeIconObjMove( syswk ) == FALSE ){
      BOX2MAIN_POKEMOVE_WORK * work = vf->work;
      PokeIconBufPosChange( syswk, work->dat );
      PMSND_PlaySE( SE_BOX2_POKE_PUT );
      PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
      syswk->get_pos = BOX2MAIN_GETPOS_NONE;
      BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );
      vf->seq = TSURETEIKU_SEQ_MOVE_ENTER2;
    }
    break;

  case TSURETEIKU_SEQ_MOVE_ENTER2:  // �ړ����s�Q
    if( party_mv == FALSE ){
      BOX2MAIN_PokeInfoOff( syswk );
      vf->seq = TSURETEIKU_SEQ_END;
    }
    break;

/*
  case TSURETEIKU_SEQ_MOVE_ENTER3:  // �ړ����s�R
    vf->seq = TSURETEIKU_SEQ_END;
    break;
*/

  // �I��
  case TSURETEIKU_SEQ_END:      // �I��
    BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
    BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, FALSE );
//    syswk->app->party_frm = 0;
    vf->seq = 0;
    return 0;
  }

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ��J�[�\���ړ�
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncCursorMove( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;
  BOX2MAIN_CURSORMOVE_WORK * cwk;
  s16 x, y;

  vf  = &syswk->app->vfunk;
  cwk = vf->work;

//  BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_Y_ST_BTN );

  if( cwk->cnt == 0 ){
    BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, cwk->px, cwk->py, CLSYS_DEFREND_MAIN );
    BOX2OBJ_MovePokeIconHand( syswk );
    return 0;
  }
  cwk->cnt--;

  BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, &x, &y, CLSYS_DEFREND_MAIN );

  if( cwk->mx == 0 ){
    x += cwk->vx;
  }else{
    x -= cwk->vx;
  }
  if( cwk->my == 0 ){
    y += cwk->vy;
  }else{
    y -= cwk->vy;
  }
  BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );
  BOX2OBJ_MovePokeIconHand( syswk );

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �߂�������ă��j���[���o��
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncMenuMove( BOX2_SYS_WORK * syswk )
{
  if( BOX2MAIN_VFuncCursorMove( syswk ) == 0 &&
      BOX2BGWFRM_PokeMenuMoveMain( syswk->app->wfrm ) == FALSE ){
    return 0;
  }
  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ���j���[�������Ė߂���o��
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
/*
int BOX2MAIN_VFuncMenuCloseKey( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;
  BOOL  menu_mv;
  int   cur_mv;

  vf = &syswk->app->vfunk;

  switch( vf->seq ){
  case 0:
    menu_mv = BOX2BGWFRM_PokeMenuMoveMain( syswk->app->wfrm );
    cur_mv  = BOX2MAIN_VFuncCursorMove( syswk );
    if( menu_mv == 0 && cur_mv == 0 ){
      vf->seq++;
    }
    break;

  case 1:
//    if( BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_RET_BTN ) == 0 ){
      vf->seq = 0;
      return 0;
//    }
    break;
  }

  return 1;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ���j���[�������Ė߂���o���i�{�b�N�X�����j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
/*
int BOX2MAIN_VFuncMenuCloseKeyArrange( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;

  vf = &syswk->app->vfunk;

  switch( vf->seq ){
  case 0:
    {
      int r1 = BOX2MAIN_VFuncCursorMove( syswk );
      int r2 = BOX2MAIN_VFuncPokeMenuMove( syswk );
      if( r1 == 0 && r2 == 0 ){
        vf->seq++;
      }
    }
    break;

  case 1:
    {
//      int r1 = BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_POKE_BTN );
//      int r2 = BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_MV_BTN );
//      int r3 = BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_RET_BTN );
      int r1 = 0;
      int r2 = 0;
      int r3 = 0;
      if( r1 == 0 && r2 == 0 && r3 == 0 ){
        vf->seq = 0;
        return 0;
      }
    }
  }

  return 1;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R������Ŏ擾������Ԃɂ���
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
void BOX2MAIN_HandGetPokeSet( BOX2_SYS_WORK * syswk )
{
  u32 icon;
  s16 x, y;

  BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_CLOSE );
  BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, &x, &y, CLSYS_DEFREND_MAIN );

  icon = syswk->app->pokeicon_id[ BOX2OBJ_POKEICON_GET_POS ];
  BOX2OBJ_SetPos( syswk->app, icon, x, y+4, CLSYS_DEFREND_MAIN );

  BOX2OBJ_PokeIconPriChg( syswk->app, BOX2OBJ_POKEICON_GET_POS, BOX2OBJ_POKEICON_PRI_CHG_GET );
}


//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ��J�[�\���Ń|�P�����擾
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncPokeMoveGetKey( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;
  s16 x, y;

  vf = &syswk->app->vfunk;

//  BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_Y_ST_BTN );

  switch( vf->seq ){
  case 0:   // ��J�[�\�����J��
    BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_OPEN );
    vf->seq++;
  case 1:   // ��J�[�\��������
    if( vf->cnt == HANDCURSOR_MOVE_CNT ){
      vf->cnt = 0;
      vf->seq++;
      break;
    }
    BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, &x, &y, CLSYS_DEFREND_MAIN );
    BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y+HANDCURSOR_MOVE_DOT, CLSYS_DEFREND_MAIN );
    vf->cnt++;
    break;

  case 2:   // ��J�[�\������
    PMSND_PlaySE( SE_BOX2_POKE_CATCH );
    BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_CLOSE );
    vf->seq++;
  case 3:   // ��J�[�\�������
    if( vf->cnt == HANDCURSOR_MOVE_CNT ){
      vf->cnt = 0;
      vf->seq = 0;
      return 0;
    }
    BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, &x, &y, CLSYS_DEFREND_MAIN );
    BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y-HANDCURSOR_MOVE_DOT, CLSYS_DEFREND_MAIN );
    {
      u8  icon = syswk->app->pokeicon_id[ BOX2OBJ_POKEICON_GET_POS ];
      BOX2OBJ_GetPos( syswk->app, icon, &x, &y, CLSYS_DEFREND_MAIN );
      BOX2OBJ_SetPos( syswk->app, icon, x, y-HANDCURSOR_MOVE_DOT, CLSYS_DEFREND_MAIN );
    }
    BOX2OBJ_PokeCursorMove( syswk->app, BOX2OBJ_POKEICON_GET_POS );
    vf->cnt++;
  }

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �|�P���������
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncPokeMovePutKey( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;

  vf = &syswk->app->vfunk;

//  BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_Y_ST_BTN );

  switch( vf->seq ){
  case 0:
//    OS_Printf( "get = %d, put = %d\n", syswk->get_pos, syswk->app->poke_put_key );
    // �莝�����g���C�ɔz�u���邩�L�����Z���̂Ƃ��ɉE�Ƀg���C�I��������ꍇ
    if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX &&
        ( syswk->app->poke_put_key == BOX2MAIN_GETPOS_NONE ||
        syswk->app->poke_put_key < BOX2OBJ_POKEICON_TRAY_MAX ) &&
//        BOX2BGWFRM_CheckPartyPokeFrameRight(syswk->app->wfrm) == FALSE &&
        BOX2BGWFRM_CheckBoxMoveFrm(syswk->app->wfrm) == TRUE ){
      BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
      BOX2BGWFRM_BoxMoveFrmOutSet( syswk->app->wfrm );
      PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
      vf->seq = 5;
      break;
    }

    if( syswk->app->poke_put_key == BOX2MAIN_GETPOS_NONE &&
        syswk->tray != syswk->get_tray &&
        syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
      u32 dir = BOX2MAIN_GetTrayScrollDir( syswk, syswk->tray, syswk->get_tray );
      syswk->tray = syswk->get_tray;
      BOX2OBJ_PokeIconBufLoad( syswk, syswk->tray );
      BOX2MAIN_WallPaperSet( syswk, BOX2MAIN_GetWallPaperNumber(syswk,syswk->tray), dir );
      if( dir == BOX2MAIN_TRAY_SCROLL_L ){
        vf->seq = 2;
        break;
      }else{
        vf->seq = 3;
        break;
      }
    }
  case 1:
    BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_OPEN );

//    put_pos = syswk->app->poke_put_key;
/*
    if( syswk->app->poke_put_key >= BOX2UI_ARRANGE_MOVE_TRAY1 &&
      syswk->app->poke_put_key <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
      put_pos = BOX2MAIN_BOXMOVE_FLG + syswk->app->poke_put_key - BOX2UI_ARRANGE_MOVE_TRAY1;
    }
*/
//    BOX2OBJ_PutPokeIcon( syswk->app, syswk->get_pos );
//    PokeIconMoveBoxPartyDataMake( syswk, syswk->get_pos, put_pos );
    PokeIconMoveBoxPartyDataMake( syswk, syswk->get_pos, syswk->app->poke_put_key );
/*
    if( ( put_pos & BOX2MAIN_BOXMOVE_FLG ) == 0 ){
      syswk->get_pos = PokeIconMoveAfterPosGet( syswk, syswk->get_pos );
    }
*/
    vf->seq = 4;
    break;

  case 2:     // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i���j
    if( BOX2MAIN_VFuncTrayScrollLeft( syswk ) == 0 ){
      vf->seq = 1;
    }
    break;

  case 3:     // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i�E�j
    if( BOX2MAIN_VFuncTrayScrollRight( syswk ) == 0 ){
      vf->seq = 1;
    }
    break;

  case 4:     // �|�P�����A�C�R������
    if( PokeIconObjMove( syswk ) == FALSE ){
      BOX2MAIN_POKEMOVE_WORK * work = vf->work;
      PokeIconBufPosChange( syswk, work->dat );
      BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_NORMAL );
      BOX2OBJ_PokeCursorVanish( syswk, FALSE );
      PMSND_PlaySE( SE_BOX2_POKE_PUT );
      vf->seq = 0;
      return 0;
    }
    break;

  case 5:   // �g���C�I���t���[���ړ�
    if( BOX2MAIN_VFuncBoxMoveFrmOut( syswk ) == 0 ){
      BOX2BGWFRM_PartyPokeFrameInitPutRight( syswk->app->wfrm );
      BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
      PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
      vf->seq = 6;
    }
    break;

  case 6:   // �莝���t���[���ړ�
    if( BOX2MAIN_VFuncPartyFrameMove( syswk ) == 0 ){
      vf->seq = 1;
    }
    break;
  }

  return 1;
}

int BOX2MAIN_VFuncPokeMovePutKey2( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;

  vf = &syswk->app->vfunk;

  switch( vf->seq ){
  case 0:
    BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_OPEN );
    PokeIconDropDataMake( syswk, syswk->get_pos, syswk->app->poke_put_key );
    vf->seq = 1;
    break;

  case 1:
    if( PokeIconObjDrop( syswk ) == FALSE ){
      BOX2MAIN_POKEMOVE_WORK * work = vf->work;
      PokeIconBufPosChange( syswk, work->dat );
      BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_NORMAL );
      PMSND_PlaySE( SE_BOX2_POKE_PUT );
      vf->seq = 0;
      return 0;
    }
  }

  return 1;
}



//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �A�C�e���擾�i�����������E�L�[����j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncItemArrangeGetKey( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;

  vf = &syswk->app->vfunk;

  switch( vf->seq ){
  case 0:
    if( syswk->app->get_item != ITEM_DUMMY_DATA ){
      BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
      BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 1 );
      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, TRUE );
    }
    vf->seq++;

  case 1:
    {
      int   cur  = BOX2MAIN_VFuncCursorMove( syswk );
      BOOL  menu = BOX2BGWFRM_PokeMenuMoveMain( syswk->app->wfrm );
      if( cur == 0 && menu == FALSE ){
        if( syswk->app->get_item != ITEM_DUMMY_DATA ){
          BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
          BOX2OBJ_ItemIconCursorOn( syswk->app );
        }
        vf->seq = 0;
        return 0;
      }
    }
  }

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �A�C�e���擾�i�����������E�^�b�`����j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
enum {
  ITEMGET_SEQ_INIT = 0,         // �����ݒ�
  ITEMGET_SEQ_GETANM,           // �A�C�e���A�C�R���擾�A�j��
  ITEMGET_SEQ_MAIN,             // ���C��
  ITEMGET_SEQ_SCROLL_L,         // �g���C�X�N���[���i���j
  ITEMGET_SEQ_SCROLL_R,         // �g���C�X�N���[���i�E�j
  ITEMGET_SEQ_CANCEL_SCROLL_L,  // �g���C�L�����Z���X�N���[���i���j
  ITEMGET_SEQ_CANCEL_SCROLL_R,  // �g���C�L�����Z���X�N���[���i�E�j
  ITEMGET_SEQ_PUT_MOVE,         // �A�C�e���A�C�R���z�u�ړ�
  ITEMGET_SEQ_MOVE,             // �A�C�e���A�C�R���ړ�
  ITEMGET_SEQ_PUTANM,           // �k���A�j���Z�b�g
  ITEMGET_SEQ_PUTANM_WAIT,      // �k���A�j���҂�
  ITEMGET_SEQ_END,              // �I��
};

int BOX2MAIN_VFuncItemGetTouch( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;
  BOOL  menu;
  BOOL  party;
  u32 tpx, tpy;

  vf = &syswk->app->vfunk;

  menu  = BOX2BGWFRM_PokeMenuMoveMain( syswk->app->wfrm );
  party = BOX2BGWFRM_PartyPokeFrameMove( syswk );

  switch( vf->seq ){
  case ITEMGET_SEQ_INIT:              // �����ݒ�
    BOX2SEQ_ItemModeMenuSet( syswk, syswk->app->get_item );
    if( syswk->app->get_item != ITEM_DUMMY_DATA ){
      BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
      BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 1 );
      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, TRUE );
      vf->seq = ITEMGET_SEQ_GETANM;
    }else{
      BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
      vf->seq = ITEMGET_SEQ_END;
    }
    break;

  case ITEMGET_SEQ_GETANM:            // �A�C�e���A�C�R���擾�A�j��
    if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
      break;
    }
    BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
    BOX2OBJ_ItemIconCursorOn( syswk->app );
    vf->seq = ITEMGET_SEQ_MAIN;

  case ITEMGET_SEQ_MAIN:              // ���C��
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == FALSE ){
      u16 put_pos;
      u16 set_pos;
      BOOL  cancel;
      BOOL  mv_mode;

      if( party == TRUE || BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == TRUE ){
        mv_mode = TRUE;
      }else{
        mv_mode = FALSE;
      }

      put_pos = TrayPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy );
      if( party == FALSE && BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == TRUE ){
        if( put_pos == BOX2MAIN_GETPOS_NONE ){
          put_pos = PartyPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy, PartyPokeAreaRight );
        }
      }
      set_pos = put_pos;
      cancel  = FALSE;
      if( put_pos == BOX2MAIN_GETPOS_NONE ){
        put_pos = syswk->get_pos;
        cancel  = TRUE;
      }else if( BOX2MAIN_PokeItemMoveCheck( syswk, syswk->get_pos, put_pos ) == FALSE ){
        put_pos = syswk->get_pos;
        cancel  = TRUE;
      }

      if( mv_mode == TRUE ){
        ItemIconMoveMake( syswk, set_pos, put_pos, BOX2MAIN_POKEMOVE_MODE_ALL );
        // �L�����Z�����̃X�N���[����
        if( cancel == TRUE && syswk->get_tray != BOX2MAIN_GETPOS_NONE && syswk->get_tray != syswk->tray ){
          u32 dir = BOX2MAIN_GetTrayScrollDir( syswk, syswk->tray, syswk->get_tray );
          syswk->tray = syswk->get_tray;
          BOX2OBJ_PokeIconBufLoad( syswk, syswk->tray );
          BOX2MAIN_WallPaperSet( syswk, BOX2MAIN_GetWallPaperNumber(syswk,syswk->tray), dir );
          if( dir == BOX2MAIN_TRAY_SCROLL_L ){
            vf->seq = ITEMGET_SEQ_CANCEL_SCROLL_L;
          }else{
            vf->seq = ITEMGET_SEQ_CANCEL_SCROLL_R;
          }
        }else{
          vf->seq = ITEMGET_SEQ_PUT_MOVE;
        }
      }else{
        ItemIconMoveMake( syswk, set_pos, put_pos, BOX2MAIN_POKEMOVE_MODE_TRAY );
        BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
        vf->seq = ITEMGET_SEQ_MOVE;
      }
      vf->cnt = 0;

    }else{
      if( vf->cnt != 0 ){
        vf->cnt--;
      }
      // �莝���t���[���\���`�F�b�N
      if( menu == FALSE && party == FALSE &&
          BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == FALSE ){
        if( BOX2UI_HitCheckPosTrayPoke( tpx, tpy ) != syswk->get_pos ){
          BOX2BGWFRM_PartyPokeFrameInitPutRight( syswk->app->wfrm );
          BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
          BOX2OBJ_PartyPokeIconFrmInSet( syswk );
          BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
          PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
          // �߂��̃V�[�P���X���u���ǂ�����v�̏����ɂ���
          BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY_MOVE, syswk->get_pos );
          CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_PTGT_BOXLIST );
          CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_PTGT_STATUS );
          BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_OFF );
          syswk->pokechg_mode = 0;
          syswk->app->msg_put = 0;
          syswk->app->old_cur_pos = syswk->get_pos;
          syswk->app->vnext_seq = BOX2SEQ_MAINSEQ_ITEM_BOXPARTY_ICONMOVE_POKEADD;
        }
      }
      // �g���C�X�N���[�����q�b�g�`�F�b�N
      if( vf->cnt == 0 ){
        u32 hit = BOX2UI_HitCheckContTrayArrow();
        if( hit == 0 ){
          PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
          BOX2MAIN_InitTrayScroll( syswk, BOX2MAIN_TRAY_SCROLL_L );
          vf->seq = ITEMGET_SEQ_SCROLL_L;
        }else if( hit == 1 ){
          PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
          BOX2MAIN_InitTrayScroll( syswk, BOX2MAIN_TRAY_SCROLL_R );
          vf->seq = ITEMGET_SEQ_SCROLL_R;
        }
      }
      MoveGetItemIcon( syswk->app, tpx, tpy );
    }
    break;

  case ITEMGET_SEQ_SCROLL_L:          // �g���C�X�N���[���i���j
    if( BOX2MAIN_VFuncTrayScrollLeft( syswk ) == 0 ){
//      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = ITEMGET_SEQ_MAIN;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetItemIcon( syswk->app, tpx, tpy );
    }
    break;

  case ITEMGET_SEQ_SCROLL_R:          // �g���C�X�N���[���i�E�j
    if( BOX2MAIN_VFuncTrayScrollRight( syswk ) == 0 ){
//      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = ITEMGET_SEQ_MAIN;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetItemIcon( syswk->app, tpx, tpy );
    }
    break;

  case ITEMGET_SEQ_CANCEL_SCROLL_L:   // �g���C�L�����Z���X�N���[���i���j
    if( BOX2MAIN_VFuncTrayScrollLeft( syswk ) == 0 ){
      vf->seq = ITEMGET_SEQ_PUT_MOVE;
    }
    break;

  case ITEMGET_SEQ_CANCEL_SCROLL_R:   // �g���C�L�����Z���X�N���[���i�E�j
    if( BOX2MAIN_VFuncTrayScrollRight( syswk ) == 0 ){
      vf->seq = ITEMGET_SEQ_PUT_MOVE;
    }
    break;

  case ITEMGET_SEQ_PUT_MOVE:          // �A�C�e���A�C�R���z�u�ړ�
    if( ItemIconMoveMain( syswk ) == FALSE ){
      PMSND_PlaySE( SE_BOX2_POKE_PUT );
      vf->seq = ITEMGET_SEQ_PUTANM;
    }
    break;

  case ITEMGET_SEQ_MOVE:              // �A�C�e���A�C�R���ړ�
    if( ItemIconMoveMain( syswk ) == FALSE ){
      PMSND_PlaySE( SE_BOX2_POKE_PUT );
      if( syswk->app->get_item != ITEM_DUMMY_DATA ){
        BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
        BOX2OBJ_ItemIconCursorOn( syswk->app );
      }
      BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF );
      vf->seq = ITEMGET_SEQ_END;
    }
    break;

  case ITEMGET_SEQ_PUTANM:            // �k���A�j���Z�b�g
    BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
    BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );
    BOX2OBJ_PokeCursorVanish( syswk, FALSE );
    vf->seq = ITEMGET_SEQ_PUTANM_WAIT;
    break;

  case ITEMGET_SEQ_PUTANM_WAIT:       // �k���A�j���҂�
    if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == FALSE ){
      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
      BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
      vf->seq = ITEMGET_SEQ_END;
    }
    break;

  case ITEMGET_SEQ_END:               // �I��
    if( menu == FALSE && party == FALSE ){
      vf->seq = 0;
      return 0;
    }
  }

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ���j���[�������Ė߂���o���i�����������j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncItemArrangeMenuClose( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;

  vf = &syswk->app->vfunk;

  switch( vf->seq ){
  case 0:
//    if( syswk->app->get_item != ITEM_DUMMY_DATA ){
    if( BOX2OBJ_VanishCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
      BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
      BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );
    }
    vf->seq++;

  case 1:
    {
      int r1 = BOX2MAIN_VFuncCursorMove( syswk );
      int r2 = BOX2MAIN_VFuncPokeMenuMove( syswk );
      if( r1 == 0 && r2 == 0 ){
        vf->seq++;
      }
    }
    break;

  case 2:
//    if( syswk->app->get_item != ITEM_DUMMY_DATA ){
    if( BOX2OBJ_VanishCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
      BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
    }
    vf->seq = 0;
    return 0;
  }

  return 1;
}

int BOX2MAIN_VFuncItemGetMenuClose( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;

  vf = &syswk->app->vfunk;

  switch( vf->seq ){
  case 0:
//    if( syswk->app->get_item != ITEM_DUMMY_DATA ){
    if( BOX2OBJ_VanishCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
      BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
      BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );
    }
    vf->seq++;

  case 1:
    if( BOX2MAIN_VFuncPokeMenuMove( syswk ) == 0 ){
      vf->seq++;
    }
    break;

  case 2:
//    if( syswk->app->get_item != ITEM_DUMMY_DATA ){
    if( BOX2OBJ_VanishCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
      BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
    }
    vf->seq = 0;
    return 0;
  }

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �A�C�e���A�C�R������ւ�����
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
/*
int BOX2MAIN_VFuncItemIconChange( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;

  vf = &syswk->app->vfunk;

  switch( vf->seq ){
  case 0:
    if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
      break;
    }
    {
      BOX2MAIN_ITEMMOVE_WORK * mvwk;
      u16 put_pos;
      u16 set_pos;

      mvwk = vf->work;
      put_pos = syswk->get_pos;
      set_pos = syswk->get_pos;
      syswk->get_pos = mvwk->put_pos;

      ItemIconMoveMake( syswk, set_pos, put_pos, BOX2MAIN_POKEMOVE_MODE_ALL );
    }
    vf->seq = 1;
    break;

  case 1:
    if( ItemIconMoveMain( syswk ) == FALSE ){
      vf->seq = 0;
      return 0;
    }
    break;
  }

  return 1;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �����������E�g���C/�莝���擾�i�^�b�`�j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
enum {
  ITEMMOVE_SEQ_ICONANM_SET = 0,   // �g��A�j���Z�b�g
  ITEMMOVE_SEQ_ICONANM_WAIT,        // �g��A�j���҂�
  ITEMMOVE_SEQ_ICON_GET,            // �A�C�R���擾��
  ITEMMOVE_SEQ_SCROLL_L,            // �g���C�X�N���[���i���j
  ITEMMOVE_SEQ_SCROLL_R,            // �g���C�X�N���[���i�E�j
  ITEMMOVE_SEQ_CANCEL_SCROLL_L,   // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i���j
  ITEMMOVE_SEQ_CANCEL_SCROLL_R,   // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i�E�j
  ITEMMOVE_SEQ_ICON_PUT,            // �A�C�R���z�u
  ITEMMOVE_SEQ_ICONANM_END_SET,   // �k���A�j���Z�b�g
  ITEMMOVE_SEQ_ICONANM_END_WAIT,    // �k���A�j���҂�
  ITEMMOVE_SEQ_END,               // �I��
};

static void MoveGetItemIcon( BOX2_APP_WORK * appwk, u32 x, u32 y )
{
  BOX2OBJ_ItemIconPosSet( appwk, x, y );
  BOX2OBJ_ItemIconCursorMove( appwk );
  appwk->tpx = x;
  appwk->tpy = y;
}

int BOX2MAIN_VFuncItemMoveTouch( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;
  u32 tpx, tpy;

  vf = &syswk->app->vfunk;

  switch( vf->seq ){
  case ITEMMOVE_SEQ_ICONANM_SET:        // �g��A�j���Z�b�g
    BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
    BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 1 );
    BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, TRUE );
    vf->seq = ITEMMOVE_SEQ_ICONANM_WAIT;
    break;

  case ITEMMOVE_SEQ_ICONANM_WAIT:     // �g��A�j���҂�
    if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == FALSE ){
      BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
      BOX2OBJ_ItemIconCursorOn( syswk->app );
      vf->seq = ITEMMOVE_SEQ_ICON_GET;
    }
    break;

  case ITEMMOVE_SEQ_ICON_GET:         // �A�C�R���擾��
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == FALSE ){
      u16 put_pos;
      u16 set_pos;
      BOOL  cancel;

      put_pos = TrayPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy );
      if( put_pos == BOX2MAIN_GETPOS_NONE ){
        put_pos = PartyPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy, PartyPokeAreaRight );
      }
      set_pos = put_pos;
      cancel  = FALSE;
      if( put_pos == BOX2MAIN_GETPOS_NONE ){
        put_pos = syswk->get_pos;
        cancel  = TRUE;
      }else if( BOX2MAIN_PokeItemMoveCheck( syswk, syswk->get_pos, put_pos ) == FALSE ){
        put_pos = syswk->get_pos;
        cancel  = TRUE;
      }
      ItemIconMoveMake( syswk, set_pos, put_pos, BOX2MAIN_POKEMOVE_MODE_ALL );

      // �L�����Z�����̃X�N���[����
      if( cancel == TRUE && syswk->get_tray != BOX2MAIN_GETPOS_NONE && syswk->get_tray != syswk->tray ){
        u32 dir = BOX2MAIN_GetTrayScrollDir( syswk, syswk->tray, syswk->get_tray );
        syswk->tray = syswk->get_tray;
        BOX2OBJ_PokeIconBufLoad( syswk, syswk->tray );
        BOX2MAIN_WallPaperSet( syswk, BOX2MAIN_GetWallPaperNumber(syswk,syswk->tray), dir );
        if( dir == BOX2MAIN_TRAY_SCROLL_L ){
          vf->seq = ITEMMOVE_SEQ_CANCEL_SCROLL_L;
        }else{
          vf->seq = ITEMMOVE_SEQ_CANCEL_SCROLL_R;
        }
      }else{
        vf->seq = ITEMMOVE_SEQ_ICON_PUT;
      }
      vf->cnt = 0;
    }else{
      if( vf->cnt != 0 ){
        vf->cnt--;
      }
/*
      // �{�b�N�X�ړ��t���[���\���`�F�b�N
      if( ret1 == 0 && ret2 == 0 &&
          BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == FALSE &&
          BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == FALSE ){
        if( BOX2UI_HitCheckPosTrayPoke( tpx, tpy ) != syswk->get_pos ){
          syswk->box_mv_pos = syswk->tray;  // ���ݎQ�Ƃ��Ă���{�b�N�X
          BOX2MAIN_InitBoxMoveFrameScroll( syswk );
          // �߂��̃V�[�P���X���u���ށv�̏����ɂ���
          BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, syswk->get_pos );
          BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
          BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_PASSIVE );
          syswk->app->old_cur_pos = syswk->get_pos;
          syswk->app->vnext_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_DATA_CHANGE;
        }
      }
*/
      // �g���C�X�N���[�����q�b�g�`�F�b�N
      if( vf->cnt == 0 ){
        u32 hit = BOX2UI_HitCheckContTrayArrow();
        if( hit == 0 ){
          PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
          BOX2MAIN_InitTrayScroll( syswk, BOX2MAIN_TRAY_SCROLL_L );
          vf->seq = ITEMMOVE_SEQ_SCROLL_L;
        }else if( hit == 1 ){
          PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
          BOX2MAIN_InitTrayScroll( syswk, BOX2MAIN_TRAY_SCROLL_R );
          vf->seq = ITEMMOVE_SEQ_SCROLL_R;
        }
      }
      MoveGetItemIcon( syswk->app, tpx, tpy );
    }
    break;

  case ITEMMOVE_SEQ_SCROLL_L:         // �g���C�X�N���[���i���j
    if( BOX2MAIN_VFuncTrayScrollLeft( syswk ) == 0 ){
//      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = ITEMMOVE_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetItemIcon( syswk->app, tpx, tpy );
    }
    break;

  case ITEMMOVE_SEQ_SCROLL_R:         // �g���C�X�N���[���i�E�j
    if( BOX2MAIN_VFuncTrayScrollRight( syswk ) == 0 ){
//      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = ITEMMOVE_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetItemIcon( syswk->app, tpx, tpy );
    }
    break;

  case ITEMMOVE_SEQ_CANCEL_SCROLL_L:    // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i���j
  case ITEMMOVE_SEQ_CANCEL_SCROLL_R:    // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i�E�j
    if( vf->seq == ITEMMOVE_SEQ_CANCEL_SCROLL_L ){
      if( BOX2MAIN_VFuncTrayScrollLeft( syswk ) == 0 ){
        vf->seq = ITEMMOVE_SEQ_ICON_PUT;
      }
    }
    if( vf->seq == ITEMMOVE_SEQ_CANCEL_SCROLL_R ){
      if( BOX2MAIN_VFuncTrayScrollRight( syswk ) == 0 ){
        vf->seq = ITEMMOVE_SEQ_ICON_PUT;
      }
    }
  case ITEMMOVE_SEQ_ICON_PUT:         // �A�C�R���z�u
    if( ItemIconMoveMain( syswk ) == FALSE ){
      if( vf->seq == ITEMMOVE_SEQ_ICON_PUT ){
        PMSND_PlaySE( SE_BOX2_POKE_PUT );
        vf->seq = ITEMMOVE_SEQ_ICONANM_END_SET;
      }
    }
    break;

  case ITEMMOVE_SEQ_ICONANM_END_SET:    // �k���A�j���Z�b�g
    BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
    BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );
    BOX2OBJ_PokeCursorVanish( syswk, FALSE );
    vf->seq = ITEMMOVE_SEQ_ICONANM_END_WAIT;
    break;

  case ITEMMOVE_SEQ_ICONANM_END_WAIT: // �k���A�j���҂�
    if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == FALSE ){
      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
      BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
      vf->seq = ITEMMOVE_SEQ_END;
    }
    break;

  case ITEMMOVE_SEQ_END:                // �I��
    vf->seq = 0;
    return 0;
  }

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �����������E�g���C/�莝������ւ��i�^�b�`�j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncItemIconChgTouch( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;

  vf = &syswk->app->vfunk;

  switch( vf->seq ){
  case 0:
    BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
    BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 1 );
    BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, TRUE );
    vf->seq = 1;
    break;

  case 1:
    if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == FALSE ){
      BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
      vf->seq = 2;
    }
    break;

  case 2:
    {
      BOX2MAIN_ITEMMOVE_WORK * mvwk;
      u16 put_pos;
      u16 set_pos;

      mvwk = vf->work;
      put_pos = syswk->get_pos;
      set_pos = syswk->get_pos;
      syswk->get_pos = mvwk->put_pos;

      ItemIconMoveMake( syswk, set_pos, put_pos, BOX2MAIN_POKEMOVE_MODE_ALL );
    }
    vf->seq = 3;
    break;

  case 3:
    if( ItemIconMoveMain( syswk ) == FALSE ){
      PMSND_PlaySE( SE_BOX2_POKE_PUT );
      vf->seq = 4;
    }
    break;

  case 4:
    BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
    BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );
    vf->seq = 5;
    break;

  case 5:
    if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == FALSE ){
      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
      BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
      vf->seq = 6;
    }
    break;

  case 6:
    vf->seq = 0;
    return 0;
  }

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ��J�[�\���ŃA�C�e���擾
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncItemArrangeBoxPartyIconGetKey( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;
  s16 x, y;

  vf = &syswk->app->vfunk;

//  BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_Y_ST_BTN );

  switch( vf->seq ){
  case 0:   // �A�C�e���A�C�R���\��
    BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
    BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 1 );
    BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, TRUE );
    vf->seq++;
    break;

  case 1:
    if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == FALSE ){
      BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
      vf->seq++;
    }
    break;

  case 2:   // ��J�[�\�����J��
    BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_OPEN );
    vf->seq++;
  case 3:   // ��J�[�\��������
    if( vf->cnt == HANDCURSOR_MOVE_CNT ){
      vf->cnt = 0;
      vf->seq++;
      break;
    }
    BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, &x, &y, CLSYS_DEFREND_MAIN );
    BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y+HANDCURSOR_MOVE_DOT, CLSYS_DEFREND_MAIN );
    vf->cnt++;
    break;

  case 4:   // ��J�[�\������
    PMSND_PlaySE( SE_BOX2_POKE_CATCH );
    BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_CLOSE );
    vf->seq++;
  case 5:   // ��J�[�\�������
    if( vf->cnt == HANDCURSOR_MOVE_CNT ){
      BOX2OBJ_ItemIconCursorAdd( syswk->app );
      BOX2OBJ_ItemIconCursorOn( syswk->app );
      vf->cnt = 0;
      vf->seq = 0;
      return 0;
    }
    BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, &x, &y, CLSYS_DEFREND_MAIN );
    BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y-HANDCURSOR_MOVE_DOT, CLSYS_DEFREND_MAIN );
    {
      BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_ITEMICON, &x, &y, CLSYS_DEFREND_MAIN );
      BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_ITEMICON, x, y-HANDCURSOR_MOVE_DOT, CLSYS_DEFREND_MAIN );
    }
    vf->cnt++;
  }

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �A�C�e�������
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncItemIconPutKey( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;
  s16 x, y;

  vf = &syswk->app->vfunk;

  switch( vf->seq ){
  case 0:
    BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_OPEN );
    vf->seq = 1;

  case 1:
    if( vf->cnt == HANDCURSOR_MOVE_CNT ){
      vf->cnt = 0;
      vf->seq = 2;
    }else{
      BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_ITEMICON, &x, &y, CLSYS_DEFREND_MAIN );
      BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_ITEMICON, x, y+HANDCURSOR_MOVE_DOT, CLSYS_DEFREND_MAIN );
      vf->cnt++;
    }
    break;

  case 2:
    PMSND_PlaySE( SE_BOX2_POKE_PUT );
    BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_NORMAL );
    BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
    BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );
    vf->seq = 3;
    break;

  case 3:
    if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == FALSE ){
      BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
      vf->seq = 4;
    }
    break;

  case 4:
    BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_NORMAL );
    vf->seq = 0;
    return 0;
  }

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �A�C�e��������E�L�����Z��
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncItemIconPutKeyCancel( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;

  vf = &syswk->app->vfunk;

  switch( vf->seq ){
  case 0:
    if( syswk->get_tray != BOX2MAIN_GETPOS_NONE && syswk->get_tray != syswk->tray ){
      u32 dir = BOX2MAIN_GetTrayScrollDir( syswk, syswk->tray, syswk->get_tray );
      syswk->tray = syswk->get_tray;
      BOX2OBJ_PokeIconBufLoad( syswk, syswk->tray );
      BOX2MAIN_WallPaperSet( syswk, BOX2MAIN_GetWallPaperNumber(syswk,syswk->tray), dir );
      if( dir == BOX2MAIN_TRAY_SCROLL_L ){
        vf->seq = 2;
        break;
      }else{
        vf->seq = 3;
        break;
      }
    }

  case 1:
    BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_OPEN );
    ItemIconMoveMakeHand( syswk, syswk->app->get_item_init_pos, BOX2MAIN_POKEMOVE_MODE_ALL );
    vf->seq = 4;
    break;

  case 2:     // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i���j
    if( BOX2MAIN_VFuncTrayScrollLeft( syswk ) == 0 ){
      vf->seq = 1;
    }
    break;

  case 3:     // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i�E�j
    if( BOX2MAIN_VFuncTrayScrollRight( syswk ) == 0 ){
      vf->seq = 1;
    }
    break;

  case 4:
    if( ItemIconMoveMainHand( syswk ) == FALSE ){
      PMSND_PlaySE( SE_BOX2_POKE_PUT );
      vf->seq = 5;
    }
    break;

  case 5:
    BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
    BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );
    vf->seq = 6;
    break;

  case 6:
    if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == FALSE ){
      BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
      vf->seq = 7;
    }
    break;

  case 7:
    BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_NORMAL );
    vf->seq = 0;
    return 0;
  }

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �莝���|�P�����̃A�C�e���擾�i�����������E�^�b�`����j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncItemPartyGetTouch( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;
  BOOL  menu;
  BOOL  party;
  u32 tpx, tpy;

  vf = &syswk->app->vfunk;

  menu  = BOX2BGWFRM_PokeMenuMoveMain( syswk->app->wfrm );
  party = BOX2BGWFRM_PartyPokeFrameMove( syswk );

  switch( vf->seq ){
  case ITEMGET_SEQ_INIT:            // �����ݒ�
    BOX2SEQ_ItemModeMenuSet( syswk, syswk->app->get_item );
    if( syswk->app->get_item != ITEM_DUMMY_DATA ){
      BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
      BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 1 );
      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, TRUE );
      vf->seq = ITEMGET_SEQ_GETANM;
    }else{
      BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
      vf->seq = ITEMGET_SEQ_END;
    }
    break;

  case ITEMGET_SEQ_GETANM:          // �A�C�e���A�C�R���擾�A�j��
    if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
      break;
    }
    BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
    BOX2OBJ_ItemIconCursorOn( syswk->app );
    vf->seq = ITEMGET_SEQ_MAIN;

  case ITEMGET_SEQ_MAIN:            // ���C��
    if( BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == TRUE ){
      BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
    }

    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == FALSE ){
      u16 put_pos;
      u16 set_pos;
      BOOL  cancel;
      BOOL  mv_mode;

      if( party == TRUE || BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == TRUE ){
        mv_mode = TRUE;
      }else{
        mv_mode = FALSE;
      }

      if( party == FALSE && BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == TRUE ){
        put_pos = TrayPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy );
        if( put_pos == BOX2MAIN_GETPOS_NONE ){
          put_pos = PartyPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy, PartyPokeAreaRight );
        }
      }else{
        put_pos = PartyPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy, PartyPokeAreaLeft );
      }
      set_pos = put_pos;
      cancel  = FALSE;
      if( put_pos == BOX2MAIN_GETPOS_NONE ){
        put_pos = syswk->get_pos;
        cancel  = TRUE;
      }else if( BOX2MAIN_PokeItemMoveCheck( syswk, syswk->get_pos, put_pos ) == FALSE ){
        put_pos = syswk->get_pos;
        cancel  = TRUE;
      }

      if( mv_mode == TRUE ){
        ItemIconMoveMake( syswk, set_pos, put_pos, BOX2MAIN_POKEMOVE_MODE_ALL );
        vf->seq = ITEMGET_SEQ_PUT_MOVE;
      }else{
        ItemIconMoveMake( syswk, set_pos, put_pos, BOX2MAIN_POKEMOVE_MODE_PARTY );
        if( put_pos == syswk->get_pos &&
            ( ITEM_CheckMail(syswk->app->get_item) == FALSE || set_pos == syswk->get_pos ) ){
          BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
        }
        vf->seq = ITEMGET_SEQ_MOVE;
      }
      vf->cnt = 0;

/*
      u16 put_pos = BOX2MAIN_GETPOS_NONE;
      u16 set_pos;

      if( syswk->app->party_frm == 2 ){
        put_pos = TrayPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy );
      }else{
        put_pos = PartyPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy, PartyPokeAreaLeft );
      }

      set_pos = put_pos;

      if( put_pos == BOX2MAIN_GETPOS_NONE ){
        put_pos = syswk->get_pos;
      }else if( BOX2MAIN_PokeItemMoveCheck( syswk, syswk->get_pos, put_pos ) == FALSE ){
        put_pos = syswk->get_pos;
      }
      if( put_pos == syswk->get_pos ){
        BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
        if( syswk->app->party_frm != 0 ){
          BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
          BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
        }
      }else{
        BOX2BGWFRM_SubDispItemFrmOutSet( syswk->app->wfrm );
      }

      ItemIconMoveMake( syswk, set_pos, put_pos, BOX2MAIN_POKEMOVE_MODE_PARTY );
      vf->seq = 3;
      break;
*/
    }else{
      if( vf->cnt != 0 ){
        vf->cnt--;
      }
      // �莝���t���[���\���`�F�b�N
      if( menu == FALSE && party == FALSE &&
          BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == FALSE ){
        if( BOX2UI_HitCheckPartyFrameLeft() == FALSE ){
          BOX2BGWFRM_PartyPokeFrameRightMoveSet( syswk->app->wfrm );
          BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
          PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
          // �߂��̃V�[�P���X���u���ǂ�����v�̏����ɂ���
          BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY_MOVE, syswk->get_pos );
          CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_PTGT_BOXLIST );
          CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_PTGT_STATUS );
          BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
          BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_OFF );
          syswk->pokechg_mode = 1;
//          syswk->app->msg_put = 0;
          syswk->app->old_cur_pos = syswk->get_pos;
          syswk->app->vnext_seq = BOX2SEQ_MAINSEQ_ITEM_BOXPARTY_ICONMOVE_POKEADD;
/*
      if( menu_mv == FALSE && syswk->app->party_frm == 0 ){
        if( tpx <  ( BOX2BGWFRM_PARTYPOKE_LX * 8 ) ||
          tpy <  ( BOX2BGWFRM_PARTYPOKE_PY * 8 ) ||
          tpx >= ( ( BOX2BGWFRM_PARTYPOKE_LX + BOX2BGWFRM_PARTYPOKE_SX ) * 8 ) ){
          syswk->app->party_frm = 1;
          BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );
        }
      }
*/
        }
      }
      // �g���C�X�N���[�����q�b�g�`�F�b�N
      if( vf->cnt == 0 ){
        u32 hit = BOX2UI_HitCheckContTrayArrow();
        if( hit == 0 ){
          PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
          BOX2MAIN_InitTrayScroll( syswk, BOX2MAIN_TRAY_SCROLL_L );
          vf->seq = ITEMGET_SEQ_SCROLL_L;
        }else if( hit == 1 ){
          PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
          BOX2MAIN_InitTrayScroll( syswk, BOX2MAIN_TRAY_SCROLL_R );
          vf->seq = ITEMGET_SEQ_SCROLL_R;
        }
      }
      MoveGetItemIcon( syswk->app, tpx, tpy );
    }
    break;

  case ITEMGET_SEQ_SCROLL_L:          // �g���C�X�N���[���i���j
    if( BOX2MAIN_VFuncTrayScrollLeft( syswk ) == 0 ){
//      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = ITEMGET_SEQ_MAIN;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetItemIcon( syswk->app, tpx, tpy );
    }
    break;

  case ITEMGET_SEQ_SCROLL_R:          // �g���C�X�N���[���i�E�j
    if( BOX2MAIN_VFuncTrayScrollRight( syswk ) == 0 ){
//      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = ITEMGET_SEQ_MAIN;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetItemIcon( syswk->app, tpx, tpy );
    }
    break;

  case ITEMGET_SEQ_PUT_MOVE:        // �A�C�e���A�C�R���z�u�ړ�
  case ITEMGET_SEQ_MOVE:            // �A�C�e���A�C�R���ړ�
    if( ItemIconMoveMain( syswk ) == FALSE ){
      PMSND_PlaySE( SE_BOX2_POKE_PUT );
      if( vf->seq == ITEMGET_SEQ_PUT_MOVE ){
        vf->seq = ITEMGET_SEQ_PUTANM;
      }else if( vf->seq == ITEMGET_SEQ_MOVE ){
        if( syswk->app->get_item != ITEM_DUMMY_DATA ){
          BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
          BOX2OBJ_ItemIconCursorOn( syswk->app );
        }
        BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF );
        vf->seq = ITEMGET_SEQ_END;
      }
    }
    break;

  case ITEMGET_SEQ_PUTANM:            // �k���A�j���Z�b�g
    BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
    BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );
    BOX2OBJ_PokeCursorVanish( syswk, FALSE );
    vf->seq = ITEMGET_SEQ_PUTANM_WAIT;
    break;

  case ITEMGET_SEQ_PUTANM_WAIT:       // �k���A�j���҂�
    if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == FALSE ){
      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
      BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
      vf->seq = ITEMGET_SEQ_END;
    }
    break;

  case ITEMGET_SEQ_END:             // �I��
    if( menu == FALSE && party == FALSE ){
      vf->seq = 0;
      return 0;
    }
  }

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �A�C�e���A�C�R������ւ�����i�莝���|�P�����j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncItemIconPartyChange( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;
//  BOOL  party_mv;

  vf = &syswk->app->vfunk;

//  party_mv = BOX2BGWFRM_PartyPokeFrameMove( syswk );

  switch( vf->seq ){
  case 0:
    if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == FALSE ){
      BOX2MAIN_ITEMMOVE_WORK * mvwk;
      u16 put_pos;
      u16 set_pos;

      mvwk = vf->work;
      put_pos = syswk->get_pos;
      set_pos = syswk->get_pos;
      syswk->get_pos = mvwk->put_pos;

      ItemIconMoveMake( syswk, set_pos, put_pos, BOX2MAIN_POKEMOVE_MODE_PARTY );
/*
      if( syswk->app->party_frm != 0 ){
        syswk->app->party_frm = 0;
        BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
        BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
      }
*/
      vf->seq = 1;
    }
    break;

  case 1:
//    if( ItemIconMoveMain( syswk ) == FALSE && party_mv == FALSE ){
    if( ItemIconMoveMain( syswk ) == FALSE ){
      vf->seq = 0;
      return 0;
    }
    break;
  }

  return 1;
}


//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �g���C�A�C�R���X�N���[���i��j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//-------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncTrayIconScrollUp( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;

  vf = &syswk->app->vfunk;

  if( vf->cnt == BOX2MAIN_TRAYICON_SCROLL_CNT ){
    u8 pos = CURSORMOVE_PosGet( syswk->app->cmwk );
    if( pos >= BOX2UI_ARRANGE_PGT_TRAY2 && pos <= BOX2UI_ARRANGE_PGT_TRAY5 ){
      BOX2OBJ_ChangeTrayName( syswk, pos-BOX2UI_ARRANGE_PGT_TRAY2, TRUE );
    }else{
      BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
    }
    BOX2OBJ_EndTrayCursorScroll( syswk );
    vf->cnt = 0;
    return 0;
  }
  vf->cnt++;
  BOX2OBJ_TrayIconScroll( syswk, -vf->cnt );

  return 1;
}

int BOX2MAIN_VFuncTrayIconScrollDown( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;

  vf = &syswk->app->vfunk;

  if( vf->cnt == BOX2MAIN_TRAYICON_SCROLL_CNT ){
    u8 pos = CURSORMOVE_PosGet( syswk->app->cmwk );
    if( pos >= BOX2UI_ARRANGE_PGT_TRAY2 && pos <= BOX2UI_ARRANGE_PGT_TRAY5 ){
      BOX2OBJ_ChangeTrayName( syswk, pos-BOX2UI_ARRANGE_PGT_TRAY2, TRUE );
    }else{
      BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
    }
    BOX2OBJ_EndTrayCursorScroll( syswk );
    vf->cnt = 0;
    return 0;
  }
  vf->cnt++;
  BOX2OBJ_TrayIconScroll( syswk, vf->cnt );

  return 1;
}

int BOX2MAIN_VFuncTrayIconScrollUpJump( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;

  vf = &syswk->app->vfunk;

  if( vf->cnt == BOX2MAIN_TRAYICON_SCROLL_CNT ){
    u8 pos = CURSORMOVE_PosGet( syswk->app->cmwk );
    if( pos >= BOX2UI_BOXJUMP_TRAY2 && pos <= BOX2UI_BOXJUMP_TRAY5 ){
      BOX2OBJ_ChangeTrayName( syswk, pos-BOX2UI_BOXJUMP_TRAY2, TRUE );
    }else{
      BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
    }
    BOX2OBJ_EndTrayCursorScroll( syswk );
    vf->cnt = 0;
    return 0;
  }
  vf->cnt++;
  BOX2OBJ_TrayIconScroll( syswk, -vf->cnt );

  return 1;
}

int BOX2MAIN_VFuncTrayIconScrollDownJump( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;

  vf = &syswk->app->vfunk;

  if( vf->cnt == BOX2MAIN_TRAYICON_SCROLL_CNT ){
    u8 pos = CURSORMOVE_PosGet( syswk->app->cmwk );
    if( pos >= BOX2UI_BOXJUMP_TRAY2 && pos <= BOX2UI_BOXJUMP_TRAY5 ){
      BOX2OBJ_ChangeTrayName( syswk, pos-BOX2UI_BOXJUMP_TRAY2, TRUE );
    }else{
      BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
    }
    BOX2OBJ_EndTrayCursorScroll( syswk );
    vf->cnt = 0;
    return 0;
  }
  vf->cnt++;
  BOX2OBJ_TrayIconScroll( syswk, vf->cnt );

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �擾�|�P�����ړ��i�o�g���{�b�N�X�E���C������j
 *
 * @param syswk �{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "1 = ������"
 * @retval  "0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2MAIN_VFuncGetPokeMoveBattleBoxMain( BOX2_SYS_WORK * syswk )
{
  BOX2_IRQWK * vf;
  u32 tpx, tpy;
  u32 put_pos;
  u32 ret1, ret2;
  u32 hit;

  vf = &syswk->app->vfunk;

  if( BGWINFRM_MoveCheck( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY ) == 1 ){
    ret1 = BOX2MAIN_VFuncPartyFrameMove( syswk );
  }else{
    ret1 = 0;
  }
  ret2 = BOX2BGWFRM_PokeMenuMoveMain( syswk->app->wfrm );

  switch( vf->seq ){
  case SEIRI_SEQ_MENU_OUT:    // ���j���[�A�E�g
    if( GFL_UI_TP_GetCont() == TRUE ){
      BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
    }
    vf->seq = SEIRI_SEQ_ICON_GET;
    break;

  case SEIRI_SEQ_ICON_GET:    // �A�C�R���擾��

    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == FALSE ){

      put_pos = BOX2MAIN_GETPOS_NONE;

      // �莝���|�P����
      if( BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == TRUE ){
        put_pos = PartyPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy, PartyPokeAreaRight );
      }
      // �Q�ƒ��̃g���C
      if( put_pos == BOX2MAIN_GETPOS_NONE ){
        put_pos = TrayPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy );
      }
      // �o�g���{�b�N�X�����b�N����Ă���Ƃ��͈ړ������Ȃ�
      if( syswk->dat->callMode == BOX_MODE_BATTLE && syswk->dat->bbRockFlg == TRUE ){
        put_pos = BOX2MAIN_GETPOS_NONE;
        syswk->app->mv_err_code = BOX2MAIN_ERR_CODE_ROCK;
      }
/*
      if( put_pos == BOX2MAIN_GETPOS_NONE ){
        put_pos = PartyPokePutAreaCheck( syswk->app->tpx, syswk->app->tpy, PartyPokeAreaRight );
      }
*/
      PokeIconMoveBoxPartyDataMake( syswk, syswk->get_pos, put_pos );
      if( BOX2BGWFRM_PokeMenuOpenPutCheck( syswk->app->wfrm ) == FALSE ){
        BOX2OBJ_PokeCursorVanish( syswk, FALSE );
      }

      // �ړ���̃J�[�\���ʒu�␳
      if( put_pos != BOX2MAIN_GETPOS_NONE ){
        syswk->get_pos = PokeIconMoveAfterPosGet( syswk, BOX2OBJ_POKEICON_GET_POS );
      }

      // ���j���[�\���J�n
      if( BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == FALSE && ret1 == 0 ){
        BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
      }

      if( put_pos == BOX2MAIN_GETPOS_NONE && syswk->tray != syswk->get_tray ){
        u32 dir = BOX2MAIN_GetTrayScrollDir( syswk, syswk->tray, syswk->get_tray );
        syswk->tray = syswk->get_tray;
        BOX2OBJ_PokeIconBufLoad( syswk, syswk->tray );
        BOX2MAIN_WallPaperSet( syswk, BOX2MAIN_GetWallPaperNumber(syswk,syswk->tray), dir );
        if( dir == BOX2MAIN_TRAY_SCROLL_L ){
          vf->seq = SEIRI_SEQ_CANCEL_SCROLL_L;
        }else{
          vf->seq = SEIRI_SEQ_CANCEL_SCROLL_R;
        }
      }else{
        vf->seq = SEIRI_SEQ_ICON_PUT;
      }
      vf->cnt = 0;

    }else{
      if( vf->cnt != 0 ){
        vf->cnt--;
      }
      // �p�[�e�B�t���[���\���`�F�b�N
      if( ret1 == 0 && ret2 == 0 &&
          BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == FALSE ){
        if( BOX2UI_HitCheckPosTrayPoke( tpx, tpy ) != syswk->get_pos ){
          syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_ALL;
          BOX2BGWFRM_PartyPokeFrameInitPutRight( syswk->app->wfrm );
          BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
          BOX2OBJ_PartyPokeIconFrmInSet( syswk );
          PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
          // �߂��̃V�[�P���X���u���ށv�̏����ɂ���
          BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY_MOVE, syswk->get_pos );
//          BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
          BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_PASSIVE );
          syswk->app->old_cur_pos = syswk->get_pos;
          syswk->app->vnext_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_DATA_CHANGE;
        }
      }
      // �g���C�X�N���[�����q�b�g�`�F�b�N
      if( vf->cnt == 0 ){
        hit = BOX2UI_HitCheckContTrayArrow();
        if( hit == 0 ){
          PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
          BOX2MAIN_InitTrayScroll( syswk, BOX2MAIN_TRAY_SCROLL_L );
          vf->seq = SEIRI_SEQ_SCROLL_L;
        }else if( hit == 1 ){
          PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
          BOX2MAIN_InitTrayScroll( syswk, BOX2MAIN_TRAY_SCROLL_R );
          vf->seq = SEIRI_SEQ_SCROLL_R;
        }
      }
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_CANCEL_SCROLL_L:   // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i���j
    if( BOX2MAIN_VFuncTrayScrollLeft( syswk ) == 0 ){
      vf->seq = SEIRI_SEQ_ICON_PUT;
    }
    break;

  case SEIRI_SEQ_CANCEL_SCROLL_R:   // �g���C���؂�ւ���Ă����ꍇ�̃X�N���[�������i�E�j
    if( BOX2MAIN_VFuncTrayScrollRight( syswk ) == 0 ){
      vf->seq = SEIRI_SEQ_ICON_PUT;
    }
    break;

  case SEIRI_SEQ_ICON_PUT:  // �A�C�R���z�u
    if( PokeIconObjMove( syswk ) == FALSE ){
      PMSND_PlaySE( SE_BOX2_POKE_PUT );
      vf->seq = SEIRI_SEQ_ICON_PUT_END;
    }
    break;

  case SEIRI_SEQ_ICON_PUT_END:
    if( ret1 == 0 && ret2 == 0 ){
      vf->seq = SEIRI_SEQ_END;
    }
    break;

  case SEIRI_SEQ_SCROLL_L:  // �g���C�X�N���[���i���j
    if( BOX2MAIN_VFuncTrayScrollLeft( syswk ) == 0 ){
//      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = SEIRI_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_SCROLL_R:  // �g���C�X�N���[���i�E�j
    if( BOX2MAIN_VFuncTrayScrollRight( syswk ) == 0 ){
//      BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
      vf->cnt = TRAY_SCROLL_NEXT_WAIT;
      vf->seq = SEIRI_SEQ_ICON_GET;
    }
    if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
      MoveGetPokeIcon( syswk, tpx, tpy );
    }
    break;

  case SEIRI_SEQ_END:       // �I��
    {
      BOX2MAIN_POKEMOVE_WORK * work = vf->work;
      PokeIconBufPosChange( syswk, work->dat );
    }
    if( BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == FALSE &&
        BOX2BGWFRM_CheckPartyPokeFrameRight( syswk->app->wfrm ) == FALSE ){
      BOX2OBJ_PokeCursorMove( syswk->app, syswk->get_pos );
      BOX2OBJ_PokeCursorVanish( syswk, TRUE );
    }else{
//      BOX2OBJ_PokeCursorMove( syswk->app, syswk->get_pos );
      BOX2OBJ_PokeCursorVanish( syswk, FALSE );
      CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
      BOX2UI_PutHandCursor( syswk, syswk->get_pos );
//      CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
    }
    vf->seq = 0;
    return 0;
  }

  return 1;
}
