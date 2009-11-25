//==============================================================================
/**
 * @file    monolith_tool.c
 * @brief   ���m���X��ʃc�[��
 * @author  matsuda
 * @date    2009.11.21(�y)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/gamesystem.h"
#include "arc_def.h"
#include "monolith_common.h"
#include "monolith.naix"
#include "monolith_tool.h"


//==============================================================================
//  �萔��`
//==============================================================================
///���ʑf�ރZ���̃A�j���V�[�P���X�ԍ�
typedef enum{
  COMMON_ANMSEQ_PANEL_LARGE,
  COMMON_ANMSEQ_PANEL_SMALL,
  COMMON_ANMSEQ_UP,
  COMMON_ANMSEQ_DOWN,
  COMMON_ANMSEQ_BLACK_TOWN,
  COMMON_ANMSEQ_WHITE_TOWN,
  COMMON_ANMSEQ_RETURN,
}COMMON_ANMSEQ;

///�\�t�g�v���C�I���e�B
enum{
  SOFTPRI_PANEL_BMPFONT = 10,
  SOFTPRI_PANEL,
};

///�A�N�^�[BG�v���C�I���e�B
enum{
  BGPRI_PANEL = 0,
};

///�p�l��(��)BMPFONT���WX
#define PANEL_LARGE_BMPFONT_POS_X   (PANEL_POS_X - PANEL_LARGE_CHARSIZE_X/2*8)
///�p�l��(��)BMPFONT���WX
#define PANEL_SMALL_BMPFONT_POS_X   (PANEL_POS_X - PANEL_SMALL_CHARSIZE_X/2*8)


//==============================================================================
//  �f�[�^
//==============================================================================
///�p�l���A�N�^�[�w�b�_
static const GFL_CLWK_DATA ActHead_Panel = {
  PANEL_POS_X,                   //pos_x
  0,                             //pos_y
  COMMON_ANMSEQ_PANEL_LARGE,     //anmseq
  SOFTPRI_PANEL,                 //softpri
  BGPRI_PANEL,                   //bgpri
};

///�p�l���A�N�^�[��BMPOAM�w�b�_
static const BMPOAM_ACT_DATA BmpOamHead_Panel = {
  NULL,                          //bmp
  0,                             //x
  0,                             //y
  0,                             //pltt_index
  0,                             //pal_offset
  SOFTPRI_PANEL_BMPFONT,         //soft_pri
  BGPRI_PANEL,                   //bg_pri
  0,                             //setSerface
  0,                             //draw_type
};


//==================================================================
/**
 * �p�l���A�N�^�[����
 *
 * @param   setup       ���m���X�S��ʋ��ʐݒ�f�[�^�ւ̃|�C���^
 * @param   dest		    ���������p�l���A�N�^�[�����
 * @param   res_index		���ʑf��Index
 * @param   size		    �p�l���T�C�Y
 * @param   y		        Y���W(���S)
 * @param   msg_id      ���b�Z�[�WID
 */
