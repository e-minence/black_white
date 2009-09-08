
//============================================================================================
/**
 * @file	gf_mcs.h
 * @brief	MCS�֘A�֐�
 * @author	soga
 * @date	2009.03.06
 */
//============================================================================================

#ifndef __GF_MCS_H_
#define __GF_MCS_H_

//============================================================================================
/**
 *	�萔�錾
 */
//============================================================================================
// Windows�A�v���P�[�V�����Ƃ̎��ʂŎg�p����`�����l���l�ł�
enum{
	MCS_CHANNEL0 = 0,
	MCS_CHANNEL1,
	MCS_CHANNEL2,
	MCS_CHANNEL3,
	MCS_CHANNEL4,
	MCS_CHANNEL5,
	MCS_CHANNEL6,
	MCS_CHANNEL7,
	MCS_CHANNEL8,
	MCS_CHANNEL9,
	MCS_CHANNEL10,
	MCS_CHANNEL11,
	MCS_CHANNEL12,
	MCS_CHANNEL13,
	MCS_CHANNEL14,
	MCS_CHANNEL15,

	MCS_CHANNEL_END,
};

extern	BOOL	MCS_Init( HEAPID heapID );
extern	void	MCS_Close( void );
extern	void	MCS_Exit( void );
extern	void	MCS_Main( void );
extern	u32		MCS_Read( void *buf, int size );
extern	BOOL	MCS_Write( int ch, const void *buf, int size );
extern	u32		MCS_CheckRead( void );
extern	BOOL	MCS_CheckEnable( void );

typedef	u16 GFL_MCS_LINKIDX;
#define GFL_MCS_LINKIDX_INVALID (0xffff)

//============================================================================================
/**
 *	�V�X�e���֐�
 *	 system/gfl_use.c������Ă΂�Ă���V�X�e���֐�
 */
//============================================================================================
extern void	GFL_MCS_Init( void );
extern void GFL_MCS_Main( void );
extern void	GFL_MCS_VIntrFunc( void );
extern void	GFL_MCS_Exit( void );

//============================================================================================
/**
 *	�f�o�C�X�I�[�v��
 *		MCS�f�o�C�X���I�[�v�����ʐM�J�n����������
 */
//============================================================================================
extern BOOL	GFL_MCS_Open( void );
//============================================================================================
/**
 *	�f�o�C�X�N���[�Y
 *		MCS�f�o�C�X���N���[�Y���I������������
 */
//============================================================================================
extern void	GFL_MCS_Close( void );

//============================================================================================
/**
 *	PC�Ƃ̃����N���m�F����
 */
//============================================================================================
extern BOOL GFL_MCS_Link( u32 categoryID );

//============================================================================================
/**
 *	�f�[�^�ǂݍ��݃`�F�b�N�i��M�f�[�^�̃T�C�Y���擾����j
 *  [IN]  categoryID	: �C�ӂ̎��ʗp�J�e�S���h�c�iPC�A�v���Ɠ���̂��́j
 *
 *		��Ɏ�M�f�[�^�ʂɍ��킹�ă������A���P�[�g����ꍇ��z�肵�Ă���
 *		���Ăяo���ۂ�GFL_MCS_Read�̑O�Ɏ��s���邱��
 *		�����̊֐����Ă΂Ȃ��Ă�GFL_MCS_Read�͎��s�\�i��M�p�o�b�t�@�T�C�Y���Œ�̏ꍇ�Ȃǁj
 */
//============================================================================================
extern int	GFL_MCS_CheckReadable( u32 categoryID );
//============================================================================================
/**
 *	�f�[�^�ǂݍ���
 *  [IN]  categoryID	: �C�ӂ̎��ʗp�J�e�S���h�c�iPC�A�v���Ɠ���̂��́j
 *				pWriteBuf		: �ǂݍ��݃f�[�^�̊i�[�|�C���^
 *				readBufSize	: ��L�ǂݍ��݃o�b�t�@�T�C�Y
 *
 *		�����O��GFL_MCS_CheckReadable���Ă΂�Ă��Ȃ��ꍇ�͂��̊֐����ŌĂяo���Ă���
 */
//============================================================================================
extern BOOL	GFL_MCS_Read( u32 categoryID, void* pReadBuf, u32 readBufSize );
//============================================================================================
/**
 *	�f�[�^��������
 *  [IN]  categoryID	: �C�ӂ̎��ʗp�J�e�S���h�c�iPC�A�v���Ɠ���̂��́j
 *				pWriteBuf		: �������݃f�[�^�̊i�[�|�C���^
 *				writeSize		: �������݃f�[�^�T�C�Y
 */
//============================================================================================
extern BOOL	GFL_MCS_Write( u32 categoryID, const void* pWriteBuf, u32 writeSize );


#endif __GF_MCS_H_
