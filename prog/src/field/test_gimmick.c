#include "fieldmap.h"
#include "arc/fieldmap/buildmodel_outdoor.naix"

#define TEST_UNIT_IDX (1)

//�ǂݍ���3D���\�[�X
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	//1P
	{ ARCID_BMODEL_OUTDOOR, NARC_output_buildmodel_outdoor_trailer_01_nsbmd, GFL_G3D_UTIL_RESARC },
///	{ ARCID_BMODEL_OUTDOOR, NARC_output_buildmodel_outdoor_h01_ship_nsbmd, GFL_G3D_UTIL_RESARC },
};


//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
	//1P
	{
		0, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		0, 			//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
/*  
  {
		1, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		1, 			//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
*/	
};


static const GFL_G3D_UTIL_SETUP Setup = {
  g3Dutil_resTbl,				//���\�[�X�e�[�u��
	NELEMS(g3Dutil_resTbl),		//���\�[�X��
	g3Dutil_objTbl,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(g3Dutil_objTbl),		//�I�u�W�F�N�g��
};

void GYM_SetupTest(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr;
  ptr = FIELDMAP_GetExpObjCntPtr(fieldWork);

  FLD_EXP_OBJ_AddUnit(ptr, &Setup, TEST_UNIT_IDX );
}

void GYM_EndTest(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr;
  ptr = FIELDMAP_GetExpObjCntPtr(fieldWork);
  FLD_EXP_OBJ_DelUnit( ptr, TEST_UNIT_IDX );
}

void GYM_MoveTest(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr;
  VecFx32 pos;

  ptr = FIELDMAP_GetExpObjCntPtr(fieldWork);

	// �v���C���[�̈ʒu����
	FIELD_PLAYER_GetPos( FIELDMAP_GetFieldPlayer( fieldWork ), &pos );
  //���W�Z�b�g
  {
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, TEST_UNIT_IDX, 0);
    status->trans = pos;
  }

  //�A�j���[�V�����̍Đ�
  ;
}
