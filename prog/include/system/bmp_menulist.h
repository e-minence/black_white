//======================================================================
/**
 * @file	bmp_list.h	
 * @brief	�r�b�g�}�b�v�a�f��ł̃��X�g�\���V�X�e��
 * @author	GAME FREAK inc.
 * @date	tetsu
 *
 * �E2008.09 DP����WB�ֈڐA
 */
//======================================================================
#ifndef __BMP_LIST_H__
#define __BMP_LIST_H__

#include <tcbl.h>
#include "print\printsys.h"
#include "print\gf_font.h"
#include "bmp_menuwork.h"

/********************************************************************/
/*                                                                  */
/*				�g�p��`											*/
/*                                                                  */
/********************************************************************/
#define	BMPMENULIST_MAX		( 4 )		// �o�^�ő吔�i�K���j

typedef struct _BMPMENULIST_WORK	BMPMENULIST_WORK;

///���X�g�w�b�_�\����
typedef struct {
	//�\�������f�[�^�|�C���^
	const BMP_MENULIST_DATA *list;
	//�J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
	void	(*call_back)(BMPMENULIST_WORK * wk,u32 param,u8 mode);
	//���\�����Ƃ̃R�[���o�b�N�֐�
	void	(*icon)(BMPMENULIST_WORK * wk,u32 param,u8 y);

	GFL_BMPWIN *win;
	
	u16		count;		//���X�g���ڐ�
	u16		line;		//�\���ő區�ڐ�
	u8		rabel_x;	//���x���\���w���W
	u8		data_x;		//���ڕ\���w���W
	u8		cursor_x;	//�J�[�\���\���w���W
	u8		line_y:4;	//�\���x���W
	u8		f_col:4;	//�\�������F
	u8		b_col:4;	//�\���w�i�F
	u8		s_col:4;	//�\�������e�F
	u16		msg_spc:3;	//�����Ԋu�w
	u16		line_spc:4;	//�����Ԋu�x
	u16		page_skip:2;//�y�[�W�X�L�b�v�^�C�v
	u16		font:6;	//�����w��(�{����u8�����ǂ���Ȃɍ��Ȃ��Ǝv���̂�)
	u16		c_disp_f:1;	//�a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)
	void * work;
	
	u16 font_size_x;		//�����T�C�YX(�h�b�g
	u16 font_size_y;		//�����T�C�YY(�h�b�g
	GFL_MSGDATA *msgdata;	//�\���Ɏg�p���郁�b�Z�[�W�o�b�t�@
//	STRBUF *strbuf;			//�\���Ɏg�p���郁�b�Z�[�W�o�b�t�@
	PRINT_UTIL *print_util; //�\���Ɏg�p����v�����g���[�e�B���e�B
	PRINT_QUE *print_que;	//�\���Ɏg�p����v�����g�L���[
	GFL_FONT *font_handle;	//�\���Ɏg�p����t�H���g�n���h��
}BMPMENULIST_HEADER;

///���X�g�w�b�_��`
#define	BMPMENULIST_NO_SKIP		(0)
#define	BMPMENULIST_LRKEY_SKIP	(1)
#define	BMPMENULIST_LRBTN_SKIP	(2)

///���X�g�A�g���r���[�g�\����
typedef struct {
	u8		pos_x;		//�J�n�w���W�I�t�Z�b�g(�L�����P��)
	u8		pos_y;		//�J�n�x���W�I�t�Z�b�g(�L�����P��)
	u8		siz_x;		//�w�T�C�Y(�L�����P��)
	u8		siz_y;		//�x�T�C�Y(�L�����P��)
	u8		pal;		//�w��p���b�g
}BMPMENULIST_ATTR;

///���X�g�p�����[�^���Ԃ�l��`
#define	BMPMENULIST_NULL	(0xffffffff)
#define	BMPMENULIST_CANCEL	(0xfffffffe)
#define	BMPMENULIST_RABEL	(0xfffffffd)

