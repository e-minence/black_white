#include "field_g3d_mapper.h"

typedef struct {
	//���u���b�N��, �c�u���b�N��, �u���b�N�P�ӂ̕�, �O���t�B�b�N�A�[�J�C�u�h�c, ���}�b�v�f�[�^
	FLD_G3D_MAPPER_RESIST	mapperData;
	VecFx32				startPos;	//�J�n�ʒu
	const char * mapname;			//�}�b�v��
}SCENE_DATA;

extern const SCENE_DATA	resistMapTbl[];
extern const unsigned int resistMapTblCount;
