////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   field_place_name.c
 * @brief  �n���\���E�B���h�E
 * @author obata_toshihiro
 * @date   2009.07.08
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "field_place_name.h"
#include "field/zonedata.h"
#include "arc/arc_def.h"
#include "arc/area_win_gra.naix"
#include "arc/message.naix"
#include "msg/msg_place_name.h"


//===================================================================================
/**
 * �萔
 */
//===================================================================================

// 1�L���� = 8�h�b�g
#define CHAR_SIZE (8) 

//-----------
// �\���ݒ�
//-----------
#define PALETTE_NO          (0)					// �p���b�g�ԍ�
#define BG_FRAME            (GFL_BG_FRAME2_M)	// �g�p����BG�t���[��
#define BG_FRAME_PRIORITY   (1)					// BG�t���[���̃v���C�I���e�B
#define	COLOR_NO_LETTER     (1)		            // �����{�̂̃J���[�ԍ�
#define	COLOR_NO_SHADOW     (2)		            // �e�����̃J���[�ԍ�
#define	COLOR_NO_BACKGROUND (0)					// �w�i���̃J���[�ԍ�

//--------------------
// �A�[�J�C�u�E�f�[�^
//--------------------
#define	ARC_DATA_ID_MAX (26) // �A�[�J�C�u���f�[�^ID�̍ő�l

//----------------------
// �O���t�B�b�N�E�T�C�Y
//----------------------
// �E�B���h�E�̃L�����N�^�E�f�[�^
#define WIN_CGR_WIDTH_CHAR (32)							     // ��  (�L�����N�^��)
#define WIN_CGR_HEIGHT_CHAR (4)							     // ����(�L�����N�^��)
#define WIN_CGR_WIDTH_DOT  (WIN_CGR_WIDTH_CHAR * CHAR_SIZE)	 // ��  (�h�b�g��)
#define WIN_CGR_HEIGHT_DOT (WIN_CGR_HEIGHT_CHAR * CHAR_SIZE) // ����(�h�b�g��)

// �r�b�g�}�b�v�E�E�B���h�E
#define BMPWIN_WIDTH_CHAR  (17)							     // ��  (�L�����N�^��)
#define BMPWIN_HEIGHT_CHAR  (4)								 // ����(�L�����N�^��)
#define BMPWIN_WIDTH_DOT   (BMPWIN_WIDTH_CHAR * CHAR_SIZE)   // ��  (�h�b�g��)
#define BMPWIN_HEIGHT_DOT  (BMPWIN_HEIGHT_CHAR * CHAR_SIZE)  // ����(�h�b�g��)

//------------------
// �e��Ԃ̓��쎞��
//------------------
#define PROCESS_TIME_APPEAR    (10)	// �\�����
#define PROCESS_TIME_WAIT      (60) // �ҋ@���
#define PROCESS_TIME_DISAPPEAR (10) // �ޏo���

//-----------------
// �V�X�e���̏��
//-----------------
typedef enum
{
	FIELD_PLACE_NAME_STATE_HIDE,		// ��\��
	FIELD_PLACE_NAME_STATE_APPEAR,		// �o����
	FIELD_PLACE_NAME_STATE_WAIT,		// �ҋ@(�\��)��
	FIELD_PLACE_NAME_STATE_DISAPPEAR,	// �ޏo��
	FIELD_PLACE_NAME_STATE_MAX,
}
FIELD_PLACE_NAME_STATE;



//===================================================================================
/**
 * �V�X�e���E���[�N
 */
//===================================================================================
struct _FIELD_PLACE_NAME
{ 
	HEAPID heapID;	// �q�[�vID

	// �n���̕\���Ɏg�p����f�[�^
	FLDMSGBG*    pFldMsgBG;			// BMPWIN�ւ̕����񏑂����݂Ɏg�p����V�X�e��( �O���C���X�^���X���Q�� )
	GFL_MSGDATA* pMsgData;			// ���b�Z�[�W�E�f�[�^( �C���X�^���X��ێ� )
	GFL_BMPWIN*  pBmpWin;			// �r�b�g�}�b�v�E�E�B���h�E( �C���X�^���X��ێ� )
	FLDMSGPRINT* pFldMsgPrint;		// BMPWIN - MSGDATA �̊֘A�t��( �C���X�^���X��ێ� )
	
	// ����Ɏg�p����f�[�^
	FIELD_PLACE_NAME_STATE state;			// ���
	u16	                   stateCount;		// ��ԃJ�E���^
	u32	                   currentZoneID;	// ���ݕ\�����̒n���ɑΉ�����]�[��ID
	u32                    nextZoneID;		// ���ɕ\������n���ɑΉ�����]�[��ID
};