// �p�����[�^�擾�E�ύX�h�c
enum {
	BMPMENULIST_ID_CALLBACK = 0,///<�J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
	BMPMENULIST_ID_ICONFUNC,	///<���\�����Ƃ̃R�[���o�b�N�֐�
	BMPMENULIST_ID_COUNT,		///<���X�g���ڐ�
	BMPMENULIST_ID_LINE,		///<�\���ő區�ڐ�
	BMPMENULIST_ID_INDEX,		///<�g�p�r�b�g�}�b�v�h�m�c�d�w
	BMPMENULIST_ID_RABEL_X,		///<���x���\���w���W
	BMPMENULIST_ID_DATA_X,		///<���ڕ\���w���W
	BMPMENULIST_ID_CURSOR_X,	///<�J�[�\���\���w���W
	BMPMENULIST_ID_LINE_Y,		///<�\���x���W
	BMPMENULIST_ID_LINE_YBLK,   ///<�P�s���V�t�g�ʎ擾(�t�H���g�̑傫���{�x�Ԋu)
	BMPMENULIST_ID_F_COL,		///<�\�������F
	BMPMENULIST_ID_B_COL,		///<�\���w�i�F
	BMPMENULIST_ID_S_COL,		///<�\�������e�F
	BMPMENULIST_ID_MSG_SPC,		///<�����Ԋu�w
	BMPMENULIST_ID_LINE_SPC,	///<�����Ԋu�x
	BMPMENULIST_ID_PAGE_SKIP,	///<�y�[�W�X�L�b�v�^�C�v
	BMPMENULIST_ID_FONT,		///<�����w��
	BMPMENULIST_ID_C_DISP_F,	///<�a�f�J�[�\��(allow)�\���t���O
	BMPMENULIST_ID_WIN,			///<�E�B���h�E�f�[�^
	BMPMENULIST_ID_WORK			///<���[�N
};

// �ړ�����
enum {
	BMPMENULIST_MOVE_NONE = 0,	// ����Ȃ�
	BMPMENULIST_MOVE_UP,		// ��ֈړ�
	BMPMENULIST_MOVE_DOWN,		// ���ֈړ�
	BMPMENULIST_MOVE_UP_SKIP,	// ��ֈړ��i�X�L�b�v�j
	BMPMENULIST_MOVE_DOWN_SKIP,	// ���ֈړ��i�X�L�b�v�j
};

//�L�����Z�����[�h
typedef enum {
  BMPMENULIST_CANCELMODE_USE, //B�L�����Z���g��
  BMPMENULIST_CANCELMODE_NOT, //B�L�����Z������
}BMPMENULIST_CANCELMODE;

/********************************************************************/
/**
 *					�ȈՃ��X�g�֐�
 *
 * @param	bmpdata		���X�g�p�r�b�g�}�b�v�f�[�^�|�C���^
 * @param	bmplist		�\�����X�g�w�b�_�|�C���^
 * @param	mode		�W���E�C���h�E�g�\���X�C�b�`
 *							EASYBMPMENULIST_NO_FRAME	�g�Ȃ�
 *							EASYBMPMENULIST_SCRFRAME	�g����A�L�����]���Ȃ�
 *							EASYBMPMENULIST_ALLFRAME	�g�A�L�����]������
 * @param	cgx			�g�b�f�w�]���I�t�Z�b�g
 * @param	pal			�g�o�`�k�]���I�t�Z�b�g(palanm.h �̒�`�ɏ���)
 *
 * @retval	param		BMPMENULIST_NULL�łȂ���΁A�I�����ꂽ���ڃp�����[�^
 */
/********************************************************************/
#define	EASYBMPMENULIST_NO_FRAME	(0)
#define	EASYBMPMENULIST_SCRFRAME	(1)
#define	EASYBMPMENULIST_ALLFRAME	(2)

