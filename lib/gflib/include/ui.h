//==============================================================================
/**
 *@file		ui.h
 *@brief	���[�U�[�C���^�[�t�F�C�X�p���J�֐� ���J�e�[�u��
 *@author	k.ohno
 *@date		2006.11.16
 */
//==============================================================================

#pragma once

#include "heap.h"

#ifdef __cplusplus
extern "C" {
#endif



//-----------------------------------------------------------------------------
/**
 * @brief �萔�錾
 */
//-----------------------------------------------------------------------------


#define _GFI_FADE_BASESPEED  (6)  //�\�t�g���Z�b�g�̍ۂ̃t�F�[�h�X�s�[�h

/**
 * @brief �\�t�g�E�G�A���Z�b�g�񋖉p
 */
typedef enum {
    GFL_UI_SOFTRESET_WIFI = 0x01,   ///< WIFI�ʐM��
    GFL_UI_SOFTRESET_SVLD = 0x02,   ///< �Z�[�u���[�h������
    GFL_UI_SOFTRESET_MB   = 0x04,   ///< �}���`�u�[�g�q�@
    GFL_UI_SOFTRESET_USER = 0x08,  ///< ��������͊O����`�ɂȂ�
} GF_UI_SOFTRESET_e;


/**
 * @brief �X���[�v�񋖉p
 */
typedef enum {
  GFL_UI_SLEEP_SVLD = 0x01,      ///< �Z�[�u���[�h
  GFL_UI_SLEEP_AGBROM = 0x02,    ///< AGB�A�N�Z�X
  GFL_UI_SLEEP_NET = 0x04,       ///< �ʐM�S��
  GFL_UI_SLEEP_MIC = 0x08,       ///< MIC
  GFL_UI_SLEEP_MB  = 0x10,       ///< �}���`�u�[�g�q�@
  GFL_UI_SLEEP_NETIRC = 0x20,    ///< �ԊO�����C�����X
  GFL_UI_SLEEP_USER = 0x40,      ///< ��������͊O����`�ɂȂ�
} GF_UI_SLEEP_e;


/**
 * @brief �L�[�J�X�^�}�C�Y�e�[�u���p
 */
typedef enum {
    GFL_UI_KEY_END = 0,     ///< �e�[�u���̏I��
    GFL_UI_KEY_COPY,     ///< �P����Q�փL�[���R�s�[���܂�
    GFL_UI_KEY_CHANGE,             ///< �P�ƂQ���������܂�
    GFL_UI_KEY_RESET          ///< �P�������܂�
} GF_UI_KEY_CUSTOM_e;

/**
 * @brief �^�b�`�p�l���e�[�u���p
 */
typedef enum {
    GFL_UI_TP_HIT_END = 0xff,			///< �e�[�u���I���R�[�h
    GFL_UI_TP_USE_CIRCLE = 0xfe,			///< �~�`�Ƃ��Ďg��
    GFL_UI_TP_SKIP = 0xfd,			///< �X�L�b�v����
    GFL_UI_TP_HIT_NONE = -1	///< ���������e�[�u���Ȃ�
} GF_UI_TP_CUSTOM_e;

/**
 * @brief �o�b�e���[�p
 */
typedef enum{
  GFL_UI_BATTLEVEL_EMP,    ///< �o�b�e���[�������
  GFL_UI_BATTLEVEL_LO2,
  GFL_UI_BATTLEVEL_LO,
  GFL_UI_BATTLEVEL_MID2,
  GFL_UI_BATTLEVEL_MID,
  GFL_UI_BATTLEVEL_HI  ///< �o�b�e���[���\��
}GFL_UI_BATTLEVEL_e;





#define	PAD_BUTTON_DECIDE	( PAD_BUTTON_A )	///< ����{�^����`
#define	PAD_BUTTON_CANCEL	( PAD_BUTTON_B )	///< �L�����Z���{�^����`
#define PAD_BUTTON_SOFTRESET   (PAD_BUTTON_START|PAD_BUTTON_SELECT|PAD_BUTTON_L|PAD_BUTTON_R) //�\�t�g���Z�b�g�{�^����`

//-----------------------------------------------------------------------------
/**
 *	@brief �\���̐錾
 */
//-----------------------------------------------------------------------------

/**
 * @struct GFL_UI_KEY_CUSTOM_TBL
 * @brief  �L�[�f�[�^�\����  �L�[�R���t�B�O�p�^�[�����w�肷��e�[�u��.
 * @note�g�p��
 <BR> �z��v�f�D�揇��:0>1>2>3>4>.....�ł�
 <BR> GFL_UI_KEY_CUSTOM_TBL key_data[] ={
 <BR>		{PAD_BUTTON_SELECT,PAD_BUTTON_START,GFL_UI_KEY_CHANGE},		//START SELECT ����
 <BR>		{PAD_BUTTON_L,PAD_BUTTON_A,GFL_UI_KEY_COPY},		//L��A������
 <BR>		{PAD_BUTTON_R,0,GFL_UI_KEY_RESET},		//L��A������
 <BR>		{GFL_UI_TP_HIT_END,0,0},		// �I���f�[�^
 <BR> };
 */
typedef struct{
    int keySource;
    int keyDist;
    u8 mode;
}GFL_UI_KEY_CUSTOM_TBL;

/**
 * @struct GFL_UI_TP_HITTBL
 * @brief  �^�b�`�p�l��������͈͂���`�A�~�`�Ŏw�肷��e�[�u��.
 * @note  �g�p��
 <BR>  �z��v�f�D�揇��:0>1>2>3>4>.....�ł� 
 <BR>  GFL_UI_TP_HITTBL tp_data[] ={
 <BR>		{128,191,0,64},
 <BR>		{32,94,129,200},
 <BR>		{0,191,0,255},
 <BR>       {TP_USE_CIRCLE, 100, 80, 16 },	 �~�`�Ƃ��Ďg���B
 <BR>		{GFL_UI_TP_HIT_END,0,0,0},		 �I���f�[�^
 <BR>  };
 * 
 */
typedef union{
	struct {
		u8	top;	///< ��i�������͓���R�[�h�j
		u8	bottom;	///< ��
		u8	left;	///< ��
		u8	right;	///< �E
	}rect;

	struct {
		u8	code;	///< TP_USE_CIRCLE ���w��
		u8	x;      ///< ���Sx���W
		u8	y;      ///< ���Sy���W
		u8	r;      ///< ���a
	}circle;
}GFL_UI_TP_HITTBL;

typedef struct _UI_SYS UISYS;  ///< UISYS �^�錾
typedef struct _UI_TPSYS UI_TPSYS; ///< UI�^�b�`�p�l��SYS�^�錾
typedef void (GFL_UI_SLEEPRELEASE_FUNC)(void* pWork);  ///< ���Z�b�g���A���ɌĂ΂��֐�
typedef void (GFL_UI_SOFTRESET_CALLBACK)( void *work ); ///<�\�t�g���Z�b�g�R�[���o�b�N�֐�
typedef BOOL (GFL_UI_NOTSLEEPCHK_FUNC)(void* pWork);  ///< �X���[�v���O�ɌĂ΂��֐�


//==============================================================================
/**
 * @brief UI�N������(UI�ɂ͏�������boot���ɍs�����̂�������܂���)
 * @param   heapID    �q�[�v�m�ۂ��s��ID
 * @param   isMbBoot    �}���`�u�[�g�̎q�@�N�����H(���Ń\�t���ƃX���[�v�̋֎~�t���O�𗧂Ă�
 * @return  UISYS  work�n���h��
 */
//==============================================================================
extern void GFL_UI_Boot(const HEAPID heapID , const BOOL isMbBoot );

//==============================================================================
/**
 * @brief UIMain����
 * @return  none
 */
//==============================================================================
extern void GFL_UI_Main(void);

//==============================================================================
/**
 * @brief   UI�I������
 * @return  none
 */
//==============================================================================

extern void GFL_UI_Exit(void);

//------------------------------------------------------------------
/**
 * @brief   �X���[�v��Ԃ��֎~����
 * @param   sleepTypeBit �X���[�v�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SleepDisable(const u8 sleepTypeBit);

//------------------------------------------------------------------
/**
 * @brief   �X���[�v��Ԃ�������
 * @param   sleepTypeBit �X���[�v�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SleepEnable(const u8 sleepTypeBit);

//------------------------------------------------------------------
/**
 * @brief   �X���[�v�Ǘ�BIT�ɋ֎~�t���O�������Ă��邩���ׂ�
 * @param   sleepTypeBit �X���[�v�Ǘ�BIT
 * @return  TRUE:�֎~�t���O�������Ă���
 * @return  TRUE:�֎~�t���O�͗����Ă��Ȃ�
 */
//------------------------------------------------------------------
extern BOOL GFL_UI_CheckSleepDisable(const u8 sleepTypeBit);

//------------------------------------------------------------------
/**
 * @brief   �X���[�v�������ɌĂ΂��֐����Z�b�g����
 * @param   pFunc   �X���[�v�������ɌĂ΂��֐�
 * @param   pWork   �Ă΂��ۂɓn�����[�N
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SleepReleaseSetFunc(GFL_UI_SLEEPRELEASE_FUNC* pFunc, void* pWork);

//------------------------------------------------------------------
/**
 * @brief   �X���[�v���O���ɌĂ΂��֐����Z�b�g����
 * @param   pFunc   �X���[�v���O���ɌĂ΂��֐�
 * @param   pWork   �Ă΂��ۂɓn�����[�N
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SleepGoSetFunc(GFL_UI_SLEEPRELEASE_FUNC* pFunc, void* pWork);


//------------------------------------------------------------------
/**
 * @brief   �X���[�v�������ɌĂ΂��T�E���h��p�֐����Z�b�g����
 * @param   pFunc   �X���[�v�������ɌĂ΂��֐�
 * @param   pWork   �Ă΂��ۂɓn�����[�N
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SleepSoundReleaseSetFunc(GFL_UI_SLEEPRELEASE_FUNC* pFunc, void* pWork);

//------------------------------------------------------------------
/**
 * @brief   �X���[�v���O���ɌĂ΂��T�E���h��p�֐����Z�b�g����
 * @param   pFunc   �X���[�v���O���ɌĂ΂��֐�
 * @param   pWork   �Ă΂��ۂɓn�����[�N
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SleepSoundGoSetFunc(GFL_UI_SLEEPRELEASE_FUNC* pFunc, void* pWork);

//------------------------------------------------------------------
/**
 * @brief   �\�t�g�E�G�A���Z�b�g��Ԃ��֎~����
 * @param   softResetBit ���Z�b�g�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SoftResetDisable(const u8 softResetBit);

//------------------------------------------------------------------
/**
 * @brief   �\�t�g�E�G�A���Z�b�g��Ԃ�������
 * @param   softResetBit ���Z�b�g�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SoftResetEnable( const u8 softResetBit);

//------------------------------------------------------------------
/**
 * @brief   �\�t�g�E�G�A���Z�b�g�Ǘ�BIT�ɋ֎~�t���O�������Ă��邩���ׂ�
 * @param   softResetBit ���Z�b�g�Ǘ�BIT
 * @return  TRUE:�֎~�t���O�������Ă���
 * @return  FALSE:�֎~�t���O�͗����Ă��Ȃ�
 */
//------------------------------------------------------------------
extern BOOL GFL_UI_CheckSoftResetDisable(const u8 softResetBit);

//------------------------------------------------------------------
/**
 * @brief   �\�t�g�E�G�A���Z�b�g���O���ɌĂ΂��֐����Z�b�g����
 * @param   pFunc   �\�t�g�E�G�A���Z�b�g���O���ɌĂ΂��֐�
 * @param   pWork   �Ă΂��ۂɓn�����[�N
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SoftResetSetFunc(GFL_UI_SOFTRESET_CALLBACK* pFunc, void* pWork);

//------------------------------------------------------------------
/**
 * @brief   �X���[�v�������ɌĂ΂��o�b�e���[�؂��p�֐����Z�b�g����
 * @param   pFunc   �X���[�v�������ɌĂ΂��֐�
 * @param   pWork   �Ă΂��ۂɓn�����[�N
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_Batt10SleepReleaseSetFunc(GFL_UI_SLEEPRELEASE_FUNC* pFunc, void* pWork);

//------------------------------------------------------------------
/**
 * @brief   �X���[�v�֎~�ɂ����p�֐����Z�b�g����
 * @param   pFunc   �X���[�v�������ɌĂ΂��֐�
 * @param   pWork   �Ă΂��ۂɓn�����[�N
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_NotSleepSetFunc(GFL_UI_NOTSLEEPCHK_FUNC* pFunc, void* pWork);


//----------------------------------------------------------------------------
/**
 * @brief	  �ӂ����J�������ǂ���
 * @param	  GFL_UI_FRAMERATE_ENUM  ��`�̃t���[��
 * @retval	none
 */
//-----------------------------------------------------------------------------
extern BOOL GFL_UI_GetOpenTrg(void);

//---------------------------------------------------------------------------
/**
 * @brief	�ӂ������Ă��邩�ǂ�����Ԃ�
 * @param	�ӂ������Ă���=TRUE
 */
//---------------------------------------------------------------------------
extern BOOL GFL_UI_CheckCoverOff(void);

//---------------------------------------------------------------------------
/**
 * @brief	�o�b�e���[���჌�x�����ǂ�����Ԃ�
 * @param	DS�ł�DSI�ł��o�b�e���[���჌�x���̏ꍇTRUE
 */
//---------------------------------------------------------------------------
extern BOOL GFL_UI_CheckLowBatt(void);

//----------------------------------------------------------------------------
//�v���g�^�C�v�錾 touchpanel.c
//----------------------------------------------------------------------------

//==============================================================================
/**
 * @brief  �^�b�`�p�l��������
 * @param   heapID    �q�[�v�m�ۂ��s��ID
 * @return  UI_TPSYS  �^�b�`�p�l���V�X�e�����[�N
 */
//==============================================================================

extern UI_TPSYS* GFL_UI_TP_Init(const HEAPID heapID);

//==============================================================================
/**
 * @brief �^�b�`�p�l���ǂݎ�菈��
 * @param   none
 * @return  none
 */
//==============================================================================

extern void GFL_UI_TP_Main(void);

//==============================================================================
/**
 * @brief �^�b�`�p�l���I������
 * @param   none
 * @return  none
 */
//==============================================================================

extern void GFL_UI_TP_Exit(void);


//==============================================================================
/**
 * @brief �I�[�g�T���v�����O���s���Ă��邩�ǂ����𓾂�
 * @param   none
 * @retval  TRUE  �I�[�g�T���v�����O�ł���
 * @retval  FALSE  ���Ă��Ȃ�
 */
//==============================================================================
extern int GFL_UI_TP_GetAutoSamplingFlg(void);

//==============================================================================
/**
 * @brief �I�[�g�T���v�����O�ݒ�
 * @param[in]    bAuto   �I�[�g�T���v�����O����Ȃ�TRUE
 * @return  none
 */
//==============================================================================
extern void GFL_UI_TP_SetAutoSamplingFlg( const BOOL bAuto);

//------------------------------------------------------------------
/**
 * @brief ���^�C�v�i��`�E�~�`�j�����Ȃ��画�肷��i�x�^���́j
 * @param[in]   tbl		�����蔻��e�[�u���i�I�[�R�[�h����j
 * @return  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 */
//------------------------------------------------------------------
extern int GFL_UI_TP_HitCont( const GFL_UI_TP_HITTBL *tbl );

//------------------------------------------------------------------
/**
 * @brief ���^�C�v�i��`�E�~�`�j�����Ȃ��画�肷��i�g���K���́j
 * @param[in]   tbl		�����蔻��e�[�u���i�I�[�R�[�h����j
 * @retval  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 */
//------------------------------------------------------------------
extern int GFL_UI_TP_HitTrg( const GFL_UI_TP_HITTBL *tbl );

//------------------------------------------------------------------
/**
 * @brief  �^�b�`�p�l���ɐG��Ă��邩
 * @param   none
 * @retval  BOOL		TRUE�ŐG��Ă���
 */
//------------------------------------------------------------------
extern BOOL GFL_UI_TP_GetCont( void );

//------------------------------------------------------------------
/**
 * @brief �^�b�`�p�l���ɐG��Ă��邩�i�g���K�j
 * @param   none
 * @retval  BOOL		TRUE�ŐG�ꂽ
 */
//------------------------------------------------------------------
extern BOOL GFL_UI_TP_GetTrg( void );

//----------------------------------------------------------------------------
/**
 *	@brief	�w�肵�����W�ŁA�����蔻����s���܂��B	����
 *	@param	tbl		�����蔻��e�[�u���i�z��j
 *	@param	x		���肘���W
 *	@param	y		���肙���W
 *	@retval  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 */
//-----------------------------------------------------------------------------
extern int GFL_UI_TP_HitSelf( const GFL_UI_TP_HITTBL *tbl, u32 x, u32 y );

//------------------------------------------------------------------
/**
 * @brief �^�b�`�p�l���ɐG��Ă���Ȃ炻�̍��W�擾�i�x�^���́j
 * @param[out]   x		�w���W�󂯎��ϐ��A�h���X
 * @param[out]   y		�x���W�󂯎��ϐ��A�h���X
 * @retval  TRUE  �G��Ă���
 * @retval  FALSE �G��Ă��Ȃ��B�����ɂ͉������Ȃ��B
 */
//------------------------------------------------------------------
extern BOOL GFL_UI_TP_GetPointCont( u32* x, u32* y );

//------------------------------------------------------------------
/**
 * @brief �^�b�`�p�l���ɐG��Ă���Ȃ炻�̍��W�擾�i�g���K���́j
 * @param[out]   x		�w���W�󂯎��ϐ��A�h���X
 * @param[out]   y		�x���W�󂯎��ϐ��A�h���X
 * @retval  TRUE  �G��Ă���
 * @retval  FALSE �G��Ă��Ȃ��B�����ɂ͉������Ȃ��B
 */
//------------------------------------------------------------------
extern BOOL GFL_UI_TP_GetPointTrg( u32* x, u32* y );

//----------------------------------------------------------------------------
//�v���g�^�C�v�錾 key.c
//----------------------------------------------------------------------------

//==============================================================================
/**
 * @brief �L�[���s�[�g�̑��x�ƃE�F�C�g���Z�b�g
 * @param[in]	speed	���x
 * @param[in]	wait	�E�F�C�g
 * @return  none
 */
//==============================================================================
extern void GFL_UI_KEY_SetRepeatSpeed(const int speed, const int wait );

//==============================================================================
/**
 * @brief �L�[�R���t�B�O�e�[�u����o�^
 * @param[in]	pTbl   �R���g���[���e�[�u���z��̃|�C���^
 * @return  none
 */
//==============================================================================
extern void GFL_UI_KEY_SetControlModeTbl(const GFL_UI_KEY_CUSTOM_TBL* pTbl );

//==============================================================================
/**
 * @brief �L�[�g���K�Q�b�g
 * @param   none
 * @return  �L�[�g���K
 */
//==============================================================================
extern int GFL_UI_KEY_GetTrg( void );

//==============================================================================
/**
 * @brief �L�[�R���g�Q�b�g
 * @param   none
 * @return  �L�[�R���g
 */
//==============================================================================
extern int GFL_UI_KEY_GetCont(void);

//==============================================================================
/**
 * @brief �L�[���s�[�g�Q�b�g
 * @param   none
 * @return  �L�[�R���g
 */
//==============================================================================
extern int GFL_UI_KEY_GetRepeat( void );

//==============================================================================
/**
 * @brief �L�[���s�[�g�̑��x�ƃE�F�C�g���Q�b�g
 * @param[out]	speed	���x
 * @param[out]	wait	�E�F�C�g
 * @return  none
 */
//==============================================================================
extern void GFL_UI_KEY_GetRepeatSpeed(int* speed, int* wait );

//==============================================================================
/**
 * @brief   �L�[�R���t�B�O�̃��[�h�̒l��ݒ肷��
 * @param[in]   mode    �L�[�R���t�B�O���[�h
 * @return  none
 */
//==============================================================================
extern void GFL_UI_KEY_SetControlMode(const int mode);

//==============================================================================
/**
 * @brief �L�[�R���t�B�O�̃��[�h�̒l�𓾂�
 * @param   none
 * @return  �R���g���[�����[�h
 */
//==============================================================================
extern int GFL_UI_KEY_GetControlMode( void );






/*-----------------------------------------------------------------------------
 *					�萔�錾
 ----------------------------------------------------------------------------*/
#define UI_REPEAT_SPEED_DEF (8)   //�L�[���s�[�g�̃f�t�H���g
#define UI_REPEAT_WAIT_DEF (15)   //�L�[���s�[�g�̃f�t�H���g



#define TP_REQUEST_CHECK_MAX		5		// ���߂��o���Ď��s������G���[��Ԃ���

// �P�V���N�Ɏ擾�ł���f�[�^�ő吔
//  1/60�Ȃ�4 1/30�Ȃ�8
#define		TP_ONE_SYNC_DATAMAX	(8)

// �P�V���N�ɃT���v�����O�ł���ő�̐���
// �T���v�����O����̂ɕK�v�ȃo�b�t�@�T�C�Y
//  TP_ONE_SYNC_DATAMAX + 1
#define		TP_ONE_SYNC_BUFF (TP_ONE_SYNC_DATAMAX + 1)



//-------------------------------------
/// �G���[�̗�
enum
{
	TP_END_BUFF = 0xffffffff,	// �T���v�����O�p�o�b�t�@����t
	TP_ERR = 0,					// ARM7�]���ȊO�̃G���[
	TP_OK,						// Touch�p�l���֐���OK
	TP_FIFO_ERR,				// ARM7��FIFO���ߓ]���G���[	
	TP_SAMP_NOT_START			// �T���v�����O�J�n����Ă��܂���
};

//-------------------------------------
/// �T���v�����O��ʗ�
enum{
	TP_SAMP_NONE,			// �T���v�����O���Ă��Ȃ�
	TP_BUFFERING,			// �o�b�t�@�i�[���[�h
	TP_NO_BUFF,				// �i�[�����Ȃ����[�h
	TP_NO_LOOP,				// �o�b�t�@�����[�v�����Ȃ�
	TP_BUFFERING_JUST,		// ���̂܂܃o�b�t�@�����O����(�����Ă��Ă��o�b�t�@�����O)
	TP_NO_LOOP_JUST,		// ���̂܂܃o�b�t�@�����O����(�����Ă��Ă��o�b�t�@�����O)
							// ���[�v�����Ȃ�
};
// ���̃o�[�W�����ł�TP_BUFFERING_JUST,	TP_NO_LOOP_JUST���g�p�����f�[�^�œ��O����͂ł��Ȃ�

/*-----------------------------------------------------------------------------
 *					�\���̐錾
 ----------------------------------------------------------------------------*/
//-------------------------------------
/// �P�t���[���̃^�b�`���i�[�\����
typedef struct
{
	u16		Size;			// ���̃t���[���̗L���T���v�����O��
	TPData	TPDataTbl[ TP_ONE_SYNC_DATAMAX ];	// ���̃t���[���̃T���v�����O�f�[�^
} TP_ONE_DATA;


//----------------------------------------------------------------------------
/**
 *
 * @brief	�T���v�����O�����Ǘ����A���̏�Ԃ�Ԃ�	
 * @param	pData�F���̃t���[���̏��(init�Ŏw�肵���T���v�����O�񐔕��̏��)
 * @param	type�F�T���v�����O��ʂ̔ԍ�
 * @param	comp_num�F�o�b�t�@�Ɋi�[����Ƃ��ɁAcomp_num�ʂ̂�����������i�[����
 * @return	u32�F�T���v�����O��ʂɂ��ω�
					type�FTP_BUFFERING		�T���v�����O���ꂽ�o�b�t�@�T�C�Y
					type�FTP_NO_LOOP		�T���v�����O���ꂽ�o�b�t�@�T�C�Y
											�o�b�t�@����t�ɂȂ����Ƃ� TP_END_BUFF
					type�FTP_NO_BUFF		TP_OK

					type�FTP_SAMP_NOT_START	�T���v�����O�J�n����Ă��܂���
 */
//-----------------------------------------------------------------------------
extern u32 GFL_UI_TP_AutoSamplingMain( TP_ONE_DATA* pData, u32 type, u32 comp_num );

//----------------------------------------------------------------------------
/**
 * @brief	�X���[�v������̍ĊJ����
 * @param   none
 * @return	none
 */
//-----------------------------------------------------------------------------
extern void GFL_UI_TP_AutoSamplingReStart( void );

//----------------------------------------------------------------------------
/**
 * @brief	�X���[�v�����O�̒�~����
 * @param   none
 * @return  none
 */
//-----------------------------------------------------------------------------
extern void GFL_UI_TP_AutoSamplingStop( void );

//----------------------------------------------------------------------------
/**
 * @brief	�^�b�`�p�l����AUTO�T���v�����O�J�n
 * @param	p_buff�F�T���v�����O�f�[�^������o�b�t�@
 * @param	size�F�o�b�t�@�̃T�C�Y
 * @retval	TP_OK�F����
 * @retval	TP_FIFO_ERR�F�]�����s
 * @retval	TP_ERR�F�]���ȊO�̎��s
 */
//-----------------------------------------------------------------------------
extern u32 GFL_UI_TP_AutoStart( TPData* p_buff, u32 size);

//----------------------------------------------------------------------------
/**
 * @brief	�^�b�`�p�l����AUTO�T���v�����O�J�n	�o�b�t�@�����O�Ȃ�
 * @retval	TP_OK�F����
 * @retval	TP_FIFO_ERR�F�]�����s
 * @retval	TP_ERR�F�]���ȊO�̎��s
 */
//-----------------------------------------------------------------------------
extern u32 GFL_UI_TP_AutoStartNoBuff(void);

//----------------------------------------------------------------------------
/**
 *@brief	�T���v�����O���I������
 *@param	none
 *@retval	TP_OK�F����
 *@retval   TP_FIFO_ERR�F�]�����s
 *@retval	TP_ERR�F�]���ȊO�̎��s
 */
//-----------------------------------------------------------------------------
extern u32 GFL_UI_TP_AutoStop( void );



//----------------------------------------------------------------------------
//�L�[�ƃ^�b�`�p�l�������g�p������p
//----------------------------------------------------------------------------

// GS��common.h����ڐA 090116Ariizumi
// �A�v���P�[�V�����̏I�����L�[�ōs�������A�^�b�`�ōs��������ێ����邽�߂̒�`
#define GFL_APP_END_KEY			( 0 )		// �L�[�ŏI��
#define GFL_APP_END_TOUCH		( 1 )		// �^�b�`�ŏI��
#define GFL_APP_KTST_KEY		(GFL_APP_END_KEY)	//�L�[���[�h�œ��쒆
#define GFL_APP_KTST_TOUCH		(GFL_APP_END_TOUCH)	//�^�b�`���[�h�œ��쒆

//�^�b�`����L�[����ւ̐؂�ւ��̑ΏۂɂȂ�L�[
#define GFL_PAD_BUTTON_KTST_CHG	( PAD_BUTTON_A | PAD_BUTTON_B | \
	PAD_BUTTON_X | PAD_BUTTON_Y | \
	PAD_KEY_LEFT | PAD_KEY_RIGHT | PAD_KEY_UP | PAD_KEY_DOWN )

