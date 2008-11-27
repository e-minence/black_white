
//============================================================================================
/**
 * @file	btl_stage.h
 * @brief	�퓬��ʕ`��p�֐��i���~�j
 * @author	soga
 * @date	2008.11.18
 */
//============================================================================================

#ifndef	__BTL_STAGE_H_
#define	__BTL_STAGE_H_

enum{
	BTL_STAGE_MINE = 0,			//���������~
	BTL_STAGE_ENEMY,			//���葤���~
	BTL_STAGE_MAX				//��ʂɏo�����~�̐�
};

typedef struct _BTL_STAGE_WORK BTL_STAGE_WORK;

extern	BTL_STAGE_WORK	*BTL_STAGE_Init( int index, HEAPID heapID );
extern	void			BTL_STAGE_Exit( BTL_STAGE_WORK *bsw );
extern	void			BTL_STAGE_Main( BTL_STAGE_WORK *bsw );
extern	void			BTL_STAGE_Draw( BTL_STAGE_WORK *bsw );

#endif	//__BTL_STAGE_H_
