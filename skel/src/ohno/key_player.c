
//============================================================================================
/**
 * @file	player.c
 * @brief	DS�Ń��b�V�[�̂��܂��@�v���[���[�v���O����
 * @author	sogabe
 * @date	2007.02.08
 */
//============================================================================================


#include "gflib.h"
#include "procsys.h"
#include "ui.h"

#include "main.h"
#include "key_yt_common.h"
#include "key_fallchr.h"

#include "sample_graphic/yossyegg.naix"

#include "key_player.h"

#define	PAD_BUTTON_ROTATE (PAD_BUTTON_A|PAD_BUTTON_B|PAD_BUTTON_X|PAD_BUTTON_Y)

//----------------------------------------------------------------------------
/**
 *	@brief	�v���[���[�p�����[�^�\����
 */
//-----------------------------------------------------------------------------
struct _PLAYER_PARAM {
	GAME_PARAM			*gp;
	u8					seq_no;			//�V�[�P���X�i���o�[
	u8					player_no;		//�v���[���[�i���o�[
	u8					type;			//�v���[���[�̎��
	u8					line_no;		//���݂��郉�C��
	u8					dir;			//�L�����N�^�̌���
	u8					anm_no;
	u8					pat_no;
	u8					anm_wait;
	void				*chr_data;
	NNSG2dCharacterData *nns_char_data;
    NET_PARAM           *pNetParam;   //�ʐM�p�\����
};

//----------------------------------------------------------------------------
/**
 *	@brief	�v���[���[�`��p�f�[�^�e�[�u��
 */
//-----------------------------------------------------------------------------
//�X�N���[���A�h���X
static	const	u16	PlayerScreenAdrs[][3]={
	//Player1
	{(0x13*0x20+0x03),(0x13*0x20+0x06),(0x13*0x20+0x09)},
	//Player2
	{(0x13*0x20+0x13),(0x13*0x20+0x16),(0x13*0x20+0x19)},
};

static const	u16	PlayerClearScreenData[]={
	0x2002,0x2003,0x2001,0x2002,
	0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,
};

static	const	u16	PlayerChrTransAdrs[]={
	0x20,0x30,
};

#define	KEY_YT_ANIME_STOP		(0xffff)
#define	KEY_YT_ANIME_LOOP		(0xfffe)
#define	KEY_YT_PLAYER_CHR_SIZE	(0x0200)

#define	KEY_YT_PLAYER_CF_0			(KEY_YT_PLAYER_CHR_SIZE*0x00)
#define	KEY_YT_PLAYER_CF2B_0		(KEY_YT_PLAYER_CHR_SIZE*0x01)
#define	KEY_YT_PLAYER_CF2B_1		(KEY_YT_PLAYER_CHR_SIZE*0x02)
#define	KEY_YT_PLAYER_CF2B_2		(KEY_YT_PLAYER_CHR_SIZE*0x03)
#define	KEY_YT_PLAYER_CB_0			(KEY_YT_PLAYER_CHR_SIZE*0x04)
#define	KEY_YT_PLAYER_CB2F_0		(KEY_YT_PLAYER_CHR_SIZE*0x05)
#define	KEY_YT_PLAYER_CB2F_1		(KEY_YT_PLAYER_CHR_SIZE*0x06)
#define	KEY_YT_PLAYER_CB2F_2		(KEY_YT_PLAYER_CHR_SIZE*0x07)

#define	KEY_YT_PLAYER_RF_0			(KEY_YT_PLAYER_CHR_SIZE*0x08)
#define	KEY_YT_PLAYER_RF2B_0		(KEY_YT_PLAYER_CHR_SIZE*0x09)
#define	KEY_YT_PLAYER_RF2B_1		(KEY_YT_PLAYER_CHR_SIZE*0x0a)
#define	KEY_YT_PLAYER_RF2B_2		(KEY_YT_PLAYER_CHR_SIZE*0x0b)
#define	KEY_YT_PLAYER_RB_0			(KEY_YT_PLAYER_CHR_SIZE*0x0c)
#define	KEY_YT_PLAYER_RB2F_0		(KEY_YT_PLAYER_CHR_SIZE*0x0d)
#define	KEY_YT_PLAYER_RB2F_1		(KEY_YT_PLAYER_CHR_SIZE*0x0e)
#define	KEY_YT_PLAYER_RB2F_2		(KEY_YT_PLAYER_CHR_SIZE*0x0f)

#define	KEY_YT_PLAYER_LF_0			(KEY_YT_PLAYER_CHR_SIZE*0x10)
#define	KEY_YT_PLAYER_LF2B_0		(KEY_YT_PLAYER_CHR_SIZE*0x11)
#define	KEY_YT_PLAYER_LF2B_1		(KEY_YT_PLAYER_CHR_SIZE*0x12)
#define	KEY_YT_PLAYER_LF2B_2		(KEY_YT_PLAYER_CHR_SIZE*0x13)
#define	KEY_YT_PLAYER_LB_0			(KEY_YT_PLAYER_CHR_SIZE*0x14)
#define	KEY_YT_PLAYER_LB2F_0		(KEY_YT_PLAYER_CHR_SIZE*0x15)
#define	KEY_YT_PLAYER_LB2F_1		(KEY_YT_PLAYER_CHR_SIZE*0x16)
#define	KEY_YT_PLAYER_LB2F_2		(KEY_YT_PLAYER_CHR_SIZE*0x17)

#define	KEY_YT_PLAYER_CF_1			(KEY_YT_PLAYER_CHR_SIZE*0x18)
#define	KEY_YT_PLAYER_CF_2			(KEY_YT_PLAYER_CHR_SIZE*0x19)
#define	KEY_YT_PLAYER_CB_1			(KEY_YT_PLAYER_CHR_SIZE*0x1a)
#define	KEY_YT_PLAYER_CB_2			(KEY_YT_PLAYER_CHR_SIZE*0x1b)
#define	KEY_YT_PLAYER_RF_1			(KEY_YT_PLAYER_CHR_SIZE*0x1c)
#define	KEY_YT_PLAYER_RF_2			(KEY_YT_PLAYER_CHR_SIZE*0x1d)
#define	KEY_YT_PLAYER_RB_1			(KEY_YT_PLAYER_CHR_SIZE*0x1e)
#define	KEY_YT_PLAYER_RB_2			(KEY_YT_PLAYER_CHR_SIZE*0x1f)
#define	KEY_YT_PLAYER_LF_1			(KEY_YT_PLAYER_CHR_SIZE*0x20)
#define	KEY_YT_PLAYER_LF_2			(KEY_YT_PLAYER_CHR_SIZE*0x21)
#define	KEY_YT_PLAYER_LB_1			(KEY_YT_PLAYER_CHR_SIZE*0x22)
#define	KEY_YT_PLAYER_LB_2			(KEY_YT_PLAYER_CHR_SIZE*0x23)