// �C���^�[�t�F�[�X
//==============================================================================
/**
 * @brief   �A�v���P�[�V�����I���̐�����L�[�ƃ^�b�`�̂ǂ����łōs�������H�擾
 * @retval  BOOL		GFL_APP_END_KEY(=0) �� GFL_APP_END_TOUCH(=1) (common.h�Ȃ̂ŃC���N���[�h�����j
 */
//==============================================================================
extern int  GFL_UI_CheckTouchOrKey(void);

//------------------------------------------------------------------
/**
 * @brief	�A�v���P�[�V�����I���̐�����L�[�ƃ^�b�`�̂ǂ����łōs�������H�ݒ�
 * @param   param		GFL_APP_END_KEY(=0) ���@GFL_APP_END_TOUCH(=1)
 */
//------------------------------------------------------------------
extern void GFL_UI_SetTouchOrKey(int param);


//----------------------------------------------------------------------------
/*
    �t���[�����[�g�̕ύX�̎d�g��

  GFL_UI_ChangeFrameRate���ĂԂƂ��̎��_�����U���Z�b�g���āA
  ���̃t���[�����J�n�t���[���ƔF������������J�n���܂�

----------------------------------------�t���[���̋���
  UI_Main  key_a�T���v�����O

   ��ʃv���O������{
   GFL_UI_ChangeFrameRate( 30 )   count=0

  key_a�T���v�����O=GetKey()
  
   }

----------------------------------------�t���[���̋���
  UI_Main  count++   �����͂��܂�1��������x�݁A�L�[�͓�������Ԃ�
  
   ���ʃv���O������{
  key_a�T���v�����O=GetKey()

  }


----------------------------------------�t���[���̋���
  UI_Main  count++   �����͂��܂�O������T���v�����O  key_b�T���v�����O�A�L�[�͐V�����Ȃ�
  
  key_b�T���v�����O=GetKey()

  
�ȍ~�J��Ԃ��E�E�E�E�E

  GFL_UI_ResetFrameRate�֐��œ����J�E���^�������I�ɂO�ɂ��鎖���ł���

  

 */
