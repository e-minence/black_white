//////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @file   field_place_name.h
 * @brief  �n���\���E�B���h�E
 * @author obata_toshihiro
 * @date   2009.07   
 *
 */
//////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "field_place_name.h"
#include "field/zonedata.h"
#include "arc/arc_def.h"
#include "arc/place_name.naix"
#include "arc/message.naix"
#include "msg/msg_place_name.h"
#include "system/bmp_oam.h"


//===================================================================================
// �� �萔�E�}�N��
//=================================================================================== 
// �ő啶����
#define MAX_NAME_LENGTH (16)

// ��Βl�ɕϊ�����
#define ABS( n ) ( ((n)<0)? -(n) : (n) )

// 1�L���� = 8�h�b�g
#define CHAR_SIZE (8) 

// �����]�[��ID
#define INVALID_ZONE_ID (0xffffffff)

//-----------
// �\���ݒ�
//-----------
#define BG_PALETTE_NO       (0)					// BG�p���b�g�ԍ�
#define BG_FRAME            (GFL_BG_FRAME3_M)	// �g�p����BG�t���[��
#define BG_FRAME_PRIORITY   (1)					// BG�t���[���̃v���C�I���e�B

#define OBJ_PALETTE_NO (1)                      // OBJ�p���b�g�ԍ�

#define	COLOR_NO_LETTER     (1)		            // �����{�̂̃J���[�ԍ�
#define	COLOR_NO_SHADOW     (2)		            // �e�����̃J���[�ԍ�
#define	COLOR_NO_BACKGROUND (0)					// �w�i���̃J���[�ԍ�

#define ALPHA_PLANE_1 (GX_BLEND_PLANEMASK_BG3)	// ���u�����h�̑�1�Ώۖ�
#define ALPHA_PLANE_2 (GX_BLEND_PLANEMASK_BG0)	// ���u�����h�̑�2�Ώۖ�
#define ALPHA_VALUE_1 (16)						// ��1�Ώۖʂ̃��u�����f�B���O�W��
#define ALPHA_VALUE_2 ( 4)						// ��1�Ώۖʂ̃��u�����f�B���O�W��

#define Y_CENTER_POS ( CHAR_SIZE * 2 - 1 )		// �w�i�т̒��Sy���W

#define LAUNCH_INTERVAL (3)	// �������ˊԊu[�P�ʁF�t���[��]

//--------------------
// �A�[�J�C�u�E�f�[�^
//--------------------
#define	ARC_DATA_ID_MAX (29) // �A�[�J�C�u���f�[�^ID�̍ő�l

//-----
// BG
//-----
#define NULL_CHAR_NO       ( 0)								// NULL�L�����ԍ�
#define	BMPWIN_WIDTH_CHAR  (32)								// �E�B���h�E��(�L�����N�^�P��)
#define BMPWIN_HEIGHT_CHAR ( 2)								// �E�B���h�E����(�L�����N�^�P��)
#define BMPWIN_WIDTH_DOT   (BMPWIN_WIDTH_CHAR * CHAR_SIZE)	// �E�B���h�E��(�h�b�g�P��)
#define BMPWIN_HEIGHT_DOT  (BMPWIN_HEIGHT_CHAR * CHAR_SIZE)	// �E�B���h�E����(�h�b�g�P��)
#define BMPWIN_POS_X_CHAR  ( 0)								// �E�B���h�E��x���W(�L�����N�^�P��)
#define BMPWIN_POS_Y_CHAR  ( 1)								// �E�B���h�E��y���W(�L�����N�^�P��)

//------------
// OBJ���\�[�X
//------------
// �L�����N�^�E���\�[�X�E�C���f�b�N�X
enum
{
	CGR_RES_INDEX_MAX
};

// �p���b�g�E���\�[�X�E�C���f�b�N�X
enum
{
	PLTT_RES_INDEX_CHAR_UNIT,	// �������j�b�g�Ŏg�p����p���b�g
	PLTT_RES_INDEX_MAX
};

// �Z���A�j���E���\�[�X�E�C���f�b�N�X
enum
{
	CLANM_RES_INDEX_MAX
};

// �Z���A�N�^�[�E���j�b�g
enum
{
	CLUNIT_INDEX_CHAR_UNIT,		// �������j�b�g
	CLUNIT_INDEX_MAX,
};

//------------------
// �e��Ԃ̓���ݒ�
//------------------
#define PROCESS_TIME_FADE_IN  (10)
#define PROCESS_TIME_WAIT_LAUNCH   (10)
#define PROCESS_TIME_WAIT_FADE_OUT   (30)
#define PROCESS_TIME_FADE_OUT (20)

//-----------------
// �V�X�e���̏��
//-----------------
typedef enum {
	STATE_HIDE,			      // ��\��
  STATE_SETUP,          // ����
	STATE_FADE_IN,	      // �t�F�[�h�E�C��
	STATE_WAIT_LAUNCE,		// ���ˑ҂�
	STATE_LAUNCH,		      // ��������
	STATE_WAIT_FADE_OUT,  // �t�F�[�h�A�E�g�҂�
	STATE_FADE_OUT,	      // �t�F�[�h�E�A�E�g
	STATE_NUM,
  STATE_MAX = STATE_NUM - 1
} STATE;


//===================================================================================
// �� �������j�b�g�E�ړ��p�����[�^
//===================================================================================
typedef struct
{
	float x;		// ���W
	float y;		// 
	float sx;		// ���x
	float sy;		// 
	float ax;		// �����x
	float ay;		// 

	float dx;		// �ڕW�ʒu
	float dy;		// 
	float dsx;	// �ڕW�ʒu�ł̑��x
	float dsy;	// 

	int tx;			// �c��ړ���( x���� )
	int ty;			// �c��ړ���( y���� )
}
CHAR_UNIT_PARAM;


//===================================================================================
// ���������j�b�g
//===================================================================================
typedef struct
{
	GFL_BMP_DATA*   bmp;		      // �r�b�g�}�b�v
	BMPOAM_ACT_PTR  bmpOamActor;	// BMPOAM�A�N�^�[
	CHAR_UNIT_PARAM param;		    // �ړ��p�����[�^
	u8              width;		    // ��
	u8              height;		    // ����
	BOOL            moving;		    // ����t���O
}
CHAR_UNIT;


//===================================================================================
// ���������j�b�g�Ɋւ���֐��Q
//===================================================================================
static void CHAR_UNIT_Initialize( 
    CHAR_UNIT* unit, BMPOAM_SYS_PTR bmpoam_sys, u32 pltt_idx, HEAPID heap_id );
static void CHAR_UNIT_Finalize( CHAR_UNIT* unit );
static void CHAR_UNIT_Write( 
    CHAR_UNIT* unit, GFL_FONT* font, STRCODE code, HEAPID heap_id );
