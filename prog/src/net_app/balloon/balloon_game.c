//==============================================================================
/**
 * @file	balloon_game.c
 * @brief	���D�c��܂��Q�[�����C��
 * @author	matsuda
 * @date	2007.11.01(��)
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
//#include "system/window.h"
//#include "system/actor_tool.h"
#include "arc_def.h"
#include "system/sdkdef.h"
#include "system/gfl_use.h"
#include <calctool.h>
#include "system/bmp_winframe.h"
#include "infowin/infowin.h"



//==============================================================================
//	�萔��`
//==============================================================================
///�T�[�o�[�o�[�W�������M�O�̓������ԍ�
#define BALLOON_INIT_TIMING_NO			(211)

///Vram�]���}�l�[�W���[�^�X�N��
#define BALLOON_VRAM_TRANSFER_TASK_NUM	(64)

//--------------------------------------------------------------
//	�J�����ݒ�
//--------------------------------------------------------------
#define BALLOON_CAMERA_PERSPWAY		(FX_GET_ROTA_NUM(22))
#define BALLOON_CAMERA_NEAR			( FX32_ONE )
#define BALLOON_CAMERA_FAR			( FX32_ONE * 900 )
#define BALLOON_CAMERA_TX			( 0 )		/// target
#define BALLOON_CAMERA_TY			( 0x1881e )
#define BALLOON_CAMERA_TZ			( 0 )
#define BALLOON_CAMERA_PX			( 0 )		/// pos
#define BALLOON_CAMERA_PY			( BALLOON_CAMERA_TY )
#define BALLOON_CAMERA_PZ			( FX32_ONE * 5)

///�J�����̒����_�܂ł̋���
#define BALLOON_CAMERA_DISTANCE		(0x7b << FX32_SHIFT)

///3D���f���p�̃J�����̒����_�܂ł̋���
#define MODEL_3D_CAMERA_DISTANCE		(80 << FX32_SHIFT)

///�J�����A���O��
#if WB_FIX
static const CAMERA_ANGLE BalloonCameraAngle = {
	FX_GET_ROTA_NUM(0), FX_GET_ROTA_NUM(0), FX_GET_ROTA_NUM(0),
};
#endif

//--------------------------------------------------------------
//	�p�C�v���f���ݒ�
//--------------------------------------------------------------
#define PIPE_AIR_X		(FX32_CONST(0))
#define PIPE_AIR_Y		(FX32_CONST(-25))
#define PIPE_AIR_Z		(FX32_CONST(0))
#define PIPE_AIR_SCALE	(FX32_CONST(1.00f))

//--------------------------------------------------------------
//	������f���ݒ�
//--------------------------------------------------------------
#define DAIZA_X			(FX32_CONST(0))
#define DAIZA_Y			(FX32_CONST(-25))
#define DAIZA_Z			(FX32_CONST(0))
#define DAIZA_SCALE		(FX32_CONST(1.00f))

//--------------------------------------------------------------
//	BMP�E�B���h�E
//--------------------------------------------------------------
///BMP�E�B���h�ECGX�G���A�J�n�ʒu(�I�t�Z�b�g)
#define BMPWIN_CGX_START			((0x8000 - 0x2000) / 32)

#define BMPWIN_TALK_COLOR			(0xe)
#define BMPWIN_TALK_CGX_OFFSET		(BMPWIN_CGX_START)
#define BMPWIN_TALK_CGX_SIZE		(20 * 4)	//(0x900 / 32)

#define BMPWIN_DEBUG_COLOR			(BMPWIN_TALK_COLOR)
#define BMPWIN_DEBUG_CGX_OFFSET		(BMPWIN_TALK_CGX_OFFSET + BMPWIN_TALK_CGX_SIZE)
#define BMPWIN_DEBUG_CGX_SIZE		(20 * 4)

//-- �T�u��ʗpBMP�E�B���h�E --//
///�T�u��ʃV�X�e���E�B���h�E�̃p���b�g�ԍ�
#define BMPWIN_SUB_SYSTEM_WIN_COLOR		(6)
///�T�u���BMP�E�B���h�E�̃p���b�g�ԍ�
#define BMPWIN_SUB_TALK_COLOR			(5)	//(0xd)
///�T�u��ʗp�V�X�e���t�H���g�J���[�w��
#define	BMPWIN_SUB_STR_PRINTCOLOR	( GF_PRINTCOLOR_MAKE( 1, 2, 15 ) )		// �t�H���g�J���[�F��
///�T�u��ʗp�V�X�e���t�H���g�J���[�w��(VIP�p)
#define	BMPWIN_SUB_STR_PRINTCOLOR_VIP	( GF_PRINTCOLOR_MAKE( 7, 8, 15 ) )

///�T�u���BMP�E�B���h�ECGX�G���A�J�n�ʒu(�I�t�Z�b�g)
#define BMPWIN_SUB_CGX_START		((0x8000 - 0x2000) / 32)

#define BMPWIN_SUB_CGX_NAME_SIZE		(10 * 2)
#define BMPWIN_SUB_CGX_OFFSET_NAME_1	(BMPWIN_SUB_CGX_START + BMPWIN_SUB_CGX_NAME_SIZE)
#define BMPWIN_SUB_CGX_OFFSET_NAME_2	(BMPWIN_SUB_CGX_OFFSET_NAME_1 + BMPWIN_SUB_CGX_NAME_SIZE)
#define BMPWIN_SUB_CGX_OFFSET_NAME_3	(BMPWIN_SUB_CGX_OFFSET_NAME_2 + BMPWIN_SUB_CGX_NAME_SIZE)
#define BMPWIN_SUB_CGX_OFFSET_SYSTEM	(BMPWIN_SUB_CGX_OFFSET_NAME_3 + BMPWIN_SUB_CGX_NAME_SIZE)
#define BMPWIN_SUB_CGX_OFFSET_SUB_TALK	(BMPWIN_SUB_CGX_OFFSET_SYSTEM + MENU_WIN_CGX_SIZ)

//--------------------------------------------------------------
//	���o�[
//--------------------------------------------------------------
#define INFOWIN_COLOR			(0xf)


//--------------------------------------------------------------
//	CL_ACT�p�̒萔��`
//--------------------------------------------------------------
///���C��	OAM�Ǘ��̈�E�J�n
#define BALLOON_OAM_START_MAIN			(0)
///���C��	OAM�Ǘ��̈�E�I��
#define BALLOON_OAM_END_MAIN				(128)
///���C��	�A�t�B���Ǘ��̈�E�J�n
#define BALLOON_OAM_AFFINE_START_MAIN		(0)
///���C��	�A�t�B���Ǘ��̈�E�I��
#define BALLOON_OAM_AFFINE_END_MAIN		(32)
///�T�u	OAM�Ǘ��̈�E�J�n
#define BALLOON_OAM_START_SUB				(0)
///�T�u	OAM�Ǘ��̈�E�I��
#define BALLOON_OAM_END_SUB				(128)
///�T�u �A�t�B���Ǘ��̈�E�J�n
#define BALLOON_OAM_AFFINE_START_SUB		(0)
///�T�u	�A�t�B���Ǘ��̈�E�I��
#define BALLOON_OAM_AFFINE_END_SUB		(32)

///�L�����}�l�[�W���F�L�����N�^ID�Ǘ���(���ʁ{�����)
#define BALLOON_CHAR_MAX					(48 + 48)
///�L�����}�l�[�W���F���C����ʃT�C�Y(byte�P��)
#define BALLOON_CHAR_VRAMSIZE_MAIN		(1024 * 0x40)	//64K
///�L�����}�l�[�W���F�T�u��ʃT�C�Y(byte�P��)
#define BALLOON_CHAR_VRAMSIZE_SUB			(512 * 0x20)	//32K

///���C����ʁ{�T�u��ʂŎg�p����A�N�^�[����
#define BALLOON_ACTOR_MAX					(96 + 128)	//���C����� + �T�u���

///OBJ�Ŏg�p����p���b�g�{��(���ʁ{�����)
#define BALLOON_OAM_PLTT_MAX				(16 + 16)

///�]�����[�h 3D = 0 main = 1 sub = 2 main/sub = 3
#define BALLOON_OAM_VRAM_TRANS			(3)

///OAM���\�[�X�F�L�����o�^�ő吔(���C����� + �T�u���)
#define BALLOON_OAMRESOURCE_CHAR_MAX		(BALLOON_CHAR_MAX)
///OAM���\�[�X�F�p���b�g�o�^�ő吔(���C����� + �T�u���)
#define BALLOON_OAMRESOURCE_PLTT_MAX		(BALLOON_OAM_PLTT_MAX)
///OAM���\�[�X�F�Z���o�^�ő吔
#define BALLOON_OAMRESOURCE_CELL_MAX		(64)
///OAM���\�[�X�F�Z���A�j���o�^�ő吔
#define BALLOON_OAMRESOURCE_CELLANM_MAX	(64)
///OAM���\�[�X�F�}���`�Z���o�^�ő吔
#define BALLOON_OAMRESOURCE_MCELL_MAX		(2)
///OAM���\�[�X�F�}���`�Z���A�j���o�^�ő吔
#define BALLOON_OAMRESOURCE_MCELLANM_MAX	(2)

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
///OBJ�p���b�g�FPFD�Ǘ��{��(�p���b�g�P��)
#define BALLOON_MAIN_OBJPAL_NUM				(16 - 2)	//-2 = �ʐM�A�C�R���{���[�J���C�Y�p
///OBJ�p���b�g�FPFD�Ǘ��{��(�J���[�P��)
#define BALLOON_MAIN_OBJPAL_COLOR_NUM		(BALLOON_MAIN_OBJPAL_NUM * 16)
///OBJ�p���b�g�F�o�C�g�T�C�Y
#define BALLOON_MAIN_OBJPAL_SIZE			(BALLOON_MAIN_OBJPAL_COLOR_NUM * sizeof(u16))
///BG�p���b�g�F�o�C�g�T�C�Y
#define BALLOON_MAIN_BGPAL_SIZE			((16 - 1) * 0x20)	//-1 = ���o�[

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
///�T�u���BG�̃X�N���[���N���A�R�[�h
#define SUB_BG_CLEAR_CODE		(0)

///���DBG�̃p���b�g�Ŏg�p����p���b�g�{��
#define FUSEN_BG_LOAD_SIZE		(0x20 * 10)
///���DBG�̃p���b�g�W�J�J�n�ʒu
#define FUSEN_BG_LOAD_POS		(6)
///���DBG�̃p���b�g�ǂݍ��݊J�n�ʒu
#define FUSEN_BG_READ_POS		(6)

//--------------------------------------------------------------
//	FONTOAM
//--------------------------------------------------------------
///�t�H���gOAM�̍ő吔
#define BALLOON_FONTOAM_MAX_MAIN	(BALLOON_COUNTER_MAX * BALLOON_COUNTER_KETA_MAX + BALLOON_COUNTER_KETA_MAX + 1)	//�J�E���^�[�{�_�~�[�J�E���^�[�{"CC"

//--------------------------------------------------------------
//	�o�b�t�@
//--------------------------------------------------------------
///������o�b�t�@�̃T�C�Y
#define BALLOON_MESSAGE_BUF_SIZE	(2*160)		///<�b��łP�U�O������

//--------------------------------------------------------------
//	�p�[�e�B�N��
//--------------------------------------------------------------
///�퓬�p�[�e�B�N���̃J�����j�A�ݒ�
#define BP_NEAR			(FX32_ONE)
///�퓬�p�[�e�B�N���̃J�����t�@�[�ݒ�
#define BP_FAR			(FX32_ONE * 900)


//==============================================================================
//	�\���̒�`
//==============================================================================


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static void _BalloonLocal_SystemUpdate(BALLOON_GAME_WORK *game);
static void BalloonVBlank(GFL_TCB *tcb, void *work);
static void Balloon_3D_Init(int heap_id);
static void BalloonSimpleSetUp(void);
static void Balloon_3D_Exit(void);
static void BalloonUpdate(GFL_TCB* tcb, void *work);
static void BalloonSys_VramBankSet(void);
static void BalloonSys_DefaultBmpWinAdd(BALLOON_GAME_WORK *game);
static void BalloonParticleInit(BALLOON_GAME_PTR game);
static void BalloonParticleExit(BALLOON_GAME_PTR game);
static void BalloonDefaultOBJSet(BALLOON_GAME_WORK *game, ARCHANDLE *hdl);
static void BalloonDefaultOBJDel(BALLOON_GAME_WORK *game);
static void BalloonDefaultOBJSet_Sub(BALLOON_GAME_WORK *game, ARCHANDLE *hdl);
static void BalloonDefaultOBJDel_Sub(BALLOON_GAME_WORK *game);
static void BalloonDefaultBGSet(BALLOON_GAME_WORK *game, ARCHANDLE *hdl);
static void BalloonDefaultBGDel(BALLOON_GAME_WORK *game);
static void BalloonDefaultBGSet_Sub(BALLOON_GAME_WORK *game, ARCHANDLE *hdl);
static void BalloonDefaultBGDel_Sub(BALLOON_GAME_WORK *game);
static void BalloonDefault3DSet(BALLOON_GAME_WORK *game, ARCHANDLE *hdl);
static void BalloonDefault3DDel(BALLOON_GAME_WORK *game);
static void Balloon_CameraInit(BALLOON_GAME_WORK *game);
static void SetCamPosByTarget_Dist_Ang(VecFx32 *pos, u16 angle_x, u16 angle_y, u16 angle_z, fx32 distance, VecFx32 *target);
static void Balloon_CameraExit(BALLOON_GAME_WORK *game);
BOOL Balloon_ServerCheck(BALLOON_GAME_PTR game);
void BalloonSio_RecvBufferSet(BALLOON_GAME_PTR game, int net_id, const BALLOON_SIO_PLAY_WORK *src);
BALLOON_SIO_PLAY_WORK * BalloonSio_RecvBufferReadPtrGet(BALLOON_GAME_PTR game, int net_id);
static BOOL BalloonSio_ImportantRecvBufferSet(
	BALLOON_GAME_PTR game, int net_id, const BALLOON_SIO_PLAY_WORK *src);
void BalloonSio_SendBufferSet(BALLOON_GAME_PTR game, const BALLOON_SIO_PLAY_WORK *src);
static BOOL BalloonSio_ImportantSendBufferSet(
	BALLOON_GAME_PTR game, const BALLOON_SIO_PLAY_WORK *src);
BALLOON_SIO_PLAY_WORK * BalloonSio_SendBufferReadPtrGet(BALLOON_GAME_PTR game);
static int BalloonSio_CommSend(BALLOON_GAME_PTR game);
static void BalloonSio_CommRecv(BALLOON_GAME_PTR game);
int Balloon_BalloonNoGet(BALLOON_GAME_PTR game);
int Balloon_ServerSelect(BALLOON_GAME_PTR game);
static BOOL Timing_AnswerSend(BALLOON_GAME_PTR game);
static int Timing_Recv(BALLOON_GAME_PTR game, int server_timing_no);
static int ServerPlay_Start(BALLOON_GAME_PTR game, BALLOON_SERVER_WORK *server);
static int ServerPlay_Game(BALLOON_GAME_PTR game, BALLOON_SERVER_WORK *server);
static int ServerPlay_Finish(BALLOON_GAME_PTR game, BALLOON_SERVER_WORK *server);
static BOOL Server_GamePlayingManage(BALLOON_GAME_PTR game);
static void GameStartMessageDraw(BALLOON_GAME_WORK *game);
static void GameStartMessageErase(BALLOON_GAME_WORK *game);
static void PlayerName_Draw(BALLOON_GAME_WORK *game);
static void PlayerName_Erase(BALLOON_GAME_PTR game, int all_erase);
BOOL Air3D_EntryAdd(BALLOON_GAME_PTR game, int air);
void Air3D_Delete(BALLOON_GAME_PTR game, int air_no, int air_size);
void Air3D_Update(BALLOON_GAME_PTR game);
void Air3D_Draw(BALLOON_GAME_PTR game);
#if WB_TEMP_FIX
static void Debug_CameraMove(GFL_G3D_CAMERA * camera);
#endif

///BalloonMainSeqTbl�̖߂�l�Ƃ��Ďg�p
enum{
	ASRET_CONTINUE,		///<����ێ�
	ASRET_NEXT,			///<���̃V�[�P���X��
	ASRET_SELECT_SEQ,	///<select_seq���[�N�ɓ����Ă���V�[�P���X�֐i��
	ASRET_END,			///<�I��
};

#if 0
///���Z�͕��僁�C���֐��̃V�[�P���X�e�[�u��
static int (* const BalloonMainSeqTbl[])(BALLOON_GAME_WORK *game, BALLOON_LOCAL_WORK *local) = {
	BalloonSeq_Init,
	BalloonSeq_Practice_FastTalk,
	BalloonSeq_FastTalk,
	BalloonSeq_TurnStart,
	BalloonSeq_UserSelect,
	BalloonSeq_SelectBreeder,
	BalloonSeq_PokemonIn,
	BalloonSeq_WazaEffectBefore,
	BalloonSeq_WazaEffect,
	BalloonSeq_AppealReview,
	BalloonSeq_PokemonOut,
	BalloonSeq_NextBreeder,
	BalloonSeq_AllReviewAfter,
	BalloonSeq_JudgeComment,
	BalloonSeq_TurnEnd,
	BalloonSeq_End,
};
///BalloonMainSeqTbl�̃V�[�P���X�ԍ�	��BalloonMainSeqTbl�ƕ��т𓯂��ɂ��Ă������ƁI�I
enum{
	ASEQ_INIT,
	ASEQ_PRACTICE_FAST_TALK,
	ASEQ_FAST_TALK,
	ASEQ_TURN_START,
	ASEQ_USER_SELECT,
	ASEQ_SELECT_BREEDER,
	ASEQ_POKEMON_IN,
	ASEQ_WAZAEFFECT_BEFORE,
	ASEQ_WAZAEFFECT,
	ASEQ_APPEAL_REVIEW,
	ASEQ_POKEMON_OUT,
	ASEQ_NEXT_BREEDER,
	ASEQ_ALL_REVIEW_AFTER,
	ASEQ_JUDGE_COMMENT,
	ASEQ_TURN_END,
	ASEQ_END,
};
#endif

//==============================================================================
//	�f�[�^
//==============================================================================
///VRAM�o���N�ݒ�
static const GFL_DISP_VRAM BalloonVramSetTable = {
	GX_VRAM_BG_128_B,				// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
	GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
	GX_VRAM_OBJ_64_E,				// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
	GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
	GX_VRAM_TEX_0_A,				// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_01_FG,			// �e�N�X�`���p���b�g�X���b�g
	GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
};

///�T�[�o�[�̃Q�[���i�s�֐��e�[�u��
static int (* const ServerPlayFuncTbl[])(BALLOON_GAME_PTR, BALLOON_SERVER_WORK *) = {
	ServerPlay_Start,
	ServerPlay_Game,
	ServerPlay_Finish,
};

///�v���C���[�̕\���ʒu�֌W�F�Q���l��4�l(�l�b�gID���Ⴂ���@��������������̂Ńl�b�gID�Ɓ��ł͂Ȃ�)
ALIGN4 static const u8 BalloonPlayerSort[][WFLBY_MINIGAME_MAX] = {
	{0, },			//player_max���̂܂܂ŃA�N�Z�X�ł���悤��1origin���ׂ̈̃_�~�[
	{0, },			//�Q���l��1�l(�f�o�b�O�p)
	{0, 1, },		//�Q���l��2�l	���A��
	{0, 2, 1,}	,	//�Q���l��3�l	���A���A�E
	{0, 3, 1, 2},	//�Q���l��4�l	���A���A��A�E
};

///�v���C���[�̕\���ʒu�֌W�F�Q���l��4�l(�l�b�gID���Ⴂ���@��������������̂Ńl�b�gID�Ɓ��ł͂Ȃ�)
ALIGN4 static const u8 BalloonPlayerSortBmpNamePosTbl[][WFLBY_MINIGAME_MAX] = {
	{0, },	//player_max���̂܂܂ŃA�N�Z�X�ł���悤��1origin���ׂ̈̃_�~�[
	{0, },			//�Q���l��1�l(�f�o�b�O�p)
	{0, BALLOON_BMPWIN_NAME_1, },		//�Q���l��2�l	���A��
	{0, BALLOON_BMPWIN_NAME_3, BALLOON_BMPWIN_NAME_2,}	,	//�Q���l��3�l	���A���A�E
	{0, BALLOON_BMPWIN_NAME_3, BALLOON_BMPWIN_NAME_1, BALLOON_BMPWIN_NAME_2},	//�Q���l��4�l	���A���A��A�E
};

//==============================================================================
//	CLACT�p�f�[�^
//==============================================================================
#if WB_FIX
static	const TCATS_OAM_INIT BalloonTcats = {
	BALLOON_OAM_START_MAIN, BALLOON_OAM_END_MAIN,
	BALLOON_OAM_AFFINE_START_MAIN, BALLOON_OAM_AFFINE_END_MAIN,
	BALLOON_OAM_START_SUB, BALLOON_OAM_END_SUB,
	BALLOON_OAM_AFFINE_START_SUB, BALLOON_OAM_AFFINE_END_SUB,
};

static	const TCATS_CHAR_MANAGER_MAKE BalloonCcmm = {
	BALLOON_CHAR_MAX,
	BALLOON_CHAR_VRAMSIZE_MAIN,
	BALLOON_CHAR_VRAMSIZE_SUB,
	GX_OBJVRAMMODE_CHAR_1D_128K,	//64K	�J�E���g�_�E���G�t�F�N�g��128KOBJ�Ȃ̂ō��킹��
	GX_OBJVRAMMODE_CHAR_1D_32K
};

static const TCATS_RESOURCE_NUM_LIST BalloonResourceList = {
	BALLOON_OAMRESOURCE_CHAR_MAX,
	BALLOON_OAMRESOURCE_PLTT_MAX,
	BALLOON_OAMRESOURCE_CELL_MAX,
	BALLOON_OAMRESOURCE_CELLANM_MAX,
	BALLOON_OAMRESOURCE_MCELL_MAX,
	BALLOON_OAMRESOURCE_MCELLANM_MAX,
};
#endif

//--------------------------------------------------------------
//	�v���C���[�ʒu�ɂ���ĕς���O���t�B�b�N�f�[�^
//--------------------------------------------------------------
#if WB_FIX
static const struct{
	u32 pipe_mdl_id;
	u32 air_s_mdl_id;
	u32 air_m_mdl_id;
	u32 air_l_mdl_id;
	u32 air_sp_mdl_id;
}PlayerPosGraphicID[] = {
	{//1P
		SONANS_PIPE_R_NSBMD,
		SONANS_BALL1R_NSBMD,
		SONANS_BALL2R_NSBMD,
		SONANS_BALL3R_NSBMD,
		SONANS_BALL4R_NSBMD,
	},
	{//2P
		SONANS_PIPE_B_NSBMD,
		SONANS_BALL1B_NSBMD,
		SONANS_BALL2B_NSBMD,
		SONANS_BALL3B_NSBMD,
		SONANS_BALL4B_NSBMD,
	},
	{//3P
		SONANS_PIPE_Y_NSBMD,
		SONANS_BALL1Y_NSBMD,
		SONANS_BALL2Y_NSBMD,
		SONANS_BALL3Y_NSBMD,
		SONANS_BALL4Y_NSBMD,
	},
	{//4P
		SONANS_PIPE_G_NSBMD,
		SONANS_BALL1G_NSBMD,
		SONANS_BALL2G_NSBMD,
		SONANS_BALL3G_NSBMD,
		SONANS_BALL4G_NSBMD,
	},
};
#else
enum{
	G3DRES_BALL1R_BMD,
	G3DRES_BALL2R_BMD,
	G3DRES_BALL3R_BMD,
	G3DRES_BALL4R_BMD,

	G3DRES_BALL1B_BMD,
	G3DRES_BALL2B_BMD,
	G3DRES_BALL3B_BMD,
	G3DRES_BALL4B_BMD,

	G3DRES_BALL1Y_BMD,
	G3DRES_BALL2Y_BMD,
	G3DRES_BALL3Y_BMD,
	G3DRES_BALL4Y_BMD,

	G3DRES_BALL1G_BMD,
	G3DRES_BALL2G_BMD,
	G3DRES_BALL3G_BMD,
	G3DRES_BALL4G_BMD,

	G3DRES_BALL1_BCA,
	G3DRES_BALL2_BCA,
	G3DRES_BALL3_BCA,
	G3DRES_BALL4_BCA,

	G3DRES_DAIZA,
};

//�ǂݍ���3D���\�[�X
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	//1P
	{ ARCID_BALLOON_GRA, SONANS_BALL1R_NSBMD, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_BALL2R_NSBMD, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_BALL3R_NSBMD, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_BALL4R_NSBMD, GFL_G3D_UTIL_RESARC },

	//2P
	{ ARCID_BALLOON_GRA, SONANS_BALL1B_NSBMD, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_BALL2B_NSBMD, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_BALL3B_NSBMD, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_BALL4B_NSBMD, GFL_G3D_UTIL_RESARC },

	//3P
	{ ARCID_BALLOON_GRA, SONANS_BALL1Y_NSBMD, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_BALL2Y_NSBMD, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_BALL3Y_NSBMD, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_BALL4Y_NSBMD, GFL_G3D_UTIL_RESARC },

	//4P
	{ ARCID_BALLOON_GRA, SONANS_BALL1G_NSBMD, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_BALL2G_NSBMD, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_BALL3G_NSBMD, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_BALL4G_NSBMD, GFL_G3D_UTIL_RESARC },

	//�{�[���A�j��
	{ ARCID_BALLOON_GRA, SONANS_BALL1_NSBCA, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_BALL2_NSBCA, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_BALL3_NSBCA, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_BALL4_NSBCA, GFL_G3D_UTIL_RESARC },

	//���
	{ ARCID_BALLOON_GRA, SONANS_DAIZA_NSBMD, GFL_G3D_UTIL_RESARC },
};

//3D�A�j��
static const GFL_G3D_UTIL_ANM g3Dutil_anm1Tbl[] = {
	{
		G3DRES_BALL1_BCA, 	//�A�j�����\�[�XID
		0,					//�A�j���f�[�^ID(���\�[�X����INDEX)
	},
	{
		G3DRES_BALL2_BCA, 	//�A�j�����\�[�XID
		0,					//�A�j���f�[�^ID(���\�[�X����INDEX)
	},
	{
		G3DRES_BALL3_BCA, 	//�A�j�����\�[�XID
		0,					//�A�j���f�[�^ID(���\�[�X����INDEX)
	},
	{
		G3DRES_BALL4_BCA, 	//�A�j�����\�[�XID
		0,					//�A�j���f�[�^ID(���\�[�X����INDEX)
	},
};

///�I�u�W�F�C���f�b�N�X	��g3Dutil_objTbl�ƕ��т𓯂��ɂ��Ă������ƁI
enum{
	G3DOBJ_BALL1R_BMD,
	G3DOBJ_BALL2R_BMD,
	G3DOBJ_BALL3R_BMD,
	G3DOBJ_BALL4R_BMD,
	G3DOBJ_BALL1B_BMD,
	G3DOBJ_BALL2B_BMD,
	G3DOBJ_BALL3B_BMD,
	G3DOBJ_BALL4B_BMD,
	G3DOBJ_BALL1Y_BMD,
	G3DOBJ_BALL2Y_BMD,
	G3DOBJ_BALL3Y_BMD,
	G3DOBJ_BALL4Y_BMD,
	G3DOBJ_BALL1G_BMD,
	G3DOBJ_BALL2G_BMD,
	G3DOBJ_BALL3G_BMD,
	G3DOBJ_BALL4G_BMD,
	G3DOBJ_DAIZA,
};
//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
	//1P
	{
		G3DRES_BALL1R_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_BALL1R_BMD, 			//�e�N�X�`�����\�[�XID
		g3Dutil_anm1Tbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anm1Tbl),	//�A�j�����\�[�X��
	},
	{
		G3DRES_BALL2R_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_BALL2R_BMD, 			//�e�N�X�`�����\�[�XID
		g3Dutil_anm1Tbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anm1Tbl),	//�A�j�����\�[�X��
	},
	{
		G3DRES_BALL3R_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_BALL3R_BMD, 			//�e�N�X�`�����\�[�XID
		g3Dutil_anm1Tbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anm1Tbl),	//�A�j�����\�[�X��
	},
	{
		G3DRES_BALL4R_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_BALL4R_BMD, 			//�e�N�X�`�����\�[�XID
		g3Dutil_anm1Tbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anm1Tbl),	//�A�j�����\�[�X��
	},
	
	//2P
	{
		G3DRES_BALL1B_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_BALL1B_BMD, 			//�e�N�X�`�����\�[�XID
		g3Dutil_anm1Tbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anm1Tbl),	//�A�j�����\�[�X��
	},
	{
		G3DRES_BALL2B_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_BALL2B_BMD, 			//�e�N�X�`�����\�[�XID
		g3Dutil_anm1Tbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anm1Tbl),	//�A�j�����\�[�X��
	},
	{
		G3DRES_BALL3B_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_BALL3B_BMD, 			//�e�N�X�`�����\�[�XID
		g3Dutil_anm1Tbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anm1Tbl),	//�A�j�����\�[�X��
	},
	{
		G3DRES_BALL4B_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_BALL4B_BMD, 			//�e�N�X�`�����\�[�XID
		g3Dutil_anm1Tbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anm1Tbl),	//�A�j�����\�[�X��
	},

	//3P
	{
		G3DRES_BALL1Y_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_BALL1Y_BMD, 			//�e�N�X�`�����\�[�XID
		g3Dutil_anm1Tbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anm1Tbl),	//�A�j�����\�[�X��
	},
	{
		G3DRES_BALL2Y_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_BALL2Y_BMD, 			//�e�N�X�`�����\�[�XID
		g3Dutil_anm1Tbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anm1Tbl),	//�A�j�����\�[�X��
	},
	{
		G3DRES_BALL3Y_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_BALL3Y_BMD, 			//�e�N�X�`�����\�[�XID
		g3Dutil_anm1Tbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anm1Tbl),	//�A�j�����\�[�X��
	},
	{
		G3DRES_BALL4Y_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_BALL4Y_BMD, 			//�e�N�X�`�����\�[�XID
		g3Dutil_anm1Tbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anm1Tbl),	//�A�j�����\�[�X��
	},

	//4P
	{
		G3DRES_BALL1G_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_BALL1G_BMD, 			//�e�N�X�`�����\�[�XID
		g3Dutil_anm1Tbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anm1Tbl),	//�A�j�����\�[�X��
	},
	{
		G3DRES_BALL2G_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_BALL2G_BMD, 			//�e�N�X�`�����\�[�XID
		g3Dutil_anm1Tbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anm1Tbl),	//�A�j�����\�[�X��
	},
	{
		G3DRES_BALL3G_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_BALL3G_BMD, 			//�e�N�X�`�����\�[�XID
		g3Dutil_anm1Tbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anm1Tbl),	//�A�j�����\�[�X��
	},
	{
		G3DRES_BALL4G_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_BALL4G_BMD, 			//�e�N�X�`�����\�[�XID
		g3Dutil_anm1Tbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anm1Tbl),	//�A�j�����\�[�X��
	},

	//���
	{
		G3DRES_DAIZA, 				//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_DAIZA, 				//�e�N�X�`�����\�[�XID
		NULL,						//�A�j���e�[�u��(�����w��̂���)
		0,							//�A�j�����\�[�X��
	},
};

static const GFL_G3D_UTIL_SETUP g3Dutil_setup = {
	g3Dutil_resTbl,				//���\�[�X�e�[�u��
	NELEMS(g3Dutil_resTbl),		//���\�[�X��
	g3Dutil_objTbl,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(g3Dutil_objTbl),		//�I�u�W�F�N�g��
};

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
enum{
	G3DRES_PIPE_R_BMD = 0,
	G3DRES_PIPE_B_BMD,
	G3DRES_PIPE_Y_BMD,
	G3DRES_PIPE_G_BMD,
};

//�ǂݍ���3D���\�[�X�@�p�C�v���f���̂�(�v���C���[�̈ʒu�ɂ���ēǂݍ��ނ��̂�ς���̂ŕʌɗp��
static const GFL_G3D_UTIL_RES g3Dutil_Pipe_resTbl[] = {
	{ ARCID_BALLOON_GRA, SONANS_PIPE_R_NSBMD, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_PIPE_B_NSBMD, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_PIPE_Y_NSBMD, GFL_G3D_UTIL_RESARC },
	{ ARCID_BALLOON_GRA, SONANS_PIPE_G_NSBMD, GFL_G3D_UTIL_RESARC },
};

//3D�I�u�W�F�N�g�ݒ�e�[�u���@�p�C�v�̂�
static const GFL_G3D_UTIL_OBJ g3Dutil_Pipe_objTbl[] = {
	{
		G3DRES_PIPE_R_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_PIPE_R_BMD, 			//�e�N�X�`�����\�[�XID
		NULL, 						//�A�j���e�[�u��(�����w��̂���)
		0,							//�A�j�����\�[�X��
	},
	{
		G3DRES_PIPE_B_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_PIPE_B_BMD, 			//�e�N�X�`�����\�[�XID
		NULL, 						//�A�j���e�[�u��(�����w��̂���)
		0,							//�A�j�����\�[�X��
	},
	{
		G3DRES_PIPE_Y_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_PIPE_Y_BMD, 			//�e�N�X�`�����\�[�XID
		NULL, 						//�A�j���e�[�u��(�����w��̂���)
		0,							//�A�j�����\�[�X��
	},
	{
		G3DRES_PIPE_G_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_PIPE_G_BMD, 			//�e�N�X�`�����\�[�XID
		NULL, 						//�A�j���e�[�u��(�����w��̂���)
		0,							//�A�j�����\�[�X��
	},
};

static const GFL_G3D_UTIL_SETUP g3Dutil_Pipe_setup = {
	g3Dutil_Pipe_resTbl,		//���\�[�X�e�[�u��
	1,							//���\�[�X��
	g3Dutil_Pipe_objTbl,		//�I�u�W�F�N�g�ݒ�e�[�u��
	1,							//�I�u�W�F�N�g��
};


#define G3DUTIL_RESCOUNT	(NELEMS(g3Dutil_resTbl) + NELEMS(g3Dutil_Pipe_resTbl))
#define G3DUTIL_OBJCOUNT	(NELEMS(g3Dutil_objTbl) + NELEMS(g3Dutil_Pipe_objTbl))

#endif


//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F������
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
GFL_PROC_RESULT BalloonGameProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	BALLOON_GAME_WORK *game;
	int i, s;
	
//	sys_HBlankIntrStop();	//HBlank���荞�ݒ�~

	GFL_DISP_GX_SetVisibleControlDirect(0);		//�SBG&OBJ�̕\��OFF
	GFL_DISP_GXS_SetVisibleControlDirect(0);
	GX_SetVisiblePlane(0);
	GXS_SetVisiblePlane(0);
	GX_SetVisibleWnd(GX_WNDMASK_NONE);
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);
	G2_SetBlendAlpha(GX_BLEND_PLANEMASK_BG0, GX_BLEND_ALL, 16, 16);
	G2S_SetBlendAlpha(BLD_PLANE_1, BLD_PLANE_2, BLD_ALPHA_1, BLD_ALPHA_2);
	
	game = GFL_PROC_AllocWork(proc, sizeof(BALLOON_GAME_WORK), HEAPID_BALLOON );
	GFL_STD_MemClear(game, sizeof(BALLOON_GAME_WORK));
	for(i = 0; i < PIPE_AIR_AIR_MAX; i++){
		for(s = 0;s < PIPE_3D_AIR_TYPE_MAX; s++){
			game->pipe_air.air[i][s].enable_anm_index = 0xffff;
		}
	}
    
    //TCB�V�X�e���쐬
    game->tcb_work = GFL_HEAP_AllocClearMemory(HEAPID_BALLOON, GFL_TCB_CalcSystemWorkSize( 64 ));
    game->tcbsys = GFL_TCB_Init(64, game->tcb_work);
    
#if WB_FIX
    // �A���P�[�^�쐬
	sys_InitAllocator(&game->allocator, HEAPID_BALLOON, 32 );
#endif

	Balloon_3D_Init(HEAPID_BALLOON);
	game->g3Dutil = GFL_G3D_UTIL_Create(G3DUTIL_RESCOUNT, G3DUTIL_OBJCOUNT, HEAPID_BALLOON);

	game->bsw = pwk;
#ifdef PM_DEBUG
	if(game->bsw->debug_offline == FALSE){
		CommCommandBalloonInitialize(game);
	}
	else{
		if(GFL_UI_KEY_GetCont() & PAD_BUTTON_L){
			game->bsw->raregame_type = MNGM_RAREGAME_BALLOON_FINE;
		}
		else if(GFL_UI_KEY_GetCont() & PAD_BUTTON_R){
			game->bsw->raregame_type = MNGM_RAREGAME_BALLOON_THICK;
		}
	}
#else
	CommCommandBalloonInitialize(game);
#endif

	//�T�[�o�[�S�������߂�
//	Balloon_ServerSelect(game);

	//�p���b�g�t�F�[�h�V�X�e���쐬
	game->pfd = PaletteFadeInit(HEAPID_BALLOON);
	PaletteTrans_AutoSet(game->pfd, TRUE);
	PaletteFadeWorkAllocSet(game->pfd, FADE_MAIN_BG, BALLOON_MAIN_BGPAL_SIZE, HEAPID_BALLOON);
	PaletteFadeWorkAllocSet(game->pfd, FADE_SUB_BG, 0x200, HEAPID_BALLOON);
	PaletteFadeWorkAllocSet(game->pfd, FADE_MAIN_OBJ, BALLOON_MAIN_OBJPAL_SIZE, HEAPID_BALLOON);
	PaletteFadeWorkAllocSet(game->pfd, FADE_SUB_OBJ, 0x200, HEAPID_BALLOON);
	
	GFL_BG_Init(HEAPID_BALLOON);
	GFL_BMPWIN_Init(HEAPID_BALLOON);

#if WB_FIX
	initVramTransferManagerHeap(BALLOON_VRAM_TRANSFER_TASK_NUM, HEAPID_BALLOON);
#endif

	//VRAM���蓖�Đݒ�
	BalloonSys_VramBankSet();

#if WB_FIX
	// �^�b�`�p�l���V�X�e��������
	InitTPSystem();
	InitTPNoBuff(4);
#endif

	//3D
	Balloon_CameraInit(game);
	
	//�A�N�^�[�V�X�e���쐬
#if WB_FIX
	game->csp=CATS_AllocMemory(HEAPID_BALLOON);
	CATS_SystemInit(game->csp,&BalloonTcats,&BalloonCcmm,BALLOON_OAM_PLTT_MAX);
	//�ʐM�A�C�R���p�ɃL�������p���b�g����
	CLACT_U_WmIcon_SetReserveAreaCharManager(NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_64K);
	CLACT_U_WmIcon_SetReserveAreaPlttManager(NNS_G2D_VRAM_TYPE_2DMAIN);
	game->crp=CATS_ResourceCreate(game->csp);
	CATS_ClactSetInit(game->csp, game->crp, BALLOON_ACTOR_MAX);
	CATS_ResourceManagerInit(game->csp,game->crp,&BalloonResourceList);
	CLACT_U_SetSubSurfaceMatrix(CATS_EasyRenderGet(game->csp), 0, BALLOON_SUB_ACTOR_DISTANCE);
#else
	{
		GFL_CLSYS_INIT clsys_init = GFL_CLSYSINIT_DEF_DIVSCREEN;
		
		clsys_init.oamst_main = GFL_CLSYS_OAMMAN_INTERVAL;	//�ʐM�A�C�R���̕�
		clsys_init.oamnum_main = 128-GFL_CLSYS_OAMMAN_INTERVAL;
		clsys_init.tr_cell = 32;	//�Z��Vram�]���Ǘ���
		clsys_init.CGR_RegisterMax = BALLOON_OAMRESOURCE_CHAR_MAX;
		clsys_init.CELL_RegisterMax = BALLOON_OAMRESOURCE_CELL_MAX;
		GFL_CLACT_SYS_Create(&clsys_init, &BalloonVramSetTable, HEAPID_BALLOON);
		
		game->clunit = GFL_CLACT_UNIT_Create(128+128, 0, HEAPID_BALLOON);
		GFL_CLACT_UNIT_SetDefaultRend(game->clunit);

		game->plttslot = PLTTSLOT_Init(HEAPID_BALLOON, 16, 16);
	}
#endif

	BalloonParticleInit(game);	//�p�[�e�B�N��������


	//���b�Z�[�W�}�l�[�W���쐬
	game->msgman = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_balloon_dat, 
		HEAPID_BALLOON);
	game->fontHandle = GFL_FONT_Create(ARCID_FONT, NARC_font_large_nftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_BALLOON);
	game->printQue = PRINTSYS_QUE_Create(HEAPID_BALLOON);

	game->wordset = WORDSET_Create(HEAPID_BALLOON);	//�P��o�b�t�@�쐬
	game->msg_buf = GFL_STR_CreateBuffer(BALLOON_MESSAGE_BUF_SIZE, HEAPID_BALLOON);//������o�b�t�@�쐬

	//�t�H���gOAM�V�X�e���쐬
	game->bsp = BmpOam_Init(HEAPID_BALLOON, game->clunit);

	Booster_Init(game, &game->booster);

	{
		ARCHANDLE* hdl;
	
		//�n���h���I�[�v��
		hdl  = GFL_ARC_OpenDataHandle(ARCID_BALLOON_GRA,  HEAPID_BALLOON); 
	
		//�풓BG�Z�b�g
		BalloonDefaultBGSet(game, hdl);
		BalloonDefaultBGSet_Sub(game, hdl);

		//BMP�E�B���h�E�ǉ�
		BalloonSys_DefaultBmpWinAdd(game);

		//�풓OBJ�Z�b�g
		BalloonDefaultOBJSet(game, hdl);
		BalloonDefaultOBJSet_Sub(game, hdl);

		//3D���f���Z�b�g
		BalloonDefault3DSet(game, hdl);
		
		//�n���h������
		GFL_ARC_CloseDataHandle( hdl );
	}

	//�V�X�e���t�H���g�p���b�g�F���C�����
	PaletteWorkSet_Arc(game->pfd, ARCID_FONT, NARC_font_default_nclr, 
		HEAPID_BALLOON, FADE_MAIN_BG, 0x20, BMPWIN_TALK_COLOR * 16);
	//�V�X�e���t�H���g�p���b�g�F�T�u���
	PaletteWorkSet_Arc(game->pfd, ARCID_FONT, NARC_font_default_nclr, 
		HEAPID_BALLOON, FADE_SUB_BG, 0x20, BMPWIN_SUB_TALK_COLOR * 16);

	//�v���C���[���`��
	PlayerName_Draw(game);
	PlayerName_Erase(game, FALSE);
	//�������\��
	GameStartMessageDraw(game);

#if WB_TEMP_FIX
	WirelessIconEasy();	//�ʐM�A�C�R��
#endif
	
	//�\�[�i���X�����ݒ�
	game->sns = Sonas_Init(game);
	
	// �P�x�ύX�Z�b�g
#if WB_TEMP_FIX
	WIPE_SYS_Start(WIPE_PATTERN_WMS, WIPE_TYPE_DOORIN, WIPE_TYPE_DOORIN, WIPE_FADE_BLACK, 
		WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BALLOON);
#endif

	game->update_tcb = GFL_TCB_AddTask(game->tcbsys, BalloonUpdate, game, TCBPRI_BALLOON_UPDATE);

	//���C����ʐݒ�
	GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_SUB);
	GFL_DISP_SetDispOn();

	GFL_DISP_SetDispOn();
	GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);
	GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);

	//�T�E���h�f�[�^���[�h(�R���e�X�g)
//	Snd_DataSetByScene( SND_SCENE_CONTEST, SEQ_CON_TEST, 1 );

#if WB_TEMP_FIX
	//���b�Z�[�W�ݒ�
	{
		MsgPrintAutoFlagSet(MSG_AUTO_ON);
		MsgPrintSkipFlagSet(MSG_SKIP_OFF);
		MsgPrintTouchPanelFlagSet(MSG_TP_OFF);
	}
#endif

#if WB_TEMP_FIX
	//�~�j�Q�[�����ʃJ�E���g�_�E���V�X�e��
	game->mgcount = MNGM_COUNT_Init(CATS_GetClactSetPtr(game->crp), HEAPID_BALLOON);
	//�~�j�Q�[�����ʃJ�E���g�_�E���V�X�e���Ŏg�p���Ă���p���b�g��PFD�ɂ��W�J
	{
		int pal_pos;
		pal_pos = MNGM_PalNoGet(game->mgcount);
		PaletteWorkSet_VramCopy(game->pfd, FADE_MAIN_OBJ, pal_pos*16, MNGM_COUNT_PALNUM*0x20);
	}
#endif

	game->vintr_tcb = GFUser_VIntr_CreateTCB(BalloonVBlank, game, 200);
	
	if ( game->bsw->vchat ){
		GFL_NET_DWC_StartVChat( HEAPID_BALLOON );
	}
	
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F���C��
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
GFL_PROC_RESULT BalloonGameProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	BALLOON_GAME_WORK * game  = mywk;
	int ret;
	enum{
		SEQ_IN,
		SEQ_INIT_TIMING,
		SEQ_INIT_TIMING_WAIT,
		SEQ_SERVER_VERSION_SEND,
		SEQ_SERVER_VERSION_RECV_WAIT,
		SEQ_MAIN,
		SEQ_OUT,
	};

	// �ʐM�ؒf�`�F�b�N
	if( game->bsw->dis_error == TRUE ){

		switch( game->bsw->dis_error_seq ){
		case 0:
			// �^�����ɂ���
			if( WIPE_SYS_EndCheck() == TRUE ){
				WIPE_SYS_ExeEnd();
			}
			WIPE_SetBrightnessFadeOut(WIPE_FADE_BLACK);


			// WND�j��
			GX_SetVisibleWnd(GX_WNDMASK_NONE);

			game->bsw->dis_error_seq ++;
			break;

		default:
		case 1:
			// �܂��͒ʐM�ؒf
		#if WB_TEMP_FIX
			if( MNGM_ERROR_DisconnectWait( &game->bsw->entry_param ) == TRUE ){
		#else
			if(TRUE){
		#endif
				// �I��������
				return GFL_PROC_RES_FINISH;
			}
			break;
		}

		_BalloonLocal_SystemUpdate(game);
		return GFL_PROC_RES_CONTINUE;
	}
	
	switch( *seq ){
	case SEQ_IN:
		if(WIPE_SYS_EndCheck() == TRUE){
			(*seq)++;

		#ifdef PM_DEBUG
			if(game->bsw->debug_offline == TRUE){
				Balloon_ServerSelect(game);		//�T�[�o�[�S�������߂�
				(*seq) = SEQ_MAIN;
				break;
			}
		#endif
		}
		break;
	
	case SEQ_INIT_TIMING:			//�T�[�o�[�o�[�W�����擾�O�̓������
		GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), BALLOON_INIT_TIMING_NO);
		(*seq)++;
		break;
	case SEQ_INIT_TIMING_WAIT:
		if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),BALLOON_INIT_TIMING_NO) == TRUE){
			(*seq)++;
		}
		break;
	case SEQ_SERVER_VERSION_SEND:	//�T�[�o�[�o�[�W�������M
		if(Send_CommServerVersion(game) == TRUE){
			(*seq)++;
		}
		break;
	case SEQ_SERVER_VERSION_RECV_WAIT:	//�T�[�o�[�o�[�W������M�҂�
		if(game->recv_server_version_cnt >= game->bsw->player_max){
			Balloon_ServerSelect(game);		//�T�[�o�[�S�������߂�
			(*seq)++;
		}
		break;

	case SEQ_MAIN:
		if(game->game_end == TRUE){
			//WIPE�ŃE�B���h�E�@�\���g����̂Ń_�~�[�J�E���^�[�𐶐�
			CounterDummyNumber_ActorCreate(game);
			
			WIPE_SYS_Start(WIPE_PATTERN_WMS, WIPE_TYPE_DOOROUT, WIPE_TYPE_DOOROUT, 
				WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BALLOON);
			(*seq)++;
		}

		//�T�[�o�[����V�[�P���X���s
		Server_GamePlayingManage(game);

		switch(game->countdown_eff){
		case BALLOON_COUNTDOWN_TOUCHPEN_DEMO_INIT:
			//�E�B���h�E�ݒ�	WIPE�Ŏg���Ă���̂ł����Őݒ�
			GX_SetVisibleWnd(GX_WNDMASK_W1);
			G2_SetWnd1Position(4, 192-32+8, 8*9+4, 192-8);	//�J�E���^�[�E�B���h�E���͂�
			G2_SetWnd1InsidePlane(GX_WND_PLANEMASK_OBJ, FALSE);	//�J�E���^�[OBJ�����\������Ȃ��悤�ݒ�
			G2_SetWndOutsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | 
				GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, TRUE);

			//�{���̃J�E���^�[���\�������̂Ń_�~�[�͍폜
			CounterDummyNumber_ActorDelete(game);

			game->countdown_eff = BALLOON_COUNTDOWN_TOUCHPEN_DEMO;
			//break;
		case BALLOON_COUNTDOWN_TOUCHPEN_DEMO:
			if(BalloonTool_TouchPenDemoMove(game, &game->pen) == TRUE){
				game->pen.demo_end = TRUE;
				game->countdown_eff = BALLOON_COUNTDOWN_NULL;
			}
			break;
			
		case BALLOON_COUNTDOWN_START:
		#if WB_TEMP_FIX
			MNGM_COUNT_StartStart(game->mgcount, game->tcbsys);
		#endif
			game->countdown_eff = BALLOON_COUNTDOWN_START_WAIT;
			break;
		case BALLOON_COUNTDOWN_START_WAIT:
		#if WB_TEMP_FIX
			if(MNGM_COUNT_Wait(game->mgcount) == TRUE){
		#else
			if(TRUE){
		#endif
				game->game_start = TRUE;
				game->booster.stop = FALSE;
				game->countdown_eff = BALLOON_COUNTDOWN_START_END;

				GameStartMessageErase(game);
				PlayerName_Erase(game, TRUE);
			}
			break;
		case BALLOON_COUNTDOWN_TIMEUP:
		#if WB_TEMP_FIX
			MNGM_COUNT_StartTimeUp(game->mgcount, game->tcbsys);
		#endif
			OS_TPrintf("�����̓��ꂽ��C�̍��v = %d\n", game->my_total_air);
			game->countdown_eff = BALLOON_COUNTDOWN_TIMEUP_WAIT;
			break;
		case BALLOON_COUNTDOWN_TIMEUP_WAIT:
		#if WB_TEMP_FIX
			if(MNGM_COUNT_Wait(game->mgcount) == TRUE){
		#else
			if(TRUE){
		#endif
				game->countdown_eff = BALLOON_COUNTDOWN_TIMEUP_END;
			}
			break;
		}
		
		//�������N�G�X�g������ꍇ�͕Ԏ���Ԃ�
		Timing_AnswerSend(game);
		
		//��M�o�b�t�@�m�F
		BalloonSio_CommRecv(game);

		if(game->game_finish == TRUE){
			break;
		}
		
		//���M�o�b�t�@�ɗ��܂��Ă���f�[�^�𑗐M
		if(BalloonSio_CommSend(game) == TRUE){
			game->sio_wait = BALLOON_SIO_SEND_WAIT;
		}

		#ifdef PM_DEBUG
			if(game->bsw->debug_offline == TRUE){
				if(GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT){
					if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_A){
						*seq = SEQ_OUT;
					}
				}
			}
		#endif
		
		if(game->time == BALLOON_NAME_ERASE_TIME){
			//PlayerName_Erase(game, TRUE);
		}
		
		if(game->game_start == TRUE && game->time <= BALLOON_GAME_PLAY_TIME){
			game->time++;
		}
		break;

	case SEQ_OUT:
		if(WIPE_SYS_EndCheck() == TRUE){
			return GFL_PROC_RES_FINISH;
		}
		break;
	}

#if WB_TEMP_FIX
	Debug_CameraMove(game->camera);
#endif
	game->main_frame++;
	
	_BalloonLocal_SystemUpdate(game);
	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F�I��
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
GFL_PROC_RESULT BalloonGameProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	BALLOON_GAME_WORK * game = mywk;
	int i;

	//�������������ʉ�ʗp���[�N�փZ�b�g
	game->bsw->result_param.balloon = game->exploded_count;
	
	//�~�j�Q�[�����ʃJ�E���g�_�E���V�X�e���폜
#if WB_TEMP_FIX
	MNGM_COUNT_Exit(game->mgcount);
#endif

	Air_ActorAllDelete(game);
	Exploded_AllDelete(game);
	
	GFL_TCB_DeleteTask(game->vintr_tcb);
//	sys_HBlankIntrStop();	//HBlank���荞�ݒ�~

	BalloonParticleExit(game);

	//�풓OBJ�폜
	BalloonDefaultOBJDel(game);
	BalloonDefaultOBJDel_Sub(game);
	//�풓BG�폜
	BalloonDefaultBGDel(game);
	BalloonDefaultBGDel_Sub(game);
	//�풓3D���f���폜
	BalloonDefault3DDel(game);
	
	//�\�[�i���X�폜
	Sonans_Exit(game, game->sns);
	
	//BMP�J��
	for(i = 0; i < BALLOON_BMPWIN_MAX; i++){
		GFL_BMPWIN_Delete(game->printUtil[i].win);
	}
	
	//���C�����BG�폜
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
	GFL_BG_FreeBGControl(BALLOON_FRAME_WIN );
	GFL_BG_FreeBGControl(BALLOON_FRAME_EFF );
	GFL_BG_FreeBGControl(BALLOON_FRAME_BACK );
	//�T�u���BG�폜
	GFL_BG_FreeBGControl(GFL_BG_FRAME0_S );
	GFL_BG_FreeBGControl(GFL_BG_FRAME1_S );
	GFL_BG_FreeBGControl(GFL_BG_FRAME2_S );
	GFL_BG_FreeBGControl(GFL_BG_FRAME3_S );

	//�t�H���gOAM�V�X�e���폜
	BmpOam_Exit(game->bsp);

	//�A�N�^�[�V�X�e���폜
	GFL_CLACT_UNIT_Delete(game->clunit);
	GFL_CLACT_SYS_Delete();
	PLTTSLOT_Exit(game->plttslot);
	
#if WB_TEMP_FIX
	//Vram�]���}�l�[�W���[�폜
	DellVramTransferManager();
#endif

	//�p���b�g�t�F�[�h�V�X�e���폜
	PaletteFadeWorkAllocFree(game->pfd, FADE_MAIN_BG);
	PaletteFadeWorkAllocFree(game->pfd, FADE_SUB_BG);
	PaletteFadeWorkAllocFree(game->pfd, FADE_MAIN_OBJ);
	PaletteFadeWorkAllocFree(game->pfd, FADE_SUB_OBJ);
	PaletteFadeFree(game->pfd);

	//���b�Z�[�W�}�l�[�W���̍폜
	GFL_STR_DeleteBuffer(game->msg_buf);
	WORDSET_Delete(game->wordset);
	GFL_MSG_Delete(game->msgman);
	GFL_FONT_Delete(game->fontHandle);
	PRINTSYS_QUE_Delete(game->printQue);

	//BGL�J��
	GFL_BG_Exit();
	GFL_BMPWIN_Exit();

	//�J�����폜
	Balloon_CameraExit(game);
	
	GFL_TCB_DeleteTask(game->update_tcb);

	GX_SetVisibleWnd(GX_WNDMASK_NONE);
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);

	//simple_3DBGExit();
	Balloon_3D_Exit();
	
	//TCB�V�X�e���폜
	GFL_TCB_Exit( game->tcbsys );
	GFL_HEAP_FreeMemory(game->tcb_work);

#if WB_TEMP_FIX
	StopTP();		//�^�b�`�p�l���̏I��
#endif

	GFL_PROC_FreeWork(proc);				// ���[�N�J��
	
#if WB_TEMP_FIX
	MsgPrintSkipFlagSet(MSG_SKIP_OFF);
	MsgPrintAutoFlagSet(MSG_AUTO_OFF);
	MsgPrintTouchPanelFlagSet(MSG_TP_OFF);
#endif

#if WB_TEMP_FIX
	WirelessIconEasyEnd();
#endif
	
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief   ���C�����[�v�ŉ񂷃V�X�e���n�̍X�V����
 *
 * @param   game		
 */
