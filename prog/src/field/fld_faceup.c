//============================================================================================
/**
 * @file	fld_faceup.c
 * @brief	��A�b�v
 */
//============================================================================================
#include "arc_def.h"
#include "gamesystem/game_event.h"
#include "system/main.h"    //for HEAPID_FLD3DCUTIN 
#include "field_bg_def.h"
#include "field_place_name.h" //for   FIELD_PLACE_NAME
#include "field_debug.h"      //for FIELD_DEBUG�`
#include "fld_faceup.h"
#include "fieldmap.h"

#include "arc/fieldmap/fld_faceup.naix"

#define BG_PLT_NO (9)
#define FACE_PLT_NO (0)
#define FACE_PLT_NUM  (2) 

#define SCR_SIZE_W  (32)
#define SCR_SIZE_H  (24)

typedef struct FACEUP_WORK_tag
{
  HEAPID HeapID;
  u8 BgPriority[4];
  GXPlaneMask Mask;
  GXBg23ControlText CntText;

  int BackNo;       //�w�iBG

  //��BG
}FACEUP_WORK;


static GMEVENT_RESULT SetupEvt( GMEVENT* event, int* seq, void* work );
static void Setup(FACEUP_WK_PTR ptr, FIELDMAP_WORK *fieldmap);
static GMEVENT_RESULT ReleaseEvt( GMEVENT* event, int* seq, void* work );
static void Release(FIELDMAP_WORK * fieldmap, FACEUP_WK_PTR ptr);

static void PushPriority(FACEUP_WK_PTR ptr);
static void PushDisp(FACEUP_WK_PTR ptr);
static void PopPriority(FACEUP_WK_PTR ptr);
static void PopDisp(FACEUP_WK_PTR ptr);

static void ChangeScreenPlt( void *rawData, u8 px, u8 py, u8 sx, u8 sy, u8 pal );

GMEVENT *FLD_FACEUP_Start(const int inBackNo, const int inCharNo, GAMESYS_WORK *gsys)
{
  GMEVENT * event;
  HEAPID heapID;
  FACEUP_WK_PTR ptr;

  FIELDMAP_WORK *fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);

  heapID = HEAPID_FLD3DCUTIN;
  //���[�N���A���P�[�V�������āA�t�F�C�X�A�b�v�f�[�^�|�C���^�ɃZ�b�g
  ptr = GFL_HEAP_AllocClearMemory(heapID, sizeof(FACEUP_WORK));   //�J�b�g�C���q�[�v����A���P�[�g

  if ( *FIELDMAP_GetFaceupWkPtrAdr(fieldmap) != NULL )
  {
    GF_ASSERT(0);
    return NULL;
  }

  *FIELDMAP_GetFaceupWkPtrAdr(fieldmap) = ptr;
  ptr->HeapID = heapID;
  ptr->BackNo = inBackNo;

  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, SetupEvt, 0 );
  return event;
}

