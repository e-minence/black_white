//======================================================================
/**
 * @file	sta_act_light.h
 * @brief	�X�e�[�W�p�@�X�|�b�g���C�g
 * @author	ariizumi
 * @data	09/03/11
 */
//======================================================================
#ifndef ACT_STA_LIGHT_H__
#define ACT_STA_LIGHT_H__

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================
typedef enum
{
	ALT_NONE,	//����
	ALT_CIRCLE,	//�~�`
	ALT_SHINES,	//��`
}STA_LIGHT_TYPE;
//Option���l
//�~�` val1 �~�̔��a val2 ����
//��` val1 ��̕� val2 ���̕�


//======================================================================
//	typedef struct
//======================================================================

typedef struct _STA_LIGHT_SYS STA_LIGHT_SYS;
typedef struct _STA_LIGHT_WORK STA_LIGHT_WORK;

//======================================================================
//	proto
//======================================================================

STA_LIGHT_SYS* STA_LIGHT_InitSystem( HEAPID heapId , ACTING_WORK* actWork );
void	STA_LIGHT_UpdateSystem( STA_LIGHT_SYS *work );
void	STA_LIGHT_DrawSystem( STA_LIGHT_SYS *work );
void	STA_LIGHT_ExitSystem( STA_LIGHT_SYS *work );

//�V�X�e���n

//LIGHT�P�̌n
STA_LIGHT_WORK *STA_LIGHT_CreateObject( STA_LIGHT_SYS *work , const STA_LIGHT_TYPE type );
void STA_LIGHT_DeleteObject( STA_LIGHT_SYS *work , STA_LIGHT_WORK *objWork );
void STA_LIGHT_SetPosition( STA_LIGHT_SYS *work , STA_LIGHT_WORK *lightWork , const VecFx32 *pos );
void STA_LIGHT_GetPosition( STA_LIGHT_SYS *work , STA_LIGHT_WORK *lightWork , VecFx32 *pos );
void STA_LIGHT_SetColor( STA_LIGHT_SYS *work , STA_LIGHT_WORK *lightWork , const GXRgb col , const u8 alpha );
void STA_LIGHT_GetColor( STA_LIGHT_SYS *work , STA_LIGHT_WORK *lightWork , GXRgb *col , u8 *alpha );
void STA_LIGHT_SetOptionValue( STA_LIGHT_SYS *work , STA_LIGHT_WORK *lightWork , const fx32 val1 , const fx32 val2 );
void STA_LIGHT_GetOptionValue( STA_LIGHT_SYS *work , STA_LIGHT_WORK *lightWork , fx32 *val1 , fx32 *val2 );

#endif ACT_STA_LIGHT_H__