/*u32	BmpListEasy
(const BMPWIN_DAT *bmpdata,const BMPMENULIST_HEADER *bmplist,u8 mode,u16 cgx,u16 pal);
*/

/********************************************************************/
/**
 *					�ݒ�֐��i�g���j
 *
 * @param	bmplist		�\�����X�g�w�b�_�|�C���^
 * @param	bmplist		�\�����X�g�p���b�g�A�g���r���[�g�|�C���^
 * @param	list_p		���X�g�����ʒu
 * @param	cursor_p	�J�[�\�������ʒu
 *
 * @retval	id			���X�g�h�c(=work_id)
 *						�����C���֐��̈����ƂȂ�̂ŊǗ���Y�ꂸ��
 *
 * call_back �ݒ�Ɋւ��āA���̊֐�����̈����� 
 *   BMPMENULIST_DATA �\���� param
 * �ƂȂ�
 */
/********************************************************************/
/*
extern u8		BmpListSetExpand
	(const BMPMENULIST_HEADER* bmplist,const BMPMENULIST_ATTR* bmpattr,u16 list_p,u16 cursor_p);
*/

//--------------------------------------------------------------------------------------------
/**
 * �ݒ�֐�
 *
 * @param	bmplist		�\�����X�g�w�b�_�|�C���^
 * @param	list_p		���X�g�����ʒu
 * @param	cursor_p	�J�[�\�������ʒu
 * @param	mode		�������擾���[�h
 *
 * @return	BMP���X�g���[�N
 *
 * @li	BMP���X�g���[�N��sys_AllocMemory�Ŋm��
 */
