//==============================================================================
/**
 * @file	balloon_tool.c
 * @brief	���D����F�c�[����
 * @author	matsuda
 * @date	2007.11.25(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/palanm.h"
#include "print\printsys.h"
#include <arc_tool.h>
#include "print\gf_font.h"
#include "net\network_define.h"
#include "message.naix"
#include "system/wipe.h"
#include <procsys.h>
#include "system/main.h"
#include "print\gf_font.h"
#include "font/font.naix"
#include "font/font.naix"

#include "balloon_common.h"
#include "balloon_comm_types.h"
#include "balloon_game_types.h"
#include "net_app/balloon.h"
#include "balloon_game.h"
#include "balloon_tcb_pri.h"
#include "balloon_sonans.h"
#include "balloon_tool.h"
#include "comm_command_balloon.h"
#include "balloon_comm_types.h"
#include "balloon_send_recv.h"
#include "balloon_id.h"
#include "balloon_control.h"
#include "balloon_snd_def.h"

#include "balloon_gra_def.h"
#include "balloon_particle.naix"
#include "balloon_particle_lst.h"
#include "msg/msg_balloon.h"
#include "wlmngm_tool.naix"		//�^�b�`�y���O���t�B�b�N
//#include "system/actor_tool.h"
#include "arc_def.h"
#include "system/sdkdef.h"
#include "system/gfl_use.h"
#include <calctool.h>
#include "system/bmp_winframe.h"


//==============================================================================
//	���D�̑ϋv��
//==============================================================================
///���D�o���ʒuX
#define BALLOON_APPEAR_POS_X	(128)
///���D�o���ʒuY
#define BALLOON_APPEAR_POS_Y	(96)

///���D�̍Œ�g�k��(�ϋv�͂�0�ł��A�ŒႱ�̑傫���͈ێ�����)
#define BALLOON_SCALE_MIN	(0x0300)
///���̃X�P�[���l���傫���Ȃ�����X��c�܂��n�߂�
#define BALLOON_SCALE_DISTORTION_START	(FX32_ONE)
///���D��c�܂���ׂɏ�Z����
#define BALLOON_SCALE_DISTORTION_X		(FX32_CONST(1.1))

///�����̋�C�̏ꍇ�A����ʂ̋�C����ɓ������Ă���o���̂ŁA���̕��o��܂ŃE�F�C�g������
#define MY_AIR_2D_APPEAR_WAIT		(25)
///��C�A�N�^�[�̈ړ����x(fx32)
#define AIR_MOVE_SPEED		(FX32_CONST(5.0))

///��ړ��̋�C�A�N�^�[��������ʒu�FY
#define AIR_MOVE_UP_DELETE_Y	(98+16)// + 16/2)
///�E�ړ��̋�C�A�N�^�[��������ʒu�FY
#define AIR_MOVE_RIGHT_DELETE_X	(128-24)// - 16/2)
///���ړ��̋�C�A�N�^�[��������ʒu�FY
#define AIR_MOVE_DOWN_DELETE_Y	(98-24)// - 16/2)
///���ړ��̋�C�A�N�^�[��������ʒu�FX
#define AIR_MOVE_LEFT_DELETE_X	(128+24)// + 16/2)

///�W���C���g�̉��h�b�g��O����k�����n�߂邩
#define AIR_REDUCE_DOT_OFFSET		(24/2 + 2)	//��COBJ�����a��24�h�b�g�Ȃ̂ŁB+2=������

///���D�A�C�R���̕\�����WX(��ԉE�[�Ɉʒu��������X���W)
#define ICON_BALLOON_POS_RIGHT_X	(9 * 8)
///���D�A�C�R���̕\�����WY
#define ICON_BALLOON_POS_Y			(4 * 8)
///���D�A�C�R���̔z�u�Ԋu�FX
#define ICON_BALLOON_POS_OFFSET_X	(16)

///���D�A�C�R���̈ړ����xX
#define ICON_BALLOON_MOVE_SP_X		(2)
///���D�̑ϋv�͂̂��̃p�[�Z���e�[�W�ȏ�(100%���Z)�ɂȂ�ƕ��D�A�C�R���̓s���`��ԂɂȂ�
#define ICON_BALLOON_PINCH_PARCENT	(75)

///���D�̂����፬���Z���ɓ����Ă���A�j���V�[�P���X
enum{
	MIXOBJ_ANMSEQ_ICON_BALLOON_1,	//���D�A�C�R���F���x���P�i�m�[�}��)
	MIXOBJ_ANMSEQ_ICON_BALLOON_2,	//���D�A�C�R���F���x���Q�i�m�[�}��)
	MIXOBJ_ANMSEQ_ICON_BALLOON_3,	//���D�A�C�R���F���x���R�i�m�[�}��)
	MIXOBJ_ANMSEQ_ICON_BALLOON_1_PINCH,		//���D�A�C�R���F���x��1�i�j�􂵂���)
	MIXOBJ_ANMSEQ_ICON_BALLOON_2_PINCH,		//���D�A�C�R���F���x��2�i�j�􂵂���)
	MIXOBJ_ANMSEQ_ICON_BALLOON_3_PINCH,		//���D�A�C�R���F���x��3�i�j�􂵂���)
	MIXOBJ_ANMSEQ_ICON_BALLOON_EXPLODED,	//���D�A�C�R���F�j��A�j��
	MIXOBJ_ANMSEQ_AIR_0,			//��C1�l��
	MIXOBJ_ANMSEQ_AIR_1,			//��C2�l��
	MIXOBJ_ANMSEQ_AIR_2,			//��C3�l��
	MIXOBJ_ANMSEQ_AIR_3,			//��C4�l��
	
	MIXOBJ_ANMSEQ_BALLOON_EXPLODED = 11,	//���D�̔j��A�j��

	MIXOBJ_ANMSEQ_BOOSTER_1P_JUMP = 12,			//�u�[�X�^�[(��)�F�W�����v
	MIXOBJ_ANMSEQ_BOOSTER_1P_SWELLING,			//�u�[�X�^�[(��)�F�c���
	MIXOBJ_ANMSEQ_BOOSTER_1P_SHRIVEL,			//�u�[�X�^�[(��)�F���ڂ�
	MIXOBJ_ANMSEQ_BOOSTER_1P_WEAK_JUMP,			//�u�[�X�^�[(��:��)�F�W�����v
	MIXOBJ_ANMSEQ_BOOSTER_1P_WEAK_SWELLING,		//�u�[�X�^�[(��:��)�F�c���
	MIXOBJ_ANMSEQ_BOOSTER_1P_WEAK_SHRIVEL,		//�u�[�X�^�[(��:��)�F���ڂ�
	MIXOBJ_ANMSEQ_BOOSTER_1P_HARD_JUMP,			//�u�[�X�^�[(��:��)�F�W�����v
	MIXOBJ_ANMSEQ_BOOSTER_1P_HARD_SWELLING,		//�u�[�X�^�[(��:��)�F�c���
	MIXOBJ_ANMSEQ_BOOSTER_1P_HARD_SHRIVEL,		//�u�[�X�^�[(��:��)�F���ڂ�

	MIXOBJ_ANMSEQ_BOOSTER_HIT_EFF = 33,			//�u�[�X�^�[�q�b�g�G�t�F�N�g
	MIXOBJ_ANMSEQ_BOOSTER_SHADOW = 34,			//�u�[�X�^�[�e
	MIXOBJ_ANMSEQ_BOOSTER_SHADOW_SMALL,
	MIXOBJ_ANMSEQ_BOOSTER_SHADOW_BIG,
	MIXOBJ_ANMSEQ_BOOSTER_LAND_SMOKE = 27,			//�u�[�X�^�[�q�b�g�G�t�F�N�g

	MIXOBJ_ANMSEQ_KAMI_STORM_GREEN = 21,		//���ӂԂ�(��)
	MIXOBJ_ANMSEQ_KAMI_STORM_RED,				//���ӂԂ�(��)
	MIXOBJ_ANMSEQ_KAMI_STORM_BLUE,				//���ӂԂ�(��)
	MIXOBJ_ANMSEQ_KAMI_STORM_YELLOW,			//���ӂԂ�(��)

	MIXOBJ_ANMSEQ_EXPLODED_SMOKE_1 = 28,		//�����̉�(��)
	MIXOBJ_ANMSEQ_EXPLODED_SMOKE_2,				//�����̉�(��)
	MIXOBJ_ANMSEQ_EXPLODED_SMOKE_3,				//�����̉�(��)

	MIXOBJ_ANMSEQ_JOINT_UD = 31,	//�W���C���g�㉺
	MIXOBJ_ANMSEQ_JOINT_LR,			//�W���C���g���E
};

///�u�[�X�^�[�̃A�j���V�[�P���X��
#define BOOSTER_ANMSEQ_PATERN		(3)
///�u�[�X�^�[�̃A�j���V�[�P���X�J�n�ʒu����̃I�t�Z�b�gID
enum{
	OFFSET_ANMID_BOOSTER_JUMP,			//�W�����v
	OFFSET_ANMID_BOOSTER_SWELLING,		//�c���
	OFFSET_ANMID_BOOSTER_SHRIVEL,		//���ڂ�
};

//--------------------------------------------------------------
//	�p�C�v�W
//--------------------------------------------------------------
///�p�C�v�̊W�FNCG��̏�W�L�����N�^�J�n�ʒu
#define PIPE_FUTA_UP_NCG_NO			(0x60)
///�p�C�v�̊W�FX��
#define PIPE_FUTA_UP_LEN_X			(4)
///�p�C�v�̊W�FY��
#define PIPE_FUTA_UP_LEN_Y			(2)
///�p�C�v�̊W�F�X�N���[���J�n�ʒu
#define PIPE_FUTA_UP_SCRN_POS		(8*32 + 14)	//Y+X

#define PIPE_FUTA_LEFT_NCG_NO		(0x10)
#define PIPE_FUTA_LEFT_LEN_X		(2)
#define PIPE_FUTA_LEFT_LEN_Y		(4)
#define PIPE_FUTA_LEFT_SCRN_POS		(10*32 + 11)

#define PIPE_FUTA_RIGHT_NCG_NO		(0xb)
#define PIPE_FUTA_RIGHT_LEN_X		(2)
#define PIPE_FUTA_RIGHT_LEN_Y		(4)
#define PIPE_FUTA_RIGHT_SCRN_POS	(10*32 + 19)

//--------------------------------------------------------------
//	�u�[�X�^�[
//--------------------------------------------------------------
///�u�[�X�^�[�F�ړ��ӂ蕝X
#define BOOSTER_FURIHABA_X		(76)
///�u�[�X�^�[�F�ړ��ӂ蕝Y
#define BOOSTER_FURIHABA_Y		(64+4)
///�u�[�X�^�[�F�I�t�Z�b�gY
#define BOOSTER_OFFSET_Y		(-24)
///�u�[�X�^�[�F�W�����v�ړ��ӂ蕝
#define BOOSTER_JUMP_FURIHABA_Y	(12 << FX32_SHIFT)

///�u�[�X�^�[�F��~�ӏ��̌�
#define BOOSTER_STOP_PART_NUM		(12)

///�u�[�X�^�[�F�W�����v�A�j���̍��v�t���[����(NITRO-CHARACTER�Őݒ肵�Ă���W�����v�A�j����
///�t���[�����̍��v������)
#define BOOSTER_ANIME_FRAME_JUMP_TOTAL		(8+8+12+8+8)

//�u�[�X�^�[�̓o�ꎞ�́A�o����W
#define BOOSTER_APPEAR_START_X_0		(128)
#define BOOSTER_APPEAR_START_Y_0		(-32)
#define BOOSTER_APPEAR_START_X_90		(256+32)
#define BOOSTER_APPEAR_START_Y_90		(96)
#define BOOSTER_APPEAR_START_X_180		(128)
#define BOOSTER_APPEAR_START_Y_180		(196+32)
#define BOOSTER_APPEAR_START_X_270		(-32)
#define BOOSTER_APPEAR_START_Y_270		(96)

///�u�[�X�^�[�q�b�g�G�t�F�N�g�F�{�̂���̃I�t�Z�b�gY
#define BOOSTER_HIT_OFFSET_Y		(-32)
///�u�[�X�^�[�q�b�g�G�t�F�N�g�F���̃A�j���p�^�[���܂ł̃t���[����
#define BOOSTER_HITEFF_NEXT_ANM_FRAME	(4 * FX32_ONE)

///�u�[�X�^�[�e�F�{�̂���̃I�t�Z�b�gY
#define BOOSTER_SHADOW_OFFSET_Y		(24)
///�u�[�X�^�[���n�̉��F�{�̂���̃I�t�Z�b�gY
#define BOOSTER_LAND_SMOKE_OFFSET_Y		(28)

//--------------------------------------------------------------
//	SIO�u�[�X�^�[
//--------------------------------------------------------------
///SIO�u�[�X�^�[�o�ꎞ�̃I�t�Z�b�gY
#define SIO_BOOSTER_APPEAR_OFFSET_Y		(-96)
///SIO�u�[�X�^�[�o�ꎞ��IN���x(fx32)
#define SIO_BOOSTER_IN_SPEED		(0x08000)
///SIO�u�[�X�^�[�ޏꎞ��OUT���x(fx32)
#define SIO_BOOSTER_OUT_SPEED		(SIO_BOOSTER_IN_SPEED)

//--------------------------------------------------------------
//	�^�b�`�y���f��
//--------------------------------------------------------------
///�^�b�`�y���f���F�y���A�N�^�[�̍��WX
#define DEMO_PEN_X				(128)
///�^�b�`�y���f���F�y���A�N�^�[�̍��WY
#define DEMO_PEN_Y				(16)
///�^�b�`�y���f���F�y���̑��x
#define DEMO_PEN_SPEED			(6)
///�^�b�`�y���f���F�y���������o���O�̃E�F�C�g
#define DEMO_PEN_START_WAIT		(15)
///�^�b�`�y���f���F�y������������̃E�F�C�g
#define DEMO_PEN_AFTER_WAIT		(30)

//==============================================================================
//	�\���̒�`
//==============================================================================
///���D���x�����ɒ�܂��Ă���f�[�^
typedef struct{
	u16 ncgr_id;		///<�L�����N�^�A�[�J�C�uID
	u16 nscr_id;		///<�X�N���[���A�[�J�C�uID
	fx32 max_scale;		///<�ő�X�P�[��
}BALLOON_LEVEL_DATA;

///�Q���l�����ɒ�܂��Ă���f�[�^
typedef struct{
	s32 max_air[BALLOON_LEVEL_MAX];	///<���D�̃��x�����̑ϋv��
}BALLOON_NUM_PARTICIPATION_DATA;

//--------------------------------------------------------------
//	�u�[�X�^�[
//--------------------------------------------------------------
//���t���[���Ŏ��̎����ֈړ������邩
//�����֓��B������A���t���[���҂����邩
//������(��T���ƂɃA�b�v���鑬�x
typedef struct{
	u8 move_frame;		///<���̒�~�ӏ��ֈړ����鎞�A���t���[�������Ĉړ����邩
	u8 wait_frame;		///<���n��A�҂t���[������
	u8 hit_anime_time;	///<�q�b�g�������̃A�j���ɂ�����t���[����(���K��move_frame�����������K�v������I)
	
	u8 padding;
}BOOSTER_MOVE_DATA;

//--------------------------------------------------------------
//	�j��
//--------------------------------------------------------------
///�j�􎆂ӂԂ�(�T�u���)�F�v���C���[�ʒu���̎��ӂԂ��̃A�j���V�[�P���X�ԍ�
static const u16 PlayerNoAnmSeq_Storm[][WFLBY_MINIGAME_MAX] = {
	{//0�l�Q��(�_�~�[)
		MIXOBJ_ANMSEQ_KAMI_STORM_RED,
		MIXOBJ_ANMSEQ_KAMI_STORM_GREEN,
		MIXOBJ_ANMSEQ_KAMI_STORM_BLUE,
		MIXOBJ_ANMSEQ_KAMI_STORM_YELLOW,
	},
	{//1�l�Q��	(���A���A��A�E)�F�f�o�b�O�p
		MIXOBJ_ANMSEQ_KAMI_STORM_RED,
		MIXOBJ_ANMSEQ_KAMI_STORM_GREEN,
		MIXOBJ_ANMSEQ_KAMI_STORM_BLUE,
		MIXOBJ_ANMSEQ_KAMI_STORM_YELLOW,
	},
	{//2�l�Q��	(���A��)
		MIXOBJ_ANMSEQ_KAMI_STORM_RED,
		MIXOBJ_ANMSEQ_KAMI_STORM_BLUE,
	},
	{//3�l�Q��	(���A���A�E)
		MIXOBJ_ANMSEQ_KAMI_STORM_RED,
		MIXOBJ_ANMSEQ_KAMI_STORM_YELLOW,
		MIXOBJ_ANMSEQ_KAMI_STORM_BLUE,
	},
	{//4�l�Q��	(���A���A��A�E)
		MIXOBJ_ANMSEQ_KAMI_STORM_RED,
		MIXOBJ_ANMSEQ_KAMI_STORM_GREEN,
		MIXOBJ_ANMSEQ_KAMI_STORM_BLUE,
		MIXOBJ_ANMSEQ_KAMI_STORM_YELLOW,
	},
};

///�j�Ђ̃p���b�g�ԍ�	�����D���x����
ALIGN4 static const u16 ExplodedChipPalNo[] = {
	PALOFS_SUB_EXPLODED_CHIP_BLUE,
	PALOFS_SUB_EXPLODED_CHIP_YELLOW,
	PALOFS_SUB_EXPLODED_CHIP_RED,
};

//==============================================================================
//	�f�[�^
//==============================================================================
//--------------------------------------------------------------
//	��C
//--------------------------------------------------------------
///��C�A�N�^�[�̏o�����W(����)
///BalloonPlayerSort�Ɠ������я�(�����J�n�ʒu�ɂ��č����)
static const AIR_POSITION_DATA AirPositionDataTbl[][WFLBY_MINIGAME_MAX] = {
	{//player_max���̂܂܂ŃA�N�Z�X�ł���悤��1origin���ׂ̈̃_�~�[
		{128, 192+16,	MIXOBJ_ANMSEQ_AIR_0, PALOFS_SUB_AIR_RED,	DIR_UP},		//��
	},
	
	{//�Q���l��1�l(�f�o�b�O�p)
		{128, 192+16,	MIXOBJ_ANMSEQ_AIR_0, PALOFS_SUB_AIR_RED,	DIR_UP},		//��
	},
	{//�Q���l��2�l
		{128, 192+16,	MIXOBJ_ANMSEQ_AIR_0, PALOFS_SUB_AIR_RED,	DIR_UP},		//��
		{128, 0-16,	MIXOBJ_ANMSEQ_AIR_3, PALOFS_SUB_AIR_BLUE,	DIR_DOWN},		//��
	},
	{//�Q���l��3�l
		{128, 192+16,	MIXOBJ_ANMSEQ_AIR_0, PALOFS_SUB_AIR_RED,	DIR_UP},		//��
		{0-16, 192/2,	MIXOBJ_ANMSEQ_AIR_2, PALOFS_SUB_AIR_YELLOW,	DIR_RIGHT},		//��
		{256+16, 192/2,	MIXOBJ_ANMSEQ_AIR_1, PALOFS_SUB_AIR_BLUE,	DIR_LEFT},	//�E
	},
	{//�Q���l��4�l
		{128, 192+16,	MIXOBJ_ANMSEQ_AIR_0, PALOFS_SUB_AIR_RED,	DIR_UP},		//��
		{0-16, 192/2,	MIXOBJ_ANMSEQ_AIR_2, PALOFS_SUB_AIR_GREEN,	DIR_RIGHT},		//��
		{128, 0-16,	MIXOBJ_ANMSEQ_AIR_3, PALOFS_SUB_AIR_BLUE,	DIR_DOWN},		//��
		{256, 192/2,	MIXOBJ_ANMSEQ_AIR_1, PALOFS_SUB_AIR_YELLOW,	DIR_LEFT},	//�E
	},
};

///��C�̃A�N�^�[�w�b�_(�T�u��ʗp)
static const GFL_CLWK_AFFINEDATA AirObjParam = {
	{//GFL_CLWK_DATA
		0, 0,		//pos_x, pos_y
		0, 			//anmseq
		BALLOONSUB_SOFTPRI_AIR, BALLOONSUB_BGPRI_AIR,	//softpri, bgpri
	},
	0, 0,		//�A�t�B��X���W�A�A�t�B��Y���W
	FX32_ONE, FX32_ONE,		//�g��X�l�A�g��Y�l
	0,			//��]�p�x(0�`0xffff 0xffff��360�x)
	CLSYS_AFFINETYPE_DOUBLE,	//CLSYS_AFFINETYPE
};

///���ӂԂ��̃A�N�^�[�w�b�_(�T�u��ʗp)
static const GFL_CLWK_DATA ExplodedStormObjParam = {
	BALLOON_APPEAR_POS_X, BALLOON_APPEAR_POS_Y,		//pos_x, pos_y
	0, 			//anmseq
	BALLOONSUB_SOFTPRI_EXPLODED_STORM, BALLOONSUB_BGPRI_EXPLODED,	//softpri, bgpri
};

///���D�A�C�R���̃A�N�^�[�w�b�_(�T�u��ʗp)
static const GFL_CLWK_DATA IconBalloonObjParam = {
	0, 0,		//pos_x, pos_y
	0, 			//anmseq
	BALLOONSUB_SOFTPRI_ICON_BALLOON, BALLOONSUB_BGPRI_ICON_BALLOON,	//softpri, bgpri
};

///�W���C���g�̃A�N�^�[�w�b�_(�T�u��ʗp)
static const GFL_CLWK_DATA JointObjParam = {
	0, 0,		//pos_x, pos_y
	0, 			//anmseq
	BALLOONSUB_SOFTPRI_JOINT, BALLOONSUB_BGPRI_JOINT,	//softpri, bgpri
};

///�u�[�X�^�[(�\�[�i�m)�̃A�N�^�[�w�b�_(�T�u��ʗp)
static const GFL_CLWK_DATA BoosterObjParam = {
	0, 0,		//pos_x, pos_y
	0, 			//anmseq
	BALLOONSUB_SOFTPRI_BOOSTER, BALLOONSUB_BGPRI_BOOSTER,	//softpri, bgpri
};

///�u�[�X�^�[�q�b�g���ɏo��G�t�F�N�g�̃A�N�^�[�w�b�_(�T�u��ʗp)
static const GFL_CLWK_DATA BoosterHitObjParam = {
	0, 0,		//pos_x, pos_y
	0, 			//anmseq
	BALLOONSUB_SOFTPRI_BOOSTER_HIT, BALLOONSUB_BGPRI_BOOSTER,	//softpri, bgpri
};

///�u�[�X�^�[�̉e�̃A�N�^�[�w�b�_(�T�u��ʗp)
static const GFL_CLWK_AFFINEDATA BoosterShadowObjParam = {
	{//GFL_CLWK_DATA
		0, 0,		//pos_x, pos_y
		0, 			//anmseq
		BALLOONSUB_SOFTPRI_BOOSTER_SHADOW, BALLOONSUB_BGPRI_BOOSTER,	//softpri, bgpri
	},
	0, 0,		//�A�t�B��X���W�A�A�t�B��Y���W
	FX32_ONE, FX32_ONE,		//�g��X�l�A�g��Y�l
	0,			//��]�p�x(0�`0xffff 0xffff��360�x)
	CLSYS_AFFINETYPE_NORMAL,	//CLSYS_AFFINETYPE
};

///�u�[�X�^�[�̒��n�̉��̃A�N�^�[�w�b�_(�T�u��ʗp)
static const GFL_CLWK_DATA BoosterLandSmokeObjParam = {
	0, 0,		//pos_x, pos_y
	0, 			//anmseq
	BALLOONSUB_SOFTPRI_BOOSTER_LAND_SMOKE, BALLOONSUB_BGPRI_BOOSTER,	//softpri, bgpri
};

///�J�E���^�[�E�B���h�E�̃A�N�^�[�w�b�_(���C����ʗp)
static const GFL_CLWK_DATA CounterWindowObjParam = {
	8*5, 192-16,		//pos_x, pos_y
	0, 			//anmseq
	BALLOON_SOFTPRI_COUNTER_WIN, BALLOON_BGPRI_COUNTER,	//softpri, bgpri
};

///�^�b�`�y���̃A�N�^�[�w�b�_(���C����ʗp)
static const GFL_CLWK_DATA TouchPenObjParam = {
	0, 0,		//pos_x, pos_y
	0, 			//anmseq
	BALLOON_SOFTPRI_TOUCH_PEN, BALLOON_BGPRI_TOUCH_PEN,	//softpri, bgpri
};

//--------------------------------------------------------------
//	�J���[�X���b�v
//--------------------------------------------------------------
///���O�E�B���h�E�̃p���b�g�ԍ�
enum{
	NAME_WIN_PALNO_RED = 1,
	NAME_WIN_PALNO_BLUE = 0,
	NAME_WIN_PALNO_YELLOW = 3,
	NAME_WIN_PALNO_GREEN = 2,
};

//�f�t�H���g�p���b�g�ł�2P�̃J���[�J�n�ʒu
#define PIPE_BG_COLOR_START_NO_RED		(1*16 + 0xd)
//�f�t�H���g�p���b�g�ł�1P�̃J���[�J�n�ʒu
#define PIPE_BG_COLOR_START_NO_BLUE		(0*16 + 0xd)
//�f�t�H���g�p���b�g�ł�4P�̃J���[�J�n�ʒu
#define PIPE_BG_COLOR_START_NO_YELLOW		(3*16 + 0xd)
//�f�t�H���g�p���b�g�ł�3P�̃J���[�J�n�ʒu
#define PIPE_BG_COLOR_START_NO_GREEN		(2*16 + 0xd)

///�p�C�vBG�F�v���C���[�ʒu���̃p���b�g�J���[�J�n�ʒu
static const u16 PlayerNoPaletteSwapTbl_Pipe[][WFLBY_MINIGAME_MAX] = {
	{//0�l�Q��(�_�~�[)
		PIPE_BG_COLOR_START_NO_RED,
		PIPE_BG_COLOR_START_NO_GREEN,
		PIPE_BG_COLOR_START_NO_BLUE,
		PIPE_BG_COLOR_START_NO_YELLOW,
	},
	{//1�l�Q��	(���A���A��A�E)�F�f�o�b�O�p
		PIPE_BG_COLOR_START_NO_RED,
		PIPE_BG_COLOR_START_NO_GREEN,
		PIPE_BG_COLOR_START_NO_BLUE,
		PIPE_BG_COLOR_START_NO_YELLOW,
	},
	{//2�l�Q��	(���A��)
		PIPE_BG_COLOR_START_NO_RED,
		PIPE_BG_COLOR_START_NO_BLUE,
	},
	{//3�l�Q��	(���A���A�E)
		PIPE_BG_COLOR_START_NO_RED,
		PIPE_BG_COLOR_START_NO_YELLOW,
		PIPE_BG_COLOR_START_NO_BLUE,
	},
	{//4�l�Q��	(���A���A��A�E)
		PIPE_BG_COLOR_START_NO_RED,
		PIPE_BG_COLOR_START_NO_GREEN,
		PIPE_BG_COLOR_START_NO_BLUE,
		PIPE_BG_COLOR_START_NO_YELLOW,
	},
};
///�p�C�vBG�F�v���C���[�J���[���p���b�g�J�n�ʒu���炢�������Ă��邩
#define PLAYER_NO_PALETTE_COLOR_NUM_PIPE		(3)

///�p�C�vBG�F�v���C���[�ʒu���̃X���b�v�����p���b�g�������݈ʒu
static const u16 PlayerNoPaletteWritePosTbl_Pipe[WFLBY_MINIGAME_MAX] = {
	PIPE_BG_COLOR_START_NO_RED,
	PIPE_BG_COLOR_START_NO_BLUE,
	PIPE_BG_COLOR_START_NO_YELLOW,
	PIPE_BG_COLOR_START_NO_GREEN,
};

//�f�t�H���g�p���b�g�ł�2P�̃J���[�J�n�ʒu
#define OBJ_COLOR_START_NO_RED		(2*16 + 0)
//�f�t�H���g�p���b�g�ł�1P�̃J���[�J�n�ʒu
#define OBJ_COLOR_START_NO_BLUE		(1*16 + 0)
//�f�t�H���g�p���b�g�ł�4P�̃J���[�J�n�ʒu
#define OBJ_COLOR_START_NO_YELLOW		(3*16 + 0)
//�f�t�H���g�p���b�g�ł�3P�̃J���[�J�n�ʒu
#define OBJ_COLOR_START_NO_GREEN		(4*16 + 0)

///�v���C���[OBJ(�T�u���)�F�v���C���[�ʒu���̃p���b�g�J���[�J�n�ʒu(PLTTID_SUB_OBJ_COMMON�����offset)
static const u16 PlayerNoPaletteSwapTbl_Obj[][WFLBY_MINIGAME_MAX] = {
	{//0�l�Q��(�_�~�[)
		OBJ_COLOR_START_NO_RED,
		OBJ_COLOR_START_NO_BLUE,
		OBJ_COLOR_START_NO_YELLOW,
		OBJ_COLOR_START_NO_GREEN,
	},
	{//1�l�Q��	(���A���A��A�E)�F�f�o�b�O�p
		OBJ_COLOR_START_NO_RED,
		OBJ_COLOR_START_NO_BLUE,
		OBJ_COLOR_START_NO_YELLOW,
		OBJ_COLOR_START_NO_GREEN,
	},
	{//2�l�Q��	(���A��)
		OBJ_COLOR_START_NO_RED,
		OBJ_COLOR_START_NO_BLUE,
	},
	{//3�l�Q��	(���A���A�E)
		OBJ_COLOR_START_NO_RED,
		OBJ_COLOR_START_NO_YELLOW,
		OBJ_COLOR_START_NO_BLUE,
	},
	{//4�l�Q��	(���A���A��A�E)
		OBJ_COLOR_START_NO_RED,
		OBJ_COLOR_START_NO_GREEN,
		OBJ_COLOR_START_NO_BLUE,
		OBJ_COLOR_START_NO_YELLOW,
	},
};
///�v���C���[OBJ�F�v���C���[�J���[���p���b�g�J�n�ʒu���炢�������Ă��邩
#define PLAYER_NO_PALETTE_COLOR_NUM_AIR		(16)

///�v���C���[OBJ�F�v���C���[�ʒu���̃X���b�v�����p���b�g�������݈ʒu
static const u16 PlayerNoPaletteWritePosTbl_Obj[WFLBY_MINIGAME_MAX] = {
	OBJ_COLOR_START_NO_RED,
	OBJ_COLOR_START_NO_BLUE,
	OBJ_COLOR_START_NO_YELLOW,
	OBJ_COLOR_START_NO_GREEN,
};


///�G�A�[OBJ�̃X�P�[���l(�u�[�X�^�[�^�C�v��)
static const fx32 Air_BoosterScale[] = {
	FX32_ONE, 			//BOOSTER_TYPE_NONE
	0x1800,				//BOOSTER_TYPE_NORMAL
	0xa00,				//BOOSTER_TYPE_WEAK
	0x2000,				//BOOSTER_TYPE_HARD
};

//�f�t�H���g�p���b�g�ł�2P�̃J���[�J�n�ʒu
#define STORM_COLOR_START_NO_RED		(5*16 + 0x2)
//�f�t�H���g�p���b�g�ł�1P�̃J���[�J�n�ʒu
#define STORM_COLOR_START_NO_BLUE		(5*16 + 0x1)
//�f�t�H���g�p���b�g�ł�4P�̃J���[�J�n�ʒu
#define STORM_COLOR_START_NO_YELLOW		(5*16 + 0x3)
//�f�t�H���g�p���b�g�ł�3P�̃J���[�J�n�ʒu
#define STORM_COLOR_START_NO_GREEN		(5*16 + 0x4)

///���ӂԂ��F�v���C���[�ʒu���̃p���b�g�J���[�J�n�ʒu
static const u16 PlayerNoPaletteSwapTbl_Storm[][WFLBY_MINIGAME_MAX] = {
	{//0�l�Q��(�_�~�[)
		STORM_COLOR_START_NO_RED,
		STORM_COLOR_START_NO_BLUE,
		STORM_COLOR_START_NO_YELLOW,
		STORM_COLOR_START_NO_GREEN,
	},
	{//1�l�Q��	(���A���A��A�E)�F�f�o�b�O�p
		STORM_COLOR_START_NO_RED,
		STORM_COLOR_START_NO_BLUE,
		STORM_COLOR_START_NO_YELLOW,
		STORM_COLOR_START_NO_GREEN,
	},
	{//2�l�Q��	(���A��)
		STORM_COLOR_START_NO_RED,
		STORM_COLOR_START_NO_BLUE,
	},
	{//3�l�Q��	(���A���A�E)
		STORM_COLOR_START_NO_RED,
		STORM_COLOR_START_NO_YELLOW,
		STORM_COLOR_START_NO_BLUE,
	},
	{//4�l�Q��	(���A���A��A�E)
		STORM_COLOR_START_NO_RED,
		STORM_COLOR_START_NO_GREEN,
		STORM_COLOR_START_NO_BLUE,
		STORM_COLOR_START_NO_YELLOW,
	},
};
///�p�C�vBG�F�v���C���[�J���[���p���b�g�J�n�ʒu���炢�������Ă��邩
#define PLAYER_NO_PALETTE_COLOR_NUM_STORM		(1)

///���ӂԂ��F�v���C���[�ʒu���̃X���b�v�����p���b�g�������݈ʒu
static const u16 PlayerNoPaletteWritePosTbl_Storm[WFLBY_MINIGAME_MAX] = {
	STORM_COLOR_START_NO_RED,
	STORM_COLOR_START_NO_BLUE,
	STORM_COLOR_START_NO_YELLOW,
	STORM_COLOR_START_NO_GREEN,
};

//--------------------------------------------------------------
//	�W���C���g
//--------------------------------------------------------------
///�W���C���g�̃A�N�^�[���W
static const struct{
	s16 x;
	s16 y;
}JointActorPosTbl[] = {
	{128, 164},		//JOINT_ACTOR_D
	{128, 28},		//JOINT_ACTOR_U
	{52, 96},		//JOINT_ACTOR_L
	{204, 96},		//JOINT_ACTOR_R
};

//--------------------------------------------------------------
//	�u�[�X�^�[
//--------------------------------------------------------------
///�u�[�X�^�[�^�C�v���̃A�j���V�[�P���X�J�n�ԍ�
static const u16 BoosterType_StartSeqAnimeNo[] = {
	MIXOBJ_ANMSEQ_BOOSTER_1P_JUMP,		//NONE
	MIXOBJ_ANMSEQ_BOOSTER_1P_JUMP,
	MIXOBJ_ANMSEQ_BOOSTER_1P_WEAK_JUMP,
	MIXOBJ_ANMSEQ_BOOSTER_1P_HARD_JUMP,
};

///�u�[�X�^�[OBJ(�T�u���)�F�v���C���[�ʒu���̃u�[�X�^�[�p���b�g�I�t�Z�b�g�ԍ�
static const u16 PlayerNoPalOfs_Booster[][WFLBY_MINIGAME_MAX] = {
	{//0�l�Q��(�_�~�[)
		PALOFS_SUB_BOOSTER_RED,
		PALOFS_SUB_BOOSTER_GREEN,
		PALOFS_SUB_BOOSTER_BLUE,
		PALOFS_SUB_BOOSTER_YELLOW,
	},
	{//1�l�Q��	(���A���A��A�E)�F�f�o�b�O�p
		PALOFS_SUB_BOOSTER_RED,
		PALOFS_SUB_BOOSTER_GREEN,
		PALOFS_SUB_BOOSTER_BLUE,
		PALOFS_SUB_BOOSTER_YELLOW,
	},
	{//2�l�Q��	(���A��)
		PALOFS_SUB_BOOSTER_RED,
		PALOFS_SUB_BOOSTER_BLUE,
	},
	{//3�l�Q��	(���A���A�E)
		PALOFS_SUB_BOOSTER_RED,
		PALOFS_SUB_BOOSTER_YELLOW,
		PALOFS_SUB_BOOSTER_BLUE,
	},
	{//4�l�Q��	(���A���A��A�E)
		PALOFS_SUB_BOOSTER_RED,
		PALOFS_SUB_BOOSTER_GREEN,
		PALOFS_SUB_BOOSTER_BLUE,
		PALOFS_SUB_BOOSTER_YELLOW,
	},
};

///SIO�u�[�X�^�[OBJ(�T�u���)�F�v���C���[�ʒu���̃W���C���g�Ή��ԍ�
static const u8 PlayerNoJointNo_SioBooster[][WFLBY_MINIGAME_MAX] = {
	{//player_max���̂܂܂ŃA�N�Z�X�ł���悤��1origin���ׂ̈̃_�~�[
		JOINT_ACTOR_D,
	},
	{//1�l�Q��	(���A���A��A�E)�F�f�o�b�O�p
		JOINT_ACTOR_D,
		JOINT_ACTOR_L,
		JOINT_ACTOR_U,
		JOINT_ACTOR_R,
	},
	{//2�l�Q��	(���A��)
		JOINT_ACTOR_D,
		JOINT_ACTOR_U,
	},
	{//3�l�Q��	(���A���A�E)
		JOINT_ACTOR_D,
		JOINT_ACTOR_L,
		JOINT_ACTOR_R,
	},
	{//4�l�Q��	(���A���A��A�E)
		JOINT_ACTOR_D,
		JOINT_ACTOR_L,
		JOINT_ACTOR_U,
		JOINT_ACTOR_R,
	},
};


//==============================================================================
//	�O���f�[�^
//==============================================================================
#include "balloon_control.dat"


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static void BalloonTool_BalloonAffineScaleUpdate(
	BALLOON_STATUS *bst, int player_max);
int BalloonTool_AirParcentGet(BALLOON_GAME_PTR game);
static void BalloonTool_BalloonAffineParamGet(
	int player_max, int balloon_lv, int air, fx32 *scale_x, fx32 *scale_y);
static int BalloonTool_AirStackAdd(BALLOON_GAME_PTR game, BALLOON_STATUS *bst, 
	int air, int net_id, int booster_type);
static int Air_ParamCreate(BALLOON_GAME_PTR game, const BALLOON_AIR_DATA * air_data, PLAYER_AIR_PARAM *air_param);
static BOOL Air_Move(BALLOON_GAME_PTR game, PLAYER_AIR_PARAM *air_param);
static GFL_CLWK* Air_ActorCreate(BALLOON_GAME_PTR game, const AIR_POSITION_DATA *air_posdata);
static void Exploded_ActorCreate(BALLOON_GAME_PTR game, EXPLODED_PARAM *exploded, BALLOON_STATUS *bst);
static void BalloonTool_BalloonBGErase(BALLOON_STATUS *bst);
static GFL_CLWK* IconBalloon_ActorCreate(BALLOON_GAME_PTR game, int icon_type, int pos);
void IconBalloon_AllCreate(BALLOON_GAME_PTR game);
BOOL IconBalloon_Update(BALLOON_GAME_PTR game);
static BOOL BalloonTool_IconBalloonParamAdd(BALLOON_GAME_PTR game, ICONBALLOON_PARAM *ibp, int pos, int balloon_no);
static void IconBalloon_PosMove(ICONBALLOON_PARAM *ibp);
static void IconBalloon_StatusUpdate(BALLOON_GAME_PTR game, ICONBALLOON_PARAM *ibp);
static int IconBalloon_TypeGet(int balloon_no);
static void IconBalloon_ExplodedReq(BALLOON_GAME_PTR game);
static void Booster_Move(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move);
static void BoosterMoveModeChange(BOOSTER_MOVE *move, int mode);
static int BoosterMove_Wait(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move);
static int BoosterMove_Appear(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move);
static int BoosterMove_Normal(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move);
static int BoosterMove_Hit(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move);
static int Booster_HitCheckNow(BALLOON_GAME_PTR game, BOOSTER_MOVE **ret_move);
static BOOL SioBoosterMove_Appear(SIO_BOOSTER_WORK *sio_booster, SIO_BOOSTER_MOVE *sio_move);
static void Balloon_CounterLastNumberSet(BALLOON_COUNTER *counter);
static BOOL Balloon_CounterAgreeCheck(BALLOON_COUNTER *counter);
static BOOL Balloon_CounterRotate(BALLOON_COUNTER *counter);
static void BoosterLandSmoke_ActorCreate(BALLOON_GAME_PTR game, BOOSTER_LAND_SMOKE *land_smoke);
static void BoosterLandSmoke_ActorDelete(BALLOON_GAME_PTR game, BOOSTER_LAND_SMOKE *land_smoke);
static void BoosterLandSmoke_SmokeReq(BALLOON_GAME_PTR game, BOOSTER_MOVE *move, BOOSTER_LAND_SMOKE *land_smoke);
static void BoosterLandSmoke_Update(BALLOON_GAME_PTR game, BOOSTER_MOVE *move, BOOSTER_LAND_SMOKE *land_smoke);


//==============================================================================
//	�V�[�P���X�e�[�u��
//==============================================================================
///�u�[�X�^�[�̓��샂�[�h	��BoosterMoveSeqTbl�ƕ��т𓯂��ɂ��Ă������ƁI
enum{
	BOOSTER_MODE_WAIT,			///<�ҋ@��
	BOOSTER_MODE_APPEAR,		///<�o��ړ�
	BOOSTER_MODE_NORMAL,		///<�ʏ�
	BOOSTER_MODE_HIT,			///<�q�b�g
};

///�u�[�X�^�[����֐��̃V�[�P���X�e�[�u��	��BOOSTER_MODE_???�ƕ��т𓯂��ɂ��Ă������ƁI
static int (* const BoosterMoveSeqTbl[])(BALLOON_GAME_PTR, BOOSTER_WORK *, BOOSTER_MOVE *) = {
	BoosterMove_Wait,
	BoosterMove_Appear,
	BoosterMove_Normal,
	BoosterMove_Hit,
};



//--------------------------------------------------------------
/**
 * @brief   ���DBG��W�J����
 *
 * @param   none
 * @param   player_max		�Q���l��
 * @param   level			���D�̃��x��(BALLOON_LEVEL_???)
 * @param   bst				�W�J�������D�̃p�����[�^�Z�b�g��
 */
