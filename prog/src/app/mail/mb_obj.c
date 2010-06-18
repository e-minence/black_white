//============================================================================================
/**
 * @file  mb_obj.c
 * @brief ���[���{�b�N�X��� OBJ�֘A
 * @author  Hiroyuki Nakamura
 * @date  09.01.31
 */
//============================================================================================


#include <gflib.h>

#include "system/main.h"
#include "arc/arc_def.h"
#include "font/font.naix"

#include "pokeicon/pokeicon.h"
#include "arc/poke_icon.naix"
#include "app/app_menu_common.h"

#include "mb_main.h"
#include "mb_obj.h"
#include "mail_gra.naix"


//============================================================================================
//  �萔��`
//============================================================================================


// �L�������\�[�XID
enum {
  CHR_ID_POKEICON1=0, // �|�P�����A�C�R��
  CHR_ID_POKEICON2,
  CHR_ID_POKEICON3,
  CHR_ID_MBOBJ,
  CHR_ID_APP_COMMON,
  CHR_ID_PMS,
  CHR_ID_MAX
};
// �p���b�g���\�[�XID
enum {
  PAL_ID_POKEICON=0,  // �|�P�����A�C�R��
  PAL_ID_MBOBJ,
  PAL_ID_APP_COMMON,
  PAL_ID_PMS,
  PAL_ID_MAX
};
// �Z�����\�[�XID
enum {
  CEL_ID_POKEICON=0,  // �|�P�����A�C�R��
  CEL_ID_MBOBJ,
  CEL_ID_APP_COMMON,
  CEL_ID_PMS,
  CEL_ID_MAX
};
// �Z���A�j�����\�[�XID
enum {
  ANM_ID_POKEICON=0,  // �|�P�����A�C�R��
  ANM_ID_MBOBJ,
  ANM_ID_MAX
};

enum{
  RES_CGR = 0,
  RES_PAL,
  RES_CELANM,
};


#define CLA_H_NONE  ( CLACT_U_HEADER_DATA_NONE )  // �Z���A�N�^�[�w�b�_�[�f�[�^�Ȃ�

#define SUB_ACTOR_DISTANCE  ( 512*FX32_ONE )    // �T�u��ʂ�OBJ�\��Y���W�I�t�Z�b�g

// �p���b�g�z�u
#define PALSIZ_POKEICON     ( POKEICON_PAL_MAX )  // �p���b�g���F�|�P�����A�C�R��
#define PALSIZ_MBOBJ      ( 2 )         // �p���b�g���F���̑��̂n�a�i

#define MAILREAD_CUR_PX   ( 224 )   //�u���[������ށv�̃J�[�\���w���W
#define MAILREAD_CUR_PY   ( 176 )   //�u���[������ށv�̃J�[�\���x���W

#define POKEICON_PX   ( 128 )   // �|�P�����A�C�R���\���w���W
#define POKEICON_PY   ( 160 )   // �|�P�����A�C�R���\���x���W
#define POKEICON_SX   ( 40 )    // �|�P�����A�C�R���\���w�Ԋu

#define POKEICON_CGX_SIZE ( 0x20*4*4 )  // �|�P�����A�C�R����CGX�T�C�Y


//============================================================================================
//  �\���̐錾
//============================================================================================
typedef struct {
  
  s16 x;              ///< [ X ] ���W
  s16 y;              ///< [ Y ] ���W
  s16 z;              ///< [ Z ] ���W
  
  u16 anm;            ///< �A�j���ԍ�
  int pri;            ///< �D�揇��
  int pal;            ///< �p���b�g�ԍ� �����̒l�� TCATS_ADD_S_PAL_AUTO �ɂ��邱�ƂŁANCE�f�[�^�̃J���[No�w����󂯌p��
  int d_area;         ///< �`��G���A
  
  int id[ 6 ];        ///< �g�p���\�[�XID�e�[�u��
    
  int bg_pri;         ///< BG�ʂւ̗D��x
  int vram_trans;     ///< Vram�]���t���O
  
} CLACT_OBJECT_ADD_PARAM;


