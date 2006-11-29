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




#undef GLOBAL


#endif //__UI_H__

