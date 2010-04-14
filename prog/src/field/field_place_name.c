//////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   field_place_name.c
 * @brief  �n���\���E�B���h�E
 * @author obata_toshihiro
 * @date   2009.07   
 */
//////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "field_place_name.h"
#include "place_name_letter.h"

#include "system/bmp_oam.h"
#include "print/wordset.h"
#include "field/zonedata.h"
#include "field/field_status.h"
#include "field/intrude_common.h"
#include "field/intrude_comm.h"
#include "field_comm/intrude_work.h"
#include "field_comm/intrude_main.h"

#include "pm_define.h"
#include "field_oam_pal.h"  // for FLDOAM_PALNO_PLACENAME

#include "arc/arc_def.h"
#include "arc/place_name.naix"
#include "arc/message.naix"
#include "msg/msg_place_name.h"

#include "../../../resource/fldmapdata/zonetable/zone_id.h"


//===================================================================================
// �� �萔�E�}�N��
//=================================================================================== 
//#define DEBUG_PRINT_ON // �f�o�b�O�o�̓X�C�b�`
#define DEBUG_PRINT_TARGET (3) // �f�o�b�O�o�͐�
#define MAX_STATE_COUNT ( 0xffff ) // ��ԃJ�E���^�ő�l

// �ő啶����
#define MAX_NAME_LENGTH (BUFLEN_PLACE_NAME - BUFLEN_EOM_SIZE)

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

#define	COLOR_NO_LETTER     (1) // �����{�̂̃J���[�ԍ�
#define	COLOR_NO_SHADOW     (2) // �e�����̃J���[�ԍ�
#define	COLOR_NO_BACKGROUND (0) // �w�i���̃J���[�ԍ�

#define ALPHA_PLANE_1 (GX_BLEND_PLANEMASK_BG3)	// ���u�����h�̑�1�Ώۖ�
#define ALPHA_PLANE_2 (GX_BLEND_PLANEMASK_BG0)	// ���u�����h�̑�2�Ώۖ�
#define ALPHA_VALUE_1 (16)						// ��1�Ώۖʂ̃��u�����f�B���O�W��
#define ALPHA_VALUE_2 ( 4)						// ��1�Ώۖʂ̃��u�����f�B���O�W��

#define Y_CENTER_POS ( CHAR_SIZE * 2 - 1 )		// �w�i�т̒��Sy���W

#define LAUNCH_INTERVAL (3)	// �������ˊԊu[�P�ʁF�t���[��]

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

// �p���b�g���\�[�X
typedef enum {
	PLTT_RES_CHAR_UNIT,	// �������j�b�g�Ŏg�p����p���b�g
	PLTT_RES_NUM        // ����
} PLTT_RES_INDEX;

// �Z���A�N�^�[�E���j�b�g
typedef enum {
	CLUNIT_CHAR_UNIT, // �������j�b�g
	CLUNIT_NUM,       // ����
} CLUNIT_INDEX;

// �e��Ԃ̓���ݒ�
#define PROCESS_TIME_FADEIN  (10)
#define PROCESS_TIME_WAIT_LAUNCH   (10)
#define PROCESS_TIME_WAIT_FADEOUT   (30)
#define PROCESS_TIME_FADEOUT (20)

// �V�X�e���̏��
typedef enum {
	SYSTEM_STATE_HIDE,			   // ��\��
  SYSTEM_STATE_SETUP,        // ����
	SYSTEM_STATE_FADEIN,	     // �t�F�[�h�E�C��
	SYSTEM_STATE_WAIT_LAUNCH,	 // ���ˑ҂�
	SYSTEM_STATE_LAUNCH,		   // ��������
	SYSTEM_STATE_WAIT_FADEOUT, // �t�F�[�h�A�E�g�҂�
	SYSTEM_STATE_FADEOUT,	     // �t�F�[�h�E�A�E�g
	SYSTEM_STATE_NUM,          // ����
  SYSTEM_STATE_MAX = SYSTEM_STATE_NUM - 1
} SYSTEM_STATE;


static const PN_LETTER_SETUP_PARAM LetterSetupParam = 
{
  256,                               // ���W
  Y_CENTER_POS - ( 13 / 2 ), // 
  -20,                               // ���x
  0,                                 // 
  24,                                // �ڕW�ʒu
  Y_CENTER_POS - ( 13 / 2 ), // 
  0,                                 // �ڕW�ʒu�ł̑��x
  0,                                 // 

  0, // �t�H���g
  0, // ����
};

GFL_BG_BGCNT_HEADER BGCntHeader = 
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



//===================================================================================
// ���n���\���V�X�e��
//===================================================================================
struct _FIELD_PLACE_NAME { 

  GAMESYS_WORK* gameSystem;
  GAMEDATA*     gameData;
  WORDSET*      wordset;
	HEAPID        heapID;
  ARCHANDLE*    arcHandle;
	GFL_MSGDATA*  msg;
  GFL_FONT*     font;

	// �n��
	STRBUF* nameBuf; // �\�����̒n��������
	u8      nameLen; // �\�����̒n��������

	// ��
	GFL_BMPWIN*   bmpWin;	     // �r�b�g�}�b�v�E�E�B���h�E
	GFL_BMP_DATA* bmpOrg;	     // �������������܂�Ă��Ȃ����
  u32           nullCharPos; // NULL�L�����N�^�̃L����No.

	// OBJ
	u32            resPltt[PLTT_RES_NUM];  // �p���b�g���\�[�X
	BMPOAM_SYS_PTR bmpOamSys;              // BMPOAM�V�X�e��
	GFL_CLUNIT*    clunit[CLUNIT_NUM];     // �Z���A�N�^�[���j�b�g

	// ����Ɏg�p����f�[�^
	SYSTEM_STATE state;	// �V�X�e�����
  u8  stateSeq;       // ��ԓ��V�[�P���X
	u16	stateCount;		  // ��ԃJ�E���^

  u16  lastZoneID; // �Ō�ɕ\�������]�[��ID
  u16  dispZoneID; // ���ɕ\������]�[��ID
  BOOL dispZoneSetFlag; // ���ɕ\������]�[��ID���Z�b�g���ꂽ���ǂ���
  BOOL forceDispFlag; // �����\���t���O

  PN_LETTER* letters[ MAX_NAME_LENGTH ];
  u8 setupLetterNum; // �Z�b�g�A�b�v���������������I�u�W�F�N�g�̐�
	u8 launchLetterNum;// ���ˍςݕ�����
  u8 writeLetterNum; // BG �ɏ������񂾕�����
};


