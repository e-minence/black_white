//======================================================================
/**
 * @file  p_sta_local_def.c
 * @brief �|�P�����X�e�[�^�X ���[�J����`
 * @author  ariizumi
 * @data  09/07/01
 *
 * ���W���[�����FPOKE_STATUS
 */
//======================================================================
#pragma once

#include "app/p_status.h"
#include "print/gf_font.h"
#include "print/printsys.h"
#include "debug/debugwin_sys.h"
#include "poke_tool/poketype.h"
#include "system/mcss.h"
#include "system/mcss_tool.h"

#include "p_status_gra.naix"
#include "msg/msg_pokestatus.h"


#define USE_STATUS_DEBUG (USE_DEBUGWIN_SYSTEM)

#pragma mark[>define BG
//BG�ʒ�`
#define PSTATUS_BG_MAIN_BG (GFL_BG_FRAME3_M)
#define PSTATUS_BG_PLATE   (GFL_BG_FRAME2_M)
#define PSTATUS_BG_PARAM   (GFL_BG_FRAME1_M)
#define PSTATUS_BG_3D      (GFL_BG_FRAME0_M)

#define PSTATUS_BG_SUB_BG     (GFL_BG_FRAME3_S)
#define PSTATUS_BG_SUB_PLATE  (GFL_BG_FRAME2_S)
#define PSTATUS_BG_SUB_TITLE  (GFL_BG_FRAME1_S)
#define PSTATUS_BG_SUB_STR    (GFL_BG_FRAME0_S)

//BG�p���b�g
#define PSTATUS_BG_PLT_MAIN (0x0)
#define PSTATUS_BG_PLT_BAR  (0x9)
#define PSTATUS_BG_PLT_FONT (0xe)

//����BG�p���b�g
#define PSTATUS_BG_SUB_PLT_HPBAR (0xd)
#define PSTATUS_BG_SUB_PLT_FONT (0xe)

//���C���y�[�W�̕�(�L�����P��)
#define PSTATUS_MAIN_PAGE_WIDTH (19)
//���ʂ̃^�C�g���̍���(�L�����P��)
#define PSTATUS_SUB_TITLE_HEIGHT (3)

#pragma mark[>define Font
//�����񋤒ʃI�t�Z�b�g
#define PSTATUS_STR_OFS_X (2)
#define PSTATUS_STR_OFS_Y (2)

//�t�H���g�J���[
#define PSTATUS_STR_COL_WHITE (PRINTSYS_LSB_Make(0xf,2,0))
#define PSTATUS_STR_COL_BLACK (PRINTSYS_LSB_Make(1,2,0))
#define PSTATUS_STR_COL_BLUE (PRINTSYS_LSB_Make(5,6,0))
#define PSTATUS_STR_COL_RED (PRINTSYS_LSB_Make(3,4,0))

//�t�H���g�J���[(�\��Ɛ��l�p
#define PSTATUS_STR_COL_TITLE PSTATUS_STR_COL_WHITE
#define PSTATUS_STR_COL_VALUE PSTATUS_STR_COL_BLACK
//���i�ɂ��␳�\���p
#define PSTATUS_STR_COL_TITLE_R (PRINTSYS_LSB_Make(0xf,4,0))
#define PSTATUS_STR_COL_TITLE_B (PRINTSYS_LSB_Make(0xf,6,0))



#pragma mark[>define OBJ
#define PSTATUS_OBJPLT_ICON (0x0) //����(3?
#define PSTATUS_OBJPLT_IJYOU (0x3)  //1�{
#define PSTATUS_OBJPLT_POKERUSU (0x4)  //1�{
#define PSTATUS_OBJPLT_MARK (0x5)  //1�{
#define PSTATUS_OBJPLT_BALL (0x6)  //1�{
#define PSTATUS_OBJPLT_POKE_TYPE (0x7)  //3�{
#define PSTATUS_OBJPLT_SKILL_PLATE (0xa)  //2�{
#define PSTATUS_OBJPLT_RIBBON_BAR (0xc)
#define PSTATUS_OBJPLT_CURSOR_COMMON (0xd)

#define PSTATUS_OBJPLT_SUB_HPBAR (0x8)
#define PSTATUS_OBJPLT_SUB_POKE_TYPE (0x5)  //3�{
#define PSTATUS_OBJPLT_SUB_RIBBON (0x0)  //5�{

