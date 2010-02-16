//=============================================================================================
/**
 * @file  printsys.c
 * @brief Bitmap���p�����`��@�V�X�e������
 * @author  taya
 *
 * @date  2007.02.06  �쐬
 * @date  2008.09.11  �ʐM�Ή��L���[�\�����쐬
 * @date  2008.09.26  �L���[�\���𗘗p���Ȃ�������`�揈��������
 */
//=============================================================================================
#ifndef __PRINTSYS_H__
#define __PRINTSYS_H__

#include <tcbl.h>
#include <bmp_win.h>
#include <strbuf.h>

#include "print\gf_font.h"


//============================================================================================================
// Typedefs

//--------------------------------------------------------------
/**
 *  Print Queue
 *  �ʐM���ɕ`�揈���������݂Ɏ��s�ł���悤�A�������e��~�ς��邽�߂̋@�\�B
 */
//--------------------------------------------------------------
typedef struct _PRINT_QUE PRINT_QUE;

//--------------------------------------------------------------------------
/**
 *  Print Stream
 *  ��b�E�B���h�E���A�P��Ԋu�łP�������`��C�\�����s�����߂̋@�\
 */
//--------------------------------------------------------------------------
typedef struct _PRINT_STREAM  PRINT_STREAM;

//--------------------------------------------------------------------------
/**
 *  Print Stream Callback
 *  Print Stream �łP�����`�悲�ƂɌĂяo�����R�[���o�b�N�֐��̌^
 *
 *  �����Fu32 gmm�Őݒ肵���C�ӂ̒l�i���ݒ�Ȃ�0�j
 *
 *  �߂�l�F��{�I�ɂ͏펞FALSE��Ԃ��悤�ɂ��Ă��������B
 *          TRUE��Ԃ��Ă���Ԃ� Stream �^�X�N�����肵�Ď��̏����֑J�ڂ��Ȃ��Ȃ�܂��B
 *          �iSE�̏I���҂��ȂǂɎg�p���邱�Ƃ�z�肵�Ă��܂��j
 */
//--------------------------------------------------------------------------
typedef BOOL (*pPrintCallBack)(u32);


//--------------------------------------------------------------------------
/**
 *  Print System �ŗp����`��F�f�[�^
 *  ����(Letter)�A�e(Shadow)�A�w�i�F(Background) �̔ԍ��e5bit���p�b�N��������
 */
//--------------------------------------------------------------------------
typedef u16   PRINTSYS_LSB;


#define PRINTSYS_MACRO_LSB(L, S, B)  (PRINTSYS_LSB)((((L)&0x1f)<<10) | (((S)&0x1f)<<5) | ((B)&0x1f))

static inline PRINTSYS_LSB PRINTSYS_LSB_Make( u8 l, u8 s, u8 b )
{
  return PRINTSYS_MACRO_LSB( l, s, b );
}
static inline u8 PRINTSYS_LSB_GetL( PRINTSYS_LSB color )
{
  return (color >> 10) & 0x1f;
}
static inline u8 PRINTSYS_LSB_GetS( PRINTSYS_LSB color )
{
  return (color >> 5) & 0x1f;
}
static inline u8 PRINTSYS_LSB_GetB( PRINTSYS_LSB color )
{
  return (color & 0x1f);
}
static inline void PRINTSYS_LSB_GetLSB( PRINTSYS_LSB color, u8* l, u8* s, u8* b )
{
  *l = PRINTSYS_LSB_GetL( color );
  *s = PRINTSYS_LSB_GetS( color );
  *b = PRINTSYS_LSB_GetB( color );
}

//============================================================================================================
// Consts

//--------------------------------------------------------------------------
/**
 *  Print Stream �̏�Ԃ�����
 */
//--------------------------------------------------------------------------
typedef enum {
  PRINTSTREAM_STATE_RUNNING = 0,  ///< �������s���i�����񂪗���Ă���j
  PRINTSTREAM_STATE_PAUSE,    ///< �ꎞ��~���i�y�[�W�؂�ւ��҂����j
  PRINTSTREAM_STATE_DONE,     ///< ������I�[�܂ŕ\������
}PRINTSTREAM_STATE;

