//============================================================================================
/**
 * @file	field_buildmodel.c
 * @brief		�z�u���f���̐���
 * @author	tamada GAMEFREAK inc.
 * @date	2009.04.20
 *
 * @todo
 * �������팸�F�i�P�j�e�N�X�`�����\�[�X���G���A�P�ʂœ�������i�Q�j�A�j�����\�[�X�̏d�������炷
 */
//============================================================================================

#include <gflib.h>

#include "arc_def.h"
#include "field/zonedata.h"
#include "field/areadata.h"

#include "field/field_const.h"
#include "field_buildmodel.h"
#include "field_bmanime.h"

#include "system/rtc_tool.h"  //GFL_RTC_GetTimeZone

#include "field_g3d_mapper.h"		//���L�w�b�_�ɕK�v

#include "savedata/save_control.h"    //�e�X�g�Ń����_���}�b�v�̓s�s�̎�ނ��擾
#include "savedata/randommap_save.h"  //�e�X�g�Ń����_���}�b�v�̓s�s�̎�ނ��擾

#include "arc/fieldmap/area_id.h"
#include "arc/fieldmap/buildmodel_info.naix"
#include "arc/fieldmap/buildmodel_outdoor.naix"
#include "arc/fieldmap/buildmodel_indoor.naix"

#include "map/dp3format.h"

#include "sound/pm_sndsys.h"  //PMSND_
#include "sound/wb_sound_data.sadl" //SE�w��

#define BMODEL_TEXSET (1)

//============================================================================================
//  DEBUG ���\�[�X�������g�p�ʂ̌���
//============================================================================================
#ifdef BMODEL_DEBUG_RESOURCE_MEMORY_SIZE

static u32 BMODEL_DEBUG_RESOURCE_MemorySize = 0;  // ���\�[�X

// ���\�[�X�������[�T�C�Y�ǉ�
#define BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( x )    BMODEL_DEBUG_RESOURCE_MemorySize += GFL_HEAP_DEBUG_GetMemoryBlockSize((x))
// ���\�[�X�������[�T�C�Y�폜
#define BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( x )    BMODEL_DEBUG_RESOURCE_MemorySize -= GFL_HEAP_DEBUG_GetMemoryBlockSize((x))
// ���\�[�X�������[�T�C�Y �A���P�[�^�p �ǉ�
#define BMODEL_DEBUG_RESOURCE_MEMORY_ALLOCATOR_SIZE_Plus( x )    BMODEL_DEBUG_RESOURCE_MemorySize += NNS_FndGetSizeForMBlockExpHeap((x))
// ���\�[�X�������[�T�C�Y  �폜
#define BMODEL_DEBUG_RESOURCE_MEMORY_ALLOCATOR_SIZE_Minus( x )    BMODEL_DEBUG_RESOURCE_MemorySize -= NNS_FndGetSizeForMBlockExpHeap((x))
// ���\�[�X�������[�T�C�Y �N���[���`�F�b�N
#define BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_IsAllDelete    GF_ASSERT( BMODEL_DEBUG_RESOURCE_MemorySize == 0 )


#else   // BMODEL_DEBUG_RESOURCE_MEMORY_SIZE


// ���\�[�X�������[�T�C�Y�ǉ�
#define BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( x )   /**/ 
// ���\�[�X�������[�T�C�Y�폜
#define BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( x )    /**/
// ���\�[�X�������[�T�C�Y �A���P�[�^�p �ǉ�
#define BMODEL_DEBUG_RESOURCE_MEMORY_ALLOCATOR_SIZE_Plus( x )   /**/ 
// ���\�[�X�������[�T�C�Y  �폜
#define BMODEL_DEBUG_RESOURCE_MEMORY_ALLOCATOR_SIZE_Minus( x )   /**/ 
// ���\�[�X�������[�T�C�Y �N���[���`�F�b�N
#define BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_IsAllDelete   /**/ 

#endif  // BMODEL_DEBUG_RESOURCE_MEMORY_SIZE

//============================================================================================
//============================================================================================

enum {
  FLD_G3D_MAP_OBJST_MAX = 32,
  FLD_G3D_MAP_OBJID_NULL = 0xffffffff,

  MAPBLOCK_MAX = 9,
};

//------------------------------------------------------------------
//------------------------------------------------------------------
enum { GLOBAL_OBJ_ANMCOUNT	= 4 };

//------------------------------------------------------------------
//  �O������\�Ȕz�u���f���̓o�^���
//------------------------------------------------------------------
enum { BMODEL_USE_MAX = 6 };

//------------------------------------------------------------------
//  �z�u���f���o�^�L�[�p�̒�`
//------------------------------------------------------------------
enum {
  BMODEL_UNIQ_VALUE = 0xA5A0,
  BMODEL_UNIQ_MASK  = 0xfff0,
};

//------------------------------------------------------------------
//------------------------------------------------------------------
enum{ 
  FILEID_BMODEL_ANIMEDATA = NARC_buildmodel_info_buildmodel_anime_bin,
  FILEID_BMINFO_OUTDOOR = NARC_buildmodel_info_buildmodel_outdoor_bin,
  FILEID_BMINFO_INDOOR = NARC_buildmodel_info_buildmodel_indoor_bin,
};

//------------------------------------------------------------------
//------------------------------------------------------------------
enum {
  BMODEL_ID_MAX = 400,  //�Ƃ肠����
  BMODEL_ENTRY_MAX = 128,  //�Ƃ肠����

  BMODEL_ID_NG  = BMODEL_ID_MAX,
  BMODEL_ENTRY_NG = BMODEL_ENTRY_MAX,

  BMANIME_NULL_ID  = 0xffff,
};

static const BMODEL_ID BM_SUBMODEL_NULL_ID = 0xffff;

//------------------------------------------------------------------
///�A�j�����
//------------------------------------------------------------------
typedef enum {
  BM_ANMMODE_NOTHING = 0, ///<�A�j����ԁF�Ȃ�
  BM_ANMMODE_LOOP,        ///<�A�j����ԁF���[�v����
  BM_ANMMODE_STOP,        ///<�A�j����ԁF��~��
  BM_ANMMODE_TEMPORARY,   ///<�A�j����ԁF�ꎞ�I�ɓK�p��
  BM_ANMMODE_REVERSE,     ///<�A�j����ԁF�t�Đ��œK�p��
  BM_ANMMODE_TIMEZONE,    ///<�A�j����ԁF���ԑтŐ���
}BM_ANMMODE;

//------------------------------------------------------------------
/// �z�u���f���A�j�����
//------------------------------------------------------------------
typedef struct _FIELD_BMANIME_DATA
{ 
  u8 anm_type;  ///<BMANIME_TYPE  �A�j���̎�ގw��
  u8 prg_type;  ///<BMANIME_PROG_TYPE ����v���O�����̎�ގw��
  u8 anmset_num; ///<�A�j���Z�b�g��
  u8 ptn_count; ///<�p�^�[����
  u16 IDs[GLOBAL_OBJ_ANMCOUNT]; ///<�A�j���A�[�J�C�u�w��ID
}FIELD_BMANIME_DATA;

//------------------------------------------------------------------
/// �z�u���f�����
//------------------------------------------------------------------
typedef struct {
  BMODEL_ID bm_id;              ///<�z�u���f��ID
  u16 prog_id;                  ///<BM_PROG_ID �v���O�����w��ID
  BMODEL_ID sub_bm_id;          ///<�]�����f���w��ID
  s16 sx, sy, sz;               ///<�]�����f���̑��Έʒu
  u16 anm_id;                   ///<�A�j���w��ID
  FIELD_BMANIME_DATA animeData; ///<�A�j���w��f�[�^
}BMINFO;

//------------------------------------------------------------------
/// �z�u���f���I�u�W�F�N�g���\�[�X
//------------------------------------------------------------------
typedef struct {
  const BMINFO * bmInfo;              ///<�z�u���f�����
  GFL_G3D_RES*  g3DresMdl;            ///<���f�����\�[�X
#ifndef BMODEL_TEXSET
  GFL_G3D_RES*  g3DresTex;            ///<�e�N�X�`�����\�[�X
#endif
  GFL_G3D_RES*  g3DresAnm[GLOBAL_OBJ_ANMCOUNT];  ///<�A�j�����\�[�X
}OBJ_RES;

//------------------------------------------------------------------
/// ���f������I�u�W�F�N�g
//------------------------------------------------------------------
typedef struct {
  GFL_G3D_OBJ*  g3Dobj;              ///<�I�u�W�F�N�g�n���h��
  const OBJ_RES * res;              ///<���\�[�X�ւ̃|�C���^
  BM_ANMMODE anmMode[GLOBAL_OBJ_ANMCOUNT];  ///<�ێ��A�j���̏��
}OBJ_HND;

//------------------------------------------------------------------
/**
 * FLD_G3D_MAP�̕ێ�����FLD_G3D_MAP_GLOBALOBJ_ST��
 * �Q�ƁA�R���g���[�����邽�߂̃��b�p�[�I�u�W�F�N�g
 */
//------------------------------------------------------------------
struct _G3DMAPOBJST{
  FLD_G3D_MAP * g3Dmap;   ///<����FLD_G3D_MAP�ւ̎Q��
  u32 entryNoBackup;      ///<���f���w��ID�̃o�b�N�A�b�v
  u16 index;              ///<����g3Dmap���ł̃C���f�b�N�X
  u16 viewFlag;           ///<���ݒ�t���O
  FLD_G3D_MAP_GLOBALOBJ_ST * objSt; ///<���I�u�W�F�N�g�ւ̃|�C���^
};

//------------------------------------------------------------------
/// �z�u���f�����䃏�[�N�i�O������̐���p�j
//------------------------------------------------------------------
struct _FIELD_BMODEL {
  FIELD_BMODEL_MAN * man; ///<�}�l�W���[�ւ̃|�C���^
  u32 currentAnmIdx;    ///<�A�j��No�ێ��i�ŐV�̂��́j
  G3DMAPOBJST * objst;  ///<���ƂȂ���OBJST�ւ̃|�C���^
  OBJ_HND objHdl;         ///<���f������I�u�W�F�N�g
  GFL_G3D_OBJSTATUS g3dObjStatus; ///<�\������I�u�W�F�N�g
};

//------------------------------------------------------------------
///���ԑуA�j�����䃏�[�N
//------------------------------------------------------------------
typedef struct {
  TIMEZONE NowTimeZone;   ///<���݂̎��ԑ�
  TIMEZONE OldTimeZone;   ///<���O�̎��ԑ�
  BOOL update_flag;       ///<�X�V���K�v���ǂ���
  u8 index;               ///<���ԑт���Z�o���ꂽ�A�j���w��
}TIMEANIME_CTRL;

//------------------------------------------------------------------
/** 
 * @brief �z�u���f���}�l�W���[�p���䃏�[�N
 */
//------------------------------------------------------------------
struct _FIELD_BMODEL_MAN
{
	HEAPID heapID;

  FLDMAPPER * fldmapper;  //�`��I�t�Z�b�g�擾�ɕK�v�B�ł�����s�v�ɂ�����
  TIMEANIME_CTRL tmanm_ctrl;  ///<���ԑуA�j�����䃏�[�N

  ///�z�u���f��ID���o�^ID�̕ϊ��e�[�u��
	u8 BMIDToEntryTable[BMODEL_ID_MAX];

  ///�z�u���f���o�^��
	u16 entryCount;
  ///�z�u���f�����
  BMINFO bmInfo[BMODEL_ENTRY_MAX];
  ///�o�^ID���z�u���f��ID�̕ϊ��e�[�u��
	BMODEL_ID entryToBMIDTable[BMODEL_ENTRY_MAX];

  ///�K�p����z�u���f���A�[�J�C�u�w��
	ARCID mdl_arc_id;
  
	FLD_G3D_MAP_GLOBALOBJ	g3dMapObj;						///<���ʃI�u�W�F�N�g

	u32	objRes_Count;		  		///<���ʃI�u�W�F�N�g���\�[�X��
	OBJ_RES * objRes;					///<���ʃI�u�W�F�N�g���\�[�X

#ifdef BMODEL_TEXSET
	GFL_G3D_RES*	g3DresTex;  ///<�e�N�X�`���Z�b�g
#endif

