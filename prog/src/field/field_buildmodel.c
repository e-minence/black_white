//============================================================================================
/**
 * @file	field_buildmodel.c
 * @brief		�z�u���f���̐���
 * @author	tamada GAMEFREAK inc.
 * @date	2009.04.20
 */
//============================================================================================

#include <gflib.h>

#include "arc_def.h"
#include "field/zonedata.h"
#include "field/areadata.h"

#include "field_buildmodel.h"
#include "field_bmanime.h"

#include "field_g3d_mapper.h"		//���L�w�b�_�ɕK�v
#include "fieldmap_resist.h"		//FLDMAPPER_RESISTDATA_OBJTBL�Ȃ�

#include "arc/fieldmap/area_id.h"
#include "arc/fieldmap/buildmodel_info.naix"

//============================================================================================
//============================================================================================

enum {  
  BMANIME_ID_COUNT_MAX = 4,

};

enum{ 
  FILEID_BMODEL_ANIMEDATA = NARC_buildmodel_info_buildmodel_anime_bin,


};
//------------------------------------------------------------------
//------------------------------------------------------------------
enum {
	BMODEL_ID_MAX = 400,	//�Ƃ肠����
	BMODEL_ENTRY_MAX = 128,	//�Ƃ肠����

	BMODEL_ID_NG	= BMODEL_ID_MAX,
	BMODEL_ENTRY_NG = BMODEL_ENTRY_MAX,

  BMANIME_NULL_ID  = 0xffff,
};

//------------------------------------------------------------------
//------------------------------------------------------------------
struct _FIELD_BMANIME_DATA
{ 
  u8 anm_type;  //BMANIME_TYPE  �A�j���̎�ގw��
  u8 prg_type;  //����v���O�����̎�ގw��
  u8 anm_count; //�A�j���J�E���g
  u8 set_count; //

  ///�A�j���A�[�J�C�u�w��ID
  u16 anm_id[BMANIME_ID_COUNT_MAX];

};

//------------------------------------------------------------------
//------------------------------------------------------------------
struct _FIELD_BMODEL_MAN
{
	HEAPID heapID;

	u8 IDToEntryTable[BMODEL_ID_MAX];

	u16 entryCount;
	BMODEL_ID entryToIDTable[BMODEL_ENTRY_MAX];
  FIELD_BMANIME_DATA animeData[BMODEL_ENTRY_MAX];

	ARCID model_arcid;

  //FLDMAPPER�Ɉ����n�����߂̐����f�[�^�ێ����[�N
	FLDMAPPER_RESISTDATA_OBJTBL	gobjData_Tbl;
	FLDMAPPER_RESISTOBJDATA resistObjTbl[BMODEL_ENTRY_MAX * 2];
};
//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void makeIDToEntryTable(FIELD_BMODEL_MAN * man);
static void makeResistObjTable(FIELD_BMODEL_MAN * man);
static u16 calcArcIndex(u16 area_id);

static void loadBModelIDList(FIELD_BMODEL_MAN * man, u16 arc_id, u16 file_id);