//===================================================================================
// ���V�X�e���Ɋւ���֐�
//===================================================================================
// BG
static void SetupBG( FIELD_PLACE_NAME* system );
static void CleanBG( FIELD_PLACE_NAME* system );
static void ResetBG( FIELD_PLACE_NAME* system );	
static void LoadBGPalette( FIELD_PLACE_NAME* system ); // BG �p���b�g��ǂݍ���
static void AllocBGNullCharArea( FIELD_PLACE_NAME* system ); // �󔒃L�����G���A���m�ۂ���
static void FreeBGNullCharaArea( FIELD_PLACE_NAME* system ); // �󔒃L�����G���A���������
static void RecoverBlankBand( FIELD_PLACE_NAME* system ); // �т𕶎����������܂�Ă��Ȃ���Ԃɕ��A����
// OBJ
static void LoadOBJResource( FIELD_PLACE_NAME* system ); // OBJ ���\�[�X��ǂݍ���
static void UnloadOBJResource( FIELD_PLACE_NAME* system ); // OBJ ���\�[�X���������
static void CreateBmpOamSystem( FIELD_PLACE_NAME* system ); // BMPOAM �V�X�e���𐶐�����
static void DeleteBmpOamSystem( FIELD_PLACE_NAME* system ); // BMPOAM �V�X�e����j������
static void CreateClactUnit( FIELD_PLACE_NAME* system ); // �Z���A�N�^�[���j�b�g�𐶐�����
static void DeleteClactUnit( FIELD_PLACE_NAME* system ); // �Z���A�N�^�[���j�b�g��j������
// �����I�u�W�F�N�g
static void SetupLetter( FIELD_PLACE_NAME* system ); // �����I�u�W�F�N�g���Z�b�g�A�b�v����
static BOOL CheckLetterSetupFinished( const FIELD_PLACE_NAME* system ); // �����I�u�W�F�N�g�̃Z�b�g�A�b�v�������������ǂ������`�F�b�N����
static void HideLetters( FIELD_PLACE_NAME* system ); // �����I�u�W�F�N�g���\���ɂ���
static void LaunchLetter( FIELD_PLACE_NAME* system ); // �����I�u�W�F�N�g�𔭎˂���
static void MoveAllCharUnit( FIELD_PLACE_NAME* system ); 
// BG, �������j�b�g�ւ̏�������
static void WriteCharUnitIntoBitmapWindow( FIELD_PLACE_NAME* system );
static BOOL CheckLetterWriteToBitmapFinished( const FIELD_PLACE_NAME* system );
// �n���̍X�V
static void SetForceDispFlag( FIELD_PLACE_NAME* system );
static void ResetForceDispFlag( FIELD_PLACE_NAME* system );
static BOOL CheckForceDispFlag( const FIELD_PLACE_NAME* system );
static BOOL CheckDispStart( const FIELD_PLACE_NAME* system );
static BOOL CheckPlaceNameIDChange( const FIELD_PLACE_NAME* system );
static void UpdatePlaceName( FIELD_PLACE_NAME* system ); 
// ��Ԃ̕ύX
static void ChangeState( FIELD_PLACE_NAME* system, SYSTEM_STATE next_state );
static void Cancel( FIELD_PLACE_NAME* system ); 
// �e��Ԏ��̓���
static void Process_HIDE( FIELD_PLACE_NAME* system );
static void Process_SETUP( FIELD_PLACE_NAME* system );
static void Process_FADEIN( FIELD_PLACE_NAME* system );
static void Process_WAIT_LAUNCH( FIELD_PLACE_NAME* system );
static void Process_LAUNCH( FIELD_PLACE_NAME* system );
static void Process_WAIT_FADEOUT( FIELD_PLACE_NAME* system );
static void Process_FADEOUT( FIELD_PLACE_NAME* system ); 
// �e��Ԏ��̕`�揈��
static void Draw_HIDE( const FIELD_PLACE_NAME* system );
static void Draw_SETUP( const FIELD_PLACE_NAME* system );
static void Draw_FADEIN( const FIELD_PLACE_NAME* system );
static void Draw_WAIT_LAUNCH( const FIELD_PLACE_NAME* system );
static void Draw_LAUNCH( const FIELD_PLACE_NAME* system );
static void Draw_WAIT_FADEOUT( const FIELD_PLACE_NAME* system );
static void Draw_FADEOUT( const FIELD_PLACE_NAME* system );


// �V�X�e��
static FIELD_PLACE_NAME* CreatePNSystem( HEAPID heapID ); // �n���\���V�X�e���𐶐�����
static void InitPNSystem( FIELD_PLACE_NAME* system, GAMESYS_WORK* gameSystem ); // �n���\���V�X�e��������������
static void DeletePNSystem( FIELD_PLACE_NAME* system ); // �n���\���V�X�e����j������
// ����
static void CreateWordset( FIELD_PLACE_NAME* system ); // ���[�h�Z�b�g�𐶐�����
static void CreateMessageData( FIELD_PLACE_NAME* system ); // ���b�Z�[�W�f�[�^�𐶐�����
static void CreatePlaceNameBuffer( FIELD_PLACE_NAME* system ); // �n���o�b�t�@�𐶐�����
static void CreateBandBlankBitmap( FIELD_PLACE_NAME* system ); // �����т̃r�b�g�}�b�v�𐶐�����
static void CreateBandBitmapWindow( FIELD_PLACE_NAME* system ); // �т̃r�b�g�}�b�v�E�B���h�E�𐶐�����
static void CreateLetters( FIELD_PLACE_NAME* system ); // �����I�u�W�F�N�g�𐶐�����
static void OpenDataHandle( FIELD_PLACE_NAME* system ); // �A�[�J�C�u�f�[�^�n���h�����J��
// �j��
static void DeleteWordset( FIELD_PLACE_NAME* system ); // ���[�h�Z�b�g��j������
static void DeleteMessageData( FIELD_PLACE_NAME* system ); // ���b�Z�[�W�f�[�^��j������
static void DeletePlaceNameBuffer( FIELD_PLACE_NAME* system ); // �n���o�b�t�@��j������
static void DeleteBandBlankBitmap( FIELD_PLACE_NAME* system ); // �����т̃r�b�g�}�b�v��j������
static void DeleteBandBitmapWindow( FIELD_PLACE_NAME* system ); // �т̃r�b�g�}�b�v�E�B���h�E��j������
static void DeleteLetters( FIELD_PLACE_NAME* system ); // �����I�u�W�F�N�g��j������
static void CloseDataHandle( FIELD_PLACE_NAME* system ); // �A�[�J�C�u�f�[�^�n���h�������
// �e��Q�b�^�[�֐�
static HEAPID GetHeapID( const FIELD_PLACE_NAME* system ); // �q�[�vID���擾����
static ARCHANDLE* GetArcHandle( const FIELD_PLACE_NAME* system ); // �A�[�J�C�u�f�[�^�n���h�����擾����
static GFL_FONT* GetFont( const FIELD_PLACE_NAME* system ); // �t�H���g���擾����
static GFL_MSGDATA* GetMessageData( const FIELD_PLACE_NAME* system ); // ���b�Z�[�W�f�[�^���擾����
// �e��Z�b�^�[�֐�
static void SetHeapID( FIELD_PLACE_NAME* system, HEAPID heapID ); // �q�[�vID��ݒ肷��
static void SetFont( FIELD_PLACE_NAME* system, GFL_FONT* font ); // �t�H���g��ݒ肷��
// �V�X�e�����
static void SetState( FIELD_PLACE_NAME* system, SYSTEM_STATE state ); // �V�X�e���̏�Ԃ�ݒ肷��
static SYSTEM_STATE GetState( const FIELD_PLACE_NAME* system ); // �V�X�e���̏�Ԃ��擾����
static u16 GetStateCount( const FIELD_PLACE_NAME* system ); // ��ԃJ�E���^���擾����
static void ResetStateCount( FIELD_PLACE_NAME* system ); // ��ԃJ�E���^�����Z�b�g����
static void IncStateCount( FIELD_PLACE_NAME* system ); // ��ԃJ�E���^���X�V����
static u8 GetStateSeq( const FIELD_PLACE_NAME* system ); // ��ԓ��V�[�P���X���擾����
static void ResetStateSeq( FIELD_PLACE_NAME* system ); // ��ԓ��V�[�P���X�����Z�b�g����
static void IncStateSeq( FIELD_PLACE_NAME* system ); // ��ԓ��V�[�P���X���C���N�������g����
// �n��
static u16 GetLastZoneID( const FIELD_PLACE_NAME* system ); // �Ō�ɕ\�������]�[��ID���擾����
static void SetLastZoneID( FIELD_PLACE_NAME* system, u16 zoneID ); // �Ō�ɕ\�������]�[��ID��ݒ肷��
static u16 GetDispZoneID( const FIELD_PLACE_NAME* system ); // ���ɕ\������]�[��ID���擾����
static void SetDispZoneID( FIELD_PLACE_NAME* system, u16 zoneID ); // ���ɕ\������]�[��ID��ݒ肷��
static const STRBUF* GetPlaceName( const FIELD_PLACE_NAME* system ); // �n���o�b�t�@���擾����
static u8 GetPlaceNameLength( const FIELD_PLACE_NAME* system ); // �n���̕��������擾����
// �f�o�b�O
//#ifdef DEBUG_PRINT_ON
static void DEBUG_PrintSystemState( const FIELD_PLACE_NAME* system ); // �V�X�e����Ԗ����o�͂���
//#endif


