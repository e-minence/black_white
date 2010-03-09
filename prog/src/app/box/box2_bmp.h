//============================================================================================
/**
 * @file		box2_bmp.h
 * @brief		�{�b�N�X��� BMP�֘A
 * @author	Hiroyuki Nakamura
 * @date		08.05.13
 */
//============================================================================================
#pragma	once


//============================================================================================
//	�萔��`
//============================================================================================
/*
// ���j���[�E�B���h�E�S���̃T�C�Y
#define	BOX2BMP_POKEMENU_SX		( 11 )
#define	BOX2BMP_POKEMENU_SY		( 3 )

//�u�Ă����|�P�����v�T�C�Y
#define	BOX2BMP_BTN_TEMOCHI_SX	( 11 )
#define	BOX2BMP_BTN_TEMOCHI_SY	( 3 )
//�u�|�P�������ǂ��v�T�C�Y
#define	BOX2BMP_BTN_IDOU_SX		( 11 )
#define	BOX2BMP_BTN_IDOU_SY		( 3 )
//�u���ǂ�v�T�C�Y
#define	BOX2BMP_BTN_MODORU_SX	( 8 )
#define	BOX2BMP_BTN_MODORU_SY	( 3 )
//�u�|�P��������������v�T�C�Y
#define	BOX2BMP_BOXMVMENU_SX	( 11 )
#define	BOX2BMP_BOXMVMENU_SY	( 6 )
//�u�{�b�N�X�����肩����v�T�C�Y
#define	BOX2BMP_BOXMVBTN_SX		( 17 )
#define	BOX2BMP_BOXMVBTN_SY		( 3 )
*/

// ���j���[�{�^���f�[�^
typedef struct {
	u16	strID;		// ������h�c
	u16	type;		// �{�^���^�C�v
}BOX2BMP_BUTTON_LIST;

// �{�^���^�C�v
enum {
	BOX2BMP_BUTTON_TYPE_WHITE = 0,	// ��
	BOX2BMP_BUTTON_TYPE_CANCEL,			// �L�����Z���p
//	BOX2BMP_BUTTON_TYPE_RED,				// ��
};

// ���������b�Z�[�W�\��ID
enum {
	BOX2BMP_MSGID_POKEFREE_CHECK = 0,	// �ق�Ƃ��Ɂ@�ɂ����܂����H
	BOX2BMP_MSGID_POKEFREE_ENTER,		// @0���@���ƂɁ@�ɂ����Ă�����
	BOX2BMP_MSGID_POKEFREE_BY,			// �΂��΂��@@0�I
	BOX2BMP_MSGID_POKEFREE_EGG,			// �^�}�S���@�ɂ������Ƃ́@�ł��܂���I
	BOX2BMP_MSGID_POKEFREE_RETURN,		// @0���@���ǂ��Ă���������I
	BOX2BMP_MSGID_POKEFREE_FEAR,		// ����ς��@�������̂��ȁc�c
	BOX2BMP_MSGID_POKEFREE_ONE,			// ���������|�P�������@���Ȃ��Ȃ�܂��I
};

// �{�b�N�X�e�[�}�ύX���b�Z�[�W�\��ID
enum {
	BOX2BMP_MSGID_THEMA_INIT = 0,		// ���̃{�b�N�X���@�ǂ����܂����H
	BOX2BMP_MSGID_THEMA_JUMP,				// �ǂ̃{�b�N�X�Ɂ@�W�����v���܂����H
	BOX2BMP_MSGID_THEMA_MES,				// �ǂ̃e�[�}�Ɂ@���܂����H
	BOX2BMP_MSGID_THEMA_WALL,				// �ǂ́@���ׂ��݂Ɂ@���܂����H
};

// �|�P��������Ă������b�Z�[�W�\��ID
enum {
	BOX2BMP_MSGID_PARTYIN_INIT = 0,			// �|�P�������@�^�b�`�I
	BOX2BMP_MSGID_PARTYIN_MENU,					// @01���@�ǂ�����H
	BOX2BMP_MSGID_PARTYIN_ERR,					// �Ă������@�����ς��ł��I
	BOX2BMP_MSGID_PARTYOUT_INIT,				// �ǂ��ց@��������H
	BOX2BMP_MSGID_PARTYOUT_BOXMAX,			// ���̃{�b�N�X�́@�����ς����I
	BOX2BMP_MSGID_PARTYOUT_MAIL,				// ���[�����@�͂����Ă��������I
//	BOX2BMP_MSGID_PARTYOUT_CAPSULE,		// �{�[���J�v�Z�����@�͂����Ă��������I
};

