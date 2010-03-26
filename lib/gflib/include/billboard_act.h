//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		billboard_act.h
 *	@brief		�r���{�[�g�A�N�^�[�V�X�e��
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __BILLBOARD_ACT_H__
#define __BILLBOARD_ACT_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
/**
 *				�萔
 */
//=============================================================================
//-------------------------------------
///	�r���{�[�h�A�N�^�[���\�[�X�^�C�v
//=====================================
typedef enum {
	GFL_BBDACT_RESTYPE_DEFAULT = 0,	//���\�[�X�f�[�^��VRAM�]���������
	GFL_BBDACT_RESTYPE_DATACUT,		//���\�[�X�f�[�^��VRAM�]����ɔj��
	GFL_BBDACT_RESTYPE_TRANSSRC,	//VRAM�]���A�j���p���\�[�X
}GFL_BBDACT_RESTYPE;

//-------------------------------------
///	���\�[�X�]���̎��
//	�i�]���p�R�[���o�b�N�ɂēn���Ă��܂��j
//=====================================
typedef enum {
	GFL_BBDACT_TRANSTYPE_DATA = 0,	//���\�[�X�̓]����ނ��f�[�^
	GFL_BBDACT_TRANSTYPE_PLTT,		//���\�[�X�̓]����ނ��p���b�g
}GFL_BBDACT_TRANSTYPE;

//=============================================================================
/**
 *				�V�X�e����`
 */
//=============================================================================
//-------------------------------------
///	�r���{�[�h�A�N�^�[�V�X�e���s���S�^
//=====================================
typedef struct _GFL_BBDACT_SYS		GFL_BBDACT_SYS;


//-------------------------------------
///	����p�R�[���o�b�N�֐�
//=====================================
typedef void	(GFL_BBDACT_FUNC)( GFL_BBDACT_SYS*, int actID, void* work );

//-------------------------------------
///	���\�[�X�]���p�R�[���o�b�N�֐�
//  GFL_BBDACT_TRANSTYPE�������Ƃ��ēn���Ă��܂��̂�
//  ���\�[�X�ʂ̓ǂݍ��݂��������ꍇ�͎w�肵�Ă��������B
//=====================================
typedef void	(GFL_BBDACT_TRANSFUNC)( GFL_BBDACT_TRANSTYPE type, u32 dst, u32 src, u32 siz );

//=============================================================================
/**
 *				���\�[�X�p��`
 */
//=============================================================================

//-------------------------------------
///	���\�[�X�ݒ�p���
//	VRAM�]���A�j���p��
//	���\�[�X�̓e�[�u���œn���悤�ɂȂ��Ă��܂��B
//=====================================
typedef struct {
	u32					arcID;
	u32					datID;
	u8					texFmt;		//GFL_BBD_TEXFMT
	u8					texSiz;		//GFL_BBD_TEXSIZ
	u8					celSizX;
	u8					celSizY;
	GFL_BBDACT_RESTYPE	dataCut;
}GFL_BBDACT_RESDATA;
typedef const GFL_BBDACT_RESDATA*	GFL_BBDACT_RES_SETTBL;

//-------------------------------------
///	G3DRES�Ń��\�[�X�ݒ�p���
//	VRAM�]���A�j���p��
//	���\�[�X�̓e�[�u���œn���悤�ɂȂ��Ă��܂��B
//=====================================
typedef struct {
	GFL_G3D_RES			*g3dres;		
	u8					texFmt;		//GFL_BBD_TEXFMT
	u8					texSiz;		//GFL_BBD_TEXSIZ
	u8					celSizX;
	u8					celSizY;
	GFL_BBDACT_RESTYPE	dataCut;
}GFL_BBDACT_G3DRESDATA;
typedef const GFL_BBDACT_G3DRESDATA*	GFL_BBDACT_G3DRES_SETTBL;

//-------------------------------------
///	���\�[�X�ݒ�ԍ��i�P�e�[�u���P�ԍ��ł��j
//=====================================
typedef u16		GFL_BBDACT_RESUNIT_ID;

//=============================================================================
/**
 *				�A�N�^�[�p��`
 */
//=============================================================================

//-------------------------------------
///	�A�N�^�[�ݒ�p���
//	�A�N�^�[����C�ɍ쐬���邱�Ƃ��ł���悤��
//	�e�[�u���œn���悤�ɂȂ��Ă��܂��B
//=====================================
typedef struct {
	u16					resID;		//�e�[�u���̒��̉��Ԗڂ��g����
	fx16				sizX;		// 0 < sizX < FX16_ONE*8-1
	fx16				sizY;		// 0 < sizY < FX16_ONE*8-1
	u8					alpha;
	BOOL				drawEnable; 
	GFL_BBD_LIGHTMASK	lightMask;
	VecFx32				trans;
	GFL_BBDACT_FUNC*	func;	//����֐�
	void*				work;
}GFL_BBDACT_ACTDATA;
typedef const GFL_BBDACT_ACTDATA*	GFL_BBDACT_ACT_SETTBL;

//-------------------------------------
///	�A�N�^�[�ݒ�ԍ��i�P�e�[�u���P�ԍ��ł��j
//=====================================
typedef u16		GFL_BBDACT_ACTUNIT_ID;


//=============================================================================
/**
 *				�A�j���[�V�����p��`
 */
//=============================================================================
#define GFL_BBDACT_ANMFLIP_ON	(1)			//���]������
#define GFL_BBDACT_ANMFLIP_OFF	(0)			//���]�����Ȃ�
//-------------------------------------
///	�A�j���p�R�}���h
//=====================================
#define GFL_BBDACT_ANMCOM_END	(0x3fff)	//�A�j���I���R�}���h
#define GFL_BBDACT_ANMCOM_CHG	(0x3ffe)	//�A�j���V�[�P���X�ύX�R�}���h
#define GFL_BBDACT_ANMCOM_LOOP	(0x3ffd)	//�A�j���t���[�����[�v�p�R�}���h
#define GFL_BBDACT_ANMCOM_JMP	(0x3ffc)	//�A�j���t���[���ύX�p�R�}���h

//-------------------------------------
///	���[�v�p�ݒ�}�N��
//=====================================
#define GFL_BBDACT_ANMLOOPPARAM( lpCnt, frmIdx )	{ ((lpCnt&0xff)<< 8)|(frmIdx&0xff) }

//-------------------------------------
///	�A�j���[�V�����ݒ�\����
//	�A�j���[�V�����ݒ�\���̂̓A�j���؂�ւ��p�ƃR�}���h�p��
//	���ʂ̃e�[�u���ɂł���悤��union�ɂȂ��Ă��܂��B
//=====================================
typedef union {
	struct {
		u16			command:14;			//�R�}���h
		u16			dummy0:1;			//�t���b�v�r
		u16			dummy1:1;			//�t���b�v�s
		u16			param;				//�p�����[�^
	} comData;

	struct {
		u16			celIdx:14;			//�Z���i���o�[
		u16			flipS:1;			//�t���b�v�r
		u16			flipT:1;			//�t���b�v�s
		u16			wait;				//�`��E�F�C�g
	} anmData;
}GFL_BBDACT_ANM;
typedef const GFL_BBDACT_ANM**	GFL_BBDACT_ANMTBL;


//=============================================================================
/**
 *								�֐��錾
 */
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
//	�r���{�[�h�A�N�g�V�X�e���쐬 (return bbdActSys)
extern GFL_BBDACT_SYS*	GFL_BBDACT_CreateSys
	( const u16 bbdResMax, const u16 bbdActMax, GFL_BBDACT_TRANSFUNC transFunc, HEAPID heapID );
//	�r���{�[�h�A�N�g���C��
extern void	GFL_BBDACT_Main( GFL_BBDACT_SYS* bbdActSys );
//	�r���{�[�h�A�N�g�`��
extern void	GFL_BBDACT_Draw
		( GFL_BBDACT_SYS* bbdActSys, GFL_G3D_CAMERA* g3Dcamera, GFL_G3D_LIGHTSET* g3Dlightset );
//	�r���{�[�h�A�N�g�V�X�e���j��
extern void	GFL_BBDACT_DeleteSys( GFL_BBDACT_SYS* bbdActSys );
//	�r���{�[�h�V�X�e���|�C���^�擾
extern GFL_BBD_SYS*	GFL_BBDACT_GetBBDSystem( GFL_BBDACT_SYS* bbdActSys );
extern const GFL_BBD_SYS* GFL_BBDACT_GetBBDSystemConst( const GFL_BBDACT_SYS* bbdActSys );

//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h�A�N�g���\�[�X�̃Z�b�g�A�b�v�Ɣj��
 */
//------------------------------------------------------------------
//�Z�b�g�A�b�v
extern GFL_BBDACT_RESUNIT_ID GFL_BBDACT_AddResourceUnit
			( GFL_BBDACT_SYS* bbdActSys, GFL_BBDACT_RES_SETTBL resTbl, u32 resCount );

extern GFL_BBDACT_RESUNIT_ID GFL_BBDACT_AddResourceG3DResUnit
			( GFL_BBDACT_SYS* bbdActSys, GFL_BBDACT_G3DRES_SETTBL resTbl, u32 resCount );
//�j��
extern void GFL_BBDACT_RemoveResourceUnit
			( GFL_BBDACT_SYS* bbdActSys, GFL_BBDACT_RESUNIT_ID idx, u32 resCount );

//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h�A�N�g�̃Z�b�g�A�b�v�Ɣj��
 */
//------------------------------------------------------------------
//�Z�b�g�A�b�v
extern GFL_BBDACT_ACTUNIT_ID GFL_BBDACT_AddAct( GFL_BBDACT_SYS* bbdActSys, 
				GFL_BBDACT_RESUNIT_ID idx, GFL_BBDACT_ACT_SETTBL actTbl, u32 actCount );
//�j��
extern void GFL_BBDACT_RemoveAct
				( GFL_BBDACT_SYS* bbdActSys, GFL_BBDACT_ACTUNIT_ID idx, u32 actCount ); 


//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h�A�N�g�̐ݒ�A�擾�֐�
 */
//------------------------------------------------------------------
//	�r���{�[�h�A�N�g�`��t���O�ݒ�
extern void	GFL_BBDACT_SetDrawEnable( GFL_BBDACT_SYS* bbdActSys, u16 actIdx, BOOL drawEnable );
extern BOOL GFL_BBDACT_GetDrawEnable( const GFL_BBDACT_SYS* bbdActSys, u16 actIdx );
//	�r���{�[�h�A�N�g���C�g���f�t���O�ݒ�
extern void	GFL_BBDACT_SetLightEnable
				( GFL_BBDACT_SYS* bbdActSys, u16 actIdx, BOOL lightEnable, u8 lightNum );
extern BOOL GFL_BBDACT_GetLightEnable
				( const GFL_BBDACT_SYS* bbdActSys, u16 actIdx, u8 lightNum );
//	�r���{�[�h�A�N�g�A�j���ݒ�
extern void	GFL_BBDACT_SetAnimeEnable( GFL_BBDACT_SYS* bbdActSys, u16 actIdx, BOOL animeEnable );
extern BOOL GFL_BBDACT_GetAnimeEnable( const GFL_BBDACT_SYS* bbdActSys, u16 actIdx );
extern void	GFL_BBDACT_SetAnimeTable( GFL_BBDACT_SYS* bbdActSys, u16 actIdx, 
									GFL_BBDACT_ANMTBL animeTable, u16 animeTableCount );
//	�A�j���C���f�b�N�X�ݒ�E�擾
extern void	GFL_BBDACT_SetAnimeIdx( GFL_BBDACT_SYS* bbdActSys, u16 actIdx, u16 animeIdx );
extern u16 GFL_BBDACT_GetAnimeIdx( const GFL_BBDACT_SYS* bbdActSys, u16 actIdx );
extern void	GFL_BBDACT_SetAnimeIdxContinue( GFL_BBDACT_SYS* bbdActSys, u16 actIdx, u16 animeIdx );
extern void	GFL_BBDACT_SetAnimeIdxImmediate
	( GFL_BBDACT_SYS* bbdActSys, u16 actIdx, u16 animeIdx, u16 wait );
//	�A�j���t���[���C���f�b�N�X�ݒ�E�擾
extern void	GFL_BBDACT_SetAnimeFrmIdx( GFL_BBDACT_SYS* bbdActSys, u16 actIdx, u16 animeFrmIdx );
extern u16 GFL_BBDACT_GetAnimeFrmIdx( const GFL_BBDACT_SYS* bbdActSys, u16 actIdx );
inline void GFL_BBDACT_SetAnimeIdxOn( GFL_BBDACT_SYS* bbdActSys, u16 actIdx, u16 animeIdx )
{
	GFL_BBDACT_SetAnimeIdx( bbdActSys, actIdx, animeIdx );
	GFL_BBDACT_SetAnimeEnable( bbdActSys, actIdx, TRUE );
}
inline void GFL_BBDACT_SetFrmIdxOn( GFL_BBDACT_SYS* bbdActSys, u16 actIdx, u16 animeFrmIdx )
{
	GFL_BBDACT_SetAnimeFrmIdx( bbdActSys, actIdx, animeFrmIdx );
	GFL_BBDACT_SetAnimeEnable( bbdActSys, actIdx, TRUE );
}
extern BOOL	GFL_BBDACT_GetAnimeLastCommand( GFL_BBDACT_SYS* bbdActSys, u16 actIdx, u16* comm );

//	����֐��ݒ�
extern void	GFL_BBDACT_SetFunc( GFL_BBDACT_SYS* bbdActSys, u16 actIdx, GFL_BBDACT_FUNC* func );
//	�]�����\�[�X�֘A�t���i���Ƀ��\�[�X�Ƃ��ēǂݍ��܂�Ă���ꍇ�Ɏg�p�j
extern void	GFL_BBDACT_BindActTexRes( GFL_BBDACT_SYS* bbdActSys, u16 actIdx, u16 resIdx );
//	�]�����\�[�X���[�h���ݒ�i���\�[�X�Ƃ��ēǂݍ��܂�Ă��Ȃ��ꍇ�Ɏg�p�j
extern void	GFL_BBDACT_BindActTexResLoad
			( GFL_BBDACT_SYS* bbdActSys, u16 actIdx, u32 arcID, u32 datID );

//	�r���{�[�h�I�u�W�F�N�g�z��h�m�c�d�w�擾
extern int	GFL_BBDACT_GetBBDActIdxResIdx( GFL_BBDACT_SYS* bbdActSys, u16 actIdx );
//	���\�[�X�h�c�w�擾
extern u16	GFL_BBDACT_GetResIdx( GFL_BBDACT_SYS* bbdActSys, u16 actIdx );
//	�A�N�^�[�̃��\�[�X�ύX
extern void GFL_BBDACT_SetResIdx( GFL_BBDACT_SYS* bbdActSys, u16 actIdx, GFL_BBDACT_RESUNIT_ID resUnitID, u16 resIdx );
 
#ifdef __cplusplus
} /* extern "C" */
#endif


#endif		// __BILLBOARD_ACT_H__
