//============================================================================================
/**
 * @file	field_func_wbnormal_file.h
 *
 *
 *
 * @brief	�f�e�}�b�v�G�f�B�^���쐬�����f�[�^����������֐��S
 *  //�z���C�g�t�H���X�g�E�u���b�N�V�e�B�p�����_������
 *
 *
 */
//============================================================================================
#include "map\dp3format.h"
//============================================================================================
/**
 * @brief	�}�b�v�f�[�^�ǂݍ���
 */
//============================================================================================
extern BOOL FieldLoadMapData_RandomGenerate( GFL_G3D_MAP* g3Dmap, void * exWork );
//============================================================================================
/**
 * @brief	�R�c�}�b�v���擾
 */
//============================================================================================
extern void FieldGetAttr_RandomGenerate( GFL_G3D_MAP_ATTRINFO* attrInfo,
									const void* mapdata, const VecFx32* posInBlock, 
									const fx32 map_width, const fx32 map_height );

extern void FIELD_FUNC_RANDOM_GENERATE_InitDebug( HEAPID heapId );
extern void FIELD_FUNC_RANDOM_GENERATE_TermDebug();
