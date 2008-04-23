//=============================================================================================
/**
 * @file	gfl_use.h
 * @author	GAME FREAK inc.
 * @date	2006.12.06
 * @brief	GF���C�u�����Ăяo������
 */
//=============================================================================================

#ifndef	__GFL_USE_H__
#define	__GFL_USE_H__

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

//VBlank��TCB�̓o�^������ǉ�
extern GFL_TCB * GFUser_VIntr_CreateTCB(GFL_TCB_FUNC * func, void * work, u32 pri);

//VBlank�J�E���^�[�擾�ƃ��Z�b�g
extern void GFUser_VIntr_ResetVblankCounter( void );
extern int GFUser_VIntr_GetVblankCounter( void );

// �ėp�����𓾂�
extern u32 GFUser_GetPublicRand(u32 max);

#endif	__GFL_USE_H__
