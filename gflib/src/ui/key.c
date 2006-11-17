//==============================================================================
/**
 * @file	key.c
 * @brief	���[�U�[�C���^�[�t�F�C�X �L�[�̊Ǘ�
            �f�o�C�X���A�N�Z�X�֐�
 * @author	k.ohno
 */
//==============================================================================

#include "gflib.h" 

//==============================================================================
//
//			��`�A�֐��錾
//
//==============================================================================

static void _keyConvert(void);

//==============================================================================
//
//			�ϐ���`
//
//==============================================================================


struct _UIKeySys {
    UIKeyControlTbl* pControl;  //�L�[���[�h��ݒ肷��e�[�u��
//	int key_control_mode;	// �L�[�擾���[�h(0��default)
	int cont_org;			// �L�[���
	int trg_org;			// �L�[�g���K
	int repeat_org;			// �L�[���s�[�g
	int cont;				// �L�[���(�Q�[���ݒ�ɂ��ϊ�����������)
	int trg;				// �L�[�g���K(�Q�[���ݒ�ɂ��ϊ�����������)
	int repeat;				// �L�[���s�[�g(�Q�[���ݒ�ɂ��ϊ�����������)
	int repeatWait_Count;	// �L�[���s�[�g�J�E���^
	int repeatSpeed;		// �L�[���s�[�g�X�s�[�h
	int repeatWait;			// �L�[���s�[�g�E�F�C�g
};

//==============================================================================
//
//			�֐�
//
//==============================================================================

