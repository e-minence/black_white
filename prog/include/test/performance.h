//===================================================================
/**
 * @file	performance.h
 * @brief	�p�t�H�[�}���X���[�^�[�@�f�o�b�O�p
 * @author	GAME FREAK Inc.
 * @date	08.08.01
 */
//===================================================================

#ifdef PM_DEBUG


//==============================================================================
//	�萔��`
//==============================================================================
///�p�t�H�[�}���X���[�^�[ID
typedef enum{
	PERFORMANCE_ID_MAIN,				///<���C�����[�v
	PERFORMANCE_ID_VBLANK,				///<V�u�����N
	
	PERFORMANCE_ID_USER_A,				///<���[�U�[�g�pA
	PERFORMANCE_ID_USER_VBLANK_A,		///<���[�U�[�g�pA(VBlank�������חp)
	PERFORMANCE_ID_USER_B,				///<���[�U�[�g�pB
	PERFORMANCE_ID_USER_VBLANK_B,		///<���[�U�[�g�pB(VBlank�������חp)

	PERFORMANCE_ID_MAX,
}PERFORMANCE_ID;

///���l�̕��ו\��������ID(VBLANK���׌v���͖��Ή�)
#define PERFORMANCE_NUM_PRINT_ID		(PERFORMANCE_ID_MAIN)


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern void DEBUG_PerformanceInit(void);
extern void DEBUG_PerformanceMain(void);
extern void DEBUG_PerformanceStartLine(PERFORMANCE_ID id);
extern void DEBUG_PerformanceEndLine(PERFORMANCE_ID id);
extern void DEBUG_PerformanceSetActive( BOOL isActive );


#endif //PM_DEBUG