//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================
static void ClactResManInit( MAILBOX_APP_WORK * appwk );
static void ClactResManExit( MAILBOX_APP_WORK * appwk );
static GFL_CLWK* ClactAdd( MAILBOX_APP_WORK * appwk, const CLACT_OBJECT_ADD_PARAM * prm );
static void ClactDel( MAILBOX_APP_WORK * appwk, u32 id );
static void ClactDelAll( MAILBOX_APP_WORK * appwk );

static void PokeIconArcHandleOpen( MAILBOX_APP_WORK * appwk );
static void PokeIconArcHandleClose( MAILBOX_APP_WORK * appwk );
static void PokeIconResLoad( MAILBOX_APP_WORK * appwk );

static void MailBoxObjResLoad( MAILBOX_APP_WORK * appwk );


//============================================================================================
//  �O���[�o���ϐ�
//============================================================================================

// �����y�[�W�̂n�a�i�f�[�^
static const CLACT_OBJECT_ADD_PARAM ClaAddPrm[] =
{
  { // �����
    8, 168, 0,
    APP_COMMON_BARICON_CURSOR_LEFT, 0, 0, CLSYS_DEFREND_MAIN,
    { CHR_ID_APP_COMMON, PAL_ID_APP_COMMON, CEL_ID_APP_COMMON, 0, 0, 0 },
    3, 0
  },
  { // �E���
    32, 168, 0,
    APP_COMMON_BARICON_CURSOR_RIGHT, 0, 0, CLSYS_DEFREND_MAIN,
    { CHR_ID_APP_COMMON, PAL_ID_APP_COMMON, CEL_ID_APP_COMMON, 0, 0, 0 },
    3, 0
  },
  { // �J�[�\��
    64, 16, 0,
    5, 0, 1, CLSYS_DEFREND_MAIN,
    { CHR_ID_MBOBJ, PAL_ID_MBOBJ, CEL_ID_MBOBJ, 0, 0, 0 },
    1, 0
  },
  { // �߂�{�^��
    224, 168, 0,
    APP_COMMON_BARICON_RETURN, 0, 0, CLSYS_DEFREND_MAIN,
    { CHR_ID_APP_COMMON, PAL_ID_APP_COMMON, CEL_ID_APP_COMMON, 0, 0, 0 },
    3, 0
  },

  { // �|�P�����A�C�R���P
    POKEICON_PX-POKEICON_SX*0, POKEICON_PY, 0,
    0, 0, 0, CLSYS_DEFREND_SUB,
    { CHR_ID_POKEICON1, PAL_ID_POKEICON, CEL_ID_POKEICON, 0, 0, 0 },
    1, 0
  },
  { // �|�P�����A�C�R���Q
    POKEICON_PX-POKEICON_SX*1, POKEICON_PY, 0,
    0, 0, 0, CLSYS_DEFREND_SUB,
    { CHR_ID_POKEICON2, PAL_ID_POKEICON, CEL_ID_POKEICON, 0, 0, 0 },
    1, 0
  },
  { // �|�P�����A�C�R���R
    POKEICON_PX-POKEICON_SX*2, POKEICON_PY, 0,
    0, 0, 0, CLSYS_DEFREND_SUB,
    { CHR_ID_POKEICON3, PAL_ID_POKEICON, CEL_ID_POKEICON, 0, 0, 0 },
    1, 0
  },
  { // �u�~�v�{�^��
    188, 168, 0, 
    APP_COMMON_BARICON_EXIT, 0, 0, CLSYS_DEFREND_MAIN,
    { CHR_ID_APP_COMMON, PAL_ID_APP_COMMON, CEL_ID_APP_COMMON, 0, 0, 0 },    
    3,0
  },
};



