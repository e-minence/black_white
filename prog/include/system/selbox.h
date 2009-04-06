/**
 *	@file	selwin.h
 *	@brief	�I�����X�g�E�B���h�E�`��E�R���g���[���V�X�e��
 *	@author	Miyuki Iwasawa
 *	@date	07.10.10
 */

#ifndef __H_SELWIN_H__
#define __H_SELWIN_H__

//�R���p�C����ʂ����߁A��肠������`���������Ă���
#include "bmp_menulist.h"
#include "palanm.h"
#include "print/printsys.h"

#define SBOX_SELECT_NULL	(BMPMENULIST_NULL)
#define SBOX_SELECT_CANCEL	(BMPMENULIST_CANCEL)

#define SBOX_WINCGX_SIZ	(27)

typedef enum{
	SBOX_OFSTYPE_LEFT,		///<���񂹃^�C�v
	SBOX_OFSTYPE_CENTER,	///<�Z���^�����O�^�C�v
}SBOX_OFSTYPE;

//�R�[���o�b�N�֐��̌Ăяo�����[�h
typedef enum{
	SBOX_CB_MODE_INI,	//��������
	SBOX_CB_MODE_MOVE,	//�J�[�\���ړ���
	SBOX_CB_MODE_DECIDE,		//�I��������
	SBOX_CB_MODE_CANCEL,	//�L�����Z��
}SBOX_CB_MODE;

///<�Z���N�g�{�b�N�XBmpWin�̈�
typedef struct _SELBOX_BMPWIN{
	u8	frm_no;	///<�t���[���i���o�[
	u8	siz_x;	///<�L�����N�^�̈�T�C�YX
	u8	siz_y;	///<�L�����N�^�̈�T�C�YY
	u8	chrofs;	///<�g�p�L�����N�^�I�t�Z�b�g
	u16	palid;	///<�g�p�p���b�gID
	u16	dmy;
}SELBOX_BMPWIN;

///<�Z���N�g�{�b�N�X�|�W�V�����f�[�^
typedef struct _SELBOX_POS{
	u8	pos_x;
	u8	pos_y;
}SELBOX_POS;

///�Z���N�g�{�b�N�X�@�w�b�_�f�[�^�\����
typedef struct _SELBOX_HEAD_PRM{
	u8	loop_f:1;		///<���[�v����Ȃ�TRUE
	u8	ofs_type:7;	///<����or�Z���^�����O(SBOX_OFSTYPE_LEFT or SXBOX_OFSTYPE_CENTER)
	
	u8	ofs_x;	///<���ڕ\���I�t�Z�b�gX���W(dot)
	u8	frm;	///<�t���[��No
	u8	pal;	///<�p���b�gID
	
	u8	bg_pri;	///<BG�v���C�I���e�B
	u8	s_pri;	///<�\�t�g�E�F�A�v���C�I���e�B

	u16	scgx;	///<������̈�cgx(�L�����P��)
	u16	fcgx;	///<�t���[���̈�cgx(�L�����P��)
	u16	cgx_siz;	///<��L����cgx�̈�T�C�Y(�L�����P��)
	
	//��(�������ŃE�B���h�E�T�C�Y���ς��̂ŁA�̈�I�[�o�[�`�F�b�N�p�Ɏg���Ă����T�C�Y��o�^���Ă���) 
}SELBOX_HEAD_PRM;

///�Z���N�g�{�b�N�X�@�w�b�_�\����
typedef struct _SELBOX_HEADER{
	SELBOX_HEAD_PRM prm;		///<�g�p����VRAM�̈��p���b�gNo�Ȃǂ̃p�����[�^���i�[����\���̌^
	const BMPLIST_DATA*	list;	///<�\�������f�[�^�|�C���^(BMPLIST,BMPMENU�Ƌ���)

	GFL_FONT	*fontHandle;
	u8	count;	///<�\�����ڐ�
}SELBOX_HEADER;


#define SBOXOAM_RESMAX	(4)
#define SBOXOAM_ACTMAX	(4)

typedef enum{
	SBOXOAM_DRAW_MAIN = NNS_G2D_VRAM_TYPE_2DMAIN,
	SBOXOAM_DRAW_SUB = NNS_G2D_VRAM_TYPE_2DSUB,
	SBOXOAM_DRAW_BOTH = NNS_G2D_VRAM_TYPE_MAX
}SBOXOAM_DRAW;

typedef enum{
	SBOXOAM_VMAP_1D32 = GX_OBJVRAMMODE_CHAR_1D_32K,
	SBOXOAM_VMAP_1D64 = GX_OBJVRAMMODE_CHAR_1D_64K,
	SBOXOAM_VMAP_1D128 = GX_OBJVRAMMODE_CHAR_1D_128K,
	SBOXOAM_VMAP_1D256 = GX_OBJVRAMMODE_CHAR_1D_256K,
}SBOXOAM_VMAP;

/**
 *	�Z���N�g�{�b�N�X�@�V�X�e�����[�N
 */
typedef struct _SELBOX_SYS{
	int heapID;

	NNSG2dCharacterData* p_char;
	NNSG2dPaletteData* p_pltt;
	void* char_buf;	
	void* pal_buf;	
	
	PALETTE_FADE_PTR palASys;
}SELBOX_SYS;

//�\����
typedef struct _SELBOX_WORK SELBOX_WORK;

//�R�[���o�b�N�֐��^��`
typedef void (*SELBOX_CB_FUNC)(SELBOX_WORK* sbox,u8 cur_pos,void* work,SBOX_CB_MODE mode);