//====================================================================================
// ���쐬�E�j��
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �n���\���V�X�e�����쐬����
 *
 * @param gameSystem
 * @param heapID     �g�p����q�[�vID
 * @param msgbg      �g�p���郁�b�Z�[�W�\���V�X�e��
 *
 * @return ���������V�X�e��
 */
//------------------------------------------------------------------------------------
FIELD_PLACE_NAME* FIELD_PLACE_NAME_Create( GAMESYS_WORK* gameSystem, HEAPID heapID, FLDMSGBG* msgbg )
{
	FIELD_PLACE_NAME* system;

	system = CreatePNSystem( heapID );
  InitPNSystem( system, gameSystem );
  OpenDataHandle( system );
  CreateWordset( system );
  CreateMessageData( system );
  CreatePlaceNameBuffer( system );
  SetHeapID( system, heapID );
  SetFont( system, FLDMSGBG_GetFontHandle( msgbg ) );

	LoadOBJResource( system );
	CreateClactUnit( system );
  CreateBmpOamSystem( system );

	CreateLetters( system ); // �����I�u�W�F�N�g�𐶐�

  GFL_BG_SetBGControl( BG_FRAME, &BGCntHeader, GFL_BG_MODE_TEXT ); 

	ChangeState( system, SYSTEM_STATE_HIDE );
	return system;
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���\���V�X�e����j������
 *
 * @param system
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Delete( FIELD_PLACE_NAME* system )
{
  DeleteLetters( system ); 
  DeletePlaceNameBuffer( system ); 
  DeleteBmpOamSystem( system ); 
  DeleteClactUnit( system );
  UnloadOBJResource( system ); 
  DeleteWordset( system );
  DeleteMessageData( system ); 
  DeleteBandBitmapWindow( system );
  DeleteBandBlankBitmap( system );
  CloseDataHandle( system );

	GFL_BG_FreeBGControl( BG_FRAME ); 

  DeletePNSystem( system );
} 


//====================================================================================
// ������
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �n���\���V�X�e���̓��쏈��
 *
 * @param system
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Process( FIELD_PLACE_NAME* system )
{
  IncStateCount( system ); // ��ԃJ�E���^���X�V

	// ��Ԃɉ���������
	switch( GetState(system) ) {
  case SYSTEM_STATE_HIDE:         Process_HIDE( system );		      break;
  case SYSTEM_STATE_SETUP:        Process_SETUP( system );		    break;
  case SYSTEM_STATE_FADEIN:       Process_FADEIN( system );	      break;
  case SYSTEM_STATE_WAIT_LAUNCH:  Process_WAIT_LAUNCH( system );	break;
  case SYSTEM_STATE_LAUNCH:       Process_LAUNCH( system );	      break;
  case SYSTEM_STATE_WAIT_FADEOUT: Process_WAIT_FADEOUT( system );	break;
  case SYSTEM_STATE_FADEOUT:      Process_FADEOUT( system );	    break;
	}

	// �������j�b�g�𓮂���
  MoveAllCharUnit( system );
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���\���E�B���h�E�̕`�揈��
 *
 * @param system
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Draw( const FIELD_PLACE_NAME* system )
{ 
	switch( GetState(system) ) {
  case SYSTEM_STATE_HIDE:		      Draw_HIDE( system );		     break;
  case SYSTEM_STATE_SETUP:		    Draw_SETUP( system );		     break;
  case SYSTEM_STATE_FADEIN:		    Draw_FADEIN( system );	     break;
  case SYSTEM_STATE_WAIT_LAUNCH:	Draw_WAIT_LAUNCH( system );	 break;
  case SYSTEM_STATE_LAUNCH:		    Draw_LAUNCH( system );	     break;
  case SYSTEM_STATE_WAIT_FADEOUT:	Draw_WAIT_FADEOUT( system ); break;
  case SYSTEM_STATE_FADEOUT:	    Draw_FADEOUT( system );	     break;
	} 
}


//====================================================================================
// ������
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �]�[���̐؂�ւ���ʒm��, �V�����n����\������
 *
 * @param system
 * @param zoneID �]�[��ID
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Display( FIELD_PLACE_NAME* system, u32 zoneID )
{ 
  // �]�[���̒n���\���t���O�������Ă��Ȃ��ꏊ�ł͕\�����Ȃ�
  if( ZONEDATA_GetPlaceNameFlag( zoneID ) == FALSE ) { return; }

  // ����͋����\��
  if( system->lastZoneID == ZONE_ID_MAX ) {
    SetForceDispFlag( system );
  }

	// �w�肳�ꂽ�]�[��ID�����ɕ\�����ׂ����̂Ƃ��ċL��
  SetDispZoneID( system, zoneID );

	// �\�����̃E�B���h�E��ޏo������
	Cancel( system ); 
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���E�B���h�E�������I�ɕ\������
 *
 * @param system
 * @param zoneID �\������ꏊ�̃]�[��ID
 */
//------------------------------------------------------------------------------------
extern void FIELD_PLACE_NAME_DisplayForce( FIELD_PLACE_NAME* system, u32 zoneID )
{
  // �����I�ɕ\��
  SetDispZoneID( system, zoneID );
  SetForceDispFlag( system );

	// �\�����̃E�B���h�E��ޏo������
	Cancel( system ); 
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���E�B���h�E�̕\���������I�ɏI������
 *
 * @param system
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Hide( FIELD_PLACE_NAME* system )
{
	ChangeState( system, SYSTEM_STATE_HIDE );
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���\���a�f���A
 *
 * @param system
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_RecoverBG( FIELD_PLACE_NAME* system )
{
  ResetBG( system );
  RecoverBlankBand( system );
}


//===================================================================================
// ������J�֐�
//===================================================================================

//-----------------------------------------------------------------------------------
/**
 * @brief BG�̃Z�b�g�A�b�v
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void SetupBG( FIELD_PLACE_NAME* system )
{
	LoadBGPalette( system ); 
  AllocBGNullCharArea( system );
  CreateBandBlankBitmap( system );
  CreateBandBitmapWindow( system );

	// �r�b�g�}�b�v�E�E�B���h�E�̃f�[�^��VRAM�ɓ]��
	GFL_BMPWIN_MakeTransWindow( system->bmpWin ); 

  // BG�\���ݒ�
	GFL_BG_SetPriority( BG_FRAME, BG_FRAME_PRIORITY );
	GFL_BG_SetVisible( BG_FRAME, VISIBLE_OFF ); 
	G2_SetBlendAlpha( ALPHA_PLANE_1, ALPHA_PLANE_2, ALPHA_VALUE_1, ALPHA_VALUE_2 );
}

//-----------------------------------------------------------------------------------
/**
 * @brief BG�̃N���[���A�b�v����
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void CleanBG( FIELD_PLACE_NAME* system )
{
	// �L����VRAM�E�X�N���[��VRAM���N���A
	GFL_BG_ClearFrame( BG_FRAME );

  FreeBGNullCharaArea( system ); 
  DeleteBandBitmapWindow( system );
  DeleteBandBlankBitmap( system );
}

//-----------------------------------------------------------------------------------
/**
 * @brief BG�̍Đݒ�
 *
 * @param sys �n���\���V�X�e��
 */ 
//-----------------------------------------------------------------------------------
static void ResetBG( FIELD_PLACE_NAME* sys )
{ 
  CleanBG( sys );
  SetupBG( sys );
}

//------------------------------------------------------------------------------------
/**
 * @brief BG�p���b�g��ǂݍ���
 *
 * @param system
 */
//------------------------------------------------------------------------------------
static void LoadBGPalette( FIELD_PLACE_NAME* system )
{
  ARCDATID datID;
	ARCHANDLE* handle;
  HEAPID heapID;
	u32 size;
	void* src;
	NNSG2dPaletteData* pal;

  heapID = GetHeapID( system ); 
	handle = GetArcHandle( system );
  datID  = NARC_place_name_place_name_back_NCLR;

	size = GFL_ARC_GetDataSizeByHandle( handle, datID );// �f�[�^�T�C�Y�擾
	src = GFL_HEAP_AllocMemory( system->heapID, size );	// �f�[�^�o�b�t�@�m��
	GFL_ARC_LoadDataByHandle( handle, datID, src );			// �f�[�^�擾
	NNS_G2dGetUnpackedPaletteData( src, &pal );					// �o�C�i������f�[�^��W�J
	GFL_BG_LoadPalette( BG_FRAME, pal->pRawData, 0x20, BG_PALETTE_NO );	// �p���b�g�f�[�^�]��
	GFL_HEAP_FreeMemory( src );											    // �f�[�^�o�b�t�@���
}

//------------------------------------------------------------------------------------
/**
 * @brief �󔒃L�����G���A���m�ۂ���
 *
 * @param system
 */
//------------------------------------------------------------------------------------
static void AllocBGNullCharArea( FIELD_PLACE_NAME* system )
{
  // ���m��
  if( system->nullCharPos == AREAMAN_POS_NOTFOUND ) {
    system->nullCharPos = 
      GFL_BG_AllocCharacterArea( 
          BG_FRAME, CHAR_SIZE * CHAR_SIZE / 2, GFL_BG_CHRAREA_GET_F );
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief �󔒃L�����G���A���������
 *
 * @param system
 */
//------------------------------------------------------------------------------------
static void FreeBGNullCharaArea( FIELD_PLACE_NAME* system )
{
  // �m�ۍς�
  if( system->nullCharPos != AREAMAN_POS_NOTFOUND ) {
    GFL_BG_FreeCharacterArea( 
        BG_FRAME, system->nullCharPos, CHAR_SIZE * CHAR_SIZE / 2 );
    system->nullCharPos = AREAMAN_POS_NOTFOUND;
  }
}


//-----------------------------------------------------------------------------------
/**
 * @brief �т𕶎����������܂�Ă��Ȃ���Ԃɕ��A����
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void RecoverBlankBand( FIELD_PLACE_NAME* system )
{
	GFL_BMP_DATA* src = system->bmpOrg;
	GFL_BMP_DATA* dest = GFL_BMPWIN_GetBmp( system->bmpWin );

	// �������������܂�Ă��Ȃ���Ԃ��R�s�[����, VRAM�ɓ]��
	GFL_BMP_Copy( src, dest );	
	//GFL_BMPWIN_MakeTransWindow( system->bmpWin );
	//GFL_BMPWIN_TransVramCharacter( system->bmpWin );
}

//-----------------------------------------------------------------------------------
/**
 * @brief OBJ ���\�[�X��ǂݍ���
 *
 * @param system 
 */
//-----------------------------------------------------------------------------------
static void LoadOBJResource( FIELD_PLACE_NAME* system )
{
  HEAPID heapID;
	ARCHANDLE* handle;

  heapID = GetHeapID( system );
	handle = GetArcHandle( system );

	system->resPltt[ PLTT_RES_CHAR_UNIT ] = 
		GFL_CLGRP_PLTT_RegisterEx( 
				handle, NARC_place_name_place_name_string_NCLR,
				CLSYS_DRAW_MAIN, FLDOAM_PALNO_PLACENAME * 32, 0, 1, heapID );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: LoadOBJResource\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief OBJ ���\�[�X���������
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void UnloadOBJResource( FIELD_PLACE_NAME* system )
{
  int i;

	for( i=0; i<PLTT_RES_NUM; i++ )
	{
		GFL_CLGRP_PLTT_Release( system->resPltt[i] );
	}

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: UnloadOBJResource\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g�𐶐�����
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void CreateClactUnit( FIELD_PLACE_NAME* system )
{
  HEAPID heapID;

  heapID = GetHeapID( system );
	system->clunit[ CLUNIT_CHAR_UNIT ] = 
		GFL_CLACT_UNIT_Create( MAX_NAME_LENGTH, BG_FRAME_PRIORITY, heapID );

	// �����ݒ�
	GFL_CLACT_UNIT_SetDrawEnable( system->clunit[ CLUNIT_CHAR_UNIT ], TRUE );
  
#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: CreateClactUnit\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g��j������
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void DeleteClactUnit( FIELD_PLACE_NAME* system )
{
  int i;

	for( i=0; i<CLUNIT_NUM; i++ )
	{
		GFL_CLACT_UNIT_Delete( system->clunit[i] );
	}

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: DeleteClactUnit\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g�𐶐�����
 *
 * @param system 
 */
//-----------------------------------------------------------------------------------
static void CreateLetters( FIELD_PLACE_NAME* system )
{
	int i;

	for( i=0; i<MAX_NAME_LENGTH; i++ )
	{
    system->letters[i] = PN_LETTER_Create( 
        system->heapID, system->bmpOamSys, system->resPltt[ PLTT_RES_CHAR_UNIT ] );
	} 

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: CreateLetters\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g��j������
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void DeleteLetters( FIELD_PLACE_NAME* system )
{
	int i;

	for( i=0; i<MAX_NAME_LENGTH; i++ )
	{
    PN_LETTER_Delete( system->letters[i] );
	} 

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: DeleteLetters\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �A�[�J�C�u�f�[�^�n���h�����J��
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void OpenDataHandle( FIELD_PLACE_NAME* system )
{
  HEAPID heapID;

  GF_ASSERT( system->arcHandle == NULL );

  heapID = GetHeapID( system );
  system->arcHandle = GFL_ARC_OpenDataHandle( ARCID_PLACE_NAME, heapID );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: OpenDataHandle\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �A�[�J�C�u�f�[�^�n���h�������
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void CloseDataHandle( FIELD_PLACE_NAME* system )
{
  if( system->arcHandle ) {
    GFL_ARC_CloseDataHandle( system->arcHandle );
    system->arcHandle = NULL;
  }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: CloseDataHandle\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g���Z�b�g�A�b�v����
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void SetupLetter( FIELD_PLACE_NAME* system )
{
  int i, idx;
  STRCODE code[ MAX_NAME_LENGTH ];
  PN_LETTER_SETUP_PARAM param;

  idx = system->setupLetterNum;
  GF_ASSERT( idx < GetPlaceNameLength(system) );

  GFL_STR_GetStringCode( GetPlaceName(system), code, MAX_NAME_LENGTH );
  param = LetterSetupParam;
  param.font = system->font;
  param.code = code[ idx ];

  // �ڕW�ʒu������
  for( i=0; i<idx; i++ )
  {
    param.dx += PN_LETTER_GetWidth( system->letters[i] );
    param.dx += 1;
  }

  PN_LETTER_Setup( system->letters[ idx ], &param );
  system->setupLetterNum++;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: SetupLetter[%d]\n", idx );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g�̃Z�b�g�A�b�v�������������ǂ������`�F�b�N����
 *
 * @param system
 *
 * @return ���ׂĂ̕����I�u�W�F�N�g�̃Z�b�g�A�b�v���������Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------------
static BOOL CheckLetterSetupFinished( const FIELD_PLACE_NAME* system )
{
  if( GetPlaceNameLength(system) <= system->setupLetterNum ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//-----------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g���\���ɂ���
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void HideLetters( FIELD_PLACE_NAME* system )
{
	int i;
	
	for( i=0; i<MAX_NAME_LENGTH; i++ )
	{
    PN_LETTER_SetDrawEnable( system->letters[i], FALSE );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g�𔭎˂���
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void LaunchLetter( FIELD_PLACE_NAME* system )
{
  int idx;

  // ���łɑS�Ẵ��j�b�g���ˍς�
  GF_ASSERT( system->launchLetterNum < GetPlaceNameLength(system) );

  // ����
  idx = system->launchLetterNum++;
  PN_LETTER_MoveStart( system->letters[ idx ] );
  PN_LETTER_SetDrawEnable( system->letters[ idx ], TRUE );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: LaunchLetter[%d]\n", idx );
#endif
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

	for( i=0; i<sys->nameLen; i++ )
	{
    PN_LETTER_Main( sys->letters[i] );
	}
} 

//-----------------------------------------------------------------------------------
/**
 * @brief BMPOAM �V�X�e���𐶐�����
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void CreateBmpOamSystem( FIELD_PLACE_NAME* system )
{
  HEAPID heapID;

  GF_ASSERT( system->bmpOamSys == NULL );

  heapID = GetHeapID( system );
	system->bmpOamSys = BmpOam_Init( heapID, system->clunit[ CLUNIT_CHAR_UNIT ] );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: CreateBmpOamSystem\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief BMPOAM �V�X�e����j������
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void DeleteBmpOamSystem( FIELD_PLACE_NAME* system )
{
  if( system->bmpOamSys ) {
    BmpOam_Exit( system->bmpOamSys );
    system->bmpOamSys = NULL;
  }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: DeleteBmpOamSystem\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �\�����̕������j�b�g���r�b�g�}�b�v�E�E�B���h�E�ɏ�������
 *
 * @param sys ����ΏۃV�X�e��
 */
//-----------------------------------------------------------------------------------
static void WriteCharUnitIntoBitmapWindow( FIELD_PLACE_NAME* sys )
{
  int idx;
	u16 dx, dy;
  u16 width, height;
  PN_LETTER* letter;
	const GFL_BMP_DATA* p_src_bmp = NULL;
	GFL_BMP_DATA* p_dst_bmp = GFL_BMPWIN_GetBmp( sys->bmpWin );

  idx = sys->writeLetterNum;
  GF_ASSERT( idx < GetPlaceNameLength(sys) );

  letter = sys->letters[idx];
  p_src_bmp = PN_LETTER_GetBitmap( letter );
  dx = PN_LETTER_GetLeft( letter ) - ( BMPWIN_POS_X_CHAR * CHAR_SIZE );
  dy = PN_LETTER_GetTop( letter )  - ( BMPWIN_POS_Y_CHAR * CHAR_SIZE );
  width = PN_LETTER_GetWidth( letter );
  height = PN_LETTER_GetHeight( letter );
  GFL_BMP_Print( p_src_bmp, p_dst_bmp, 0, 0, dx, dy, width, height, 0 );

  sys->writeLetterNum++;

	// �X�V���ꂽ�L�����f�[�^��VRAM�ɓ]��
	//GFL_BMPWIN_TransVramCharacter( sys->bmpWin );
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���ׂĂ̕����I�u�W�F�N�g�̏������݂������������ǂ������`�F�b�N����
 *
 * @param system
 *
 * @return ���ׂĊ��������ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------------
static BOOL CheckLetterWriteToBitmapFinished( const FIELD_PLACE_NAME* system )
{
  if( GetPlaceNameLength(system) <= system->writeLetterNum ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//-----------------------------------------------------------------------------------
/**
 * @brief �����\���t���O�𗧂Ă�
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void SetForceDispFlag( FIELD_PLACE_NAME* system )
{
  system->forceDispFlag = TRUE;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �����\���t���O�𗎂Ƃ�
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void ResetForceDispFlag( FIELD_PLACE_NAME* system )
{
  system->forceDispFlag = FALSE;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �����\���t���O���`�F�b�N����
 *
 * @param system
 *
 * @return �����\���t���O�������Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------------
static BOOL CheckForceDispFlag( const FIELD_PLACE_NAME* system )
{
  return system->forceDispFlag;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �n���̍X�V���`�F�b�N����
 *
 * @param system
 *
 * @return �n���̕\�����J�n����ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------------
static BOOL CheckDispStart( const FIELD_PLACE_NAME* system )
{
  // �����\��
  if( CheckForceDispFlag(system) ) { return TRUE; }

  // �V�����]�[�����ʒm���ꂽ
  if( system->dispZoneSetFlag ) {
    if( system->lastZoneID != system->dispZoneID ) {
      return TRUE;
    }
  }

  return FALSE;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �n�����ω��������ǂ������`�F�b�N����
 *
 * @param system
 *
 * @return �n�����ω������ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------------
static BOOL CheckPlaceNameIDChange( const FIELD_PLACE_NAME* system )
{
  u32 lastID = ZONEDATA_GetPlaceNameID( system->lastZoneID );
  u32 dispID = ZONEDATA_GetPlaceNameID( system->dispZoneID );

  OS_Printf( "lastZoneID = %d, lastID = %d\n", system->lastZoneID, lastID );
  OS_Printf( "dispZoneID = %d, dispID = %d\n", system->dispZoneID, dispID );

  // �n���ɕω����Ȃ�
  if( lastID == dispID ) {
    return FALSE;
  }
  else {
    return TRUE;
  }
}

//-----------------------------------------------------------------------------------
/**
 * @brief �\������n�����X�V����
 *
 * @param sys �X�V����V�X�e��
 */
//-----------------------------------------------------------------------------------
static void UpdatePlaceName( FIELD_PLACE_NAME* sys )
{
	u16 str_id;
  BOOL intrudeFlag = FALSE;
  INTRUDE_COMM_SYS_PTR intrudeComm;
  u8 intrudeNetID;
  
	// �]�[��ID����n����������擾����
	str_id = ZONEDATA_GetPlaceNameID( GetDispZoneID(sys) );

  // �G���[���
	if( (str_id < 0) || (msg_place_name_max <= str_id) ){ str_id = 0; } 
	if( str_id == 0 ) 
  { //�u�Ȃ��̂΂���v�Ȃ�\�����Ȃ�
    OBATA_Printf( "�u�Ȃ��̂΂���v�����o( zone id = %d )\n", GetDispZoneID(sys) );
    FIELD_PLACE_NAME_Hide( sys );
  }

  // �N���悩�ǂ����𔻒�
  {
    GAME_COMM_SYS_PTR gameComm;
    FIELD_STATUS* fieldStatus;
    int myNetID;

    gameComm = GAMESYSTEM_GetGameCommSysPtr( sys->gameSystem );
    intrudeComm = Intrude_Check_CommConnect( gameComm );
    fieldStatus = GAMEDATA_GetFieldStatus( sys->gameData );
    myNetID = GAMEDATA_GetIntrudeMyID( sys->gameData );

    if( intrudeComm ) {
      intrudeNetID = Intrude_GetPalaceArea( intrudeComm );

      // ���l�̃t�B�[���h�ɂ���
      if( FIELD_STATUS_GetMapMode( fieldStatus ) == MAPMODE_INTRUDE ) { 
        intrudeFlag = TRUE;
      }
      // ���l�̃p���X�ɂ���
      else if( ZONEDATA_IsPalace( GetDispZoneID(sys) ) && (myNetID != intrudeNetID) ) {
        intrudeFlag = TRUE;
      }
    }
    else {
      intrudeFlag = FALSE;
    }
  }

  // �N����ɂ���
  if( intrudeFlag ) {
    // �N����̃v���C���[����W�J
    STRBUF* strbuf = GFL_MSG_CreateString( sys->msg, MAPNAME_INTRUDE );
    MYSTATUS* status = Intrude_GetMyStatus( intrudeComm, intrudeNetID );
    GFL_MSG_GetString( sys->msg,	str_id, sys->nameBuf );
    WORDSET_RegisterPlayerName( sys->wordset, 0, status );
    WORDSET_RegisterPlaceName( sys->wordset, 1, str_id );
    WORDSET_ExpandStr( sys->wordset, sys->nameBuf, strbuf );
    GFL_STR_DeleteBuffer( strbuf );
  }
  // �����̃t�B�[���h�ɂ���
  else {
    GFL_MSG_GetString( sys->msg,	str_id, sys->nameBuf );
  }
  sys->nameLen = GFL_STR_GetBufferLength( sys->nameBuf );
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��Ԃ�ύX����
 *
 * @param system
 * @param next_state �ݒ肷����
 */
//-----------------------------------------------------------------------------------
static void ChangeState( FIELD_PLACE_NAME* system, SYSTEM_STATE next_state )
{
  SetState( system, next_state ); // ��Ԃ�ݒ�
  ResetStateCount( system ); // ��ԃJ�E���^�����Z�b�g
  ResetStateSeq( system ); // ��ԓ��V�[�P���X�����Z�b�g

	// �J�ڐ�̏�Ԃɉ�����������
	switch( next_state ) {
		case SYSTEM_STATE_HIDE:	
			GFL_BG_SetVisible( BG_FRAME, VISIBLE_OFF );	// BG���\��
			HideLetters( system ); // �����I�u�W�F�N�g���\���ɂ���
			break;
		case SYSTEM_STATE_FADEIN:
      //GFL_BG_SetVisible( BG_FRAME, VISIBLE_ON );	// BG��\��
      //Draw_FADEIN( system );
			break;
		case SYSTEM_STATE_LAUNCH:
			//system->launchLetterNum = 0;	// ���˕����������Z�b�g
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
	SYSTEM_STATE next_state;
	int   start_count;
	float passed_rate;
	
	// ���݂̏�Ԃɉ���������
	switch( sys->state ) {
  case SYSTEM_STATE_HIDE:	
  case SYSTEM_STATE_FADEOUT:
  case SYSTEM_STATE_LAUNCH:
    return;
  case SYSTEM_STATE_WAIT_LAUNCH:
  case SYSTEM_STATE_WAIT_FADEOUT:
    next_state  = SYSTEM_STATE_WAIT_FADEOUT;
    start_count = PROCESS_TIME_WAIT_FADEOUT;
    break;
  case SYSTEM_STATE_FADEIN:
    next_state = SYSTEM_STATE_FADEOUT;
    // �o�ߍς݃t���[�������Z�o
    // (stateCount����, �\���ʒu���Z�o���Ă��邽��, 
    //  �����ޏo�������ꍇ�� stateCount ��K�؂Ɍv�Z����K�v������)
    passed_rate = sys->stateCount / (float)PROCESS_TIME_FADEIN;
    start_count = (int)( (1.0f - passed_rate) * PROCESS_TIME_FADEOUT );
    break;
  case SYSTEM_STATE_SETUP:
    next_state = SYSTEM_STATE_HIDE;
    start_count = 0;
    break;
	}

	// ��Ԃ��X�V
	ChangeState( sys, next_state );
	sys->stateCount = start_count;
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��\����Ԏ��̓���
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void Process_HIDE( FIELD_PLACE_NAME* system )
{
  switch( GetStateSeq(system) ) {
  case 0:
    if( CheckDispStart( system ) ) {
      system->dispZoneSetFlag = FALSE;
      IncStateSeq( system );
    }
    break;

  case 1:
    if( CheckForceDispFlag(system) ) {
      IncStateSeq( system );
    }
    else if( CheckPlaceNameIDChange( system ) ) {
      IncStateSeq( system );
    }
    else {
      ResetStateSeq( system );
    }
    break;

  case 2:
    // �\���J�n
    ResetForceDispFlag( system );
    SetLastZoneID( system, GetDispZoneID(system) );
    ChangeState( system, SYSTEM_STATE_SETUP );
    break;
  }
}

//-----------------------------------------------------------------------------------
/**
 * @brief ������Ԏ��̓���
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void Process_SETUP( FIELD_PLACE_NAME* system )
{
  switch( GetStateSeq(system) ) {
  case 0:
    system->setupLetterNum = 0;
    UpdatePlaceName( system );
    IncStateSeq( system );
    break;

  case 1:
    GFL_BG_ClearCharacter( BG_FRAME );
    IncStateSeq( system );
    break; 
  case 2:
    GFL_BG_ClearScreen( BG_FRAME );
    IncStateSeq( system );
    break;
  case 3:
    FreeBGNullCharaArea( system ); 
    DeleteBandBitmapWindow( system );
    DeleteBandBlankBitmap( system );
    IncStateSeq( system );
    break; 

  case 4:
    LoadBGPalette( system ); 
    IncStateSeq( system );
    break; 
  case 5:
    AllocBGNullCharArea( system );
    IncStateSeq( system );
    break; 
  case 6:
    CreateBandBlankBitmap( system );
    IncStateSeq( system );
    break; 
  case 7:
    CreateBandBitmapWindow( system );
    IncStateSeq( system );
    break; 
  case 8:
    RecoverBlankBand( system );
    IncStateSeq( system );
    break;
  case 9:
    GFL_BMPWIN_MakeTransWindow( system->bmpWin ); 
    IncStateSeq( system );
    break; 

  case 10:
    GFL_BG_SetPriority( BG_FRAME, BG_FRAME_PRIORITY );
    GFL_BG_SetVisible( BG_FRAME, VISIBLE_OFF ); 
    G2_SetBlendAlpha( ALPHA_PLANE_1, ALPHA_PLANE_2, ALPHA_VALUE_1, ALPHA_VALUE_2 );
    IncStateSeq( system );
    break; 

  case 11:
    SetupLetter( system ); // �����I�u�W�F�N�g���Z�b�g�A�b�v

    // �S�Ă̕����I�u�W�F�N�g�̃Z�b�g�A�b�v������
    if( CheckLetterSetupFinished(system) ) {
      ChangeState( system, SYSTEM_STATE_FADEIN );
    }
    break;
  } 
}

//-----------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�E�C����Ԏ��̓���
 *
 * @param system �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_FADEIN( FIELD_PLACE_NAME* system )
{
  switch( GetStateSeq(system) ) {
  case 0:
    Draw_FADEIN( system ); // ���l��������
    GFL_BG_SetVisible( BG_FRAME, VISIBLE_ON );	// BG��\��
    IncStateSeq( system );
    break;

  case 1:
    // ��莞�Ԃ��o�߂�����, ���̏�Ԃ�
    if( PROCESS_TIME_FADEIN < system->stateCount ) {
      ChangeState( system, SYSTEM_STATE_WAIT_LAUNCH );
    }
    break;
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
		ChangeState( sys, SYSTEM_STATE_LAUNCH );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���ˏ�Ԏ��̓���
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void Process_LAUNCH( FIELD_PLACE_NAME* system )
{
  switch( GetStateSeq(system) ) {
  case 0:
		system->launchLetterNum = 0;	// ���˕����������Z�b�g
    IncStateSeq( system );
    break;

  case 1:
    // ���Ԋu�ŕ����𔭎�
    if( system->stateCount % LAUNCH_INTERVAL == 0 ) {

      LaunchLetter( system ); // ����

      // ���ׂĂ̕����𔭎˂�����, ���̏�Ԃ�
      if( system->nameLen <= system->launchLetterNum ) {
        ChangeState( system, SYSTEM_STATE_WAIT_FADEOUT );
      }
    }
    break;
  }
}

//-----------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�A�E�g�҂���Ԏ��̓���
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_WAIT_FADEOUT( FIELD_PLACE_NAME* sys )
{
	int i;
  BOOL moveFinish = TRUE;

  switch( GetStateSeq(sys) ) {
  case 0:
    // �S�Ă̕����̒�~�҂�
    for( i=0; i<sys->nameLen; i++ )
    {
      if( PN_LETTER_IsMoving( sys->letters[i] ) ) { 
        moveFinish = FALSE;
        break; 
      }
    }
    if( moveFinish ) {
      sys->writeLetterNum = 0;
      IncStateSeq( sys );
    }
    break;

  case 1:
    WriteCharUnitIntoBitmapWindow( sys );
    // �S�Ă̕����̏������݊���
    if( CheckLetterWriteToBitmapFinished(sys) ) {
      IncStateSeq( sys );
    }
    break;
  case 2:
	  GFL_BMPWIN_TransVramCharacter( sys->bmpWin );
    IncStateSeq( sys );
    break; 
  case 3:
    HideLetters( sys ); // �����I�u�W�F�N�g���\���ɂ���
    IncStateSeq( sys );
    break; 

  case 4: 
    // ��莞�Ԃ��o��
    if( PROCESS_TIME_WAIT_FADEOUT < sys->stateCount ) { 
      ChangeState( sys, SYSTEM_STATE_FADEOUT );
    }
  }
}

//-----------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�E�A�E�g��Ԏ��̓���
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_FADEOUT( FIELD_PLACE_NAME* sys )
{
	// ��莞�Ԃ��o�߂�����, ���̏�Ԃ�
	if( PROCESS_TIME_FADEOUT < sys->stateCount )
	{
		ChangeState( sys, SYSTEM_STATE_HIDE );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��\����Ԏ��̕`�揈��
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_HIDE( const FIELD_PLACE_NAME* sys )
{
}

//-----------------------------------------------------------------------------------
/**
 * @brief ������Ԏ��̕`�揈��
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_SETUP( const FIELD_PLACE_NAME* sys )
{
}

//-----------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�E�C����Ԏ��̕`�揈��
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_FADEIN( const FIELD_PLACE_NAME* sys )
{
	int val1, val2;
	float rate;

	// ���u�����f�B���O�W�����X�V
	rate  = (float)sys->stateCount / (float)PROCESS_TIME_FADEIN;
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
static void Draw_WAIT_LAUNCH( const FIELD_PLACE_NAME* sys )
{
}

//-----------------------------------------------------------------------------------
/**
 * @brief �������ˏ�Ԏ��̕`�揈��
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_LAUNCH( const FIELD_PLACE_NAME* sys )
{
}

//-----------------------------------------------------------------------------------
/**
 * @brief �ҋ@��Ԏ��̕`�揈��
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_WAIT_FADEOUT( const FIELD_PLACE_NAME* sys )
{
}

//-----------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�E�A�E�g��Ԏ��̕`�揈��
 *
 * @param sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Draw_FADEOUT( const FIELD_PLACE_NAME* sys )
{
	int val1, val2;
	float rate;
	
	// ���u�����f�B���O�W�����X�V
	rate  = (float)sys->stateCount / (float)PROCESS_TIME_FADEOUT;
	val1 = (int)( ALPHA_VALUE_1 * (1.0f - rate) );
	val2 = (int)( ALPHA_VALUE_2 + (16 - ALPHA_VALUE_2) * rate );
	G2_SetBlendAlpha( ALPHA_PLANE_1, ALPHA_PLANE_2, val1, val2 );
} 


//-----------------------------------------------------------------------------------
/**
 * @brief �n���\���V�X�e���𐶐�����
 * 
 * @param heapID �g�p����q�[�vID
 */
//-----------------------------------------------------------------------------------
static FIELD_PLACE_NAME* CreatePNSystem( HEAPID heapID )
{
	FIELD_PLACE_NAME* system;

	system = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_PLACE_NAME) );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: CreatePNSystem\n" );
#endif

  return system;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �n���\���V�X�e��������������
 * 
 * @param system
 * @param gameSystem
 */
//-----------------------------------------------------------------------------------
static void InitPNSystem( FIELD_PLACE_NAME* system, GAMESYS_WORK* gameSystem )
{
  // �[���N���A
  GFL_STD_MemClear( system, sizeof(FIELD_PLACE_NAME) );

  // ������
  system->gameSystem = gameSystem;
  system->gameData = GAMESYSTEM_GetGameData( gameSystem );
  system->nullCharPos = AREAMAN_POS_NOTFOUND;
  system->forceDispFlag = FALSE;
  system->dispZoneSetFlag = FALSE;
  system->lastZoneID = ZONE_ID_MAX;
  system->dispZoneID = ZONE_ID_MAX;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: InitPNSystem\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �n���\���V�X�e����j������
 * 
 * @param system
 */
//-----------------------------------------------------------------------------------
static void DeletePNSystem( FIELD_PLACE_NAME* system )
{
  GFL_HEAP_FreeMemory( system );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: DeletePNSystem\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���[�h�Z�b�g�𐶐�����
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void CreateWordset( FIELD_PLACE_NAME* system )
{
  HEAPID heapID;

  GF_ASSERT( system->wordset == NULL );

  heapID = GetHeapID( system ); 
  system->wordset = WORDSET_Create( heapID );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: CreateWordset\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^�𐶐�����
 * 
 * @param system
 */
//-----------------------------------------------------------------------------------
static void CreateMessageData( FIELD_PLACE_NAME* system )
{
  HEAPID heapID;

  GF_ASSERT( system->msg == NULL );

  heapID = GetHeapID( system ); 
	system->msg = GFL_MSG_Create( 
      GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_place_name_dat, heapID );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: CreateMessagData\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �n���o�b�t�@�𐶐�����
 * 
 * @param system
 */
//-----------------------------------------------------------------------------------
static void CreatePlaceNameBuffer( FIELD_PLACE_NAME* system )
{
  HEAPID heapID;
  int length;

  GF_ASSERT( system->nameBuf == NULL );

  heapID = GetHeapID( system ); 
  length = MAX_NAME_LENGTH + 1; // EOM���l������+1
	system->nameBuf = GFL_STR_CreateBuffer( length, heapID ); 

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: CreatePlaceNameBuffer\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �����т̃r�b�g�}�b�v�𐶐�����
 * 
 * @param system
 */
//-----------------------------------------------------------------------------------
static void CreateBandBlankBitmap( FIELD_PLACE_NAME* system )
{
  HEAPID heapID;
  ARCHANDLE* handle;

  GF_ASSERT( system->bmpOrg == NULL );

  heapID = GetHeapID( system );
  handle = GetArcHandle( system );

  system->bmpOrg = 
    GFL_BMP_LoadCharacterByHandle( 
        handle, NARC_place_name_place_name_back_NCGR, FALSE, heapID ); 

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: CreateBandBlankBitmap\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �т̃r�b�g�}�b�v�E�B���h�E�𐶐�����
 * 
 * @param system
 */
//-----------------------------------------------------------------------------------
static void CreateBandBitmapWindow( FIELD_PLACE_NAME* system )
{
  HEAPID heapID;

  GF_ASSERT( system->bmpWin == NULL );

  heapID = GetHeapID( system );
  system->bmpWin = GFL_BMPWIN_Create( BG_FRAME, 
                                      BMPWIN_POS_X_CHAR, BMPWIN_POS_Y_CHAR, 
                                      BMPWIN_WIDTH_CHAR, BMPWIN_HEIGHT_CHAR, 
                                      BG_PALETTE_NO, GFL_BMP_CHRAREA_GET_F ); 

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: CreateBandBitmapWindow\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���[�h�Z�b�g��j������
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void DeleteWordset( FIELD_PLACE_NAME* system )
{
  if( system->wordset ) {
    WORDSET_Delete( system->wordset );
    system->wordset = NULL;
  }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: DeleteWordset\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^��j������
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void DeleteMessageData( FIELD_PLACE_NAME* system )
{
  if( system->msg ) {
    GFL_MSG_Delete( system->msg );
    system->msg = NULL;
  }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: DeleteMessageData\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �n���o�b�t�@��j������
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void DeletePlaceNameBuffer( FIELD_PLACE_NAME* system )
{
  if( system->nameBuf ) { 
    GFL_STR_DeleteBuffer( system->nameBuf );
    system->nameBuf = NULL;
  }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: DeletePlaceNameBuffer\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �����т̃r�b�g�}�b�v��j������
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void DeleteBandBlankBitmap( FIELD_PLACE_NAME* system )
{
  if( system->bmpOrg ) { 
    GFL_BMP_Delete( system->bmpOrg ); 
    system->bmpOrg = NULL;
  }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: DeleteBandBlankBitmap\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �т̃r�b�g�}�b�v�E�B���h�E��j������
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void DeleteBandBitmapWindow( FIELD_PLACE_NAME* system )
{
	if( system->bmpWin ) { 
    GFL_BMPWIN_Delete( system->bmpWin ); 
    system->bmpWin = NULL;
  }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: DeleteBandBitmapWindow\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �q�[�vID���擾����
 *
 * @param system
 *
 * @return �q�[�vID
 */
//-----------------------------------------------------------------------------------
static HEAPID GetHeapID( const FIELD_PLACE_NAME* system )
{
  return system->heapID;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �A�[�J�C�u�f�[�^�n���h�����擾����
 *
 * @param system
 *
 * @return �A�[�J�C�u�f�[�^�n���h��
 */
//-----------------------------------------------------------------------------------
static ARCHANDLE* GetArcHandle( const FIELD_PLACE_NAME* system )
{
  return system->arcHandle;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �t�H���g���擾����
 *
 * @param system
 *
 * @return �t�H���g
 */
//-----------------------------------------------------------------------------------
static GFL_FONT* GetFont( const FIELD_PLACE_NAME* system )
{
  return system->font;
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^���擾����
 *
 * @param system
 *
 * @return ���b�Z�[�W�f�[�^
 */
//-----------------------------------------------------------------------------------
static GFL_MSGDATA* GetMessageData( const FIELD_PLACE_NAME* system )
{
  return system->msg;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �q�[�vID��ݒ肷��
 *
 * @param system
 * @param heapID 
 */
//-----------------------------------------------------------------------------------
static void SetHeapID( FIELD_PLACE_NAME* system, HEAPID heapID )
{
  system->heapID = heapID;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: SetHeapID\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �t�H���g��ݒ肷��
 *
 * @param system
 * @param font 
 */
//-----------------------------------------------------------------------------------
static void SetFont( FIELD_PLACE_NAME* system, GFL_FONT* font )
{
  system->font = font;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: SetFont\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �V�X�e���̏�Ԃ�ݒ肷��
 *
 * @param system
 * @param state  �ݒ肷����
 */
//-----------------------------------------------------------------------------------
static void SetState( FIELD_PLACE_NAME* system, SYSTEM_STATE state )
{
  system->state = state;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: SetState ==> " );
  DEBUG_PrintSystemState( system );
  OS_TFPrintf( DEBUG_PRINT_TARGET, "\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �V�X�e���̏�Ԃ��擾����
 *
 * @param system
 *
 * @return �V�X�e���̏��
 */
//-----------------------------------------------------------------------------------
static SYSTEM_STATE GetState( const FIELD_PLACE_NAME* system )
{
  return system->state;
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��ԃJ�E���^���擾����
 *
 * @param system
 *
 * @return ��ԃJ�E���^�̒l
 */
//-----------------------------------------------------------------------------------
static u16 GetStateCount( const FIELD_PLACE_NAME* system )
{
  return system->stateCount;
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��ԃJ�E���^�����Z�b�g����
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void ResetStateCount( FIELD_PLACE_NAME* system )
{
  system->stateCount = 0;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: ResetStateCount\n" );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��ԃJ�E���^���X�V����
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void IncStateCount( FIELD_PLACE_NAME* system )
{
  system->stateCount++;

  // �ő�l�␳
  if( MAX_STATE_COUNT < system->stateCount ) {
    system->stateCount = MAX_STATE_COUNT;
  }
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��ԓ��V�[�P���X���擾����
 *
 * @param system
 *
 * @return ��ԓ��V�[�P���X�ԍ�
 */
//-----------------------------------------------------------------------------------
static u8 GetStateSeq( const FIELD_PLACE_NAME* system )
{
  return system->stateSeq;
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��ԓ��V�[�P���X�����Z�b�g����
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void ResetStateSeq( FIELD_PLACE_NAME* system )
{
  system->stateSeq = 0;
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��ԓ��V�[�P���X���C���N�������g����
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void IncStateSeq( FIELD_PLACE_NAME* system )
{
  system->stateSeq++;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �Ō�ɕ\�������]�[��ID���擾����
 *
 * @param system
 *
 * @return �Ō�ɕ\�������ꏊ�̃]�[��ID
 */
//-----------------------------------------------------------------------------------
static u16 GetLastZoneID( const FIELD_PLACE_NAME* system )
{
  return system->lastZoneID;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �Ō�ɕ\�������]�[��ID��ݒ肷��
 *
 * @param system
 * @param zoneID
 */
//-----------------------------------------------------------------------------------
static void SetLastZoneID( FIELD_PLACE_NAME* system, u16 zoneID )
{
  system->lastZoneID = zoneID;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: SetLastZoneID (%d)\n", zoneID );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���ɕ\������]�[��ID���擾����
 *
 * @param system
 *
 * @return ���ɕ\������ꏊ�̃]�[��ID
 */
//-----------------------------------------------------------------------------------
static u16 GetDispZoneID( const FIELD_PLACE_NAME* system )
{
  return system->dispZoneID;
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���ɕ\������]�[��ID��ݒ肷��
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void SetDispZoneID( FIELD_PLACE_NAME* system, u16 zoneID )
{
  system->dispZoneID = zoneID;
  system->dispZoneSetFlag = TRUE;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: SetDispZoneID (%d)\n", zoneID );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �n���o�b�t�@���擾����
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static const STRBUF* GetPlaceName( const FIELD_PLACE_NAME* system )
{
  GF_ASSERT( system->nameBuf );

  return system->nameBuf;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �n���̕��������擾����
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static u8 GetPlaceNameLength( const FIELD_PLACE_NAME* system )
{
  return system->nameLen;
}


//#ifdef DEBUG_PRINT_ON
//-----------------------------------------------------------------------------------
/**
 * @brief �V�X�e����Ԗ����o�͂���
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void DEBUG_PrintSystemState( const FIELD_PLACE_NAME* system )
{
  switch( GetState(system) ) {
  case SYSTEM_STATE_HIDE:         OS_TFPrintf( DEBUG_PRINT_TARGET, "HIDE" );          break;
  case SYSTEM_STATE_SETUP:        OS_TFPrintf( DEBUG_PRINT_TARGET, "SETUP" );         break;
  case SYSTEM_STATE_FADEIN:       OS_TFPrintf( DEBUG_PRINT_TARGET, "FADEIN" );        break;
  case SYSTEM_STATE_WAIT_LAUNCH:  OS_TFPrintf( DEBUG_PRINT_TARGET, "WAIT_LAUNCH" );   break;
  case SYSTEM_STATE_LAUNCH:       OS_TFPrintf( DEBUG_PRINT_TARGET, "LAUNCH" );        break;
  case SYSTEM_STATE_WAIT_FADEOUT: OS_TFPrintf( DEBUG_PRINT_TARGET, "WAIT_FADEOUT" );  break;
  case SYSTEM_STATE_FADEOUT:      OS_TFPrintf( DEBUG_PRINT_TARGET, "FADEOUT" );       break;
  }
}
//#endif






























