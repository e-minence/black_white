//============================================================================================
/**
 * @file		box2_bgwfrm.h
 * @brief		�{�b�N�X��� BG�E�B���h�E�t���[���֘A
 * @author	Hiroyuki Nakamura
 * @date		09.10.07
 *
 *	���W���[�����FBOX2BGWFRM
 */
//============================================================================================
#pragma	once


//============================================================================================
//	�萔��`
//============================================================================================

// �莝���|�P�����t���[���f�[�^
#define	BOX2BGWFRM_PARTYPOKE_LX	( 2 )
//#define	WINFRM_PARTYPOKE_RX	( 21 )
#define	BOX2BGWFRM_PARTYPOKE_PY	( 6 )
//#define	WINFRM_PARTYPOKE_INIT_PY	( 24 )
//#define	WINFRM_PARTYPOKE_RET_PX		( 24 )
//#define	WINFRM_PARTYPOKE_RET_PY		( 15 )
#define	BOX2BGWFRM_PARTYPOKE_SX	( 11 )		// �莝���|�P�����t���[���w�T�C�Y
#define	BOX2BGWFRM_PARTYPOKE_SY	( 15 )		// �莝���|�P�����t���[���x�T�C�Y


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�t���[�����[�N�쐬�i�S�́j
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_Init( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�t���[�����[�N���
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_Exit( BOX2_APP_WORK * appwk );


//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�Ă����|�P�����v�{�^���z�u
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_TemochiButtonOn( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�{�b�N�X���X�g�v�{�^���z�u
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_BoxListButtonOn( BOX2_APP_WORK * appwk );



//extern void BOX2BGWFRM_SubDispWazaFrmOutPosSet( BGWINFRM_WORK * wk );

extern BOOL BOX2BGWFRM_PokeMenuMoveMain( BGWINFRM_WORK * wk );

extern BOOL BOX2BGWFRM_PartyPokeFrameMove( BOX2_SYS_WORK * syswk );


extern BOOL BOX2BGWFRM_CheckPartyPokeFrameRight( BGWINFRM_WORK * wk );
extern BOOL BOX2BGWFRM_CheckPartyPokeFrameLeft( BGWINFRM_WORK * wk );

extern void BOX2BGWFRM_PokeMenuOutSet( BGWINFRM_WORK * wk );

extern BOOL BOX2BGWFRM_PokeMenuPutCheck( BGWINFRM_WORK * wk );

//extern void BOX2BGWFRM_SubDispItemFrmOutSet( BGWINFRM_WORK * wk );


extern void BOX2BGWFRM_BoxMoveFrmOutSet( BGWINFRM_WORK * wk );
extern BOOL BOX2BGWFRM_CheckBoxMoveFrm( BGWINFRM_WORK * wk );



extern void BOX2BGWFRM_MarkingFrameOutSet( BGWINFRM_WORK * wk );

extern int BOX2BGWFRM_MarkingFrameMove( BOX2_SYS_WORK * syswk );

extern void BOX2BGWFRM_PartyPokeFrameInSet( BGWINFRM_WORK * wk );

extern void BOX2BGWFRM_PartyPokeFrameOutSet( BGWINFRM_WORK * wk );

extern void BOX2BGWFRM_PartyPokeFrameRightMoveSet( BGWINFRM_WORK * wk );

extern void BOX2BGWFRM_PokeMenuInSet( BGWINFRM_WORK * wk );

extern void BOX2BGWFRM_PokeMenuOff( BGWINFRM_WORK * wk );

//extern void BOX2BGWFRM_SubDispItemFrmInSet( BGWINFRM_WORK * wk );


extern void BOX2BGWFRM_PartyPokeFrameInitPutRight( BGWINFRM_WORK * wk );
extern void BOX2BGWFRM_BoxMoveFrmInSet( BGWINFRM_WORK * wk );
//extern void BOX2BGWFRM_SubDispItemFrmOutPosSet( BGWINFRM_WORK * wk );
//extern void BOX2BGWFRM_SubDispWazaFrmInSet( BOX2_APP_WORK * appwk );
extern void BOX2BGWFRM_PartyPokeFrameLeftMoveSet( BGWINFRM_WORK * wk );
//extern void BOX2BGWFRM_SubDispWazaFrmOutSet( BOX2_APP_WORK * appwk );
extern void BOX2BGWFRM_MarkingFrameInSet( BGWINFRM_WORK * wk );
extern void BOX2BGWFRM_BoxListButtonOff( BOX2_APP_WORK * appwk );
extern void BOX2BGWFRM_TemochiButtonOff( BOX2_APP_WORK * appwk );
extern void BOX2BGWFRM_PartyPokeFrameInitPutLeft( BGWINFRM_WORK * wk );
//extern void BOX2BGWFRM_PartyPokeFrameOff( BOX2_SYS_WORK * syswk );
extern void BOX2BGWFRM_PartyPokeFramePut( BGWINFRM_WORK * wk );
extern void BOX2BGWFRM_PartyPokeFramePutRight( BGWINFRM_WORK * wk );
extern void BOX2BGWFRM_PokeMenuOpenPosSet( BGWINFRM_WORK * wk );
//extern void BOX2BGWFRM_SubDispItemFrmInPosSet( BGWINFRM_WORK * wk );
//extern void BOX2BGWFRM_SubDispWazaFrmInPosSet( BOX2_APP_WORK * appwk );

//extern void BOX2BGWFRM_SubDispWinFrmMove( BOX2_SYS_WORK * syswk );

extern void BOX2BGWFRM_PutTouchBar( BGWINFRM_WORK * wk );