//--------------------------------------------------------------------------
/**
 *  Print Stream �̈ꎞ��~��Ԃ�����
 */
//--------------------------------------------------------------------------
typedef enum {
  PRINTSTREAM_PAUSE_LINEFEED = 0, ///< �y�[�W�؂�ւ��҂��i���s�j
  PRINTSTREAM_PAUSE_CLEAR,        /// <�y�[�W�؂�ւ��҂��i�\���N���A�j
}PRINTSTREAM_PAUSE_TYPE;


//============================================================================================================
// Prototypes

//==============================================================================================
/**
 * �V�X�e���������i�v���O�����N�����ɂP�x�����Ăяo���j
 *
 * @param   heapID    �������p�q�[�vID
 *
 */
//==============================================================================================
extern void PRINTSYS_Init( HEAPID heapID );

//==============================================================================================
/**
 * �v�����g�L���[���f�t�H���g�̃o�b�t�@�T�C�Y�Ő�������B
 *
 * @param   heapID      �����p�q�[�vID
 *
 * @retval  PRINT_QUE*    �������ꂽ�v�����g�L���[
 */
//==============================================================================================
extern PRINT_QUE* PRINTSYS_QUE_Create( HEAPID heapID );

//==============================================================================================
/**
 * �v�����g�L���[���T�C�Y�w�肵�Đ�������
 *
 * @param   size      �o�b�t�@�T�C�Y
 * @param   heapID      �����p�q�[�vID
 *
 * @retval  PRINT_QUE*    �������ꂽ�v�����g�L���[
 */
//==============================================================================================
extern PRINT_QUE* PRINTSYS_QUE_CreateEx( u16 buf_size, HEAPID heapID );

//=============================================================================================
/**
 * �v�����g�L���[���ʐM���A��x�̏����Ɏg�����ԁiTick�j��ݒ肷��
 *
 * @param   que     �v�����g�L���[
 * @param   tick    ��������
 *
 */
//=============================================================================================
extern void PRINTSYS_QUE_SetLimitTick( PRINT_QUE* que, OSTick tick );

//==============================================================================================
/**
 * �v�����g�L���[���C�������i���C�����[�v���łP��Ăяo���j
 *
 * @param   que   �v�����g�L���[
 *
 * @retval  BOOL  �L���[�������c���Ă��Ȃ��ꍇ��TRUE�^�ɂ܂��������c���Ă���ꍇ��FALSE
 */
//==============================================================================================
extern BOOL PRINTSYS_QUE_Main( PRINT_QUE* que );

//==============================================================================================
/**
 * �v�����g�L���[�ɏ������c���Ă��邩����
 *
 * @param   que   �v�����g�L���[
 *
 * @retval  BOOL  �L���[�������c���Ă��Ȃ��ꍇ��TRUE�^�ɂ܂��������c���Ă���ꍇ��FALSE
 */
//==============================================================================================
extern BOOL PRINTSYS_QUE_IsFinished( const PRINT_QUE* que );

//==============================================================================================
/**
 * �v�����g�L���[�ɁA����Bitmap���o�͑ΏۂƂ����������c���Ă��邩����
 *
 * @param   que       �v�����g�L���[
 * @param   targetBmp   �o�͑Ώۂ�Bitmap
 *
 * @retval  BOOL      targetBmp�ɑ΂��鏈�����c���Ă���ꍇ��TRUE�^����ȊO��FALSE
 */
//==============================================================================================
extern BOOL PRINTSYS_QUE_IsExistTarget( const PRINT_QUE* que, const GFL_BMP_DATA* targetBmp );

