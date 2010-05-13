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
#include "savedata/situation.h"
#include "savedata/save_control.h"

#include "buflen.h"
#include "field_oam_pal.h"  // for FLDOAM_PALNO_PLACENAME

#include "arc/arc_def.h"
#include "arc/place_name.naix"
#include "arc/message.naix"
#include "msg/msg_place_name.h"

#include "../../../resource/fldmapdata/zonetable/zone_id.h"


//===================================================================================
// ���f�o�b�O
//=================================================================================== 
//#define DEBUG_PRINT_ON // �f�o�b�O�o�̓X�C�b�`
#define DEBUG_PRINT_TARGET (3) // �f�o�b�O�o�͐�

#ifdef PM_DEBUG
BOOL PlaceNameEnable = TRUE; // �n���\���̗L���t���O
#endif

//===================================================================================
// �� �萔�E�}�N��
//=================================================================================== 
#define MAX_STATE_COUNT (0xffff) // ��ԃJ�E���^�ő�l
//#define MAX_NAME_LENGTH (BUFLEN_PLACE_NAME - BUFLEN_EOM_SIZE) // �ő啶����
#define MAX_NAME_LENGTH (28)
// ���p���X�N�����͐N������̖��O��\�����邽��, �傫�߂ɐݒ肷��.2010.05.11
// �����_�̍ő咷�u�z���C�g�t�H���X�g�v+ ���O5���� = 14���� 
// ==> ���[�J���C�Y�l���� 28����

//-----------
// �\���ݒ�
//-----------
// BG
#define BG_PALETTE_NO          (0) // BG�p���b�g�ԍ�
#define BG_FRAME (GFL_BG_FRAME3_M) // �g�p����BG�t���[��
#define BG_FRAME_PRIORITY      (1) // BG�t���[���̃v���C�I���e�B
// �t�H���g
#define	COLOR_NO_LETTER     (1) // �����{�̂̃J���[�ԍ�
#define	COLOR_NO_SHADOW     (2) // �e�����̃J���[�ԍ�
#define	COLOR_NO_BACKGROUND (0) // �w�i���̃J���[�ԍ�
// ���u�����h
#define ALPHA_PLANE_1 (GX_BLEND_PLANEMASK_BG3) // ���u�����h�̑�1�Ώۖ�
#define ALPHA_PLANE_2 (GX_BLEND_PLANEMASK_BG0) // ���u�����h�̑�2�Ώۖ�
#define ALPHA_VALUE_1                     (16) // ��1�Ώۖʂ̃��u�����f�B���O�W��
#define ALPHA_VALUE_2                      (4) // ��2�Ώۖʂ̃��u�����f�B���O�W��

#define CHAR_SIZE                    (8) // 1�L���� = 8�h�b�g
#define Y_CENTER_POS (CHAR_SIZE * 2 - 1) // �w�i�т̒��Sy���W

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

//-----------
// ����ݒ�
//-----------
#define LAUNCH_INTERVAL (3)	// �����I�u�W�F�N�g�̔��ˊԊu[frame]

// �e��Ԃ̓���ݒ�
#define PROCESS_TIME_FADEIN  (10)
#define PROCESS_TIME_WAIT_LAUNCH   (10)
#define PROCESS_TIME_WAIT_FADEOUT   (30)
#define PROCESS_TIME_FADEOUT (20)

//--------------
// �C���f�b�N�X
//--------------
// �p���b�g���\�[�X
typedef enum {
	PLTT_RES_LETTER,	// �����I�u�W�F�N�g�Ŏg�p����p���b�g
	PLTT_RES_NUM      // ����
} PLTT_RES_INDEX;

// �Z���A�N�^�[���j�b�g
typedef enum {
	CLUNIT_LETTER, // �����I�u�W�F�N�g
	CLUNIT_NUM,    // ����
} CLUNIT_INDEX;

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
} SYSTEM_STATE; 

// �����I�u�W�F�N�g�̃Z�b�g�A�b�v�p�����[�^
static const PN_LETTER_SETUP_PARAM LetterSetupParam = 
{
  256,                       // ���W
  Y_CENTER_POS - ( 13 / 2 ), // 
  -20,                       // ���x
  0,                         // 
  24,                        // �ڕW�ʒu
  Y_CENTER_POS - ( 13 / 2 ), // 
  0,                         // �ڕW�ʒu�ł̑��x
  0,                         // 
  NULL,                      // �t�H���g
  0,                         // �����R�[�h
};

