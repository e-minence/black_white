//============================================================================================
/**
 * @file	
 * @brief	
 * @date	
 */
//============================================================================================
#include "map3d.h"
#include "act3d.h"

#define PLAYER_SETUP_MAX	(8)		//���̂Ƃ��낱��ȏ�\��������͓̂�������A���C��
#define PLAYER_SETUP_NUM	(8)
#define STATUS_SETUP_NUM	(48)		//�X�e�[�^�X�A�C�R���ő吔

typedef struct _GAME_SYSTEM GAME_SYSTEM;

extern GAME_SYSTEM*	SetupGameSystem( HEAPID heapID );
extern void			RemoveGameSystem( GAME_SYSTEM* gs );
extern void			MainGameSystemPref( GAME_SYSTEM* gs );
extern void			MainGameSystemAfter( GAME_SYSTEM* gs );

extern GFL_G3D_UTIL*		Get_GS_G3Dutil( GAME_SYSTEM* gs );
extern GFL_G3D_SCENE*		Get_GS_G3Dscene( GAME_SYSTEM* gs );
extern GFL_G3D_CAMERA*		Get_GS_G3Dcamera( GAME_SYSTEM* gs, int cameraID );
extern GFL_G3D_LIGHTSET*	Get_GS_G3Dlight( GAME_SYSTEM* gs, int lightID );
extern GFL_BMPWIN*			Get_GS_BmpWin( GAME_SYSTEM* gs, int bmpwinID );
extern GFL_PTC_PTR			Get_GS_Perticle( GAME_SYSTEM* gs );
extern SCENE_MAP*			Get_GS_SceneMap( GAME_SYSTEM* gs );
extern SCENE_ACT*			Get_GS_SceneAct( GAME_SYSTEM* gs );
extern GFL_CLUNIT*			Get_GS_ClactUnit( GAME_SYSTEM* gs, u32 unitID );
extern u32					Get_GS_ClactResIdx( GAME_SYSTEM* gs, u32 resID );

#define PLT_2D_COL_WHITE	(15)
#define PLT_2D_COL_BLACK	(1)
#define PLT_2D_COL_NULL		(0)
//------------------------------------------------------------------
/**
 * @brief	�f�[�^�w�b�_
 */
//------------------------------------------------------------------
//�R�c�J�����h�m�c�d�w
enum {
	MAINCAMERA_ID = 0,
	SUBCAMERA_ID,
};

//�R�c�J�����h�m�c�d�w
enum {
	MAINLIGHT_ID = 0,
	SUBLIGHT_ID,
};

//�R�c�O���t�B�b�N���\�[�X�e�[�u���h�m�c�d�w
enum {
	G3DRES_MAP_FLOOR = 0,
	G3DRES_HUMAN2_BMD,
	G3DRES_HUMAN_COMMON_BCA,
	G3DRES_HUMAN2_ATTACK_BCA,
	G3DRES_HUMAN2_SHOOT_BCA,
	G3DRES_HUMAN2_SPELL_BCA,
	G3DRES_ACCE_SWORD,
	G3DRES_ACCE_SHIELD,
	G3DRES_ACCE_BOW,
	G3DRES_ACCE_STAFF,
	G3DRES_EFFECT_WALL,
	G3DRES_EFFECT_ARROW,
};

//---------------------
//�R�c�I�u�W�F�N�g�A�j���[�V������`�e�[�u���h�m�c�d�w
enum {
	HUMAN_ANM_COMMON = 0,
	HUMAN2_ANM_ATTACK,
	HUMAN2_ANM_SHOOT,
	HUMAN2_ANM_SPELL,
};

//---------------------
//�R�c�I�u�W�F�N�g��`�e�[�u���h�m�c�d�w
enum {
	G3DOBJ_MAP_FLOOR,
// �v���[���[�� (=PLAYER_SETUP_NUM)
	G3DOBJ_HUMAN1,
	G3DOBJ_HUMAN2,
	G3DOBJ_HUMAN3,
	G3DOBJ_HUMAN4,
	G3DOBJ_HUMAN5,
	G3DOBJ_HUMAN6,
	G3DOBJ_HUMAN7,
	G3DOBJ_HUMAN8,
//
	G3DOBJ_ACCE_SWORD,
	G3DOBJ_ACCE_SHIELD,
	G3DOBJ_ACCE_BOW,
	G3DOBJ_ACCE_STAFF,

	G3DOBJ_EFFECT_WALL,
	G3DOBJ_EFFECT_ARROW,
	G3DOBJ_NPC,
};

//---------------------
//�Q�c�r�b�g�}�b�v�E�C���h�E��`�e�[�u���h�m�c�d�w
enum {
	G2DBMPWIN_STATUS = 0,
	G2DBMPWIN_MSG,
	G2DBMPWIN_MAP,
	G2DBMPWIN_MASK,
};

//---------------------
//�b�k�`�b�s���\�[�X��`�e�[�u���h�m�c�d�w
enum {
	CLACTRES_MAPOBJ_CGX = 0,
	CLACTRES_MAPOBJ_PLT_R,
	CLACTRES_MAPOBJ_PLT_B,
	CLACTRES_MAPOBJ_CEL,
	CLACTRES_STATUS_CEL,
	CLACTRES_STATUS_CGX,
	CLACTRES_STATUS_PLT = CLACTRES_STATUS_CGX + STATUS_SETUP_NUM,
};

//---------------------
//�b�k�`�b�s���j�b�g��`�e�[�u���h�m�c�d�w
enum {
	CLACTUNIT_MAPOBJ = 0,
	CLACTUNIT_STATUS,
};

