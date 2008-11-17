//======================================================================
/**
 * @file	field_comm_func.c
 * @brief	�t�B�[���h�ʐM�@�ʐM�@�\��
 * @author	ariizumi
 * @data	08/11/13
 */
//======================================================================

#ifndef FIELD_COMM_FUNC_H__
#define FIELD_COMM_FUNC_H__

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================
typedef struct _FIELD_COMM_FUNC FIELD_COMM_FUNC;

//======================================================================
//	proto
//======================================================================

extern FIELD_COMM_FUNC* FieldCommFunc_InitSystem( HEAPID heapID );
extern void	FieldCommFunc_TermSystem( FIELD_COMM_FUNC *commFunc );
extern void	FieldCommFunc_InitCommSystem( FIELD_COMM_FUNC *commFunc );
extern void	FieldCommFunc_TermCommSystem( FIELD_COMM_FUNC *commFunc );
extern void	FieldCommFunc_UpdateSystem( FIELD_COMM_FUNC *commFunc );

extern void	FieldCommFunc_StartCommWait( FIELD_COMM_FUNC *commFunc );
extern void	FieldCommFunc_StartCommSearch( FIELD_COMM_FUNC *commFunc );
extern void	FieldCommFunc_StartCommChangeover( FIELD_COMM_FUNC *commFunc );

//���M�p�֐�
extern void	FieldCommFunc_Send_SelfData( FIELD_COMM_FUNC *commFunc );

//�e��`�F�b�N�֐�
extern const BOOL FieldCommFunc_IsFinishInitCommSystem( FIELD_COMM_FUNC *commFunc );
extern const BOOL FieldCommFunc_IsFinishTermCommSystem( FIELD_COMM_FUNC *commFunc );
extern const int	FieldCommFunc_GetMemberNum( FIELD_COMM_FUNC *commFunc );
extern const int	FieldCommFunc_GetSelfIndex( FIELD_COMM_FUNC *commFunc );



#endif //FIELD_COMM_FUNC_H__