//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�C�x���g
 * @param     event	            �C�x���g�|�C���^
 * @param     seq               �V�[�P���T
 * @param     work              ���[�N�|�C���^
 * @return    GMEVENT_RESULT   �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT_RESULT SetupEvt( GMEVENT* event, int* seq, void* work )
{
  FACEUP_WK_PTR ptr;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);

  ptr = *FIELDMAP_GetFaceupWkPtrAdr(fieldmap);

  switch(*seq){
  case 0:
    //�u���b�N�A�E�g�J�n
    GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 0, 16, 0 );
    (*seq)++;
    break;
  case 1:
    //�u���b�N�A�E�g�҂�
    if ( GFL_FADE_CheckFade() ) break;
    //�Z�b�g�A�b�v
    Setup(ptr, fieldmap);
    //�u���b�N�C���J�n
    GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 16, 0, 0 );
    (*seq)++;
    break;
  case 2:
    if ( GFL_FADE_CheckFade() == FALSE ){
      return GMEVENT_RES_FINISH;
    }
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v
 * @param     ptr	            ���[�N�|�C���^
 * @return    none
 */
//--------------------------------------------------------------
static void Setup(FACEUP_WK_PTR ptr, FIELDMAP_WORK *fieldmap)
{
  int ncgr;
  int nscr;
  //�v���C�I���e�B�ۑ�
  PushPriority(ptr);
  //�\����Ԃ̕ۑ�
  PushDisp(ptr);
  //BG�ݒ�ۑ�
  ptr->CntText = G2_GetBG3ControlText();
  
  //�a�f3�Z�b�g�A�b�v�i��BG256�F�j
  G2_SetBG3ControlText(
      GX_BG_SCRSIZE_TEXT_256x256,
      GX_BG_COLORMODE_256,
      GX_BG_SCRBASE_0x1000,
      GX_BG_CHARBASE_0x04000);

  G2_BlendNone();

  { //BG���\�[�X���N���A
    BOOL rc;
    FLDMSGBG *fmb = FIELDMAP_GetFldMsgBG( fieldmap );
    rc = FLDMSGBG_ReleaseBG2Resource( fmb );
    if (rc) FLDMSGBG_ReqResetBG2( fmb );
    else
    {
      //���݂̏�Ԃ����
      GFL_BG_FreeBGControl( FLDBG_MFRM_EFF1 );
    }
    //�Z�b�g���Ȃ���
    {
      //BG Frame
		  GFL_BG_BGCNT_HEADER bgcntText = {
		   0, 0, FLDBG_MFRM_EFF1_SCRSIZE, 0,
		   GFL_BG_SCRSIZ_256x256, FLDBG_MFRM_EFF1_COLORMODE,
		   FLDBG_MFRM_EFF1_SCRBASE, FLDBG_MFRM_EFF1_CHARBASE, FLDBG_MFRM_EFF1_CHARSIZE,
		   GX_BG_EXTPLTT_01, FLDBG_MFRM_EFF1_PRI, 0, 0, FALSE
		  };
      GFL_BG_SetBGControl( FLDBG_MFRM_EFF1, &bgcntText, GFL_BG_MODE_TEXT );
    }
	}

  if (ptr->BackNo == 0)
  {
    ncgr = NARC_fld_faceup_faceup_bg_white_ncgr;
    nscr = NARC_fld_faceup_faceup_bg_white_nscr;
  }
  else
  {
    ncgr = NARC_fld_faceup_faceup_bg_black_ncgr;
    nscr = NARC_fld_faceup_faceup_bg_black_nscr;
  }
  //�^�C�v���Ƃ̔w�i�A��BG�����[�h
  {
    void *buf;    
    //�w�i
    //�L����
    {
      NNSG2dCharacterData *chr;
		  buf = GFL_ARC_LoadDataAlloc( ARCID_FLD_FACEUP, ncgr, ptr->HeapID );
		  GF_ASSERT( buf != NULL );
      if( NNS_G2dGetUnpackedBGCharacterData(buf,&chr) == FALSE ){
        GF_ASSERT( 0 );
      }
      GFL_BG_LoadCharacter( FLDBG_MFRM_EFF1, chr->pRawData, chr->szByte, 0 );
      GFL_HEAP_FreeMemory( buf );
    }    
    //�X�N���[��
    {
      NNSG2dScreenData *scr;
      buf = GFL_ARC_LoadDataAlloc( ARCID_FLD_FACEUP, nscr, ptr->HeapID );
      GF_ASSERT( buf != NULL );
      if( NNS_G2dGetUnpackedScreenData(buf,&scr) == FALSE ){
        GF_ASSERT( 0 );
      }
      ChangeScreenPlt( scr->rawData, 0, 0, SCR_SIZE_W, SCR_SIZE_H, BG_PLT_NO );
      GFL_BG_LoadScreen( FLDBG_MFRM_EFF1, scr->rawData, scr->szByte, 0 );
      GFL_HEAP_FreeMemory( buf );
    }
    //�p���b�g
    {
      NNSG2dPaletteData *pal;
      buf = GFL_ARC_LoadDataAlloc( ARCID_FLD_FACEUP, NARC_fld_faceup_faceup_bg_nclr, ptr->HeapID );
      GF_ASSERT( buf != NULL );
      if( NNS_G2dGetUnpackedPaletteData(buf,&pal) == FALSE ){
        GF_ASSERT( 0 );
      }
      GFL_BG_LoadPalette( FLDBG_MFRM_EFF1, pal->pRawData, 32, BG_PLT_NO*32 );
      GFL_HEAP_FreeMemory( buf );
    }
    //��
    //�L����
    {
      NNSG2dCharacterData *chr;
      buf = GFL_ARC_LoadDataAlloc( ARCID_FLD_FACEUP, NARC_fld_faceup_faceup_ncgr, ptr->HeapID );
		  GF_ASSERT( buf != NULL );
      if( NNS_G2dGetUnpackedBGCharacterData(buf,&chr) == FALSE ){
        GF_ASSERT( 0 );
      }
      GFL_BG_LoadCharacter( FLDBG_MFRM_EFF2, chr->pRawData, chr->szByte, 0 );
      GFL_HEAP_FreeMemory( buf );
    }
    //�X�N���[��
    {
      NNSG2dScreenData *scr;
      buf = GFL_ARC_LoadDataAlloc( ARCID_FLD_FACEUP, NARC_fld_faceup_faceup_nscr, ptr->HeapID );
      GF_ASSERT( buf != NULL );
      if( NNS_G2dGetUnpackedScreenData(buf,&scr) == FALSE ){
        GF_ASSERT( 0 );
      }
      GFL_BG_LoadScreen( FLDBG_MFRM_EFF2, scr->rawData, scr->szByte, 0 );
      GFL_HEAP_FreeMemory( buf );
    }
    //�p���b�g
    {
      NNSG2dPaletteData *pal;
      buf = GFL_ARC_LoadDataAlloc( ARCID_FLD_FACEUP, NARC_fld_faceup_faceup_nclr, ptr->HeapID );
      GF_ASSERT( buf != NULL );
      if( NNS_G2dGetUnpackedPaletteData(buf,&pal) == FALSE ){
        GF_ASSERT( 0 );
      }
      GFL_BG_LoadPalette( FLDBG_MFRM_EFF2, pal->pRawData, FACE_PLT_NUM*32, FACE_PLT_NO*32 );
      GFL_HEAP_FreeMemory( buf );
    }
  }

  //�v���C�I���e�B
  GFL_BG_SetPriority( GFL_BG_FRAME0_M, 3 );
  GFL_BG_SetPriority( GFL_BG_FRAME1_M, 0 );
  GFL_BG_SetPriority( GFL_BG_FRAME2_M, 2 );
  GFL_BG_SetPriority( GFL_BG_FRAME3_M, 1 );

  //�\��
  GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_ON );
}

GMEVENT *FLD_FACEUP_End(GAMESYS_WORK *gsys)
{
  GMEVENT * event;
  FIELDMAP_WORK *fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
  if ( *FIELDMAP_GetFaceupWkPtrAdr(fieldmap) == NULL )
  {
    GF_ASSERT(0);
    return NULL;
  }

  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, ReleaseEvt, 0 );
  return event;
}