//--------------------------------------------------------------
void BalloonTool_BalloonBGSet(int player_max, int level, BALLOON_STATUS *bst)
{
	ARCHANDLE* hdl;

	OS_TPrintf("balloon level = %d\n", level);
	
	//�n���h���I�[�v��
	hdl  = GFL_ARC_OpenDataHandle(ARCID_BALLOON_GRA,  HEAPID_BALLOON); 

	GFL_ARCHDL_UTIL_TransVramBgCharacter(hdl, BalloonLevelData[level].ncgr_id, 
		BALLOON_SUBFRAME_BALLOON, 0, 0, 0, HEAPID_BALLOON);
	GFL_ARCHDL_UTIL_TransVramScreen(hdl, BalloonLevelData[level].nscr_id, 
		BALLOON_SUBFRAME_BALLOON, 0, 0, 0, HEAPID_BALLOON);

	//�n���h������
	GFL_ARC_CloseDataHandle( hdl );
	
	
	//�X�e�[�^�X�p�����[�^�Z�b�g
	GFL_STD_MemClear(bst, sizeof(BALLOON_STATUS));
	bst->max_air = BalloonParicipationData[player_max].max_air[level];
	bst->level = level;
	bst->occ = TRUE;
	bst->bg_on_req = TRUE;
	
	//�g�k���ݒ�
	BalloonTool_BalloonAffineScaleUpdate(bst, player_max);
	
#if WB_TEMP_FIX
	Snd_SePlay(SE_BALLOON_APPEAR);
#endif
}

