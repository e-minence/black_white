//==============================================================================
/**
 *@file		ui.h
 *@brief	���[�U�[�C���^�[�t�F�C�X�p���J�֐� ���J�e�[�u��
 *@author	k.ohno
 *@date		2006.11.16
 */
//==============================================================================

#ifndef __UI_H__
#define	__UI_H__

#undef GLOBAL
#ifdef __UI_SYS_H_GLOBAL__
#define GLOBAL /*	*/
#else
#define GLOBAL extern
#endif

// �f�o�b�O�p���܂蕶��----------------------
#define GFL_UI_DEBUG   (0)   ///< ���[�U�[�C���^�[�t�F�C�X�f�o�b�O�p 0:���� 1:�L��

#if defined(DEBUG_ONLY_FOR_ohno)
#undef GFL_UI_DEBUG
#define GFL_UI_DEBUG   (1)
#endif  //DEBUG_ONLY_FOR_ohno

#ifndef GFL_UI_PRINT
#if GFL_UI_DEBUG
#define GFL_UI_PRINT(...) \
  (void) ((OS_Printf(__VA_ARGS__)))
#else   //GFL_UI_DEBUG
#define GFL_UI_PRINT(...)           ((void) 0)
#endif  // GFL_UI_DEBUG
#endif  //GFL_UI_PRINT
// �f�o�b�O�p���܂蕶��----------------------


//-----------------------------------------------------------------------------
/**
 * @brief �萔�錾
 */
//-----------------------------------------------------------------------------

/**
 * @brief �\�t�g�E�G�A���Z�b�g�񋖉p
 */
typedef enum {
    GFL_UI_SOFTRESET_WIFI = 0x01,   ///< WIFI�ʐM��
    GFL_UI_SOFTRESET_SVLD = 0x02,   ///< �Z�[�u���[�h������
    GFL_UI_SOFTRESET_USER = 0x04,  ///< ��������͊O����`�ɂȂ�
} GF_UI_SOFTRESET_e;


/**
 * @brief �X���[�v�񋖉p
 */
