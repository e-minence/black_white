//============================================================================================
/**
 * @file	worldtrade_input.h
 * @brief	���E�����������̓E�C���h�E�����w�b�_�t�@�C��
 * @author	Akito Mori
 * @date	08.11.01
 */
//============================================================================================
#ifndef __WORLDTRADE_INPUT_H__
#define __WORLDTRADE_INPUT_H__

#include "worldtrade_adapter.h"

// �E�u�|�P����������ʁv�Ɓu�|�P�����a���������́v�ɓ����Ɏg��
// �@�|�P�������E���ʁE���x�������E�������̓V�X�e���B
// �E�w���BG�ʂ���g�p���A�����ʂ�BMPWIN�Ńe�L�X�g���`�悷��
// �E�J�[�\�������GFL_CLACT_WK_Create���Ă����K�v������B�i���Ă���Init�ɓn���j

// ���̓��[�h�̒�`
enum{
	MODE_POKEMON_NAME,	// �|�P�������O�w��
	MODE_SEX,			// ���ʏ����w��
	MODE_LEVEL,			// ���x�������w��
	MODE_NATION,		// ����

	// �����܂ł̓��[�h�w��Ƃ��Ă��g���B
	// �������牺�͓����̓��̓��[�h�p�̒�`

	MODE_HEADWORD_1,	// �������q���w��
	MODE_HEADWORD_2,	// �������ꉹ�w��
	MODE_NATION_HEAD1,	// �����������w��
	MODE_NATION_HEAD2,	// �����������w��
};


// ���̓V�X�e�����g���V�`���G�[�V����
enum{
	SITUATION_DEPOSIT=0,		// �a���鎞
	SITUATION_SEARCH,			// �������鎞
};

// �X�N���[���ɏ������ރ{�b�N�X��`
enum{
	SELECT_BOX_N_LETTER=0,
	SELECT_BOX_R_LETTER,
	SELECT_BOX_N_TEXT,
	SELECT_BOX_R_TEXT,
	SELECT_BOX_N_NATION,
	SELECT_BOX_R_NATION,
	SELECT_BOX_END,
};

// BMPWIN���w�肷�邽�߂̒�`
enum{
	BMPWIN_HEADWORD1_WIN0=0,
	BMPWIN_HEADWORD1_WIN1,
	BMPWIN_HEADWORD1_WIN2,
	BMPWIN_HEADWORD1_WIN3,
	BMPWIN_HEADWORD1_WIN4,
	BMPWIN_HEADWORD1_WIN5,
	BMPWIN_HEADWORD1_WIN6,
	BMPWIN_HEADWORD1_WIN7,
	BMPWIN_HEADWORD1_WIN8,
	BMPWIN_HEADWORD1_WIN9,
	BMPWIN_SELECT_END_WIN,
	BMPWIN_NONE_SELECT_WIN,
	

	BMPWIN_HEADWORD2_WIN0=0,
	BMPWIN_HEADWORD2_WIN1,
	BMPWIN_HEADWORD2_WIN2,
	BMPWIN_HEADWORD2_WIN3,
	BMPWIN_HEADWORD2_WIN4,
	BMPWIN_HEADWORD2_WIN5,
	
	BMPWIN_POKENAME_WIN0=0,
	BMPWIN_POKENAME_WIN1,
	BMPWIN_POKENAME_WIN2,
	BMPWIN_POKENAME_WIN3,
	BMPWIN_POKENAME_WIN4,
	BMPWIN_POKENAME_WIN5,
	BMPWIN_POKENAME_PAGE_WIN,
	
	BMPWIN_NATION_WIN0=0,
	BMPWIN_NATION_WIN1,
	BMPWIN_NATION_WIN2,
	BMPWIN_NATION_WIN3,
	BMPWIN_NATION_WIN4,
	BMPWIN_NATION_PAGE_WIN,
	
	
};


// �E�C���h�E��������̃p���b�g�w��i�E�C���h�E�Ɠ����j
#define WORLDTRADE_INPUT_PAL	( 1 )

// �E�C���h�E�`��ʂ��e�L�X�g��z�u�ł���X�^�[�g�ʒu
#define INPUT_BMPWIN_OFFSET		( 16*3 )

// ���̓��[�h����BMPWIN�̎g�p��
#define MODE_HEADWORD1_WIN_NUM			( 10 )
#define MODE_HEADWORD2_WIN_NUM			(  6 )
#define MODE_POKENAME_WIN_NUM			(  4 )
#define MODE_PAGE_WIN_NUM				(  1 ) 
#define MODE_SEX_WIN_NUM				(  3 )
#define MODE_LEVEL_WIN_NUM				(  4 )
#define MODE_NATION_HEADWORD1_WIN_NUM	( 10 )
#define MODE_NATION_WIN_NUM				(  5 )

