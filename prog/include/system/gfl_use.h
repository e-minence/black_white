//=============================================================================================
/**
 * @file	gfl_use.h
 * @author	GAME FREAK inc.
 * @date	2006.12.06
 * @brief	GF���C�u�����Ăяo������
 */
//=============================================================================================

#pragma once

//gs�܂ł̃|�P�����Ŏg���Ă���gf_rand��u16�ŕԂ��Ă����̂ŁA
//�ڐA�p�ɒ�`nagihashi
#define GFUSER_RAND_PAST_MAX (0xFFFF)

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//GFL�֘A����������
extern void GFLUser_Init(void);

//GFL�֘AMain�Ăяo��
extern void GFLUser_Main(void);

//GFL�֘A�`�揈���Ăяo��
extern void GFLUser_Display(void);

//GFL�֘A�I������
extern void GFLUser_Exit(void);

//GFL�֘AHBlank���荞�ݏ���
extern void GFLUser_HIntr(void);

//HBlank��TCB�̓o�^������ǉ�
extern GFL_TCB * GFUser_HIntr_CreateTCB(GFL_TCB_FUNC * func, void * work, u32 pri);

//GFL�֘AVBlank���荞�ݏ���
extern void GFLUser_VIntr(void);

//GFL�֘AVBlank�^�C�~���O����
extern void GFLUser_VTiming(void);

//VBlank�^�C�~���O��TCB�̓o�^������ǉ�
extern GFL_TCB * GFUser_VIntr_CreateTCB(GFL_TCB_FUNC * func, void * work, u32 pri);

//VBlank�^�C�~���O��TCBSYS�̎擾
extern	GFL_TCBSYS * GFUser_VIntr_GetTCBSYS( void );

//VBlank�J�E���^�[�擾�ƃ��Z�b�g
extern void GFUser_VIntr_ResetVblankCounter( void );
extern int GFUser_VIntr_GetVblankCounter( void );

//�ėp�����������֐��ŏ���������
extern void GFUser_SetStartRandContext(GFL_STD_RandContext *context);

// �ėp�����𓾂�
extern u32 GFUser_GetPublicRand(u32 range);
extern u32 GFUser_GetPublicRand0(u32 range);


//	�A�[�J�C�uID�Ńt�@�C�������擾
extern const char * GFUser_GetFileNameByArcID(ARCID arcID);

//�A�T�[�g�̏I�������֐�(DEBUG_SD_PRINT�Ŏg�����ߒ�`���o��
extern void GFUser_AssertFinish( void );

typedef void (*GFL_USE_VINTR_FUNC)( void );
//VBlank���荞�݊֐��̓o�^
extern BOOL GFUser_SetVIntrFunc(GFL_USE_VINTR_FUNC func);
//VBlank���荞�݊֐��̉���
extern void GFUser_ResetVIntrFunc(void);

//�f�t�H���g��DS�J���T�E���h����R�[���o�b�N��o�^
extern void GFUser_SetSleepSoundFunc( void );