//--------------------------------------------------------------
static void _BalloonLocal_SystemUpdate(BALLOON_GAME_WORK *game)
{
	BOOL que_ret;
	int i, s;
	
	GFL_TCB_Main(game->tcbsys);
	
	que_ret = PRINTSYS_QUE_Main(game->printQue );
	for(i = 0; i < BALLOON_BMPWIN_MAX; i++){
		PRINT_UTIL_Trans(&game->printUtil[i], game->printQue);
	}
	for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++){
		for(s = 0; s < BALLOON_COUNTER_MAX; s++){
			Balloon_FontOamBmpTrans(game, &game->counter.fontact[i][s]);
		}
	}
	Balloon_FontOamBmpTrans(game, &game->counter.fontact_cc);
	for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++){
		Balloon_FontOamBmpTrans(game, &game->counter.fontact_dummy[i]);
	}
}

//--------------------------------------------------------------
/**
 * @brief	VBLANK�֐�
 *
 * @param	work	���Z�͕���Ǘ����[�N�ւ̃|�C���^
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BalloonVBlank(GFL_TCB *tcb, void *work)
{
	BALLOON_GAME_WORK *game = work;
	
	Sonans_VBlank(game, game->sns);
	
	GFL_CLACT_SYS_VBlankFunc();
	
#if WB_TEMP_FIX
	DoVramTransferManager();	// Vram�]���}�l�[�W���[���s
#endif
	PaletteFadeTrans(game->pfd);
	
	if(game->bst.bg_on_req == TRUE){
		GFL_BG_SetVisible(BALLOON_SUBFRAME_BALLOON, VISIBLE_ON);
		game->bst.bg_on_req = 0;
	}
	else if(game->bst.bg_off_req == TRUE){
		GFL_BG_SetVisible(BALLOON_SUBFRAME_BALLOON, VISIBLE_OFF);
		game->bst.bg_off_req = 0;
	}
	
	GFL_BG_VBlankFunc();
}

//--------------------------------------------------------------
/**
 * @brief   �R���e�X�g�p3DBG�������֐�
 * 
 * @param   �q�[�vID
 */