// ��������b�Z�[�W�\���h�c
enum {
	BOX2BMP_MSGID_ITEM_A_SET = 0,		// �ǂ������@�������܂�
	BOX2BMP_MSGID_ITEM_A_INIT,			// @0���@�ǂ����܂����H
	BOX2BMP_MSGID_ITEM_A_RET_CHECK,		// @0���@���܂��܂����H
	BOX2BMP_MSGID_ITEM_A_RET,			// @0���@�o�b�O�Ɂ@���ꂽ�I
	BOX2BMP_MSGID_ITEM_A_MAIL,			// ���[�������܂����Ƃ́@�ł��܂���I
	BOX2BMP_MSGID_ITEM_A_EGG,			// �^�}�S�́@�ǂ������@���Ă܂���I
	BOX2BMP_MSGID_ITEM_A_MAX,			// �o�b�O���@�����ς��ł��I
	BOX2BMP_MSGID_ITEM_A_MAIL_MOVE,		// ���[���́@���ǂ��ł��܂���I
};

// VBLANK�ŕ\�����郁�b�Z�[�WID
enum {
	BOX2BMP_MSGID_VBLANK_AZUKERU = 0,		// �{�b�N�X�Ɂ@�����܂��I
	BOX2BMP_MSGID_VBLANK_TSURETEIKU,		// �Ă����Ɂ@���킦�邱�Ƃ��@�ł��܂��I
	BOX2BMP_MSGID_VBLANK_AZUKERU_ERR,		// ���̃{�b�N�X�́@�����ς��ł��I
	BOX2BMP_MSGID_VBLANK_TSURETEIKU_ERR,	// �Ă����́@�����ς��ł��I
	BOX2BMP_MSGID_VBLANK_ITEM_SET,			// �ǂ������@�������܂�
	BOX2BMP_MSGID_VBLANK_ITEM_INIT,			// @0���@�ǂ����܂����H
	BOX2BMP_MSGID_VBLANK_MAIL_ERR,			// ���[�����@�����Ă��܂��I
	BOX2BMP_MSGID_VBLANK_CAPSULE_ERR,		// �{�[���J�v�Z�����@�����Ă��܂��I
	BOX2BMP_MSGID_VBLANK_BATTLE_POKE_ERR,	// ���������|�P�������@���Ȃ��Ȃ�܂��I
	BOX2BMP_MSGID_VBLANK_MAIL_MOVE,			// ���[���́@���ǂ��ł��܂���I
	BOX2BMP_MSGID_VBLANK_ITEM_MOVE,			// �|�P�����Ɂ@�Z�b�g�I
};


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�l�o�֘A������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_Init( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�l�o�֘A�폜
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_Exit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�v�����g���C��
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PrintUtilTrans( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�C�g���\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_TitlePut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�f�t�H���g������\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_DefStrPut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		���ʃ|�P�����f�[�^�\��
 *
 * @param		syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param		info	�\���f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PokeDataPut( BOX2_SYS_WORK * syswk, BOX2_POKEINFO_DATA * info );

//--------------------------------------------------------------------------------------------
/**
 * @brief		���ʃ|�P�����f�[�^��\��
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PokeDataOff( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�E�B���h�E�t���[���쐬�F�e���j���[
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PokeMenuBgFrmWkMake( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�E�B���h�E�t���[���쐬�F�u�Ă����|�P�����v
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 *
 * @li	�o�g���{�b�N�X�̏ꍇ�́u�o�g���{�b�N�X�v
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_TemochiButtonBgFrmWkMake( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�E�B���h�E�t���[���쐬�F�u�{�b�N�X���X�g�v
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_BoxListButtonBgFrmWkMake( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�b�N�X�ړ������̃{�b�N�X���\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		tray		�g���C�ԍ�
 * @param		idx			OAM�t�H���g�̃C���f�b�N�X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_BoxNameWrite( BOX2_SYS_WORK * syswk, u32 tray, u32 idx );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�b�N�X�ړ������̃{�b�N�X���\���i�ʒu�w��j
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�z�u�ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_BoxMoveNameWrite( BOX2_SYS_WORK * syswk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�b�N�X�ړ������̊i�[���\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		tray		�g���C�ԍ�
 * @param		idx			OAM�t�H���g�̃C���f�b�N�X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_WriteTrayNum( BOX2_SYS_WORK * syswk, u32 tray, u32 idx );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�}�[�L���O�t���[���̃{�^���`��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_MarkingButtonPut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�}�[�L���O�t���[���ɂ�BMPWIN���Z�b�g
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_MarkingButtonFrmPut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		���j���[�쐬
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		list		���j���[���X�g
 * @param		max			���j���[��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_MenuStrPrint( BOX2_SYS_WORK * syswk, const BOX2BMP_BUTTON_LIST * list, u32 max );

//--------------------------------------------------------------------------------------------
/**
 * @brief		���j���[��\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		max			���j���[��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_MenuVanish( BOX2_SYS_WORK * syswk, u32 max );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�V�X�e���E�B���h�E��\���i�u�a�k�`�m�j�œ]���j
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 * @param		winID		BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_SysWinVanish( BOX2_APP_WORK * appwk, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�V�X�e���E�B���h�E��\���i���]���j
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 * @param		winID		BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_SysWinVanishTrans( BOX2_APP_WORK * appwk, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�b�N�X�I�����b�Z�[�W�\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		winID		BMPWIN ID
 * @param		flg			�^�b�` or �L�[
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_BoxEndMsgPut( BOX2_SYS_WORK * syswk, u32 winID, BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A�C�e���擾�`�F�b�N���b�Z�[�W�\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		item		�A�C�e���ԍ�
 * @param		winID		BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ItemGetCheckMsgPut( BOX2_SYS_WORK * syswk, u32 item, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		���[���擾�G���[���b�Z�[�W�\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		winID		BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_MailGetErrorPut( BOX2_SYS_WORK * syswk, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		���[���ړ��G���[���b�Z�[�W�\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		winID		BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_MailMoveErrorPut( BOX2_SYS_WORK * syswk, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A�C�e���擾���b�Z�[�W�\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		item		�A�C�e���ԍ�
 * @param		winID		BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ItemGetMsgPut( BOX2_SYS_WORK * syswk, u32 item, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A�C�e���擾�G���[���b�Z�[�W�\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		winID		BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ItemGetErrorPut( BOX2_SYS_WORK * syswk, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A�C�e���Z�b�g���b�Z�[�W�\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		item		�A�C�e���ԍ�
 * @param		winID		BMPWIN ID
 *
 * @return	none
 *
 *	item = 0 �̂Ƃ��́A�͂����񂾂܂��������悤�Ƃ����Ƃ��̃G���[�Ƃ���
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ItemSetMsgPut( BOX2_SYS_WORK * syswk, u32 item, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�}�[�L���O���b�Z�[�W�\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		winID		BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_MarkingMsgPut( BOX2_SYS_WORK * syswk, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		���������b�Z�[�W�\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		msgID		���b�Z�[�W�h�c
 * @param		winID		BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PokeFreeMsgPut( BOX2_SYS_WORK * syswk, u32 msgID, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�b�N�X�e�[�}�ύX���b�Z�[�W�\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		msgID		���b�Z�[�W�h�c
 * @param		winID		BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_BoxThemaMsgPut( BOX2_SYS_WORK * syswk, u32 msgID, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����I�����b�Z�[�W�\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�|�P�����̈ʒu
 * @param		msgID		���b�Z�[�W�h�c
 * @param		winID		BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PokeSelectMsgPut( BOX2_SYS_WORK * syswk, u32 pos, u32 msgID, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		��������b�Z�[�W�\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		msgID		���b�Z�[�W�h�c
 * @param		winID		BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ItemArrangeMsgPut( BOX2_SYS_WORK * syswk, u32 msgID, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ړ��G���[���b�Z�[�W�\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		errID		�G���[�h�c
 * @param		winID		BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PutPokeMoveErrMsg( BOX2_SYS_WORK * syswk, u32 errID, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�˂�����v�p���b�Z�[�W�\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_SleepSelectMsgPut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�o�g���{�b�N�X�v�p���b�N�ς݃��b�Z�[�W�\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_BattleBoxRockMsgPut( BOX2_SYS_WORK * syswk );