//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[������
 *
 * @param syswk   �V�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBOBJ_Init( MAILBOX_SYS_WORK * syswk )
{
  u32 i;

  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );    // MAIN DISP OBJ ON
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );   // SUB DISP OBJ ON
  for( i=0; i<MBMAIN_OBJ_MAX; i++ ){
    syswk->app->clres[0][i] = 0;
    syswk->app->clres[1][i] = 0;
    syswk->app->clres[2][i] = 0;
  }


  PokeIconArcHandleOpen( syswk->app );
  ClactResManInit( syswk->app );

  MailBoxObjResLoad( syswk->app );
  PokeIconResLoad( syswk->app );


  for( i=0; i<MBMAIN_OBJ_MAX; i++ ){
    syswk->app->clwk[i] = ClactAdd( syswk->app, &ClaAddPrm[i] );
    GFL_CLACT_WK_SetAutoAnmFlag( syswk->app->clwk[i], TRUE );

  }

  if( syswk->app->mail_max <= MBMAIN_MAILLIST_MAX ){
    MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_ARROW_L, FALSE );
    MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_ARROW_R, FALSE );
  }else if( syswk->sel_page == 0 ){
    MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_ARROW_L, FALSE );
  }else{
    MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_ARROW_R, FALSE );
  }
  MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_POKEICON1, FALSE );
  MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_POKEICON2, FALSE );
  MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_POKEICON3, FALSE );
}

//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[���
 *
 * @param syswk   �V�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBOBJ_Exit( MAILBOX_SYS_WORK * syswk )
{

  ClactDelAll( syswk->app );
  ClactResManExit( syswk->app );

  PokeIconArcHandleClose( syswk->app );
}

//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[�A�j�����C��
 *
 * @param appwk   �A�v�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBOBJ_Main( MAILBOX_APP_WORK * appwk )
{
  
  if(appwk->clUnit != NULL){
    GFL_CLACT_SYS_Main();
  }

}