//�p���b�g�A�j���p
#define PSTATUS_PALANIM_PAL_NO (3)
#define PSTATUS_PALANIM_PAL_POS (6)
#define PSTATUS_PALANIM_NUM (4)
#define PSTATUS_PALANIM_SPEED (0x400) //0x10000�ň��
#define PSTATUS_PALANIM_COL_LENGTH (8)


//OBJ���\�[�XIdx
enum PSTATUS_CEL_RESOURCE
{
  SCR_PLT_ICON_COMMON,
  SCR_PLT_BALL,
  SCR_PLT_MARK,
  SCR_PLT_POKE_TYPE,
  SCR_PLT_SUB_POKE_TYPE,
  SCR_PLT_HPBASE,
  SCR_PLT_SKILL,
  SCR_PLT_RIBBON_ICON,
  SCR_PLT_RIBBON_BAR,
  SCR_PLT_POKERUSU,
  SCR_PLT_ST_IJYOU,
  SCR_PLT_CURSOR_COMMON,
  
  SCR_NCG_ICON,
  SCR_NCG_ICON_COMMON,
  SCR_NCG_BALL,
  SCR_NCG_MARK,
  SCR_NCG_SKILL_TYPE_HENKA,
  SCR_NCG_SKILL_TYPE_BUTURI,
  SCR_NCG_SKILL_TYPE_TOKUSHU,
  SCR_NCG_HPBASE,
  SCR_NCG_SKILL,
  SCR_NCG_SKILL_CUR,
  SCR_NCG_POKERUSU,
  SCR_NCG_ST_IJYOU,
  SCR_NCG_RIBBON_CUR,
  
  SCR_ANM_ICON,
  SCR_ANM_ICON_COMMON,
  SCR_ANM_BALL,
  SCR_ANM_MARK,
  SCR_ANM_POKE_TYPE,
  SCR_ANM_SUB_POKE_TYPE,
  SCR_ANM_HPBASE,
  SCR_ANM_SKILL,
  SCR_ANM_SKILL_CUR,
  SCR_ANM_POKERUSU,
  SCR_ANM_ST_IJYOU,
  SCR_ANM_RIBBON_ICON,
  SCR_ANM_RIBBON_CUR,
  
  SCR_MAX ,
  
  SCR_PLT_START = SCR_PLT_ICON_COMMON,
  SCR_PLT_END = SCR_PLT_CURSOR_COMMON,
  SCR_NCG_START = SCR_NCG_ICON,
  SCR_NCG_END = SCR_NCG_RIBBON_CUR,
  SCR_ANM_START = SCR_ANM_ICON,
  SCR_ANM_END = SCR_ANM_RIBBON_CUR,
  
};

//�A�C�R���̃Z���̃A�j�����
enum PSTATUS_BARICON_ANIME
{

  SBA_PAGE1_NORMAL,
  SBA_PAGE2_NORMAL,
  SBA_PAGE3_NORMAL,
  SBA_PAGE1_SELECT,
  SBA_PAGE2_SELECT,
  SBA_PAGE3_SELECT,
  SBA_PAGE1_OFF,
  SBA_PAGE2_OFF,
  SBA_PAGE3_OFF,
};

//BAR�̃A�C�R��(�y�[�W�ȊO
enum PSTATUS_BARICON_TYPE
{
  SBT_PAGE1,
  SBT_PAGE2,
  SBT_PAGE3,
  SBT_CHECK,
  SBT_CURSOR_UP,
  SBT_CURSOR_DOWN,
  SBT_EXIT,
  SBT_RETURN,
  
  SBT_MAX,
};


#pragma mark[>define
//BAR�̍��W
#define PSTATUS_BAR_CELL_Y (168)
#define PSTATUS_BAR_CELL_PAGE1_X (0)
#define PSTATUS_BAR_CELL_PAGE2_X (40)
#define PSTATUS_BAR_CELL_PAGE3_X (80)
#define PSTATUS_BAR_CELL_CHECK_X (120)
#define PSTATUS_BAR_CELL_CURSOR_UP_X (144)
#define PSTATUS_BAR_CELL_CURSOR_DOWN_X (168)
#define PSTATUS_BAR_CELL_CURSOR_EXIT (200)
#define PSTATUS_BAR_CELL_CURSOR_RETURN (232)

typedef enum
{
  SRT_CONTINUE,
  SRT_RETURN,
  SRT_EXIT,
}PSTATUS_RETURN_TYPE;

