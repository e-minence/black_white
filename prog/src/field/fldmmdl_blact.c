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

#ifdef FLDMMDL_BLACT_HEAD3_TEST
#include "test_graphic/fldmmdl_btx.naix"
#endif

//======================================================================
//	define
//======================================================================

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	FLDMMDL_BLACTCONT
//--------------------------------------------------------------
struct _TAG_FLDMMDL_BLACTCONT{
	GFL_BBDACT_SYS *pBbdActSys;
	GFL_BBDACT_RESUNIT_ID bbdActResUnitID;
	GFL_BBDACT_ACTUNIT_ID bbdActActUnitID;
	u16 bbdActResCount;
	u16 bbdActActCount;
	
	FLDMMDLSYS *fmmdlsys;
};

//======================================================================
//	proto
//======================================================================
static void BlActFunc( GFL_BBDACT_SYS *bbdActSys, int actIdx, void *work );

static const FLDMMDL_BBDACT_ANMTBL * BlActAnm_GetAnmTbl( u32 no );

//test data
const GFL_BBDACT_RESDATA testResTable[];
const u32 testResTableMax;

//======================================================================
//	�t�B�[���h���샂�f���@�r���{�[�h�A�N�^�[�Ǘ�
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDL_BLACTCONT �r���{�[�h�A�N�^�[�Ǘ��@�Z�b�g�A�b�v
 * @param	fmmdlsys	�\�z�ς݂�FLDMMDLSYS
 * @param	pBbdActSys	�\�z�ς݂�GFL_BBDACT_SYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_BLACTCONT_Setup(
	FLDMMDLSYS *fmmdlsys, GFL_BBDACT_SYS *pBbdActSys )
{
	HEAPID heapID;
	GFL_BBDACT_ACTDATA *actData;
	GFL_BBDACT_ACTUNIT_ID actUnitID;
	FLDMMDL_BLACTCONT *pBlActCont;
	
	heapID = FLDMMDLSYS_GetHeapID( fmmdlsys );
	pBlActCont = GFL_HEAP_AllocClearMemory(
			heapID, sizeof(FLDMMDL_BLACTCONT) );
	
	pBlActCont->fmmdlsys = fmmdlsys;
	pBlActCont->pBbdActSys = pBbdActSys;
	
	pBlActCont->bbdActResCount = testResTableMax;
	pBlActCont->bbdActResUnitID = GFL_BBDACT_AddResourceUnit(
			pBbdActSys, testResTable, pBlActCont->bbdActResCount );
	
	FLDMMDLSYS_SetBlActCont( fmmdlsys, pBlActCont );
}

//--------------------------------------------------------------
/**
 * FLDMMDL_BLACTCONT �r���{�[�h�A�N�^�[�Ǘ��@�폜
 * @param	fmmdlsys	FLDMMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_BLACTCONT_Release( FLDMMDLSYS *fmmdlsys )
{
	FLDMMDL_BLACTCONT *pBlActCont = FLDMMDLSYS_GetBlActCont( fmmdlsys );
	
	GFL_BBDACT_RemoveResourceUnit( pBlActCont->pBbdActSys,
		pBlActCont->bbdActResUnitID, pBlActCont->bbdActResCount );
	GFL_HEAP_FreeMemory( pBlActCont );
	
	FLDMMDLSYS_SetBlActCont( fmmdlsys, NULL );
}

//======================================================================
//	�t�B�[���h���샂�f�� �r���{�[�h�A�N�^�[�ǉ�
//======================================================================
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[ �ǉ�
 * @param	fmmdl	FLDMMDL
 * @param	resID	�ǉ�����A�N�^�[�Ŏg�p���郊�\�[�XID
 * @retval	GFL_BBDACT_ACTUNIT_ID
 */
//--------------------------------------------------------------
GFL_BBDACT_ACTUNIT_ID FLDMMDL_BLACTCONT_AddActor( FLDMMDL *fmmdl, u32 resID )
{
	VecFx32 pos;
	GFL_BBDACT_ACTDATA actData;
	GFL_BBDACT_ACTUNIT_ID actID;
	const FLDMMDL_BBDACT_ANMTBL *anmTbl;
	FLDMMDLSYS *fmmdlsys = (FLDMMDLSYS*)FLDMMDL_GetFldMMdlSys( fmmdl );
	FLDMMDL_BLACTCONT *pBlActCont = FLDMMDLSYS_GetBlActCont( fmmdlsys );

	FLDMMDL_GetDrawVectorPos( fmmdl, &pos );
	
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

	if( resID == 0 ){ //��
		anmTbl = BlActAnm_GetAnmTbl( FLDMMDL_BLACT_ANMTBLNO_HERO );
	}else{
		anmTbl = BlActAnm_GetAnmTbl( FLDMMDL_BLACT_ANMTBLNO_BLACT );
	}
	
	GFL_BBDACT_SetAnimeTable( pBlActCont->pBbdActSys,
		actID, (GFL_BBDACT_ANMTBL)anmTbl->pAnmTbl, anmTbl->anm_max );
	
	GFL_BBDACT_SetAnimeIdxOn(
			pBlActCont->pBbdActSys, actID, 0 );
	return( actID );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[ �폜
 * @param	fmmdl	FLDMMDL*
 * @param	actID GFL_BBDACT_ACTUNIT_ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_BLACTCONT_DeleteActor( FLDMMDL *fmmdl, u32 actID )
{
	FLDMMDLSYS *pFMMdlSys = (FLDMMDLSYS*)FLDMMDL_GetFldMMdlSys( fmmdl );
	FLDMMDL_BLACTCONT *pBlActCont = FLDMMDLSYS_GetBlActCont( pFMMdlSys );
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
 * @param	pBlActCont FLDMMDL_BLACTCONT
 * @retval	GFL_BBDACT_SYS
 */
//--------------------------------------------------------------
GFL_BBDACT_SYS * FLDMMDL_BLACTCONT_GetBbdActSys(
		FLDMMDL_BLACTCONT *pBlActCont )
{
	return( pBlActCont->pBbdActSys );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�Ǘ� GFL_BBDACT_RESUNIT_ID�擾
 * @param	pBlActCont FLDMMDL_BLACTCONT
 * @retval	GFL_BBDACT_RESUNIT_ID
 */
//--------------------------------------------------------------
GFL_BBDACT_RESUNIT_ID FLDMMDL_BLACTCONT_GetResUnitID(
		FLDMMDL_BLACTCONT *pBlActCont )
{
	return( pBlActCont->bbdActResUnitID );
}

//======================================================================
//	parts
//======================================================================
//--------------------------------------------------------------
/**
 * �e�[�u���ԍ�����A�N�^�[�A�j���e�[�u���擾
 * @param	no	FLDMMDL_BLACT_ANMTBLNO_MAX
 * @retval	FLDMMDL_BBDACT_ANMTBL*
 */
//--------------------------------------------------------------
static const FLDMMDL_BBDACT_ANMTBL * BlActAnm_GetAnmTbl( u32 no )
{
	GF_ASSERT( no < FLDMMDL_BLACT_ANMTBLNO_MAX );
	return( &DATA_FLDMMDL_BBDACT_ANM_ListTable[no] );
}

//======================================================================
//	test data
//======================================================================
static const GFL_BBDACT_RESDATA testResTable[] = {
#ifdef FLDMMDL_BLACT_HEAD3_TEST
	{ ARCID_FLDMMDL_BTX, NARC_fldmmdl_btx_obj_kabi32_nsbtx, 
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