//--------------------------------------------------------------------------------------------
extern BMPMENULIST_WORK * BmpMenuList_Set(
						const BMPMENULIST_HEADER * bmplist, u16 list_p, u16 cursor_p, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * ���C���֐�
 *
 * @param	lw		BMP���X�g���[�N
 *
 * @retval	"param = �I���p�����[�^"
 * @retval	"BMPMENULIST_NULL = �I��"
 * @retval	"BMPMENULIST_CANCEL	= �L�����Z��(�a�{�^��)"
 */
//--------------------------------------------------------------------------------------------
extern u32 BmpMenuList_Main( BMPMENULIST_WORK * lw );

//--------------------------------------------------------------------------------------------
/**
 * �I���֐�
 *
 * @param	lw			BMP���X�g���[�N
 * @param	list_bak	���X�g�ʒu�o�b�N�A�b�v���[�N�|�C���^
 * @param	cursor_bak	�J�[�\���ʒu�o�b�N�A�b�v���[�N�|�C���^
 *
 * @return	none
 *
 * @li	lw��sys_FreeMemory�ŊJ��
 * @li	�o�b�N�A�b�v���[�N�́ANULL�w��ŕۑ����Ȃ�
 */
//--------------------------------------------------------------------------------------------
extern void BmpMenuList_Exit( BMPMENULIST_WORK * lw, u16 * list_bak, u16 * cursor_bak );

//--------------------------------------------------------------------------------------------
/**
 * ���X�g�ĕ`��
 *
 * @param	lw		BMP���X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BmpMenuList_Rewrite( BMPMENULIST_WORK * lw );

//--------------------------------------------------------------------------------------------
/**
 * �����F�ύX�֐�
 *
 * @param	lw			BMP���X�g���[�N
 * @param	f_col		�����F
 * @param	b_col		�w�i�F
 * @param	s_col		�e�F
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BmpMenuList_ColorControl( BMPMENULIST_WORK * lw, u8 f_col, u8 b_col, u8 s_col );

//--------------------------------------------------------------------------------------------
/**
 * �\���ʒu�ύX�֐�
 *
 * @param	lw			BMP���X�g���[�N
 * @param	x			�\���w���W(�L�����P��)
 * @param	y			�\���x���W(�L�����P��)
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BmpMenuList_MoveControl( BMPMENULIST_WORK * lw, u8 x, u8 y );

//--------------------------------------------------------------------------------------------
/**
 * �O���R���g���[���֐�(�`��E�R�[���o�b�N�w��)
 *
 * @param	lw			BMP���X�g���[�N
 * @param	lh			�\�����X�g�w�b�_�|�C���^
 * @param	list_p		���X�g�����ʒu
 * @param	cursor_p	�J�[�\�������ʒu
 * @param	print		�`��t���O
 * @param	direct		�w�����(�L�[��`)
 * @param	list_bak	���X�g�ʒu�o�b�N�A�b�v���[�N�|�C���^
 * @param	cursor_bak	�J�[�\���ʒu�o�b�N�A�b�v���[�N�|�C���^
 *
 * @return	BMPMENULIST_NULL	
 *
 * ���X�g�֐������삵�Ă��Ȃ��Ƃ��A�O������̎w��ɂ��
 * �J�[�\���ʒu�̕ϓ����`�F�b�N���A�w��o�b�N�A�b�v���[�N�ɕۑ�
 */
//--------------------------------------------------------------------------------------------
extern u32 BmpMenuList_MainOutControlEx(
			BMPMENULIST_WORK * lw, BMPMENULIST_HEADER * bmplist,
			u16 list_p, u16 cursor_p, u16 print, u16 direct, u16 * list_bak, u16 * cursor_bak );

//--------------------------------------------------------------------------------------------
/**
 * �O���R���g���[���֐�(�J�[�\���ړ�����)
 *
 * @param	lw			BMP���X�g���[�N
 * @param	bmplist		�\�����X�g�w�b�_�|�C���^
 * @param	list_p		���X�g�����ʒu
 * @param	cursor_p	�J�[�\�������ʒu
 * @param	direct		�w�����(�L�[��`)
 * @param	list_bak	���X�g�ʒu�o�b�N�A�b�v���[�N�|�C���^
 * @param	cursor_bak	�J�[�\���ʒu�o�b�N�A�b�v���[�N�|�C���^
 *
 * @return	BMPMENULIST_NULL	
 *
 * ���X�g�֐������삵�Ă��Ȃ��Ƃ��A�O������̎w��ɂ��
 * �J�[�\���ʒu�̕ϓ����`�F�b�N���A�w��o�b�N�A�b�v���[�N�ɕۑ�
 */
//--------------------------------------------------------------------------------------------
extern u32 BmpMenuList_MainOutControl(
				BMPMENULIST_WORK * lw, BMPMENULIST_HEADER * bmplist,
				u16 list_p, u16 cursor_p, u16 direct, u16 * list_bak, u16 * cursor_bak );

//--------------------------------------------------------------------------------------------
/**
 * BMP_MENULIST_DATA���ēx�ݒ肷��
 *
 * @param	BMP_MENULIST_DATA			BMPMENU���X�g�|�C���^
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BmpMenuList_SetMenuListData(BMPMENULIST_WORK * lw, BMP_MENULIST_DATA* pList);

//--------------------------------------------------------------------------------------------
/**
 * �����F�ꎞ�ύX
 *
 * @param	lw		BMP���X�g���[�N
 * @param	f_col	�����F
 * @param	b_col	�w�i�F
 * @param	s_col	�e�F
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BmpMenuList_TmpColorChange( BMPMENULIST_WORK * lw, u8 f_col, u8 b_col, u8 s_col );

//--------------------------------------------------------------------------------------------
/**
 * BMP���X�g�̑S�̈ʒu���擾
 *
 * @param	lw			BMP���X�g���[�N
 * @param	pos_bak		�S�̃��X�g�ʒu�o�b�N�A�b�v���[�N�|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BmpMenuList_DirectPosGet( BMPMENULIST_WORK * lw, u16 * pos_bak );

//--------------------------------------------------------------------------------------------
/**
 * BMP���X�g�̃��X�g�ʒu�A�J�[�\���ʒu���擾
 *
 * @param	lw			BMP���X�g���[�N
 * @param	list_bak	���X�g�ʒu�o�b�N�A�b�v���[�N�|�C���^
 * @param	cursor_bak	�J�[�\���ʒu�o�b�N�A�b�v���[�N�|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BmpMenuList_PosGet( BMPMENULIST_WORK * lw, u16 * list_bak, u16 * cursor_bak );

//----------------------------------------------------------------------------
/**
 * BMP���X�g�̃��X�g�ʒu�A�J�[�\���ʒu��ݒ�
 *
 * @param	lw			BMP���X�g���[�N
 * @param	list		���X�g�ʒu
 * @param	cursor	�J�[�\���ʒu
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
extern void BmpMenuList_PosSet( BMPMENULIST_WORK * lw, u16 list, u16 cursor );
//--------------------------------------------------------------------------------------------
/**
 * BMP���X�g�̃J�[�\��Y���W���擾
 *
 * @param	lw		BMP���X�g���[�N
 *
 * @return	�J�[�\��Y���W
 */
//--------------------------------------------------------------------------------------------
extern u16 BmpMenuList_CursorYGet( BMPMENULIST_WORK * lw );

//--------------------------------------------------------------------------------------------
/**
 * BMP���X�g�̃J�[�\���ړ������擾
 *
 * @param	lw		BMP���X�g���[�N
 *
 * @return	�J�[�\���ړ������擾
 */
//--------------------------------------------------------------------------------------------
extern u8 BmpMenuList_MoveSiteGet( BMPMENULIST_WORK * lw );

//--------------------------------------------------------------------------------------------
/**
 * �w��ʒu�̃p�����[�^�擾
 *
 * @param	lw		BMP���X�g���[�N
 * @param	pos		�ʒu
 *
 * @return	�p�����[�^
 */
//--------------------------------------------------------------------------------------------
extern u32 BmpMenuList_PosParamGet( BMPMENULIST_WORK * lw, u16 pos );

//--------------------------------------------------------------------------------------------
/**
 * �p�����[�^�擾�֐�
 *
 * @param	lw		BMP���X�g���[�N
 * @param	mode	�p�����[�^�擾�h�c
 *
 * @retval	"0xffffffff = �G���["
 * @retval	"0xffffffff != �p�����[�^"
 */
//--------------------------------------------------------------------------------------------
extern u32	BmpListParamGet( BMPMENULIST_WORK * lw, u8 mode );

//--------------------------------------------------------------------------------------------
/**
 * �p�����[�^�ύX�֐�
 *
 * @param	lw		BMP���X�g���[�N
 * @param	mode	�p�����[�^�擾�h�c
 * @param	param	�ύX�l
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BmpMenuList_ParamSet( BMPMENULIST_WORK * lw, u8 mode, u32 param );

//--------------------------------------------------------------
/**
 * �J�[�\��������Z�b�g
 */
//--------------------------------------------------------------
extern void BmpMenuList_SetCursorString( BMPMENULIST_WORK *lw, u32 strID );

//--------------------------------------------------------------
/**
 * �J�[�\���r�b�g�}�b�v�Z�b�g
 */
//--------------------------------------------------------------
extern void BmpMenuList_SetCursorBmp( BMPMENULIST_WORK *lw, u32 heapID );

//--------------------------------------------------------------
/**
 * �w�b�_�[�ɃZ�b�g�������[�N�����o��
 */
//--------------------------------------------------------------
extern void* BmpMenuList_GetWorkPtr( BMPMENULIST_WORK *lw );

//--------------------------------------------------------------
/// �L�����Z�����[�h�Z�b�g
//--------------------------------------------------------------
extern void BmpMenuList_SetCancelMode( BMPMENULIST_WORK *lw, BMPMENULIST_CANCELMODE mode );

#endif //__BMP_LIST_H__
