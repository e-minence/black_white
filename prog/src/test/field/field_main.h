#ifndef __FIELD_MAIN_H__
#define __FIELD_MAIN_H__

#include "field_g3d_mapper.h"
#include "field_player.h"			//PC_ACT_FUNC

typedef struct _DEPEND_FUNCTIONS DEPEND_FUNCTIONS;
typedef struct _FIELD_MAIN_WORK FIELD_MAIN_WORK;

typedef struct {
	//���u���b�N��, �c�u���b�N��, �u���b�N�P�ӂ̕�, �O���t�B�b�N�A�[�J�C�u�h�c, ���}�b�v�f�[�^
	FLD_G3D_MAPPER_RESIST	mapperData;
	VecFx32				startPos;	//�J�n�ʒu
	const DEPEND_FUNCTIONS * dep_funcs;
}SCENE_DATA;

extern void DEBUG_FldGridProc_Camera( FIELD_MAIN_WORK *fieldWork );

extern const DEPEND_FUNCTIONS FieldBaseFunctions;
extern const DEPEND_FUNCTIONS FieldNoGridFunctions;
extern const DEPEND_FUNCTIONS FieldGridFunctions;
extern const SCENE_DATA	resistMapTbl[];
extern const unsigned int resistMapTblCount;

//field_sub_grid.c
extern void DEBUG_FldGridProc_Camera( FIELD_MAIN_WORK *fieldWork );
extern void DEBUG_FldGridProc_ScaleChange( FIELD_MAIN_WORK *fieldWork );

#endif //__FIELD_MAIN_H__
