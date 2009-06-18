//======================================================================
/**
 * @file	fldmmdl_blact.c
 * @brief	�t�B�[���h���샂�f�� �r���{�[�h�A�N�^�[�Ǘ�
 * @author	kagaya
 * @data	05.07.13
 */
//======================================================================
#include "fldmmdl.h"
#include "test_graphic/fld_act.naix"

#ifdef MMDL_BLACT_HEAD3_TEST
#include "test_graphic/fldmmdl_btx.naix"
#endif

//======================================================================
//	define
//======================================================================
#define REGIDCODE_MAX (0xffff) ///<BBDRESID�ő�
#define BLACT_RESID_NULL (0xffff) ///<�r���{�[�h���\�[�X ����ID

typedef enum
{
  BBDRESBIT_GUEST = (1<<0), ///<���\�[�X���� �Q�X�g�o�^ OFF��=���M�����[
  BBDRESBIT_TRANS = (1<<1), ///<���\�[�X���� VRAM�]���p���\�[�X
  
  ///���\�[�X�@VRAM�풓�@���M�����[
  BBDRES_VRAM_REGULAR =(0),
  ///���\�[�X�@VRAM�]���@���M�����[
  BBDRES_TRANS_REGULAR =(BBDRESBIT_TRANS),
  ///���\�[�X�@VRAM�풓�@�Q�X�g
  BBDRES_VRAM_GUEST=(BBDRESBIT_GUEST),
  ///���\�[�X�@VRAM�]���@�Q�X�g
  BBDRES_TRANS_GUEST=(BBDRESBIT_TRANS|BBDRESBIT_GUEST),
}BBDRESBIT;

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	IDCODE
//--------------------------------------------------------------
typedef struct
{
	u16 code; ///<�o�^�R�[�h
	u16 id; ///<�o�^�R�[�h����Ԃ�ID
  u16 flag; ///<�o�^���e��\���t���O
  u16 dmy;
}IDCODE;

//--------------------------------------------------------------
///	IDCODEIDX
//--------------------------------------------------------------
typedef struct
{
	int max;
	IDCODE *pIDCodeBuf;
}IDCODEIDX;

//--------------------------------------------------------------
/// ADDRES_RESERVE
//--------------------------------------------------------------
typedef struct
{
  BOOL compFlag; //�f�[�^�̐ݒ肪���������t���O
  
  u16 code; //�o�^�p�R�[�h
  BBDRESBIT flag; //�o�^�p�t���O
  GFL_G3D_RES *pG3dRes; //�o�^���\�[�X
}ADDRES_RESERVE;

//--------------------------------------------------------------
/// ADDACT_RESERVE
//--------------------------------------------------------------
typedef struct
{
  BOOL compFlag; //�f�[�^�̐ݒ肪���������t���O
  
  MMDL *fmmdl;
  u16 code;
  u16 dummy;
  u16 *outID;
  GFL_G3D_RES *pTransActRes; ///<�]���p�A�N�^�[���\�[�X
}ADDACT_RESERVE;

//--------------------------------------------------------------
/// BLACT_RESERVE
//--------------------------------------------------------------
typedef struct
{
  BOOL funcFlag; //TRUE=�\�񏈗������s����

  u16 resMax;
  u16 actMax;
  ADDRES_RESERVE *pReserveRes;
  ADDACT_RESERVE *pReserveAct;
}BLACT_RESERVE;

//--------------------------------------------------------------
///	MMDL_BLACTCONT
//--------------------------------------------------------------
struct _TAG_MMDL_BLACTCONT
{
  int resourceMax;

	GFL_BBDACT_SYS *pBbdActSys; //���[�U�[����
	GFL_BBDACT_RESUNIT_ID bbdActResUnitID;
	GFL_BBDACT_ACTUNIT_ID bbdActActUnitID;
	u16 bbdActResCount;
	u16 bbdActActCount;
  
  ARCHANDLE *arcH_res;
  
	MMDLSYS *fmmdlsys;
  
	IDCODEIDX BBDResUnitIdx;
  BLACT_RESERVE *pReserve;
};

//======================================================================
//	proto
//======================================================================
static void BlActFunc( GFL_BBDACT_SYS *bbdActSys, int actIdx, void *work );

static void IDCodeIndex_Init( IDCODEIDX *idx, int max, HEAPID heapID );
static void IDCodeIndex_RegistCode(
    IDCODEIDX *idx, u16 code, u16 id, u16 flag );
static void IDCodeIndex_DeleteCode( IDCODEIDX *idx, u16 code );
static BOOL IDCodeIndex_SearchCode(
    IDCODEIDX *idx, u16 code, u16 *outID, u16 *outFlag );
static void BBDResUnitIndex_Init( MMDL_BLACTCONT *pBlActCont, int max );
static void BBDResUnitIndex_Delete( MMDL_BLACTCONT *pBlActCont );
static void BBDResUnitIndex_AddResource( MMDL_BLACTCONT *pBlActCont,
    GFL_G3D_RES *g3dres, u16 obj_code, BBDRESBIT flag );
static void BBDResUnitIndex_AddResUnit(
    MMDL_BLACTCONT *pBlActCont, u16 obj_code, BBDRESBIT flag );
static void BBDResUnitIndex_RemoveResUnit(
    MMDL_BLACTCONT *pBlActCont, u16 obj_code );
static BOOL BBDResUnitIndex_SearchResID(
  MMDL_BLACTCONT *pBlActCont, u16 obj_code, u16 *outID, u16 *outFlag );

static void BlActAddReserve_Init( MMDL_BLACTCONT *pBlActCont );
static void BlActAddReserve_Delete( MMDL_BLACTCONT *pBlActCont );
static void BlActAddReserve_Digest( MMDL_BLACTCONT *pBlActCont );
static void BlActAddReserve_RegistResource(
    MMDL_BLACTCONT *pBlActCont, u16 code, BBDRESBIT flag );
static void BlActAddReserve_DigestResource( MMDL_BLACTCONT *pBlActCont );
static BOOL BlActAddReserve_SearchResource(
    MMDL_BLACTCONT *pBlActCont, u16 code, u16 *outFlag );
static BOOL BlActAddReserve_CancelResource(
    MMDL_BLACTCONT *pBlActCont, u16 code );
static void BlActAddReserve_RegistActor(
    MMDL_BLACTCONT *pBlActCont, MMDL *fmmdl, u16 code, u16 *outID );
static void BlActAddReserve_DigestActor( MMDL_BLACTCONT *pBlActCont );
static BOOL BlActAddReserve_SearchActorOBJCode(
    MMDL_BLACTCONT *pBlActCont, u16 code, MMDL *fmmdl );
static void BlActAddReserve_CancelActor(
    MMDL_BLACTCONT *pBlActCont, MMDL *fmmdl );

static const MMDL_BBDACT_ANMTBL * BlActAnm_GetAnmTbl( u32 no );
static GFL_BBDACT_RESUNIT_ID BlActRes_AddRes(
    MMDL_BLACTCONT *pBlActCont, u16 code,
    GFL_G3D_RES *g3dres, GFL_BBDACT_RESTYPE type );

//test data
/*
const GFL_BBDACT_RESDATA testResTable[];
const u32 testResTableMax;
*/

