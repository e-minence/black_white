#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "app/app_menu_common.h"
#include "arc_def.h"

#include "trcard_obj.h"
#include "trainer_case.naix"

#define MAIN_LCD  ( GFL_BG_MAIN_DISP )  // 0
#define SUB_LCD   ( GFL_BG_SUB_DISP )   // 1

#define TR_SUBSURFACE_Y (32)

//** CharManager PlttManager�p **//
#define TR_CARD_CHAR_CONT_NUM       (2)
#define TR_CARD_CHAR_VRAMTRANS_MAIN_SIZE  (0)//(2048)
#define TR_CARD_CHAR_VRAMTRANS_SUB_SIZE   (0)
#define TR_CARD_PLTT_CONT_NUM       (2)
#define TR_CARD_PALETTE_NUM         (16)  //�p���b�g9�{
#define TR_CARD_BADGE_PALETTE_NUM     (4) //�p���b�g4�{

#define BTN_EFF_POS_X (12*8)
#define BTN_EFF_POS_Y (17*8)


typedef struct BADGE_POS_tag{
  int x;
  int y;
}BADGE_POS;

#define BADGE_LX0 (8*12)
#define BADGE_LX1 (8*18)
#define BADGE_LX2 (8*24)
#define BADGE_LX3 (8*30)

#define BADGE_LY0 (8*6)
#define BADGE_LY1 (8*11)
#define BADGE_LY2 (8*17)
#define BADGE_LY3 (8*22)

#define BADGE_NC_OFS  (8*5)

static const BADGE_POS BadgePos[] = {
  {BADGE_LX0,BADGE_LY0},
  {BADGE_LX1,BADGE_LY0},
  {BADGE_LX2,BADGE_LY0},
  {BADGE_LX3,BADGE_LY0},
  {BADGE_LX0,BADGE_LY1},
  {BADGE_LX1,BADGE_LY1},
  {BADGE_LX2,BADGE_LY1},
  {BADGE_LX3,BADGE_LY1},

  {BADGE_LX0,BADGE_LY2},
  {BADGE_LX1,BADGE_LY2},
  {BADGE_LX2,BADGE_LY2},
  {BADGE_LX3,BADGE_LY2},
  {BADGE_LX0,BADGE_LY3},
  {BADGE_LX1,BADGE_LY3},
  {BADGE_LX2,BADGE_LY3},
  {BADGE_LX3,BADGE_LY3},
};

static const BADGE_POS StarPos[] = {
  {8*3,8*5},
  {8*10,8*5},
  {8*17,8*5},
  {8*24,8*5},
  {8*3,8*12},
  {8*10,8*12},
  {8*17,8*12},
  {8*24,8*12},
};

static void InitCharPlttManager(void);

//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[������
 *
 * @param wk  �g���[�i�[�J�[�hOBJ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void InitTRCardCellActor( TR_CARD_OBJ_WORK *wk , const GFL_DISP_VRAM *vramBank)
{
  int i;
  ARCHANDLE *p_handle = GFL_ARC_OpenDataHandle( ARCID_TRAINERCARD , wk->heapId );
  InitCharPlttManager();

  GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN , vramBank, wk->heapId );
  // �Z���A�N�^�[������
  wk->cellUnit = GFL_CLACT_UNIT_Create( TR_CARD_ACT_MAX, 0, wk->heapId );

  // �Z���A�N�^�[���\�[�X�]��
  wk->resCell[0].pltIdx = GFL_CLGRP_PLTT_RegisterEx( p_handle, NARC_trainer_case_card_button_NCLR, 
                                                     CLSYS_DRAW_SUB, 0, 0, 4, wk->heapId );
  wk->resCell[0].ncgIdx = GFL_CLGRP_CGR_Register( p_handle , NARC_trainer_case_card_button_NCGR, FALSE , 
                                                  CLSYS_DRAW_SUB , wk->heapId );
  wk->resCell[0].anmIdx = GFL_CLGRP_CELLANIM_Register( p_handle , 
                                                       NARC_trainer_case_card_button_NCER, 
                                                       NARC_trainer_case_card_button_NANR, wk->heapId );
  GFL_ARC_CloseDataHandle( p_handle );


  // �^�b�`�o�[�p���\�[�X�]��
  p_handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId() , wk->heapId );
  wk->resCell[1].pltIdx = GFL_CLGRP_PLTT_Register( p_handle, APP_COMMON_GetBarIconPltArcIdx(),
                                                   CLSYS_DRAW_SUB , 4*32 , wk->heapId );
  wk->resCell[1].ncgIdx = GFL_CLGRP_CGR_Register( p_handle, APP_COMMON_GetBarIconCharArcIdx(),
                                                   FALSE, CLSYS_DRAW_SUB, wk->heapId );
  wk->resCell[1].anmIdx = GFL_CLGRP_CELLANIM_Register( p_handle, 
                                                       APP_COMMON_GetBarIconCellArcIdx( APP_COMMON_MAPPING_32K ),
                                                       APP_COMMON_GetBarIconAnimeArcIdx( APP_COMMON_MAPPING_32K ),
                                                       wk->heapId );
  GFL_ARC_CloseDataHandle( p_handle );
  
  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );    // MAIN DISP OBJ ON
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );   // SUB  DISP OBJ ON
}



