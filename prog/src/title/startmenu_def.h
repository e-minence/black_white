//============================================================================================
/**
 * @file		startmenu_def.h
 * @brief		�^�C�g�����j���[��`
 * @author	Hiroyuki Nakamura
 * @date		10.02.25
 */
//============================================================================================
#pragma	once

// [ HEAPID_STARTMENU ] ����m�ۗp��`
#define	HEAPID_STARTMENU_L		( GFL_HEAP_LOWID(HEAPID_STARTMENU) )

#define	EXP_BUF_SIZE		( 1024 )		// �ėp������o�b�t�@�T�C�Y

// ���ڃC���f�b�N�X
enum {
	LIST_ITEM_CONTINUE = 0,		// ��������
	LIST_ITEM_NEW_GAME,				// �ŏ�����
	LIST_ITEM_HUSHIGI,				// �s�v�c�ȑ��蕨
	LIST_ITEM_BATTLE,					// �o�g�����
	LIST_ITEM_GAME_SYNC,			// �Q�[���V���N�ݒ�
	LIST_ITEM_WIFI_SET,				// Wi-Fi�ݒ�
	LIST_ITEM_MIC_TEST,				// �}�C�N�e�X�g
	LIST_ITEM_MACHINE,				// �]���}�V�����g��
	LIST_ITEM_MAX
};

// BMPWIN
enum {
	BMPWIN_TITLE = 0,
	BMPWIN_NAME,
	BMPWIN_PLACE,
	BMPWIN_TIME,
	BMPWIN_ZUKAN,
	BMPWIN_BADGE,

	BMPWIN_START,
	BMPWIN_HUSHIGI,
	BMPWIN_BATTLE,
	BMPWIN_GAMESYNC,
	BMPWIN_WIFI,
	BMPWIN_MIC,
	BMPWIN_MACHINE,

	BMPWIN_MSG,

	BMPWIN_MAX
};