//======================================================================
//	�t�B�[���h���샂�f���@�r���{�[�h�A�N�^�[�Ǘ�
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT �r���{�[�h�A�N�^�[�Ǘ��@�Z�b�g�A�b�v
 * @param	fmmdlsys	�\�z�ς݂�MMDLSYS
 * @param	pBbdActSys	�\�z�ς݂�GFL_BBDACT_SYS
 * @param res_max ���\�[�X���ő�
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_Setup( MMDLSYS *fmmdlsys,
	GFL_BBDACT_SYS *pBbdActSys, int res_max )
{
	HEAPID heapID;
	GFL_BBDACT_ACTDATA *actData;
	GFL_BBDACT_ACTUNIT_ID actUnitID;
	MMDL_BLACTCONT *pBlActCont;
	
	heapID = MMDLSYS_GetHeapID( fmmdlsys );
	pBlActCont = GFL_HEAP_AllocClearMemory( heapID, sizeof(MMDL_BLACTCONT) );
	pBlActCont->fmmdlsys = fmmdlsys;
	pBlActCont->pBbdActSys = pBbdActSys;
  pBlActCont->arcH_res = GFL_ARC_OpenDataHandle( ARCID_MMDL_RES, heapID );
  pBlActCont->resourceMax = res_max;
  
	MMDLSYS_SetBlActCont( fmmdlsys, pBlActCont );
	
	BBDResUnitIndex_Init( pBlActCont, res_max );
  BlActAddReserve_Init( pBlActCont );
}

//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT �r���{�[�h�A�N�^�[�Ǘ��@�폜
 * @param	fmmdlsys	MMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_Release( MMDLSYS *fmmdlsys )
{
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( fmmdlsys );
  BlActAddReserve_Delete( pBlActCont );
	BBDResUnitIndex_Delete( pBlActCont );
  
  GFL_ARC_CloseDataHandle( pBlActCont->arcH_res );
	
	GFL_HEAP_FreeMemory( pBlActCont );
	MMDLSYS_SetBlActCont( fmmdlsys, NULL );
}

//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT V�u�����N����
 * @param	fmmdlsys	MMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_ProcVBlank( MMDLSYS *fmmdlsys )
{
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( fmmdlsys );
  BlActAddReserve_Digest( pBlActCont );
}

//======================================================================
//	�t�B�[���h���샂�f���@�r���{�[�h���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT ���\�[�X�ǉ� ���M�����[�AVRAM�풓�^�œo�^
 * @param	fmmdlsys	MMDLSYS
 * @param	code �\���R�[�h�z��
 * @param	max code�v�f��
 * @retval	nothing
 * @note �Ă΂ꂽ���̏�œǂݍ��݂���������B
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_AddResourceTex(
	MMDLSYS *fmmdlsys, const u16 *code, int max )
{
  MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( fmmdlsys );
	while( max ){
		BBDResUnitIndex_AddResUnit( pBlActCont, *code, BBDRES_VRAM_REGULAR );
		code++;
		max--;
	}
}

//======================================================================
//	�t�B�[���h���샂�f�� �r���{�[�h�A�N�^�[�ǉ�
//======================================================================
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[ �A�N�^�[�ǉ�
 * @param	fmmdl	MMDL
 * @param	resID �ǉ�����A�N�^�[�Ŏg�p���郊�\�[�X�C���f�b�N�X
 * @retval GFL_BBDACT_ACTUNIT_ID
 */
//--------------------------------------------------------------
static GFL_BBDACT_ACTUNIT_ID blact_AddActor(
    MMDL *fmmdl, u16 code, u16 resID )
{
	VecFx32 pos;
	GFL_BBDACT_ACTDATA actData;
	GFL_BBDACT_ACTUNIT_ID actID;
  
  MMDLSYS *fmmdlsys =
    (MMDLSYS*)MMDL_GetMMdlSys( fmmdl );
	MMDL_BLACTCONT *pBlActCont =
    MMDLSYS_GetBlActCont( fmmdlsys );
	
	MMDL_GetDrawVectorPos( fmmdl, &pos );
	
  actData.resID = resID;
	actData.sizX = FX16_ONE*8-1;
	actData.sizY = FX16_ONE*8-1;
	actData.trans = pos;
	actData.alpha = 31;
	actData.drawEnable = TRUE;
	actData.lightMask = GFL_BBD_LIGHTMASK_01;
	actData.work = fmmdl;
	actData.func = BlActFunc;
	
	actID = GFL_BBDACT_AddAct(
		pBlActCont->pBbdActSys, pBlActCont->bbdActResUnitID, &actData, 1 );
  
  {
	  const OBJCODE_PARAM *prm =
      MMDLSYS_GetOBJCodeParam( fmmdlsys, code );
	  const MMDL_BBDACT_ANMTBL *anmTbl =
      BlActAnm_GetAnmTbl( prm->anm_id );
    
	  if( anmTbl->pAnmTbl != NULL ){
		  GFL_BBDACT_SetAnimeTable( pBlActCont->pBbdActSys,
        actID, (GFL_BBDACT_ANMTBL)anmTbl->pAnmTbl, anmTbl->anm_max );
    }
     
	  GFL_BBDACT_SetAnimeIdxOn( pBlActCont->pBbdActSys, actID, 0 );
	}
	
	return( actID );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�@���\�[�X����A�N�^�[�Z�b�g
 * @param fmmdl MMDL
 * @param code �\���R�[�h
 * @param resID �A�N�^�[���\�[�XID
 * @param transResID �]���p���\�[�X
 * @retval
 */
//--------------------------------------------------------------
static GFL_BBDACT_ACTUNIT_ID blact_SetResActor(
    MMDL *fmmdl, u16 code, u16 resID, u16 transResID )
{
	GFL_BBDACT_ACTUNIT_ID actID;
	MMDLSYS *fmmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( fmmdl );
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( fmmdlsys );
	
  actID = blact_AddActor( fmmdl, code, resID );
  
  if( transResID != BLACT_RESID_NULL ){ //�]���p���\�[�X�ƌ���
    GFL_BBDACT_BindActTexRes( pBlActCont->pBbdActSys, actID, transResID );
  }
   
	return( actID );
}

#if 0
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�@�]���^�A�N�^�[�Z�b�g
 * @param fmmdl MMDL
 * @param code �\���R�[�h
 * @param transResID �]���p���\�[�XID
 * @param pTransActRes �]���p�r���{�[�h���\�[�XID
 * @retval GFL_BBDACT_ACTUNIT_ID
 */
//--------------------------------------------------------------
static GFL_BBDACT_ACTUNIT_ID blact_SetTransActor(
    MMDL *fmmdl, u16 code, u16 transResID, GFL_G3D_RES pTransActRes )
{
  u16 resID;
	GFL_BBDACT_ACTUNIT_ID actID;
	MMDLSYS *fmmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( fmmdl );
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( fmmdlsys );
	
  {
  	const OBJCODE_PARAM *prm;
  	GFL_BBDACT_G3DRESDATA data;
	  prm = MMDLSYS_GetOBJCodeParam( pBlActCont->fmmdlsys, obj_code );
    
    data.g3dres = g3dres;
	  data.texFmt = GFL_BBD_TEXFMT_PAL16;
	  data.texSiz = prm->tex_size;
	  data.celSizX = 32;				//������mdl_size����
	  data.celSizY = 32;
  }

    actID = blact_AddActor( fmmdl, code, resID );
  
  GFL_BBDACT_ACTUNIT_ID blact_SetResActor(
    MMDL *fmmdl, u16 code, u16 resID, resID );
  u16 transResID )
  if( transResID != BLACT_RESID_NULL ){ //�]���p���\�[�X�ƌ���
    GFL_BBDACT_BindActTexRes( pBlActCont->pBbdActSys, actID, transResID );
  }
   
	return( actID );
}
#endif

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�@�A�N�^�[�Z�b�g
 * @param fmmdl MMDL
 * @param code �\���R�[�h
 * @retval nothing
 */
//--------------------------------------------------------------
#if 0
static GFL_BBDACT_ACTUNIT_ID blact_SetActor( MMDL *fmmdl, u16 code )
{
  u16 resID,flag;
	GFL_BBDACT_ACTUNIT_ID actID;
	MMDLSYS *fmmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( fmmdl );
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( fmmdlsys );
	
	BBDResUnitIndex_SearchResID( pBlActCont, code, &resID, &flag );
	
  if( resID == REGIDCODE_MAX ){ //��o�^���\�[�X
    MMDL_BLACTCONT_AddOBJCodeRes( fmmdlsys, code,  FALSE, TRUE );
	  BBDResUnitIndex_SearchResID( pBlActCont, code, &resID, &flag );
    GF_ASSERT( resID != REGIDCODE_MAX );
    
    #ifdef DEBUG_MMDL
    KAGAYA_Printf( "MMDL ADD GUEST RESOURCE %xH\n", code );
    #endif
  }
  
  if( (flag&BBDRESBIT_TRANS) ){ //�]���^ �]���惊�\�[�X�쐬
    //�]���p���\�[�X���擾���ǉ��B
    //resID = �A�N�^�[�p�̃��\�[�X
    GF_ASSERT( 0 ); //���󖢑Ή�
  }
  
  actID = blact_AddActor( fmmdl, code, resID );
  
  if( (flag&BBDRESBIT_TRANS) ){ //�]���^ ���\�[�X�ƌ���
	  BBDResUnitIndex_SearchResID( pBlActCont, code, &resID, &flag );
    GFL_BBDACT_BindActTexRes( pBlActCont->pBbdActSys, actID, resID );
  }
  
	return( actID );
}
#endif

static GFL_BBDACT_ACTUNIT_ID blact_SetActor( MMDL *fmmdl, u16 code )
{
  u16 resID,flag;
	GFL_BBDACT_ACTUNIT_ID actID;
	MMDLSYS *fmmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( fmmdl );
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( fmmdlsys );
	
	BBDResUnitIndex_SearchResID( pBlActCont, code, &resID, &flag );
	GF_ASSERT( resID != REGIDCODE_MAX ); //���\�[�X����
  GF_ASSERT( !(flag&BBDRESBIT_TRANS) ); //�]���^�C�v
  
  actID = blact_AddActor( fmmdl, code, resID );
	return( actID );
}

#if 0
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[ �ǉ�
 * @param	fmmdl	MMDL
 * @param	code �g�p����OBJ�R�[�h
 * @param outID �ǉ��A�N�^�[ID�i�[�� MMDL_BBDACT_ACTID_NULL=�ǉ���
 * @retval BOOL TRUE=�ǉ��BFALSE=�ǉ����B
 */
//--------------------------------------------------------------
GFL_BBDACT_ACTUNIT_ID MMDL_BLACTCONT_AddActor( MMDL *fmmdl, u32 code )
{
  u16 flag;
	VecFx32 pos;
	GFL_BBDACT_ACTDATA actData;
	GFL_BBDACT_ACTUNIT_ID actID;
	const MMDL_BBDACT_ANMTBL *anmTbl;
	const OBJCODE_PARAM *prm;
	MMDLSYS *fmmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( fmmdl );
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( fmmdlsys );
	
	prm = MMDLSYS_GetOBJCodeParam( fmmdlsys, code );
	MMDL_GetDrawVectorPos( fmmdl, &pos );
	
	BBDResUnitIndex_SearchResID( fmmdlsys, code, &actData.resID, &flag );
	
  if( actData.resID == REGIDCODE_MAX ){ //��o�^���\�[�X
    MMDL_BLACTCONT_AddOBJCodeRes( fmmdlsys, code,  FALSE, TRUE );
	  BBDResUnitIndex_SearchResID( fmmdlsys, code, &actData.resID, &flag );
    GF_ASSERT( actData.resID != REGIDCODE_MAX );
    
    #ifdef DEBUG_MMDL
    KAGAYA_Printf( "MMDL ADD GUEST RESOURCE %xH\n", code );
    #endif
  }
  
	actData.sizX = FX16_ONE*8-1;
	actData.sizY = FX16_ONE*8-1;
	actData.trans = pos;
	actData.alpha = 31;
	actData.drawEnable = TRUE;
	actData.lightMask = GFL_BBD_LIGHTMASK_01;
	actData.work = fmmdl;
	actData.func = BlActFunc;
	
	actID = GFL_BBDACT_AddAct(
		pBlActCont->pBbdActSys, pBlActCont->bbdActResUnitID, &actData, 1 );
  
#if 0 //�]���p���\�[�X�ƃo�C���h
  if( flag & == HERO ){
//    GFL_BBDACT_BindActTexRes( pBlActCont->pBbdActSys, actID, actData.resID );
  }
#endif

	anmTbl = BlActAnm_GetAnmTbl( prm->anm_id );
	
	if( anmTbl->pAnmTbl != NULL ){
		GFL_BBDACT_SetAnimeTable( pBlActCont->pBbdActSys,
			actID, (GFL_BBDACT_ANMTBL)anmTbl->pAnmTbl, anmTbl->anm_max );
	}
	
	GFL_BBDACT_SetAnimeIdxOn(
			pBlActCont->pBbdActSys, actID, 0 );
	return( actID );
}
#endif

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[ �ǉ�
 * @param	fmmdl	MMDL
 * @param	code �g�p����OBJ�R�[�h
 * @param outID �ǉ��A�N�^�[ID�i�[�� MMDL_BLACTID_NULL=�ǉ���
 * @retval BOOL TRUE=�ǉ��BFALSE=�ǉ����B
 */
//--------------------------------------------------------------
BOOL MMDL_BLACTCONT_AddActor(
    MMDL *fmmdl, u16 code, GFL_BBDACT_ACTUNIT_ID *outID )
{
  u16 resID,flag;
	MMDLSYS *fmmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( fmmdl );
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( fmmdlsys );
  
	BBDResUnitIndex_SearchResID( pBlActCont, code, &resID, &flag );
	
  if( resID != REGIDCODE_MAX && (flag&BBDRESBIT_TRANS) == 0 ){
    (*outID) = blact_SetActor( fmmdl, code );
    return( TRUE );
  }
  
  { //�\��o�^
#if 1
    *outID = MMDL_BLACTID_NULL;
    BlActAddReserve_RegistActor( pBlActCont, fmmdl, code, outID );
#else
    (*outID) = blact_SetActor( fmmdl, code );
    return( TRUE );
#endif
  }
  
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[ �폜
 * @param	fmmdl	MMDL*
 * @param	actID GFL_BBDACT_ACTUNIT_ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_DeleteActor( MMDL *fmmdl, u32 actID )
{
  u16 id,flag;
  u16 code = MMDL_GetOBJCode( fmmdl );
	MMDLSYS *pMMdlSys = (MMDLSYS*)MMDL_GetMMdlSys( fmmdl );
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( pMMdlSys );
  
  if( actID == MMDL_BLACTID_NULL ){ //�܂��ǉ�����Ă��Ȃ��B
    BlActAddReserve_CancelActor( pBlActCont, fmmdl ); //�\�񂠂�΃L�����Z��
    return;
  }
  
  if( BBDResUnitIndex_SearchResID(pBlActCont,code,&id,&flag) == TRUE )
  {
    if( (flag&BBDRESBIT_TRANS) ) //�]���^ �A�N�^�[�̓]���p���\�[�X�폜
    {
      u16 idx = GFL_BBDACT_GetResIdx( pBlActCont->pBbdActSys, actID );
	    GFL_BBDACT_RemoveResourceUnit( pBlActCont->pBbdActSys, idx, 1 );
    }
    
    if( (flag&BBDRESBIT_GUEST) ) //�Q�X�g�o�^ ���ɗ��p������΍폜
    {
      if( MMDL_SearchUseOBJCode(fmmdl,code) == FALSE )
      {
        BBDResUnitIndex_RemoveResUnit( pBlActCont, code );
        
        KAGAYA_Printf(
          "MMDL DEL GUEST RESOURCE CODE=%d, RESID=%d\n", code, id );
      }
    }
  }
  
	GFL_BBDACT_RemoveAct( pBlActCont->pBbdActSys, actID, 1 );
}

//======================================================================
//	�t�B�[���h���샂�f���@�r���{�[�h�A�N�^�[����
//======================================================================
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[����֐�
 * @param	bbdActSys	GFL_BBDACT_SYS
 * @param	actIdx		int
 * @param	work		void*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BlActFunc( GFL_BBDACT_SYS *bbdActSys, int actIdx, void *work )
{
}

//======================================================================
//	�t�B�[���h���샂�f���@�r���{�[�h�A�N�^�[�Ǘ��@�Q��
//======================================================================
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�Ǘ��@GFL_BBDACT_SYS�擾
 * @param	pBlActCont MMDL_BLACTCONT
 * @retval	GFL_BBDACT_SYS
 */
//--------------------------------------------------------------
GFL_BBDACT_SYS * MMDL_BLACTCONT_GetBbdActSys(
		MMDL_BLACTCONT *pBlActCont )
{
	return( pBlActCont->pBbdActSys );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�Ǘ� GFL_BBDACT_RESUNIT_ID�擾
 * @param	pBlActCont MMDL_BLACTCONT
 * @retval	GFL_BBDACT_RESUNIT_ID
 */
//--------------------------------------------------------------
GFL_BBDACT_RESUNIT_ID MMDL_BLACTCONT_GetResUnitID(
		MMDL_BLACTCONT *pBlActCont )
{
	return( pBlActCont->bbdActResUnitID );
}

//======================================================================
//	IDCODEIDX
//======================================================================
//--------------------------------------------------------------
/**
 * IDCODEIDX ������
 * @param	idx IDCODEIDX*
 * @param	max �o�^�ő吔
 * @param	heapID HEAPID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void IDCodeIndex_Init( IDCODEIDX *idx, int max, HEAPID heapID )
{
	IDCODE *tbl;
	GF_ASSERT( max );
	tbl = GFL_HEAP_AllocClearMemory( heapID, sizeof(IDCODE)*max );
	idx->max = max;
	idx->pIDCodeBuf = tbl;
	do{
		tbl->code = REGIDCODE_MAX;
		tbl->id = REGIDCODE_MAX;
		tbl++;
		max--;
	}while( max );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX �폜
 * @param	idx	IDCODEIDX*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void IDCodeIndex_Delete( IDCODEIDX *idx )
{
	GF_ASSERT( idx->pIDCodeBuf );
	GFL_HEAP_FreeMemory( idx->pIDCodeBuf );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX �w�肳�ꂽ�R�[�h��ID�A�t���O��o�^
 * @param	idx IDCODEIDX
 * @param	code �R�[�h
 * @param	id ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void IDCodeIndex_RegistCode(
    IDCODEIDX *idx, u16 code, u16 id, u16 flag )
{
	int i = 0;
	IDCODE *tbl = idx->pIDCodeBuf;
	do{
		if( tbl->code == REGIDCODE_MAX ){
			tbl->code = code;
			tbl->id = id;
      tbl->flag = flag;
			return;
		}
		tbl++;
		i++;
	}while( i < idx->max );
	GF_ASSERT( 0 );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX �w�肳�ꂽ�R�[�h���폜
 * @param	idx IDCODEIDX
 * @param	code OBJ�R�[�h
 * @retval	nothing
 */
//--------------------------------------------------------------
static void IDCodeIndex_DeleteCode( IDCODEIDX *idx, u16 code )
{
	int i = 0;
	IDCODE *tbl = idx->pIDCodeBuf;
	do{
		if( tbl->code == code ){
			tbl->code = REGIDCODE_MAX;
			tbl->id = REGIDCODE_MAX;
      tbl->flag = 0;
			return;
		}
		tbl++;
		i++;
	}while( i < idx->max );
	GF_ASSERT( 0 );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX �w�肳�ꂽ�R�[�h�Ɉ�v����ID�A�t���O��Ԃ�
 * @param	idx IDCODEIDX
 * @param	code OBJ�R�[�h
 * @param outID ID�i�[�� NULL=�i�[���Ȃ�
 * @param outFlag �t���O�i�[�� NULL=�i�[���Ȃ�
 * @retval BOOL TRUE=��v�L��AFALSE=�R�[�h�����݂��Ȃ�
 */
//--------------------------------------------------------------
static BOOL IDCodeIndex_SearchCode(
    IDCODEIDX *idx, u16 code, u16 *outID, u16 *outFlag )
{
	int i = 0;
	IDCODE *tbl = idx->pIDCodeBuf;
  
  if( outID != NULL ){
    *outID = REGIDCODE_MAX;
  }
  
	do{
		if( tbl->code == code ){
      if( outID != NULL ){
        *outID = tbl->id;
      }
      if( outFlag != NULL ){
        *outFlag = tbl->flag;
      }
			return( TRUE );
		}
		tbl++;
		i++;
	}while( i < idx->max );
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT ������
 * @param	pBlActCont MMDL_BLACTCONT
 * @param	max	�o�^�ő吔
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BBDResUnitIndex_Init( MMDL_BLACTCONT *pBlActCont, int max )
{
	HEAPID heapID = MMDLSYS_GetHeapID( pBlActCont->fmmdlsys );
	IDCodeIndex_Init( &pBlActCont->BBDResUnitIdx, max, heapID );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT �폜
 * @param	pBlActCont MMDL_BLACTCONT
 * @param	max	�o�^�ő吔
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BBDResUnitIndex_Delete( MMDL_BLACTCONT *pBlActCont )
{
	int i = 0;
	IDCODE *tbl = pBlActCont->BBDResUnitIdx.pIDCodeBuf;
	
	do{
		if( tbl->code != REGIDCODE_MAX ){
			BBDResUnitIndex_RemoveResUnit( pBlActCont, tbl->code );
		}
		tbl++;
		i++;
	}while( i < pBlActCont->BBDResUnitIdx.max );
	
	IDCodeIndex_Delete( &pBlActCont->BBDResUnitIdx );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT ���\�[�X�ǉ�
 * @param	pBlActCont MMDL_BLACTCONT
 * @param	code	OBJ�R�[�h
 * @param flag �o�^�t���O BBDRESBIT_VRAM��
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BBDResUnitIndex_AddResource( MMDL_BLACTCONT *pBlActCont,
    GFL_G3D_RES *g3dres, u16 obj_code, BBDRESBIT flag )
{
	GFL_BBDACT_RESUNIT_ID id;
  GFL_BBDACT_RESTYPE type;
  
	type = GFL_BBDACT_RESTYPE_DATACUT; //��{�̓��\�[�X�j���^
  
  if( (flag&BBDRESBIT_TRANS) ){ //�]���p���\�[�X�w��
    type = GFL_BBDACT_RESTYPE_TRANSSRC;
  }
  
  id = BlActRes_AddRes( pBlActCont, obj_code, g3dres, type );
	IDCodeIndex_RegistCode( &pBlActCont->BBDResUnitIdx, obj_code, id, flag );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT ���\�[�X���j�b�g�ǉ�
 * @param	pBlActCont MMDL_BLACTCONT
 * @param	code	OBJ�R�[�h
 * @param flag �o�^�t���O BBDRESBIT_VRAM��
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BBDResUnitIndex_AddResUnit(
    MMDL_BLACTCONT *pBlActCont, u16 obj_code, BBDRESBIT flag )
{
  GFL_G3D_RES *g3dres;
	const OBJCODE_PARAM *prm;
  
	prm = MMDLSYS_GetOBJCodeParam( pBlActCont->fmmdlsys, obj_code );
  g3dres = GFL_G3D_CreateResourceHandle(
      pBlActCont->arcH_res, prm->res_idx );
  BBDResUnitIndex_AddResource( pBlActCont, g3dres, obj_code, flag );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT �w��OBJ�R�[�h�̃��\�[�X���j�b�g�폜
 * @param	pBlActCont MMDL_BLACTCONT
 * @param	code	OBJ�R�[�h
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BBDResUnitIndex_RemoveResUnit(
		MMDL_BLACTCONT *pBlActCont, u16 obj_code )
{
	GFL_BBDACT_RESUNIT_ID id;
  BOOL ret = IDCodeIndex_SearchCode(
      &pBlActCont->BBDResUnitIdx, obj_code, &id, NULL );
	GF_ASSERT( ret == TRUE );
	GFL_BBDACT_RemoveResourceUnit( pBlActCont->pBbdActSys, id, 1 );
	IDCodeIndex_DeleteCode( &pBlActCont->BBDResUnitIdx, obj_code );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT�@�w��R�[�h�̃��\�[�XID���擾
 * @param	pBlActCont MMDL_BLACTCONT
 * @param	code	OBJ�R�[�h
 * @retval	BOOL TRUE=�w��R�[�h�o�^�L��
 */
//--------------------------------------------------------------
static BOOL BBDResUnitIndex_SearchResID(
    MMDL_BLACTCONT *pBlActCont, u16 obj_code, u16 *outID, u16 *outFlag )
{
  BOOL ret = IDCodeIndex_SearchCode(
    &pBlActCont->BBDResUnitIdx, obj_code, outID, outFlag );
  return( ret );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT �w��R�[�h�̃��\�[�XID���ǉ��ς݂��`�F�b�N
 * @param	fmmdlsys	MMDLSYS
 * @param	code OBJ�R�[�h
 * @retval	BOOL	TRUE=�ǉ��ς� FALSE=�ǉ��܂�
 */
//--------------------------------------------------------------
BOOL MMDL_BLACTCONT_CheckOBJCodeRes( MMDLSYS *fmmdlsys, u16 code )
{
	u16 res;
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( fmmdlsys );
	
	if( IDCodeIndex_SearchCode(&pBlActCont->BBDResUnitIdx,code,NULL,NULL) ){
    return( TRUE );
  }
  
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT �w��R�[�h�̃��\�[�X��ǉ�
 * @param	fmmdlssy	MMDLSYS
 * @param	code	OBJ�R�[�h
 * @param trans TRUE=�e�N�X�`���]���^�œo�^
 * @param guest TRUE=�g�p����Ȃ��Ȃ�Ɣj�������Q�X�g�^�C�v�œo�^
 * @retval	BOOL	TRUE=�ǉ������BFALSE=���ɒǉ��ς�
 * @note �Ă΂ꂽ���̏�œǂݍ��݂���������B
 */
//--------------------------------------------------------------
BOOL MMDL_BLACTCONT_AddOBJCodeRes(
    MMDLSYS *fmmdlsys, u16 code, BOOL trans, BOOL guest )
{
	if( MMDL_BLACTCONT_CheckOBJCodeRes(fmmdlsys,code) == FALSE ){
    MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( fmmdlsys );
    BBDRESBIT flag = 0;
    if( trans ){ flag |= BBDRESBIT_TRANS; }
    if( guest ){ flag |= BBDRESBIT_GUEST; }
		BBDResUnitIndex_AddResUnit( pBlActCont, code, flag );
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT �w��R�[�h�̃��\�[�X���폜
 * @param	fmmdlssy	MMDLSYS
 * @param	code	OBJ�R�[�h
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_DeleteOBJCodeRes( MMDLSYS *fmmdlsys, u16 code )
{
  MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( fmmdlsys );
	BBDResUnitIndex_RemoveResUnit( pBlActCont, code );
}

//======================================================================
//  �A�N�^�[�ǉ��\�񏈗�
//======================================================================
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�ǉ��\�񏈗��@������
 * @param pBlActCont MMDL_BLACTCONT*
 * @retval nothing
 */
//--------------------------------------------------------------
static void BlActAddReserve_Init( MMDL_BLACTCONT *pBlActCont )
{
  HEAPID heapID = MMDLSYS_GetHeapID( pBlActCont->fmmdlsys );
  BLACT_RESERVE *pReserve = 
    GFL_HEAP_AllocClearMemory( heapID, sizeof(BLACT_RESERVE) );
  
  pReserve->resMax = pBlActCont->resourceMax;
  pReserve->actMax = MMDLSYS_GetMMdlMax( pBlActCont->fmmdlsys );
  
  pReserve->pReserveRes = GFL_HEAP_AllocClearMemory(
      heapID, sizeof(ADDRES_RESERVE)*pReserve->resMax );
  pReserve->pReserveAct = GFL_HEAP_AllocClearMemory(
      heapID, sizeof(ADDACT_RESERVE)*pReserve->actMax );
  
  pReserve->funcFlag = TRUE;
  pBlActCont->pReserve = pReserve;
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�ǉ��\�񏈗��@�폜
 * @param pBlActCont MMDL_BLACTCONT*
 * @retval nothing
 */
//--------------------------------------------------------------
static void BlActAddReserve_Delete( MMDL_BLACTCONT *pBlActCont )
{
  BLACT_RESERVE *pReserve = pBlActCont->pReserve;
  
  if( pReserve != NULL ){
    u32 i;
    pReserve->funcFlag = FALSE;
    
    { //���\�[�X
      ADDRES_RESERVE *pRes = pReserve->pReserveRes;
      for( i = 0; i < pReserve->resMax; i++, pRes++ ){
        if( pRes->pG3dRes != NULL ){
          GFL_G3D_DeleteResource( pRes->pG3dRes );
        }
      }
      GFL_HEAP_FreeMemory( pReserve->pReserveRes );
    }

    { //�A�N�^�[
      ADDACT_RESERVE *pRes = pReserve->pReserveAct;
      for( i = 0; i < pReserve->actMax; i++, pRes++ ){
        if( pRes->fmmdl != NULL && pRes->pTransActRes != NULL ){
          GFL_G3D_DeleteResource( pRes->pTransActRes );
        }
      }
      GFL_HEAP_FreeMemory( pReserve->pReserveAct );
    }
    
    GFL_HEAP_FreeMemory( pReserve );
    pBlActCont->pReserve = NULL;
  }
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�ǉ��\�񏈗��@�\�����
 * @param MMDLSYS *fmmdlsys
 * @retval nothing
 */
//--------------------------------------------------------------
static void BlActAddReserve_Digest( MMDL_BLACTCONT *pBlActCont )
{
  BLACT_RESERVE *pReserve = pBlActCont->pReserve;
  
  if( pReserve != NULL )
  {
    if( pReserve->funcFlag == TRUE )
    {
      BlActAddReserve_DigestResource( pBlActCont );
      BlActAddReserve_DigestActor( pBlActCont );
    }
  }
}

//--------------------------------------------------------------
/**
 * ���\�[�X�ǉ��\�񏈗��@�\��o�^
 * @param pBlActCont MMDL_BLACTCONT
 * @param code �\���R�[�h
 * @param flag BBDRESBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void BlActAddReserve_RegistResource(
    MMDL_BLACTCONT *pBlActCont, u16 code, BBDRESBIT flag )
{
  u32 i = 0;
  BLACT_RESERVE *pReserve = pBlActCont->pReserve;
  ADDRES_RESERVE *pRes = pReserve->pReserveRes;
  
  for( ; i < pReserve->resMax; i++, pRes++ ){
    if( pRes->pG3dRes == NULL ){
	    const OBJCODE_PARAM *prm;
      prm = MMDLSYS_GetOBJCodeParam( pBlActCont->fmmdlsys, code );
      
      pRes->compFlag = FALSE;
      pRes->code = code;
      pRes->flag = flag;
      pRes->pG3dRes = GFL_G3D_CreateResourceHandle(
              pBlActCont->arcH_res, prm->res_idx );
      pRes->compFlag = TRUE;
      
      KAGAYA_Printf(
        "MMDL BLACT RESERVE ADD RESOURCE CODE=%d,ARCIDX=%d\n",
        pRes->code, prm->res_idx );
      return;
    }
  }
  
  GF_ASSERT( 0 );
}

//--------------------------------------------------------------
/**
 * ���\�[�X�ǉ��\�񏈗��@�\�����
 * @param pBlActCont MMDL_BLACTCONT
 * @retval nothing
 */
//--------------------------------------------------------------
static void BlActAddReserve_DigestResource( MMDL_BLACTCONT *pBlActCont )
{
  u32 i = 0;
  BLACT_RESERVE *pReserve = pBlActCont->pReserve;
  ADDRES_RESERVE *pRes = pReserve->pReserveRes;
  
  for( ; i < pReserve->resMax; i++, pRes++ ){
    if( pRes->pG3dRes != NULL && pRes->compFlag == TRUE ){
      BBDResUnitIndex_AddResource(
          pBlActCont, pRes->pG3dRes, pRes->code, pRes->flag );
      pRes->pG3dRes = NULL;
    
#ifdef DEBUG_MMDL_DEVELOP
      {
        u16 id,flag;
        BBDResUnitIndex_SearchResID( pBlActCont, pRes->code, &id, &flag );
        
        KAGAYA_Printf(
          "MMDL BLACT RESERVE DIG RESOURCE CODE=%d, RESID=%d\n",
          pRes->code, id );
      }
#endif
    }
  }
}

//--------------------------------------------------------------
/**
 * ���\�[�X�ǉ��\�񏈗��@�\�񌟍�
 * @param pBlActCont MMDL_BLACTCONT
 * @param code ��������\���R�[�h
 * @param outFlag *�\��̍ۂɎw�肳�ꂽBBDRESBIT�i�[��
 * @retval BOOL TRUE=�\��ς� FALSE=�\��Ȃ�
 */
//--------------------------------------------------------------
static BOOL BlActAddReserve_SearchResource(
    MMDL_BLACTCONT *pBlActCont, u16 code, u16 *outFlag )
{
  u32 i = 0;
  BLACT_RESERVE *pReserve = pBlActCont->pReserve;
  ADDRES_RESERVE *pRes = pReserve->pReserveRes;
  
  for( ; i < pReserve->resMax; i++, pRes++ ){
    if( pRes->pG3dRes != NULL ){
      *outFlag = pRes->flag;
      return( TRUE );
    }
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���\�[�X�ǉ��\�񏈗��@�\��L�����Z��
 * @param pBlActCont MMDL_BLACTCONT
 * @param code �L�����Z������\���R�[�h
 * @retval BOOL TRUE=�L�����Z������ FALSE=�\��Ȃ�
 */
//--------------------------------------------------------------
static BOOL BlActAddReserve_CancelResource(
    MMDL_BLACTCONT *pBlActCont, u16 code )
{
  u32 i = 0;
  BLACT_RESERVE *pReserve = pBlActCont->pReserve;
  ADDRES_RESERVE *pRes = pReserve->pReserveRes;
  
  for( ; i < pReserve->resMax; i++, pRes++ ){
    if( pRes->pG3dRes != NULL && pRes->code == code ){
      GFL_G3D_DeleteResource( pRes->pG3dRes );
      pRes->pG3dRes = NULL;
      KAGAYA_Printf( "MMDL BLACT RESERVE CANCEL CODE=%d\n", pRes->code );
      return( TRUE );
    }
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�ǉ��\�񏈗��@�\��o�^
 * @param pBlActCont MMDL_BLACTCONT
 * @param fmmdl MMDL*
 * @param code �\���R�[�h
 * @param outID �A�N�^�[ID�i�[��
 * @retval nothing
 */
//--------------------------------------------------------------
static void BlActAddReserve_RegistActor(
    MMDL_BLACTCONT *pBlActCont, MMDL *fmmdl, u16 code, u16 *outID )
{
  u32 i = 0;
  u32 max = MMDLSYS_GetMMdlMax( pBlActCont->fmmdlsys );
  ADDACT_RESERVE *pRes = pBlActCont->pReserve->pReserveAct;
  
  for( ; i < max; i++, pRes++ )
  {
    if( pRes->fmmdl == NULL )
    {
      u16 resID,flag;
	    const OBJCODE_PARAM *prm;
      
      pRes->compFlag = FALSE;
      pRes->fmmdl = fmmdl;
      pRes->code = code;
      pRes->outID = outID;
      
	    prm = MMDLSYS_GetOBJCodeParam( pBlActCont->fmmdlsys, pRes->code );
	    BBDResUnitIndex_SearchResID( pBlActCont, code, &resID, &flag );
      
      if( resID == REGIDCODE_MAX ) //��o�^���\�[�X
      {
        if( BlActAddReserve_SearchResource( //���\�[�X�\�񌟍�
              pBlActCont,code,&flag) == FALSE )
        {
          flag = BBDRES_VRAM_GUEST; //��{VRAM�풓�Q�X�g�^�œo�^
          BlActAddReserve_RegistResource( pBlActCont, code, flag );
        }
      }

      if( (flag&BBDRESBIT_TRANS) ){ //�]���^ �]���惊�\�[�X�쐬
        #if 0
        //�]���p���\�[�X�f�[�^���擾
        pRes->pTransActRes = GFL_G3D_CreateResourceHandle(
              pBlActCont->arcH_res, prm->res_idx );
        #else
        GF_ASSERT( 0 ); //���󖢑Ή�
        #endif
      }
      
      pRes->compFlag = TRUE;
      KAGAYA_Printf( "MMDL BLACT RESERVE ADD ACTOR OBJID=%d, CODE=%d\n", 
          MMDL_GetOBJID(fmmdl), pRes->code );
      return;
    }
  }
  
  GF_ASSERT( 0 && "MMDL BLACT RESERVE ADD MAX" );
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�ǉ��\�񏈗��@�\�����
 * @param MMDLSYS *fmmdlsys
 * @retval nothing
 */
//--------------------------------------------------------------
static void BlActAddReserve_DigestActor( MMDL_BLACTCONT *pBlActCont )
{
  u32 i = 0;
  u32 max = MMDLSYS_GetMMdlMax( pBlActCont->fmmdlsys );
  ADDACT_RESERVE *pRes = pBlActCont->pReserve->pReserveAct;
  
  for( ; i < max; i++, pRes++ )
  {
    if( pRes->fmmdl != NULL && pRes->compFlag == TRUE )
    {
      BOOL ret;
      u16 resID,flag;
      u16 transID = BLACT_RESID_NULL;
      
      if( pRes->pTransActRes != NULL ) //�]���^
      {
        resID = BlActRes_AddRes( pBlActCont, pRes->code,
            pRes->pTransActRes, GFL_BBDACT_RESTYPE_DATACUT );
        pRes->pTransActRes = NULL;
        
        ret = BBDResUnitIndex_SearchResID(
            pBlActCont, pRes->code, &transID, &flag );
        
        GF_ASSERT( ret == TRUE &&
          "BLACT ADD RESERVE RESOURCE NONE" ); //�]���p���\�[�X����
        GF_ASSERT( (flag&BBDRESBIT_TRANS) &&
          "BLACT ADD RESERVE RESOURCE ERROR" ); //�]���p���\�[�X�ł͖���
      }
      else
      {
        ret = BBDResUnitIndex_SearchResID(
            pBlActCont, pRes->code, &resID, &flag );
        GF_ASSERT( ret == TRUE &&
          "BLACT ADD RESERVE RESOURCE NONE" ); //���\�[�X����
      }
      
      *(pRes->outID) = blact_SetResActor( //�A�N�^�[�ǉ�
          pRes->fmmdl, pRes->code, resID, transID );
      MMDL_CallDrawProc( pRes->fmmdl ); //�`�揈���Ăяo��
      
      KAGAYA_Printf( "MMDL BLACT RESERVE DIG ACTOR " );
      KAGAYA_Printf( "OBJID=%d, CODE=%d, RESID=%d,TRANSID=%d\n",
          MMDL_GetOBJID(pRes->fmmdl), pRes->code, resID, transID );
      
      pRes->fmmdl = NULL;
    }
  }
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�ǉ��\�񏈗��@�\��ς݂̕\���R�[�h������
 * @param pBlActCont MMDL_BLACTCONT
 * @param code ��������\���R�[�h
 * @param fmmdl �����ΏۂƂ��Ȃ�MMDL
 * @retval BOOL TRUE=��v�L�� FALSE=��v����
 */
//--------------------------------------------------------------
static BOOL BlActAddReserve_SearchActorOBJCode(
    MMDL_BLACTCONT *pBlActCont, u16 code, MMDL *fmmdl )
{
  u32 i = 0;
  u32 max = MMDLSYS_GetMMdlMax( pBlActCont->fmmdlsys );
  ADDACT_RESERVE *pRes = pBlActCont->pReserve->pReserveAct;
  
  for( ; i < max; i++, pRes++ ){
    if( pRes->fmmdl != NULL && pRes->fmmdl != fmmdl ){
      if( pRes->code == code ){
        return( TRUE );
      }
    }
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�ǉ��\�񏈗��@�\��L�����Z��
 * @param pBlActCont MMDL_BLACTCONT
 * @param fmmdl �\�񂵂�MMDL*
 * @retval nothing
 * @note �ǉ����Ɏw�肳�ꂽ�R�[�h�����\�[�X�ǉ��\��ɓ����Ă����ۂ�
 * ���\�[�X�\��L�����Z�������킹�čs���B
 */
//--------------------------------------------------------------
static void BlActAddReserve_CancelActor(
    MMDL_BLACTCONT *pBlActCont, MMDL *fmmdl )
{
  u32 i = 0;
  u32 max = MMDLSYS_GetMMdlMax( pBlActCont->fmmdlsys );
  ADDACT_RESERVE *pRes = pBlActCont->pReserve->pReserveAct;
  
  for( ; i < max; i++, pRes++ )
  {
    if( pRes->fmmdl == fmmdl )
    {
      u16 flag;
      
      if( BlActAddReserve_SearchResource( //���\�[�X�\��L��
            pBlActCont,pRes->code,&flag) == TRUE )
      {
        if( BlActAddReserve_SearchActorOBJCode( //�\�񒆂Ŏg�p����
              pBlActCont,pRes->code,pRes->fmmdl) == FALSE )
        {
          BlActAddReserve_CancelResource( pBlActCont, pRes->code );
        }
      }

      if( pRes->pTransActRes != NULL ) //�]���p���\�[�X�L��
      {
        GFL_G3D_DeleteResource( pRes->pTransActRes );
        pRes->pTransActRes = NULL;
      }
      
      pRes->fmmdl = NULL;
      
      KAGAYA_Printf( "MMDL BLACT RESERVE CANCEL OBJID=%d, CODE=%d\n", 
          MMDL_GetOBJID(fmmdl), pRes->code );
      return;
    }
  }
}

//======================================================================
//	parts
//======================================================================
//--------------------------------------------------------------
/**
 * �e�[�u���ԍ�����A�N�^�[�A�j���e�[�u���擾
 * @param	no	MMDL_BLACT_ANMTBLNO_MAX
 * @retval	MMDL_BBDACT_ANMTBL*
 */
//--------------------------------------------------------------
static const MMDL_BBDACT_ANMTBL * BlActAnm_GetAnmTbl( u32 no )
{
	GF_ASSERT( no < MMDL_BLACT_ANMTBLNO_MAX );
	return( &DATA_MMDL_BBDACT_ANM_ListTable[no] );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[���\�[�X�ǉ�
 * @param pBlActCont MMDL_BLACTCONT
 * @param code OBJ�R�[�h
 * g3dres �ǉ����郊�\�[�X*
 * @param type ���\�[�X�^�C�v
 * @retval GFL_BBDACT_RESUNIT_ID
 */
//--------------------------------------------------------------
static GFL_BBDACT_RESUNIT_ID BlActRes_AddRes(
    MMDL_BLACTCONT *pBlActCont, u16 code,
    GFL_G3D_RES *g3dres, GFL_BBDACT_RESTYPE type )
{
	GFL_BBDACT_RESUNIT_ID id;
	const OBJCODE_PARAM *prm;
	GFL_BBDACT_G3DRESDATA data;
  
	prm = MMDLSYS_GetOBJCodeParam( pBlActCont->fmmdlsys, code );
  
  data.g3dres = g3dres;
	data.texFmt = GFL_BBD_TEXFMT_PAL16;
	data.texSiz = prm->tex_size;
	data.celSizX = 32;				//������mdl_size����
	data.celSizY = 32;
  data.dataCut = type;
  
	id = GFL_BBDACT_AddResourceG3DResUnit( pBlActCont->pBbdActSys, &data, 1 );
  
  KAGAYA_Printf( "MMDL ADD RESOURCE CODE=%d,RESID=%d\n", code, id );
  return( id );
}

//======================================================================
//	test data
//======================================================================
/*
static const GFL_BBDACT_RESDATA testResTable[] = {
#ifdef MMDL_BLACT_HEAD3_TEST
	{ ARCID_MMDL_BTX, NARC_fldmmdl_btx_obj_kabi32_nsbtx, 
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
#else
	{ ARCID_FLDMAP_ACTOR, NARC_fld_act_hero_nsbtx, 
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x1024, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
#endif
	{ ARCID_FLDMAP_ACTOR, NARC_fld_act_achamo_nsbtx, 
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDMAP_ACTOR, NARC_fld_act_badman_nsbtx, 
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDMAP_ACTOR, NARC_fld_act_beachgirl_nsbtx,
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDMAP_ACTOR, NARC_fld_act_idol_nsbtx,
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDMAP_ACTOR, NARC_fld_act_lady_nsbtx,
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDMAP_ACTOR, NARC_fld_act_oldman1_nsbtx,
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDMAP_ACTOR, NARC_fld_act_policeman_nsbtx,
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
};
const u32 testResTableMax = NELEMS( testResTable );
*/

//======================================================================
//	���샂�f���Ǘ����Œ�`�����e�N�X�`���T�C�Y�w�肪
//	�r���{�[�h�A�N�^�[�Œ�`����Ă�����̂Ɠ��ꂩ�ǂ���
//	�v���v���Z�b�T���߂Ń`�F�b�N
//======================================================================
#define MMDL_CHECK_GFL_BBD_TEXSIZDEF ///<��`�Ń`�F�b�N�L��

#ifdef MMDL_CHECK_GFL_BBD_TEXSIZDEF
#include "field/fldmmdl_list_symbol.h"

#if (GFL_BBD_TEXSIZDEF_8x8-TEXSIZE_8x8)
#error (GFL_BBD_TEXSIZDEF_8x8-TEXSIZE_8x8)
#endif
#if (GFL_BBD_TEXSIZDEF_8x16-TEXSIZE_8x16)
#error (GFL_BBD_TEXSIZDEF_8x16-TEXSIZE_8x16)
#endif
#if (GFL_BBD_TEXSIZDEF_8x32-TEXSIZE_8x32)
#error (GFL_BBD_TEXSIZDEF_8x32-TEXSIZE_8x32)
#endif
#if (GFL_BBD_TEXSIZDEF_8x64-TEXSIZE_8x64)
#error (GFL_BBD_TEXSIZDEF_8x64-TEXSIZE_8x64)
#endif
#if (GFL_BBD_TEXSIZDEF_8x128-TEXSIZE_8x128)
#error (GFL_BBD_TEXSIZDEF_8x128-TEXSIZE_8x128)
#endif
#if (GFL_BBD_TEXSIZDEF_8x256-TEXSIZE_8x256)
#error (GFL_BBD_TEXSIZDEF_8x256-TEXSIZE_8x256)
#endif
#if (GFL_BBD_TEXSIZDEF_8x512-TEXSIZE_8x512)
#error (GFL_BBD_TEXSIZDEF_8x512-TEXSIZE_8x512)
#endif
#if (GFL_BBD_TEXSIZDEF_8x1024-TEXSIZE_8x1024)
#error (GFL_BBD_TEXSIZDEF_8x1024-TEXSIZE_8x1024)
#endif
#if (GFL_BBD_TEXSIZDEF_16x8-TEXSIZE_16x8)
#error (GFL_BBD_TEXSIZDEF_16x8-TEXSIZE_16x8)
#endif
#if (GFL_BBD_TEXSIZDEF_16x16-TEXSIZE_16x16)
#error (GFL_BBD_TEXSIZDEF_16x16-TEXSIZE_16x16)
#endif
#if (GFL_BBD_TEXSIZDEF_16x32-TEXSIZE_16x32)
#error (GFL_BBD_TEXSIZDEF_16x32-TEXSIZE_16x32)
#endif
#if (GFL_BBD_TEXSIZDEF_16x64-TEXSIZE_16x64)
#error (GFL_BBD_TEXSIZDEF_16x64-TEXSIZE_16x64)
#endif
#if (GFL_BBD_TEXSIZDEF_16x128-TEXSIZE_16x128)
#error (GFL_BBD_TEXSIZDEF_16x128-TEXSIZE_16x128)
#endif
#if (GFL_BBD_TEXSIZDEF_16x256-TEXSIZE_16x256)
#error (GFL_BBD_TEXSIZDEF_16x256-TEXSIZE_16x256)
#endif
#if (GFL_BBD_TEXSIZDEF_16x512-TEXSIZE_16x512)
#error (GFL_BBD_TEXSIZDEF_16x512-TEXSIZE_16x512)
#endif
#if (GFL_BBD_TEXSIZDEF_16x1024-TEXSIZE_16x1024)
#error (GFL_BBD_TEXSIZDEF_16x1024-TEXSIZE_16x1024)
#endif
#if (GFL_BBD_TEXSIZDEF_32x8-TEXSIZE_32x8)
#error (GFL_BBD_TEXSIZDEF_32x8-TEXSIZE_32x8)
#endif
#if (GFL_BBD_TEXSIZDEF_32x16-TEXSIZE_32x16)
#error (GFL_BBD_TEXSIZDEF_32x16-TEXSIZE_32x16)
#endif
#if (GFL_BBD_TEXSIZDEF_32x32-TEXSIZE_32x32)
#error (GFL_BBD_TEXSIZDEF_32x32-TEXSIZE_32x32)
#endif
#if (GFL_BBD_TEXSIZDEF_32x64-TEXSIZE_32x64)
#error (GFL_BBD_TEXSIZDEF_32x64-TEXSIZE_32x64)
#endif
#if (GFL_BBD_TEXSIZDEF_32x128-TEXSIZE_32x128)
#error (GFL_BBD_TEXSIZDEF_32x128-TEXSIZE_32x128)
#endif
#if (GFL_BBD_TEXSIZDEF_32x256-TEXSIZE_32x256)
#error (GFL_BBD_TEXSIZDEF_32x256-TEXSIZE_32x256)
#endif
#if (GFL_BBD_TEXSIZDEF_32x512-TEXSIZE_32x512)
#error (GFL_BBD_TEXSIZDEF_32x512-TEXSIZE_32x512)
#endif
#if (GFL_BBD_TEXSIZDEF_32x1024-TEXSIZE_32x1024)
#error (GFL_BBD_TEXSIZDEF_32x1024-TEXSIZE_32x1024)
#endif
#if (GFL_BBD_TEXSIZDEF_64x8-TEXSIZE_64x8)
#error (GFL_BBD_TEXSIZDEF_64x8-TEXSIZE_64x8)
#endif
#if (GFL_BBD_TEXSIZDEF_64x16-TEXSIZE_64x16)
#error (GFL_BBD_TEXSIZDEF_64x16-TEXSIZE_64x16)
#endif
#if (GFL_BBD_TEXSIZDEF_64x32-TEXSIZE_64x32)
#error (GFL_BBD_TEXSIZDEF_64x32-TEXSIZE_64x32)
#endif
#if (GFL_BBD_TEXSIZDEF_64x64-TEXSIZE_64x64)
#error (GFL_BBD_TEXSIZDEF_64x64-TEXSIZE_64x64)
#endif
#if (GFL_BBD_TEXSIZDEF_64x128-TEXSIZE_64x128)
#error (GFL_BBD_TEXSIZDEF_64x128-TEXSIZE_64x128)
#endif
#if (GFL_BBD_TEXSIZDEF_64x256-TEXSIZE_64x256)
#error (GFL_BBD_TEXSIZDEF_64x256-TEXSIZE_64x256)
#endif
#if (GFL_BBD_TEXSIZDEF_64x512-TEXSIZE_64x512)
#error (GFL_BBD_TEXSIZDEF_64x512-TEXSIZE_64x512)
#endif
#if (GFL_BBD_TEXSIZDEF_64x1024-TEXSIZE_64x1024)
#error (GFL_BBD_TEXSIZDEF_64x1024-TEXSIZE_64x1024)
#endif
#if (GFL_BBD_TEXSIZDEF_128x8-TEXSIZE_128x8)
#error (GFL_BBD_TEXSIZDEF_128x8-TEXSIZE_128x8)
#endif
#if (GFL_BBD_TEXSIZDEF_128x16-TEXSIZE_128x16)
#error (GFL_BBD_TEXSIZDEF_128x16-TEXSIZE_128x16)
#endif
#if (GFL_BBD_TEXSIZDEF_128x32-TEXSIZE_128x32)
#error (GFL_BBD_TEXSIZDEF_128x32-TEXSIZE_128x32)
#endif
#if (GFL_BBD_TEXSIZDEF_128x64-TEXSIZE_128x64)
#error (GFL_BBD_TEXSIZDEF_128x64-TEXSIZE_128x64)
#endif
#if (GFL_BBD_TEXSIZDEF_128x128-TEXSIZE_128x128)
#error (GFL_BBD_TEXSIZDEF_128x128-TEXSIZE_128x128)
#endif
#if (GFL_BBD_TEXSIZDEF_128x256-TEXSIZE_128x256)
#error (GFL_BBD_TEXSIZDEF_128x256-TEXSIZE_128x256)
#endif
#if (GFL_BBD_TEXSIZDEF_128x512-TEXSIZE_128x512)
#error (GFL_BBD_TEXSIZDEF_128x512-TEXSIZE_128x512)
#endif
#if (GFL_BBD_TEXSIZDEF_128x1024-TEXSIZE_128x1024)
#error (GFL_BBD_TEXSIZDEF_128x1024-TEXSIZE_128x1024)
#endif
#if (GFL_BBD_TEXSIZDEF_256x8-TEXSIZE_256x8)
#error (GFL_BBD_TEXSIZDEF_256x8-TEXSIZE_256x8)
#endif
#if (GFL_BBD_TEXSIZDEF_256x16-TEXSIZE_256x16)
#error (GFL_BBD_TEXSIZDEF_256x16-TEXSIZE_256x16)
#endif
#if (GFL_BBD_TEXSIZDEF_256x32-TEXSIZE_256x32)
#error (GFL_BBD_TEXSIZDEF_256x32-TEXSIZE_256x32)
#endif
#if (GFL_BBD_TEXSIZDEF_256x64-TEXSIZE_256x64)
#error (GFL_BBD_TEXSIZDEF_256x64-TEXSIZE_256x64)
#endif
#if (GFL_BBD_TEXSIZDEF_256x128-TEXSIZE_256x128)
#error (GFL_BBD_TEXSIZDEF_256x128-TEXSIZE_256x128)
#endif
#if (GFL_BBD_TEXSIZDEF_256x256-TEXSIZE_256x256)
#error (GFL_BBD_TEXSIZDEF_256x256-TEXSIZE_256x256)
#endif
#if (GFL_BBD_TEXSIZDEF_256x512-TEXSIZE_256x512)
#error (GFL_BBD_TEXSIZDEF_256x512-TEXSIZE_256x512)
#endif
#if (GFL_BBD_TEXSIZDEF_256x1024-TEXSIZE_256x1024)
#error (GFL_BBD_TEXSIZDEF_256x1024-TEXSIZE_256x1024)
#endif
#if (GFL_BBD_TEXSIZDEF_512x8-TEXSIZE_512x8)
#error (GFL_BBD_TEXSIZDEF_512x8-TEXSIZE_512x8)
#endif
#if (GFL_BBD_TEXSIZDEF_512x16-TEXSIZE_512x16)
#error (GFL_BBD_TEXSIZDEF_512x16-TEXSIZE_512x16)
#endif
#if (GFL_BBD_TEXSIZDEF_512x32-TEXSIZE_512x32)
#error (GFL_BBD_TEXSIZDEF_512x32-TEXSIZE_512x32)
#endif
#if (GFL_BBD_TEXSIZDEF_512x64-TEXSIZE_512x64)
#error (GFL_BBD_TEXSIZDEF_512x64-TEXSIZE_512x64)
#endif
#if (GFL_BBD_TEXSIZDEF_512x128-TEXSIZE_512x128)
#error (GFL_BBD_TEXSIZDEF_512x128-TEXSIZE_512x128)
#endif
#if (GFL_BBD_TEXSIZDEF_512x256-TEXSIZE_512x256)
#error (GFL_BBD_TEXSIZDEF_512x256-TEXSIZE_512x256)
#endif
#if (GFL_BBD_TEXSIZDEF_512x512-TEXSIZE_512x512)
#error (GFL_BBD_TEXSIZDEF_512x512-TEXSIZE_512x512)
#endif
#if (GFL_BBD_TEXSIZDEF_512x1024-TEXSIZE_512x1024)
#error (GFL_BBD_TEXSIZDEF_512x1024-TEXSIZE_512x1024)
#endif
#if (GFL_BBD_TEXSIZDEF_1024x8-TEXSIZE_1024x8)
#error (GFL_BBD_TEXSIZDEF_1024x8-TEXSIZE_1024x8)
#endif
#if (GFL_BBD_TEXSIZDEF_1024x16-TEXSIZE_1024x16)
#error (GFL_BBD_TEXSIZDEF_1024x16-TEXSIZE_1024x16)
#endif
#if (GFL_BBD_TEXSIZDEF_1024x32-TEXSIZE_1024x32)
#error (GFL_BBD_TEXSIZDEF_1024x32-TEXSIZE_1024x32)
#endif
#if (GFL_BBD_TEXSIZDEF_1024x64-TEXSIZE_1024x64)
#error (GFL_BBD_TEXSIZDEF_1024x64-TEXSIZE_1024x64)
#endif
#if (GFL_BBD_TEXSIZDEF_1024x128-TEXSIZE_1024x128)
#error (GFL_BBD_TEXSIZDEF_1024x128-TEXSIZE_1024x128)
#endif
#if (GFL_BBD_TEXSIZDEF_1024x256-TEXSIZE_1024x256)
#error (GFL_BBD_TEXSIZDEF_1024x256-TEXSIZE_1024x256)
#endif
#if (GFL_BBD_TEXSIZDEF_1024x512-TEXSIZE_1024x512)
#error (GFL_BBD_TEXSIZDEF_1024x512-TEXSIZE_1024x512)
#endif
#if (GFL_BBD_TEXSIZDEF_1024x1024-TEXSIZE_1024x1024)
#error (GFL_BBD_TEXSIZDEF_1024x1024-TEXSIZE_1024x1024)
#endif
#endif //MMDL_CHECK_GFL_BBD_TEXSIZDEF