static void CHAR_UNIT_SetMoveParam( CHAR_UNIT* unit, const CHAR_UNIT_PARAM* p_param );
static void CHAR_UNIT_MoveStart( CHAR_UNIT* unit );
static void CHAR_UNIT_Move( CHAR_UNIT* unit );
static BOOL CHAR_UNIT_IsMoving( CHAR_UNIT* unit ); 


//------------------------------------------------------------------------------------ 
/**
 * @brief �������֐�
 *
 * @param unit       ����������ϐ��ւ̃|�C���^
 * @param bmpoam_sys BMPOAM�A�N�^�[��ǉ�����V�X�e��
 * @param pltt_idx   �K�p����p���b�g�̓o�^�C���f�b�N�X
 * @param heap_id    �g�p����q�[�vID
 */
//------------------------------------------------------------------------------------
static void CHAR_UNIT_Initialize( 
    CHAR_UNIT* unit, BMPOAM_SYS_PTR bmpoam_sys, u32 pltt_idx, HEAPID heap_id )
{
	BMPOAM_ACT_DATA actor_data;

	// �r�b�g�}�b�v���쐬
	unit->bmp = GFL_BMP_Create( 2, 2, GFL_BMP_16_COLOR, heap_id );

	// BMPOAM�A�N�^�[���쐬
	actor_data.bmp        = unit->bmp;
	actor_data.x          = 0;
	actor_data.y          = 0;
	actor_data.pltt_index = pltt_idx;
	actor_data.pal_offset = 0;
	actor_data.soft_pri   = 0;
	actor_data.bg_pri     = BG_FRAME_PRIORITY;
	actor_data.setSerface = CLSYS_DEFREND_MAIN;
	actor_data.draw_type  = CLSYS_DRAW_MAIN; 
	unit->bmpOamActor = BmpOam_ActorAdd( bmpoam_sys, &actor_data ); 
	BmpOam_ActorSetDrawEnable( unit->bmpOamActor, FALSE );  // ��\��

	// ������
	unit->param.tx = 0;
	unit->param.ty = 0;
	unit->width    = 0;
	unit->height   = 0;
	unit->moving   = FALSE;
}

//------------------------------------------------------------------------------------
/**
 * @brief ��n���֐�
 *
 * @param unit �g�p���I������ϐ��ւ̃|�C���^
 */
//------------------------------------------------------------------------------------
static void CHAR_UNIT_Finalize( CHAR_UNIT* unit )
{
	// BMPOAM�A�N�^�[��j��
	BmpOam_ActorDel( unit->bmpOamActor );

	// �r�b�g�}�b�v��j��
	GFL_BMP_Delete( unit->bmp );
}

//------------------------------------------------------------------------------------
/**
 * @brief �������j�b�g�̎��r�b�g�}�b�v�ɕ�������������
 *
 * @param unit    �������ݐ惆�j�b�g
 * @param font    �t�H���g
 * @param code    �������ޕ���
 * @param heap_id �g�p����q�[�vID
 */
//------------------------------------------------------------------------------------
static void CHAR_UNIT_Write( CHAR_UNIT* unit, GFL_FONT* font, STRCODE code, HEAPID heap_id )
{
	int i;

	STRCODE string[2];
	STRBUF* strbuf = NULL;
  HEAPID heap_low_id = GFL_HEAP_LOWID( heap_id );
  PRINTSYS_LSB color = 
    PRINTSYS_LSB_Make( COLOR_NO_LETTER, COLOR_NO_SHADOW, COLOR_NO_BACKGROUND );

	// ���̕�������쐬��, �����o�b�t�@���쐬
	string[0] = code;
	string[1] = GFL_STR_GetEOMCode();
	strbuf    = GFL_STR_CreateBuffer( 2, heap_low_id );
	GFL_STR_SetStringCodeOrderLength( strbuf, string, 2 );

	// �T�C�Y�擾
	unit->width  = (u8)PRINTSYS_GetStrWidth( strbuf, font, 0 );
	unit->height = (u8)PRINTSYS_GetStrHeight( strbuf, font );

	// �r�b�g�}�b�v�ɏ�������
	GFL_BMP_Clear( unit->bmp, 0 );
	PRINTSYS_PrintColor( unit->bmp, 0, 0, strbuf, font, color );
	BmpOam_ActorBmpTrans( unit->bmpOamActor );

	// ��n��
	GFL_STR_DeleteBuffer( strbuf );
}

//------------------------------------------------------------------------------------
/**
 * @brief �ړ��J�n���̏����p�����[�^��ݒ肷��
 *
 * @param unit  �ݒ肷��ϐ�
 * @param param �����p�����[�^
 */
//------------------------------------------------------------------------------------ 
static void CHAR_UNIT_SetMoveParam( CHAR_UNIT* unit, const CHAR_UNIT_PARAM* param )
{
	float dist_x;
	float dist_y;

	// �p�����[�^���R�s�[
	unit->param    = *param;
	unit->param.ax = 0;
	unit->param.ay = 0;
	unit->param.tx = 0;
	unit->param.ty = 0;

	// �����x���v�Z
	dist_x = param->dx - param->x;
	if( dist_x != 0 )
  {
    unit->param.ax = 
      ( ( param->dsx * param->dsx ) - ( param->sx * param->sx ) ) / ( 2 * dist_x );
  }
	dist_y = param->dy - param->y;
	if( dist_y != 0 )
  {
    unit->param.ay = 
      ( ( param->dsy * param->dsy ) - ( param->sy * param->sy ) ) / ( 2 * dist_y );
  }

	// �ړ��񐔂��v�Z
	if( unit->param.ax != 0 )
  {
    unit->param.tx = (int)( ( unit->param.dsx - unit->param.sx ) / unit->param.ax );
  }
	if( unit->param.ay != 0 )
  {
    unit->param.ty = (int)( ( unit->param.dsy - unit->param.sy ) / unit->param.ay );
  }

	// BMPOAM�A�N�^�[�̕\���ʒu�𔽉f
	BmpOam_ActorSetPos( unit->bmpOamActor, param->x, param->y );
}

//------------------------------------------------------------------------------------ 
/**
 * @brief �������j�b�g�̈ړ����J�n����
 *
 * @param unit �ړ����J�n���郆�j�b�g
 */
//------------------------------------------------------------------------------------ 
static void CHAR_UNIT_MoveStart( CHAR_UNIT* unit )
{
  unit->moving = TRUE;
}

//------------------------------------------------------------------------------------ 
/**
 * @brief ������
 *
 * @param unit ���������j�b�g
 */