//==========================================
// �Z���A�N�^�[�o�^�p���
//==========================================
typedef struct{
  u16 x,y;
  u8  cgx, pal, cel, pri;
  u32 anime;
} CLACT_ENTRY_DAT;

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʃZ���A�N�^�[���Z�b�g
 *
 * @param wk        �g���[�i�[�J�[�hOBJ���[�N
 * @param inBadgeDisp   �o�b�W�\���t���O���X�g
 * @param isClear     �a�����肵�Ă��邩�H
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void SetTrCardActorSub( TR_CARD_OBJ_WORK *wk)
{
  int i;
  const CLACT_ENTRY_DAT entry_dat[]={
    { 228, 21*8, 1,1,1,0, APP_COMMON_BARICON_RETURN },   // �u�t�v
    { 204, 21*8, 1,1,1,0, APP_COMMON_BARICON_EXIT   },   // �u�~�v
    {   4, 21*8, 0,0,0,0, ANMS_BADGE_L,             },   // ���[�O��ʂ�
    {   0,    0, 0,0,0,0, ANMS_EFF                  },   // �G�t�F�N�g
    {  36, 21*8, 0,0,0,0, ANMS_LOUPE_L              },   // �����`��{�^��
    {  68, 21*8, 0,0,0,0, ANMS_BLACK_PEN_L          },   // �y����{�^��
    { 100, 21*8, 0,0,0,0, ANMS_TURN_L,              },   // �J�[�h���Ԃ��{�^��
    { 180, 21*8+4, 1,1,1,0, APP_COMMON_BARICON_CHECK_OFF },// �u�b�N�}�[�N�{�^��
    
  };

  {
    GFL_CLWK_DATA addData;
    
    //�Z���A�N�^�[�\���J�n
    // �����(�T�u���)
    for(i=0;i<TR_CARD_SUB_ACT_MAX;i++){
      addData.pos_x  = entry_dat[i].x;
      addData.pos_y  = entry_dat[i].y;  //TODO
      addData.anmseq = entry_dat[i].anime;
      addData.bgpri  = entry_dat[i].pri;
      wk->ClActWorkS[i] = GFL_CLACT_WK_Create( wk->cellUnit , 
                                               wk->resCell[entry_dat[i].pal].pltIdx,
                                               wk->resCell[entry_dat[i].cgx].ncgIdx,
                                               wk->resCell[entry_dat[i].cel].anmIdx ,
                                               &addData , CLSYS_DEFREND_SUB , wk->heapId );
      GFL_CLACT_WK_SetDrawEnable( wk->ClActWorkS[i],   TRUE); //�\��
      GFL_CLACT_WK_SetAutoAnmSpeed( wk->ClActWorkS[i], FX32_ONE );
      GFL_CLACT_WK_SetAutoAnmFlag( wk->ClActWorkS[i],  TRUE );

      // �^�b�`�G�t�F�N�g�͔�\���ɂ���
      if(i==ACTS_BTN_EFF){
        GFL_CLACT_WK_SetDrawEnable( wk->ClActWorkS[i], FALSE); //��\��
      }
    }
    
    // �G�t�F�N�g�A�N�^�[�͔�\���ɂ���
    GFL_CLACT_WK_SetDrawEnable( wk->ClActWorkS[ANMS_EFF], FALSE); //��\��
   
  } 
}
//--------------------------------------------------------------------------------------------
/**
 * 2D�Z���I�u�W�F�N�g���
 *
 * @param wk  �I�u�W�F�N�g���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void RereaseCellObject(TR_CARD_OBJ_WORK *wk)
{
  u8 i,j;

  // �Z���A�N�^�[���\�[�X���
  for(i=0;i<TR_CARD_CLACT_RES_NUM;i++)
  {
    GFL_CLGRP_PLTT_Release( wk->resCell[i].pltIdx );
    GFL_CLGRP_CGR_Release( wk->resCell[i].ncgIdx );
    GFL_CLGRP_CELLANIM_Release( wk->resCell[i].anmIdx );
  }

  // �Z���A�N�^�[�Z�b�g�j��
  GFL_CLACT_UNIT_Delete(wk->cellUnit);

  //OAM�����_���[�j��
  GFL_CLACT_SYS_Delete();

//  DeleteCharManager();
//  DeletePlttManager();
  
  GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_32K);  
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʃ{�^���A�N�^�[�@�\����ԕύX
 *
 * @param wk      OBJ���[�N
 * @param inBadgeNo �o�b�W�i���o�[
 * @param inOalNo   �p���b�g�ԍ�
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void SetSActDrawSt( TR_CARD_OBJ_WORK *wk, u8 act_id, u8 anm_pat ,u8 draw_f)
{
//  CLACT_SetAnmFlag(wk->ClActWorkS[act_id],TRUE);
  GFL_CLACT_WK_SetAnmSeq( wk->ClActWorkS[act_id],anm_pat);
  GFL_CLACT_WK_ResetAnm(wk->ClActWorkS[act_id]);
  GFL_CLACT_WK_SetDrawEnable(wk->ClActWorkS[act_id],draw_f);  
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʃ{�^���G�t�F�N�g�A�N�^�[�@�\����ԕύX
 *
 * @param wk      OBJ���[�N
 * @param inBadgeNo �o�b�W�i���o�[
 * @param inOalNo   �p���b�g�ԍ�
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void SetEffActDrawSt( TR_CARD_OBJ_WORK *wk, u8 pat ,u8 draw_f)
{
  GFL_CLACTPOS cellPos;
  static const u8 pos_x[2] = {28*8,7*8};
  u32 tpx,tpy;
  GFL_UI_TP_GetPointCont( &tpx , &tpy );
  if(pat == 2)
  {
    cellPos.x = tpx;
    cellPos.y = tpy; //TODO +TR_SUBSURFACE_Y
    GFL_CLACT_WK_SetPos(wk->ClActWorkS[ACTS_BTN_EFF],&cellPos,CLSYS_DEFREND_SUB);
  }
  else
  {
    cellPos.x = pos_x[pat];
    cellPos.y = 22*8; //TODO +TR_SUBSURFACE_Y
    GFL_CLACT_WK_SetPos(wk->ClActWorkS[ACTS_BTN_EFF],&cellPos,CLSYS_DEFREND_SUB);
  }
  GFL_CLACT_WK_ResetAnm(wk->ClActWorkS[ACTS_BTN_EFF]);
  GFL_CLACT_WK_SetDrawEnable(wk->ClActWorkS[ACTS_BTN_EFF],draw_f);  
}

//--------------------------------------------------------------------------------------------
/**
 * �L�����N�^�}�l�[�W���[
 *�p���b�g�}�l�[�W���[�̏�����
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void InitCharPlttManager(void)
{
  /*
  // �L�����N�^�}�l�[�W���[������
  {
    CHAR_MANAGER_MAKE cm = {
      TR_CARD_CHAR_CONT_NUM,
      TR_CARD_CHAR_VRAMTRANS_MAIN_SIZE,
      TR_CARD_CHAR_VRAMTRANS_SUB_SIZE,
      HEAPID_TR_CARD
    };
    InitCharManager(&cm);
  }
  // �p���b�g�}�l�[�W���[������
  InitPlttManager(TR_CARD_PLTT_CONT_NUM, HEAPID_TR_CARD);

  // �ǂݍ��݊J�n�ʒu��������
  CharLoadStartAll();
  PlttLoadStartAll();

  REND_OAM_UtilOamRamClear_Main(HEAPID_TR_CARD);
  REND_OAM_UtilOamRamClear_Sub(HEAPID_TR_CARD);

//  DellVramTransferManager();
  */
}

