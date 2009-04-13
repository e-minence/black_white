//============================================================================================
/**
 * @file	snd_status.c
 * @brief	�T�E���h�X�e�[�^�X�r���[���[
 * @author	
 * @date	
 */
//============================================================================================
//
//	�g�p�̍ۂɂ�graphic�t�H���_����sndstatus.narc�����[�g�ɔz�u���Ă�������
//	�i"sndstatus.narc"�p�X�w��Ŏ擾�ł���ꏊ�j
//
//	�g�pBG�ʂ̃R���g���[�����W�X�^�͌Ăяo�����Őݒ肳�ꂽ��ԂɈˑ����܂��B
//	
typedef struct _GFL_SNDVIEWER GFL_SNDVIEWER;
typedef NNSSndHandle*	GFL_SNDVIEWER_GETSNDHANDLE_FUNC(void);
typedef u32				GFL_SNDVIEWER_GETPLAYERNO_FUNC(void);
typedef BOOL			GFL_SNDVIEWER_GETREVERBFLAG_FUNC(void);

typedef enum {
	GFL_SNDVIEWER_CONTROL_NONE		= 0x0000,	// �R���g���[���Ȃ�
	GFL_SNDVIEWER_CONTROL_ENABLE	= 0x0001,	// ���삠��
	GFL_SNDVIEWER_CONTROL_EXIT		= 0x0002,	// �d�w�h�s��t����

}GFL_SNDVIEWER_CONTROL;

//�@�ݒ��`
typedef struct {
	u32					cancelKey;		//���f�L�[����g���K(0�ŃL�[���f�s��)

	GFL_DISPUT_BGID		bgID;			//�g�pBGVRAMID
	GFL_DISPUT_PALID	bgPalID;		//�g�pBG�p���b�gID

	GFL_SNDVIEWER_GETSNDHANDLE_FUNC*	getSndHandleFunc;
	GFL_SNDVIEWER_GETPLAYERNO_FUNC*		getPlayerNoFunc;
	GFL_SNDVIEWER_GETREVERBFLAG_FUNC*	getReverbFlagFunc;

	u16					controlFlag;	//GFL_SNDVIEWER_CONTROL�̑g�ݍ��킹

}GFL_SNDVIEWER_SETUP;

//============================================================================================
/**
 *
 * @brief	�V�X�e���N��&�I��
 *	
 */
//============================================================================================
extern GFL_SNDVIEWER*	GFL_SNDVIEWER_Create( const GFL_SNDVIEWER_SETUP* setup, HEAPID heapID );
extern void				GFL_SNDVIEWER_Delete( GFL_SNDVIEWER* gflSndViewer );
extern BOOL				GFL_SNDVIEWER_Main( GFL_SNDVIEWER* gflSndViewer );	//FALSE�ŏI��

extern void	GFL_SNDVIEWER_InitControl( GFL_SNDVIEWER* gflSndViewer );
extern void	GFL_SNDVIEWER_InitReverbControl( GFL_SNDVIEWER* gflSndViewer );
extern u16	GFL_SNDVIEWER_GetControl( GFL_SNDVIEWER* gflSndViewer );
extern void	GFL_SNDVIEWER_SetControl( GFL_SNDVIEWER* gflSndViewer, u16 flag );
extern void	GFL_SNDVIEWER_ChangeSndHandle( GFL_SNDVIEWER* gflSndViewer, NNSSndHandle* pBgmHandle );