//--------------------------------------------------------------
static void Balloon_3D_Init(int heap_id)
{
	GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX128K, GFL_G3D_VMANLNK, GFL_G3D_PLT32K,
						0x1000, heap_id, BalloonSimpleSetUp);
	GFL_G3D_SetSystemSwapBufferMode(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);
}

static void BalloonSimpleSetUp(void)
{
	// �R�c�g�p�ʂ̐ݒ�(�\�����v���C�I���e�B�[)
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
    G2_SetBG0Priority(1);

	// �e��`�惂�[�h�̐ݒ�(�V�F�[�h���A���`�G�C���A�X��������)
    G3X_SetShading( GX_SHADING_TOON );
    G3X_AntiAlias( TRUE );
	G3X_AlphaTest( FALSE, 0 );	// �A���t�@�e�X�g�@�@�I�t
	G3X_AlphaBlend( TRUE );		// �A���t�@�u�����h�@�I��
	G3X_EdgeMarking( FALSE );
	G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

	// �N���A�J���[�̐ݒ�
    G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog

	// �r���[�|�[�g�̐ݒ�
    G3_ViewPort(0, 0, 255, 191);
}

//--------------------------------------------------------------
/**
 * @brief   �R���e�X�g�p3DBG�I������
 */
//--------------------------------------------------------------
static void Balloon_3D_Exit(void)
{
	GFL_G3D_Exit();
}