static void loadAnimeData(FIELD_BMODEL_MAN * man, u16 file_id);
//------------------------------------------------------------------
//------------------------------------------------------------------
static void FIELD_BMANIME_DATA_init(FIELD_BMANIME_DATA * data);

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �z�u���f���}�l�W���[����
 * @param heapID
 */
//------------------------------------------------------------------
FIELD_BMODEL_MAN * FIELD_BMODEL_MAN_Create(HEAPID heapID)
{	
	FIELD_BMODEL_MAN * man = GFL_HEAP_AllocMemory(heapID, sizeof(FIELD_BMODEL_MAN));
	man->heapID = heapID;
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
	GFL_HEAP_FreeMemory(man);
}

//------------------------------------------------------------------
/**
 * @brief �z�u���f���f�[�^�ǂݍ��ݏ���
 * @param man �z�u���f���}�l�W���[�ւ̃|�C���^
 */
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_Load(FIELD_BMODEL_MAN * man, u16 zoneid)
{	
	u16 area_id = ZONEDATA_GetAreaID(zoneid);
	u16 bmlist_index = calcArcIndex(area_id);
  ARCID bmlist_arc_id;
  u16 model_info_dataid;
  int i;

	GFL_STD_MemClear(man->IDToEntryTable, sizeof(man->IDToEntryTable));
	GFL_STD_MemFill16(man->entryToIDTable, BMODEL_ENTRY_NG, sizeof(man->entryToIDTable));
  for (i = 0; i < BMODEL_ENTRY_MAX; i++)
  { 
    FIELD_BMANIME_DATA_init(&man->animeData[i]);
  }

  if (AREADATA_GetInnerOuterSwitch(area_id) != 0) 
  {	
    man->model_arcid = ARCID_BMODEL_OUTDOOR;
    model_info_dataid = NARC_buildmodel_info_buildmodel_outdoor_bin;
    bmlist_arc_id = ARCID_BMODEL_IDX_OUTDOOR;
  }
  else
  {	
    man->model_arcid = ARCID_BMODEL_INDOOR;
    model_info_dataid = NARC_buildmodel_info_buildmodel_indoor_bin;
    bmlist_arc_id = ARCID_BMODEL_IDX_INDOOR;
  }

  //�G���A�ʔz�u���f��ID���X�g�̓ǂݍ���
  loadBModelIDList(man, bmlist_arc_id, bmlist_index);

	//ID���o�^�����̕ϊ��f�[�^�e�[�u������
	makeIDToEntryTable(man);

  //�K�v�Ȕz�u���f���A�j���f�[�^�̓ǂݍ���
  loadAnimeData(man, model_info_dataid);

  //FLDMAPPER�Ɉ����n���f�[�^�𐶐�
	makeResistObjTable(man);
}

//------------------------------------------------------------------
/**
 * @brief
 * @param man �z�u���f���}�l�W���[�ւ̃|�C���^
 * @return  FLDMAPPER_RESISTDATA_OBJTBL ���������f�[�^�ւ̃|�C���^
 *
 * �����f�[�^��ێ����郁�����͂̓}�l�W���[�Ǘ����ɒu����Ă���B
 * ���̃^�C�~���O�Ŋm�ۂ��Ă���킯�ł͂Ȃ�
 */
//------------------------------------------------------------------
const FLDMAPPER_RESISTDATA_OBJTBL * FIELD_BMODEL_MAN_GetOBJTBL(const FIELD_BMODEL_MAN * man)
{	
	return &man->gobjData_Tbl;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
#if 0
u16 FIELD_BMODEL_MAN_GetNarcIndex(const FIELD_BMODEL_MAN * man, BMODEL_ID id)
{
	int index;
	GF_ASSERT(id < BMODEL_ID_MAX);
	index = man->IDToEntryTable[id];
	GF_ASSERT(index < BMODEL_ENTRY_MAX);
	return man->entryToIDTable[index];
}
#endif

//------------------------------------------------------------------
/**
 * @brief �z�u���f��ID��o�^�ςݔz�u���f���̃C���f�b�N�X�ɕϊ�����
 * @param man �z�u���f���}�l�W���[�ւ̃|�C���^
 */
//------------------------------------------------------------------
u16 FIELD_BMODEL_MAN_GetEntryIndex(const FIELD_BMODEL_MAN * man, BMODEL_ID id)
{	
	u16 entry;
	GF_ASSERT(id < BMODEL_ID_MAX);
	entry = man->IDToEntryTable[id];
	TAMADA_Printf("bmodel: id(%d) -->entry_index(%d)\n",id, entry);
	return entry;
}

//------------------------------------------------------------------
/**
 * @brief �z�u���f��ID����Ή�����A�j���f�[�^���擾����
 * @param bm_id   �z�u���f��ID
 */
//------------------------------------------------------------------
const FIELD_BMANIME_DATA * FIELD_BMODEL_MAN_GetAnimeData(FIELD_BMODEL_MAN * man, u16 bm_id)
{ 
  u16 entry_id = man->IDToEntryTable[bm_id];
  GF_ASSERT(bm_id < BMODEL_ID_MAX);
  GF_ASSERT(entry_id < man->entryCount);
  return &man->animeData[entry_id];
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
 * entryCount/entryToIDTable������������
 */
//------------------------------------------------------------------
static void loadBModelIDList(FIELD_BMODEL_MAN * man, u16 arc_id, u16 file_id)
{ 
  ARCHANDLE * hdl = GFL_ARC_OpenDataHandle(arc_id, man->heapID);
  u16 data_count = GFL_ARC_GetDataFileCntByHandle(hdl);

  TAMADA_Printf("bmodel list id = %d\n",file_id);
  if (data_count < file_id)
  {	
    GF_ASSERT_MSG(0, "�z�u���f�����X�g�f�[�^������܂���(%d<%d)\n", data_count, file_id);
    file_id = 0;		//�Ƃ肠�����n���O�A�b�v���
  }

	//�ǂݍ���
	{	
		u16 size = GFL_ARC_GetDataSizeByHandle(hdl, file_id);
		man->entryCount = size / sizeof(BMODEL_ID);
		if(size > sizeof(man->entryToIDTable))
		{	
			GF_ASSERT_MSG(0, "�z�u���f�����X�g�f�[�^���I�[�o�[�isize=%d ARCINDEX=%d)\n", size, file_id);
			man->entryCount = BMODEL_ENTRY_MAX;	//�Ƃ肠�����n���O�A�b�v���
			size = sizeof(man->entryToIDTable);
		}
		TAMADA_Printf("entryCount=%d\n", man->entryCount);
		GFL_ARC_LoadDataOfsByHandle(hdl, file_id, 0, size, man->entryToIDTable);
	}
	GFL_ARC_CloseDataHandle(hdl);
}

//------------------------------------------------------------------
//BMODEL_ID�ň���IDToEntryTable�ɂ�entryToIDTable�ւ̃I�t�Z�b�g���i�[
//------------------------------------------------------------------
static void makeIDToEntryTable(FIELD_BMODEL_MAN * man)
{
	int i;
	for (i = 0; i < man->entryCount; i++)
	{	
		int pos = man->entryToIDTable[i];
		man->IDToEntryTable[pos] = i;
	}
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


//------------------------------------------------------------------
/**
 * @brief FLDMAPPER�ł̓o�^�Ɏg�p�ł���f�[�^�𐶐�
 */
//------------------------------------------------------------------
static void makeResistObjTable(FIELD_BMODEL_MAN * man)
{
	static const FLDMAPPER_RESISTDATA_OBJTBL init = {	
		0, NULL, 0, 0, NULL, 0
	};
	int i;
	FLDMAPPER_RESISTDATA_OBJTBL * resist = &man->gobjData_Tbl;
	FLDMAPPER_RESISTOBJDATA * mapperObj = man->resistObjTbl;
	GFL_STD_MemFill16(mapperObj, NON_LOWQ, sizeof(man->resistObjTbl));
	for (i = 0; i < man->entryCount; i++)
	{	
		mapperObj[i].highQ_ID = man->entryToIDTable[i];
	}
	*resist = init;
	resist->objArcID = man->model_arcid;
	resist->objData = mapperObj;
	resist->objCount = man->entryCount;
}

//============================================================================================
//
//
//
//    �z�u���f���A�j���f�[�^
//
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �ێ����Ă���A�j��ID�ւ̃|�C���^��Ԃ�
 */
//------------------------------------------------------------------
const u16 * FIELD_BMANIME_DATA_getAnimeFileID(const FIELD_BMANIME_DATA * data)
{ 
  return data->anm_id;
}

//------------------------------------------------------------------
/**
 * @brief �ێ����Ă���A�j��ID�̐���Ԃ�
 */
//------------------------------------------------------------------
u32 FIELD_BMANIME_DATA_getAnimeCount(const FIELD_BMANIME_DATA * data)
{ 
  u32 i,count;
  for (i = 0, count = 0; i < BMANIME_ID_COUNT_MAX; i++)
  { 
    if (data->anm_id[i] != BMANIME_NULL_ID)
    { 
      count ++;
    }
  }
  return count;
}
//------------------------------------------------------------------
/**
 * @brief �A�j���^�C�v��Ԃ�
 * @return  BMANIME_TYPE
 */
//------------------------------------------------------------------
BMANIME_TYPE FIELD_BMANIME_DATA_getAnimeType(const FIELD_BMANIME_DATA * data)
{ 
  return data->anm_type;
}

//------------------------------------------------------------------
/**
 * @brief �v���O�����^�C�v��Ԃ�
 * @return  BMANIME_PROG_TYPE
 */
//------------------------------------------------------------------
BMANIME_PROG_TYPE FIELD_BMANIME_DATA_getProgType(const FIELD_BMANIME_DATA * data)
{ 
  return data->prg_type;
}

//------------------------------------------------------------------
/**
 * @brief FIELD_BMANIME_DATA�̏�����
 */
//------------------------------------------------------------------
static void FIELD_BMANIME_DATA_init(FIELD_BMANIME_DATA * data)
{ 
  static const FIELD_BMANIME_DATA init = {  
    BMANIME_TYPE_NONE,  //�A�j���̎�ގw��
    BMANIME_PROG_TYPE_NONE,  //����v���O�����̎�ގw��
    0,  //�A�j���J�E���g�i���j
    0,  //�Z�b�g�J�E���g�i���j
    //�A�j���A�[�J�C�u�w��ID
    { BMANIME_NULL_ID, BMANIME_NULL_ID, BMANIME_NULL_ID, BMANIME_NULL_ID, }

  };
  *data = init;
}
//------------------------------------------------------------------
/**
 * @brief �z�u���f���p�A�j���f�[�^�̓ǂݍ���
 */
//------------------------------------------------------------------
static void loadAnimeData(FIELD_BMODEL_MAN * man, u16 file_id)
{ 
  ARCHANDLE * info_hdl = GFL_ARC_OpenDataHandle(ARCID_BMODEL_INFO, man->heapID);
  ARCHANDLE * animeHdl = GFL_ARC_OpenDataHandle(ARCID_BMODEL_ANIME, man->heapID);
  u16 anime_id;
  int i;

  for (i = 0; i < man->entryCount; i++)
  { 
    BMODEL_ID id = man->entryToIDTable[i];
    GFL_ARC_LoadDataOfsByHandle(info_hdl, file_id, id * sizeof(u16), sizeof(u16), &anime_id);
    TAMADA_Printf("bmodel: entry ID(%d) --> anime_id(%d)\n", id, anime_id);
    if (anime_id != 0xffff)
    { 
      GFL_ARC_LoadDataOfsByHandle(animeHdl, FILEID_BMODEL_ANIMEDATA,
          anime_id * sizeof(FIELD_BMANIME_DATA), sizeof(FIELD_BMANIME_DATA),
          &man->animeData[i]);
    }
  }
  GFL_ARC_CloseDataHandle(info_hdl);
  GFL_ARC_CloseDataHandle(animeHdl);
}

//------------------------------------------------------------------
//------------------------------------------------------------------