  u32 objHdl_Count;
  OBJ_HND * objHdl;

  FIELD_BMODEL * bmodels[BMODEL_USE_MAX];

  G3DMAPOBJST g3DmapObjSt[FLD_G3D_MAP_OBJST_MAX * MAPBLOCK_MAX];
};

//============================================================================================
//
//    �f�o�b�O�p�֐�
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static inline BMODEL_ID BMIDAssert(BMODEL_ID bm_id)
{
  if (bm_id >= BMODEL_ID_MAX)
  {
    OS_TPrintf("BuildModel ID over %d\n",bm_id );
    GF_ASSERT(0);
    bm_id = 0;
  }
  return bm_id;
}
#define BMID_ASSERT(bm_id) {bm_id = BMIDAssert(bm_id);}

//------------------------------------------------------------------
//------------------------------------------------------------------
static inline u8 EntryNoAssert(const FIELD_BMODEL_MAN * man, u8 entryNo)
{
  if (entryNo >= BMODEL_ENTRY_MAX || entryNo >= man->entryCount)
  {
    OS_TPrintf("BuildModel Entry No Over(%d)\n",entryNo);
    GF_ASSERT(0);
    entryNo = 0;
  }
  return entryNo;
}

#define ENTRYNO_ASSERT(man, entryNo) {entryNo = EntryNoAssert(man, entryNo);}

//------------------------------------------------------------------
//------------------------------------------------------------------
static inline u8 AnimeNoAssert(u8 anmNo)
{
  if (anmNo >= GLOBAL_OBJ_ANMCOUNT)
  {
    OS_TPrintf("anmNo Index Over!!(%d)\n", anmNo);
    GF_ASSERT(0);
    anmNo = 0;
  }
  return anmNo;
}

#define ANIMENO_ASSERT(anmNo) {anmNo = AnimeNoAssert(anmNo);}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void makeBMIDToEntryTable(FIELD_BMODEL_MAN * man);
static u16 calcArcIndex(u16 area_id);

static void loadEntryToBMIDTable(FIELD_BMODEL_MAN * man, u16 arc_id, u16 file_id);
static u8 BMIDtoEntryNo(const FIELD_BMODEL_MAN * man, BMODEL_ID id);

#ifdef BMODEL_TEXSET
static void loadBMTextureSet(FIELD_BMODEL_MAN * man, u16 arc_id, u16 file_id);
static void freeBMTextureSet(FIELD_BMODEL_MAN * man);
#endif

//------------------------------------------------------------------
//------------------------------------------------------------------
static const BMINFO * BMODELMAN_GetBMInfo(const FIELD_BMODEL_MAN * man, BMODEL_ID bm_id);

static void BMINFO_Load(FIELD_BMODEL_MAN * man, u16 file_id);
static void BMINFO_init(BMINFO * bmInfo);
//�A�j���f�[�^�̎擾����
static const FIELD_BMANIME_DATA * BMINFO_getAnimeData(const BMINFO * bmInfo);

static u32 BMANIME_getCount(const FIELD_BMANIME_DATA * data);
static void BMANIME_init(FIELD_BMANIME_DATA * data);
static void DEBUG_BMANIME_dump(const FIELD_BMANIME_DATA * data);

//------------------------------------------------------------------
//------------------------------------------------------------------

static void createAllResource(FIELD_BMODEL_MAN * man);
static void deleteAllResource(FIELD_BMODEL_MAN * man);

static void createFullTimeObjHandle(FIELD_BMODEL_MAN * man, FLD_G3D_MAP_GLOBALOBJ * g3dMapObj);
static void deleteFullTimeObjHandle(FIELD_BMODEL_MAN * man, FLD_G3D_MAP_GLOBALOBJ * g3dMapObj);

