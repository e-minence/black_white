#ifndef _TRCARD_SYS_H_
#define _TRCARD_SYS_H_

#include "system/bmp_menu.h"
#include "system/touch_subwindow.h"
#include "app/trainer_card.h"
#include "trcard_obj.h"
#include "trcard_snd.h"

#define TRC_USE_SND (0)
#define TRC_FRAME_RATE (60)

#define TRC_SCREEN_WIDTH	(32)
#define TR_CARD_WIN_MAX		(14)
#define TR_CARD_PARAM_MAX	(11)
#define TR_CPRM_STR_MAX		(14)

#define TR_CARD_BADGE_LV_MAX	(4)		//�o�b�W�̍ő僌�x��
#define TR_BADGE_NUM_MAX	(16)

//�p���b�g�g�p���X�g
enum{
	CASE_BD_PAL = 0,
	YNWIN_PAL = 11,	//2�{�g��
	TALKWIN_PAL = 13,
	SYS_FONT_PAL = 14,
	TR_FONT_PAL = 15,
	BACK_FONT_PAL = 15,
};

typedef struct TR_SCRUCH_SND_tag
{
	s8 OldDirX;
	s8 OldDirY;
	s8 DirX;
	s8 DirY;
	u8 Snd;
}TR_SCRUCH_SND;

typedef struct TR_CARD_WORK_tag
{
	HEAPID heapId;
	GFL_BMPWIN	*win[TR_CARD_WIN_MAX];			// BMP�E�B���h�E�f�[�^�i�ʏ�j

	TRCARD_CALL_PARAM* tcp;	///<�J�[�h�Ăяo���p�����[�^
	TR_CARD_DATA *TrCardData;
	u8 TrSignData[SIGN_SIZE_X*SIGN_SIZE_Y*64];	//�]������̂łS�o�C�g���E�̈ʒu�ɒu������
	void *TrArcData;							//�g���[�i�[�O���t�B�b�N�A�[�J�C�u�f�[�^
	NNSG2dCharacterData* TrCharData;			//�g���[�i�[�L�����f�[�^�|�C���^
	BOOL is_back;
	BOOL brushOK;
	
	fx32 CardScaleX;
	fx32 CardScaleY;
	fx32 CoverScaleY;
	int	 RotateCount;

//	int touch;
	int key_mode;
	BOOL rc_cover;
	int BeforeX;
	int BeforeY;
	TR_CARD_OBJ_WORK ObjWork;
//	int RevSpeed;
	int Scroll;
	BOOL ButtonPushFlg;
	SND_BADGE_WORK SndBadgeWork;
	TR_SCRUCH_SND ScruchSnd;
	
	void *TrScrnArcData;
	NNSG2dScreenData* ScrnData;
	void *pScrnBCase;
	NNSG2dScreenData* pSBCase;
	
	TOUCH_SW_SYS* ynbtn_wk;		// YesNo�I���{�^�����[�N
	GFL_TCB	*vblankFunc;
	GFL_TCBLSYS *vblankTcblSys;

	GFL_FONT	*fontHandle;
	GFL_MSGDATA* msgMan;
	
	STRBUF	*PlayTimeBuf;		//�v���C���ԗp������o�b�t�@
	STRBUF	*SecBuf;			//�v���C���ԃR�����p������o�b�t�@
	STRBUF	*TmpBuf;			//�e���|����������
	STRBUF	*DigitBuf;			//�J�[�h���l�p�����[�^�W�J�p������o�b�t�@
	STRBUF	*ExpBuf[3];			//<�������b�Z�[�W������o�b�t�@
	STRBUF	*SignBuf[2];		//�T�C���������b�Z�[�W������o�b�t�@
	STRBUF	*CPrmBuf[TR_CPRM_STR_MAX];	//�J�[�h�p�����[�^�W�J�p������o�b�t�@

//	u8 ScruchCounter[TR_BADGE_NUM_MAX];
	u8 badge[TR_BADGE_NUM_MAX];

	u8 IsOpen;
	u8 Counter;
	u8 sub_seq;
//	u8 ButtonAnmCount;
	
	u8 AnmNum;
	u8 AnimeType;
	u8 SecCount;				//�b�\���̂��߂̃J�E���^
	u8 isClear:1;				//�a�����肵�Ă��邩�ۂ��H
	u8 isComm:1;				//�ʐM�����ۂ��H
	u8 aff_req:1;				//�A�t�B���ϊ����s���N�G�X�g
	u8 drug_f:1;				//�h���b�O�t���O
	u8 Dummy:5;					//�p�f�B���O
	
	PRINT_STREAM	*printHandle;
//	u8 msgIdx;
	u8 win_type;	///<�E�B���h�E�^�C�v
	u8 msg_spd;		///<���b�Z�[�W�X�s�[�h
	u8 scrl_ct;		///<�X�N���[���J�E���^
	u32 tp_x;
	u32 tp_y;
	u8 draw_seq;
	u8 dmy;
}TR_CARD_WORK;

extern GFL_PROC_RESULT TrCardProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
extern GFL_PROC_RESULT TrCardProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
extern GFL_PROC_RESULT TrCardProc_End( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

#endif //_TRCARD_SYS_H_


