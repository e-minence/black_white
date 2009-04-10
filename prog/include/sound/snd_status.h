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
typedef struct _GFL_SNDSTATUS GFL_SNDSTATUS;

typedef enum {
	GFL_SNDSTATUS_CONTROL_NONE		= 0x0000,	// �R���g���[���Ȃ�
	GFL_SNDSTATUS_CONTROL_ENABLE	= 0x0001,	// ���삠��
	GFL_SNDSTATUS_CONTROL_EXIT		= 0x0002,	// �d�w�h�s��t����

}GFL_SNDSTATUS_CONTROL;

//�@�ݒ��`
typedef struct {
	u32					cancelKey;		//���f�L�[����g���K(0�ŃL�[���f�s��)

	GFL_DISPUT_BGID		bgID;			//�g�pBGVRAMID
	GFL_DISPUT_PALID	bgPalID;		//�g�pBG�p���b�gID

	NNSSndHandle*		pBgmHandle;

	u16					controlFlag;	//GFL_SNDSTATUS_CONTROL�̑g�ݍ��킹

}GFL_SNDSTATUS_SETUP;

//============================================================================================
/**
 *
 * @brief	�V�X�e���N��&�I��
 *	
 */
//============================================================================================
extern GFL_SNDSTATUS*	GFL_SNDSTATUS_Create( const GFL_SNDSTATUS_SETUP* setup, HEAPID heapID );
extern void				GFL_SNDSTATUS_Delete( GFL_SNDSTATUS* gflSndStatus );
extern BOOL				GFL_SNDSTATUS_Main( GFL_SNDSTATUS* gflSndStatus );	//FALSE�ŏI��

extern void	GFL_SNDSTATUS_InitControl( GFL_SNDSTATUS* gflSndStatus );
extern void	GFL_SNDSTATUS_InitReverbControl( GFL_SNDSTATUS* gflSndStatus );
extern u16	GFL_SNDSTATUS_GetControl( GFL_SNDSTATUS* gflSndStatus );
extern void	GFL_SNDSTATUS_SetControl( GFL_SNDSTATUS* gflSndStatus, u16 flag );
extern void	GFL_SNDSTATUS_ChangeSndHandle( GFL_SNDSTATUS* gflSndStatus, NNSSndHandle* pBgmHandle );
