
//============================================================================================
/**
 * @file	btlv_field.h
 * @brief	�퓬��ʕ`��p�֐��i�t�B�[���h�j
 * @author	soga
 * @date	2008.11.18
 */
//============================================================================================

#ifndef	__BTLV_FIELD_H_
#define	__BTLV_FIELD_H_

enum{
	BTLV_FIELD_GROUND = 0,		//�n��
	BTLV_FIELD_BG,				//�w�i
//	BTLV_FIELD_SHADOW_LINE,		//�e��
	BTLV_FIELD_MAX				//�t�B�[���h�\��OBJ��
};

#define	BTLV_FIELD_SHADOW_LINE_START_POS	( FX32_ONE * 12 )	//�e���̏����ʒu
#define	BTLV_FIELD_SHADOW_LINE_MOVE_FRAME	( 32 )				//�e���̈ړ��t���[����
#define	BTLV_FIELD_SHADOW_LINE_MOVE_WAIT	( 2 )				//�e���̈ړ��E�F�C�g

typedef struct _BTLV_FIELD_WORK BTLV_FIELD_WORK;

extern	BTLV_FIELD_WORK	*BTLV_FIELD_Init( GFL_TCBSYS *tcb_sys, int index, HEAPID heapID );
extern	void			BTLV_FIELD_Exit( BTLV_FIELD_WORK *bsw );
extern	void			BTLV_FIELD_Main( BTLV_FIELD_WORK *bsw );
extern	void			BTLV_FIELD_Draw( BTLV_FIELD_WORK *bsw );

#endif	//__BTLV_FIELD_H_
