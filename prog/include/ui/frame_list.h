//============================================================================================
/**
 * @file		frame_list.h
 * @brief		�a�f�t���[���S�̂��g�p�������X�g����
 * @author	Hiroyuki Nakamura
 * @date		10.01.23
 *
 *	���W���[�����FFRAMELIST
 *
 *	�E�P��ʂɂ��A�a�f�P�ʂ��L���܂��i�X�N���[�����邽�߁j
 *	�E������BMPWIN���m�ۂ��Ă��邽�߁ABMPWIN�̏�������Ƀ��X�g���쐬���Ă�������
 */
//============================================================================================
#pragma	once

#include "print/printsys.h"


//============================================================================================
//	�萔��`
//============================================================================================

// ���X�g���쌋��
#define	FRAMELIST_RET_CURSOR_ON		( 0xfffffff6 )		// �J�[�\���\��
#define	FRAMELIST_RET_MOVE				( 0xfffffff7 )		// �J�[�\���ړ�
#define	FRAMELIST_RET_SCROLL			( 0xfffffff8 )		// �X�N���[���ʏ�
#define	FRAMELIST_RET_RAIL				( 0xfffffff9 )		// ���[���X�N���[��
#define	FRAMELIST_RET_SLIDE				( 0xfffffffa )		// �X���C�h�X�N���[��
#define	FRAMELIST_RET_PAGE_UP			( 0xfffffffb )		// �P�y�[�W���
#define	FRAMELIST_RET_PAGE_DOWN		( 0xfffffffc )		// �P�y�[�W����
#define	FRAMELIST_RET_JUMP_TOP		( 0xfffffffd )		// ���X�g�ŏ㕔�փW�����v
#define	FRAMELIST_RET_JUMP_BOTTOM	( 0xfffffffe )		// ���X�g�ŉ����փW�����v
#define	FRAMELIST_RET_NONE				( 0xffffffff )		// ����Ȃ�

#define	FRAMELIST_SUB_BG_NONE	( 0xff )		// ���ʂa�f���g�p

#define	FRAMELIST_SPEED_MAX		( 6 )				// �X�N���[�����x�̒i�K

// �^�b�`�p�����[�^
typedef enum {
	FRAMELIST_TOUCH_PARAM_SLIDE = 0,					// �X���C�h

	FRAMELIST_TOUCH_PARAM_RAIL,								// ���[��

	FRAMELIST_TOUCH_PARAM_UP,									// ��ړ�
	FRAMELIST_TOUCH_PARAM_DOWN,								// ���ړ�

	FRAMELIST_TOUCH_PARAM_PAGE_UP,						// �y�[�W��ړ�
	FRAMELIST_TOUCH_PARAM_PAGE_DOWN,					// �y�[�W���ړ�

	FRAMELIST_TOUCH_PARAM_LIST_TOP,						// ���X�g�ŏ�ʂ�
	FRAMELIST_TOUCH_PARAM_LIST_BOTTOM,				// ���X�g�ŉ��ʂ�

//	FRAMELIST_TOUCH_PARAM_ITEM = 0x80000000,	// ����
	FRAMELIST_TOUCH_PARAM_ITEM,								// ����

}FRAMELIST_TOUCH_PARAM;

// �R�[���o�b�N�֐�
typedef struct {
	// ���[�N, ���ڈʒu, PRINT_UTIL, �\���x���W, �`���
	void (*draw)(void*,u32,PRINT_UTIL*,s16,BOOL);		// �`�揈��
	// ���[�N, ���X�g�ʒu, �t���O�i��������FALSE�j
	void (*move)(void*,u32,BOOL);										// �ړ�����
	// ���[�N, �X�N���[���l
	void (*scroll)(void*,s8);												// �X�N���[����
}FRAMELIST_CALLBACK;