extern const GFL_DISP_VRAM MAILBOX_VramTbl;
#define OBJ_MAILBOX_MAX ( 2 )

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�}�l�[�W���[������
 *
 * @param appwk   �A�v�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void ClactResManInit( MAILBOX_APP_WORK * appwk )
{
  static const GFL_CLSYS_INIT clsys_init  =
  {
    0, 0,   //���C���T�[�t�F�[�X�̍���X,Y���W
    0, 512, //�T�u�T�[�t�F�[�X�̍���X,Y���W
    4, 64,  //���C��OAM�Ǘ��J�n�`�I�� �i�ʐM�A�C�R���p�ɊJ�n��4�ȏ�ɁA�܂����ׂ�4�̔{�������Ă��������j
    4, 64,  //��bOAM�Ǘ��J�n�`�I��  �i�ʐM�A�C�R���p�ɊJ�n��4�ȏ�ɁA�܂����ׂ�4�̔{�������Ă��������j
    0,      //�Z��Vram�]���Ǘ���
    CHR_ID_MAX, PAL_ID_MAX, CEL_ID_MAX, 32,
    16, 16,       //���C���A�T�u��CGRVRAM�Ǘ��̈�J�n�I�t�Z�b�g�i�ʐM�A�C�R���p��16�ȏ�ɂ��Ă��������j
  };


  //�V�X�e���쐬
  GFL_CLACT_SYS_Create( &clsys_init, &MAILBOX_VramTbl, HEAPID_MAILBOX_APP );
  appwk->clUnit = GFL_CLACT_UNIT_Create( MBMAIN_OBJ_MAX, 0, HEAPID_MAILBOX_APP );
  GFL_CLACT_UNIT_SetDefaultRend( appwk->clUnit );

  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

/*
  appwk->csp = CATS_AllocMemory( HEAPID_MAILBOX_APP );
  appwk->crp = CATS_ResourceCreate( appwk->csp );

  {
    TCATS_OAM_INIT  coi = {
      0, 128,
      0, 32,
      0, 128,
      0, 32,
    };
    TCATS_CHAR_MANAGER_MAKE ccmm = {
      1024,
      1024*64,
      1024*16,
      GX_OBJVRAMMODE_CHAR_1D_32K,
      GX_OBJVRAMMODE_CHAR_1D_32K
    };
    CATS_SystemInit( appwk->csp, &coi, &ccmm, 32 );
  }
  {
    TCATS_RESOURCE_NUM_LIST crnl = {
      CHR_ID_MAX, PAL_ID_MAX, CEL_ID_MAX, ANM_ID_MAX, 0, 0
    };
    CATS_ClactSetInit( appwk->csp, appwk->crp, MBMAIN_OBJ_MAX );
    CATS_ResourceManagerInit( appwk->csp, appwk->crp, &crnl );
  }
  {
    CLACT_U_EASYRENDER_DATA * renddata = CATS_EasyRenderGet( appwk->csp );
    CLACT_U_SetSubSurfaceMatrix( renddata, 0, SUB_ACTOR_DISTANCE );
  }
*/
}

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�}�l�[�W���[���
 *
 * @param appwk   �A�v�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void ClactResManExit( MAILBOX_APP_WORK * appwk )
{
//  CATS_ResourceDestructor_S( appwk->csp, appwk->crp );
//  CATS_FreeMemory( appwk->csp );

  GFL_CLGRP_CELLANIM_Release( appwk->clres[RES_PAL][PAL_ID_POKEICON]);
  GFL_CLGRP_PLTT_Release( appwk->clres[RES_CELANM][CEL_ID_POKEICON] );
 
  //�V�X�e���j��
  GFL_CLACT_UNIT_Delete( appwk->clUnit );
  GFL_CLACT_SYS_Delete();
  
}

//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[�ǉ�
 *
 * @param appwk �A�v�����[�N
 * @param prm   �ǉ��f�[�^
 *
 * @return  �ǉ������Z���A�N�^�[�̃|�C���^
 */
//--------------------------------------------------------------------------------------------
static GFL_CLWK* ClactAdd( MAILBOX_APP_WORK * appwk, const CLACT_OBJECT_ADD_PARAM * prm )
{
  GFL_CLWK_DATA dat;
  dat.pos_x   = prm->x;   ///< [ X ] ���W
  dat.pos_y   = prm->y;   ///< [ Y ] ���W
  dat.anmseq  = prm->anm;        ///< �A�j���ԍ�
  dat.softpri = 0;
  dat.bgpri   = prm->bg_pri;

  return GFL_CLACT_WK_Create( appwk->clUnit, appwk->clres[RES_CGR][prm->id[0]], 
                                             appwk->clres[RES_PAL][prm->id[1]],
                                             appwk->clres[RES_CELANM][prm->id[2]], &dat, 
                                             prm->d_area, HEAPID_MAILBOX_APP );


}

//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[�폜
 *
 * @param appwk �A�v�����[�N
 * @param id    OBJ ID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void ClactDel( MAILBOX_APP_WORK * appwk, u32 id )
{
  if( appwk->clwk[id] != NULL ){
    GFL_CLACT_WK_Remove( appwk->clwk[id] );
    appwk->clwk[id] = NULL;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[�S�폜
 *
 * @param appwk   �A�v�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void ClactDelAll( MAILBOX_APP_WORK * appwk )
{
  u32 i;

  for( i=0; i<MBMAIN_OBJ_MAX; i++ ){
    ClactDel( appwk, i );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[�A�j���ύX
 *
 * @param appwk �A�v�����[�N
 * @param id    OBJ ID
 * @param anm   �A�j���ԍ�
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBOBJ_AnmSet( MAILBOX_APP_WORK * appwk, u32 id, u32 anm )
{
//  CATS_ObjectAnimeFrameSetCap( appwk->clwk[id], 0 );
//  CATS_ObjectAnimeSeqSetCap( appwk->clwk[id], anm );

  GFL_CLACT_WK_SetAnmSeq( appwk->clwk[id], anm );
}


//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[�\���؂�ւ�
 *
 * @param appwk �A�v�����[�N
 * @param id    OBJ ID
 * @param flg   �\���t���O
 *
 * @return  none
 *
 * @li  flg = TRUE : �\��
 * @li  flg = FALSE : ��\��
 */
//--------------------------------------------------------------------------------------------
void MBOBJ_Vanish( MAILBOX_APP_WORK * appwk, u32 id, BOOL flg )
{

  GFL_CLACT_WK_SetDrawEnable( appwk->clwk[id], flg );

}

//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[�ړ�
 *
 * @param appwk �A�v�����[�N
 * @param id    OBJ ID
 * @param px    �w���W
 * @param py    �x���W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBOBJ_PosSet( MAILBOX_APP_WORK * appwk, u32 id, s16 px, s16 py )
{
//  CATS_ObjectPosSetCap( appwk->clwk[id], px, py );
  GFL_CLACTPOS pos;
  pos.x = px;
  pos.y = py;

  if(py < 256){
    GFL_CLACT_WK_SetPos( appwk->clwk[id], &pos, CLSYS_DEFREND_MAIN );
  }else{
    GFL_CLACT_WK_SetPos( appwk->clwk[id], &pos, CLSYS_DEFREND_SUB );
  }
  GFL_CLACT_WK_SetAnmFrame( appwk->clwk[id], 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[�������ݒ�
 *
 * @param appwk �A�v�����[�N
 * @param id    OBJ ID
 * @param flg   ON/OFF�t���O
 *
 * @return  none
 *
 * @li  flg : TRUE = ON, FALSE = OFF
 */
//--------------------------------------------------------------------------------------------
void MBOBJ_BlendModeSet( MAILBOX_APP_WORK * appwk, u32 id, BOOL flg )
{
  if( flg == TRUE ){
    GFL_CLACT_WK_SetObjMode( appwk->clwk[id], GX_OAM_MODE_XLU );
  }else{
    GFL_CLACT_WK_SetObjMode( appwk->clwk[id], GX_OAM_MODE_NORMAL );
  }
}


//============================================================================================
//  �|�P�����A�C�R��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���̃A�[�J�C�u�n���h���擾
 *
 * @param appwk   �A�v�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconArcHandleOpen( MAILBOX_APP_WORK * appwk )
{
  appwk->pokeicon_ah = GFL_ARC_OpenDataHandle( ARCID_POKEICON, HEAPID_MAILBOX_APP );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���̃A�[�J�C�u�n���h���j��
 *
 * @param appwk   �A�v�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconArcHandleClose( MAILBOX_APP_WORK * appwk )
{
  GFL_ARC_CloseDataHandle( appwk->pokeicon_ah );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R�����\�[�X�ǂݍ���
 *
 * @param appwk   �A�v�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconResLoad( MAILBOX_APP_WORK * appwk )
{
  u32 i;

  ARCHANDLE *handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_MAILBOX_APP);

  for( i=CHR_ID_POKEICON1; i<=CHR_ID_POKEICON3; i++ ){
//    CATS_LoadResourceCharArcH(
//      appwk->csp, appwk->crp, appwk->pokeicon_ah,
//      PokeIconCgxArcIndexGetByMonsNumber(0,0,0),
//      0, NNS_G2D_VRAM_TYPE_2DSUB, i );

    // �|�P�����A�C�R���L�����o�^
    appwk->clres[RES_CGR][i] = GFL_CLGRP_CGR_Register( 
                                                handle,
                                                APP_COMMON_GetPokeItemIconCharArcIdx(), 0,
                                                CLSYS_DRAW_SUB, HEAPID_MAILBOX_APP );


  }


/*
  CATS_LoadResourcePlttArcH(
    appwk->csp, appwk->crp,
    appwk->pokeicon_ah, PokeIconPalArcIndexGet(), 0,
    PALSIZ_POKEICON, NNS_G2D_VRAM_TYPE_2DSUB, PAL_ID_POKEICON );
  CATS_LoadResourceCellArcH(
    appwk->csp, appwk->crp,
    appwk->pokeicon_ah, PokeIconCellArcIndexGet(), 0, CEL_ID_POKEICON );
  CATS_LoadResourceCellAnmArcH(
    appwk->csp, appwk->crp,
    appwk->pokeicon_ah, PokeIconAnmCellArcIndexGet(), 0, ANM_ID_POKEICON );
*/

 // Pltt
  appwk->clres[RES_PAL][PAL_ID_POKEICON] = GFL_CLGRP_PLTT_Register(
                                handle, POKEICON_GetPalArcIndex(),
                                CLSYS_DRAW_SUB, 0, HEAPID_MAILBOX_APP );
  // CellAnm
  appwk->clres[RES_CELANM][CEL_ID_POKEICON] = GFL_CLGRP_CELLANIM_Register(
                                      handle, 
                                      APP_COMMON_GetPokeItemIconCellArcIdx(APP_COMMON_MAPPING_32K),
                                      APP_COMMON_GetPokeItemIconAnimeArcIdx(APP_COMMON_MAPPING_32K), 
                                      HEAPID_MAILBOX_APP );

  GFL_ARC_CloseDataHandle( handle );


}

/*
static void PokeIconResFree( MAILBOX_APP_WORK * appwk )
{
  u32 i;

  for( i=CHR_ID_POKEICON1; i<=CHR_ID_POKEICON3; i++ ){
    CATS_FreeResourceChar( appwk->crp, i );
  }
  CATS_FreeResourcePltt( appwk->crp, PAL_ID_POKEICON );
  CATS_FreeResourceCell( appwk->crp, CEL_ID_POKEICON );
  CATS_FreeResourceCellAnm( appwk->crp, ANM_ID_POKEICON );
}
*/


//============================================================================================
//  ���̑��̂n�a�i
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ���[���{�b�N�X��ʂ̃��\�[�X�ǂݍ���
 *
 * @param appwk   �A�v�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void MailBoxObjResLoad( MAILBOX_APP_WORK * appwk )
{
  ARCHANDLE * handle = GFL_ARC_OpenDataHandle( ARCID_MAIL_GRA, HEAPID_MAILBOX_APP );

  // ���[���{�b�N�X�f��
  // Char
  appwk->clres[RES_CGR][CHR_ID_MBOBJ] = GFL_CLGRP_CGR_Register( handle,
                                            NARC_mail_gra_mailbox_obj_m_lz_NCGR, 1,
                                            CLSYS_DRAW_MAIN, HEAPID_MAILBOX_APP );
  // Pltt
  appwk->clres[RES_PAL][PAL_ID_MBOBJ] = GFL_CLGRP_PLTT_Register( handle,
                                            NARC_mail_gra_mailbox_obj_NCLR,
                                            CLSYS_DRAW_MAIN, 0, HEAPID_MAILBOX_APP );
  // CellAnm
  appwk->clres[RES_CELANM][CEL_ID_MBOBJ] = GFL_CLGRP_CELLANIM_Register( handle,
                                            NARC_mail_gra_mailbox_obj_NCER,
                                            NARC_mail_gra_mailbox_obj_NANR, 
                                            HEAPID_MAILBOX_APP );

  GFL_ARC_CloseDataHandle( handle );

  // �^�b�`�o�[�f��
  handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_MAILBOX_APP );

  // Char
  appwk->clres[RES_CGR][CHR_ID_APP_COMMON] = GFL_CLGRP_CGR_Register( handle,
                                            APP_COMMON_GetBarIconCharArcIdx(), 0,
                                            CLSYS_DRAW_MAIN, HEAPID_MAILBOX_APP );
  // Pltt
  appwk->clres[RES_PAL][PAL_ID_APP_COMMON] = GFL_CLGRP_PLTT_Register( handle,
                                            APP_COMMON_GetBarIconPltArcIdx(),
                                            CLSYS_DRAW_MAIN, 4*32, HEAPID_MAILBOX_APP );
  // CellAnm
  appwk->clres[RES_CELANM][CEL_ID_APP_COMMON] = GFL_CLGRP_CELLANIM_Register( handle,
                                            APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_32K),
                                            APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_32K), 
                                            HEAPID_MAILBOX_APP );
  
  GFL_ARC_CloseDataHandle( handle );

}

//--------------------------------------------------------------------------------------------
/**
 * �u���[������ށv�J�[�\���Z�b�g
 *
 * @param appwk   �A�v�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBOBJ_MailReadCurMove( MAILBOX_APP_WORK * appwk )
{
  MBOBJ_PosSet( appwk, MBMAIN_OBJ_CURSOR, MAILREAD_CUR_PX, MAILREAD_CUR_PY );
  MBOBJ_AnmSet( appwk, MBMAIN_OBJ_CURSOR, 4 );
}