//==============================================================================================
/**
 * �v�����g�L���[�����������`��
 *
 * @param   que   [out] �`�揈�����e���L�^���邽�߂̃v�����g�L���[
 * @param   dst   [out] �`���Bitmap
 * @param   xpos  [in]  �`��J�n�w���W�i�h�b�g�j
 * @param   ypos  [in]  �`��J�n�x���W�i�h�b�g�j
 * @param   str   [in]  ������
 * @param   font  [in]  �t�H���g
 *
 */
//==============================================================================================
extern void PRINTSYS_PrintQue( PRINT_QUE* que, GFL_BMP_DATA* dst, s16 xpos, s16 ypos, const STRBUF* str, GFL_FONT* font );

//==============================================================================================
/**
 * �v�����g�L���[�����������`��i�F�ύX�ɑΉ��j
 *
 * @param   que   [out] �`�揈�����e���L�^���邽�߂̃v�����g�L���[
 * @param   dst   [out] �`���Bitmap
 * @param   xpos  [in]  �`��J�n�w���W�i�h�b�g�j
 * @param   ypos  [in]  �`��J�n�x���W�i�h�b�g�j
 * @param   str   [in]  ������
 * @param   font  [in]  �t�H���g
 * @param   color [in]  �F�ԍ�
 *
 */
//==============================================================================================
extern void PRINTSYS_PrintQueColor( PRINT_QUE* que, GFL_BMP_DATA* dst, s16 xpos, s16 ypos, const STRBUF* str, GFL_FONT* font, PRINTSYS_LSB color );

//=============================================================================================
/**
 * �v�����g�L���[�ɒ��܂��Ă��鏈����S�ăN���A����
 *
 * @param   que   �v�����g�L���[
 *
 */
//=============================================================================================
extern void PRINTSYS_QUE_Clear( PRINT_QUE* que );

//==============================================================================================
/**
 * �v�����g�L���[���폜����
 *
 * @param   que   �v�����g�L���[
 *
 */
//==============================================================================================
extern void PRINTSYS_QUE_Delete( PRINT_QUE* que );

//=============================================================================================
/**
 * [�f�o�b�O�p] ��ʐM���ɂ��ʐM���Ɠ��l�̋������s�킹��
 *
 * @param   que
 * @param   flag    TRUE�ŗL���^FALSE�Ŗ����ɖ߂�
 *
 */
//=============================================================================================
extern void PRINTSYS_QUE_ForceCommMode( PRINT_QUE* que, BOOL flag );


//==============================================================================================
/**
 * Bitmap �֒��ڂ̕�����`��
 *
 * @param   dst   [out] �`���Bitmap
 * @param   xpos  [in]  �`��J�n�w���W�i�h�b�g�j
 * @param   ypos  [in]  �`��J�n�x���W�i�h�b�g�j
 * @param   str   [in]  ������
 * @param   font  [in]  �t�H���g
 *
 */
//==============================================================================================
extern void PRINTSYS_Print( GFL_BMP_DATA* dst, s16 xpos, s16 ypos, const STRBUF* str, GFL_FONT* font );

//=============================================================================================
/**
 * Bitmap �֒��ڂ̕�����`��i�J���[�w��Łj
 *
 * @param   dst   [out] �`���Bitmap
 * @param   xpos  [in]  �`��J�n�w���W�i�h�b�g�j
 * @param   ypos  [in]  �`��J�n�x���W�i�h�b�g�j
 * @param   str   [in]  ������
 * @param   font  [in]  �t�H���g
 * @param   color [in]  �`��J���[
 *
 */
//=============================================================================================
extern void PRINTSYS_PrintColor( GFL_BMP_DATA* dst, s16 xpos, s16 ypos, const STRBUF* str, GFL_FONT* font, PRINTSYS_LSB color );

