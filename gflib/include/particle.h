//==============================================================================
/**
 * @file	particle.h
 * @brief	�p�[�e�B�N������w�b�_
 * @author	matsuda
 * @date	2005.07.11(��)
 */
//==============================================================================
#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "spl.h"

#define MATSUDA_TEST

//==============================================================================
//	�萔��`
//==============================================================================
//--------------------------------------------------------------
//	Particle_ResourceSet�֐���vram_free_mode
//--------------------------------------------------------------
///�e�N�X�`����������Ȃ�
#define PTC_AUTOTEX_NULL	(0)
///�t���[���e�N�X�`��VRAM�}�l�[�W�����g���ĉ��
#define PTC_AUTOTEX_FRM		(1 << 0)
///�����N�h���X�g�e�N�X�`��VRAM�}�l�[�W�����g���ĉ��
#define PTC_AUTOTEX_LNK		(1 << 1)

///�p���b�g��������Ȃ�
#define PTC_AUTOPLTT_NULL	(0)
///�t���[���p���b�gVRAM�}�l�[�W�����g���ĉ��
#define PTC_AUTOPLTT_FRM	(1 << 2)
///�����N�h���X�g�p���b�gVRAM�}�l�[�W�����g���ĉ��
#define PTC_AUTOPLTT_LNK	(1 << 3)

//--------------------------------------------------------------
//	�V�X�e����SPL���C�u�����Ƃ���肷��ۂɎg�p����O���[�o�����[�N�̔ԍ�
//--------------------------------------------------------------
enum{
	PARTICLE_GLOBAL_0 = 0,
	PARTICLE_GLOBAL_1,
	PARTICLE_GLOBAL_2,
	//�{�[���J�X�^�}�C�Y���ő�8��spa��ǂݍ��ނ̂ő��₵��
	PARTICLE_GLOBAL_3,
	PARTICLE_GLOBAL_4,
	PARTICLE_GLOBAL_5,
	PARTICLE_GLOBAL_6,
	PARTICLE_GLOBAL_7,

	//�{�[���J�X�^�}�C�Y��2�����ɏo��̂�2�{�K�v����B
	PARTICLE_GLOBAL_8,
	PARTICLE_GLOBAL_9,
	PARTICLE_GLOBAL_10,
	PARTICLE_GLOBAL_11,
	PARTICLE_GLOBAL_12,
	PARTICLE_GLOBAL_13,
	PARTICLE_GLOBAL_14,
	PARTICLE_GLOBAL_15,
	
	PARTICLE_GLOBAL_MAX,
};

//--------------------------------------------------------------
//	�g�p�q�[�v�̈�
//--------------------------------------------------------------
///�p�[�e�B�N���V�X�e���Ŏg�p����q�[�v�̈�
#define PTC_SYS_HEAP		(HEAPID_BATTLE)	//(SYSTEM_W_HEAP_AREA)

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
///�p�[�e�B�N���V�X�e������SPL���C�u�������g�p����q�[�v���[�N�̕W���T�C�Y
//#define PARTICLE_LIB_HEAP_SIZE		(0x4200)	///< EMIT_MAX��3����20�ɑ��₵���̂ŁA0x3800����ύX
#define PARTICLE_LIB_HEAP_SIZE		(0x4800)	///< �J�X�^���{�[���ȊO�@���X0x4200
#define PARTICLE_LIB_HEAP_SIZE_CB	(0x4200)	///< �J�X�^���{�[�� 0x4200�܂ł�����0x41D8�����E


///< �p�[�e�B�N���ŕK�{�Ƃ��Ă郁�����̃T�C�Y
///< (0x419c)

//==============================================================================
//	�^��`
//==============================================================================
///�p�[�e�B�N���V�X�e�����[�N�ւ̃|�C���^
typedef struct _PARTICLE_SYSTEM * GFL_PTC_PTR;


#ifdef PM_DEBUG

extern GFL_PTC_PTR gDebugPTC;

static inline GFL_PTC_PTR DEBUG_GFL_PTC_PTR_Get(void)
{
	return gDebugPTC;
}

#endif


///�G�~�b�^�̃|�C���^�^
typedef SPLEmitter * GFL_EMIT_PTR;

///�e�N�X�`�����[�h����VRAM�A�h���X��Ԃ����߂̃R�[���o�b�N�֐��̌^
typedef u32 (*pTexCallback)(u32, BOOL);

///�G�~�b�^�������ɌĂ΂��R�[���o�b�N�֐��̌^
typedef void (*pEmitFunc)(GFL_EMIT_PTR);

