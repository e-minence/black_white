//============================================================================================
/**
 * @file  b_plist_obj.c
 * @brief �퓬�p�|�P�������X�g���OBJ����
 * @author  Hiroyuki Nakamura
 * @date  05.02.07
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "poke_tool/gauge_tool.h"
#include "pokeicon/pokeicon.h"
#include "item/item.h"
//#include "item/itemsym.h"
#include "app/app_menu_common.h"
#include "waza_tool/wazadata.h"

/*��[GS_CONVERT_TAG]*/
//#include "system/procsys.h"
/*��[GS_CONVERT_TAG]*/
/*
#include "system/palanm.h"
#include "system/msgdata.h"
#include "system/numfont.h"
#include "system/wordset.h"
*/
//#include "system/clact_tool.h"
/*��[GS_CONVERT_TAG]*/
/*
#include "battle/battle_common.h"
#include "battle/fight_tool.h"
#include "battle/wazatype_icon.h"
#include "poketool/pokeparty.h"
#include "poketool/pokeicon.h"
#include "application/p_status.h"
#include "application/pokelist.h"
#include "application/app_tool.h"
#include "itemtool/item.h"
#include "b_app_tool.h"
*/

#include "battle/battgra_wb.naix"
#include "app_menu_common.naix"

#include "b_plist.h"
#include "b_plist_main.h"
#include "b_plist_obj.h"
#include "b_plist_bmp.h"
#include "b_plist_gra.naix"


//============================================================================================
//  �萔��`
//============================================================================================
/*
#define CLACT_ID_COMMON   ( 45063 ) // ���̉�ʂŎg�p����Z���A�N�^�[��ID

// �L�������\�[�XID
enum {
  CHR_ID_POKE1 = CLACT_ID_COMMON,   // �|�P�����A�C�R���F�P�C��
  CHR_ID_POKE2,           // �|�P�����A�C�R���F�Q�C��
  CHR_ID_POKE3,           // �|�P�����A�C�R���F�R�C��
  CHR_ID_POKE4,           // �|�P�����A�C�R���F�S�C��
  CHR_ID_POKE5,           // �|�P�����A�C�R���F�T�C��
  CHR_ID_POKE6,           // �|�P�����A�C�R���F�U�C��

  CHR_ID_STATUS,            // ��Ԉُ�A�C�R���F�P�C��

  CHR_ID_POKETYPE1,         // �|�P�����^�C�v�A�C�R���P
  CHR_ID_POKETYPE2,         // �|�P�����^�C�v�A�C�R���Q

  CHR_ID_WAZATYPE1,         // �Z�^�C�v�A�C�R���P
  CHR_ID_WAZATYPE2,         // �Z�^�C�v�A�C�R���Q
  CHR_ID_WAZATYPE3,         // �Z�^�C�v�A�C�R���R
  CHR_ID_WAZATYPE4,         // �Z�^�C�v�A�C�R���S
  CHR_ID_WAZATYPE5,         // �Z�^�C�v�A�C�R���T

  CHR_ID_BUNRUI,            // ���ރA�C�R��

  CHR_ID_ITEM,            // �A�C�e���A�C�R��

  CHR_ID_CND,             // �R���f�B�V����

  CHR_ID_CURSOR,            // �J�[�\��

  CHR_ID_MAX = CHR_ID_CURSOR - CLACT_ID_COMMON + 1
};

// �p���b�g���\�[�XID
enum {
  PAL_ID_POKE = CLACT_ID_COMMON,    // �|�P�����A�C�R���i�R�{�j
  PAL_ID_STATUS,            // ��Ԉُ�A�C�R���i�P�{�j
  PAL_ID_TYPE,            // �^�C�v/���ރA�C�R���i�R�{�j
  PAL_ID_ITEM,            // �A�C�e���A�C�R���i�P�{�j
  PAL_ID_CND,             // �R���f�B�V�����i�P�{�j
  PAL_ID_CURSOR,            // �J�[�\���i�P�{�j
  PAL_ID_MAX = PAL_ID_CURSOR - CLACT_ID_COMMON + 1
};

// �Z�����\�[�XID
enum {
  CEL_ID_POKE = CLACT_ID_COMMON,    // �|�P�����A�C�R��
  CEL_ID_STATUS,            // ��Ԉُ�A�C�R��
  CEL_ID_TYPE,            // �^�C�v/���ރA�C�R��
  CEL_ID_ITEM,            // �A�C�e���A�C�R��
  CEL_ID_CND,             // �R���f�B�V����
  CEL_ID_CURSOR,            // �J�[�\��
  CEL_ID_MAX = CEL_ID_CURSOR - CLACT_ID_COMMON + 1
};

// �Z���A�j�����\�[�XID
enum {
  ANM_ID_POKE = CLACT_ID_COMMON,    // �|�P�����A�C�R��
  ANM_ID_STATUS,            // ��Ԉُ�A�C�R��
  ANM_ID_TYPE,            // �^�C�v/���ރA�C�R��
  ANM_ID_ITEM,            // �A�C�e���A�C�R��
  ANM_ID_CND,             // �R���f�B�V����
  ANM_ID_CURSOR,            // �J�[�\��
  ANM_ID_MAX = ANM_ID_CURSOR - CLACT_ID_COMMON + 1
};
*/

#define CURSOR_CLA_MAX    ( 5 )   // �J�[�\����OBJ��

// �p���b�g�ʒu���T�C�Y
#define PALOFS_POKEICON     ( 0 )
#define PALSIZ_POKEICON     ( 3 )
#define PALOFS_STATUSICON   ( PALOFS_POKEICON + 0x20 * PALSIZ_POKEICON )
#define PALSIZ_STATUSICON   ( 1 )
#define PALOFS_ITEMICON     ( PALOFS_STATUSICON + 0x20 * PALSIZ_STATUSICON )
#define PALSIZ_ITEMICON     ( 1 )
#define PALOFS_TYPEICON     ( PALOFS_ITEMICON + 0x20 * PALSIZ_ITEMICON )
#define PALSIZ_TYPEICON     ( 3 )
#define PALOFS_CURSOR       ( PALOFS_TYPEICON + 0x20 * PALSIZ_TYPEICON )
#define PALSIZ_CURSOR       ( 1 )
#define PALOFS_HPGAUGE      ( PALOFS_CURSOR + 0x20 * PALSIZ_CURSOR )
#define PALSIZ_HPGAUGE      ( 1 )


//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================
static void BPL_ClactResManInit( BPLIST_WORK * wk );
static void BPL_ClactPokeLoad( BPLIST_WORK * wk );
static void BPL_ClactStatusLoad( BPLIST_WORK * wk );
//static void BPL_ClactConditionLoad( BPLIST_WORK * wk );
static void BPL_ClactTypeLoad( BPLIST_WORK * wk );
static void BPL_ClactItemLoad( BPLIST_WORK * wk );
static void BPL_ClactAddAll( BPLIST_WORK * wk );

static void BPL_PokeIconPaletteChg( BPLIST_WORK * wk );

static void BPL_Page1ObjSet( BPLIST_WORK * wk );
static void BPL_ChgPageObjSet( BPLIST_WORK * wk );
static void BPL_StMainPageObjSet( BPLIST_WORK * wk );
static void BPL_StWazaSelPageObjSet( BPLIST_WORK * wk );
static void BPL_Page4ObjSet( BPLIST_WORK * wk );
static void BPL_Page5ObjSet( BPLIST_WORK * wk );
static void BPL_Page6ObjSet( BPLIST_WORK * wk );
static void BPL_Page7ObjSet( BPLIST_WORK * wk );
//static void BPL_Page8ObjSet( BPLIST_WORK * wk );
//static void BPL_Page9ObjSet( BPLIST_WORK * wk );

//static void BPL_EzConditionPut( BPLIST_WORK * wk );

static void BPL_ClactCursorAdd( BPLIST_WORK * wk );
static void BPL_CursorDel( BPLIST_WORK * wk );

static void CursorResLoad( BPLIST_WORK * wk );
static void HPGaugeResLoad( BPLIST_WORK * wk );


