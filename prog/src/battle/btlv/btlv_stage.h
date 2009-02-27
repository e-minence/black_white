
//============================================================================================
/**
 * @file	btlv_stage.h
 * @brief	�퓬��ʕ`��p�֐��i���~�j
 * @author	soga
 * @date	2008.11.18
 */
//============================================================================================

#ifndef	__BTLV_STAGE_H_
#define	__BTLV_STAGE_H_

enum{
	BTLV_STAGE_MINE = 0,			//���������~
	BTLV_STAGE_ENEMY,			//���葤���~
	BTLV_STAGE_MAX				//��ʂɏo�����~�̐�
};

typedef struct _BTLV_STAGE_WORK BTLV_STAGE_WORK;

extern	BTLV_STAGE_WORK	*BTLV_STAGE_Init( int index, HEAPID heapID );
extern	void			BTLV_STAGE_Exit( BTLV_STAGE_WORK *bsw );
extern	void			BTLV_STAGE_Main( BTLV_STAGE_WORK *bsw );
extern	void			BTLV_STAGE_Draw( BTLV_STAGE_WORK *bsw );

#endif	//__BTLV_STAGE_H_