// BG �R���g���[��
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

// �����_���[�̃C���f�b�N�X
typedef enum {
  RENDERER_LETTER, // �����I�u�W�F�N�g�p
  RENDERER_NUM
} RENDERER_INDEX;

// �Z���A�N�^�[�̃����_���[
static const GFL_REND_SURFACE_INIT RendererSurface[ RENDERER_NUM ] = 
{
  {
    0,                                  // �T�[�t�F�[�X���゘���W
    0,                                  // �T�[�t�F�[�X���゙���W
    256,                                // �T�[�t�F�[�X��
    192,                                // �T�[�t�F�[�X����
    CLSYS_DRAW_MAIN,                    // �T�[�t�F�[�X�^�C�v(CLSYS_DRAW_TYPE)
    CLSYS_REND_CULLING_TYPE_NOT_AFFINE, // �J�����O�^�C�v
  },
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
	GFL_MSGDATA*  message;
  GFL_FONT*     font;

	// �n��
	STRBUF* nameBuf; // �\�����̒n��������
	u8      nameLen; // �\�����̒n��������

	// ��
	GFL_BMPWIN*   bmpWin;	     // �r�b�g�}�b�v�E�E�B���h�E
	GFL_BMP_DATA* bmpOrg;	     // �������������܂�Ă��Ȃ����
  u32           nullCharPos; // NULL�L�����N�^�̃L����No.

	// OBJ
	u32             resPltt[ PLTT_RES_NUM ]; // �p���b�g���\�[�X
	BMPOAM_SYS_PTR  bmpOamSys;               // BMPOAM�V�X�e��
	GFL_CLUNIT*     clunit[ CLUNIT_NUM ];    // �Z���A�N�^�[���j�b�g
  GFL_CLSYS_REND* renderer;                // �����_���[�V�X�e��

  // �V�X�e�����
	SYSTEM_STATE state;	
  u8  stateSeq;   // ��ԓ��V�[�P���X
	u16	stateCount;	// ��ԃJ�E���^

  // �\������]�[��
  u16  prevZoneID;    // �O��̃]�[��ID
  u16  nowZoneID;     // ���݂̃]�[��ID
  BOOL dispFlag;      // �\���t���O
  BOOL forceDispFlag; // �����\���t���O

  // �����I�u�W�F�N�g
  PN_LETTER* letters[ MAX_NAME_LENGTH ];
  u8 setupLetterNum; // �Z�b�g�A�b�v���������������I�u�W�F�N�g�̐�
	u8 launchLetterNum;// ���ˍςݕ�����
  u8 writeLetterNum; // BG �ɏ������񂾕�����

  // BG �p���b�g
  void* BGPalBinary; // �o�C�i���f�[�^
	NNSG2dPaletteData* BGPalData; // �o�C�i���W�J�f�[�^
};