// �^�b�`�f�[�^
/*�@�� �^�b�`�f�[�^�́A���ڕ������ŏ��ɒǉ����Ă�������
		{ { 0,15,128,255}, FRAMELIST_TOUCH_PARAM_ITEM or FRAMELIST_TOUCH_PARAM_SLIDE },	// ���ڂP
		{ {16,31,128,255}, FRAMELIST_TOUCH_PARAM_ITEM or FRAMELIST_TOUCH_PARAM_SLIDE },	// ���ڂQ
		{ {32,47,128,255}, FRAMELIST_TOUCH_PARAM_ITEM or FRAMELIST_TOUCH_PARAM_SLIDE },	// ���ڂR
		{ {48,63,128,255}, FRAMELIST_TOUCH_PARAM_ITEM or FRAMELIST_TOUCH_PARAM_SLIDE },	// ���ڂS
				:
				:
		{ {....}, FRAMELIST_TOUCH_PARAM_RAIL },					// ���[��
		{ {....}, FRAMELIST_TOUCH_PARAM_PAGE_UP },			// �y�[�W�A�b�v
		{ {....}, FRAMELIST_TOUCH_PARAM_PAGE_DOWN },		// �y�[�W�_�E��

		{ {GFL_UI_TP_HIT_END,0,0,0}, 0 },								// �f�[�^�I��
*/
typedef struct {
	GFL_UI_TP_HITTBL	tbl;				// �͈̓f�[�^
	FRAMELIST_TOUCH_PARAM	prm;		// �p�����[�^
}FRAMELIST_TOUCH_DATA;

// �w�b�_�[�f�[�^
typedef struct {
	u8	mainBG;								// ����ʂa�f
	u8	subBG;								// ���ʂa�f ( FRAMELIST_SUB_BG_NONE = ���g�p )

	s8	itemPosX;							// ���ڃt���[���\���J�n�w���W
	s8	itemPosY;							// ���ڃt���[���\���J�n�x���W
	u8	itemSizX;							// ���ڃt���[���\���w�T�C�Y
	u8	itemSizY;							// ���ڃt���[���\���x�T�C�Y

	u8	bmpPosX;							// �t���[�����ɕ\������BMPWIN�̕\���w���W
	u8	bmpPosY;							// �t���[�����ɕ\������BMPWIN�̕\���x���W
	u8	bmpSizX;							// �t���[�����ɕ\������BMPWIN�̕\���w�T�C�Y
	u8	bmpSizY;							// �t���[�����ɕ\������BMPWIN�̕\���x�T�C�Y
	u8	bmpPal;								// �t���[�����ɕ\������BMPWIN�̃p���b�g

	u8	scrollSpeed[FRAMELIST_SPEED_MAX];		// �X�N���[�����x [0] = �ő� ��itemSizY*8������؂��l�ł��邱�ƁI

	u8	selPal;								// �I�����ڂ̃p���b�g

	u8	barSize;							// �X�N���[���o�[�̂x�T�C�Y

	u16	itemMax;							// ���ړo�^��
	u16	graMax;								// �w�i�o�^��

	u8	initPos;							// �����J�[�\���ʒu
	u8	posMax;								// �J�[�\���ړ��͈�
	u16	initScroll;						// �����X�N���[���l

	const FRAMELIST_TOUCH_DATA * touch;	// �^�b�`�f�[�^

	const FRAMELIST_CALLBACK * cbFunc;	// �R�[���o�b�N�֐�
	void * cbWork;											// �R�[���o�b�N�֐��ɓn����郏�[�N

}FRAMELIST_HEADER;