#define	KEY_YT_PLAYER_CF_PUNCH_C	(KEY_YT_PLAYER_CHR_SIZE*0x24)
#define	KEY_YT_PLAYER_CF_PUNCH_R	(KEY_YT_PLAYER_CHR_SIZE*0x25)
#define	KEY_YT_PLAYER_CF_PUNCH_L	(KEY_YT_PLAYER_CHR_SIZE*0x26)
#define	KEY_YT_PLAYER_CB_PUNCH_C	(KEY_YT_PLAYER_CHR_SIZE*0x27)
#define	KEY_YT_PLAYER_CB_PUNCH_R	(KEY_YT_PLAYER_CHR_SIZE*0x28)
#define	KEY_YT_PLAYER_CB_PUNCH_L	(KEY_YT_PLAYER_CHR_SIZE*0x29)
#define	KEY_YT_PLAYER_RF_PUNCH_C	(KEY_YT_PLAYER_CHR_SIZE*0x2a)
#define	KEY_YT_PLAYER_RF_PUNCH_R	(KEY_YT_PLAYER_CHR_SIZE*0x2b)
#define	KEY_YT_PLAYER_RF_PUNCH_L	(KEY_YT_PLAYER_CHR_SIZE*0x2c)
#define	KEY_YT_PLAYER_RB_PUNCH_C	(KEY_YT_PLAYER_CHR_SIZE*0x2d)
#define	KEY_YT_PLAYER_RB_PUNCH_R	(KEY_YT_PLAYER_CHR_SIZE*0x2e)
#define	KEY_YT_PLAYER_RB_PUNCH_L	(KEY_YT_PLAYER_CHR_SIZE*0x2f)
#define	KEY_YT_PLAYER_LF_PUNCH_C	(KEY_YT_PLAYER_CHR_SIZE*0x30)
#define	KEY_YT_PLAYER_LF_PUNCH_R	(KEY_YT_PLAYER_CHR_SIZE*0x31)
#define	KEY_YT_PLAYER_LF_PUNCH_L	(KEY_YT_PLAYER_CHR_SIZE*0x32)
#define	KEY_YT_PLAYER_LB_PUNCH_C	(KEY_YT_PLAYER_CHR_SIZE*0x33)
#define	KEY_YT_PLAYER_LB_PUNCH_R	(KEY_YT_PLAYER_CHR_SIZE*0x34)
#define	KEY_YT_PLAYER_LB_PUNCH_L	(KEY_YT_PLAYER_CHR_SIZE*0x35)

#define	KEY_YT_PLAYER_LOSE_0		(KEY_YT_PLAYER_CHR_SIZE*0x36)
#define	KEY_YT_PLAYER_LOSE_1		(KEY_YT_PLAYER_CHR_SIZE*0x37)
#define	KEY_YT_PLAYER_WIN_0			(KEY_YT_PLAYER_CHR_SIZE*0x38)
#define	KEY_YT_PLAYER_WIN_1			(KEY_YT_PLAYER_CHR_SIZE*0x39)

typedef struct{
	u16	pat_adrs;
	u16	wait;
}KEY_YT_ANIME_TABLE;

enum{
	KEY_YT_PLAYER_ANM_LF=0,
	KEY_YT_PLAYER_ANM_CF,
	KEY_YT_PLAYER_ANM_RF,
	KEY_YT_PLAYER_ANM_LB,
	KEY_YT_PLAYER_ANM_CB,
	KEY_YT_PLAYER_ANM_RB,
	KEY_YT_PLAYER_ANM_LF2B,
	KEY_YT_PLAYER_ANM_CF2B,
	KEY_YT_PLAYER_ANM_RF2B,
	KEY_YT_PLAYER_ANM_LB2F,
	KEY_YT_PLAYER_ANM_CB2F,
	KEY_YT_PLAYER_ANM_RB2F,
	KEY_YT_PLAYER_ANM_LF_PUNCH_L,
	KEY_YT_PLAYER_ANM_LF_PUNCH_C,
	KEY_YT_PLAYER_ANM_LF_PUNCH_R,
	KEY_YT_PLAYER_ANM_CF_PUNCH_L,
	KEY_YT_PLAYER_ANM_CF_PUNCH_C,
	KEY_YT_PLAYER_ANM_CF_PUNCH_R,
	KEY_YT_PLAYER_ANM_RF_PUNCH_L,
	KEY_YT_PLAYER_ANM_RF_PUNCH_C,
	KEY_YT_PLAYER_ANM_RF_PUNCH_R,
	KEY_YT_PLAYER_ANM_LB_PUNCH_L,
	KEY_YT_PLAYER_ANM_LB_PUNCH_C,
	KEY_YT_PLAYER_ANM_LB_PUNCH_R,
	KEY_YT_PLAYER_ANM_CB_PUNCH_L,
	KEY_YT_PLAYER_ANM_CB_PUNCH_C,
	KEY_YT_PLAYER_ANM_CB_PUNCH_R,
	KEY_YT_PLAYER_ANM_RB_PUNCH_L,
	KEY_YT_PLAYER_ANM_RB_PUNCH_C,
	KEY_YT_PLAYER_ANM_RB_PUNCH_R,
	KEY_YT_PLAYER_ANM_WIN,
	KEY_YT_PLAYER_ANM_LOSE,
	KEY_YT_PLAYER_ANM_MAX
};

#define	KEY_YT_ANIME_LOOP_WAIT	(16)	//�J��Ԃ��n��WAIT
#define	KEY_YT_ANIME_TURN_WAIT	(2)		//���n��WAIT
#define	KEY_YT_ANIME_PUNCH_WAIT	(4)		//�p���`�n��WAIT