//--------------------------------------------------------------
/**
 * @brief   �J�����쐬
 *
 * @param   game		�Q�[�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Balloon_CameraInit(BALLOON_GAME_WORK *game)
{
	VecFx32	target = { BALLOON_CAMERA_TX, BALLOON_CAMERA_TY, BALLOON_CAMERA_TZ };
	VecFx32	pos	   = { BALLOON_CAMERA_PX, BALLOON_CAMERA_PY, BALLOON_CAMERA_PZ };
	VecFx32	camup  = { 0, FX32_ONE, 0 };
	fx32		fovySin;			// ����p/2�̐������Ƃ����l
    fx32		fovyCos;			// ����p/2�̗]�����Ƃ����l
	fx32	height, width;			// �����ƕ�
	fx32 aspect;
	
#if WB_FIX
	game->camera = GFC_AllocCamera( HEAPID_BALLOON );

	GFC_InitCameraTDA(&target, BALLOON_CAMERA_DISTANCE, &BalloonCameraAngle,
						BALLOON_CAMERA_PERSPWAY, GF_CAMERA_ORTHO, FALSE, game->camera);

	GFC_SetCameraClip( BALLOON_CAMERA_NEAR, BALLOON_CAMERA_FAR, game->camera);

	GFC_AttachCamera(game->camera);
#else
	fovySin  = FX_SinIdx( BALLOON_CAMERA_PERSPWAY );
	fovyCos  = FX_CosIdx( BALLOON_CAMERA_PERSPWAY );
	aspect = FX32_ONE * 4 / 3;
	height = FX_Mul(FX_Div(fovySin, fovyCos), BALLOON_CAMERA_DISTANCE);
	width  = FX_Mul(height, aspect);
	SetCamPosByTarget_Dist_Ang(&pos, 
		FX_GET_ROTA_NUM(0), FX_GET_ROTA_NUM(0), FX_GET_ROTA_NUM(0), 
		BALLOON_CAMERA_DISTANCE, &target);
	game->camera = GFL_G3D_CAMERA_CreateOrtho(height, -height, -width, width, 
			BALLOON_CAMERA_NEAR, BALLOON_CAMERA_FAR, 0, 
			&pos, &camup, &target, HEAPID_BALLOON);
	GFL_G3D_CAMERA_Switching(game->camera);
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief	�J�����ʒu�𒍎��_�A�����A�A���O������Z�o����
 * 
 * �v���`�i��camera.c����ڐA
 */
//---------------------------------------------------------------------------
static void SetCamPosByTarget_Dist_Ang(VecFx32 *pos, u16 angle_x, u16 angle_y, u16 angle_z, fx32 distance, VecFx32 *target)
{
	u16 f_angle_x;
	
	//�p�˒n�ʂ���̌X���ɕϊ�
	f_angle_x = -angle_x;
	/*== �J�������W�����߂� ==*/
	pos->x = FX_Mul( FX_Mul( FX_SinIdx(angle_y), distance ), FX_CosIdx( angle_x ) );
	
	pos->z = FX_Mul(FX_Mul(FX_CosIdx(angle_y), distance), FX_CosIdx(angle_x));
	
	pos->y = FX_Mul( FX_SinIdx( f_angle_x ), distance );

	/*== ���_����̋����ɂ��� ==*/
	VEC_Add(pos, target, pos);
}

//--------------------------------------------------------------
/**
 * @brief   �J�������
 *
 * @param   game		�Q�[�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Balloon_CameraExit(BALLOON_GAME_WORK *game)
{
#if WB_FIX
	GFC_FreeCamera(game->camera);
	GFC_FreeCamera(game->camera_3d);
#else
	GFL_G3D_CAMERA_Delete(game->camera);
#endif
}

//--------------------------------------------------------------
/**
 * @brief   ���C�����[�v�̍Ō�ɍs���V�X�e���֘A�̍X�V����
 *
 * @param   tcb			TCB�ւ̃|�C���^
 * @param   work		game
 */
//--------------------------------------------------------------
static void BalloonUpdate(GFL_TCB* tcb, void *work)
{
	BALLOON_GAME_WORK *game = work;
	VecFx32 scale_vec, trans;
	MtxFx33 rot;
	
	scale_vec.x = FX32_ONE;
	scale_vec.y = FX32_ONE;
	scale_vec.z = FX32_ONE;
	
	trans.x = 0;
	trans.y = 0;
	trans.z = 0;
	
	MTX_Identity33(&rot);
	
	Balloon_CounterUpdate(game, &game->counter);

	//�u�[�X�^�[�X�V����
	Booster_Update(game, &game->booster);
	SioBooster_Update(game, &game->sio_booster);

	//��C�X�V����
	Air_Update(game);

	//���D�X�V����
	BalloonTool_BalloonUpdate(game, &game->bst);
	
	//�j��X�V����
	Exploded_Update(game);

	//��C���f���X�V����
	Air3D_Update(game);

	//���D�A�C�R���X�V����
	IconBalloon_Update(game);

	{
		//�R�c�`��J�n
		GFL_G3D_DRAW_Start();

#if WB_FIX
		GFC_AttachCamera(game->camera);
		GFC_SetCameraView(GF_CAMERA_ORTHO, game->camera); //���ˉe�ݒ�
		GFC_CameraLookAt();
#else
		GFL_G3D_DRAW_Start();
		GFL_G3D_DRAW_SetLookAt();
#endif

		// ���C�g�ƃA���r�G���g
		NNS_G3dGlbLightVector( 0, 0, -FX32_ONE, 0 );
		NNS_G3dGlbLightColor( 0, GX_RGB( 28,28,28 ) );
		NNS_G3dGlbMaterialColorDiffAmb( GX_RGB( 31,31,31 ), GX_RGB( 31,31,31 ), FALSE );
		NNS_G3dGlbMaterialColorSpecEmi( GX_RGB( 31,31,31 ), GX_RGB( 31,31,31 ), FALSE );
		
	// �ʒu�ݒ�
		NNS_G3dGlbSetBaseTrans(&trans);
		// �p�x�ݒ�
		NNS_G3dGlbSetBaseRot(&rot);
		// �X�P�[���ݒ�
		NNS_G3dGlbSetBaseScale(&scale_vec);
	
		NNS_G3dGlbFlush();
		
//		NNS_G3dGlbFlush();

		Sonans_Update(game, game->sns);

//		GFC_AttachCamera(game->camera_3d);
//		GFC_SetCameraView(GF_CAMERA_ORTHO, game->camera_3d); //���ˉe�ݒ�
//		GFC_CameraLookAt();
		// �`��
		NNS_G3dGePushMtx();
		{
		#if WB_FIX
			//���
			D3DOBJ_Draw( &game->daiza.obj );
			//�p�C�v����C�`��
			D3DOBJ_Draw( &game->pipe_air.pipe_obj );
		#else
			//���
			GFL_G3D_DRAW_DrawObject(game->daiza.g3dobj, &game->daiza.status);
			//�p�C�v����C�`��
			GFL_G3D_DRAW_DrawObject(game->pipe_air.g3dobj, &game->pipe_air.status);
		#endif
			Air3D_Draw(game);
		}
		NNS_G3dGePopMtx(1);
	}

	{//�p�[�e�B�N��
		int draw_num;

		GFL_G3D_DRAW_Start();
		
		draw_num = GFL_PTC_DrawAll();	//�p�[�e�B�N���`��
		if(draw_num > 0){
			//�p�[�e�B�N���̕`�悪�I�������̂ŁA�Ăу\�t�g�E�F�A�X�v���C�g�p�J�����ɐݒ�
			GFL_G3D_DRAW_Start();
		}

		GFL_PTC_CalcAll();	//�p�[�e�B�N���v�Z
	}

//	BattleParticle_Main();
	GFL_CLACT_SYS_Main();
	GFL_G3D_DRAW_End();
	
#if WB_TEMP_FIX
	CommErrorCheck(HEAPID_BALLOON);
#endif

#ifdef PM_DEBUG		//�|���S���̃��C���Y�I�[�o�[�`�F�b�N
	if(G3X_IsLineBufferUnderflow() != 0){
		OS_TPrintf("--------------���C���Y�I�[�o�[�����I�I----------\n");
		//GF_ASSERT(0 && "���C���Y�I�[�o�[���������܂���");
		G3X_ResetLineBufferUnderflow();
	}
	else if(G3X_GetRenderedLineCount() < 10){
		OS_TPrintf("========== ���C���Y�I�[�o�[�������������ł��c Count = %d\n", 
			G3X_GetRenderedLineCount());
	}
#endif

	INFOWIN_Update();
}

//--------------------------------------------------------------
/**
 * @brief   Vram�o���N�ݒ���s��
 */