typedef enum
{
  SMS_FADEIN,
  SMS_FADEIN_WAIT,
  SMS_FADEOUT,
  SMS_FADEOUT_WAIT,
  SMS_UPDATE,
}PSTATUS_MAIN_SEQ;

typedef enum
{
  SMES_NONE,
  SMES_FADEOUT,
  SMES_WAIT,
  SMES_FADEIN,
}PSTATUS_MOSAIC_EFFECT_SEQ;


typedef struct _PSTATUS_SUB_WORK PSTATUS_SUB_WORK;
typedef struct _PSTATUS_INFO_WORK PSTATUS_INFO_WORK;
typedef struct _PSTATUS_RIBBON_WORK PSTATUS_RIBBON_WORK;
typedef struct _PSTATUS_SKILL_WORK PSTATUS_SKILL_WORK;
typedef struct _PSTATUS_DEBUG_WORK PSTATUS_DEBUG_WORK;

typedef struct
{
  HEAPID heapId;
  GFL_TCB *vBlankTcb;
  
  PSTATUS_DATA *psData;
  
  BOOL isWaitDisp;
  BOOL reqPlayVoice;
  
  u16 dataPos;  //���Ԗڂ��H
  u8  befDataPos;
  u8  scrollCnt;
  u8  ktst;
  u32 befVCount;
  int barButtonHit; //�o�[�̃{�^����������Ă��邩�H

  u32      tpx;
  u32      tpy;
  u32      befTpx;
  u32      befTpy;
  
  //�Q�Ƃ������̂ŕێ����Ă���pp
  BOOL isEgg;
  u32  friend;
  u32  isHaveRibbon;
  
  BOOL isActiveBarButton;
  BOOL isAnimeBarCursor[2]; //�㉺�{�^���̃A�j����ɈÓ]�`�F�b�N�p
  PSTATUS_MAIN_SEQ    mainSeq;
  PSTATUS_RETURN_TYPE retVal;
    
  PSTATUS_PAGE_TYPE page;
  PSTATUS_PAGE_TYPE befPage;
  BOOL              shortCutCheck[PPT_MAX];

  //3D
  GFL_G3D_CAMERA    *camera;
  GFL_BBD_SYS       *bbdSys;
  MCSS_SYS_WORK     *mcssSys;
  
  //MSG�n
  GFL_MSGDATA *msgHandle;
  GFL_FONT *fontHandle;
  PRINT_QUE *printQue;

  //�T�u���[�N
  PSTATUS_SUB_WORK *subWork;
  PSTATUS_INFO_WORK *infoWork;
  PSTATUS_SKILL_WORK *skillWork;
  PSTATUS_RIBBON_WORK *ribbonWork;
  
  //Cell�n
  u32 cellRes[SCR_MAX];
  u32 cellResTypeNcg[POKETYPE_MAX];
  GFL_CLUNIT  *cellUnit;
  GFL_CLWK    *clwkBarIcon[SBT_MAX];
  GFL_CLWK    *clwkTypeIcon[2];
  GFL_CLWK    *clwkExitButton;  //�I���{�^���̃A�j���I�����o�p
  
  //�p���b�g�A�j��
  u16   anmCnt;
  u16   anmPalBase[PSTATUS_PALANIM_NUM];
  u16   anmPal[PSTATUS_PALANIM_NUM];

  u16   anmSkillPalBase[2][16];
  u16   anmSkillPal[16];

  //ppp�̎�pp�ɕϊ�����悤
  //PSTATUS_UTIL_GetCurrentPP�Ŏ擾
  POKEMON_PARAM *calcPP;

  PSTATUS_MOSAIC_EFFECT_SEQ mosaicEffSeq;
  u8 mosaicCnt;
  
#if USE_STATUS_DEBUG
  VecFx32 shadowScale;
  u16 shadowRotate;
  VecFx32 shadowOfs;

  
  //�f�o�b�O�p
  u32 year1;
  u32 month1;
  u32 day1;
  u32 place1;
  u32 year2;
  u32 month2;
  u32 day2;
  u32 place2;
  u32 level;
  u32 natsuki;
  u32 devRom;
  BOOL isDevEgg;
  BOOL isDevParent;
  BOOL isDevEvent;
  BOOL isDevMemo;
  BOOL isDevRibbon;
#endif

}PSTATUS_WORK;