//----------------------------------------------------------------------------

typedef enum{
  GFL_UI_FRAMERATE_60 = 60,
  GFL_UI_FRAMERATE_30 = 30,
  GFL_UI_FRAMECOUNT_LCM = 2, //�ŏ����{�� �ۂ߂Ɏg�p
} GFL_UI_FRAMERATE_ENUM;


//----------------------------------------------------------------------------
/**
 * @brief	  �t���[�����[�g�̕ύX
 * @param	  GFL_UI_FRAMERATE_ENUM  ��`�̃t���[��
 * @retval	none
 */
//-----------------------------------------------------------------------------
extern void GFL_UI_ChangeFrameRate( const GFL_UI_FRAMERATE_ENUM framerate );

//----------------------------------------------------------------------------
/**
 * @brief	  �t���[�����[�g�̎擾
 * @param	  none
 * @retval	GFL_UI_FRAMERATE_ENUM  ��`�̃t���[��
 */
//-----------------------------------------------------------------------------
extern u32 GFL_UI_GetFrameRate( void );

//----------------------------------------------------------------------------
/**
 * @brief	  �t���[�����[�g�J�E���^���Z�b�g
 * @param	  none
 * @retval	none
 */
//-----------------------------------------------------------------------------
extern void GFL_UI_ResetFrameRate( void );