//------------------------------------------------------------------------------------ 
static void CHAR_UNIT_Move( CHAR_UNIT* unit )
{
	// ����t���O���Z�b�g����Ă��Ȃ����, �����Ȃ�
	if( unit->moving != TRUE ) return;

	// x��������
	if( 0 < unit->param.tx )
	{
		// �ړ�
		unit->param.x += unit->param.sx;

		// ����
		unit->param.sx += unit->param.ax;

		// �c��ړ��񐔂��f�N�������g
		unit->param.tx--;
	}
	// y��������
	if( 0 < unit->param.ty )
	{
		// �ړ�
		unit->param.y += unit->param.sy;

		// ����
		unit->param.sy += unit->param.ay;

		// �c��ړ��񐔂��f�N�������g
		unit->param.ty--;
	}

	// �c��ړ��񐔂������Ȃ�����, ����t���O�𗎂Ƃ�
	if( ( unit->param.tx <= 0 ) && ( unit->param.ty <= 0 ) )
	{
		unit->moving = FALSE;
	}
	// BMPOAM�A�N�^�[�̕\���ʒu�ɔ��f
	BmpOam_ActorSetPos( unit->bmpOamActor, unit->param.x, unit->param.y );
}

//------------------------------------------------------------------------------------ 
/**
 * @brief �ړ������ǂ����𒲂ׂ�
 *
 * @param unit ���ׂ郆�j�b�g
 *
 * @return �ړ����Ȃ� TRUE
 */
//------------------------------------------------------------------------------------ 
static BOOL CHAR_UNIT_IsMoving( CHAR_UNIT* unit )
{
	return unit->moving;
}


//===================================================================================
// ���n���\���V�X�e��
//===================================================================================
struct _FIELD_PLACE_NAME
{ 
	HEAPID heapID;	// �q�[�vID

	// �\���Ɏg�p����f�[�^
	GFL_MSGDATA* msg;	  // ���b�Z�[�W�E�f�[�^
  GFL_FONT*    font;  // �t�H���g

	// BG
	GFL_BMPWIN*   bmpWin;	 // �r�b�g�}�b�v�E�E�B���h�E
	GFL_BMP_DATA* bmpOrg;	 // �������������܂�Ă��Ȃ����

	// ���\�[�X
	u32 resPltt[PLTT_RES_INDEX_MAX];  // �p���b�g

	// �Z���A�N�^�[���j�b�g
	GFL_CLUNIT* clunit[CLUNIT_INDEX_MAX];

	// BMPOAM�V�X�e��
	BMPOAM_SYS_PTR bmpOamSys;

	// �������j�b�g
	CHAR_UNIT charUnit[MAX_NAME_LENGTH];

	// ����Ɏg�p����f�[�^
	STATE state;			    // �V�X�e�����
	u16	  stateCount;		  // ��ԃJ�E���^
	u32	  currentZoneID;  // ���ݕ\�����̒n���ɑΉ�����]�[��ID
	u32   nextZoneID;		  // ���ɕ\������n���ɑΉ�����]�[��ID
	u8    launchUnitNum;  // ���ˍςݕ�����

	// �n��
	STRBUF* nameBuf;		  // �\�����̒n��������
	u8      nameLength;	  // �\�����̒n��������
  u8      writeCharNum; // �r�b�g�}�b�v�ւ̏������݂���������������
};


//===================================================================================
/**
 * @brief �V�X�e���Ɋւ���֐�
 */
//===================================================================================
// BG�̐ݒ���s��
static void SetupBG( FIELD_PLACE_NAME* sys );	
static void AllocNullCharacterArea();

// ���\�[�X�̓ǂݍ���
static void LoadBGScreenData( FIELD_PLACE_NAME* sys, u32 arc_id, u32 data_id );
static void LoadBGCharacterData( FIELD_PLACE_NAME* sys, u32 arc_id, u32 data_id );
static void LoadBGPaletteData( FIELD_PLACE_NAME* sys, u32 arc_id, u32 data_id ); 
static void LoadClactResource( FIELD_PLACE_NAME* sys );

// �Z���A�N�^�[���j�b�g���쐬����
static void CreateClactUnit( FIELD_PLACE_NAME* sys );

// �������j�b�g������������
static void CreateCharUnit( FIELD_PLACE_NAME* sys );

// �w�肵���]�[��ID�ɑΉ�����n���𕶎����j�b�g�ɏ�������
static void WritePlaceName( FIELD_PLACE_NAME* sys, u32 zone_id );

// �\������n�����X�V����
static void SetupPlaceName( FIELD_PLACE_NAME* sys );

// �������j�b�g�̓����ݒ肷��
static void SetupCharUnitAction( FIELD_PLACE_NAME* sys );

// �������j�b�g�𔭎˂���
static void LaunchCharUnit( FIELD_PLACE_NAME* sys );

// ��Ԃ�ύX����
static void SetState( FIELD_PLACE_NAME* sys, STATE next_state );

// �����I�ɃE�B���h�E��ޏo������
static void Cancel( FIELD_PLACE_NAME* sys );

// �S�������j�b�g���\���ɐݒ肷��
static void SetAllCharUnitVisibleOff( FIELD_PLACE_NAME* sys );

// �S�������j�b�g�𓮂���
static void MoveAllCharUnit( FIELD_PLACE_NAME* sys );

// �r�b�g�}�b�v�E�E�B���h�E��BG�I���W�i���f�[�^�ɕ�������
static void RecoveryBitmapWindow( FIELD_PLACE_NAME* sys );

// �\�����̕������j�b�g���r�b�g�}�b�v�E�E�B���h�E�ɏ�������
static void WriteCharUnitToBitmapWindow( FIELD_PLACE_NAME* sys );

// �e��Ԏ��̓���
static void Process_HIDE( FIELD_PLACE_NAME* sys );
static void Process_SETUP( FIELD_PLACE_NAME* sys );
static void Process_FADE_IN( FIELD_PLACE_NAME* sys );
static void Process_WAIT_LAUNCH( FIELD_PLACE_NAME* sys );
static void Process_LAUNCH( FIELD_PLACE_NAME* sys );
static void Process_WAIT_FADE_OUT( FIELD_PLACE_NAME* sys );
static void Process_FADE_OUT( FIELD_PLACE_NAME* sys );

// �e��Ԏ��̕`�揈��
static void Draw_HIDE( FIELD_PLACE_NAME* sys );
static void Draw_SETUP( FIELD_PLACE_NAME* sys );
static void Draw_FADE_IN( FIELD_PLACE_NAME* sys );
static void Draw_WAIT_LAUNCH( FIELD_PLACE_NAME* sys );
static void Draw_LAUNCH( FIELD_PLACE_NAME* sys );
static void Draw_WAIT_FADE_OUT( FIELD_PLACE_NAME* sys );
static void Draw_FADE_OUT( FIELD_PLACE_NAME* sys );

// �f�o�b�O�o��
static void DebugPrint( FIELD_PLACE_NAME* sys );