//--------------------------------------------------------------
static void BalloonSys_VramBankSet(void)
{
	GFL_DISP_GX_SetVisibleControlDirect(0);		//�SBG&OBJ�̕\��OFF

	//VRAM�ݒ�
	{
		GFL_DISP_SetBank( &BalloonVramSetTable );//H32���]��B�T�uBG�ʂ̊g���p���b�g�Ƃ��ē��Ă���

		//VRAM�N���A
		GFL_STD_MemClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
		GFL_STD_MemClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
		GFL_STD_MemClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
		GFL_STD_MemClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
	}

	// BG SYSTEM
	{
		GFL_BG_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_1, GX_BG0_AS_3D,
		};
		GFL_BG_SetBGMode( &BGsys_data );
	}

	//���C����ʃt���[���ݒ�
	{
		GFL_BG_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME1_M	�E�B���h�E
			{
				0, 0, 0x1000, 0, GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
//				0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, 0x8000,
				GX_BG_EXTPLTT_01, BALLOON_BGPRI_WINDOW, 0, 0, FALSE
			},
			///<FRAME2_M	�G�t�F�N�g
			{
				0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, 0x4000,
				GX_BG_EXTPLTT_01, BALLOON_BGPRI_EFFECT, 0, 0, FALSE
			},
			///<FRAME3_M	�w�i
			{
				0, 0, 0x1000, 0, GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x10000, 0x8000,
				GX_BG_EXTPLTT_01, BALLOON_BGPRI_BACKGROUND, 0, 0, FALSE
			},
		};
		GFL_BG_SetBGControl(BALLOON_FRAME_WIN, &TextBgCntDat[0], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(BALLOON_FRAME_WIN );
		GFL_BG_SetScroll(BALLOON_FRAME_WIN, GFL_BG_SCROLL_X_SET, 0);
		GFL_BG_SetScroll(BALLOON_FRAME_WIN, GFL_BG_SCROLL_Y_SET, 0);
		GFL_BG_SetVisible(BALLOON_FRAME_WIN, VISIBLE_ON);
		GFL_BG_SetBGControl(BALLOON_FRAME_EFF, &TextBgCntDat[1], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(BALLOON_FRAME_EFF );
		GFL_BG_SetScroll(BALLOON_FRAME_EFF, GFL_BG_SCROLL_X_SET, 0);
		GFL_BG_SetScroll(BALLOON_FRAME_EFF, GFL_BG_SCROLL_Y_SET, 0);
		GFL_BG_SetVisible(BALLOON_FRAME_EFF, VISIBLE_ON);
		GFL_BG_SetBGControl(BALLOON_FRAME_BACK, &TextBgCntDat[2], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(BALLOON_FRAME_BACK );
		GFL_BG_SetScroll(BALLOON_FRAME_BACK, GFL_BG_SCROLL_X_SET, 0);
		GFL_BG_SetScroll(BALLOON_FRAME_BACK, GFL_BG_SCROLL_Y_SET, 0);
		GFL_BG_SetVisible(BALLOON_FRAME_BACK, VISIBLE_ON);

		G2_SetBG0Priority(BALLOON_3DBG_PRIORITY);
		GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	}

	//�T�u��ʃt���[���ݒ�
	{
		int i;
		static const GFL_BG_BGCNT_HEADER SubBgCntDat[] = {
			{//GFL_BG_FRAME0_S	�E�B���h�E
				0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x08000, 0x8000,
				GX_BG_EXTPLTT_01, BALLOON_SUBBG_WIN_PRI, 0, 0, FALSE
			},
			{//GFL_BG_FRAME1_S	�p�C�v
				0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000, 0x4000,
				GX_BG_EXTPLTT_01, BALLOON_SUBBG_PIPE_PRI, 0, 0, FALSE
			},
			{//GFL_BG_FRAME2_S	�w�i
				0, 0, 0x1000, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x4000, GX_BG_CHARBASE_0x14000, 0x4000,
				GX_BG_EXTPLTT_01, BALLOON_SUBBG_BACK_PRI, 0, 0, FALSE
			},
			{//GFL_BG_FRAME3_S	���D
				0, 0, 0x400, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
				GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x18000, 0x8000,
				GX_BG_EXTPLTT_01, BALLOON_SUBBG_BALLOON_PRI, 0, 0, FALSE
			},
		};
		
		for(i = 0; i < NELEMS(SubBgCntDat); i++){
			if(i < 3){
				GFL_BG_SetBGControl(GFL_BG_FRAME0_S + i, &SubBgCntDat[i], GFL_BG_MODE_TEXT);
			}
			else{
				GFL_BG_SetBGControl(GFL_BG_FRAME0_S + i, &SubBgCntDat[i], GFL_BG_MODE_AFFINE);
			}
			GFL_BG_SetClearCharacter( GFL_BG_FRAME0_S + i, 0x20, 0, HEAPID_BALLOON);
			GFL_BG_ClearScreen(GFL_BG_FRAME0_S + i);
			GFL_BG_SetScroll(GFL_BG_FRAME0_S + i, GFL_BG_SCROLL_X_SET, 0);
			GFL_BG_SetScroll(GFL_BG_FRAME0_S + i, GFL_BG_SCROLL_Y_SET, 0);
			GFL_BG_SetVisible(GFL_BG_FRAME0_S + i, VISIBLE_ON);
		}
		//�ŏ��͕��DBG��\��
		GFL_BG_SetVisible(BALLOON_SUBFRAME_BALLOON, VISIBLE_OFF);
	}
}

//--------------------------------------------------------------
/**
 * @brief   ����BMP�E�B���h�E��ݒ肷��
 *
 * @param   game		���Z�͊Ǘ����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void BalloonSys_DefaultBmpWinAdd(BALLOON_GAME_WORK *game)
{
	int i;
	GFL_BMPWIN *win;
	
#if WB_TEMP_FIX
	win = GFL_BMPWIN_Create(BALLOON_FRAME_WIN, 11, 0x13, 20, 4, 
		BMPWIN_TALK_COLOR, GFL_BMP_CHRAREA_GET_F);
	PRINT_UTIL_Setup(&game->printUtil[BALLOON_BMPWIN_TALK], win);
	GFL_BMPWIN_MakeScreen(game->printUtil[BALLOON_BMPWIN_TALK].win);
#endif
	
	//-- �T�u��ʗp --//
	win = GFL_BMPWIN_Create(BALLOON_SUBFRAME_WIN, 
		0x12, 1, 10, 2, BMPWIN_SUB_TALK_COLOR, GFL_BMP_CHRAREA_GET_F);
	PRINT_UTIL_Setup(&game->printUtil[BALLOON_BMPWIN_NAME_1], win);
	
	win = GFL_BMPWIN_Create(BALLOON_SUBFRAME_WIN, 
		0x15, 0xe, 10, 2, BMPWIN_SUB_TALK_COLOR, GFL_BMP_CHRAREA_GET_F);
	PRINT_UTIL_Setup(&game->printUtil[BALLOON_BMPWIN_NAME_2], win);
	
	win = GFL_BMPWIN_Create(BALLOON_SUBFRAME_WIN, 
		1, 14, 10, 2, BMPWIN_SUB_TALK_COLOR, GFL_BMP_CHRAREA_GET_F);
	PRINT_UTIL_Setup(&game->printUtil[BALLOON_BMPWIN_NAME_3], win);

	GFL_BMPWIN_MakeScreen(game->printUtil[BALLOON_BMPWIN_NAME_1].win);
	GFL_BMPWIN_MakeScreen(game->printUtil[BALLOON_BMPWIN_NAME_2].win);
	GFL_BMPWIN_MakeScreen(game->printUtil[BALLOON_BMPWIN_NAME_3].win);
	GFL_BMP_Clear(GFL_BMPWIN_GetBmp(game->printUtil[BALLOON_BMPWIN_NAME_1].win), 0xf);
	GFL_BMP_Clear(GFL_BMPWIN_GetBmp(game->printUtil[BALLOON_BMPWIN_NAME_2].win), 0xf);
	GFL_BMP_Clear(GFL_BMPWIN_GetBmp(game->printUtil[BALLOON_BMPWIN_NAME_3].win), 0xf);

	//�V�X�e���E�B���h�E
	win = GFL_BMPWIN_Create(BALLOON_SUBFRAME_WIN, 
		2, 0x13, 0x1c, 4, BMPWIN_SUB_TALK_COLOR, GFL_BMP_CHRAREA_GET_F);
	PRINT_UTIL_Setup(&game->printUtil[BALLOON_BMPWIN_SUB_TALK], win);
	GFL_BMPWIN_MakeScreen(game->printUtil[BALLOON_BMPWIN_SUB_TALK].win);
	GFL_BMP_Clear(GFL_BMPWIN_GetBmp(game->printUtil[BALLOON_BMPWIN_SUB_TALK].win), 0xf);
}

//--------------------------------------------------------------
/**
 * @brief   �Q�[���J�n���b�Z�[�W�\��
 *
 * @param   game		
 */
//--------------------------------------------------------------
static void GameStartMessageDraw(BALLOON_GAME_WORK *game)
{
	STRBUF *message;
	
	GFL_BMPWIN_MakeFrameScreen(game->printUtil[BALLOON_BMPWIN_SUB_TALK].win, 
		game->sysmenu_charno, BMPWIN_SUB_SYSTEM_WIN_COLOR);
	BmpWinFrame_Write(game->printUtil[BALLOON_BMPWIN_SUB_TALK].win, WINDOW_TRANS_OFF, 
		game->sysmenu_charno, BMPWIN_SUB_SYSTEM_WIN_COLOR);
	
	message = GFL_MSG_CreateString(game->msgman, msg_balloon_start);
	PRINT_UTIL_Print(&game->printUtil[BALLOON_BMPWIN_SUB_TALK], game->printQue, 
		0, 0, message, game->fontHandle);
	GFL_STR_DeleteBuffer(message);
}

//--------------------------------------------------------------
/**
 * @brief   �Q�[���J�n���b�Z�[�W������
 *
 * @param   game		
 */
//--------------------------------------------------------------
static void GameStartMessageErase(BALLOON_GAME_WORK *game)
{
	GFL_BG_FillScreen(BALLOON_SUBFRAME_WIN, 
		1023, 2-1, 0x13-1, 0x1c+2, 4+2, GFL_BG_SCRWRT_PALIN);
	GFL_BG_LoadScreenV_Req(BALLOON_SUBFRAME_WIN);
}

//--------------------------------------------------------------
/**
 * @brief   �v���C���[���`��
 *
 * @param   game		
 */
//--------------------------------------------------------------
static void PlayerName_Draw(BALLOON_GAME_WORK *game)
{
#if WB_TEMP_FIX
	int i, bmp_pos;
	MYSTATUS *mystatus;
	STRBUF *name;
	int current_id;
	PRINTSYS_LSB print_color;
	int dot_len, draw_x_offset;
	
#ifdef PM_DEBUG
	if(game->bsw->debug_offline == TRUE){
	}
#endif

	current_id = GFL_NET_SystemGetCurrentID();
	for(i = 0; i < game->bsw->player_max; i++){
		if(current_id != game->bsw->player_netid[i]){
		//	mystatus = CommInfoGetMyStatus(game->bsw->player_netid[i]);
			mystatus = MNGM_ENRES_PARAM_GetMystatus(
				&game->bsw->entry_param, game->bsw->player_netid[i]);
			name = MyStatus_CreateNameString(mystatus, HEAPID_BALLOON);
			bmp_pos = Balloon_NetID_to_PlayerPos(game, game->bsw->player_netid[i]);
			if(MNGM_ENRES_PARAM_GetVipFlag(&game->bsw->entry_param, game->bsw->player_netid[i]) == TRUE){
				print_color = BMPWIN_SUB_STR_PRINTCOLOR_VIP;
			}
			else{
				print_color = BMPWIN_SUB_STR_PRINTCOLOR;
			}
			dot_len = PRINTSYS_GetStrWidth(name, GFL_FONT* font/*FONT_SYSTEM*/, 0);
			draw_x_offset = 8*10/2 - dot_len/2;	//�Z���^�[��
			if(dot_len & 1){
				draw_x_offset--;
			}
			OS_TPrintf("name dot_len (bmp_pos=%d) = %d, x_offset = %d\n", bmp_pos, dot_len, draw_x_offset);
			PRINT_UTIL_PrintColor(&game->printUtil[BalloonPlayerSortBmpNamePosTbl[game->bsw->player_max][bmp_pos]], game->printQue, 
				draw_x_offset, 0, name, game->fontHandle);
			GF_STR_PrintColor(
				&game->printUtil[BalloonPlayerSortBmpNamePosTbl[game->bsw->player_max][bmp_pos]].win, 
				FONT_SYSTEM, name, draw_x_offset, 0, MSG_ALLPUT, print_color, NULL);
			GFL_HEAP_FreeMemory(name);
		}
	}
#endif
}

//--------------------------------------------------------------
/**
 * @brief   ���O����������
 *
 * @param   game		
 * @param   all_erase	TRUE:�S�����B�@FALSE:�Q���҂����Ȃ�����������
 */
//--------------------------------------------------------------
static void PlayerName_Erase(BALLOON_GAME_PTR game, int all_erase)
{
	if(all_erase == TRUE){
		//��
		GFL_BG_FillScreen(BALLOON_SUBFRAME_WIN, 1023, 0, 13, 12, 4, GFL_BG_SCRWRT_PALIN);
		//��
		GFL_BG_FillScreen(BALLOON_SUBFRAME_WIN, 1023, 0x11, 0, 12, 4, GFL_BG_SCRWRT_PALIN);
		//�E
		GFL_BG_FillScreen(BALLOON_SUBFRAME_WIN, 1023, 0x14, 13, 12, 4, GFL_BG_SCRWRT_PALIN);
	}
	else{
		switch(game->bsw->player_max){
		case 2:
			//��
			GFL_BG_FillScreen(BALLOON_SUBFRAME_WIN, 
				1023, 0, 13, 12, 4, GFL_BG_SCRWRT_PALIN);
			//�E
			GFL_BG_FillScreen(BALLOON_SUBFRAME_WIN, 
				1023, 0x14, 13, 12, 4, GFL_BG_SCRWRT_PALIN);
			break;
		case 3:
			//��
			GFL_BG_FillScreen(BALLOON_SUBFRAME_WIN, 
				1023, 0x11, 0, 12, 4, GFL_BG_SCRWRT_PALIN);
			break;
		}
	}
	
	GFL_BG_LoadScreenV_Req(BALLOON_SUBFRAME_WIN);
}

//--------------------------------------------------------------
/**
 * @brief   ���Z����p�p�[�e�B�N��������
 */
//--------------------------------------------------------------
static void BalloonParticleInit(BALLOON_GAME_PTR game)
{
	void *heap;
	GFL_G3D_CAMERA * camera_ptr;
	void *resource;
	fx32 near, far;
	
	//�p�[�e�B�N���V�X�e�����[�N������
	GFL_PTC_Init(HEAPID_BALLOON);
	
	heap = GFL_HEAP_AllocMemory(HEAPID_BALLOON, PARTICLE_LIB_HEAP_SIZE);
	game->ptc = GFL_PTC_Create(heap, PARTICLE_LIB_HEAP_SIZE, TRUE, HEAPID_BALLOON);
	camera_ptr = GFL_PTC_GetCameraPtr(game->ptc);
	near = BP_NEAR;
	far = BP_FAR;
	GFL_G3D_CAMERA_SetNear(camera_ptr, &near);
	GFL_G3D_CAMERA_GetFar(camera_ptr, &far);

	//���\�[�X�ǂݍ��݁��o�^
	resource = GFL_PTC_LoadArcResource(
		ARCID_BALLOON_PARTICLE, NARC_balloon_particle_balloon_spa, HEAPID_BALLOON);
	GFL_PTC_SetResource(game->ptc, resource, TRUE, NULL);
}

//--------------------------------------------------------------
/**
 * @brief   �p�[�e�B�N���V�X�e���폜
 *
 * @param   game		
 */
//--------------------------------------------------------------
static void BalloonParticleExit(BALLOON_GAME_PTR game)
{
	void *heap;
	
	heap = GFL_PTC_GetHeapPtr(game->ptc);
	GFL_PTC_Exit();
	GFL_HEAP_FreeMemory(heap);
}

//--------------------------------------------------------------
/**
 * @brief   �G�~�b�^����
 *
 * @param   game		
 * @param   emit_no		
 *
 * @retval  
 */
//--------------------------------------------------------------
void BalloonParticle_EmitAdd(BALLOON_GAME_PTR game, int emit_no)
{
	switch(emit_no){
	case BALLOON_HINOMARU:
		GFL_PTC_CreateEmitterCallback(game->ptc, BALLOON_HINOMARU, NULL, game);
		break;
	case BALLOON_PUSYU:
		GFL_PTC_CreateEmitterCallback(game->ptc, BALLOON_PUSYU, NULL, game);
		break;
	case BALLOON_PUSYU2:
		GFL_PTC_CreateEmitterCallback(game->ptc, BALLOON_PUSYU2, NULL, game);
		break;
	default:
		GF_ASSERT(0);
		return;
	}
}

//--------------------------------------------------------------
/**
 * @brief   ���Z�͕���Ŏg�p�����{�I�ȏ풓OBJ�̓o�^���s��
 * @param   game		���Z�͕���Ǘ����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void BalloonDefaultOBJSet(BALLOON_GAME_WORK *game, ARCHANDLE *hdl)
{
	int palno;
	
	//-- �J�E���^�[ --//
	{
		STRBUF *str0, *str1;
		int i;
		
		game->pltt_id[PLTTID_COUNTER] = PLTTSLOT_ResourceSet(game->plttslot, hdl, 
			MINI_FUSEN_CCOBJ_NCLR, CLSYS_DRAW_MAIN, 1, HEAPID_BALLOON);
		palno = GFL_CLGRP_PLTT_GetAddr(game->pltt_id[PLTTID_COUNTER], CLSYS_DRAW_MAIN) / 0x20;
		PaletteWorkSet_VramCopy(game->pfd, FADE_MAIN_OBJ, palno*16, 1*0x20);

		str0 = GFL_MSG_CreateString(game->msgman, msg_balloon_counter001);
		str1 = GFL_MSG_CreateString(game->msgman, msg_balloon_counter002);
		for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++){
			BalloonTool_FontOamCreate(game->printQue, game->bsp, 
				&game->counter.fontact[i][BALLOON_COUNTER_0], str0, game->fontHandle, 
				COUNTER_FONT_COLOR, game->pltt_id[PLTTID_COUNTER], 0, 
				COUNTER_BASE_X + COUNTER_X_SPACE * i, COUNTER_Y, FALSE,
				BALLOON_BGPRI_COUNTER, BALLOON_SOFTPRI_COUNTER, 2*5);
			BalloonTool_FontOamCreate(game->printQue, game->bsp, 
				&game->counter.fontact[i][BALLOON_COUNTER_1], str1, game->fontHandle, 
				COUNTER_FONT_COLOR, game->pltt_id[PLTTID_COUNTER], 0, 
				COUNTER_BASE_X + COUNTER_X_SPACE * i, COUNTER_Y, FALSE,
				BALLOON_BGPRI_COUNTER, BALLOON_SOFTPRI_COUNTER, 2*5);
		}
		GFL_STR_DeleteBuffer(str0);
		GFL_STR_DeleteBuffer(str1);
		
		//CC
		str0 = GFL_MSG_CreateString(game->msgman, msg_balloon_cc);
		BalloonTool_FontOamCreate(game->printQue, game->bsp, 
			&game->counter.fontact_cc, str0, game->fontHandle, 
			COUNTER_FONT_COLOR, game->pltt_id[PLTTID_COUNTER], 0, 
			COUNTER_BASE_X + COUNTER_X_SPACE * BALLOON_COUNTER_KETA_MAX, COUNTER_Y, FALSE,
			BALLOON_BGPRI_DUMMY_COUNTER, BALLOON_SOFTPRI_COUNTER, 2);
		GFL_STR_DeleteBuffer(str0);
	
		Balloon_CounterPosUpdate(&game->counter);
		
		//�J�E���^�[�_�~�[�A�N�^�[(WIPE���E�B���h�E�@�\���g���Ă���̂ňꎞ�I�ɐ���)
		CounterDummyNumber_ActorCreate(game);

		//BG�̉��ɕ~���A�N�^�[
		game->pltt_id[PLTTID_COUNTER_WIN] = PLTTSLOT_ResourceSet(game->plttslot, hdl, 
			MINI_FUSEN_CCOBJ_NCLR, CLSYS_DRAW_MAIN, 1, HEAPID_BALLOON);
		palno = GFL_CLGRP_PLTT_GetAddr(game->pltt_id[PLTTID_COUNTER_WIN], CLSYS_DRAW_MAIN) / 0x20;
		PaletteWorkSet_VramCopy(game->pfd, FADE_MAIN_OBJ, palno*16, 1*0x20);
		game->cgr_id[CHARID_COUNTER_WIN] = GFL_CLGRP_CGR_Register(
			hdl, MINI_FUSEN_CCOBJ_NCGR, FALSE, CLSYS_DRAW_MAIN, HEAPID_BALLOON);
		game->cell_id[CELLID_COUNTER_WIN] = GFL_CLGRP_CELLANIM_Register(
			hdl, MINI_FUSEN_CCOBJ_NCER, MINI_FUSEN_CCOBJ_NANR, HEAPID_BALLOON);
		game->counter.win_cap = CounterWindow_ActorCreate(game);
	}

	//�^�b�`�y��
	{
		ARCHANDLE *hdl_pen;
		
		hdl_pen = GFL_ARC_OpenDataHandle(ARCID_WLMNGM_TOOL_GRA, HEAPID_BALLOON);
		
		game->pltt_id[PLTTID_TOUCH_PEN] = PLTTSLOT_ResourceSet(game->plttslot, hdl_pen, 
			NARC_wlmngm_tool_touchpen_NCLR, CLSYS_DRAW_MAIN, 1, HEAPID_BALLOON);
		palno = GFL_CLGRP_PLTT_GetAddr(game->pltt_id[PLTTID_TOUCH_PEN], CLSYS_DRAW_MAIN) / 0x20;
		PaletteWorkSet_VramCopy(game->pfd, FADE_MAIN_OBJ, palno*16, 1*0x20);
		game->cgr_id[CHARID_TOUCH_PEN] = GFL_CLGRP_CGR_Register(hdl_pen, 
			NARC_wlmngm_tool_touchpen_NCGR, FALSE, CLSYS_DRAW_MAIN, HEAPID_BALLOON);
		game->cell_id[CELLID_TOUCH_PEN] = GFL_CLGRP_CELLANIM_Register(hdl_pen, 
			NARC_wlmngm_tool_touchpen_NCER, NARC_wlmngm_tool_touchpen_NANR, HEAPID_BALLOON);

		GFL_ARC_CloseDataHandle(hdl_pen);

		game->pen.cap = TouchPen_ActorCreate(game);
	}
}

//--------------------------------------------------------------
/**
 * @brief   ���Z�͕���Ŏg�p�����{�I�ȏ풓OBJ�̍폜���s��
 * @param   game		���Z�͕���Ǘ����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void BalloonDefaultOBJDel(BALLOON_GAME_WORK *game)
{
	int i;
	
	for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++){
		Balloon_FontOamDelete(&game->counter.fontact[i][BALLOON_COUNTER_0]);
		Balloon_FontOamDelete(&game->counter.fontact[i][BALLOON_COUNTER_1]);
	}
	Balloon_FontOamDelete(&game->counter.fontact_cc);
	CounterDummyNumber_ActorDelete(game);

	CounterWindow_ActorDelete(game, game->counter.win_cap);
	TouchPen_ActorDelete(game, game->pen.cap);
}

//--------------------------------------------------------------
/**
 * @brief   �T�u���OBJ�F���Z�͕���Ŏg�p�����{�I�ȏ풓OBJ�̓o�^���s��
 * @param   game		���Z�͕���Ǘ����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void BalloonDefaultOBJSet_Sub(BALLOON_GAME_WORK *game, ARCHANDLE *hdl)
{
	int palno;
	
	//�풓OBJ�p���b�g���[�h
	game->pltt_id[PLTTID_SUB_OBJ_COMMON] = PLTTSLOT_ResourceSet(
		game->plttslot, hdl, MINI_FUSEN_OBJ_NCLR,
		CLSYS_DRAW_SUB, BALLOON_SUB_COMMON_PAL_NUM, HEAPID_BALLOON);
	palno = GFL_CLGRP_PLTT_GetAddr(game->pltt_id[PLTTID_SUB_OBJ_COMMON], CLSYS_DRAW_SUB) / 0x20;
	PaletteWorkSet_VramCopy(game->pfd, FADE_SUB_OBJ, palno*16, BALLOON_SUB_COMMON_PAL_NUM*0x20);
	
	//���D�Ȃǂ̃L�������Z���o�^
	game->cgr_id[CHARID_SUB_BALLOON_MIX] = GFL_CLGRP_CGR_Register(
		hdl, MINI_FUSEN_OBJ_NCGR, FALSE, CLSYS_DRAW_SUB, HEAPID_BALLOON);
	game->cell_id[CELLID_SUB_BALLOON_MIX] = GFL_CLGRP_CELLANIM_Register(hdl, 
		MINI_FUSEN_OBJ_NCER, MINI_FUSEN_OBJ_NANR, HEAPID_BALLOON);

	//���D�A�C�R���A�N�^�[����
	IconBalloon_AllCreate(game);

	Joint_ActorCreateAll(game, &game->joint);
	Booster_ActorCreateAll(game, &game->booster);
	SioBooster_ActorCreateAll(game, &game->sio_booster);
	
	//�J���[�X���b�v
	BalloonTool_PaletteSwap_PlayerOBJ(game);
	BalloonTool_PaletteSwap_Storm(game);
}

//--------------------------------------------------------------
/**
 * @brief   �T�u���OBJ�F���Z�͕���Ŏg�p�����{�I�ȏ풓OBJ�̍폜���s��
 * @param   game		���Z�͕���Ǘ����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void BalloonDefaultOBJDel_Sub(BALLOON_GAME_WORK *game)
{
	//���D�A�C�R���A�N�^�[�폜
	IconBalloon_AllDelete(game);
	Joint_ActorDeleteAll(game, &game->joint);
	Booster_ActorDeleteAll(game, &game->booster);
	SioBooster_ActorDeleteAll(game, &game->sio_booster);
}

//--------------------------------------------------------------
/**
 * @brief   ���C�����BG�F���Z�͕���Ŏg�p�����{BG�f�[�^�̃Z�b�g���s��
 * @param   game		���Z�͕���Ǘ����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void BalloonDefaultBGSet(BALLOON_GAME_WORK *game, ARCHANDLE *hdl)
{
	//BG���ʃp���b�g
	PaletteWorkSet_Arc(game->pfd, ARCID_BALLOON_GRA, MINI_FUSEN_BOTTOM_NCLR, 
		HEAPID_BALLOON, FADE_MAIN_BG, BALLOON_MAIN_BGPAL_SIZE, 0);

	//�w�i
	GFL_ARCHDL_UTIL_TransVramBgCharacter(hdl, MINI_FUSEN_BOTTOM_NCGR, 
		BALLOON_FRAME_BACK, 0, 0, 0, HEAPID_BALLOON);
	GFL_ARCHDL_UTIL_TransVramScreen(hdl, MINI_FUSEN_BOTTOM_NSCR, 
		BALLOON_FRAME_BACK, 0, 0, 0, HEAPID_BALLOON);
	//�E�B���h�E
	GFL_ARCHDL_UTIL_TransVramBgCharacter(hdl, MINI_FUSEN_CCWIN_NCGR, 
		BALLOON_FRAME_EFF, 0, 0, 0, HEAPID_BALLOON);
	GFL_ARCHDL_UTIL_TransVramScreen(hdl, MINI_FUSEN_CCWIN_NSCR, 
		BALLOON_FRAME_EFF, 0, 0, 0, HEAPID_BALLOON);

	//���E�B���h�E
	INFOWIN_Init(BALLOON_FRAME_WIN, INFOWIN_COLOR, HEAPID_BALLOON);
	
	//�o�b�N�O���E���h�ɍ��̃J���[������
	PaletteWork_Clear(game->pfd, FADE_MAIN_BG, FADEBUF_ALL, 0x0000, 0, 1);
}

//--------------------------------------------------------------
/**
 * @brief   ���C�����BG�F���Z�͕���Ŏg�p�����{BG�f�[�^�̍폜�������s��
 * @param   game		���Z�͕���Ǘ����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void BalloonDefaultBGDel(BALLOON_GAME_WORK *game)
{
	INFOWIN_Exit();
}

//--------------------------------------------------------------
/**
 * @brief   �T�u���BG�F���Z�͕���Ŏg�p�����{BG�f�[�^�̃Z�b�g���s��
 * @param   game		���Z�͕���Ǘ����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void BalloonDefaultBGSet_Sub(BALLOON_GAME_WORK *game, ARCHANDLE *hdl)
{
	//BG���ʃp���b�g
	PaletteWorkSet_Arc(game->pfd, ARCID_BALLOON_GRA, MINI_FUSEN_TOP_NCLR, 
		HEAPID_BALLOON, FADE_SUB_BG, 5 * 0x20, 0);
	//BG���ʃp���b�g
	PaletteWorkSetEx_Arc(game->pfd, ARCID_BALLOON_GRA, MINI_FUSEN_POKEFUSEN_NCLR, HEAPID_BALLOON, 
		FADE_SUB_BG, FUSEN_BG_LOAD_SIZE, FUSEN_BG_LOAD_POS * 16, FUSEN_BG_READ_POS * 16);

	//�w�i
	GFL_ARCHDL_UTIL_TransVramBgCharacter(hdl, MINI_FUSEN_BG_NCGR, 
		BALLOON_SUBFRAME_BACK, 0, 0, 0, HEAPID_BALLOON);
	GFL_ARCHDL_UTIL_TransVramScreen(hdl, MINI_FUSEN_BG_NSCR, 
		BALLOON_SUBFRAME_BACK, 0, 0, 0, HEAPID_BALLOON);


	//�p�C�v
	GFL_ARCHDL_UTIL_TransVramBgCharacter(hdl, MINI_FUSEN_WINDOW_NCGR, 
		BALLOON_SUBFRAME_PIPE, 0, 0, 0, HEAPID_BALLOON);
	switch(game->bsw->player_max){
	case 2:
		GFL_ARCHDL_UTIL_TransVramScreen(hdl, MINI_FUSEN_PIPE2P_NSCR, 
			BALLOON_SUBFRAME_PIPE, 0, 0, 0, HEAPID_BALLOON);
		break;
	case 3:
		GFL_ARCHDL_UTIL_TransVramScreen(hdl, MINI_FUSEN_PIPE3P_NSCR, 
			BALLOON_SUBFRAME_PIPE, 0, 0, 0, HEAPID_BALLOON);
		break;
	default:
		GFL_ARCHDL_UTIL_TransVramScreen(hdl, MINI_FUSEN_PIPE4P_NSCR, 
			BALLOON_SUBFRAME_PIPE, 0, 0, 0, HEAPID_BALLOON);
		break;
	}

	//�E�B���h�E��
//	GFL_ARCHDL_UTIL_TransVramBgCharacter(hdl, MINI_FUSEN_WINDOW_NCGR, 
//		BALLOON_SUBFRAME_WIN, 0, 0, 0, HEAPID_BALLOON);
	//BMP�E�B���h�E��CGX�̊m�ۂ����Ă���̂�BG�L�������̈�m�ۂɂ���
	GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(hdl, MINI_FUSEN_WINDOW_NCGR, 
		BALLOON_SUBFRAME_WIN, 0, 0, HEAPID_BALLOON);
	GFL_ARCHDL_UTIL_TransVramScreen(hdl, MINI_FUSEN_WINDOW_NSCR, 
		BALLOON_SUBFRAME_WIN, 0, 0, 0, HEAPID_BALLOON);

	//�v���C���[�ʒu�ɏ]���ăJ���[�X���b�v���s��
	BalloonTool_PaletteSwap_Pipe(game);
	//���O�E�B���h�E�̃p���b�g�ԍ����Q���l���ɉ����ď�������
	BalloonTool_NameWindowPalNoSwap(game);

	//�V�X�e���E�B���h�E��o�^
	game->sysmenu_charno = BmpWinFrame_GraphicSetAreaMan(
		BALLOON_SUBFRAME_WIN, //BMPWIN_SUB_CGX_OFFSET_SYSTEM, 
		BMPWIN_SUB_SYSTEM_WIN_COLOR, MENU_TYPE_SYSTEM, HEAPID_BALLOON);
	PaletteWorkSet_VramCopy(game->pfd, FADE_SUB_BG, BMPWIN_SUB_SYSTEM_WIN_COLOR * 16, 0x20);
	
	//�o�b�N�O���E���h�ɍ��̃J���[������
	PaletteWork_Clear(game->pfd, FADE_SUB_BG, FADEBUF_ALL, 0x0000, 0, 1);
}

//--------------------------------------------------------------
/**
 * @brief   �T�u���BG�F���Z�͕���Ŏg�p�����{BG�f�[�^�̍폜�������s��
 * @param   game		���Z�͕���Ǘ����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void BalloonDefaultBGDel_Sub(BALLOON_GAME_WORK *game)
{
	return;
}

//--------------------------------------------------------------
/**
 * @brief   �풓����3D���f���̓o�^
 *
 * @param   game	
 * @param   hdl		
 */
//--------------------------------------------------------------
static void BalloonDefault3DSet(BALLOON_GAME_WORK *game, ARCHANDLE *hdl)
{
	int i, s;
	PIPE_AIR_WORK *pa = &game->pipe_air;
	DAIZA_WORK *daiza = &game->daiza;
	int entry_pos;
	void *anm_resource = NULL;
#if (WB_FIX == 0)
	u16 objIdx, objIdx_pipe;
	const u32 AirObjIndexTbl[] = {
		G3DOBJ_BALL1R_BMD,
		G3DOBJ_BALL1B_BMD,
		G3DOBJ_BALL1Y_BMD,
		G3DOBJ_BALL1G_BMD,
	};
#endif

	entry_pos = Balloon_NetID_to_EntryNo(game, GFL_NET_SystemGetCurrentID());

	
#if WB_FIX
	//-- �p�C�v --//
    //���f���ް��ǂݍ���
	D3DOBJ_MdlLoadH(&pa->pipe_mdl, hdl, PlayerPosGraphicID[entry_pos].pipe_mdl_id, HEAPID_BALLOON);
    //�����_�[�I�u�W�F�N�g�ɓo�^
    D3DOBJ_Init( &pa->pipe_obj, &pa->pipe_mdl );
    //���W�ݒ�
    D3DOBJ_SetMatrix( &pa->pipe_obj, PIPE_AIR_X, PIPE_AIR_Y, PIPE_AIR_Z);
    D3DOBJ_SetScale(&pa->pipe_obj, PIPE_AIR_SCALE, PIPE_AIR_SCALE, PIPE_AIR_SCALE);
    D3DOBJ_SetDraw( &pa->pipe_obj, TRUE );

	//-- ��C --//
    //���f���f�[�^�ǂݍ���
	D3DOBJ_MdlLoadH(&pa->air_mdl[PIPE_3D_AIR_TYPE_SMALL], 
		hdl, PlayerPosGraphicID[entry_pos].air_s_mdl_id, HEAPID_BALLOON);
    //���f���f�[�^�ǂݍ���
	D3DOBJ_MdlLoadH(&pa->air_mdl[PIPE_3D_AIR_TYPE_MIDDLE], 
		hdl, PlayerPosGraphicID[entry_pos].air_m_mdl_id, HEAPID_BALLOON);
    //���f���f�[�^�ǂݍ���
	D3DOBJ_MdlLoadH(&pa->air_mdl[PIPE_3D_AIR_TYPE_BIG], 
		hdl, PlayerPosGraphicID[entry_pos].air_l_mdl_id, HEAPID_BALLOON);
    //���f���f�[�^�ǂݍ���
	D3DOBJ_MdlLoadH(&pa->air_mdl[PIPE_3D_AIR_TYPE_SPECIAL], 
		hdl, PlayerPosGraphicID[entry_pos].air_sp_mdl_id, HEAPID_BALLOON);
	for(i = 0; i < PIPE_AIR_AIR_MAX; i++){
		for(s = 0; s < PIPE_3D_AIR_TYPE_MAX; s++){
			//�A�j���f�[�^�ǂݍ���
			if(anm_resource == NULL){
				D3DOBJ_AnmLoadH(&pa->air[i][s].anm, &pa->air_mdl[s], hdl, 
					SONANS_BALL1_NSBCA + s, HEAPID_BALLOON, &game->allocator);
				anm_resource = pa->air[i][s].anm.pResAnm;
			}
			else{
				D3DOBJ_AnmLoad_Data(&pa->air[i][s].anm, &pa->air_mdl[s], 
					anm_resource, &game->allocator);
			}
			D3DOBJ_AnmSet(&pa->air[i][s].anm, 0);
		}
	}
	
	//-- ��� --//
    //���f���f�[�^�ǂݍ���
	D3DOBJ_MdlLoadH(&daiza->mdl, hdl, SONANS_DAIZA_NSBMD, HEAPID_BALLOON);
    //�����_�[�I�u�W�F�N�g�ɓo�^
    D3DOBJ_Init( &daiza->obj, &daiza->mdl );
    //���W�ݒ�
    D3DOBJ_SetMatrix( &daiza->obj, DAIZA_X, DAIZA_Y, DAIZA_Z);
    D3DOBJ_SetScale(&daiza->obj, DAIZA_SCALE, DAIZA_SCALE, DAIZA_SCALE);
    D3DOBJ_SetDraw( &daiza->obj, TRUE );
#else
	game->g3DutilUnitIdx = GFL_G3D_UTIL_AddUnit( game->g3Dutil, &g3Dutil_setup );
	{//�p�C�v
		GFL_G3D_UTIL_SETUP pipe_setup;
		
		pipe_setup = g3Dutil_Pipe_setup;
		pipe_setup.resTbl = &g3Dutil_Pipe_resTbl[entry_pos];
		pipe_setup.objTbl = &g3Dutil_Pipe_objTbl[entry_pos];
		game->g3DutilUnitIdx_Pipe = GFL_G3D_UTIL_AddUnit( game->g3Dutil, &pipe_setup );
	}

	objIdx = GFL_G3D_UTIL_GetUnitObjIdx(game->g3Dutil, game->g3DutilUnitIdx );
	objIdx_pipe = GFL_G3D_UTIL_GetUnitObjIdx(game->g3Dutil, game->g3DutilUnitIdx_Pipe );

	//��C���ꂼ���OBJINDEX���擾
	for(i = 0; i < PIPE_AIR_AIR_MAX; i++){
		for(s = 0; s < PIPE_3D_AIR_TYPE_MAX; s++){
			pa->air[i][s].g3dobj 
				= GFL_G3D_UTIL_GetObjHandle(game->g3Dutil, objIdx + AirObjIndexTbl[i] + s);
		}
	}

	//-- �p�C�v --//
	pa->g3dobj = GFL_G3D_UTIL_GetObjHandle(game->g3Dutil, objIdx_pipe + 0);
	VEC_Set(&pa->status.trans, PIPE_AIR_X, PIPE_AIR_Y, PIPE_AIR_Z);
	VEC_Set(&pa->status.scale, PIPE_AIR_SCALE, PIPE_AIR_SCALE, PIPE_AIR_SCALE);
	MTX_Identity33(&pa->status.rotate);

	//-- ��� --//
	daiza->g3dobj = GFL_G3D_UTIL_GetObjHandle(game->g3Dutil, objIdx + G3DOBJ_DAIZA);
	VEC_Set(&daiza->status.trans, DAIZA_X, DAIZA_Y, DAIZA_Z);
	VEC_Set(&daiza->status.scale, DAIZA_SCALE, DAIZA_SCALE, DAIZA_SCALE);
	MTX_Identity33(&daiza->status.rotate);
#endif
}

//--------------------------------------------------------------
/**
 * @brief   �풓����3D���f�����폜
 *
 * @param   game		
 */
//--------------------------------------------------------------
static void BalloonDefault3DDel(BALLOON_GAME_WORK *game)
{
	int i, s;
	PIPE_AIR_WORK *pa = &game->pipe_air;

#if WB_FIX
    D3DOBJ_MdlDelete( &pa->pipe_mdl );
    for(i = 0; i < PIPE_3D_AIR_TYPE_MAX; i++){
		D3DOBJ_MdlDelete( &pa->air_mdl[i] );
	}
	for(i = 0; i < PIPE_AIR_AIR_MAX; i++){
		for(s = 0; s < PIPE_3D_AIR_TYPE_MAX; s++){
			D3DOBJ_AnmDelete(&pa->air[i][s].anm, &game->allocator);
		}
	}
    D3DOBJ_MdlDelete( &game->daiza.mdl );
#else
	GFL_G3D_UTIL_DelUnit( game->g3Dutil, game->g3DutilUnitIdx );
	GFL_G3D_UTIL_Delete( game->g3Dutil );
#endif
}

//--------------------------------------------------------------
/**
 * @brief   ��C���f�����G���g���[������
 *
 * @param   game			
 * @param   air_size		��C��
 *
 * @retval  TRUE:�G���g���[����  FALSE:���s
 */
//--------------------------------------------------------------
BOOL Air3D_EntryAdd(BALLOON_GAME_PTR game, int air)
{
	PIPE_AIR_WORK *pa = &game->pipe_air;
	int i, air_size;
#if WB_FIX
	D3DOBJ_MDL *mdl;
#else
	u16 objIdx = GFL_G3D_UTIL_GetUnitObjIdx(game->g3Dutil, game->g3DutilUnitIdx );
	int anmframe = 0;
#endif
	
	if(air > SONANS_POMP_MAX_AIR){	//�Ԃ��Ȃ��Ă��鎞 or ���A����
		air_size = PIPE_3D_AIR_TYPE_SPECIAL;
	}
	else if(air > PIPE_3D_AIR_TYPE_SPECIAL / 3 * 2){
		air_size = PIPE_3D_AIR_TYPE_BIG;
	}
	else if(air > PIPE_3D_AIR_TYPE_SPECIAL / 3 * 1){
		air_size = PIPE_3D_AIR_TYPE_MIDDLE;
	}
	else{
		air_size = PIPE_3D_AIR_TYPE_SMALL;
	}
	
#ifdef OSP_BALLOON_ON
	OS_TPrintf("��C�T�C�Y��%d\n", air_size);
#endif
	
#if WB_FIX
	mdl = &pa->air_mdl[air_size];

	for(i = 0; i < PIPE_AIR_AIR_MAX; i++){
		if(pa->air[i][air_size].occ == FALSE){
			//�����_�[�I�u�W�F�N�g�ɓo�^
			D3DOBJ_Init(&pa->air[i][air_size].obj, mdl);
			//���W�ݒ�
			D3DOBJ_SetMatrix( &pa->air[i][air_size].obj, PIPE_AIR_X, PIPE_AIR_Y, PIPE_AIR_Z);
			D3DOBJ_SetScale(&pa->air[i][air_size].obj, 
				PIPE_AIR_SCALE, PIPE_AIR_SCALE, PIPE_AIR_SCALE);
			D3DOBJ_SetDraw( &pa->air[i][air_size].obj, TRUE );
			//�A�j���֘A�t��
			D3DOBJ_AnmSet(&pa->air[i][air_size].anm, 0);
			D3DOBJ_AddAnm(&pa->air[i][air_size].obj, &pa->air[i][air_size].anm);
			
			pa->air[i][air_size].occ = TRUE;
			Snd_SePlay(SE_BALLOON_POMP_AIR);
			return TRUE;
		}
	}
#else
	for(i = 0; i < PIPE_AIR_AIR_MAX; i++){
		if(pa->air[i][air_size].occ == FALSE){
			//�����_�[�I�u�W�F�N�g������
			if(pa->air[i][air_size].enable_anm_index != 0xffff){
				GFL_G3D_OBJECT_DisableAnime(
					pa->air[i][air_size].g3dobj, pa->air[i][air_size].enable_anm_index);
			}
			//���W�ݒ�
			VEC_Set(&pa->air[i][air_size].status.trans, PIPE_AIR_X, PIPE_AIR_Y, PIPE_AIR_Z);
			VEC_Set(&pa->air[i][air_size].status.scale, 
				PIPE_AIR_SCALE, PIPE_AIR_SCALE, PIPE_AIR_SCALE);
			MTX_Identity33(&pa->air[i][air_size].status.rotate);
			//�A�j���֘A�t��
			GFL_G3D_OBJECT_SetAnimeFrame(pa->air[i][air_size].g3dobj, air_size, &anmframe);
			GFL_G3D_OBJECT_EnableAnime(pa->air[i][air_size].g3dobj, air_size);
			
			pa->air[i][air_size].enable_anm_index = air_size;
			pa->air[i][air_size].occ = TRUE;
		#if WB_TEMP_FIX
			Snd_SePlay(SE_BALLOON_POMP_AIR);
		#endif
			return TRUE;
		}
	}
#endif

	//GF_ASSERT(0);	//�����_�[�I�u�W�F�N�g�ɋ󂫂��Ȃ�
#ifdef OSP_BALLOON_ON
	OS_TPrintf("��C3D�̃����_�[�I�u�W�F�N�g�ɋ󂫂�����܂���\n");
#endif
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   ��C���f�����폜����
 *
 * @param   game		
 * @param   air_no		��C���f���̊Ǘ��ԍ�
 * @param   air_size	��C�̃T�C�Y
 */
//--------------------------------------------------------------
void Air3D_Delete(BALLOON_GAME_PTR game, int air_no, int air_size)
{
	PIPE_AIR_WORK *pa = &game->pipe_air;
	
	//�A�j���̊֘A�t����؂�
#if WB_FIX
	D3DOBJ_DelAnm(&pa->air[air_no][air_size].obj, &pa->air[air_no][air_size].anm);
#else
	u16 objIdx = GFL_G3D_UTIL_GetUnitObjIdx(game->g3Dutil, game->g3DutilUnitIdx );

	GFL_G3D_OBJECT_DisableAnime(pa->air[air_no][air_size].g3dobj,
		pa->air[air_no][air_size].enable_anm_index);
	pa->air[air_no][air_size].enable_anm_index = 0xffff;
#endif

	pa->air[air_no][air_size].occ = FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   ��C���f���X�V����
 *
 * @param   game		
 */
//--------------------------------------------------------------
void Air3D_Update(BALLOON_GAME_PTR game)
{
	PIPE_AIR_WORK *pa = &game->pipe_air;
	int i, s;
	
	for(i = 0; i < PIPE_AIR_AIR_MAX; i++){
		for(s = 0; s < PIPE_3D_AIR_TYPE_MAX; s++){
		#if WB_FIX
			if(pa->air[i][s].occ == TRUE){
				if(D3DOBJ_AnmNoLoop(&pa->air[i][s].anm, FX32_ONE) == TRUE){
		#else
			if(pa->air[i][s].occ == TRUE && pa->air[i][s].enable_anm_index != 0xffff){
				if(GFL_G3D_OBJECT_LoopAnimeFrame(
					pa->air[i][s].g3dobj, pa->air[i][s].enable_anm_index, FX32_ONE) == FALSE){
		#endif
					Air3D_Delete(game, i, s);
				}
			}
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   ��C���f���`�揈��
 *
 * @param   game		
 */
//--------------------------------------------------------------
void Air3D_Draw(BALLOON_GAME_PTR game)
{
	PIPE_AIR_WORK *pa = &game->pipe_air;
	int i, s;
	
	for(i = 0; i < PIPE_AIR_AIR_MAX; i++){
		for(s = 0; s < PIPE_3D_AIR_TYPE_MAX; s++){
			if(pa->air[i][s].occ == TRUE){
			#if WB_FIX
				D3DOBJ_Draw( &pa->air[i][s].obj );
			#else
				GFL_G3D_DRAW_DrawObject(pa->air[i][s].g3dobj, &pa->air[i][s].status);
			#endif
			}
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   �������T�[�o�[���m�F
 *
 * @param   game		
 *
 * @retval  TRUE:�������T�[�o�[
 */
//--------------------------------------------------------------
BOOL Balloon_ServerCheck(BALLOON_GAME_PTR game)
{
#ifdef PM_DEBUG
	if(game->bsw->debug_offline == TRUE){
		return TRUE;
	}
#endif

	if(game->server_netid == GFL_NET_SystemGetCurrentID()){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �T�[�o�[�S����N�ɂ��邩���߂�
 *
 * @param   game		
 *
 * @retval  �T�[�o�[�ɂȂ����l�̃l�b�gID
 */
//--------------------------------------------------------------
int Balloon_ServerSelect(BALLOON_GAME_PTR game)
{
	int server_netid, i;
	u32 now_version;
	
#ifdef PM_DEBUG
	if(game->bsw->debug_offline == TRUE){
		return 0;
	}
#endif

	//�T�[�o�[�o�[�W��������ԍ����l���T�[�o�[�ɂȂ�B
	//�����ꍇ�̓l�b�gID���Ⴂ�����D��
	server_netid = game->bsw->player_netid[0];
	now_version = game->recv_server_version[0];
	for(i = 1; i < game->bsw->player_max; i++){
		if(now_version < game->recv_server_version[i]){
			now_version = game->recv_server_version[i];
			server_netid = game->bsw->player_netid[i];
		}
	}
	game->server_netid = server_netid;
	OS_TPrintf("�T�[�o�[�S����%d��(Version = %d)\n", game->server_netid, now_version);
	return game->server_netid;
}

//--------------------------------------------------------------
/**
 * @brief   ��M�o�b�t�@�Ƀf�[�^���Z�b�g����
 *
 * @param   game		
 * @param   net_id		�Ώۃf�[�^�̃l�b�gID
 * @param   src			�Z�b�g����f�[�^
 */
//--------------------------------------------------------------
void BalloonSio_RecvBufferSet(BALLOON_GAME_PTR game, int net_id, const BALLOON_SIO_PLAY_WORK *src)
{
	BALLOON_SIO_PLAY_WORK *recv;
	int ring_no;
	
	if(BalloonSio_ImportantRecvBufferSet(game, net_id, src) == TRUE){
		return;
	}
	
	//�f�[�^�����܂肷�������Aread��write��ring_no�̉�荞�݂��������Ȃ��悤��
	//ring_no�͏�ɃC���N�������g�������A�o�b�t�@�ւ̃A�N�Z�X�͏��Z������]�ōs��
	ring_no = game->recv_write_ring_no[net_id] % SIO_BUFFER_CHAIN_MAX;
	recv = &game->recv_buf[net_id].play_work[ring_no];
	
	game->recv_write_ring_no[net_id]++;	//�������ݗp�����O�o�b�t�@�ԍ���i�߂�
	
	//����肷��͉̂��o�f�[�^�����Ȃ̂ŏ㏑�����Ă����Ȃ���
	//�f�o�b�O���͂ǂꂮ�炢�㏑�����������邩�m�F����ׁAASSERT�Ŏ~�߂�
	GF_ASSERT(recv->order_code == ORDER_CODE_NULL);
	
	*recv = *src;
}

//--------------------------------------------------------------
/**
 * @brief   ��M�o�b�t�@����f�[�^�����o��(�ǂݎ��p)
 *
 * @param   game		
 * @param   net_id		���o���Ώۂ̃l�b�gID
 *
 * @retval  �ǂݎ��ʒu�̃f�[�^�̃|�C���^�B�f�[�^���Ȃ��ꍇ��NULL�B
 */
//--------------------------------------------------------------
BALLOON_SIO_PLAY_WORK * BalloonSio_RecvBufferReadPtrGet(BALLOON_GAME_PTR game, int net_id)
{
	BALLOON_SIO_PLAY_WORK *recv;
	int ring_no;
	
	//�f�[�^�����܂肷�������Aread��write��ring_no�̉�荞�݂��������Ȃ��悤��
	//ring_no�͏�ɃC���N�������g�������A�o�b�t�@�ւ̃A�N�Z�X�͏��Z������]�ōs��
	ring_no = game->recv_read_ring_no[net_id] % SIO_BUFFER_CHAIN_MAX;
	recv = &game->recv_buf[net_id].play_work[ring_no];
	
	if(recv->order_code != ORDER_CODE_NULL){
		//�f�[�^�������Ă����Ȃ�Γǂݍ��ݗp�����O�o�b�t�@�ԍ���i�߂�
		game->recv_read_ring_no[net_id]++;
		return recv;
	}
	return NULL;	//�f�[�^�������Ă��Ȃ�
}

//--------------------------------------------------------------
/**
 * @brief   �d�v�f�[�^����M�o�b�t�@�ɃZ�b�g����
 *
 * @param   game		
 * @param   net_id		�Ώۃf�[�^�̃l�b�gID
 * @param   src			�Z�b�g����f�[�^�ւ̃|�C���^
 *
 * @retval  TRUE:�Z�b�g�����B�@FALSE:�d�v�f�[�^�ł͂Ȃ������̂ŃZ�b�g���Ȃ�����
 */
//--------------------------------------------------------------
static BOOL BalloonSio_ImportantRecvBufferSet(BALLOON_GAME_PTR game, int net_id, const BALLOON_SIO_PLAY_WORK *src)
{
	switch(src->order_code){
	case ORDER_CODE_EXPLODED:
	case ORDER_CODE_APPEAR:
	case ORDER_CODE_START:
	case ORDER_CODE_FINISH:
	case ORDER_CODE_TIMING:
	case ORDER_CODE_TOUCHPEN_DEMO:
		GF_ASSERT(game->recv_important.order_code == ORDER_CODE_NULL);
		game->recv_important = *src;
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   ���M�o�b�t�@�Ƀf�[�^���Z�b�g����
 *
 * @param   game		
 * @param   src			�Z�b�g����f�[�^
 */
//--------------------------------------------------------------
void BalloonSio_SendBufferSet(BALLOON_GAME_PTR game, const BALLOON_SIO_PLAY_WORK *src)
{
	BALLOON_SIO_PLAY_WORK *send;
	int ring_no;
	
	if(BalloonSio_ImportantSendBufferSet(game, src) == TRUE){
		return;
	}
	
	//�f�[�^�����܂肷�������Aread��write��ring_no�̉�荞�݂��������Ȃ��悤��
	//ring_no�͏�ɃC���N�������g�������A�o�b�t�@�ւ̃A�N�Z�X�͏��Z������]�ōs��
	ring_no = game->send_write_ring_no % SIO_BUFFER_CHAIN_MAX;
	send = &game->send_buf.play_work[ring_no];
	
	game->send_write_ring_no++;	//�������ݗp�����O�o�b�t�@�ԍ���i�߂�
	
	//����肷��͉̂��o�f�[�^�����Ȃ̂ŏ㏑�����Ă����Ȃ���
	//�f�o�b�O���͂ǂꂮ�炢�㏑�����������邩�m�F����ׁAASSERT�Ŏ~�߂�
	GF_ASSERT(send->order_code == ORDER_CODE_NULL);
	
	*send = *src;
	OS_TPrintf("��ʃf�[�^���M�o�b�t�@�փZ�b�g order_code = %d, ring_no = %d\n", src->order_code, ring_no);
}

//--------------------------------------------------------------
/**
 * @brief   �d�v�f�[�^�𑗐M�o�b�t�@�ɃZ�b�g����
 *
 * @param   game		
 * @param   src			�Z�b�g����f�[�^�ւ̃|�C���^
 *
 * @retval  TRUE:�Z�b�g�����B�@FALSE:�d�v�f�[�^�ł͂Ȃ������̂ŃZ�b�g���Ȃ�����
 */
//--------------------------------------------------------------
static BOOL BalloonSio_ImportantSendBufferSet(BALLOON_GAME_PTR game, const BALLOON_SIO_PLAY_WORK *src)
{
	switch(src->order_code){
	case ORDER_CODE_EXPLODED:
	case ORDER_CODE_APPEAR:
	case ORDER_CODE_START:
	case ORDER_CODE_FINISH:
	case ORDER_CODE_TIMING:
	case ORDER_CODE_TOUCHPEN_DEMO:
		if(game->send_important.order_code == ORDER_CODE_NULL){
			OS_TPrintf("�d�v�f�[�^���M�o�b�t�@�փZ�b�g order_code = %d\n", src->order_code);
			game->send_important = *src;
			return TRUE;
		}
		else if(src->order_code == game->send_important.order_code){
			//�������߂����Ƀ��N�G�X�g�ς݁B
			//�d�v�f�[�^���N�G�X�g���������Ă���ꍇ�͈�ʃf�[�^�𑗐M���Ȃ��悤��TRUE�ŕԂ�
			OS_TPrintf("�d�v�f�[�^2�d���N�G�X�g���� order_code = %d\n", src->order_code);
			return TRUE;
		}
		else{
			//�d�v�f�[�^�̃��N�G�X�g�����ɔ������Ă���̂ɁA�Ⴄ��ނ̏d�v�f�[�^��
			//���N�G�X�g�������Ă���
			OS_TPrintf("���܂��Ă���order_code = %d, �V�K�Ƀ��N�G�X�g����order_code = %d\n", game->send_important.order_code, src->order_code);
			GF_ASSERT(0);
			return FALSE;
		}
		break;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   ���M�o�b�t�@����f�[�^�����o��(�ǂݎ��p)
 *
 * @param   game		
 *
 * @retval  �ǂݎ��ʒu�̃f�[�^�̃|�C���^�B�f�[�^���Ȃ��ꍇ��NULL�B
 *
 * ���M�o�b�t�@��read_ring_no�́A���o���Ă��A���M���s�̉\��������̂�
 * ���̊֐����ł̓C���N�������g���Ȃ�
 */
//--------------------------------------------------------------
BALLOON_SIO_PLAY_WORK * BalloonSio_SendBufferReadPtrGet(BALLOON_GAME_PTR game)
{
	BALLOON_SIO_PLAY_WORK *send;
	int ring_no;
	
	//�f�[�^�����܂肷�������Aread��write��ring_no�̉�荞�݂��������Ȃ��悤��
	//ring_no�͏�ɃC���N�������g�������A�o�b�t�@�ւ̃A�N�Z�X�͏��Z������]�ōs��
	ring_no = game->send_read_ring_no % SIO_BUFFER_CHAIN_MAX;
	send = &game->send_buf.play_work[ring_no];
	
	if(send->order_code != ORDER_CODE_NULL){
//		game->send_read_ring_no++;
		return send;
	}
	return NULL;	//�f�[�^�������Ă��Ȃ�
}

//--------------------------------------------------------------
/**
 * @brief   ���M�o�b�t�@�ɗ��܂��Ă���f�[�^�𑗐M����
 *
 * @param   game		
 *
 * @retval  TRUE:���M�����B�@FALSE:���M���Ȃ�����(�f�[�^���Ȃ��ꍇ������)
 */
//--------------------------------------------------------------
static int BalloonSio_CommSend(BALLOON_GAME_PTR game)
{
	BALLOON_SIO_PLAY_WORK *send;

#ifdef PM_DEBUG
	if(game->bsw->debug_offline == TRUE){
		game->sio_wait = 0;
	}
#endif

	if(game->sio_wait > 0){
		game->sio_wait--;	//Wifi�{�C�X�`���b�g���ׂɑς�����悤�Ɏ��̑��M�܂ŊԂ�����
		return FALSE;
	}

#ifdef PM_DEBUG
	if(game->bsw->debug_offline == TRUE){
		//�d�v�f�[�^
		if(game->send_important.order_code != ORDER_CODE_NULL){
			game->recv_important = game->send_important;
			game->send_important.order_code = ORDER_CODE_NULL;
			return TRUE;
		}

		//��ʃf�[�^���M
		send = BalloonSio_SendBufferReadPtrGet(game);
		if(send == NULL){
			return FALSE;
		}
		BalloonSio_RecvBufferSet(game, 0, send);
		send->order_code = ORDER_CODE_NULL;
		game->send_read_ring_no++;
		return TRUE;
	}
#endif

	//�d�v�f�[�^������ꍇ�͗D��I�ɑ��M
	if(game->send_important.order_code != ORDER_CODE_NULL){
		if(Send_CommBalloonPlayData(game, &game->send_important) == TRUE){
			OS_TPrintf("���M�F�d�v�f�[�^ order_code = %d\n", game->send_important.order_code);
			game->send_important.order_code = ORDER_CODE_NULL;
			return TRUE;
		}
		else{
			return FALSE;
		}
	}
	
	//��ʃf�[�^���M
	send = BalloonSio_SendBufferReadPtrGet(game);
	if(send == NULL){
		return FALSE;
	}
	if(Send_CommBalloonPlayData(game, send) == TRUE){
		OS_TPrintf("���M�F��ʃf�[�^ order_code = %d\n", send->order_code);
		send->order_code = ORDER_CODE_NULL;
		game->send_read_ring_no++;
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   ��M�o�b�t�@�ɗ��܂��Ă���f�[�^�����߁A���s����
 *
 * @param   game		
 */
//--------------------------------------------------------------
static void BalloonSio_CommRecv(BALLOON_GAME_PTR game)
{
	int net_id, player_no;
	BALLOON_SIO_PLAY_WORK *recv;
	int ret, order_code;
	
	//�d�v�f�[�^������ꍇ�͗D��I�Ɏ��s
	order_code = game->recv_important.order_code;
	if(RecvBalloon_FuncCall(game, &game->recv_important) == TRUE){
		OS_TPrintf("��M�F�d�v�f�[�^ order_code = %d\n", order_code);
		return;
	}
	
	//��ʃf�[�^�`�F�b�N
	recv = &game->recv_important;
	for(player_no = 0; player_no < game->bsw->player_max; player_no++){
		while(1){
			recv = BalloonSio_RecvBufferReadPtrGet(game, game->bsw->player_netid[player_no]);
			if(recv == NULL){
				break;
			}
			order_code = recv->order_code;
			ret = RecvBalloon_FuncCall(game, recv);
			if(ret == TRUE){
				OS_TPrintf("��M�F��ʃf�[�^ order_code = %d\n", order_code);
			}
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   ���D�ԍ��擾
 *
 * @param   game		
 *
 * @retval  ���D�ԍ�
 */
//--------------------------------------------------------------
int Balloon_BalloonNoGet(BALLOON_GAME_PTR game)
{
	return game->balloon_no;
}

//--------------------------------------------------------------
/**
 * @brief   �l�b�gID���炻�̃v���C���[�̈ʒu���擾����
 *
 * @param   game		
 * @param   net_id		�l�b�gID
 *
 * @retval  6���̈ʒu��0�Ƃ��č����ɁA�P�C�Q�C�R�E�E�ƂȂ�
 */
//--------------------------------------------------------------
int Balloon_NetID_to_PlayerPos(BALLOON_GAME_PTR game, int net_id)
{
	int target_pos, my_pos, offset_pos;
	int target_index, my_index;
	int i, current_id;
	int tbl_max;
	const u8 *index_tbl;
	
#if PM_DEBUG
	if(game->bsw->debug_offline == TRUE){
		return 0;
	}
#endif

	current_id = GFL_NET_SystemGetCurrentID();
	target_index = 0xff;
	my_index = 0xff;
	
	//�Q���l�b�gID���X�g���牽�Ԗڂ̎Q���҂�Index���擾
	for(i = 0; i < game->bsw->player_max; i++){
		if(game->bsw->player_netid[i] == current_id){
			my_index = i;
		}
		if(game->bsw->player_netid[i] == net_id){
			target_index = i;
		}
	#ifdef OSP_BALLOON_ON
		OS_TPrintf("player_netid[i] = %d\n", game->bsw->player_netid[i]);
	#endif
	}
#ifdef OSP_BALLOON_ON
	OS_TPrintf("current_id = %d, net_id = %d, my_index = %d, target_index = %d\n", current_id, net_id, my_index, target_index);
#endif
	GF_ASSERT(target_index != 0xff && my_index != 0xff);
	
	//���̃}�V���̃l�b�gID�����Index����ʒu�֌W��c��
	for(i = 0; i < game->bsw->player_max; i++){
		if(BalloonPlayerSort[game->bsw->player_max][i] == my_index){
			my_pos = i;
		#ifdef OSP_BALLOON_ON
			OS_TPrintf("my_pos ���� =%d\n", my_pos);
		#endif
		}
		if(BalloonPlayerSort[game->bsw->player_max][i] == target_index){
			target_pos = i;
		#ifdef OSP_BALLOON_ON
			OS_TPrintf("target_pos ���� =%d\n", target_pos);
		#endif
		}
	}
	offset_pos = target_pos - my_pos;
#ifdef OSP_BALLOON_ON
	OS_TPrintf("target_pos = %d, my_pos = %d, player_max = %d\n", target_pos, my_pos, game->bsw->player_max);
#endif
	if(offset_pos < 0){
		offset_pos += game->bsw->player_max;
	}
//	pos = BALLOON_PLAYER_POS_4D + offset_pos;
#ifdef OSP_BALLOON_ON
	OS_TPrintf("offset_pos = %d\n", offset_pos);
#endif
	return offset_pos;	//�����玞�v���ŉ��Ԗ�
}

//--------------------------------------------------------------
/**
 * @brief   �l�b�gID���牽�Ԗڂ̎Q���҂Ȃ̂����擾
 *
 * @param   game		
 * @param   net_id		
 *
 * @retval  ���Ԗڂ̎Q���҂�
 */
//--------------------------------------------------------------
int Balloon_NetID_to_EntryNo(BALLOON_GAME_PTR game, int net_id)
{
	int i;
	
	//�Q���l�b�gID���X�g���牽�Ԗڂ̎Q���҂�Index���擾
	for(i = 0; i < game->bsw->player_max; i++){
		if(game->bsw->player_netid[i] == net_id){
			return i;
		}
	}
	GF_ASSERT(0);
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   �������Ԏ����N�G�X�g�p�����[�^���Z�b�g
 *
 * @param   game			
 * @param   timing_req		TIMING_REQ_???
 * @param   timing_no		�����ԍ�
 */
//--------------------------------------------------------------
void Timing_AnswerReqParamSet(BALLOON_GAME_PTR game, int timing_req, u8 timing_no)
{
	GF_ASSERT(game->timing_req == TIMING_REQ_NULL);
	
	game->timing_req = timing_req;
	game->timing_no = timing_no;
}

//--------------------------------------------------------------
/**
 * @brief   �������̕Ԏ����N�G�X�g���������Ă���ꍇ�A�����ɏ]���ē����R�}���h�𑗐M����
 *
 * @param   game		
 *
 * @retval  TRUE:�������R�}���h�𑗐M����
 */
//--------------------------------------------------------------
static BOOL Timing_AnswerSend(BALLOON_GAME_PTR game)
{
	int req = FALSE;
	
	switch(game->timing_req){
	case TIMING_REQ_NORMAL:
		req = TRUE;
		break;
	
	case TIMING_REQ_START_AFTER:
		req = TRUE;
		break;
	case TIMING_REQ_FINISH_AFTER:
		req = TRUE;
		break;
	
	case TIMING_REQ_APPEAR_AFTER:
		if(game->balloon_occ == TRUE){
			req = TRUE;
		}
		break;
	case TIMING_REQ_EXPLODED_AFTER:
		if(game->exploded_param.occ == FALSE){
			req = TRUE;
		}
		break;
	
	case TIMING_REQ_TOUCHPEN_DEMO_AFTER:
		if(game->pen.demo_end == TRUE){
			req = TRUE;
		}
		break;
	
	default:
		return FALSE;
	}
	
	if(req == TRUE){
		#ifdef PM_DEBUG
			if(game->bsw->debug_offline == TRUE){
				game->timing_req = TIMING_REQ_WAIT;
				return TRUE;
			}
		#endif

		GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), game->timing_no);
		if(Balloon_ServerCheck(game) == TRUE){
			//�T�[�o�[�͎�M�m�F��NULL������ׁAWAIT
			game->timing_req = TIMING_REQ_WAIT;
		}
		else{
			//�N���C�A���g�͂��̏��NULL��
			game->timing_req = TIMING_REQ_NULL;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �������R�}���h�𑗐M�ς݂̏ꍇ�A���̃}�V���̓����m�F���s��
 *
 * @param   game		
 *
 * @retval  ��������ꂽ�ꍇ�F���̓������N�G�X�g�ԍ�
 * @retval	�����҂��FTIMING_REQ_WAIT
 * @retval	���N�G�X�g���������Ă��Ȃ��ꍇ�FTIMING_REQ_NULL
 */
//--------------------------------------------------------------
static int Timing_Recv(BALLOON_GAME_PTR game, int server_timing_no)
{
#ifdef PM_DEBUG
	if(game->bsw->debug_offline == TRUE){
		game->timing_req = TIMING_REQ_NULL;
		return TRUE;
	}
#endif
	
	if(game->timing_req != TIMING_REQ_WAIT){
		return FALSE;
	}
	
	if(game->timing_no != server_timing_no){
		return FALSE;	//�T�[�o�[�̋��߂Ă��铯���ԍ��ƈႤ
	}
	
	if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),game->timing_no) == TRUE){
		OS_TPrintf("�������� no = %d\n", game->timing_no);
		game->timing_req = TIMING_REQ_NULL;
		return TRUE;
	}
	return FALSE;
}

//==============================================================================
//
//	�T�[�o�[����
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �T�[�o�[����F�Q�[���J�n����
 *
 * @param   game		
 * @param   server		�T�[�o�[���[�N�ւ̃|�C���^
 *
 * @retval  TRUE:���̊֐��̓���I��
 */
//--------------------------------------------------------------
static int ServerPlay_Start(BALLOON_GAME_PTR game, BALLOON_SERVER_WORK *server)
{
	switch(server->local_seq){
	//�Q�[���J�n�O�̓������
	case 0:
		SendBalloon_Timing(game, TIMING_REQ_NORMAL, server->req_timing_no);
		server->local_seq++;
		break;
	case 1:
		if(Timing_Recv(game, server->req_timing_no) == TRUE){
			server->req_timing_no++;
			server->local_seq++;
		}
		break;
	
	//�^�b�`�y���f���̊J�n�Ɠ����҂�
	case 2:
		SendBalloon_TouchPenDemo(game, server->req_timing_no);
		server->local_seq++;
		break;
	case 3:
		if(Timing_Recv(game, server->req_timing_no) == TRUE){
		#ifdef PM_DEBUG
			if(game->bsw->debug_offline == TRUE){
				if(game->pen.demo_end == FALSE){
					break;
				}
			}
		#endif
			server->req_timing_no++;
			server->local_seq++;
		}
		break;
		
	//�J�n���o�Ɖ��o�I���̓����҂�
	case 4:
		SendBalloon_Start(game, server->req_timing_no);
		server->local_seq++;
		break;
	case 5:
		if(Timing_Recv(game, server->req_timing_no) == TRUE){
			server->req_timing_no++;
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �T�[�o�[����F�Q�[���v���C���̓��쐧��
 *
 * @param   game		
 * @param   server		�T�[�o�[���[�N�ւ̃|�C���^
 *
 * @retval  TRUE:���̊֐��̓���I��
 */
//--------------------------------------------------------------
static int ServerPlay_Game(BALLOON_GAME_PTR game, BALLOON_SERVER_WORK *server)
{
	enum{
		SEQ_APPEAR,
		SEQ_APPEAR_TIMING_WAIT,
		SEQ_EXPLODED,
		SEQ_EXPLODED_TIMING_WAIT,
	};
	
	//�~�j�Q�[���I�����ԃ`�F�b�N
	if(game->time > BALLOON_GAME_PLAY_TIME){
		if(game->timing_req == TIMING_REQ_NULL 
				&& game->send_important.order_code == ORDER_CODE_NULL
				&& game->recv_important.order_code == ORDER_CODE_NULL){
			return TRUE;
		}
		else{
//			OS_TPrintf("�I�������ɓ��B���Ă��邯�ǏI���o���Ȃ� timing_req = %d, send_code = %d, recv_code = %d\n", game->timing_req, game->send_important.order_code, game->recv_important.order_code);
		}
	}
	
	switch(server->local_seq){
	//���D�o��
	case SEQ_APPEAR:
		OS_TPrintf("SEQ_APPEAR\n");
		SendBalloon_Appear(game, game->balloon_no, 
			BalloonTool_BalloonLevelGet(game->balloon_no), server->req_timing_no);
		server->local_seq++;
		break;
	case SEQ_APPEAR_TIMING_WAIT:
	#ifdef OSP_BALLOON_ON
		OS_TPrintf("SEQ_APPEAR_TIMING_WAIT\n");
	#endif
		if(Timing_Recv(game, server->req_timing_no) == TRUE){
			server->req_timing_no++;
			server->local_seq++;
			OS_TPrintf("SEQ_APPEAR_TIMING_WAIT��������\n");
		}
		break;
	
	//���D�̋�C���j���ԂɂȂ�܂ő҂��Ĕj�􃊃N�G�X�g�𑗂�
	case SEQ_EXPLODED:
//		OS_TPrintf("SEQ_EXPLODED\n");
	#ifdef PM_DEBUG
		if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT){
			game->bst.air = game->bst.max_air;
		}
	#endif
		if(game->bst.max_air != 0 && game->bst.air >= game->bst.max_air){
			game->balloon_no++;
			SendBalloon_Exploded(game, Balloon_BalloonNoGet(game), server->req_timing_no);
			server->local_seq++;
			OS_TPrintf("SEQ_EXPLODED���M max_air=%d, air=%d\n", game->bst.max_air, game->bst.air);
		}
		break;
	case SEQ_EXPLODED_TIMING_WAIT:
	#ifdef OSP_BALLOON_ON
		OS_TPrintf("SEQ_EXPLODED_TIMING_WAIT\n");
	#endif
		if(Timing_Recv(game, server->req_timing_no) == TRUE){
			server->req_timing_no++;
			server->local_seq = SEQ_APPEAR;
			OS_TPrintf("SEQ_EXPLODED_TIMING_WAIT��������\n");
		}
		break;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �T�[�o�[����F�Q�[���I������
 *
 * @param   game		
 * @param   server		�T�[�o�[���[�N�ւ̃|�C���^
 *
 * @retval  TRUE:���̊֐��̓���I��
 */
//--------------------------------------------------------------
static int ServerPlay_Finish(BALLOON_GAME_PTR game, BALLOON_SERVER_WORK *server)
{
	switch(server->local_seq){
	//�I�����o�Ɖ��o�I���̓����҂�
	case 0:
		SendBalloon_Finish(game, server->req_timing_no);
		server->local_seq++;
		break;
	case 1:
		if(Timing_Recv(game, server->req_timing_no) == TRUE){
			server->req_timing_no++;
			server->local_seq++;
		}
		break;
	case 2:
		if(game->countdown_eff == BALLOON_COUNTDOWN_TIMEUP_END){
			server->local_seq++;
		}
		break;
		
	//�Q�[���Ó]�ֈڍs����
	case 3:		//�I���O�̃E�F�C�g(��C�A�N�^�[�Ȃǂ��S�č폜�����̂�҂�)
		server->local_wait++;
		if(server->local_wait < 90){	//��check�b��
			OS_TPrintf("local_wait = %d, finish = %d\n", server->local_wait, game->game_finish);
			break;
		}
		
		#ifdef PM_DEBUG
			if(game->bsw->debug_offline == TRUE){
				game->game_end = TRUE;
				return TRUE;
			}
		#endif
		
		if(Send_CommGameEnd(game) == TRUE){
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �T�[�o�[�̃Q�[���i�s���䃁�C��
 *
 * @param   game		
 *
 * @retval  TRUE:�S�Ă̓��삪�I������
 */
//--------------------------------------------------------------
static BOOL Server_GamePlayingManage(BALLOON_GAME_PTR game)
{
	int ret;
	BALLOON_SERVER_WORK *server;
	
	if(Balloon_ServerCheck(game) == FALSE){
		return FALSE;
	}
	
	server = &game->server_work;
	
	if(server->seq >= NELEMS(ServerPlayFuncTbl)){
		return TRUE;
	}

	ret = ServerPlayFuncTbl[server->seq](game, server);
	if(ret == TRUE){
		server->seq++;
		server->local_seq = 0;
		server->local_wait = 0;
		server->local_work = 0;
	}
	
	return FALSE;
	
}

//--------------------------------------------------------------
/**
 * @brief   �f�o�b�O�@�\�F�J�����ړ�
 *
 * @param   camera
 */
//--------------------------------------------------------------
#if WB_TEMP_FIX	//�ǂ����g��Ȃ����A���̂܂܏����Ă��ǂ� 2009.02.03(��)
static void Debug_CameraMove(GFL_G3D_CAMERA * camera)
{
#ifdef PM_DEBUG
	VecFx32 move = {0,0,0};
	fx32 value = FX32_ONE;
	int add_angle = 64;
	CAMERA_ANGLE angle = {0,0,0,0};
	int mode = 0;
	enum{
		MODE_SHIFT,			//���s�ړ�
		MODE_DISTANCE,		//����
		MODE_ANGLE_REV,		//���]
	};
	
	if((GFL_UI_KEY_GetCont() & PAD_BUTTON_L) && (GFL_UI_KEY_GetCont() & PAD_BUTTON_R)){
		mode = MODE_DISTANCE;
	}
	else if(GFL_UI_KEY_GetCont() & PAD_BUTTON_L){
		mode = MODE_SHIFT;
	}
	else if(GFL_UI_KEY_GetCont() & PAD_BUTTON_R){
		mode = MODE_ANGLE_REV;
	}
	else{
		return;
	}
	
	switch(mode){
	case MODE_SHIFT:
		if(GFL_UI_KEY_GetCont() & PAD_KEY_LEFT){
			move.x -= value;
		}
		if(GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT){
			move.x += value;
		}
		if(GFL_UI_KEY_GetCont() & PAD_KEY_UP){
			move.y += value;
		}
		if(GFL_UI_KEY_GetCont() & PAD_KEY_DOWN){
			move.y -= value;
		}
		if(GFL_UI_KEY_GetCont() & PAD_BUTTON_X){
			move.z -= value;
		}
		if(GFL_UI_KEY_GetCont() & PAD_BUTTON_B){
			move.z += value;
		}
		GFC_ShiftCamera(&move, camera);
		move = GFC_GetCameraPos(camera);
		OS_TPrintf("�J�����ʒu x=%d(16�i:%x), y=%d(16�i:%x), z=%d(16�i:%x)\n", move.x, move.x, move.y, move.y, move.z, move.z);
		break;
	
	case MODE_ANGLE_REV:
		if(GFL_UI_KEY_GetCont() & PAD_KEY_LEFT){
			angle.y -= add_angle;
		}
		if(GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT){
			angle.y += add_angle;
		}
		if(GFL_UI_KEY_GetCont() & PAD_KEY_UP){
			angle.x += add_angle;
		}
		if(GFL_UI_KEY_GetCont() & PAD_KEY_DOWN){
			angle.x -= add_angle;
		}
		if(GFL_UI_KEY_GetCont() & PAD_BUTTON_X){
			angle.z -= add_angle;
		}
		if(GFL_UI_KEY_GetCont() & PAD_BUTTON_B){
			angle.z += add_angle;
		}
		GFC_AddCameraAngleRev(&angle, camera);
		angle = GFC_GetCameraAngle(camera);
		OS_TPrintf("�J�����A���O���@x=%d, y=%d, z=%d\n", angle.x, angle.y, angle.z);
		break;
	case MODE_DISTANCE:
		if(GFL_UI_KEY_GetCont() & PAD_KEY_UP){
			GFC_AddCameraDistance(FX32_ONE, camera);
		}
		if(GFL_UI_KEY_GetCont() & PAD_KEY_DOWN){
			GFC_AddCameraDistance(-FX32_ONE, camera);
		}
		OS_TPrintf("�J����������%d(16�i:%x)\n", GFC_GetCameraDistance(camera), GFC_GetCameraDistance(camera));
		break;
	}
#endif
}
#endif
