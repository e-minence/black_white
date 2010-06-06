//============================================================================================
/**
 * @file	effect_viewer.h
 * @brief	�G�t�F�N�g�r���[���[�Ŏg�p����f�[�^�Q
 * @author	soga
 * @date	2009.04.09
 */
//============================================================================================

#include "msg/debug/msg_d_soga.h"

#define	NO_MOVE	( 0xff )
#define	NO_EDIT	( -1 )

#define	STR_DOT	( 12 )		//�����T�C�Y

//typedef BOOL (*EV_FUNC)( EFFECT_VIEWER_WORK * );

typedef struct
{
	u8		cursor_pos_x;	//�J�[�\��X�ʒu
	u8		cursor_pos_y;	//�J�[�\��Y�ʒu
	u8		move_up;		//��L�[���������Ƃ��̈ړ���
	u8		move_down;		//���L�[���������Ƃ��̈ړ���
	u8		move_left;		//���L�[���������Ƃ��̈ړ���
	u8		move_right;		//�E�L�[���������Ƃ��̈ړ���
}MENU_LIST_PARAM;

enum{
	EDIT_NONE = 0,
	EDIT_DECIDE,
	EDIT_CANCEL,
	EDIT_INT,
	EDIT_FX32,
	EDIT_COMBOBOX,
};

typedef struct
{
	u16	label_x;	//���o��X���W
	u16	label_y;	//���o��Y���W
	u32	strID;		//���o��strID
	u16	data_x;		//�f�[�^X���W
	u16	data_y;		//�f�[�^Y���W
	u16	edit_type;	//�ҏW�^�C�v
	u8	edit_min;	//�ҏW�^�C�v��EDIT_COMBOBOX�̂Ƃ��̍ŏ��l
	u8	edit_max;	//�ҏW�^�C�v��EDIT_COMBOBOX�̂Ƃ��̍ő�l
}MENU_SCREEN_DATA;

typedef struct
{
	int							count;
	const	MENU_SCREEN_DATA	**msd;
	const	MENU_LIST_PARAM		*mlp;
}MENU_SCREEN_PARAM;

//============================================================================================
/**
 *		���ʌ��o��
 */
//============================================================================================

#define	ATTACK_LABEL_X	( STR_DOT * 1 )
#define	ATTACK_LABEL_Y	( STR_DOT * 1 )
#define	ATTACK_DATA_X	( ATTACK_LABEL_X + STR_DOT * 6 )
#define	ATTACK_DATA_Y	( ATTACK_LABEL_Y )
#define	DEFENCE_LABEL_X	( STR_DOT * 1 )
#define	DEFENCE_LABEL_Y	( STR_DOT * 2 )
#define	DEFENCE_DATA_X	( DEFENCE_LABEL_X + STR_DOT * 6 )
#define	DEFENCE_DATA_Y	( DEFENCE_LABEL_Y )
#define	DECIDE_LABEL_X	( STR_DOT * 1 )
#define	DECIDE_LABEL_Y	( STR_DOT * 12 )
#define	CANCEL_LABEL_X	( DECIDE_LABEL_X + STR_DOT * 8 )
#define	CANCEL_LABEL_Y	( STR_DOT * 12 )

static const MENU_SCREEN_DATA menu_attack = {
	ATTACK_LABEL_X, ATTACK_LABEL_Y, EVMSG_ATTACK,
	ATTACK_DATA_X, ATTACK_DATA_Y, EDIT_COMBOBOX, EVMSG_POS_AA, EVMSG_POS_D, //EVMSG_POS_F,
};

static const MENU_SCREEN_DATA menu_defence = {
	DEFENCE_LABEL_X, DEFENCE_LABEL_Y, EVMSG_DEFENCE,
	DEFENCE_DATA_X, DEFENCE_DATA_Y, EDIT_COMBOBOX, EVMSG_POS_AA, EVMSG_POS_D, //EVMSG_POS_F,
};

static const MENU_SCREEN_DATA menu_decide = {
	DECIDE_LABEL_X, DECIDE_LABEL_Y, EVMSG_DECIDE,
	0, 0, EDIT_DECIDE, 0, 0,
};

static const MENU_SCREEN_DATA menu_cancel = {
	CANCEL_LABEL_X, CANCEL_LABEL_Y, EVMSG_CANCEL,
	0, 0, EDIT_CANCEL, 0, 0,
};

//============================================================================================
/**
 *		�^�C�g��
 */
//============================================================================================

#define	TITLE_STRLEN	( 14 )
#define	TITLE_LABEL_X	( 128 - ( TITLE_STRLEN * STR_DOT / 2 ) )
#define	TITLE_LABEL_Y	( 96 - ( STR_DOT / 2 ) )

static const MENU_SCREEN_DATA menu_title = {
	TITLE_LABEL_X, TITLE_LABEL_Y, EVMSG_TITLE,
	0, 0, EDIT_NONE, 0, 0,
};

static const MENU_SCREEN_DATA *msd_title[] = {
	&menu_title,
};

static const MENU_SCREEN_PARAM	msp_title = {
	NELEMS( msd_title ),
	&msd_title[ 0 ],
	NULL
};

//============================================================================================
/**
 *		�p�[�e�B�N���Đ��G�f�B�b�g
 */
//============================================================================================

