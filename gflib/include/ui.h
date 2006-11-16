//==============================================================================
/**
 *@file		ui.h
 *@brief	���[�U�[�C���^�[�t�F�C�X�p���J�֐� ���J�e�[�u��
 *@author	k.ohno
 *@data		2006.11.16
 */
//==============================================================================

#ifndef __UI_H__
#define	__UI_H__

//-----------------------------------------------------------------------------
/**
 * @brief �萔�錾
 */
//-----------------------------------------------------------------------------

/** @gflib �L�[�J�X�^�}�C�Y�e�[�u���p */
#define   GFL_UI_KEY_COPY          (1)     ///< �P����Q�փL�[���R�s�[���܂�
#define   GFL_UI_KEY_CHANGE        (2)     ///< �P�ƂQ���������܂�
#define   GFL_UI_KEY_RESET         (3)     ///< �P�������܂�
#define   GFL_UI_KEY_END         (0)     ///< �e�[�u���̏I��

/** @brief �^�b�`�p�l���e�[�u���p */
#define		GFL_UI_TP_HIT_END		(0xff)			///< �e�[�u���I���R�[�h
#define		GFL_UI_TP_USE_CIRCLE	(0xfe)			///< �~�`�Ƃ��Ďg��
#define		GFL_UI_TP_HIT_NONE		(-1)	///< ���������e�[�u���Ȃ�

//-----------------------------------------------------------------------------
/**
 *	@brief �\���̐錾
 */
//-----------------------------------------------------------------------------

/**
 * @struct GFL_UI_KEY_CUSTOM_TBL
 * @brief  �L�[�f�[�^�\����  �L�[�R���t�B�O�p�^�[�����w�肷��e�[�u��.
 * �g�p��  �z��v�f�D�揇��:0>1>2>3>4>.....�ł�
 *
 * GFL_UI_KEY_CUSTOM_TBL key_data[] ={
 *		{PAD_BUTTON_SELECT,PAD_BUTTON_START,GFL_UI_KEY_CHANGE},		//START SELECT ����
 *		{PAD_BUTTON_L,PAD_BUTTON_A,GFL_UI_KEY_COPY},		//L��A������
 *		{PAD_BUTTON_R,0,GFL_UI_KEY_RESET},		//L��A������
 *		{GFL_UI_KEY_END,0,0},		// �I���f�[�^
 * };
 *
 */
typedef struct{
    int keySource;
    int keyDist;
    u8 mode;
}GFL_UI_KEY_CUSTOM_TBL;

/**
 * @struct GFL_UI_TP_HITTBL
 * @brief  �^�b�`�p�l��������͈͂���`�A�~�`�Ŏw�肷��e�[�u��.
 * @note  �g�p��  �z��v�f�D�揇��:0>1>2>3>4>.....�ł� 
 * <BR>GFL_UI_TP_HITTBL tp_data[] ={
 * <BR>		{128,191,0,64},
 * <BR>		{32,94,129,200},
 * <BR>		{0,191,0,255},
 * <BR>      {TP_USE_CIRCLE, 100, 80, 16 },	 �~�`�Ƃ��Ďg���B
 * <BR>		{TP_HIT_END,0,0,0},		 �I���f�[�^
 * <BR>};
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
typedef struct _UISYS UISYS;


//----------------------------------------------------------------------------
//�v���g�^�C�v�錾 touchpanel.c
//----------------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief ���^�C�v�i��`�E�~�`�j�����Ȃ��画�肷��i�x�^���́j
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @param[in]   tbl		�����蔻��e�[�u���i�I�[�R�[�h����j
 * @return  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 */
//------------------------------------------------------------------
extern int GFL_UI_TouchPanelHitCont( const UISYS* tpsys, const GFL_UI_TP_HITTBL *tbl );

//------------------------------------------------------------------
/**
 * @brief ���^�C�v�i��`�E�~�`�j�����Ȃ��画�肷��i�g���K���́j
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @param[in]   tbl		�����蔻��e�[�u���i�I�[�R�[�h����j
 * @retval  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 */
//------------------------------------------------------------------
extern int GFL_UI_TouchPanelHitTrg( const UISYS* tpsys, const GFL_UI_TP_HITTBL *tbl );

//------------------------------------------------------------------
/**
 * @brief  �^�b�`�p�l���ɐG��Ă��邩
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @retval  BOOL		TRUE�ŐG��Ă���
 */
//------------------------------------------------------------------
extern BOOL GFL_UI_TouchPanelGetCont( const UISYS* tpsys );

//------------------------------------------------------------------
/**
 * @brief �^�b�`�p�l���ɐG��Ă��邩�i�g���K�j
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @retval  BOOL		TRUE�ŐG�ꂽ
 */
//------------------------------------------------------------------
extern BOOL GFL_UI_TouchPanelGetTrg( const UISYS* tpsys );

//----------------------------------------------------------------------------
/**
 *	@brief	�w�肵�����W�ŁA�����蔻����s���܂��B	����
 *	@param	tbl		�����蔻��e�[�u���i�z��j
 *	@param	x		���肘���W
 *	@param	y		���肙���W
 *	@retval  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 */
//-----------------------------------------------------------------------------
extern int GFL_UI_TouchPanelHitSelf( const GFL_UI_TP_HITTBL *tbl, u32 x, u32 y );

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
extern BOOL GFL_UI_TouchPanelGetPointCont( const UISYS* tpsys, u32* x, u32* y );

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
extern BOOL GFL_UI_TouchPanelGetPointTrg( const UISYS* tpsys, u32* x, u32* y );

//----------------------------------------------------------------------------
//�v���g�^�C�v�錾 key.c
//----------------------------------------------------------------------------

#endif //__UI_H__

