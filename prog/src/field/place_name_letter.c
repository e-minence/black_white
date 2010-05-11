////////////////////////////////////////////////////////////////////////////
/**
 * @file   place_name_letter.c
 * @brief  �n���\���̕����I�u�W�F�N�g
 * @author obata_toshihiro
 * @date   2010.03.26
 */
////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "place_name_letter.h"

#include "print/printsys.h"  // for PRINTSYSS_xxxx



//===========================================================================
// �� �f�o�b�O
//===========================================================================
//#define DEBUG_MODE
#define PRINT_TARGET (3) // �o�͐�


//===========================================================================
// �� �萔�E�}�N��
//=========================================================================== 
#define CHAR_SIZE (8) // 1�L���� = 8�h�b�g

#define BMP_XSIZE_CHR (2) // �r�b�g�}�b�v�� x �T�C�Y ( �L�����P�� )
#define BMP_YSIZE_CHR (2) // �r�b�g�}�b�v�� y �T�C�Y ( �L�����P�� )

#define	COLOR_NO_LETTER (1)  // �����{�̂̃J���[�ԍ�
#define	COLOR_NO_SHADOW (2)  // �e�����̃J���[�ԍ�
#define	COLOR_NO_BACK   (0)  // �w�i���̃J���[�ԍ�



//===========================================================================
// �� �����I�u�W�F�N�g�̈ړ��p�����[�^
//===========================================================================
typedef struct {

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

} PN_LETTER_PARAM;


//===========================================================================
// �������I�u�W�F�N�g
//===========================================================================
struct _PN_LETTER {

  HEAPID          heapID;       // �q�[�vID
  GFL_FONT*       font;         // �t�H���g
	GFL_BMP_DATA*   bmp;		      // �r�b�g�}�b�v
	BMPOAM_ACT_PTR  bmpOamActor;	// BMPOAM�A�N�^�[
	PN_LETTER_PARAM moveParam;    // �ړ��p�����[�^
	u8              width;		    // ������
	u8              height;		    // ��������
	BOOL            moveFlag;		  // �ړ������ǂ���
  STRCODE         strcode;      // �����R�[�h

};