//==============================================================================
//	�O���֐��錾
//==============================================================================
extern	void			GFL_PTC_Init(HEAPID heapID);
extern	void			GFL_PTC_Main(void);
extern	GFL_PTC_PTR		GFL_PTC_Create(void *work, int work_size, int personal_camera, int heap_id);
extern	void			GFL_PTC_Delete(GFL_PTC_PTR psys);
extern	void			GFL_PTC_Exit(void);
extern	BOOL			GFL_PTC_CheckEnable(void);

//����path�w��͂Ƃ肠�����Ȃ�
//extern	void			*Particle_ResourceLoad(int heap_id, const char *path);
extern	void			*GFL_PTC_LoadArcResource(int file_kind, int index, int heap_id);
extern	void			GFL_PTC_SetResource(GFL_PTC_PTR psys, void *resource, int tex_at_once, GFL_TCBSYS *tcbsys);
extern	GFL_EMIT_PTR	GFL_PTC_CreateEmitter(GFL_PTC_PTR psys, int res_no, const VecFx32 *init_pos);
extern	void			GFL_PTC_Draw(GFL_PTC_PTR psys);
extern	void			GFL_PTC_Calc(GFL_PTC_PTR psys);
extern	int				GFL_PTC_DrawAll(void);
extern	int				GFL_PTC_CalcAll(void);
extern	s32				GFL_PTC_GetEmitterNum(GFL_PTC_PTR psys);
extern	void			GFL_PTC_DeleteEmitterAll(GFL_PTC_PTR psys);
extern	void			GFL_PTC_DeleteEmitter(GFL_PTC_PTR psys, GFL_EMIT_PTR emit);
extern	GFL_EMIT_PTR	GFL_PTC_GetTempEmitterPtr(GFL_PTC_PTR psys);
extern	GFL_EMIT_PTR	GFL_PTC_CreateEmitterCallback(GFL_PTC_PTR psys, int res_no, pEmitFunc callback, void *temp_ptr);
extern	void			*GFL_PTC_GetHeapPtr(GFL_PTC_PTR psys);
extern	void			GFL_PTC_GetEye(GFL_PTC_PTR psys, VecFx32 *eye);
extern	void			GFL_PTC_SetEye(GFL_PTC_PTR psys, const VecFx32 *eye);
extern	void			GFL_PTC_GetVup(GFL_PTC_PTR psys, VecFx32 *up);
extern	void			GFL_PTC_SetVup(GFL_PTC_PTR psys, const VecFx32 *up);
extern	void			*GFL_PTC_GetTempPtr(void);
extern	void			GFL_PTC_GetDefaultEye(VecFx32 *eye);
extern	void			GFL_PTC_GetDefaultUp(VecFx32 *v_up);
extern	void			GFL_PTC_GetDefaultAt(VecFx32 *at);
extern	int				GFL_PTC_GetActionNum(void);
extern	GFL_G3D_CAMERA	*GFL_PTC_GetCameraPtr(GFL_PTC_PTR ptc);
extern	void			GFL_PTC_SetCameraType(GFL_PTC_PTR ptc, int camera_type);
extern	u8				GFL_PTC_GetCameraType(GFL_PTC_PTR ptc);
extern	void			GFL_PTC_SetLnkTexKey(NNSGfdTexKey tex_key);
extern	void			GFL_PTC_GetLnkTexKey(NNSGfdTexKey tex_key);
extern	void			GFL_PTC_SetPlttLnkTexKey(NNSGfdPlttKey pltt_key);

extern	void			GFL_PTC_GetEmitterVelocity(GFL_EMIT_PTR emit, VecFx32 *vel);
extern	void			GFL_PTC_SetEmitterVelocity(GFL_EMIT_PTR emit, const VecFx32 *vel);
extern	void			GFL_PTC_GetEmitterAxis(GFL_EMIT_PTR emit, VecFx16 *vec);
extern	fx32			GFL_PTC_GetEmitterRadius(GFL_EMIT_PTR emit);
extern	fx16			GFL_PTC_GetEmitterInitVelocityPos(GFL_EMIT_PTR emit);
extern	fx16			GFL_PTC_GetEmitterInitVelocityAxis(GFL_EMIT_PTR emit);
extern	fx16			GFL_PTC_GetEmitterBaseScale(GFL_EMIT_PTR emit);
extern	u16				GFL_PTC_GetEmitterEmissionInterval(GFL_EMIT_PTR emit);
extern	u16				GFL_PTC_GetEmitterBaseAlpha(GFL_EMIT_PTR emit);
extern	GXRgb			GFL_PTC_GetEmitterGlobalColor(GFL_EMIT_PTR emit);
extern	void			GFL_PTC_SetEmitterGenerationRatio(GFL_EMIT_PTR emit, fx32 gen_num);
extern	fx32			GFL_PTC_GetEmitterGenerationRatio(GFL_EMIT_PTR emit);
extern	void			GFL_PTC_GetEmitterPosition(GFL_EMIT_PTR emit, VecFx32 * p_pos);
extern	void			GFL_PTC_GetEmitterBasePosition(GFL_EMIT_PTR emit, VecFx32 * p_pos);