//===================================================================================
/**
 * ����J�֐��̃v���g�^�C�v�錾
 */
//===================================================================================
// �������֐�
static void SetupBG( FIELD_PLACE_NAME* p_sys );	// BG�̐ݒ���s��
// ���\�[�X�̓ǂݍ���
static void LoadPaletteData( FIELD_PLACE_NAME* p_sys, u32 arc_id, u32 data_id );
static void LoadCharacterData( FIELD_PLACE_NAME* p_sys, u32 arc_id, u32 data_id );
static void LoadScreenData( FIELD_PLACE_NAME* p_sys, u32 arc_id, u32 data_id );
// �w�肵���]�[��ID�ɑΉ�����r�b�g�}�b�v�E�B���h�E���쐬����
static void CreateBitmapWindow( FIELD_PLACE_NAME* p_sys, u32 zone_id );
static void WriteStringToWindow( FIELD_PLACE_NAME* p_sys, u32 zone_id );
// ��Ԃ�ύX����
static void SetState( FIELD_PLACE_NAME* p_sys, FIELD_PLACE_NAME_STATE next_state );
// �e��Ԏ��̓���
static void Process_HIDE( FIELD_PLACE_NAME* p_sys );
static void Process_APPEAR( FIELD_PLACE_NAME* p_sys );
static void Process_WAIT( FIELD_PLACE_NAME* p_sys );
static void Process_DISAPPEAR( FIELD_PLACE_NAME* p_sys );


