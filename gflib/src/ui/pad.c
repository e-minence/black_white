//==============================================================================
/**
 * @file	pad.c
 * @brief	���[�U�[�C���^�[�t�F�C�X �p�b�h�̊Ǘ�
            �f�o�C�X���A�N�Z�X�֐�
 * @author	GAME FREAK Inc.
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

//==============================================================================
//
//			�֐�
//
//==============================================================================

//==============================================================================
/**
 * �p�b�h������
 * @param   UIHandle     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================

void GFL_UI_Pad_sysInit(UIHandle* pUI)
{
	TPCalibrateParam calibrate;

	pUI->key_control_mode = 0;	//�L�[�擾���[�h

	pUI->cont_org	= 0;	// �L�[���
	pUI->trg_org		= 0;	// �L�[�g���K�[
	pUI->repeat_org	= 0;	// �L�[���s�[�g
	pUI->cont		= 0;	// �L�[���(�Q�[���ݒ�ɂ��ϊ�����������)
	pUI->trg			= 0;	// �L�[�g���K�[(�Q�[���ݒ�ɂ��ϊ�����������)
	pUI->repeat      = 0;	// �L�[���s�[�g(�Q�[���ݒ�ɂ��ϊ�����������)
	pUI->repeatWait_Count = 0;
	pUI->repeatSpeed	= 8;
	pUI->repeatWait	= 15;


	pUI->tp_x		 = 0;	// �^�b�`�p�l��X���W
	pUI->tp_y		 = 0;	// �^�b�`�p�l��Y���W
	pUI->tp_trg		 = 0;	// �^�b�`�p�l���ڐG����g���K
	pUI->tp_cont		 = 0;	// �^�b�`�p�l���ڐG������
	pUI->tp_auto_samp = 0;	// �^�b�`�p�l���̃I�[�g�T���v�����O���s���Ă��邩�̃t���O
									
	// �^�b�`�p�l���̏������ƃL�����u���[�V�������Z�b�g
	TP_Init();

	// �}�V���̃L�����u���[�V�����l���擾
	if( TP_GetUserInfo( &calibrate ) == TRUE ){
		// �L�����u���[�V�����l�̐ݒ�
		TP_SetCalibrateParam( &calibrate );
		UI_PRINT("Get Calibration Parameter from NVRAM\n");
	}
	else{
		// �擾�Ɏ��s�����̂Ńf�t�H���g�̃L�����u���[�V�����̐ݒ�
		calibrate.x0 = 0x02ae;
		calibrate.y0 = 0x058c;
		calibrate.xDotSize = 0x0e25;
		calibrate.yDotSize = 0x1208;
		TP_SetCalibrateParam( &calibrate );
		UI_PRINT( "Warrning : TouchPanelInit( not found valid calibration data )\n" );
	}
}

//==============================================================================
/**
 * �p�b�h�ǂݎ�菈��
 * @param   UIHandle     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================

void sys_MainKeyRead(UIHandle* pUI)
{
	TPData	tpTemp;
	TPData	tpDisp;
	int	keyData;

	// �ӂ����܂��Ă���ꍇ�͑S�Ă̓��͂��Ȃ��ɂ���
	if(PAD_DetectFold()){
	  pUI->trg	= 0;
	  pUI->cont	= 0;
	  pUI->repeat	= 0;
	  pUI->tp_trg	= 0;
	  pUI->tp_cont	= 0;
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

	// �^�b�`�p�l���f�[�^���擾
	if(pUI->tp_auto_samp == 0){
		while( TP_RequestRawSampling( &tpTemp ) != 0 ){};	//�T���v�����O�ɐ�������܂ő҂�
	}else{
		TP_GetLatestRawPointInAuto( &tpTemp );	// �I�[�g�T���v�����O���̃f�[�^���擾
	}

	TP_GetCalibratedPoint( &tpDisp, &tpTemp );	// ���W����ʍ��W�i�O�`�Q�T�T�j�ɂ���

	
	if( tpDisp.validity == TP_VALIDITY_VALID  ){		// ���W�̗L�������`�F�b�N
		// �^�b�`�p�l�����W�L��
		pUI->tp_x = tpDisp.x;
		pUI->tp_y = tpDisp.y;
	}else{
		// �^�b�`�p�l�����W����
		// 1�V���N�O�̍��W���i�[����Ă���Ƃ��̂ݍ��W�����̂܂܎󂯌p��
		if( pUI->tp_cont ){
			switch(tpDisp.validity){
			case TP_VALIDITY_INVALID_X:
				pUI->tp_y = tpDisp.y;
				break;
			case TP_VALIDITY_INVALID_Y:
				pUI->tp_x = tpDisp.x;
				break;
			case TP_VALIDITY_INVALID_XY:
				break;
			default:	// ����
				break;
			}
		}else{
			// �g���K�̃^�C�~���O�Ȃ�A
			// �^�b�`�p�l���������Ă��Ȃ����Ƃɂ���
			tpDisp.touch = 0;
		}
	}
	pUI->tp_trg	= (u16)(tpDisp.touch & (tpDisp.touch ^ pUI->tp_cont));	// �g���K ����
	pUI->tp_cont	= tpDisp.touch;										// �x�^ ����

}

//--------------------------------------------------------------------------------------------
/**
 * �L�[���̕ϊ�
 *
 * @param	none
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
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

static void _keyConvert(void)
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

//--------------------------------------------------------------------------------------------
/**
 * �L�[���s�[�g�̑��x�ƃE�F�C�g���Z�b�g
 *
 * @param	speed	���x
 * @param	wait	�E�F�C�g
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void sys_KeyRepeatSpeedSet( int speed, int wait )
{
	pUI->repeatSpeed	= speed;
	pUI->repeatWait	= wait;
}