//==============================================================================
/**
 * �p�b�h������
 * @param[in,out]   UIPadHandle     ���[�U�[�C���^�[�t�F�C�X�p�b�h�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================

void GFL_UI_Pad_sysInit(UIPadHandle* pUI)
{
	TPCalibrateParam calibrate;

//	pUI->key_control_mode = 0;	//�L�[�擾���[�h

	pUI->cont_org	= 0;	// �L�[���
	pUI->trg_org		= 0;	// �L�[�g���K�[
	pUI->repeat_org	= 0;	// �L�[���s�[�g
	pUI->cont		= 0;	// �L�[���(�Q�[���ݒ�ɂ��ϊ�����������)
	pUI->trg			= 0;	// �L�[�g���K�[(�Q�[���ݒ�ɂ��ϊ�����������)
	pUI->repeat      = 0;	// �L�[���s�[�g(�Q�[���ݒ�ɂ��ϊ�����������)
	pUI->repeatWait_Count = 0;
	pUI->repeatSpeed	= 8;
	pUI->repeatWait	= 15;

}

//==============================================================================
/**
 * �p�b�h�ǂݎ�菈��
 * @param[in,out]   UIPadHandle     ���[�U�[�C���^�[�t�F�C�X�p�b�h�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================

void GFL_UI_Pad_sysMain(UIPadHandle* pUI)
{
	TPData	tpTemp;
	TPData	tpDisp;
	int	keyData;

	// �ӂ����܂��Ă���ꍇ�͑S�Ă̓��͂��Ȃ��ɂ���
	if(PAD_DetectFold()){
	  pUI->trg	= 0;
	  pUI->cont	= 0;
	  pUI->repeat	= 0;
	  return;
	}

	keyData = PAD_Read();
	
	pUI->trg_org		= keyData & (keyData ^ pUI->cont_org);	// �g���K ����

    pUI->repeat_org	=	keyData & (keyData ^ pUI->cont_org);        // ���s�[�g ����

    if((keyData!=0)&&(pUI->cont_org==keyData)){
		if(--pUI->repeatWait_Count==0){
			pUI->repeat_org				=	keyData;
			pUI->repeatWait_Count	=	pUI->repeatSpeed;
		}
	}
	else{
		pUI->repeatWait_Count = pUI->repeatWait;
	}


	pUI->cont_org	= keyData;							// �x�^ ����

	pUI->trg		= pUI->trg_org;
	pUI->cont	= pUI->cont_org;
	pUI->repeat	= pUI->repeat_org;

	_keyConvert();
}

//==============================================================================
// �L�[���̕ϊ��}�N��
//==============================================================================
#define KEYCONV( key, check_pat, set_pat )	{ if( key & check_pat ) key |= set_pat; }
#define KEYCONV_RESET( key, check_pat, set_pat ) {	\
	if( key & check_pat ){							\
		key |= set_pat;								\
		key &= ( check_pat^0xffff );				\
	}												\
}
#define KEYCONV_EXCHANGE( key, pat1, pat2 ) {	\
	int	tmp = 0;								\
	if( key & pat1 ){							\
		tmp |= pat2;							\
	}											\
	if( key & pat2 ){							\
		tmp |= pat1;							\
	}											\
	key &= ( (pat1 | pat2 )^0xffff );			\
	key |= tmp;									\
}
#define KEY_RESET( key, reset_pat ) {			\
	key &= ( reset_pat^0xffff );				\
}

//==============================================================================
/**
 * �L�[���̕ϊ�
 * @param[in,out]   UIPadHandle     ���[�U�[�C���^�[�t�F�C�X�p�b�h�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================
static void _keyConvert(UIPadHandle* pUI)
{
	switch( pUI->key_control_mode ){

	default:
	case KEYCONTROL_NORMAL:
		break;

	case KEYCONTROL_PATTERN1:

		KEYCONV( pUI->trg, PAD_BUTTON_START, PAD_BUTTON_X )
		KEYCONV( pUI->cont, PAD_BUTTON_START, PAD_BUTTON_X )
		KEYCONV( pUI->repeat, PAD_BUTTON_START, PAD_BUTTON_X )
		break;

	case KEYCONTROL_PATTERN2:

		KEYCONV_EXCHANGE( pUI->trg, PAD_BUTTON_X,PAD_BUTTON_Y )
		KEYCONV_EXCHANGE( pUI->cont, PAD_BUTTON_X,PAD_BUTTON_Y )
		KEYCONV_EXCHANGE( pUI->repeat, PAD_BUTTON_X,PAD_BUTTON_Y )
		break;

	case KEYCONTROL_PATTERN3:

		KEYCONV( pUI->trg, PAD_BUTTON_L, PAD_BUTTON_A )
		KEYCONV( pUI->cont, PAD_BUTTON_L, PAD_BUTTON_A )
		KEYCONV( pUI->repeat, PAD_BUTTON_L, PAD_BUTTON_A )

		KEY_RESET( pUI->trg, ( PAD_BUTTON_L | PAD_BUTTON_R ) )
		KEY_RESET( pUI->cont, ( PAD_BUTTON_L | PAD_BUTTON_R ) )
		KEY_RESET( pUI->repeat, ( PAD_BUTTON_L | PAD_BUTTON_R ) )
		break;
	}
}

//==============================================================================
/**
 * �L�[���s�[�g�̑��x�ƃE�F�C�g���Z�b�g
 * @param[in,out]   UIPadHandle     ���[�U�[�C���^�[�t�F�C�X�p�b�h�n���h���̃|�C���^
 * @param[in]	speed	���x
 * @param[in]	wait	�E�F�C�g
 * @return  none
 */
//==============================================================================
void GFL_UI_KeyRepeatSpeedSet( int speed, int wait )
{
	pUI->repeatSpeed	= speed;
	pUI->repeatWait	= wait;
}

//==============================================================================
/**
 * �L�[�g���K�Q�b�g
 * @param[in,out]   UIPadHandle     ���[�U�[�C���^�[�t�F�C�X�p�b�h�n���h���̃|�C���^
 * @return  �L�[�g���K
 */
//==============================================================================
void GFL_UI_KeyGetTrg( int speed, int wait )
{
	pUI->repeatSpeed	= speed;
	pUI->repeatWait	= wait;
}