//===================================================================================
/**
 * ���J�֐��̎���( �V�X�e���̉ғ��Ɋւ���֐� )
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

	// �r�b�g�}�b�v�E�E�B���h�E�쐬
	p_sys->pBmpWin = GFL_BMPWIN_Create( BG_FRAME, 0, 0, BMPWIN_WIDTH_CHAR, BMPWIN_HEIGHT_CHAR, 0, GFL_BMP_CHRAREA_GET_B );

	// ���b�Z�[�W�E�f�[�^���쐬
	p_sys->pMsgData = FLDMSGBG_CreateMSGDATA( p_sys->pFldMsgBG, NARC_message_place_name_dat );

	// ���b�Z�[�W�`������쐬
	p_sys->pFldMsgPrint = FLDMSGPRINT_SetupPrint( p_sys->pFldMsgBG, p_sys->pMsgData, p_sys->pBmpWin );


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
	// ��ԃJ�E���^���X�V
	p_sys->stateCount++;

	// ��Ԃɉ�����������s��
	switch( p_sys->state )
	{
		case FIELD_PLACE_NAME_STATE_HIDE:		Process_HIDE( p_sys );		break;
		case FIELD_PLACE_NAME_STATE_APPEAR:		Process_APPEAR( p_sys );	break;
		case FIELD_PLACE_NAME_STATE_WAIT:		Process_WAIT( p_sys );		break;
		case FIELD_PLACE_NAME_STATE_DISAPPEAR:	Process_DISAPPEAR( p_sys );	break;
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
	int scroll_value = 0;

	// �X�N���[���l���v�Z
	switch( p_sys->state )
	{
		case FIELD_PLACE_NAME_STATE_HIDE:	
			scroll_value = BMPWIN_HEIGHT_DOT;
			break;
		case  FIELD_PLACE_NAME_STATE_APPEAR:	
			scroll_value = BMPWIN_HEIGHT_DOT - (int)( BMPWIN_HEIGHT_DOT * ( ( (float)( p_sys->stateCount ) ) / PROCESS_TIME_APPEAR ) );
			break;
		case FIELD_PLACE_NAME_STATE_WAIT:
			scroll_value = 0;	
			break;
		case FIELD_PLACE_NAME_STATE_DISAPPEAR:	
			scroll_value = (int)( BMPWIN_HEIGHT_DOT * ( ( (float)( p_sys->stateCount ) ) / PROCESS_TIME_DISAPPEAR ) );	
			break;
	}
	
	// �X�N���[���l��ݒ�
	GFL_BG_SetScroll( BG_FRAME, GFL_BG_SCROLL_Y_SET, scroll_value );

	// �f�o�b�O�o��
	/*
	{
		char* state;
		switch( p_sys->state )
		{
			case FIELD_PLACE_NAME_STATE_HIDE:		state = "HIDE";	break;
			case FIELD_PLACE_NAME_STATE_APPEAR:		state = "APPEAR";	break;
			case FIELD_PLACE_NAME_STATE_WAIT:		state = "WAIT";	break;
			case FIELD_PLACE_NAME_STATE_DISAPPEAR:	state = "DISAPPER";	break;
			default:								state = "error"; break;
		}
		OBATA_Printf( "state = %s,  stateCount = %d\n", state, p_sys->stateCount );
	}
	*/
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
	// �`��p�C���X�^���X�̔j��
	FLDMSGPRINT_Delete( p_sys->pFldMsgPrint );
	GFL_MSG_Delete( p_sys->pMsgData );
	GFL_BMPWIN_Delete( p_sys->pBmpWin );

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
	// �w�肳�ꂽ�]�[��ID�����ɕ\�����ׂ����̂Ƃ��ċL��
	p_sys->nextZoneID = next_zone_id;

	// �o���� or �ҋ@�� �Ȃ�, �����I�ɑޏo������
	if( ( p_sys->state == FIELD_PLACE_NAME_STATE_APPEAR ) ||
		( p_sys->state == FIELD_PLACE_NAME_STATE_WAIT ) )
	{
		SetState( p_sys, FIELD_PLACE_NAME_STATE_DISAPPEAR ); 
	}
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
	SetState( p_sys, FIELD_PLACE_NAME_STATE_HIDE );
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
	GFL_BMP_DATA* p_win_bmp;
	GFL_BMP_DATA* p_name_bmp;
	GFL_BMP_DATA* p_combine;
	void* p_char_adrs;

	GFL_BG_BGCNT_HEADER bgcnt = 
	{
		0, 0,				    // �����\���ʒu
		0x800,					// �X�N���[���o�b�t�@�T�C�Y
		0,						// �X�N���[���o�b�t�@�I�t�Z�b�g
		GFL_BG_SCRSIZ_256x256,	// �X�N���[���T�C�Y
		GX_BG_COLORMODE_16,		// �J���[���[�h
		GX_BG_SCRBASE_0x0800,	// �X�N���[���x�[�X�u���b�N
		GX_BG_CHARBASE_0x04000,	// �L�����N�^�x�[�X�u���b�N
		GFL_BG_CHRSIZ_256x256,	// �L�����N�^�G���A�T�C�Y
		GX_BG_EXTPLTT_01,		// BG�g���p���b�g�X���b�g�I��
		1,						// �\���v���C�I���e�B�[
		0,						// �G���A�I�[�o�[�t���O
		0,						// DUMMY
		FALSE,					// ���U�C�N�ݒ�
	};

	GFL_BG_SetBGControl( BG_FRAME, &bgcnt, GFL_BG_MODE_TEXT );
	GFL_BG_SetPriority( BG_FRAME, BG_FRAME_PRIORITY );
	GFL_BG_SetVisible( BG_FRAME, VISIBLE_ON ); 
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
static void LoadPaletteData( FIELD_PLACE_NAME* p_sys, u32 arc_id, u32 data_id )
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
	GFL_BG_LoadPalette( BG_FRAME, p_palette->pRawData, 0x20, PALETTE_NO );	// �p���b�g�f�[�^�]��
	GFL_HEAP_FreeMemory( p_src );											// �f�[�^�o�b�t�@���
	GFL_ARC_CloseDataHandle( p_h_arc );										// �A�[�J�C�u�f�[�^�n���h���N���[�Y
}

//------------------------------------------------------------------------------------
/**
 * @brief �L�����N�^�E�f�[�^���r�b�g�}�b�v�E�E�B���h�E�̓����o�b�t�@�ɓǂݍ���
 *
 * @param p_sys   �ǂݍ��݂��s���V�X�e��
 * @param arc_id  �A�[�J�C�u�f�[�^�̃C���f�b�N�X
 * @param data_id �A�[�J�C�u���f�[�^�C���f�b�N�X
 */