typedef struct _FRAMELIST_WORK	FRAMELIST_WORK;		// �a�f�t���[�����X�g���[�N


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�t���[�����X�g�쐬
 *
 * @param		hed			�w�b�_�[�f�[�^
 * @param		heapID	�q�[�v�h�c
 *
 * @return	���[�N
 */
//--------------------------------------------------------------------------------------------
extern FRAMELIST_WORK * FRAMELIST_Create( FRAMELIST_HEADER * hed, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�t���[�����X�g�폜
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void FRAMELIST_Exit( FRAMELIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		���ڒǉ�
 *
 * @param		wk			���[�N
 * @param		type		�w�i�ԍ�
 * @param		param		�p�����[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void FRAMELIST_AddItem( FRAMELIST_WORK * wk, u32 type, u32 param );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�w�i�O���t�B�b�N�ǂݍ���
 *
 * @param		wk				���[�N
 * @param		ah				�A�[�J�C�u�n���h��
 * @param		dataIdx		�f�[�^�C���f�b�N�X
 * @param		comp			���k�t���O TRUE = ���k
 * @param		frameNum	�w�i�ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void FRAMELIST_LoadFrameGraphicAH( FRAMELIST_WORK * wk, ARCHANDLE * ah, u32 dataIdx, BOOL comp, u32 frameNum );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���p���b�g�A�j���p�f�[�^�ǂݍ���
 *
 * @param		wk				���[�N
 * @param		ah				�A�[�J�C�u�n���h��
 * @param		dataIdx		�f�[�^�C���f�b�N�X
 * @param		startPal	�p���b�g�P 0�`15
 * @param		endPal		�p���b�g�Q 0�`15
 *
 * @return	���[�N
 */
//--------------------------------------------------------------------------------------------
extern void FRAMELIST_LoadBlinkPalette( FRAMELIST_WORK * wk, ARCHANDLE * ah, u32 dataIdx, u32 startPal, u32 endPal );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�t���[�����X�g�����`��
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern BOOL FRAMELIST_Init( FRAMELIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�t���[�����X�g���C������
 *
 * @param		wk		���[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 FRAMELIST_Main( FRAMELIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X�N���[���o�[�̕\���x���W���擾
 *
 * @param		wk			���[�N
 * @param		barSY		�o�[�̂x�T�C�Y
 *
 * @return	�x���W
 */
//--------------------------------------------------------------------------------------------
extern u32 FRAMELIST_GetScrollBarPY( FRAMELIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �w��ʒu�̍��ڂ̃p���b�g��ύX	
 *
 * @param		wk		���[�N
 * @param		pos		�J�[�\���ʒu
 * @param		pal		�p���b�g�ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void FRAMELIST_ChangePosPalette( FRAMELIST_WORK * wk, u16 pos, u16 pal );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�v�����g�L���[�擾
 *
 * @param		wk				���[�N
 *
 * @return	�v�����g�L���[
 */
//--------------------------------------------------------------------------------------------
extern PRINT_QUE * FRAMELIST_GetPrintQue( FRAMELIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		���ڃp�����[�^�擾
 *
 * @param		wk				���[�N
 * @param		itemIdx		���ڃC���f�b�N�X
 *
 * @return	���ڃp�����[�^
 */
//--------------------------------------------------------------------------------------------
extern u32 FRAMELIST_GetItemParam( FRAMELIST_WORK * wk, u32 itemIdx );

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g�ʒu�擾
 *
 * @param		wk				���[�N
 *
 * @return	���X�g�ʒu
 */
//--------------------------------------------------------------------------------------------
extern u32 FRAMELIST_GetListPos( FRAMELIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ʒu�擾
 *
 * @param		wk				���[�N
 *
 * @return	���ڃp�����[�^
 */
//--------------------------------------------------------------------------------------------
extern u32 FRAMELIST_GetCursorPos( FRAMELIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g�X�N���[���J�E���^���擾
 *
 * @param		wk		���[�N
 *
 * @return	�X�N���[���J�E���^
 */
//--------------------------------------------------------------------------------------------
extern u32 FRAMELIST_GetScrollCount( FRAMELIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g�����ɃX�N���[���ł��邩
 *
 * @param		wk		���[�N
 *
 * @retval	"TRUE = ��"
 * @retval	"FALSE = �s��"
 */
//--------------------------------------------------------------------------------------------
extern BOOL FRAMELIST_CheckScrollMax( FRAMELIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ʒu��ݒ�
 *
 * @param		wk					���[�N
 * @param		pos					�J�[�\���ʒu
 *
 * @return	none
 *
 *	�R�[���o�b�N[move]���Ă΂�܂�
 */
//--------------------------------------------------------------------------------------------
extern void FRAMELIST_SetCursorPos( FRAMELIST_WORK * wk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ʒu�̃p���b�g�����点��
 *
 * @param		wk					���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void FRAMELIST_SetCursorPosPalette( FRAMELIST_WORK * wk );