///<�Z���N�g�{�b�N�X
struct _SELBOX_WORK{
	SELBOX_SYS*	sys_wk;	///<�V�X�e�����[�N�ւ̎Q�ƃ|�C���^
	SELBOX_HEADER	hed;	///<�w�b�_�[�f�[�^

	GFL_BMPWIN 			**win;	///<BmpWin�f�[�^
	GFL_UI_TP_HITTBL	*tbl;	///<Hit�e�[�u��

	u8	cp;		///<�J�[�\���|�C���g
	u8	seq;	///<�V�[�P���X
	u8	width;	///<�E�B���h�E��
	u8	se_manual_f:1;	///<SE�̓R�[���o�b�N���g���Ď����ōĐ�����ꍇ
	u8	key_mode:7;	///<�L�[���샂�[�hor�^�b�`���샂�[�h

	u8	px;		///<�|�W�V����X
	u8	py;		///<�|�W�V����Y
	u8 ret;	///<���^�[���l�i�[���[�N
	u8	wait;	///<�I���E�F�C�g�J�E���^�[
	u32 heapID;	///<�������A���P�[�g�ɗ��p����ID

	SELBOX_CB_FUNC cb_func;	//�R�[���o�b�N
	void*			cb_work;	//�R�[���o�b�N�Ɉ����n���郏�[�N
	
	PRINT_QUE	*printQue;
	PRINT_UTIL	*printUtil;
};


/**
 *	@brief	�Z���N�g�{�b�N�X�@�V�X�e�����[�N�m��
 *
 *	@param	heapID	���p�q�[�v
 *	@param	palASys	�p���b�g�A�j�����[�N�̎Q�ƃ|�C���^�BNULL�w���
 */
extern SELBOX_SYS*	SelectBoxSys_AllocWork( int heapID, PALETTE_FADE_PTR palASys );

/**
 *	@brief	�Z���N�g�{�b�N�X�@�V�X�e�����[�N���
 */
extern void SelectBoxSys_Free(SELBOX_SYS* wk);

/**
 *	@brief	�Z���N�g�{�b�N�X�@�Z�b�g
 *
 *	@param	header	�Z���N�g�{�b�N�X�w�b�_�|�C���^
 * 	@param	key_mode	���샂�[�h(APP_END_KEY or APP_END_TOUCH)
 *	@param	x		�`��J�n�ʒux(char)
 *	@param	y		�`��J�n�ʒuy(char)
 *	@param	width	�E�B���h�E��(char) 0���w�肵���ꍇ�A������T�C�Y�ˑ�
 *	@param	cursor	�����J�[�\���|�W�V����
 
 *	@li	���[�N�������Alloc���邽�߁A�K��SelectBoxExit�ŉ�����Ă�������
 */
extern SELBOX_WORK* SelectBoxSet(SELBOX_SYS* sp,const SELBOX_HEADER* header,
				u8 key_mode,u8 x,u8 y,u8 width,u8 cursor);

/**
 *	@brief	�Z���N�g�{�b�N�X�@�Z�b�gEx(�R�[���o�b�N�����ASE�������Ŗ炷���ǂ����I�ׂ�)
 *
 *	@param	header	�Z���N�g�{�b�N�X�w�b�_�|�C���^
 *	@param	key_mode	���샂�[�h(APP_END_KEY or APP_END_TOUCH)
 *	@param	x		�`��J�n�ʒux(char)
 *	@param	y		�`��J�n�ʒuy(char)
 *	@param	width	�E�B���h�E��(char) 0���w�肵���ꍇ�A������T�C�Y�ˑ�
 *	@param	cursor	�����J�[�\���|�W�V����
 *	@param	cb_func	�R�[���o�b�N�֐��ւ̃|�C���^
 *	@param	cb_work	�R�[���o�b�N�֐��Ɉ����n���A�C�ӂ̃��[�N�|�C���^
 *	@param	se_manual_f	TRUE��n����SE�����O�Ŗ炷�K�v������
 
 *	@li	���[�N�������Alloc���邽�߁A�K��SelectBoxExit�ŉ�����Ă�������
 */
extern SELBOX_WORK* SelectBoxSetEx(SELBOX_SYS* sp,const SELBOX_HEADER* header,u8 key_mode,
	u8 x,u8 y,u8 width,u8 cursor,SELBOX_CB_FUNC cb_func,void* cb_work,BOOL se_manual_f);

/**
 *	@brief	�Z���N�g�{�b�N�X�@�I�����̃L�[�^�b�`�X�e�[�^�X���擾
 *
 *	@retval APP_END_KEY
 *	@retval APP_END_TOUCH
 *
 *	SelectBoxExit()�̃R�[���O�ɌĂяo���Ă�������
 */
extern int SelectBoxGetKTStatus(SELBOX_WORK* wk);

/**
 *	@brief	�Z���N�g�{�b�N�X�@�I���E�������
 */
extern void SelectBoxExit(SELBOX_WORK* wk);

/**
 *	@brief	�Z���N�g�{�b�N�X�@�R���g���[�����C��
 *
 * @retval	"param = �I���p�����[�^"
 * @retval	"SBOX_SELECT_NULL = �I��"
 * @retval	"SBOX_SELECT_CANCEL	= �L�����Z��(�a�{�^��)"
 */
extern u32 SelectBoxMain(SELBOX_WORK* wk);

#endif	//__H_SELWIN_H__
