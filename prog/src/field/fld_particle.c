//======================================================================
/**
 * @file	fld_particle.c
 * @brief	�t�B�[���h�p�[�e�B�N��
 * @author	Saito
 *
 */
//======================================================================

#include <gflib.h>	//for particle
#include "fld_particle.h"

#include "arc_def.h"


///�퓬�p�[�e�B�N���̃J�����j�A�ݒ�
#define BP_NEAR			(FX32_ONE)
///�퓬�p�[�e�B�N���̃J�����t�@�[�ݒ�
#define BP_FAR			(FX32_ONE * 900)


typedef struct FLD_PRTCL_SYS_tag
{
  GFL_G3D_CAMERA *CameraPtr;
  HEAPID HeapID;
  void *PrtclBuff;
	GFL_PTC_PTR PrtclPtr;
}FLD_PRTCL_SYS;


static void ParticleCallBack(GFL_EMIT_PTR emit);

//--------------------------------------------------------------------------------------------
/**
 * ������
 *
 * @param   inHeapID			    �q�[�v�h�c
 *
 * @return	FLD_PRTCL_SYS_PTR   ���W���[���|�C���^
 */
//--------------------------------------------------------------------------------------------
FLD_PRTCL_SYS_PTR FLD_PRTCL_Init(const HEAPID inHeapID)
{
  FLD_PRTCL_SYS_PTR sys;

  sys = GFL_HEAP_AllocClearMemory(inHeapID, sizeof(FLD_PRTCL_SYS));
  sys->HeapID = inHeapID;
	
  GFL_PTC_Init(inHeapID);

	//�p�[�e�B�N���������m��
	sys->PrtclBuff = GFL_HEAP_AllocClearMemory(inHeapID, PARTICLE_LIB_HEAP_SIZE);
#if 0
	sys->PrtclPtr = GFL_PTC_Create(
											  sys->PrtclBuff, 
											  PARTICLE_LIB_HEAP_SIZE, TRUE, inHeapID);
  camera_ptr = GFL_PTC_GetCameraPtr(sys->PrtclPtr);

  {
    fx32 near = BP_NEAR;
    fx32 far = BP_FAR;
    GFL_G3D_CAMERA_SetNear( camera_ptr, &near );
    GFL_G3D_CAMERA_SetFar( camera_ptr, &far );
  }

  //@todo
  {
    void *resource;
    resource = FLD_PRTCL_LoadResource(sys,
			ARCID_FLD_SPA, /*NARC_particledata_title_part_spa*/0);

    FLD_PRTCL_SetResource(sys, resource, TRUE, NULL);
  }
#endif
  return sys;
}

//--------------------------------------------------------------------------------------------
/**
 * �p�[�e�B�N���|�C���^����
 *
 * @param   FLD_PRTCL_SYS_PTR sys
 *
 * @return	GFL_PTC_PTR   �p�[�e�B�N���|�C���^
 *
 * @note    �߂�l�Ƃ��Ď擾�����|�C���^�͊O�ŉ�����Ȃ��悤�ɁB
 */
//--------------------------------------------------------------------------------------------
GFL_PTC_PTR FLD_PRTCL_Create(FLD_PRTCL_SYS_PTR sys)
{
  GFL_G3D_CAMERA *camera_ptr;

  sys->PrtclPtr = GFL_PTC_Create(
											  sys->PrtclBuff, 
											  PARTICLE_LIB_HEAP_SIZE, TRUE, sys->HeapID);
  camera_ptr = GFL_PTC_GetCameraPtr(sys->PrtclPtr);

  {
    fx32 near = BP_NEAR;
    fx32 far = BP_FAR;
    GFL_G3D_CAMERA_SetNear( camera_ptr, &near );
    GFL_G3D_CAMERA_SetFar( camera_ptr, &far );
  }

  return sys->PrtclPtr;
}

//--------------------------------------------------------------------------------------------
/**
 * �p�e�B�N���|�C���^�j��
 *
 * @param   FLD_PRTCL_SYS_PTR sys
 *
 * @return	FLD_PRTCL_SYS_PTR   ���W���[���|�C���^
 */
//--------------------------------------------------------------------------------------------
void FLD_PRTCL_Delete(FLD_PRTCL_SYS_PTR sys)
{
  if (sys->PrtclPtr!=NULL){
    GFL_PTC_Delete(sys->PrtclPtr);
    sys->PrtclPtr = NULL;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�̃��[�h
 *
 * @param   
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void *FLD_PRTCL_LoadResource(FLD_PRTCL_SYS_PTR sys, int inArcID, int inArcIdx)
{
  void *resource;
  resource = GFL_PTC_LoadArcResource(inArcID, inArcIdx, sys->HeapID);
  return resource;
}

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�̃Z�b�g
 *
 * @param   
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FLD_PRTCL_SetResource(FLD_PRTCL_SYS_PTR sys, void *resource, int inTexAtOnce, GFL_TCBSYS *tcbsys)
{
  GFL_PTC_SetResource(sys->PrtclPtr, resource, inTexAtOnce, tcbsys);
}

//--------------------------------------------------------------------------------------------
/**
 * �G�~�b�^�[�̏I���`�F�b�N
 *
 * @param   
 *
 * @return	BOOL      TRUE:�I��   FALSE:�p��
 */
//--------------------------------------------------------------------------------------------
BOOL FLD_PRTCL_CheckEmitEnd(FLD_PRTCL_SYS_PTR sys)
{
  if (sys->PrtclPtr == NULL){
    return TRUE;
  }

  if ( GFL_PTC_GetEmitterNum(sys->PrtclPtr) == 0 ){
    return TRUE;
  }

  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * ���
 *
 * @param   sys     ���W���[���|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FLD_PRTCL_End(FLD_PRTCL_SYS_PTR sys)
{
  //�p�[�e�B�N�����
  FLD_PRTCL_Delete(sys);
	GFL_PTC_Exit();
  GFL_HEAP_FreeMemory( sys->PrtclBuff );
	GFL_HEAP_FreeMemory( sys );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���֐�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FLD_PRTCL_Main(void)
{
///  GF_G3X_Reset();
  GFL_PTC_Main();
}
