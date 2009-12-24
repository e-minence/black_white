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
#define WBGRIDPACK_HEADER 'BW'  //"WB"�̋t �W���O���b�h�}�b�v�p
#define WBGCROSSPACK_HEADER 'CG' //"GC" GridCross�̋t �O���b�h���̌����}�b�v�p
#define WBRANDOMPACK_HEADER 'DR'  //"RD"�̋t random�}�b�v�p

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

	u32		attr:31;          //�A�g���r���[�g�r�b�g    0-15��value 16-30��flg
	u32		tryangleType:1;   //�O�p�`�̌`�̃^�C�v  �^ = 0 ��:vecN1,�E:vecN2  �_ = 1 �E:vecN1,��:vecN2
} NormalVtxSt;

typedef struct{
    u16			width;           // �����f�[�^�̉��̌�
    u16			height;          // �����f�[�^�̏c�̌�
//	NormalVtxSt* vtxData;     // �����f�[�^���z��ŉ��~�c������ ���ݕs�v�ׁ̈A�R�����g�� 081113 kaga
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
//�T���v����
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


/// �S�����p�b�N�����t�@�C���̃w�b�_�[
//WB�O���b�h�}�b�v�p(�W��)
typedef struct {
    u16 DataID;         ////< DP3PACK_HEADER
    u16 dummy1;
    u32 nsbmdOffset;    ///< �t�@�C���̐擪����nsbmd�̏ꏊ�܂ł�OFFSET
  //  u32 nsbtxOffset;    ///< �t�@�C���̐擪����nsbtx�̏ꏊ�܂ł�OFFSET
  // u32 nsbtpOffset;    ///< �t�@�C���̐擪����nsbtp�̏ꏊ�܂ł�OFFSET
    u32 vertexOffset;   ///< �t�@�C���̐擪����@���{�A�g���r���[�g�̏ꏊ�܂ł�OFFSET
    u32 positionOffset; ///< �t�@�C���̐擪����|�W�V�����̏ꏊ�܂ł�OFFSET
    u32 endPos;         ///< �t�@�C���̐擪����|�W�V�����̍Ō�܂ł�OFFSET
} WBGridMapPackHeaderSt;

//WB�O���b�h���̌����}�b�v�p(�O���b�h�g���^)
typedef struct {
    u16 DataID;         ////< DP3PACK_HEADER
    u16 dummy1;
    u32 nsbmdOffset;    ///< �t�@�C���̐擪����nsbmd�̏ꏊ�܂ł�OFFSET
    u32 vertexOffset;   ///< �t�@�C���̐擪����@���{�A�g���r���[�g�̏ꏊ�܂ł�OFFSET
    u32 exAttrOffset;   ///< �t�@�C���̐擪����G�L�X�g���A�g���r���[�g�܂ł�OFFSET
    u32 positionOffset; ///< �t�@�C���̐擪����|�W�V�����̏ꏊ�܂ł�OFFSET
    u32 endPos;         ///< �t�@�C���̐擪����|�W�V�����̍Ō�܂ł�OFFSET
} WBGridCrossMapPackHeaderSt;





//-----------------------------------------------------------------------------
/**
 * @brief      index�Ԗڂ̖@���x�N�g���\���̃|�C���^�𓾂�
 * @param[in]  index    
 * @param[in]  LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @retval     PositionSt
 */
//-----------------------------------------------------------------------------
extern NormalVtxSt* DP3MAP_GetNormalVtxSt(const int index,const NormalVtxFormat* pNormalVtx);

//-----------------------------------------------------------------------------
/**
 * @brief       index�Ԗڂ̖@���x�N�g���𓾂�
 * @param[in]   index    
 * @param[in]   LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @param[out]  VecFx32* �Ԃ� VecFx32
 * @param[out]  VecFx32* �Ԃ� VecFx32
 * @retval      none
 */
//-----------------------------------------------------------------------------
extern void DP3MAP_GetNormalVtxPosition(const int index,const NormalVtxFormat* pNormalVtx,VecFx16* vc1 ,VecFx16* vc2);

//-----------------------------------------------------------------------------
/**
 * @brief       index�Ԗڂ̓��ϒl�𓾂�
 * @param[in]   index    
 * @param[in]   LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @param[out]  ���ςP
 * @param[out]  ���ςQ
 * @retval      none
 */
//-----------------------------------------------------------------------------
extern void DP3MAP_GetNormalVtxInnerProduct(const int index,const NormalVtxFormat* pNormalVtx,fx16* ip1 ,fx16* ip2);

//-----------------------------------------------------------------------------
/**
 * @brief       index�Ԗڂ̃A�g���r���[�gBIT�𓾂�
 * @param[in]   index    
 * @param[in]   LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @param[out]  ���ςP
 * @param[out]  ���ςQ
 * @retval      none
 */
//-----------------------------------------------------------------------------
extern u32 DP3MAP_GetNormalVtxAttrBit(const int index,const NormalVtxFormat* pNormalVtx);

//-----------------------------------------------------------------------------
/**
 * @brief       index�Ԗڂ�tryangleType�𓾂�
 * @param[in]   index    
 * @param[in]   LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @retval      �O�p�`�̌`�̃^�C�v  �_ = 0  �^ = 1
 */
//-----------------------------------------------------------------------------
extern u32 DP3MAP_GetNormalVtxTriangleType(const int index,const NormalVtxFormat* pNormalVtx);


//-----------------------------------------------------------------------------
/**
 * @brief       �^����ꂽ�ʒu�̖@���x�N�g���Ɠ��ς�Ԃ�
 * @param[in]   LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @param[in]   posInBlock*   ������W
 * @param[in]   map_width     �l�`�o�̒���
 * @param[in]   map_height    �l�`�o�̍���
 * @param[out]  nomalOut      �@���x�N�g��
 * @retval      ����
 */
//-----------------------------------------------------------------------------

extern fx32 DP3MAP_GetNormalVector( const NormalVtxFormat* pNormalVtx, const VecFx32* posInBlock,
                             const fx32 map_width, const fx32 map_height, VecFx32* normalOut );

//-----------------------------------------------------------------------------
/**
 * @brief      index�Ԗڂ̔z�u�f�[�^�\���̃|�C���^�𓾂�
 * @param[in]  index    
 * @param[in]  LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @retval     PositionSt
 */
//-----------------------------------------------------------------------------
extern PositionSt* DP3MAP_GetLayoutPositionSt(const int index,const LayoutFormat* pLayout);

//-----------------------------------------------------------------------------
/**
 * @brief      index�Ԗڂ̔z�u�𓾂�
 * @param[in]  index    
 * @param[in]  LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @param[out]  VecFx32* �Ԃ� VecFx32
 * @retval  none
 */
//-----------------------------------------------------------------------------

extern void DP3MAP_GetLayoutPosition(const int index,const LayoutFormat* pLayout,VecFx32* vc );

//-----------------------------------------------------------------------------
/**
 * @brief      index�Ԗڂ̊p�x�𓾂�
 * @param[in]  index    
 * @param[in]  LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @retval      �p�x
 */
//-----------------------------------------------------------------------------

extern u16 DP3MAP_GetLayoutRotate(const int index,const LayoutFormat* pLayout );

//-----------------------------------------------------------------------------
/**
 * @brief       index�Ԗڂ̃r���{�[�h�t���O�𓾂�
 * @param[in]   index
 * @param[in]   LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @retval      TRUE�Ȃ�r���{�[�h
 */
//-----------------------------------------------------------------------------

extern BOOL DP3MAP_GetLayoutBillboard(const int index,const LayoutFormat* pLayout );

//-----------------------------------------------------------------------------
/**
 * @brief       index�Ԗڂ̔z�u��ID�𓾂�
 * @param[in]   index
 * @param[in]   LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @retval      �z�u��ID
 */
//-----------------------------------------------------------------------------

extern u8 DP3MAP_GetLayoutResourceID(const int index,const LayoutFormat* pLayout );


#endif //_DP3FORMAT_H_