//===========================================================================
// ��index
//========================================================================= ==
// �������E�����E�j��
static PN_LETTER* CreateLetter( HEAPID heapID ); // �����I�u�W�F�N�g�𐶐�����
static void DeleteLetter( PN_LETTER* letter ); // �����I�u�W�F�N�g��j������
static void InitLetter( PN_LETTER* letter ); // �����I�u�W�F�N�g������������
static void CreateBitmap( PN_LETTER* letter ); // �r�b�g�}�b�v�𐶐�����
static void DeleteBitmap( PN_LETTER* letter ); // �r�b�g�}�b�v��j������
static void CreateBmpOamActor( PN_LETTER* letter, BMPOAM_SYS_PTR bmpOamSys, u16 surface, u32 plttRegIdx ); // BMPOAM�A�N�^�[�𐶐�����
static void DeleteBmpOamActor( PN_LETTER* letter ); // BMPOAM�A�N�^�[��j������
// �A�N�Z�b�T
static HEAPID GetHeapID( const PN_LETTER* letter ); // �q�[�vID���擾����
static void SetHeapID( PN_LETTER* letter, HEAPID heapID ); // �q�[�vID��ݒ肷��
static GFL_FONT* GetFont( const PN_LETTER* letter ); // �t�H���g���擾����
static void SetFont( PN_LETTER* letter, GFL_FONT* font ); // �t�H���g��ݒ肷��
static STRCODE GetStrCode( const PN_LETTER* letter ); // �����R�[�h���擾����
static void SetStrCode( PN_LETTER* letter, STRCODE code ); // �����R�[�h��ݒ肷��
static int GetLeft( const PN_LETTER* letter ); // ���� x ���W���擾����
static int GetTop( const PN_LETTER* letter ); // ���� y ���W���擾����
static int GetStrWidth( const PN_LETTER* letter ); // �����̕����擾����
static void SetStrWidth( PN_LETTER* letter, u8 width ); // �����̕���ݒ肷��
static int GetStrHeight( const PN_LETTER* letter ); //�����̍������擾����
static void SetStrHeight( PN_LETTER* letter, u8 height ); //�����̍�����ݒ肷��
static const PN_LETTER_PARAM* GetMoveParam( const PN_LETTER* letter ); // �ړ��p�����[�^���擾����
static void SetMoveParam( PN_LETTER* letter, const PN_LETTER_PARAM* param ); // �ړ��p�����[�^��ݒ肷��
static GFL_BMP_DATA* GetBitmap( const PN_LETTER* letter ); // �r�b�g�}�b�v���擾����
static void CalcPrintSize( PN_LETTER* letter ); // �����T�C�Y���v�Z����
// ����
static BOOL CheckMoving( const PN_LETTER* letter ); // �ړ������ǂ������`�F�b�N����
static BOOL CheckMoveEnd( const PN_LETTER* letter ); // �ړ����I���������ǂ������`�F�b�N����
static BOOL CheckXMoveEnd( const PN_LETTER* letter ); // x �����̈ړ����I���������ǂ������`�F�b�N����
static BOOL CheckYMoveEnd( const PN_LETTER* letter ); // y �����̈ړ����I���������ǂ������`�F�b�N����
// ����
static void LetterMain( PN_LETTER* letter ); // ���C������
static void LetterMoveX( PN_LETTER* letter ); // x �����ɓ�����
static void LetterMoveY( PN_LETTER* letter ); // y �����ɓ�����
// ����
static void StartMove( PN_LETTER* letter ); // �ړ����J�n����
static void EndMove( PN_LETTER* letter ); // �ړ����I������
static void SetupMoveParam( PN_LETTER* letterl ); // �ړ��p�����[�^���Z�b�g�A�b�v����
static void SetupMoveParam_bySetupParam( PN_LETTER* letter, const PN_LETTER_SETUP_PARAM* setup ); // �Z�b�g�A�b�v�p�����[�^����ړ��p�����[�^���Z�b�g�A�b�v����
// �r�b�g�}�b�v����
static void ClearBitmap( PN_LETTER* letter ); // �r�b�g�}�b�v���N���A����
static void PrintLetterToBitmap( PN_LETTER* letter ); // �������r�b�g�}�b�v�ɏ�������
static void UpdateBmpOamActorPos( PN_LETTER* letter ); // BMPOAM�A�N�^�[�̍��W���X�V����
static void SetDrawEnable( PN_LETTER* letter, BOOL enable ); // �\���E��\����ύX����
// ���[�e�B���e�B
static void GetPrintSize( GFL_FONT* font, STRCODE code, HEAPID heapID, u8* destX, u8* destY ); // �����T�C�Y���擾����




//===========================================================================
// ��public method
//===========================================================================


//---------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g�𐶐�����
 *
 * @param heapID     �g�p����q�[�vID
 * @param bmpOamSys  �A�N�^�[��ǉ�����V�X�e��
 * @param plttRegIdx �A�N�^�[�ɓK�p����p���b�g�o�^�C���f�b�N�X
 *
 * @return �������������I�u�W�F�N�g
 */
//---------------------------------------------------------------------------
PN_LETTER* PN_LETTER_Create( 
    HEAPID heapID, BMPOAM_SYS_PTR bmpOamSys, u16 surface, u32 plttRegIdx )
{
  PN_LETTER* letter;

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "��PN_LETTER: create\n" );
#endif

  letter = CreateLetter( heapID ); // �{�̂𐶐�
  InitLetter( letter ); // ������
  SetHeapID( letter, heapID ); // �q�[�vID���Z�b�g
  CreateBitmap( letter ); // �r�b�g�}�b�v�𐶐�
  CreateBmpOamActor( letter, bmpOamSys, surface, plttRegIdx ); // BMPOAM�A�N�^�[�𐶐�
  SetDrawEnable( letter, FALSE ); // ��\���ɂ���

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "��PN_LETTER: create\n" );
#endif

  return letter;
}

//---------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g��j������
 *
 * @param letter
 */
//---------------------------------------------------------------------------
void PN_LETTER_Delete( PN_LETTER* letter )
{
  GF_ASSERT( letter );

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "��PN_LETTER: delete\n" );
#endif

  DeleteBmpOamActor( letter ); // BMPOAM�A�N�^�[��j��
  DeleteBitmap( letter ); // �r�b�g�}�b�v��j��
  DeleteLetter( letter ); // �{�̂�j��

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "��PN_LETTER: delete\n" );
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g���Z�b�g�A�b�v����
 *
 * @param letter
 * @param param �Z�b�g�A�b�v�E�p�����[�^
 */
//---------------------------------------------------------------------------
void PN_LETTER_Setup( PN_LETTER* letter, const PN_LETTER_SETUP_PARAM* param )
{
  GF_ASSERT( letter );

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "��PN_LETTER: setup\n" );
#endif

  SetFont( letter, param->font );
  SetStrCode( letter, param->code );
  CalcPrintSize( letter );
  ClearBitmap( letter );
  PrintLetterToBitmap( letter );
  SetupMoveParam_bySetupParam( letter, param );

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "��PN_LETTER: setup\n" );
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g�̃��C������
 *
 * @param letter
 */