//	KEY_YT_PLAYER_ANM_LF
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_lf[]={
	{KEY_YT_PLAYER_LF_0,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_LF_1,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_LF_0,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_LF_2,KEY_YT_ANIME_LOOP_WAIT},
	{0,KEY_YT_ANIME_LOOP},
};
//	KEY_YT_PLAYER_ANM_CF
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_cf[]={
	{KEY_YT_PLAYER_CF_0,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_CF_1,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_CF_0,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_CF_2,KEY_YT_ANIME_LOOP_WAIT},
	{0,KEY_YT_ANIME_LOOP},
};
//	KEY_YT_PLAYER_ANM_RF
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_rf[]={
	{KEY_YT_PLAYER_RF_0,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_RF_1,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_RF_0,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_RF_2,KEY_YT_ANIME_LOOP_WAIT},
	{0,KEY_YT_ANIME_LOOP},
};
//	KEY_YT_PLAYER_ANM_LB
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_lb[]={
	{KEY_YT_PLAYER_LB_0,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_LB_1,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_LB_0,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_LB_2,KEY_YT_ANIME_LOOP_WAIT},
	{0,KEY_YT_ANIME_LOOP},
};
//	KEY_YT_PLAYER_ANM_CB
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_cb[]={
	{KEY_YT_PLAYER_CB_0,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_CB_1,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_CB_0,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_CB_2,KEY_YT_ANIME_LOOP_WAIT},
	{0,KEY_YT_ANIME_LOOP},
};
//	KEY_YT_PLAYER_ANM_RB
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_rb[]={
	{KEY_YT_PLAYER_RB_0,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_RB_1,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_RB_0,KEY_YT_ANIME_LOOP_WAIT},
	{KEY_YT_PLAYER_RB_2,KEY_YT_ANIME_LOOP_WAIT},
	{0,KEY_YT_ANIME_LOOP},
};
//	KEY_YT_PLAYER_ANM_LF2B
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_lf2b[]={
	{KEY_YT_PLAYER_LF2B_0,KEY_YT_ANIME_TURN_WAIT},
	{KEY_YT_PLAYER_LF2B_1,KEY_YT_ANIME_TURN_WAIT},
	{KEY_YT_PLAYER_LF2B_2,KEY_YT_ANIME_TURN_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_CF2B
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_cf2b[]={
	{KEY_YT_PLAYER_CF2B_0,KEY_YT_ANIME_TURN_WAIT},
	{KEY_YT_PLAYER_CF2B_1,KEY_YT_ANIME_TURN_WAIT},
	{KEY_YT_PLAYER_CF2B_2,KEY_YT_ANIME_TURN_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_RF2B
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_rf2b[]={
	{KEY_YT_PLAYER_RF2B_0,KEY_YT_ANIME_TURN_WAIT},
	{KEY_YT_PLAYER_RF2B_1,KEY_YT_ANIME_TURN_WAIT},
	{KEY_YT_PLAYER_RF2B_2,KEY_YT_ANIME_TURN_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_LB2F
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_lb2f[]={
	{KEY_YT_PLAYER_LB2F_0,KEY_YT_ANIME_TURN_WAIT},
	{KEY_YT_PLAYER_LB2F_1,KEY_YT_ANIME_TURN_WAIT},
	{KEY_YT_PLAYER_LB2F_2,KEY_YT_ANIME_TURN_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_CB2F
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_cb2f[]={
	{KEY_YT_PLAYER_CB2F_0,KEY_YT_ANIME_TURN_WAIT},
	{KEY_YT_PLAYER_CB2F_1,KEY_YT_ANIME_TURN_WAIT},
	{KEY_YT_PLAYER_CB2F_2,KEY_YT_ANIME_TURN_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_RB2F
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_rb2f[]={
	{KEY_YT_PLAYER_RB2F_0,KEY_YT_ANIME_TURN_WAIT},
	{KEY_YT_PLAYER_RB2F_1,KEY_YT_ANIME_TURN_WAIT},
	{KEY_YT_PLAYER_RB2F_2,KEY_YT_ANIME_TURN_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_LF_PUNCH_L
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_lf_punch_l[]={
	{KEY_YT_PLAYER_LF_PUNCH_L,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_LF_PUNCH_C
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_lf_punch_c[]={
	{KEY_YT_PLAYER_LF_PUNCH_C,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_LF_PUNCH_R
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_lf_punch_r[]={
	{KEY_YT_PLAYER_LF_PUNCH_R,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_CF_PUNCH_L
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_cf_punch_l[]={
	{KEY_YT_PLAYER_CF_PUNCH_L,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_CF_PUNCH_C
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_cf_punch_c[]={
	{KEY_YT_PLAYER_CF_PUNCH_C,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_CF_PUNCH_R
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_cf_punch_r[]={
	{KEY_YT_PLAYER_CF_PUNCH_R,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_RF_PUNCH_L
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_rf_punch_l[]={
	{KEY_YT_PLAYER_RF_PUNCH_L,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_RF_PUNCH_C
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_rf_punch_c[]={
	{KEY_YT_PLAYER_RF_PUNCH_C,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_RF_PUNCH_R
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_rf_punch_r[]={
	{KEY_YT_PLAYER_RF_PUNCH_R,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_LB_PUNCH_L
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_lb_punch_l[]={
	{KEY_YT_PLAYER_LB_PUNCH_L,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_LB_PUNCH_C
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_lb_punch_c[]={
	{KEY_YT_PLAYER_LB_PUNCH_C,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_LB_PUNCH_R
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_lb_punch_r[]={
	{KEY_YT_PLAYER_LB_PUNCH_R,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_CB_PUNCH_L
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_cb_punch_l[]={
	{KEY_YT_PLAYER_CB_PUNCH_L,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_CB_PUNCH_C
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_cb_punch_c[]={
	{KEY_YT_PLAYER_CB_PUNCH_C,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_CB_PUNCH_R
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_cb_punch_r[]={
	{KEY_YT_PLAYER_CB_PUNCH_R,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_RB_PUNCH_L
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_rb_punch_l[]={
	{KEY_YT_PLAYER_CB_PUNCH_L,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_RB_PUNCH_C
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_rb_punch_c[]={
	{KEY_YT_PLAYER_RB_PUNCH_C,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_RB_PUNCH_R
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_rb_punch_r[]={
	{KEY_YT_PLAYER_RB_PUNCH_R,KEY_YT_ANIME_PUNCH_WAIT},
	{0,KEY_YT_ANIME_STOP},
};
//	KEY_YT_PLAYER_ANM_WIN
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_win[]={
	{KEY_YT_PLAYER_WIN_1,12},
	{KEY_YT_PLAYER_WIN_0,12},
	{KEY_YT_PLAYER_WIN_1,12},
	{KEY_YT_PLAYER_WIN_0,12},
	{KEY_YT_PLAYER_WIN_1,12},
	{KEY_YT_PLAYER_WIN_0,32},
	{0,KEY_YT_ANIME_LOOP},
};
//	KEY_YT_PLAYER_ANM_LOSE
static	const	KEY_YT_ANIME_TABLE	yt_player_anm_lose[]={
	{KEY_YT_PLAYER_LOSE_0,8},
	{KEY_YT_PLAYER_LOSE_1,8},
	{KEY_YT_PLAYER_LOSE_0,8},
	{KEY_YT_PLAYER_LOSE_1,8},
	{KEY_YT_PLAYER_LOSE_0,8},
	{KEY_YT_PLAYER_LOSE_1,32},
	{0,KEY_YT_ANIME_LOOP},
};

static	const	KEY_YT_ANIME_TABLE	* const yt_anime_table[]={
//	KEY_YT_PLAYER_ANM_LF
	yt_player_anm_lf,
//	KEY_YT_PLAYER_ANM_CF
	yt_player_anm_cf,
//	KEY_YT_PLAYER_ANM_RF
	yt_player_anm_rf,
//	KEY_YT_PLAYER_ANM_LB
	yt_player_anm_lb,
//	KEY_YT_PLAYER_ANM_CB
	yt_player_anm_cb,
//	KEY_YT_PLAYER_ANM_RB
	yt_player_anm_rb,
//	KEY_YT_PLAYER_ANM_LF2B
	yt_player_anm_lf2b,
//	KEY_YT_PLAYER_ANM_CF2B
	yt_player_anm_cf2b,
//	KEY_YT_PLAYER_ANM_RF2B
	yt_player_anm_rf2b,
//	KEY_YT_PLAYER_ANM_LB2F
	yt_player_anm_lb2f,
//	KEY_YT_PLAYER_ANM_CB2F
	yt_player_anm_cb2f,
//	KEY_YT_PLAYER_ANM_RB2F
	yt_player_anm_rb2f,
//	KEY_YT_PLAYER_ANM_LF_PUNCH_L
	yt_player_anm_lf_punch_l,
//	KEY_YT_PLAYER_ANM_LF_PUNCH_C
	yt_player_anm_lf_punch_c,
//	KEY_YT_PLAYER_ANM_LF_PUNCH_R
	yt_player_anm_lf_punch_r,
//	KEY_YT_PLAYER_ANM_CF_PUNCH_L
	yt_player_anm_cf_punch_l,
//	KEY_YT_PLAYER_ANM_CF_PUNCH_C
	yt_player_anm_cf_punch_c,
//	KEY_YT_PLAYER_ANM_CF_PUNCH_R
	yt_player_anm_cf_punch_r,
//	KEY_YT_PLAYER_ANM_RF_PUNCH_L
	yt_player_anm_rf_punch_l,
//	KEY_YT_PLAYER_ANM_RF_PUNCH_C
	yt_player_anm_rf_punch_c,
//	KEY_YT_PLAYER_ANM_RF_PUNCH_R
	yt_player_anm_rf_punch_r,
//	KEY_YT_PLAYER_ANM_LB_PUNCH_L
	yt_player_anm_lb_punch_l,
//	KEY_YT_PLAYER_ANM_LB_PUNCH_C
	yt_player_anm_lb_punch_c,
//	KEY_YT_PLAYER_ANM_LB_PUNCH_R
	yt_player_anm_lb_punch_r,
//	KEY_YT_PLAYER_ANM_CB_PUNCH_L
	yt_player_anm_cb_punch_l,
//	KEY_YT_PLAYER_ANM_CB_PUNCH_C
	yt_player_anm_cb_punch_c,
//	KEY_YT_PLAYER_ANM_CB_PUNCH_R
	yt_player_anm_cb_punch_r,
//	KEY_YT_PLAYER_ANM_RB_PUNCH_L
	yt_player_anm_rb_punch_l,
//	KEY_YT_PLAYER_ANM_RB_PUNCH_C
	yt_player_anm_rb_punch_c,
//	KEY_YT_PLAYER_ANM_RB_PUNCH_R
	yt_player_anm_rb_punch_r,
//	KEY_YT_PLAYER_ANM_WIN
	yt_player_anm_win,
//	KEY_YT_PLAYER_ANM_LOSE
	yt_player_anm_lose,
};

//----------------------------------------------------------------------------
/**
 *	@brief	�v���[���[�A�N�V�����i���o�[
 */
//-----------------------------------------------------------------------------
enum{
	KEY_YT_PLAYER_ACT_MOVE_L=0,
	KEY_YT_PLAYER_ACT_MOVE_R,
	KEY_YT_PLAYER_ACT_ROTATE,
	KEY_YT_PLAYER_ACT_OVERTURN_L,
	KEY_YT_PLAYER_ACT_OVERTURN_R,
	KEY_YT_PLAYER_ACT_OVERTURN_C,
	KEY_YT_PLAYER_ACT_MAX
};

static	void	KEY_YT_MainPlayer(GFL_TCB *tcb,void *work);
static	u8		KEY_YT_PlayerActGet(PLAYER_PARAM *pp);
static	void	KEY_YT_PlayerAnimeSet(PLAYER_PARAM *pp,int anm_no);
static	void	KEY_YT_PlayerAnimeMain(PLAYER_PARAM *pp);
static	BOOL	KEY_YT_PlayerAnimeCheck(PLAYER_PARAM *pp);
static	void	KEY_YT_PlayerChrTrans(PLAYER_PARAM *pp);
static	void	KEY_YT_PlayerScreenMake(PLAYER_PARAM *pp,u8 old_line_no,u8 new_line_no);
static	void	KEY_YT_PlayerRotateScreenMake(PLAYER_PARAM *pp,u8 flag);
static	void	KEY_YT_PlayerRotateAct(PLAYER_PARAM *pp,KEY_YT_PLAYER_STATUS *ps);
static	void	KEY_YT_PlayerOverTurnAct(PLAYER_PARAM *pp,KEY_YT_PLAYER_STATUS *ps,int flag);

//----------------------------------------------------------------------------
/**
 *	@brief	�v���[���[������
 *	
 *	@param	gp			�Q�[���p�����[�^�|�C���^
 *	@param	player_no	1P or 2P
 *	@param	type		�v���[���[�^�C�v
 *  @retval PLAYER_PARAM
 */
//-----------------------------------------------------------------------------
PLAYER_PARAM* KEY_YT_InitPlayer(GAME_PARAM *gp,u8 player_no,u8 type,u8 bNetSend)
{
	PLAYER_PARAM *pp=(PLAYER_PARAM *)GFL_HEAP_AllocMemory(gp->heapID,sizeof(PLAYER_PARAM));

	pp->gp=gp;
	pp->seq_no=0;
	pp->player_no=player_no;
	pp->type=type;
	pp->line_no=1;
	pp->dir=0;
    pp->pNetParam = (bNetSend==TRUE) ? gp->pNetParam : NULL;

	//�L�����N�^�f�[�^�W�J
	pp->chr_data=GFL_ARC_UTIL_LoadCharacter(0,NARC_yossyegg_YT_MARIO_NCGR,0,&pp->nns_char_data,gp->heapID);

	KEY_YT_PlayerScreenMake(pp,pp->line_no,pp->line_no);

	KEY_YT_PlayerAnimeSet(pp,KEY_YT_PLAYER_ANM_LF+pp->line_no);
    return pp;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���[���[���^�X�N�ɓo�^
 *	
 *	@param	gp			�Q�[���p�����[�^�|�C���^
 *	@param	type		�v���[���[�^�C�v
 *	@param	player_no	1P or 2P
 *  @retval void
 */
//-----------------------------------------------------------------------------
void KEY_YT_InitPlayerAddTask(GAME_PARAM *gp, PLAYER_PARAM* pp,u8 player_no)
{
    gp->ps[player_no].tcb_player = GFL_TCB_AddTask(gp->tcbsys,
                                                   KEY_YT_MainPlayer,pp,
                                                   TCB_PRI_PLAYER);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���[���[������
 *	
 *	@param	gp			�Q�[���p�����[�^�|�C���^
 *	@param	player_no	1P or 2P
 *	@param	type		�v���[���[�^�C�v
 */
//-----------------------------------------------------------------------------
enum{
	SEQ_PLAYER_ACT_CHECK=0,		//�v���[���[�̓���`�F�b�N
	SEQ_PLAYER_ROTATE,
	SEQ_PLAYER_OVERTURN,
	SEQ_PLAYER_WIN_INIT,
	SEQ_PLAYER_LOSE_INIT,
	SEQ_PLAYER_WIN_LOSE,

	OVER_TURN_L=1,
	OVER_TURN_R=2,
};
static	void	KEY_YT_MainPlayer(GFL_TCB *tcb,void *work)
{
	PLAYER_PARAM		*pp=(PLAYER_PARAM *)work;
	KEY_YT_PLAYER_STATUS	*ps=&pp->gp->ps[pp->player_no];
    int actno=0;

	KEY_YT_PlayerAnimeMain(pp);

	if(pp->seq_no!=SEQ_PLAYER_WIN_LOSE){
		switch(ps->status.win_lose_flag){
		case KEY_YT_GAME_WIN:
			pp->seq_no=SEQ_PLAYER_WIN_INIT;
			GFL_SOUND_PlayBGM(SEQ_CLEAR);
			break;
		case KEY_YT_GAME_LOSE:
		case KEY_YT_GAME_DRAW:
			pp->seq_no=SEQ_PLAYER_LOSE_INIT;
			GFL_SOUND_PlayBGM(SEQ_GAMEOVER);
			break;
		}
	}

	switch(pp->seq_no){
	case SEQ_PLAYER_ACT_CHECK:
        actno = KEY_YT_PlayerActGet(pp);
		switch(actno){
		case KEY_YT_PLAYER_ACT_MOVE_L:
			if(pp->line_no){
				KEY_YT_PlayerScreenMake(pp,pp->line_no,pp->line_no-1);
				KEY_YT_PlayerAnimeSet(pp,KEY_YT_PLAYER_ANM_LF+pp->line_no+3*pp->dir);
			}
			break;
		case KEY_YT_PLAYER_ACT_MOVE_R:
			if(pp->line_no<2){
				KEY_YT_PlayerScreenMake(pp,pp->line_no,pp->line_no+1);
				KEY_YT_PlayerAnimeSet(pp,KEY_YT_PLAYER_ANM_LF+pp->line_no+3*pp->dir);
			}
			break;
		case KEY_YT_PLAYER_ACT_ROTATE:
			if(ps->status.no_active_flag==0){
				KEY_YT_PlayerAnimeSet(pp,KEY_YT_PLAYER_ANM_LF2B+pp->line_no+3*pp->dir);
				KEY_YT_PlayerRotateScreenMake(pp,0);
				pp->seq_no=SEQ_PLAYER_ROTATE;
				KEY_YT_NET_PlaySE(SE_ROTATE,pp->pNetParam);
				KEY_YT_PlayerRotateAct(pp,ps);
			}
			break;
		case KEY_YT_PLAYER_ACT_OVERTURN_L:
			if(ps->status.no_active_flag==0){
				KEY_YT_PlayerAnimeSet(pp,KEY_YT_PLAYER_ANM_LF_PUNCH_L+3*pp->line_no+9*pp->dir);
				pp->seq_no=SEQ_PLAYER_OVERTURN;
				KEY_YT_NET_PlaySE(SE_OVERTURN,pp->pNetParam);
				KEY_YT_PlayerOverTurnAct(pp,ps,OVER_TURN_L);
			}
			break;
		case KEY_YT_PLAYER_ACT_OVERTURN_R:
			if(ps->status.no_active_flag==0){
				KEY_YT_PlayerAnimeSet(pp,KEY_YT_PLAYER_ANM_LF_PUNCH_R+3*pp->line_no+9*pp->dir);
				pp->seq_no=SEQ_PLAYER_OVERTURN;
				KEY_YT_NET_PlaySE(SE_OVERTURN,pp->pNetParam);
				KEY_YT_PlayerOverTurnAct(pp,ps,OVER_TURN_R);
			}
			break;
		case KEY_YT_PLAYER_ACT_OVERTURN_C:
			if(ps->status.no_active_flag==0){
				KEY_YT_PlayerAnimeSet(pp,KEY_YT_PLAYER_ANM_LF_PUNCH_C+3*pp->line_no+9*pp->dir);
				pp->seq_no=SEQ_PLAYER_OVERTURN;
				KEY_YT_NET_PlaySE(SE_OVERTURN,pp->pNetParam);
				KEY_YT_PlayerOverTurnAct(pp,ps,OVER_TURN_L|OVER_TURN_R);
			}
			break;
		default:
			break;
		}
		break;
	case SEQ_PLAYER_ROTATE:
		switch(KEY_YT_PlayerActGet(pp)){
		case KEY_YT_PLAYER_ACT_MOVE_L:
			if(pp->line_no){
				pp->dir^=1;
				KEY_YT_PlayerRotateScreenMake(pp,1);
				KEY_YT_PlayerScreenMake(pp,pp->line_no,pp->line_no-1);
				KEY_YT_PlayerAnimeSet(pp,KEY_YT_PLAYER_ANM_LF+pp->line_no+3*pp->dir);
				pp->seq_no=SEQ_PLAYER_ACT_CHECK;
			}
			break;
		case KEY_YT_PLAYER_ACT_MOVE_R:
			if(pp->line_no<2){
				pp->dir^=1;
				KEY_YT_PlayerRotateScreenMake(pp,1);
				KEY_YT_PlayerScreenMake(pp,pp->line_no,pp->line_no+1);
				KEY_YT_PlayerAnimeSet(pp,KEY_YT_PLAYER_ANM_LF+pp->line_no+3*pp->dir);
				pp->seq_no=SEQ_PLAYER_ACT_CHECK;
			}
			break;
		default:
			if(KEY_YT_PlayerAnimeCheck(pp)==FALSE){
				pp->dir^=1;
				KEY_YT_PlayerAnimeSet(pp,KEY_YT_PLAYER_ANM_LF+pp->line_no+3*pp->dir);
				KEY_YT_PlayerRotateScreenMake(pp,1);
				pp->seq_no=SEQ_PLAYER_ACT_CHECK;
			}
		}
		break;
	case SEQ_PLAYER_OVERTURN:
		if(KEY_YT_PlayerAnimeCheck(pp)==FALSE){
			KEY_YT_PlayerAnimeSet(pp,KEY_YT_PLAYER_ANM_LF+pp->line_no+3*pp->dir);
			pp->seq_no=SEQ_PLAYER_ACT_CHECK;
		}
		break;
	case SEQ_PLAYER_WIN_INIT:
			KEY_YT_PlayerAnimeSet(pp,KEY_YT_PLAYER_ANM_WIN);
			pp->seq_no=SEQ_PLAYER_WIN_LOSE;
		break;
	case SEQ_PLAYER_LOSE_INIT:
			KEY_YT_PlayerAnimeSet(pp,KEY_YT_PLAYER_ANM_LOSE);
			pp->seq_no=SEQ_PLAYER_WIN_LOSE;
		break;
	case SEQ_PLAYER_WIN_LOSE:
		if(pp->gp->seq_no==SEQ_GAME_TO_TITLE){
			GFL_HEAP_FreeMemory(pp->chr_data);
			GFL_HEAP_FreeMemory(work);
			GFL_TCB_DeleteTask(tcb);
		}
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���[���[��������擾
 *	
 *	@param	pp			�v���[���[�p�����[�^�|�C���^
 */
//-----------------------------------------------------------------------------
static	u8		KEY_YT_PlayerActGet(PLAYER_PARAM *pp)
{
	//�L�[���瓮���I��
	if( GFL_NET_KEY_GetTrg(pp->player_no) & PAD_KEY_LEFT ){
		return KEY_YT_PLAYER_ACT_MOVE_L;
	}
	if( GFL_NET_KEY_GetTrg(pp->player_no) & PAD_KEY_RIGHT ){
		return KEY_YT_PLAYER_ACT_MOVE_R;
	}
	if( GFL_NET_KEY_GetTrg(pp->player_no) & PAD_BUTTON_ROTATE ){
		return KEY_YT_PLAYER_ACT_ROTATE;
	}
	if( GFL_NET_KEY_GetTrg(pp->player_no) & PAD_BUTTON_L ){
		return KEY_YT_PLAYER_ACT_OVERTURN_L;
	}
	if( GFL_NET_KEY_GetTrg(pp->player_no) & PAD_BUTTON_R ){
		return KEY_YT_PLAYER_ACT_OVERTURN_R;
	}
	if( GFL_NET_KEY_GetTrg(pp->player_no) & PAD_KEY_UP ){
		return KEY_YT_PLAYER_ACT_OVERTURN_C;
	}
	return KEY_YT_PLAYER_ACT_MAX;
}



//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM�ł����v���[���[�A�j�����Z�b�g
 *	
 *	@param	pp		�v���[���[�p�����[�^�|�C���^
 *	@param	anm_no	�Z�b�g����A�j���i���o�[
 */
//-----------------------------------------------------------------------------
void KEY_YT_NetPlayerChrTrans(GAME_PARAM *gp,PLAYER_PARAM *pp,int player_no,int anm_no,int line_no,int rot,int pat_no)
{
	const	KEY_YT_ANIME_TABLE	* yat=yt_anime_table[anm_no];

	pp->anm_no=anm_no;
	pp->pat_no=pat_no;
	pp->anm_wait=yat[0].wait;
    pp->gp=gp;
    pp->seq_no=0;
    pp->player_no = player_no;
    pp->type = player_no;
    pp->line_no = line_no;
    pp->dir = rot;
	KEY_YT_PlayerChrTrans(pp);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���[���[�A�j�����Z�b�g
 *	
 *	@param	pp		�v���[���[�p�����[�^�|�C���^
 *	@param	anm_no	�Z�b�g����A�j���i���o�[
 */
//-----------------------------------------------------------------------------
static void	KEY_YT_PlayerAnimeSet(PLAYER_PARAM *pp,int anm_no)
{
	const	KEY_YT_ANIME_TABLE	* yat=yt_anime_table[anm_no];

	pp->anm_no=anm_no;
	pp->pat_no=0;
	pp->anm_wait=yat[0].wait;

	KEY_YT_PlayerChrTrans(pp);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���[���[�A�j�����C������
 *	
 *	@param	pp			�v���[���[�p�����[�^�|�C���^
 */
//-----------------------------------------------------------------------------
static	void	KEY_YT_PlayerAnimeMain(PLAYER_PARAM *pp)
{
	if(pp->anm_no<KEY_YT_PLAYER_ANM_MAX){
		if(pp->anm_wait){
			pp->anm_wait--;
		}
		else{
			{
				const	KEY_YT_ANIME_TABLE	* yat=yt_anime_table[pp->anm_no];

				pp->pat_no++;
				switch(yat[pp->pat_no].wait){
				//�A�j���I��
				case KEY_YT_ANIME_STOP:
					pp->anm_no=KEY_YT_PLAYER_ANM_MAX;
					break;
				//�A�j�����[�v
				case KEY_YT_ANIME_LOOP:
					pp->pat_no=0;
				default:
					pp->anm_wait=yat[pp->pat_no].wait;
					KEY_YT_PlayerChrTrans(pp);
					break;
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���[���[�A�j�������삵�Ă��邩�`�F�b�N
 *	
 *	@param	pp			�v���[���[�p�����[�^�|�C���^
 *
 *	@retval	TRUE:���쒆�@FALSE:�I��
 */
//-----------------------------------------------------------------------------
static	BOOL	KEY_YT_PlayerAnimeCheck(PLAYER_PARAM *pp)
{
	return (pp->anm_no<KEY_YT_PLAYER_ANM_MAX);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���[���[�L�����]��
 *	
 *	@param	pp			�v���[���[�p�����[�^�|�C���^
 *
 *	@retval	TRUE:���쒆�@FALSE:�I��
 */
//-----------------------------------------------------------------------------
static	void	KEY_YT_PlayerChrTrans(PLAYER_PARAM *pp)
{
	const	KEY_YT_ANIME_TABLE	* yat=yt_anime_table[pp->anm_no];
	u8		*buf=pp->nns_char_data->pRawData;
	
	buf+=yat[pp->pat_no].pat_adrs;

	GFL_BG_LoadCharacter( KEY_YT_PLAYER_FRAME, buf, KEY_YT_PLAYER_CHR_SIZE, PlayerChrTransAdrs[pp->player_no] );

    // �ʐM�̂Ƃ��͏ꏊ�𑗂�
    if(pp->pNetParam){
        KEY_YT_NET_SendPlayerAnmReq(pp->player_no, pp->anm_no, pp->pat_no, pp->line_no, pp->dir, pp->pNetParam);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���[���[�\���p�X�N���[������
 *	
 *	@param	pp			�v���[���[�p�����[�^�|�C���^
 *	@param	old_line_no	���݂̃v���[���[�̃��C���ʒu
 *	@param	new_line_no	�V���ȃv���[���[�̃��C���ʒu
 */
//-----------------------------------------------------------------------------
static void	KEY_YT_PlayerScreenMake(PLAYER_PARAM *pp,u8 old_line_no,u8 new_line_no)
{
	int	x,y;
	u16	chr_no=0;
	u16	*scr_adrs=(u16 *)GFL_BG_GetScreenBufferAdrs( KEY_YT_PLAYER_FRAME );

	if(old_line_no!=new_line_no){
		//���݈ʒu�̃X�N���[���f�[�^������
		for(y=0;y<4;y++){
			for(x=0;x<4;x++){
				scr_adrs[PlayerScreenAdrs[pp->player_no][old_line_no]+y*0x20+x]=PlayerClearScreenData[chr_no];
				chr_no++;
			}
		}
	}

	chr_no=(KEY_YT_PLAYER_CHRNO+0x10*pp->player_no)|KEY_YT_PLAYER_PALNO;

	//�X�N���[���f�[�^�𐶐�
	for(y=0;y<4;y++){
		for(x=0;x<4;x++){
			scr_adrs[PlayerScreenAdrs[pp->player_no][new_line_no]+y*0x20+x]=chr_no;
			chr_no++;
		}
	}

	GFL_BG_LoadScreenV_Req( KEY_YT_PLAYER_FRAME );

	pp->line_no=new_line_no;

    // �ʐM�̎��ɂ͑���
    if(pp->pNetParam){
        KEY_YT_NET_SendPlayerScreenMake(pp->player_no, old_line_no, new_line_no, pp->pNetParam);
    }
}


//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM�p �v���[���[�\���p�X�N���[������
 *	
 *	@param	pp			�v���[���[�p�����[�^�|�C���^
 *	@param	old_line_no	���݂̃v���[���[�̃��C���ʒu
 *	@param	new_line_no	�V���ȃv���[���[�̃��C���ʒu
 */
//-----------------------------------------------------------------------------

void KEY_YT_PlayerScreenMakeNetFunc(PLAYER_PARAM *pp, u8 player_no, u8 old_line_no,u8 new_line_no)
{
    pp->player_no = player_no;
    KEY_YT_PlayerScreenMake(pp, old_line_no, new_line_no);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���[���[�\���p�X�N���[�������i�^�[������Ƃ��j
 *	
 *	@param	pp		�v���[���[�p�����[�^�|�C���^
 *	@param	flag	0:�^�[���J�n���@1:�^�[���I����
 */
//-----------------------------------------------------------------------------
static	void	KEY_YT_PlayerRotateScreenMake(PLAYER_PARAM *pp,u8 flag)
{
	u16	*scr_adrs=(u16 *)GFL_BG_GetScreenBufferAdrs( KEY_YT_PLAYER_FRAME );

	if(flag){
		scr_adrs[PlayerScreenAdrs[pp->player_no][pp->line_no]-1]=0x2001;
		scr_adrs[PlayerScreenAdrs[pp->player_no][pp->line_no]+4]=0x2003;
	}
	else{
		scr_adrs[PlayerScreenAdrs[pp->player_no][pp->line_no]-1]=0;
		scr_adrs[PlayerScreenAdrs[pp->player_no][pp->line_no]+4]=0;
	}

	GFL_BG_LoadScreenV_Req( KEY_YT_PLAYER_FRAME );

    // �ʐM�̎��ɂ͑���
    if(pp->pNetParam){
        KEY_YT_NET_SendPlayerRotateScreenMake(pp->player_no, pp->line_no,flag, pp->pNetParam);
    }

}


//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM�p �v���[���[�\���p�X�N���[�������i�^�[������Ƃ��j
 *	
 *	@param	pp		�v���[���[�p�����[�^�|�C���^
 *	@param	flag	0:�^�[���J�n���@1:�^�[���I����
 */
//-----------------------------------------------------------------------------
void	KEY_YT_PlayerRotateScreenMakeNetFunc(PLAYER_PARAM *pp, u8 player_no, u8 line_no, u8 flag)
{
    pp->player_no = player_no;
    pp->line_no = line_no;

    KEY_YT_PlayerRotateScreenMake(pp,flag);
}

//----------------------------------------------------------------------------
/**
 *	@brief	��]����
 *	
 *	@param	pp		�v���[���[�p�����[�^�|�C���^
 *	@param	ps		�v���[���[�X�e�[�^�X
 */
//-----------------------------------------------------------------------------
static	void	KEY_YT_PlayerRotateAct(PLAYER_PARAM *pp,KEY_YT_PLAYER_STATUS *ps)
{
	int				i;
	int				left_line;
	int				right_line;
	int				left_height=0;
	int				right_height=0;
	FALL_CHR_PARAM	*fcp;
	int				height_tbl[]={144,130,116,102,88,74,60,46,32};
	GFL_CLACTPOS	pos;

	ps->status.rotate_flag=pp->line_no+1;

	left_line=ps->stoptbl[pp->line_no];
	right_line=ps->stoptbl[pp->line_no+1];
	ps->stoptbl[pp->line_no+1]=left_line;
	ps->stoptbl[pp->line_no]=right_line;

	for(i=0;i<KEY_YT_HEIGHT_MAX;i++){
		fcp=ps->stop[left_line][i];
		if(fcp){
			fcp->rotate_flag=ps->status.rotate_flag;
			fcp->wait_value=i;
			left_height++;
		}
		fcp=ps->stop[right_line][i];
		if(fcp){
			fcp->rotate_flag=ps->status.rotate_flag;
			fcp->wait_value=i;
			right_height++;
		}
	}

	left_line=ps->falltbl[pp->line_no];
	right_line=ps->falltbl[pp->line_no+1];
	if(left_height!=right_height){
		if(left_height>right_height){
			for(i=0;i<KEY_YT_HEIGHT_MAX;i++){
				fcp=ps->fall[right_line][i];
				if(fcp){
					GFL_CLACT_WK_GetWldPos(fcp->clwk,&pos);
					if(pos.y>height_tbl[left_height]){
						fcp->rotate_flag=ps->status.rotate_flag;
						ps->falltbl[pp->line_no+1]=left_line;
						ps->falltbl[pp->line_no]=right_line;
						break;
					}
				}
			}
		}
		else{
			for(i=0;i<KEY_YT_HEIGHT_MAX;i++){
				fcp=ps->fall[left_line][i];
				if(fcp){
					GFL_CLACT_WK_GetWldPos(fcp->clwk,&pos);
					if(pos.y>height_tbl[right_height]){
						fcp->rotate_flag=ps->status.rotate_flag;
						ps->falltbl[pp->line_no+1]=left_line;
						ps->falltbl[pp->line_no]=right_line;
						break;
					}
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Ђ�����Ԃ�����
 *	
 *	@param	pp		�v���[���[�p�����[�^�|�C���^
 *	@param	ps		�v���[���[�X�e�[�^�X
 *	@param	flag	�Ђ�����Ԃ����C���t���O
 */
//-----------------------------------------------------------------------------
static	void	KEY_YT_PlayerOverTurnAct(PLAYER_PARAM *pp,KEY_YT_PLAYER_STATUS *ps,int flag)
{
	int				i;
	int				left_line;
	int				right_line;
	FALL_CHR_PARAM	*fcp;
	int				height_tbl[]={144,130,116,102,88,74,60,46,32};

	left_line=ps->stoptbl[pp->line_no];
	right_line=ps->stoptbl[pp->line_no+1];

	if(flag&OVER_TURN_L){
		ps->status.overturn_flag|=(1<<pp->line_no);
	}
	if(flag&OVER_TURN_R){
		ps->status.overturn_flag|=(1<<(pp->line_no+1));
	}

	for(i=0;i<KEY_YT_HEIGHT_MAX;i++){
		if(flag&OVER_TURN_L){
			fcp=ps->stop[left_line][i];
			if(fcp){
				fcp->overturn_flag=1;
				fcp->offset_pos.y=-1;
			}
		}
		if(flag&OVER_TURN_R){
			fcp=ps->stop[right_line][i];
			if(fcp){
				fcp->overturn_flag=1;
				fcp->offset_pos.y=-1;
			}
		}
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	���������t���O�𗧂Ă�
 *	@param	pp		�v���[���[�p�����[�^�|�C���^
 *	@param	bWin	�������ꍇ��TRUE
 */
//-----------------------------------------------------------------------------

void KEY_YT_PlayerSetWinLoseFlag(KEY_YT_PLAYER_STATUS *ps, int bWin)
{
    if(bWin){
        ps->status.win_lose_flag|=KEY_YT_GAME_WIN;
    }
    else{
        ps->status.win_lose_flag|=KEY_YT_GAME_LOSE;
    }
}