//==============================================================================================
/**
 * �v�����g�X�g���[���𗘗p����������`��i�ʏ�� - �R�[���o�b�N�Ȃ��j
 *
 * @param   dst     �`���Bitmap
 * @param   xpos    �`��J�n�w���W�i�h�b�g�j
 * @param   ypos    �`��J�n�x���W�i�h�b�g�j
 * @param   str     ������
 * @param   font    �t�H���g�n���h��
 * @param   wait    �P�����`�悲�Ƃ̃E�F�C�g�t���[�����i���j
 * @param   tcbsys    �g�p����GFL_TCBL�V�X�e���|�C���^
 * @param   tcbpri    �g�p����^�X�N�v���C�I���e�B
 * @param   heapID    �g�p����q�[�vID
 *
 * @retval  PRINT_STREAM* �X�g���[���n���h��
 *
 * �� wait ��-2 �ȉ��̒l��ݒ肷�邱�ƂŁA���t���[���Q�����ȏ�̕`����s���B-2�i�Q�����j, -3�i�R�����j...
 */
//==============================================================================================
extern PRINT_STREAM* PRINTSYS_PrintStream(
    GFL_BMPWIN* dst, s16 xpos, s16 ypos, const STRBUF* str, GFL_FONT* font,
    int wait, GFL_TCBLSYS* tcbsys, u32 tcbpri, HEAPID heapID, u16 clearColor );

//==============================================================================================
/**
 * �v�����g�X�g���[���𗘗p����������`��i�R�[���o�b�N����j
 *
 * @param   dst     �`���Bitmap
 * @param   xpos    �`��J�n�w���W�i�h�b�g�j
 * @param   ypos    �`��J�n�x���W�i�h�b�g�j
 * @param   str     ������
 * @param   font    �t�H���g�n���h��
 * @param   wait    �P�����`�悲�Ƃ̃E�F�C�g�t���[�����i�� �ʏ�łƓ����j
 * @param   tcbsys    �g�p����GFL_TCBL�V�X�e���|�C���^
 * @param   tcbpri    �g�p����^�X�N�v���C�I���e�B
 * @param   heapID    �g�p����q�[�vID
 * @param   callback  �P�����`�悲�Ƃ̃R�[���o�b�N�֐��A�h���X
 *
 * @retval  PRINT_STREAM* �X�g���[���n���h��
 *
 */
//==============================================================================================
extern PRINT_STREAM* PRINTSYS_PrintStreamCallBack(
    GFL_BMPWIN* dst, s16 xpos, s16 ypos, const STRBUF* str, GFL_FONT* font,
    int wait, GFL_TCBLSYS* tcbsys, u32 tcbpri, HEAPID heapID, u16 clearColor, pPrintCallBack callback );

//==============================================================================================
/**
 * �v�����g�X�g���[����Ԏ擾
 *
 * @param   handle    �X�g���[���n���h��
 *
 * @retval  PRINTSTREAM_STATE   ���
 */
//==============================================================================================
extern PRINTSTREAM_STATE PRINTSYS_PrintStreamGetState( PRINT_STREAM* handle );

//==============================================================================================
/**
 * �v�����g�X�g���[���ꎞ��~�^�C�v�擾
 *
 * @param   handle    �X�g���[���n���h��
 *
 * @retval  PRINTSTREAM_PAUSE_TYPE    ��~�^�C�v
 */
//==============================================================================================
extern PRINTSTREAM_PAUSE_TYPE PRINTSYS_PrintStreamGetPauseType( PRINT_STREAM* handle );

//==============================================================================================
/**
 * �v�����g�X�g���[���ꎞ��~������
 *
 * @param   handle    �X�g���[���n���h��
 *
 */
//==============================================================================================
extern void PRINTSYS_PrintStreamReleasePause( PRINT_STREAM* handle );

//==============================================================================================
/**
 * �v�����g�X�g���[���폜
 *
 * @param   handle    �X�g���[���n���h��
 *
 */
//==============================================================================================
extern void PRINTSYS_PrintStreamDelete( PRINT_STREAM* handle );

//=============================================================================================
/**
 * �v�����g�X�g���[�������[�U�v���O�����������I�Ɉꎞ��~����
 *
 * @param   handle
 */
//=============================================================================================
extern void PRINTSYS_PrintStreamStop( PRINT_STREAM* handle );