//====================================================================================
// ���쐬�E�j��
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �n���\���V�X�e�����쐬����
 *
 * @param heap_id �g�p����q�[�vID
 * @param msgbg   �g�p���郁�b�Z�[�W�\���V�X�e��
 *
 * @return �n���\���V�X�e���E���[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------------------------
FIELD_PLACE_NAME* FIELD_PLACE_NAME_Create( HEAPID heap_id, FLDMSGBG* msgbg )
{
	FIELD_PLACE_NAME* sys;

	// �V�X�e���E���[�N�쐬
	sys = (FIELD_PLACE_NAME*)GFL_HEAP_AllocClearMemory( heap_id, sizeof( FIELD_PLACE_NAME ) );

	// �V�X�e���̐ݒ�
	sys->heapID = heap_id;

	// BG�̎g�p����
	SetupBG( sys );

	// �Z���A�N�^�[�p���\�[�X�̓ǂݍ���
	LoadClactResource( sys );

	// �Z���A�N�^�[���j�b�g���쐬
	CreateClactUnit( sys );

	// BMPOAM�V�X�e���쐬
	sys->bmpOamSys = BmpOam_Init( heap_id, sys->clunit[ CLUNIT_INDEX_CHAR_UNIT ] );

	// �������j�b�g���쐬
	CreateCharUnit( sys );

	// ���b�Z�[�W�E�f�[�^���쐬
	sys->msg = FLDMSGBG_CreateMSGDATA( msgbg, NARC_message_place_name_dat );

  // �t�H���g�擾
	sys->font = FLDMSGBG_GetFontHandle( msgbg );

	// �����o�b�t�@���쐬
	sys->nameBuf = GFL_STR_CreateBuffer( MAX_NAME_LENGTH + 1, sys->heapID );

	// ���̑��̏�����
	sys->currentZoneID = INVALID_ZONE_ID;
	sys->nextZoneID    = INVALID_ZONE_ID;
	sys->nameLength    = 0;
  sys->writeCharNum  = 0;

	// ������Ԃ�ݒ�
	SetState( sys, STATE_HIDE );

	// �쐬�����V�X�e����Ԃ�
	return sys;
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���\���V�X�e����j������
 *
 * @param sys �j������V�X�e��
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Delete( FIELD_PLACE_NAME* sys )
{
	int i;

	// �����o�b�t�@�����
	GFL_STR_DeleteBuffer( sys->nameBuf );

	// BMPOAM�V�X�e���̌�n��
	for( i=0; i<MAX_NAME_LENGTH; i++ )
	{ 
		CHAR_UNIT_Finalize( &sys->charUnit[i] );
	}
	BmpOam_Exit( sys->bmpOamSys );
	
	// �Z���A�N�^�[���j�b�g�̔j��
	for( i=0; i<CLUNIT_INDEX_MAX; i++ )
	{
		GFL_CLACT_UNIT_Delete( sys->clunit[i] );
	}
	
	// �Z���A�N�^�[�p���\�[�X�̔j��
	for( i=0; i<PLTT_RES_INDEX_MAX; i++ )
	{
		GFL_CLGRP_PLTT_Release( sys->resPltt[i] );
	}
	
	// �`��p�C���X�^���X�̔j��
	GFL_MSG_Delete( sys->msg );

	// �r�b�g�}�b�v�E�E�B���h�E�̔j��
	GFL_BMPWIN_Delete( sys->bmpWin );
	GFL_BMP_Delete( sys->bmpOrg );

	// BGSYS�̌�n��
	GFL_BG_FreeBGControl( BG_FRAME );

	// �V�X�e���E���[�N���
	GFL_HEAP_FreeMemory( sys );
} 


//====================================================================================
// ������
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �n���\���V�X�e���̓��쏈��
 *
 * @param sys �������V�X�e��
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Process( FIELD_PLACE_NAME* sys )
{
	// ��ԃJ�E���^���X�V
	sys->stateCount++;

	// ��Ԃɉ���������
	switch( sys->state )
	{
  case STATE_HIDE:          Process_HIDE( sys );		       break;
  case STATE_SETUP:         Process_SETUP( sys );		       break;
  case STATE_FADE_IN:       Process_FADE_IN( sys );	       break;
  case STATE_WAIT_LAUNCE:   Process_WAIT_LAUNCH( sys );	   break;
  case STATE_LAUNCH:        Process_LAUNCH( sys );	       break;
  case STATE_WAIT_FADE_OUT: Process_WAIT_FADE_OUT( sys );	 break;
  case STATE_FADE_OUT:      Process_FADE_OUT( sys );	     break;
	}

	// �������j�b�g�𓮂���
  MoveAllCharUnit( sys );
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���\���E�B���h�E�̕`�揈��
 *
 * @param sys �`��ΏۃV�X�e��
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Draw( FIELD_PLACE_NAME* sys )
{ 
	switch( sys->state )
	{
  case STATE_HIDE:		        Draw_HIDE( sys );		        break;
  case STATE_SETUP:		        Draw_SETUP( sys );		      break;
  case STATE_FADE_IN:		      Draw_FADE_IN( sys );	      break;
  case STATE_WAIT_LAUNCE:		  Draw_WAIT_LAUNCH( sys );	  break;
  case STATE_LAUNCH:		      Draw_LAUNCH( sys );	        break;
  case STATE_WAIT_FADE_OUT:		Draw_WAIT_FADE_OUT( sys );	break;
  case STATE_FADE_OUT:	      Draw_FADE_OUT( sys );	      break;
	} 
}


//====================================================================================
// ������
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �]�[���̐؂�ւ���ʒm��, �V�����n����\������
 *
 * @param sys        �]�[���؂�ւ���ʒm����V�X�e��
 * @param next_zone_id �V�����]�[��ID
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Display( FIELD_PLACE_NAME* sys, u32 next_zone_id )
{ 
  // �]�[���̒n���\���t���O�������Ă��Ȃ��ꏊ�ł͕\�����Ȃ�
  if( ZONEDATA_GetPlaceNameFlag( next_zone_id ) != TRUE ) return;

	// �w�肳�ꂽ�]�[��ID�����ɕ\�����ׂ����̂Ƃ��ċL��
	sys->nextZoneID = next_zone_id;

	// �\�����̃E�B���h�E��ޏo������
	Cancel( sys ); 
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���E�B���h�E�������I�ɕ\������
 *
 * @param sys   �\������V�X�e��
 * @param zone_id �\������ꏊ�̃]�[��ID
 */
//------------------------------------------------------------------------------------
extern void FIELD_PLACE_NAME_DisplayForce( FIELD_PLACE_NAME* sys, u32 zone_id )
{
  // �����I�ɕ\��
  sys->currentZoneID = INVALID_ZONE_ID;
  sys->nextZoneID    = zone_id;

	// �\�����̃E�B���h�E��ޏo������
	Cancel( sys ); 
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���E�B���h�E�̕\���������I�ɏI������
 *
 * @param sys �\�����I������V�X�e��
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Hide( FIELD_PLACE_NAME* sys )
{
	SetState( sys, STATE_HIDE );
}


//===================================================================================
// ������J�֐�
//===================================================================================

//-----------------------------------------------------------------------------------
/**
 * @brief BG�̐ݒ���s��
 *
 * @param sys �ݒ���s���V�X�e��
 */ 
//-----------------------------------------------------------------------------------
static void SetupBG( FIELD_PLACE_NAME* sys )
{ 
	GFL_BG_BGCNT_HEADER bgcnt = 
	{
		0, 0,					          // �����\���ʒu
		0x800,						      // �X�N���[���o�b�t�@�T�C�Y
		0,							        // �X�N���[���o�b�t�@�I�t�Z�b�g
		GFL_BG_SCRSIZ_256x256,	// �X�N���[���T�C�Y
		GX_BG_COLORMODE_16,			// �J���[���[�h
		GX_BG_SCRBASE_0x1000,		// �X�N���[���x�[�X�u���b�N
		GX_BG_CHARBASE_0x04000,	// �L�����N�^�x�[�X�u���b�N
		GFL_BG_CHRSIZ_256x256,	// �L�����N�^�G���A�T�C�Y
		GX_BG_EXTPLTT_01,			  // BG�g���p���b�g�X���b�g�I��
		1,							        // �\���v���C�I���e�B�[
		0,							        // �G���A�I�[�o�[�t���O
		0,							        // DUMMY
		FALSE,						      // ���U�C�N�ݒ�
	}; 
	GFL_BG_SetBGControl( BG_FRAME, &bgcnt, GFL_BG_MODE_TEXT );
	GFL_BG_SetPriority( BG_FRAME, BG_FRAME_PRIORITY );
	GFL_BG_SetVisible( BG_FRAME, VISIBLE_OFF ); 
	G2_SetBlendAlpha( ALPHA_PLANE_1, ALPHA_PLANE_2, ALPHA_VALUE_1, ALPHA_VALUE_2 );

	// �L����VRAM�E�X�N���[��VRAM���N���A
	GFL_BG_ClearFrame( BG_FRAME );

	// NULL�L�����N�^�f�[�^���m��
	AllocNullCharacterArea();

	// �r�b�g�}�b�v�E�E�B���h�E���쐬����
	sys->bmpWin = GFL_BMPWIN_Create( 
			BG_FRAME,
			BMPWIN_POS_X_CHAR, BMPWIN_POS_Y_CHAR, BMPWIN_WIDTH_CHAR, BMPWIN_HEIGHT_CHAR,
			BG_PALETTE_NO, GFL_BMP_CHRAREA_GET_F );

	// �p���b�g�E�L�����N�^�E�X�N���[����]��
	LoadBGCharacterData( sys, ARCID_PLACE_NAME, NARC_place_name_place_name_back_NCGR );
	LoadBGPaletteData( sys, ARCID_PLACE_NAME, NARC_place_name_place_name_back_NCLR );
	//LoadBGScreenData( sys, ARCID_PLACE_NAME, NARC_area_win_gra_place_name_back_NSCR );
	
	// �r�b�g�}�b�v�E�E�B���h�E�̃f�[�^��VRAM�ɓ]��
	GFL_BMPWIN_MakeTransWindow( sys->bmpWin );

	// DEBUG: �X�N���[���E�o�b�t�@�������o��
	/*
	{
		int i,j;
		u8* p_screen = (u8*)GFL_BG_GetScreenBufferAdrs( BG_FRAME );

		OBATA_Printf( "=============================== screen\n" );

		for( i=0; i<32; i++ )
		{
			for( j=0; j<32; j++ )
			{
				OBATA_Printf( "%d ", p_screen[ i*32 + j ] );
			}
			OBATA_Printf( "\n" );
		}
	}
	*/
}

//------------------------------------------------------------------------------------
/**
 * @brief ��̃L�����N�^�f�[�^��1�L������VRAM�Ɋm�ۂ���
 *        �L�����N�^�ԍ�0���󔒃L�����N�^�Ƃ��Ďg�p���邽�߁B
 */
//------------------------------------------------------------------------------------
static void AllocNullCharacterArea()
{
	GFL_BG_AllocCharacterArea( BG_FRAME, CHAR_SIZE * CHAR_SIZE / 2, GFL_BG_CHRAREA_GET_F );
	//GFL_BG_FillCharacter( BG_FRAME, 0, 1, 0 );
}

//------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���E�f�[�^��ǂݍ���
 *
 * @param sys   �ǂݍ��݂��s���V�X�e��
 * @param arc_id  �A�[�J�C�u�f�[�^�̃C���f�b�N�X
 * @param data_id �A�[�J�C�u���f�[�^�C���f�b�N�X
 */
//------------------------------------------------------------------------------------
static void LoadBGScreenData( FIELD_PLACE_NAME* sys, u32 arc_id, u32 data_id )
{
	ARCHANDLE* handle;
	u32 size;
	void* src;
	NNSG2dScreenData* p_screen;
	handle = GFL_ARC_OpenDataHandle( arc_id, sys->heapID );					// �A�[�J�C�u�f�[�^�n���h���I�[�v��
	size    = GFL_ARC_GetDataSizeByHandle( handle, data_id );					// �f�[�^�T�C�Y�擾
	src   = GFL_HEAP_AllocMemoryLo( sys->heapID, size );					// �f�[�^�o�b�t�@�m��
	GFL_ARC_LoadDataByHandle( handle, data_id, src );						// �f�[�^�擾
	NNS_G2dGetUnpackedScreenData( src, &p_screen );							// �o�C�i������f�[�^��W�J
	GFL_BG_LoadScreenBuffer( BG_FRAME, p_screen->rawData, p_screen->szByte );	// BGSYS�����o�b�t�@�ɓ]��
	GFL_BG_LoadScreenReq( BG_FRAME );											// VRAM�ɓ]��
	GFL_HEAP_FreeMemory( src );												// �f�[�^�o�b�t�@���
	GFL_ARC_CloseDataHandle( handle );											// �A�[�J�C�u�f�[�^�n���h���N���[�Y
}

//------------------------------------------------------------------------------------
/**
 * @brief �L�����N�^�f�[�^�����[�h����
 *
 * @param sys   �ǂݍ��݂��s���V�X�e��
 * @param arc_id  �A�[�J�C�u�f�[�^�̃C���f�b�N�X
 * @param data_id �A�[�J�C�u���f�[�^�C���f�b�N�X
 */
//------------------------------------------------------------------------------------
static void LoadBGCharacterData( FIELD_PLACE_NAME* sys, u32 arc_id, u32 data_id )
{ 
	int i;
	u16 sx, sy, dx, dy;
	GFL_BMP_DATA* dst_bmp = NULL;

	// �R�s�[���E�R�s�[��r�b�g�}�b�v���擾
	sys->bmpOrg = GFL_BMP_LoadCharacter( arc_id, data_id, FALSE, sys->heapID );
	dst_bmp     = GFL_BMPWIN_GetBmp( sys->bmpWin );

	// 1�L�������R�s�[����
	sx = 0;
	sy = 0;
	dx = 0;
	dy = 0;
	for( i=0; i<BMPWIN_WIDTH_CHAR * BMPWIN_HEIGHT_CHAR; i++ )
	{
		// �R�s�[
		GFL_BMP_Print( sys->bmpOrg, dst_bmp, dx, sy, dx, dy, CHAR_SIZE, CHAR_SIZE, 0 );

		// �R�s�[�����W�̍X�V
		sx += CHAR_SIZE;
		if( BMPWIN_WIDTH_DOT <= sx )
		{
			sx  = 0;
			sy += CHAR_SIZE;
		}
		// �R�s�[����W�̍X�V
		dx += CHAR_SIZE;
		if( BMPWIN_WIDTH_DOT <= dx )
		{
			dx  = 0;
			dy += CHAR_SIZE;
		}
	}
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�E�f�[�^��ǂݍ���
 *
 * @param sys   �ǂݍ��݂��s���V�X�e��
 * @param arc_id  �A�[�J�C�u�f�[�^�̃C���f�b�N�X
 * @param data_id �A�[�J�C�u���f�[�^�C���f�b�N�X
 */
//------------------------------------------------------------------------------------
static void LoadBGPaletteData( FIELD_PLACE_NAME* sys, u32 arc_id, u32 data_id )
{
	ARCHANDLE* handle;
	u32 size;
	void* src;
	NNSG2dPaletteData* p_palette;
	handle = GFL_ARC_OpenDataHandle( arc_id, sys->heapID );				// �A�[�J�C�u�f�[�^�n���h���I�[�v��
	size    = GFL_ARC_GetDataSizeByHandle( handle, data_id );				// �f�[�^�T�C�Y�擾
	src   = GFL_HEAP_AllocMemoryLo( sys->heapID, size );				// �f�[�^�o�b�t�@�m��
	GFL_ARC_LoadDataByHandle( handle, data_id, src );					// �f�[�^�擾
	NNS_G2dGetUnpackedPaletteData( src, &p_palette );						// �o�C�i������f�[�^��W�J
	GFL_BG_LoadPalette( BG_FRAME, p_palette->pRawData, 0x20, BG_PALETTE_NO );	// �p���b�g�f�[�^�]��
	GFL_HEAP_FreeMemory( src );											// �f�[�^�o�b�t�@���
	GFL_ARC_CloseDataHandle( handle );										// �A�[�J�C�u�f�[�^�n���h���N���[�Y
}

//-----------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�Ŏg�p���郊�\�[�X��ǂݍ���
 *
 * @param sys �ǂݍ��ݑΏۃV�X�e��
 */
//-----------------------------------------------------------------------------------
static void LoadClactResource( FIELD_PLACE_NAME* sys )
{
	ARCHANDLE* p_arc_handle;

	p_arc_handle = GFL_ARC_OpenDataHandle( ARCID_PLACE_NAME, sys->heapID );

	sys->resPltt[ PLTT_RES_INDEX_CHAR_UNIT ] = 
		GFL_CLGRP_PLTT_RegisterEx( 
				p_arc_handle, NARC_place_name_place_name_string_NCLR,
				CLSYS_DRAW_MAIN, OBJ_PALETTE_NO * 32, 0, 1, sys->heapID );

	GFL_ARC_CloseDataHandle( p_arc_handle );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g���쐬����
 *
 * @param sys �쐬�ΏۃV�X�e��
 */
//-----------------------------------------------------------------------------------
static void CreateClactUnit( FIELD_PLACE_NAME* sys )
{
	// �Z���A�N�^�[���j�b�g���쐬
	sys->clunit[ CLUNIT_INDEX_CHAR_UNIT ] = 
		GFL_CLACT_UNIT_Create( MAX_NAME_LENGTH, BG_FRAME_PRIORITY, sys->heapID );

	// �����ݒ�
	GFL_CLACT_UNIT_SetDrawEnable( sys->clunit[ CLUNIT_INDEX_CHAR_UNIT ], TRUE );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �������j�b�g������������
 *
 * @param sys �쐬�ΏۃV�X�e��
 */
//-----------------------------------------------------------------------------------
static void CreateCharUnit( FIELD_PLACE_NAME* sys )
{
	int i;

	for( i=0; i<MAX_NAME_LENGTH; i++ )
	{
		CHAR_UNIT_Initialize( 
				&sys->charUnit[i], sys->bmpOamSys, 
				sys->resPltt[ PLTT_RES_INDEX_CHAR_UNIT ], sys->heapID );
	} 
}

//-----------------------------------------------------------------------------------
/**
 * @brief �w�肵���]�[��ID�ɑΉ�����n���𕶎����j�b�g�ɏ�������
 *
 * @param sys   ����ΏۃV�X�e��
 * @param zone_id �]�[��ID
 */
//----------------------------------------------------------------------------------- 
static void WritePlaceName( FIELD_PLACE_NAME* sys, u32 zone_id )
{
  int i;
	const STRCODE* strcode = NULL;

  // ���łɏ������݂������ς�
  if( sys->nameLength <= sys->writeCharNum ) return;

  // ���̕�������擾
  strcode = GFL_STR_GetStringCodePointer( sys->nameBuf );

	// 1�������ݒ�
  i = sys->writeCharNum++;
  CHAR_UNIT_Write( &sys->charUnit[i], sys->font, strcode[i], sys->heapID ); 
	sys->charUnit[i].param.x = 256;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �\������n�����X�V����
 *
 * @param sys �X�V����V�X�e��
 */
//-----------------------------------------------------------------------------------
static void SetupPlaceName( FIELD_PLACE_NAME* sys )
{
	u16 str_id;  // ���b�Z�[�W�f�[�^���̕�����ID
  
	// �]�[��ID����n����������擾����
	str_id = ZONEDATA_GetPlaceNameID( sys->nextZoneID );
	if( (str_id < 0) || 
      (msg_place_name_max <= str_id) ) 
  { // �G���[���
    str_id = 0;	
  }
	GFL_MSG_GetString( sys->msg,	str_id, sys->nameBuf );
	sys->nameLength = GFL_STR_GetBufferLength( sys->nameBuf );	// ���������擾

	// �������`�F�b�N
	if( MAX_NAME_LENGTH < sys->nameLength )
	{
    OBATA_Printf( "==================================================\n" );
		OBATA_Printf( "PLACE_NAME: �n�����ő啶�������I�[�o�[���Ă��܂��B\n" );
    OBATA_Printf( "==================================================\n" );
		sys->nameLength =	MAX_NAME_LENGTH;
	}

	//�u�Ȃ��̂΂���v�Ȃ�\�����Ȃ�
	if( str_id == 0 ) 
  {
    OBATA_Printf( "�u�Ȃ��̂΂���v�����o( zone id = %d )\n", sys->nextZoneID );
    FIELD_PLACE_NAME_Hide( sys );
  }
}

//-----------------------------------------------------------------------------------
/**
 * @brief �������j�b�g�̓����ݒ肷��
 *
 * @param �ݒ�ΏۃV�X�e��
 */
//-----------------------------------------------------------------------------------
static void SetupCharUnitAction( FIELD_PLACE_NAME* sys )
{
	int i;
	int str_width;	// ������̕�
	int str_height;	// ������̍���

	// ������̃T�C�Y���擾
	str_width  = PRINTSYS_GetStrWidth( sys->nameBuf, sys->font, 0 );
  str_height = PRINTSYS_GetStrHeight( sys->nameBuf, sys->font );

	// �e�������j�b�g�̐ݒ�
	{
		CHAR_UNIT_PARAM param;

		// 1�����ڂ̖ڕW�p�����[�^��ݒ�
		param.x   = 256;
		param.y   = Y_CENTER_POS - ( str_height / 2 );
		param.sx  = -20;
		param.sy  =  0;
		//param.dx  = 128 - ( str_width / 2 ) - ( sys->nameLength / 2 );	// ��������
		param.dx  = 24;			// ���l��
		param.dy  = param.y;
		param.dsx = 0;
		param.dsy = 0;

		// 1�������ݒ肷��
		for( i=0; i<sys->nameLength; i++ )
		{
			// �ڕW�p�����[�^��ݒ�
			CHAR_UNIT_SetMoveParam( &sys->charUnit[i], &param );

			// �ڕW�p�����[�^���X�V
			param.dx += sys->charUnit[i].width + 1;
		}
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �������j�b�g�𔭎˂���
 *
 * @param sys ���˂���V�X�e��
 */
//-----------------------------------------------------------------------------------
static void LaunchCharUnit( FIELD_PLACE_NAME* sys )
{
  int i;

  // ���łɑS�Ẵ��j�b�g���ˍς�
  if( sys->nameLength <= sys->launchUnitNum ) return;

  // ����
  i = sys->launchUnitNum++;
	CHAR_UNIT_MoveStart( &sys->charUnit[i] );
  BmpOam_ActorSetDrawEnable( sys->charUnit[i].bmpOamActor, TRUE ); // �\��
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��Ԃ�ύX����
 *
 * @param sys      ��Ԃ�ύX����V�X�e��
 * @param next_state �ݒ肷����
 */
//-----------------------------------------------------------------------------------
static void SetState( FIELD_PLACE_NAME* sys, STATE next_state )
{
	// ��Ԃ�ύX��, ��ԃJ�E���^������������
	sys->state      = next_state;
	sys->stateCount = 0;

	// �J�ڐ�̏�Ԃɉ�����������
	switch( next_state )
	{
		case STATE_HIDE:	
			GFL_BG_SetVisible( BG_FRAME, VISIBLE_OFF );	// BG���\��
			SetAllCharUnitVisibleOff( sys );			      // �������j�b�g���\����
			break;
    case STATE_SETUP:
      sys->currentZoneID = sys->nextZoneID;	   // �\�����]�[��ID���X�V
			RecoveryBitmapWindow( sys );	  // �r�b�g�}�b�v�E�B���h�E�𕜋A
      SetupPlaceName( sys );          // �\������n�����X�V
      sys->writeCharNum = 0;          // �������ݏ���
      break;
		case STATE_FADE_IN:
      GFL_BG_SetVisible( BG_FRAME, VISIBLE_ON );	// BG��\��
      Draw_FADE_IN( sys );
			break;
		case STATE_WAIT_LAUNCE:
			break;
		case STATE_LAUNCH:
			sys->launchUnitNum = 0;	     // ���˕����������Z�b�g
			SetupCharUnitAction( sys );	 // �������j�b�g�̓������Z�b�g
			break;
		case STATE_WAIT_FADE_OUT:
			break;
		case STATE_FADE_OUT:
			WriteCharUnitToBitmapWindow( sys );	// �����_�ł̕�����BG�ɏ�������
			SetAllCharUnitVisibleOff( sys );		// �������j�b�g���\����
			break;
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �����I�ɃE�B���h�E��ޏo������
 *
 * @param sys ��Ԃ��X�V����V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Cancel( FIELD_PLACE_NAME* sys )
{
	STATE next_state;
	int   start_count;
	float passed_rate;
	
	// ���݂̏�Ԃɉ���������
	switch( sys->state )
	{
  case STATE_HIDE:	
  case STATE_FADE_OUT:
    return;
  case STATE_WAIT_LAUNCE:
  case STATE_WAIT_FADE_OUT:
  case STATE_LAUNCH:
    next_state  = STATE_WAIT_FADE_OUT;
    start_count = PROCESS_TIME_WAIT_FADE_OUT;
    break;
  case STATE_FADE_IN:
    next_state = STATE_FADE_OUT;
    // �o�ߍς݃t���[�������Z�o
    // (stateCount����, �\���ʒu���Z�o���Ă��邽��, 
    //  �����ޏo�������ꍇ�� stateCount ��K�؂Ɍv�Z����K�v������)
    passed_rate = sys->stateCount / (float)PROCESS_TIME_FADE_IN;
    start_count = (int)( (1.0f - passed_rate) * PROCESS_TIME_FADE_OUT );
    break;
  case STATE_SETUP:
    next_state = STATE_HIDE;
    start_count = 0;
    break;
	}

	// ��Ԃ��X�V
	SetState( sys, next_state );
	sys->stateCount = start_count;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �S�������j�b�g���\���ɐݒ肷��
 *
 * @param sys �ݒ�ΏۃV�X�e��
 */
//-----------------------------------------------------------------------------------
static void SetAllCharUnitVisibleOff( FIELD_PLACE_NAME* sys )
{
	int i;
	
	for( i=0; i<MAX_NAME_LENGTH; i++ )
	{
		BmpOam_ActorSetDrawEnable( sys->charUnit[i].bmpOamActor, FALSE );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �S�������j�b�g�𓮂���
 *
 * @param sys ����ΏۃV�X�e��
 */
//-----------------------------------------------------------------------------------
static void MoveAllCharUnit( FIELD_PLACE_NAME* sys )
{
  int i;
	for( i=0; i<sys->nameLength; i++ )
	{
		CHAR_UNIT_Move( &sys->charUnit[i] );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�E�E�B���h�E��BG�I���W�i���f�[�^�ɕ�������
 *
 * @param sys ����ΏۃV�X�e��
 */
//-----------------------------------------------------------------------------------
static void RecoveryBitmapWindow( FIELD_PLACE_NAME* sys )
{
	GFL_BMP_DATA* p_src = sys->bmpOrg;
	GFL_BMP_DATA* p_dst = GFL_BMPWIN_GetBmp( sys->bmpWin );

	// �I���W�i��(�������������܂�Ă��Ȃ����)���R�s�[����, VRAM�ɓ]��
	GFL_BMP_Copy( p_src, p_dst );	
	GFL_BMPWIN_TransVramCharacter( sys->bmpWin );	
}

//-----------------------------------------------------------------------------------
/**
 * @brief �\�����̕������j�b�g���r�b�g�}�b�v�E�E�B���h�E�ɏ�������
 *
 * @param sys ����ΏۃV�X�e��
 */
//-----------------------------------------------------------------------------------
static void WriteCharUnitToBitmapWindow( FIELD_PLACE_NAME* sys )
{
	int i;
	u16 dx, dy;
	CHAR_UNIT* p_char;
	GFL_BMP_DATA* p_src_bmp = NULL;
	GFL_BMP_DATA* p_dst_bmp = GFL_BMPWIN_GetBmp( sys->bmpWin );

	// �\�����̑S�Ă̕������j�b�g���R�s�[����
	for( i=0; i<sys->nameLength; i++ )
	{
		p_char = &sys->charUnit[i];
		p_src_bmp = p_char->bmp;
		dx = p_char->param.x - ( BMPWIN_POS_X_CHAR * CHAR_SIZE );
		dy = p_char->param.y - ( BMPWIN_POS_Y_CHAR * CHAR_SIZE );
		GFL_BMP_Print( p_src_bmp, p_dst_bmp, 0, 0, dx, dy, p_char->width, p_char->height, 0 );
	}

	// �X�V���ꂽ�L�����f�[�^��VRAM�ɓ]��
	GFL_BMPWIN_TransVramCharacter( sys->bmpWin );
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��\����Ԏ��̓���
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_HIDE( FIELD_PLACE_NAME* sys )
{
	// �]�[���`�F���W���ʒm���ꂽ��, ���̏�Ԃ�
	if( sys->currentZoneID != sys->nextZoneID )
	{
		SetState( sys, STATE_SETUP );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief ������Ԏ��̓���
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_SETUP( FIELD_PLACE_NAME* sys )
{
  // �V�����n������������
  WritePlaceName( sys, sys->nextZoneID );	 

  // �������݂�����������, ���̏�Ԃ�
  if( sys->nameLength <= sys->writeCharNum )
  {
    SetState( sys, STATE_FADE_IN );
  }
}

//-----------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�E�C����Ԏ��̓���
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_FADE_IN( FIELD_PLACE_NAME* sys )
{
	// ��莞�Ԃ��o�߂�����, ���̏�Ԃ�
	if( PROCESS_TIME_FADE_IN < sys->stateCount )
	{
		SetState( sys, STATE_WAIT_LAUNCE );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���ˑ҂���Ԏ��̓���
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_WAIT_LAUNCH( FIELD_PLACE_NAME* sys )
{
	// ��莞�Ԃ��o�߂�����, ���̏�Ԃ�
	if( PROCESS_TIME_WAIT_LAUNCH < sys->stateCount )
	{
		SetState( sys, STATE_LAUNCH );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���ˏ�Ԏ��̓���
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_LAUNCH( FIELD_PLACE_NAME* sys )
{
	// ���Ԋu�ŕ����𔭎�
	if( sys->stateCount % LAUNCH_INTERVAL == 0 )
	{
		// ����
		LaunchCharUnit( sys );

		// ���ׂĂ̕����𔭎˂�����, ���̏�Ԃ�
		if( sys->nameLength <= sys->launchUnitNum )
		{
			SetState( sys, STATE_WAIT_FADE_OUT );
		}
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�A�E�g�҂���Ԏ��̓���
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_WAIT_FADE_OUT( FIELD_PLACE_NAME* sys )
{
	int i;

	// ��莞�Ԃ��o��
	if( PROCESS_TIME_WAIT_FADE_OUT < sys->stateCount )
	{
		// �����Ă��镶��������Ȃ�, �ҋ@��Ԃ��ێ�
		for( i=0; i<sys->nameLength; i++ )
		{
			if( CHAR_UNIT_IsMoving( &sys->charUnit[i] ) == TRUE ) break;
		}

		// ���̏�Ԃ�
		SetState( sys, STATE_FADE_OUT );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�E�A�E�g��Ԏ��̓���
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_FADE_OUT( FIELD_PLACE_NAME* sys )
{
	// ��莞�Ԃ��o�߂�����, ���̏�Ԃ�
	if( PROCESS_TIME_FADE_OUT < sys->stateCount )
	{
		SetState( sys, STATE_HIDE );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��\����Ԏ��̕`�揈��
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_HIDE( FIELD_PLACE_NAME* sys )
{
}

//-----------------------------------------------------------------------------------
/**
 * @brief ������Ԏ��̕`�揈��
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_SETUP( FIELD_PLACE_NAME* sys )
{
}

//-----------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�E�C����Ԏ��̕`�揈��
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_FADE_IN( FIELD_PLACE_NAME* sys )
{
	int val1, val2;
	float rate;

	// ���u�����f�B���O�W�����X�V
	rate  = (float)sys->stateCount / (float)PROCESS_TIME_FADE_IN;
	val1 = (int)( ALPHA_VALUE_1 * rate );
	val2 = (int)( ALPHA_VALUE_2 + (16 - ALPHA_VALUE_2) * (1.0f - rate) );
	G2_SetBlendAlpha( ALPHA_PLANE_1, ALPHA_PLANE_2, val1, val2 );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �ҋ@��Ԏ��̕`�揈��
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_WAIT_LAUNCH( FIELD_PLACE_NAME* sys )
{
}

//-----------------------------------------------------------------------------------
/**
 * @brief �������ˏ�Ԏ��̕`�揈��
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_LAUNCH( FIELD_PLACE_NAME* sys )
{
}

//-----------------------------------------------------------------------------------
/**
 * @brief �ҋ@��Ԏ��̕`�揈��
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_WAIT_FADE_OUT( FIELD_PLACE_NAME* sys )
{
}

//-----------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�E�A�E�g��Ԏ��̕`�揈��
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_FADE_OUT( FIELD_PLACE_NAME* sys )
{
	int val1, val2;
	float rate;
	
	// ���u�����f�B���O�W�����X�V
	rate  = (float)sys->stateCount / (float)PROCESS_TIME_FADE_OUT;
	val1 = (int)( ALPHA_VALUE_1 * (1.0f - rate) );
	val2 = (int)( ALPHA_VALUE_2 + (16 - ALPHA_VALUE_2) * rate );
	G2_SetBlendAlpha( ALPHA_PLANE_1, ALPHA_PLANE_2, val1, val2 );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �f�o�b�O�o��
 *
 * @param sys �f�[�^���o�͂���V�X�e��
 */
//-----------------------------------------------------------------------------------
static void DebugPrint( FIELD_PLACE_NAME* sys )
{
	char* str;

	switch( sys->state )
	{
		case STATE_HIDE:		str = "HIDE";		break;
		case STATE_FADE_IN:		str = "FADE_IN";	break;
		case STATE_WAIT_LAUNCE:		str = "WAIT_1";		break;
		case STATE_LAUNCH:		str = "LAUNCE";		break;
		case STATE_WAIT_FADE_OUT:		str = "WAIT_2";		break;
		case STATE_FADE_OUT:	str = "FADE_OUT";	break;
	}

	// DEBUG:
	OBATA_Printf( "-------------------------------FIELD_PLACE_NAME\n" );
	OBATA_Printf( "state         = %s\n", str );
	OBATA_Printf( "stateCount    = %d\n", sys->stateCount );
	OBATA_Printf( "currentZoneID = %d\n", sys->currentZoneID );
	OBATA_Printf( "nextZoneID    = %d\n", sys->nextZoneID );
}