//===================================================================================
// ��prototype
//===================================================================================
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// BG
static void SetupBG( FIELD_PLACE_NAME* system );
static void CleanBG( FIELD_PLACE_NAME* system );
static void ResetBG( FIELD_PLACE_NAME* system );	
static void LoadBGPaletteData( FIELD_PLACE_NAME* system ); // BG �p���b�g�f�[�^��ǂݍ���
static void UnloadBGPaletteData( FIELD_PLACE_NAME* system ); // BG �p���b�g�f�[�^���������
static void TransBGPalette( FIELD_PLACE_NAME* system ); // BG �p���b�g��]������
static void AllocBGNullCharArea( FIELD_PLACE_NAME* system ); // �󔒃L�����G���A���m�ۂ���
static void FreeBGNullCharaArea( FIELD_PLACE_NAME* system ); // �󔒃L�����G���A���������
static void RecoverBlankBand( FIELD_PLACE_NAME* system ); // �т𕶎����������܂�Ă��Ȃ���Ԃɕ��A����
// OBJ
static void CreateClactRenderer( FIELD_PLACE_NAME* system ); // �Z���A�N�^�[�̃����_���[���쐬����
static void DeleteClactRenderer( FIELD_PLACE_NAME* system ); // �Z���A�N�^�[�̃����_���[��j������
static void LoadOBJResource( FIELD_PLACE_NAME* system ); // OBJ ���\�[�X��ǂݍ���
static void UnloadOBJResource( FIELD_PLACE_NAME* system ); // OBJ ���\�[�X���������
static void CreateBmpOamSystem( FIELD_PLACE_NAME* system ); // BMPOAM �V�X�e���𐶐�����
static void DeleteBmpOamSystem( FIELD_PLACE_NAME* system ); // BMPOAM �V�X�e����j������
static void CreateClactUnit( FIELD_PLACE_NAME* system ); // �Z���A�N�^�[���j�b�g�𐶐�����
static void DeleteClactUnit( FIELD_PLACE_NAME* system ); // �Z���A�N�^�[���j�b�g��j������
static void SetUserRenderer( FIELD_PLACE_NAME* system, CLUNIT_INDEX unitIdx ); // �Z���A�N�^�[���j�b�g�Ƀ����_���[��ݒ肷��
// �����I�u�W�F�N�g
static void SetupLetter_init( FIELD_PLACE_NAME* system ); // �����I�u�W�F�N�g�̃Z�b�g�A�b�v���J�n����
static void SetupLetter_main( FIELD_PLACE_NAME* system ); // �����I�u�W�F�N�g���P�������Z�b�g�A�b�v����
static BOOL SetupLetter_check( const FIELD_PLACE_NAME* system ); // �����I�u�W�F�N�g�̃Z�b�g�A�b�v�������������ǂ������`�F�b�N����
static void HideLetters( FIELD_PLACE_NAME* system ); // �����I�u�W�F�N�g���\���ɂ���
static void LaunchLetter( FIELD_PLACE_NAME* system ); // �����I�u�W�F�N�g�𔭎˂���
static void MoveLetters( FIELD_PLACE_NAME* system ); // �����I�u�W�F�N�g�𓮂���
// BG, �������j�b�g�ւ̏�������
static void PrintLetter_init( FIELD_PLACE_NAME* system ); // �����I�u�W�F�N�g�̏������݂��J�n����
static void PrintLetter_main( FIELD_PLACE_NAME* system ); // �����I�u�W�F�N�g���P�������r�b�g�}�b�v�E�E�B���h�E�ɏ�������
static BOOL PrintLetter_check( const FIELD_PLACE_NAME* system ); // �����I�u�W�F�N�g�̏������݊������`�F�b�N����
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
//-----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------- 
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
static void LoadNowZoneID( FIELD_PLACE_NAME* system ); // ���݂̃]�[��ID��ǂݍ���
static void SaveNowZoneID( const FIELD_PLACE_NAME* system ); // ���݂̃]�[��ID���L�^����
static u16 GetPrevZoneID( const FIELD_PLACE_NAME* system ); // �O��̃]�[��ID���擾����
static u16 GetNowZoneID( const FIELD_PLACE_NAME* system ); // ���݂̃]�[��ID���擾����
static void SetNowZoneID( FIELD_PLACE_NAME* system, u16 zoneID ); // ���݂̃]�[��ID��ݒ肷��
static const STRBUF* GetPlaceName( const FIELD_PLACE_NAME* system ); // �n���o�b�t�@���擾����
static u8 GetPlaceNameLength( const FIELD_PLACE_NAME* system ); // �n���̕��������擾����
// �n���̍X�V
static BOOL CheckForceDispFlag( const FIELD_PLACE_NAME* system ); // �����\���t���O���`�F�b�N����
static void SetForceDispFlag( FIELD_PLACE_NAME* system ); // �����\���t���O�𗧂Ă�
static void ResetForceDispFlag( FIELD_PLACE_NAME* system ); // �����\���t���O�𗎂Ƃ�
static BOOL CheckDispFlag( const FIELD_PLACE_NAME* system ); // �\���t���O���`�F�b�N����
static void SetDispFlag( FIELD_PLACE_NAME* system ); // �\���t���O���Z�b�g����
static void ResetDispFlag( FIELD_PLACE_NAME* system ); // �\���t���O�𗎂Ƃ�
static BOOL CheckPlaceNameIDChange( const FIELD_PLACE_NAME* system ); // �n�����ω��������ǂ������`�F�b�N����
static void UpdatePlaceName( FIELD_PLACE_NAME* system ); // �\������n�����X�V����
static BOOL CheckIntrudeField( const FIELD_PLACE_NAME* system, u16 zoneID ); // �N���悩�ǂ������`�F�b�N����
// ���[�e�B���e�B
static BOOL CheckPlaceNameFlag( u16 zoneID ); // �n���\���t���O���`�F�b�N����
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
  LoadNowZoneID( system );
  OpenDataHandle( system );
  CreateWordset( system );
  CreateMessageData( system );
  CreatePlaceNameBuffer( system );
  SetHeapID( system, heapID );
  SetFont( system, FLDMSGBG_GetFontHandle( msgbg ) );

  CreateClactRenderer( system );
	LoadOBJResource( system );
	CreateClactUnit( system );
  SetUserRenderer( system, CLUNIT_LETTER );
  CreateBmpOamSystem( system ); 
	CreateLetters( system );

  GFL_BG_SetBGControl( BG_FRAME, &BGCntHeader, GFL_BG_MODE_TEXT ); 
  LoadBGPaletteData( system );

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
  SaveNowZoneID( system );

  UnloadBGPaletteData( system ); 
  DeleteLetters( system ); 
  DeletePlaceNameBuffer( system ); 
  DeleteBmpOamSystem( system ); 
  DeleteClactUnit( system );
  UnloadOBJResource( system ); 
  DeleteClactRenderer( system );
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