//=============================================================================================
/**
 * �v�����g�X�g���[���̃��[�U�����ꎞ��~����������
 *
 * @param   handle
 */
//=============================================================================================
extern void PRINTSYS_PrintStreamRun( PRINT_STREAM* handle );

//==============================================================================================
/**
 * �v�����g�X�g���[���E�F�C�g�Z�k�i�L�[�������Ȃǂɖ��t���[���Ăяo�����Ƃŋ����I�ɃE�F�C�g��Z�k�j
 *
 * @param   handle    �X�g���[���n���h��
 * @param   wait    �ݒ肷��E�F�C�g�t���[����
 *
 */
//==============================================================================================
extern void PRINTSYS_PrintStreamShortWait( PRINT_STREAM* handle, u16 wait );

//==============================================================================================
/**
 * �������Bitmap�\������ۂ̕��i�h�b�g���j���v�Z
 * �������񂪕����s����ꍇ�A���̍Œ��s�̃h�b�g��
 *
 * @param   str     ������
 * @param   font    �t�H���g�n���h��
 * @param   margin    ���ԃX�y�[�X�i�h�b�g�j
 *
 * @retval  u32     �����񕝁i�h�b�g�j
 */
//==============================================================================================
extern u32 PRINTSYS_GetStrWidth( const STRBUF* str, GFL_FONT* font, u16 margin );

//=============================================================================================
/**
 * �����[�J���C�Y���ӁI�@�g�p�͋����ł��B��
 *
 * ������̍s����Ԃ�
 *
 * @param   str
 *
 * @retval  u32
 */
//=============================================================================================
extern u32 PRINTSYS_GetLineCount( const STRBUF* str );

//=============================================================================================
/**
 * �����[�J���C�Y���ӁI�@�g�p�͋����ł��B��
 *
 * �������Bitmap�\������ۂ̕��i�h�b�g���j���v�Z
 * �������񂪕����s����ꍇ�A�s���Ƃ̕����擾�ł���
 *

 * @param   str       ������
 * @param   font      �t�H���g�n���h��
 * @param   margin    ���ԃX�y�[�X�i�h�b�g�j
 * @param   dst       �s���Ƃ̕����擾���邽�߂̔z��
 * @param   dstElems  dst �̔z��v�f���i������z���Ȃ��悤�ɏ������݂܂��j
 *
 * @retval  u32       dst �Ɋi�[���������i�ő�ł� dstElems���z���Ȃ��j
 */
//=============================================================================================
extern u32 PRINTSYS_GetStrLineWidth( const STRBUF* str, GFL_FONT* font, u16 margin, u32* dst, u32 dstElems );

//=============================================================================================
/**
 * �����[�J���C�Y���ӁI�@�g�p�͋����ł��B��
 *
 * �����s���镶���񂩂�w��s�̕�����݂̂����o��
 *
 *
 * @param   src     �ǂݎ�茳�̕�����
 * @param   dst     �ǂݎ���o�b�t�@
 * @param   line    �s�ԍ��i0�`�j
 *
 * @retval  BOOL    �w�� line ���傫�����ēǂݎ��Ȃ��ꍇFALSE�^����ȊO��TRUE
 */
//=============================================================================================
extern BOOL PRINTSYS_CopyLineStr( const STRBUF* src, STRBUF* dst, u32 line );



//=============================================================================================
/**
 * �������Bitmap�\������ۂ̕��i�h�b�g���j���v�Z
 *
 * @param   str     ������
 * @param   font    �t�H���g�n���h��
 *
 * @retval  u32     �����񍂂��i�h�b�g�j
 */
//=============================================================================================
extern u32 PRINTSYS_GetStrHeight( const STRBUF* str, GFL_FONT* font );

//=============================================================================================
/**
 * ������̒��Ɋ܂܂��A�P��^�O�̐����J�E���g���ĕԂ�
 *
 * @param   str   ������
 *
 * @retval  u16   �P��^�O�̐�
 */
//=============================================================================================
extern u16 PRINTSYS_GetTagCount( const STRBUF* str );

