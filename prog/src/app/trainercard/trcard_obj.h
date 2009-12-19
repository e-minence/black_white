#ifndef _TRAINER_CARD_OBJ_H_
#define _TRAINER_CARD_OBJ_H_

// CellActor�ɏ��������郊�\�[�X�}�l�[�W���̎�ނ̐��i���}���`�Z���E�}���`�Z���A�j���͎g�p���Ȃ��j
#define RESOURCE_NUM                ( 4)
#define TR_CARD_BADGE_ACT_MAX       (16)
#define TR_CARD_STAR_ACT_MAX        ( 8)
#define TR_CARD_GRATE_STAR_ACT_MAX  ( 8)
#define TR_CARD_BTNEFF_ACT_MAX      ( 1)
#define TR_CARD_CLACT_RES_NUM       ( 3)

#define TR_CARD_MAIN_ACT_MAX  (TR_CARD_BADGE_ACT_MAX+TR_CARD_STAR_ACT_MAX+\
    TR_CARD_GRATE_STAR_ACT_MAX+TR_CARD_BTNEFF_ACT_MAX)
enum{
 ACTS_BTN_BACK,     // ��߂�uU�v
 ACTS_BTN_END,      // ���ڃt�B�[���h�܂Ŗ߂�u�~�v
 ACTS_BTN_CHANGE,   // �o�b�W��ʂƐ؂�ւ���
 ACTS_BTN_EFF,      // �^�b�`�����Ƃ��̃G�t�F�N�g
 ACTS_BTN_LOUPE,    // �����ҏW�{�^��
 ACTS_BTN_PEN,      // �y���捕���؂�ւ��{�^��
 ACTS_BTN_TURN,     // �J�[�h���Ԃ��{�^��
 ACTS_BTN_BOOKMARK, // �u�b�N�}�[�N�{�^��
 TR_CARD_SUB_ACT_MAX,
};

enum{
 ANMS_BACK_OFF,
 ANMS_BACK_ON,
 ANMS_SIGN_OFF,
 ANMS_SIGN_ON,
 ANMS_EXP,
 ANMS_EFF,
 ANMS_BLACK_PEN_L,
 ANMS_BLACK_PEN_G,
 ANMS_WHITE_PEN_L,
 ANMS_WHITE_PEN_G,
 ANMS_ANIME_L,
 ANMS_ANIME_G,
 ANMS_STOP_L,
 ANMS_STOP_G,
 ANMS_CARD_L,
 ANMS_CARD_G,
 ANMS_BADGE_L,
 ANMS_BADGE_G,
 ANMS_LOUPE_L,
 ANMS_LOUPE_G,
 ANMS_SIMPLE_L,
 ANMS_SIMPLE_G,
 ANMS_TURN_L,
 ANMS_TURN_G,
};

#define TR_CARD_BADGE_ACT_START (0)
#define TR_CARD_STAR_ACT_START  ( TR_CARD_BADGE_ACT_MAX )
#define TR_CARD_GRATE_STAR_ACT_START  (TR_CARD_STAR_ACT_START+TR_CARD_STAR_ACT_MAX)
#define TR_CARD_BTNEFF_ACT_START  (TR_CARD_GRATE_STAR_ACT_START+TR_CARD_GRATE_STAR_ACT_MAX)

#define TR_CARD_ACT_MAX (TR_CARD_BADGE_ACT_MAX+TR_CARD_STAR_ACT_MAX+\
    TR_CARD_GRATE_STAR_ACT_MAX+TR_CARD_BTNEFF_ACT_MAX)

///typedef struct TR_CARD_OBJ_WORK_tag *TR_CARD_OBJ_PTR;
typedef struct
{
  u32 pltIdx;
  u32 ncgIdx;
  u32 anmIdx;
}TRCARD_CELL_RES;

typedef struct TR_CARD_OBJ_WORK_tag
{
  /*
  CLACT_SET_PTR       ClactSet;               // �Z���A�N�^�[�Z�b�g
  CLACT_U_EASYRENDER_DATA RendData;               // �ȈՃ����_�[�f�[�^
  CLACT_U_RES_MANAGER_PTR ResMan[2][RESOURCE_NUM];        // ���\�[�X�}�l�[�W��
  CLACT_U_RES_OBJ_PTR   ResObjTbl[2][RESOURCE_NUM];   // ���\�[�X�I�u�W�F�e�[�u��(�㉺���)
  CLACT_WORK_PTR      ClActWork[TR_CARD_ACT_MAX];       // �Z���A�N�^�[���[�N�|�C���^�z��
  CLACT_WORK_PTR      ClActWorkS[TR_CARD_SUB_ACT_MAX];    // �Z���A�N�^�[���[�N�|�C���^�z��
  */
  HEAPID  heapId;

  GFL_CLUNIT  *cellUnit;
  GFL_CLSYS_REND* cellRender;

  GFL_CLWK  *ClActWork[TR_CARD_ACT_MAX];    // �Z���A�N�^�[���[�N�|�C���^�z��
  GFL_CLWK  *ClActWorkS[TR_CARD_SUB_ACT_MAX]; // �Z���A�N�^�[���[�N�|�C���^�z��
  
  TRCARD_CELL_RES resCell[TR_CARD_CLACT_RES_NUM];

  void *badgePalBuf[TR_CARD_BADGE_ACT_MAX];   //�p���b�g�f�[�^TR_CARD_BADGE_ACT_MAX��
  NNSG2dPaletteData *badgePalData[TR_CARD_BADGE_ACT_MAX]; //�p���b�g�f�[�^TR_CARD_BADGE_ACT_MAX��
}TR_CARD_OBJ_WORK;


extern void InitTRCardCellActor( TR_CARD_OBJ_WORK *wk , const GFL_DISP_VRAM *vramBank);
extern void SetTrCardActor( TR_CARD_OBJ_WORK *wk, const u8 *inBadgeDisp ,const BOOL isClear);
extern void SetTrCardActorSub( TR_CARD_OBJ_WORK *wk);
extern void RereaseCellObject(TR_CARD_OBJ_WORK *wk);

extern void SetSActDrawSt( TR_CARD_OBJ_WORK *wk, u8 act_id, u8 anm_pat ,u8 draw_f);
extern void SetEffActDrawSt( TR_CARD_OBJ_WORK *wk, u8 pat ,u8 draw_f);

#endif //_TRAINER_CARD_OBJ_H_

