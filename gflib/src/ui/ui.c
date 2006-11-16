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



/*---------------------------------------------------------------------------*
  Name:         sys_KeyRead

  Description:  �L�[��ǂݍ��݁A�g���K�ƃ����[�X�g���K�����߂܂��B
				
  Arguments:    �Ȃ��B
  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/

//������
void sys_InitKeyRead(void)
{
	TPCalibrateParam calibrate;

	sys.key_control_mode = 0;	//�L�[�擾���[�h

	sys.cont_org	= 0;	// �L�[���
	sys.trg_org		= 0;	// �L�[�g���K�[
	sys.repeat_org	= 0;	// �L�[���s�[�g
	sys.cont		= 0;	// �L�[���(�Q�[���ݒ�ɂ��ϊ�����������)
	sys.trg			= 0;	// �L�[�g���K�[(�Q�[���ݒ�ɂ��ϊ�����������)
	sys.repeat      = 0;	// �L�[���s�[�g(�Q�[���ݒ�ɂ��ϊ�����������)
	sys.repeatWait_Count = 0;
	sys.repeatSpeed	= 8;
	sys.repeatWait	= 15;


	sys.tp_x		 = 0;	// �^�b�`�p�l��X���W
	sys.tp_y		 = 0;	// �^�b�`�p�l��Y���W
	sys.tp_trg		 = 0;	// �^�b�`�p�l���ڐG����g���K
	sys.tp_cont		 = 0;	// �^�b�`�p�l���ڐG������
	sys.tp_auto_samp = 0;	// �^�b�`�p�l���̃I�[�g�T���v�����O���s���Ă��邩�̃t���O
									
	// �^�b�`�p�l���̏������ƃL�����u���[�V�������Z�b�g
	TP_Init();

	// �}�V���̃L�����u���[�V�����l���擾
	if( TP_GetUserInfo( &calibrate ) == TRUE ){
		// �L�����u���[�V�����l�̐ݒ�
		TP_SetCalibrateParam( &calibrate );
		OS_Printf("Get Calibration Parameter from NVRAM\n");
	}
	else{
		// �擾�Ɏ��s�����̂Ńf�t�H���g�̃L�����u���[�V�����̐ݒ�
		calibrate.x0 = 0x02ae;
		calibrate.y0 = 0x058c;
		calibrate.xDotSize = 0x0e25;
		calibrate.yDotSize = 0x1208;
		TP_SetCalibrateParam( &calibrate );
		OS_Printf( "Warrning : TauchPanelInit( not found valid calibration data )\n" );
	}
}

/*---------------------------------------------------------------------------*
  @brief	�`�f�a�J�[�g���b�W������
 *---------------------------------------------------------------------------*/
void sys_InitAgbCasetteVer(int version)
{
	sys.AgbCasetteVersion = version;
}

/*---------------------------------------------------------------------------*
  @brief	�X���[�v��Ԃ��֎~����
 *---------------------------------------------------------------------------*/
void sys_SleepNG(u8 sleepTypeBit)
{
	sys.DontSleep |= sleepTypeBit;
}

/*---------------------------------------------------------------------------*
  @brief	�X���[�v��Ԃ�������
 *---------------------------------------------------------------------------*/
void sys_SleepOK(u8 sleepTypeBit)
{
	sys.DontSleep &= ~(sleepTypeBit);
}

//------------------------------------------------------------------
static void KeyConvert(void);

//�ǂݎ��

