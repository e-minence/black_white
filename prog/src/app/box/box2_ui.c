//============================================================================================
/**
 * @file		box2_ui.c
 * @brief		�{�b�N�X��� �L�[�^�b�`�֘A
 * @author	Hiroyuki Nakamura
 * @date		09.10.05
 *
 *	���W���[�����FBOX2_UI
 */
//============================================================================================
#include <gflib.h>

#include "system/cursor_move.h"

#include "system/main.h"
#include "ui/touchbar.h"

#include "box2_main.h"
#include "box2_bmp.h"
#include "box2_obj.h"
#include "box2_ui.h"
#include "box2_bgwfrm.h"


//============================================================================================
//	�萔��`
//============================================================================================

// �J�[�\���ړ��f�[�^
typedef struct {
//	const GFL_UI_TP_HITTBL * hit;
//	const CURSORMOVE_DATA * pos;
	const CURSORMOVE_DATA * dat;
	const CURSORMOVE_CALLBACK * cb;
}MOVE_DATA_TBL;

#define	TRAYPOKE_SIZE_X		( BOX2OBJ_TRAYPOKE_SX )							// �g���C�̃|�P�����̃^�b�`�͈̓T�C�Y�w
#define	TRAYPOKE_SIZE_Y		( 24 )											// �g���C�̃|�P�����̃^�b�`�͈̓T�C�Y�x
#define	TRAYPOKE_START_X	( BOX2OBJ_TRAYPOKE_PX - TRAYPOKE_SIZE_X / 2 )	// �g���C�̃|�P�����̃^�b�`�J�n���W�w
#define	TRAYPOKE_START_Y	( 40 )											// �g���C�̃|�P�����̃^�b�`�J�n���W�x

#define	TRAYPOKE_PX(a)	( TRAYPOKE_START_X + TRAYPOKE_SIZE_X * a )		// �g���C�̃|�P�����̃^�b�`�J�n���W�w
#define	TRAYPOKE_PY(a)	( TRAYPOKE_START_Y + TRAYPOKE_SIZE_Y * a )		// �g���C�̃|�P�����̃^�b�`�J�n���W�x

#define	PICUR_X		( 12 )	// �J�[�\���\���␳�w���W
#define	PICUR_Y		( -4 )	// �J�[�\���\���␳�x���W


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void CursorObjMove( BOX2_SYS_WORK * syswk, int pos );