//==================================================================
void MonolithTool_Panel_Create(MONOLITH_SETUP *setup, PANEL_ACTOR *dest, COMMON_RESOURCE_INDEX res_index, PANEL_SIZE size, int y, u32 msg_id)
{
  CLSYS_DEFREND_TYPE defrend_type;
  GFL_CLWK_DATA head = ActHead_Panel;
  BMPOAM_ACT_DATA bmpoam_head = BmpOamHead_Panel;
  int bmp_size_x;
  STRBUF *strbuf;
  
  //BMP�쐬
  bmp_size_x = (size == PANEL_SIZE_LARGE) ? PANEL_LARGE_CHARSIZE_X : PANEL_SMALL_CHARSIZE_X;
  dest->bmp = GFL_BMP_Create(bmp_size_x, PANEL_CHARSIZE_Y, GFL_BMP_16_COLOR, HEAPID_MONOLITH);
  
  //�w�b�_�쐬
  head.anmseq = (size == PANEL_SIZE_LARGE) ? COMMON_ANMSEQ_PANEL_LARGE : COMMON_ANMSEQ_PANEL_SMALL;
  head.pos_y = y;
  bmpoam_head.y = y + 8;
  bmpoam_head.bmp = dest->bmp;
  bmpoam_head.pltt_index = setup->common_res[res_index].pltt_bmpfont_index;
  if(res_index == COMMON_RESOURCE_INDEX_UP){
    defrend_type = CLSYS_DEFREND_MAIN;
    head.anmseq = COMMON_ANMSEQ_PANEL_LARGE;
    bmpoam_head.x = PANEL_LARGE_BMPFONT_POS_X;
    bmpoam_head.draw_type = CLSYS_DRAW_MAIN;
  }
  else{
    defrend_type = CLSYS_DEFREND_SUB;
    head.anmseq = COMMON_ANMSEQ_PANEL_SMALL;
    bmpoam_head.x = PANEL_SMALL_BMPFONT_POS_X;
    bmpoam_head.draw_type = CLSYS_DRAW_SUB;
  }
  bmpoam_head.setSerface = defrend_type;
  
  //����
  dest->cap = GFL_CLACT_WK_Create(setup->clunit, setup->common_res[res_index].char_index, 
    setup->common_res[res_index].pltt_index, setup->common_res[res_index].cell_index, 
	  &head, defrend_type, HEAPID_MONOLITH);
  dest->bmpoam = BmpOam_ActorAdd(setup->bmpoam_sys, &bmpoam_head);
  
  //�����`��
  strbuf = GFL_MSG_CreateString( setup->mm_monolith, msg_id );
  PRINTSYS_PrintQue( setup->printQue, dest->bmp, 0, 0, strbuf, setup->font_handle );
  
  //�����t���O�Z�b�g
  dest->trans_flag = TRUE;
}

//==================================================================
/**
 * �p�l���A�N�^�[�폜
 *
 * @param   panel		�p�l���A�N�^�[�ւ̃|�C���^
 */
//==================================================================
void MonolithTool_Panel_Delete(PANEL_ACTOR *panel)
{
  GFL_CLACT_WK_Remove(panel->cap);
  BmpOam_ActorDel(panel->bmpoam);
  GFL_BMP_Delete(panel->bmp);
  panel->cap = NULL;
  panel->bmpoam = NULL;
  panel->bmp = NULL;
}

//==================================================================
/**
 * �p�l���A�N�^�[�X�V����
 *
 * @param   setup   ���m���X�S��ʋ��ʐݒ�f�[�^�ւ̃|�C���^
 * @param   panel		�p�l���A�N�^�[�ւ̃|�C���^
 *
 * @retval  BOOL		TRUE:BMP�]�����s����
 */
//==================================================================
BOOL MonolithTool_Panel_TransUpdate(MONOLITH_SETUP *setup, PANEL_ACTOR *panel)
{
  if( panel->trans_flag == TRUE 
      && PRINTSYS_QUE_IsExistTarget(setup->printQue, panel->bmp) == FALSE ){
    BmpOam_ActorBmpTrans(panel->bmpoam);
    panel->trans_flag = FALSE;
    return TRUE;
  }
  return FALSE;
}



#if 0
//--------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�`��֘A�̐ݒ�
 *
 * @param   cs		
 */
//--------------------------------------------------------------
static void CCLocal_MessageSetting(COMPATI_SYS *cs)
{
	int i;

	cs->drawwin[CCBMPWIN_TITLE].win = GFL_BMPWIN_Create(
		FRAME_MSG_S, 8, 2, 16, 2, CC_FONT_PALNO, GFL_BMP_CHRAREA_GET_B);
	cs->drawwin[CCBMPWIN_POINT].win = GFL_BMPWIN_Create(
		FRAME_MSG_S, 14, 8, 6, 2, CC_FONT_PALNO, GFL_BMP_CHRAREA_GET_B);
	cs->drawwin[CCBMPWIN_TALK].win = GFL_BMPWIN_Create(
		FRAME_MSG_S, 1, 18, 30, 4, CC_FONT_PALNO, GFL_BMP_CHRAREA_GET_B);
	for(i = 0; i < CCBMPWIN_MAX; i++){
		GF_ASSERT(cs->drawwin[i].win != NULL);
		cs->drawwin[i].bmp = GFL_BMPWIN_GetBmp(cs->drawwin[i].win);
		GFL_BMP_Clear( cs->drawwin[i].bmp, 0xff );
		GFL_BMPWIN_MakeScreen( cs->drawwin[i].win );
		GFL_BMPWIN_TransVramCharacter( cs->drawwin[i].win );
		PRINT_UTIL_Setup( cs->drawwin[i].printUtil, cs->drawwin[i].win );
	}
	GFL_BG_LoadScreenReq(FRAME_MSG_S);	//MakeScreen�����̂𔽉f

	cs->fontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_MONOLITH );