//#define TICK_TEST
//------------------------------------------------------------------------------------
/**
 * @brief �n���\���V�X�e���̓��쏈��
 *
 * @param system
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Process( FIELD_PLACE_NAME* system )
{ 
#ifdef TICK_TEST
  OSTick start, end;
#endif

#ifdef PM_DEBUG
  if( PlaceNameEnable == FALSE ) { return; }
#endif

  IncStateCount( system ); // ��ԃJ�E���^���X�V

#ifdef TICK_TEST
  DEBUG_PrintSystemState( system );
  OS_TFPrintf( 3, "[%d]: ", GetStateSeq(system) );
  start = OS_GetTick();
#endif

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

	// �����I�u�W�F�N�g�𓮂���
  MoveLetters( system );

#ifdef TICK_TEST
  end = OS_GetTick();
  OS_TFPrintf( 3, "tick = %d, micro sec = %d\n", 
      (u32)(end - start),
      (u32)OS_TicksToMicroSeconds( (end - start) ) );
#endif
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
 * @param zoneID �n����\������ꏊ�̃]�[��ID
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Display( FIELD_PLACE_NAME* system, u32 zoneID )
{ 
  // ���N�G�X�g�̂������]�[��ID���X�V
  SetNowZoneID( system, zoneID ); 

  // �n���\���t���O�������Ă��Ȃ��ꏊ�ł͕\�����Ȃ�
  if( CheckPlaceNameFlag( zoneID ) == FALSE ) { return; }

  // �\�����̃E�B���h�E��ޏo������
	Cancel( system ); 

  // �\���t���O�𗧂Ă�
  SetDispFlag( system );
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���E�B���h�E�������I�ɕ\������
 *
 * @param system
 * @param zoneID �n����\������ꏊ�̃]�[��ID
 */
