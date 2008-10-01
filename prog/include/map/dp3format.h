//=============================================================================
/**
 * @file	dp3format.h
 * @brief   �}�b�v�G�f�B�^�[�̏o�̓t�@�C����`
 * @author	ohno_katsunmi@gamefreak.co.jp
 * @date    2008.09.02
 */
//=============================================================================

#ifndef _DP3FORMAT_H_
#define _DP3FORMAT_H_

#define DP3PACK_HEADER (0x4433)   //"3D"���Ђ�����Ԃ���

/// �����f�[�^�擾�p
typedef struct {
	fx16	vecN1_x;        //�P�ڂ̎O�p�`�̖@���x�N�g��
	fx16	vecN1_y;
	fx16	vecN1_z;

	fx16	vecN2_x;       //�Q�ڂ̎O�p�`�̖@���x�N�g��
	fx16	vecN2_y;
	fx16	vecN2_z;

	fx32	vecN1_D;       // ax+by+cz+d =0 ��D�l
	fx32	vecN2_D;

	u32		attr:31;            //�A�g���r���[�g�r�b�g
	u32		tryangleType:1;   //�O�p�`�̌`�̃^�C�v  �_ = 0  �^ = 1
} NormalVtxSt;

typedef struct{
    u16			width;           // �����f�[�^�̉��̌�
    u16			height;          // �����f�[�^�̏c�̌�
	NormalVtxSt* vtxData;       // �����f�[�^���z��ŉ��~�c������
} NormalVtxFormat;



///�|�W�V�����f�[�^�擾

typedef struct{
    fx32	xpos;
    fx32	ypos;
    fx32	zpos;
    u16		rotate;
    u8		billboard;
    u8		resourceID;
} PositionSt;

// �|�W�V�����f�[�^�S��
typedef struct{
    u32			count;       ///< �|�W�V�����f�[�^��
	PositionSt* posData;     ///< �|�W�V�����f�[�^
} LayoutFormat;

/// �S�����p�b�N�����t�@�C���̃w�b�_�[
typedef struct {
    u16 DataID;         ////< DP3PACK_HEADER
    u16 dummy1;
    u32 nsbmdOffset;    ///< �t�@�C���̐擪����nsbmd�̏ꏊ�܂ł�OFFSET
    u32 nsbtxOffset;    ///< �t�@�C���̐擪����nsbtx�̏ꏊ�܂ł�OFFSET
    u32 nsbtpOffset;    ///< �t�@�C���̐擪����nsbtp�̏ꏊ�܂ł�OFFSET
    u32 vertexOffset;   ///< �t�@�C���̐擪����@���{�A�g���r���[�g�̏ꏊ�܂ł�OFFSET
    u32 positionOffset; ///< �t�@�C���̐擪����|�W�V�����̏ꏊ�܂ł�OFFSET
    u32 endPos;         ///< �t�@�C���̐擪����|�W�V�����̍Ō�܂ł�OFFSET
} Dp3packHeaderSt;


#endif //_DP3FORMAT_H_
