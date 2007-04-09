//=============================================================================================
/**
 * @file	g3d_scene.h
 * @brief	�R�c�V�[���Ǘ��v���O����
 */
//=============================================================================================
#ifndef _G3D_SCENE_H_
#define _G3D_SCENE_H_

//=============================================================================================
//	�^�錾
//=============================================================================================
typedef struct _GFL_G3D_SCENE		GFL_G3D_SCENE;
typedef struct _GFL_G3D_SCENEOBJ	GFL_G3D_SCENEOBJ;	
typedef void						(GFL_G3D_SCENEOBJFUNC)( GFL_G3D_SCENEOBJ*, void* );

typedef struct {
	u32						objID;
	u8						movePriority;				
	u8						drawPriority;				
	u8						blendAlpha;
	BOOL					drawSW;
	GFL_G3D_OBJSTATUS		status; 
	GFL_G3D_SCENEOBJFUNC*	func;
}GFL_G3D_SCENEOBJ_DATA;

//=============================================================================================
/**
 * �Ǘ��V�X�e��
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * �V�X�e���쐬
 *
 * @param	g3Dutil			�ˑ�����g3Dutil
 * @param	SceneObjMax		�z�u�\�ȃI�u�W�F�N�g��
 * @param	SceneObjWkSiz	�P�I�u�W�F�N�g�Ɋ��蓖�Ă郏�[�N�̃T�C�Y
 * @param	heapID			�q�[�v�h�c
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_SCENE*
	GFL_G3D_SCENE_Create
		( GFL_G3D_UTIL* g3Dutil, const u16 sceneObjMax, const u32 sceneObjWkSiz, 
			const HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * �V�X�e�����C���i����j
 *
 * @param	g3Dscene		�V�X�e���|�C���^
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_SCENE_Main
		( GFL_G3D_SCENE* g3Dscene );  

//--------------------------------------------------------------------------------------------
/**
 * �V�X�e�����C���i�`��j
 *
 * @param	g3Dscene		�V�X�e���|�C���^
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_SCENE_Draw
		( GFL_G3D_SCENE* g3Dscene );  

//--------------------------------------------------------------------------------------------
/**
 * �V�X�e���j��
 *
 * @param	g3Dscene		�V�X�e���|�C���^
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_SCENE_Delete
		( GFL_G3D_SCENE* g3Dscene );  

//--------------------------------------------------------------------------------------------
/**
 * �z�u�I�u�W�F�N�g�|�C���^���h�m�c�d�w���擾
 *
 * @param	g3Dscene		�V�X�e���|�C���^
 * @param	idx				�A�N�^�[�z�u�h�m�c�d�w
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_SCENEOBJ*
	GFL_G3D_SCENEOBJ_Get
		( GFL_G3D_SCENE* g3Dscene, u32 idx );

//=============================================================================================
/**
 * �z�u�I�u�W�F�N�g
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * �쐬
 *
 * @param	g3Dscene		�V�X�e���|�C���^
 * @param	sceneObjTbl		�z�u�I�u�W�F�N�g�ݒ�f�[�^
 * @param	sceneObjCount	�z�u�I�u�W�F�N�g��
 *
 * @return	idx				�z�u�I�u�W�F�N�g�擪�h�m�c�d�w
 */
//--------------------------------------------------------------------------------------------
extern u32
	GFL_G3D_SCENEOBJ_Add
		( GFL_G3D_SCENE* g3Dscene, const GFL_G3D_SCENEOBJ_DATA* sceneObjTbl, 
			const u16 sceneObjCount );

//--------------------------------------------------------------------------------------------
/**
 * �j��
 *
 * @param	g3Dscene		�V�X�e���|�C���^
 * @param	idx				�z�u�I�u�W�F�N�g�擪�h�m�c�d�w
 * @param	sceneObjCount	�z�u�I�u�W�F�N�g��
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_SCENEOBJ_Remove
		( GFL_G3D_SCENE* g3Dscene, u32 idx, const u16 sceneObjCount );

//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g�n���h��(G3D_OBJ)�̎擾
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_OBJ*
	GFL_G3D_SCENEOBJ_GetG3DobjHandle
		( GFL_G3D_SCENEOBJ* g3DsceneObj );

//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g�h�c�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	objID			�I�u�W�F�N�g�h�c�̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_GetObjID( GFL_G3D_SCENEOBJ* g3DsceneObj, u16* objID );
extern void GFL_G3D_SCENEOBJ_SetObjID( GFL_G3D_SCENEOBJ* g3DsceneObj, u16* objID );

//--------------------------------------------------------------------------------------------
/**
 * �`��v���C�I���e�B�[�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	drawPri			�`��v���C�I���e�B�[�̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_GetDrawPri( GFL_G3D_SCENEOBJ* g3DsceneObj, u8* drawPri );
extern void GFL_G3D_SCENEOBJ_SetDrawPri( GFL_G3D_SCENEOBJ* g3DsceneObj, u8* drawPri );

//--------------------------------------------------------------------------------------------
/**
 * ���u�����h�l�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	blendAlpha		���u�����h�̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_GetBlendAlpha( GFL_G3D_SCENEOBJ* g3DsceneObj, u8* blendAlpha );
extern void GFL_G3D_SCENEOBJ_SetBlendAlpha( GFL_G3D_SCENEOBJ* g3DsceneObj, u8* blendAlpha );

//--------------------------------------------------------------------------------------------
/**
 * �`��X�C�b�`�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	drawSW			�`��X�C�b�`�̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_GetDrawSW( GFL_G3D_SCENEOBJ* g3DsceneObj, BOOL* drawSW );
extern void GFL_G3D_SCENEOBJ_SetDrawSW( GFL_G3D_SCENEOBJ* g3DsceneObj, BOOL* drawSW );

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�i�ʒu���j�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	trans			�ʒu���̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_GetPos( GFL_G3D_SCENEOBJ* g3DsceneObj, VecFx32* trans );
extern void GFL_G3D_SCENEOBJ_SetPos( GFL_G3D_SCENEOBJ* g3DsceneObj, VecFx32* trans );

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�i�X�P�[�����j�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	scale			�X�P�[�����̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_GetScale( GFL_G3D_SCENEOBJ* g3DsceneObj, VecFx32* scale );
extern void GFL_G3D_SCENEOBJ_SetScale( GFL_G3D_SCENEOBJ* g3DsceneObj, VecFx32* scale );

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�i��]���j�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	rotate			��]���̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_GetRotate( GFL_G3D_SCENEOBJ* g3DsceneObj, MtxFx33* rotate );
extern void GFL_G3D_SCENEOBJ_SetRotate( GFL_G3D_SCENEOBJ* g3DsceneObj, MtxFx33* rotate );

//--------------------------------------------------------------------------------------------
/**
 * ����֐��̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	scale			�X�P�[�����̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_GetFunc( GFL_G3D_SCENEOBJ* g3DsceneObj, GFL_G3D_SCENEOBJFUNC** func );
extern void GFL_G3D_SCENEOBJ_SetFunc( GFL_G3D_SCENEOBJ* g3DsceneObj, GFL_G3D_SCENEOBJFUNC** func );



#endif