//------------------------------------------------------------------------------------
extern void FIELD_PLACE_NAME_DisplayForce( FIELD_PLACE_NAME* system, u32 zoneID )
{
  // �\�����̃E�B���h�E��ޏo������
	Cancel( system ); 

  // �����I�ɕ\��
  SetForceDispFlag( system ); 
  SetNowZoneID( system, zoneID );
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

  // �S�Ă̕\�����N�G�X�g�𖳌���
  ResetDispFlag( system );
  ResetForceDispFlag( system );
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
	TransBGPalette( system ); 
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
 * @brief BG �p���b�g�f�[�^��ǂݍ���
 *
 * @param system
 */
//------------------------------------------------------------------------------------
static void LoadBGPaletteData( FIELD_PLACE_NAME* system )
{
  ARCDATID datID;
  ARCHANDLE* handle;
  HEAPID heapID;
  u32 size;

  GF_ASSERT( system->BGPalBinary == NULL );
  GF_ASSERT( system->BGPalData == NULL );

  heapID = GetHeapID( system ); 
  handle = GetArcHandle( system );
  datID  = NARC_place_name_place_name_back_NCLR;

  size = GFL_ARC_GetDataSizeByHandle( handle, datID ); // �f�[�^�T�C�Y�擾
  system->BGPalBinary = GFL_HEAP_AllocMemory( heapID, size );	// �f�[�^�o�b�t�@�m��
  GFL_ARC_LoadDataByHandle( handle, datID, system->BGPalBinary ); // �f�[�^�擾
  NNS_G2dGetUnpackedPaletteData( system->BGPalBinary, &system->BGPalData ); // �o�C�i����W�J
}

//------------------------------------------------------------------------------------
/**
 * @brief BG �p���b�g�f�[�^���������
 *
 * @param system
 */
//------------------------------------------------------------------------------------
static void UnloadBGPaletteData( FIELD_PLACE_NAME* system )
{
  if( system->BGPalBinary ) {
    GFL_HEAP_FreeMemory( system->BGPalBinary );
    system->BGPalBinary = NULL;
    system->BGPalData = NULL;
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief BG �p���b�g��]������
 *
 * @param system
 */
//------------------------------------------------------------------------------------
static void TransBGPalette( FIELD_PLACE_NAME* system )
{
	GFL_BG_LoadPalette( 
      BG_FRAME, system->BGPalData->pRawData, 0x20, BG_PALETTE_NO );
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

	// �������������܂�Ă��Ȃ���Ԃ��R�s�[
	GFL_BMP_Copy( src, dest );	
}

//-----------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�̃����_���[���쐬����
 *
 * @param system 
 */
//-----------------------------------------------------------------------------------
static void CreateClactRenderer( FIELD_PLACE_NAME* system )
{
  GF_ASSERT( system->renderer == NULL );

  system->renderer = GFL_CLACT_USERREND_Create( 
      RendererSurface, NELEMS(RendererSurface), system->heapID );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�̃����_���[��j������
 *
 * @param system 
 */
//-----------------------------------------------------------------------------------
static void DeleteClactRenderer( FIELD_PLACE_NAME* system )
{
  GF_ASSERT( system->renderer );

  if( system->renderer ) {
    GFL_CLACT_USERREND_Delete( system->renderer );
    system->renderer = NULL;
  }
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

	system->resPltt[ PLTT_RES_LETTER ] = 
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
	system->clunit[ CLUNIT_LETTER ] = 
		GFL_CLACT_UNIT_Create( MAX_NAME_LENGTH, BG_FRAME_PRIORITY, heapID );

	// �����ݒ�
	GFL_CLACT_UNIT_SetDrawEnable( system->clunit[ CLUNIT_LETTER ], TRUE );
  
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
 * @brief �Z���A�N�^�[���j�b�g�Ƀ����_���[��ݒ肷��
 *
 * @param system
 * @param unitIdx   ���j�b�g
 */
//-----------------------------------------------------------------------------------
static void SetUserRenderer( FIELD_PLACE_NAME* system, CLUNIT_INDEX unitIdx )
{
  if( system->renderer ) {
    GFL_CLACT_UNIT_SetUserRend( system->clunit[ unitIdx ], system->renderer );
  }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: SetUserRenderer\n" );
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
        system->heapID, system->bmpOamSys, RENDERER_LETTER, 
        system->resPltt[ PLTT_RES_LETTER ] );
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
 * @brief �����I�u�W�F�N�g�̃Z�b�g�A�b�v���J�n����
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void SetupLetter_init( FIELD_PLACE_NAME* system )
{
  system->setupLetterNum = 0;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g���P�������Z�b�g�A�b�v����
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void SetupLetter_main( FIELD_PLACE_NAME* system )
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
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: SetupLetter_main[%d]\n", idx );
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
static BOOL SetupLetter_check( const FIELD_PLACE_NAME* system )
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
 * @brief ���ׂĂ̕����I�u�W�F�N�g�𓮂���
 *
 * @param system 
 */
//-----------------------------------------------------------------------------------
static void MoveLetters( FIELD_PLACE_NAME* system )
{
  int i;
  int len = GetPlaceNameLength( system );

	for( i=0; i<len; i++ )
	{
    PN_LETTER_Main( system->letters[i] );
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
	system->bmpOamSys = BmpOam_Init( heapID, system->clunit[ CLUNIT_LETTER ] );

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
 * @brief �����I�u�W�F�N�g�̏������݂��J�n����
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void PrintLetter_init( FIELD_PLACE_NAME* system )
{
  system->writeLetterNum = 0;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g���P�������r�b�g�}�b�v�E�E�B���h�E�ɏ�������
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void PrintLetter_main( FIELD_PLACE_NAME* system )
{
  int idx;
	u16 dx, dy;
  u16 width, height;
  PN_LETTER* letter;
	const GFL_BMP_DATA* sourceBMP = NULL;
	GFL_BMP_DATA* destBMP = NULL;

  idx = system->writeLetterNum;
  GF_ASSERT( idx < GetPlaceNameLength(system) );

  letter    = system->letters[idx];
  sourceBMP = PN_LETTER_GetBitmap( letter );
	destBMP   = GFL_BMPWIN_GetBmp( system->bmpWin );
  dx        = PN_LETTER_GetLeft( letter ) - ( BMPWIN_POS_X_CHAR * CHAR_SIZE );
  dy        = PN_LETTER_GetTop( letter )  - ( BMPWIN_POS_Y_CHAR * CHAR_SIZE );
  width     = PN_LETTER_GetWidth( letter );
  height    = PN_LETTER_GetHeight( letter );

  GFL_BMP_Print( sourceBMP, destBMP, 0, 0, dx, dy, width, height, 0 );
  system->writeLetterNum++;
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
static BOOL PrintLetter_check( const FIELD_PLACE_NAME* system )
{
  int len = GetPlaceNameLength( system ); 
  int num = system->writeLetterNum;

  if( len <= num ) {
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
 * @brief �\���t���O���`�F�b�N����
 *
 * @param system
 *
 * @return �����\���t���O�������Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------------
static BOOL CheckDispFlag( const FIELD_PLACE_NAME* system )
{
  return system->dispFlag;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �\���t���O�𗧂Ă�
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void SetDispFlag( FIELD_PLACE_NAME* system )
{
  system->dispFlag = TRUE;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �\���t���O�𗎂Ƃ�
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void ResetDispFlag( FIELD_PLACE_NAME* system )
{
  system->dispFlag = FALSE;
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
  u32 prevID = ZONEDATA_GetPlaceNameID( system->prevZoneID );
  u32 nowID  = ZONEDATA_GetPlaceNameID( system->nowZoneID );

  // �n���ɕω����Ȃ�
  if( prevID == nowID ) {
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
 * @param system
 */
//-----------------------------------------------------------------------------------
static void UpdatePlaceName( FIELD_PLACE_NAME* system )
{
  u16 zoneID;
	u16 strID;
  
	// �]�[��ID����n����������擾����
  zoneID = GetNowZoneID(system);
	strID = ZONEDATA_GetPlaceNameID( zoneID );

  // �G���[���
	if( (strID < 0) || (msg_place_name_max <= strID) ){ strID = 0; } 

  //�u�Ȃ��̂΂���v�Ȃ�\�����Ȃ�
	if( strID == 0 ) { 
    OS_Printf( "�u�Ȃ��̂΂���v�����o ( zoneID = %d )\n", zoneID );
    FIELD_PLACE_NAME_Hide( system );
    return;
  }

  // �N����ɂ���
  if( CheckIntrudeField( system, zoneID ) ) {
    // �N����̃v���C���[����W�J
    GAME_COMM_SYS_PTR gameComm = GAMESYSTEM_GetGameCommSysPtr( system->gameSystem );
    INTRUDE_COMM_SYS_PTR intrudeComm= Intrude_Check_CommConnect( gameComm );
    u8 intrudeNetID = Intrude_GetPalaceArea( intrudeComm );
    MYSTATUS* status = Intrude_GetMyStatus( intrudeComm, intrudeNetID );
    STRBUF* strbuf = GFL_MSG_CreateString( system->message, MAPNAME_INTRUDE );
    GFL_MSG_GetString( system->message,	strID, system->nameBuf );
    WORDSET_RegisterPlayerName( system->wordset, 0, status );
    WORDSET_RegisterPlaceName( system->wordset, 1, strID );
    WORDSET_ExpandStr( system->wordset, system->nameBuf, strbuf );
    GFL_STR_DeleteBuffer( strbuf );
  }
  // �����̃t�B�[���h�ɂ���
  else {
    GFL_MSG_GetString( system->message,	strID, system->nameBuf );
  }
  system->nameLen = GFL_STR_GetBufferLength( system->nameBuf );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �N���悩�ǂ������`�F�b�N����
 *
 * @param system
 * @param zoneID
 *
 * @return �w�肵���]�[�����N����Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------------
static BOOL CheckIntrudeField( const FIELD_PLACE_NAME* system, u16 zoneID )
{
  GAME_COMM_SYS_PTR gameComm;
  INTRUDE_COMM_SYS_PTR intrudeComm;
  FIELD_STATUS* fieldStatus;
  int myNetID;
  u8 intrudeNetID;

  gameComm = GAMESYSTEM_GetGameCommSysPtr( system->gameSystem );
  intrudeComm = Intrude_Check_CommConnect( gameComm );
  fieldStatus = GAMEDATA_GetFieldStatus( system->gameData );
  myNetID = GAMEDATA_GetIntrudeMyID( system->gameData );

  if( intrudeComm ) {
    intrudeNetID = Intrude_GetPalaceArea( intrudeComm );

    // ���l�̃t�B�[���h�ɂ���
    if( FIELD_STATUS_GetMapMode( fieldStatus ) == MAPMODE_INTRUDE ) { 
      return TRUE;
    }
    // ���l�̃p���X�ɂ���
    else if( ZONEDATA_IsPalace( zoneID ) && (myNetID != intrudeNetID) ) {
      return TRUE;
    }
  }

  return FALSE;
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
  ResetStateCount( system );      // ��ԃJ�E���^�����Z�b�g
  ResetStateSeq( system );        // ��ԓ��V�[�P���X�����Z�b�g

	// �J�ڐ�̏�Ԃɉ�����������
	switch( next_state ) {
		case SYSTEM_STATE_HIDE:	
			GFL_BG_SetVisible( BG_FRAME, VISIBLE_OFF );	// BG���\��
			HideLetters( system ); // �����I�u�W�F�N�g���\���ɂ���
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
  BOOL start = FALSE;

  // �����\��
  if( CheckForceDispFlag(system) ) {
    start = TRUE; 
    ResetForceDispFlag( system );
  } 
  // �\�����N�G�X�g�L
  else if( CheckDispFlag(system) ) { 
    // �n�����ω�
    if( CheckPlaceNameIDChange(system) ) {
      start = TRUE;
    }
    ResetDispFlag( system );
  }

  // �\���J�n
  if( start ) {
    ChangeState( system, SYSTEM_STATE_SETUP );
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
    UpdatePlaceName( system );
    IncStateSeq( system );
    break;

  case 1:
    GFL_BG_ClearCharacter( BG_FRAME );
    GFL_BG_ClearScreen( BG_FRAME );
    FreeBGNullCharaArea( system ); 
    DeleteBandBitmapWindow( system );
    DeleteBandBlankBitmap( system );
    TransBGPalette( system ); 
    AllocBGNullCharArea( system );
    IncStateSeq( system );
    break; 

  case 2:
    CreateBandBlankBitmap( system );
    IncStateSeq( system );
    break; 

  case 3:
    CreateBandBitmapWindow( system );
    RecoverBlankBand( system );
    GFL_BMPWIN_MakeTransWindow( system->bmpWin ); 
    GFL_BG_SetPriority( BG_FRAME, BG_FRAME_PRIORITY );
    GFL_BG_SetVisible( BG_FRAME, VISIBLE_OFF ); 
    G2_SetBlendAlpha( ALPHA_PLANE_1, ALPHA_PLANE_2, ALPHA_VALUE_1, ALPHA_VALUE_2 );
    SetupLetter_init( system );
    IncStateSeq( system );
    break; 

  case 4:
    SetupLetter_main( system ); // �����I�u�W�F�N�g���Z�b�g�A�b�v

    // �S�Ă̕����I�u�W�F�N�g�̃Z�b�g�A�b�v������
    if( SetupLetter_check(system) ) {
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
    GFL_BG_SetVisible( BG_FRAME, VISIBLE_ON ); // BG��\��
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
 * @param system
 */
//-----------------------------------------------------------------------------------
static void Process_WAIT_LAUNCH( FIELD_PLACE_NAME* system )
{
	// ��莞�Ԃ��o�߂�����, ���̏�Ԃ�
	if( PROCESS_TIME_WAIT_LAUNCH < system->stateCount )
	{
		ChangeState( system, SYSTEM_STATE_LAUNCH );
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
      PrintLetter_init( sys );
      IncStateSeq( sys );
    }
    break;

  case 1:
    PrintLetter_main( sys );
    // �S�Ă̕����̏������݊���
    if( PrintLetter_check(sys) ) {
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
 * @param system
 */
//-----------------------------------------------------------------------------------
static void Process_FADEOUT( FIELD_PLACE_NAME* system )
{
	// ��莞�Ԃ��o�߂�����, ���̏�Ԃ�
	if( PROCESS_TIME_FADEOUT < system->stateCount ) {
		ChangeState( system, SYSTEM_STATE_HIDE );
    GFL_BG_SetVisible( BG_FRAME, VISIBLE_OFF );	// BG���\���ɂ���
    HideLetters( system ); // �����I�u�W�F�N�g���\���ɂ���
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
  system->gameSystem    = gameSystem;
  system->gameData      = GAMESYSTEM_GetGameData( gameSystem );
  system->nullCharPos   = AREAMAN_POS_NOTFOUND;
  system->forceDispFlag = FALSE;
  system->dispFlag      = FALSE;
  system->prevZoneID    = ZONE_ID_MAX;
  system->nowZoneID     = ZONE_ID_MAX;

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

  GF_ASSERT( system->message == NULL );

  heapID = GetHeapID( system ); 
	system->message = GFL_MSG_Create( 
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
  if( system->message ) {
    GFL_MSG_Delete( system->message );
    system->message = NULL;
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
  return system->message;
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
 * @brief ���݂̃]�[��ID��ǂݍ���
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void LoadNowZoneID( FIELD_PLACE_NAME* system )
{
  GAMEDATA* gameData;
  SAVE_CONTROL_WORK* save;
  SITUATION* situation;

  gameData  = GAMESYSTEM_GetGameData( system->gameSystem );
  save      = GAMEDATA_GetSaveControlWork( gameData );
  situation = SaveData_GetSituation( save );
  system->nowZoneID = Situation_GetPlaceNameLastDispID( situation );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, 
      "FIELD-PLACE-NAME: LoadNowZoneID (%d)\n", system->nowZoneID );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���݂̃]�[��ID���L�^����
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void SaveNowZoneID( const FIELD_PLACE_NAME* system )
{
  GAMEDATA* gameData;
  SAVE_CONTROL_WORK* save;
  SITUATION* situation;

  gameData  = GAMESYSTEM_GetGameData( system->gameSystem );
  save      = GAMEDATA_GetSaveControlWork( gameData );
  situation = SaveData_GetSituation( save );
  Situation_SetPlaceNameLastDispID( situation, system->nowZoneID );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, 
      "FIELD-PLACE-NAME: SaveNowZoneID (%d)\n", system->nowZoneID );
#endif
}

//-----------------------------------------------------------------------------------
/**
 * @brief �O��̃]�[��ID���擾����
 *
 * @param system
 *
 * @return �Ō�ɕ\�������ꏊ�̃]�[��ID
 */
//-----------------------------------------------------------------------------------
static u16 GetPrevZoneID( const FIELD_PLACE_NAME* system )
{
  return system->prevZoneID;
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���݂̃]�[��ID���擾����
 *
 * @param system
 *
 * @return ���ɕ\������ꏊ�̃]�[��ID
 */
//-----------------------------------------------------------------------------------
static u16 GetNowZoneID( const FIELD_PLACE_NAME* system )
{
  return system->nowZoneID;
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���݂̃]�[��ID��ݒ肷��
 *
 * @param system
 */
//-----------------------------------------------------------------------------------
static void SetNowZoneID( FIELD_PLACE_NAME* system, u16 zoneID )
{
  system->prevZoneID = system->nowZoneID;
  system->nowZoneID  = zoneID;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( DEBUG_PRINT_TARGET, "FIELD-PLACE-NAME: SetNowZoneID (%d)\n", zoneID );
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

//-----------------------------------------------------------------------------------
/**
 * @brief �n���\���t���O���`�F�b�N����
 *
 * @param zoneID �`�F�b�N����]�[��
 *
 * @return �w�肵���]�[���̒n���\���t���O�������Ă���ꍇ TRUE
 */
//-----------------------------------------------------------------------------------
static BOOL CheckPlaceNameFlag( u16 zoneID )
{
  return ZONEDATA_GetPlaceNameFlag( zoneID );
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