//		PRINTSYS_Init( HEAPID_MONOLITH );
	cs->printQue = PRINTSYS_QUE_Create( HEAPID_MONOLITH );

	cs->mm = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_compati_check_dat, HEAPID_MONOLITH);
	for(i = 0; i < CCBMPWIN_MAX; i++){
		cs->strbuf_win[i] = GFL_STR_CreateBuffer( 64, HEAPID_MONOLITH );
	}
	cs->strbuf_point = GFL_MSG_CreateString(cs->mm, COMPATI_STR_009);
}

//--------------------------------------------------------------
/**
 * @brief   �~�A�N�^�[�쐬
 *
 * @param   cs		
 * @param   hdl		�A�[�J�C�u�n���h��
 */
//--------------------------------------------------------------
static void CCLocal_CircleActor_Create(COMPATI_SYS *cs, ARCHANDLE *hdl)
{
	int i;
	
	//���\�[�X�o�^
	cs->cgr_id[CHARID_CIRCLE] = GFL_CLGRP_CGR_Register(
		hdl, NARC_compati_check_cc_circle_NCGR, FALSE, CLSYS_DRAW_MAIN, HEAPID_MONOLITH);
	cs->cell_id[CELLID_CIRCLE] = GFL_CLGRP_CELLANIM_Register(
		hdl, NARC_compati_check_cc_circle_NCER, NARC_compati_check_cc_circle_NANR, HEAPID_MONOLITH);
	//�p���b�g���\�[�X�o�^(�������~�����F��ς���̂ŉ~�̐����o�^)
	for(i = 0; i < CC_CIRCLE_MAX; i++){
		cs->pltt_id[PLTTID_CIRCLE + i] = GFL_CLGRP_PLTT_RegisterEx(hdl, 
			NARC_compati_check_cc_circle_NCLR,
			CLSYS_DRAW_MAIN, 0x20 * i, 0, 1, HEAPID_MONOLITH);
	}
	
	//�A�N�^�[�o�^
	for(i = 0; i < CC_CIRCLE_MAX; i++){
		cs->circle_cap[i] = GFL_CLACT_WK_Create(cs->clunit, cs->cgr_id[CHARID_CIRCLE], 
			cs->pltt_id[PLTTID_CIRCLE + i], cs->cell_id[CELLID_CIRCLE], 
			&CircleObjParam, CLSYS_DEFREND_MAIN, HEAPID_MONOLITH);
		GFL_CLACT_WK_AddAnmFrame(cs->circle_cap[i], FX32_ONE);
		GFL_CLACT_WK_SetDrawEnable(cs->circle_cap[i], FALSE);
	}
}

//--------------------------------------------------------------
/**
 * @brief   �~�A�N�^�[�j��
 *
 * @param   cs		
 */
//--------------------------------------------------------------
static void CCLocal_CircleActor_Delete(COMPATI_SYS *cs)
{
	int i;
	
	//�A�N�^�[�폜
	for(i = 0; i < CC_CIRCLE_MAX; i++){
		GFL_CLACT_WK_Remove(cs->circle_cap[i]);
	}
	
	//���\�[�X���
	GFL_CLGRP_CGR_Release(cs->cgr_id[CHARID_CIRCLE]);
	GFL_CLGRP_CELLANIM_Release(cs->cell_id[CELLID_CIRCLE]);
	for(i = 0; i < CC_CIRCLE_MAX; i++){
		GFL_CLGRP_PLTT_Release(cs->pltt_id[PLTTID_CIRCLE + i]);
	}
}
#endif

