
// -------------------------------------------------------------
/** 
 * @file wbshop.h
 * @brief 
 **/
// -------------------------------------------------------------

// �u�����Â����v
#define WINDOW0_WIN_X     (1)
#define WINDOW0_WIN_Y     (1)
#define WINDOW0_WIN_W     (9)
#define WINDOW0_WIN_H     (2)
#define WINDOW0_WIN_PAL     (11)
#define WINDOW0_WIN_CHR     (20)

// �H�H�H�H�H�H�~
#define WINDOW1_WIN_X     (3)
#define WINDOW1_WIN_Y     (3)
#define WINDOW1_WIN_W     (8)
#define WINDOW1_WIN_H     (2)
#define WINDOW1_WIN_PAL     (11)
#define WINDOW1_WIN_CHR     (38)

// �ǂ������X�g�\���p
#define WINDOW2_WIN_X     (12)
#define WINDOW2_WIN_Y     (1)
#define WINDOW2_WIN_W     (19)
#define WINDOW2_WIN_H     (16)
#define WINDOW2_WIN_PAL     (2)
#define WINDOW2_WIN_CHR     (54)

// �u�����Ă邩���@�H�H�R�v
#define WINDOW3_WIN_X     (1)
#define WINDOW3_WIN_Y     (15)
#define WINDOW3_WIN_W     (15)
#define WINDOW3_WIN_H     (2)
#define WINDOW3_WIN_PAL     (11)
#define WINDOW3_WIN_CHR     (415)

// �u�w�H�H�@�H�H�H�H�H�H�~�v
#define WINDOW5_WIN_X     (18)
#define WINDOW5_WIN_Y     (15)
#define WINDOW5_WIN_W     (13)
#define WINDOW5_WIN_H     (2)
#define WINDOW5_WIN_PAL     (11)
#define WINDOW5_WIN_CHR     (447)

// �ǂ�������
#define WINDOW6_WIN_X     (6)
#define WINDOW6_WIN_Y     (18)
#define WINDOW6_WIN_W     (26)
#define WINDOW6_WIN_H     (6)
#define WINDOW6_WIN_PAL     (11)
#define WINDOW6_WIN_CHR     (469)

// ��b�E�C���h�E
#define WINDOW7_WIN_X     (1)
#define WINDOW7_WIN_Y     (19)
#define WINDOW7_WIN_W     (30)
#define WINDOW7_WIN_H     (4)
#define WINDOW7_WIN_PAL     (11)
#define WINDOW7_WIN_CHR     (631)




const BMPWIN_DAT Window0BmpData={
  GFL_BG_FRAME1_M,
  WINDOW0_WIN_X,  WINDOW0_WIN_Y,
  WINDOW0_WIN_W,  WINDOW0_WIN_H,
  WINDOW0_WIN_PAL,  WINDOW0_WIN_CHR,
};

const BMPWIN_DAT Window1BmpData={
  GFL_BG_FRAME1_M,
  WINDOW1_WIN_X,  WINDOW1_WIN_Y,
  WINDOW1_WIN_W,  WINDOW1_WIN_H,
  WINDOW1_WIN_PAL,  WINDOW1_WIN_CHR,
};

const BMPWIN_DAT Window2BmpData={
  GFL_BG_FRAME1_M,
  WINDOW2_WIN_X,  WINDOW2_WIN_Y,
  WINDOW2_WIN_W,  WINDOW2_WIN_H,
  WINDOW2_WIN_PAL,  WINDOW2_WIN_CHR,
};

const BMPWIN_DAT Window3BmpData={
  GFL_BG_FRAME1_M,
  WINDOW3_WIN_X,  WINDOW3_WIN_Y,
  WINDOW3_WIN_W,  WINDOW3_WIN_H,
  WINDOW3_WIN_PAL,  WINDOW3_WIN_CHR,
};

const BMPWIN_DAT Window5BmpData={
  GFL_BG_FRAME1_M,
  WINDOW5_WIN_X,  WINDOW5_WIN_Y,
  WINDOW5_WIN_W,  WINDOW5_WIN_H,
  WINDOW5_WIN_PAL,  WINDOW5_WIN_CHR,
};

const BMPWIN_DAT Window6BmpData={
  GFL_BG_FRAME1_M,
  WINDOW6_WIN_X,  WINDOW6_WIN_Y,
  WINDOW6_WIN_W,  WINDOW6_WIN_H,
  WINDOW6_WIN_PAL,  WINDOW6_WIN_CHR,
};

const BMPWIN_DAT Window7BmpData={
  GFL_BG_FRAME1_M,
  WINDOW7_WIN_X,  WINDOW7_WIN_Y,
  WINDOW7_WIN_W,  WINDOW7_WIN_H,
  WINDOW7_WIN_PAL,  WINDOW7_WIN_CHR,
};



const BMPWIN_DAT* Window0BmpDataTable[]={
  &Window0BmpData,
  &Window1BmpData,
  &Window2BmpData,
  &Window3BmpData,
  &Window5BmpData,
  &Window6BmpData,
  &Window7BmpData,
};

