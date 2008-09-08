//============================================================================================
/**
 * @file	func_pmcustom.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "..\g3d_map.h"
#include "map\dp3format.h"

#include "func_pmcustom_file.h"
//============================================================================================
/**
 *
 *
 *
 * @brief	�|�P�����Ŏg�p���ꂽ�f�[�^����������֐��S
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�^�錾
 */
//------------------------------------------------------------------
//XZ���_�f�[�^
typedef struct XZ_VERTEX_tag
{
	fx32 X;
	fx32 Z;
}XZ_VERTEX;

typedef struct POLYGON_DATA_tag{
	u16 vtx_idx0;
	u16 vtx_idx1;
	u16	nrm_idx;	//�@���f�[�^�z��(���K���ς�)�ւ̃C���f�b�N�XNo
	u16 d_idx;		//�c�l�z��ւ̃C���f�b�N�XNo
}POLYGON_DATA;

typedef struct HEIGHT_ARRAY_tag
{
	fx32 Height;
	int Prev;
	int Next;
}HEIGHT_ARRAY;

typedef struct LINE_DATA_tag
{
	fx32 LineZ;
	u16 EntryNum;
	u16 ParamIndex;
}LINE_DATA;

typedef struct MAP_HEIGHT_INFO_tag{
	XZ_VERTEX		*VertexArray;
	VecFx32			*NormalArray;
	fx32			*DvalArray;
	POLYGON_DATA	*PolygonData;
	LINE_DATA		*LineDataTbl;
	u16				*PolyIDList;

	int				LineNum;
}MAP_HEIGHT_INFO;

typedef struct {
	u32 signature;
	u16 VtxNum;
	u16 NrmNum;
	u16 DNum;
	u16 PolygonNum;
	u16 LineNum;
	u16 PolyIDListNum;
}HEIGHT_DATA_HEADER;

//------------------------------------------------------------------
//�w�b�_�[���
typedef struct { 
	u32		attrSize;
	u32		objSize;
	u32		mapSize;
	u32		heightSize;
}MAP_FILE_HEADER;

//�T�E���h�w�b�_�[���
typedef struct {
	u16		signature;
	u16		size;
}SOUND_DATA_HEADER;

//�f�[�^�擾�p���
typedef struct { 
	MAP_HEIGHT_INFO heightInfo;	//�����擾�p

}MAP_DATA_INFO;

//============================================================================================
/**
 *
 *
 *
 * @brief	�}�b�v�f�[�^�ǂݍ���
 *				���V�[�P���X0�Ԃ�IDLING 1�Ԃ̓X�^�[�g�h�c�w�ŌŒ�
 *
 *
 */
//============================================================================================
enum {
	FILE_LOAD_START = LOAD_START,
	FILE_LOAD,
	FILE_HEADER_SET,
	RND_CREATE,
};