void FLD_FACEUP_Release( FIELDMAP_WORK *fieldmap )
{
  FACEUP_WK_PTR ptr;
  ptr = *FIELDMAP_GetFaceupWkPtrAdr(fieldmap);
  Release(fieldmap, ptr);
}

//--------------------------------------------------------------
/**
 * �����[�X�C�x���g
 * @param     event	            �C�x���g�|�C���^
 * @param     seq               �V�[�P���T
 * @param     work              ���[�N�|�C���^
 * @return    GMEVENT_RESULT   �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT_RESULT ReleaseEvt( GMEVENT* event, int* seq, void* work )
{
  FACEUP_WK_PTR ptr;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);

  ptr = *FIELDMAP_GetFaceupWkPtrAdr(fieldmap);

  switch(*seq){
  case 0:
    //�u���b�N�A�E�g�J�n
    GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 0, 16, 0 );
    (*seq)++;
    break;
  case 1:
    //�u���b�N�A�E�g�҂�
    if ( GFL_FADE_CheckFade() ) break;
    //�����[�X
    Release(fieldmap, ptr);
    //�u���b�N�C���J�n
    GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 16, 0, 0 );
    (*seq)++;
    break;
  case 2:
    if ( GFL_FADE_CheckFade() == FALSE ){
      return GMEVENT_RES_FINISH;
    }
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �����[�X
 * @param     event	            �C�x���g�|�C���^
 * @param     seq               �V�[�P���T
 * @param     work              ���[�N�|�C���^
 * @return    GMEVENT_RESULT   �C�x���g����
 */