#define SEE_CHECK_MAX                   (50)    //�܏\����

typedef struct{
	GFL_BMPWIN	**MenuBmp;				// ���j���[�pBMPWIN�\���̊i�[�z��
	GFL_BMPWIN	**BackBmp;				// �u���ǂ�v�\���pBMPWIN�\���̃|�C���^
	GFL_CLWK*	CursorAct;				// �J�[�\���A�N�^�[�|�C���^
	GFL_CLWK*	ArrowAct[2];			// ���E���A�N�^�[�|�C���^
	GFL_CLWK*	SearchCursorAct;		// ������ʗp�J�[�\���A�N�^�[�|�C���^�i�a�����ʂł͎g��Ȃ��j

	GFL_MSGDATA *MsgManager;			// ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[
	GFL_MSGDATA *MonsNameManager;		// �|�P���������b�Z�[�W�f�[�^�}�l�[�W���[
	GFL_MSGDATA *CountryNameManager;	// ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[
	ZUKAN_SAVEDATA		*Zukan;					// ��������f�[�^
	u8				*SinouTable;			// �V���I�E�|�P�������ǂ������t���O�Ŋi�[����Ă���e�[�u��
	const CONFIG	*config;				//	�R���t�B�O�f�[�^


}WORLDTRADE_INPUT_HEADER;

typedef struct{
	GFL_BMPWIN	**MenuBmp;	            // ���j���[�pBMPWIN�\���̊i�[�z��
	GFL_BMPWIN	**BackBmp;               // �u���ǂ�v�\���pBMPWIN�\���̃|�C���^
	GFL_CLWK*	CursorAct;				// �I���J�[�\��
	GFL_CLWK*  ArrowAct[2];			// ���E�y�[�W�J�[�\��
	GFL_CLWK*	SearchCursorAct;		// ������ʗp�J�[�\���A�N�^�[�|�C���^�i�a�����ʂł͎g��Ȃ��j

	BGWINFRM_WORK   *BgWinFrm;				// BG�X�N���[�����H���[�`�����[�N
	ZUKAN_SAVEDATA		*zukan;					// ��������f�[�^	

	GFL_MSGDATA *MsgManager;			// ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[
	GFL_MSGDATA *MonsNameManager;		// �|�P���������b�Z�[�W�f�[�^�}�l�[�W���[
	GFL_MSGDATA *CountryNameManager;	// ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[

	u8*				SinouTable;				// �V���I�E�|�P�������ǂ������t���O�Ŋi�[����Ă���e�[�u��
	BMPLIST_DATA	*NameList;				// �|�P�������E���E�e�������i�[����e�[�u���|�C���^
	NUMFONT			*NumFontSys;			// �����t�H���g�`��V�X�e���\����

	s16		Head1;			// �������i�q���j
	s16		Head2;			// �������i�ꉹ�j
	s16		Poke;			// �|�P����NO
	s16		Nation;			// ��NO
	s8		Sex;			// ����No
	s8		Level;			// �~������ׂ�
	int		listpos;		// �J�[�\���ʒu
	int		seq;			// ���͗p�V�[�P���X
	int		next;			// ����V�[�P���X
	int		BgFrame;		// �`��BG�t���[��

	s16		type;			// ���̓^�C�v
	s16		page;			// ���݂̕\���y�[�W�i�S���ڈȏ�̏ꍇ�j
	int		listMax;		// �\�����X�g�̍��ڐ�
	int		Situation;		// ���p����V�`���G�[�V�����i��������H�����H�j

    u8     see_check[SEE_CHECK_MAX];   // �܏\���̃��X�g�̒��Ƀ|�P������1�C�ȏ�܂܂�Ă��邩
    u8     listpos_backup_x;   // X�ʒu�L���ׂ̈̃o�b�N�A�b�v
    u8      padding;

		WT_PRINT	print;
}WORLDTRADE_INPUT_WORK;

extern void WorldTrade_Input_SysPrint( BGWINFRM_WORK *wfwk, GFL_BMPWIN *win, STRBUF *strbuf, int x, PRINTSYS_LSB color, WT_PRINT *print );

WORLDTRADE_INPUT_WORK * WorldTrade_Input_Init( WORLDTRADE_INPUT_HEADER *wih, int frame, int situation );
extern u32 WorldTrade_Input_Main( WORLDTRADE_INPUT_WORK *wk );
extern void WorldTrade_Input_Exit( WORLDTRADE_INPUT_WORK *wk );
extern void WorldTrade_Input_Start( WORLDTRADE_INPUT_WORK *wk, int type );


#endif
