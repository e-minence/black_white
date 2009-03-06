//======================================================================
/**
 * @file	sta_act_obj.h
 * @brief	�X�e�[�W�p�@�I�u�W�F�N�g
 * @author	ariizumi
 * @data	09/03/06
 */
//======================================================================
#ifndef STA_ACT_OBJ_H__
#define STA_ACT_OBJ_H__

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================

typedef struct _STA_OBJ_SYS STA_OBJ_SYS;
typedef struct _STA_OBJ_WORK STA_OBJ_WORK;

//======================================================================
//	proto
//======================================================================

STA_OBJ_SYS* STA_OBJ_InitSystem( HEAPID heapId , GFL_BBD_SYS* bbdSys );
//�|�P�����̕`��̌�ɃA�C�e���̍X�V������K�v������̂ŕʊ֐���
void	STA_OBJ_UpdateSystem( STA_OBJ_SYS *work );
void	STA_OBJ_UpdateSystem_Item( STA_OBJ_SYS *work );
void	STA_OBJ_DrawSystem( STA_OBJ_SYS *work );
void	STA_OBJ_ExitSystem( STA_OBJ_SYS *work );

//�V�X�e���n
void	STA_OBJ_System_SetScrollOffset( STA_OBJ_SYS *work , const u16 scrOfs );

//�|�P�P�̌n
STA_OBJ_WORK *STA_OBJ_AddObject( STA_OBJ_SYS *work , const u16 itemId );
void STA_OBJ_SetPosition( STA_OBJ_SYS *work , STA_OBJ_WORK *pokeWork , const VecFx32 *pos );

#endif STA_ACT_OBJ_H__