//=============================================================================================
/**
 * �w�肵���^�O�������񒆉��s�ڂɂ��邩��Ԃ�
 *
 * @param   str     ������
 * @param   tag_id  �^�O�C���f�b�N�X�i�o�b�t�@�ԍ��j
 *
 * @retval  u8      �s��
 *
 * @note  add by genya hosaka
 */
//=============================================================================================
extern u8 PRINTSYS_GetTagLine( const STRBUF* str, u8 tag_id );

//=============================================================================================
/**
 * �w�肵���^�O�����h�b�g�ڂɂ��邩��Ԃ�
 *
 * @param   str     ������
 * @param   tag_id  �^�O�C���f�b�N�X�i�o�b�t�@�ԍ��j
 * @param   font    �t�H���g�^�C�v
 * @param   margin  ���ԃX�y�[�X�i�h�b�g�j
 *
 * @retval  u8      X���W�i�h�b�g�j
 *
 * @note  add by genya hosaka
 */
//=============================================================================================
extern u8 PRINTSYS_GetTagWidth( const STRBUF* str, u8 tag_id, GFL_FONT* font, u16 margin );


//--------------------------------------------------------------
/**
 *  ���[�e�B���e�B�Ǘ��\����
 *
 *  ���g�����J���Ă�̂�Alloc�s�v�B
 *  �e�����[�N�̈�ɕK�v�Ȃ�������Ă�������
 */
//--------------------------------------------------------------
typedef struct {
  GFL_BMPWIN*   win;
  u8        transReq;
}PRINT_UTIL;


//--------------------------------------------------------------------------------------
/**
 * ���[�e�B���e�B�������iBitmapWindow�ƂP�΂P�Ŋ֘A�t�����s���B�P�񂾂��Ăяo���j
 *
 * @param   wk
 * @param   win
 */
//--------------------------------------------------------------------------------------
inline void PRINT_UTIL_Setup( PRINT_UTIL* wk, GFL_BMPWIN* win )
{
  wk->win = win;
  wk->transReq = FALSE;
}

//--------------------------------------------------------------------------------------
/**
 * ���[�e�B���e�B�����Bitmap������`��
 *
 * @param   wk
 * @param   que
 * @param   xpos
 * @param   ypos
 * @param   buf
 * @param   font
 *
 */
//--------------------------------------------------------------------------------------
inline void PRINT_UTIL_Print( PRINT_UTIL* wk, PRINT_QUE* que, s16 xpos, s16 ypos, const STRBUF* buf, GFL_FONT* font )
{
  PRINTSYS_PrintQue( que, GFL_BMPWIN_GetBmp(wk->win), xpos, ypos, buf, font );
  wk->transReq = TRUE;
}
//--------------------------------------------------------------------------------------
/**
 * ���[�e�B���e�B�����Bitmap������`��i�F�ύX�ɑΉ��j
 *
 * @param   wk
 * @param   que
 * @param   xpos
 * @param   ypos
 * @param   buf
 * @param   font
 * @param   color
 *
 */
//--------------------------------------------------------------------------------------
inline void PRINT_UTIL_PrintColor( PRINT_UTIL* wk, PRINT_QUE* que, s16 xpos, s16 ypos, const STRBUF* buf, GFL_FONT* font, PRINTSYS_LSB color )
{
  PRINTSYS_PrintQueColor( que, GFL_BMPWIN_GetBmp(wk->win), xpos, ypos, buf, font, color );
  wk->transReq = TRUE;
}


//--------------------------------------------------------------------------------------
/**
 * �K�v�ȃ^�C�~���O�𔻒f���āACGX�f�[�^��VRAM�]�����s���܂�
 *
 * @param   wk
 * @param   que
 *
 * @retval  BOOL  �]�����I����Ă���ꍇ��TRUE�^�I����Ă��Ȃ��ꍇ��FALSE
 */