//--------------------------------------------------------------
/**
 * @brief   ���DBG����������
 *
 * @param   none
 * @param   bst		���D�p�����[�^�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void BalloonTool_BalloonBGErase(BALLOON_STATUS *bst)
{
	GFL_BG_ClearScreen(BALLOON_SUBFRAME_BALLOON);
	bst->occ = FALSE;
	bst->bg_off_req = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���D�̊g�k�������݂̋�C�ʂōX�V
 *
 * @param   none
 * @param   bst			���D�p�����[�^�ւ̃|�C���^
 * @param   player_max	�Q���l��
 */
//--------------------------------------------------------------
static void BalloonTool_BalloonAffineScaleUpdate(BALLOON_STATUS *bst, int player_max)
{
	fx32 scale_x, scale_y;
	int x_offset, y_offset;
	
	if(bst->occ == FALSE){
		return;
	}
	
	BalloonTool_BalloonAffineParamGet(player_max, bst->level, bst->air, &scale_x, &scale_y);
	
	//BG�̕\���𒆐S���ێ�����ׁA�X�P�[���l�ɉ�����X,Y���W�̃I�t�Z�b�g�𑫂�����
	x_offset = 0;
	y_offset = 0;
	if(scale_x == FX32_ONE){
		x_offset = 0;
	}
	else if(scale_x < FX32_ONE){
		x_offset = 128 - (128 * scale_x / FX32_ONE);
	}
	else{// if(scale_x > FX32_ONE){
		x_offset = -(128 * (scale_x - FX32_ONE) / FX32_ONE);
	}

	if(scale_y == FX32_ONE){
		y_offset = 0;
	}
	else if(scale_y < FX32_ONE){
		y_offset = 128 - (128 * scale_y / FX32_ONE);
	}
	else{// if(scale_y > Fy32_ONE){
		y_offset = -(128 * (scale_y - FX32_ONE) / FX32_ONE);
	}
	
#if 1
	{
		MtxFx22 mtx22;
	    fx32 rScale_x,rScale_y;
		int shake_x;
		
		if(bst->air_stack == 0){
			shake_x = 0;
		}
		else{
			//shake_x = bst->air % 8;
			shake_x = GFUser_GetPublicRand(8) + 1;
			if(bst->shake_flag & 1){
				shake_x = -shake_x;
			}
			bst->shake_flag ^= 1;
		}
		
		rScale_x = FX_Inv(scale_x);
		rScale_y = FX_Inv(scale_y);

		GFL_BG_SetScaleReq(BALLOON_SUBFRAME_BALLOON, GFL_BG_SCALE_X_SET, rScale_x);
		GFL_BG_SetScaleReq(BALLOON_SUBFRAME_BALLOON, GFL_BG_SCALE_Y_SET, rScale_y);

		GFL_BG_SetScrollReq(BALLOON_SUBFRAME_BALLOON, GFL_BG_SCROLL_X_SET, 0-x_offset + shake_x);
		GFL_BG_SetScrollReq(BALLOON_SUBFRAME_BALLOON, GFL_BG_SCROLL_Y_SET, (256-192)/2 + 7 - y_offset);
	}
#else
	{
		MtxFx22 mtx22;
	    fx32 rScale_x,rScale_y;
		
		rScale_x = FX_Inv(scale_x);
		rScale_y = FX_Inv(scale_y);
	    mtx22._00 = rScale_x;
	    mtx22._01 = 0;
	    mtx22._10 = 0;
	    mtx22._11 = rScale_y;
		
		GF_BGL_AffineScrollSet(BALLOON_SUBFRAME_BALLOON, GFL_BG_SCROLL_X_SET, 0 - x_offset, &mtx22, 0, 0);
		GF_BGL_AffineScrollSet(BALLOON_SUBFRAME_BALLOON, GFL_BG_SCROLL_Y_SET, (256-192)/2 - y_offset, &mtx22, 0, 0);
	}
#endif
}

//--------------------------------------------------------------
/**
 * @brief   ���D�̌��݂̋�C�ʂ��畗�DBG�̊g�k�����Z�o����
 * @param   air		���D�̌��݋�C��
 * @retval  �g�k��
 */
//--------------------------------------------------------------
static void BalloonTool_BalloonAffineParamGet(int player_max, int balloon_lv, int air, fx32 *scale_x, fx32 *scale_y)
{
	fx32 scale;
	
	scale = BalloonLevelData[balloon_lv].max_scale * air / BalloonParicipationData[player_max].max_air[balloon_lv];
	scale += BALLOON_SCALE_MIN;
	
	*scale_x = scale;
	*scale_y = scale;
	
	//������x�傫���Ȃ������ŕ��D��c�܂���ׁAX�����̃X�P�[����傫������
	if(scale > BALLOON_SCALE_DISTORTION_START){
		*scale_x += FX_Mul((scale - BALLOON_SCALE_DISTORTION_START), BALLOON_SCALE_DISTORTION_X);
	}
	
//	scale = BALLOON_SCALE_MAX - scale;	//�����������������傫���Ȃ�̂Ŕ��]
//	return scale;
}

//--------------------------------------------------------------
/**
 * @brief   ���D�ԍ����畗�D�̃��x�����擾����
 * @param   balloon_no		���D�ԍ�
 * @retval  ���D�̃��x��(BALLOON_LEVEL_???)
 */
//--------------------------------------------------------------
int BalloonTool_BalloonLevelGet(int balloon_no)
{
	if(BALLOON_LEVEL3_NUM <= balloon_no){
		return BALLOON_LEVEL_3;
	}
	else if(BALLOON_LEVEL2_NUM <= balloon_no){
		return BALLOON_LEVEL_2;
	}
	return BALLOON_LEVEL_1;
}

//--------------------------------------------------------------
/**
 * @brief   ���D�ɒ��������C���X�^�b�N����
 *
 * @param   bst		���D�p�����[�^�ւ̃|�C���^
 * @param   air		���Z�����C��
 * @param   net_id	�l�b�gID
 * @param   booster_type	�u�[�X�^�[�^�C�v
 *
 * @retval  ���Z��̃X�^�b�N��C��
 */
//--------------------------------------------------------------
static int BalloonTool_AirStackAdd(BALLOON_GAME_PTR game, BALLOON_STATUS *bst, int air, int net_id, int booster_type)
{
	bst->air_stack += air;
	bst->add_air = bst->air_stack / AIR_ADD_FRAME;
	bst->player_air[Balloon_NetID_to_PlayerPos(game, net_id)] += air;
	OS_TPrintf("�ǉ����ꂽair = %d, add_air = %d, stack = %d, net_id = %d, booster_type = %d, main_frame = %d(%dsec)\n", air, bst->add_air, bst->air_stack, net_id, booster_type, game->main_frame, game->main_frame/30);
	
	if(net_id == GFL_NET_SystemGetCurrentID()){
		game->my_total_air += air;
		Balloon_CounterNextNumberSet(&game->counter, game->my_total_air);
		OS_TPrintf("my_total_air = %d\n", game->my_total_air);
	}
	return bst->air_stack;
}

//--------------------------------------------------------------
/**
 * @brief   ���D�ɃX�^�b�N����Ă����C�𒍓�����
 *
 * @param   bst		���D�p�����[�^�ւ̃|�C���^
 *
 * @retval  TRUE:�������ꂽ�B�@FALSE:����Ă��Ȃ�
 */