// BMPWIN��`
#define	BMPWIN_LIST_PAL			( FONT_PALETTE_M )
#define	BMPWIN_LIST_FRM			( GFL_BG_FRAME1_M )
//�u��������n�߂�v
#define	BMPWIN_TITLE_PX			( 2 )
#define	BMPWIN_TITLE_PY			( 0 )
#define	BMPWIN_TITLE_SX			( 18 )
#define	BMPWIN_TITLE_SY			( 3 )
// �v���C���[��
#define	BMPWIN_NAME_PX			( 7 )
#define	BMPWIN_NAME_PY			( 4 )
#define	BMPWIN_NAME_SX			( 8 )
#define	BMPWIN_NAME_SY			( 2 )
// �Z�[�u�ꏊ
#define	BMPWIN_PLACE_PX			( 7 )
#define	BMPWIN_PLACE_PY			( 6 )
#define	BMPWIN_PLACE_SX			( 15 )
#define	BMPWIN_PLACE_SY			( 2 )
// �v���C����
#define	BMPWIN_TIME_PX			( 2 )
#define	BMPWIN_TIME_PY			( 11 )
#define	BMPWIN_TIME_SX			( 18 )
#define	BMPWIN_TIME_SY			( 2 )
// �}��
#define	BMPWIN_ZUKAN_PX			( 12 )
#define	BMPWIN_ZUKAN_PY			( 9 )
#define	BMPWIN_ZUKAN_SX			( 13 )
#define	BMPWIN_ZUKAN_SY			( 2 )
// �o�b�W
#define	BMPWIN_BADGE_PX			( 2 )
#define	BMPWIN_BADGE_PY			( 9 )
#define	BMPWIN_BADGE_SX			( 10 )
#define	BMPWIN_BADGE_SY			( 2 )
//�u�ŏ�����n�߂�v
#define	BMPWIN_START_PX			( 2 )
#define	BMPWIN_START_PY			( 0 )
#define	BMPWIN_START_SX			( 19 )
#define	BMPWIN_START_SY			( 3 )
//�u�s�v�c�ȑ��蕨�v
#define	BMPWIN_HUSHIGI_PX		( 2 )
#define	BMPWIN_HUSHIGI_PY		( 0 )
#define	BMPWIN_HUSHIGI_SX		( 19 )
#define	BMPWIN_HUSHIGI_SY		( 3 )
//�u�o�g�����v
#define	BMPWIN_BATTLE_PX		( 2 )
#define	BMPWIN_BATTLE_PY		( 0 )
#define	BMPWIN_BATTLE_SX		( 19 )
#define	BMPWIN_BATTLE_SY		( 3 )
//�u�Q�[���V���N�ݒ�v
#define	BMPWIN_GAMESYNC_PX	( 2 )
#define	BMPWIN_GAMESYNC_PY	( 0 )
#define	BMPWIN_GAMESYNC_SX	( 19 )
#define	BMPWIN_GAMESYNC_SY	( 3 )
//�uWi-Fi�ݒ�v
#define	BMPWIN_WIFI_PX			( 2 )
#define	BMPWIN_WIFI_PY			( 0 )
#define	BMPWIN_WIFI_SX			( 19 )
#define	BMPWIN_WIFI_SY			( 3 )
//�u�}�C�N�e�X�g�v
#define	BMPWIN_MIC_PX				( 2 )
#define	BMPWIN_MIC_PY				( 0 )
#define	BMPWIN_MIC_SX				( 19 )
#define	BMPWIN_MIC_SY				( 3 )
//�u�]���}�V�����g���v
#define	BMPWIN_MACHINE_PX		( 2 )
#define	BMPWIN_MACHINE_PY		( 0 )
#define	BMPWIN_MACHINE_SX		( 19 )
#define	BMPWIN_MACHINE_SY		( 3 )
// ���b�Z�[�W
#define	BMPWIN_MSG_FRM			( GFL_BG_FRAME0_M )
#define	BMPWIN_MSG_PX				( 1 )
#define	BMPWIN_MSG_PY				( 19 )
#define	BMPWIN_MSG_SX				( 30 )
#define	BMPWIN_MSG_SY				( 4 )
#define	BMPWIN_MSG_PAL			( FONT_PALETTE_M )
// �͂��E������
#define	BMPWIN_YESNO_FRM		( GFL_BG_FRAME0_M )
#define	BMPWIN_YESNO_PX			( 24 )
#define	BMPWIN_YESNO_PY			( 13 )
#define	BMPWIN_YESNO_PAL		( FONT_PALETTE_M )

// �ŏ�����n�߂�̌x���E�B���h�E
#define	BMPWIN_NEWGAME_WIN_FRM	( GFL_BG_FRAME0_M )
#define	BMPWIN_NEWGAME_WIN_PX		( 2 )
#define	BMPWIN_NEWGAME_WIN_PY		( 2 )
#define	BMPWIN_NEWGAME_WIN_SX		( 30 )
#define	BMPWIN_NEWGAME_WIN_SY		( 20 )
#define	BMPWIN_NEWGAME_WIN_PAL	( FONT_PALETTE_M )

// �ŏ�����n�߂�̌x�����\�����W
#define	NEW_GAME_WARRNING_ATTENTION_PY	( 0 )
#define	NEW_GAME_WARRNING_MESSAGE_PY		( 3*8 )
#define	NEW_GAME_WARRNING_A_BUTTON_PY		( 16*8 )
#define	NEW_GAME_WARRNING_B_BUTTON_PY		( 18*8 )
#define	NEW_GAME_WARRNING_WIN_PX				( 32 )
#define	NEW_GAME_WARRNING_WIN_PY				( 1 )
#define	NEW_GAME_WARRNING_WIN_SX				( 32 )
#define	NEW_GAME_WARRNING_WIN_SY				( 22 )

#define	WARRNING_WIN_CHAR_NUM		( 1 )		// �E�B���h�E�L�����ԍ�
#define	WARRNING_WIN_PLTT_NUM		( 1 )		// �E�B���h�E�p���b�g�ԍ�

// ��������n�߂�̐����E�B���h�E
#define	BMPWIN_CONTINUE_WIN_FRM		( GFL_BG_FRAME0_S )
#define	BMPWIN_CONTINUE_WIN_PX		( 2 )
#define	BMPWIN_CONTINUE_WIN_PY		( 3 )
#define	BMPWIN_CONTINUE_WIN_SX		( 30 )
#define	BMPWIN_CONTINUE_WIN_SY		( 19 )
#define	BMPWIN_CONTINUE_WIN_PAL		( FONT_PALETTE_S )

// ��������n�߂�̐������\�����W
#define	CONTINUE_INFO_ATTENTION_PY	( 0 )
#define	CONTINUE_INFO_MESSAGE_PY		( 3*8 )
#define	CONTINUE_INFO_WIN_PX				( 0 )
#define	CONTINUE_INFO_WIN_PY				( 1 )
#define	CONTINUE_INFO_WIN_SX				( 32 )
#define	CONTINUE_INFO_WIN_SY				( 22 )


// ���b�Z�[�W�E�B���h�E
#define	MESSAGE_WIN_CHAR_NUM			( 1 )
#define	MESSAGE_WIN_PLTT_NUM			( 14 )

// ���X�g�t���[���f�[�^
#define	LIST_FRAME_SX						( 26 )
#define	LIST_FRAME_CONTINUE_SY	( 14 )
#define	LIST_FRAME_NEW_GAME_SY	( 3 )
#define	LIST_FRAME_HUSHIGI_SY		( 3 )
#define	LIST_FRAME_BATTLE_SY		( 3 )
#define	LIST_FRAME_GAME_SYNC_SY	( 3 )
#define	LIST_FRAME_WIFI_SET_SY	( 3 )
#define	LIST_FRAME_MIC_TEST_SY	( 3 )
#define	LIST_FRAME_MACHINE_SY		( 3 )

// ���X�g��\������G���A�T�C�Y
//define	LIST_SCRN_SX			( 32 )
#define	LIST_SCRN_SY			( 64 )

// ���X�g���ڕ\�����W
#define	LIST_ITEM_PX			( 3 )
#define	LIST_ITEM_PY			( 2 )
//#define	LIST_ITEM_SCROLL_DOWN_PY	( 22 )
//#define	LIST_ITEM_SCROLL_UP_PY		( 17 )

// �a�f�p���b�g
#define	LIST_PALETTE			( 1 )
#define	CURSOR_PALETTE		( 2 )
#define	CURSOR_PALETTE2		( 3 )
#define	FONT_PALETTE_M		( 15 )
#define	FONT_PALETTE_S		( 15 )

// �����F
#define	FCOL_MP15WN		( PRINTSYS_LSB_Make(15,2,0) )
#define	FCOL_MP15BLN	( PRINTSYS_LSB_Make(6,5,0) )
#define	FCOL_MP15RN		( PRINTSYS_LSB_Make(4,3,0) )


// OBJ INDEX
enum {
	OBJ_ID_PLAYER = 0,
	OBJ_ID_NEW_HUSHIGI,
	OBJ_ID_NEW_BATTLE,
	OBJ_ID_NEW_GAMESYNC,
	OBJ_ID_NEW_MACHINE,
	OBJ_ID_MAX
};

// OBJ���\�[�X
enum {
	// �L����
	CHRRES_PLAYER = 0,
	CHRRES_OBJ_U,
	CHRRES_MAX,

	// �p���b�g
	PALRES_PLAYER = 0,
	PALRES_OBJ_U,
	PALRES_MAX,

	// �Z��
	CELRES_PLAYER = 0,
	CELRES_OBJ_U,
	CELRES_MAX,
};

// �p���b�g
#define	PALNUM_PLAYER		( 0 )
#define	PALSIZ_PLAYER		( 1 )
#define	PALNUM_OBJ_U		( PALNUM_PLAYER+PALSIZ_PLAYER )
#define	PALSIZ_OBJ_U		( 1 )

// ���W
#define	PLAYER_OBJ_PX		( 48 )
#define	PLAYER_OBJ_PY		( 64 )
#define	NEW_OBJ_PX			( 208 )
