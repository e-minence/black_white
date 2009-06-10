//============================================================================================
/**
 * @file	camera_adjust_view.h
 * @brief	�J���������p�r���[���[
 * @author	
 * @date	
 */
//============================================================================================
#ifndef __DEBUG_CAMADJUST_H__
#define __DEBUG_CAMADJUST_H__

//
//	�g�p�̍ۂɂ�graphic�t�H���_����camera_adjust_view.narc�����[�g�ɔz�u���Ă�������
//	�i"camera_adjust_view.narc"�p�X�w��Ŏ擾�ł���ꏊ�j
//
//	�g�pBG�ʂ̃R���g���[�����W�X�^�͌Ăяo�����Őݒ肳�ꂽ��ԂɈˑ����܂��B
//	
typedef struct _GFL_CAMADJUST GFL_CAMADJUST;

//�@�ݒ��`
typedef struct {
	u32					cancelKey;		//���f�L�[����g���K(0�ŃL�[���f�s��)

	GFL_DISPUT_BGID		bgID;			//�g�pBGVRAMID
	GFL_DISPUT_PALID	bgPalID;		//�g�pBG�p���b�gID

	u16*				pCameraAngleV;
	u16*				pCameraAngleH;
	fx32*				pCameraLength;
}GFL_CAMADJUST_SETUP;

//============================================================================================
/**
 *
 * @brief	�V�X�e���N��&�I��
 *	
 */
//============================================================================================
extern GFL_CAMADJUST*	GFL_CAMADJUST_Create( const GFL_CAMADJUST_SETUP* setup, HEAPID heapID );
extern void				GFL_CAMADJUST_Delete( GFL_CAMADJUST* gflCamAdjust );
extern BOOL				GFL_CAMADJUST_Main( GFL_CAMADJUST* gflCamAdjust );	//FALSE�ŏI��

extern void	GFL_CAMADJUST_SetCameraParam( GFL_CAMADJUST* gflCamAdjust, 
																		u16* pAngleV, u16* pAngleH, 
																		fx32* pLength, u16* pFovy, fx32* pFar );
extern void	GFL_CAMADJUST_SetWipeParam( GFL_CAMADJUST* gflCamAdjust, fx32* pWipeSize );

#endif