//---------------------------------------------------------------------------
void PN_LETTER_Main( PN_LETTER* letter )
{
  GF_ASSERT( letter );

  LetterMain( letter );
}

//---------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g�̈ړ����J�n����
 *
 * @param letter
 */
//---------------------------------------------------------------------------
void PN_LETTER_MoveStart( PN_LETTER* letter )
{
  GF_ASSERT( letter );

  StartMove( letter );
}

//---------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g�̃r�b�g�}�b�v���擾����
 *
 * @param letter
 */
//---------------------------------------------------------------------------
const GFL_BMP_DATA* PN_LETTER_GetBitmap( const PN_LETTER* letter )
{
  GF_ASSERT( letter );
  GF_ASSERT( letter->bmp );

  return GetBitmap( letter );
}

//---------------------------------------------------------------------------
/**
 * @brief �\���E��\����ύX����
 *
 * @param letter
 * @param enable �\������ꍇ TRUE
 */
//---------------------------------------------------------------------------
void PN_LETTER_SetDrawEnable( PN_LETTER* letter, BOOL enable )
{
  SetDrawEnable( letter, enable );
}

//---------------------------------------------------------------------------
/**
 * @brief ���������擾����
 *
 * @param letter
 *
 * @return ������
 */
//---------------------------------------------------------------------------
int PN_LETTER_GetWidth( const PN_LETTER* letter )
{
  return GetStrWidth( letter );
}

//---------------------------------------------------------------------------
/**
 * @brief �����������擾����
 *
 * @param letter
 *
 * @return ��������
 */
//---------------------------------------------------------------------------
int PN_LETTER_GetHeight( const PN_LETTER* letter )
{
  return GetStrHeight( letter );
}

//---------------------------------------------------------------------------
/**
 * @brief ���� x ���W���擾����
 *
 * @param letter
 *
 * @return ���� x ���W
 */
//---------------------------------------------------------------------------
int PN_LETTER_GetLeft( const PN_LETTER* letter )
{
  return GetLeft( letter );
}

//---------------------------------------------------------------------------
/**
 * @brief ���� y ���W���擾����
 *
 * @param letter
 *
 * @return ���� y ���W
 */
//---------------------------------------------------------------------------
int PN_LETTER_GetTop( const PN_LETTER* letter )
{
  return GetTop( letter );
}

//---------------------------------------------------------------------------
/**
 * @brief �ړ������ǂ������`�F�b�N����
 *
 * @param letter
 *
 * @return �ړ����Ȃ� TRUE, �����łȂ��Ȃ� FALSE.
 */
//---------------------------------------------------------------------------
BOOL PN_LETTER_IsMoving( const PN_LETTER* letter )
{
  return CheckMoving( letter );
}



//===========================================================================
// ��private method
//===========================================================================


//---------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g�𐶐�����
 *
 * @param letter
 *
 * @return �������������I�u�W�F�N�g
 */
//---------------------------------------------------------------------------
static PN_LETTER* CreateLetter( HEAPID heapID )
{
  PN_LETTER* letter;

  letter = GFL_HEAP_AllocMemory( heapID, sizeof(PN_LETTER) );

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "PN_LETTER: create letter\n" );
#endif

  return letter;
}