//--------------------------------------------------------------
static int BalloonTool_AirUpdate(BALLOON_STATUS *bst)
{
	int add_air;
	
	if(bst->air_stack == 0){
		return FALSE;
	}
	
	if(bst->air_stack >= bst->add_air){
		add_air = bst->add_air;
	}
	else{
		add_air = bst->air_stack;
	}
	bst->air_stack -= add_air;
	bst->air += add_air;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���D�X�V����
 *
 * @param   none
 * @param   bst		���D�p�����[�^�ւ̃|�C���^
 */
//--------------------------------------------------------------
void BalloonTool_BalloonUpdate(BALLOON_GAME_PTR game, BALLOON_STATUS *bst)
{
	if(game->balloon_occ == FALSE){
		return;
	}
	BalloonTool_AirUpdate(bst);
	BalloonTool_BalloonAffineScaleUpdate(bst, game->bsw->player_max);
}

//--------------------------------------------------------------
/**
 * @brief   ���݂̕��D�ϋv�͂��p�[�Z���e�[�W�Ŏ擾����
 *
 * @param   game		
 *
 * @retval  ���D�ϋv�͂��p�[�Z���e�[�W���Z��������(100%MAX)
 */
//--------------------------------------------------------------
int BalloonTool_AirParcentGet(BALLOON_GAME_PTR game)
{
	BALLOON_STATUS *bst = &game->bst;
	
	return 100 * bst->air / bst->max_air;
}

//--------------------------------------------------------------
/**
 * @brief   �v���C���[������������C�����p�����[�^�o�^����
 *
 * @param   air_data		��C�f�[�^
 *
 * @retval  TRUE:�o�^�����B�@FALSE:���s
 */
//--------------------------------------------------------------
BOOL BalloonTool_PlayerAirParamAdd(BALLOON_GAME_PTR game, const BALLOON_AIR_DATA *air_data)
{
	int i, arrival_frame;
	
	if(game->balloon_no != air_data->no){
//		OS_TPrintf("�Ⴄ���D�ԍ��̃G�A�[�ׁ̈A����\n");
//		return FALSE;	//�Ⴄ���D�ԍ��̃G�A�[�ׁ̈A����
	}
	
	for(i = 0; i < PLAYER_AIR_PARAM_MAX; i++){
		if(game->air_param[i].occ == FALSE){
			arrival_frame = Air_ParamCreate(game, air_data, &game->air_param[i]);
			SioBooster_Appear(game, &game->sio_booster, 
				air_data->booster_type, air_data->net_id, arrival_frame);
			return TRUE;
		}
	}
	
	//���i�łł̓��[�N�������ς����������́A���̃f�[�^�͖����ƌ��Ȃ�
	//�f�o�b�O���̓p�����[�^���ǂ̒��x�̕p�x�ŃI�[�o�[�t���[���N���������؂����
	//assert�Ŏ~�߂�
	GF_ASSERT(0);
	return FALSE;	//�p�����[�^���[�N�������ς��ŐV�K�ǉ����o���Ȃ�
}

//--------------------------------------------------------------
/**
 * @brief   ��C�p�����[�^���[�N�𐶐�
 *
 * @param   game			
 * @param   air_data		��C�f�[�^
 * @param   air_param		�p�����[�^�Z�b�g��
 * 
 * @retval  �W���C���g�����܂ł̃t���[����
 */
//--------------------------------------------------------------
static int Air_ParamCreate(BALLOON_GAME_PTR game, const BALLOON_AIR_DATA * air_data, PLAYER_AIR_PARAM *air_param)
{
	int player_pos;
	const AIR_POSITION_DATA *air_posdata;
	int arrival_frame;
	GFL_CLACTPOS before;
	
	GF_ASSERT(air_param->occ == FALSE);
	
	GFL_STD_MemClear(air_param, sizeof(PLAYER_AIR_PARAM));
	
	player_pos = Balloon_NetID_to_PlayerPos(game, air_data->net_id);
	air_posdata = &AirPositionDataTbl[game->bsw->player_max][player_pos];
	
	air_param->net_id = air_data->net_id;
	air_param->balloon_no = air_data->no;
	air_param->air = air_data->last_air;
	air_param->apd = air_posdata;
	air_param->booster_type = air_data->booster_type;
	air_param->cap = Air_ActorCreate(game, air_posdata);

	GFL_CLACT_WK_GetPos(air_param->cap, &before, CLSYS_DEFREND_SUB);
	air_param->x = before.x * FX32_ONE;
	air_param->y = before.y * FX32_ONE;

	if(air_data->net_id == GFL_NET_SystemGetCurrentID()){
		Air3D_EntryAdd(game, air_data->air);
		air_param->wait = MY_AIR_2D_APPEAR_WAIT;
		GFL_CLACT_WK_SetDrawEnable(air_param->cap, FALSE);
	}
	
	air_param->occ = TRUE;


	{//�W���C���g�����܂ł̃t���[������Ԃ�
		s32 before_joint_offset = 0;

		switch(air_posdata->move_dir){
		case DIR_DOWN:
			before_joint_offset = JointActorPosTbl[JOINT_ACTOR_U].y - before.y;
			break;
		case DIR_LEFT:
			before_joint_offset = before.x - JointActorPosTbl[JOINT_ACTOR_R].x;
			break;
		case DIR_UP:
			before_joint_offset = before.y - JointActorPosTbl[JOINT_ACTOR_D].y;
			break;
		case DIR_RIGHT:
			before_joint_offset = JointActorPosTbl[JOINT_ACTOR_L].x - before.x;
			break;
		}
		arrival_frame = (before_joint_offset * FX32_ONE) / AIR_MOVE_SPEED;
	}
	return arrival_frame;
}

//--------------------------------------------------------------
/**
 * @brief   ��C�A�N�^�[�X�V����
 *
 * @param   game		
 */
//--------------------------------------------------------------
void Air_Update(BALLOON_GAME_PTR game)
{
	int air_no;
	PLAYER_AIR_PARAM *air_param;
	
	air_param = game->air_param;
	for(air_no = 0; air_no < PLAYER_AIR_PARAM_MAX; air_no++){
		if(air_param->occ == TRUE){
			if(air_param->wait > 0){
				air_param->wait--;
				if(air_param->wait == 0){
					GFL_CLACT_WK_SetDrawEnable(air_param->cap, TRUE);
				}
			}
			else if(Air_Move(game, air_param) == TRUE){
				if(game->balloon_occ == TRUE //&& game->balloon_no == air_param->balloon_no
						&& game->game_finish == FALSE){
					//���D�ԍ�����v���Ă���Ȃ�΋�C����
					BalloonTool_AirStackAdd(game, 
						&game->bst, air_param->air, air_param->net_id, air_param->booster_type);
				}
				GFL_CLACT_WK_Remove(air_param->cap);
				air_param->occ = FALSE;
			}
		}
		air_param++;
	}
}

//--------------------------------------------------------------
/**
 * @brief   ��C�A�N�^�[�ړ�
 *
 * @param   air_param		��C�p�����[�^
 *
 * @retval  TRUE:�ڕW�n�_�ɓ��B
 */
//--------------------------------------------------------------
static BOOL Air_Move(BALLOON_GAME_PTR game, PLAYER_AIR_PARAM *air_param)
{
	int del_flag = FALSE;
	s32 joint_offset, before_joint_offset;	//+�̓W���C���g��O�A-�̓W���C���g�ʉߌ�
	fx32 check_scale_xy;
	GFL_CLACTPOS before, pos, move_pos;

	GFL_CLACT_WK_GetPos(air_param->cap, &before, CLSYS_DEFREND_SUB);
	
	switch(air_param->apd->move_dir){
	case DIR_DOWN:
		before_joint_offset = JointActorPosTbl[JOINT_ACTOR_U].y - before.y;
		
		air_param->y += AIR_MOVE_SPEED;
		move_pos.x = air_param->x / FX32_ONE;
		move_pos.y = air_param->y / FX32_ONE;
		GFL_CLACT_WK_SetPos(air_param->cap, &move_pos, CLSYS_DEFREND_SUB);
		GFL_CLACT_WK_GetPos(air_param->cap, &pos, CLSYS_DEFREND_SUB);
		joint_offset = JointActorPosTbl[JOINT_ACTOR_U].y - pos.y;
		if(pos.y > AIR_MOVE_DOWN_DELETE_Y){
			del_flag = TRUE;
		}
		break;
	case DIR_LEFT:
		before_joint_offset = before.x - JointActorPosTbl[JOINT_ACTOR_R].x;

		air_param->x += -AIR_MOVE_SPEED;
		move_pos.x = air_param->x / FX32_ONE;
		move_pos.y = air_param->y / FX32_ONE;
		GFL_CLACT_WK_SetPos(air_param->cap, &move_pos, CLSYS_DEFREND_SUB);
		GFL_CLACT_WK_GetPos(air_param->cap, &pos, CLSYS_DEFREND_SUB);
		joint_offset = pos.x - JointActorPosTbl[JOINT_ACTOR_R].x;
		if(pos.x < AIR_MOVE_LEFT_DELETE_X){
			del_flag = TRUE;
		}
		break;
	case DIR_UP:
		before_joint_offset = before.y - JointActorPosTbl[JOINT_ACTOR_D].y;

		air_param->y += -AIR_MOVE_SPEED;
		move_pos.x = air_param->x / FX32_ONE;
		move_pos.y = air_param->y / FX32_ONE;
		GFL_CLACT_WK_SetPos(air_param->cap, &move_pos, CLSYS_DEFREND_SUB);
		GFL_CLACT_WK_GetPos(air_param->cap, &pos, CLSYS_DEFREND_SUB);
		joint_offset = pos.y - JointActorPosTbl[JOINT_ACTOR_D].y;
		if(pos.y < AIR_MOVE_UP_DELETE_Y){
			del_flag = TRUE;
		}
		break;
	case DIR_RIGHT:
		before_joint_offset = JointActorPosTbl[JOINT_ACTOR_L].x - before.x;

		air_param->x += AIR_MOVE_SPEED;
		move_pos.x = air_param->x / FX32_ONE;
		move_pos.y = air_param->y / FX32_ONE;
		GFL_CLACT_WK_SetPos(air_param->cap, &move_pos, CLSYS_DEFREND_SUB);
		GFL_CLACT_WK_GetPos(air_param->cap, &pos, CLSYS_DEFREND_SUB);
		joint_offset = JointActorPosTbl[JOINT_ACTOR_L].x - pos.x;
		if(pos.x > AIR_MOVE_RIGHT_DELETE_X){
			del_flag = TRUE;
		}
		break;
	}
	
	if(joint_offset >= 0){	//�W���C���g��O
		if(joint_offset < AIR_REDUCE_DOT_OFFSET){
			check_scale_xy = FX32_ONE * joint_offset / AIR_REDUCE_DOT_OFFSET;
			if(check_scale_xy < 0x400){
				check_scale_xy = 0x400;
			}
			GFL_CLACT_WK_SetTypeScale(air_param->cap, check_scale_xy, CLSYS_MAT_X);
			GFL_CLACT_WK_SetTypeScale(air_param->cap, check_scale_xy, CLSYS_MAT_Y);
		}
	}
	else{	//�W���C���g�ʉߌ�@�u�[�X�^�[�ł̑傫�������Ɋg�k����ݒ肷��
		if(joint_offset > -AIR_REDUCE_DOT_OFFSET){
			check_scale_xy = Air_BoosterScale[air_param->booster_type] 
				* (-joint_offset) / AIR_REDUCE_DOT_OFFSET;
			if(check_scale_xy < 0x400){
				check_scale_xy = 0x400;
			}
			GFL_CLACT_WK_SetTypeScale(air_param->cap, check_scale_xy, CLSYS_MAT_X);
			GFL_CLACT_WK_SetTypeScale(air_param->cap, check_scale_xy, CLSYS_MAT_Y);
		}
	}
	
	//�W���C���g�̃Z���^�[��ʉ߂������̃u�[�X�^�[�����`�F�b�N
	if(air_param->net_id == GFL_NET_SystemGetCurrentID() && before_joint_offset >= 0 && joint_offset <= 0){
		BOOSTER_MOVE *hit_move;
		air_param->booster_type = Booster_HitCheckNow(game, &hit_move);
		switch(air_param->booster_type){
		case BOOSTER_TYPE_NORMAL:
			air_param->air *= 2;
			break;
		case BOOSTER_TYPE_WEAK:
			air_param->air /= 2;
			break;
		case BOOSTER_TYPE_HARD:
			air_param->air *= 3;
			break;
		}
		BalloonTool_SendAirBoosterSet(game, air_param->booster_type);
		if(air_param->booster_type != BOOSTER_TYPE_NONE){
			BoosterMoveModeChange(hit_move, BOOSTER_MODE_HIT);
		}

		//�u�[�X�^�[�ʉ߃^�C�~���O�ŃX�^�b�N����Ă����C�f�[�^�𑗐M�o�b�t�@�֑��肱��
		{
			BALLOON_AIR_DATA *air_data;
			
			air_data = BalloonTool_SendAirPop(game);
			if(air_data != NULL){
				SendBalloon_Air(game, air_data);
			}
			else{
				OS_TPrintf("Air�X�^�b�N������ꂽ��C���u�[�X�^�[�ʉ�\n");
			}
		}
	}
	
	if(del_flag == TRUE){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   ��C�A�N�^�[����
 *
 * @param   game			
 * @param   air_posdata		��C�f�[�^
 *
 * @retval  ���������A�N�^�[�ւ̃|�C���^
 */
//--------------------------------------------------------------
static GFL_CLWK* Air_ActorCreate(BALLOON_GAME_PTR game, const AIR_POSITION_DATA *air_posdata)
{
	GFL_CLWK* cap;
	GFL_CLWK_AFFINEDATA act_head;
	
	//-- �A�N�^�[���� --//
	act_head = AirObjParam;
	act_head.clwkdata.pos_x = air_posdata->x;
	act_head.clwkdata.pos_y = air_posdata->y;
	act_head.clwkdata.anmseq = air_posdata->anmseq;
	cap = GFL_CLACT_WK_CreateAffine( game->clunit, game->cgr_id[CHARID_SUB_BALLOON_MIX], 
		game->pltt_id[PLTTID_SUB_OBJ_COMMON], 
		game->cell_id[CELLID_SUB_BALLOON_MIX], &act_head, CLSYS_DEFREND_SUB, HEAPID_BALLOON);
	GFL_CLACT_WK_SetPlttOffs(cap, air_posdata->pal_ofs);
	
	GFL_CLACT_WK_AddAnmFrame(cap, FX32_ONE);
	return cap;
}

//--------------------------------------------------------------
/**
 * @brief   ���D�A�N�^�[��S�č폜
 *
 * @param   game		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
void Air_ActorAllDelete(BALLOON_GAME_PTR game)
{
	int i;
	
	for(i = 0; i < PLAYER_AIR_PARAM_MAX; i++){
		if(game->air_param[i].occ == TRUE){
			GFL_CLACT_WK_Remove(game->air_param[i].cap);
			game->air_param[i].occ = FALSE;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   �j��A�N�^�[����
 *
 * @param   game		
 *
 * @retval  ���������j��A�N�^�[�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Exploded_ActorCreate(BALLOON_GAME_PTR game, EXPLODED_PARAM *exploded, BALLOON_STATUS *bst)
{
	GFL_CLWK_DATA act_head;
	GFL_CLWK* cap;
	int player_pos;
	int act_num[WFLBY_MINIGAME_MAX];
	int total_air, total_act, anmseq;
	int act_index = 0;
	int i, s;
	
	act_head = ExplodedStormObjParam;
	total_air = bst->air + bst->air_stack;	//�X�^�b�N�̎c������݂Ŋ������o��
	for(i = 0; i < game->bsw->player_max; i++){
		player_pos = Balloon_NetID_to_PlayerPos(game, game->bsw->player_netid[i]);
		act_num[i] = bst->player_air[player_pos] * EXPLODED_STORM_ACTOR_MAX / total_air;
		anmseq = PlayerNoAnmSeq_Storm[game->bsw->player_max][player_pos];
		OS_TPrintf("���ӂԂ��@player_pos = %d, num = %d, anmseq = %d\n", player_pos, act_num[i], anmseq);
		for(s = 0; s < act_num[i]; s++){
			STORM_WORK *storm = &exploded->storm[act_index];
			
			cap = GFL_CLACT_WK_Create(game->clunit, game->cgr_id[CHARID_SUB_BALLOON_MIX], 
				game->pltt_id[PLTTID_SUB_OBJ_COMMON], 
				game->cell_id[CELLID_SUB_BALLOON_MIX],
				&act_head, CLSYS_DEFREND_SUB, HEAPID_BALLOON);
			if(cap == NULL){
				break;
			}
			GFL_CLACT_WK_SetAnmSeq( cap, anmseq );
			GFL_CLACT_WK_SetPlttOffs(cap, PALOFS_SUB_EXPLODED);
			GFL_CLACT_WK_AddAnmFrame(cap, FX32_ONE);
			
			storm->furihaba = GFUser_GetPublicRand(0x2000) + 0x2000;
			storm->add_furihaba = GFUser_GetPublicRand(0x4000) + 0x2000;
			storm->sec = GFUser_GetPublicRand(360) << FX32_SHIFT;
			storm->add_sec = GFUser_GetPublicRand(0x14000) + 0xa000;
			storm->delete_wait = GFUser_GetPublicRand(15) + 20;
			exploded->storm[act_index].cap = cap;
			act_index++;
		}
	}
	
	//-- ���� --//
	act_head.softpri = BALLOONSUB_SOFTPRI_EXPLODED_SMOKE;
	for(i = 0; i < EXPLODED_SMOKE_ACTOR_MAX; i++){
		STORM_WORK *smoke = &exploded->smoke[i];
		
		cap = GFL_CLACT_WK_Create(game->clunit, game->cgr_id[CHARID_SUB_BALLOON_MIX], 
			game->pltt_id[PLTTID_SUB_OBJ_COMMON], 
			game->cell_id[CELLID_SUB_BALLOON_MIX], &act_head, CLSYS_DEFREND_SUB, HEAPID_BALLOON);
		if(cap == NULL){
			break;
		}
		GFL_CLACT_WK_SetAnmSeq( cap, MIXOBJ_ANMSEQ_EXPLODED_SMOKE_1 + GFUser_GetPublicRand(3) );
		GFL_CLACT_WK_SetPlttOffs(cap, PALOFS_SUB_EXPLODED_SMOKE);
		GFL_CLACT_WK_AddAnmFrame(cap, FX32_ONE);
		
		smoke->furihaba = GFUser_GetPublicRand(0x2000) + 0x2000;
		smoke->add_furihaba = GFUser_GetPublicRand(0x4000) + 0x2000;
		smoke->sec = GFUser_GetPublicRand(360) << FX32_SHIFT;
		smoke->add_sec = GFUser_GetPublicRand(0x14000) + 0x10000;
		smoke->delete_wait = GFUser_GetPublicRand(15) + 20;
		exploded->smoke[i].cap = cap;
	}
	
	//-- �j�� --//
	act_head.softpri = BALLOONSUB_SOFTPRI_EXPLODED_CHIP;
	for(i = 0; i < EXPLODED_CHIP_ACTOR_MAX; i++){
		STORM_WORK *chip = &exploded->chip[i];
		
		cap = GFL_CLACT_WK_Create(game->clunit, game->cgr_id[CHARID_SUB_BALLOON_MIX], 
			game->pltt_id[PLTTID_SUB_OBJ_COMMON], 
			game->cell_id[CELLID_SUB_BALLOON_MIX], &act_head, CLSYS_DEFREND_SUB, HEAPID_BALLOON);
		if(cap == NULL){
			break;
		}
		GFL_CLACT_WK_SetAnmSeq( cap, MIXOBJ_ANMSEQ_BALLOON_EXPLODED);
		GFL_CLACT_WK_SetPlttOffs(cap, ExplodedChipPalNo[bst->level]);
		GFL_CLACT_WK_AddAnmFrame(cap, FX32_ONE);
		
		chip->furihaba = GFUser_GetPublicRand(0x2000) + 0x2000;
		chip->add_furihaba = GFUser_GetPublicRand(0x4000) + 0x2000;
		chip->sec = GFUser_GetPublicRand(360) << FX32_SHIFT;
		chip->add_sec = GFUser_GetPublicRand(0x14000) + 0x10000;
		chip->delete_wait = GFUser_GetPublicRand(15) + 20;
		exploded->chip[i].cap = cap;
	}
}

//--------------------------------------------------------------
/**
 * @brief   ���D�j��A�N�^�[���폜����
 *
 * @param   game		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
void Exploded_AllDelete(BALLOON_GAME_PTR game)
{
	int i;
	STORM_WORK *storm, *smoke, *chip;
	
	for(i = 0; i < EXPLODED_STORM_ACTOR_MAX; i++){
		storm = &game->exploded_param.storm[i];
		if(storm->cap != NULL){
			GFL_CLACT_WK_Remove(storm->cap);
		}
	}

	for(i = 0; i < EXPLODED_SMOKE_ACTOR_MAX; i++){
		smoke = &game->exploded_param.smoke[i];
		if(smoke->cap != NULL){
			GFL_CLACT_WK_Remove(smoke->cap);
		}
	}

	for(i = 0; i < EXPLODED_CHIP_ACTOR_MAX; i++){
		chip = &game->exploded_param.chip[i];
		if(chip->cap != NULL){
			GFL_CLACT_WK_Remove(chip->cap);
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   ���D�j��A�N�^�[�X�V����
 *
 * @param   game		
 *
 * @retval  TRUE:�A�N�^�[���쒆�B�@FALSE:�������얳��
 */
//--------------------------------------------------------------
BOOL Exploded_Update(BALLOON_GAME_PTR game)
{
	EXPLODED_PARAM *exploded = &game->exploded_param;
	STORM_WORK *storm, *smoke, *chip;
	int eff_count = 0;
	int i;
	GFL_CLACTPOS pos;
	
	if(exploded->occ == FALSE){
		return FALSE;
	}
	
	//-- ���ӂԂ� --//
	for(i = 0; i < EXPLODED_STORM_ACTOR_MAX; i++){
		storm = &exploded->storm[i];
		if(storm->cap != NULL){
			if(storm->delete_wait == 0){
				GFL_CLACT_WK_Remove(storm->cap);
				storm->cap = NULL;
				continue;
			}
			storm->sec += storm->add_sec;
			storm->furihaba += storm->add_furihaba;
			if(storm->furihaba >= (128+32) << FX32_SHIFT){
				storm->furihaba = (128+32) << FX32_SHIFT;
			}
			pos.x = BALLOON_APPEAR_POS_X + (FX_Mul(Sin360FX(storm->sec), storm->furihaba)) / FX32_ONE;
			pos.y = BALLOON_APPEAR_POS_Y + (-FX_Mul(Cos360FX(storm->sec), storm->furihaba)) / FX32_ONE;
			GFL_CLACT_WK_SetPos(storm->cap, &pos, CLSYS_DEFREND_SUB);
			storm->delete_wait--;
			eff_count++;
		}
	}

	//-- �� --//
	for(i = 0; i < EXPLODED_SMOKE_ACTOR_MAX; i++){
		smoke = &exploded->smoke[i];
		if(smoke->cap != NULL){
			if(smoke->delete_wait == 0){
				GFL_CLACT_WK_Remove(smoke->cap);
				smoke->cap = NULL;
				continue;
			}
			smoke->sec += smoke->add_sec;
			smoke->furihaba += smoke->add_furihaba;
			if(smoke->furihaba >= (128+32) << FX32_SHIFT){
				smoke->furihaba = (128+32) << FX32_SHIFT;
			}
			pos.x = BALLOON_APPEAR_POS_X + (FX_Mul(Sin360FX(smoke->sec), smoke->furihaba)) / FX32_ONE;
			pos.y = BALLOON_APPEAR_POS_Y + (-FX_Mul(Cos360FX(smoke->sec), smoke->furihaba)) / FX32_ONE;
			GFL_CLACT_WK_SetPos(smoke->cap, &pos, CLSYS_DEFREND_SUB);
			smoke->delete_wait--;
			eff_count++;
		}
	}
	
	//-- �j�� --//
	for(i = 0; i < EXPLODED_CHIP_ACTOR_MAX; i++){
		chip = &exploded->chip[i];
		if(chip->cap != NULL){
			if(chip->delete_wait == 0){
				GFL_CLACT_WK_Remove(chip->cap);
				chip->cap = NULL;
				continue;
			}
			chip->sec += chip->add_sec;
			chip->furihaba += chip->add_furihaba;
			if(chip->furihaba >= (128+32) << FX32_SHIFT){
				chip->furihaba = (128+32) << FX32_SHIFT;
			}
			pos.x = BALLOON_APPEAR_POS_X + (FX_Mul(Sin360FX(chip->sec), chip->furihaba)) / FX32_ONE;
			pos.y = BALLOON_APPEAR_POS_Y + (-FX_Mul(Cos360FX(chip->sec), chip->furihaba)) / FX32_ONE;
			GFL_CLACT_WK_SetPos(chip->cap, &pos, CLSYS_DEFREND_SUB);
			chip->delete_wait--;
			eff_count++;
		}
	}
	
	if(eff_count == 0){
		exploded->occ = FALSE;
	}
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �j��p�����[�^�Z�b�g
 *
 * @param   game		
 * @param   exploded		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
BOOL BalloonTool_ExplodedParamAdd(BALLOON_GAME_PTR game)
{
	EXPLODED_PARAM *exploded = &game->exploded_param;
	int i;
	
	if(exploded->occ == TRUE){
		return FALSE;
	}
	
	GFL_STD_MemClear(exploded, sizeof(EXPLODED_PARAM));
	
	//�����A�N�^�[�����@�����̉�ʏ󋵂𔽉f�������bst�͌X�̃��[�J���̂��g�p
	Exploded_ActorCreate(game, &game->exploded_param, &game->bst);
	
	//���DBG����
	BalloonTool_BalloonBGErase(&game->bst);
	
	//���D�A�C�R�������A�j��
	IconBalloon_ExplodedReq(game);
	
	
	//-- bst�N���A --//
	game->balloon_occ = FALSE;
	game->bst.air = 0;
	game->bst.air_stack = 0;
	for(i = 0; i < WFLBY_MINIGAME_MAX; i++){
		game->bst.player_air[i] = 0;
	}

#if WB_TEMP_FIX
	Snd_SePlay(SE_BALLOON_EXPLODED);
#endif
	exploded->occ = TRUE;
	return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief   ���D�A�C�R���A�N�^�[����
 *
 * @param   game		
 *
 * @retval  ���������j��A�N�^�[�ւ̃|�C���^
 */
//--------------------------------------------------------------
static GFL_CLWK* IconBalloon_ActorCreate(BALLOON_GAME_PTR game, int icon_type, int pos)
{
	GFL_CLWK* cap;
	GFL_CLWK_DATA act_head;

	//-- �A�N�^�[���� --//
	act_head = IconBalloonObjParam;
	act_head.pos_x = ICON_BALLOON_POS_RIGHT_X - pos * ICON_BALLOON_POS_OFFSET_X;
	act_head.pos_y = ICON_BALLOON_POS_Y;
	cap = GFL_CLACT_WK_Create(game->clunit, game->cgr_id[CHARID_SUB_BALLOON_MIX], 
		game->pltt_id[PLTTID_SUB_OBJ_COMMON], game->cell_id[CELLID_SUB_BALLOON_MIX], 
		&act_head, CLSYS_DEFREND_SUB, HEAPID_BALLOON);
	GFL_CLACT_WK_SetPlttOffs(cap, PALOFS_SUB_ICON_BALLOON);
	
	GFL_CLACT_WK_SetAnmSeq(cap, MIXOBJ_ANMSEQ_ICON_BALLOON_1 + icon_type);
	
	GFL_CLACT_WK_AddAnmFrame(cap, FX32_ONE);
	GFL_CLACT_WK_SetAutoAnmFlag(cap, TRUE);
	return cap;
}

//--------------------------------------------------------------
/**
 * @brief   ���D�A�C�R����S�č쐬����
 *
 * @param   game		
 */
//--------------------------------------------------------------
void IconBalloon_AllCreate(BALLOON_GAME_PTR game)
{
	int i;
	ICONBALLOON_PARAM *ibp;
	
	for(i = 0; i < ICON_BALLOON_PARAM_MAX; i++){
		ibp = &game->iconballoon_param[i];
		BalloonTool_IconBalloonParamAdd(game, ibp, i, i);
		GFL_CLACT_WK_AddAnmFrame(ibp->cap, FX32_ONE*4 * i);	//�ŏ��̃A�j�����o����������
	}
}

//--------------------------------------------------------------
/**
 * @brief   ���D�A�C�R����S�č폜����
 *
 * @param   game		
 */
//--------------------------------------------------------------
void IconBalloon_AllDelete(BALLOON_GAME_PTR game)
{
	int i;
	ICONBALLOON_PARAM *ibp;
	
	for(i = 0; i < ICON_BALLOON_PARAM_MAX; i++){
		ibp = &game->iconballoon_param[i];
		GFL_CLACT_WK_Remove(ibp->cap);
	}
}

//--------------------------------------------------------------
/**
 * @brief   ���D�j��A�N�^�[�X�V����
 *
 * @param   game		
 *
 * @retval  TRUE:�A�N�^�[���쒆�B�@FALSE:�������얳��
 */
//--------------------------------------------------------------
BOOL IconBalloon_Update(BALLOON_GAME_PTR game)
{
	ICONBALLOON_PARAM *ibp;
	int i;
	
	for(i = 0; i < ICON_BALLOON_PARAM_MAX; i++){
		ibp = &game->iconballoon_param[i];
		IconBalloon_PosMove(ibp);
		IconBalloon_StatusUpdate(game, ibp);
	}

	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���D�A�C�R���p�����[�^�Z�b�g
 *
 * @param   game		
 * @param   ibp				�����
 * @param   pos				�ʒu
 * @param   balloon_no		�S�����镗�D�ԍ�
 *
 * @retval  TRUE:�Z�b�g����
 */
//--------------------------------------------------------------
static BOOL BalloonTool_IconBalloonParamAdd(BALLOON_GAME_PTR game, ICONBALLOON_PARAM *ibp, int pos, int balloon_no)
{
	GFL_STD_MemClear(ibp, sizeof(ICONBALLOON_PARAM));
	
	ibp->type = IconBalloon_TypeGet(balloon_no);
	ibp->status = ICON_BALLOON_STATUS_NORMAL;
	ibp->cap = IconBalloon_ActorCreate(game, ibp->type, pos);
	ibp->pos = pos;
	ibp->balloon_no = balloon_no;
	
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���D�A�C�R���̍��W�X�V����
 *
 * @param   ibp		���D�A�C�R���p�����[�^�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void IconBalloon_PosMove(ICONBALLOON_PARAM *ibp)
{
	s16 x;
	GFL_CLACTPOS now;
	
	x = ICON_BALLOON_POS_RIGHT_X - ibp->pos * ICON_BALLOON_POS_OFFSET_X;
	GFL_CLACT_WK_GetPos(ibp->cap, &now, CLSYS_DEFREND_SUB);
	
	if(x > now.x){
		now.x += ICON_BALLOON_MOVE_SP_X;
		if(now.x > x){
			now.x = x;
		}
		GFL_CLACT_WK_SetPos(ibp->cap, &now, CLSYS_DEFREND_SUB);
	}
}

//--------------------------------------------------------------
/**
 * @brief   ���D�A�C�R���̃X�e�[�^�X�X�V
 *
 * @param   game		
 * @param   ibp		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void IconBalloon_StatusUpdate(BALLOON_GAME_PTR game, ICONBALLOON_PARAM *ibp)
{
	int parcent, i;
	ICONBALLOON_PARAM *get_ibp;
	GFL_CLACTPOS pos;
	
	if(ibp->pos != 0){
		return;	//��ԉE�[�ȊO�͊֌W�Ȃ�
	}
	
	switch(ibp->status){
	//�����A�j���I���`�F�b�N
	case ICON_BALLOON_STATUS_EXPLODED:
		if(GFL_CLACT_WK_CheckAnmActive(ibp->cap) == FALSE){
			//�����������D�A�C�R����������V�����o�ꂷ��A�C�R���Ƃ��čė��p����
			ibp->balloon_no += ICON_BALLOON_PARAM_MAX;	//���D�ԍ��X�V
			ibp->type = IconBalloon_TypeGet(ibp->balloon_no);
			ibp->status = ICON_BALLOON_STATUS_NORMAL;
			GFL_CLACT_WK_SetAnmSeq(ibp->cap, MIXOBJ_ANMSEQ_ICON_BALLOON_1 + ibp->type);
			pos.x = ICON_BALLOON_POS_RIGHT_X - ICON_BALLOON_PARAM_MAX * ICON_BALLOON_POS_OFFSET_X;
			pos.y = ICON_BALLOON_POS_Y;
			GFL_CLACT_WK_SetPos(ibp->cap, &pos, CLSYS_DEFREND_SUB);
			//�S�Ă̕��D�A�C�R����pos���l�߂�
			for(i = 0; i < ICON_BALLOON_PARAM_MAX; i++){
				get_ibp = &game->iconballoon_param[i];
				if(get_ibp->pos == 0){
					get_ibp->pos = ICON_BALLOON_PARAM_MAX - 1;
				}
				else{
					get_ibp->pos--;
				}
			}
		}
		break;
	//�s���`��ԃ`�F�b�N
	case ICON_BALLOON_STATUS_NORMAL:
		parcent = BalloonTool_AirParcentGet(game);
		if(ibp->status == ICON_BALLOON_STATUS_NORMAL && parcent > ICON_BALLOON_PINCH_PARCENT){
			ibp->status = ICON_BALLOON_STATUS_PINCH;
			GFL_CLACT_WK_SetAnmSeq(ibp->cap, MIXOBJ_ANMSEQ_ICON_BALLOON_1_PINCH + ibp->type);
		#if WB_TEMP_FIX
			Snd_SePlay(SE_BALLOON_PINCH);
		#endif
		}
		break;
	}
}

//--------------------------------------------------------------
/**
 * @brief   ���D�ԍ����畗�D�̃��x���^�C�v���擾����
 * @param   balloon_no		���D�ԍ�
 * @retval  ���x���^�C�v
 */
//--------------------------------------------------------------
static int IconBalloon_TypeGet(int balloon_no)
{
	if(balloon_no >= BALLOON_LEVEL3_NUM){
		return BALLOON_LEVEL_3;
	}
	if(balloon_no >= BALLOON_LEVEL2_NUM){
		return BALLOON_LEVEL_2;
	}
	return BALLOON_LEVEL_1;
}

//--------------------------------------------------------------
/**
 * @brief   ���D�A�C�R���ɔ����A�j�����N�G�X�g��ݒ肷��
 *
 * @param   game		
 */
//--------------------------------------------------------------
static void IconBalloon_ExplodedReq(BALLOON_GAME_PTR game)
{
	int i;
	ICONBALLOON_PARAM *ibp;
	
	for(i = 0; i < ICON_BALLOON_PARAM_MAX; i++){
		ibp = &game->iconballoon_param[i];
		if(ibp->pos == 0){
			ibp->status = ICON_BALLOON_STATUS_EXPLODED;
			GFL_CLACT_WK_SetAnmSeq(ibp->cap, MIXOBJ_ANMSEQ_ICON_BALLOON_EXPLODED);
			return;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   �Q���l���ɉ����Ė��O�E�B���h�E�̃p���b�g�ԍ�������������
 *
 * @param   game		
 */
//--------------------------------------------------------------
void BalloonTool_NameWindowPalNoSwap(BALLOON_GAME_PTR game)
{
	int i;
	
	switch(game->bsw->player_max){
	case 3:	//��肪����̂�3�l�̎������B(2�l�A4�l�͌��f�[�^�̂܂܂�OK)
		GFL_BG_ChangeScreenPalette(BALLOON_SUBFRAME_WIN, 0, 13, 12, 4, NAME_WIN_PALNO_YELLOW);
		GFL_BG_ChangeScreenPalette(BALLOON_SUBFRAME_WIN, 0x14, 13, 12, 4, NAME_WIN_PALNO_BLUE);
		break;
	}
}

//--------------------------------------------------------------
/**
 * @brief   �v���C���[�ʒu�ɏ]���ăp�C�v�̃J���[�̃X���b�v���s��
 *
 * @param   game		
 */
//--------------------------------------------------------------
void BalloonTool_PaletteSwap_Pipe(BALLOON_GAME_PTR game)
{
	u16 color[WFLBY_MINIGAME_MAX][PLAYER_NO_PALETTE_COLOR_NUM_PIPE];
	int i, player;
	int my_player = 0;
	int read_pos, write_pos, k, current_id;
	u16 *plttbuf, *transbuf;
	
	//�e�v���C���[�̃J���[�f�[�^���e���|�����o�b�t�@�փR�s�[
	for(player = 0; player < game->bsw->player_max; player++){
		read_pos = PlayerNoPaletteWritePosTbl_Pipe[player];
		for(i = 0; i < PLAYER_NO_PALETTE_COLOR_NUM_PIPE; i++){
			color[player][i] = PaletteWork_ColorGet(game->pfd, FADE_SUB_BG, FADEBUF_SRC, 
				read_pos + i);
		}
	}
	
	//�����̃v���C���[�ʒu���擾
	current_id = GFL_NET_SystemGetCurrentID();
	for(my_player = 0; my_player < game->bsw->player_max; my_player++){
		if(game->bsw->player_netid[my_player] == current_id){
			break;
		}
	}
	
	//�����̃v���C���[�ʒu����ɃJ���[���Z�b�g���Ȃ���
	plttbuf = PaletteWorkDefaultWorkGet(game->pfd, FADE_SUB_BG);
	transbuf = PaletteWorkTransWorkGet(game->pfd, FADE_SUB_BG);
	for(i = 0; i < game->bsw->player_max; i++){
		read_pos = i;
		write_pos = PlayerNoPaletteSwapTbl_Pipe[game->bsw->player_max][Balloon_NetID_to_PlayerPos(game, game->bsw->player_netid[i])];
		for(k = 0; k < PLAYER_NO_PALETTE_COLOR_NUM_PIPE; k++){
			plttbuf[write_pos + k] = color[read_pos][k];
			transbuf[write_pos + k] = color[read_pos][k];
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   �v���C���[�ʒu�ɏ]���ăv���C���[����OBJ(�u�[�X�^�[���C)�̃J���[�̃X���b�v���s��
 *
 * @param   game		
 */
//--------------------------------------------------------------
void BalloonTool_PaletteSwap_PlayerOBJ(BALLOON_GAME_PTR game)
{
	u16 color[WFLBY_MINIGAME_MAX][PLAYER_NO_PALETTE_COLOR_NUM_AIR];
	int i, player;
	int my_player = 0;
	int read_pos, write_pos, k, current_id;
	u16 *plttbuf, *transbuf;
	int common_palno;
	
	common_palno = GFL_CLGRP_PLTT_GetAddr(game->pltt_id[PLTTID_SUB_OBJ_COMMON], CLSYS_DRAW_SUB) / 0x20;
	
	//�e�v���C���[�̃J���[�f�[�^���e���|�����o�b�t�@�փR�s�[
	for(player = 0; player < game->bsw->player_max; player++){
		read_pos = PlayerNoPaletteWritePosTbl_Obj[player];
		for(i = 0; i < PLAYER_NO_PALETTE_COLOR_NUM_AIR; i++){
			color[player][i] = PaletteWork_ColorGet(game->pfd, FADE_SUB_OBJ, FADEBUF_SRC, 
				common_palno*16 + read_pos + i);
		}
	}
	
	//�����̃v���C���[�ʒu���擾
	current_id = GFL_NET_SystemGetCurrentID();
	for(my_player = 0; my_player < game->bsw->player_max; my_player++){
		if(game->bsw->player_netid[my_player] == current_id){
			break;
		}
	}
	
	//�����̃v���C���[�ʒu����ɃJ���[���Z�b�g���Ȃ���
	plttbuf = PaletteWorkDefaultWorkGet(game->pfd, FADE_SUB_OBJ);
	transbuf = PaletteWorkTransWorkGet(game->pfd, FADE_SUB_OBJ);
	for(i = 0; i < game->bsw->player_max; i++){
		read_pos = i;
		write_pos = PlayerNoPaletteSwapTbl_Obj[game->bsw->player_max][Balloon_NetID_to_PlayerPos(game, game->bsw->player_netid[i])] + common_palno*16;
		for(k = 0; k < PLAYER_NO_PALETTE_COLOR_NUM_AIR; k++){
			plttbuf[write_pos + k] = color[read_pos][k];
			transbuf[write_pos + k] = color[read_pos][k];
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   �v���C���[�ʒu�ɏ]���Ď��ӂԂ��̃J���[�̃X���b�v���s��
 *
 * @param   game		
 */
//--------------------------------------------------------------
void BalloonTool_PaletteSwap_Storm(BALLOON_GAME_PTR game)
{
	u16 color[WFLBY_MINIGAME_MAX][PLAYER_NO_PALETTE_COLOR_NUM_STORM];
	int i, player;
	int my_player = 0;
	int read_pos, write_pos, k, current_id;
	u16 *plttbuf, *transbuf;
	int common_palno;
	
	common_palno = GFL_CLGRP_PLTT_GetAddr(game->pltt_id[PLTTID_SUB_OBJ_COMMON], CLSYS_DRAW_SUB) / 0x20;
	
	//�e�v���C���[�̃J���[�f�[�^���e���|�����o�b�t�@�փR�s�[
	for(player = 0; player < game->bsw->player_max; player++){
		read_pos = PlayerNoPaletteWritePosTbl_Storm[player];
		for(i = 0; i < PLAYER_NO_PALETTE_COLOR_NUM_STORM; i++){
			color[player][i] = PaletteWork_ColorGet(game->pfd, FADE_SUB_OBJ, FADEBUF_SRC, 
				common_palno*16 + read_pos + i);
		}
	}
	
	//�����̃v���C���[�ʒu���擾
	current_id = GFL_NET_SystemGetCurrentID();
	for(my_player = 0; my_player < game->bsw->player_max; my_player++){
		if(game->bsw->player_netid[my_player] == current_id){
			break;
		}
	}
	
	//�����̃v���C���[�ʒu����ɃJ���[���Z�b�g���Ȃ���
	plttbuf = PaletteWorkDefaultWorkGet(game->pfd, FADE_SUB_OBJ);
	transbuf = PaletteWorkTransWorkGet(game->pfd, FADE_SUB_OBJ);
	for(i = 0; i < game->bsw->player_max; i++){
		read_pos = i;
		write_pos = PlayerNoPaletteSwapTbl_Storm[game->bsw->player_max][Balloon_NetID_to_PlayerPos(game, game->bsw->player_netid[i])] + common_palno*16;
		for(k = 0; k < PLAYER_NO_PALETTE_COLOR_NUM_STORM; k++){
			plttbuf[write_pos + k] = color[read_pos][k];
			transbuf[write_pos + k] = color[read_pos][k];
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   �G�A�[�f�[�^���쐬����
 *
 * @param   game			
 * @param   balloon_no		���D�ԍ�
 * @param   air				��C��(�␳��u�[�X�^�[�O�̏����ȋ�C��)
 * @param   air_data		�Z�b�g��
 */
//--------------------------------------------------------------
void BalloonTool_AirDataCreate(BALLOON_GAME_PTR game, int balloon_no, s32 air, BALLOON_AIR_DATA *air_data)
{
	GFL_STD_MemClear(air_data, sizeof(BALLOON_AIR_DATA));
	air_data->net_id = GFL_NET_SystemGetCurrentID();
	air_data->no = balloon_no;
	air_data->air = air;
	air_data->last_air = air;
	air_data->booster_type = BOOSTER_TYPE_NONE;
	air_data->booster_occ = 0;
}

//--------------------------------------------------------------
/**
 * @brief   ��C�f�[�^�𑗐M�o�b�t�@�ɃX�^�b�N����
 *
 * @param   game		
 * @param   air_data	��C�f�[�^
 *
 * @retval  TRUE:���M�o�b�t�@�ɃX�^�b�N���ꂽ
 * @retval  FALSE:���M�o�b�t�@�������ς��������׎󂯎��Ȃ�����
 *
 * FALSE���Ԃ��Ă��Ă���{�A���o�f�[�^�Ȃ̂Ŗ������Ă��܂��Ă悢
 */
//--------------------------------------------------------------
BOOL BalloonTool_SendAirPush(BALLOON_GAME_PTR game, BALLOON_AIR_DATA *air_data)
{
	if(game->send_airdata_write_no - game->send_airdata_read_no >= PIPE_AIR_AIR_MAX){
		return FALSE;
	}
	game->send_airdata_stack[game->send_airdata_write_no % PIPE_AIR_AIR_MAX] = *air_data;
	game->send_airdata_write_no++;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ��C�f�[�^�𑗐M�o�b�t�@������o��
 *
 * @param   game		
 *
 * @retval  ��C�f�[�^�ւ̃|�C���^(�f�[�^�������ꍇ��NULL)
 */
//--------------------------------------------------------------
BALLOON_AIR_DATA * BalloonTool_SendAirPop(BALLOON_GAME_PTR game)
{
	BALLOON_AIR_DATA *air_data;
	
	if(game->send_airdata_read_no >= game->send_airdata_write_no){
		return NULL;
	}
	air_data = &game->send_airdata_stack[game->send_airdata_read_no % PIPE_AIR_AIR_MAX];
	game->send_airdata_read_no++;
	return air_data;
}

//--------------------------------------------------------------
/**
 * @brief   �擪�ɃX�^�b�N����Ă����C�f�[�^�Ƀu�[�X�^�[�^�C�v��ݒ肷��
 *
 * @param   game		
 *
 * @retval  TRUE:�Z�b�g����
 * @retval  FALSE:�Z�b�g�o���Ȃ�����(�X�^�b�N����Ă����C����������)
 */
//--------------------------------------------------------------
BOOL BalloonTool_SendAirBoosterSet(BALLOON_GAME_PTR game, int booster_type)
{
	BALLOON_AIR_DATA *air_data = NULL;
	int i, div_read, div_write;
	
	if(game->send_airdata_read_no == game->send_airdata_write_no
			|| booster_type == BOOSTER_TYPE_NONE){
		return FALSE;
	}
	
	div_read = game->send_airdata_read_no % PIPE_AIR_AIR_MAX;
	div_write = game->send_airdata_write_no % PIPE_AIR_AIR_MAX;
	if(div_read < div_write){
		for(i = div_read; i < div_write; i++){
			if(game->send_airdata_stack[i].booster_occ == 0){
				air_data = &game->send_airdata_stack[i];
				break;
			}
		}
	}
	else{
		for(i = div_read; i < PIPE_AIR_AIR_MAX; i++){
			if(game->send_airdata_stack[i].booster_occ == 0){
				air_data = &game->send_airdata_stack[i];
				break;
			}
		}
		if(air_data == NULL){
			for(i = 0; i < div_write; i++){
				if(game->send_airdata_stack[i].booster_occ == 0){
					air_data = &game->send_airdata_stack[i];
					break;
				}
			}
		}
	}
	if(air_data == NULL){
		GF_ASSERT(0);		//�u�[�X�^�[���ݒ�̃G�A�[�f�[�^��1������
		return FALSE;
	}
	
	GF_ASSERT(air_data->booster_type == 0);
	air_data->booster_type = booster_type;
	air_data->booster_occ = TRUE;
	switch(booster_type){
	case BOOSTER_TYPE_NORMAL:
		air_data->last_air = air_data->air * 2;
		break;
	case BOOSTER_TYPE_WEAK:
		air_data->last_air = air_data->air / 2;
		break;
	case BOOSTER_TYPE_HARD:
		air_data->last_air = air_data->air * 3;
		break;
	}
	
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �W���C���g�A�N�^�[����
 *
 * @param   game		
 */
//--------------------------------------------------------------
void Joint_ActorCreateAll(BALLOON_GAME_PTR game, JOINT_WORK *joint)
{
	GFL_CLWK* cap;
	GFL_CLWK_DATA act_head;
	int i;

	act_head = JointObjParam;
	for(i = 0; i < JOINT_ACTOR_MAX; i++){
		act_head.pos_x = JointActorPosTbl[i].x;
		act_head.pos_y = JointActorPosTbl[i].y;
		joint->cap[i] = GFL_CLACT_WK_Create(game->clunit, 
			game->cgr_id[CHARID_SUB_BALLOON_MIX], game->pltt_id[PLTTID_SUB_OBJ_COMMON], 
			game->cell_id[CELLID_SUB_BALLOON_MIX], &act_head, CLSYS_DEFREND_SUB, HEAPID_BALLOON);
		GFL_CLACT_WK_SetPlttOffs(joint->cap[i], PALOFS_SUB_JOINT);
		GFL_CLACT_WK_SetAnmSeq(joint->cap[i], MIXOBJ_ANMSEQ_JOINT_UD + i/2);
		GFL_CLACT_WK_AddAnmFrame(joint->cap[i], FX32_ONE);
	}
	
	switch(game->bsw->player_max){
	case 2:
		GFL_CLACT_WK_SetDrawEnable(joint->cap[JOINT_ACTOR_L], FALSE);
		GFL_CLACT_WK_SetDrawEnable(joint->cap[JOINT_ACTOR_R], FALSE);
		break;
	case 3:
		GFL_CLACT_WK_SetDrawEnable(joint->cap[JOINT_ACTOR_U], FALSE);
		break;
	}
}

//--------------------------------------------------------------
/**
 * @brief   �W���C���g�A�N�^�[��S�č폜����
 *
 * @param   game		
 */
//--------------------------------------------------------------
void Joint_ActorDeleteAll(BALLOON_GAME_PTR game, JOINT_WORK *joint)
{
	int i;
	
	for(i = 0; i < JOINT_ACTOR_MAX; i++){
		GFL_CLACT_WK_Remove(joint->cap[i]);
	}
}

//--------------------------------------------------------------
/**
 * @brief   �u�[�X�^�[�A�N�^�[����
 *
 * @param   game		
 * @param   booster		�u�[�X�^�[���[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
void Booster_ActorCreateAll(BALLOON_GAME_PTR game, BOOSTER_WORK *booster)
{
	GFL_CLWK_DATA act_head;
	GFL_CLWK_AFFINEDATA aff_head;
	int i, pal_ofs;
	fx32 base_x, base_y;
	
//	pal_ofs = PlayerNoPalOfs_Booster[game->bsw->player_max][Balloon_NetID_to_PlayerPos(game, CommGetCurrentID())];

	act_head = BoosterObjParam;
	for(i = 0; i < BOOSTER_ACTOR_MAX; i++){
		base_x = 256/2 + FX_Mul(Sin360FX(
			booster->theta + booster->move[i].start_theta), BOOSTER_FURIHABA_X);
		base_y = 196/2 + (-FX_Mul(
			Cos360FX(booster->theta + booster->move[i].start_theta), BOOSTER_FURIHABA_Y));
		
		act_head.pos_x = base_x;
		act_head.pos_y = base_y + BOOSTER_OFFSET_Y;
		booster->move[i].cap = GFL_CLACT_WK_Create(game->clunit, 
			game->cgr_id[CHARID_SUB_BALLOON_MIX], game->pltt_id[PLTTID_SUB_OBJ_COMMON], 
			game->cell_id[CELLID_SUB_BALLOON_MIX], &act_head, CLSYS_DEFREND_SUB, HEAPID_BALLOON);
		GFL_CLACT_WK_SetPlttOffs(booster->move[i].cap, PALOFS_SUB_BOOSTER_RED);
		GFL_CLACT_WK_SetAnmSeq(booster->move[i].cap, 
			BoosterType_StartSeqAnimeNo[i + BOOSTER_TYPE_NORMAL]);
		GFL_CLACT_WK_AddAnmFrame(booster->move[i].cap, FX32_ONE);
		GFL_CLACT_WK_SetDrawEnable(booster->move[i].cap, FALSE);
		
		//�q�b�g�G�t�F�N�g
		booster->move[i].hit_cap = GFL_CLACT_WK_Create(game->clunit, 
			game->cgr_id[CHARID_SUB_BALLOON_MIX], game->pltt_id[PLTTID_SUB_OBJ_COMMON], 
			game->cell_id[CELLID_SUB_BALLOON_MIX], 
			&BoosterHitObjParam, CLSYS_DEFREND_SUB, HEAPID_BALLOON);
		GFL_CLACT_WK_SetPlttOffs(booster->move[i].hit_cap, PALOFS_SUB_BOOSTER_HIT);
		GFL_CLACT_WK_SetAnmSeq(booster->move[i].hit_cap, MIXOBJ_ANMSEQ_BOOSTER_HIT_EFF);
		GFL_CLACT_WK_AddAnmFrame(booster->move[i].hit_cap, FX32_ONE);
		GFL_CLACT_WK_SetDrawEnable(booster->move[i].hit_cap, FALSE);
		
		//�e
		aff_head = BoosterShadowObjParam;
		aff_head.clwkdata.pos_x = base_x;
		aff_head.clwkdata.pos_y = base_y + BOOSTER_OFFSET_Y + BOOSTER_SHADOW_OFFSET_Y;
		booster->move[i].shadow_cap = GFL_CLACT_WK_CreateAffine(game->clunit, 
			game->cgr_id[CHARID_SUB_BALLOON_MIX], game->pltt_id[PLTTID_SUB_OBJ_COMMON], 
			game->cell_id[CELLID_SUB_BALLOON_MIX], 
			&aff_head, CLSYS_DEFREND_SUB, HEAPID_BALLOON);
		GFL_CLACT_WK_SetPlttOffs(booster->move[i].shadow_cap, PALOFS_SUB_BOOSTER_SHADOW);
		GFL_CLACT_WK_SetObjMode(booster->move[i].shadow_cap, GX_OAM_MODE_XLU);	//������ON
		GFL_CLACT_WK_SetAnmSeq(booster->move[i].shadow_cap, MIXOBJ_ANMSEQ_BOOSTER_SHADOW + i);
		GFL_CLACT_WK_AddAnmFrame(booster->move[i].shadow_cap, FX32_ONE);
		GFL_CLACT_WK_SetDrawEnable(booster->move[i].shadow_cap, FALSE);
		
		//���n�̉�
		BoosterLandSmoke_ActorCreate(game, &booster->move[i].land_smoke);
	}
	
	GFL_CLACT_WK_SetDrawEnable(booster->move[BOOSTER_ACTOR_NORMAL].cap, TRUE);
	GFL_CLACT_WK_SetDrawEnable(booster->move[BOOSTER_ACTOR_NORMAL].shadow_cap, TRUE);
}

//--------------------------------------------------------------
/**
 * @brief   �u�[�X�^�[�A�N�^�[��S�č폜����
 *
 * @param   game		
 * @param   booster		�u�[�X�^�[���[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
void Booster_ActorDeleteAll(BALLOON_GAME_PTR game, BOOSTER_WORK *booster)
{
	int i;
	
	for(i = 0; i < BOOSTER_ACTOR_MAX; i++){
		GFL_CLACT_WK_Remove(booster->move[i].cap);
		GFL_CLACT_WK_Remove(booster->move[i].hit_cap);
		GFL_CLACT_WK_Remove(booster->move[i].shadow_cap);
		BoosterLandSmoke_ActorDelete(game, &booster->move[i].land_smoke);
	}
}

//--------------------------------------------------------------
/**
 * @brief   �u�[�X�^�[���쐧�䃏�[�N������
 *
 * @param   game		
 * @param   booster		�u�[�X�^�[���쐧�䃏�[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
void Booster_Init(BALLOON_GAME_PTR game, BOOSTER_WORK *booster)
{
	int i;
	
	booster->add_theta = (360 << FX32_SHIFT) / BOOSTER_STOP_PART_NUM / BoosterMoveDataTbl[booster->lap].move_frame;
	for(i = 0; i < BOOSTER_ACTOR_MAX; i++){
		booster->move[i].start_theta = (i * 90) << FX32_SHIFT;
		booster->move[i].booster_type = BOOSTER_TYPE_NORMAL + i;
	}
	booster->move[BOOSTER_ACTOR_NORMAL].mode = BOOSTER_MODE_NORMAL;
	
	booster->stop = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �u�[�X�^�[�X�V����
 *
 * @param   game		
 * @param   booster		�u�[�X�^�[���쐧�䃏�[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
void Booster_Update(BALLOON_GAME_PTR game, BOOSTER_WORK *booster)
{
	int i, world_frame, end_frame = 0;
	
	if(booster->stop == TRUE){
		return;
	}
	
	booster->world_frame++;
	
	if(booster->wait == 0){
		//�b�j��i�߂�
		booster->theta += booster->add_theta;
		booster->frame++;
		if(booster->frame >= BoosterMoveDataTbl[booster->lap].move_frame){
			booster->frame = 0;
			booster->byousin_pos++;
			booster->theta = (360 << FX32_SHIFT) / BOOSTER_STOP_PART_NUM * booster->byousin_pos;
			booster->wait = BoosterMoveDataTbl[booster->lap].wait_frame;
		}
	}
	else{	//�҂�����
		booster->wait--;
		if(booster->wait == 0){
			if(booster->byousin_pos >= BOOSTER_STOP_PART_NUM){	//1������
				booster->byousin_pos = 0;
				booster->lap++;
				if(booster->lap >= NELEMS(BoosterMoveDataTbl)){
					booster->lap = NELEMS(BoosterMoveDataTbl) - 1;
				}
				booster->add_theta = (360 << FX32_SHIFT) / BOOSTER_STOP_PART_NUM / BoosterMoveDataTbl[booster->lap].move_frame;
			}
		}
	}

	//�V�����u�[�X�^�[�̓o��`�F�b�N
	if(booster->lap == BOOSTER_2ND_APPEAR_LAP - 1){
		for(i = 0; i < BOOSTER_2ND_APPEAR_LAP; i++){
			end_frame += BoosterMoveDataTbl[i].move_frame * BOOSTER_STOP_PART_NUM
				+ BoosterMoveDataTbl[i].wait_frame * BOOSTER_STOP_PART_NUM;
		}
		if(end_frame - BOOSTER_APPEAR_FRAME == booster->world_frame){
			BoosterMoveModeChange(&booster->move[1], BOOSTER_MODE_APPEAR);
		}
	}
	else if(booster->lap == BOOSTER_3RD_APPEAR_LAP - 1){
		for(i = 0; i < BOOSTER_3RD_APPEAR_LAP; i++){
			end_frame += BoosterMoveDataTbl[i].move_frame * BOOSTER_STOP_PART_NUM
				+ BoosterMoveDataTbl[i].wait_frame * BOOSTER_STOP_PART_NUM;
		}
		if(end_frame - BOOSTER_APPEAR_FRAME == booster->world_frame){
			BoosterMoveModeChange(&booster->move[2], BOOSTER_MODE_APPEAR);
		}
	}
	
	//�e�u�[�X�^�[�̓���
	for(i = 0; i < BOOSTER_ACTOR_MAX; i++){
		Booster_Move(game, booster, &booster->move[i]);
		BoosterLandSmoke_Update(game, &booster->move[i], &booster->move[i].land_smoke);
	}
}

//--------------------------------------------------------------
/**
 * @brief   ���̃^�C�~���O�Ńu�[�X�^�[�ƃq�b�g�\���`�F�b�N
 *
 * @param   game					
 * @param   ret_move		�q�b�g�����u�[�X�^�[�ւ̃|�C���^����������
 * 
 * @retval  �u�[�X�^�[�^�C�v
 */
//--------------------------------------------------------------
static int Booster_HitCheckNow(BALLOON_GAME_PTR game, BOOSTER_MOVE **ret_move)
{
	BOOSTER_WORK *booster = &game->booster;
	int i;
	
	for(i = 0; i < BOOSTER_ACTOR_MAX; i++){
		if(booster->move[i].hit_ok == TRUE){
			OS_TPrintf("hit booster_type = %d\n", booster->move[i].booster_type);
			*ret_move = &booster->move[i];
			return booster->move[i].booster_type;
		}
	}
	*ret_move = NULL;
	return BOOSTER_TYPE_NONE;
}

#if 0
static int Booster_HitCheck(BALLOON_GAME_PTR game, PLAYER_AIR_PARAM)
{
	BOOSTER_WORK *booster = &game->booster;
	int i;
	int arrival_frame;
	int player_pos;
	const AIR_POSITION_DATA *air_posdata;
	fx32 joint_len_y;
	
	player_pos = Balloon_NetID_to_PlayerPos(game, GFL_NET_SystemGetCurrentID());
	air_posdata = &AirPositionDataTbl[game->bsw->player_max][player_pos];
	
	joint_len_y = (air_posdata->y - JointActorPosTbl[JOINT_ACTOR_D].y) << FX32_SHIFT;
	arrival_frame = FX_Div(joint_len_y, AIR_MOVE_SPEED);
	if(FX_Mod(joint_len_y, AIR_MOVE_SPEED)){
		arrival_frame++;
	}
	arrival_frame += MY_AIR_2D_APPEAR_WAIT;		//���t���[����ɋ�C���W���C���g��ʉ߂��邩
	
	//arrival_frame�ɒB�����Ƃ��A�u�[�X�^�[��HIT�A��Ԃ��`�F�b�N
	for(i = 0; i < 
}
#endif

//--------------------------------------------------------------
/**
 * @brief   �u�[�X�^�[�̃��[�h�ؑ�
 *
 * @param   move		
 * @param   mode		BOOSTER_MODE_???
 */
//--------------------------------------------------------------
static void BoosterMoveModeChange(BOOSTER_MOVE *move, int mode)
{
	move->mode = mode;
	move->seq = 0;
	move->local_frame = 0;
	move->local_work = 0;
	move->hit_ok = FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �u�[�X�^�[�̈ړ�����
 *
 * @param   game		
 * @param   booster		�u�[�X�^�[���쐧�䃏�[�N�ւ̃|�C���^
 * @param   move		�u�[�X�^�[���샏�[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Booster_Move(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move)
{
	BoosterMoveSeqTbl[move->mode](game, booster, move);
}

//--------------------------------------------------------------
/**
 * @brief   �u�[�X�^�[����F�ҋ@��
 *
 * @param   game		
 * @param   booster		�u�[�X�^�[���쐧�䃏�[�NH
 * @param   move		�u�[�X�^�[���샏�[�N�ւ̃|�C���^
 *
 * @retval  
 */
//--------------------------------------------------------------
static int BoosterMove_Wait(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move)
{
	move->hit_ok = FALSE;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �u�[�X�^�[����F�o��
 *
 * @param   game		
 * @param   booster		�u�[�X�^�[���쐧�䃏�[�NH
 * @param   move		�u�[�X�^�[���샏�[�N�ւ̃|�C���^
 *
 * @retval  
 */
//--------------------------------------------------------------
static int BoosterMove_Appear(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move)
{
	s16 add_x, add_y, offset_y, offset_x;
	fx32 jump_theta;
	int jump_y = 0;
	fx32 check_scale_xy;
	GFL_CLACTPOS pos, srcpos;
	
	add_x = 0;
	add_y = 0;
	pos.x = move->local_x;
	pos.y = move->local_y;
	
	switch(move->seq){
	case 0:
		GFL_CLACT_WK_SetDrawEnable(move->cap, TRUE);
		GFL_CLACT_WK_SetDrawEnable(move->shadow_cap, TRUE);
		GFL_CLACT_WK_GetPos(move->cap, &pos, CLSYS_DEFREND_SUB);
		move->local_x = pos.x;
		move->local_y = pos.y;
		move->seq++;
		//break;
	case 1:
		switch(move->start_theta){
		case 0:
			offset_y = move->local_y - BOOSTER_APPEAR_START_Y_0;
			add_y = move->local_frame * offset_y / BOOSTER_APPEAR_FRAME;
			pos.y = BOOSTER_APPEAR_START_Y_0 + add_y;
			break;
		case 90<<FX32_SHIFT:
			offset_x = move->local_x - BOOSTER_APPEAR_START_X_90;
			add_x = move->local_frame * offset_x / BOOSTER_APPEAR_FRAME;
			pos.x = BOOSTER_APPEAR_START_X_90 + add_x;
			break;
		case 180<<FX32_SHIFT:
			offset_y = move->local_y - BOOSTER_APPEAR_START_Y_180;
			add_y = move->local_frame * offset_y / BOOSTER_APPEAR_FRAME;
			pos.y = BOOSTER_APPEAR_START_Y_180 + add_y;
			break;
		case 270<<FX32_SHIFT:
			offset_x = move->local_x - BOOSTER_APPEAR_START_X_270;
			add_x = move->local_frame * offset_x / BOOSTER_APPEAR_FRAME;
			pos.x = BOOSTER_APPEAR_START_X_270 + add_x;
			break;
		default:
			GF_ASSERT(0);
			break;
		}
		
		if(move->local_frame >= BOOSTER_APPEAR_FRAME){
			srcpos.x = move->local_x;
			srcpos.y = move->local_y;
			GFL_CLACT_WK_SetPos(move->cap, &srcpos, CLSYS_DEFREND_SUB);
			srcpos.x = move->local_x;
			srcpos.y = move->local_y + BOOSTER_SHADOW_OFFSET_Y;
			GFL_CLACT_WK_SetPos(move->shadow_cap, &srcpos, CLSYS_DEFREND_SUB);
			BoosterMoveModeChange(move, BOOSTER_MODE_NORMAL);
			BoosterLandSmoke_SmokeReq(game, move, &move->land_smoke);
			return TRUE;
		}

		jump_theta = ((180 * move->local_frame) << FX32_SHIFT) / BOOSTER_APPEAR_FRAME;
		jump_y = -(FX_Mul(Sin360FX(jump_theta), BOOSTER_APPEAR_JUMP_FURIHABA_Y)) / FX32_ONE;
		srcpos.x = pos.x;
		srcpos.y = pos.y + jump_y;
		GFL_CLACT_WK_SetPos(move->cap, &srcpos, CLSYS_DEFREND_SUB);
		srcpos.x = pos.x;
		srcpos.y = pos.y + BOOSTER_SHADOW_OFFSET_Y;
		GFL_CLACT_WK_SetPos(move->shadow_cap, &srcpos, CLSYS_DEFREND_SUB);
		check_scale_xy 
			= FX32_ONE - (FX32_ONE * (-jump_y/3) / (BOOSTER_APPEAR_JUMP_FURIHABA_Y >> FX32_SHIFT));
		GFL_CLACT_WK_SetTypeScale(move->shadow_cap, check_scale_xy, CLSYS_MAT_X);
		GFL_CLACT_WK_SetTypeScale(move->shadow_cap, check_scale_xy, CLSYS_MAT_Y);
		break;
	}
	move->local_frame++;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �u�[�X�^�[����F�ʏ�
 *
 * @param   game		
 * @param   booster		�u�[�X�^�[���쐧�䃏�[�NH
 * @param   move		�u�[�X�^�[���샏�[�N�ւ̃|�C���^
 *
 * @retval  
 */
//--------------------------------------------------------------
static int BoosterMove_Normal(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move)
{
	fx32 base_x, base_y;
	fx32 jump_theta;
	int offset_y = 0;
	fx32 check_scale_xy;
	GFL_CLACTPOS pos;
	
	if(booster->frame > 0){
		GFL_CLACT_WK_AddAnmFrame(move->cap, (BOOSTER_ANIME_FRAME_JUMP_TOTAL << FX32_SHIFT) / BoosterMoveDataTbl[booster->lap].move_frame);
	//	OS_TPrintf("booster anime frame = %d, lap = %d, frame = %d\n", (BOOSTER_ANIME_FRAME_JUMP_TOTAL << FX32_SHIFT) / BoosterMoveDataTbl[booster->lap].move_frame, booster->lap, booster->frame);
	}
	else{
	//	OS_TPrintf("0 booster anime frame = %d, lap = %d, frame = %d\n", (BOOSTER_ANIME_FRAME_JUMP_TOTAL << FX32_SHIFT) / BoosterMoveDataTbl[booster->lap].move_frame, booster->lap, booster->frame);
		GFL_CLACT_WK_SetAnmFrame(move->cap, 0);
	}
	
	jump_theta = ((180 * booster->frame) << FX32_SHIFT) / BoosterMoveDataTbl[booster->lap].move_frame;
	offset_y = -(FX_Mul(Sin360FX(jump_theta), BOOSTER_JUMP_FURIHABA_Y)) / FX32_ONE;

	base_x = 256/2 + FX_Mul(Sin360FX(booster->theta + move->start_theta), BOOSTER_FURIHABA_X);
	base_y = 196/2 + (-FX_Mul(Cos360FX(booster->theta + move->start_theta), BOOSTER_FURIHABA_Y));
	pos.x = base_x;
	pos.y = base_y + BOOSTER_OFFSET_Y + offset_y;
//	OS_TPrintf("pos.x = %d, y = %d\n", pos.x, pos.y);
	GFL_CLACT_WK_SetPos(move->cap, &pos, CLSYS_DEFREND_SUB);
	
	//�e
	pos.x = base_x;
	pos.y = base_y + BOOSTER_OFFSET_Y + BOOSTER_SHADOW_OFFSET_Y;
	GFL_CLACT_WK_SetPos(move->shadow_cap, &pos, CLSYS_DEFREND_SUB);
	check_scale_xy 
		= FX32_ONE - (FX32_ONE * (-offset_y/3) / (BOOSTER_JUMP_FURIHABA_Y >> FX32_SHIFT));
	GFL_CLACT_WK_SetTypeScale(move->shadow_cap, check_scale_xy, CLSYS_MAT_X);
	GFL_CLACT_WK_SetTypeScale(move->shadow_cap, check_scale_xy, CLSYS_MAT_Y);

	if(booster->frame == BoosterMoveDataTbl[booster->lap].move_frame - 1){
		BoosterLandSmoke_SmokeReq(game, move, &move->land_smoke);
	}
	
	if(((booster->theta + move->start_theta) >> FX32_SHIFT) % 360 == 180){
		move->hit_ok = TRUE;
	}
	else{
		move->hit_ok = FALSE;
	}
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �u�[�X�^�[����F�q�b�g
 *
 * @param   game		
 * @param   booster		�u�[�X�^�[���쐧�䃏�[�NH
 * @param   move		�u�[�X�^�[���샏�[�N�ւ̃|�C���^
 *
 * @retval  
 */
//--------------------------------------------------------------
static int BoosterMove_Hit(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move)
{
	int next_frame, next_byousin;
	s16 end_x, end_y;
	fx32 jump_theta;
	int jump_y = 0;
	GFL_CLACTPOS pos, start;
	
	switch(move->seq){
	case 0:
	#if WB_TEMP_FIX
		Snd_SePlay(SE_BALLOON_BOOSTER_HIT);
	#endif
		GFL_CLACT_WK_GetPos(move->cap, &pos, CLSYS_DEFREND_SUB);
		pos.y += BOOSTER_HIT_OFFSET_Y;
		GFL_CLACT_WK_SetPos(move->hit_cap, &pos, CLSYS_DEFREND_SUB);
		GFL_CLACT_WK_SetAnmSeq(move->hit_cap, MIXOBJ_ANMSEQ_BOOSTER_HIT_EFF);//�A�j�����Z�b�g
		GFL_CLACT_WK_SetDrawEnable(move->hit_cap, TRUE);
		
		//�{�̂̃A�j����ύX
		GFL_CLACT_WK_SetAnmSeq(move->cap, 	//1���ڂ��c���ł���G�Ȃ̂�
			BoosterType_StartSeqAnimeNo[move->booster_type] + OFFSET_ANMID_BOOSTER_SHRIVEL);
		
		move->local_frame = BoosterMoveDataTbl[booster->lap].hit_anime_time;
		move->local_wait = BoosterMoveDataTbl[booster->lap].hit_anime_time / 2;
		move->seq++;
		break;
	case 1:
		if(move->local_frame == move->local_wait){
			GFL_CLACT_WK_SetAnmSeq(move->cap, 	//1���ڂ�����ł���G�Ȃ̂�
				BoosterType_StartSeqAnimeNo[move->booster_type] + OFFSET_ANMID_BOOSTER_SWELLING);
			GFL_CLACT_WK_SetDrawEnable(move->hit_cap, FALSE);
			GFL_CLACT_WK_AddAnmFrame(move->hit_cap, BOOSTER_HITEFF_NEXT_ANM_FRAME);
		}
		if(move->local_frame == 0){
			GFL_CLACT_WK_SetDrawEnable(move->hit_cap, FALSE);	//�ꉞ�����ł�
			//1���ڂ����ڂ�ł���G�Ȃ̂�
			GFL_CLACT_WK_SetAnmSeq(move->cap, 	//1���ڂ��c���ł���G�Ȃ̂�
//				BoosterType_StartSeqAnimeNo[move->booster_type] + OFFSET_ANMID_BOOSTER_SWELLING);
				BoosterType_StartSeqAnimeNo[move->booster_type] + OFFSET_ANMID_BOOSTER_JUMP);
			
			//BoosterMoveModeChange(move, BOOSTER_MODE_NORMAL);
			//return TRUE;
			move->seq++;
			break;
		}
		move->local_frame--;
		break;
	case 2:
		next_frame = 0;
		if(booster->wait == 0){
			next_frame += BoosterMoveDataTbl[booster->lap].move_frame - booster->frame;
			next_frame += BoosterMoveDataTbl[booster->lap].wait_frame;
			next_frame += BoosterMoveDataTbl[booster->lap].move_frame;
			next_byousin = booster->byousin_pos + 2;
		}
		else{
			next_frame += booster->wait;
			if(booster->byousin_pos >= BOOSTER_STOP_PART_NUM){
				next_frame += BoosterMoveDataTbl[booster->lap + 1].move_frame;
				next_byousin = 1;
			}
			else{
				next_frame += BoosterMoveDataTbl[booster->lap].move_frame;
				next_byousin = booster->byousin_pos + 1;
				if(next_byousin >= BOOSTER_STOP_PART_NUM){
//					next_byousin = 0;
				}
			}
		}
		next_byousin += move->start_theta / ((360 / BOOSTER_STOP_PART_NUM) << FX32_SHIFT);
		next_byousin %= BOOSTER_STOP_PART_NUM;
		
		end_x = 256/2 + FX_Mul(Sin360FX((360<<FX32_SHIFT) / BOOSTER_STOP_PART_NUM * next_byousin), BOOSTER_FURIHABA_X);
		end_y = 196/2 + (-FX_Mul(Cos360FX((360<<FX32_SHIFT) / BOOSTER_STOP_PART_NUM * next_byousin), BOOSTER_FURIHABA_Y));
		GFL_CLACT_WK_GetPos(move->cap, &start, CLSYS_DEFREND_SUB);
		start.y -= BOOSTER_OFFSET_Y;
		move->local_add_x = (end_x - start.x) * FX32_ONE / next_frame;
		move->local_add_y = (end_y - start.y) * FX32_ONE / next_frame;
		move->local_fx_x = start.x * FX32_ONE;
		move->local_fx_y = start.y * FX32_ONE;
		move->local_frame = next_frame;
		move->local_work = next_frame;
		move->seq++;
//		OS_TPrintf("move add_x = %d, add_y = %d, fx_x = %d, fx_y = %d, frame = %d, next_byousin = %d\n", move->local_add_x, move->local_add_y, move->local_fx_x, move->local_fx_y, move->local_frame, next_byousin);
		//break;
	case 3:
		move->local_fx_x += move->local_add_x;
		move->local_fx_y += move->local_add_y;
		
		jump_theta = ((180 * move->local_frame) << FX32_SHIFT) / move->local_work;
		jump_y = -(FX_Mul(Sin360FX(jump_theta), BOOSTER_JUMP_FURIHABA_Y)) / FX32_ONE;
		
		pos.x = move->local_fx_x / FX32_ONE;
		pos.y = move->local_fx_y / FX32_ONE + BOOSTER_OFFSET_Y + jump_y;
		GFL_CLACT_WK_SetPos(move->cap, &pos, CLSYS_DEFREND_SUB);
		GFL_CLACT_WK_AddAnmFrame(move->cap, (BOOSTER_ANIME_FRAME_JUMP_TOTAL << FX32_SHIFT) / move->local_work);

		//�e
		pos.x = move->local_fx_x / FX32_ONE;
		pos.y = move->local_fx_y / FX32_ONE + BOOSTER_OFFSET_Y + BOOSTER_SHADOW_OFFSET_Y;
		GFL_CLACT_WK_SetPos(move->shadow_cap, &pos, CLSYS_DEFREND_SUB);
		
		move->local_frame--;
		if(move->local_frame <= 0){
			GFL_CLACT_WK_SetAnmSeq(move->cap, 
				BoosterType_StartSeqAnimeNo[move->booster_type] + OFFSET_ANMID_BOOSTER_JUMP);
			GFL_CLACT_WK_SetAnmFrame(move->cap, 0);
			BoosterMoveModeChange(move, BOOSTER_MODE_NORMAL);
			return TRUE;
		}
		break;
	}
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   SIO�u�[�X�^�[�A�N�^�[����
 *
 * @param   game		
 * @param   sio_booster		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
void SioBooster_ActorCreateAll(BALLOON_GAME_PTR game, SIO_BOOSTER_WORK *sio_booster)
{
	GFL_CLWK_DATA act_head;
	int i;
	
	act_head = BoosterObjParam;
	for(i = 0; i < SIO_BOOSTER_ACTOR_MAX; i++){
		sio_booster->move[i].cap = GFL_CLACT_WK_Create(game->clunit, 
			game->cgr_id[CHARID_SUB_BALLOON_MIX], game->pltt_id[PLTTID_SUB_OBJ_COMMON], 
			game->cell_id[CELLID_SUB_BALLOON_MIX], &act_head, CLSYS_DEFREND_SUB, HEAPID_BALLOON);
		GFL_CLACT_WK_SetPlttOffs(sio_booster->move[i].cap, PALOFS_SUB_BOOSTER_RED);
		GFL_CLACT_WK_SetDrawEnable(sio_booster->move[i].cap, FALSE);
		
		//�q�b�g�G�t�F�N�g
		sio_booster->move[i].hit_cap = GFL_CLACT_WK_Create(game->clunit, 
			game->cgr_id[CHARID_SUB_BALLOON_MIX], game->pltt_id[PLTTID_SUB_OBJ_COMMON], 
			game->cell_id[CELLID_SUB_BALLOON_MIX], &act_head, CLSYS_DEFREND_SUB, HEAPID_BALLOON);
		GFL_CLACT_WK_SetPlttOffs(sio_booster->move[i].hit_cap, PALOFS_SUB_BOOSTER_HIT);
		GFL_CLACT_WK_SetAnmSeq(sio_booster->move[i].hit_cap, MIXOBJ_ANMSEQ_BOOSTER_HIT_EFF);
		GFL_CLACT_WK_AddAnmFrame(sio_booster->move[i].hit_cap, FX32_ONE);
		GFL_CLACT_WK_SetDrawEnable(sio_booster->move[i].hit_cap, FALSE);
	}
}

//--------------------------------------------------------------
/**
 * @brief   SIO�u�[�X�^�[�A�N�^�[��S�č폜����
 *
 * @param   game		
 * @param   sio_booster		SIO�u�[�X�^�[���[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
void SioBooster_ActorDeleteAll(BALLOON_GAME_PTR game, SIO_BOOSTER_WORK *sio_booster)
{
	int i;
	
	for(i = 0; i < SIO_BOOSTER_ACTOR_MAX; i++){
		GFL_CLACT_WK_Remove(sio_booster->move[i].cap);
		GFL_CLACT_WK_Remove(sio_booster->move[i].hit_cap);
	}
}

//--------------------------------------------------------------
/**
 * @brief   SIO�u�[�X�^�[�o��
 *
 * @param   game		
 * @param   sio_booster		SIO�u�[�X�^�[���[�N�ւ̃|�C���^
 * @param   booster_type	�u�[�X�^�[�^�C�v
 * @param   net_id			�Ώۂ̃l�b�gID
 * @param   arrival_frame	��C�A�N�^�[���W���C���g�����܂łɂ�����t���[����
 */
//--------------------------------------------------------------
void SioBooster_Appear(BALLOON_GAME_PTR game, SIO_BOOSTER_WORK *sio_booster, int booster_type, int net_id, int arrival_frame)
{
	int i, pal_ofs, player_pos, joint_no;
	SIO_BOOSTER_MOVE *sio_move = NULL;
	GFL_CLACTPOS pos;
	
	if(booster_type == BOOSTER_TYPE_NONE || net_id == GFL_NET_SystemGetCurrentID()){
		return;
	}
	for(i = 0; i < SIO_BOOSTER_ACTOR_MAX; i++){
		if(sio_booster->move[i].occ == FALSE){
			sio_move = &sio_booster->move[i];
			break;
		}
	}
	if(sio_move == NULL){
		return;
	}
	
	//�p���b�g�ݒ�
	player_pos = Balloon_NetID_to_PlayerPos(game, net_id);
	pal_ofs = PlayerNoPalOfs_Booster[game->bsw->player_max][player_pos];
//	CATS_ObjectPaletteOffsetSetCap(sio_move->cap, pal_ofs);
	GFL_CLACT_WK_SetPlttOffs(sio_move->cap, pal_ofs);
	
	//�A�j���ݒ�
	GFL_CLACT_WK_SetAnmSeq(sio_move->cap, 
		BoosterType_StartSeqAnimeNo[booster_type] + OFFSET_ANMID_BOOSTER_JUMP);
	
	//���W�ݒ�
	joint_no = PlayerNoJointNo_SioBooster[game->bsw->player_max][player_pos];
	sio_move->end_y = JointActorPosTbl[joint_no].y + BOOSTER_OFFSET_Y;
	pos.x = JointActorPosTbl[joint_no].x;
	pos.y = sio_move->end_y + SIO_BOOSTER_APPEAR_OFFSET_Y;
	GFL_CLACT_WK_SetPos(sio_move->cap, &pos, CLSYS_DEFREND_SUB);

	GFL_CLACT_WK_AddAnmFrame(sio_move->cap, FX32_ONE);
	sio_move->booster_type = booster_type;
	sio_move->start_wait = arrival_frame - (MATH_ABS(SIO_BOOSTER_APPEAR_OFFSET_Y) * FX32_ONE) / SIO_BOOSTER_IN_SPEED;
	sio_move->seq = 0;
	sio_move->occ = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   SIO�u�[�X�^�[�X�V����
 *
 * @param   game		
 * @param   sio_booster		SIO�u�[�X�^�[���[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
void SioBooster_Update(BALLOON_GAME_PTR game, SIO_BOOSTER_WORK *sio_booster)
{
	int i;
	SIO_BOOSTER_MOVE *sio_move;
	
	for(i = 0; i < SIO_BOOSTER_ACTOR_MAX; i++){
		if(sio_booster->move[i].occ == TRUE){
			sio_move = &sio_booster->move[i];
			SioBoosterMove_Appear(sio_booster, sio_move);
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   SIO�u�[�X�^�[����F�o��
 *
 * @param   sio_booster		SIO�u�[�X�^�[���[�N�ւ̃|�C���^
 * @param   sio_move		SIO�u�[�X�^�[���샏�[�N�ւ̃|�C���^
 *
 * @retval  TRUE:�����p�����A�@FALSE:�����I��
 */
//--------------------------------------------------------------
static BOOL SioBoosterMove_Appear(SIO_BOOSTER_WORK *sio_booster, SIO_BOOSTER_MOVE *sio_move)
{
	GFL_CLACTPOS pos;
	
	switch(sio_move->seq){
	case 0:		//���[�N������
		GFL_CLACT_WK_GetPos(sio_move->cap, &pos, CLSYS_DEFREND_SUB);
		sio_move->local_fx_x = pos.x * FX32_ONE;
		sio_move->local_fx_y = pos.y * FX32_ONE;
		sio_move->seq++;
		//break;
	case 1:		//����
		if(sio_move->start_wait > 0){
			sio_move->start_wait--;
			break;
		}
		GFL_CLACT_WK_SetDrawEnable(sio_move->cap, TRUE);
		sio_move->local_fx_y += SIO_BOOSTER_IN_SPEED;
		if((sio_move->local_fx_y / FX32_ONE) >= sio_move->end_y){
			sio_move->local_fx_y = sio_move->end_y * FX32_ONE;
			sio_move->seq++;
		}
		break;
	case 2:		//�A�j��
		GFL_CLACT_WK_GetPos(sio_move->cap, &pos, CLSYS_DEFREND_SUB);
		pos.y += BOOSTER_HIT_OFFSET_Y;
		GFL_CLACT_WK_SetPos(sio_move->hit_cap, &pos, CLSYS_DEFREND_SUB);
		GFL_CLACT_WK_SetAnmSeq(sio_move->hit_cap, MIXOBJ_ANMSEQ_BOOSTER_HIT_EFF);
		GFL_CLACT_WK_SetDrawEnable(sio_move->hit_cap, TRUE);
	#if WB_TEMP_FIX
		Snd_SePlay(SE_BALLOON_BOOSTER_HIT);
	#endif
	
		//�{�̂̃A�j����ύX
		GFL_CLACT_WK_SetAnmSeq(sio_move->cap, 	//1���ڂ��c���ł���G�Ȃ̂�
			BoosterType_StartSeqAnimeNo[sio_move->booster_type] + OFFSET_ANMID_BOOSTER_SHRIVEL);
		sio_move->local_frame = 8;
		sio_move->seq++;
		break;
	case 3:
		sio_move->local_frame--;
		if(sio_move->local_frame == 3){
			GFL_CLACT_WK_AddAnmFrame(sio_move->hit_cap, BOOSTER_HITEFF_NEXT_ANM_FRAME);
		}
		if(sio_move->local_frame == 0){
			GFL_CLACT_WK_SetDrawEnable(sio_move->hit_cap, FALSE);
			GFL_CLACT_WK_SetAnmSeq(sio_move->cap,	//1���ڂ����ڂ�ł���G�Ȃ̂�
				BoosterType_StartSeqAnimeNo[sio_move->booster_type] + OFFSET_ANMID_BOOSTER_JUMP);
			sio_move->seq++;
		}
		break;
	case 4:		//�㏸
		sio_move->local_fx_y -= SIO_BOOSTER_OUT_SPEED;
		if((sio_move->local_fx_y / FX32_ONE) <= sio_move->end_y + SIO_BOOSTER_APPEAR_OFFSET_Y){
			GFL_CLACT_WK_SetDrawEnable(sio_move->cap, FALSE);
			sio_move->seq = 0;
			sio_move->occ = FALSE;
			return FALSE;
		}
		break;
	}
	
	pos.x = sio_move->local_fx_x / FX32_ONE;
	pos.y = sio_move->local_fx_y / FX32_ONE;
	GFL_CLACT_WK_SetPos(sio_move->cap, &pos, CLSYS_DEFREND_SUB);
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���C����ʂ̃t�H���gOAM���쐬����
 *
 * @param   none
 * @param   crp				crp�ւ̃|�C���^
 * @param   fontoam_sys		�t�H���g�V�X�e���ւ̃|�C���^
 * @param   ret_fontoam		���������t�H���gOAM�����
 * @param   ret_cma			���������t�H���gOAM�̃L�����̈�������
 * @param   str				������
 * @param   font_type		�t�H���g�^�C�v(FONT_SYSTEM��)
 * @param   color			�t�H���g�J���[�\��
 * @param   pal_offset		�p���b�g�ԍ��I�t�Z�b�g
 * @param   pal_id			�o�^�J�n�p���b�gID
 * @param   x				���WX
 * @param   y				���WY
 * @param   pos_center  	FALSE(X���[���W) or TRUE(X���S���W)
 * @param   bg_pri			BG�v���C�I���e�B
 * @param   soft_pri		�\�t�g�v���C�I���e�B
 * @param   y_char_len		BMP��Y�T�C�Y(�L�����P��)
 */
//--------------------------------------------------------------
#if WB_TEMP_FIX
void BalloonTool_FontOamCreate(CATS_RES_PTR crp, 
	FONTOAM_SYS_PTR fontoam_sys, BALLOON_FONTACT *fontact, const STRBUF *str, 
	FONT_TYPE font_type, GF_PRINTCOLOR color, int pal_offset, int pal_id, 
	int x, int y, int pos_center, int bg_pri, int soft_pri, int y_char_len)
{
	FONTOAM_INIT finit;
	GFL_BMPWIN* bmpwin;
	CHAR_MANAGER_ALLOCDATA cma;
	int vram_size;
	FONTOAM_OBJ_PTR fontoam;
	int font_len, char_len;
	int margin = 0;
	
	//������̃h�b�g������A�g�p����L���������Z�o����
	{
		font_len = FontProc_GetPrintStrWidth(font_type, str, margin);
		char_len = font_len / 8;
		if(FX_ModS32(font_len, 8) != 0){
			char_len++;
		}
	}

	//BMP�쐬
	{
		GF_BGL_BmpWinInit(&bmpwin);
		GF_BGL_BmpWinObjAdd(&bmpwin, char_len, y_char_len, 0, 0);
		GF_STR_PrintExpand(&bmpwin, font_type, str, 0, 0, MSG_NO_PUT, color, 
			margin, 0, NULL);
//		GF_STR_PrintColor(&bmpwin, font_type, str, 0, 0, MSG_NO_PUT, color, NULL );
	}

	vram_size = FONTOAM_NeedCharSize(&bmpwin, NNS_G2D_VRAM_TYPE_2DMAIN,  HEAPID_BALLOON);
	CharVramAreaAlloc(vram_size, CHARM_CONT_AREACONT, NNS_G2D_VRAM_TYPE_2DMAIN, &cma);
	
	//���W�ʒu�C��
	if(pos_center == TRUE){
		x -= font_len / 2;
	}
	y += MAIN_SURFACE_Y_INTEGER;// - 8;
	
	finit.fontoam_sys = fontoam_sys;
	finit.bmp = &bmpwin;
	finit.clact_set = CATS_GetClactSetPtr(crp);
	finit.pltt = CATS_PlttProxy(crp, pal_id);
	finit.parent = NULL;
	finit.char_ofs = cma.alloc_ofs;
	finit.x = x;
	finit.y = y;
	finit.bg_pri = bg_pri;
	finit.soft_pri = soft_pri;
	finit.draw_area = NNS_G2D_VRAM_TYPE_2DMAIN;
	finit.heap = HEAPID_BALLOON;
	
	fontoam = FONTOAM_Init(&finit);
	if(pal_offset != 0){
		FONTOAM_SetPaletteOffset(fontoam, pal_offset);
	}
	FONTOAM_SetMat(fontoam, x, y);
	
	//�������
	GFL_BMPWIN_Delete(&bmpwin);
	
	fontact->fontoam = fontoam;
	fontact->cma = cma;
	fontact->len = font_len;
}
#endif

//--------------------------------------------------------------
/**
 * @brief   �t�H���gOAM���폜����
 * @param   fontact		�t�H���g�A�N�^�[�ւ̃|�C���^
 */
//--------------------------------------------------------------
void Balloon_FontOamDelete(BALLOON_FONTACT *fontact)
{
#if WB_TEMP_FIX
	FONTOAM_Delete(fontact->fontoam);
	CharVramAreaFree(&fontact->cma);
	
	fontact->fontoam = NULL;
#endif
}

//--------------------------------------------------------------
/**
 * @brief   �J�E���^�[�̍��W�X�V
 *
 * @param   counter		�J�E���^�[���쐧�䃏�[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
void Balloon_CounterPosUpdate(BALLOON_COUNTER *counter)
{
	int i, dot_pos;
	int act_len = 16*5;	//1�̃A�N�^�[�̒���
	int x, y, y0, y1;
	
	for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++){
		dot_pos = counter->number[i];
		if(dot_pos < act_len - 16){
			y0 = act_len - 16 - dot_pos;
			y1 = y0 - act_len;
		}
//		else if(dot_pos > act_len*2 - 32){
//			y0 = -32 - dot_pos;
//			y1 = y0 - act_len;
//		}
		else if(dot_pos < 160-32){
			y1 = act_len - 16 - (dot_pos - act_len);
			y0 = y1 - act_len;
		}
		else{
			y0 = act_len - 16 - (dot_pos - 160);
			y1 = y0 - act_len;
		}
	#if WB_TEMP_FIX
		FONTOAM_GetMat(counter->fontact[i][BALLOON_COUNTER_0].fontoam, &x, &y);
		FONTOAM_SetMat(counter->fontact[i][BALLOON_COUNTER_0].fontoam, x, COUNTER_Y - y0);
		FONTOAM_SetMat(counter->fontact[i][BALLOON_COUNTER_1].fontoam, x, COUNTER_Y - y1);
	#endif
	}
}

//--------------------------------------------------------------
/**
 * @brief   �J�E���^�[�X�V����
 *
 * @param   game		
 * @param   counter		�J�E���^�[���쐧�䃏�[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
void Balloon_CounterUpdate(BALLOON_GAME_PTR game, BALLOON_COUNTER *counter)
{
	int i;
	BOOL rotate_ret;
	
	if(counter->wait > 0){
		counter->wait--;
		return;
	}
	
	switch(counter->seq){
	case 0:
		if(Balloon_CounterAgreeCheck(counter) == TRUE){
			Balloon_CounterLastNumberSet(counter);
			break;
		}
		else{
			counter->seq++;
		}
		//break;
	case 1:
		rotate_ret = Balloon_CounterRotate(counter);
		Balloon_CounterPosUpdate(counter);
		if(rotate_ret == TRUE){
			counter->wait = COUNTER_AGREE_AFTER_WAIT;
			counter->seq--;
		}
		break;
	}
}

//--------------------------------------------------------------
/**
 * @brief   �J�E���^�[�̎��ɕ\�������鐔�l�����N�G�X�g���Ă���
 *
 * @param   counter		
 * @param   next_number		���l
 */
//--------------------------------------------------------------
void Balloon_CounterNextNumberSet(BALLOON_COUNTER *counter, int next_number)
{
	int i, div_num, number;
	
	div_num = 100000;
	GF_ASSERT(BALLOON_COUNTER_KETA_MAX == 6);	//keta���ς���Ă�����div_num�̏����l���ύX
	
	for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++){
		number = next_number / div_num;
		counter->next_number[i] = number * 16;
		next_number -= number * div_num;
		div_num /= 10;
	}
}

//--------------------------------------------------------------
/**
 * @brief   �J�E���^�[�̕\�������鐔�l�����N�G�X�g���������Ă��鐔�l�ōX�V����
 *
 * @param   counter		
 */
//--------------------------------------------------------------
static void Balloon_CounterLastNumberSet(BALLOON_COUNTER *counter)
{
	int i;
	
	for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++){
		counter->last_number[i] = counter->next_number[i];
		counter->move_wait[i] = COUNTER_MOVE_START_WAIT * (BALLOON_COUNTER_KETA_MAX - i - 1);
		counter->rotate_count[i] = 0;
	}
	counter->move_keta = 0;
}

//--------------------------------------------------------------
/**
 * @brief   �J�E���^�[�̐��l���\�����l�ƈ�v���Ă��邩�m�F
 *
 * @param   counter		
 *
 * @retval  TRUE:��v�B�@FALSE�F�s��v
 */
//--------------------------------------------------------------
static BOOL Balloon_CounterAgreeCheck(BALLOON_COUNTER *counter)
{
	int i;
	
	for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++){
		if(counter->number[i] != counter->last_number[i]){
			return FALSE;
		}
	}
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �J�E���^�[��]����
 *
 * @param   counter		
 *
 * @retval  TRUE�F��]����I��
 * @retval  FALSE�F��]����p����
 */
//--------------------------------------------------------------
static BOOL Balloon_CounterRotate(BALLOON_COUNTER *counter)
{
	int i;
	int pos = BALLOON_COUNTER_KETA_MAX - 1;
	int before_number, mae_stop, stop, over_last_number;
	
	if(counter->move_keta > BALLOON_COUNTER_KETA_MAX){
		return TRUE;
	}
	
	mae_stop = FALSE;
	stop = FALSE;
	for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++, pos--){
		mae_stop = stop;

		if(counter->move_wait[pos] > 0){
			counter->move_wait[pos]--;
			stop = FALSE;
			continue;
		}
		
		if(counter->rotate_count[pos] > COUNTER_ROTATE_NUM 
				&& counter->number[pos] == counter->last_number[pos]){
			stop = TRUE;
			continue;
		}
		stop = FALSE;
		
		before_number = counter->number[pos];
		counter->number[pos] += COUNTER_SPEED;
		over_last_number = counter->last_number[pos] + 16*10;	//���オ��`�F�b�N�p

		if((before_number <= counter->last_number[pos] 
				&& counter->number[pos] >= counter->last_number[pos])
				|| (before_number <= over_last_number 
				&& counter->number[pos] >= over_last_number)){
			//�������
			if(counter->rotate_count[pos] < COUNTER_ROTATE_NUM){
				counter->rotate_count[pos]++;
			}
			else if(pos == BALLOON_COUNTER_KETA_MAX - 1 || mae_stop == TRUE){
				counter->rotate_count[pos]++;
				counter->number[pos] = counter->last_number[pos];
				counter->move_keta++;
				if(counter->move_keta >= BALLOON_COUNTER_KETA_MAX){
					return TRUE;
				}
			}
		}
		counter->number[pos] %= 10*16;
	}
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �J�E���^�[�E�B���h�E�A�N�^�[����
 *
 * @param   game		
 *
 * @retval  ���������j��A�N�^�[�ւ̃|�C���^
 */
//--------------------------------------------------------------
GFL_CLWK* CounterWindow_ActorCreate(BALLOON_GAME_PTR game)
{
	GFL_CLWK* cap;

	//-- �A�N�^�[���� --//
	cap = GFL_CLACT_WK_Create(game->clunit, game->cgr_id[CHARID_COUNTER_WIN], 
		game->pltt_id[PLTTID_COUNTER_WIN], game->cell_id[CELLID_COUNTER_WIN], 
		&CounterWindowObjParam, CLSYS_DEFREND_MAIN, HEAPID_BALLOON);
	GFL_CLACT_WK_AddAnmFrame(cap, FX32_ONE);
	return cap;
}

//--------------------------------------------------------------
/**
 * @brief   �J�E���^�[�_�~�[�A�N�^�[����
 *
 * @param   game		
 */
//--------------------------------------------------------------
void CounterDummyNumber_ActorCreate(BALLOON_GAME_PTR game)
{
#if WB_TEMP_FIX
	//-- �J�E���^�[ --//
	STRBUF *str0;
	int i;
	u32 number;
	
	number = game->my_total_air;
	for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++){
		GF_ASSERT(game->counter.fontact_dummy[i].fontoam == NULL);
		str0 = MSGMAN_AllocString(game->msgman, msg_balloon_num0 + (number % 10));
		number /= 10;
		BalloonTool_FontOamCreate(game->crp, game->fontoam_sys,
			&game->counter.fontact_dummy[i], str0, FONT_SYSTEM, 
			COUNTER_FONT_COLOR, 0, PLTTID_COUNTER, 
			(COUNTER_BASE_X + COUNTER_X_SPACE * (BALLOON_COUNTER_KETA_MAX-1)) - i*COUNTER_X_SPACE,
			COUNTER_Y, FALSE, BALLOON_BGPRI_DUMMY_COUNTER, BALLOON_SOFTPRI_COUNTER, 2*1);
		GFL_STR_DeleteBuffer(str0);
	}
#endif
}

//--------------------------------------------------------------
/**
 * @brief   �J�E���^�[�_�~�[�A�N�^�[�폜
 *
 * @param   game		
 */
//--------------------------------------------------------------
void CounterDummyNumber_ActorDelete(BALLOON_GAME_PTR game)
{
#if WB_TEMP_FIX
	int i;
	
	for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++){
		if(game->counter.fontact_dummy[i].fontoam != NULL){
			Balloon_FontOamDelete(&game->counter.fontact_dummy[i]);
		}
	}
#endif
}

//--------------------------------------------------------------
/**
 * @brief   ���D�A�C�R����S�č폜����
 *
 * @param   game		
 */
//--------------------------------------------------------------
void CounterWindow_ActorDelete(BALLOON_GAME_PTR game, GFL_CLWK* cap)
{
	GFL_CLACT_WK_Remove(cap);
}

//--------------------------------------------------------------
/**
 * @brief   �^�b�`�y���A�N�^�[����
 *
 * @param   game		
 */
//--------------------------------------------------------------
GFL_CLWK* TouchPen_ActorCreate(BALLOON_GAME_PTR game)
{
	GFL_CLWK* cap;

	cap = GFL_CLACT_WK_Create(game->clunit, game->cgr_id[CHARID_TOUCH_PEN], 
		game->pltt_id[PLTTID_TOUCH_PEN], 
		game->cell_id[CELLID_TOUCH_PEN], &TouchPenObjParam, CLSYS_DEFREND_MAIN, HEAPID_BALLOON);
	GFL_CLACT_WK_SetDrawEnable(cap, FALSE);
	GFL_CLACT_WK_AddAnmFrame(cap, FX32_ONE);
	return cap;
}

//--------------------------------------------------------------
/**
 * @brief   �^�b�`�y���A�N�^�[��S�č폜����
 *
 * @param   game		
 */
//--------------------------------------------------------------
void TouchPen_ActorDelete(BALLOON_GAME_PTR game, GFL_CLWK* cap)
{
	GFL_CLACT_WK_Remove(cap);
}

//--------------------------------------------------------------
/**
 * @brief   �^�b�`�y���f������
 *
 * @param   game		
 * @param   pen			�^�b�`�y�����쐧�䃏�[�N�ւ̃|�C���^
 *
 * @retval  TRUE:�f���I��
 * @retval  FALSE:�f���p����
 */
//--------------------------------------------------------------
BOOL BalloonTool_TouchPenDemoMove(BALLOON_GAME_PTR game, BALLOON_PEN *pen)
{
	GFL_CLACTPOS pos, calcpos;

	switch(pen->seq){
	case 0:
	case 5:
		pos.x = DEMO_PEN_X;
		pos.y = DEMO_PEN_Y;
		GFL_CLACT_WK_SetPos(pen->cap, &pos, CLSYS_DEFREND_MAIN);
		GFL_CLACT_WK_SetDrawEnable(pen->cap, TRUE);
		pen->seq++;
		break;
	case 1:
	case 6:
		pen->wait++;
		if(pen->wait > DEMO_PEN_START_WAIT){
			pen->wait = 0;
			pen->tp_cont = TRUE;
			pen->tp_trg = TRUE;
			pen->tp_x = 128;
			pen->tp_y = 32;
			pen->seq++;
		}
		break;
	case 2:
	case 7:
		pen->tp_trg = FALSE;
		pen->tp_y += DEMO_PEN_SPEED;
		GFL_CLACT_WK_GetPos(pen->cap, &calcpos, CLSYS_DEFREND_MAIN);
		calcpos.y += DEMO_PEN_SPEED;
		GFL_CLACT_WK_SetPos(pen->cap, &calcpos, CLSYS_DEFREND_MAIN);
		if(pen->tp_y > 192 - 96){
			pen->seq++;
		}
		break;
	case 3:
	case 8:
		pen->tp_cont = FALSE;
		GFL_CLACT_WK_SetDrawEnable(pen->cap, FALSE);
		pen->seq++;
		//break;
	case 4:
	case 9:
		pen->wait++;
		if(pen->wait > DEMO_PEN_AFTER_WAIT){
			pen->wait = 0;
			pen->seq++;
		}
		break;
	case 10:
		return TRUE;
	}
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �u�[�X�^�[�̒��n���̉��A�N�^�[����
 *
 * @param   game		
 */
//--------------------------------------------------------------
static void BoosterLandSmoke_ActorCreate(BALLOON_GAME_PTR game, BOOSTER_LAND_SMOKE *land_smoke)
{
	GFL_CLWK* cap;
	int i;
	
	for(i = 0; i < BOOSTER_LAND_SMOKE_NUM; i++){
		cap = GFL_CLACT_WK_Create(game->clunit, game->cgr_id[CHARID_SUB_BALLOON_MIX], 
			game->pltt_id[PLTTID_SUB_OBJ_COMMON], 
			game->cell_id[CELLID_SUB_BALLOON_MIX], 
			&BoosterLandSmokeObjParam, CLSYS_DEFREND_SUB, HEAPID_BALLOON);
		GFL_CLACT_WK_SetPlttOffs(cap, PALOFS_SUB_BOOSTER_LAND_SMOKE);
		GFL_CLACT_WK_SetAnmSeq(cap, MIXOBJ_ANMSEQ_BOOSTER_LAND_SMOKE);
		GFL_CLACT_WK_SetObjMode(cap, GX_OAM_MODE_XLU);	//������ON
		GFL_CLACT_WK_SetDrawEnable(cap, FALSE);
		GFL_CLACT_WK_AddAnmFrame(cap, FX32_ONE);
		
		land_smoke->cap[i] = cap;
	}
}

//--------------------------------------------------------------
/**
 * @brief   �u�[�X�^�[�̒��n���̉��A�N�^�[�폜
 *
 * @param   game		
 */
//--------------------------------------------------------------
static void BoosterLandSmoke_ActorDelete(BALLOON_GAME_PTR game, BOOSTER_LAND_SMOKE *land_smoke)
{
	int i;
	
	for(i = 0; i < BOOSTER_LAND_SMOKE_NUM; i++){
		GFL_CLACT_WK_Remove(land_smoke->cap[i]);
	}
}

//--------------------------------------------------------------
/**
 * @brief   �u�[�X�^�[���n���̓�������N�G�X�g
 *
 * @param   game		
 * @param   move			�u�[�X�^�[���쐧�䃏�[�N�ւ̃|�C���^
 * @param   land_smoke		���n�����쐧�䃏�[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void BoosterLandSmoke_SmokeReq(BALLOON_GAME_PTR game, BOOSTER_MOVE *move, BOOSTER_LAND_SMOKE *land_smoke)
{
	int i;
	GFL_CLACTPOS pos, srcpos;

	GFL_CLACT_WK_GetPos(move->cap, &pos, CLSYS_DEFREND_SUB);
	
	for(i = 0; i < BOOSTER_LAND_SMOKE_NUM; i++){
		srcpos.x = pos.x;
		srcpos.y = pos.y + BOOSTER_LAND_SMOKE_OFFSET_Y;
		land_smoke->x[i] = srcpos.x * FX32_ONE;
		land_smoke->y[i] = srcpos.y * FX32_ONE;
		GFL_CLACT_WK_SetPos(land_smoke->cap[i], &srcpos, CLSYS_DEFREND_SUB);
		GFL_CLACT_WK_SetAnmSeq(land_smoke->cap[i], 
			MIXOBJ_ANMSEQ_BOOSTER_LAND_SMOKE);
		GFL_CLACT_WK_SetObjMode(land_smoke->cap[i], GX_OAM_MODE_XLU);	//������ON
		GFL_CLACT_WK_SetDrawEnable(land_smoke->cap[i], TRUE);

		land_smoke->add_x[i] = GFUser_GetPublicRand(0x1000) + 0x1000;
		land_smoke->add_y[i] = GFUser_GetPublicRand(0x400) + 0x400;
		if(i & 1){
			land_smoke->add_x[i] *= -1;
		}
	}
	land_smoke->time = 0;
	land_smoke->seq = 1;
}

//--------------------------------------------------------------
/**
 * @brief   �u�[�X�^�[���n���̍X�V����
 *
 * @param   game			
 * @param   move			�u�[�X�^�[���쐧�䃏�[�N�ւ̃|�C���^
 * @param   land_smoke		���n�����쐧�䃏�[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void BoosterLandSmoke_Update(BALLOON_GAME_PTR game, BOOSTER_MOVE *move, BOOSTER_LAND_SMOKE *land_smoke)
{
	int i;
	GFL_CLACTPOS move_pos;
	
	switch(land_smoke->seq){
	case 0:
		break;
	case 1:
		for(i = 0; i < BOOSTER_LAND_SMOKE_NUM; i++){
			land_smoke->x[i] += land_smoke->add_x[i];
			land_smoke->y[i] += -land_smoke->add_y[i];
			move_pos.x = land_smoke->x[i] / FX32_ONE;
			move_pos.y = land_smoke->y[i] / FX32_ONE;
			GFL_CLACT_WK_SetPos(land_smoke->cap[i], &move_pos, CLSYS_DEFREND_SUB);
			GFL_CLACT_WK_AddAnmFrame(land_smoke->cap[i], FX32_ONE);
		}
		land_smoke->time++;
		if(land_smoke->time > BOOSTER_LAND_SMOKE_DELETE_TIME){
			for(i = 0; i < BOOSTER_LAND_SMOKE_NUM; i++){
				GFL_CLACT_WK_SetDrawEnable(land_smoke->cap[i], FALSE);
			}
			land_smoke->seq = 0;
		}
		break;
	}
}
