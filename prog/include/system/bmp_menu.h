//======================================================================
/**
 * @file  bmp_menu.h
 * @brief BMP���j���[����
 * @author  Hiroyuki Nakamura
 * @date  2004.11.10
 */
//======================================================================
#ifndef BMP_MENU_H
#define BMP_MENU_H

#include <tcbl.h>
#include "print\printsys.h"
#include "print\gf_font.h"

#include "bmp_menuwork.h"

//======================================================================
//  �V���{����`
//======================================================================
#define BMPMENU_NULL  ( 0xffffffff )
#define BMPMENU_CANCEL  ( 0xfffffffe )
#define BMPMENU_DUMMY ( 0xfffffffd )

typedef struct {
  const BMPMENU_DATA * menu;
  GFL_BMPWIN *win;
  u8  dummy;
  u8  x_max;      // ���������ڍő吔�i�K���P�ȏ�j
  u8  y_max;      // �c�������ڍő吔�i�K���P�ȏ�j
  u8  line_spc:4;   // �����Ԋu�x
  u8  c_disp_f:2;   // �J�[�\���\���t���O
  u8  loop_f:2;   // LOOP�t���O ( 0=OFF, 1=ON )
  
  u16 font_size_x;    //�����T�C�YX(�h�b�g
  u16 font_size_y;    //�����T�C�YY(�h�b�g
  GFL_MSGDATA *msgdata; //�\���Ɏg�p���郁�b�Z�[�W�o�b�t�@
//  STRBUF *strbuf;     //�\���Ɏg�p���郁�b�Z�[�W�o�b�t�@
  PRINT_UTIL *print_util; //�\���Ɏg�p����v�����g���[�e�B���e�B
  PRINT_QUE *print_que; //�\���Ɏg�p����v�����g�L���[
  GFL_FONT *font_handle;  //�\���Ɏg�p����t�H���g�n���h��

  BOOL  b_trans;  // PRINT_QUE����������ɓ]�����Ă����TRUE
}BMPMENU_HEADER;

typedef struct _BMPMENU_WORK  BMPMENU_WORK;

// �O���R���g���[���p�����[�^
enum {
  BMPMENU_CNTROL_DECIDE = 0,  // ����
  BMPMENU_CNTROL_CANCEL,    // �L�����Z��
  BMPMENU_CNTROL_UP,      // ��
  BMPMENU_CNTROL_DOWN,    // ��
  BMPMENU_CNTROL_LEFT,    // ��
  BMPMENU_CNTROL_RIGHT,   // �E
};

// �ړ�����
enum {
  BMPMENU_MOVE_NONE = 0,  // ����Ȃ�
  BMPMENU_MOVE_UP,    // ��ֈړ�
  BMPMENU_MOVE_DOWN,    // ���ֈړ�
  BMPMENU_MOVE_LEFT,    // ���ֈړ�
  BMPMENU_MOVE_RIGHT,   // �E�ֈړ�
};

//======================================================================
//  �v���g�^�C�v�錾
//======================================================================
//--------------------------------------------------------------
/**
 * BMP���j���[�o�^�i�X�N���[���]�����Ȃ��j
 *
 * @param dat     �w�b�_�f�[�^
 * @param px      ���ڕ\��X���W
 * @param py      ���ڕ\��Y���W
 * @param pos     �����J�[�\���ʒu
 * @aram  mode    �������擾���[�h
 * @param cancel    �L�����Z���{�^��
 *
 * @return  BMP���j���[���[�N
 *
 * @li  BMP���X�g���[�N��sys_AllocMemory�Ŋm��
 */
//--------------------------------------------------------------
extern BMPMENU_WORK * BmpMenu_AddNoTrans( const BMPMENU_HEADER * dat,
    u8 px, u8 py, u8 pos, HEAPID heap_id, u32 cancel );

//--------------------------------------------------------------
/**
 * BMP���j���[�o�^�i�L�����Z���{�^���w��j
 *
 * @param dat     �w�b�_�f�[�^
 * @param px      ���ڕ\��X���W
 * @param py      ���ڕ\��Y���W
 * @param pos     �����J�[�\���ʒu
 * @aram  mode    �������擾���[�h
 * @param cancel    �L�����Z���{�^��
 *
 * @return  BMP���j���[���[�N
 *
 * @li  BMP���X�g���[�N��sys_AllocMemory�Ŋm��
 */
//--------------------------------------------------------------
extern BMPMENU_WORK * BmpMenu_AddEx( const BMPMENU_HEADER * dat,
    u8 px, u8 py, u8 pos, HEAPID heap_id, u32 cancel );

//--------------------------------------------------------------
/**
 * BMP���j���[�o�^�i�ȈՔŁj
 *
 * @param dat     �w�b�_�f�[�^
 * @param pos     �����J�[�\���ʒu
 * @aram  mode    �������擾���[�h
 *
 * @return  BMP���j���[���[�N
 *
 * @li  BMP���X�g���[�N��sys_AllocMemory�Ŋm��
 * @li  B�{�^���L�����Z��
 */
//--------------------------------------------------------------
extern BMPMENU_WORK * BmpMenu_Add(
  const BMPMENU_HEADER * dat, u8 pos, HEAPID heap_id );

//--------------------------------------------------------------
/**
 * BMP���j���[�j��
 *
 * @param mw    BMP���j���[���[�N
 * @param backup  �J�[�\���ʒu�ۑ��ꏊ
 *
 * @return  none
 */
