//============================================================================================
/**
 * @file	func_pmcustom.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "map\dp3format.h"

#include "field_func_pmcustom_file.h"
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
typedef struct {
	fx32 x;
	fx32 z;
}XZ_VERTEX;

typedef struct {
	u16 vtx_idx0;
	u16 vtx_idx1;
	u16	nrm_idx;	//�@���f�[�^�z��(���K���ς�)�ւ̃C���f�b�N�XNo
	u16 d_idx;		//�c�l�z��ւ̃C���f�b�N�XNo
}POLYGON_DATA;

typedef struct {
	fx32	lineZ;
	u16		entryNum;
	u16		paramIndex;
}LINE_DATA;

typedef struct {
	XZ_VERTEX		*vertexArray;
	VecFx32			*normalArray;
	fx32			*valDArray;
	POLYGON_DATA	*polygonData;
	LINE_DATA		*lineDataTbl;
	u16				*polyIDList;

	int				lineNum;
}MAP_HEIGHT_INFO;

typedef struct {
	u32 signature;
	u16 vtxNum;
	u16 nrmNum;
	u16 valDNum;
	u16 polygonNum;
	u16 lineNum;
	u16 polyIDListNum;
}HEIGHT_DATA_HEADER;

typedef	struct	{
	int	id;			//�f�[�^�h�c

	VecFx32  global;	//�O���[�o�����W
	VecFx32  rotate;	//��]
	VecFx32  scale;		//�X�P�[��

	int		dummy[2];	//
}MAP3D_OBJECT_ST;

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
	u32				attrAdrs;	//�A�g���r���[�g�擾�p
	MAP_HEIGHT_INFO heightInfo;	//�����擾�p

}MAP_DATA_INFO;

#define GRID_SIZE (16)
#define GRID_DATASIZE (sizeof(u16))
#define	GRIDBLOCKW	(32)
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
	FILE_LOAD_START = GFL_G3D_MAP_LOAD_START,
	FILE_LOAD,
	FILE_HEADER_SET,
	RND_CREATE,
};

BOOL FieldLoadMapData_PMcustomFile( GFL_G3D_MAP* g3Dmap )
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
			mapdataInfo->attrAdrs = (u32)mem + dataOffset;
			dataOffset += header->attrSize;

			//�z�u�I�u�W�F�N�g�ݒ�
			{
				MAP3D_OBJECT_ST* objStatus = (MAP3D_OBJECT_ST*)((u32)mem + dataOffset);
				GFL_G3D_MAP_GLOBALOBJ_ST status;
				int i, count = header->objSize/sizeof(MAP3D_OBJECT_ST);
				u32 id;

				for( i=0; i<count; i++ ){
					if( GFL_G3D_MAP_GetGlobalObjectID( g3Dmap, objStatus[i].id, &id ) == TRUE ){
						status.id = id;
						status.trans = objStatus[i].global;
						status.rotate = 0;
						GFL_G3D_MAP_ResistGlobalObj( g3Dmap, &status, i );
					} else {
						OS_Printf("cannot exchange ID = %x\n", objStatus[i].id );
					}
				}
			}
			dataOffset += header->objSize;
			//���f�����\�[�X�ݒ�
			GFL_G3D_MAP_CreateResourceMdl(g3Dmap, (void*)((u32)mem + dataOffset));
			dataOffset += header->mapSize;

			//�������\�[�X�ݒ�
			heightHeader = (HEIGHT_DATA_HEADER*)((u32)mem + dataOffset);
			{
				//�����f�[�^�擾�p���ݒ�
				MAP_HEIGHT_INFO* heightInfo = &mapdataInfo->heightInfo;
				u32 hdataOffset = 0;

				hdataOffset += sizeof(HEIGHT_DATA_HEADER);

				//���C������
				heightInfo->lineNum = heightHeader->lineNum;
				//���_�f�[�^�|�C���^�Z�o
				heightInfo->vertexArray = (XZ_VERTEX*)((u32)heightHeader + hdataOffset);
				hdataOffset += sizeof(XZ_VERTEX) * heightHeader->vtxNum;
				//�@���f�[�^�|�C���^�Z�o
				heightInfo->normalArray = (VecFx32*)((u32)heightHeader + hdataOffset);
				hdataOffset += sizeof(VecFx32) * heightHeader->nrmNum;
				//�c�l�f�[�^�|�C���^�Z�o
				heightInfo->valDArray = (fx32*)((u32)heightHeader + hdataOffset);
				hdataOffset += sizeof(fx32) * heightHeader->valDNum;
				//�|���S���f�[�^�|�C���^�Z�o
				heightInfo->polygonData = (POLYGON_DATA*)((u32)heightHeader + hdataOffset);
				hdataOffset += sizeof(POLYGON_DATA) * heightHeader->polygonNum;
				//���C���e�[�u���f�[�^�|�C���^�Z�o
				heightInfo->lineDataTbl = (LINE_DATA*)((u32)heightHeader + hdataOffset);
				hdataOffset += sizeof(LINE_DATA) * heightHeader->lineNum;
				//���C�����|���S���h�c�G���g���f�[�^�|�C���^�Z�o
				heightInfo->polyIDList = (u16*)((u32)heightHeader + hdataOffset);
				hdataOffset += sizeof(u16) * heightHeader->polyIDListNum;
			}
			ldst->seq = RND_CREATE;
		}
		break;

	case RND_CREATE:
		//�����_�[�쐬
		GFL_G3D_MAP_MakeRenderObj( g3Dmap );

		ldst->mdlLoaded = TRUE;
		ldst->attrLoaded = TRUE;

		ldst->seq = GFL_G3D_MAP_LOAD_IDLING;
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
static BOOL CheckRectIO( const XZ_VERTEX* vtx0, const XZ_VERTEX *vtx1, const VecFx32* pos );
static BOOL	BinSearch( const LINE_DATA *list, const u16 size, const fx32 valZ, u16* idx );
//============================================================================================
void FieldGetAttr_PMcustomFile( GFL_G3D_MAP_ATTRINFO* attrInfo, const void* mapdata,
					const VecFx32* posInBlock, const fx32 map_width, const fx32 map_height )
{
	//�f�[�^�擾�p���ݒ�
	MAP_DATA_INFO*		mapdataInfo = (MAP_DATA_INFO*)mapdata;
	MAP_HEIGHT_INFO*	heightInfo = &mapdataInfo->heightInfo;
	XZ_VERTEX			*rectVtx0, *rectVtx1;
	VecFx32				vecN;
	fx32				valD, by;
	POLYGON_DATA*		polygon;
	u32					offsIdx;
	u16					lineIdx, polIdx, polNum;
	int					i;

	attrInfo->mapAttrCount = 0;
	
	//���C���f�[�^��2���T�����āA�Ώۃ|���S�������݂��郉�C�������o
	if( BinSearch(heightInfo->lineDataTbl, heightInfo->lineNum, posInBlock->z, &lineIdx) == FALSE ){
		return;
	}
	polNum = heightInfo->lineDataTbl[lineIdx].entryNum;
	offsIdx = heightInfo->lineDataTbl[lineIdx].paramIndex;

	//�����擾�p�O���b�h�����C���f�[�^�ɓo�^����Ă���f�[�^�����[�v���āA�Ώۃ|���S�������o
	for( i=0; i<polNum; i++ ){
		polIdx = heightInfo->polyIDList[offsIdx+i];
		polygon = &heightInfo->polygonData[polIdx];

		//���_�f�[�^�擾
		rectVtx0 = &heightInfo->vertexArray[polygon->vtx_idx0];
		rectVtx1 = &heightInfo->vertexArray[polygon->vtx_idx1];

		//�l�p�`�̓��O����
		if( CheckRectIO( rectVtx0, rectVtx1, posInBlock ) == TRUE ){
			//�@���擾
			vecN = heightInfo->normalArray[polygon->nrm_idx];

			//���ʂ̕������ɕK�v�ȃf�[�^�i���_��ʂ镽�ʂ���̃I�t�Z�b�g�j���擾
			valD = heightInfo->valDArray[polygon->d_idx];

			//���ʂ̕�������荂���擾
			by = -( FX_Mul(vecN.x, posInBlock->x) + FX_Mul(vecN.z, posInBlock->z) + valD );
			attrInfo->mapAttr[attrInfo->mapAttrCount].height = FX_Div(by, vecN.y);
			VEC_Fx16Set( &attrInfo->mapAttr[attrInfo->mapAttrCount].vecN, vecN.x, vecN.y, vecN.z );
			
			attrInfo->mapAttrCount++;
			if (attrInfo->mapAttrCount >= GFL_G3D_MAP_ATTR_GETMAX){
				return;	//�擾�I�[�o�[�t���[
			}
		}
	}
	{
		int bx,bz;
		int attrmax;
		int x,z,num;
		const u16 *buf;
		const MAP_FILE_HEADER *header;
		
		buf = (u16*)mapdataInfo->attrAdrs;
		
		header = (MAP_FILE_HEADER*)((u32)mapdata + sizeof(MAP_DATA_INFO));
		attrmax = header->attrSize / GRID_DATASIZE;	//�ő�v�f��
		//TAMADA_Printf("attrAdrs = %08x\n",mapdataInfo->attrAdrs);
		//TAMADA_Printf("attrSize = %04x\n",header->attrSize);
		//TAMADA_Printf("objSize = %04x\n",header->objSize);
		//TAMADA_Printf("mapSize = %04x\n",header->mapSize);
		//TAMADA_Printf("heightSize = %04x\n",header->heightSize);
		
		//�u���b�N���S�̍��W�����ォ��̍��W�ɕ␳
		x = (posInBlock->x + map_width/2);
		z = (posInBlock->z + map_width/2);
		//�O���b�h�P�ʂɕ␳
		x /= FX32_ONE * GRID_SIZE;
		z /= FX32_ONE * GRID_SIZE;
		num = x + (z * GRIDBLOCKW);
		
		if( num >= attrmax ){
			//TAMADA_Printf( "GetAttrData GX=%d,GZ=%d, ERROR!!\n", x, z );
			attrInfo->mapAttr[0].attr = 0xffff;
		} else {
			//TAMADA_Printf( "GetAttrData GX=%d,GZ=%d, Attr=%xH\n", x, z,  buf[num] );
			attrInfo->mapAttr[0].attr = (buf[num] &0x8000) >> 15;
		}
	}
}

//------------------------------------------------------------------
/**
 *	XZ�ʏ�̎l�p���O����
*/
//------------------------------------------------------------------
static BOOL CheckRectIO( const XZ_VERTEX* vtx0, const XZ_VERTEX *vtx1, const VecFx32* pos )
{
	fx32 x0, x1, z0, z1;

	if( vtx0->x <= vtx1->x ){
		x0 = vtx0->x;
		x1 = vtx1->x;
	}else{
		x0 = vtx1->x;
		x1 = vtx0->x;
	}
	if( vtx0->z <= vtx1->z ){
		z0 = vtx0->z;
		z1 = vtx1->z;
	}else{
		z0 = vtx1->z;
		z1 = vtx0->z;
	}
	if ( (x0 <= pos->x)&&(pos->x <= x1)&&(z0 <= pos->z)&&(pos->z <= z1) ){
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------
/**
 *�@Z�\�[�g����Ă���f�[�^��2���T��
 *	@param	list		�f�[�^���X�g		
 *	@param	size		�f�[�^�T�C�Y
 *	@param	valZ		Z�l
 *	@param	idx			�f�[�^�C���f�b�N�X
 *	
 *	@retval	BOOL	TRUE:�f�[�^����@FALSE:�f�[�^����
*/
//------------------------------------------------------------------
static BOOL	BinSearch( const LINE_DATA *list, const u16 size, const fx32 valZ, u16* idx )
{
	int min,max; 
	u32 data_idx;
	fx32 lineZ;

	if( size == 0 ){
		return FALSE;//�f�[�^�Ȃ��Ȃ̂ŁA�T���I��
	}else if ( size == 1 ){
		*idx = 0;//�T���I��
		return TRUE;
	}
	min = 0;
	max = size - 1;
	data_idx = max/2;
	
	do{
		lineZ = list[data_idx].lineZ;

		if( lineZ > valZ ){
			if( max-1 > min ){
				max = data_idx;		//�T���p��
				data_idx = (min + max)/2;
			}else{			
				*idx = data_idx;	//�T���I��
				return TRUE;
			}
		} else {				
			if( min+1 < max ){
				min = data_idx;		//�T���p��
				data_idx = (min + max)/2;
			}else{
				*idx = data_idx+1;	//�T���I��
				return TRUE;
			}
		}
	}while(1);

	return FALSE;
}







#if 0
#define MAP_READ_OBJ_3D_MAX (32)	//1�u���b�N32�̂n�a�i
//==============================================================================
/**
 * 3DOBJ���[�h
 *
 * @param	inMapResource		�}�b�v���\�[�X�|�C���^
 * @param	inFileName			�t�@�C����
 * @param	outMap3DObjList		3DOBJ���X�g�ւ̃|�C���^
 * @param	field_3d_anime_ptr	3D�A�j���|�C���^
 *
 * @return	none
 */
//==============================================================================
void M3DO_LoadArc3DObjData(	ARCHANDLE *ioHandle,
							const int inDataSize,
							const MAP_RESOURCE_PTR inMapResource,
							M3DOL_PTR outMap3DObjList,
							FLD_3D_ANM_MNG_PTR field_3d_anime_ptr)
{
	MAP3D_OBJECT_HEADER * obj_dat = NULL;
	u32	obj_max;
	int i;
	//�A�[�J�C�u�f�[�^�ǂݍ���
	if (inDataSize != 0){
		obj_dat = sys_AllocMemoryLo(HEAPID_FIELD, inDataSize);
		ArchiveDataLoadByHandleContinue( ioHandle, inDataSize, obj_dat );
		obj_max = inDataSize / sizeof(MAP3D_OBJECT_HEADER);
	}else{
		obj_max = 0;
	}

	for(i=0;i<MAP_READ_OBJ_3D_MAX;i++){
		M3DO_PTR ptr;
		ptr = &(outMap3DObjList->Map3DObjectData[i]);
		if(i < obj_max){
			ptr->id		    = obj_dat[i].id;		//�h�c
			ptr->valid		= TRUE;					//�f�[�^�L��
			ptr->RotateFlg  = FALSE;				//��]����
			ptr->global	    = obj_dat[i].global;	//���W
			ptr->rotate	    = obj_dat[i].rotate;	//��]
			ptr->scale	    = obj_dat[i].scale;		//�X�P�[��

			// �ǂݍ��񂾃e�N�X�`�����\�[�X���Z�b�g
			SetMap3DModel(
					ptr->id,
					inMapResource,
					&(ptr->objectdata),
					&(ptr->objectmodel) );
			if (CheckResourceEntry(inMapResource, ptr->id) == FALSE){
				OS_Printf("�_�~�[�\���̂��߁A�z�u���f��ID�����������܂�%d��0\n",ptr->id);
				ptr->id = 0;
			}
			
#ifdef FOG_MDL_SET_CHECK
			NNS_G3dMdlSetMdlFogEnableFlagAll(ptr->objectmodel, TRUE);
#endif
			F3DA_SetFld3DAnimeEasy(	ptr->id, &(ptr->objectdata), field_3d_anime_ptr);
		}else{
			VecFx32 init_data = {0,0,0};

			ptr->id		    = 0;		// =OFF
			ptr->valid		= FALSE;	//�f�[�^����
			ptr->RotateFlg	= FALSE;
			ptr->global     = init_data;
			ptr->rotate     = init_data;
			ptr->scale	    = init_data;
		}
	}
	if( obj_dat != NULL ){
		sys_FreeMemoryEz( obj_dat );
	}

}
#endif


#if 0
//==============================================================================
/**
 * �}�b�v�A�g���r���[�g�擾
 * @param	gridBlockW	�O���b�h�P�ʂł̃u���b�N����
 * @return	u16	�}�b�v�A�g���r���[�g
 */
//==============================================================================
u16 FieldGetAttrData_PMcustomFile(
	GFL_G3D_MAP *g3Dmap, const VecFx32 *pos, int gridBlockW )
{
	void *mem;
	int bx,bz;
	int attrmax,linemax;
	int x,z,num;
	const u16 *buf;
	const MAP_FILE_HEADER *header;
	const MAP_DATA_INFO *mapdataInfo;
	
	GFL_G3D_MAP_GetLoadMemoryPointer( g3Dmap, &mem );
	mapdataInfo = (MAP_DATA_INFO*)mem;
	buf = (u16*)mapdataInfo->attrAdrs;
	
	header = (MAP_FILE_HEADER*)((u32)mem + sizeof(MAP_FILE_HEADER));
	attrmax = header->attrSize / GRID_DATASIZE;	//�ő�v�f��
	
	x = (pos->x / GRID_SIZE) / FX32_ONE;				//�����O���b�h�P��
	z = (pos->z / GRID_SIZE) / FX32_ONE;
	bx = (x / gridBlockW) * gridBlockW;			//�擪�u���b�NNo
	bz = (z / gridBlockW) * gridBlockW;
	x = x - bx;									//���Έʒu
	z = z - bz;
	num = x + (z * gridBlockW);
	
	if( num >= attrmax ){
#if 0
		OS_Printf( "GetAttrData GX=%d,GZ=%d, gridBlockW = %d, ERROR!!\n",
			x, z, gridBlockW );
#endif
		return( 0xffff );
	}
#if 0
	OS_Printf( "GetAttrData GX=%d,GZ=%d, gridBlockW = %d, Attr=%xH\n",
			x, z, gridBlockW, buf[num] );
#endif
	return( buf[num] );
}
#endif
