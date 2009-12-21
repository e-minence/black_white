//============================================================================================
/**
 * @file  trcard_bmp.h
 * @bfief �g���[�i�[�J�[�h���BMP�֘A�w�b�_
 * @author  Akito Mori / Nozomu Saito (WB�ڐA�͗L�򂭂�)
 * @date  05.11.15
 */
//============================================================================================
#ifndef _TRCARD_BMP_H_
#define _TRCARD_BMP_H_

#include "trcard_sys.h"


//=====================================
// ���l�̌����ő吔
//=====================================
typedef enum {
  TR_ID_DIGIT = 5,    // �g���[�i�[�h�c�̌���
  MONEY_DIGIT = 6,
  MONS_NUM_DIGIT = 3,
  SCORE_DIGIT = 9,
  TIME_H_DIGIT = 3,
  TIME_M_DIGIT = 2,
  YEAR_DIGIT = 2,
  MONTH_DIGIT = 2,
  DAY_DIGIT = 2,
  COMM_DIGIT = 6,
  BATTLE_DIGIT = 4,
  TRADE_DIGIT = 6,
}TR_DIGIT;

#define TR_DIGIT_MAX  (12)


// �X�N���[������X�R�A���X�g�̕`����e��`
enum{
  SCORE_LINE_ID=0,
  SCORE_LINE_MONEY,
  SCORE_LINE_START_DATE_1,
  SCORE_LINE_START_DATE_2,
  SCORE_LINE_FIRST_CHAMPION_1,
  SCORE_LINE_FIRST_CHAMPION_2,
  SCORE_LINE_COMM_NUM,
  SCORE_LINE_COMM_BATTLE_NUM,
  SCORE_LINE_COMM_WIN_NUM,
  SCORE_LINE_COMM_LOSE_NUM,
  SCORE_LINE_COMM_TRADE_NUM,
  SCORE_LINE_MAX,
};


//�����E�B���h�E�`��p�^�[����`
enum{
 EXPWIN_PAT_SIGN,
 EXPWIN_PAT_TOUCH,
};

#define TR_STRING_LEN (32)        //���[�J���C�Y���l�����߂ɁB
#define TR_DIGIT_LEN  (TR_DIGIT_MAX+1)

extern void TRCBmp_AddTrCardBmp(TR_CARD_WORK* wk);
extern void TRCBmp_ExitTrCardBmpWin(TR_CARD_WORK* wk);
extern void TRCBmp_TransTrWinInfo(TR_CARD_WORK* wk,GFL_BMPWIN *win[]);
extern void TRCBmp_WriteTrWinInfo(TR_CARD_WORK* wk, GFL_BMPWIN *win[], const TR_CARD_DATA* inTrCardData );
extern void TRCBmp_WriteTrWinInfoRev(TR_CARD_WORK* wk, GFL_BMPWIN *win[], const TR_CARD_DATA* inTrCardData );
extern void TRCBmp_NonDispWinInfo(GFL_BMPWIN *win[], const u8 start, const u8 end);
extern void TRCBmp_WritePlayTime(TR_CARD_WORK* wk, GFL_BMPWIN *win[], const TR_CARD_DATA *inTrCardData, STRBUF *str);
extern void TRCBmp_WriteSec(TR_CARD_WORK* wk,GFL_BMPWIN *win, const BOOL inDisp, STRBUF *inSecBuf);
extern void TRCBmp_WriteBackButton(TR_CARD_WORK* wk,const u8 on_f);
extern void TRCBmp_WriteExpWin(TR_CARD_WORK* wk,u8 pat);
extern void TRCBmp_SignDrawMsgPut(TR_CARD_WORK* wk,const u8 pat);
extern void TRCBmp_SignDrawYesNoCall(TR_CARD_WORK* wk,const u8 pat);
extern int  TRCBmp_SignDrawYesNoWait(TR_CARD_WORK* wk,const u8 pat);
extern void TRCBmp_PrintTrainerType( TR_CARD_WORK *wk, int trtype, int trans_sw );
extern void TRCBmp_PrintPersonality( TR_CARD_WORK *wk, int personality, int trans_sw );
extern void TRCBmp_WriteScoreListWin( TR_CARD_WORK *wk, int scrol_point, int trans_sw );


#endif //_TRCARD_BMP_H_

