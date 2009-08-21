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
#include "field_place_name.h"
#include "field/zonedata.h"
#include "arc/arc_def.h"
#include "arc/place_name.naix"
#include "arc/message.naix"
#include "msg/msg_place_name.h"
#include "system/bmp_oam.h"


//===================================================================================
/**
 * @brief �萔�E�}�N��
 */
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
#define PROCESS_TIME_WAIT_1   (10)
#define PROCESS_TIME_WAIT_2   (30)
#define PROCESS_TIME_FADE_OUT (20)

//-----------------
// �V�X�e���̏��
//-----------------
typedef enum
{
	STATE_HIDE,			// ��\��
	STATE_FADE_IN,		// �t�F�[�h�E�C��
	STATE_WAIT_1,		// �ҋ@1
	STATE_LAUNCH,		// ��������
	STATE_WAIT_2,		// �ҋ@2
	STATE_FADE_OUT,		// �t�F�[�h�E�A�E�g
	STATE_MAX,
}
STATE;


//===================================================================================
/**
 * @brief �������j�b�g�E�p�����[�^
 */
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
	float dsx;		// �ڕW�ʒu�ł̑��x
	float dsy;		// 

	int tx;			// �c��ړ���( x���� )
	int ty;			// �c��ړ���( y���� )
}
CHAR_UNIT_PARAM;

//===================================================================================
/**
 * @brief �������j�b�g�\����
 */
//===================================================================================
typedef struct
{
	GFL_BMP_DATA*   pBmp;		// �r�b�g�}�b�v
	BMPOAM_ACT_PTR  pBmpOamAct;	// BMPOAM�A�N�^�[
	CHAR_UNIT_PARAM param;		// �ړ��p�����[�^
	u8              width;		// ��
	u8              height;		// ����
	BOOL            moving;		// ����t���O
}
CHAR_UNIT;

//===================================================================================
/**
 * @brief �������j�b�g�\���̂Ɋւ���֐��Q
 */
//===================================================================================
// �������֐�
static void CHAR_UNIT_Initialize( CHAR_UNIT* p_unit, BMPOAM_SYS_PTR p_bmp_oam_sys, u32 pltt_idx, HEAPID heap_id );

// ��n���֐�
static void CHAR_UNIT_Finalize( CHAR_UNIT* p_unit );

// �r�b�g�}�b�v���쐬����
static void CHAR_UNIT_SetCharCode( CHAR_UNIT* p_unit, GFL_FONT* p_font, STRCODE code, HEAPID heap_id );

// �ړ��J�n���̏����p�����[�^��ݒ肷��
static void CHAR_UNIT_SetMoveParam( CHAR_UNIT* p_unit, const CHAR_UNIT_PARAM* p_param );

// ������
static void CHAR_UNIT_Move( CHAR_UNIT* p_unit );

// �ړ������ǂ����𒲂ׂ�
static BOOL CHAR_UNIT_IsMoving( CHAR_UNIT* p_unit ); 



//------------------------------------------------------------------------------------ 
/**
 * @brief �������֐�
 *
 * @param p_unit        ����������ϐ��ւ̃|�C���^
 * @param p_bmp_oam_sys BMPOAM�A�N�^�[��ǉ�����V�X�e��
 * @param pltt_idx      �K�p����p���b�g�̓o�^�C���f�b�N�X
 * @param heap_id       �g�p����q�[�vID
 */
//------------------------------------------------------------------------------------
static void CHAR_UNIT_Initialize( CHAR_UNIT* p_unit, BMPOAM_SYS_PTR p_bmp_oam_sys, u32 pltt_idx, HEAPID heap_id )
{
	BMPOAM_ACT_DATA data;

	// �r�b�g�}�b�v���쐬
	p_unit->pBmp = GFL_BMP_Create( 2, 2, GFL_BMP_16_COLOR, heap_id );

	// BMPOAM�A�N�^�[�̏����ݒ�
	data.bmp        = p_unit->pBmp;
	data.x          = 0;
	data.y          = 0;
	data.pltt_index = pltt_idx;
	data.pal_offset = 0;
	data.soft_pri   = 0;
	data.bg_pri     = BG_FRAME_PRIORITY;
	data.setSerface = CLSYS_DEFREND_MAIN;
	data.draw_type  = CLSYS_DRAW_MAIN; 

	// BMPOAM�A�N�^�[���쐬
	p_unit->pBmpOamAct = BmpOam_ActorAdd( p_bmp_oam_sys, &data );

	// ��\���ɐݒ�
	BmpOam_ActorSetDrawEnable( p_unit->pBmpOamAct, FALSE );

	// ������
	p_unit->param.tx = 0;
	p_unit->param.ty = 0;
	p_unit->width  = 0;
	p_unit->height = 0;
	p_unit->moving = FALSE;
}

//------------------------------------------------------------------------------------
/**
 * @brief ��n���֐�
 *
 * @param p_unit �g�p���I������ϐ��ւ̃|�C���^
 */
//------------------------------------------------------------------------------------
static void CHAR_UNIT_Finalize( CHAR_UNIT* p_unit )
{
	// BMPOAM�A�N�^�[��j��
	BmpOam_ActorDel( p_unit->pBmpOamAct );

	// �r�b�g�}�b�v��j��
	GFL_BMP_Delete( p_unit->pBmp );
}

