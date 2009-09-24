//============================================================================================
/**
 * @file	b_bag_anm.h
 * @brief	�퓬�p�o�b�O��� �{�^������
 * @author	Hiroyuki Nakamura
 * @date	09.08.26
 */
//============================================================================================
#ifndef B_BAG_ANM_H
#define B_BAG_ANM_H


//============================================================================================
//	�萔��`
//============================================================================================
// �{�^��ID
enum {
	BBAG_BTNANM_PAGE1 = 0,
	BBAG_BTNANM_PAGE2,
	BBAG_BTNANM_PAGE3,
	BBAG_BTNANM_PAGE4,
	BBAG_BTNANM_LAST,
	BBAG_BTNANM_RET1,

	BBAG_BTNANM_ITEM1,
	BBAG_BTNANM_ITEM2,
	BBAG_BTNANM_ITEM3,
	BBAG_BTNANM_ITEM4,
	BBAG_BTNANM_ITEM5,
	BBAG_BTNANM_ITEM6,
	BBAG_BTNANM_UP,
	BBAG_BTNANM_DOWN,
	BBAG_BTNANM_RET2,

	BBAG_BTNANM_USE,
	BBAG_BTNANM_RET3,
/*
	BBAG_BTNANM_PAGE1 = 0,
	BBAG_BTNANM_PAGE2,
	BBAG_BTNANM_PAGE3,
	BBAG_BTNANM_PAGE4,
	BBAG_BTNANM_PAGE5,
	BBAG_BTNANM_LAST,
	BBAG_BTNANM_RET1,

	BBAG_BTNANM_ITEM1,
	BBAG_BTNANM_ITEM2,
	BBAG_BTNANM_ITEM3,
	BBAG_BTNANM_ITEM4,
	BBAG_BTNANM_ITEM5,
	BBAG_BTNANM_ITEM6,
	BBAG_BTNANM_NEXT,
	BBAG_BTNANM_RET2,

	BBAG_BTNANM_USE,
	BBAG_BTNANM_RET3,
*/
};

// �{�^��
enum {
	BBAG_BGWF_POCKET01 = 0,
	BBAG_BGWF_POCKET02,
	BBAG_BGWF_POCKET03,
	BBAG_BGWF_POCKET04,

	BBAG_BGWF_ITEM01,
	BBAG_BGWF_ITEM02,
	BBAG_BGWF_ITEM03,
	BBAG_BGWF_ITEM04,
	BBAG_BGWF_ITEM05,
	BBAG_BGWF_ITEM06,

	BBAG_BGWF_RETURN,
	BBAG_BGWF_LEFT,
	BBAG_BGWF_RIGHT,

	BBAG_BGWF_USE,

	BBAG_BGWF_MAX,
};


// �{�^���A�j�����[�h
enum {
//	BBAG_BTNANM_MODE_CHG = 0,	// �������玟��
	BBAG_BTNANM_MODE_END = 0,	// �ĉ����\
};


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �{�^���쐬
 *
 * @param	wk		�퓬�o�b�O�̃��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BBAGANM_ButtonInit( BBAG_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �{�^���폜
 *
 * @param	wk		�퓬�o�b�O�̃��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BBAGANM_ButtonExit( BBAG_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �{�^��BG������
 *
 * @param	wk		�퓬�o�b�O�̃��[�N
 * @param	page	�y�[�W�ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BBAGANM_PageButtonPut( BBAG_WORK * wk, u8 page );

//--------------------------------------------------------------------------------------------
/**
 * �{�^���A�j��������
 *
 * @param	wk		�퓬�o�b�O�̃��[�N
 * @param	id		�{�^��ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BBAGANM_ButtonAnmInit( BBAG_WORK * wk, u8 id );

//--------------------------------------------------------------------------------------------
/**
 * �{�^���A�j�����C��
 *
 * @param	wk		�퓬�o�b�O�̃��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BBAGANM_ButtonAnmMain( BBAG_WORK * wk );




#if 0
//--------------------------------------------------------------------------------------------
/**
 * �{�^���X�N���[���쐬
 *
 * @param	wk		�퓬�o�b�O�̃��[�N
 * @param	scrn	�O���t�B�b�N�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BattleBag_ButtonScreenMake( BBAG_WORK * wk, u16 * scrn );

//--------------------------------------------------------------------------------------------
/**
 * �{�^���A�j��������
 *
 * @param	wk		�퓬�o�b�O�̃��[�N
 * @param	id		�{�^��ID
 * @param	mode	�A�j�����[�h
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BattleBag_ButtonAnmInit( BBAG_WORK * wk, u8 id, u8 mode );

//--------------------------------------------------------------------------------------------
/**
 * �{�^���A�j�����C��
 *
 * @param	wk		�퓬�o�b�O�̃��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BattleBag_ButtonAnmMain( BBAG_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �{�^��BG������
 *
 * @param	wk		�퓬�o�b�O�̃��[�N
 * @param	page	�y�[�W�ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BattleBag_ButtonPageScreenInit( BBAG_WORK * wk, u8 page );
#endif


#endif	// B_BAG_ANM_H