//�G�~�b�^�̈ʒu��ݒ肷��
extern	void			GFL_PTC_SetEmitterPosition(GFL_EMIT_PTR emit, const VecFx32 * p_pos);
//�G�~�b�^�̕�����ݒ肷��
extern	void			GFL_PTC_SetEmitterAxis(GFL_EMIT_PTR emit, const VecFx16 * p_vec);
//�G�~�b�^�̃X�P�[����ݒ肷��
extern	void			GFL_PTC_SetEmitterBaseScale(GFL_EMIT_PTR emit, const fx16 p_scale);

// -----------------------------------------
//
//	Field param ����֐�
//
// -----------------------------------------
extern	void			GFL_PTC_SetEmitterGravityPos(GFL_EMIT_PTR emit, VecFx16 *mag);
extern	void			GFL_PTC_GetEmitterGravityPos(GFL_EMIT_PTR emit, VecFx16 *mag);
extern	void			GFL_PTC_SetEmitterRandomMag(GFL_EMIT_PTR emit, VecFx16* mag);
extern	void			GFL_PTC_GetEmitterRandomMag(GFL_EMIT_PTR emit, VecFx16* mag);
extern	void			GFL_PTC_SetEmitterRandomIntvl(GFL_EMIT_PTR emit, u16* mag);
extern	void			GFL_PTC_GetEmitterRandomIntvl(GFL_EMIT_PTR emit, u16* mag);
extern	void			GFL_PTC_SetEmitterMagnetPos(GFL_EMIT_PTR emit, VecFx32* p_pos);
extern	void			GFL_PTC_GetEmitterMagnetPos(GFL_EMIT_PTR emit, VecFx32* p_pos);
extern	void			GFL_PTC_SetEmitterMagnetMag(GFL_EMIT_PTR emit, fx16* mag);
extern	void			GFL_PTC_GetEmitterMagnetMag(GFL_EMIT_PTR emit, fx16* mag);
extern	void			GFL_PTC_SetEmitterSpinRad(GFL_EMIT_PTR emit, u16* rad);
extern	void			GFL_PTC_GetEmitterSpinRad(GFL_EMIT_PTR emit, u16* rad);
extern	void			GFL_PTC_SetEmitterSpinAxisType(GFL_EMIT_PTR emit, u16* axis_type);
extern	void			GFL_PTC_GetEmitterSpinAxisType(GFL_EMIT_PTR emit, u16* axis_type);
extern	void			GFL_PTC_SetEmitterSimpleCollisionYPos(GFL_EMIT_PTR emit, fx32* y);
extern	void			GFL_PTC_GetEmitterSimpleCollisionYPos(GFL_EMIT_PTR emit, fx32* y);
extern	void			GFL_PTC_SetEmitterSimpleCollisionCoeffBounce(GFL_EMIT_PTR emit, fx16* coeff_bounce);
extern	void			GFL_PTC_GetEmitterSimpleCollisionCoeffBounce(GFL_EMIT_PTR emit, fx16* coeff_bounce);
extern	void			GFL_PTC_SetEmitterSimpleCollisionEventType(GFL_EMIT_PTR emit, u16* ev_type);
extern	void			GFL_PTC_GetEmitterSimpleCollisionEventType(GFL_EMIT_PTR emit, u16* ev_type);
extern	void			GFL_PTC_SetEmitterSimpleCollisionGlobal(GFL_EMIT_PTR emit, u16* global);
extern	void			GFL_PTC_GetEmitterSimpleCollisionGlobal(GFL_EMIT_PTR emit, u16* global);
extern	void			GFL_PTC_SetEmitterConvergencePos(GFL_EMIT_PTR emit, VecFx32* p_pos);
extern	void			GFL_PTC_GetEmitterConvergencePos(GFL_EMIT_PTR emit, VecFx32* p_pos);
extern	void			GFL_PTC_SetEmitterConvergenceRatio(GFL_EMIT_PTR emit, fx16* ratio);
extern	void			GFL_PTC_GetEmitterConvergenceRatio(GFL_EMIT_PTR emit, fx16* ratio);

#endif	//__PARTICLE_H__

