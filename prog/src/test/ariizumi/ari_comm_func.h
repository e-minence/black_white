//======================================================================
/**
 *
 * @file	ari_comm_func.c
 * @brief	�ʐM�p�@�\�֐�
 * @author	ariizumi
 * @data	08.10.14
 */
//======================================================================
#ifndef ARI_COMM_FUNC_H__
#define ARI_COMM_FUNC_H__
//======================================================================
//	define
//======================================================================
    //�t�B�[���h�ł̒ʐM(�e�q)
typedef enum 
{
    FCT_PARENT,
    FCT_CHILD,
    
    FCT_MAX,
}FIELD_COMM_TYPE;

    //�t�B�[���h�̒ʐM���(�o�g�����)
typedef enum 
{
    FCM_2_SINGLE,
    FCM_2_DOUBLE,
    FCM_2_MIX,
    FCM_4_MULTI,
    FCM_4_MIX,

    FCM_MAX,
}FIELD_COMM_MODE;
    
//======================================================================
//	struct
//======================================================================
typedef struct _FIELD_COMM_DATA FIELD_COMM_DATA;

//======================================================================
//	extern
//======================================================================
extern FIELD_COMM_DATA *FieldComm_InitData( u32 heapID );
extern BOOL FieldComm_InitSystem();
extern void	FieldComm_InitParent();
extern void	FieldComm_InitChild();
extern int	FieldComm_UpdateSearchParent();



//�e��R�[���o�b�N
extern FieldComm_IsFinish_InitSystem();


#endif //ARI_COMM_FUNC_H__