void sys_MainKeyRead(void)
{
	TPData	tpTemp;
	TPData	tpDisp;
	int	keyData;

	// �ӂ����܂��Ă���ꍇ�͑S�Ă̓��͂��Ȃ��ɂ���
	if(PAD_DetectFold()){
	  sys.trg	= 0;
	  sys.cont	= 0;
	  sys.repeat	= 0;
	  sys.tp_trg	= 0;
	  sys.tp_cont	= 0;
	  return;
	}

	keyData = PAD_Read();
	
	sys.trg_org		= keyData & (keyData ^ sys.cont_org);	// �g���K ����

    sys.repeat_org	=	keyData & (keyData ^ sys.cont_org);        // ���s�[�g ����

    if((keyData!=0)&&(sys.cont_org==keyData)){
		if(--sys.repeatWait_Count==0){
			sys.repeat_org				=	keyData;
			sys.repeatWait_Count	=	sys.repeatSpeed;
		}
	}
	else{
		sys.repeatWait_Count = sys.repeatWait;
	}


	sys.cont_org	= keyData;							// �x�^ ����

	sys.trg		= sys.trg_org;
	sys.cont	= sys.cont_org;
	sys.repeat	= sys.repeat_org;

	KeyConvert();

	// �^�b�`�p�l���f�[�^���擾
	if(sys.tp_auto_samp == 0){
		while( TP_RequestRawSampling( &tpTemp ) != 0 ){};	//�T���v�����O�ɐ�������܂ő҂�
	}else{
		TP_GetLatestRawPointInAuto( &tpTemp );	// �I�[�g�T���v�����O���̃f�[�^���擾
	}

	TP_GetCalibratedPoint( &tpDisp, &tpTemp );	// ���W����ʍ��W�i�O�`�Q�T�T�j�ɂ���

#if 0	// 2006.07.05	���W�擾���s��������ύX
	switch(tpDisp.validity){		// ���W�̗L�������`�F�b�N
	case TP_VALIDITY_INVALID_X:
		sys.tp_x = 0xffff;
		sys.tp_y = tpDisp.y;
		break;
	case TP_VALIDITY_INVALID_Y:
		sys.tp_x = tpDisp.x;			// ���W���Z�b�g
		sys.tp_y = 0xffff;
		break;
	case TP_VALIDITY_INVALID_XY:
		sys.tp_x = 0xffff;
		sys.tp_y = 0xffff;
		break;
	default:
		sys.tp_x = tpDisp.x;
		sys.tp_y = tpDisp.y;
	}

	sys.tp_trg	= (u16)(tpDisp.touch & (tpDisp.touch ^ sys.tp_cont));	// �g���K ����
	sys.tp_cont	= tpDisp.touch;										// �x�^ ����
	
#endif	/// 
	
	if( tpDisp.validity == TP_VALIDITY_VALID  ){		// ���W�̗L�������`�F�b�N
		// �^�b�`�p�l�����W�L��
		sys.tp_x = tpDisp.x;
		sys.tp_y = tpDisp.y;
	}else{
		// �^�b�`�p�l�����W����
		// 1�V���N�O�̍��W���i�[����Ă���Ƃ��̂ݍ��W�����̂܂܎󂯌p��
		if( sys.tp_cont ){
			switch(tpDisp.validity){
			case TP_VALIDITY_INVALID_X:
				sys.tp_y = tpDisp.y;
				break;
			case TP_VALIDITY_INVALID_Y:
				sys.tp_x = tpDisp.x;
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
	sys.tp_trg	= (u16)(tpDisp.touch & (tpDisp.touch ^ sys.tp_cont));	// �g���K ����
	sys.tp_cont	= tpDisp.touch;										// �x�^ ����

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

static void KeyConvert(void)
{
	switch( sys.key_control_mode ){

	default:
	case KEYCONTROL_NORMAL:
		break;

	case KEYCONTROL_PATTERN1:

		KEYCONV( sys.trg, PAD_BUTTON_START, PAD_BUTTON_X )
		KEYCONV( sys.cont, PAD_BUTTON_START, PAD_BUTTON_X )
		KEYCONV( sys.repeat, PAD_BUTTON_START, PAD_BUTTON_X )
		break;

	case KEYCONTROL_PATTERN2:

		KEYCONV_EXCHANGE( sys.trg, PAD_BUTTON_X,PAD_BUTTON_Y )
		KEYCONV_EXCHANGE( sys.cont, PAD_BUTTON_X,PAD_BUTTON_Y )
		KEYCONV_EXCHANGE( sys.repeat, PAD_BUTTON_X,PAD_BUTTON_Y )
		break;

	case KEYCONTROL_PATTERN3:

		KEYCONV( sys.trg, PAD_BUTTON_L, PAD_BUTTON_A )
		KEYCONV( sys.cont, PAD_BUTTON_L, PAD_BUTTON_A )
		KEYCONV( sys.repeat, PAD_BUTTON_L, PAD_BUTTON_A )

		KEY_RESET( sys.trg, ( PAD_BUTTON_L | PAD_BUTTON_R ) )
		KEY_RESET( sys.cont, ( PAD_BUTTON_L | PAD_BUTTON_R ) )
		KEY_RESET( sys.repeat, ( PAD_BUTTON_L | PAD_BUTTON_R ) )
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
	sys.repeatSpeed	= speed;
	sys.repeatWait	= wait;
}


/*---------------------------------------------------------------------------*
  @brief	�\�t�g�E�G�A���Z�b�g��Ԃ��֎~����
 *---------------------------------------------------------------------------*/
void sys_SoftResetNG(u8 softResetBit)
{
	sys.DontSoftReset |= softResetBit;
}

/*---------------------------------------------------------------------------*
  @brief	�\�t�g�E�G�A���Z�b�g��Ԃ�������
 *---------------------------------------------------------------------------*/
void sys_SoftResetOK(u8 softResetBit)
{
	sys.DontSoftReset &= ~(softResetBit);
}