BOOL LoadMapData_PMcustomFile( GFL_G3D_MAP* g3Dmap )
{
	GFL_G3D_MAP_LOAD_STATUS* ldst;

	GFL_G3D_MAP_GetLoadStatusPointer( g3Dmap, &ldst );

	switch( ldst->seq ){

	case FILE_LOAD_START:
		GFL_G3D_MAP_ResetLoadStatus(g3Dmap);

		//�������擪�ɂ̓f�[�^�擾�p����z�u���邽�߁A�ǂݍ��݃|�C���^�����炷
		ldst->mOffs += sizeof(MAP_DATA_INFO);

		//���f���f�[�^���[�h�J�n
		{
			u32		datID;
			GFL_G3D_MAP_GetLoadDatID( g3Dmap, &datID );
			GFL_G3D_MAP_StartFileLoad( g3Dmap, datID );
		}
		ldst->seq = FILE_LOAD;
		break;

	case FILE_LOAD:
		if( GFL_G3D_MAP_ContinueFileLoad(g3Dmap) == FALSE ){
			ldst->seq = FILE_HEADER_SET;
		}
		break;

	case FILE_HEADER_SET:
		{
			void*				mem;
			MAP_FILE_HEADER*	header;
			MAP_DATA_INFO*		mapdataInfo;
			SOUND_DATA_HEADER*	soundHeader;
			HEIGHT_DATA_HEADER*	heightHeader;
			u32					dataOffset = 0;

			GFL_G3D_MAP_GetLoadMemoryPointer( g3Dmap, &mem );

			//�f�[�^�擾�p���ݒ�
			mapdataInfo = (MAP_DATA_INFO*)mem;
			dataOffset += sizeof(MAP_DATA_INFO);
			
			//�t�@�C�����e�w�b�_�[�ݒ�
			header = (MAP_FILE_HEADER*)((u32)mem + dataOffset);
			dataOffset += sizeof(MAP_FILE_HEADER);

			//�����f�[�^�ݒ�
			soundHeader = (SOUND_DATA_HEADER*)((u32)mem + dataOffset);
			dataOffset += (sizeof(SOUND_DATA_HEADER) + soundHeader->size);

			//�A�g���r���[�g���\�[�X�ݒ�
			heightHeader = (HEIGHT_DATA_HEADER*)((u32)mem + dataOffset);
			{
				//�A�g���r���[�g�f�[�^�擾�p���ݒ�
				MAP_HEIGHT_INFO* heightInfo = &mapdataInfo->heightInfo;
				u32 hdataOffset = 0;

				hdataOffset += sizeof(HEIGHT_DATA_HEADER);

				//���C������
				heightInfo->LineNum = heightHeader->LineNum;
				//���_�f�[�^�|�C���^�Z�o
				heightInfo->VertexArray = (XZ_VERTEX*)((u32)heightHeader + hdataOffset);
				hdataOffset += sizeof(XZ_VERTEX) * heightHeader->VtxNum;
				//�@���f�[�^�|�C���^�Z�o
				heightInfo->NormalArray = (VecFx32*)((u32)heightHeader + hdataOffset);
				hdataOffset += sizeof(VecFx32) * heightHeader->NrmNum;
				//�c�l�f�[�^�|�C���^�Z�o
				heightInfo->DvalArray = (fx32*)((u32)heightHeader + hdataOffset);
				hdataOffset += sizeof(fx32) * heightHeader->DNum;
				//�|���S���f�[�^�|�C���^�Z�o
				heightInfo->PolygonData = (POLYGON_DATA*)((u32)heightHeader + hdataOffset);
				hdataOffset += sizeof(POLYGON_DATA) * heightHeader->PolygonNum;
				//���C���e�[�u���f�[�^�|�C���^�Z�o
				heightInfo->LineDataTbl = (LINE_DATA*)((u32)heightHeader + hdataOffset);
				hdataOffset += sizeof(LINE_DATA) * heightHeader->LineNum;
				//���C�����|���S���h�c�G���g���f�[�^�|�C���^�Z�o
				heightInfo->PolyIDList = (u16*)((u32)heightHeader + hdataOffset);
				hdataOffset += sizeof(u16) * heightHeader->PolyIDListNum;
			}
			dataOffset += header->attrSize;

			//�z�u�I�u�W�F�N�g�ݒ�
			dataOffset += header->objSize;
			//���f�����\�[�X�ݒ�
			GFL_G3D_MAP_CreateResourceMdl(g3Dmap, (void*)((u32)mem + dataOffset));
			dataOffset += header->mapSize;

			ldst->seq = RND_CREATE;
		}
		break;

	case RND_CREATE:
		//�����_�[�쐬
		GFL_G3D_MAP_MakeRenderObj( g3Dmap );

		ldst->mdlLoaded = TRUE;
		ldst->attrLoaded = TRUE;

		ldst->seq = LOAD_IDLING;
		return FALSE;
		break;
	}
	return TRUE;
}

//============================================================================================
/**
 *
 *
 *
 * @brief	�R�c�}�b�v���擾
 *
 *
 *
 */
BOOL GetHeightForBlock( const fx32 inX, const fx32 inZ, MAP_HEIGHT_INFO* inMap3DInfo );
//============================================================================================
void GetAttr_PMcustomFile( GFL_G3D_MAP_ATTRINFO* attrInfo, const void* mapodata,
					const VecFx32* posInBlock, const fx32 map_width, const fx32 map_height )
{
	VEC_Fx16Set( &attrInfo->mapAttr[0].vecN, 0, FX16_ONE, 0 );
	attrInfo->mapAttr[0].attr = 0;
	attrInfo->mapAttr[0].height = 0;

	attrInfo->mapAttrCount = 1;
}











#define LINE_ONE_DATA	(5)		//2�o�C�g�f�[�^��5��10�o�C�g
#define Z_VAL_OFFSET_L(data_idx)	( data_idx*LINE_ONE_DATA+1 )
#define Z_VAL_OFFSET_H(data_idx)	( data_idx*LINE_ONE_DATA+2 )

#define LINE_Z_VAL(list,idx)	( (list[Z_VAL_OFFSET_H(data_idx)]<<16) | list[Z_VAL_OFFSET_L(data_idx)] )

