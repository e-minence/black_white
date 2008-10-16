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
extern void FieldComm_TermSystem();
extern void	FieldComm_InitParent();
extern void	FieldComm_InitChild();
extern u8	FieldComm_UpdateSearchParent();
extern BOOL	FieldComm_GetSearchParentName( const u8 idx , STRBUF *name );
extern BOOL	FieldComm_IsValidParentData( const u8 idx );
extern void	FieldComm_ConnectParent( u8 idx );
extern u8	FieldComm_GetMemberNum();
extern BOOL	FieldComm_GetMemberName( const u8 idx , STRBUF *name );

//����M�֐�
extern void	FieldComm_SendSelfData();

//�`�F�b�N�֐�
extern BOOL	FieldComm_IsFinish_ConnectParent();
extern BOOL	FieldComm_IsFinish_InitSystem();
extern BOOL	FieldComm_IsErro();
extern BOOL	FieldComm_IsDutyMemberData();
extern void	FieldComm_ResetDutyMemberData();

//�e��R�[���o�b�N


#endif //ARI_COMM_FUNC_H__