//------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v���쐬����
 *
 * @param p_unit  �������ݐ�r�b�g�}�b�v�������j�b�g
 * @param p_font  �t�H���g
 * @param code    �������ޕ����̕����R�[�h
 * @param heap_id �g�p����q�[�vID
 */
//------------------------------------------------------------------------------------
static void CHAR_UNIT_SetCharCode( CHAR_UNIT* p_unit, GFL_FONT* p_font, STRCODE code, HEAPID heap_id )
{
	int i;

	STRCODE code_arr[2];
	STRBUF* buf = NULL;

	// ���̕�������쐬��, �����o�b�t�@���쐬
	code_arr[0] = code;
	code_arr[1] = GFL_STR_GetEOMCode();
	buf         = GFL_STR_CreateBuffer( 2, heap_id );
	GFL_STR_SetStringCodeOrderLength( buf, code_arr, 2 );

	// �T�C�Y���擾
	p_unit->width  = (u8)PRINTSYS_GetStrWidth( buf, p_font, 0 );
	p_unit->height = (u8)PRINTSYS_GetStrHeight( buf, p_font );

	// �t�H���g�̐F��ݒ�
	GFL_FONTSYS_SetColor( COLOR_NO_LETTER, COLOR_NO_SHADOW, COLOR_NO_BACKGROUND );

	// �r�b�g�}�b�v�ɏ�������
	GFL_BMP_Clear( p_unit->pBmp, 0 );
	PRINTSYS_Print( p_unit->pBmp, 0, 0, buf, p_font );
	BmpOam_ActorBmpTrans( p_unit->pBmpOamAct );

	// ��n��
	GFL_STR_DeleteBuffer( buf );
}

//------------------------------------------------------------------------------------
/**
 * @brief �ړ��J�n���̏����p�����[�^��ݒ肷��
 *
 * @param p_unit  �ݒ肷��ϐ�
 * @param p_param �����p�����[�^
 */
//------------------------------------------------------------------------------------ 
static void CHAR_UNIT_SetMoveParam( CHAR_UNIT* p_unit, const CHAR_UNIT_PARAM* p_param )
{
	const CHAR_UNIT_PARAM* p = p_param;
	float dist_x;
	float dist_y;

	// �p�����[�^���R�s�[
	p_unit->param    = *p_param;
	p_unit->param.ax = 0;
	p_unit->param.ay = 0;
	p_unit->param.tx = 0;
	p_unit->param.ty = 0;

	// �����x���v�Z
	dist_x = p->dx - p->x;
	dist_y = p->dy - p->y;
	if( dist_x != 0 ) p_unit->param.ax = ( ( p->dsx * p->dsx ) - ( p->sx * p->sx ) ) / ( 2 * dist_x );
	if( dist_y != 0 ) p_unit->param.ay = ( ( p->dsy * p->dsy ) - ( p->sy * p->sy ) ) / ( 2 * dist_y );

	// �ړ��񐔂��v�Z
	if( p_unit->param.ax != 0 ) p_unit->param.tx = (int)( ( p_unit->param.dsx - p_unit->param.sx ) / p_unit->param.ax );
	if( p_unit->param.ay != 0 ) p_unit->param.ty = (int)( ( p_unit->param.dsy - p_unit->param.sy ) / p_unit->param.ay );

	// BMPOAM�A�N�^�[�̕\���ʒu�𔽉f
	BmpOam_ActorSetPos( p_unit->pBmpOamAct, p_param->x, p_param->y );
}

//------------------------------------------------------------------------------------ 
/**
 * @brief ������
 *
 * @param p_unit ���������j�b�g
 */
//------------------------------------------------------------------------------------ 
static void CHAR_UNIT_Move( CHAR_UNIT* p_unit )
{
	// ����t���O���Z�b�g����Ă��Ȃ����, �����Ȃ�
	if( p_unit->moving != TRUE ) return;

	// x��������
	if( 0 < p_unit->param.tx )
	{
		// �ړ�
		p_unit->param.x += p_unit->param.sx;

		// ����
		p_unit->param.sx += p_unit->param.ax;

		// �c��ړ��񐔂��f�N�������g
		p_unit->param.tx--;
	}
	// y��������
	if( 0 < p_unit->param.ty )
	{
		// �ړ�
		p_unit->param.y += p_unit->param.sy;

		// ����
		p_unit->param.sy += p_unit->param.ay;

		// �c��ړ��񐔂��f�N�������g
		p_unit->param.ty--;
	}

	// BMPOAM�A�N�^�[�̕\���ʒu�𔽉f
	BmpOam_ActorSetPos( p_unit->pBmpOamAct, p_unit->param.x, p_unit->param.y );

	// �c��ړ��񐔂������Ȃ�����, ����t���O�𗎂Ƃ�
	if( ( p_unit->param.tx <= 0 ) && ( p_unit->param.ty <= 0 ) )
	{
		p_unit->moving = FALSE;
	}
}

//------------------------------------------------------------------------------------ 
/**
 * @brief �ړ������ǂ����𒲂ׂ�
 *
 * @param p_unit ���ׂ郆�j�b�g
 *
 * @return �ړ����Ȃ� TRUE
 */
//------------------------------------------------------------------------------------ 
static BOOL CHAR_UNIT_IsMoving( CHAR_UNIT* p_unit )
{
	return p_unit->moving;
}


//===================================================================================
/**
 * @brief �V�X�e���E���[�N
 */
//===================================================================================
struct _FIELD_PLACE_NAME
{ 
	HEAPID heapID;	// �q�[�vID

	// �n���̕\���Ɏg�p����f�[�^
	FLDMSGBG*    pFldMsgBG;			// BMPWIN�ւ̕����񏑂����݂Ɏg�p����V�X�e��( �O���C���X�^���X���Q�� )
	GFL_MSGDATA* pMsgData;			// ���b�Z�[�W�E�f�[�^( �C���X�^���X��ێ� )

	// BG
	GFL_BMPWIN*   pBmpWin;	        // �r�b�g�}�b�v�E�E�B���h�E
	GFL_BMP_DATA* pBGOriginalBmp;	// �I���W�i���E�r�b�g�}�b�v�E�f�[�^

	// CLACT���\�[�X( �o�^�C���f�b�N�X )
	//u32 hResource_CGR[ CGR_RES_INDEX_MAX ];
	u32 hResource_PLTT[ PLTT_RES_INDEX_MAX ];
	//u32 hResource_CLANM[ CLANM_RES_INDEX_MAX ];

	// �Z���A�N�^�[���j�b�g
	GFL_CLUNIT* pClactUnit[ CLUNIT_INDEX_MAX ];

	// BMPOAM�V�X�e��
	BMPOAM_SYS_PTR pBmpOamSys;

	// �������j�b�g
	CHAR_UNIT charUnit[ MAX_NAME_LENGTH ];

	// ����Ɏg�p����f�[�^
	STATE state;			// ���
	u16	  stateCount;		// ��ԃJ�E���^
	u32	  currentZoneID;	// ���ݕ\�����̒n���ɑΉ�����]�[��ID
	u32   nextZoneID;		// ���ɕ\������n���ɑΉ�����]�[��ID
	u8    launchNum;		// ���ˍςݕ�����

	// �n��
	STRBUF* pNameBuf;		// �\�����̒n��������
	u8      nameLength;		// �\�����̒n��������
};


//===================================================================================
/**
 * @brief �V�X�e���Ɋւ���֐�
 */
//===================================================================================
// BG�̐ݒ���s��
static void SetupBG( FIELD_PLACE_NAME* p_sys );	
static void LoadNullCharacterData();

// ���\�[�X�̓ǂݍ���
static void LoadBGScreenData( FIELD_PLACE_NAME* p_sys, u32 arc_id, u32 data_id );
static void LoadBGCharacterData( FIELD_PLACE_NAME* p_sys, u32 arc_id, u32 data_id );
static void LoadBGPaletteData( FIELD_PLACE_NAME* p_sys, u32 arc_id, u32 data_id ); 
static void LoadClactResource( FIELD_PLACE_NAME* p_sys );

// �Z���A�N�^�[���j�b�g���쐬����
static void CreateClactUnit( FIELD_PLACE_NAME* p_sys );

// �������j�b�g������������
static void CreateCharUnit( FIELD_PLACE_NAME* p_sys );

// �w�肵���]�[��ID�ɑΉ�����n���𕶎����j�b�g�ɏ�������
static void WritePlaceName( FIELD_PLACE_NAME* p_sys, u32 zone_id );

// �������j�b�g�̓����ݒ肷��
static void SetCharUnitAction( FIELD_PLACE_NAME* p_sys );

// �������j�b�g�𔭎˂���
static void LaunchCharUnit( FIELD_PLACE_NAME* p_sys, int unit_index );

// ��Ԃ�ύX����
static void SetState( FIELD_PLACE_NAME* p_sys, STATE next_state );

// �����I�ɃE�B���h�E��ޏo������
static void Cancel( FIELD_PLACE_NAME* p_sys );

// �S�������j�b�g���\���ɐݒ肷��
static void SetAllCharUnitVisibleOff( FIELD_PLACE_NAME* p_sys );

// �r�b�g�}�b�v�E�E�B���h�E��BG�I���W�i���f�[�^�ɕ�������
static void RecoveryBitmapWindow( FIELD_PLACE_NAME* p_sys );

// �\�����̕������j�b�g���r�b�g�}�b�v�E�E�B���h�E�ɏ�������
static void WriteCharUnitToBitmapWindow( FIELD_PLACE_NAME* p_sys );

// �e��Ԏ��̓���
static void Process_HIDE( FIELD_PLACE_NAME* p_sys );
static void Process_FADE_IN( FIELD_PLACE_NAME* p_sys );
static void Process_WAIT_1( FIELD_PLACE_NAME* p_sys );
static void Process_LAUNCH( FIELD_PLACE_NAME* p_sys );
static void Process_WAIT_2( FIELD_PLACE_NAME* p_sys );
static void Process_FADE_OUT( FIELD_PLACE_NAME* p_sys );

// �e��Ԏ��̕`�揈��
static void Draw_HIDE( FIELD_PLACE_NAME* p_sys );
static void Draw_FADE_IN( FIELD_PLACE_NAME* p_sys );
static void Draw_WAIT_1( FIELD_PLACE_NAME* p_sys );
static void Draw_LAUNCH( FIELD_PLACE_NAME* p_sys );
static void Draw_WAIT_2( FIELD_PLACE_NAME* p_sys );
static void Draw_FADE_OUT( FIELD_PLACE_NAME* p_sys );

// �f�o�b�O�o��
static void DebugPrint( FIELD_PLACE_NAME* p_sys );


//===================================================================================
/**
 * @brief ���J�֐��̎���( �V�X�e���̉ғ��Ɋւ���֐� )
 */
//===================================================================================
//------------------------------------------------------------------------------------
/**
 * @brief �n���\���V�X�e�����쐬����
 *
 * @param heap_id      �g�p����q�[�vID
 * @param p_fld_msg_bg �g�p���郁�b�Z�[�W�\���V�X�e��
 *
 * @return �n���\���V�X�e���E���[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------------------------
FIELD_PLACE_NAME* FIELD_PLACE_NAME_Create( HEAPID heap_id, FLDMSGBG* p_fld_msg_bg )
{
	FIELD_PLACE_NAME* p_sys;

	// �V�X�e���E���[�N�쐬
	p_sys = (FIELD_PLACE_NAME*)GFL_HEAP_AllocClearMemory( heap_id, sizeof( FIELD_PLACE_NAME ) );

	// �V�X�e���̐ݒ�
	p_sys->heapID    = heap_id;
	p_sys->pFldMsgBG = p_fld_msg_bg;

	// BG�̎g�p����
	SetupBG( p_sys );

	// �Z���A�N�^�[�p���\�[�X�̓ǂݍ���
	LoadClactResource( p_sys );

	// �Z���A�N�^�[���j�b�g���쐬
	CreateClactUnit( p_sys );

	// BMPOAM�V�X�e���쐬
	p_sys->pBmpOamSys = BmpOam_Init( heap_id, p_sys->pClactUnit[ CLUNIT_INDEX_CHAR_UNIT ] );

	// �������j�b�g���쐬
	CreateCharUnit( p_sys );

	// ���b�Z�[�W�E�f�[�^���쐬
	p_sys->pMsgData = FLDMSGBG_CreateMSGDATA( p_sys->pFldMsgBG, NARC_message_place_name_dat );

	// �����o�b�t�@���쐬
	p_sys->pNameBuf = GFL_STR_CreateBuffer( MAX_NAME_LENGTH + 1, p_sys->heapID );

	// ���̑��̏�����
	p_sys->currentZoneID = INVALID_ZONE_ID;
	p_sys->nextZoneID    = INVALID_ZONE_ID;
	p_sys->nameLength    = 0;

	// ������Ԃ�ݒ�
	SetState( p_sys, STATE_HIDE );

	// �쐬�����V�X�e����Ԃ�
	return p_sys;
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���\���V�X�e���̓��쏈��
 *
 * @param p_sys �������V�X�e��
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Process( FIELD_PLACE_NAME* p_sys )
{
	int i;

	// ��ԃJ�E���^���X�V
	p_sys->stateCount++;

	// ��Ԃɉ���������
	switch( p_sys->state )
	{
		case STATE_HIDE:		Process_HIDE( p_sys );		break;
		case STATE_FADE_IN:		Process_FADE_IN( p_sys );	break;
		case STATE_WAIT_1:		Process_WAIT_1( p_sys );	break;
		case STATE_LAUNCH:		Process_LAUNCH( p_sys );	break;
		case STATE_WAIT_2:		Process_WAIT_2( p_sys );	break;
		case STATE_FADE_OUT:	Process_FADE_OUT( p_sys );	break;
	}

	// �������j�b�g�𓮂���
	for( i=0; i<p_sys->nameLength; i++ )
	{
		CHAR_UNIT_Move( &p_sys->charUnit[i] );
	}
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���\���E�B���h�E�̕`�揈��
 *
 * @param p_sys �`��ΏۃV�X�e��
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Draw( FIELD_PLACE_NAME* p_sys )
{ 
	// ��Ԃɉ������`�揈��
	switch( p_sys->state )
	{
		case STATE_HIDE:		Draw_HIDE( p_sys );		break;
		case STATE_FADE_IN:		Draw_FADE_IN( p_sys );	break;
		case STATE_WAIT_1:		Draw_WAIT_1( p_sys );	break;
		case STATE_LAUNCH:		Draw_LAUNCH( p_sys );	break;
		case STATE_WAIT_2:		Draw_WAIT_2( p_sys );	break;
		case STATE_FADE_OUT:	Draw_FADE_OUT( p_sys );	break;
	}

	// DEBUG: �f�o�b�O�o��
	//DebugPrint( p_sys );
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���\���V�X�e����j������
 *
 * @param p_sys �j������V�X�e��
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Delete( FIELD_PLACE_NAME* p_sys )
{
	int i;

	// �����o�b�t�@�����
	GFL_STR_DeleteBuffer( p_sys->pNameBuf );

	// BMPOAM�V�X�e���̌�n��
	for( i=0; i<MAX_NAME_LENGTH; i++ )
	{ 
		CHAR_UNIT_Finalize( &p_sys->charUnit[i] );
	}
	BmpOam_Exit( p_sys->pBmpOamSys );
	
	// �Z���A�N�^�[���j�b�g�̔j��
	for( i=0; i<CLUNIT_INDEX_MAX; i++ )
	{
		GFL_CLACT_UNIT_Delete( p_sys->pClactUnit[i] );
	}
	
	// �Z���A�N�^�[�p���\�[�X�̔j��
	for( i=0; i<CLANM_RES_INDEX_MAX; i++ )
	{
		//GFL_CLGRP_CELLANIM_Release( p_sys->hResource_CLANM[i] );
	}
	for( i=0; i<PLTT_RES_INDEX_MAX; i++ )
	{
		GFL_CLGRP_PLTT_Release( p_sys->hResource_PLTT[i] );
	}
	for( i=0; i<CGR_RES_INDEX_MAX; i++ )
	{
		//GFL_CLGRP_CGR_Release( p_sys->hResource_CGR[i] );
	}
	
	// �`��p�C���X�^���X�̔j��
	GFL_MSG_Delete( p_sys->pMsgData );

	// �r�b�g�}�b�v�E�E�B���h�E�̔j��
	GFL_BMPWIN_Delete( p_sys->pBmpWin );
	GFL_BMP_Delete( p_sys->pBGOriginalBmp );

	// BGSYS�̌�n��
	GFL_BG_FreeBGControl( BG_FRAME );

	// �V�X�e���E���[�N���
	GFL_HEAP_FreeMemory( p_sys );
} 


//===================================================================================
/**
 * ���J�֐��̎���( �V�X�e���̐���Ɋւ���֐� )
 */
//===================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �]�[���̐؂�ւ���ʒm��, �V�����n����\������
 *
 * @param p_sys        �]�[���؂�ւ���ʒm����V�X�e��
 * @param next_zone_id �V�����]�[��ID
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_ZoneChange( FIELD_PLACE_NAME* p_sys, u32 next_zone_id )
{ 
  // �]�[���̒n���\���t���O�������Ă��Ȃ��ꍇ�͕\�����Ȃ�
  if( ZONEDATA_GetPlaceNameFlag( next_zone_id ) != TRUE ) return;

	// �w�肳�ꂽ�]�[��ID�����ɕ\�����ׂ����̂Ƃ��ċL��
	p_sys->nextZoneID = next_zone_id;

	// �\�����̃E�B���h�E��ޏo������
	Cancel( p_sys ); 
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���E�B���h�E�������I�ɕ\������
 *
 * @param p_sys   �\������V�X�e��
 * @param zone_id �\������ꏊ�̃]�[��ID
 */
//------------------------------------------------------------------------------------
extern void FIELD_PLACE_NAME_Display( FIELD_PLACE_NAME* p_sys, u32 zone_id )
{
  // �����I�ɕ\��
  p_sys->currentZoneID = INVALID_ZONE_ID;
  p_sys->nextZoneID    = zone_id;

	// �\�����̃E�B���h�E��ޏo������
	Cancel( p_sys ); 
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���E�B���h�E�̕\���������I�ɏI������
 *
 * @param p_sys �\�����I������V�X�e��
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Hide( FIELD_PLACE_NAME* p_sys )
{
	SetState( p_sys, STATE_HIDE );
}


//===================================================================================
/**
 * ����J�֐��̎���
 */
//===================================================================================

//-----------------------------------------------------------------------------------
/**
 * @brief BG�̐ݒ���s��
 *
 * @param p_sys �ݒ���s���V�X�e��
 */ 
//-----------------------------------------------------------------------------------
static void SetupBG( FIELD_PLACE_NAME* p_sys )
{ 
	GFL_BG_BGCNT_HEADER bgcnt = 
	{
		0, 0,					    // �����\���ʒu
		0x800,						// �X�N���[���o�b�t�@�T�C�Y
		0,							// �X�N���[���o�b�t�@�I�t�Z�b�g
		GFL_BG_SCRSIZ_256x256,		// �X�N���[���T�C�Y
		GX_BG_COLORMODE_16,			// �J���[���[�h
		GX_BG_SCRBASE_0x0800,		// �X�N���[���x�[�X�u���b�N
		GX_BG_CHARBASE_0x04000,		// �L�����N�^�x�[�X�u���b�N
		GFL_BG_CHRSIZ_256x256,		// �L�����N�^�G���A�T�C�Y
		GX_BG_EXTPLTT_01,			// BG�g���p���b�g�X���b�g�I��
		1,							// �\���v���C�I���e�B�[
		0,							// �G���A�I�[�o�[�t���O
		0,							// DUMMY
		FALSE,						// ���U�C�N�ݒ�
	};

	GFL_BG_SetBGControl( BG_FRAME, &bgcnt, GFL_BG_MODE_TEXT );
	GFL_BG_SetPriority( BG_FRAME, BG_FRAME_PRIORITY );
	GFL_BG_SetVisible( BG_FRAME, VISIBLE_ON ); 
	G2_SetBlendAlpha( ALPHA_PLANE_1, ALPHA_PLANE_2, ALPHA_VALUE_1, ALPHA_VALUE_2 );

	// �L����VRAM�E�X�N���[��VRAM���N���A
	GFL_BG_ClearFrame( BG_FRAME );

	// NULL�L�����N�^�f�[�^���Z�b�g
	LoadNullCharacterData();

	// �r�b�g�}�b�v�E�E�B���h�E���쐬����
	p_sys->pBmpWin = GFL_BMPWIN_Create( 
			BG_FRAME,
			BMPWIN_POS_X_CHAR, BMPWIN_POS_Y_CHAR, BMPWIN_WIDTH_CHAR, BMPWIN_HEIGHT_CHAR,
			BG_PALETTE_NO, GFL_BMP_CHRAREA_GET_F );

	// �p���b�g�E�L�����N�^�E�X�N���[����]��
	LoadBGCharacterData( p_sys, ARCID_PLACE_NAME, NARC_place_name_place_name_back_NCGR );
	LoadBGPaletteData( p_sys, ARCID_PLACE_NAME, NARC_place_name_place_name_back_NCLR );
	//LoadBGScreenData( p_sys, ARCID_PLACE_NAME, NARC_area_win_gra_place_name_back_NSCR );
	
	// �r�b�g�}�b�v�E�E�B���h�E�̃f�[�^��VRAM�ɓ]��
	GFL_BMPWIN_MakeTransWindow( p_sys->pBmpWin );

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
 */
//------------------------------------------------------------------------------------
static void LoadNullCharacterData()
{
	GFL_BG_AllocCharacterArea( BG_FRAME, CHAR_SIZE * CHAR_SIZE / 2, GFL_BG_CHRAREA_GET_F );
	//GFL_BG_FillCharacter( BG_FRAME, 0, 1, 0 );
}

//------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���E�f�[�^��ǂݍ���
 *
 * @param p_sys   �ǂݍ��݂��s���V�X�e��
 * @param arc_id  �A�[�J�C�u�f�[�^�̃C���f�b�N�X
 * @param data_id �A�[�J�C�u���f�[�^�C���f�b�N�X
 */
//------------------------------------------------------------------------------------
static void LoadBGScreenData( FIELD_PLACE_NAME* p_sys, u32 arc_id, u32 data_id )
{
	ARCHANDLE* p_h_arc;
	u32 size;
	void* p_src;
	void* p_tmp;
	NNSG2dScreenData* p_screen;
	p_h_arc = GFL_ARC_OpenDataHandle( arc_id, p_sys->heapID );					// �A�[�J�C�u�f�[�^�n���h���I�[�v��
	size    = GFL_ARC_GetDataSizeByHandle( p_h_arc, data_id );					// �f�[�^�T�C�Y�擾
	p_src   = GFL_HEAP_AllocMemoryLo( p_sys->heapID, size );					// �f�[�^�o�b�t�@�m��
	GFL_ARC_LoadDataByHandle( p_h_arc, data_id, p_src );						// �f�[�^�擾
	NNS_G2dGetUnpackedScreenData( p_src, &p_screen );							// �o�C�i������f�[�^��W�J
	GFL_BG_LoadScreenBuffer( BG_FRAME, p_screen->rawData, p_screen->szByte );	// BGSYS�����o�b�t�@�ɓ]��
	GFL_BG_LoadScreenReq( BG_FRAME );											// VRAM�ɓ]��
	GFL_HEAP_FreeMemory( p_src );												// �f�[�^�o�b�t�@���
	GFL_ARC_CloseDataHandle( p_h_arc );											// �A�[�J�C�u�f�[�^�n���h���N���[�Y
}

//------------------------------------------------------------------------------------
/**
 * @brief �L�����N�^�f�[�^�����[�h����
 *
 * @param p_sys   �ǂݍ��݂��s���V�X�e��
 * @param arc_id  �A�[�J�C�u�f�[�^�̃C���f�b�N�X
 * @param data_id �A�[�J�C�u���f�[�^�C���f�b�N�X
 */
//------------------------------------------------------------------------------------
static void LoadBGCharacterData( FIELD_PLACE_NAME* p_sys, u32 arc_id, u32 data_id )
{
	/*
		u32 size;
		void* pSrc;
		ARCHANDLE* handle;
		NNSG2dCharacterData* pChar;

		handle = GFL_ARC_OpenDataHandle( arc_id, p_sys->heapID );

		size = GFL_ARC_GetDataSizeByHandle( handle, data_id );
		pSrc = GFL_HEAP_AllocMemoryLo( p_sys->heapID, size );
		GFL_ARC_LoadDataByHandle(handle, data_id, (void*)pSrc);

		NNS_G2dGetUnpackedCharacterData(pSrc, &pChar);
		GFL_BG_LoadCharacter(BG_FRAME, pChar->pRawData, pChar->szByte, 0);

		GFL_HEAP_FreeMemory( pSrc );

		GFL_ARC_CloseDataHandle( handle ); 
	*/

	int i;
	u16 sx, sy, dx, dy;
	GFL_BMP_DATA* p_dst_bmp = NULL;

	// �R�s�[���E�R�s�[��r�b�g�}�b�v���擾
	p_sys->pBGOriginalBmp = GFL_BMP_LoadCharacter( arc_id, data_id, FALSE, p_sys->heapID );
	p_dst_bmp             = GFL_BMPWIN_GetBmp( p_sys->pBmpWin );

	// 1�L�������R�s�[����
	sx = 0;
	sy = 0;
	dx = 0;
	dy = 0;
	for( i=0; i<BMPWIN_WIDTH_CHAR * BMPWIN_HEIGHT_CHAR; i++ )
	{
		// �R�s�[
		GFL_BMP_Print( p_sys->pBGOriginalBmp, p_dst_bmp, dx, sy, dx, dy, CHAR_SIZE, CHAR_SIZE, 0 );

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
 * @param p_sys   �ǂݍ��݂��s���V�X�e��
 * @param arc_id  �A�[�J�C�u�f�[�^�̃C���f�b�N�X
 * @param data_id �A�[�J�C�u���f�[�^�C���f�b�N�X
 */
//------------------------------------------------------------------------------------
static void LoadBGPaletteData( FIELD_PLACE_NAME* p_sys, u32 arc_id, u32 data_id )
{
	ARCHANDLE* p_h_arc;
	u32 size;
	void* p_src;
	NNSG2dPaletteData* p_palette;
	p_h_arc = GFL_ARC_OpenDataHandle( arc_id, p_sys->heapID );				// �A�[�J�C�u�f�[�^�n���h���I�[�v��
	size    = GFL_ARC_GetDataSizeByHandle( p_h_arc, data_id );				// �f�[�^�T�C�Y�擾
	p_src   = GFL_HEAP_AllocMemoryLo( p_sys->heapID, size );				// �f�[�^�o�b�t�@�m��
	GFL_ARC_LoadDataByHandle( p_h_arc, data_id, p_src );					// �f�[�^�擾
	NNS_G2dGetUnpackedPaletteData( p_src, &p_palette );						// �o�C�i������f�[�^��W�J
	GFL_BG_LoadPalette( BG_FRAME, p_palette->pRawData, 0x20, BG_PALETTE_NO );	// �p���b�g�f�[�^�]��
	GFL_HEAP_FreeMemory( p_src );											// �f�[�^�o�b�t�@���
	GFL_ARC_CloseDataHandle( p_h_arc );										// �A�[�J�C�u�f�[�^�n���h���N���[�Y
}

//-----------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�Ŏg�p���郊�\�[�X��ǂݍ���
 *
 * @param p_sys �ǂݍ��ݑΏۃV�X�e��
 */
//-----------------------------------------------------------------------------------
static void LoadClactResource( FIELD_PLACE_NAME* p_sys )
{
	ARCHANDLE* p_arc_handle;

	p_arc_handle = GFL_ARC_OpenDataHandle( ARCID_PLACE_NAME, p_sys->heapID );

	p_sys->hResource_PLTT[ PLTT_RES_INDEX_CHAR_UNIT ] = 
		GFL_CLGRP_PLTT_RegisterEx( 
				p_arc_handle, NARC_place_name_place_name_string_NCLR,
				CLSYS_DRAW_MAIN, OBJ_PALETTE_NO * 32, 0, 1, p_sys->heapID );

	GFL_ARC_CloseDataHandle( p_arc_handle );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g���쐬����
 *
 * @param p_sys �쐬�ΏۃV�X�e��
 */
//-----------------------------------------------------------------------------------
static void CreateClactUnit( FIELD_PLACE_NAME* p_sys )
{
	// �Z���A�N�^�[���j�b�g���쐬
	p_sys->pClactUnit[ CLUNIT_INDEX_CHAR_UNIT ] = 
		GFL_CLACT_UNIT_Create( MAX_NAME_LENGTH, BG_FRAME_PRIORITY, p_sys->heapID );

	// �����ݒ�
	GFL_CLACT_UNIT_SetDrawEnable( p_sys->pClactUnit[ CLUNIT_INDEX_CHAR_UNIT ], TRUE );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �������j�b�g������������
 *
 * @param p_sys �쐬�ΏۃV�X�e��
 */
//-----------------------------------------------------------------------------------
static void CreateCharUnit( FIELD_PLACE_NAME* p_sys )
{
	int i;

	for( i=0; i<MAX_NAME_LENGTH; i++ )
	{
		CHAR_UNIT_Initialize( 
				&p_sys->charUnit[i], p_sys->pBmpOamSys, 
				p_sys->hResource_PLTT[ PLTT_RES_INDEX_CHAR_UNIT ], p_sys->heapID );
	} 
}

//-----------------------------------------------------------------------------------
/**
 * @brief �w�肵���]�[��ID�ɑΉ�����n���𕶎����j�b�g�ɏ�������
 *
 * @param p_sys   ����ΏۃV�X�e��
 * @param zone_id �]�[��ID
 */
//----------------------------------------------------------------------------------- 
static void WritePlaceName( FIELD_PLACE_NAME* p_sys, u32 zone_id )
{
	int            i;
	u16            str_id;				// ���b�Z�[�W�f�[�^���̕�����ID
	const STRCODE* p_str_code = NULL;	// ���̕�����
	GFL_FONT*      p_font     = NULL;	// �t�H���g

	// �t�H���g���擾
	p_font = FLDMSGBG_GetFontHandle( p_sys->pFldMsgBG );		

	// �]�[��ID���當������擾����
	str_id = ZONEDATA_GetPlaceNameID( zone_id );					// ���b�Z�[�W�ԍ�������
	if( str_id < 0 | msg_place_name_max <= str_id ) str_id = 0;		// ���b�Z�[�W�ԍ��͈̔̓`�F�b�N
	GFL_MSG_GetString( p_sys->pMsgData,	str_id, p_sys->pNameBuf );	// �n����������擾
	p_str_code = GFL_STR_GetStringCodePointer( p_sys->pNameBuf );	// ���̕�������擾
	p_sys->nameLength = GFL_STR_GetBufferLength( p_sys->pNameBuf );	// ���������擾

	// �������`�F�b�N
	//GF_ASSERT_MSG( ( p_sys->nameLength < MAX_NAME_LENGTH ), "�ő啶�������I�[�o�[���Ă��܂��B" );	// �~�߂Ȃ�
	if( MAX_NAME_LENGTH < p_sys->nameLength )
	{
		OBATA_Printf( "�n���̍ő啶�������I�[�o�[���Ă��܂��B\n" );
		p_sys->nameLength =	MAX_NAME_LENGTH;
	}

	// 1�������ݒ�
	for( i=0; i<p_sys->nameLength; i++ )
	{
		// �r�b�g�}�b�v���쐬����
		CHAR_UNIT_SetCharCode( &p_sys->charUnit[i], p_font, p_str_code[i], p_sys->heapID );

		// �\����Ԃ�ݒ�
		BmpOam_ActorSetDrawEnable( p_sys->charUnit[i].pBmpOamAct, TRUE );
	}
	for( i=p_sys->nameLength; i<MAX_NAME_LENGTH; i++ )
	{
		// �\����Ԃ�ݒ�
		BmpOam_ActorSetDrawEnable( p_sys->charUnit[i].pBmpOamAct, FALSE );
	}

	// TEMP: "�Ȃ��̂΂���" �Ȃ�\�����Ȃ�
	if( str_id == 0 ) 
  {
    OBATA_Printf( "�u�Ȃ��̂΂���v�����o( zone id = %d )\n", zone_id );
    FIELD_PLACE_NAME_Hide( p_sys );
  }
}

//-----------------------------------------------------------------------------------
/**
 * @brief �������j�b�g�̓����ݒ肷��
 *
 * @param �ݒ�ΏۃV�X�e��
 */
//-----------------------------------------------------------------------------------
static void SetCharUnitAction( FIELD_PLACE_NAME* p_sys )
{
	int i;
	int str_width;	// ������̕�
	int str_height;	// ������̍���
	const STRCODE* p_str_code = NULL;
	GFL_FONT*      p_font     = NULL;

	// �t�H���g���擾
	p_font = FLDMSGBG_GetFontHandle( p_sys->pFldMsgBG );		

	// ������̃T�C�Y���擾
	str_width  = PRINTSYS_GetStrWidth( p_sys->pNameBuf, p_font, 0 );
    str_height = PRINTSYS_GetStrHeight( p_sys->pNameBuf, p_font );

	// �e�������j�b�g�̐ݒ�
	{
		CHAR_UNIT_PARAM param;

		// 1�����ڂ̖ڕW�p�����[�^��ݒ�
		param.x   = 256;
		param.y   = Y_CENTER_POS - ( str_height / 2 );
		param.sx  = -20;
		param.sy  =  0;
		//param.dx  = 128 - ( str_width / 2 ) - ( p_sys->nameLength / 2 );	// ��������
		param.dx  = 24;			// ���l��
		param.dy  = param.y;
		param.dsx = 0;
		param.dsy = 0;

		// 1�������ݒ肷��
		for( i=0; i<p_sys->nameLength; i++ )
		{
			// �ڕW�p�����[�^��ݒ�
			CHAR_UNIT_SetMoveParam( &p_sys->charUnit[i], &param );

			// �ڕW�p�����[�^���X�V
			param.dx += p_sys->charUnit[i].width + 1;
		}
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �������j�b�g�𔭎˂���
 *
 * @param p_sys      �V�X�e��
 * @param unit_index ���˂��镶�����j�b�g�ԍ�(�������ڂ�)
 */
//-----------------------------------------------------------------------------------
static void LaunchCharUnit( FIELD_PLACE_NAME* p_sys, int unit_index )
{
	// ����t���O�𗧂Ă�
	p_sys->charUnit[ unit_index ].moving = TRUE;
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��Ԃ�ύX����
 *
 * @param p_sys      ��Ԃ�ύX����V�X�e��
 * @param next_state �ݒ肷����
 */
//-----------------------------------------------------------------------------------
static void SetState( FIELD_PLACE_NAME* p_sys, STATE next_state )
{
	// ��Ԃ�ύX��, ��ԃJ�E���^������������
	p_sys->state      = next_state;
	p_sys->stateCount = 0;

	// �J�ڐ�̏�Ԃɉ�����������
	switch( next_state )
	{
		case STATE_HIDE:	
			GFL_BG_SetVisible( BG_FRAME, VISIBLE_OFF );	// BG���\��
			SetAllCharUnitVisibleOff( p_sys );			// �������j�b�g���\����
			break;
		case STATE_FADE_IN:
			p_sys->currentZoneID = p_sys->nextZoneID;	// �\�����]�[��ID���X�V
			RecoveryBitmapWindow( p_sys );				// �r�b�g�}�b�v�E�B���h�E�𕜋A
			WritePlaceName( p_sys, p_sys->nextZoneID );	// �V�����n������������
			SetCharUnitAction( p_sys );					// �������j�b�g�̓������Z�b�g
			break;
		case STATE_WAIT_1:
			break;
		case STATE_LAUNCH:
			p_sys->launchNum = 0;	// ���˕����������Z�b�g
			break;
		case STATE_WAIT_2:
			break;
		case STATE_FADE_OUT:
			WriteCharUnitToBitmapWindow( p_sys );	// �����_�ł̕�����BG�ɏ�������
			SetAllCharUnitVisibleOff( p_sys );		// �������j�b�g���\����
			break;
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �����I�ɃE�B���h�E��ޏo������
 *
 * @param p_sys ��Ԃ��X�V����V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Cancel( FIELD_PLACE_NAME* p_sys )
{
	STATE next_state;
	int   start_count;
	float passed_rate;
	
	// ���݂̏�Ԃɉ���������
	switch( p_sys->state )
	{
		case STATE_HIDE:	
		case STATE_FADE_OUT:
			return;
		case STATE_WAIT_1:
		case STATE_WAIT_2:
		case STATE_LAUNCH:
			next_state  = STATE_WAIT_2;
			start_count = PROCESS_TIME_WAIT_2;
			break;
		case STATE_FADE_IN:
			next_state = STATE_FADE_OUT;
			// �o�ߍς݃t���[�������Z�o
			// (stateCount����, �\���ʒu���Z�o���Ă��邽��, �����ޏo�������ꍇ, stateCount ��K�؂Ɍv�Z����K�v������)
			passed_rate = p_sys->stateCount / (float)PROCESS_TIME_FADE_IN;			// �o�߂������Ԃ̊������Z�o
			start_count = (int)( (1.0f - passed_rate) * PROCESS_TIME_FADE_OUT );	// �J�ڐ��Ԃ̌o�ߍς݃J�E���g���Z�o
			break;
	}

	// ��Ԃ��X�V
	SetState( p_sys, next_state );
	p_sys->stateCount = start_count;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �S�������j�b�g���\���ɐݒ肷��
 *
 * @param p_sys �ݒ�ΏۃV�X�e��
 */
//-----------------------------------------------------------------------------------
static void SetAllCharUnitVisibleOff( FIELD_PLACE_NAME* p_sys )
{
	int i;
	
	for( i=0; i<MAX_NAME_LENGTH; i++ )
	{
		BmpOam_ActorSetDrawEnable( p_sys->charUnit[i].pBmpOamAct, FALSE );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�E�E�B���h�E��BG�I���W�i���f�[�^�ɕ�������
 *
 * @param p_sys ����ΏۃV�X�e��
 */
//-----------------------------------------------------------------------------------
static void RecoveryBitmapWindow( FIELD_PLACE_NAME* p_sys )
{
	GFL_BMP_DATA* p_src = p_sys->pBGOriginalBmp;
	GFL_BMP_DATA* p_dst = GFL_BMPWIN_GetBmp( p_sys->pBmpWin );

	// �I���W�i��(�������������܂�Ă��Ȃ����)���R�s�[����, VRAM�ɓ]��
	GFL_BMP_Copy( p_src, p_dst );	
	GFL_BMPWIN_TransVramCharacter( p_sys->pBmpWin );	
}

//-----------------------------------------------------------------------------------
/**
 * @brief �\�����̕������j�b�g���r�b�g�}�b�v�E�E�B���h�E�ɏ�������
 *
 * @param p_sys ����ΏۃV�X�e��
 */
//-----------------------------------------------------------------------------------
static void WriteCharUnitToBitmapWindow( FIELD_PLACE_NAME* p_sys )
{
	int i;
	u16 dx, dy;
	CHAR_UNIT* p_char;
	GFL_BMP_DATA* p_src_bmp = NULL;
	GFL_BMP_DATA* p_dst_bmp = GFL_BMPWIN_GetBmp( p_sys->pBmpWin );

	// �\�����̑S�Ă̕������j�b�g���R�s�[����
	for( i=0; i<p_sys->nameLength; i++ )
	{
		p_char = &p_sys->charUnit[i];
		p_src_bmp = p_char->pBmp;
		dx = p_char->param.x - ( BMPWIN_POS_X_CHAR * CHAR_SIZE );
		dy = p_char->param.y - ( BMPWIN_POS_Y_CHAR * CHAR_SIZE );
		GFL_BMP_Print( p_src_bmp, p_dst_bmp, 0, 0, dx, dy, p_char->width, p_char->height, 0 );
	}

	// �X�V���ꂽ�L�����f�[�^��VRAM�ɓ]��
	GFL_BMPWIN_TransVramCharacter( p_sys->pBmpWin );
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��\����Ԏ��̓���
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_HIDE( FIELD_PLACE_NAME* p_sys )
{
	// �]�[���؂�ւ������o������, ���̏�Ԃ�
	if( p_sys->currentZoneID != p_sys->nextZoneID )
	{
		SetState( p_sys, STATE_FADE_IN );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�E�C����Ԏ��̓���
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_FADE_IN( FIELD_PLACE_NAME* p_sys )
{
	// ��莞�Ԃ��o�߂�����, ���̏�Ԃ�
	if( PROCESS_TIME_FADE_IN < p_sys->stateCount )
	{
		SetState( p_sys, STATE_WAIT_1 );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �ҋ@��Ԏ��̓���
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_WAIT_1( FIELD_PLACE_NAME* p_sys )
{
	// ��莞�Ԃ��o�߂�����, ���̏�Ԃ�
	if( PROCESS_TIME_WAIT_1 < p_sys->stateCount )
	{
		SetState( p_sys, STATE_LAUNCH );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �������ˏ�Ԏ��̓���
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_LAUNCH( FIELD_PLACE_NAME* p_sys )
{
	// ���Ԋu���o�߂�����, �����𔭎�
	if( p_sys->stateCount % LAUNCH_INTERVAL == 0 )
	{
		// ����
		LaunchCharUnit( p_sys, p_sys->launchNum );

		// ���˂��������̐����J�E���g
		p_sys->launchNum++;

		// ���ׂĂ̕����𔭎˂�����, ���̏�Ԃ�
		if( p_sys->nameLength <= p_sys->launchNum )
		{
			SetState( p_sys, STATE_WAIT_2 );
		}
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �ҋ@��Ԏ��̓���
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_WAIT_2( FIELD_PLACE_NAME* p_sys )
{
	int i;

	// ��莞�Ԃ��o�߂�����, ���̏�Ԃ�
	if( PROCESS_TIME_WAIT_2 < p_sys->stateCount )
	{
		// �����Ă��镶��������Ȃ�, �ҋ@��Ԃ��ێ�
		for( i=0; i<p_sys->nameLength; i++ )
		{
			if( CHAR_UNIT_IsMoving( &p_sys->charUnit[i] ) == TRUE ) break;
		}

		// ���̏�Ԃ�
		SetState( p_sys, STATE_FADE_OUT );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�E�A�E�g��Ԏ��̓���
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_FADE_OUT( FIELD_PLACE_NAME* p_sys )
{
	// ��莞�Ԃ��o�߂�����, ���̏�Ԃ�
	if( PROCESS_TIME_FADE_OUT < p_sys->stateCount )
	{
		SetState( p_sys, STATE_HIDE );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��\����Ԏ��̕`�揈��
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_HIDE( FIELD_PLACE_NAME* p_sys )
{
	// BG���\��
	GFL_BG_SetVisible( BG_FRAME, VISIBLE_OFF );	
}

//-----------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�E�C����Ԏ��̕`�揈��
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_FADE_IN( FIELD_PLACE_NAME* p_sys )
{
	int val1, val2;
	float rate;

	// BG��\��
	GFL_BG_SetVisible( BG_FRAME, VISIBLE_ON );	
	
	// ���u�����f�B���O�W�����X�V
	rate  = (float)p_sys->stateCount / (float)PROCESS_TIME_FADE_IN;
	val1 = (int)( ALPHA_VALUE_1 * rate );
	val2 = (int)( ALPHA_VALUE_2 + (16 - ALPHA_VALUE_2) * (1.0f - rate) );
	G2_SetBlendAlpha( ALPHA_PLANE_1, ALPHA_PLANE_2, val1, val2 );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �ҋ@��Ԏ��̕`�揈��
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_WAIT_1( FIELD_PLACE_NAME* p_sys )
{
}

//-----------------------------------------------------------------------------------
/**
 * @brief �������ˏ�Ԏ��̕`�揈��
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_LAUNCH( FIELD_PLACE_NAME* p_sys )
{
}

//-----------------------------------------------------------------------------------
/**
 * @brief �ҋ@��Ԏ��̕`�揈��
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_WAIT_2( FIELD_PLACE_NAME* p_sys )
{
}

//-----------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�E�A�E�g��Ԏ��̕`�揈��
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_FADE_OUT( FIELD_PLACE_NAME* p_sys )
{
	int val1, val2;
	float rate;
	
	// ���u�����f�B���O�W�����X�V
	rate  = (float)p_sys->stateCount / (float)PROCESS_TIME_FADE_OUT;
	val1 = (int)( ALPHA_VALUE_1 * (1.0f - rate) );
	val2 = (int)( ALPHA_VALUE_2 + (16 - ALPHA_VALUE_2) * rate );
	G2_SetBlendAlpha( ALPHA_PLANE_1, ALPHA_PLANE_2, val1, val2 );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �f�o�b�O�o��
 *
 * @param p_sys �f�[�^���o�͂���V�X�e��
 */
//-----------------------------------------------------------------------------------
static void DebugPrint( FIELD_PLACE_NAME* p_sys )
{
	char* str;

	switch( p_sys->state )
	{
		case STATE_HIDE:		str = "HIDE";		break;
		case STATE_FADE_IN:		str = "FADE_IN";	break;
		case STATE_WAIT_1:		str = "WAIT_1";		break;
		case STATE_LAUNCH:		str = "LAUNCE";		break;
		case STATE_WAIT_2:		str = "WAIT_2";		break;
		case STATE_FADE_OUT:	str = "FADE_OUT";	break;
	}

	// DEBUG:
	OBATA_Printf( "-------------------------------FIELD_PLACE_NAME\n" );
	OBATA_Printf( "state         = %s\n", str );
	OBATA_Printf( "stateCount    = %d\n", p_sys->stateCount );
	OBATA_Printf( "currentZoneID = %d\n", p_sys->currentZoneID );
	OBATA_Printf( "nextZoneID    = %d\n", p_sys->nextZoneID );
}