//------------------------------------------------------------------
/**
 *	�l�p���O����
*/
//------------------------------------------------------------------
static BOOL CheckRectIO
	( const XZ_VERTEX *inVtx1, const XZ_VERTEX *inVtx2, const XZ_VERTEX * inTarget )
{
	const fx32 *small_x, *big_x,*small_z,*big_z;
	if (inVtx1->X <= inVtx2->X){
		small_x = &inVtx1->X;
		big_x = &inVtx2->X;
	}else{
		small_x = &inVtx2->X;
		big_x = &inVtx1->X;
	}

	if (inVtx1->Z <= inVtx2->Z){
		small_z = &inVtx1->Z;
		big_z = &inVtx2->Z;
	}else{
		small_z = &inVtx2->Z;
		big_z = &inVtx1->Z;
	}
		
	if ( ( (*small_x <= inTarget->X)&&(inTarget->X <= *big_x) )&&
			( (*small_z <= inTarget->Z)&&(inTarget->Z <= *big_z) ) ){
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------
/**
 *	�|���S���C���f�b�N�X����|���S�����_�f�[�^���擾
 *	@param	inMAp3DInfo		�}�b�v�������		
 *	@param	inIdx			�|���S���f�[�^�C���f�b�N�X
 *	@param	outVertex		���_�f�[�^�i�[��|�C���^
 *	
 *	@retval	none
*/
//------------------------------------------------------------------
static void GetPolygonVertex(MAP_HEIGHT_INFO* inMap3DInfo, u16 inIdx, XZ_VERTEX *outVertex)
{
	outVertex[0] = inMap3DInfo->VertexArray[inMap3DInfo->PolygonData[inIdx].vtx_idx0];
	outVertex[1] = inMap3DInfo->VertexArray[inMap3DInfo->PolygonData[inIdx].vtx_idx1];
}

//------------------------------------------------------------------
/**
 *	�|���S���C���f�b�N�X����|���S���̖@�����擾
 *	@param	inMAp3DInfo		�}�b�v�������		
 *	@param	inIdx			�|���S���f�[�^�C���f�b�N�X
 *	@param	outVertex		�@���f�[�^�i�[��|�C���^
 *	
 *	@retval	none
*/
//------------------------------------------------------------------
static void GetPolygonNrm(MAP_HEIGHT_INFO* inMap3DInfo, u16 inIdx, VecFx32 *outVertex)
{
	*outVertex = inMap3DInfo->NormalArray[inMap3DInfo->PolygonData[inIdx].nrm_idx];
}

//------------------------------------------------------------------
/**
 *	�|���S���C���f�b�N�X����D�l���擾
 *	@param	inMAp3DInfo		�}�b�v�������		
 *	@param	inIdx			�|���S���f�[�^�C���f�b�N�X
 *	@param	outD		D�f�[�^�i�[��|�C���^
 *	
 *	@retval	none
*/
//------------------------------------------------------------------
static void GetPolygonD(MAP_HEIGHT_INFO* inMap3DInfo, u16 inIdx, fx32 *outD)
{
	*outD = inMap3DInfo->DvalArray[inMap3DInfo->PolygonData[inIdx].d_idx];
}

//------------------------------------------------------------------
//------------------------------------------------------------------
/**
�\�[�g�p�z��̏�����
*/
static void InitArray(HEIGHT_ARRAY *outArray)
{
	int i;
	for(i=0;i<GFL_G3D_MAP_ATTR_GETMAX;i++){
		outArray[i].Height = 0;
		outArray[i].Prev = -1;
		outArray[i].Next = -1;
	}
}

/**
 �����z��f�[�^�̃\�[�g(����)
*/
static void SortArray(int inCount, HEIGHT_ARRAY *ioArray)
{
	int i;
	int prev;
	for(i=0;i<=inCount-1;i++){
		if (ioArray[i].Height<ioArray[inCount].Height){
			prev = ioArray[i].Prev;
			ioArray[i].Prev = inCount;
			ioArray[inCount].Next = i;
			ioArray[inCount].Prev = prev;
			if (prev>=0){
				ioArray[prev].Next = inCount;
			}
			//FirstIdx = inCount;
			return;
		}else{
			ioArray[inCount].Prev = i;
			ioArray[i].Next = inCount;
		}
	}
	if (inCount>0){
		ioArray[inCount].Next = ioArray[i].Next;
		ioArray[i].Next = inCount;
		ioArray[inCount].Prev = i;
	}
	return;
}

//------------------------------------------------------------------
/**
 *�@Z�\�[�g����Ă���f�[�^��2���T��
 *	@param	inDataList		�f�[�^���X�g		
 *	@param	inDataSize		�f�[�^�T�C�Y
 *	@param	inZ_Val			Z�l
 *	@param	outIndex		�f�[�^�C���f�b�N�X
 *	
 *	@retval	BOOL	TRUE:�f�[�^����@FALSE:�f�[�^����
*/
//------------------------------------------------------------------
static BOOL	BinSearch
		( const LINE_DATA *inDataList, const u16 inDataSize, const fx32 inZ_Val, u16 *outIndex)
{
	int min,max; 
	u32 data_idx;
	fx32 z_val;
	if (inDataSize == 0){
		OS_Printf("Z�\�[�g�f�[�^������܂���\n");
		return FALSE;//�f�[�^�Ȃ��Ȃ̂ŁA�T���I��
	}else if (inDataSize == 1){
		*outIndex = 0;//�T���I��
		return TRUE;
	}

	min = 0;
	max = inDataSize-1;
	data_idx = max/2;
	
	do{
///OS_Printf("min_max:%d,%d\n",min,max);
		z_val = inDataList[data_idx].LineZ;
///OS_Printf("%d z_val:%x\n",data_idx,z_val);
		if (z_val>inZ_Val){	//�T���p��
			if (max-1 > min){
				max = data_idx;
				data_idx = (min+max)/2;
			}else{				//�T���I��
				*outIndex = data_idx;
				return TRUE;
			}
		}else{					//�T���p��
			if (min+1 < max){
				min = data_idx;
				data_idx = (min+max)/2;
			}else{				//�T���I��
				*outIndex = data_idx+1;
				return TRUE;
			}
		}
	}while(1);

	return FALSE;
}

//------------------------------------------------------------------
/**
 *	�����擾
 *
 *	@param	mode			�����擾���[�h
 *	@param	inX				���߂���������X���W
 *	@param	inZ				���߂���������Z���W
 *	@param	inMap3DInfo		�������
 *	
 *	@retval	BOOL			TRUE:�������擾�ł���	FALSE:�������擾�ł��Ȃ�����
*/
//------------------------------------------------------------------
BOOL GetHeightForBlock( const fx32 inX, const fx32 inZ, MAP_HEIGHT_INFO* inMap3DInfo )
{
	XZ_VERTEX vertex[2];
	XZ_VERTEX target;
	VecFx32 nrm;
	BOOL result;
	u16 i,pol_index;
	fx32 d,y;
	int pol_count;
	u32 line_num,tbl_idx,tbl_x_idx,tbl_z_idx;
	HEIGHT_ARRAY height_array[GFL_G3D_MAP_ATTR_GETMAX];
	fx32 tbl_x,tbl_z;

	u32 offset_idx;

	u16 line_data_idx;
	u16 line_index;
	u16 polygon_num;

	const LINE_DATA *line_list;

	result = FALSE;
	//ZX���ʂɎˉe
	target.X = inX;
	target.Z = inZ;
	pol_count = 0;
	InitArray(height_array);
	
	line_num = inMap3DInfo->LineNum;
	
	line_list = inMap3DInfo->LineDataTbl;
	//���C���f�[�^��2���T�����āA�����������Ă���|���S�������݂��郉�C�������o
	if ( BinSearch(line_list,line_num,target.Z,&line_index)==FALSE){
		return FALSE;
	}

	polygon_num = line_list[line_index].EntryNum;
	offset_idx = line_list[line_index].ParamIndex;

	//�����擾�p�O���b�h�����C���f�[�^�ɓo�^����Ă���f�[�^�����[�v���āA
	//�����������Ă���|���S�������o
	for(i=0;i<polygon_num;i++){
		pol_index = inMap3DInfo->PolyIDList[offset_idx+i];

		//���_�f�[�^�擾
		GetPolygonVertex(inMap3DInfo,pol_index, vertex);
		//�l�p�`�̓��O����
		result = CheckRectIO( &vertex[0], &vertex[1], &target);
		if (result == TRUE){//�l�p�`��
			//�@���擾
			GetPolygonNrm(inMap3DInfo,pol_index, &nrm);
			//���ʂ̕������ɕK�v�ȃf�[�^�i���_��ʂ镽�ʂ���̃I�t�Z�b�g�j���擾
			GetPolygonD(inMap3DInfo,pol_index, &d);
			//���ʂ̕�������荂���擾
			y = -(FX_Mul(nrm.x, target.X)+FX_Mul(nrm.z, target.Z)+d);
			y = FX_Div(y, nrm.y);
			height_array[pol_count].Height = y;
			//�z��̃\�[�g�i���s�ł͋@�\���Ă܂���B�Ή��\��j<<���g�p�ɂ��܂����B20060801
			///SortArray(pol_count,height_array);
			
			pol_count++;
			if (pol_count >= GFL_G3D_MAP_ATTR_GETMAX){
				break;
			}
		}
	}
	return FALSE;
}