#define	PPE_OFSY_LABEL_X	( STR_DOT * 1 )
#define	PPE_OFSY_LABEL_Y	( STR_DOT * 6 )
#define	PPE_OFSY_DATA_X		( PPE_OFSY_LABEL_X + STR_DOT * 8 )
#define	PPE_OFSY_DATA_Y		( PPE_OFSY_LABEL_Y )
#define	PPE_ANGLE_LABEL_X	( STR_DOT * 1 )
#define	PPE_ANGLE_LABEL_Y	( STR_DOT * 7 )
#define	PPE_ANGLE_DATA_X	( PPE_ANGLE_LABEL_X + STR_DOT * 8 )
#define	PPE_ANGLE_DATA_Y	( PPE_ANGLE_LABEL_Y )

#define	PPE_SEND_SIZE		( 4 * 3 )		//MCS��PC�ɑ��M����f�[�^�T�C�Y

static const MENU_SCREEN_DATA menu_ppe_ofsy = {
	PPE_OFSY_LABEL_X, PPE_OFSY_LABEL_Y, EVMSG_PPLAY_OFSY,
	PPE_OFSY_DATA_X, PPE_OFSY_DATA_Y, EDIT_FX32, 0, 0,
};

static const MENU_SCREEN_DATA menu_ppe_angle = {
	PPE_ANGLE_LABEL_X, PPE_ANGLE_LABEL_Y, EVMSG_PPLAY_ANGLE,
	PPE_ANGLE_DATA_X, PPE_ANGLE_DATA_Y, EDIT_FX32, 0, 0,
};

static const MENU_SCREEN_DATA *msd_ppe[] = {
	&menu_attack,
	&menu_defence,
	&menu_ppe_ofsy,
	&menu_ppe_angle,
	&menu_decide,
	&menu_cancel,
};

enum{
	MLP_PPE_ATTACK = 0,
	MLP_PPE_DEFENCE,
	MLP_PPE_OFSY,
	MLP_PPE_ANGLE,
	MLP_PPE_DECIDE,
	MLP_PPE_CANCEL,
};

enum{
	PPEPARAM_NUM = 0,	//�Đ��p�[�e�B�N���i���o�[
	PPEPARAM_INDEX,		//spa���C���f�b�N�X�i���o�[
	PPEPARAM_START_POS,	//�p�[�e�B�N���Đ��J�n�����ʒu
	PPEPARAM_DIR_POS,	//�p�[�e�B�N���Đ����������ʒu
	PPEPARAM_OFS_Y,		//�p�[�e�B�N���Đ�Y�����I�t�Z�b�g
	PPEPARAM_DIR_ANGLE,	//�p�[�e�B�N���Đ�����Y�p�x
};

enum{ 
  SPPARAM_SE_NO = 0, 
  SPPARAM_PLAYER, 
  SPPARAM_PAN, 
  SPPARAM_WAIT, 
  SPPARAM_PITCH, 
  SPPARAM_VOLUME, 
  SPPARAM_MODUDEPTH, 
  SPPARAM_MODUSPEED, 
};

enum{ 
  SPANPARAM_PLAYER = 0,
  SPANPARAM_TYPE,
  SPANPARAM_START,
  SPANPARAM_END,
  SPANPARAM_START_WAIT,
  SPANPARAM_FRAME,
  SPANPARAM_WAIT,
  SPANPARAM_COUNT,
};

enum{ 
  SEFFPARAM_PLAYER = 0,
  SEFFPARAM_TYPE,
  SEFFPARAM_PARAM,
  SEFFPARAM_START,
  SEFFPARAM_END,
  SEFFPARAM_START_WAIT,
  SEFFPARAM_FRAME,
  SEFFPARAM_WAIT,
  SEFFPARAM_COUNT,
};

static const MENU_LIST_PARAM mlp_ppe[]={
	//MLP_PPE_ATTACK
	{
		ATTACK_LABEL_X - STR_DOT,
		ATTACK_LABEL_Y,
		NO_MOVE,	
		MLP_PPE_DEFENCE,	
		NO_MOVE,
		NO_MOVE,
	},
	//MLP_PPE_DEFENCE
	{
		DEFENCE_LABEL_X - STR_DOT,
		DEFENCE_LABEL_Y,
		MLP_PPE_ATTACK,	
		MLP_PPE_OFSY,	
		NO_MOVE,
		NO_MOVE,
	},
	//MLP_PPE_OFSY
	{
		PPE_OFSY_LABEL_X - STR_DOT,
		PPE_OFSY_LABEL_Y,
		MLP_PPE_DEFENCE,	
		MLP_PPE_ANGLE,	
		NO_MOVE,
		NO_MOVE,
	},
	//MLP_PPE_ANGLE
	{
		PPE_ANGLE_LABEL_X - STR_DOT,
		PPE_ANGLE_LABEL_Y,
		MLP_PPE_OFSY,	
		MLP_PPE_DECIDE,	
		NO_MOVE,
		NO_MOVE,
	},
	//MLP_PPE_DECIDE
	{
		DECIDE_LABEL_X - STR_DOT,
		DECIDE_LABEL_Y,
		MLP_PPE_ANGLE,	
		NO_MOVE,	
		NO_MOVE,
		MLP_PPE_CANCEL,
	},
	//MLP_PPE_CANCEL
	{
		CANCEL_LABEL_X - STR_DOT,
		CANCEL_LABEL_Y,
		MLP_PPE_ANGLE,	
		NO_MOVE,	
		MLP_PPE_DECIDE,
		NO_MOVE,
	},
};

static const MENU_SCREEN_PARAM msp_ppe = {
	NELEMS( msd_ppe ),
	&msd_ppe[ 0 ],
	&mlp_ppe[ 0 ],
};

//============================================================================================
/**
 *		���j���[���X�g
 */
//============================================================================================

enum{
	MENULIST_TITLE = 0,
	MENULIST_PPE,
};

static const MENU_SCREEN_PARAM	*msp[] = {
	&msp_title,
	&msp_ppe,
};