//--------------------------------------------------------------
static void Release(FIELDMAP_WORK * fieldmap, FACEUP_WK_PTR ptr)
{
  FIELD_PLACE_NAME *place_name_sys = FIELDMAP_GetPlaceNameSys( fieldmap );
  FIELD_DEBUG_WORK *debug = FIELDMAP_GetDebugWork( fieldmap );

  GFL_BG_ClearFrame( GFL_BG_FRAME2_M );

  //BG�ݒ蕜�A
  G2_SetBG3ControlText(
      ptr->CntText.screenSize,
      ptr->CntText.colorMode,
      ptr->CntText.screenBase,
      ptr->CntText.charBase);
  //���A
  FIELD_PLACE_NAME_RecoverBG( place_name_sys );
  FIELD_DEBUG_RecoverBgCont( debug );

  //�v���C�I���e�B���A
  PopPriority(ptr);
  //�\����ԕ��A
  PopDisp(ptr);
  //���[�N���
  GFL_HEAP_FreeMemory( ptr );
  *FIELDMAP_GetFaceupWkPtrAdr(fieldmap) = NULL;
}

//�L�����ύX
void FLD_FACEUP_Change(void)
{
  void *buf;
  HEAPID heapID = HEAPID_FLD3DCUTIN;
  //�L����
  {
    NNSG2dCharacterData *chr;
    buf = GFL_ARC_LoadDataAlloc( ARCID_FLD_FACEUP, NARC_fld_faceup_faceup_ncgr, heapID );
		GF_ASSERT( buf != NULL );
    if( NNS_G2dGetUnpackedBGCharacterData(buf,&chr) == FALSE ){
      GF_ASSERT( 0 );
    }
    GFL_BG_LoadCharacter( FLDBG_MFRM_EFF2, chr->pRawData, chr->szByte, 0 );
    GFL_HEAP_FreeMemory( buf );
  }
  
  //�p���b�g
  {
    NNSG2dPaletteData *pal;
    buf = GFL_ARC_LoadDataAlloc( ARCID_FLD_FACEUP, NARC_fld_faceup_faceup_nclr, heapID );
    GF_ASSERT( buf != NULL );
    if( NNS_G2dGetUnpackedPaletteData(buf,&pal) == FALSE ){
      GF_ASSERT( 0 );
    }
    GFL_BG_LoadPalette( FLDBG_MFRM_EFF2, pal->pRawData, FACE_PLT_NUM*32, FACE_PLT_NO*32 );
    GFL_HEAP_FreeMemory( buf );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���C�I���e�B�v�b�V��
 *
 *	@param	ptr       ���[�N�|�C���^
 *	@return none
 */
//-----------------------------------------------------------------------------
static void PushPriority(FACEUP_WK_PTR ptr)
{
  int i;
  for(i=0;i<4;i++){
    ptr->BgPriority[i] = GFL_BG_GetPriority( i );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�\����ԃv�b�V��
 *
 *	@param	ptr       ���[�N�|�C���^
 *	@return none
 */
//-----------------------------------------------------------------------------
static void PushDisp(FACEUP_WK_PTR ptr)
{
  ptr->Mask = GFL_DISP_GetMainVisible();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���C�I���e�B�|�b�v
 *
 *	@param	ptr       ���[�N�|�C���^
 *	@return none
 */
//-----------------------------------------------------------------------------
static void PopPriority(FACEUP_WK_PTR ptr)
{
  int i;
  for(i=0;i<4;i++){
    GFL_BG_SetPriority( i, ptr->BgPriority[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�\����ԃ|�b�v
 *
 *	@param	ptr       ���[�N�|�C���^
 *	@return none
 */
//-----------------------------------------------------------------------------
static void PopDisp(FACEUP_WK_PTR ptr)
{
  GFL_DISP_GX_SetVisibleControlDirect(ptr->Mask);
}



static void ChangeScreenPlt( void *rawData, u8 px, u8 py, u8 sx, u8 sy, u8 pal )
{
  u16 * scrn;
  u8  scr_sx, scr_sy;
  u8  i, j;

  scrn = (u16 *)rawData;

  scr_sx = 32;
  scr_sy = 32;

  for( i=py; i<py+sy; i++ ){
    if( i >= scr_sy ){ break; }
    for( j=px; j<px+sx; j++ ){
      if( j >= scr_sx ){ break; }
      {
        u16 scr_pos = i * 32 + j;

        scrn[scr_pos] = ( scrn[scr_pos] & 0xfff ) | ( pal << 12 );
      }
    }
  }
}