//---------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g��j������
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void DeleteLetter( PN_LETTER* letter )
{
  GFL_HEAP_FreeMemory( letter );

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "PN_LETTER: delete letter\n" );
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g������������
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void InitLetter( PN_LETTER* letter )
{
  // �[���N���A
  GFL_STD_MemClear( letter, sizeof(PN_LETTER) );

  // ������
  letter->moveFlag = FALSE;
  letter->strcode  = GFL_STR_GetEOMCode();

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "PN_LETTER: init letter\n" );
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�𐶐�����
 *
 * @param letter
 * @param heapID
 */
//---------------------------------------------------------------------------
static void CreateBitmap( PN_LETTER* letter )
{
  GF_ASSERT( letter->bmp == NULL );

	letter->bmp = GFL_BMP_Create( 
      BMP_XSIZE_CHR, BMP_YSIZE_CHR, GFL_BMP_16_COLOR, letter->heapID );

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "PN_LETTER: create bitmap\n" );
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v��j������
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void DeleteBitmap( PN_LETTER* letter )
{
  if( letter->bmp ) {
    GFL_BMP_Delete( letter->bmp );
    letter->bmp = NULL;
  } 

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "PN_LETTER: delete bitmap\n" );
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief BMPOAM�A�N�^�[�𐶐�����
 *
 * @param letter
 * @param bmpOamSys  �A�N�^�[��ǉ�����V�X�e��
 * @param plttRegIdx �K�p����p���b�g�̓o�^�C���f�b�N�X
 */
//---------------------------------------------------------------------------
static void CreateBmpOamActor( 
    PN_LETTER* letter, BMPOAM_SYS_PTR bmpOamSys, u16 surface, u32 plttRegIdx )
{
	BMPOAM_ACT_DATA actorData;

  GF_ASSERT( letter->bmp );
  GF_ASSERT( letter->bmpOamActor == NULL );

	actorData.bmp        = letter->bmp;
	actorData.x          = 0;
	actorData.y          = 0;
	actorData.pltt_index = plttRegIdx;
	actorData.pal_offset = 0;
	actorData.soft_pri   = 0;
	actorData.bg_pri     = 0;
	actorData.setSerface = surface;
	actorData.draw_type  = CLSYS_DRAW_MAIN; 

  // �A�N�^�[��ǉ�
	letter->bmpOamActor = BmpOam_ActorAdd( bmpOamSys, &actorData ); 

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "PN_LETTER: create BMPOAM actor\n" );
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief BMPOAM�A�N�^�[��j������
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void DeleteBmpOamActor( PN_LETTER* letter )
{
  if( letter->bmpOamActor ) {
    BmpOam_ActorDel( letter->bmpOamActor );
    letter->bmpOamActor = NULL;
  } 

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "PN_LETTER: delete BMPOAM actor\n" );
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief �q�[�vID���擾����
 *
 * @param letter
 *
 * @return �q�[�vID
 */
//---------------------------------------------------------------------------
static HEAPID GetHeapID( const PN_LETTER* letter )
{
  return letter->heapID;
}

//---------------------------------------------------------------------------
/**
 * @brief �q�[�vID��ݒ肷��
 *
 * @param letter
 * @param heapID
 */
//---------------------------------------------------------------------------
static void SetHeapID( PN_LETTER* letter, HEAPID heapID )
{
  letter->heapID = heapID;

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "PN_LETTER: set heapID\n" );
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief �t�H���g���擾����
 *
 * @param letter
 *
 * @return �t�H���g
 */
//---------------------------------------------------------------------------
static GFL_FONT* GetFont( const PN_LETTER* letter )
{
  GF_ASSERT( letter->font );

  return letter->font;
}

//---------------------------------------------------------------------------
/**
 * @brief �t�H���g��ݒ肷��
 *
 * @param letter
 * @param font   �t�H���g
 */
//---------------------------------------------------------------------------
static void SetFont( PN_LETTER* letter, GFL_FONT* font )
{
  letter->font = font;

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "PN_LETTER: set font\n" );
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief �����R�[�h���擾����
 *
 * @param letter
 *
 * @return �����R�[�h
 */
//---------------------------------------------------------------------------
static STRCODE GetStrCode( const PN_LETTER* letter )
{
  return letter->strcode;
}

//---------------------------------------------------------------------------
/**
 * @brief �����R�[�h��ݒ肷��
 *
 * @param letter 
 * @param code   �����R�[�h
 */
//---------------------------------------------------------------------------
static void SetStrCode( PN_LETTER* letter, STRCODE code )
{
  letter->strcode = code;

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "PN_LETTER: set strcode\n" );
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief �ړ������ǂ������`�F�b�N����
 *
 * @param letter
 *
 * @return �ړ����Ȃ� TRUE, �����łȂ��Ȃ� FALSE.
 */
//---------------------------------------------------------------------------
static BOOL CheckMoving( const PN_LETTER* letter )
{
  return letter->moveFlag;
}

//---------------------------------------------------------------------------
/**
 * @brief ���� x ���W���擾����
 *
 * @param letter
 *
 * @return ���� x ���W
 */
//---------------------------------------------------------------------------
static int GetLeft( const PN_LETTER* letter )
{
  return letter->moveParam.x;
}

//---------------------------------------------------------------------------
/**
 * @brief ���� y ���W���擾����
 *
 * @param letter
 *
 * @return ���� y ���W
 */
//---------------------------------------------------------------------------
static int GetTop( const PN_LETTER* letter )
{
  return letter->moveParam.y;
}

//---------------------------------------------------------------------------
/**
 * @brief �����̕����擾����
 *
 * @param letter
 *
 * @return �����̕�
 */
//---------------------------------------------------------------------------
static int GetStrWidth( const PN_LETTER* letter )
{
  return letter->width;
}

//---------------------------------------------------------------------------
/**
 * @brief �����̕���ݒ肷��
 *
 * @param letter
 * @param width  �����̕�
 */
//---------------------------------------------------------------------------
static void SetStrWidth( PN_LETTER* letter, u8 width )
{
  letter->width = width;
}

//---------------------------------------------------------------------------
/**
 * @brief �����̍������擾����
 *
 * @param letter
 *
 * @return �����̍���
 */
//---------------------------------------------------------------------------
static int GetStrHeight( const PN_LETTER* letter )
{
  return letter->height;
}

//---------------------------------------------------------------------------
/**
 * @brief �����̍�����ݒ肷��
 *
 * @param letter
 * @param height  �����̍���
 */
//---------------------------------------------------------------------------
static void SetStrHeight( PN_LETTER* letter, u8 height )
{
  letter->height = height;
}

//---------------------------------------------------------------------------
/**
 * @brief �ړ��p�����[�^���擾����
 *
 * @param letter
 *
 * @return �ړ��p�����[�^
 */
//---------------------------------------------------------------------------
static const PN_LETTER_PARAM* GetMoveParam( const PN_LETTER* letter )
{
  return &(letter->moveParam);
}

//---------------------------------------------------------------------------
/**
 * @brief �ړ��p�����[�^��ݒ肷��
 *
 * @param letter
 * @param param �ړ��p�����[�^
 */
//---------------------------------------------------------------------------
static void SetMoveParam( PN_LETTER* letter, const PN_LETTER_PARAM* param )
{
  letter->moveParam = *param;
}

//---------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^���擾����
 *
 * @param letter
 *
 * @return �r�b�g�}�b�v
 */
//---------------------------------------------------------------------------
static GFL_BMP_DATA* GetBitmap( const PN_LETTER* letter )
{
  return letter->bmp;
}

//---------------------------------------------------------------------------
/**
 * @brief �����T�C�Y���v�Z����
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void CalcPrintSize( PN_LETTER* letter )
{
  GFL_FONT* font;
  STRCODE code;
  HEAPID heapID;

  font   = GetFont( letter );
  code   = GetStrCode( letter );
  heapID = GetHeapID( letter );

  GetPrintSize( font, code, heapID, &letter->width, &letter->height );

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "PN_LETTER: calc print size. (w=%d, h=%d)\n", 
      GetStrWidth(letter), GetStrHeight(letter) );
#endif
} 

//---------------------------------------------------------------------------
/**
 * @brief �ړ����I���������ǂ������`�F�b�N����
 *
 * @param letter
 *
 * @return �ړ����I�������ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//---------------------------------------------------------------------------
static BOOL CheckMoveEnd( const PN_LETTER* letter )
{ 
  // �ړ����Ă��Ȃ�
  if( CheckMoving(letter) == FALSE ) { return TRUE; }

  // x ����, y ���� �Ƃ��Ɉړ����I��
	if( CheckXMoveEnd(letter) && CheckYMoveEnd(letter) ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//---------------------------------------------------------------------------
/**
 * @brief x �����̈ړ����I���������ǂ������`�F�b�N����
 *
 * @param letter
 *
 * @return �ړ����I�������ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//---------------------------------------------------------------------------
static BOOL CheckXMoveEnd( const PN_LETTER* letter )
{
  const PN_LETTER_PARAM* param;

  param = GetMoveParam( letter );

  // �ړ����Ă��Ȃ�
  if( CheckMoving(letter) == FALSE ) { return TRUE; }

  // x �����̈ړ����I��
	if( param->tx <= 0 ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//---------------------------------------------------------------------------
/**
 * @brief y �����̈ړ����I���������ǂ������`�F�b�N����
 *
 * @param letter
 *
 * @return �ړ����I�������ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//---------------------------------------------------------------------------
static BOOL CheckYMoveEnd( const PN_LETTER* letter )
{
  const PN_LETTER_PARAM* param;

  param = GetMoveParam( letter );

  // �ړ����Ă��Ȃ�
  if( CheckMoving(letter) == FALSE ) { return TRUE; }

  // y �����̈ړ����I��
	if( param->ty <= 0 ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//---------------------------------------------------------------------------
/**
 * @brief ���C������
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void LetterMain( PN_LETTER* letter )
{
	// �����Ă��Ȃ�
	if( CheckMoving(letter) == FALSE ) { return; }

  // �ړ�
  LetterMoveX( letter );
  LetterMoveY( letter );

	// BMPOAM�A�N�^�[�̕\���ʒu���X�V
  UpdateBmpOamActorPos( letter );

	// �ړ�������
	if( CheckMoveEnd(letter) == TRUE ) {
    EndMove( letter );
	} 
}

//---------------------------------------------------------------------------
/**
 * @brief x �����ɓ�����
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void LetterMoveX( PN_LETTER* letter )
{
  PN_LETTER_PARAM* param;

  GF_ASSERT( CheckMoving(letter) == TRUE );

  // �ړ������ς�
  if( CheckXMoveEnd(letter) == TRUE ) { return; }

  param = &(letter->moveParam);

  param->x  += param->sx; // �ړ�
  param->sx += param->ax; // ����
  param->tx--;            // �c��ړ��񐔂��f�N�������g
}

//---------------------------------------------------------------------------
/**
 * @brief y �����ɓ�����
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void LetterMoveY( PN_LETTER* letter )
{
  PN_LETTER_PARAM* param;

  GF_ASSERT( CheckMoving(letter) == TRUE );

  // �ړ������ς�
  if( CheckYMoveEnd(letter) == TRUE ) { return; }

  param = &(letter->moveParam);
  
  param->y  += param->sy; // �ړ�
  param->sy += param->ay; // ����
  param->ty--;            // �c��ړ��񐔂��f�N�������g
} 

//---------------------------------------------------------------------------
/**
 * @brief �ړ����J�n����
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void StartMove( PN_LETTER* letter )
{
  letter->moveFlag = TRUE;

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "PN_LETTER: start move\n" );
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief �ړ����I������
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void EndMove( PN_LETTER* letter )
{
  letter->moveFlag = FALSE;

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "PN_LETTER: end move\n" );
#endif
}

//--------------------------------------------------------------------------
/**
 * @brief �ړ��p�����[�^���Z�b�g�A�b�v����
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void SetupMoveParam( PN_LETTER* letter )
{
	float xDist;
	float yDist;
  PN_LETTER_PARAM* param;

  param = &(letter->moveParam);

  // �ړ��������v�Z
	xDist = param->dx - param->x;
	yDist = param->dy - param->y;

	// �����x���v�Z
	if( xDist != 0 ) {
    param->ax = 
      ( ( param->dsx * param->dsx ) - ( param->sx * param->sx ) ) / ( 2 * xDist );
  }
	if( yDist != 0 ) {
    param->ay = 
      ( ( param->dsy * param->dsy ) - ( param->sy * param->sy ) ) / ( 2 * yDist );
  }

	// �ړ��񐔂��v�Z
	if( param->ax != 0 ) {
    param->tx = (int)( ( param->dsx - param->sx ) / param->ax );
  }
	if( param->ay != 0 ) {
    param->ty = (int)( ( param->dsy - param->sy ) / param->ay );
  }

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "PN_LETTER: setup move param\n" );
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief �Z�b�g�A�b�v�p�����[�^����ړ��p�����[�^�����߂�
 *
 * @param letter
 * @param setup �Z�b�g�A�b�v�p�����[�^
 */
//---------------------------------------------------------------------------
static void SetupMoveParam_bySetupParam( 
    PN_LETTER* letter, const PN_LETTER_SETUP_PARAM* setup )
{
  // �ړ��p�����[�^�̃Z�b�g�A�b�v�ɕK�v�ȃf�[�^��ݒ�
  letter->moveParam.x = setup->x;
  letter->moveParam.y = setup->y;
  letter->moveParam.sx = setup->sx;
  letter->moveParam.sy = setup->sy;
  letter->moveParam.dx = setup->dx;
  letter->moveParam.dy = setup->dy;
  letter->moveParam.dsx = setup->dsx;
  letter->moveParam.dsy = setup->dsy;

  // �ړ��p�����[�^���Z�b�g�A�b�v
  SetupMoveParam( letter );

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "PN_LETTER: setup move param by setup param\n" );
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v���N���A����
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void ClearBitmap( PN_LETTER* letter )
{
	GFL_BMP_Clear( letter->bmp, 0 );

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "PN_LETTER: create bitmap\n" );
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief �������r�b�g�}�b�v�ɏ�������
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void PrintLetterToBitmap( PN_LETTER* letter )
{
	STRCODE string[2];
	STRBUF* strbuf;
  PRINTSYS_LSB color;

	string[0] = GetStrCode( letter );
	string[1] = GFL_STR_GetEOMCode();
	strbuf    = GFL_STR_CreateBuffer( 2, letter->heapID );
	GFL_STR_SetStringCodeOrderLength( strbuf, string, 2 );

  color = PRINTSYS_LSB_Make( COLOR_NO_LETTER, COLOR_NO_SHADOW, COLOR_NO_BACK );

	PRINTSYS_PrintColor( letter->bmp, 0, 0, strbuf, letter->font, color );
	BmpOam_ActorBmpTrans( letter->bmpOamActor );

	GFL_STR_DeleteBuffer( strbuf );

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "PN_LETTER: print letter to bitmap\n" );
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief BMPOAM�A�N�^�[�̍��W���X�V����
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void UpdateBmpOamActorPos( PN_LETTER* letter )
{
  const PN_LETTER_PARAM* param;

  param = GetMoveParam( letter );

	BmpOam_ActorSetPos( letter->bmpOamActor, param->x, param->y );
}

//---------------------------------------------------------------------------
/**
 * @brief �\���E��\����ύX����
 *
 * @param letter
 * @param enable �\������ꍇ TRUE
 */
//---------------------------------------------------------------------------
static void SetDrawEnable( PN_LETTER* letter, BOOL enable )
{
	BmpOam_ActorSetDrawEnable( letter->bmpOamActor, enable );  
} 

//---------------------------------------------------------------------------
/**
 * @brief �����T�C�Y���擾����
 *
 * @param font   �t�H���g
 * @param code   �T�C�Y���擾���镶��
 * @param heapID �ꎞ�g�p����q�[�vID
 * @param destX  �������̊i�[��
 * @param destY  �������̊i�[��
 */
//---------------------------------------------------------------------------
static void GetPrintSize( 
    GFL_FONT* font, STRCODE code, HEAPID heapID, u8* destX, u8* destY )
{
	STRCODE strcode[2];
	STRBUF* strbuf;

	strcode[0] = code;
	strcode[1] = GFL_STR_GetEOMCode();
	strbuf     = GFL_STR_CreateBuffer( 2, heapID );
	GFL_STR_SetStringCodeOrderLength( strbuf, strcode, 2 );

	// �T�C�Y�擾
	*destX = PRINTSYS_GetStrWidth( strbuf, font, 0 );
	*destY = PRINTSYS_GetStrHeight( strbuf, font );

	GFL_STR_DeleteBuffer( strbuf );
} 