static void OBJRES_initialize( FIELD_BMODEL_MAN * man, OBJ_RES * objRes, BMODEL_ID bm_id);
static void OBJRES_finalize( OBJ_RES * objRes );
#ifndef BMODEL_TEXSET
static GFL_G3D_RES* OBJRES_getResTex(const OBJ_RES * resTex);
#endif
static void OBJHND_initialize(const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl, const OBJ_RES* objRes);
static void OBJHND_finalize( OBJ_HND * objHdl );
static void OBJHND_animate(const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_setAnime( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req);
static BOOL OBJHND_getAnimeStatus( const OBJ_HND * objHdl, u32 anmNo);
static fx32 OBJHND_getAnimeFrame( const OBJ_HND * objHdl, u32 anmNo);

static void TIMEANIME_CTRL_init( TIMEANIME_CTRL * tmanm_ctrl );
static void TIMEANIME_CTRL_update( TIMEANIME_CTRL * tmanm_ctrl );
static BOOL TIMEANIME_CTRL_needUpdate( const TIMEANIME_CTRL * tmanm_ctrl );
static u8   TIMEANIME_CTRL_getIndex( const TIMEANIME_CTRL * tmanm_ctrl );

//------------------------------------------------------------------
//------------------------------------------------------------------
static G3DMAPOBJST * G3DMAPOBJST_create(
    FIELD_BMODEL_MAN * man, FLD_G3D_MAP * g3Dmap, FLD_G3D_MAP_GLOBALOBJ_ST * status, int idx);
static void G3DMAPOBJST_init(FIELD_BMODEL_MAN * man, G3DMAPOBJST * obj);
static void G3DMAPOBJST_deleteByObject(FIELD_BMODEL_MAN * man, G3DMAPOBJST * obj);
static void G3DMAPOBJST_deleteByG3Dmap(FIELD_BMODEL_MAN * man, FLD_G3D_MAP * g3Dmap);

static void FIELD_BMODEL_Draw( const FIELD_BMODEL * bmodel );

static void BMODELMAN_EntryBuildModel(FIELD_BMODEL_MAN * man, FIELD_BMODEL * bmodel);
static void BMODELMAN_releaseBuildModel(FIELD_BMODEL_MAN * man, FIELD_BMODEL * bmodel);

static void makeRect(FLDHIT_RECT * rect, const VecFx32 * pos);
//============================================================================================
//
//
//    �O�����J�֐��F
//    �}�l�W���[������폜�E���[�h�E���C��
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �z�u���f���}�l�W���[����
 * @param heapID
 */
//------------------------------------------------------------------
FIELD_BMODEL_MAN * FIELD_BMODEL_MAN_Create(HEAPID heapID, FLDMAPPER * fldmapper)
{	
  int i;
	FIELD_BMODEL_MAN * man = GFL_HEAP_AllocMemory(heapID, sizeof(FIELD_BMODEL_MAN));
	man->heapID = heapID;
  man->fldmapper = fldmapper;
  TIMEANIME_CTRL_init( &man->tmanm_ctrl );

  for (i = 0; i < BMODEL_USE_MAX; i++)
  {
    man->bmodels[i] = NULL;
  }
  for (i = 0; i < NELEMS(man->g3DmapObjSt); i++)
  {
    G3DMAPOBJST_init( man, &man->g3DmapObjSt[i] );
  }

	man->g3dMapObj.gobj = NULL;
	man->g3dMapObj.gobjCount = 0;
	man->g3dMapObj.gobjIDexchange = NULL;

	man->g3dMapObj.gddobj = NULL;
	man->g3dMapObj.gddobjCount = 0;
	man->g3dMapObj.gddobjIDexchange = NULL;
	
  man->objRes_Count = 0;
  man->objRes = NULL;

  man->objHdl_Count = 0;
  man->objHdl = NULL;

	return man;
}

//------------------------------------------------------------------
/**
 * @brief �z�u���f���}�l�W���[����
 * @param man �z�u���f���}�l�W���[�ւ̃|�C���^
 */
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_Delete(FIELD_BMODEL_MAN * man)
{	
  int i;
  for (i = 0; i < BMODEL_USE_MAX; i++)
  {
    if (man->bmodels[i])
    {
      FIELD_BMODEL_Delete(man->bmodels[i]);
    }
  }
  deleteFullTimeObjHandle( man, &man->g3dMapObj );
  deleteAllResource( man );

#ifdef BMODEL_TEXSET
  freeBMTextureSet( man );
#endif
	GFL_HEAP_FreeMemory(man);

  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_IsAllDelete;
}

//------------------------------------------------------------------
/**
 * @brief �z�u���f���}�l�W���[�F�X�V����
 */
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_Main(FIELD_BMODEL_MAN * man)
{ 
  int i;

  TIMEANIME_CTRL_update( &man->tmanm_ctrl );

  for (i = 0; i < BMODEL_USE_MAX; i++)
  {
    if (man->bmodels[i])
    {
      OBJ_HND * objHdl = &man->bmodels[i]->objHdl;
      OBJHND_animate( man, objHdl );
    }
  }

  for( i=0; i<man->objHdl_Count; i++ ){
    OBJ_HND * objHdl = &man->objHdl[i];
    OBJHND_animate( man, objHdl );
  }
}

//------------------------------------------------------------------
/**
 * @brief   �z�u���f���}�l�W���[�F�`�揈��
 */
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_Draw(FIELD_BMODEL_MAN * man)
{
  int i;
  for (i = 0; i < BMODEL_USE_MAX; i++)
  {
    if (man->bmodels[i])
    {
      FIELD_BMODEL_Draw(man->bmodels[i]);
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief �z�u���f���f�[�^�ǂݍ��ݏ���
 * @param man �z�u���f���}�l�W���[�ւ̃|�C���^
 */
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_Load(FIELD_BMODEL_MAN * man, u16 zoneid, const AREADATA * areadata)
{	
	u16 area_id = ZONEDATA_GetAreaID(zoneid);
	u16 bmlist_index = calcArcIndex(area_id);
  ARCID bmlist_arc_id,bmtex_arc_id;
  u16 model_info_dataid;
  int i;

	GFL_STD_MemClear(man->BMIDToEntryTable, sizeof(man->BMIDToEntryTable));
	GFL_STD_MemFill16(man->entryToBMIDTable, BMODEL_ENTRY_NG, sizeof(man->entryToBMIDTable));
  for (i = 0; i < BMODEL_ENTRY_MAX; i++)
  { 
    BMINFO_init( &man->bmInfo[i] );
  }

  if (AREADATA_GetInnerOuterSwitch(areadata) != 0) 
  {	
    man->mdl_arc_id = ARCID_BMODEL_OUTDOOR;
    model_info_dataid = FILEID_BMINFO_OUTDOOR;
    bmlist_arc_id = ARCID_BMODEL_IDX_OUTDOOR;
#ifdef BMODEL_TEXSET
    bmtex_arc_id = ARCID_BMODEL_TEXSET_OUTDOOR;
#endif
  }
  else
  {	
    man->mdl_arc_id = ARCID_BMODEL_INDOOR;
    model_info_dataid = FILEID_BMINFO_INDOOR;
    bmlist_arc_id = ARCID_BMODEL_IDX_INDOOR;
#ifdef BMODEL_TEXSET
    bmtex_arc_id = ARCID_BMODEL_TEXSET_INDOOR;
#endif
  }

  //�G���A�ʔz�u���f��ID���X�g�̓ǂݍ���
  loadEntryToBMIDTable(man, bmlist_arc_id, bmlist_index);

#ifdef BMODEL_TEXSET
  //�G���A�ʔz�u���f���e�N�X�`���Z�b�g�̓ǂݍ���
  loadBMTextureSet(man, bmtex_arc_id, bmlist_index );
#endif

	//ID���o�^�����̕ϊ��f�[�^�e�[�u������
	makeBMIDToEntryTable(man);

  //�K�v�Ȕz�u���f���f�[�^�̓ǂݍ���
  BMINFO_Load(man, model_info_dataid);

  createAllResource( man );
  createFullTimeObjHandle( man, &man->g3dMapObj );
}

//============================================================================================
//
//    �O�����J�֐��F
//
//============================================================================================
static u8 BMODELMAN_GetEntryIndex(const FIELD_BMODEL_MAN* man, BMODEL_ID id);
static BOOL BMODELMAN_GetSubModel(const FIELD_BMODEL_MAN * man, 
    u16 bm_id, VecFx32 * ofs, u32 * entryNo);

//------------------------------------------------------------------
/**
 * @brief �z�u���f��ID��o�^�ςݔz�u���f���̃C���f�b�N�X�ɕϊ�����
 * @param man �z�u���f���}�l�W���[�ւ̃|�C���^
 */
//------------------------------------------------------------------
static u8 BMODELMAN_GetEntryIndex(const FIELD_BMODEL_MAN * man, BMODEL_ID id)
{	
  return BMIDtoEntryNo( man, id );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static const BMINFO * BMODELMAN_GetBMInfo(const FIELD_BMODEL_MAN * man, BMODEL_ID bm_id)
{
  u16 entryNo = BMIDtoEntryNo( man, bm_id );
  return &man->bmInfo[entryNo];
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL BMODELMAN_GetSubModel(const FIELD_BMODEL_MAN * man, 
    u16 bm_id, VecFx32 * ofs, u32 * entryNo)
{
  u16 submodel_id;
  const BMINFO * bmInfo;
  bmInfo = BMODELMAN_GetBMInfo( man, bm_id );
  submodel_id = bmInfo->sub_bm_id;
  if (submodel_id == BM_SUBMODEL_NULL_ID)
  {
    return FALSE;
  }
  *entryNo = BMIDtoEntryNo(man, submodel_id);
  VEC_Set( ofs, bmInfo->sx << FX32_SHIFT, bmInfo->sy << FX32_SHIFT, bmInfo->sz << FX32_SHIFT );
  return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief �q�[�v�w��̎擾
 * @param man
 * @return  HEAPID  �q�[�v�w��
 */
//------------------------------------------------------------------
HEAPID FIELD_BMODEL_MAN_GetHeapID(const FIELD_BMODEL_MAN * man)
{
  return man->heapID;
}

//============================================================================================
//
//    �O�����J�֐��F
//      �z�u���f���o�^�E�폜����
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static inline u16 getResourceID( const PositionSt * objStatus )
{
  return objStatus->resourceID + ( objStatus->resourceIDupper << 8 );
}
//------------------------------------------------------------------
//------------------------------------------------------------------
int FIELD_BMODEL_MAN_ResistAllMapObjects
(FIELD_BMODEL_MAN * man, FLD_G3D_MAP * g3Dmap, const PositionSt* objStatus, u32 objCount)
{
  FLD_G3D_MAP_GLOBALOBJ_ST status;
  int dataCount, resistCount, count;

  for( dataCount=0, resistCount = 0, count = objCount; dataCount<count ; resistCount++, dataCount++ )
  {
    if (resistCount >= FLD_G3D_MAP_OBJST_MAX)
    {
      OS_Printf("�}�b�v�u���b�N���̔z�u���f������%d�𒴂��Ă��邽�ߕ\���ł��܂���\n",
          FLD_G3D_MAP_OBJST_MAX);
      GF_ASSERT(0);
    }

    FIELD_BMODEL_MAN_ResistMapObject(man, g3Dmap, &objStatus[dataCount], resistCount);

    if (TRUE == BMODELMAN_GetSubModel(man,
          getResourceID( &objStatus[dataCount] ), &status.trans, &status.id) )
    {
      TAMADA_Printf("Resist Sub Model:index(%d) model id(%d)\n", dataCount, status.id);
      resistCount++;
      status.rotate = objStatus[dataCount].rotate;
      status.trans.x += objStatus[dataCount].xpos;
      status.trans.y += objStatus[dataCount].ypos;
      status.trans.z -= objStatus[dataCount].zpos;
      G3DMAPOBJST_create(man, g3Dmap, &status, resistCount);
    }

  }

  // ���ۂ̓o�^����Ԃ�
  return resistCount;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_ReleaseAllMapObjects
(FIELD_BMODEL_MAN * man, FLD_G3D_MAP * g3Dmap)
{
  G3DMAPOBJST_deleteByG3Dmap( man, g3Dmap );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_ResistMapObject
(FIELD_BMODEL_MAN * man, FLD_G3D_MAP * g3Dmap, const PositionSt* objStatus, u32 objCount)
{
  FLD_G3D_MAP_GLOBALOBJ_ST status;
  status.id = BMIDtoEntryNo( man, getResourceID( objStatus ) );
  VEC_Set(&status.trans, objStatus->xpos, objStatus->ypos, -objStatus->zpos);
  status.rotate = (u16)(objStatus->rotate);
  G3DMAPOBJST_create(man, g3Dmap, &status, objCount);
}


//------------------------------------------------------------------
//------------------------------------------------------------------
static void BMODELMAN_releaseBuildModel(FIELD_BMODEL_MAN * man, FIELD_BMODEL * bmodel)
{
  int i;
  for (i = 0; i < BMODEL_USE_MAX; i++)
  {
    if (man->bmodels[i] == bmodel)
    {
      man->bmodels[i] = NULL;
      return;
    }
  }
  GF_ASSERT(0);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void BMODELMAN_EntryBuildModel(FIELD_BMODEL_MAN * man, FIELD_BMODEL * bmodel)
{
  int i;
  for (i = 0; i < BMODEL_USE_MAX; i++)
  {
    if (man->bmodels[i] == NULL)
    {
      man->bmodels[i] = bmodel;
      return;
    }
  }
  GF_ASSERT(0);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
FIELD_BMODEL * FIELD_BMODEL_MAN_GetBModelByUniqKey( FIELD_BMODEL_MAN * man, u16 uniq_key )
{
  u16 masked =(uniq_key & BMODEL_UNIQ_MASK );
  u16 raw_key = ( uniq_key & ~BMODEL_UNIQ_MASK );
  if ( masked != BMODEL_UNIQ_MASK ) return NULL;
  if (raw_key >= BMODEL_USE_MAX ) return NULL;
  return man->bmodels[ raw_key ];
}

//------------------------------------------------------------------
//------------------------------------------------------------------
u16 FIELD_BMODEL_MAN_GetUniqKey( FIELD_BMODEL_MAN * man, const FIELD_BMODEL * bmodel )
{
  int i;
  if ( bmodel == NULL ) return 0;
  for (i = 0; i < BMODEL_USE_MAX; i++)
  {
    if (man->bmodels[i] == bmodel)
    {
      return ( i | BMODEL_UNIQ_MASK );
    }
  }
  GF_ASSERT(0);
  return 0;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
FLD_G3D_MAP_GLOBALOBJ * FIELD_BMODEL_MAN_GetGlobalObjects(FIELD_BMODEL_MAN * man)
{
  return &man->g3dMapObj;
}




//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �G���A�ʔz�u���f��ID���X�g�̓ǂݍ���
 * @param man
 * @param arc_id  �G���A�ʔz�u���f�����X�g�̃A�[�J�C�u�w��ID�i����or���O�j
 * @param file_id  �G���AID����ϊ������t�@�C���w��ID
 *
 * entryCount/entryToBMIDTable������������
 */
//------------------------------------------------------------------
static void loadEntryToBMIDTable(FIELD_BMODEL_MAN * man, u16 arc_id, u16 file_id)
{ 
  ARCHANDLE * handle = GFL_ARC_OpenDataHandle(arc_id, man->heapID);

  {
    u16 data_max = GFL_ARC_GetDataFileCntByHandle(handle);
    if (data_max <= file_id)
    {	
      GF_ASSERT_MSG(0, "�z�u���f�����X�g�f�[�^������܂���(%d<%d)\n", data_max, file_id);
      file_id = 0;		//�Ƃ肠�����n���O�A�b�v���
    }
  }

	{	
		u16 size = GFL_ARC_GetDataSizeByHandle(handle, file_id);
		man->entryCount = size / sizeof(BMODEL_ID);
		if(size > sizeof(man->entryToBMIDTable))
		{	
			GF_ASSERT_MSG(0, "�z�u���f�����X�g�f�[�^���I�[�o�[�isize=%d ARCINDEX=%d)\n", size, file_id);
			man->entryCount = BMODEL_ENTRY_MAX;	//�Ƃ肠�����n���O�A�b�v���
			size = sizeof(man->entryToBMIDTable);
		}
		//TAMADA_Printf("entryCount=%d\n", man->entryCount);
		GFL_ARC_LoadDataOfsByHandle(handle, file_id, 0, size, man->entryToBMIDTable);
	}
	
	GFL_ARC_CloseDataHandle(handle);
}

//------------------------------------------------------------------
//BMODEL_ID�ň���BMIDToEntryTable�ɂ�entryToBMIDTable�ւ̃I�t�Z�b�g���i�[
//------------------------------------------------------------------
static void makeBMIDToEntryTable(FIELD_BMODEL_MAN * man)
{
	u8 entryNo;
	for (entryNo = 0; entryNo < man->entryCount; entryNo++)
	{	
		BMODEL_ID bm_id = man->entryToBMIDTable[entryNo];
		man->BMIDToEntryTable[bm_id] = entryNo;
	}
}

//------------------------------------------------------------------
/**
 * BMODEL_ID --> EntryNo�ւ̕ϊ�
 */
//------------------------------------------------------------------
static u8 BMIDtoEntryNo(const FIELD_BMODEL_MAN * man, BMODEL_ID bm_id)
{
	u8 entryNo;
  BMID_ASSERT( bm_id );
	entryNo = man->BMIDToEntryTable[bm_id];
  ENTRYNO_ASSERT( man, entryNo );
	return entryNo;
}

//------------------------------------------------------------------
/**
 * @brief �G���AID����z�u���f�����ւ̃C���f�b�N�X�擾
 * @param area_id 
 * @return  u16 ���A�[�J�C�u�̎w��ID
 *
 * @todo  �G���A�f�[�^�̕��тɈˑ����Čv�Z���Ă���̂ňڍs���l���邱�ƁI
 */
//------------------------------------------------------------------
static u16 calcArcIndex(u16 area_id)
{	
	if (area_id >= AREA_ID_IN01)
	{	
		return area_id - AREA_ID_IN01;
	}
	if (area_id >= AREA_ID_OUT01)
	{	
		return (area_id - AREA_ID_OUT01) / 4;
	}
	return 0;	//�Ƃ肠����
}

#ifdef BMODEL_TEXSET
//------------------------------------------------------------------
/**
 * @brief �G���A�ʔz�u���f��ID�e�N�X�`���Z�b�g�̓ǂݍ���
 * @param man
 * @param arc_id  �G���A�ʔz�u���f�����X�g�̃A�[�J�C�u�w��ID�i����or���O�j
 * @param file_id  �G���AID����ϊ������t�@�C���w��ID
 */
//------------------------------------------------------------------
static void loadBMTextureSet(FIELD_BMODEL_MAN * man, u16 arc_id, u16 file_id)
{
  man->g3DresTex = GFL_G3D_CreateResourceArc( arc_id, file_id );
  
  DEBUG_Field_Grayscale( man->g3DresTex );
  if(	!GFL_G3D_TransVramTextureAndFreeImageEntity( man->g3DresTex )){
    GF_ASSERT(0);
    return;
  }
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( man->g3DresTex );
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( GFL_G3D_GetResourceFileHeader( man->g3DresTex ) );
}

//------------------------------------------------------------------
/**
 * @brief �G���A�ʔz�u���f��ID�e�N�X�`���Z�b�g�̔j��
 * @param man
 * @param arc_id  �G���A�ʔz�u���f�����X�g�̃A�[�J�C�u�w��ID�i����or���O�j
 * @param file_id  �G���AID����ϊ������t�@�C���w��ID
 */
//------------------------------------------------------------------
static void freeBMTextureSet(FIELD_BMODEL_MAN * man)
{
  if(man->g3DresTex != NULL){
    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( GFL_G3D_GetResourceFileHeader( man->g3DresTex ) );
    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( man->g3DresTex );
    GFL_G3D_FreeVramTexture( man->g3DresTex );
    GFL_G3D_DeleteResource( man->g3DresTex );
    man->g3DresTex = NULL;
  }
}

#endif

//============================================================================================
//
//
//    �z�u���f���A�j���f�[�^
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �ێ����Ă���A�j��ID�̐���Ԃ�
 * @param data  �A�j���f�[�^�ւ̃|�C���^
 * @param u32 �ێ����Ă���A�j��ID�̐�
 */
//------------------------------------------------------------------
static u32 BMANIME_getCount(const FIELD_BMANIME_DATA * data)
{ 
  u32 anmNo,count;
  for (anmNo = 0, count = 0; anmNo < GLOBAL_OBJ_ANMCOUNT; anmNo++)
  { 
    if (data->IDs[anmNo] != BMANIME_NULL_ID)
    { 
      count ++;
    }
  }
  GF_ASSERT( count == data->anmset_num * data->ptn_count );
  return count;
}
//------------------------------------------------------------------
/**
 * @brief FIELD_BMANIME_DATA�̏�����
 */
//------------------------------------------------------------------
static void BMANIME_init(FIELD_BMANIME_DATA * data)
{ 
  static const FIELD_BMANIME_DATA init = {  
    BMANIME_TYPE_NONE,  //�A�j���K�p�̎�ގw��
    BMANIME_PROG_TYPE_NONE,  //����v���O�����̎�ގw��
    0,  //�A�j���J�E���g�i���j
    0,  //�Z�b�g�J�E���g�i���j
    //�A�j���A�[�J�C�u�w��ID
    { BMANIME_NULL_ID, BMANIME_NULL_ID, BMANIME_NULL_ID, BMANIME_NULL_ID, }

  };
  *data = init;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
#ifdef  PM_DEBUG
static void DEBUG_BMANIME_dump(const FIELD_BMANIME_DATA * data)
{ 
  static const char * animetype[] ={  
    "BMANIME_TYPE_NONE",
    "BMANIME_TYPE_ETERNAL",
    "BMANIME_TYPE_EVENT",
    "BMANIME_TYPE_ERROR",
  };
  int anmNo;
  int type = data->anm_type;
  if (type >= BMANIME_TYPE_MAX) type = BMANIME_TYPE_MAX - 1;
  TAMADA_Printf("FIELD_BMANIME_DATA:");
  TAMADA_Printf("%s, %d\n", animetype[data->anm_type], type);
  TAMADA_Printf("%d %d %d\n",data->prg_type, data->anmset_num, data->ptn_count);
  for (anmNo = 0; anmNo < GLOBAL_OBJ_ANMCOUNT; anmNo++)
  {
    TAMADA_Printf("%04x ", data->IDs[anmNo]);
  }
  TAMADA_Printf("\n");
}
#endif

//------------------------------------------------------------------
//------------------------------------------------------------------
static void BMINFO_Load(FIELD_BMODEL_MAN * man, u16 file_id)
{
  typedef struct {
    u16 anime_id;     ///<�A�j���w��
    u16 prog_id;      ///<�v���O�����w��
    u16 submodel_id;  ///<�T�u���f���w��
    s16 submodel_x;
    s16 submodel_y;
    s16 submodel_z;
  }BM_INFO_BIN;

  enum {
    BM_INFO_SIZE = sizeof(BM_INFO_BIN),
    BM_ANMDATA_SIZE = sizeof(FIELD_BMANIME_DATA),
  };

  ARCHANDLE * handle = GFL_ARC_OpenDataHandle(ARCID_BMODEL_INFO, man->heapID);
  BM_INFO_BIN * infobin = GFL_HEAP_AllocMemory(man->heapID, BM_INFO_SIZE);
  u8 entryNo;

  for (entryNo = 0; entryNo < man->entryCount; entryNo++)
  { 
    BMINFO * bmInfo = &man->bmInfo[entryNo];
    u16 anm_id;
    bmInfo->bm_id = man->entryToBMIDTable[entryNo];
    GFL_ARC_LoadDataOfsByHandle(handle, file_id, bmInfo->bm_id * BM_INFO_SIZE, BM_INFO_SIZE, infobin);
    bmInfo->prog_id = infobin->prog_id;
    bmInfo->sub_bm_id = infobin->submodel_id;
    bmInfo->sx = infobin->submodel_x;
    bmInfo->sy = infobin->submodel_y;
    bmInfo->sz = infobin->submodel_z;
    bmInfo->anm_id = infobin->anime_id;
    if (bmInfo->anm_id == 0xffff) continue;
    GFL_ARC_LoadDataOfsByHandle(handle, FILEID_BMODEL_ANIMEDATA,
        bmInfo->anm_id * BM_ANMDATA_SIZE, BM_ANMDATA_SIZE,
        &bmInfo->animeData);
    //DEBUG_BMANIME_dump(&man->animeData[entryNo]);
  }
  GFL_HEAP_FreeMemory(infobin);
  GFL_ARC_CloseDataHandle(handle);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void BMINFO_init(BMINFO * bmInfo)
{
  bmInfo->bm_id = 0;
  bmInfo->prog_id = BM_PROG_ID_NONE;
  bmInfo->sub_bm_id = BM_SUBMODEL_NULL_ID;
  bmInfo->sx = 0;
  bmInfo->sy = 0;
  bmInfo->sz = 0;
  bmInfo->anm_id = 0xffff;
  BMANIME_init(&bmInfo->animeData);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static const FIELD_BMANIME_DATA * BMINFO_getAnimeData(const BMINFO * bmInfo)
{
  return &bmInfo->animeData;
}

//------------------------------------------------------------------
/**
 * @brief ����ID�̎擾
 */
//------------------------------------------------------------------
static BM_SEARCH_ID BMINFO_getSearchID( const BMINFO * bmInfo )
{
  const u8 search_id[] = {
    BM_SEARCH_ID_NULL,        //BM_PROG_ID_NONE
    BM_SEARCH_ID_DOOR,        //BM_PROG_ID_DOOR_NORMAL,
    BM_SEARCH_ID_DOOR,        //BM_PROG_ID_DOOR_AUTO,
    BM_SEARCH_ID_DOOR,        //BM_PROG_ID_BADGEGATE,
    BM_SEARCH_ID_SANDSTREAM,  //BM_PROG_ID_SANDSTREAM,
    BM_SEARCH_ID_NULL,        //BM_PROG_ID_PCELEVATOR,
    BM_SEARCH_ID_PCMACHINE,   //BM_PROG_ID_PCMACHINE,
    BM_SEARCH_ID_PC,          //BM_PROG_ID_PC,
    BM_SEARCH_ID_PCEV_DOOR,   //BM_PROG_ID_PCEV_DOOR,
    BM_SEARCH_ID_PCEV_FLOOR,  //BM_PROG_ID_PCEV_FLOOR,
    BM_SEARCH_ID_WARP,        //BM_PROG_ID_WARP,
    BM_SEARCH_ID_EVENT,       //BM_PROG_ID_EVENT,
    BM_SEARCH_ID_PPMACHINE,   //BM_PROG_ID_PPMACHINE,
    BM_SEARCH_ID_DOOR,        //BM_PROG_ID_DOOR_ELEVATOR,
    BM_SEARCH_ID_DOOR,        //BM_PROG_ID_DOOR_BELL,
    BM_SEARCH_ID_DOOR,        //BM_PROG_ID_DOOR_AUTO2
  };
  SDK_COMPILER_ASSERT( NELEMS(search_id) == BM_PROG_ID_MAX );

  if ( bmInfo->prog_id >= BM_PROG_ID_MAX )
  {
    GF_ASSERT_MSG(0, "�z�u���f���v���O�����w��(%d)�̃G���[�ł�\n", bmInfo->prog_id );
    return BM_SEARCH_ID_NULL;
  }
  return search_id[ bmInfo->prog_id ];
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void TIMEANIME_CTRL_init(TIMEANIME_CTRL * tmanm_ctrl)
{
  tmanm_ctrl->NowTimeZone = TIMEZONE_MAX; //���肦�Ȃ���
  TIMEANIME_CTRL_update( tmanm_ctrl );
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void TIMEANIME_CTRL_update(TIMEANIME_CTRL * tmanm_ctrl)
{
  //5�i�K��TIMEZONE��4��ނ̃A�j���Ɋ���U��
  static const u8 index[TIMEZONE_MAX] = {
    0, 1, 2, 3, 3,
  };
  tmanm_ctrl->OldTimeZone = tmanm_ctrl->NowTimeZone;
  tmanm_ctrl->NowTimeZone = GFL_RTC_GetTimeZone();
  tmanm_ctrl->update_flag = (tmanm_ctrl->NowTimeZone != tmanm_ctrl->OldTimeZone);
  tmanm_ctrl->index = index[tmanm_ctrl->NowTimeZone];
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL TIMEANIME_CTRL_needUpdate(const TIMEANIME_CTRL * tmanm_ctrl)
{
  return tmanm_ctrl->update_flag;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static u8   TIMEANIME_CTRL_getIndex( const TIMEANIME_CTRL * tmanm_ctrl )
{
  return tmanm_ctrl->index;
}

//============================================================================================
//
//
//  ���\�[�X����
//
//
//============================================================================================
//------------------------------------------------------------------
//�z�u���f���}�l�W���[����̓��e�Ő���
//------------------------------------------------------------------
static void createAllResource(FIELD_BMODEL_MAN * man)
{ 
  u32 entryCount = man->entryCount;

	if( entryCount == 0 ){
    return;
  }

  man->objRes_Count = entryCount;
  man->objRes = GFL_HEAP_AllocClearMemory ( man->heapID, sizeof(OBJ_RES) * entryCount );
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( man->objRes );

  /** �z�u���f�����\�[�X�o�^ */
  {
    u8 entryNo;

    for ( entryNo = 0; entryNo < man->objRes_Count; entryNo++ )
    {
      OBJ_RES * objRes = &man->objRes[entryNo];
      BMODEL_ID bm_id = man->entryToBMIDTable[entryNo];
      //TAMADA_Printf("BM:Create Rsc %d (%d)\n", entryNo, bm_id);
      OBJRES_initialize( man, objRes, bm_id );
    }
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void deleteAllResource(FIELD_BMODEL_MAN * man)
{
  int i;
	if( man->objRes == NULL ){
    return;
  }
  for( i=0; i<man->objRes_Count; i++ ){
    OBJRES_finalize( &man->objRes[i] );
  }
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( man->objRes );
  GFL_HEAP_FreeMemory( man->objRes );
  man->objRes_Count = 0;
  man->objRes = NULL;
}


//------------------------------------------------------------------
/** �z�u���f��G3D_OBJECT�o�^ */
//------------------------------------------------------------------
static void createFullTimeObjHandle(FIELD_BMODEL_MAN * man, FLD_G3D_MAP_GLOBALOBJ * g3dMapObj)
{
  int i;
  u32 entryCount = man->entryCount;

  if( entryCount == 0 ){
    return;
  }

  g3dMapObj->gobjCount = entryCount;
  g3dMapObj->gobj = GFL_HEAP_AllocClearMemory( man->heapID, sizeof(FLD_G3D_MAP_OBJ) * entryCount );
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( g3dMapObj->gobj );

  man->objHdl_Count = entryCount;
  man->objHdl = GFL_HEAP_AllocClearMemory( man->heapID, sizeof(OBJ_HND) * entryCount );
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( man->objHdl );

  for ( i = 0; i < man->objHdl_Count; i++ )
  {
    OBJ_HND * objHdl = &man->objHdl[i];
    OBJ_RES * objRes = &man->objRes[i];

    OBJHND_initialize( man, objHdl, objRes );

    g3dMapObj->gobj[i].g3DobjHQ = objHdl->g3Dobj;
    g3dMapObj->gobj[i].g3DobjLQ = NULL;
  }
}


//------------------------------------------------------------------
//------------------------------------------------------------------
static void deleteFullTimeObjHandle(FIELD_BMODEL_MAN * man, FLD_G3D_MAP_GLOBALOBJ * g3dMapObj)
{
	if( g3dMapObj->gobj != NULL ){
    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( g3dMapObj->gobj );
		GFL_HEAP_FreeMemory( g3dMapObj->gobj );
		g3dMapObj->gobj = NULL;
	}
	if( g3dMapObj->gobjIDexchange != NULL ){
    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( g3dMapObj->gobjIDexchange );
		GFL_HEAP_FreeMemory( g3dMapObj->gobjIDexchange );
		g3dMapObj->gobjIDexchange = NULL;
	}

  if( man->objHdl != NULL ){
    int i;
    for( i=0; i<man->objHdl_Count; i++ ){
			OBJHND_finalize( &man->objHdl[i] );
    }
    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( man->objHdl );
    GFL_HEAP_FreeMemory( man->objHdl );
    man->objHdl_Count = 0;
    man->objHdl = NULL;
  }
}

//============================================================================================
//
//  GFL_G3D �p���\�[�X�Ǘ��I�u�W�F�N�g
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
#ifndef BMODEL_TEXSET
static GFL_G3D_RES* OBJRES_getResTex(const OBJ_RES * objRes)
{
  if ( objRes->g3DresTex != NULL ) {
    return objRes->g3DresTex;
  } else {
    return objRes->g3DresMdl;
  }
}
#endif

//------------------------------------------------------------------
// �I�u�W�F�N�g���\�[�X���쐬
//------------------------------------------------------------------
static void OBJRES_initialize( FIELD_BMODEL_MAN * man, OBJ_RES * objRes, BMODEL_ID bm_id)
{
	GFL_G3D_RES* resTex;

  //�z�u���f���ɑΉ��������ւ̃|�C���^���Z�b�g
  objRes->bmInfo = BMODELMAN_GetBMInfo(man, bm_id);

  //���f���f�[�^����
	objRes->g3DresMdl = GFL_G3D_CreateResourceArc( man->mdl_arc_id, bm_id );
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( objRes->g3DresMdl );
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( GFL_G3D_GetResourceFileHeader( objRes->g3DresMdl ) );

#ifndef BMODEL_TEXSET
  //�e�N�X�`���o�^
  //����A�e�N�X�`���̓��f�����Oimd�Ɋ܂܂����̂��g�p���Ă���
  objRes->g3DresTex = NULL;
  resTex = objRes->g3DresMdl;
  if (GFL_G3D_CheckResourceType( resTex, GFL_G3D_RES_CHKTYPE_TEX ) == TRUE)
  {
	  DEBUG_Field_Grayscale( resTex );
	  GFL_G3D_TransVramTexture( resTex );
  } else {
    OS_Printf("�z�u���f���Ƀe�N�X�`�����܂܂�Ă��܂���I(mdlID=%d)\n", bm_id);
    GF_ASSERT(0);
  }
#endif

  {
    const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objRes->bmInfo);
    int anmNo;
    int count = BMANIME_getCount(anmData);
    for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
      if ( anmNo < count ) {
        objRes->g3DresAnm[anmNo] = GFL_G3D_CreateResourceArc( ARCID_BMODEL_ANIME, anmData->IDs[anmNo] );

        BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( objRes->g3DresAnm[anmNo] );
        BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( GFL_G3D_GetResourceFileHeader( objRes->g3DresAnm[anmNo] ) );
      } else {
        objRes->g3DresAnm[anmNo] = NULL;
      }
    }
  }

}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void OBJRES_finalize( OBJ_RES * objRes )
{
  int anmNo;
	GFL_G3D_RES*	resTex;

  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
    if( objRes->g3DresAnm[anmNo] != NULL ){
      BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( GFL_G3D_GetResourceFileHeader( objRes->g3DresAnm[anmNo] ) );
      BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( objRes->g3DresAnm[anmNo] );
      GFL_G3D_DeleteResource( objRes->g3DresAnm[anmNo] );
      objRes->g3DresAnm[anmNo] = NULL;
    }
  }
#ifndef BMODEL_TEXSET
  if( objRes->g3DresTex == NULL ){
    resTex = objRes->g3DresMdl;
  } else {
    resTex = objRes->g3DresTex;
    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( GFL_G3D_GetResourceFileHeader( objRes->g3DresTex ) );
    GFL_G3D_DeleteResource( objRes->g3DresTex );
    objRes->g3DresTex = NULL;
  }
  GFL_G3D_FreeVramTexture( resTex );
#endif

  if( objRes->g3DresMdl != NULL ){
    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( GFL_G3D_GetResourceFileHeader( objRes->g3DresMdl ) );
    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( objRes->g3DresMdl );
    GFL_G3D_DeleteResource( objRes->g3DresMdl );
    objRes->g3DresMdl = NULL;
  }
}


//============================================================================================
//
//  GFL_G3D_OBJECT�Ǘ��p�I�u�W�F�N�g
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief BMANIME_TYPE�ʂ̏�������p�e�[�u����`
 */
//------------------------------------------------------------------
typedef struct {
  ///�A�j��������
  void (* init_anime_func)( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
  ///�A�j���X�V
  void (* do_anime_func)(const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
  ///�A�j���ύX
  void (* set_anime_func)( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req);
}OBJHND_FUNCS;

static void OBJHND_TYPENONE_initAnime( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_TYPEETERNAL_initAnime( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_TYPETIMEZONE_initAnime( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );

static void OBJHND_TYPENONE_setAnime( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req );
static void OBJHND_TYPEEVENT_setAnime( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req );

static void OBJHND_TYPETIMEZONE_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_TYPEEVENT_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_TYPEETERNAL_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_TYPENONE_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );

static void disableAllAnime(OBJ_HND * objHdl);
//------------------------------------------------------------------
///   �e�A�j���^�C�v���Ƃ̏����֐��e�[�u��
//------------------------------------------------------------------
static const OBJHND_FUNCS objHndFuncTable[BMANIME_TYPE_MAX] = {
  { //BMANIME_TYPE_NONE �A�j�����Ȃ�
    OBJHND_TYPENONE_initAnime,
    OBJHND_TYPENONE_animate,
    OBJHND_TYPENONE_setAnime,
  },
  { //BMANIME_TYPE_ETERNAL  �����Ƃ��Ȃ��A�j��
    OBJHND_TYPEETERNAL_initAnime,
    OBJHND_TYPEETERNAL_animate,
    OBJHND_TYPENONE_setAnime,
  },
  { //BMANIME_TYPE_EVENT  �C�x���g����A�j�����N�G�X�g�i�ʏ��~�j
    OBJHND_TYPENONE_initAnime,
    OBJHND_TYPEEVENT_animate,
    OBJHND_TYPEEVENT_setAnime,
  },
  { //BMANIME_TYPE_TIMEZONE ���ԑтɂ��A�j���ύX
    OBJHND_TYPETIMEZONE_initAnime,
    OBJHND_TYPETIMEZONE_animate,
    OBJHND_TYPENONE_setAnime,
  },
};
//------------------------------------------------------------------
/// OBJHND����������
//------------------------------------------------------------------
static void OBJHND_initialize(const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl, const OBJ_RES* objRes)
{
  GFL_G3D_RND* g3Drnd;
  GFL_G3D_RES* resTex;
  GFL_G3D_ANM* anmTbl[GLOBAL_OBJ_ANMCOUNT];
  int anmNo;
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objRes->bmInfo);

#ifdef BMODEL_TEXSET
  //�����_�[����
  g3Drnd = GFL_G3D_RENDER_Create( objRes->g3DresMdl, 0, man->g3DresTex );
#else
  resTex = OBJRES_getResTex( objRes );
  //�����_�[����
  g3Drnd = GFL_G3D_RENDER_Create( objRes->g3DresMdl, 0, resTex );
#endif
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( g3Drnd );
  BMODEL_DEBUG_RESOURCE_MEMORY_ALLOCATOR_SIZE_Plus( GFL_G3D_RENDER_GetRenderObj( g3Drnd ) );

  //�A�j���I�u�W�F�N�g����
  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
    if( objRes->g3DresAnm[anmNo] != NULL ){
      anmTbl[anmNo] = GFL_G3D_ANIME_Create( g3Drnd, objRes->g3DresAnm[anmNo], 0 );  
      BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( anmTbl[anmNo] );
      BMODEL_DEBUG_RESOURCE_MEMORY_ALLOCATOR_SIZE_Plus( GFL_G3D_ANIME_GetAnmObj( anmTbl[anmNo] ) );
      //���f���ƃA�j��������Ă���ƃ������j�󂪔�������̂ŁA�����Ō��o
      GF_ASSERT_MSG( GFL_HEAP_CheckHeapSafe( man->heapID ) == TRUE,
          "BuildModel Create Error! id=%d\n", objRes->bmInfo->bm_id );
    } else {
      anmTbl[anmNo] = NULL;
    }
  }
  //GFL_G3D_OBJECT����
  objHdl->g3Dobj = GFL_G3D_OBJECT_Create( g3Drnd, anmTbl, GLOBAL_OBJ_ANMCOUNT );
  objHdl->res = objRes;
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( objHdl->g3Dobj );

  //�A�j����ԏ�����
  {
    BMANIME_TYPE type = anmData->anm_type;
    objHndFuncTable[type].init_anime_func( man, objHdl );
  }
}

//------------------------------------------------------------------
///OBJHND�������F�A�j���^�C�v���̑�
//------------------------------------------------------------------
static void OBJHND_TYPENONE_initAnime( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
{
  int anmNo;
  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
    objHdl->anmMode[anmNo] = BM_ANMMODE_NOTHING;
  }
}
//------------------------------------------------------------------
///OBJHND�������F�A�j���^�C�vETERNAL
//------------------------------------------------------------------
static void OBJHND_TYPEETERNAL_initAnime( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
{
  int anmNo;
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objHdl->res->bmInfo);
  u32 set_num = anmData->anmset_num;
  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT && anmNo<set_num; anmNo++ ){
    GFL_G3D_OBJECT_EnableAnime( objHdl->g3Dobj, anmNo );  //render��anime�̊֘A�t��
    GFL_G3D_OBJECT_ResetAnimeFrame( objHdl->g3Dobj, anmNo ); 
    objHdl->anmMode[anmNo] = BM_ANMMODE_LOOP;
  }
  for( ;anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo ++)
  {
    objHdl->anmMode[anmNo] = BM_ANMMODE_NOTHING;
  }
}
//------------------------------------------------------------------
///OBJHND�������F�A�j���^�C�vTIMEZONE
//------------------------------------------------------------------
static void OBJHND_TYPETIMEZONE_initAnime( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
{
  int anmNo, nowNo;
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objHdl->res->bmInfo);
  GF_ASSERT( anmData->anmset_num == 1 );  //���ԑуA�j���̓Z�b�g�����傫���͖̂����I
  nowNo = TIMEANIME_CTRL_getIndex( &man->tmanm_ctrl );
  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
    if (anmNo != nowNo)
    {
      objHdl->anmMode[anmNo] = BM_ANMMODE_NOTHING;
    }
    else
    {
      GFL_G3D_OBJECT_EnableAnime( objHdl->g3Dobj, anmNo );  //render��anime�̊֘A�t��
      GFL_G3D_OBJECT_ResetAnimeFrame( objHdl->g3Dobj, anmNo ); 
      objHdl->anmMode[anmNo] = BM_ANMMODE_LOOP;
    }
  }
}

//------------------------------------------------------------------
/// OBJHND�I������
//------------------------------------------------------------------
static void OBJHND_finalize( OBJ_HND * objHdl )
{
  GFL_G3D_RND*  g3Drnd;

  if( objHdl->g3Dobj != NULL ){
    int anmNo;
    //�e��n���h���擾
    u16 g3DanmCount = GFL_G3D_OBJECT_GetAnimeCount( objHdl->g3Dobj );
    for( anmNo=0; anmNo<g3DanmCount; anmNo++ ){
      GFL_G3D_ANM*  g3Danm = GFL_G3D_OBJECT_GetG3Danm( objHdl->g3Dobj, anmNo ); 
      if (g3Danm != NULL) {
        BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( g3Danm );
        BMODEL_DEBUG_RESOURCE_MEMORY_ALLOCATOR_SIZE_Minus( GFL_G3D_ANIME_GetAnmObj( g3Danm ) );
        GFL_G3D_ANIME_Delete( g3Danm ); 
      }
    }
    g3Drnd = GFL_G3D_OBJECT_GetG3Drnd( objHdl->g3Dobj );
  
    //�e��n���h�������\�[�X�폜
    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( objHdl->g3Dobj );
    GFL_G3D_OBJECT_Delete( objHdl->g3Dobj );
    objHdl->g3Dobj = NULL;
    objHdl->res = NULL;

    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( g3Drnd );
    BMODEL_DEBUG_RESOURCE_MEMORY_ALLOCATOR_SIZE_Minus( GFL_G3D_RENDER_GetRenderObj( g3Drnd ) );
    GFL_G3D_RENDER_Delete( g3Drnd );
  }
}

//------------------------------------------------------------------
/**
 * @brief OBJHND�A�j���X�V����
 */
//------------------------------------------------------------------
static void OBJHND_animate(const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
{
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objHdl->res->bmInfo);
  BMANIME_TYPE type = anmData->anm_type;

  if( objHdl->g3Dobj == NULL ){
    return;
  }

  objHndFuncTable[type].do_anime_func( man, objHdl );
}

//------------------------------------------------------------------
/// OBJHND�A�j���X�V�����F���̑�
//------------------------------------------------------------------
static void OBJHND_TYPENONE_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
{
  /* DO NOTHING !! */
}

//------------------------------------------------------------------
/// OBJHND�A�j���X�V�����FTIMEZONE
//------------------------------------------------------------------
static void OBJHND_TYPETIMEZONE_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
{
  /* DO NOTHING !! */
  int anmNo = TIMEANIME_CTRL_getIndex( &man->tmanm_ctrl );
  BOOL change_flag = TIMEANIME_CTRL_needUpdate( &man->tmanm_ctrl );
  if (change_flag)
  {
    disableAllAnime( objHdl );
    GFL_G3D_OBJECT_EnableAnime( objHdl->g3Dobj, anmNo );  //render��anime�̊֘A�t��
    GFL_G3D_OBJECT_ResetAnimeFrame( objHdl->g3Dobj, anmNo ); 
    objHdl->anmMode[anmNo] = BM_ANMMODE_LOOP;
  }
  else
  {
    GFL_G3D_OBJECT_LoopAnimeFrame( objHdl->g3Dobj, anmNo, FX32_ONE ); 
  }
}

//------------------------------------------------------------------
/// OBJHND�A�j���X�V�����FETERNAL
//------------------------------------------------------------------
static void OBJHND_TYPEETERNAL_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
{
  int anmNo;
  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
    GFL_G3D_OBJECT_LoopAnimeFrame( objHdl->g3Dobj, anmNo, FX32_ONE ); 
  }
}
//------------------------------------------------------------------
/// OBJHND�A�j���X�V�����FEVENT
//------------------------------------------------------------------
static void OBJHND_TYPEEVENT_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
{
  int anmNo;

  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
    BOOL result;
    switch ((BM_ANMMODE)objHdl->anmMode[anmNo])
    {
    case BM_ANMMODE_NOTHING:
    case BM_ANMMODE_STOP:
      break;
    case BM_ANMMODE_TEMPORARY:
      result = GFL_G3D_OBJECT_IncAnimeFrame( objHdl->g3Dobj, anmNo, FX32_ONE );
      if (!result) {
        objHdl->anmMode[anmNo] = BM_ANMMODE_STOP;
      }
      break;
    case BM_ANMMODE_REVERSE:
      result = GFL_G3D_OBJECT_IncAnimeFrame( objHdl->g3Dobj, anmNo, -FX32_ONE );
      if (!result) {
        GFL_G3D_OBJECT_SetAnimeFrame( objHdl->g3Dobj, anmNo, 0 );
        objHdl->anmMode[anmNo] = BM_ANMMODE_STOP;
      }
      break;
    case BM_ANMMODE_LOOP:
      GFL_G3D_OBJECT_LoopAnimeFrame( objHdl->g3Dobj, anmNo, FX32_ONE ); 
      break;
    }
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void applyNormalAnime( OBJ_HND * objHdl, u32 anmNo )
{
  int i;
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objHdl->res->bmInfo);
  u32 setNo = anmData->anmset_num * anmNo;
  for (i = 0; i < anmData->anmset_num; i++) {
    GFL_G3D_OBJECT_EnableAnime(objHdl->g3Dobj, setNo + i );
    GFL_G3D_OBJECT_ResetAnimeFrame(objHdl->g3Dobj, setNo + i );
    objHdl->anmMode[setNo + i] = BM_ANMMODE_TEMPORARY;
  }
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void applyLoopAnime( OBJ_HND * objHdl, u32 anmNo )
{
  int i;
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objHdl->res->bmInfo);
  u32 setNo = anmData->anmset_num * anmNo;
  for (i = 0; i < anmData->anmset_num; i++) {
    GFL_G3D_OBJECT_EnableAnime(objHdl->g3Dobj, setNo + i );
    GFL_G3D_OBJECT_ResetAnimeFrame(objHdl->g3Dobj, setNo + i );
    objHdl->anmMode[setNo + i] = BM_ANMMODE_LOOP;
  }
}


//------------------------------------------------------------------
//------------------------------------------------------------------
static void applyReverseAnime( OBJ_HND * objHdl, u32 anmNo )
{
  int i;
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objHdl->res->bmInfo);
  u32 setNo = anmData->anmset_num * anmNo;
  for ( i = 0; i < anmData->anmset_num; i++ ) {
    GFL_G3D_OBJECT_EnableAnime(objHdl->g3Dobj, setNo + i );
    {
      GFL_G3D_ANM * g3Danm = GFL_G3D_OBJECT_GetG3Danm( objHdl->g3Dobj, setNo + i );
      NNSG3dAnmObj * anmObj = GFL_G3D_ANIME_GetAnmObj( g3Danm );
      fx32 num = NNS_G3dAnmObjGetNumFrame( anmObj );
      GFL_G3D_OBJECT_SetAnimeFrame(objHdl->g3Dobj, setNo + i, (const int*)&num );
    }
    objHdl->anmMode[setNo + i] = BM_ANMMODE_REVERSE;
  }
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void applyStopAnime( OBJ_HND * objHdl, u32 anmNo )
{
  int i;
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objHdl->res->bmInfo);
  u32 setNo = anmData->anmset_num * anmNo;
  for ( i = 0; i < anmData->anmset_num; i++ ) {
    if (objHdl->anmMode[setNo + i] != BM_ANMMODE_NOTHING) {
      objHdl->anmMode[setNo + i] = BM_ANMMODE_STOP;
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief �A�j���S��~
 */
//------------------------------------------------------------------
static void disableAllAnime(OBJ_HND * objHdl)
{
  int anmNo;
  for (anmNo = 0; anmNo < GLOBAL_OBJ_ANMCOUNT; anmNo++)
  {
    if (objHdl->anmMode[anmNo] != BM_ANMMODE_NOTHING) {
      GFL_G3D_OBJECT_DisableAnime( objHdl->g3Dobj, anmNo );
      objHdl->anmMode[anmNo] = BM_ANMMODE_NOTHING;
    }
  }
}
//------------------------------------------------------------------
/**
 * @brief �A�j���ύX����
 */
//------------------------------------------------------------------
static void OBJHND_setAnime( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req)
{
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objHdl->res->bmInfo);
  BMANIME_TYPE type = anmData->anm_type;
  ANIMENO_ASSERT(anmNo);

  objHndFuncTable[type].set_anime_func( objHdl, anmNo, req );
}

//------------------------------------------------------------------
/// OBJHND�A�j���ύX�����FEVENT
//------------------------------------------------------------------
static void OBJHND_TYPEEVENT_setAnime( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req )
{
  GF_ASSERT_MSG(req < BMANM_REQ_MAX, "BMANM_REQ(%d)�͖��Ή��ł�\n", req ); //����`���N�G�X�g

  switch ((BMANM_REQUEST)req) {
  case BMANM_REQ_START:
    disableAllAnime( objHdl );
    applyNormalAnime( objHdl, anmNo );
    break;
  case BMANM_REQ_REVERSE_START:
    disableAllAnime( objHdl );
    applyReverseAnime( objHdl, anmNo );
    break;
  case BMANM_REQ_LOOP:
    disableAllAnime( objHdl );
    applyLoopAnime( objHdl, anmNo );
    break;
  case BMANM_REQ_STOP:
    applyStopAnime( objHdl, anmNo );
    break;
  case BMANM_REQ_END:
    disableAllAnime( objHdl );
    break;
    break;
  }
}
//------------------------------------------------------------------
/// OBJHND�A�j���ύX�����F���̑�
//------------------------------------------------------------------
static void OBJHND_TYPENONE_setAnime( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req )
{
  /* DO NOTHING !! */
}

//------------------------------------------------------------------
/**
 * @brief �A�j����Ԏ擾
 * @return  BOOL  TRUE=��~��ԁ@FALSE=���쒆
 */
//------------------------------------------------------------------
static BOOL OBJHND_getAnimeStatus( const OBJ_HND * objHdl, u32 anmNo)
{
  ANIMENO_ASSERT(anmNo);

  switch ((BM_ANMMODE)objHdl->anmMode[anmNo]) {
  case BM_ANMMODE_NOTHING:
    return TRUE;
  case BM_ANMMODE_LOOP:
    return FALSE;
  case BM_ANMMODE_STOP:
    return TRUE;
  case BM_ANMMODE_TEMPORARY:
  case BM_ANMMODE_REVERSE:
    return FALSE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V�����t���[�����擾����
 *
 *	@param	objHdl  �n���h��
 *  
 *	@return �t���[����
 */
//-----------------------------------------------------------------------------
static fx32 OBJHND_getAnimeFrame( const OBJ_HND * objHdl, u32 anmNo)
{
  int frame;
  
  GF_ASSERT( objHdl );
  GFL_G3D_OBJECT_GetAnimeFrame( objHdl->g3Dobj, anmNo, &frame );

  return frame;
}



//============================================================================================
//
//  FLD_G3D_MAP�̕ێ�����FLD_G3D_MAP_GLOBALOBJ_ST���Q�ƁA�R���g���[�����邽�߂�
//  ���b�p�[�I�u�W�F�N�g
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static inline BOOL G3DMAPOBJST_exists(const G3DMAPOBJST * obj)
{
  return (obj->g3Dmap != NULL);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void G3DMAPOBJST_init(FIELD_BMODEL_MAN * man, G3DMAPOBJST * obj)
{
  obj->g3Dmap = NULL;
  obj->entryNoBackup = FLD_G3D_MAP_OBJID_NULL;
  obj->index = 0;
  obj->viewFlag = FALSE;
  obj->objSt = NULL;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static G3DMAPOBJST * G3DMAPOBJST_create(
    FIELD_BMODEL_MAN * man, FLD_G3D_MAP * g3Dmap, FLD_G3D_MAP_GLOBALOBJ_ST * status, int idx)
{
  int i;
  for (i = 0; i < NELEMS(man->g3DmapObjSt); i++)
  {
    G3DMAPOBJST * obj = &man->g3DmapObjSt[i];
    if ( G3DMAPOBJST_exists(obj) == FALSE )
    {
      // ���d�o�^�`�F�b�N
      GF_ASSERT( FLD_G3D_MAP_GetGlobalObj(g3Dmap, idx) == NULL );
      
      FLD_G3D_MAP_ResistGlobalObj( g3Dmap, status, idx );
      obj->g3Dmap = g3Dmap;
      obj->entryNoBackup = FLD_G3D_MAP_OBJID_NULL;
      obj->index = idx;
      obj->viewFlag = TRUE;
      obj->objSt = FLD_G3D_MAP_GetGlobalObj( g3Dmap, idx );

      return obj;
    }
  }
  GF_ASSERT(0);
  return NULL;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void G3DMAPOBJST_deleteByG3Dmap(FIELD_BMODEL_MAN * man, FLD_G3D_MAP * g3Dmap)
{
  int i, max = NELEMS(man->g3DmapObjSt);
  for (i = 0; i < max; i++)
  {
    G3DMAPOBJST * obj = &man->g3DmapObjSt[i];
    if (obj->g3Dmap == g3Dmap)
    {
      G3DMAPOBJST_deleteByObject(man, obj);
    }
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void G3DMAPOBJST_deleteByObject(FIELD_BMODEL_MAN * man, G3DMAPOBJST * obj)
{
  GF_ASSERT( obj->objSt == FLD_G3D_MAP_GetGlobalObj(obj->g3Dmap, obj->index) );
  FLD_G3D_MAP_ReleaseGlobalObj( obj->g3Dmap, obj->index );
  G3DMAPOBJST_init( man, obj );
}

//------------------------------------------------------------------
///  �\������Ă��邩�ǂ����̃`�F�b�N
//------------------------------------------------------------------
static BOOL G3DMAPOBJST_isVanished( const G3DMAPOBJST * obj )
{
  return (obj->objSt->id == FLD_G3D_MAP_OBJID_NULL );
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static u8 G3DMAPOBJST_getEntryNo( const G3DMAPOBJST * obj )
{
  if ( obj->objSt->id == FLD_G3D_MAP_OBJID_NULL )
  {
    return obj->entryNoBackup;
  }
  return obj->objSt->id;
}

//------------------------------------------------------------------
/// �z�u���f�����f�[�^�F�\������
//------------------------------------------------------------------
void G3DMAPOBJST_changeViewFlag(G3DMAPOBJST * obj, BOOL flag)
{
  GF_ASSERT(obj->viewFlag == !flag);
  if (flag)
  {
    obj->objSt->id = obj->entryNoBackup;
    obj->entryNoBackup = FLD_G3D_MAP_OBJID_NULL;
    obj->viewFlag = TRUE;
  }
  else
  {
    obj->entryNoBackup = obj->objSt->id;
    obj->objSt->id = FLD_G3D_MAP_OBJID_NULL;
    obj->viewFlag = FALSE;
  }
}


//============================================================================================
//
//
//  �O�����J�֐��F
//    G3DMAPOBJST�o�R�ł̑���֐��Q
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �z�u���f�����f�[�^�F��������
 * @param man   �z�u���f���}�l�W���[�ւ̃|�C���^
 * @param rect  �����͈�
 * @param search  ��������z�u���f����ID
 * @param num     ���������z�u���f����
 * @return  G3DMAPOBJST�̔z��
 */
//------------------------------------------------------------------
G3DMAPOBJST ** FIELD_BMODEL_MAN_CreateObjStatusList
( FIELD_BMODEL_MAN* man, const FLDHIT_RECT * rect, BM_SEARCH_ID search, u32 * num )
{
  G3DMAPOBJST ** result;
  int objNo, count = 0;
  result = GFL_HEAP_AllocClearMemory(man->heapID, sizeof(G3DMAPOBJST*) * FLD_G3D_MAP_OBJST_MAX);

  GF_ASSERT( search < BM_SEARCH_ID_MAX );
  for (objNo = 0; objNo < NELEMS(man->g3DmapObjSt); objNo ++)
  {
    VecFx32 pos;
    G3DMAPOBJST * obj = &man->g3DmapObjSt[objNo];
    if ( G3DMAPOBJST_exists(obj) == FALSE ) continue;
    if (search != BM_SEARCH_ID_NULL &&
        search != G3DMAPOBJST_getSearchID(man, obj) )
    { //�����Ώۂ��u���ł��v�łȂ��A�Ώ�OBJ�ƈ�v���Ȃ��ꍇ�͎�������
      continue;
    }

    FLD_G3D_MAP_GetTrans( obj->g3Dmap, &pos );
    VEC_Add( &obj->objSt->trans, &pos, &pos );
    if ( FLDHIT_RECT_IsIncludePos( rect, pos.x, pos.z ) == TRUE )
    {
      result[count] = obj;
      count ++;
      TAMADA_Printf("BLOCK:%x IDX:%02d POS(%d, %d)\n",
          obj->g3Dmap, obj->index, FX_Whole(pos.x), FX_Whole(pos.z) );
      TAMADA_Printf("Search Hit.\n");
    }
  }
  *num = count;
  return result;
}

//------------------------------------------------------------------
/**
 * @brief �w��ʒu�t�߂̔z�u���f������������
 * @param bmodel_man  �z�u���f���}�l�W���[�ւ̃|�C���^
 * @param id          ����ID�iBM_SEARCH_ID�Q�Ɓj
 * @param rect         �����ʒu
 * @retval NULL   ������Ȃ�����
 * @retval G3DMAPOBJST *  �������h�A�z�u���f���ւ̎Q��
 *
 * @note
 * ������FIELD_BMODEL_MAN_CreateObjStatusList���g�p���A�������擪�̗v�f��Ԃ��Ă���
 */
//------------------------------------------------------------------
G3DMAPOBJST * FIELD_BMODEL_MAN_SearchObjStatusRect(
    FIELD_BMODEL_MAN * bmodel_man, BM_SEARCH_ID id, const FLDHIT_RECT * rect)
{
  G3DMAPOBJST * entry = NULL;
  G3DMAPOBJST ** array;
  u32 result_num;

  GF_ASSERT( id < BM_SEARCH_ID_MAX );
  {
    //��`�͈͓��̔z�u���f�����X�g�𐶐�����
    array = FIELD_BMODEL_MAN_CreateObjStatusList(bmodel_man, rect, id, &result_num);
  }
  entry = array[0];
  //��`�͈͓��̔z�u���f�����X�g���������
  GFL_HEAP_FreeMemory(array);

  return entry;
}
//------------------------------------------------------------------
/**
 * @brief �w��ʒu�t�߂̔z�u���f������������
 * @param bmodel_man  �z�u���f���}�l�W���[�ւ̃|�C���^
 * @param id          ����ID�iBM_SEARCH_ID�Q�Ɓj
 * @param pos         �����ʒu
 * @retval NULL   ������Ȃ�����
 * @retval G3DMAPOBJST *  �������h�A�z�u���f���ւ̎Q��
 *
 * @note
 * ������FIELD_BMODEL_MAN_CreateObjStatusList���g�p���A�������擪�̗v�f��Ԃ��Ă���
 */
//------------------------------------------------------------------
G3DMAPOBJST * FIELD_BMODEL_MAN_SearchObjStatusPos(
    FIELD_BMODEL_MAN * bmodel_man, BM_SEARCH_ID id, const VecFx32 * pos)
{
  //������`���쐬
  FLDHIT_RECT rect;
  makeRect(&rect, pos);
  return FIELD_BMODEL_MAN_SearchObjStatusRect( bmodel_man, id, &rect );
}


//------------------------------------------------------------------
/// �z�u���f�����f�[�^�F����ID�̎擾
//------------------------------------------------------------------
BM_SEARCH_ID G3DMAPOBJST_getSearchID( const FIELD_BMODEL_MAN * man, const G3DMAPOBJST * obj )
{
  u8 entryNo;
  GF_ASSERT( G3DMAPOBJST_exists(obj) );
  entryNo = G3DMAPOBJST_getEntryNo( obj );
  //entryNo = obj->objSt->id;
  ENTRYNO_ASSERT( man, entryNo );
  return BMINFO_getSearchID(&man->bmInfo[entryNo]);
}

//------------------------------------------------------------------
/// �z�u���f�����f�[�^�F�A�j�����f
//------------------------------------------------------------------
void G3DMAPOBJST_setAnime( FIELD_BMODEL_MAN * man, G3DMAPOBJST * obj, u32 anm_idx, BMANM_REQUEST req )
{
  u8 entryNo;
  entryNo = G3DMAPOBJST_getEntryNo( obj );
  //entryNo = obj->objSt->id;
  ENTRYNO_ASSERT( man, entryNo );
  OBJHND_setAnime( &man->objHdl[entryNo], anm_idx, req );
}

//------------------------------------------------------------------
/// �z�u���f�����f�[�^�F�ʒu�擾
//------------------------------------------------------------------
void G3DMAPOBJST_getPos(G3DMAPOBJST * obj, VecFx32 * dst)
{
  VecFx32 pos;
  FLD_G3D_MAP_GetTrans( obj->g3Dmap, &pos );
  VEC_Add( &obj->objSt->trans, &pos, dst );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �z�u���f�����f�[�^�F�A�j���[�V�����t���[���擾
 *
 *	@param	obj       �I�u�W�F
 *	@param  anm_idx   �A�j���[�V�����C���f�b�N�X
 *
 *	@return �A�j���[�V�����t���[��
 */
//-----------------------------------------------------------------------------
fx32 G3DMAPOBJST_getAnimeFrame(FIELD_BMODEL_MAN* man, const G3DMAPOBJST * obj, u32 anm_idx )
{
  u8 entryNo;
  entryNo = G3DMAPOBJST_getEntryNo( obj );
  //entryNo = obj->objSt->id;
  ENTRYNO_ASSERT( man, entryNo );
  return OBJHND_getAnimeFrame( &man->objHdl[entryNo], anm_idx );
}



//============================================================================================
//
//
//  �O�����J�֐��F
//  �z�u���f���̐���
//    �}�b�v��񂩂�łȂ��A�ӎ��I�ɃC�x���g�ȂǂŐ�������ꍇ�̊֐��Q
//
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
FIELD_BMODEL * FIELD_BMODEL_Create(FIELD_BMODEL_MAN * man, G3DMAPOBJST * obj)
{
  const OBJ_RES * objRes;
  const FLD_G3D_MAP_GLOBALOBJ_ST * status = obj->objSt;
  VecFx32 drawOffset;

  FIELD_BMODEL * bmodel = GFL_HEAP_AllocMemory( man->heapID, sizeof(FIELD_BMODEL) );
  bmodel->man = man;
  bmodel->objst = obj;
  bmodel->currentAnmIdx = BMANM_INDEX_NULL;

  //���[�e�[�V�����ݒ�
  {
    fx32 sin = FX_SinIdx(status->rotate);
    fx32 cos = FX_CosIdx(status->rotate);
    MTX_RotY33( &bmodel->g3dObjStatus.rotate, sin, cos );
  }
  //�ʒu�ݒ�Fg3Dmap�Ɉˑ������\�����邽�߁A�����ō��W���Z���Ă���
  FLD_G3D_MAP_GetTrans( obj->g3Dmap, &bmodel->g3dObjStatus.trans );
  VEC_Add( &bmodel->g3dObjStatus.trans, &status->trans, &bmodel->g3dObjStatus.trans );
  FLDMAPPER_GetDrawOffset( man->fldmapper, &drawOffset );
  VEC_Add( &bmodel->g3dObjStatus.trans, &drawOffset, &bmodel->g3dObjStatus.trans );
  
  //�X�P�[���ݒ�
  VEC_Set( &bmodel->g3dObjStatus.scale, FX32_ONE, FX32_ONE, FX32_ONE );

  GF_ASSERT( status->id < man->objRes_Count );
  objRes = &man->objRes[status->id];
  OBJHND_initialize( man, &bmodel->objHdl, objRes );

  BMODELMAN_EntryBuildModel( man, bmodel );
  G3DMAPOBJST_changeViewFlag( bmodel->objst, FALSE ); //�R�s�[�����\���ɂ���
  return bmodel;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
FIELD_BMODEL * FIELD_BMODEL_Create_Search(
    FIELD_BMODEL_MAN * bmodel_man, BM_SEARCH_ID id, const VecFx32 * pos )
{
  G3DMAPOBJST * obj;
  FIELD_BMODEL * entry;

  obj = FIELD_BMODEL_MAN_SearchObjStatusPos( bmodel_man, id, pos );
  if (obj == NULL) return NULL;
  entry = FIELD_BMODEL_Create( bmodel_man, obj );

  return entry;
}

//------------------------------------------------------------------
/**
 * @brief �o�^�ς݂̃��\�[�X�𗘗p���Ĕz�u���f�����쐬����
 * @param man       �z�u���f���}�l�[�W��
 * @param bmodel_id �쐬����z�u���f����ID
 * @param status    �\���X�e�[�^�X
 * @return �쐬�����z�u���f��
 */
//------------------------------------------------------------------
FIELD_BMODEL * FIELD_BMODEL_Create_Direct(
    FIELD_BMODEL_MAN * man, BMODEL_ID bmodel_id, const GFL_G3D_OBJSTATUS* status )
{
  FIELD_BMODEL* bmodel = NULL;
  const OBJ_RES * objRes;
  u8 entryNo;

  // �C���X�^���X����
  bmodel = GFL_HEAP_AllocMemory( man->heapID, sizeof(FIELD_BMODEL) );
  bmodel->man = man;
  bmodel->objst = NULL;
  bmodel->currentAnmIdx = BMANM_INDEX_NULL;

  // �\���X�e�[�^�X�ݒ�
  bmodel->g3dObjStatus = *status;
  // ���f������I�u�W�F�N�g���쐬
  entryNo = man->BMIDToEntryTable[bmodel_id];
  objRes = &(man->objRes[entryNo]);
  OBJHND_initialize( man, &bmodel->objHdl, objRes );

  BMODELMAN_EntryBuildModel( man, bmodel );
  return bmodel;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_BMODEL_Delete(FIELD_BMODEL * bmodel)
{
  if ( bmodel == NULL ) return;
  BMODELMAN_releaseBuildModel( bmodel->man, bmodel );

  OBJHND_finalize( &bmodel->objHdl );
  if ( bmodel->objst ) {
    G3DMAPOBJST_changeViewFlag( bmodel->objst, TRUE ); //�R�s�[�����ĕ\������
  }

  GFL_HEAP_FreeMemory(bmodel);
}

//------------------------------------------------------------------
/// �z�u���f���F�A�j���ύX
//------------------------------------------------------------------
void FIELD_BMODEL_SetAnime(FIELD_BMODEL * bmodel, u32 idx, BMANM_REQUEST req)
{
  if ( bmodel == NULL ) return;
  bmodel->currentAnmIdx = idx;
  OBJHND_setAnime(&bmodel->objHdl, idx, req);
}

//------------------------------------------------------------------
/// �z�u���f���F�A�j���ύX�i���O�̃C���f�b�N�X�Ώہj
//------------------------------------------------------------------
void FIELD_BMODEL_SetCurrentAnime( FIELD_BMODEL * bmodel, BMANM_REQUEST req )
{
  if ( bmodel == NULL ) return;
  OBJHND_setAnime(&bmodel->objHdl, bmodel->currentAnmIdx, req);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL FIELD_BMODEL_WaitAnime( FIELD_BMODEL * bmodel, u32 idx )
{
  if (bmodel == NULL) return TRUE;
  if ( FIELD_BMODEL_GetAnimeStatus( bmodel, idx ) == TRUE )
  {
    FIELD_BMODEL_SetAnime( bmodel, idx, BMANM_REQ_STOP );
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL FIELD_BMODEL_WaitCurrentAnime( FIELD_BMODEL * bmodel )
{
  if (bmodel == NULL) return TRUE;
  if ( FIELD_BMODEL_GetCurrentAnimeStatus( bmodel) == TRUE )
  {
    FIELD_BMODEL_SetCurrentAnime( bmodel, BMANM_REQ_STOP );
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------
/// �z�u���f���F�A�j����Ԏ擾
//------------------------------------------------------------------
BOOL FIELD_BMODEL_GetAnimeStatus( const FIELD_BMODEL * bmodel, u32 idx)
{
  if ( bmodel == NULL ) return FALSE;
  return OBJHND_getAnimeStatus( &bmodel->objHdl, idx );
}

//------------------------------------------------------------------
/// �z�u���f���F�A�j����Ԏ擾�i���O�̃C���f�b�N�X�Ώہj
//------------------------------------------------------------------
BOOL FIELD_BMODEL_GetCurrentAnimeStatus( const FIELD_BMODEL * bmodel )
{
  if ( bmodel == NULL ) return FALSE;
  return OBJHND_getAnimeStatus( &bmodel->objHdl, bmodel->currentAnmIdx );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BM_PROG_ID FIELD_BMODEL_GetProgID(const FIELD_BMODEL * bmodel)
{
  if ( bmodel == NULL ) return BM_PROG_ID_NONE;
  return bmodel->objHdl.res->bmInfo->prog_id;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
const G3DMAPOBJST * FIELD_BMODEL_GetG3DMAPOBJST( const FIELD_BMODEL * bmodel )
{
  if ( bmodel == NULL ) return NULL;
  return bmodel->objst;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void FIELD_BMODEL_Draw( const FIELD_BMODEL * bmodel )
{
  if ( bmodel == NULL ) return;
  GFL_G3D_DRAW_DrawObject( bmodel->objHdl.g3Dobj, &bmodel->g3dObjStatus );
}

//------------------------------------------------------------------
/**
 * @brief �z�u���f���ɑΉ��t�����ꂽSE�̎擾
 */
//------------------------------------------------------------------
BOOL FIELD_BMODEL_GetSENo( const FIELD_BMODEL * bmodel, u32 anm_idx, u16 * se_no )
{
  static const struct {
    u16 prog_id;
    u16 seID[BMANM_INDEX_MAX];
  } SeTbl[] = {
    { BM_PROG_ID_DOOR_NORMAL,   { SEQ_SE_FLD_20,  SEQ_SE_FLD_21 } },
    { BM_PROG_ID_DOOR_AUTO,     { SEQ_SE_FLD_22,  SEQ_SE_FLD_22 } },
    //{ BM_PROG_ID_BADGEGATE,   { SEQ_SE_FLD_20,  SEQ_SE_FLD_21 } },
    { BM_PROG_ID_PCELEVATOR,    { SEQ_SE_FLD_22,  SEQ_SE_FLD_22 } },
    { BM_PROG_ID_DOOR_ELEVATOR, { SEQ_SE_FLD_134, SEQ_SE_FLD_135 } },
    { BM_PROG_ID_DOOR_BELL,     { SEQ_SE_FLD_138, SEQ_SE_FLD_139 } },
    { BM_PROG_ID_DOOR_AUTO2,    { SEQ_SE_FLD_136, SEQ_SE_FLD_137 } },
  };
  int i;
  BM_PROG_ID id;

  if ( bmodel == NULL ) return FALSE;
  id = FIELD_BMODEL_GetProgID( bmodel );
  *se_no = 0;
  if ( anm_idx >= BMANM_INDEX_MAX )
  {
    TAMADA_Printf( "anm_idx(%d) >= BMANM_INDEX_MAX\n", anm_idx );
    return FALSE;
  }
  for (i = 0; i < NELEMS(SeTbl); i++)
  {
    if (SeTbl[i].prog_id == id)
    {
      *se_no = SeTbl[i].seID[anm_idx];
      //TAMADA_Printf("BMANIME_PROG_ID(%d) SE(%d)\n", id, *se_no);
      return TRUE;
    }
  }
  return FALSE;
}
//------------------------------------------------------------------
/// �z�u���f���F���݂̃A�j���Ή�SE�̎擾
//------------------------------------------------------------------
BOOL FIELD_BMODEL_GetCurrentSENo( const FIELD_BMODEL * bmodel, u16 * se_no )
{
  if ( bmodel == NULL ) return FALSE;
  return FIELD_BMODEL_GetSENo( bmodel, bmodel->currentAnmIdx, se_no );
}

//------------------------------------------------------------------
/// �z�u���f���F�Ή�SE�̏I���`�F�b�N
//------------------------------------------------------------------
BOOL FIELD_BMODEL_CheckCurrentSE( const FIELD_BMODEL * bmodel )
{
  u16 se_no;
  SEPLAYER_ID playerID;
  if ( FIELD_BMODEL_GetCurrentSENo( bmodel, &se_no ) == FALSE )
  {
    return FALSE;
  }
  playerID = PMSND_GetSE_DefaultPlayerID( se_no );
  return PMSND_CheckPlaySE_byPlayerID( playerID );
}





//------------------------------------------------------------------
//------------------------------------------------------------------
static void makeRect(FLDHIT_RECT * rect, const VecFx32 * pos)
{
  enum {
    RECT_SIZE_W = (FIELD_CONST_GRID_SIZE * 2) << FX32_SHIFT,
    RECT_SIZE_H = (FIELD_CONST_GRID_SIZE * 3) << FX32_SHIFT      
  };
  rect->top = pos->z - RECT_SIZE_H;
  rect->bottom = pos->z + RECT_SIZE_H;
  rect->left = pos->x - RECT_SIZE_W;
  rect->right = pos->x + RECT_SIZE_W;
}



#ifdef PM_DEBUG

// �X�N���v�g�p����\��
void FIELD_BMODEL_DEBUG_ScriptPrintInfo( const FIELD_BMODEL * bmodel )
{
  static const char* ANMTYPE_STR[BMANIME_TYPE_MAX] = 
  {
    "BMANIME_TYPE_NONE",
    "BMANIME_TYPE_ETERNAL",
    "BMANIME_TYPE_EVENT",
    "BMANIME_TYPE_TIMEZONE",
  };
  
  if ( bmodel == NULL )
  {
    OS_TPrintf( "address == NULL :�z�u���f�������݂��܂���I\n" );
    return;
  }
  OS_TPrintf( "�z�u���f�� ID�@      %d\n", bmodel->objHdl.res->bmInfo->bm_id+1 );
  OS_TPrintf( "�z�u���f���A�j�� ID�@%d\n", bmodel->objHdl.res->bmInfo->anm_id+1 );
  OS_TPrintf( "�z�u���f���ʒu�@x[%d] y[%d] z[%d]\n", FX_Whole(bmodel->g3dObjStatus.trans.x), FX_Whole(bmodel->g3dObjStatus.trans.y), FX_Whole(bmodel->g3dObjStatus.trans.z) );
  OS_TPrintf( "�z�u���f���O���b�h�@x[%d] y[%d] z[%d]\n", 
      FX_Whole(bmodel->g3dObjStatus.trans.x)/FIELD_CONST_GRID_SIZE, 
      FX_Whole(bmodel->g3dObjStatus.trans.y)/FIELD_CONST_GRID_SIZE,
      FX_Whole(bmodel->g3dObjStatus.trans.z)/FIELD_CONST_GRID_SIZE );

  OS_TPrintf( "�A�j���[�V�����^�C�v %s\n", ANMTYPE_STR[ bmodel->objHdl.res->bmInfo->animeData.anm_type ] );

  GF_ASSERT_MSG( bmodel->objHdl.res->bmInfo->animeData.anm_type == BMANIME_TYPE_EVENT, "�X�N���v�g����ANIMETYPE EVENT�ȊO�̔z�u�I�u�W�F�𑀍삵�悤�Ƃ��Ă��܂��B�@�ʒu���m�F���Ă��������B\n" );
}

#endif



//============================================================================================
//  DEBUG ���\�[�X�������g�p�ʂ̌���
//============================================================================================
#ifdef BMODEL_DEBUG_RESOURCE_MEMORY_SIZE

//----------------------------------------------------------------------------
/**
 *	@brief  ���\�[�X�Ŏg�p���Ă��郁�����[�T�C�Y�̎擾
 */
//-----------------------------------------------------------------------------
u32 FIELD_BMODEL_MAN_GetUseResourceMemorySize(void)
{
  return BMODEL_DEBUG_RESOURCE_MemorySize;
}

#endif

#include "script_def.h"
//�v���O�������Q�Ƃ̒�`�ƃX�N���v�g���Q�Ƃ̒�`������Ă�����assert
SDK_COMPILER_ASSERT( SCR_BMANM_DOOR_OPEN == BMANM_INDEX_DOOR_OPEN );
SDK_COMPILER_ASSERT( SCR_BMANM_DOOR_CLOSE == BMANM_INDEX_DOOR_CLOSE );

SDK_COMPILER_ASSERT( SCR_BMID_DOOR == BM_SEARCH_ID_DOOR );
SDK_COMPILER_ASSERT( SCR_BMID_SANDSTREAM == BM_SEARCH_ID_SANDSTREAM );
SDK_COMPILER_ASSERT( SCR_BMID_PCMACHINE == BM_SEARCH_ID_PCMACHINE );
SDK_COMPILER_ASSERT( SCR_BMID_PC == BM_SEARCH_ID_PC );
SDK_COMPILER_ASSERT( SCR_BMID_PCEV_DOOR == BM_SEARCH_ID_PCEV_DOOR );
SDK_COMPILER_ASSERT( SCR_BMID_PCEV_FLOOR == BM_SEARCH_ID_PCEV_FLOOR );

