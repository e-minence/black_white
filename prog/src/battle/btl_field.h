
//============================================================================================
/**
 * @file	btl_field.h
 * @brief	�퓬��ʕ`��p�֐��i�t�B�[���h�j
 * @author	soga
 * @date	2008.11.18
 */
//============================================================================================

#ifndef	__BTL_FIELD_H_
#define	__BTL_FIELD_H_

enum{
	BTL_FIELD_GROUND = 0,		//�n��
	BTL_FIELD_BG,				//�w�i
	BTL_FIELD_SHADOW_LINE,		//�e��
	BTL_FIELD_MAX				//�t�B�[���h�\��OBJ��
};

#define	BTL_FIELD_SHADOW_LINE_START_POS		( FX32_ONE * 12 )	//�e���̏����ʒu
#define	BTL_FIELD_SHADOW_LINE_MOVE_FRAME	( 32 )		//�e���̈ړ��t���[����
#define	BTL_FIELD_SHADOW_LINE_MOVE_WAIT		( 2 )		//�e���̈ړ��E�F�C�g

typedef struct _BTL_FIELD_WORK BTL_FIELD_WORK;

extern	BTL_FIELD_WORK	*BTL_FIELD_Init( GFL_TCBSYS *tcb_sys, int index, HEAPID heapID );
extern	void			BTL_FIELD_Exit( BTL_FIELD_WORK *bsw );
extern	void			BTL_FIELD_Main( BTL_FIELD_WORK *bsw );
extern	void			BTL_FIELD_Draw( BTL_FIELD_WORK *bsw );

#endif	//__BTL_FIELD_H_