static void CursorMoveCallBack_On( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Off( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Move( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Touch( void * work, int now_pos, int old_pos );

static void PartyOutMainCallBack_On( void * work, int now_pos, int old_pos );

static void PartyOutBoxSelCallBack_On( void * work, int now_pos, int old_pos );
static void PartyOutBoxSelCallBack_Move( void * work, int now_pos, int old_pos );
static void PartyOutBoxSelCallBack_Touch( void * work, int now_pos, int old_pos );

static void PartyInMainCallBack_On( void * work, int now_pos, int old_pos );
static void PartyInMainCallBack_Touch( void * work, int now_pos, int old_pos );

static void BoxArrangeMainCallBack_On( void * work, int now_pos, int old_pos );
static void BoxArrangeMainCallBack_Touch( void * work, int now_pos, int old_pos );
static void BoxArrangeMainCallBack_Move( void * work, int now_pos, int old_pos );

static void BoxArrangePokeMoveCallBack_On( void * work, int now_pos, int old_pos );
static void BoxArrangePokeMoveCallBack_Move( void * work, int now_pos, int old_pos );
static void BoxArrangePokeMoveCallBack_Touch( void * work, int now_pos, int old_pos );

static void BoxArrangePartyPokeMoveCallBack_On( void * work, int now_pos, int old_pos );
static void BoxArrangePartyPokeMoveCallBack_Move( void * work, int now_pos, int old_pos );
static void BoxArrangePartyPokeMoveCallBack_Touch( void * work, int now_pos, int old_pos );

static void BoxArrangePartyMoveCallBack_On( void * work, int now_pos, int old_pos );
static void BoxArrangePartyMoveCallBack_Move( void * work, int now_pos, int old_pos );

static void BoxItemArrangeMainCallBack_On( void * work, int now_pos, int old_pos );
static void BoxItemArrangeMainCallBack_Touch( void * work, int now_pos, int old_pos );
static void BoxItemArrangeMainCallBack_Move( void * work, int now_pos, int old_pos );

static void BoxItemArrangePartyMoveCallBack_On( void * work, int now_pos, int old_pos );
static void BoxItemArrangePartyMoveCallBack_Move( void * work, int now_pos, int old_pos );

static void BoxThemaChgCallBack_On( void * work, int now_pos, int old_pos );
static void BoxThemaChgCallBack_Move( void * work, int now_pos, int old_pos );
static void BoxThemaChgCallBack_Touch( void * work, int now_pos, int old_pos );

static void WallPaperChgCallBack_On( void * work, int now_pos, int old_pos );
static void WallPaperChgCallBack_Move( void * work, int now_pos, int old_pos );
static void WallPaperChgCallBack_Touch( void * work, int now_pos, int old_pos );

static void BoxJumpCallBack_On( void * work, int now_pos, int old_pos );
static void BoxJumpCallBack_Move( void * work, int now_pos, int old_pos );
static void BoxJumpCallBack_Touch( void * work, int now_pos, int old_pos );

static void BattleBoxMainCallBack_On( void * work, int now_pos, int old_pos );
static void BattleBoxMainCallBack_Move( void * work, int now_pos, int old_pos );
static void BattleBoxMainCallBack_Touch( void * work, int now_pos, int old_pos );

static void BattleBoxPartyMainCallBack_On( void * work, int now_pos, int old_pos );
static void BattleBoxPartyMainCallBack_Move( void * work, int now_pos, int old_pos );


//============================================================================================
//	�O���[�o���ϐ�
//============================================================================================

// ��������E���C��
static const CURSORMOVE_DATA PartyOutMainCursorMoveData[] =
{
	{  40,  52, 0, 0,	 6,  2,  0,  1, {  68-12,  68+12-1, 42-12, 42+12-1 } },		// 00: �|�P�����P
	{  80,  60, 0, 0,	 6,  3,  0,  2, {  76-12,  76+12-1, 78-12, 78+12-1 } },		// 01: �|�P�����Q
	{  40,  84, 0, 0,	 0,  4,  1,  3, { 100-12, 100+12-1, 42-12, 42+12-1 } },		// 02: �|�P�����R
	{  80,  92, 0, 0,	 1,  5,  2,  4, { 108-12, 108+12-1, 78-12, 78+12-1 } },		// 03: �|�P�����S
	{  40, 116, 0, 0,	 2,  6,  3,  5, { 132-12, 132+12-1, 42-12, 42+12-1 } },		// 04: �|�P�����T
	{  80, 124, 0, 0,	 3,  6,  4,  6, { 140-12, 140+12-1, 78-12, 78+12-1 } },		// 05: �|�P�����U

	{ 224, 168, 0, 0,	CURSORMOVE_RETBIT|5, 0, 5, 6, { 168, 191, 192, 255 } },		// 06: ���ǂ�

	{ 212,  64, 0, 0,	11,  8,  7,  7, {  64,  87, 168, 255 } },		// 07: ��������
	{ 212,  88, 0, 0,	 7,  9,  8,  8, {  88, 111, 168, 255 } },		// 08: �悤�����݂�
	{ 212, 112, 0, 0,	 8, 10,  9,  9, { 112, 135, 168, 255 } },		// 09: �}�[�L���O
	{ 212, 136, 0, 0,	 9, 11, 10, 10, { 136, 159, 168, 255 } },		// 10: �ɂ���
	{ 224, 168, 0, 0,	10,  7, 11, 11, { 168, 191, 192, 255 } },		// 11: �Ƃ���

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
/*
static const GFL_UI_TP_HITTBL PartyOutMainHitTbl[] =
{
	{  68-12,  68+12-1, 42-12, 42+12-1 },	// 00: �|�P�����P
	{  76-12,  76+12-1, 78-12, 78+12-1 },	// 01: �|�P�����Q
	{ 100-12, 100+12-1, 42-12, 42+12-1 },	// 02: �|�P�����R
	{ 108-12, 108+12-1, 78-12, 78+12-1 },	// 03: �|�P�����S
	{ 132-12, 132+12-1, 42-12, 42+12-1 },	// 04: �|�P�����T
	{ 140-12, 140+12-1, 78-12, 78+12-1 },	// 05: �|�P�����U

	{ 168, 191, 192, 255 },		// 06: ���ǂ�

	{  64,  87, 168, 255 },		// 07: ��������
	{  88, 111, 168, 255 },		// 08: �悤�����݂�
	{ 112, 135, 168, 255 },		// 09: �}�[�L���O
	{ 136, 159, 168, 255 },		// 10: �ɂ���
	{ 168, 191, 192, 255 },		// 11: �Ƃ���

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const CURSORMOVE_DATA PartyOutMainKeyTbl[] =
{
	{ 40,  52, 0, 0,	6, 2, 0, 1 },	// 00: �|�P�����P
	{ 80,  60, 0, 0,	6, 3, 0, 2 },	// 01: �|�P�����Q
	{ 40,  84, 0, 0,	0, 4, 1, 3 },	// 02: �|�P�����R
	{ 80,  92, 0, 0,	1, 5, 2, 4 },	// 03: �|�P�����S
	{ 40, 116, 0, 0,	2, 6, 3, 5 },	// 04: �|�P�����T
	{ 80, 124, 0, 0,	3, 6, 4, 6 },	// 05: �|�P�����U

	{ 224, 168, 0, 0,	CURSORMOVE_RETBIT|5, 0, 5, 6 },	// 06: ���ǂ�

	{ 212,  64, 0, 0,	11,  8,  7,  7 },	// 07: ��������
	{ 212,  88, 0, 0,	 7,  9,  8,  8 },	// 08: �悤�����݂�
	{ 212, 112, 0, 0,	 8, 10,  9,  9 },	// 09: �}�[�L���O
	{ 212, 136, 0, 0,	 9, 11, 10, 10 },	// 10: �ɂ���
	{ 224, 168, 0, 0,	10,  7, 11, 11 },	// 11: �Ƃ���
};
*/
static const CURSORMOVE_CALLBACK PartyOutMainCallBack = {
	PartyOutMainCallBack_On,
	CursorMoveCallBack_Off,
	CursorMoveCallBack_Move,
	CursorMoveCallBack_Touch
};


// ��������E�{�b�N�X�I��
static const CURSORMOVE_DATA PartyOutBoxSelCursorMoveData[] =
{
	{  43,  83, 0, 0,	0, 8, 5, 1, {  71,  94,  31,  54 } },		// 00: �g���C�A�C�R��
	{  77,  83, 0, 0,	1, 8, 0, 2, {  71,  94,  65,  88 } },		// 01: �g���C�A�C�R��
	{ 111,  83, 0, 0,	2, 8, 1, 3, {  71,  94,  99, 122 } },		// 02: �g���C�A�C�R��
	{ 145,  83, 0, 0,	3, 8, 2, 4, {  71,  94, 133, 156 } },		// 03: �g���C�A�C�R��
	{ 179,  83, 0, 0,	4, 8, 3, 5, {  71,  94, 167, 190 } },		// 04: �g���C�A�C�R��
	{ 213,  83, 0, 0,	5, 8, 4, 0, {  71,  94, 201, 224 } },		// 05: �g���C�A�C�R��

	{  12,  83, 0, 0,	6, 6, 6, 6, {  71,  94,   1,  22 } },		// 06: �g���C���i���j
	{ 244,  83, 0, 0,	7, 7, 7, 7, {  71,  94, 233, 254 } },		// 07: �g���C���i�E�j

	{ 212, 112, 0, 0,	0, 9, 8, 8, { 112, 159, 168, 255 } },		// 08:�u�|�P��������������v
	{ 224, 168, 0, 0,	8, 9, 9, 9, { 168, 191, 192, 255 } },		// 09:�u��߂�v

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
/*
static const GFL_UI_TP_HITTBL PartyOutBoxSelHitTbl[] =
{
	{  71,  94,  31,  54 },		// 00: �g���C�A�C�R��
	{  71,  94,  65,  88 },		// 01: �g���C�A�C�R��
	{  71,  94,  99, 122 },		// 02: �g���C�A�C�R��
	{  71,  94, 133, 156 },		// 03: �g���C�A�C�R��
	{  71,  94, 167, 190 },		// 04: �g���C�A�C�R��
	{  71,  94, 201, 224 },		// 05: �g���C�A�C�R��

	{  71,  94,   1,  22 },		// 06: �g���C���i���j
	{  71,  94, 233, 254 },		// 07: �g���C���i�E�j

	{ 112, 159, 168, 255 },		// 08:�u�|�P��������������v
	{ 168, 191, 192, 255 },		// 09:�u��߂�v

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const CURSORMOVE_DATA PartyOutBoxSelKeyTbl[] =
{
	{  43,  83, 0, 0,	0, 8, 5, 1 },		// 00: �g���C�A�C�R��
	{  77,  83, 0, 0,	1, 8, 0, 2 },		// 01: �g���C�A�C�R��
	{ 111,  83, 0, 0,	2, 8, 1, 3 },		// 02: �g���C�A�C�R��
	{ 145,  83, 0, 0,	3, 8, 2, 4 },		// 03: �g���C�A�C�R��
	{ 179,  83, 0, 0,	4, 8, 3, 5 },		// 04: �g���C�A�C�R��
	{ 213,  83, 0, 0,	5, 8, 4, 0 },		// 05: �g���C�A�C�R��

	{  12,  83, 0, 0,	6, 6, 6, 6 },		// 06: �g���C���i���j
	{ 244,  83, 0, 0,	7, 7, 7, 7 },		// 07: �g���C���i�E�j

	{ 212, 112, 0, 0,	0, 9, 8, 8 },		// 08:�u�|�P��������������v
	{ 224, 168, 0, 0,	8, 9, 9, 9 },		// 09:�u��߂�v
};
*/
static const CURSORMOVE_CALLBACK PartyOutBoxSelCallBack = {
	PartyOutBoxSelCallBack_On,
	CursorMoveCallBack_Off,
	PartyOutBoxSelCallBack_Move,
	PartyOutBoxSelCallBack_Touch,
};

// �}�[�L���O
static const CURSORMOVE_DATA MarkingCursorMoveData[] =
{
	{ 204,  42, 0, 0,	0, 2, 0, 1, { 44,  59, 188, 203 } },		// 00: ��
	{ 236,  42, 0, 0,	1, 3, 0, 1, { 44,  59, 220, 235 } },		// 01: ��
	{ 204,  66, 0, 0,	0, 4, 2, 3, { 68,  83, 188, 203 } },		// 02: ��
	{ 236,  66, 0, 0,	1, 5, 2, 3, { 68,  83, 220, 235 } },		// 03: �n�[�g
	{ 204,  90, 0, 0,	2, 6, 4, 5, { 92, 107, 188, 203 } },		// 04: ��
	{ 236,  90, 0, 0,	3, 6, 4, 5, { 92, 107, 220, 235 } },		// 05: ��
	{ 212, 112, 0, 0,	CURSORMOVE_RETBIT|4, 7, 6, 6, { 112, 135, 178, 245 } },		// 06:�u�����Ă��v
	{ 212, 136, 0, 0,	6, 7, 7, 7, { 136, 159, 178, 245 } },		// 07:�u��߂�v

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
static const CURSORMOVE_CALLBACK MarkingCallBack = {
	CursorMoveCallBack_On,
	CursorMoveCallBack_Off,
	CursorMoveCallBack_Move,
	CursorMoveCallBack_Touch
};

// ��Ă����E���C��
static const CURSORMOVE_DATA PartyInMainCursorMoveData[] =
{
	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  6, 5, 1, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },		// 00: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  7, 0, 2, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  8, 1, 3, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  9, 2, 4, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 10, 3, 5, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 11, 4, 0, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	0, 12, 11,  7, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 06: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	1, 13,  6,  8, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	2, 14,  7,  9, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	3, 15,  8, 10, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	4, 16,  9, 11, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	5, 17, 10,  6, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 6, 18, 17, 13, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 12: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 7, 19, 12, 14, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 8, 20, 13, 15, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 9, 21, 14, 16, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	10, 22, 15, 17, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	11, 23, 16, 12, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	12, 24, 23, 19, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 18: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	13, 25, 18, 20, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	14, 26, 19, 21, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	15, 27, 20, 22, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	16, 28, 21, 23, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	17, 29, 22, 18, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	18, 33, 29, 25, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 24: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	19, 33, 24, 26, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	20, 33, 25, 27, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	21, 33, 26, 28, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	22, 33, 27, 29, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	23, 33, 28, 24, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },


	{  84,  16, 0, 0,	33, CURSORMOVE_RETBIT|0, 30, 30, {  17,  39,  26, 141 } },		// 30: �{�b�N�X��
	{   0,   0, 0, 0,	31, 31, 31, 31, {  17,  38,   1,  22 } },						// 31: �g���C�؂�ւ����E��
	{   0,   0, 0, 0,	32, 32, 32, 32, {  17,  38, 145, 166 } },						// 32: �g���C�؂�ւ����E�E
	{ 224, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 33, { 168, 191, 192, 255 } },		// 33: ���ǂ�

	{ 212,  64, 0, 0,	38, 35, 34, 34, {  64,  87, 168, 255 } },		// 34: ��Ă���
	{ 212,  88, 0, 0,	34, 36, 35, 35, {  88, 111, 168, 255 } },		// 35: �悤�����݂�
	{ 212, 112, 0, 0,	35, 37, 36, 36, { 112, 135, 168, 255 } },		// 36: �}�[�L���O
	{ 212, 136, 0, 0,	36, 38, 37, 37, { 136, 159, 168, 255 } },		// 37: �ɂ���
	{ 224, 168, 0, 0,	37, 34, 38, 38, { 168, 191, 192, 255 } },		// 38: �Ƃ���

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
/*
static const GFL_UI_TP_HITTBL PartyInMainHitTbl[] =
{
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },		// 00: �|�P����
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

#if	BTS_5462_FIX
	{  17,  39,  26, 141 },		// 30: �{�b�N�X��
#else
	{   9,  31,  26, 141 },		// 30: �{�b�N�X��
#endif
	{  17,  38,   1,  22 },		// 31: �g���C�؂�ւ����E��
	{  17,  38, 145, 166 },		// 32: �g���C�؂�ւ����E�E
	{ 168, 191, 192, 255 },		// 33: ���ǂ�

	{  64,  87, 168, 255 },		// 34: ��Ă���
	{  88, 111, 168, 255 },		// 35: �悤�����݂�
	{ 112, 135, 168, 255 },		// 36: �}�[�L���O
	{ 136, 159, 168, 255 },		// 37: �ɂ���
	{ 168, 191, 192, 255 },		// 38: �Ƃ���

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const CURSORMOVE_DATA PartyInMainKeyTbl[] =
{
	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  6, 5, 1 },		// 00: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  7, 0, 2 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  8, 1, 3 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  9, 2, 4 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 10, 3, 5 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 11, 4, 0 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	0, 12, 11,  7 },	// 06: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	1, 13,  6,  8 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	2, 14,  7,  9 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	3, 15,  8, 10 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	4, 16,  9, 11 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	5, 17, 10,  6 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 6, 18, 17, 13 },	// 12: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 7, 19, 12, 14 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 8, 20, 13, 15 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 9, 21, 14, 16 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	10, 22, 15, 17 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	11, 23, 16, 12 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	12, 24, 23, 19 },	// 18: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	13, 25, 18, 20 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	14, 26, 19, 21 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	15, 27, 20, 22 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	16, 28, 21, 23 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	17, 29, 22, 18 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	18, 33, 29, 25 },	// 24: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	19, 33, 24, 26 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	20, 33, 25, 27 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	21, 33, 26, 28 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	22, 33, 27, 29 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	23, 33, 28, 24 },

	{  84,  16, 0, 0,	33, CURSORMOVE_RETBIT|0, 30, 30 },		// 30: �{�b�N�X��
	{   0,   0, 0, 0,	31, 31, 31, 31 },						// 31: �g���C�؂�ւ����E��
	{   0,   0, 0, 0,	32, 32, 32, 32 },						// 32: �g���C�؂�ւ����E�E
	{ 224, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 33 },		// 33: ���ǂ�

	{ 212,  64, 0, 0,	38, 35, 34, 34 },	// 34: ��Ă���
	{ 212,  88, 0, 0,	34, 36, 35, 35 },	// 35: �悤�����݂�
	{ 212, 112, 0, 0,	35, 37, 36, 36 },	// 36: �}�[�L���O
	{ 212, 136, 0, 0,	36, 38, 37, 37 },	// 37: �ɂ���
	{ 224, 168, 0, 0,	37, 34, 38, 38 },	// 38: �Ƃ���
};
*/
static const CURSORMOVE_CALLBACK PartyInMainCallBack = {
	PartyInMainCallBack_On,
	CursorMoveCallBack_Off,
	CursorMoveCallBack_Move,
	PartyInMainCallBack_Touch,
};

// �{�b�N�X�e�[�}�ύX
static const CURSORMOVE_DATA BoxThemaChgCursorMoveData[] =
{
	{ 212,  64, 0, 0,	3, 1, 0, 0, {  64,  87, 168, 255 } },		// 00: �W�����v����
	{ 212,  88, 0, 0,	0, 2, 1, 1, {  88, 111, 168, 255 } },		// 01: ���ׂ���
	{ 212, 112, 0, 0,	1, 3, 2, 2, { 112, 135, 168, 255 } },		// 02: �Ȃ܂�
	{ 212, 136, 0, 0,	2, 0, 3, 3, { 136, 159, 192, 255 } },		// 03: ��߂�

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
/*
static const GFL_UI_TP_HITTBL BoxThemaChgHitTbl[] =
{
	{  15,  38,  31,  54 },		// 00: �g���C�A�C�R��
	{  15,  38,  65,  88 },		// 01: �g���C�A�C�R��
	{  15,  38,  99, 122 },		// 02: �g���C�A�C�R��
	{  15,  38, 133, 156 },		// 03: �g���C�A�C�R��
	{  15,  38, 167, 190 },		// 04: �g���C�A�C�R��
	{  15,  38, 201, 224 },		// 05: �g���C�A�C�R��

	{  15,  38,   1,  22 },		// 06: �g���C���i���j
	{  15,  38, 233, 254 },		// 07: �g���C���i�E�j

	{  64, 111, 168, 255 },		// 08: �{�b�N�X�����肩����
	{ 112, 135, 168, 255 },		// 09: ���ׂ���
	{ 136, 159, 168, 255 },		// 10: �Ȃ܂�
	{ 168, 191, 192, 255 },		// 11: ��߂�

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const CURSORMOVE_DATA BoxThemaChgKeyTbl[] =
{
	{  43, 23, 0, 0,	0, 8, 5, 1 },		// 00: �g���C�A�C�R��
	{  77, 23, 0, 0,	1, 8, 0, 2 },		// 01: �g���C�A�C�R��
	{ 111, 23, 0, 0,	2, 8, 1, 3 },		// 02: �g���C�A�C�R��
	{ 145, 23, 0, 0,	3, 8, 2, 4 },		// 03: �g���C�A�C�R��
	{ 179, 23, 0, 0,	4, 8, 3, 5 },		// 04: �g���C�A�C�R��
	{ 213, 23, 0, 0,	5, 8, 4, 0 },		// 05: �g���C�A�C�R��

	{ 0, 0, 0, 0,	6, 6, 6, 6 },			// 06: �g���C���i���j
	{ 0, 0, 0, 0,	7, 7, 7, 7 },			// 07: �g���C���i�E�j

	{ 212,  64, 0, 0,	 0,  9,  8,  8 },	// 08: �{�b�N�X�����肩����
	{ 212, 112, 0, 0,	 8, 10,  9,  9 },	// 09: ���ׂ���
	{ 212, 136, 0, 0,	 9, 11, 10, 10 },	// 10: �Ȃ܂�
	{ 224, 168, 0, 0,	10, 11, 11, 11 },	// 11: ��߂�
};
*/
static const CURSORMOVE_CALLBACK BoxThemaChgCallBack = {
	BoxThemaChgCallBack_On,
	CursorMoveCallBack_Off,
	BoxThemaChgCallBack_Move,
	BoxThemaChgCallBack_Touch
};

// �ǎ��ύX
static const CURSORMOVE_DATA WallPaperChgCursorMoveData[] =
{
	{ 212,  40, 0, 0,	4, 1, 0, 0, {  40,  63, 168, 255 } },		// 00: ���j���[�P
	{ 212,  64, 0, 0,	0, 2, 1, 1, {  64,  87, 168, 255 } },		// 01: ���j���[�Q
	{ 212,  88, 0, 0,	1, 3, 2, 2, {  88, 111, 168, 255 } },		// 02: ���j���[�R
	{ 212, 112, 0, 0,	2, 4, 3, 3, { 112, 135, 168, 255 } },		// 03: ���j���[�S
	{ 212, 136, 0, 0,	3, 0, 4, 4, { 136, 159, 192, 255 } },		// 04: ���ǂ�

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
static const CURSORMOVE_CALLBACK WallPaperChgCallBack = {
	WallPaperChgCallBack_On,
	CursorMoveCallBack_Off,
	WallPaperChgCallBack_Move,
	WallPaperChgCallBack_Touch
};

// �{�b�N�X�����E���C��
static const CURSORMOVE_DATA BoxArrangeMainCursorMoveData[] =
{
	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  6, 5, 1, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },		// 00: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  7, 0, 2, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  8, 1, 3, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  9, 2, 4, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 10, 3, 5, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 11, 4, 0, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	0, 12, 11,  7, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 06: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	1, 13,  6,  8, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	2, 14,  7,  9, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	3, 15,  8, 10, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	4, 16,  9, 11, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	5, 17, 10,  6, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 6, 18, 17, 13, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 12: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 7, 19, 12, 14, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 8, 20, 13, 15, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 9, 21, 14, 16, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	10, 22, 15, 17, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	11, 23, 16, 12, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	12, 24, 23, 19, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 18: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	13, 25, 18, 20, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	14, 26, 19, 21, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	15, 27, 20, 22, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	16, 28, 21, 23, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	17, 29, 22, 18, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	18, 33, 29, 25, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 24: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	19, 33, 24, 26, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	20, 33, 25, 27, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	21, 33, 26, 28, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	22, 33, 27, 29, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	23, 33, 28, 24, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{  84,  16, 0, 0,	33, CURSORMOVE_RETBIT|0, 30, 30, {  17,  39,  26, 141 } },		// 30: �{�b�N�X��
	{   0,   0, 0, 0,	31, 31, 31, 31, {  17,  38,   1,  22 } },						// 31: �g���C�؂�ւ����E��
	{   0,   0, 0, 0,	32, 32, 32, 32, {  17,  38, 145, 166 } },						// 32: �g���C�؂�ւ����E�E

	{  44, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 33, { 168, 191,   0,  95 } },		// 33: �莝���|�P����

	{ 0, 0, 0, 0,	34, 34, 34, 34, { TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_06, TOUCHBAR_ICON_X_06+TOUCHBAR_ICON_WIDTH-1 } },			// 34: �߂�P
	{ 0, 0, 0, 0,	35, 35, 35, 35, { TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH-1 } },			// 35: �߂�Q

	{ 212,  16, 0, 0,	41, 37, 36, 36, {  16,  39, 168, 255 } },	// 36: ����
	{ 212,  40, 0, 0,	36, 38, 37, 37, {  40,  63, 168, 255 } },	// 37: �悤�����݂�
	{ 212,  64, 0, 0,	37, 39, 38, 38, {  64,  87, 168, 255 } },	// 38: ��������
	{ 212,  88, 0, 0,	38, 40, 39, 39, {  88, 111, 168, 255 } },	// 39: �}�[�L���O
	{ 212, 112, 0, 0,	39, 41, 40, 40, { 112, 135, 168, 255 } },	// 40: �ɂ���
	{ 212, 136, 0, 0,	40, 36, 41, 41, { 136, 159, 168, 255 } },	// 41: �Ƃ���

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
static const CURSORMOVE_CALLBACK BoxArrangeMainCallBack = {
	BoxArrangeMainCallBack_On,
	CursorMoveCallBack_Off,
	BoxArrangeMainCallBack_Move,
	BoxArrangeMainCallBack_Touch
};

// �{�b�N�X�����E�|�P�����ړ��i�{�b�N�X�ԁj
static const CURSORMOVE_DATA BoxArrangePokeMoveCursorMoveData[] =
{
	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  6, 34, 1, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 00: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  7,  0, 2, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  8,  1, 3, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  9,  2, 4, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 10,  3, 5, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 11,  4, CURSORMOVE_RETBIT|34, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	0, 12, 35,  7, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 06: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	1, 13,  6,  8, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	2, 14,  7,  9, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	3, 15,  8, 10, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	4, 16,  9, 11, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	5, 17, 10, CURSORMOVE_RETBIT|35, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 6, 18, 36, 13, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 12: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 7, 19, 12, 14, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 8, 20, 13, 15, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 9, 21, 14, 16, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	10, 22, 15, 17, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	11, 23, 16, CURSORMOVE_RETBIT|36, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	12, 24, 37, 19, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 18: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	13, 25, 18, 20, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	14, 26, 19, 21, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	15, 27, 20, 22, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	16, 28, 21, 23, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	17, 29, 22, CURSORMOVE_RETBIT|37, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	18, 39, 37, 25, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 24: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	19, 39, 24, 26, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	20, 39, 25, 27, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	21, 39, 26, 28, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	22, 39, 27, 29, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	23, 39, 28, CURSORMOVE_RETBIT|37, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ 84, 16, 0, 0,	39, CURSORMOVE_RETBIT|0, 30, 30, { 17, 39, 26, 141 } },		// 30: �{�b�N�X��
	{  0,  0, 0, 0,	31, 31, 31, 31, { 17, 38,   1,  22 } },		// 31: �{�b�N�X�؂�ւ����i���j
	{  0,  0, 0, 0,	32, 32, 32, 32, { 17, 38, 145, 166 } },		// 32: �{�b�N�X�؂�ւ����i�E�j

	{   0,   0, 0, 0,	33, 33, CURSORMOVE_RETBIT|33, 33, {   0,  11, 200, 223 } },		// 33: �g���C�A�C�R��
	{ 212,  22, 0, 0,	34, 35, CURSORMOVE_RETBIT| 5,  0, {  22,  45, 200, 223 } },		// 34: �g���C�A�C�R��
	{ 212,  56, 0, 0,	34, 36, CURSORMOVE_RETBIT|11,  6, {  56,  79, 200, 223 } },		// 35: �g���C�A�C�R��
	{ 212,  90, 0, 0,	35, 37, CURSORMOVE_RETBIT|17, 12, {  90, 113, 200, 223 } },		// 36: �g���C�A�C�R��
	{ 212, 124, 0, 0,	36, 37, CURSORMOVE_RETBIT|23, 18, { 124, 147, 200, 223 } },		// 37: �g���C�A�C�R��
	{   0,   0, 0, 0,	38, 38, CURSORMOVE_RETBIT|38, 38, { 158, 167, 200, 223 } },		// 38: �g���C�A�C�R��

	{ 44, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 39, 39, { 168, 191, 0, 87 } },			// 39: �莝���|�P����
	{  0,   0, 0, 0,	40, 40, 40, 40, { TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_03, TOUCHBAR_ICON_X_03+TOUCHBAR_ICON_WIDTH-1 } },		// 40: �X�e�[�^�X
	{  0,   0, 0, 0,	41, 41, 41, 41, { TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH-1 } },		// 41: �߂�

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
static const CURSORMOVE_CALLBACK BoxArrangePokeMoveCallBack = {
	BoxArrangePokeMoveCallBack_On,
	CursorMoveCallBack_Off,
	BoxArrangePokeMoveCallBack_Move,
	BoxArrangePokeMoveCallBack_Touch
};

// �{�b�N�X�����E�莝���|�P����
static const CURSORMOVE_DATA BoxArrangePartyPokeCursorMoveData[] =
{
	{ 40,  52, 0, 0,	6, 2, 0, 1, {  68-12,  68+12-1, 42-12, 42+12-1 } },	// 00: �|�P�����P
	{ 80,  60, 0, 0,	6, 3, 0, 2, {  76-12,  76+12-1, 78-12, 78+12-1 } },	// 01: �|�P�����Q
	{ 40,  84, 0, 0,	0, 4, 1, 3, { 100-12, 100+12-1, 42-12, 42+12-1 } },	// 02: �|�P�����R
	{ 80,  92, 0, 0,	1, 5, 2, 4, { 108-12, 108+12-1, 78-12, 78+12-1 } },	// 03: �|�P�����S
	{ 40, 116, 0, 0,	2, 6, 3, 5, { 132-12, 132+12-1, 42-12, 42+12-1 } },	// 04: �|�P�����T
	{ 80, 124, 0, 0,	3, 6, 4, 6, { 140-12, 140+12-1, 78-12, 78+12-1 } },	// 05: �|�P�����U

	{ 44, 168, 0, 0,	CURSORMOVE_RETBIT|5, 0, 5, 0, { 168, 191, 0, 95 } },		// 06:�u�{�b�N�X���X�g�v

	{ 0, 0, 0, 0,	7, 7, 7, 7, { TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_06, TOUCHBAR_ICON_X_06+TOUCHBAR_ICON_WIDTH-1 } },			// 07: �߂�P
	{ 0, 0, 0, 0,	8, 8, 8, 8, { TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH-1 } },			// 08: �߂�Q

	{ 212,  16, 0, 0,	14, 10,  9,  9, {  16,  39, 168, 255 } },	// 09: ����
	{ 212,  40, 0, 0,	 9, 11, 10, 10, {  40,  63, 168, 255 } },	// 10: �悤�����݂�
	{ 212,  64, 0, 0,	10, 12, 11, 11, {  64,  87, 168, 255 } },	// 11: ��������
	{ 212,  88, 0, 0,	11, 13, 12, 12, {  88, 111, 168, 255 } },	// 12: �}�[�L���O
	{ 212, 112, 0, 0,	12, 14, 13, 13, { 112, 135, 168, 255 } },	// 13: �ɂ���
	{ 212, 136, 0, 0,	13,  9, 14, 14, { 136, 159, 168, 255 } },	// 14: �Ƃ���

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
static const CURSORMOVE_CALLBACK BoxArrangePartyPokeCallBack = {
	BoxArrangePartyMoveCallBack_On,
	CursorMoveCallBack_Off,
	BoxArrangePartyMoveCallBack_Move,
	CursorMoveCallBack_Touch
};

// �{�b�N�X�����E�|�P�����ړ��i�莝���|�P�����j
static const CURSORMOVE_DATA BoxArrangePartyPokeMoveCursorMoveData[] =
{
	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	36,  6, 31, 1, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 00: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	36,  7,  0, 2, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	36,  8,  1, 3, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	36,  9,  2, 4, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	36, 10,  3, 5, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	36, 11,  4, CURSORMOVE_RETBIT|30, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	0, 12, 31,  7, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 06: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	1, 13,  6,  8, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	2, 14,  7,  9, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	3, 15,  8, 10, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	4, 16,  9, 11, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	5, 17, 10, CURSORMOVE_RETBIT|30, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 6, 18, 33, 13, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 12: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 7, 19, 12, 14, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 8, 20, 13, 15, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 9, 21, 14, 16, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	10, 22, 15, 17, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	11, 23, 16, CURSORMOVE_RETBIT|32, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	12, 24, 35, 19, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 18: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	13, 25, 18, 20, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	14, 26, 19, 21, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	15, 27, 20, 22, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	16, 28, 21, 23, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	17, 29, 22, CURSORMOVE_RETBIT|34, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	18, 39, 35, 25, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 24: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	19, 39, 24, 26, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	20, 39, 25, 27, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	21, 39, 26, 28, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	22, 39, 27, 29, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	23, 39, 28, CURSORMOVE_RETBIT|34, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ 192,  52, 0, 0,	34, 32, CURSORMOVE_RETBIT|5, 31, {  68-12,  68+12-1, 194-12, 194+12-1 } },		// 30: �莝���|�P����
	{ 232,  60, 0, 0,	35, 33, 30, CURSORMOVE_RETBIT|0, {  76-12,  76+12-1, 230-12, 230+12-1 } },
	{ 192,  84, 0, 0,	30, 34, CURSORMOVE_RETBIT|17, 33, { 100-12, 100+12-1, 194-12, 194+12-1 } },
	{ 232,  92, 0, 0,	31, 35, 32, CURSORMOVE_RETBIT|12, { 108-12, 108+12-1, 230-12, 230+12-1 } },
	{ 192, 116, 0, 0,	32, 30, CURSORMOVE_RETBIT|23, 35, { 132-12, 132+12-1, 194-12, 194+12-1 } },
	{ 232, 124, 0, 0,	33, 31, 34, CURSORMOVE_RETBIT|24, { 140-12, 140+12-1, 230-12, 230+12-1 } },

	{ 84, 16, 0, 0,	39, CURSORMOVE_RETBIT|0, 36, 36, { 17, 39, 26, 141 } },		// 36: �{�b�N�X��
	{  0,  0, 0, 0,	37, 37, 37, 37, { 17, 38,   1,  22 } },		// 37: �{�b�N�X�؂�ւ����i���j
	{  0,  0, 0, 0,	38, 38, 38, 38, { 17, 38, 145, 166 } },		// 38: �{�b�N�X�؂�ւ����i�E�j

	{ 44, 168, 0, 0,	CURSORMOVE_RETBIT|24, 36, 39, 39, { 168, 191, 0, 87 } },			// 39: �{�b�N�X���X�g
	{  0,   0, 0, 0,	40, 40, 40, 40, { TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_03, TOUCHBAR_ICON_X_03+TOUCHBAR_ICON_WIDTH-1 } },		// 40: �X�e�[�^�X
	{  0,   0, 0, 0,	41, 41, 41, 41, { TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH-1 } },		// 41: �߂�

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
static const CURSORMOVE_CALLBACK BoxArrangePartyPokeMoveCallBack = {
	BoxArrangePartyPokeMoveCallBack_On,
	CursorMoveCallBack_Off,
	BoxArrangePartyPokeMoveCallBack_Move,
	BoxArrangePartyPokeMoveCallBack_Touch
};

// �����������E���C��
static const CURSORMOVE_DATA BoxItemArrangeMainCursorMoveData[] =
{
	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  6, 5, 1, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },		// 00: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  7, 0, 2, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  8, 1, 3, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  9, 2, 4, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 10, 3, 5, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 11, 4, 0, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	0, 12, 11,  7, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 06: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	1, 13,  6,  8, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	2, 14,  7,  9, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	3, 15,  8, 10, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	4, 16,  9, 11, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	5, 17, 10,  6, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 6, 18, 17, 13, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 12: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 7, 19, 12, 14, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 8, 20, 13, 15, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 9, 21, 14, 16, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	10, 22, 15, 17, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	11, 23, 16, 12, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	12, 24, 23, 19, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 18: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	13, 25, 18, 20, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	14, 26, 19, 21, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	15, 27, 20, 22, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	16, 28, 21, 23, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	17, 29, 22, 18, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	18, CURSORMOVE_RETBIT|33, 29, 25, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 24: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	19, CURSORMOVE_RETBIT|33, 24, 26, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	20, CURSORMOVE_RETBIT|33, 25, 27, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	21, CURSORMOVE_RETBIT|33, 26, 28, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	22, CURSORMOVE_RETBIT|33, 27, 29, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	23, CURSORMOVE_RETBIT|33, 28, 24, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{  84,  16, 0, 0,	33, CURSORMOVE_RETBIT|0, 30, 30, {  17,  39,  26, 141 } },		// 30: �{�b�N�X��
	{   0,   0, 0, 0,	31, 31, 31, 31, {  17,  38,   1,  22 } },						// 31: �g���C�؂�ւ����E��
	{   0,   0, 0, 0,	32, 32, 32, 32, {  17,  38, 145, 166 } },						// 32: �g���C�؂�ւ����E�E
	{  44, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 34, { 168, 191,   0,  87 } },		// 33: �莝���|�P����
	{ 140, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 35, { 168, 191,  96, 183 } },		// 34: ����������
	{ 224, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 34, 35, { 168, 191, 192, 255 } },		// 35: ���ǂ�

	{ 212, 136, 0, 0,	37, 37, 36, 36, { 136, 159, 168, 255 } },	// 36: �o�b�O�� or ��������
	{ 224, 168, 0, 0,	36, 36, 37, 37, { 168, 191, 192, 255 } },	// 37: �Ƃ���

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
/*
static const GFL_UI_TP_HITTBL BoxItemArrangeMainHitTbl[] =
{
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },		// 00: �|�P����
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{  17,  39,  26, 141 },		// 30: �{�b�N�X��
	{  17,  38,   1,  22 },		// 31: �g���C�؂�ւ����E��
	{  17,  38, 145, 166 },		// 32: �g���C�؂�ւ����E�E
	{ 168, 191,   0,  87 },		// 33: �莝���|�P����
	{ 168, 191,  96, 183 },		// 34: ����������
	{ 168, 191, 192, 255 },		// 35: ���ǂ�

	{ 136, 159, 168, 255 },		// 36: �o�b�O�� or ��������
	{ 168, 191, 192, 255 },		// 37: �Ƃ���

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const CURSORMOVE_DATA BoxItemArrangeMainKeyTbl[] =
{
	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  6, 5, 1 },		// 00: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  7, 0, 2 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  8, 1, 3 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  9, 2, 4 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 10, 3, 5 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 11, 4, 0 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	0, 12, 11,  7 },	// 06: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	1, 13,  6,  8 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	2, 14,  7,  9 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	3, 15,  8, 10 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	4, 16,  9, 11 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	5, 17, 10,  6 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 6, 18, 17, 13 },	// 12: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 7, 19, 12, 14 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 8, 20, 13, 15 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 9, 21, 14, 16 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	10, 22, 15, 17 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	11, 23, 16, 12 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	12, 24, 23, 19 },	// 18: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	13, 25, 18, 20 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	14, 26, 19, 21 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	15, 27, 20, 22 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	16, 28, 21, 23 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	17, 29, 22, 18 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	18, CURSORMOVE_RETBIT|33, 29, 25 },	// 24: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	19, CURSORMOVE_RETBIT|33, 24, 26 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	20, CURSORMOVE_RETBIT|33, 25, 27 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	21, CURSORMOVE_RETBIT|33, 26, 28 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	22, CURSORMOVE_RETBIT|33, 27, 29 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	23, CURSORMOVE_RETBIT|33, 28, 24 },

	{  84,  16, 0, 0,	33, CURSORMOVE_RETBIT|0, 30, 30 },		// 30: �{�b�N�X��
	{   0,   0, 0, 0,	31, 31, 31, 31 },						// 31: �g���C�؂�ւ����E��
	{   0,   0, 0, 0,	32, 32, 32, 32 },						// 32: �g���C�؂�ւ����E�E
	{  44, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 34 },		// 33: �莝���|�P����
	{ 140, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 35 },		// 34: ����������
	{ 224, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 34, 35 },		// 35: ���ǂ�

	{ 212, 136, 0, 0,	37, 37, 36, 36 },	// 36: �o�b�O�� or ��������
	{ 224, 168, 0, 0,	36, 36, 37, 37 },	// 37: �Ƃ���
};
*/
static const CURSORMOVE_CALLBACK BoxItemArrangeMainCallBack = {
	BoxItemArrangeMainCallBack_On,
	CursorMoveCallBack_Off,
	BoxItemArrangeMainCallBack_Move,
	BoxItemArrangeMainCallBack_Touch
};

// �����������E�莝���|�P����
static const CURSORMOVE_DATA BoxItemArrangePartyPokeCursorMoveData[] =
{
	{ 40,  52, 0, 0,	4, 2, 0, 1, {  68-12,  68+12-1, 42-12, 42+12-1 } },	// 00: �|�P�����P
	{ 80,  60, 0, 0,	5, 3, 0, 2, {  76-12,  76+12-1, 78-12, 78+12-1 } },	// 01: �|�P�����Q
	{ 40,  84, 0, 0,	0, 4, 1, 3, { 100-12, 100+12-1, 42-12, 42+12-1 } },	// 02: �|�P�����R
	{ 80,  92, 0, 0,	1, 5, 2, 4, { 108-12, 108+12-1, 78-12, 78+12-1 } },	// 03: �|�P�����S
	{ 40, 116, 0, 0,	2, 6, 3, 5, { 132-12, 132+12-1, 42-12, 42+12-1 } },	// 04: �|�P�����T
	{ 80, 124, 0, 0,	3, 6, 4, 6, { 140-12, 140+12-1, 78-12, 78+12-1 } },	// 05: �|�P�����U

	{  60, 162, 0, 0,	CURSORMOVE_RETBIT|5, 7, 5, 7, { 162, 189,  26,  93 } },	// 06: ���ꂩ��

	{ 224, 168, 0, 0,	6, 7, 6, 7, { 168, 191, 192, 255 } },	// 07: ���ǂ�

	{ 212, 136, 0, 0,	9, 9, 8, 8, { 136, 159, 168, 255 } },	// 08: �o�b�O�� or  ��������
	{ 224, 168, 0, 0,	8, 8, 9, 9, { 168, 191, 192, 255 } },	// 09: �Ƃ���

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
/*
static const GFL_UI_TP_HITTBL BoxItemArrangePartyPokeHitTbl[] =
{
	{  68-12,  68+12-1, 42-12, 42+12-1 },	// 00: �|�P�����P
	{  76-12,  76+12-1, 78-12, 78+12-1 },	// 01: �|�P�����Q
	{ 100-12, 100+12-1, 42-12, 42+12-1 },	// 02: �|�P�����R
	{ 108-12, 108+12-1, 78-12, 78+12-1 },	// 03: �|�P�����S
	{ 132-12, 132+12-1, 42-12, 42+12-1 },	// 04: �|�P�����T
	{ 140-12, 140+12-1, 78-12, 78+12-1 },	// 05: �|�P�����U

	{ 162, 189,  26,  93 },		// 06: ���ꂩ��

	{ 168, 191, 192, 255 },		// 07: ���ǂ�

	{ 136, 159, 168, 255 },		// 08: �o�b�O�� or  ��������
	{ 168, 191, 192, 255 },		// 09: �Ƃ���

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const CURSORMOVE_DATA BoxItemArrangePartyPokeKeyTbl[] =
{
	{ 40,  52, 0, 0,	4, 2, 0, 1 },	// 00: �|�P�����P
	{ 80,  60, 0, 0,	5, 3, 0, 2 },	// 01: �|�P�����Q
	{ 40,  84, 0, 0,	0, 4, 1, 3 },	// 02: �|�P�����R
	{ 80,  92, 0, 0,	1, 5, 2, 4 },	// 03: �|�P�����S
	{ 40, 116, 0, 0,	2, 6, 3, 5 },	// 04: �|�P�����T
	{ 80, 124, 0, 0,	3, 6, 4, 6 },	// 05: �|�P�����U

	{  60, 162, 0, 0,	CURSORMOVE_RETBIT|5, 7, 5, 7 },	// 06: ���ꂩ��

	{ 224, 168, 0, 0,	6, 7, 6, 7 },	// 07: ���ǂ�

	{ 212, 136, 0, 0,	9, 9, 8, 8 },	// 08: �o�b�O�� or  ��������
	{ 224, 168, 0, 0,	8, 8, 9, 9 },	// 09: �Ƃ���
};
*/
static const CURSORMOVE_CALLBACK BoxItemArrangePartyPokeCallBack = {
	BoxItemArrangePartyMoveCallBack_On,
	CursorMoveCallBack_Off,
	BoxItemArrangePartyMoveCallBack_Move,
	CursorMoveCallBack_Touch
};

// �{�b�N�X�W�����v
static const CURSORMOVE_DATA BoxJumpCursorMoveData[] =
{
	{   0,   0, 0, 0,	0, 0, 0, 0, {   0,  11, 200, 223 } },		// 00: �g���C�A�C�R��
	{ 212,  22, 0, 0,	1, 2, 1, 1, {  22,  45, 200, 223 } },		// 01: �g���C�A�C�R��
	{ 212,  56, 0, 0,	1, 3, 2, 2, {  56,  79, 200, 223 } },		// 02: �g���C�A�C�R��
	{ 212,  90, 0, 0,	2, 4, 3, 3, {  90, 113, 200, 223 } },		// 03: �g���C�A�C�R��
	{ 212, 124, 0, 0,	3, 4, 4, 4, { 124, 147, 200, 223 } },		// 04: �g���C�A�C�R��
	{   0,   0, 0, 0,	5, 5, 5, 5, { 158, 167, 200, 223 } },		// 05: �g���C�A�C�R��

	{ 0, 0, 0, 0,	6, 6, 6, 6, { TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH-1 } },		// 06: �߂�

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
static const CURSORMOVE_CALLBACK BoxJumpCallBack = {
	BoxJumpCallBack_On,
	CursorMoveCallBack_Off,
	BoxJumpCallBack_Move,
	BoxJumpCallBack_Touch
};

/*
// �x�X�e�[�^�X�{�^���`�F�b�N
static const GFL_UI_TP_HITTBL YStatusHitTbl[] =
{
	{ 168, 191, 0, 135 },
	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
*/

// �o�g���{�b�N�X�E���C��
static const CURSORMOVE_DATA BattleBoxMainCursorMoveData[] =
{
	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  6, 5, 1, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },		// 00: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  7, 0, 2, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  8, 1, 3, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  9, 2, 4, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 10, 3, 5, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 11, 4, 0, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	0, 12, 11,  7, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 06: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	1, 13,  6,  8, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	2, 14,  7,  9, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	3, 15,  8, 10, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	4, 16,  9, 11, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	5, 17, 10,  6, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 6, 18, 17, 13, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 12: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 7, 19, 12, 14, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 8, 20, 13, 15, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 9, 21, 14, 16, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	10, 22, 15, 17, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	11, 23, 16, 12, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	12, 24, 23, 19, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 18: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	13, 25, 18, 20, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	14, 26, 19, 21, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	15, 27, 20, 22, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	16, 28, 21, 23, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	17, 29, 22, 18, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	18, 33, 29, 25, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 24: �|�P����
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	19, 33, 24, 26, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	20, 33, 25, 27, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	21, 33, 26, 28, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	22, 33, 27, 29, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	23, 33, 28, 24, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{  84,  16, 0, 0,	33, CURSORMOVE_RETBIT|0, 30, 30, {  17,  39,  26, 141 } },		// 30: �{�b�N�X��
	{   0,   0, 0, 0,	31, 31, 31, 31, {  17,  38,   1,  22 } },						// 31: �g���C�؂�ւ����E��
	{   0,   0, 0, 0,	32, 32, 32, 32, {  17,  38, 145, 166 } },						// 32: �g���C�؂�ւ����E�E

	{  44, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 33, { 168, 191,   0,  95 } },		// 33: �o�g���{�b�N�X

	{ 0, 0, 0, 0,	34, 34, 34, 34, { TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_06, TOUCHBAR_ICON_X_06+TOUCHBAR_ICON_WIDTH-1 } },			// 34: �߂�P
	{ 0, 0, 0, 0,	35, 35, 35, 35, { TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH-1 } },			// 35: �߂�Q

	{ 212,  64, 0, 0,	39, 37, 36, 36, {  64,  87, 168, 255 } },	// 36: ���ǂ�����
	{ 212,  88, 0, 0,	36, 38, 37, 37, {  88, 111, 168, 255 } },	// 37: �悤�����݂�
	{ 212, 112, 0, 0,	37, 39, 38, 38, { 112, 135, 168, 255 } },	// 38: ��������
	{ 212, 136, 0, 0,	38, 36, 39, 39, { 136, 159, 168, 255 } },	// 39: ��߂�

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
static const CURSORMOVE_CALLBACK BattleBoxMainCallBack = {
	BattleBoxMainCallBack_On,
	CursorMoveCallBack_Off,
	BattleBoxMainCallBack_Move,
	BattleBoxMainCallBack_Touch
};

// �o�g���{�b�N�X�E�p�[�e�B����
static const CURSORMOVE_DATA BattleBoxPartyMainCursorMoveData[] =
{
	{ 40,  52, 0, 0,	6, 2, 0, 1, {  68-12,  68+12-1, 42-12, 42+12-1 } },	// 00: �|�P�����P
	{ 80,  60, 0, 0,	6, 3, 0, 2, {  76-12,  76+12-1, 78-12, 78+12-1 } },	// 01: �|�P�����Q
	{ 40,  84, 0, 0,	0, 4, 1, 3, { 100-12, 100+12-1, 42-12, 42+12-1 } },	// 02: �|�P�����R
	{ 80,  92, 0, 0,	1, 5, 2, 4, { 108-12, 108+12-1, 78-12, 78+12-1 } },	// 03: �|�P�����S
	{ 40, 116, 0, 0,	2, 6, 3, 5, { 132-12, 132+12-1, 42-12, 42+12-1 } },	// 04: �|�P�����T
	{ 80, 124, 0, 0,	3, 6, 4, 6, { 140-12, 140+12-1, 78-12, 78+12-1 } },	// 05: �|�P�����U

	{ 44, 168, 0, 0,	CURSORMOVE_RETBIT|5, 0, 5, 0, { 168, 191, 0, 95 } },		// 06:�u�{�b�N�X���X�g�v

	{ 0, 0, 0, 0,	7, 7, 7, 7, { TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_06, TOUCHBAR_ICON_X_06+TOUCHBAR_ICON_WIDTH-1 } },			// 07: �߂�P
	{ 0, 0, 0, 0,	8, 8, 8, 8, { TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH-1 } },			// 08: �߂�Q

	{ 212,  64, 0, 0,	12, 10,  9,  9, {  64,  87, 168, 255 } },	// 09: ���ǂ�����
	{ 212,  88, 0, 0,	 9, 11, 10, 10, {  88, 111, 168, 255 } },	// 10: �悤�����݂�
	{ 212, 112, 0, 0,	10, 12, 11, 11, { 112, 135, 168, 255 } },	// 11: ��������
	{ 212, 136, 0, 0,	11,  9, 12, 12, { 136, 159, 168, 255 } },	// 12: �Ƃ���

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
static const CURSORMOVE_CALLBACK BattleBoxPartyMainCallBack = {
	BattleBoxPartyMainCallBack_On,
	CursorMoveCallBack_Off,
	BattleBoxPartyMainCallBack_Move,
	CursorMoveCallBack_Touch
};





// �^�b�`�`�F�b�N�̏I���f�[�^
static const GFL_UI_TP_HITTBL TpCheckEnd = { GFL_UI_TP_HIT_END, 0, 0, 0 };

// �J�[�\���ړ��f�[�^�e�[�u��
static const MOVE_DATA_TBL MoveDataTable[] =
{
	{	// ��������E���C��
		PartyOutMainCursorMoveData,
		&PartyOutMainCallBack
	},
	{	// ��������E�{�b�N�X�I��
		PartyOutBoxSelCursorMoveData,
		&PartyOutBoxSelCallBack
	},

	{	// ��Ă����E���C��
		PartyInMainCursorMoveData,
		&PartyInMainCallBack
	},

	{	// �{�b�N�X�����E���C��
		BoxArrangeMainCursorMoveData,
		&BoxArrangeMainCallBack
	},
	{	// �{�b�N�X�����E�|�P�����ړ��i�{�b�N�X�ԁj
		BoxArrangePokeMoveCursorMoveData,
		&BoxArrangePokeMoveCallBack
	},
	{	// �{�b�N�X�����E�莝���|�P����
		BoxArrangePartyPokeCursorMoveData,
		&BoxArrangePartyPokeCallBack
	},
	{	// �{�b�N�X�����E�|�P�����ړ��i�莝���j
		BoxArrangePartyPokeMoveCursorMoveData,
		&BoxArrangePartyPokeMoveCallBack
	},

	{	// �����������E���C��
		BoxItemArrangeMainCursorMoveData,
		&BoxItemArrangeMainCallBack
	},
	{	// �����������E�莝���|�P����
		BoxItemArrangePartyPokeCursorMoveData,
		&BoxItemArrangePartyPokeCallBack
	},

	{	// �}�[�L���O
		MarkingCursorMoveData,
		&MarkingCallBack
	},

	{	// �{�b�N�X�e�[�}�ύX
		BoxThemaChgCursorMoveData,
		&BoxThemaChgCallBack
	},

	{	// �ǎ��ύX
		WallPaperChgCursorMoveData,
		&WallPaperChgCallBack
	},

	{	// �{�b�N�X�W�����v
		BoxJumpCursorMoveData,
		&BoxJumpCallBack
	},

	{	// �o�g���{�b�N�X�E���C��
		BattleBoxMainCursorMoveData,
		&BattleBoxMainCallBack
	},
	{	// �o�g���{�b�N�X�E�p�[�e�B����
		BattleBoxPartyMainCursorMoveData,
		&BattleBoxPartyMainCallBack
	},
};



//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ����[�N������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2UI_CursorMoveInit( BOX2_SYS_WORK * syswk )
{
	u32		id;

	switch( syswk->dat->callMode ){
	case BOX_MODE_AZUKERU:
		id = BOX2UI_INIT_ID_PTOUT_MAIN;
		break;
	case BOX_MODE_TURETEIKU:
		id = BOX2UI_INIT_ID_PTIN_MAIN;
		break;
	case BOX_MODE_SEIRI:
		id = BOX2UI_INIT_ID_ARRANGE_MAIN;
		break;
	case BOX_MODE_ITEM:
		id = BOX2UI_INIT_ID_ITEM_A_MAIN;
		break;

	case BOX_MODE_BATTLE:
		id = BOX2UI_INIT_ID_BATTLEBOX_MAIN;
		break;
	}

	BOX2UI_CursorMoveExit( syswk );

	syswk->app->cmwk = CURSORMOVE_Create(
							MoveDataTable[id].dat,
							MoveDataTable[id].cb,
							syswk,
							TRUE,
							syswk->cur_rcv_pos,			// pos
							HEAPID_BOX_APP );

	CursorObjMove( syswk, syswk->cur_rcv_pos );

	syswk->app->old_cur_pos = syswk->cur_rcv_pos;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ����[�N�؂�ւ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	id		�J�[�\���ړ��f�[�^�ԍ�
 * @param	pos		�J�[�\�������ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2UI_CursorMoveChange( BOX2_SYS_WORK * syswk, u32 id, u32 pos )
{
	BOX2UI_CursorMoveExit( syswk );

	syswk->app->cmwk = CURSORMOVE_Create(
							MoveDataTable[id].dat,
							MoveDataTable[id].cb,
							syswk,
							TRUE,
							pos,					// pos
							HEAPID_BOX_APP );

	CursorObjMove( syswk, pos );

	syswk->app->old_cur_pos = pos;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ����[�N�폜
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2UI_CursorMoveExit( BOX2_SYS_WORK * syswk )
{
	if( syswk->app->cmwk != NULL ){
		CURSORMOVE_Exit( syswk->app->cmwk );
		syswk->app->cmwk = NULL;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���n�a�i�ړ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CursorObjMove( BOX2_SYS_WORK * syswk, int pos )
{
	const CURSORMOVE_DATA * pw;
	u8	x, y;

	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, pos );

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );

	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );
	BOX2OBJ_MovePokeIconHand( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ��ėp�R�[���o�b�N�F�J�[�\���\����
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ��ėp�R�[���o�b�N�F�J�[�\����\����
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveCallBack_Off( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ��ėp�R�[���o�b�N�F�J�[�\���ړ���
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );

	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ��ėp�R�[���o�b�N�F�^�b�`��
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveCallBack_Touch( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P��������������v���C������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_PartyOutMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	u8	now;

	now = CURSORMOVE_PosGet( syswk->app->cmwk );
	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	if( ret < NELEMS(PartyOutMainCursorMoveData) ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			if( ret >= BOX2UI_PTOUT_MAIN_AZUKERU && ret <= BOX2UI_PTOUT_MAIN_CLOSE ){
				CURSORMOVE_PosSet( syswk->app->cmwk, now );
				CursorObjMove( syswk, now );
				return CURSORMOVE_NONE;
			}
		}else{
			if( ret == BOX2UI_PTOUT_MAIN_RETURN ){
				return BOX2UI_PTOUT_MAIN_CLOSE;
			}
		}
		return ret;
	}

	if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
		if( ret == BOX2UI_PTOUT_MAIN_RETURN || ret == CURSORMOVE_CANCEL ){
			return BOX2UI_PTOUT_MAIN_CLOSE;
		}
	}

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P��������������v���C������R�[���o�b�N�֐��F�J�[�\���\����
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PartyOutMainCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	
	syswk = work;

	if( old_pos != CURSORMOVE_ONOFF_DIRECT ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			if( syswk->get_pos != BOX2MAIN_GETPOS_NONE &&
				syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
				now_pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
			}else{
				now_pos = BOX2UI_PTOUT_MAIN_POKE1;
			}
		}else{
			if( now_pos != BOX2UI_PTOUT_MAIN_STATUS &&
				now_pos != BOX2UI_PTOUT_MAIN_MARKING &&
				now_pos != BOX2UI_PTOUT_MAIN_FREE ){
				now_pos = BOX2UI_PTOUT_MAIN_AZUKERU;
			}
		}
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P��������������v�a����g���C�I���R�[���o�b�N�֐��F�J�[�\���\����
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PartyOutBoxSelCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	u8	x, y;

	syswk = work;

	if( now_pos == BOX2UI_PTOUT_BOXSEL_LEFT || now_pos == BOX2UI_PTOUT_BOXSEL_RIGHT ){
		now_pos = syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;
		syswk->app->old_cur_pos = now_pos;
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, now_pos );

	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );

	if( now_pos >= BOX2UI_PTOUT_BOXSEL_TRAY1 && now_pos <= BOX2UI_PTOUT_BOXSEL_TRAY6 ){
//		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
	}else{
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P��������������v�a����g���C�I���R�[���o�b�N�֐��F�J�[�\���ړ���
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PartyOutBoxSelCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	if( now_pos >= BOX2UI_PTOUT_BOXSEL_TRAY1 && now_pos <= BOX2UI_PTOUT_BOXSEL_TRAY6 ){
//		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
	}else{
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	}

	//�u��������v����g���C�ւ̈ړ�
	if( old_pos == BOX2UI_PTOUT_BOXSEL_ENTER && now_pos == BOX2UI_PTOUT_BOXSEL_TRAY1 ){
		now_pos = syswk->app->old_cur_pos;
		CURSORMOVE_PosSetEx( syswk->app->cmwk, now_pos, BOX2UI_PTOUT_BOXSEL_ENTER, BOX2UI_PTOUT_BOXSEL_ENTER );
	}

	// �O��ʒu�X�V
	if( now_pos >= BOX2UI_PTOUT_BOXSEL_TRAY1 && now_pos <= BOX2UI_PTOUT_BOXSEL_TRAY6 ){
		syswk->app->old_cur_pos = now_pos;
	}

	// �g���C������E�ւ̃��[�v
	if( now_pos == BOX2UI_PTOUT_BOXSEL_TRAY1 && old_pos == BOX2UI_PTOUT_BOXSEL_TRAY6 ){
		if( syswk->box_mv_pos + 1 >= syswk->trayMax ){
			syswk->box_mv_pos = 0;
		}else{
			syswk->box_mv_pos++;
		}
		BOX2OBJ_TrayIconChange( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2BMP_BoxMoveNameWrite( syswk, 0 );
//		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_RA, BOX2OBJ_ANM_R_ARROW_ON );
		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	// �g���C�E���獶�ւ̃��[�v
	if( now_pos == BOX2UI_PTOUT_BOXSEL_TRAY6 && old_pos == BOX2UI_PTOUT_BOXSEL_TRAY1 ){
		if( syswk->box_mv_pos - 1 < 0 ){
			syswk->box_mv_pos = syswk->trayMax - 1;
		}else{
			syswk->box_mv_pos--;
		}
		BOX2OBJ_TrayIconChange( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2BMP_BoxMoveNameWrite( syswk, 0 );
//		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_LA, BOX2OBJ_ANM_L_ARROW_ON );
		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	if( now_pos >= BOX2UI_PTOUT_BOXSEL_TRAY1 && now_pos <= BOX2UI_PTOUT_BOXSEL_TRAY6 ){
		if( old_pos != BOX2UI_PTOUT_BOXSEL_ENTER ){
			syswk->box_mv_pos = BOX2MAIN_MV_TRAY_DEF(syswk->box_mv_pos) + now_pos;
			BOX2OBJ_BoxMoveNameSet( syswk );
			BOX2BMP_BoxMoveNameWrite( syswk, 0 );
		}
	}

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P��������������v�a����g���C�I���R�[���o�b�N�֐��F�^�b�`��
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PartyOutBoxSelCallBack_Touch( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	if( now_pos != BOX2UI_PTOUT_BOXSEL_LEFT && now_pos != BOX2UI_PTOUT_BOXSEL_RIGHT ){
		CursorObjMove( syswk, now_pos );
	}
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P��������Ă����v���C������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_PartyInMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	u8	now;

	now = CURSORMOVE_PosGet( syswk->app->cmwk );
	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	if( ret < NELEMS(PartyInMainCursorMoveData) ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			if( ret >= BOX2UI_PTIN_MAIN_TSURETEIKU && ret <= BOX2UI_PTIN_MAIN_CLOSE ){
				CURSORMOVE_PosSet( syswk->app->cmwk, now );
				CursorObjMove( syswk, now );
				return CURSORMOVE_NONE;
			}
		}else{
			if( ret == BOX2UI_PTIN_MAIN_RETURN ){
				return BOX2UI_PTIN_MAIN_CLOSE;
			}
		}
		return ret;
	}

	if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
		if( ret == BOX2UI_PTIN_MAIN_RETURN || ret == CURSORMOVE_CANCEL ){
			return BOX2UI_PTIN_MAIN_CLOSE;
		}
	}

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P��������Ă����v���C������R�[���o�b�N�֐��F�J�[�\���\����
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PartyInMainCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	
	syswk = work;

	if( old_pos != CURSORMOVE_ONOFF_DIRECT ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			if( now_pos == BOX2UI_PTIN_MAIN_LEFT || now_pos == BOX2UI_PTIN_MAIN_RIGHT ){
				now_pos = BOX2UI_PTIN_MAIN_NAME;
			}else if( now_pos != BOX2UI_PTIN_MAIN_NAME && now_pos != BOX2UI_PTIN_MAIN_RETURN ){
				if( syswk->get_pos != BOX2MAIN_GETPOS_NONE &&
					syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
					now_pos = syswk->get_pos;
				}else{
					now_pos = BOX2UI_PTIN_MAIN_POKE1;
				}
			}
		}else{
			if( now_pos != BOX2UI_PTIN_MAIN_STATUS &&
				now_pos != BOX2UI_PTIN_MAIN_MARKING &&
				now_pos != BOX2UI_PTIN_MAIN_FREE ){
				now_pos = BOX2UI_PTIN_MAIN_TSURETEIKU;
			}
		}
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P��������Ă����v���C������R�[���o�b�N�֐��F�^�b�`��
 *
 * @param	syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PartyInMainCallBack_Touch( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	if( now_pos != BOX2UI_PTIN_MAIN_LEFT && now_pos != BOX2UI_PTIN_MAIN_RIGHT ){
		CursorObjMove( syswk, now_pos );
	}
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v���C������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_BoxArrangeMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	u8	now;

	now = CURSORMOVE_PosGet( syswk->app->cmwk );
	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	// ���j���[��\��
	if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
		if( ret >= BOX2UI_ARRANGE_MAIN_GET && ret <= BOX2UI_ARRANGE_MAIN_CLOSE ){
			CURSORMOVE_PosSet( syswk->app->cmwk, now );
			CursorObjMove( syswk, now );
			return CURSORMOVE_NONE;
		}
	// ���j���[�\����
	}else{
		if( ret == BOX2UI_ARRANGE_MAIN_PARTY ||
				ret == BOX2UI_ARRANGE_MAIN_RETURN1 ||
				ret == BOX2UI_ARRANGE_MAIN_RETURN2 ){
			CURSORMOVE_PosSet( syswk->app->cmwk, now );
			CursorObjMove( syswk, now );
			return CURSORMOVE_NONE;
		}
		if( ret == CURSORMOVE_CANCEL ){
			return BOX2UI_ARRANGE_MAIN_CLOSE;
		}
	}

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v���C������R�[���o�b�N�֐��F�J�[�\���\����
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangeMainCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	
	syswk = work;

	if( old_pos != CURSORMOVE_ONOFF_DIRECT ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			switch( now_pos ){
			case BOX2UI_ARRANGE_MAIN_LEFT:
			case BOX2UI_ARRANGE_MAIN_RIGHT:
				now_pos = BOX2UI_ARRANGE_MAIN_NAME;
				break;

			case BOX2UI_ARRANGE_MAIN_NAME:
			case BOX2UI_ARRANGE_MAIN_PARTY:
			case BOX2UI_ARRANGE_MAIN_RETURN1:
			case BOX2UI_ARRANGE_MAIN_RETURN2:
				break;

			default:
				if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
					now_pos = syswk->get_pos;
				}else{
					now_pos = BOX2UI_ARRANGE_MOVE_POKE1;
				}
			}
		}else{
			if( now_pos != BOX2UI_ARRANGE_MAIN_GET ||
				now_pos != BOX2UI_ARRANGE_MAIN_STATUS ||
				now_pos != BOX2UI_ARRANGE_MAIN_ITEM ||
				now_pos != BOX2UI_ARRANGE_MAIN_MARKING ||
				now_pos != BOX2UI_ARRANGE_MAIN_FREE ||
				now_pos != BOX2UI_ARRANGE_MAIN_CLOSE ){

				now_pos = BOX2UI_ARRANGE_MAIN_GET;
			}
		}
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v���C������R�[���o�b�N�֐��F�J�[�\���ړ���
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangeMainCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );

	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v���C������R�[���o�b�N�֐��F�^�b�`��
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangeMainCallBack_Touch( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v�|�P�����ړ�����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_BoxArrangePokeMove( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	
	if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == FALSE ){
		CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_CHANGE );
	}

	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	CURSORMOVE_MoveTableInit( syswk->app->cmwk );

	return ret;
}

// �|�P�����擾���̃^�b�`�o�[�{�^���ݒ�
static void SetTouchBarIconPokeGet( BOX2_SYS_WORK * syswk, u32 pos, u32 max )
{
	u8	ret, status;

	ret    = BOX2OBJ_TB_ICON_ON;
	status = BOX2OBJ_TB_ICON_ON;

	if( syswk->poke_get_key == 1 ){
		ret = BOX2OBJ_TB_ICON_PASSIVE;
	}else{
		if( pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
			status = BOX2OBJ_TB_ICON_PASSIVE;
		}else if( BOX2MAIN_PokeParaGet(syswk,pos,syswk->tray,ID_PARA_poke_exist,NULL) == 0 ){
			status = BOX2OBJ_TB_ICON_PASSIVE;
		}
	}
	BOX2OBJ_SetTouchBarButton( syswk, ret, BOX2OBJ_TB_ICON_OFF, status );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v�|�P�����ړ�����R�[���o�b�N�֐��F�J�[�\���\����
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangePokeMoveCallBack_On( void * work, int now_pos, int old_pos )
{
/*
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	u8	x, y;

	syswk = work;

	if( now_pos == BOX2UI_ARRANGE_MOVE_LEFT || now_pos == BOX2UI_ARRANGE_MOVE_RIGHT ){
		now_pos = BOX2UI_ARRANGE_MOVE_TRAY1 + syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;
		syswk->app->old_cur_pos = now_pos;
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, now_pos );

	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );

	if( syswk->poke_get_key == 0 ){
		if( now_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && now_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
			BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
		}else{
			BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
		}
	}
*/
	BOX2_SYS_WORK * syswk;

	syswk = work;

//	CursorObjMove( syswk, now_pos );

	if( now_pos == BOX2UI_ARRANGE_PGT_LEFT ||			// 31: �{�b�N�X�؂�ւ����i���j
			now_pos == BOX2UI_ARRANGE_PGT_RIGHT ||		// 32: �{�b�N�X�؂�ւ����i�E�j
			now_pos == BOX2UI_ARRANGE_PGT_TRAY1 ||		// 33: �g���C�A�C�R��
			now_pos == BOX2UI_ARRANGE_PGT_TRAY6 ||		// 38: �g���C�A�C�R��
			now_pos == BOX2UI_ARRANGE_PGT_STTUS ||		// 40: �X�e�[�^�X
			now_pos == BOX2UI_ARRANGE_PGT_RET ){			// 41: �߂�
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->app->old_cur_pos );
		now_pos = syswk->app->old_cur_pos;
	}else{
		CursorObjMove( syswk, now_pos );
	}

	SetTouchBarIconPokeGet( syswk, now_pos, BOX2OBJ_POKEICON_TRAY_MAX );

	syswk->app->old_cur_pos = now_pos;
}

// �J�[�\�����[�v�̂��߂̈ʒu
#define	BAPM_TRAY_POKE_LINE1_S	( BOX2UI_ARRANGE_MOVE_POKE1 )
#define	BAPM_TRAY_POKE_LINE1_E	( BOX2UI_ARRANGE_MOVE_POKE1 + BOX2OBJ_POKEICON_H_MAX - 1 )
#define	BAPM_TRAY_POKE_LINE5_S	( BOX2UI_ARRANGE_MOVE_POKE1 + BOX2OBJ_POKEICON_TRAY_MAX - BOX2OBJ_POKEICON_H_MAX )
#define	BAPM_TRAY_POKE_LINE5_E	( BOX2UI_ARRANGE_MOVE_POKE1 + BOX2OBJ_POKEICON_TRAY_MAX - 1 )
#define	BAPM_PARTY_POKE_LINE1_S	( BOX2UI_ARRANGE_MOVE_PARTY1 )
#define	BAPM_PARTY_POKE_LINE1_E	( BOX2UI_ARRANGE_MOVE_PARTY1 + 1 )

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v�|�P�����ړ�����R�[���o�b�N�֐��F�J�[�\���ړ���
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangePokeMoveCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

/*
	if( syswk->poke_get_key == 0 ){
		if( now_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && now_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
			BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
		}else{
			BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
		}
	}

	// �|�P�����A�C�R������g���C�ւ̈ړ�
	if( now_pos == BOX2UI_ARRANGE_MOVE_TRAY1 &&
		( ( old_pos >= BAPM_TRAY_POKE_LINE1_S && old_pos <= BAPM_TRAY_POKE_LINE1_E ) ||
		  ( old_pos >= BAPM_TRAY_POKE_LINE5_S && old_pos <= BAPM_TRAY_POKE_LINE5_E ) ||
		  ( old_pos >= BAPM_PARTY_POKE_LINE1_S && old_pos <= BAPM_PARTY_POKE_LINE1_E ) ||
		  old_pos == BOX2UI_ARRANGE_MOVE_RETURN ) ){

		now_pos = syswk->app->old_cur_pos;
		CURSORMOVE_PosSetEx( syswk->app->cmwk, now_pos, old_pos, CURSORMOVE_SavePosGet(syswk->app->cmwk) );
	}

	// �O��ʒu�X�V
	if( now_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && now_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
		syswk->app->old_cur_pos = now_pos;
	}

	// �g���C������E�ւ̃��[�v
	if( now_pos == BOX2UI_ARRANGE_MOVE_TRAY1 && old_pos == BOX2UI_ARRANGE_MOVE_TRAY6 ){
		if( syswk->box_mv_pos + 1 >= syswk->trayMax ){
			syswk->box_mv_pos = 0;
		}else{
			syswk->box_mv_pos++;
		}
		BOX2OBJ_TrayIconChange( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2BMP_BoxMoveNameWrite( syswk, 0 );
//		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_RA, BOX2OBJ_ANM_R_ARROW_ON );
		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	// �g���C�E���獶�ւ̃��[�v
	if( now_pos == BOX2UI_ARRANGE_MOVE_TRAY6 && old_pos == BOX2UI_ARRANGE_MOVE_TRAY1 ){
		if( syswk->box_mv_pos - 1 < 0 ){
			syswk->box_mv_pos = syswk->trayMax - 1;
		}else{
			syswk->box_mv_pos--;
		}
		BOX2OBJ_TrayIconChange( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2BMP_BoxMoveNameWrite( syswk, 0 );
//		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_LA, BOX2OBJ_ANM_L_ARROW_ON );
		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	if( now_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && now_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
		if( old_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && old_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
			syswk->box_mv_pos = BOX2MAIN_MV_TRAY_DEF(syswk->box_mv_pos) + now_pos - BOX2UI_ARRANGE_MOVE_TRAY1;
			BOX2OBJ_BoxMoveNameSet( syswk );
			BOX2BMP_BoxMoveNameWrite( syswk, 0 );
		}
	}
*/

/*
	if( now_pos >= BOX2UI_ARRANGE_PGT_TRAY2 && now_pos <= BOX2UI_ARRANGE_PGT_TRAY5 ){
		BOX2OBJ_ChangeTrayName( syswk, now_pos-BOX2UI_ARRANGE_PGT_TRAY2, TRUE );
	}else{
		BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
	}
*/

	SetTouchBarIconPokeGet( syswk, now_pos, BOX2OBJ_POKEICON_TRAY_MAX );

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );

	syswk->app->old_cur_pos = now_pos;
}

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v�|�P�����ړ�����R�[���o�b�N�֐��F�^�b�`��
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangePokeMoveCallBack_Touch( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;
/*
	// �O��ʒu�X�V
	if( now_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && now_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
		syswk->app->old_cur_pos = now_pos;
	}

	if( now_pos != BOX2UI_ARRANGE_MOVE_LEFT && now_pos != BOX2UI_ARRANGE_MOVE_RIGHT ){
		CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );
		CursorObjMove( syswk, now_pos );
	}
*/
	if( now_pos == BOX2UI_ARRANGE_PGT_LEFT ||			// 31: �{�b�N�X�؂�ւ����i���j
			now_pos == BOX2UI_ARRANGE_PGT_RIGHT ||		// 32: �{�b�N�X�؂�ւ����i�E�j
			now_pos == BOX2UI_ARRANGE_PGT_TRAY1 ||		// 33: �g���C�A�C�R��
			now_pos == BOX2UI_ARRANGE_PGT_TRAY6 ||		// 38: �g���C�A�C�R��
			now_pos == BOX2UI_ARRANGE_PGT_STTUS ||		// 40: �X�e�[�^�X
			now_pos == BOX2UI_ARRANGE_PGT_RET ){			// 41: �߂�
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->app->old_cur_pos );
		now_pos = syswk->app->old_cur_pos;
	}else{
		CursorObjMove( syswk, now_pos );
	}

	SetTouchBarIconPokeGet( syswk, now_pos, BOX2OBJ_POKEICON_TRAY_MAX );

	syswk->app->old_cur_pos = now_pos;
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v�|�P�����ړ����ɃA�C�R����͂�ł���Ƃ��̑���
 *
 * @param	syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_ArrangePokeGetMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

/*
	if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == FALSE ){
		CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_CHANGE );
	}

	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_LEFT );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_RIGHT );
*/

	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

//	CURSORMOVE_MoveTableInit( syswk->app->cmwk );

	return ret;
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v�莝���|�P��������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_BoxArrangePartyMoveMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	u8	now;

	now = CURSORMOVE_PosGet( syswk->app->cmwk );
	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == FALSE ){
		if( ret >= BOX2UI_ARRANGE_PARTY_GET && ret <= BOX2UI_ARRANGE_PARTY_CLOSE ){
			CURSORMOVE_PosSet( syswk->app->cmwk, now );
			CursorObjMove( syswk, now );
			return CURSORMOVE_NONE;
		}
	}else{
		if( ret == BOX2UI_ARRANGE_PARTY_BOXLIST ||
				ret == BOX2UI_ARRANGE_PARTY_RETURN1 ||
				ret == BOX2UI_ARRANGE_PARTY_RETURN2 ){
			CURSORMOVE_PosSet( syswk->app->cmwk, now );
			CursorObjMove( syswk, now );
			return CURSORMOVE_NONE;
		}

		if( ret == CURSORMOVE_CANCEL ){
			return BOX2UI_ARRANGE_PARTY_CLOSE;
		}
	}

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v�莝���|�P��������R�[���o�b�N�֐��F�J�[�\���\����
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangePartyMoveCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	
	syswk = work;

	if( old_pos != CURSORMOVE_ONOFF_DIRECT ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			if( now_pos >= BOX2UI_ARRANGE_PARTY_BOXLIST ){
				if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
					now_pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
				}else{
					now_pos = BOX2UI_ARRANGE_PARTY_POKE1;
				}
			}
		}else{
			if( now_pos != BOX2UI_ARRANGE_PARTY_GET ||
				now_pos != BOX2UI_ARRANGE_PARTY_STATUS ||
				now_pos != BOX2UI_ARRANGE_PARTY_ITEM ||
				now_pos != BOX2UI_ARRANGE_PARTY_MARKING ||
				now_pos != BOX2UI_ARRANGE_PARTY_FREE ||
				now_pos != BOX2UI_ARRANGE_PARTY_CLOSE ){

				now_pos = BOX2UI_ARRANGE_PARTY_GET;
			}
		}
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v�莝���|�P��������R�[���o�b�N�֐��F�J�[�\���ړ���
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangePartyMoveCallBack_Move( void * work, int now_pos, int old_pos )
{
/*
	BOX2_SYS_WORK * syswk = work;

	// ���j���[���\������Ă�Ƃ�
	if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
		if( now_pos <= BOX2UI_ARRANGE_PARTY_BOXLIST ){
			if( old_pos >= BOX2UI_ARRANGE_PARTY_RETURN ){
				BOX2MAIN_PokeSelectOff( syswk );
				BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
				BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncMenuCloseKeyArrange );
				return;
			}
		}
	}else{
		if( now_pos == BOX2UI_ARRANGE_PARTY_FREE && old_pos == BOX2UI_ARRANGE_PARTY_RETURN ){
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_PARTY_RETURN );
			now_pos = BOX2UI_ARRANGE_PARTY_RETURN;
		}
	}

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );

	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
*/
	BOX2_SYS_WORK * syswk = work;

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );

	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}

//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v�|�P�����ړ����ɃA�C�R����͂�ł���Ƃ��̑���
 *
 * @param	syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_ArrangePartyPokeGetMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

/*
	if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == FALSE ){
		CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_CHANGE );
	}

	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_LEFT );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_RIGHT );
*/

	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

//	CURSORMOVE_MoveTableInit( syswk->app->cmwk );

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v�|�P�����ړ�����R�[���o�b�N�֐��F�J�[�\���\����
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangePartyPokeMoveCallBack_On( void * work, int now_pos, int old_pos )
{
/*
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	u8	x, y;

	syswk = work;

	if( now_pos == BOX2UI_ARRANGE_MOVE_LEFT || now_pos == BOX2UI_ARRANGE_MOVE_RIGHT ){
		now_pos = BOX2UI_ARRANGE_MOVE_TRAY1 + syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;
		syswk->app->old_cur_pos = now_pos;
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, now_pos );

	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );

	if( syswk->poke_get_key == 0 ){
		if( now_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && now_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
			BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
		}else{
			BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
		}
	}
*/
	BOX2_SYS_WORK * syswk;

	syswk = work;

//	CursorObjMove( syswk, now_pos );

	if( now_pos == BOX2UI_ARRANGE_PTGT_LEFT ||		// 31: �{�b�N�X�؂�ւ����i���j
			now_pos == BOX2UI_ARRANGE_PTGT_RIGHT ||		// 32: �{�b�N�X�؂�ւ����i�E�j
			now_pos == BOX2UI_ARRANGE_PTGT_STTUS ||		// 40: �X�e�[�^�X
			now_pos == BOX2UI_ARRANGE_PTGT_RET ){			// 41: �߂�
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->app->old_cur_pos );
		now_pos = syswk->app->old_cur_pos;
	}else{
		CursorObjMove( syswk, now_pos );
	}

	SetTouchBarIconPokeGet( syswk, now_pos, BOX2OBJ_POKEICON_PUT_MAX );

	syswk->app->old_cur_pos = now_pos;
}

// �J�[�\�����[�v�̂��߂̈ʒu
#define	BAPM_TRAY_POKE_LINE1_S	( BOX2UI_ARRANGE_MOVE_POKE1 )
#define	BAPM_TRAY_POKE_LINE1_E	( BOX2UI_ARRANGE_MOVE_POKE1 + BOX2OBJ_POKEICON_H_MAX - 1 )
#define	BAPM_TRAY_POKE_LINE5_S	( BOX2UI_ARRANGE_MOVE_POKE1 + BOX2OBJ_POKEICON_TRAY_MAX - BOX2OBJ_POKEICON_H_MAX )
#define	BAPM_TRAY_POKE_LINE5_E	( BOX2UI_ARRANGE_MOVE_POKE1 + BOX2OBJ_POKEICON_TRAY_MAX - 1 )
#define	BAPM_PARTY_POKE_LINE1_S	( BOX2UI_ARRANGE_MOVE_PARTY1 )
#define	BAPM_PARTY_POKE_LINE1_E	( BOX2UI_ARRANGE_MOVE_PARTY1 + 1 )

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v�|�P�����ړ�����R�[���o�b�N�֐��F�J�[�\���ړ���
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangePartyPokeMoveCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

/*
	if( syswk->poke_get_key == 0 ){
		if( now_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && now_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
			BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
		}else{
			BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
		}
	}

	// �|�P�����A�C�R������g���C�ւ̈ړ�
	if( now_pos == BOX2UI_ARRANGE_MOVE_TRAY1 &&
		( ( old_pos >= BAPM_TRAY_POKE_LINE1_S && old_pos <= BAPM_TRAY_POKE_LINE1_E ) ||
		  ( old_pos >= BAPM_TRAY_POKE_LINE5_S && old_pos <= BAPM_TRAY_POKE_LINE5_E ) ||
		  ( old_pos >= BAPM_PARTY_POKE_LINE1_S && old_pos <= BAPM_PARTY_POKE_LINE1_E ) ||
		  old_pos == BOX2UI_ARRANGE_MOVE_RETURN ) ){

		now_pos = syswk->app->old_cur_pos;
		CURSORMOVE_PosSetEx( syswk->app->cmwk, now_pos, old_pos, CURSORMOVE_SavePosGet(syswk->app->cmwk) );
	}

	// �O��ʒu�X�V
	if( now_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && now_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
		syswk->app->old_cur_pos = now_pos;
	}

	// �g���C������E�ւ̃��[�v
	if( now_pos == BOX2UI_ARRANGE_MOVE_TRAY1 && old_pos == BOX2UI_ARRANGE_MOVE_TRAY6 ){
		if( syswk->box_mv_pos + 1 >= syswk->trayMax ){
			syswk->box_mv_pos = 0;
		}else{
			syswk->box_mv_pos++;
		}
		BOX2OBJ_TrayIconChange( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2BMP_BoxMoveNameWrite( syswk, 0 );
//		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_RA, BOX2OBJ_ANM_R_ARROW_ON );
		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	// �g���C�E���獶�ւ̃��[�v
	if( now_pos == BOX2UI_ARRANGE_MOVE_TRAY6 && old_pos == BOX2UI_ARRANGE_MOVE_TRAY1 ){
		if( syswk->box_mv_pos - 1 < 0 ){
			syswk->box_mv_pos = syswk->trayMax - 1;
		}else{
			syswk->box_mv_pos--;
		}
		BOX2OBJ_TrayIconChange( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2BMP_BoxMoveNameWrite( syswk, 0 );
//		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_LA, BOX2OBJ_ANM_L_ARROW_ON );
		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	if( now_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && now_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
		if( old_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && old_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
			syswk->box_mv_pos = BOX2MAIN_MV_TRAY_DEF(syswk->box_mv_pos) + now_pos - BOX2UI_ARRANGE_MOVE_TRAY1;
			BOX2OBJ_BoxMoveNameSet( syswk );
			BOX2BMP_BoxMoveNameWrite( syswk, 0 );
		}
	}
*/

	SetTouchBarIconPokeGet( syswk, now_pos, BOX2OBJ_POKEICON_PUT_MAX );

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );

	syswk->app->old_cur_pos = now_pos;
}

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v�|�P�����ړ�����R�[���o�b�N�֐��F�^�b�`��
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangePartyPokeMoveCallBack_Touch( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;
/*
	// �O��ʒu�X�V
	if( now_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && now_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
		syswk->app->old_cur_pos = now_pos;
	}

	if( now_pos != BOX2UI_ARRANGE_MOVE_LEFT && now_pos != BOX2UI_ARRANGE_MOVE_RIGHT ){
		CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );
		CursorObjMove( syswk, now_pos );
	}
*/
	if( now_pos == BOX2UI_ARRANGE_PTGT_LEFT ||			// 31: �{�b�N�X�؂�ւ����i���j
			now_pos == BOX2UI_ARRANGE_PTGT_RIGHT ||		// 32: �{�b�N�X�؂�ւ����i�E�j
			now_pos == BOX2UI_ARRANGE_PTGT_STTUS ||		// 40: �X�e�[�^�X
			now_pos == BOX2UI_ARRANGE_PTGT_RET ){			// 41: �߂�
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->app->old_cur_pos );
		now_pos = syswk->app->old_cur_pos;
	}else{
		CursorObjMove( syswk, now_pos );
	}

	SetTouchBarIconPokeGet( syswk, now_pos, BOX2OBJ_POKEICON_PUT_MAX );

	syswk->app->old_cur_pos = now_pos;
}







//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �u�������̂������肷��v���C������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_BoxItemArrangeMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	u8	now;

	now = CURSORMOVE_PosGet( syswk->app->cmwk );
	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
		if( ret == BOX2UI_ITEM_A_MAIN_MENU1 ){
			CURSORMOVE_PosSet( syswk->app->cmwk, now );
			CursorObjMove( syswk, now );
			return CURSORMOVE_NONE;
		}
	}else{
		if( ret == BOX2UI_ITEM_A_MAIN_PARTY || ret == BOX2UI_ITEM_A_MAIN_CHANGE ){
			CURSORMOVE_PosSet( syswk->app->cmwk, now );
			CursorObjMove( syswk, now );
			return CURSORMOVE_NONE;
		}
		if( ret == BOX2UI_ITEM_A_MAIN_RETURN || ret == CURSORMOVE_CANCEL ){
			return BOX2UI_ITEM_A_MAIN_CLOSE;
		}
	}

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * �u�������̂������肷��v���C������R�[���o�b�N�֐��F�J�[�\���\����
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxItemArrangeMainCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	
	syswk = work;

	if( old_pos != CURSORMOVE_ONOFF_DIRECT ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			switch( now_pos ){
			case BOX2UI_ITEM_A_MAIN_LEFT:
			case BOX2UI_ITEM_A_MAIN_RIGHT:
				now_pos = BOX2UI_ITEM_A_MAIN_NAME;
				break;

			case BOX2UI_ITEM_A_MAIN_NAME:
			case BOX2UI_ITEM_A_MAIN_PARTY:
			case BOX2UI_ITEM_A_MAIN_CHANGE:
			case BOX2UI_ITEM_A_MAIN_RETURN:
				break;

			default:
				if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
					now_pos = syswk->get_pos;
				}else{
					now_pos = BOX2UI_ITEM_A_MAIN_POKE1;
				}
			}
		}else{
			if( now_pos != BOX2UI_ITEM_A_MAIN_MENU1 ){
				now_pos = BOX2UI_ITEM_A_MAIN_MENU1;
			}
		}
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�������̂������肷��v���C������R�[���o�b�N�֐��F�J�[�\���ړ���
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxItemArrangeMainCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );

	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�������̂������肷��v���C������R�[���o�b�N�֐��F�^�b�`��
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxItemArrangeMainCallBack_Touch( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �u�������̂������肷��v�莝���|�P��������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_BoxItemArrangePartyMoveMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	u8	now;

	now = CURSORMOVE_PosGet( syswk->app->cmwk );
	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == FALSE ){
		if( ret == BOX2UI_ITEM_A_PARTY_MENU1 ){
			CURSORMOVE_PosSet( syswk->app->cmwk, now );
			CursorObjMove( syswk, now );
			return CURSORMOVE_NONE;
		}
	}else{
		if( ret == BOX2UI_ITEM_A_PARTY_RETURN || ret == CURSORMOVE_CANCEL ){
			return BOX2UI_ITEM_A_PARTY_CLOSE;
		}
	}

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * �u�������̂������肷��v�莝���|�P��������R�[���o�b�N�֐��F�J�[�\���\����
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxItemArrangePartyMoveCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	
	syswk = work;

	if( old_pos != CURSORMOVE_ONOFF_DIRECT ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			if( now_pos >= BOX2UI_ITEM_A_PARTY_MENU1 ){
				if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
					now_pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
				}else{
					now_pos = BOX2UI_ITEM_A_PARTY_POKE1;
				}
			}
		}else{
			if( now_pos != BOX2UI_ITEM_A_PARTY_MENU1 ){
				now_pos = BOX2UI_ITEM_A_PARTY_MENU1;
			}
		}
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�������̂������肷��v�莝���|�P��������R�[���o�b�N�֐��F�J�[�\���ړ���
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxItemArrangePartyMoveCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );

	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�e�[�}�ύX����R�[���o�b�N�֐��F�J�[�\���\����
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxThemaChgCallBack_On( void * work, int now_pos, int old_pos )
{
/*
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	u8	x, y;

	syswk = work;

	if( now_pos == BOX2UI_BOXTHEMA_CHG_LEFT || now_pos == BOX2UI_BOXTHEMA_CHG_RIGHT ){
		now_pos = syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;
		syswk->app->old_cur_pos = now_pos;
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, now_pos );

	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );

	if( now_pos >= BOX2UI_BOXTHEMA_CHG_TRAY1 && now_pos <= BOX2UI_BOXTHEMA_CHG_TRAY6 ){
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
	}else{
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	}
*/
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );
	CursorObjMove( syswk, now_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�e�[�}�ύX����R�[���o�b�N�֐��F�J�[�\���ړ���
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxThemaChgCallBack_Move( void * work, int now_pos, int old_pos )
{
/*
	BOX2_SYS_WORK * syswk = work;

	if( now_pos >= BOX2UI_BOXTHEMA_CHG_TRAY1 && now_pos <= BOX2UI_BOXTHEMA_CHG_TRAY6 ){
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
	}else{
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	}

	//�u�{�b�N�X�����肩����v����g���C�ւ̈ړ�
	if( old_pos == BOX2UI_BOXTHEMA_CHG_MOVE && now_pos == BOX2UI_BOXTHEMA_CHG_TRAY1 ){
		now_pos = syswk->app->old_cur_pos;
		CURSORMOVE_PosSetEx( syswk->app->cmwk, now_pos, BOX2UI_BOXTHEMA_CHG_MOVE, BOX2UI_BOXTHEMA_CHG_MOVE );
	}

	// �O��ʒu�X�V
	if( now_pos >= BOX2UI_BOXTHEMA_CHG_TRAY1 && now_pos <= BOX2UI_BOXTHEMA_CHG_TRAY6 ){
		syswk->app->old_cur_pos = now_pos;
	}

	// �g���C������E�ւ̃��[�v
	if( now_pos == BOX2UI_BOXTHEMA_CHG_TRAY1 && old_pos == BOX2UI_BOXTHEMA_CHG_TRAY6 ){
		if( syswk->box_mv_pos + 1 >= syswk->trayMax ){
			syswk->box_mv_pos = 0;
		}else{
			syswk->box_mv_pos++;
		}
		BOX2OBJ_TrayIconChange( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2BMP_BoxMoveNameWrite( syswk, 0 );
//		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_RA, BOX2OBJ_ANM_R_ARROW_ON );

		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	// �g���C�E���獶�ւ̃��[�v
	if( now_pos == BOX2UI_BOXTHEMA_CHG_TRAY6 && old_pos == BOX2UI_BOXTHEMA_CHG_TRAY1 ){
		if( syswk->box_mv_pos - 1 < 0 ){
			syswk->box_mv_pos = syswk->trayMax - 1;
		}else{
			syswk->box_mv_pos--;
		}
		BOX2OBJ_TrayIconChange( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2BMP_BoxMoveNameWrite( syswk, 0 );
//		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_LA, BOX2OBJ_ANM_L_ARROW_ON );

		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	if( now_pos >= BOX2UI_BOXTHEMA_CHG_TRAY1 && now_pos <= BOX2UI_BOXTHEMA_CHG_TRAY6 ){
		if( old_pos != BOX2UI_BOXTHEMA_CHG_MOVE ){
			syswk->box_mv_pos = BOX2MAIN_MV_TRAY_DEF(syswk->box_mv_pos) + now_pos;
			BOX2OBJ_BoxMoveNameSet( syswk );
			BOX2BMP_BoxMoveNameWrite( syswk, 0 );
		}
	}

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
*/
	BOX2_SYS_WORK * syswk = work;

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�e�[�}�ύX����R�[���o�b�N�֐��F�J�[�\���ړ���
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxThemaChgCallBack_Touch( void * work, int now_pos, int old_pos )
{
/*
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	if( now_pos != BOX2UI_BOXTHEMA_CHG_LEFT && now_pos != BOX2UI_BOXTHEMA_CHG_RIGHT ){
		CursorObjMove( syswk, now_pos );
	}
*/
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );
	CursorObjMove( syswk, now_pos );
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��ύX����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
static void WallPaperChgCallBack_On( void * work, int now_pos, int old_pos )
{
/*
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	u8	x, y;

	syswk = work;

	if( now_pos == BOX2UI_WALLPAPER_CHG_LEFT || now_pos == BOX2UI_WALLPAPER_CHG_RIGHT ){
		now_pos = syswk->app->wallpaper_pos % BOX2OBJ_WPICON_MAX;
		syswk->app->old_cur_pos = now_pos;
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, now_pos );

	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );

	if( now_pos >= BOX2UI_WALLPAPER_CHG_ICON1 && now_pos <= BOX2UI_WALLPAPER_CHG_ICON4 ){
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
	}else{
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	}
*/
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );
	CursorObjMove( syswk, now_pos );
}

static void WallPaperChgCallBack_Move( void * work, int now_pos, int old_pos )
{
/*
	BOX2_SYS_WORK * syswk = work;

	if( now_pos >= BOX2UI_WALLPAPER_CHG_ICON1 && now_pos <= BOX2UI_WALLPAPER_CHG_ICON4 ){
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
	}else{
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	}

	//�u�͂肩����v����g���C�ւ̈ړ�
	if( old_pos == BOX2UI_WALLPAPER_CHG_ENTER && now_pos == BOX2UI_WALLPAPER_CHG_ICON1 ){
		now_pos = syswk->app->old_cur_pos;
		CURSORMOVE_PosSetEx( syswk->app->cmwk, now_pos, BOX2UI_WALLPAPER_CHG_ENTER, BOX2UI_WALLPAPER_CHG_ENTER );
	}

	// �O��ʒu�X�V
	if( now_pos >= BOX2UI_WALLPAPER_CHG_ICON1 && now_pos <= BOX2UI_WALLPAPER_CHG_ICON4 ){
		syswk->app->old_cur_pos = now_pos;
	}

	// �g���C������E�ւ̃��[�v
	if( now_pos == BOX2UI_WALLPAPER_CHG_ICON1 && old_pos == BOX2UI_WALLPAPER_CHG_ICON4 ){
		if( syswk->app->wallpaper_pos + 1 >= BOX_TOTAL_WALLPAPER_MAX ){
			syswk->app->wallpaper_pos = 0;
		}else{
			syswk->app->wallpaper_pos++;
		}
		BOX2OBJ_WallPaperObjChange( syswk );
		BOX2OBJ_WallPaperCursorSet( syswk );
		BOX2OBJ_WallPaperNameSet( syswk );
		BOX2BMP_WallPaperNameWrite( syswk );
//		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_RA, BOX2OBJ_ANM_R_ARROW_ON );

		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	// �g���C�E���獶�ւ̃��[�v
	if( now_pos == BOX2UI_WALLPAPER_CHG_ICON4 && old_pos == BOX2UI_WALLPAPER_CHG_ICON1 ){
		if( syswk->app->wallpaper_pos - 1 < 0 ){
			syswk->app->wallpaper_pos = BOX_TOTAL_WALLPAPER_MAX - 1;
		}else{
			syswk->app->wallpaper_pos--;
		}
		BOX2OBJ_WallPaperObjChange( syswk );
		BOX2OBJ_WallPaperCursorSet( syswk );
		BOX2OBJ_WallPaperNameSet( syswk );
		BOX2BMP_WallPaperNameWrite( syswk );
//		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_LA, BOX2OBJ_ANM_L_ARROW_ON );

		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	if( now_pos >= BOX2UI_WALLPAPER_CHG_ICON1 && now_pos <= BOX2UI_WALLPAPER_CHG_ICON4 ){
		if( old_pos != BOX2UI_WALLPAPER_CHG_ENTER ){
			syswk->app->wallpaper_pos = syswk->app->wallpaper_pos / BOX2OBJ_WPICON_MAX * BOX2OBJ_WPICON_MAX + now_pos;
			BOX2OBJ_WallPaperNameSet( syswk );
			BOX2BMP_WallPaperNameWrite( syswk );
		}
	}

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
*/
	BOX2_SYS_WORK * syswk = work;

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��ύX����R�[���o�b�N�֐��F�^�b�`��
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void WallPaperChgCallBack_Touch( void * work, int now_pos, int old_pos )
{
/*
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	if( now_pos != BOX2UI_WALLPAPER_CHG_LEFT && now_pos != BOX2UI_WALLPAPER_CHG_RIGHT ){
		CursorObjMove( syswk, now_pos );
	}
*/
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );
	CursorObjMove( syswk, now_pos );
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�W�����v�R�[���o�b�N�֐��F�J�[�\���\����
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxJumpCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;

	syswk = work;

//	CursorObjMove( syswk, now_pos );

	if( now_pos == BOX2UI_BOXJUMP_TRAY1 ||		// 00: �g���C�A�C�R��
			now_pos == BOX2UI_BOXJUMP_TRAY6 ||		// 05: �g���C�A�C�R��
			now_pos == BOX2UI_BOXJUMP_RET ){			// 06: �߂�
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->app->old_cur_pos );
		now_pos = syswk->app->old_cur_pos;
	}else{
		CursorObjMove( syswk, now_pos );
	}

	syswk->app->old_cur_pos = now_pos;
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�W�����v�R�[���o�b�N�֐��F�J�[�\���ړ���
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxJumpCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	if( now_pos >= BOX2UI_BOXJUMP_TRAY2 && now_pos <= BOX2UI_BOXJUMP_TRAY5 ){
		BOX2OBJ_ChangeTrayName( syswk, now_pos-BOX2UI_BOXJUMP_TRAY2, TRUE );
	}else{
		BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
	}

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );

	syswk->app->old_cur_pos = now_pos;
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�W�����v�R�[���o�b�N�֐��F�^�b�`��
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxJumpCallBack_Touch( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	if( now_pos == BOX2UI_BOXJUMP_TRAY1 ||		// 00: �g���C�A�C�R��
			now_pos == BOX2UI_BOXJUMP_TRAY6 ||		// 05: �g���C�A�C�R��
			now_pos == BOX2UI_BOXJUMP_RET ){			// 06: �߂�
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->app->old_cur_pos );
		now_pos = syswk->app->old_cur_pos;
	}else{
		CursorObjMove( syswk, now_pos );
	}

	syswk->app->old_cur_pos = now_pos;
}

//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �u�o�g���{�b�N�X�v���C������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_BattleBoxMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	u8	now;

	now = CURSORMOVE_PosGet( syswk->app->cmwk );
	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	// ���j���[��\��
	if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
		if( ret >= BOX2UI_BATTLEBOX_MAIN_GET && ret <= BOX2UI_BATTLEBOX_MAIN_CLOSE ){
			CURSORMOVE_PosSet( syswk->app->cmwk, now );
			CursorObjMove( syswk, now );
			return CURSORMOVE_NONE;
		}
	// ���j���[�\����
	}else{
		if( ret == BOX2UI_BATTLEBOX_MAIN_PARTY ||
				ret == BOX2UI_BATTLEBOX_MAIN_RETURN1 ||
				ret == BOX2UI_BATTLEBOX_MAIN_RETURN2 ){
			CURSORMOVE_PosSet( syswk->app->cmwk, now );
			CursorObjMove( syswk, now );
			return CURSORMOVE_NONE;
		}
		if( ret == CURSORMOVE_CANCEL ){
			return BOX2UI_BATTLEBOX_MAIN_CLOSE;
		}
	}

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * �u�o�g���{�b�N�X�v���C������R�[���o�b�N�֐��F�J�[�\���\����
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BattleBoxMainCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	
	syswk = work;

	if( old_pos != CURSORMOVE_ONOFF_DIRECT ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			switch( now_pos ){
			case BOX2UI_BATTLEBOX_MAIN_LEFT:
			case BOX2UI_BATTLEBOX_MAIN_RIGHT:
				now_pos = BOX2UI_BATTLEBOX_MAIN_NAME;
				break;

			case BOX2UI_BATTLEBOX_MAIN_NAME:
			case BOX2UI_BATTLEBOX_MAIN_PARTY:
			case BOX2UI_BATTLEBOX_MAIN_RETURN1:
			case BOX2UI_BATTLEBOX_MAIN_RETURN2:
				break;

			default:
				if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
					now_pos = syswk->get_pos;
				}else{
					now_pos = BOX2UI_BATTLEBOX_MAIN_POKE1;
				}
			}
		}else{
			if( now_pos != BOX2UI_BATTLEBOX_MAIN_GET ||
					now_pos != BOX2UI_BATTLEBOX_MAIN_STATUS ||
					now_pos != BOX2UI_BATTLEBOX_MAIN_ITEM ||
					now_pos != BOX2UI_BATTLEBOX_MAIN_CLOSE ){

				now_pos = BOX2UI_BATTLEBOX_MAIN_GET;
			}
		}
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�o�g���{�b�N�X�v���C������R�[���o�b�N�֐��F�J�[�\���ړ���
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BattleBoxMainCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );

	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�o�g���{�b�N�X�v���C������R�[���o�b�N�֐��F�^�b�`��
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BattleBoxMainCallBack_Touch( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �u�o�g���{�b�N�X�v�p�[�e�B�|�P��������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_BattleBoxPartyMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	u8	now;

	now = CURSORMOVE_PosGet( syswk->app->cmwk );
	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == FALSE ){
		if( ret >= BOX2UI_BATTLEBOX_PARTY_GET && ret <= BOX2UI_BATTLEBOX_PARTY_CLOSE ){
			CURSORMOVE_PosSet( syswk->app->cmwk, now );
			CursorObjMove( syswk, now );
			return CURSORMOVE_NONE;
		}
	}else{
		if( ret == BOX2UI_BATTLEBOX_PARTY_BOXLIST ||
				ret == BOX2UI_BATTLEBOX_PARTY_RETURN1 ||
				ret == BOX2UI_BATTLEBOX_PARTY_RETURN2 ){
			CURSORMOVE_PosSet( syswk->app->cmwk, now );
			CursorObjMove( syswk, now );
			return CURSORMOVE_NONE;
		}

		if( ret == CURSORMOVE_CANCEL ){
			return BOX2UI_BATTLEBOX_PARTY_CLOSE;
		}
	}

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * �u�o�g���{�b�N�X�v�p�[�e�B�|�P��������R�[���o�b�N�֐��F�J�[�\���\����
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BattleBoxPartyMainCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	if( old_pos != CURSORMOVE_ONOFF_DIRECT ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			if( now_pos >= BOX2UI_BATTLEBOX_PARTY_BOXLIST ){
				if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
					now_pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
				}else{
					now_pos = BOX2UI_BATTLEBOX_PARTY_POKE1;
				}
			}
		}else{
			if( now_pos != BOX2UI_BATTLEBOX_PARTY_GET ||
				now_pos != BOX2UI_BATTLEBOX_PARTY_STATUS ||
				now_pos != BOX2UI_BATTLEBOX_PARTY_ITEM ||
				now_pos != BOX2UI_BATTLEBOX_PARTY_CLOSE ){

				now_pos = BOX2UI_BATTLEBOX_PARTY_GET;
			}
		}
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�o�g���{�b�N�X�v�p�[�e�B�|�P��������R�[���o�b�N�֐��F�J�[�\���ړ���
 *
 * @param	work		�{�b�N�X��ʃV�X�e�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BattleBoxPartyMainCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );

	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �u�������̂������肷��v�������������ɃA�C�R����͂�ł���Ƃ��̑���
 *
 * @param	syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 *
 *	���{�b�N�X�����E�|�P�����ړ��̃��[�N���g�p
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_BoxItemArrangeMoveHand( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_LEFT );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_RIGHT );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_CHANGE );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY1 );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY2 );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY3 );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY4 );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY5 );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY6 );

	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	CURSORMOVE_MoveTableInit( syswk->app->cmwk );

	return ret;
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ���VBLANK�ōs�����߂̃��[�N�ݒ�
 *
 * @param	appwk		�{�b�N�X��ʃA�v�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2UI_CursorMoveVFuncWorkSet( BOX2_APP_WORK * appwk, int now_pos, int old_pos )
{
	const CURSORMOVE_DATA * pw;
	BOX2MAIN_CURSORMOVE_WORK * cwk;
	u8	nx, ny, ox, oy;

	pw = CURSORMOVE_GetMoveData( appwk->cmwk, now_pos );
	nx = pw->px;
	ny = pw->py;
	pw = CURSORMOVE_GetMoveData( appwk->cmwk, old_pos );
	ox = pw->px;
	oy = pw->py;

	cwk = GFL_HEAP_AllocMemory( HEAPID_BOX_APP, sizeof(BOX2MAIN_CURSORMOVE_WORK) );
//��[GS_CONVERT_TAG]
	cwk->cnt = 4;
	cwk->px  = nx;
	cwk->py  = ny;

	if( nx >= ox ){
		cwk->vx = nx - ox;
		cwk->mx = 0;
	}else{
		cwk->vx = ox - nx;
		cwk->mx = 1;
	}
	if( ny >= oy ){
		cwk->vy = ny - oy;
		cwk->my = 0;
	}else{
		cwk->vy = oy - ny;
		cwk->my = 1;
	}
	cwk->vx = ( ( cwk->vx << 8 ) / cwk->cnt ) >> 8;
	cwk->vy = ( ( cwk->vy << 8 ) / cwk->cnt ) >> 8;

	appwk->vfunk.work = cwk;
}

//--------------------------------------------------------------------------------------------
/**
 * �x�X�e�[�^�X�{�^���`�F�b�N
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"TRUE = �N����"
 * @retval	"FALSE = �N���s��"
 */
//--------------------------------------------------------------------------------------------
/*
BOOL BOX2UI_YStatusButtonCheck( BOX2_SYS_WORK * syswk )
{
	if( BOX2BGWFRM_YStatusButtonCheck(syswk->app->wfrm) == FALSE ){
		return FALSE;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ){
		return TRUE;
	}

	if( GFL_UI_TP_HitTrg( YStatusHitTbl ) != GFL_UI_TP_HIT_NONE ){
		return TRUE;
	}

	return FALSE;
}
*/


static u32 HitCheckTrg( const CURSORMOVE_DATA * dat, u32 max )
{
	GFL_UI_TP_HITTBL	rect[2];
	u32	i;

	rect[1] = TpCheckEnd;

	for( i=0; i<max; i++ ){
		rect[0] = dat[i].rect;
		if( GFL_UI_TP_HitTrg( rect ) != GFL_UI_TP_HIT_NONE ){
			return i;
		}
	}

	return GFL_UI_TP_HIT_NONE;
}

static u32 HitCheckCont( const CURSORMOVE_DATA * dat, u32 max )
{
	GFL_UI_TP_HITTBL	rect[2];
	u32	i;

	rect[1] = TpCheckEnd;

	for( i=0; i<max; i++ ){
		rect[0] = dat[i].rect;
		if( GFL_UI_TP_HitCont( rect ) != GFL_UI_TP_HIT_NONE ){
			return i;
		}
	}

	return GFL_UI_TP_HIT_NONE;
}

static u32 HitCheckPos( const CURSORMOVE_DATA * dat, u32 max, u32 x, u32 y )
{
	GFL_UI_TP_HITTBL	rect[2];
	u32	i;

	rect[1] = TpCheckEnd;

	for( i=0; i<max; i++ ){
		rect[0] = dat[i].rect;
		if( GFL_UI_TP_HitSelf( rect, x, y ) != GFL_UI_TP_HIT_NONE ){
			return i;
		}
	}

	return GFL_UI_TP_HIT_NONE;
}


//--------------------------------------------------------------------------------------------
/**
 * �^�b�`�`�F�b�N�F�g���C�̃|�P����
 *
 * @param	none
 *
 * @return	GFL_UI_TP_HIT_NONE = �G���Ă��Ȃ�
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_HitTrayPokeTrg(void)
{
	return HitCheckTrg( BoxArrangeMainCursorMoveData, BOX2OBJ_POKEICON_TRAY_MAX );
}

u32 BOX2UI_HitCheckTrayPoke( u32 x, u32 y )
{
	return HitCheckPos( BoxArrangeMainCursorMoveData, BOX2OBJ_POKEICON_TRAY_MAX, x, y );
}


//--------------------------------------------------------------------------------------------
/**
 * �^�b�`�`�F�b�N�F�莝���|�P�����i���j
 *
 * @param	none
 *
 * @return	GFL_UI_TP_HIT_NONE = �G���Ă��Ȃ�
//��[GS_CONVERT_TAG]
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_HitCheckTrgPartyPokeLeft(void)
{
	return HitCheckTrg( BoxArrangePartyPokeCursorMoveData, BOX2OBJ_POKEICON_MINE_MAX );
}

//--------------------------------------------------------------------------------------------
/**
 * �^�b�`�`�F�b�N�F�莝���|�P�����i�E�j
 *
 * @param	none
 *
 * @return	GFL_UI_TP_HIT_NONE = �G���Ă��Ȃ�
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_HitCheckTrgPartyPokeRight(void)
{
	return HitCheckTrg(
					&BoxArrangePartyPokeMoveCursorMoveData[BOX2UI_ARRANGE_PTGT_PARTY_POKE], BOX2OBJ_POKEICON_MINE_MAX );
}



u32 BOX2UI_HitCheckContTrayArrow(void)
{
	return HitCheckCont( &BoxArrangeMainCursorMoveData[BOX2UI_ARRANGE_MAIN_LEFT], 2 );
}

u32 BOX2UI_HitCheckContTrayIconScroll(void)
{
	u32	ret = HitCheckCont( &BoxArrangePokeMoveCursorMoveData[BOX2UI_ARRANGE_PGT_TRAY1], 6 );

	if( ret != GFL_UI_TP_HIT_NONE ){
		ret += BOX2UI_ARRANGE_PGT_TRAY1;
		if( ret == BOX2UI_ARRANGE_PGT_TRAY1 ){
			return 0;
		}
		if( ret == BOX2UI_ARRANGE_PGT_TRAY6 ){
			return 1;
		}
	}
	return GFL_UI_TP_HIT_NONE;
}


static const GFL_UI_TP_HITTBL TrayScrollStartHitTbl[] =
{
	{ 0, 167, 228, 255 },
	{ 0, 167, 168, 195 },
	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const GFL_UI_TP_HITTBL TrayScrollHitTbl[] =
{
	{ 0, 191, 168, 255 },
	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};


BOOL BOX2UI_HitCheckTrayScrollStart( u32 * x, u32 * y )
{
	if( GFL_UI_TP_HitCont( TrayScrollStartHitTbl ) != GFL_UI_TP_HIT_NONE ){
		GFL_UI_TP_GetPointCont( x, y );
		return TRUE;
	}
	return FALSE;
}

BOOL BOX2UI_HitCheckTrayScroll( u32 * x, u32 * y )
{
	if( GFL_UI_TP_HitCont( TrayScrollHitTbl ) != GFL_UI_TP_HIT_NONE ){
		GFL_UI_TP_GetPointCont( x, y );
		return TRUE;
	}
	return FALSE;
}

u32 BOX2UI_HitCheckTrayIcon( u32 x, u32 y )
{
	u32	ret = HitCheckPos( &BoxArrangePokeMoveCursorMoveData[BOX2UI_ARRANGE_PGT_TRAY2], 4, x, y );

	if( ret != GFL_UI_TP_HIT_NONE ){
		return ( BOX2OBJ_POKEICON_PUT_MAX + ret );
	}
	return BOX2MAIN_GETPOS_NONE;
}


#define	PARTY_FRM_UPY		( BOX2BGWFRM_PARTYPOKE_PY*8 )
#define	PARTY_FRM_DPY		( PARTY_FRM_UPY+BOX2BGWFRM_PARTYPOKE_SY*8-1 )
#define	PARTY_FRM_LLX		( BOX2BGWFRM_PARTYPOKE_LX*8 )
#define	PARTY_FRM_LRX		( PARTY_FRM_LLX+BOX2BGWFRM_PARTYPOKE_SX*8-1 )

static const GFL_UI_TP_HITTBL PartyLeftHitTbl[] =
{
	{ PARTY_FRM_UPY, PARTY_FRM_DPY, PARTY_FRM_LLX, PARTY_FRM_LRX },
	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};

BOOL BOX2UI_HitCheckPartyFrameLeft(void)
{
	if( GFL_UI_TP_HitCont( PartyLeftHitTbl ) != GFL_UI_TP_HIT_NONE ){
		return TRUE;
	}
	return FALSE;
}

















#if 0
//============================================================================================
/**
 * @file	box2_tp.c
 * @brief	�V�{�b�N�X��� �^�b�`�p�l���֘A
 * @author	Hiroyuki Nakamura
 * @date	08.05.30
 */
//============================================================================================
#include <gflib.h>
//��[GS_CONVERT_TAG]

#include "gflib/touchpanel.h"

#include "box2_main.h"
#include "box2_obj.h"
#include "box2_tp.h"


//============================================================================================
//	�萔��`
//============================================================================================
#define	TRAYPOKE_SX		( BOX2OBJ_TRAYPOKE_SX )						// �g���C�̃|�P�����̃^�b�`�͈̓T�C�Y�w
#define	TRAYPOKE_SY		( 24 )										// �g���C�̃|�P�����̃^�b�`�͈̓T�C�Y�x
#define	TRAYPOKE_PX		( BOX2OBJ_TRAYPOKE_PX - TRAYPOKE_SX / 2 )	// �g���C�̃|�P�����̃^�b�`�J�n���W�w
#define	TRAYPOKE_PY		( 40 )										// �g���C�̃|�P�����̃^�b�`�J�n���W�x


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================


//============================================================================================
//	�O���[�o���ϐ�
//============================================================================================

// �^�b�`���W�e�[�u���F�g���C�̃|�P����
static const GFL_UI_TP_HITTBL TrayPokeHitTbl[] =
//��[GS_CONVERT_TAG]
{
	{ TRAYPOKE_PY+TRAYPOKE_SX*0, TRAYPOKE_PY+TRAYPOKE_SY*1-1, TRAYPOKE_PX+TRAYPOKE_SX*0, TRAYPOKE_PX+TRAYPOKE_SX*1-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*0, TRAYPOKE_PY+TRAYPOKE_SY*1-1, TRAYPOKE_PX+TRAYPOKE_SX*1, TRAYPOKE_PX+TRAYPOKE_SX*2-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*0, TRAYPOKE_PY+TRAYPOKE_SY*1-1, TRAYPOKE_PX+TRAYPOKE_SX*2, TRAYPOKE_PX+TRAYPOKE_SX*3-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*0, TRAYPOKE_PY+TRAYPOKE_SY*1-1, TRAYPOKE_PX+TRAYPOKE_SX*3, TRAYPOKE_PX+TRAYPOKE_SX*4-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*0, TRAYPOKE_PY+TRAYPOKE_SY*1-1, TRAYPOKE_PX+TRAYPOKE_SX*4, TRAYPOKE_PX+TRAYPOKE_SX*5-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*0, TRAYPOKE_PY+TRAYPOKE_SY*1-1, TRAYPOKE_PX+TRAYPOKE_SX*5, TRAYPOKE_PX+TRAYPOKE_SX*6-1 },

	{ TRAYPOKE_PY+TRAYPOKE_SX*1, TRAYPOKE_PY+TRAYPOKE_SY*2-1, TRAYPOKE_PX+TRAYPOKE_SX*0, TRAYPOKE_PX+TRAYPOKE_SX*1-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*1, TRAYPOKE_PY+TRAYPOKE_SY*2-1, TRAYPOKE_PX+TRAYPOKE_SX*1, TRAYPOKE_PX+TRAYPOKE_SX*2-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*1, TRAYPOKE_PY+TRAYPOKE_SY*2-1, TRAYPOKE_PX+TRAYPOKE_SX*2, TRAYPOKE_PX+TRAYPOKE_SX*3-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*1, TRAYPOKE_PY+TRAYPOKE_SY*2-1, TRAYPOKE_PX+TRAYPOKE_SX*3, TRAYPOKE_PX+TRAYPOKE_SX*4-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*1, TRAYPOKE_PY+TRAYPOKE_SY*2-1, TRAYPOKE_PX+TRAYPOKE_SX*4, TRAYPOKE_PX+TRAYPOKE_SX*5-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*1, TRAYPOKE_PY+TRAYPOKE_SY*2-1, TRAYPOKE_PX+TRAYPOKE_SX*5, TRAYPOKE_PX+TRAYPOKE_SX*6-1 },

	{ TRAYPOKE_PY+TRAYPOKE_SX*2, TRAYPOKE_PY+TRAYPOKE_SY*3-1, TRAYPOKE_PX+TRAYPOKE_SX*0, TRAYPOKE_PX+TRAYPOKE_SX*1-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*2, TRAYPOKE_PY+TRAYPOKE_SY*3-1, TRAYPOKE_PX+TRAYPOKE_SX*1, TRAYPOKE_PX+TRAYPOKE_SX*2-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*2, TRAYPOKE_PY+TRAYPOKE_SY*3-1, TRAYPOKE_PX+TRAYPOKE_SX*2, TRAYPOKE_PX+TRAYPOKE_SX*3-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*2, TRAYPOKE_PY+TRAYPOKE_SY*3-1, TRAYPOKE_PX+TRAYPOKE_SX*3, TRAYPOKE_PX+TRAYPOKE_SX*4-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*2, TRAYPOKE_PY+TRAYPOKE_SY*3-1, TRAYPOKE_PX+TRAYPOKE_SX*4, TRAYPOKE_PX+TRAYPOKE_SX*5-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*2, TRAYPOKE_PY+TRAYPOKE_SY*3-1, TRAYPOKE_PX+TRAYPOKE_SX*5, TRAYPOKE_PX+TRAYPOKE_SX*6-1 },

	{ TRAYPOKE_PY+TRAYPOKE_SX*3, TRAYPOKE_PY+TRAYPOKE_SY*4-1, TRAYPOKE_PX+TRAYPOKE_SX*0, TRAYPOKE_PX+TRAYPOKE_SX*1-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*3, TRAYPOKE_PY+TRAYPOKE_SY*4-1, TRAYPOKE_PX+TRAYPOKE_SX*1, TRAYPOKE_PX+TRAYPOKE_SX*2-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*3, TRAYPOKE_PY+TRAYPOKE_SY*4-1, TRAYPOKE_PX+TRAYPOKE_SX*2, TRAYPOKE_PX+TRAYPOKE_SX*3-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*3, TRAYPOKE_PY+TRAYPOKE_SY*4-1, TRAYPOKE_PX+TRAYPOKE_SX*3, TRAYPOKE_PX+TRAYPOKE_SX*4-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*3, TRAYPOKE_PY+TRAYPOKE_SY*4-1, TRAYPOKE_PX+TRAYPOKE_SX*4, TRAYPOKE_PX+TRAYPOKE_SX*5-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*3, TRAYPOKE_PY+TRAYPOKE_SY*4-1, TRAYPOKE_PX+TRAYPOKE_SX*5, TRAYPOKE_PX+TRAYPOKE_SX*6-1 },

	{ TRAYPOKE_PY+TRAYPOKE_SX*4, TRAYPOKE_PY+TRAYPOKE_SY*5-1, TRAYPOKE_PX+TRAYPOKE_SX*0, TRAYPOKE_PX+TRAYPOKE_SX*1-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*4, TRAYPOKE_PY+TRAYPOKE_SY*5-1, TRAYPOKE_PX+TRAYPOKE_SX*1, TRAYPOKE_PX+TRAYPOKE_SX*2-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*4, TRAYPOKE_PY+TRAYPOKE_SY*5-1, TRAYPOKE_PX+TRAYPOKE_SX*2, TRAYPOKE_PX+TRAYPOKE_SX*3-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*4, TRAYPOKE_PY+TRAYPOKE_SY*5-1, TRAYPOKE_PX+TRAYPOKE_SX*3, TRAYPOKE_PX+TRAYPOKE_SX*4-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*4, TRAYPOKE_PY+TRAYPOKE_SY*5-1, TRAYPOKE_PX+TRAYPOKE_SX*4, TRAYPOKE_PX+TRAYPOKE_SX*5-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*4, TRAYPOKE_PY+TRAYPOKE_SY*5-1, TRAYPOKE_PX+TRAYPOKE_SX*5, TRAYPOKE_PX+TRAYPOKE_SX*6-1 },

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
//��[GS_CONVERT_TAG]
};

// �^�b�`���W�e�[�u���F�莝���|�P�����i���j
static const GFL_UI_TP_HITTBL PartyPokeLeftHitTbl[] =
//��[GS_CONVERT_TAG]
{
	{  68-12,  68+12-1, 42-12, 42+12-1 }, {  76-12,  76+12-1, 78-12, 78+12-1 },
	{ 100-12, 100+12-1, 42-12, 42+12-1 }, { 108-12, 108+12-1, 78-12, 78+12-1 },
	{ 132-12, 132+12-1, 42-12, 42+12-1 }, { 140-12, 140+12-1, 78-12, 78+12-1 },

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
//��[GS_CONVERT_TAG]
};

// �^�b�`���W�e�[�u���F�莝���|�P�����i�E�j
static const GFL_UI_TP_HITTBL PartyPokeRightHitTbl[] =
//��[GS_CONVERT_TAG]
{
	{  68-12,  68+12-1, 194-12, 194+12-1 }, {  76-12,  76+12-1, 230-12, 230+12-1 },
	{ 100-12, 100+12-1, 194-12, 194+12-1 }, { 108-12, 108+12-1, 230-12, 230+12-1 },
	{ 132-12, 132+12-1, 194-12, 194+12-1 }, { 140-12, 140+12-1, 230-12, 230+12-1 },

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
//��[GS_CONVERT_TAG]
};


#endif