//----------------------------------------------------------------------------
/**
 * @brief	  �t���[�����[�g�̊J�n
 * @param	  GFL_UI_FRAMERATE_ENUM  ��`�̃t���[��
 * @retval	none
 */
//-----------------------------------------------------------------------------
extern void GFL_UI_StartFrameRateMode( const GFL_UI_FRAMERATE_ENUM framerate );

//----------------------------------------------------------------------------
/**
 * @brief	  �o�b�e���[���x���̎擾
 * @param	  none
 * @retval	GFL_UI_BATTLEVEL_e
 */
//-----------------------------------------------------------------------------
extern int GFL_UI_GetBattLevel(void);





//-----------------------------------------------------------------------------
/**
 *      UI�f�o�b�N�V�X�e��
 *
 *      UI���㏑���R�[���o�b�N
 */
//-----------------------------------------------------------------------------
#define	GFL_UI_DEBUGSYS_ENABLE

#ifdef GFL_UI_DEBUGSYS_ENABLE

/**
 * @brief UI �㏑���@�f�[�^�\����
 */
typedef struct {
  u16 trg;
  u16 cont;
  u16 repeat;
  u16 tp_x;
  u16 tp_y;
  u8 tp_trg;
  u8 tp_cont;
} GFL_UI_DEBUG_OVERWRITE;

/**
 * @brief UI �㏑���@�R�[���o�b�N�^�@
 *
 * @param GFL_UI_DEBUG_OVERWRITE* �f�[�^�i�[��
 * @param GFL_UI_DEBUG_OVERWRITE* �f�[�^�i�[�� 30�t���[�����̏㏑�����
 * @retval  TRUE    �㏑�� ����
 * @retval  FALSE   �㏑�� ���Ȃ�
 */
typedef BOOL (GFL_UI_DEBUG_OVERWRITE_FUNC)( GFL_UI_DEBUG_OVERWRITE* pOverWrite, GFL_UI_DEBUG_OVERWRITE* pOverWrite30 );

//----------------------------------------------------------------------------
/**
 * @brief	  UI �㏑���@�R�[���o�b�N�@�ݒ�
 * @param	  GFL_UI_DEBUG_OVERWRITE_FUNC*
 */
//-----------------------------------------------------------------------------
extern void GFL_UI_DEBUG_OVERWRITE_SetCallBack( GFL_UI_DEBUG_OVERWRITE_FUNC* pFunc );

#endif // GFL_UI_DEBUGSYS_ENABLE







#ifdef __cplusplus
}/* extern "C" */
#endif