//--------------------------------------------------------------
extern void BmpMenu_Exit( BMPMENU_WORK *mw, u8 * backup );

//--------------------------------------------------------------
/**
 * ���j���[����֐�
 *
 * @param mw    BMP���j���[���[�N
 *
 * @return  ���쌋��
 */
//--------------------------------------------------------------
extern u32 BmpMenu_Main( BMPMENU_WORK * mw );

//--------------------------------------------------------------
/**
 * ���j���[����֐��i�\���L�[��SE���w��ł���j
 *
 * @param mw    BMP���j���[���[�N
 * @param key_se  �\���L�[��SE
 *
 * @return  ���쌋��
 */
//--------------------------------------------------------------
extern u32 BmpMenu_MainSE( BMPMENU_WORK * mw, u16 key_se );

//--------------------------------------------------------------
/**
 * ���j���[�O���R���g���[��
 *
 * @param mw    BMP���j���[���[�N
 * @param prm   �R���g���[���p�����[�^
 *
 * @return  ���쌋��
 */
//--------------------------------------------------------------
extern u32 BmpMenu_MainOutControl( BMPMENU_WORK * mw, u8 prm );


//--------------------------------------------------------------
/**
 * �J�[�\���ʒu�擾
 *
 * @param mw    BMP���j���[���[�N
 *
 * @return  �J�[�\���ʒu
 */
//--------------------------------------------------------------
extern u8 BmpMenu_CursorPosGet( BMPMENU_WORK * mw );

//--------------------------------------------------------------
/**
 * �ړ������擾
 *
 * @param mw    BMP���j���[���[�N
 *
 * @return  �ړ������擾
 */
//--------------------------------------------------------------
extern u8 BmpMenu_MoveSiteGet( BMPMENU_WORK * mw );

//--------------------------------------------------------------
/**
 * �J�[�\��������Z�b�g
 * @param mw  BMPMENU_WORK
 * @param strID ������ID
 * @retval  nothing
 */
//--------------------------------------------------------------
extern void BmpMenu_SetCursorString( BMPMENU_WORK *mw, u32 strID );

//�֐��ǉ����܂��� Ari081015
//--------------------------------------------------------------
/**
 * ���j���[�ĕ`��
 * @param mw  BMPMENU_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
extern void BmpMenu_RedrawString( BMPMENU_WORK *mw );


typedef struct {
  u8  frmnum; ///<�E�C���h�E�g�p�t���[��
  u8  pos_x;    ///<�E�C���h�E�̈�̍����X���W�i�L�����P�ʂŎw��j
  u8  pos_y;    ///<�E�C���h�E�̈�̍����Y���W�i�L�����P�ʂŎw��j
  u8  palnum;   ///<�E�C���h�E�̈�̃p���b�g�i���o�[
  u16 chrnum;   ///<�E�C���h�E�L�����̈�̊J�n�L�����N�^�i���o�[
} BMPWIN_YESNO_DAT;

//--------------------------------------------------------------------------------------------
/**
 * �͂��E�������E�B���h�E�Z�b�g�i�J�[�\���ʒu�w��j
 *
 * @param data  �E�B���h�E�f�[�^
 * @param cgx   �E�B���h�E�L�����ʒu
 * @param pal   �E�B���h�E�p���b�g�ԍ�
 * @param pos   �����J�[�\���ʒu
 * @param heap  �q�[�vID
 *
 * @return  BMP���j���[���[�N
 *
 * @li  BMP�E�B���h�E��BMP���j���[���[�N��Alloc�Ŏ擾���Ă���
 */
//--------------------------------------------------------------------------------------------
extern BMPMENU_WORK * BmpMenu_YesNoSelectInit
  ( const BMPWIN_YESNO_DAT *data, u16 winframecgx, u8 winframepal, u8 pos, HEAPID heap );
extern BMPMENU_WORK * BmpMenu_YesNoSelectInitFixPos
  ( const BMPWIN_YESNO_DAT *data, u16 winframecgx, u8 winframepal, u8 pos, HEAPID heap );

//--------------------------------------------------------------------------------------------
/**
 * �͂��E�������I���E�B���h�E�̐���
 *
 * @param ini   BGL�f�[�^
 * @param heap  �q�[�vID
 *
 * @retval  "BMPMENU_NULL �I������Ă��Ȃ�"
 * @retval  "0        �͂���I��"
 * @retval  "BMPMENU_CANCEL ������or�L�����Z��"
 */
//--------------------------------------------------------------------------------------------
extern u32 BmpMenu_YesNoSelectMain( BMPMENU_WORK * mw );


#ifdef BUGFIX_AF_BTS7810_20100806
//----------------------------------------------------------------------------
/**
 *	@brief    �͂��E�������I���E�B���h�E�̕\��
 *
 *	@param	  ���[�N
 *
 *	@retval   TRUE�ŕ\������  FALSE�ŕ\����
 */
//-----------------------------------------------------------------------------
extern BOOL BmpMenu_YesNoSelectPrintMain( BMPMENU_WORK * mw );
#endif

//--------------------------------------------------------------------------------------------
/**
 * �͂��E�������E�B���h�E�폜
 *
 * @param ini   BGL�f�[�^
 * @param heap  �q�[�vID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void BmpMenu_YesNoMenuExit( BMPMENU_WORK * mw );



#endif  /* BMP_MENU_H */