//============================================================================================
//  �O���[�o���ϐ�
//============================================================================================
// ���\�[�X�e�[�u��
static const u32 ObjParamEz[][5] =
{ // �L�����A�p���b�g�A�Z���A�Z���A�j���ABG�v���C�I���e�B�AOBJ�v���C�I���e�B
  { BPLIST_CHRRES_ITEM, BPLIST_PALRES_ITEM, BPLIST_CELRES_ITEM, 1, 0 },     // �A�C�e���A�C�R���P
  { BPLIST_CHRRES_ITEM, BPLIST_PALRES_ITEM, BPLIST_CELRES_ITEM, 1, 0 },     // �A�C�e���A�C�R���Q
  { BPLIST_CHRRES_ITEM, BPLIST_PALRES_ITEM, BPLIST_CELRES_ITEM, 1, 0 },     // �A�C�e���A�C�R���R
  { BPLIST_CHRRES_ITEM, BPLIST_PALRES_ITEM, BPLIST_CELRES_ITEM, 1, 0 },     // �A�C�e���A�C�R���S
  { BPLIST_CHRRES_ITEM, BPLIST_PALRES_ITEM, BPLIST_CELRES_ITEM, 1, 0 },     // �A�C�e���A�C�R���T
  { BPLIST_CHRRES_ITEM, BPLIST_PALRES_ITEM, BPLIST_CELRES_ITEM, 1, 0 },     // �A�C�e���A�C�R���U
  { BPLIST_CHRRES_ITEM, BPLIST_PALRES_ITEM, BPLIST_CELRES_ITEM, 1, 0 },     // �A�C�e���A�C�R���V

  { BPLIST_CHRRES_POKE1, BPLIST_PALRES_POKE, BPLIST_CELRES_POKE, 1, 1 },      // �|�P�����A�C�R���P
  { BPLIST_CHRRES_POKE2, BPLIST_PALRES_POKE, BPLIST_CELRES_POKE, 1, 1 },      // �|�P�����A�C�R���Q
  { BPLIST_CHRRES_POKE3, BPLIST_PALRES_POKE, BPLIST_CELRES_POKE, 1, 1 },      // �|�P�����A�C�R���R
  { BPLIST_CHRRES_POKE4, BPLIST_PALRES_POKE, BPLIST_CELRES_POKE, 1, 1 },      // �|�P�����A�C�R���S
  { BPLIST_CHRRES_POKE5, BPLIST_PALRES_POKE, BPLIST_CELRES_POKE, 1, 1 },      // �|�P�����A�C�R���T
  { BPLIST_CHRRES_POKE6, BPLIST_PALRES_POKE, BPLIST_CELRES_POKE, 1, 1 },      // �|�P�����A�C�R���U

  { BPLIST_CHRRES_STATUS, BPLIST_PALRES_STATUS, BPLIST_CELRES_STATUS, 1, 1 }, // ��Ԉُ�A�C�R���P
  { BPLIST_CHRRES_STATUS, BPLIST_PALRES_STATUS, BPLIST_CELRES_STATUS, 1, 1 }, // ��Ԉُ�A�C�R���Q
  { BPLIST_CHRRES_STATUS, BPLIST_PALRES_STATUS, BPLIST_CELRES_STATUS, 1, 1 }, // ��Ԉُ�A�C�R���R
  { BPLIST_CHRRES_STATUS, BPLIST_PALRES_STATUS, BPLIST_CELRES_STATUS, 1, 1 }, // ��Ԉُ�A�C�R���S
  { BPLIST_CHRRES_STATUS, BPLIST_PALRES_STATUS, BPLIST_CELRES_STATUS, 1, 1 }, // ��Ԉُ�A�C�R���T
  { BPLIST_CHRRES_STATUS, BPLIST_PALRES_STATUS, BPLIST_CELRES_STATUS, 1, 1 }, // ��Ԉُ�A�C�R���U

  { BPLIST_CHRRES_HPGAUGE, BPLIST_PALRES_HPGAUGE, BPLIST_CELRES_HPGAUGE, 2, 0 },  // �g�o�Q�[�W�g�P
  { BPLIST_CHRRES_HPGAUGE, BPLIST_PALRES_HPGAUGE, BPLIST_CELRES_HPGAUGE, 2, 0 },  // �g�o�Q�[�W�g�Q
  { BPLIST_CHRRES_HPGAUGE, BPLIST_PALRES_HPGAUGE, BPLIST_CELRES_HPGAUGE, 2, 0 },  // �g�o�Q�[�W�g�R
  { BPLIST_CHRRES_HPGAUGE, BPLIST_PALRES_HPGAUGE, BPLIST_CELRES_HPGAUGE, 2, 0 },  // �g�o�Q�[�W�g�S
  { BPLIST_CHRRES_HPGAUGE, BPLIST_PALRES_HPGAUGE, BPLIST_CELRES_HPGAUGE, 2, 0 },  // �g�o�Q�[�W�g�T
  { BPLIST_CHRRES_HPGAUGE, BPLIST_PALRES_HPGAUGE, BPLIST_CELRES_HPGAUGE, 2, 0 },  // �g�o�Q�[�W�g�U

  { BPLIST_CHRRES_POKETYPE1, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �|�P�����^�C�v�A�C�R���P
  { BPLIST_CHRRES_POKETYPE2, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �|�P�����^�C�v�A�C�R���Q
  { BPLIST_CHRRES_POKETYPE3, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �|�P�����^�C�v�A�C�R���R
  { BPLIST_CHRRES_POKETYPE4, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �|�P�����^�C�v�A�C�R���S

  { BPLIST_CHRRES_WAZATYPE1, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���P
  { BPLIST_CHRRES_WAZATYPE2, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���Q
  { BPLIST_CHRRES_WAZATYPE3, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���R
  { BPLIST_CHRRES_WAZATYPE4, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���S
  { BPLIST_CHRRES_WAZATYPE5, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���T
  { BPLIST_CHRRES_WAZATYPE6, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���U
  { BPLIST_CHRRES_WAZATYPE7, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���V
  { BPLIST_CHRRES_WAZATYPE8, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���W
  { BPLIST_CHRRES_WAZATYPE9, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���X
  { BPLIST_CHRRES_WAZATYPE10, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���P�O

  { BPLIST_CHRRES_BUNRUI, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },			 // ���ރA�C�R��
  { BPLIST_CHRRES_BUNRUI2, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },     // ���ރA�C�R���Q
};

// �y�[�W�P�̃|�P�����A�C�R���̍��W
static const GFL_CLACTPOS P1_PokePos[] =
{
  { BPL_COMM_POKEICON_LPX, BPL_COMM_POKEICON1_PY }, { BPL_COMM_POKEICON_RPX, BPL_COMM_POKEICON2_PY },
  { BPL_COMM_POKEICON_LPX, BPL_COMM_POKEICON3_PY }, { BPL_COMM_POKEICON_RPX, BPL_COMM_POKEICON4_PY },
  { BPL_COMM_POKEICON_LPX, BPL_COMM_POKEICON5_PY }, { BPL_COMM_POKEICON_RPX, BPL_COMM_POKEICON6_PY },
};

// �y�[�W�P�̏�Ԉُ�A�C�R���̍��W
static const GFL_CLACTPOS P1_StatusPos[] =
{
  { BPL_COMM_POKEICON_LPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON1_PY+BPL_COMM_STATUSICON_Y },
	{ BPL_COMM_POKEICON_RPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON2_PY+BPL_COMM_STATUSICON_Y },
	{ BPL_COMM_POKEICON_LPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON3_PY+BPL_COMM_STATUSICON_Y },
	{ BPL_COMM_POKEICON_RPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON4_PY+BPL_COMM_STATUSICON_Y },
  { BPL_COMM_POKEICON_LPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON5_PY+BPL_COMM_STATUSICON_Y },
	{ BPL_COMM_POKEICON_RPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON6_PY+BPL_COMM_STATUSICON_Y },
};

// �y�[�W�P�̂g�o�Q�[�W�g�̍��W
static const GFL_CLACTPOS P1_HPGaugePos[] =
{
  { BPL_COMM_POKEICON_LPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON1_PY+BPL_COMM_HPGAUGE_Y },
	{ BPL_COMM_POKEICON_RPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON2_PY+BPL_COMM_HPGAUGE_Y},
	{ BPL_COMM_POKEICON_LPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON3_PY+BPL_COMM_HPGAUGE_Y },
	{ BPL_COMM_POKEICON_RPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON4_PY+BPL_COMM_HPGAUGE_Y },
  { BPL_COMM_POKEICON_LPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON5_PY+BPL_COMM_HPGAUGE_Y },
	{ BPL_COMM_POKEICON_RPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON6_PY+BPL_COMM_HPGAUGE_Y },
};

static const GFL_CLACTPOS P_CHG_PokePos = { 128, 72 };  // ����ւ��y�[�W�̃|�P�����A�C�R���̍��W

static const GFL_CLACTPOS P2_PokePos = { 24, 12 };    // �y�[�W�Q�̃|�P�����A�C�R���̍��W
static const GFL_CLACTPOS P2_StatusPos = { 192+6, 17+3 }; // �y�[�W�Q�̏�Ԉُ�A�C�R���̍��W
// �y�[�W�Q�̃|�P�����̃^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P2_PokeTypePos[] =
{
  { 128+2, 16 },
  { 160+4, 16 }
};
// �y�[�W�Q�̃A�C�e�������̃A�C�e���A�C�R���̍��W
static const GFL_CLACTPOS P2_ItemIconPos = { 20, 132 };
// �y�[�W�Q�̂g�o�Q�[�W�g�̍��W
static const GFL_CLACTPOS P2_HPGaugePos = { 223, 52 };

static const GFL_CLACTPOS P3_PokePos = { 24, 12 };    // �y�[�W�R�̃|�P�����A�C�R���̍��W
static const GFL_CLACTPOS P3_StatusPos = { 192+6, 17+3 }; // �y�[�W�R�̏�Ԉُ�A�C�R���̍��W
// �y�[�W�R�̃|�P�����̃^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P3_PokeTypePos[] =
{
  { 128+2, 16 },
  { 160+4, 16 }
};
// �y�[�W�R�̋Z�̃^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P3_WazaTypePos[] =
{
  {  24,  80 },
  { 152,  80 },
  {  24, 128 },
  { 152, 128 }
};

static const GFL_CLACTPOS P4_PokePos = { 24, 12 };    // �y�[�W�S�̃|�P�����A�C�R���̍��W
static const GFL_CLACTPOS P4_StatusPos = { 192+6, 17+3 }; // �y�[�W�S�̏�Ԉُ�A�C�R���̍��W
// �y�[�W�S�̃|�P�����̃^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P4_PokeTypePos[] =
{
  { 128+2, 16 },
  { 160+4, 16 }
};
static const GFL_CLACTPOS P4_WazaTypePos = { 136, 40 }; // �y�[�W�S�̋Z�^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P4_WazaKindPos = { 24, 80 };  // �y�[�W�S�̋Z���ރA�C�R���̍��W

static const GFL_CLACTPOS P5_PokePos = { 24, 12 };    // �y�[�W�T�̃|�P�����A�C�R���̍��W
static const GFL_CLACTPOS P5_StatusPos = { 192+6, 17+3 }; // �y�[�W�T�̏�Ԉُ�A�C�R���̍��W
// �y�[�W�T�̃|�P�����̃^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P5_PokeTypePos[] =
{
  { 128+2, 16 },
  { 160+4, 16 }
};
// �y�[�W�T�̋Z�̃^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P5_WazaTypePos[] =
{
  {  24,  80 },
  { 152,  80 },
  {  24, 128 },
  { 152, 128 },
  {  88, 176 }
};

static const GFL_CLACTPOS P6_PokePos = { 24, 12 };    // �y�[�W�U�̃|�P�����A�C�R���̍��W
static const GFL_CLACTPOS P6_StatusPos = { 192+6, 17+3 }; // �y�[�W�T�̏�Ԉُ�A�C�R���̍��W
// �y�[�W�U�̃|�P�����̃^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P6_PokeTypePos[] =
{
  { 128+2, 16 },
  { 160+4, 16 }
};
static const GFL_CLACTPOS P6_WazaTypePos = { 136, 40 }; // �y�[�W�U�̋Z�^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P6_WazaKindPos = { 24, 80 };  // �y�[�W�U�̋Z���ރA�C�R���̍��W



//--------------------------------------------------------------------------------------------
/**
 * �퓬�p�|�P���X�gOBJ������
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_ObjInit( BPLIST_WORK * wk )
{
  BPL_ClactResManInit( wk );
  BPL_ClactPokeLoad( wk );
  BPL_ClactStatusLoad( wk );
  BPL_ClactItemLoad( wk );
  BPL_ClactTypeLoad( wk );
//  BPL_ClactConditionLoad( wk );
  CursorResLoad( wk );
  HPGaugeResLoad( wk );
  BPL_ClactAddAll( wk );
  BPL_ClactCursorAdd( wk );

  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�}�l�[�W���[������
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ClactResManInit( BPLIST_WORK * wk )
{
  u32 i;

  for( i=0; i<BPLIST_CHRRES_MAX; i++ ){
    wk->chrRes[i] = 0xffffffff;
  }
  for( i=0; i<BPLIST_PALRES_MAX; i++ ){
    wk->palRes[i] = 0xffffffff;
  }
  for( i=0; i<BPLIST_CELRES_MAX; i++ ){
    wk->celRes[i] = 0xffffffff;
  }

/*
  TCATS_RESOURCE_NUM_LIST crnl = { CHR_ID_MAX, PAL_ID_MAX, CEL_ID_MAX, ANM_ID_MAX, 0, 0 };
  CATS_SYS_PTR  csp = BattleWorkCATS_SYS_PTRGet( wk->dat->bw );

  wk->crp = CATS_ResourceCreate( csp );
  CATS_ClactSetInit( csp, wk->crp, BPL_CA_MAX+CURSOR_CLA_MAX );
  CATS_ResourceManagerInit( csp, wk->crp, &crnl );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���̃O���t�B�b�N���[�h
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ClactPokeLoad( BPLIST_WORK * wk )
{
  ARCHANDLE * ah;
  u32 * res;
  u16 i;
	u16	pos;

  ah = GFL_ARC_OpenDataHandle( ARCID_POKEICON, wk->dat->heap );

  // �L����
  res = &wk->chrRes[BPLIST_CHRRES_POKE1];
  for( i=0; i<TEMOTI_POKEMAX; i++ ){
//		pos = BPLISTMAIN_GetListRow( wk, i );
		pos = i;
    if( wk->poke[pos].mons != 0 ){
      res[i] = GFL_CLGRP_CGR_Register(
                ah,
                POKEICON_GetCgxArcIndex(PP_GetPPPPointerConst(wk->poke[pos].pp)),
                FALSE,
                CLSYS_DRAW_SUB,
                wk->dat->heap );
    }else{
      res[i] = GFL_CLGRP_CGR_Register(
                ah,
                POKEICON_GetCgxArcIndexByMonsNumber(0,0,0),
                FALSE,
                CLSYS_DRAW_SUB,
                wk->dat->heap );
    }
  }

  // �p���b�g
  wk->palRes[BPLIST_PALRES_POKE] = GFL_CLGRP_PLTT_RegisterComp(
                                    ah, POKEICON_GetPalArcIndex(),
                                    CLSYS_DRAW_SUB, PALOFS_POKEICON, wk->dat->heap );

  // �Z���E�A�j��
  wk->celRes[BPLIST_CELRES_POKE] = GFL_CLGRP_CELLANIM_Register(
                                    ah,
                                    POKEICON_GetCellSubArcIndex(),
                                    POKEICON_GetAnmSubArcIndex(),
                                    wk->dat->heap );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * ��Ԉُ�A�C�R���̃O���t�B�b�N���[�h
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ClactStatusLoad( BPLIST_WORK * wk )
{
  ARCHANDLE * ah;

  ah = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, wk->dat->heap );

  // �L����
  wk->chrRes[BPLIST_CHRRES_STATUS] = GFL_CLGRP_CGR_Register(
                                      ah, NARC_app_menu_common_p_st_ijou_NCGR,
                                      FALSE, CLSYS_DRAW_SUB, wk->dat->heap );
  // �p���b�g
  wk->palRes[BPLIST_PALRES_STATUS] = GFL_CLGRP_PLTT_Register(
                                      ah, NARC_app_menu_common_p_st_ijou_NCLR,
                                      CLSYS_DRAW_SUB, PALOFS_STATUSICON, wk->dat->heap );
  // �Z���E�A�j��
  wk->celRes[BPLIST_CELRES_STATUS] = GFL_CLGRP_CELLANIM_Register(
                                      ah,
                                      NARC_app_menu_common_p_st_ijou_32k_NCER,
                                      NARC_app_menu_common_p_st_ijou_32k_NANR,
                                      wk->dat->heap );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * �^�C�v/���ރA�C�R���̃O���t�B�b�N���[�h
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ClactTypeLoad( BPLIST_WORK * wk )
{
  ARCHANDLE * ah;
  u32 i;

  ah = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, wk->dat->heap );

  // �p���b�g
  wk->palRes[BPLIST_PALRES_TYPE] = GFL_CLGRP_PLTT_Register(
                                      ah, NARC_app_menu_common_p_st_type_NCLR,
                                      CLSYS_DRAW_SUB, PALOFS_TYPEICON, wk->dat->heap );
  // �Z���E�A�j��
  wk->celRes[BPLIST_CELRES_TYPE] = GFL_CLGRP_CELLANIM_Register(
                                      ah,
                                      NARC_app_menu_common_p_st_type_32k_NCER,
                                      NARC_app_menu_common_p_st_type_32k_NANR,
                                      wk->dat->heap );
  // �^�C�v�A�C�R���L����
  for( i=BPLIST_CHRRES_POKETYPE1; i<=BPLIST_CHRRES_WAZATYPE10; i++ ){
    wk->chrRes[i] = GFL_CLGRP_CGR_Register(
                      ah, APP_COMMON_GetPokeTypeCharArcIdx(0),
                      FALSE, CLSYS_DRAW_SUB, wk->dat->heap );
  }

  // ���ރA�C�R���L����
  wk->chrRes[BPLIST_CHRRES_BUNRUI] = GFL_CLGRP_CGR_Register(
                                      ah, APP_COMMON_GetWazaKindCharArcIdx(0),
                                      FALSE, CLSYS_DRAW_SUB, wk->dat->heap );
  wk->chrRes[BPLIST_CHRRES_BUNRUI2] = GFL_CLGRP_CGR_Register(
	                                      ah, APP_COMMON_GetWazaKindCharArcIdx(0),
		                                    FALSE, CLSYS_DRAW_SUB, wk->dat->heap );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R���̃O���t�B�b�N���[�h
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ClactItemLoad( BPLIST_WORK * wk )
{
  ARCHANDLE * ah;

  ah = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, wk->dat->heap );

  // �L����
  wk->chrRes[BPLIST_CHRRES_ITEM] = GFL_CLGRP_CGR_Register(
                                      ah, NARC_app_menu_common_item_icon_NCGR,
                                      FALSE, CLSYS_DRAW_SUB, wk->dat->heap );
  // �p���b�g
  wk->palRes[BPLIST_PALRES_ITEM] = GFL_CLGRP_PLTT_Register(
                                      ah, NARC_app_menu_common_item_icon_NCLR,
                                      CLSYS_DRAW_SUB, PALOFS_ITEMICON, wk->dat->heap );
  // �Z���E�A�j��
  wk->celRes[BPLIST_CELRES_ITEM] = GFL_CLGRP_CELLANIM_Register(
                                      ah,
                                      NARC_app_menu_common_item_icon_32k_NCER,
                                      NARC_app_menu_common_item_icon_32k_NANR,
                                      wk->dat->heap );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * �R���f�B�V�����̃O���t�B�b�N���[�h
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
static void BPL_ClactConditionLoad( BPLIST_WORK * wk )
{
  CATS_SYS_PTR  csp = BattleWorkCATS_SYS_PTRGet( wk->dat->bw );

  // �p���b�g
  CATS_LoadResourcePlttWorkArc(
    wk->pfd, FADE_SUB_OBJ, csp, wk->crp, ARC_BPLIST_GRA,
    NARC_b_plist_gra_b_plist_obj_NCLR, 0, 1, NNS_G2D_VRAM_TYPE_2DSUB, PAL_ID_CND );
  // �Z��
  CATS_LoadResourceCellArc(
    csp, wk->crp, ARC_BPLIST_GRA, NARC_b_plist_gra_b_plist_obj_NCER, 0, CEL_ID_CND );
  // �Z���A�j��
  CATS_LoadResourceCellAnmArc(
    csp, wk->crp, ARC_BPLIST_GRA, NARC_b_plist_gra_b_plist_obj_NANR, 0, ANM_ID_CND );
  // �L����
  CATS_LoadResourceCharArc(
    csp, wk->crp, ARC_BPLIST_GRA,
    NARC_b_plist_gra_b_plist_obj_NCGR, 0, NNS_G2D_VRAM_TYPE_2DSUB, CHR_ID_CND );
}
*/

static void CursorResLoad( BPLIST_WORK * wk )
{
  ARCHANDLE * ah;

  ah = GFL_ARC_OpenDataHandle( ARCID_BATTGRA, wk->dat->heap );

  // �L����
  wk->chrRes[BPLIST_CHRRES_CURSOR] = GFL_CLGRP_CGR_Register(
                                      ah, NARC_battgra_wb_battle_w_cursor_NCGR,
                                      FALSE, CLSYS_DRAW_SUB, wk->dat->heap );
  // �p���b�g
  wk->palRes[BPLIST_PALRES_CURSOR] = GFL_CLGRP_PLTT_RegisterEx(
                                      ah, NARC_battgra_wb_battle_w_obj_NCLR,
                                      CLSYS_DRAW_SUB, PALOFS_CURSOR, 1, PALSIZ_CURSOR, wk->dat->heap );

  // �Z���E�A�j��
  wk->celRes[BPLIST_CELRES_CURSOR] = GFL_CLGRP_CELLANIM_Register(
                                      ah,
                                      NARC_battgra_wb_battle_w_cursor_NCER,
                                      NARC_battgra_wb_battle_w_cursor_NANR,
                                      wk->dat->heap );

  GFL_ARC_CloseDataHandle( ah );
}

static void HPGaugeResLoad( BPLIST_WORK * wk )
{
  ARCHANDLE * ah;

  ah = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), wk->dat->heap );

  // �L����
  wk->chrRes[BPLIST_CHRRES_HPGAUGE] = GFL_CLGRP_CGR_Register(
                                        ah,
                                        APP_COMMON_GetHPBarBaseCharArcIdx(APP_COMMON_MAPPING_32K),
                                        FALSE, CLSYS_DRAW_SUB, wk->dat->heap );
  // �p���b�g
/*
  wk->palRes[BPLIST_PALRES_HPGAUGE] = GFL_CLGRP_PLTT_RegisterEx(
                                        ah, APP_COMMON_GetHPBarBasePltArcIdx(),
                                        CLSYS_DRAW_SUB, PALOFS_HPGAUGE, 1, PALSIZ_HPGAUGE, wk->dat->heap );
*/
  wk->palRes[BPLIST_PALRES_HPGAUGE] = GFL_CLGRP_PLTT_Register(
                                        ah, APP_COMMON_GetHPBarBasePltArcIdx(),
                                        CLSYS_DRAW_SUB, PALOFS_HPGAUGE, wk->dat->heap );

  // �Z���E�A�j��
  wk->celRes[BPLIST_CELRES_HPGAUGE] = GFL_CLGRP_CELLANIM_Register(
                                        ah,
                                        APP_COMMON_GetHPBarBaseCellArcIdx(APP_COMMON_MAPPING_32K),
                                        APP_COMMON_GetHPBarBaseAnimeArcIdx(APP_COMMON_MAPPING_32K),
                                        wk->dat->heap );

  GFL_ARC_CloseDataHandle( ah );
}



//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[�ǉ��i�ʁj
 *
 * @param wk    ���[�N
 * @param id    �ǉ�����Z���A�N�^�[��ID
 *
 * @return  �Z���A�N�^�[�f�[�^
 */
//--------------------------------------------------------------------------------------------
static GFL_CLWK * BPL_ClactAdd( BPLIST_WORK * wk, const u32 * res )
{
  GFL_CLWK * clwk;
  GFL_CLWK_DATA dat;

  dat.pos_x = 0;
  dat.pos_y = 0;
  dat.anmseq = 0;
  dat.softpri = res[4];
  dat.bgpri = res[3];

  clwk = GFL_CLACT_WK_Create(
            wk->clunit,
            wk->chrRes[res[0]],
            wk->palRes[res[1]],
            wk->celRes[res[2]],
            &dat, CLSYS_DRAW_SUB, wk->dat->heap );

//  GFL_CLACT_WK_SetPlttOffs( clwk , prm->palNum , CLWK_PLTTOFFS_MODE_PLTT_TOP );
//  GFL_CLACT_WK_SetAutoAnmFlag( plateWork->pokeIcon , TRUE );

  return clwk;

/*
  TCATS_OBJECT_ADD_PARAM_S  prm;
  CATS_SYS_PTR  csp;

  csp = BattleWorkCATS_SYS_PTRGet( wk->dat->bw );

  prm.x = 0;
  prm.y = 0;
  prm.z = 0;

  prm.anm = 0;
  prm.pri = ObjParamEz[id][4];
  prm.pal = 0;
  prm.d_area = NNS_G2D_VRAM_TYPE_2DSUB;

  prm.id[0] = ObjParamEz[id][0];
  prm.id[1] = ObjParamEz[id][1];
  prm.id[2] = ObjParamEz[id][2];
  prm.id[3] = ObjParamEz[id][3];

  prm.bg_pri = 1;
  prm.vram_trans = 0;

  return CATS_ObjectAdd_S( csp, wk->crp, &prm );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[�ǉ��i�S�āj
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ClactAddAll( BPLIST_WORK * wk )
{
  u32 i;

  wk->clunit = GFL_CLACT_UNIT_Create( BPL_CA_MAX+BAPPTOOL_CURSOR_MAX, 0, wk->dat->heap );

  for( i=0; i<BPL_CA_MAX; i++ ){
    wk->clwk[i] = BPL_ClactAdd( wk, ObjParamEz[i] );
  }
  GFL_NET_ReloadIcon();
/*
  {
    u16 * pltt_vram = (u16 *)HW_DB_OBJ_PLTT;
    OS_Printf( "������ pal ������\n" );
    for( i=0; i<16; i++ ){
      OS_Printf( "0x%x, ", pltt_vram[14*16+i] );
    }
    OS_Printf( "\n������ pal ������\n" );
  }
*/

  PaletteWorkSet_VramCopy( wk->pfd, FADE_SUB_OBJ, 0, 0x1e0 );
  BPL_PokeIconPaletteChg( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * �퓬�p�|�P���X�gOBJ�폜
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_ObjFree( BPLIST_WORK * wk )
{
  u32 i;

  for( i=0; i<BPL_CA_MAX; i++ ){
    GFL_CLACT_WK_Remove( wk->clwk[i] );
  }
  BAPPTOOL_DelCursor( wk->cpwk );

  for( i=0; i<BPLIST_CHRRES_MAX; i++ ){
    GFL_CLGRP_CGR_Release( wk->chrRes[i] );
  }
  for( i=0; i<BPLIST_PALRES_MAX; i++ ){
    GFL_CLGRP_PLTT_Release( wk->palRes[i] );
  }
  for( i=0; i<BPLIST_CELRES_MAX; i++ ){
    GFL_CLGRP_CELLANIM_Release( wk->celRes[i] );
  }

  GFL_CLACT_UNIT_Delete( wk->clunit );

/*
  CATS_SYS_PTR  csp;
  u32 i;

  csp = BattleWorkCATS_SYS_PTRGet( wk->dat->bw );

  for( i=0; i<BPL_CA_MAX; i++ ){
    CATS_ActorPointerDelete_S( wk->cap[i] );
  }

  BPL_CursorDel( wk );

  CATS_ResourceDestructor_S( csp, wk->crp );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[��\�����č��W�ύX
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ClactOn( GFL_CLWK * clwk, const GFL_CLACTPOS * pos )
{
  GFL_CLACT_WK_SetDrawEnable( clwk, TRUE );
  GFL_CLACT_WK_SetPos( clwk, pos, CLSYS_DRAW_SUB );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���̃p���b�g�؂�ւ�
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_PokeIconPaletteChg( BPLIST_WORK * wk )
{
	u16	pos;
  s16 i;

  for( i=0; i<TEMOTI_POKEMAX; i++ ){
//		pos = BPLISTMAIN_GetListRow( wk, i );
		pos = i;
    if( wk->poke[pos].mons == 0 ){ continue; }
/*
    CATS_ObjectPaletteSetCap(
      wk->cap[BPL_CA_POKE1+i],
      PokeIconPalNumGet(wk->poke[i].mons,wk->poke[i].form,wk->poke[i].egg) );
*/
// CLWK_PLTTOFFS_MODE_OAM_COLOR
    GFL_CLACT_WK_SetPlttOffs(
      wk->clwk[BPL_CA_POKE1+i],
      POKEICON_GetPalNum(wk->poke[pos].mons,wk->poke[pos].form,wk->poke[pos].egg),
      CLWK_PLTTOFFS_MODE_PLTT_TOP );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �^�C�v�A�C�R���؂�ւ�
 *
 * @param wk    ���[�N
 * @param cap   �Z���A�N�^�[�f�[�^
 * @param res_id  ���\�[�XID
 * @param type  �^�C�v
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_TypeIconChange( BPLIST_WORK * wk, GFL_CLWK * clwk, u32 chrResID, u32 type )
{
  NNSG2dCharacterData * dat;
  void * buf;
  ARCHANDLE * ah;

  ah = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, wk->dat->heap );

  buf = GFL_ARCHDL_UTIL_LoadOBJCharacter(
          ah, APP_COMMON_GetPokeTypeCharArcIdx(type), FALSE, &dat, wk->dat->heap );
  GFL_CLGRP_CGR_Replace( wk->chrRes[chrResID], dat );
  GFL_HEAP_FreeMemory( buf );

  GFL_ARC_CloseDataHandle( ah );

  GFL_CLACT_WK_SetPlttOffs(
    clwk, APP_COMMON_GetPokeTypePltOffset(type), CLWK_PLTTOFFS_MODE_PLTT_TOP );
}

//--------------------------------------------------------------------------------------------
/**
 * ���ރA�C�R���؂�ւ�
 *
 * @param wk    ���[�N
 * @param cap   �Z���A�N�^�[�f�[�^
 * @param kind  ����
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_KindIconChange( BPLIST_WORK * wk, u32 objID, u32 chrResID, u32 kind )
{
  NNSG2dCharacterData * dat;
  void * buf;
  ARCHANDLE * ah;

	objID += wk->waza_kind_swap;
	chrResID += wk->waza_kind_swap;

  ah = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, wk->dat->heap );

  buf = GFL_ARCHDL_UTIL_LoadOBJCharacter(
          ah, APP_COMMON_GetWazaKindCharArcIdx(kind), FALSE, &dat, wk->dat->heap );
  GFL_CLGRP_CGR_Replace( wk->chrRes[chrResID], dat );
  GFL_HEAP_FreeMemory( buf );

  GFL_ARC_CloseDataHandle( ah );

  GFL_CLACT_WK_SetPlttOffs(
    wk->clwk[objID], APP_COMMON_GetWazaKindPltOffset(kind), CLWK_PLTTOFFS_MODE_PLTT_TOP );

	wk->waza_kind_swap ^= 1;
}

//--------------------------------------------------------------------------------------------
/**
 * ��Ԉُ�A�C�R���؂�ւ�
 *
 * @param st    �X�e�[�^�X
 * @param cap   �Z���A�N�^�[�f�[�^
 * @param x   X���W
 * @param y   Y���W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_StIconPut( u16 st, GFL_CLWK * clwk, const GFL_CLACTPOS * pos )
{
  if( st == APP_COMMON_ST_ICON_NONE ){ return; }

  GFL_CLACT_WK_SetAnmSeq( clwk, st );
  BPL_ClactOn( clwk, pos );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����̃^�C�v�A�C�R���؂�ւ�
 *
 * @param wk    ���[�N
 * @param pd    �|�P�����f�[�^
 * @param pos   ���W�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_PokeTypeIconPut( BPLIST_WORK * wk, BPL_POKEDATA * pd, const GFL_CLACTPOS * pos )
{
	u16	objID, chrRes;

	if( wk->poke_type_swap == 0 ){
		objID  = BPL_CA_POKETYPE1;
		chrRes = BPLIST_CHRRES_POKETYPE1;
	}else{
		objID  = BPL_CA_POKETYPE3;
		chrRes = BPLIST_CHRRES_POKETYPE3;
	}
  BPL_TypeIconChange( wk, wk->clwk[objID], chrRes, pd->type1 );
  BPL_ClactOn( wk->clwk[objID], &pos[0] );
  if( pd->type1 != pd->type2 ){
    BPL_TypeIconChange( wk, wk->clwk[objID+1], chrRes+1, pd->type2 );
    BPL_ClactOn( wk->clwk[objID+1], &pos[1] );
  }

	wk->poke_type_swap ^= 1;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R���؂�ւ�
 *
 * @param item  �X�e�[�^�X
 * @param cap   �Z���A�N�^�[�f�[�^
 * @param x   X���W
 * @param y   Y���W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ItemIconPut( u16 item, GFL_CLWK * clwk, const GFL_CLACTPOS * pos )
{
  if( item == ITEM_DUMMY_DATA ){ return; }

  if( ITEM_CheckMail( item ) == TRUE ){
    GFL_CLACT_WK_SetAnmSeq( clwk, 1 );
  }else{
    GFL_CLACT_WK_SetAnmSeq( clwk, 0 );
  }
  BPL_ClactOn( clwk, pos );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R���؂�ւ�
 *
 * @param cb    �J�X�^���{�[��ID
 * @param cap   �Z���A�N�^�[�f�[�^
 * @param x   X���W
 * @param y   Y���W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
static void BPL_CustomBallIconPut( u8 cb, CATS_ACT_PTR cap, const int x, const int y )
{
  if( cb == 0 ){ return; }

  GFL_CLACT_WK_SetAnmSeq( cap, 2 );
  BPL_ClactOn( cap, x, y );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W���Ƃ�OBJ���Z�b�g
 *
 * @param wk    ���[�N
 * @param page  �y�[�W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_PageObjSet( BPLIST_WORK * wk, u32 page )
{
  u32 i;

  for( i=0; i<BPL_CA_MAX; i++ ){
    GFL_CLACT_WK_SetDrawEnable( wk->clwk[i], FALSE );
  }

  switch( page ){
  case BPLIST_PAGE_SELECT:	  // �|�P�����I���y�[�W
	case BPLIST_PAGE_DEAD:			// �m������ւ��I���y�[�W
    BPL_Page1ObjSet( wk );
    break;

  case BPLIST_PAGE_POKE_CHG:  // �|�P��������ւ��y�[�W
    BPL_ChgPageObjSet( wk );
    break;

  case BPLIST_PAGE_MAIN:    // �X�e�[�^�X���C���y�[�W
    BPL_StMainPageObjSet( wk );
    break;

  case BPLIST_PAGE_WAZA_SEL:  // �X�e�[�^�X�Z�I���y�[�W
    BPL_StWazaSelPageObjSet( wk );
    break;

  case BPLIST_PAGE_SKILL:   // �X�e�[�^�X�Z�y�[�W
    BPL_Page4ObjSet( wk );
    break;

  case BPLIST_PAGE_PP_RCV:    // PP�񕜋Z�I���y�[�W
    BPL_Page7ObjSet( wk );
    break;

  case BPLIST_PAGE_WAZASET_BS:  // �X�e�[�^�X�Z�Y��P�y�[�W�i�퓬�Z�I���j
    BPL_Page5ObjSet( wk );
    break;

  case BPLIST_PAGE_WAZASET_BI:  // �X�e�[�^�X�Z�Y��Q�y�[�W�i�퓬�Z�ڍׁj
    BPL_Page6ObjSet( wk );
    break;

/*
  case BPLIST_PAGE_WAZASET_CS:  // �X�e�[�^�X�Z�Y��R�y�[�W�i�R���e�X�g�Z�ڍׁj
    BPL_Page9ObjSet( wk );
    break;

  case BPLIST_PAGE_WAZASET_CI:  // �X�e�[�^�X�Z�Y��S�y�[�W�i�R���e�X�g�Z�I���j
    BPL_Page8ObjSet( wk );
    break;
*/
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�P��OBJ���Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_Page1ObjSet( BPLIST_WORK * wk )
{
	u16	pos;
  s16 i;

  for( i=0; i<TEMOTI_POKEMAX; i++ ){
		pos = BPLISTMAIN_GetListRow( wk, i );

    if( wk->poke[pos].mons == 0 ){ continue; }
    // �|�P�����A�C�R��
    BPL_ClactOn( wk->clwk[BPL_CA_POKE1+pos], &P1_PokePos[i] );
    // ��Ԉُ�A�C�R��
    BPL_StIconPut(
      wk->poke[pos].st, wk->clwk[BPL_CA_STATUS1+pos], &P1_StatusPos[i] );
    // �g�o�Q�[�W�g
    BPL_ClactOn( wk->clwk[BPL_CA_HPGAUGE1+pos], &P1_HPGaugePos[i] );
    // �A�C�e���A�C�R��
    {
      GFL_CLACTPOS  actpos = P1_PokePos[i];
      actpos.x += BPL_COMM_ITEMICON_X;
      actpos.y += BPL_COMM_ITEMICON_Y;
      BPL_ItemIconPut(
        wk->poke[pos].item, wk->clwk[BPL_CA_ITEM1+pos], &actpos );
    }
  }

/*
  s32 i;

  for( i=0; i<TEMOTI_POKEMAX; i++ ){
    if( wk->poke[i].mons == 0 ){ continue; }
    // �|�P�����A�C�R��
    BPL_ClactOn( wk->cap[BPL_CA_POKE1+i], P1_PokePos[i][0], P1_PokePos[i][1] );
    // ��Ԉُ�A�C�R��
    BPL_StIconPut(
      wk->poke[i].st, wk->cap[BPL_CA_STATUS1+i], P1_StatusPos[i][0], P1_StatusPos[i][1] );
    // �A�C�e���A�C�R��
    BPL_ItemIconPut(
      wk->poke[i].item, wk->cap[BPL_CA_ITEM1+i], P1_PokePos[i][0]+8, P1_PokePos[i][1]+8 );
    // �J�X�^���{�[���A�C�R��
    BPL_CustomBallIconPut(
      wk->poke[i].cb, wk->cap[BPL_CA_CB1+i], P1_PokePos[i][0]+16, P1_PokePos[i][1]+8 );
  }
*/
}

//--------------------------------------------------------------------------------------------
/**
 * ����ւ��y�[�W��OBJ���Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ChgPageObjSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd;
  GFL_CLACTPOS  pos;
	u32	row;

	row = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
  pd  = &wk->poke[row];
  pos = P_CHG_PokePos;

  // �|�P�����A�C�R��
  BPL_ClactOn( wk->clwk[BPL_CA_POKE1+row], &pos );
  // �A�C�e���A�C�R��
  pos.x += 8;
  pos.y += 8;
  BPL_ItemIconPut( pd->item, wk->clwk[BPL_CA_ITEM1+row], &pos );

/*
  BPL_POKEDATA * pd;
  u16 i;

  pd = &wk->poke[wk->dat->sel_poke];

  // �|�P�A�C�R��
  BPL_ClactOn( wk->cap[BPL_CA_POKE1+wk->dat->sel_poke], P_CHG_PokePos[0], P_CHG_PokePos[1] );
  // �A�C�e���A�C�R��
  BPL_ItemIconPut(
    pd->item, wk->cap[BPL_CA_ITEM1+wk->dat->sel_poke], P_CHG_PokePos[0]+8, P_CHG_PokePos[1]+8 );
  // �J�X�^���{�[���A�C�R��
  BPL_CustomBallIconPut(
    pd->cb, wk->cap[BPL_CA_CB1+wk->dat->sel_poke], P_CHG_PokePos[0]+16, P_CHG_PokePos[1]+8 );
*/
}


//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�Q��OBJ���Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_StMainPageObjSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd;
  GFL_CLACTPOS  pos;
	u32	row;

	row = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
  pd  = &wk->poke[row];
  pos = P2_PokePos;

  // �|�P�����A�C�R��
  BPL_ClactOn( wk->clwk[BPL_CA_POKE1+row], &pos );
  // ��Ԉُ�A�C�R��
  BPL_StIconPut(
    pd->st, wk->clwk[BPL_CA_STATUS1+row], &P2_StatusPos );
  // �^�C�v
  BPL_PokeTypeIconPut( wk, pd, P2_PokeTypePos );
  // �A�C�e���A�C�R��
  pos.x += 8;
  pos.y += 8;
  BPL_ItemIconPut( pd->item, wk->clwk[BPL_CA_ITEM1+row], &pos );

  // �A�C�e���A�C�R���i�A�C�e�����̉��́j
  BPL_ItemIconPut( pd->item, wk->clwk[BPL_CA_ITEM7], &P2_ItemIconPos );

  // �g�o�Q�[�W
  BPL_ClactOn( wk->clwk[BPL_CA_HPGAUGE1], &P2_HPGaugePos );
}

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�Z�I���y�[�W��OBJ���Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_StWazaSelPageObjSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd;
  GFL_CLACTPOS  pos;
	u32	row;
//  u32 i;

	row = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
  pd  = &wk->poke[row];
  pos = P3_PokePos;

  // �|�P�����A�C�R��
  BPL_ClactOn( wk->clwk[BPL_CA_POKE1+row], &pos );
  // ��Ԉُ�A�C�R��
  BPL_StIconPut(
    pd->st, wk->clwk[BPL_CA_STATUS1+row], &P3_StatusPos );
  // �^�C�v
  BPL_PokeTypeIconPut( wk, pd, P3_PokeTypePos );
  // �A�C�e���A�C�R��
  pos.x += 8;
  pos.y += 8;
  BPL_ItemIconPut( pd->item, wk->clwk[BPL_CA_ITEM1+row], &pos );
  // �Z�^�C�v
  BattlePokelist_WazaTypeSet( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�S��OBJ���Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_Page4ObjSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd;
  GFL_CLACTPOS  pos;
	u32	row;
  u32 i;

	row = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
  pd  = &wk->poke[row];
  pos = P4_PokePos;

  // �|�P�����A�C�R��
  BPL_ClactOn( wk->clwk[BPL_CA_POKE1+row], &pos );
  // ��Ԉُ�A�C�R��
  BPL_StIconPut(
    pd->st, wk->clwk[BPL_CA_STATUS1+row], &P4_StatusPos );
  // �^�C�v
  BPL_PokeTypeIconPut( wk, pd, P4_PokeTypePos );
  // �A�C�e���A�C�R��
  pos.x += 8;
  pos.y += 8;
  BPL_ItemIconPut( pd->item, wk->clwk[BPL_CA_ITEM1+row], &pos );
	// �Z�^�C�v�A�C�R��
	if( wk->waza_type_swap == 0 ){
		BPL_ClactOn( wk->clwk[BPL_CA_WAZATYPE6+wk->dat->sel_wp], &P4_WazaTypePos );
	}else{
		BPL_ClactOn( wk->clwk[BPL_CA_WAZATYPE1+wk->dat->sel_wp], &P4_WazaTypePos );
	}
  // ���ރA�C�R��
	if( wk->waza_kind_swap == 0 ){
	  BPL_ClactOn( wk->clwk[BPL_CA_BUNRUI], &P4_WazaKindPos );
	}else{
	  BPL_ClactOn( wk->clwk[BPL_CA_BUNRUI2], &P4_WazaKindPos );
	}
	BPL_KindIconChange( wk, BPL_CA_BUNRUI, BPLIST_CHRRES_BUNRUI, pd->waza[wk->dat->sel_wp].kind );
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�T��OBJ���Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_Page5ObjSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd;
  GFL_CLACTPOS  pos;
	u32	row;

	row = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
  pd  = &wk->poke[row];
  pos = P5_PokePos;

  // �|�P�����A�C�R��
  BPL_ClactOn( wk->clwk[BPL_CA_POKE1+row], &pos );
  // ��Ԉُ�A�C�R��
  BPL_StIconPut(
    pd->st, wk->clwk[BPL_CA_STATUS1+row], &P5_StatusPos );
  // �^�C�v
  BPL_PokeTypeIconPut( wk, pd, P5_PokeTypePos );
  // �A�C�e���A�C�R��
  pos.x += 8;
  pos.y += 8;
  BPL_ItemIconPut( pd->item, wk->clwk[BPL_CA_ITEM1+row], &pos );
  // �Z�^�C�v�A�C�R��
  BattlePokelist_WazaTypeSet( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�U��OBJ���Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_Page6ObjSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd;
  GFL_CLACTPOS  pos;
  u32 i;
	u32	row;

	row = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
  pd  = &wk->poke[row];
  pos = P6_PokePos;

  // �|�P�����A�C�R��
  BPL_ClactOn( wk->clwk[BPL_CA_POKE1+row], &pos );
  // ��Ԉُ�A�C�R��
  BPL_StIconPut(
    pd->st, wk->clwk[BPL_CA_STATUS1+row], &P6_StatusPos );
  // �^�C�v
  BPL_PokeTypeIconPut( wk, pd, P6_PokeTypePos );
  // �A�C�e���A�C�R��
  pos.x += 8;
  pos.y += 8;
  BPL_ItemIconPut( pd->item, wk->clwk[BPL_CA_ITEM1+row], &pos );
  // �Z�^�C�v�A�C�R��
	if( wk->waza_type_swap == 0 ){
	  BPL_ClactOn( wk->clwk[BPL_CA_WAZATYPE6+wk->dat->sel_wp], &P6_WazaTypePos );
	}else{
	  BPL_ClactOn( wk->clwk[BPL_CA_WAZATYPE1+wk->dat->sel_wp], &P6_WazaTypePos );
	}
  // ���ރA�C�R��
	if( wk->waza_kind_swap == 0 ){
	  BPL_ClactOn( wk->clwk[BPL_CA_BUNRUI], &P6_WazaKindPos );
	}else{
	  BPL_ClactOn( wk->clwk[BPL_CA_BUNRUI2], &P6_WazaKindPos );
	}
  if( wk->dat->sel_wp < 4 ){
    BPL_KindIconChange( wk, BPL_CA_BUNRUI, BPLIST_CHRRES_BUNRUI, pd->waza[wk->dat->sel_wp].kind );
  }else{
    BPL_KindIconChange(
      wk, BPL_CA_BUNRUI, BPLIST_CHRRES_BUNRUI, WAZADATA_GetParam(wk->dat->chg_waza,WAZAPARAM_DAMAGE_TYPE) );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�V��OBJ���Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_Page7ObjSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd;
  GFL_CLACTPOS  pos;
	u32	row;

	row = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
  pd  = &wk->poke[row];
  pos = P5_PokePos;

  // �|�P�����A�C�R��
  BPL_ClactOn( wk->clwk[BPL_CA_POKE1+row], &pos );
  // ��Ԉُ�A�C�R��
  BPL_StIconPut(
    pd->st, wk->clwk[BPL_CA_STATUS1+row], &P5_StatusPos );
  // �^�C�v
  BPL_PokeTypeIconPut( wk, pd, P5_PokeTypePos );
  // �A�C�e���A�C�R��
  pos.x += 8;
  pos.y += 8;
  BPL_ItemIconPut( pd->item, wk->clwk[BPL_CA_ITEM1+row], &pos );
  // �Z�^�C�v�A�C�R��
  BattlePokelist_WazaTypeSet( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�W��OBJ���Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
static void BPL_Page8ObjSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd = &wk->poke[wk->dat->sel_poke];

  // �|�P�A�C�R��
  BPL_ClactOn( wk->cap[BPL_CA_POKE1+wk->dat->sel_poke], P6_PokePos[0], P6_PokePos[1] );

  // �Z�^�C�v
  BPL_ClactOn(
    wk->cap[BPL_CA_WAZATYPE1+wk->dat->sel_wp], P8_WazaTypePos[0], P8_WazaTypePos[1] );
  // �A�C�e���A�C�R��
  BPL_ItemIconPut(
    pd->item, wk->cap[BPL_CA_ITEM1+wk->dat->sel_poke], P6_PokePos[0]+8, P6_PokePos[1]+8 );
  // �J�X�^���{�[���A�C�R��
  BPL_CustomBallIconPut(
    pd->cb, wk->cap[BPL_CA_CB1+wk->dat->sel_poke], P6_PokePos[0]+16, P6_PokePos[1]+8 );

  // �R���f�B�V����
  BPL_EzConditionPut( wk );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�X��OBJ���Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
static void BPL_Page9ObjSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd = &wk->poke[wk->dat->sel_poke];

  // �|�P�A�C�R��
  BPL_ClactOn( wk->cap[BPL_CA_POKE1+wk->dat->sel_poke], P6_PokePos[0], P6_PokePos[1] );

  // �Z�^�C�v
//  BPL_ClactOn(
//    wk->cap[BPL_CA_WAZATYPE1+wk->dat->sel_wp], P8_WazaTypePos[0], P8_WazaTypePos[1] );

  // �A�C�e���A�C�R��
  BPL_ItemIconPut(
    pd->item, wk->cap[BPL_CA_ITEM1+wk->dat->sel_poke], P6_PokePos[0]+8, P6_PokePos[1]+8 );
  // �J�X�^���{�[���A�C�R��
  BPL_CustomBallIconPut(
    pd->cb, wk->cap[BPL_CA_CB1+wk->dat->sel_poke], P6_PokePos[0]+16, P6_PokePos[1]+8 );

  // �Z�^�C�v
  BattlePokelist_WazaTypeSet( wk );

  // �R���f�B�V����
  BPL_EzConditionPut( wk );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �Z�A�C�R���Z�b�g�F�퓬 or �R���e�X�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokelist_WazaTypeSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd;
  u32 i;
	u16	objID, chrRes;

  pd = &wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ];

	if( wk->waza_type_swap == 0 ){
		objID  = BPL_CA_WAZATYPE1;
		chrRes = BPLIST_CHRRES_WAZATYPE1;
	}else{
		objID  = BPL_CA_WAZATYPE6;
		chrRes = BPLIST_CHRRES_WAZATYPE6;
	}

  // �Z�^�C�v
  for( i=0; i<4; i++ ){
    if( pd->waza[i].id == 0 ){ continue; }

    BPL_TypeIconChange( wk, wk->clwk[objID+i], chrRes+i, pd->waza[i].type );
    BPL_ClactOn( wk->clwk[objID+i], &P5_WazaTypePos[i] );
  }

  if( wk->dat->chg_waza != 0 && wk->dat->mode == BPL_MODE_WAZASET ){
      BPL_TypeIconChange(
        wk, wk->clwk[objID+i], chrRes+i,
        WAZADATA_GetParam( wk->dat->chg_waza, WAZAPARAM_TYPE ) );
      BPL_ClactOn( wk->clwk[objID+i], &P5_WazaTypePos[i] );
  }

	wk->waza_type_swap ^= 1;
}


//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���A�j���؂�ւ�
 *
 * @param cap   �Z���A�N�^�[�f�[�^
 * @param anm   �A�j��
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_PokeIconAnmChg( GFL_CLWK * clwk, u16 anm )
{
/*
  if( CATS_ObjectAnimeSeqGetCap( cap ) == anm ){ return; }
  GFL_CLACT_WK_SetAnmFrameCap( cap, 0 );
  GFL_CLACT_WK_SetAnmSeq( cap, anm );
*/
  if( GFL_CLACT_WK_GetAnmSeq( clwk ) == anm ){ return; }

  GFL_CLACT_WK_SetAnmFrame( clwk, 0 );
  GFL_CLACT_WK_SetAnmSeq( clwk, anm );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���A�j���`�F�b�N
 *
 * @param pd    �|�P�����f�[�^
 *
 * @return  �A�j���ԍ�
 */
//--------------------------------------------------------------------------------------------
static u8 BPL_PokeIconAnmCheck( BPL_POKEDATA * pd )
{
  if( pd->hp == 0 ){
    return POKEICON_ANM_DEATH;
  }

  if( pd->st != APP_COMMON_ST_ICON_NONE && pd->st != APP_COMMON_ST_ICON_HINSI ){
    return POKEICON_ANM_STCHG;
  }

  if( pd->hp == pd->mhp ){
    return POKEICON_ANM_HPMAX;
  }

  switch( GAUGETOOL_GetGaugeDottoColor( pd->hp, pd->mhp ) ){
  case GAUGETOOL_HP_DOTTO_GREEN:    // ��
    return POKEICON_ANM_HPGREEN;
  case GAUGETOOL_HP_DOTTO_YELLOW: // ��
    return POKEICON_ANM_HPYERROW;
  case GAUGETOOL_HP_DOTTO_RED:      // ��
    return POKEICON_ANM_HPRED;
  }

  return POKEICON_ANM_DEATH;

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���A�j��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BPL_PokeIconAnime( BPLIST_WORK * wk )
{
  u16 i;
  u8	anm;
	u8	pos;

//  for( i=0; i<PokeParty_GetPokeCount(wk->dat->pp); i++ ){
  for( i=0; i<TEMOTI_POKEMAX; i++ ){
		pos = BPLISTMAIN_GetListRow( wk, i );

    if( wk->poke[pos].mons == 0 ){ continue; }

    anm = BPL_PokeIconAnmCheck( &wk->poke[pos] );

    BPL_PokeIconAnmChg( wk->clwk[BPL_CA_POKE1+pos], anm );
    GFL_CLACT_WK_AddAnmFrame( wk->clwk[BPL_CA_POKE1+pos], FX32_ONE );
//    GFL_CLACT_WK_AddAnmFrameNumCap( wk->cap[BPL_CA_POKE1+i], FX32_ONE );
  }
}

//============================================================================================
//  �ȈՃR���f�B�V����
//============================================================================================
/*
#define EZCON1_MIN_X  ( 144 )
#define EZCON1_MAX_X  ( 144 )
#define EZCON1_MIN_Y  ( 24 )
#define EZCON1_MAX_Y  ( 2 )

#if AFTER_MASTER_070409_59_EUR_FIX

#define EZCON2_MIN_X  ( 144 )
#define EZCON2_MAX_X  ( 164 )
#define EZCON2_MIN_Y  ( 24 )
#define EZCON2_MAX_Y  ( 16 )

#define EZCON3_MIN_X  ( 144 )
#define EZCON3_MAX_X  ( 156 )
#define EZCON3_MIN_Y  ( 24 )
#define EZCON3_MAX_Y  ( 41 )

#define EZCON4_MIN_X  ( 143 )
#define EZCON4_MAX_X  ( 131 )
#define EZCON4_MIN_Y  ( 24 )
#define EZCON4_MAX_Y  ( 41 )

#define EZCON5_MIN_X  ( 143 )
#define EZCON5_MAX_X  ( 123 )
#define EZCON5_MIN_Y  ( 24 )
#define EZCON5_MAX_Y  ( 16 )

#else

#define EZCON2_MIN_X  ( 144 )
#define EZCON2_MAX_X  ( 167 )
#define EZCON2_MIN_Y  ( 24 )
#define EZCON2_MAX_Y  ( 18 )

#define EZCON3_MIN_X  ( 144 )
#define EZCON3_MAX_X  ( 159 )
#define EZCON3_MIN_Y  ( 24 )
#define EZCON3_MAX_Y  ( 47 )

#define EZCON4_MIN_X  ( 143 )
#define EZCON4_MAX_X  ( 128 )
#define EZCON4_MIN_Y  ( 24 )
#define EZCON4_MAX_Y  ( 47 )

#define EZCON5_MIN_X  ( 143 )
#define EZCON5_MAX_X  ( 120 )
#define EZCON5_MIN_Y  ( 24 )
#define EZCON5_MAX_Y  ( 18 )

#endif

#define EZCND_MAX_RPM ( 300 )
#define EZCND_DEF_RPM ( 44 )    // 300 = 256 + EZCND_DEF_RPM
*/

//--------------------------------------------------------------------------------------------
/**
 * �ȈՃR���f�B�V�����̕\�����W�擾
 *
 * @param prm   �p�����[�^
 * @param max   �ő�l
 * @param min   �ŏ��l
 *
 * @return  �\�����W
 */
//--------------------------------------------------------------------------------------------
/*
static s16 BPL_EzCndPosGet( u32 prm, s16 max, s16 min )
{
  u32 dot;

  prm += EZCND_DEF_RPM;

  if( min > max ){
    dot = ( ( min - max ) * prm ) << 16;
    dot = ( dot / EZCND_MAX_RPM ) >> 16;
    return min + (s16)dot * -1;
  }

  dot = ( ( max - min ) * prm ) << 16;
  dot = ( dot / EZCND_MAX_RPM ) >> 16;
  return min + (s16)dot;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �ȈՃR���f�B�V�����\��
 *
 * @param wk    �퓬�|�P���X�g�̃��[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
static void BPL_EzConditionPut( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd;
  u16 i;

  pd = &wk->poke[wk->dat->sel_poke];

  BPL_PokeIconAnmChg( wk->cap[BPL_CA_CND1], 0 );
  BPL_PokeIconAnmChg( wk->cap[BPL_CA_CND2], 1 );
  BPL_PokeIconAnmChg( wk->cap[BPL_CA_CND3], 3 );
  BPL_PokeIconAnmChg( wk->cap[BPL_CA_CND4], 4 );
  BPL_PokeIconAnmChg( wk->cap[BPL_CA_CND5], 2 );

  BPL_ClactOn(
    wk->cap[BPL_CA_CND1],
    BPL_EzCndPosGet( pd->style, EZCON1_MAX_X, EZCON1_MIN_X ),
    BPL_EzCndPosGet( pd->style, EZCON1_MAX_Y, EZCON1_MIN_Y ) );
  BPL_ClactOn(
    wk->cap[BPL_CA_CND2],
    BPL_EzCndPosGet( pd->beautiful, EZCON2_MAX_X, EZCON2_MIN_X ),
    BPL_EzCndPosGet( pd->beautiful, EZCON2_MAX_Y, EZCON2_MIN_Y ) );
  BPL_ClactOn(
    wk->cap[BPL_CA_CND3],
    BPL_EzCndPosGet( pd->cute, EZCON3_MAX_X, EZCON3_MIN_X ),
    BPL_EzCndPosGet( pd->cute, EZCON3_MAX_Y, EZCON3_MIN_Y ) );
  BPL_ClactOn(
    wk->cap[BPL_CA_CND4],
    BPL_EzCndPosGet( pd->clever, EZCON4_MAX_X, EZCON4_MIN_X ),
    BPL_EzCndPosGet( pd->clever, EZCON4_MAX_Y, EZCON4_MIN_Y ) );
  BPL_ClactOn(
    wk->cap[BPL_CA_CND5],
    BPL_EzCndPosGet( pd->strong, EZCON5_MAX_X, EZCON5_MIN_X ),
    BPL_EzCndPosGet( pd->strong, EZCON5_MAX_Y, EZCON5_MIN_Y ) );
}
*/


//============================================================================================
//  �J�[�\��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ǉ�
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ClactCursorAdd( BPLIST_WORK * wk )
{
  BAPPTOOL_AddCursor(
    wk->cpwk,
    wk->clunit,
    wk->chrRes[BPLIST_CHRRES_CURSOR],
    wk->palRes[BPLIST_PALRES_CURSOR],
    wk->celRes[BPLIST_CELRES_CURSOR] );

  BAPPTOOL_VanishCursor( wk->cpwk, FALSE );

/*
  CATS_SYS_PTR csp;
  BCURSOR_PTR cursor;

  csp = BattleWorkCATS_SYS_PTRGet( wk->dat->bw );

  BCURSOR_ResourceLoad(
    csp, wk->crp, wk->pfd, wk->dat->heap, CHR_ID_CURSOR, PAL_ID_CURSOR, CEL_ID_CURSOR, ANM_ID_CURSOR );

  cursor = BCURSOR_ActorCreate(
        csp, wk->crp, wk->dat->heap,
        CHR_ID_CURSOR, PAL_ID_CURSOR, CEL_ID_CURSOR, ANM_ID_CURSOR, 0, 1 );

  BAPP_CursorMvWkSetBCURSOR_PTR( wk->cmv_wk, cursor );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���폜
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_CursorDel( BPLIST_WORK * wk )
{
  BAPPTOOL_DelCursor( wk->cpwk );

/*
  BCURSOR_ActorDelete( BAPP_CursorMvWkGetBCURSOR_PTR( wk->cmv_wk ) );
  BCURSOR_ResourceFree(
    wk->crp, CHR_ID_CURSOR, PAL_ID_CURSOR, CEL_ID_CURSOR, ANM_ID_CURSOR );
*/
}

/*
// �|�P�����I����ʈړ��e�[�u��
static const POINTSEL_WORK P1_CursorPosTbl[] =
{
  {   8,   8, 120,  40, 6, 2, 6, 1 },   // 0
  { 136,  16, 248,  48, 4, 3, 0, 2 },   // 1
  {   8,  56, 120,  88, 0, 4, 1, 3 },   // 2
  { 136,  64, 248,  96, 1, 5, 2, 4 },   // 3
  {   8, 104, 120, 136, 2, 1, 3, 5 },   // 4
  { 136, 112, 248, 144, 3, 6, 4, 6 },   // 5
  { 224, 160, 248, 184, 5, 0, 5, 0 },   // 6�i�߂�j
};
static const POINTSEL_WORK Chg_CursorPosTbl[] =
{
  {  16,  16, 240, 136, 0, BAPP_CMV_RETBIT|1, 0, 0 },   // 0 : ���ꂩ����
  {   8, 160,  96, 184, 0, 1, 1, 2 },           // 1 : ����������
  { 112, 160, 200, 184, 0, 2, 1, 3 },           // 2 : �Z������
  { 224, 160, 248, 184, 0, 3, 2, 3 },           // 3 : �߂�
};
static const POINTSEL_WORK StMain_CursorPosTbl[] =
{
  {   8, 160,  32, 184, 0, 0, 0, 1 },   // 0 : ��
  {  48, 160,  72, 184, 1, 1, 0, 2 },   // 1 : ��
  { 104, 160, 192, 184, 2, 2, 1, 3 },   // 2 : �Z������
  { 224, 160, 248, 184, 3, 3, 2, 3 },   // 3 : �߂�
};
static const POINTSEL_WORK StWazaSel_CursorPosTbl[] =
{
  {   8,  56, 120,  88, 0, 2, 0, 1 },   // 0 : �Z�P
  { 136,  56, 248,  88, 1, 3, 0, 1 },   // 1 : �Z�Q
  {   8, 104, 120, 136, 0, BAPP_CMV_RETBIT|4, 2, 3 },   // 2 : �Z�R
  { 136, 104, 248, 136, 1, BAPP_CMV_RETBIT|7, 2, 3 },   // 3 : �Z�S
  {   8, 160,  32, 184, 2, 4, 4, 5 },   // 4 : ��
  {  48, 160,  72, 184, 2, 5, 4, 6 },   // 5 : ��
  { 104, 160, 192, 184, 3, 6, 5, 7 },   // 6 : ����������
  { 224, 160, 248, 184, 3, 7, 6, 7 },   // 7 : �߂�
};
static const POINTSEL_WORK StWazaInfo_CursorPosTbl[] =
{
  {  92, 157, 124, 165, 0, 2, 0, 1 },   // 0 : �Z�P
  { 132, 157, 164, 165, 1, 3, 0, 4 },   // 1 : �Z�Q
  {  92, 173, 124, 181, 0, 2, 2, 3 },   // 2 : �Z�R
  { 132, 173, 164, 181, 1, 3, 2, 4 },   // 3 : �Z�S
  { 224, 160, 248, 184, 4, 4, BAPP_CMV_RETBIT|3, 4 },   // 4 : �߂�
};
static const POINTSEL_WORK DelSel_CursorPosTbl[] =
{
  {   8,  56, 120,  88, 5, 2, 0, 1 },           // 0 : �Z�P��ʂ�
  { 136,  56, 248,  88, 5, 3, 0, 1 },           // 1 : �Z�Q��ʂ�
  {   8, 104, 120, 136, 0, 4, 2, 3 },           // 2 : �Z�R��ʂ�
  { 136, 104, 248, 136, 1, 6, 2, 3 },           // 3 : �Z�S��ʂ�
  {  72, 152, 184, 184, 2, 4, 4, 6 },           // 4 : �Z�T��ʂ�
  { 192,   8, 248,  24, 5, BAPP_CMV_RETBIT|1, 0, 5 },   // 5 : �퓬<->�R���e�X�g�̐؂�ւ�
  { 224, 160, 248, 184, 3, 6, 4, 6 },           // 6 : �߂�
};

static const POINTSEL_WORK DelInfo_CursorPosTbl[] =
{
  {   8, 160, 200, 184, 1, 0, 0, 2 },           // 0 : �킷���
  { 192,   8, 248,  24, 1, BAPP_CMV_RETBIT|2, 0, 1 },   // 1 : �퓬<->�R���e�X�g�̐؂�ւ�
  { 224, 160, 248, 184, 1, 2, 0, 2 },           // 2 : �߂�
};

static const POINTSEL_WORK PPRcv_CursorPosTbl[] =
{
  {   8,  56, 120,  88, 0, 2, 0, 1 },           // 0 : �Z�P��
  { 136,  56, 248,  88, 1, 3, 0, 1 },           // 1 : �Z�Q��
  {   8, 104, 120, 136, 0, 4, 2, 3 },           // 2 : �Z�R��
  { 136, 104, 248, 136, 1, 4, 2, 3 },           // 3 : �Z�S��
  { 224, 160, 248, 184, BAPP_CMV_RETBIT|3, 4, 4, 4 },   // 4 : �߂�
};

static const POINTSEL_WORK * const CursorPosTable[] = {
  P1_CursorPosTbl,      // �|�P�����I���y�[�W
  Chg_CursorPosTbl,     // �|�P��������ւ��y�[�W
  StMain_CursorPosTbl,    // �X�e�[�^�X���C���y�[�W
  StWazaSel_CursorPosTbl,   // �X�e�[�^�X�Z�I���y�[�W
  StWazaInfo_CursorPosTbl,  // �X�e�[�^�X�Z�ڍ׃y�[�W
  PPRcv_CursorPosTbl,     // PP�񕜋Z�I���y�[�W
  DelSel_CursorPosTbl,    // �X�e�[�^�X�Z�Y��P�y�[�W�i�퓬�Z�I���j
  DelInfo_CursorPosTbl,   // �X�e�[�^�X�Z�Y��Q�y�[�W�i�퓬�Z�ڍׁj
  DelSel_CursorPosTbl,    // �X�e�[�^�X�Z�Y��R�y�[�W�i�R���e�X�g�Z�I���j
  DelInfo_CursorPosTbl,   // �X�e�[�^�X�Z�Y��S�y�[�W�i�R���e�X�g�Z�ڍׁj
};
*/
/*

/*
#define WAZADEL_SEL_MV_TBL_N  ( 0x7f )  // �Z�Y��̋Z�I���̃J�[�\���ړ��e�[�u���i�ʏ�j
#define WAZADEL_SEL_MV_TBL_C  ( 0x5f )  // �Z�Y��̋Z�I���̃J�[�\���ړ��e�[�u���i�R���e�X�g�Ȃ��j
#define WAZADEL_MAIN_MV_TBL_N ( 7 ) // �Z�Y��̏ڍׂ̃J�[�\���ړ��e�[�u���i�ʏ�j
#define WAZADEL_MAIN_MV_TBL_C ( 5 ) // �Z�Y��̏ڍׂ̃J�[�\���ړ��e�[�u���i�R���e�X�g�Ȃ��j
*/

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ��e�[�u���쐬�i�Z�Y��Z�I���y�[�W�j
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
static void BPL_WazaDelSelCursorMvTblMake( BPLIST_WORK * wk )
{
  if( wk->ev_contest == 0 ){
    BAPP_CursorMvWkSetMvTbl( wk->cmv_wk, WAZADEL_SEL_MV_TBL_C );
  }else{
    BAPP_CursorMvWkSetMvTbl( wk->cmv_wk, WAZADEL_SEL_MV_TBL_N );
  }
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ��e�[�u���쐬�i�Z�Y��Z����y�[�W�j
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
static void BPL_WazaDelMainCursorMvTblMake( BPLIST_WORK * wk )
{
  if( wk->ev_contest == 0 ){
    BAPP_CursorMvWkSetMvTbl( wk->cmv_wk, WAZADEL_MAIN_MV_TBL_C );
  }else{
    BAPP_CursorMvWkSetMvTbl( wk->cmv_wk, WAZADEL_MAIN_MV_TBL_N );
  }
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���Z�b�g
 *
 * @param wk    ���[�N
 * @param page  �y�[�WID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
void BattlePokeList_CursorMoveSet( BPLIST_WORK * wk, u8 page )
{
  BAPP_CursorMvWkSetPoint( wk->cmv_wk, CursorPosTable[page] );

  switch( page ){

  case BPLIST_PAGE_SELECT:    // �|�P�����I���y�[�W
      BAPP_CursorMvWkSetPos( wk->cmv_wk, wk->dat->sel_poke );
    wk->chg_page_cp = 0;
    wk->dat->sel_wp = 0;
    break;

  case BPLIST_PAGE_POKE_CHG:    // �|�P��������ւ��y�[�W
    BAPP_CursorMvWkSetPos( wk->cmv_wk, wk->chg_page_cp );
    wk->dat->sel_wp = 0;
    break;

  case BPLIST_PAGE_WAZA_SEL:    // �X�e�[�^�X�Z�I���y�[�W
  case BPLIST_PAGE_SKILL:     // �X�e�[�^�X�Z�ڍ׃y�[�W
    BAPP_CursorMvWkSetPos( wk->cmv_wk, wk->dat->sel_wp );
    break;

  case BPLIST_PAGE_WAZASET_BS:  // �X�e�[�^�X�Z�Y��P�y�[�W�i�퓬�Z�I���j
//  case BPLIST_PAGE_WAZASET_CS:  // �X�e�[�^�X�Z�Y��R�y�[�W�i�R���e�X�g�Z�I���j
    BPL_WazaDelSelCursorMvTblMake( wk );
    BAPP_CursorMvWkSetPos( wk->cmv_wk, wk->wws_page_cp );
    break;

  case BPLIST_PAGE_WAZASET_BI:  // �X�e�[�^�X�Z�Y��Q�y�[�W�i�퓬�Z�ڍׁj
//  case BPLIST_PAGE_WAZASET_CI:  // �X�e�[�^�X�Z�Y��S�y�[�W�i�R���e�X�g�Z�ڍׁj
    BPL_WazaDelMainCursorMvTblMake( wk );
    BAPP_CursorMvWkSetPos( wk->cmv_wk, wk->wwm_page_cp );
    break;
  }
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\����\��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
void BattlePokeList_CursorOff( BPLIST_WORK * wk )
{
  BAPP_CursorMvWkSetFlag( wk->cmv_wk, 0 );
  BAPP_CursorMvWkPosInit( wk->cmv_wk );
  BCURSOR_OFF( BAPP_CursorMvWkGetBCURSOR_PTR( wk->cmv_wk ) );
}
*/

void BPLISTOBJ_MoveDeadChg( BPLIST_WORK * wk, u8 num, s8 mv )
{
	GFL_CLACTPOS	pos;
	u32	row;

	row = BPLISTMAIN_GetListRow( wk, num );

  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_ITEM1+row], &pos, CLSYS_DRAW_SUB );
	pos.x += mv;
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_ITEM1+row], &pos, CLSYS_DRAW_SUB );

  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_POKE1+row], &pos, CLSYS_DRAW_SUB );
	pos.x += mv;
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_POKE1+row], &pos, CLSYS_DRAW_SUB );

  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_STATUS1+row], &pos, CLSYS_DRAW_SUB );
	pos.x += mv;
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_STATUS1+row], &pos, CLSYS_DRAW_SUB );

  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_HPGAUGE1+row], &pos, CLSYS_DRAW_SUB );
	pos.x += mv;
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_HPGAUGE1+row], &pos, CLSYS_DRAW_SUB );
}

void BPLISTOBJ_ChgDeadSel( BPLIST_WORK * wk, u8 num1, u8 num2 )
{
	GFL_CLACTPOS	pos1, pos2;
	u32	row1, row2;

	row1 = BPLISTMAIN_GetListRow( wk, num1 );
	row2 = BPLISTMAIN_GetListRow( wk, num2 );

  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_ITEM1+row1], &pos1, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_ITEM1+row2], &pos2, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_ITEM1+row1], &pos2, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_ITEM1+row2], &pos1, CLSYS_DRAW_SUB );

  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_POKE1+row1], &pos1, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_POKE1+row2], &pos2, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_POKE1+row1], &pos2, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_POKE1+row2], &pos1, CLSYS_DRAW_SUB );

  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_STATUS1+row1], &pos1, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_STATUS1+row2], &pos2, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_STATUS1+row1], &pos2, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_STATUS1+row2], &pos1, CLSYS_DRAW_SUB );

  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_HPGAUGE1+row1], &pos1, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_HPGAUGE1+row2], &pos2, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_HPGAUGE1+row1], &pos2, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_HPGAUGE1+row2], &pos1, CLSYS_DRAW_SUB );
}
