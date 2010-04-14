//=============================================================================================
/**
 * @file	bmp_win.h
 * @brief	�r�b�g�}�b�v�E�B���h�E�V�X�e��
 */
//=============================================================================================
#ifndef _BMP_WIN_H_
#define _BMP_WIN_H_

#include <arc_tool.h>
#include <bg_sys.h>
#include <bmp.h>

#ifdef __cplusplus
extern "C" {
#endif


//----------------------------------------------------
/**
 *  �n���h���\���̂̌^�錾
 */
//----------------------------------------------------
typedef struct _GFL_BMPWIN	GFL_BMPWIN;

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
/**
 *
 * �r�b�g�}�b�v�Ǘ��V�X�e���̊J�n�A�I��
 *
 * ��Ƀv���Z�X�N���A�I�����ɌĂ΂��B
 * �r�b�g�}�b�v�E�C���h�E���g�p����a�f�t���[���i�����w��j�A�g�p�b�f�w�̈�̐ݒ���s���B
 * ����Ȍ�A�e�r�b�g�}�b�v�쐬�̍ۂ́A�����Őݒ肵���X�e�[�^�X���p������B
 */
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
/**
 * �V�X�e��������
 *
 * @param	heapID	�g�p�q�[�v�̈�
 */
//--------------------------------------------------------------------------------------------
extern void	GFL_BMPWIN_Init( HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * �V�X�e���I��
 */
//--------------------------------------------------------------------------------------------
extern void GFL_BMPWIN_Exit( void );

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�C���h�E�̍쐬�A�j��
 *
 * �r�b�g�}�b�v�Ƃ��Ďg�p�������a�f�t���[���A�`��ʒu�A�g�p�b�f�w�T�C�Y�̐ݒ�Ȃǂ��w�肵
 * �r�b�g�}�b�v�E�C���h�E�Ǘ��n���h�����쐬����B
 * �����łb�f�w�o�b�t�@�̈�̃q�[�v�m�ۂ��s���B
 * �쐬��̃r�b�g�}�b�v�E�C���h�E����́A�n���h���|�C���^�ɂ���čs����B
 */
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

//=============================================================================================
/**
 *
 * �r�b�g�}�b�v�E�C���h�E�̍쐬
 *�i�̈掩���m�۔ŁF�ʏ�͂�������g���Ă��������j
 *
 * @param[in]	frmnum	GFL_BGL�g�p�t���[��
 * @param[in]	posx	�w���W�i�L�����N�^�[�P�ʁj
 * @param[in]	posy	�x���W�i�L�����N�^�[�P�ʁj
 * @param[in]	sizx	�w�T�C�Y�i�L�����N�^�[�P�ʁj
 * @param[in]	sizy	�x�T�C�Y�i�L�����N�^�[�P�ʁj
 * @param[in]	panum	�g�p�p���b�g�i���o�[
 * @param[in]	dir		�m�ۂu�q�`�l����(GFL_BMP_CHRAREA_GET_F:�O���m�ہ@GFL_BMP_CHRAREA_GET_B:����m�ہj
 *
 * @return	�r�b�g�}�b�v�E�C���h�E�n���h���|�C���^
 */
//=============================================================================================
extern GFL_BMPWIN* GFL_BMPWIN_Create( u8 frmnum, u8 posx, u8 posy, u8 sizx, u8 sizy, u8 palnum, u8 dir );

//=============================================================================================
/**
 * �r�b�g�}�b�v�E�C���h�E�̍쐬
 *�i�̈�Œ�ŁF����̈ڐA�Ȃǂ̂��߂Ɏb��I�ɍ�����B����ȊO�ɂ͎g��Ȃ��悤�ɂ��肢���܂��B�j
 *
 * @param[in]	frmnum	GFL_BGL�g�p�t���[��
 * @param[in]	posx		�w���W�i�L�����N�^�[�P�ʁj
 * @param[in]	posy		�x���W�i�L�����N�^�[�P�ʁj
 * @param[in]	sizx		�w�T�C�Y�i�L�����N�^�[�P�ʁj
 * @param[in]	sizy		�x�T�C�Y�i�L�����N�^�[�P�ʁj
 * @param[in]	panum		�g�p�p���b�g�i���o�[
 * @param[in]	charPos	�g�p����擪�L�����i���o
 *
 * @retval  GFL_BMPWIN*		
 *
 */
//=============================================================================================
extern GFL_BMPWIN* GFL_BMPWIN_CreateFixPos( u8 frmnum, u8 posx, u8 posy, u8 sizx, u8 sizy, u8 palnum, u32 charPos );


//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�C���h�E�̔j��
 *
 * @param	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 */
//--------------------------------------------------------------------------------------------
extern void GFL_BMPWIN_Delete( GFL_BMPWIN* bmpwin );

//--------------------------------------------------------------------------------------------
/**
 *
 * �`�搧��
 *
 */
//--------------------------------------------------------------------------------------------
//---------------------------------------------------------
/**
 * �r�b�g�}�b�v�p�L�����N�^�[������o�b�t�@����u�q�`�l�֓]��
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 */
//---------------------------------------------------------
extern	void	GFL_BMPWIN_TransVramCharacter( GFL_BMPWIN * bmpwin );

//---------------------------------------------------------
/**
 * �r�b�g�}�b�v�p�X�N���[�����֘A�a�f�X�N���[���o�b�t�@�ɍ쐬����
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 */
//---------------------------------------------------------
extern	void	GFL_BMPWIN_MakeScreen( GFL_BMPWIN * bmpwin );

//---------------------------------------------------------
/**
 * �E�B���h�E�g�̃X�N���[���f�[�^���֘A�a�f�X�N���[���o�b�t�@�ɍ쐬����
 *
 * @param[in]	win				�r�b�g�}�b�v�E�C���h�E�|�C���^
 * @param[in]	start_char_num	�E�B���h�E�g�J�n�L�����i���o�[
 * @param[in]	pal_num			�E�B���h�E�g�p���b�g�i���o�[
 *
 *  �E�B���h�E�g�L�����N�^�[�f�[�^�́A
 * �y����(0)�C���(1)�C�E��(2)�C����(3)�C�E��(4)�A����(5)�C����(6)�C�E��(7)�z
 *  �̏��ɕ���ł��邱�Ƃ�z�肵�Ă���B
 *                                                  0111112
 *                                                  3     4
 *                                                  3     4
 *                                                  5666667
 */
//---------------------------------------------------------
extern	void	GFL_BMPWIN_MakeFrameScreen( GFL_BMPWIN* win, u16 start_char_num, u16 pal_num );

//---------------------------------------------------------
/**
 * �֘A�a�f�X�N���[���o�b�t�@�̃r�b�g�}�b�v�p�X�N���[�����N���A����
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 */
//---------------------------------------------------------
extern	void	GFL_BMPWIN_ClearScreen( GFL_BMPWIN * bmpwin );

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�C���h�E�X�e�[�^�X�擾
 */
//--------------------------------------------------------------------------------------------
//--------------------------------
/**
 *
 * �t���[���i���o�[�̎擾
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 *
 * @retval	�t���[���i���o�[
 */
//--------------------------------
extern	u8	GFL_BMPWIN_GetFrame( const GFL_BMPWIN * bmpwin );

//--------------------------------
/**
 * BMP�̈��X�T�C�Y�̎擾
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 *
 * @retval	BMP�̈��X�T�C�Y
 */
//--------------------------------
extern	u8	GFL_BMPWIN_GetSizeX( const GFL_BMPWIN * bmpwin );

//--------------------------------
/**
 * BMP�̈��Y�T�C�Y�̎擾
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 *
 * @retval	BMP�̈��Y�T�C�Y
 */
//--------------------------------
extern	u8	GFL_BMPWIN_GetSizeY( const GFL_BMPWIN * bmpwin );

//--------------------------------
/**
 * �X�N���[��X�T�C�Y�̎擾
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 *
 * @retval	�X�N���[��X�T�C�Y
 */
//--------------------------------
extern	u8	GFL_BMPWIN_GetScreenSizeX( const GFL_BMPWIN * bmpwin );

//--------------------------------
/**
 * �X�N���[��Y�T�C�Y�̎擾
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 *
 * @retval	�X�N���[��Y�T�C�Y
 */
//--------------------------------
extern	u8	GFL_BMPWIN_GetScreenSizeY( const GFL_BMPWIN * bmpwin );

//--------------------------------
/**
 * �`��X�T�C�Y�̎擾
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 *
 * @retval	�`��X�T�C�Y
 */
//--------------------------------
extern	u8	GFL_BMPWIN_GetPosX( const GFL_BMPWIN * bmpwin );

//--------------------------------
/**
 * �`��Y�T�C�Y�̎擾
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 *
 * @retval	�`��Y�T�C�Y
 */
//--------------------------------
extern	u8	GFL_BMPWIN_GetPosY( const GFL_BMPWIN * bmpwin );

//--------------------------------
/**
 * �L�����N�^�[�i���o�[�̎擾
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 *
 * @retval	�L�����N�^�[�i���o�[
 */
//--------------------------------
extern	u16	GFL_BMPWIN_GetChrNum( const GFL_BMPWIN * bmpwin );

//--------------------------------
/**
 * �r�b�g�}�b�v�|�C���^�̎擾
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 *
 * @retval	�r�b�g�}�b�v�|�C���^
 */
//--------------------------------
extern	GFL_BMP_DATA*	GFL_BMPWIN_GetBmp( GFL_BMPWIN * bmpwin );

//--------------------------------
/**
 * �p���b�g�i���o�[�̎擾
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 *
 * @retval	�p���b�g�i���o�[
 */
//--------------------------------
extern u8 GFL_BMPWIN_GetPalette( const GFL_BMPWIN * bmpwin );

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�C���h�E�X�e�[�^�X�ύX
 */
//--------------------------------------------------------------------------------------------
//--------------------------------
/**
 * �`��X���W�̕ύX
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 * @param[in]	px		�Z�b�g����`��X���W
 */
//--------------------------------
extern	void	GFL_BMPWIN_SetPosX( GFL_BMPWIN * bmpwin, u8 px );

//--------------------------------
/**
 * �`��Y���W�̕ύX
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 * @param[in]	py		�Z�b�g����`��Y���W
 */
//--------------------------------
extern	void	GFL_BMPWIN_SetPosY( GFL_BMPWIN * bmpwin, u8 py );

//--------------------------------
/**
 * �X�N���[���`��T�C�YX�̕ύX
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 * @param[in]	sx		�Z�b�g����X�N���[���`��T�C�YX
 */
//--------------------------------
extern	void	GFL_BMPWIN_SetScreenSizeX( GFL_BMPWIN * bmpwin, u8 sx );

//--------------------------------
/**
 * �X�N���[���`��T�C�Y�x�̕ύX
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 * @param[in]	sy		�Z�b�g����X�N���[���`��T�C�YY
 */
//--------------------------------
extern	void	GFL_BMPWIN_SetScreenSizeY( GFL_BMPWIN * bmpwin, u8 sy );

//--------------------------------
/**
 * �p���b�g�i���o�[�̕ύX
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 * @param[in]	palnum	�Z�b�g����p���b�g�i���o�[
 */
//--------------------------------
extern	void	GFL_BMPWIN_SetPalette( GFL_BMPWIN * bmpwin, u8 palnum );

//--------------------------------------------------------------------------------------------
/**
 *
 * �`�搧��⏕
 *
 */
//--------------------------------------------------------------------------------------------
//---------------------------------------------------------
/**
 * BMPWIN�̃L������]���E�X�N���[����Make�E�]�����ꊇ�ōs��
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 */
//---------------------------------------------------------
inline	void	GFL_BMPWIN_MakeTransWindow( GFL_BMPWIN * bmpwin )
{
	GFL_BMPWIN_TransVramCharacter( bmpwin );
	GFL_BMPWIN_MakeScreen( bmpwin );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(bmpwin) );
}

//---------------------------------------------------------
/**
 * BMPWIN�̃L������]���E�X�N���[����Make�E�]�����ꊇ�ōs��(VBlank�p
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 */
//---------------------------------------------------------
inline	void	GFL_BMPWIN_MakeTransWindow_VBlank( GFL_BMPWIN * bmpwin )
{
	GFL_BMPWIN_TransVramCharacter( bmpwin );
	GFL_BMPWIN_MakeScreen( bmpwin );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(bmpwin) );
}

//---------------------------------------------------------
/**
 * �X�N���[����Clear�E�]�����ꊇ�ōs��
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 */
//---------------------------------------------------------
inline	void	GFL_BMPWIN_ClearTransWindow( GFL_BMPWIN * bmpwin )
{
	GFL_BMPWIN_ClearScreen( bmpwin );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(bmpwin) );
}

//---------------------------------------------------------
/**
 * �X�N���[����Clear�E�]�����ꊇ�ōs��(VBlank�p
 *
 * @param[in]	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 */
//---------------------------------------------------------
inline	void	GFL_BMPWIN_ClearTransWindow_VBlank( GFL_BMPWIN * bmpwin )
{
	GFL_BMPWIN_ClearScreen( bmpwin );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(bmpwin) );
}


#ifdef __cplusplus
}/* extern "C" */
#endif

#endif