//--------------------------------------------------------------------------------------
inline BOOL PRINT_UTIL_Trans( PRINT_UTIL* wk, PRINT_QUE* que )
{
  if( wk->transReq )
  {
    if( !PRINTSYS_QUE_IsExistTarget(que, GFL_BMPWIN_GetBmp(wk->win)) )
    {
      GFL_BMPWIN_TransVramCharacter( wk->win );
      wk->transReq = FALSE;
    }
  }
  return !(wk->transReq);
}



//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

// �^�O�啪��
typedef enum {
  PRINTSYS_TAGGROUP_WORD = 1,           ///< �ʏ�̒P��
  PRINTSYS_TAGGROUP_NUM  = 2,           ///< ���l
  PRINTSYS_TAGGROUP_CTRL_GEN = 189,     ///< �ėp�R���g���[��
  PRINTSYS_TAGGROUP_CTRL_STREAM = 190,  ///< �X�g���[���R���g���[��
}PrintSysTagGroup;

// �ėp�R���g���[���^�C�v
typedef enum {
  PRINTSYS_CTRL_GENERAL_COLOR        = (0x0000), ///< �F�ύX
  PRINTSYS_CTRL_GENERAL_RESET_COLOR  = (0x0001), ///< �F�ύX
  PRINTSYS_CTRL_GENERAL_X_CENTERING  = (0x0002), ///< �w���W�E��
  PRINTSYS_CTRL_GENERAL_X_RIGHTFIT   = (0x0003), ///< �w���W�Z���^�����O
  PRINTSYS_CTRL_GENERAL_X_ADD        = (0x0004), ///< �w���W�����Z
  PRINTSYS_CTRL_GENERAL_X_SET        = (0x0005), ///< �w���W���w��
}PrintSysGenCtrlCode;

// �X�g���[���R���g���[���^�C�v
typedef enum {
  PRINTSYS_CTRL_STREAM_LINE_FEED     = (0x0000), ///< ���y�[�W�i�s����҂��j
  PRINTSYS_CTRL_STREAM_PAGE_CLEAR    = (0x0001), ///< ���y�[�W�i�`��N���A�҂��j
  PRINTSYS_CTRL_STREAM_CHANGE_WAIT   = (0x0002), ///< �`��E�F�C�g�ύX�i�P��j
  PRINTSYS_CTRL_STREAM_SET_WAIT      = (0x0003), ///< �`��E�F�C�g�ύX�i�i���j
  PRINTSYS_CTRL_STREAM_RESET_WAIT    = (0x0004), ///< �`��E�F�C�g���f�t�H���g�ɖ߂�
  PRINTSYS_CTRL_STREAM_CHANGE_ARGV   = (0x0005), ///< �R�[���o�b�N������ύX�i�P��j
  PRINTSYS_CTRL_STREAM_SET_ARGV      = (0x0006), ///< �R�[���o�b�N������ύX�i�i���j
  PRINTSYS_CTRL_STREAM_RESET_ARGV    = (0x0007), ///< �R�[���o�b�N�������f�t�H���g�ɖ߂�
  PRINTSYS_CTRL_STREAM_FORCE_CLEAR   = (0x0008), ///< �����`��N���A
  PRINTSYS_CTRL_STREAM_SPEED_CTRL    = (0x0009), ///< ���x�R���g���[��
}PrintSysStreamCtrlCode;


extern STRCODE PRINTSYS_GetTagStartCode( void );
extern BOOL PRINTSYS_IsWordSetTagGroup( const STRCODE* sp );
extern PrintSysTagGroup  PRINTSYS_GetTagGroup( const STRCODE* sp );
extern u8  PRINTSYS_GetTagIndex( const STRCODE* sp );
extern u16 PRINTSYS_GetTagParam( const STRCODE* sp, u16 paramIdx );
extern const STRCODE* PRINTSYS_SkipTag( const STRCODE* sp );
extern void PRINTSYS_CreateTagCode( STRBUF* str, PrintSysTagGroup tagGrp, u8 tagIdx, u8 numParams, const u16* params );

#endif