typedef enum {
  GFL_UI_SLEEP_SVLD = 0x01,      ///< �Z�[�u���[�h
  GFL_UI_SLEEP_AGBROM = 0x02,    ///< AGB�A�N�Z�X
  GFL_UI_SLEEP_NET = 0x04,       ///< �ʐM�S��
  GFL_UI_SLEEP_USER = 0x08,      ///< ��������͊O����`�ɂȂ�
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
    GFL_UI_TP_HIT_NONE = -1	///< ���������e�[�u���Ȃ�
} GF_UI_TP_CUSTOM_e;

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
 <BR>		{GFL_UI_KEY_END,0,0},		// �I���f�[�^
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
 <BR>		{TP_HIT_END,0,0,0},		 �I���f�[�^
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

//------------------------------------------------------------------
/**
 * \var  UISYS
 * @brief	UISYS �^�錾
 * ���[�U�[�C���^�[�t�F�C�X�̊Ǘ���ێ�����\����
 * ���e�͉B��
  */
//------------------------------------------------------------------
typedef struct _UI_SYS UISYS;


//==============================================================================
/**
 * @brief UI������
 * @param   heapID    �q�[�v�m�ۂ��s��ID
 * @return  UISYS  work�n���h��
 */
//==============================================================================
GLOBAL UISYS* GFL_UI_sysInit(const int heapID);

//==============================================================================
/**
 * @brief UIMain����
 * @param   pUI    UISYS
 * @return  none
 */
//==============================================================================
GLOBAL void GFL_UI_sysMain(UISYS* pUI);

//==============================================================================
/**
 * @brief   UI�I������
 * @param   pUI    UISYS
 * @return  none
 */
//==============================================================================

GLOBAL void GFL_UI_sysEnd(UISYS* pUI);

//------------------------------------------------------------------
/**
 * @brief   �X���[�v��Ԃ��֎~����
 * @param   pUI		���[�U�[�C���^�[�t�F�C�X�Ǘ��\����
 * @param   sleepTypeBit �X���[�v�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
GLOBAL void GFL_UI_SleepDisable(UISYS* pUI,const u8 sleepTypeBit);

//------------------------------------------------------------------
/**
 * @brief   �X���[�v��Ԃ�������
 * @param   pUI		���[�U�[�C���^�[�t�F�C�X�Ǘ��\����
 * @param   sleepTypeBit �X���[�v�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
GLOBAL void GFL_UI_SleepEnable(UISYS* pUI, const u8 sleepTypeBit);

//------------------------------------------------------------------
/**
 * @brief   �\�t�g�E�G�A���Z�b�g��Ԃ��֎~����
 * @param   pUI		���[�U�[�C���^�[�t�F�C�X�Ǘ��\����
 * @param   softResetBit ���Z�b�g�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
GLOBAL void GFL_UI_SoftResetDisable(UISYS* pUI,const u8 softResetBit);

//------------------------------------------------------------------
/**
 * @brief   �\�t�g�E�G�A���Z�b�g��Ԃ�������
 * @param   pUI		���[�U�[�C���^�[�t�F�C�X�Ǘ��\����
 * @param   softResetBit ���Z�b�g�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
GLOBAL void GFL_UI_SoftResetEnable(UISYS* pUI, const u8 softResetBit);


//----------------------------------------------------------------------------
//�v���g�^�C�v�錾 touchpanel.c
//----------------------------------------------------------------------------


//==============================================================================
/**
 * @brief �I�[�g�T���v�����O���s���Ă��邩�ǂ����𓾂�
 * @param[in]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @retval  TRUE  �I�[�g�T���v�����O�ł���
 * @retval  FALSE  ���Ă��Ȃ�
 */
//==============================================================================
GLOBAL int GFL_UI_TPGetAutoSamplingFlg(const UISYS* pUI);

//==============================================================================
/**
 * @brief �I�[�g�T���v�����O�ݒ�
 * @param[out]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @param[in]    bAuto   �I�[�g�T���v�����O����Ȃ�TRUE
 * @return  none
 */
//==============================================================================
GLOBAL void GFL_UI_TPSetAutoSamplingFlg(UISYS* pUI, const BOOL bAuto);

//------------------------------------------------------------------
/**
 * @brief ���^�C�v�i��`�E�~�`�j�����Ȃ��画�肷��i�x�^���́j
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @param[in]   tbl		�����蔻��e�[�u���i�I�[�R�[�h����j
 * @return  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 */
//------------------------------------------------------------------
GLOBAL int GFL_UI_TouchPanelHitCont( const UISYS* pUI, const GFL_UI_TP_HITTBL *tbl );

//------------------------------------------------------------------
/**
 * @brief ���^�C�v�i��`�E�~�`�j�����Ȃ��画�肷��i�g���K���́j
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @param[in]   tbl		�����蔻��e�[�u���i�I�[�R�[�h����j
 * @retval  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 */
//------------------------------------------------------------------
GLOBAL int GFL_UI_TouchPanelHitTrg( const UISYS* pUI, const GFL_UI_TP_HITTBL *tbl );

//------------------------------------------------------------------
/**
 * @brief  �^�b�`�p�l���ɐG��Ă��邩
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @retval  BOOL		TRUE�ŐG��Ă���
 */
//------------------------------------------------------------------
GLOBAL BOOL GFL_UI_TouchPanelGetCont( const UISYS* pUI );

//------------------------------------------------------------------
/**
 * @brief �^�b�`�p�l���ɐG��Ă��邩�i�g���K�j
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @retval  BOOL		TRUE�ŐG�ꂽ
 */
//------------------------------------------------------------------
GLOBAL BOOL GFL_UI_TouchPanelGetTrg( const UISYS* pUI );

//----------------------------------------------------------------------------
/**
 *	@brief	�w�肵�����W�ŁA�����蔻����s���܂��B	����
 *	@param	tbl		�����蔻��e�[�u���i�z��j
 *	@param	x		���肘���W
 *	@param	y		���肙���W
 *	@retval  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 */
//-----------------------------------------------------------------------------
GLOBAL int GFL_UI_TouchPanelHitSelf( const GFL_UI_TP_HITTBL *tbl, u32 x, u32 y );

//------------------------------------------------------------------
/**
 * @brief �^�b�`�p�l���ɐG��Ă���Ȃ炻�̍��W�擾�i�x�^���́j
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @param[out]   x		�w���W�󂯎��ϐ��A�h���X
 * @param[out]   y		�x���W�󂯎��ϐ��A�h���X
 * @retval  TRUE  �G��Ă���
 * @retval  FALSE �G��Ă��Ȃ��B�����ɂ͉������Ȃ��B
 */
//------------------------------------------------------------------
GLOBAL BOOL GFL_UI_TouchPanelGetPointCont( const UISYS* pUI, u32* x, u32* y );

//------------------------------------------------------------------
/**
 * @brief �^�b�`�p�l���ɐG��Ă���Ȃ炻�̍��W�擾�i�g���K���́j
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @param[out]   x		�w���W�󂯎��ϐ��A�h���X
 * @param[out]   y		�x���W�󂯎��ϐ��A�h���X
 * @retval  TRUE  �G��Ă���
 * @retval  FALSE �G��Ă��Ȃ��B�����ɂ͉������Ȃ��B
 */
//------------------------------------------------------------------
GLOBAL BOOL GFL_UI_TouchPanelGetPointTrg( const UISYS* pUI, u32* x, u32* y );

//----------------------------------------------------------------------------
//�v���g�^�C�v�錾 key.c
//----------------------------------------------------------------------------

//==============================================================================
/**
 * @brief �L�[���s�[�g�̑��x�ƃE�F�C�g���Z�b�g
 * @param[in,out]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @param[in]	speed	���x
 * @param[in]	wait	�E�F�C�g
 * @return  none
 */
//==============================================================================
GLOBAL void GFL_UI_KeySetRepeatSpeed(UISYS* pUI, const int speed, const int wait );

//==============================================================================
/**
 * @brief �L�[�R���t�B�O�e�[�u����o�^
 * @param[in,out]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @param[in]	pTbl   �R���g���[���e�[�u���z��̃|�C���^
 * @return  none
 */
//==============================================================================
GLOBAL void GFL_UI_KeySetControlModeTbl(UISYS* pUI, const GFL_UI_KEY_CUSTOM_TBL* pTbl );

//==============================================================================
/**
 * @brief �L�[�g���K�Q�b�g
 * @param[in]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  �L�[�g���K
 */
//==============================================================================
GLOBAL GFL_UI_KeyGetTrg( const UISYS* pUI );

//==============================================================================
/**
 * @brief �L�[�R���g�Q�b�g
 * @param[in]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  �L�[�R���g
 */
//==============================================================================
GLOBAL GFL_UI_KeyGetCont( const UISYS* pUI );

//==============================================================================
/**
 * @brief �L�[���s�[�g�Q�b�g
 * @param[in]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  �L�[�R���g
 */
//==============================================================================
GLOBAL GFL_UI_KeyGetRepeat( const UISYS* pUI );

//==============================================================================
/**
 * @brief �L�[���s�[�g�̑��x�ƃE�F�C�g���Q�b�g
 * @param[in]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @param[out]	speed	���x
 * @param[out]	wait	�E�F�C�g
 * @return  none
 */
//==============================================================================
GLOBAL void GFL_UI_KeyGetRepeatSpeed(const UISYS* pUI, int* speed, int* wait );

//==============================================================================
/**
 * @brief   �L�[�R���t�B�O�̃��[�h�̒l��ݒ肷��
 * @param[in,out]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @param[in]   mode    �L�[�R���t�B�O���[�h
 * @return  none
 */
//==============================================================================
GLOBAL void GFL_UI_KeySetControlMode(UISYS* pUI,const int mode);

//==============================================================================
/**
 * @brief �L�[�R���t�B�O�̃��[�h�̒l�𓾂�
 * @param[in]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  �R���g���[�����[�h
 */
//==============================================================================
GLOBAL int GFL_UI_KeyGetControlMode(const UISYS* pUI);






/*-----------------------------------------------------------------------------
 *					�萔�錾
 ----------------------------------------------------------------------------*/
#define TP_REQUEST_CHECK_MAX		5		// ���߂��o���Ď��s������G���[��Ԃ���
											
#define		TP_ONE_SYNC_BUFF (9)			// �P�V���N�ɃT���v�����O�ł���ő�̐���
											// �T���v�����O����̂ɕK�v�ȃo�b�t�@�T�C�Y
											
#define		TP_ONE_SYNC_DATAMAX	(8)			// �P�V���N�Ɏ擾�ł���f�[�^�ő吔


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


//-------------------------------------
/// �G�̃f�[�^�쐬�t���O
enum{
	TP_MAKE_PICT_NORMAL,		// �͂܂ꂽ�G���A�����������Ă��悢
	TP_MAKE_PICT_ONE_FAST,		// �]�v�ȕ������Ȃ���
								// ��ԍŏ��ɂ�������_������Ă���
								// �}�`�̃f�[�^���쐬

	TP_MAKE_PICT_ONE_END		// �������̐悩�猩�Ĉ�ԍŌ�ɂ�������_
								// ������Ă���}�`�̃f�[�^���쐬
								// ���̃t���O���Z�b�g����ƕK���}�`�͂P�ɂȂ�
};

/*-----------------------------------------------------------------------------
 *					�\���̐錾
 ----------------------------------------------------------------------------*/
//-------------------------------------
#if 0
/// �^�b�`�p�l���̓��O����p�}�`�f�[�^�\����
typedef struct
{
	u32	Size;					// TP_VECTOR_DATA�̔z��̗v�f��
	Vec2DS32*	pPointData;		// ���_�f�[�^�̔z��
} TP_PICTURE_DATA;
#endif

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
 * @param   pUI  UI���[�N
 * @param	pData�F���̃t���[���̏��(init�Ŏw�肵���T���v�����O�񐔕��̏��)
 * @param	type�F�T���v�����O��ʂ̔ԍ�
 * @param	comp_num�F�o�b�t�@�Ɋi�[����Ƃ��ɁAcomp_num�ʂ̂�����������i�[����
 *
 * @return	u32�F�T���v�����O��ʂɂ��ω�
					type�FTP_BUFFERING		�T���v�����O���ꂽ�o�b�t�@�T�C�Y
					type�FTP_NO_LOOP		�T���v�����O���ꂽ�o�b�t�@�T�C�Y
											�o�b�t�@����t�ɂȂ����Ƃ� TP_END_BUFF
					type�FTP_NO_BUFF		TP_OK

					type�FTP_SAMP_NOT_START	�T���v�����O�J�n����Ă��܂���
 */
//-----------------------------------------------------------------------------
GLOBAL u32 GFL_UI_TPAutoSamplingMain( UISYS* pUI, TP_ONE_DATA* pData, u32 type, u32 comp_num );

//----------------------------------------------------------------------------
/**
 * @brief	�X���[�v������̍ĊJ����
 * @param   pUI             UI�̃��[�N
 * @return	none
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_UI_TPAutoSamplingReStart( UISYS* pUI );

//----------------------------------------------------------------------------
/**
 * @brief	�X���[�v�����O�̒�~����
 * @param   pUI             UI�̃��[�N
 * @return  none
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_UI_TPAutoSamplingStop( UISYS* pUI );

//----------------------------------------------------------------------------
/**
 * @brief	�^�b�`�p�l����AUTO�T���v�����O�J�n
 * @param	pUI�F UIwork�|�C���^
 * @param	p_buff�F�T���v�����O�f�[�^������o�b�t�@
 * @param	size�F�o�b�t�@�̃T�C�Y
 * @param	sync�F�P�t���[���ɉ���T���v�����O����̂�(MAX4)
 * @retval	TP_OK�F����
 * @retval	TP_FIFO_ERR�F�]�����s
 * @retval	TP_ERR�F�]���ȊO�̎��s
 */
//-----------------------------------------------------------------------------
GLOBAL u32 GFL_UI_TPAutoStart(UISYS* pUI, TPData* p_buff, u32 size, u32 sync);

//----------------------------------------------------------------------------
/**
 * @brief	�^�b�`�p�l����AUTO�T���v�����O�J�n	�o�b�t�@�����O�Ȃ�
 * @param	pUI�F UIwork�|�C���^
 * @param	sync�F�P�t���[���ɉ���T���v�����O����̂�(MAX4)
 * @retval	TP_OK�F����
 * @retval	TP_FIFO_ERR�F�]�����s
 * @retval	TP_ERR�F�]���ȊO�̎��s
 */
//-----------------------------------------------------------------------------
GLOBAL u32 GFL_UI_TPAutoStartNoBuff(UISYS* pUI, u32 sync);

//----------------------------------------------------------------------------
/**
 *@brief	�T���v�����O���I������
 *@param	pUI�F UIwork�|�C���^
 *@retval	TP_OK�F����
 *@retval�@	TP_FIFO_ERR�F�]�����s
 *@retval	TP_ERR�F�]���ȊO�̎��s
 */
//-----------------------------------------------------------------------------
GLOBAL u32 GFL_UI_TPAutoStop( UISYS* pUI );




#undef GLOBAL


#endif //__UI_H__

