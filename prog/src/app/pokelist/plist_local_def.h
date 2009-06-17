//======================================================================
/**
 * @file  plist_local_def.h
 * @brief �|�P�������X�g ���[�J����`
 * @author  ariizumi
 * @data  09/06/10
 */
//======================================================================
#pragma once

#include "app/pokelist.h"
#include "print/gf_font.h"
#include "print/printsys.h"
#include "debug/debugwin_sys.h"

#define PLIST_LIST_MAX (6)

//BG�ʒ�`(�A���t�@�ݒ�̂Ƃ���͒�`���Ⴄ�̂ŕς�����ύX���邱��
#define PLIST_BG_MAIN_BG (GFL_BG_FRAME3_M)
#define PLIST_BG_PLATE   (GFL_BG_FRAME2_M)
#define PLIST_BG_PARAM   (GFL_BG_FRAME1_M)
#define PLIST_BG_MENU    (GFL_BG_FRAME0_M)
#define PLIST_BG_3D      (GFL_BG_FRAME0_M)

#define PLIST_BG_SUB_BG (GFL_BG_FRAME3_S)

//BG�p���b�g
#define PLIST_BG_PLT_FONT (0x0e)

//�����F
#define PLIST_FONT_PARAM_LETTER (0xF)
#define PLIST_FONT_PARAM_SHADOW (0x1)

//OBJ���\�[�XIdx
typedef enum
{
  PCR_PLT_OBJ_COMMON,
  PCR_PLT_ITEM_ICON,
  PCR_PLT_CONDITION,
  PCR_PLT_HP_BASE,
  PCR_PLT_POKEICON,
  
  PCR_NCG_CURSOR,
  PCR_NCG_BALL,
  PCR_NCG_BAR_ICON,
  PCR_NCG_ITEM_ICON,
  PCR_NCG_CONDITION,
  PCR_NCG_HP_BASE,
  //�|�P�A�C�R����NCG�͊e�v���[�g�Ŏ���
  
  PCR_ANM_CURSOR,
  PCR_ANM_BALL,
  PCR_ANM_BAR_ICON,
  PCR_ANM_ITEM_ICON,
  PCR_ANM_CONDITION,
  PCR_ANM_HP_BASE,
  PCR_ANM_POKEICON,
  
  PCR_PLT_START = PCR_PLT_OBJ_COMMON,
  PCR_PLT_END = PCR_PLT_POKEICON,
  PCR_NCG_START = PCR_NCG_CURSOR,
  PCR_NCG_END = PCR_NCG_HP_BASE,
  PCR_ANM_START = PCR_ANM_CURSOR,
  PCR_ANM_END = PCR_ANM_POKEICON,
  
  PCR_MAX,
  
}PLIST_CEL_RESOURCE;

//OBJ�p���b�g�̊J�n�ʒu
#define PLIST_OBJPLT_COMMON (0)     //3�{
#define PLIST_OBJPLT_ITEM_ICON (3)  //1�{
#define PLIST_OBJPLT_CONDITION (4)  //1�{
#define PLIST_OBJPLT_HP_BASE   (5)  //1�{
#define PLIST_OBJPLT_POKEICON  (6)  //3�{

//�J�[�\���̃A�j��
enum PLIST_CURCOR_ANIME
{
  PCA_NORMAL_A, //�ʏ�0�ԗp
  PCA_NORMAL_B, //�ʏ�
  PCA_CHANGE_A, //����ւ�0�ԗp
  PCA_CHANGE_B, //����ւ�
  PCA_SELECT_A, //�I��0�ԗp
  PCA_SELECT_B, //�I��
  PCA_MENU,   //���j���[�p

};


typedef struct _PLIST_PLATE_WORK PLIST_PLATE_WORK;
typedef struct _PLIST_DEBUG_WORK PLIST_DEBUG_WORK;

typedef struct
{
  HEAPID heapId;
  GFL_TCB *vBlankTcb;

  GFL_MSGDATA *msgHandle;
  GFL_FONT *fontHandle;
  GFL_FONT *sysFontHandle;
  PRINT_QUE *printQue;
  
  //�v���[�gScr
  void  *plateScrRes;
  NNSG2dScreenData *plateScrData;

  //�v���[�g�f�[�^
  PLIST_PLATE_WORK  *plateWork[PLIST_LIST_MAX];
  
  //Cell�n
  u32 cellRes[PCR_MAX];
  GFL_CLUNIT  *cellUnit;
  GFL_CLWK    *clwkCursor[2];
  

  PLIST_DATA *plData;
#if USE_DEBUGWIN_SYSTEM
  PLIST_DEBUG_WORK *debWork;
#endif
}PLIST_WORK;