//------------------------------------------------------------------------------------
static void LoadCharacterData( FIELD_PLACE_NAME* p_sys, u32 arc_id, u32 data_id )
{
	int i;
	GFL_BMP_DATA* p_src;	// �]�����r�b�g�}�b�v
	GFL_BMP_DATA* p_bmp;	// �]����r�b�g�}�b�v
	int x_src, y_src;		// �]�������W
	int x_dest, y_dest;		// �]������W
	int copy_char_num;		// �]���L�����N�^��

	// �]����r�b�g�}�b�v = �r�b�g�}�b�v�E�B���h�E
	p_bmp = GFL_BMPWIN_GetBmp( p_sys->pBmpWin );

	// �]�����r�b�g�}�b�v���쐬( �A�[�J�C�u����L�����f�[�^���擾 )
	p_src = GFL_BMP_LoadCharacter( arc_id, data_id, 0, p_sys->heapID );

	// �ǂݍ��񂾃L�����f�[�^��, �r�b�g�}�b�v�E�B���h�E�ɓ]��
	x_src  = CHAR_SIZE;
	y_src  = 0;
	x_dest = 0;
	y_dest = 0;
	copy_char_num = (WIN_CGR_WIDTH_CHAR * WIN_CGR_HEIGHT_CHAR) - 1;		// �擪�L�����ȊO���R�s�[����
	for( i=0; i<copy_char_num; i++ )
	{
		// 1�L�������R�s�[
		GFL_BMP_Print( p_src, p_bmp, x_src, y_src, x_dest, y_dest, CHAR_SIZE, CHAR_SIZE, 0 );

		// �R�s�[�����W���X�V
		x_src += CHAR_SIZE;
		if( WIN_CGR_WIDTH_DOT <= x_src  )
		{
			x_src  = 0;
			y_src += CHAR_SIZE;
		}
		// �R�s�[����W���X�V
		x_dest += CHAR_SIZE;
		if( BMPWIN_WIDTH_DOT <= x_dest )
		{
			x_dest  = 0;
			y_dest += CHAR_SIZE;
		}
	}
	
	// �ǂݍ��񂾃L�����f�[�^��j��
	GFL_BMP_Delete( p_src ); 
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
static void LoadScreenData( FIELD_PLACE_NAME* p_sys, u32 arc_id, u32 data_id )
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

//-----------------------------------------------------------------------------------
/**
 * @brief �w�肵���]�[��ID�ɑΉ�����r�b�g�}�b�v�E�B���h�E���쐬����
 *
 * @param p_sys   ����ΏۃV�X�e��
 * @param zone_id �]�[��ID
 */
//----------------------------------------------------------------------------------- 
static void CreateBitmapWindow( FIELD_PLACE_NAME* p_sys, u32 zone_id )
{ 
	u16 win_id;

	// �L�����N�^�E�p���b�g�̏�������
	win_id = ZONEDATA_GetPlaceNameWinID( zone_id );			// �E�B���h�EID���擾
	if( win_id < 0 | ARC_DATA_ID_MAX < win_id ) win_id = 0; // �E�B���h�EID�͈̔̓`�F�b�N
	LoadCharacterData( p_sys, ARCID_PLACE_NAME, win_id );	// �r�b�g�}�b�v�E�E�B���h�E�̓����o�b�t�@�ɃL�����N�^�E�f�[�^��]��
	LoadPaletteData( p_sys, ARCID_PLACE_NAME, win_id + 1 ); // �p���b�g�]��

	// �r�b�g�}�b�v�E�E�B���h�E�̓����o�b�t�@�ɕ�������������
	WriteStringToWindow( p_sys, zone_id );

	// �r�b�g�}�b�v�E�E�B���h�E�̃f�[�^��VRAM�ɓ]��
	GFL_BMPWIN_TransVramCharacter( p_sys->pBmpWin );	// VRAM�ɃL�����f�[�^��]��	
	GFL_BMPWIN_MakeScreen( p_sys->pBmpWin );			// BGSYS�̓����o�b�t�@���X�V
	GFL_BG_LoadScreenReq( BG_FRAME );					// BGSYS�̓����o�b�t�@��VRAM�ɓ]��


	// �f�o�b�O�o��
	OBATA_Printf( "win_id = %d\n", (int)win_id );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�E�E�B���h�E�ɑ΂�, �n���̏������݂��s��
 *
 * @param p_sys   ����ΏۃV�X�e��
 * @param zone_id �]�[��ID
 */
//----------------------------------------------------------------------------------- 
static void WriteStringToWindow( FIELD_PLACE_NAME* p_sys, u32 zone_id )
{
	u16 str_id;
	int str_width;
	int str_x;
	STRBUF* p_str_buf = NULL;
	GFL_FONT* p_font = NULL;

	str_id    = ZONEDATA_GetPlaceNameID( zone_id );				// ���b�Z�[�W�ԍ�������
	if( str_id < 0 | msg_place_name_max <= str_id ) str_id = 0;	// ���b�Z�[�W�ԍ��͈̔̓`�F�b�N
	p_str_buf = GFL_STR_CreateBuffer( 256, p_sys->heapID );		// �o�b�t�@���쐬
	GFL_MSG_GetString( p_sys->pMsgData,	str_id, p_str_buf );	// �n����������擾
	p_font    = FLDMSGBG_GetFontHandle( p_sys->pFldMsgBG );		// �t�H���g�����擾
	str_width = PRINTSYS_GetStrWidth( p_str_buf, p_font, 0 );	// ������̕����擾( �����Ԋu�̓f�t�H���g��0 )
	str_x     = ( BMPWIN_WIDTH_DOT / 2 ) - ( str_width / 2 );	// ������̍��[x���W���Z�o

	// ��������
	FLDMSGPRINT_PrintColor( 
			p_sys->pFldMsgPrint, str_x, 1*CHAR_SIZE+2, str_id, 
			PRINTSYS_LSB_Make( COLOR_NO_LETTER, COLOR_NO_SHADOW, COLOR_NO_BACKGROUND ) );

	// ��n��
	GFL_STR_DeleteBuffer( p_str_buf );

	// �f�o�b�O�o��
	OBATA_Printf( "BMPWIN_WIDTH_DOT / 2 = %d\n", BMPWIN_WIDTH_DOT / 2 );
	OBATA_Printf( "str_width / 2 = %d\n", str_width / 2 );
	OBATA_Printf( "str_x = %d\n", str_x );
	OBATA_Printf( "str_id = %d\n", (int)str_id );
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��Ԃ�ύX����
 *
 * @param p_sys      ��Ԃ�ύX����V�X�e��
 * @param next_state �ݒ肷����
 */
//-----------------------------------------------------------------------------------
static void SetState( FIELD_PLACE_NAME* p_sys, FIELD_PLACE_NAME_STATE next_state )
{
	u16 start_count = 0;

	// �o����� -> �ޏo��ԂɈڍs����ꍇ, ��ԃJ�E���g��K�؂Ɍv�Z����K�v������
	// ( stateCount ���� �E�B���h�E�̕\���ʒu���v�Z���Ă��邽�� )
	if( ( p_sys->state == FIELD_PLACE_NAME_STATE_APPEAR ) &&
		( next_state   == FIELD_PLACE_NAME_STATE_DISAPPEAR ) )
	{
		float rate  = p_sys->stateCount / (float)PROCESS_TIME_APPEAR;	// �o�߂������Ԃ̊������Z�o
		start_count = (int)( (1.0f - rate) * PROCESS_TIME_DISAPPEAR );	// �ޏo��ԊJ�n���̃J�E���g���Z�o
	}
	
	// ��Ԃ�ύX��, ��ԃJ�E���^������������
	p_sys->state      = next_state;
	p_sys->stateCount = start_count;

	// �o����ԂɈڍs���鎞��, �\�����̃]�[��ID���X�V����
	if( next_state == FIELD_PLACE_NAME_STATE_APPEAR )
	{
		p_sys->currentZoneID = p_sys->nextZoneID; 
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��\�����( FIELD_PLACE_NAME_STATE_HIDE )���̓���
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_HIDE( FIELD_PLACE_NAME* p_sys )
{
	// �]�[���̐؂�ւ��ʒm���󂯂Ă�����, �o����ԂɈڍs����
	if( p_sys->currentZoneID != p_sys->nextZoneID )
	{
		// �o����ԂɈڍs��,
		SetState( p_sys, FIELD_PLACE_NAME_STATE_APPEAR );

		// �r�b�g�}�b�v�E�B���h�E���\�z
		CreateBitmapWindow( p_sys, p_sys->currentZoneID );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �o�����( FIELD_PLACE_NAME_STATE_APPEAR )���̓���
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_APPEAR( FIELD_PLACE_NAME* p_sys )
{
	// ��莞�Ԃ��o�߂�����, �ҋ@��Ԃֈڍs����
	if( PROCESS_TIME_APPEAR < p_sys->stateCount )
	{
		SetState( p_sys, FIELD_PLACE_NAME_STATE_WAIT );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �ҋ@( FIELD_PLACE_NAME_STATE_WAIT )���̓���
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_WAIT( FIELD_PLACE_NAME* p_sys )
{
	// ��莞�Ԃ��o�߂�����, �ޏo��Ԃֈڍs����
	if( PROCESS_TIME_WAIT < p_sys->stateCount )
	{
		SetState( p_sys, FIELD_PLACE_NAME_STATE_DISAPPEAR );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �ޏo���( FIELD_PLACE_NAME_STATE_DISAPPEAR )���̓���
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_DISAPPEAR( FIELD_PLACE_NAME* p_sys )
{
	// ��莞�Ԃ��o�߂�����, ��\����Ԃֈڍs����
	if( PROCESS_TIME_DISAPPEAR < p_sys->stateCount )
	{
		SetState( p_sys, FIELD_PLACE_NAME_STATE_HIDE );
	}
}
