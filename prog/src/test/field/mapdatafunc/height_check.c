//============================================================================================
/**
 * @file	haight_check.c
 * @brief	�����擾�֘A
 * @author	Nozomu Saito
 * @date	2005.05.06
 */
//============================================================================================
//#include "common.h"
#include "height.h"
#define MAX_GET_POL	(10)

#define GRID_ENTRY_ONE_DATA	(2)	//2�o�C�g�f�[�^��2��4�o�C�g
#define LINE_ONE_DATA	(5)		//2�o�C�g�f�[�^��5��10�o�C�g
#define Z_VAL_OFFSET_L(data_idx)	( data_idx*LINE_ONE_DATA+1 )
#define Z_VAL_OFFSET_H(data_idx)	( data_idx*LINE_ONE_DATA+2 )

#define LINE_Z_VAL(list,idx)	( (list[Z_VAL_OFFSET_H(data_idx)]<<16) | list[Z_VAL_OFFSET_L(data_idx)] )

static BOOL	BinSearch(const u16 *inDataList,const u16 inDataSize,const fx32 inZ_Val,u16 *outIndex);
static void GetPolygonNrm(MHI_CONST_PTR inMap3DInfo, u16 inIdx, VecFx32 *outVertex);
static void GetPolygonVertex(MHI_CONST_PTR inMap3DInfo, u16 inIdx, VecFx32 *outVertex);



//---------------------------------------------------------------------------------------------------------
/**
 *	�Ζʂ�o��Ƃ��̈ړ��␳�B
 *	�Ζʂ�o��Ƃ��́A���n���ړ�����Ƃ������ړ��ʂ�����
 *	@param	inMoveVal		�ړ���
 *	@param	inNewVec		�������W
 *	@param	inOldVec		���ݍ��W
 *
 *	@retval	VecFx32			�C����ړ��ʁi�ړ������j
*/
//---------------------------------------------------------------------------------------------------------
VecFx32 MoveRevise(fx32 inMoveVal,VecFx32 inNewVec,VecFx32 inOldVec)
{
	VecFx32 vec,dstVec;
	VecFx32 dmyVec = {0,0,0};
	
	//vec = inNewVec - inOldVec;
	//vec = nomalize(vec);
	//dstVec = inMoveVal * vec + dmyVec;
	//	dstVec = normalize(inNewVec - inOldVec) * inMoveVal;
	VEC_Subtract(&inNewVec,&inOldVec,&vec);
	VEC_Normalize(&vec, &vec);
	VEC_MultAdd(inMoveVal,&vec,&dmyVec,&dstVec);
	
	return dstVec;
}

//---------------------------------------------------------------------------------------------------------
/**
 *	�|���S���C���f�b�N�X����|���S�����_�f�[�^���擾
 *	@param	inMAp3DInfo		�}�b�v�������		
 *	@param	inIdx			�|���S���f�[�^�C���f�b�N�X
 *	@param	outVertex		���_�f�[�^�i�[��|�C���^
 *	
 *	@retval	none
*/
//---------------------------------------------------------------------------------------------------------
static void GetPolygonVertex(MHI_CONST_PTR inMap3DInfo, u16 inIdx, VecFx32 *outVertex)
{
	outVertex[0] = inMap3DInfo->VertexArray[inMap3DInfo->PolygonData[inIdx].vtx_idx0];
	outVertex[1] = inMap3DInfo->VertexArray[inMap3DInfo->PolygonData[inIdx].vtx_idx1];
	outVertex[2] = inMap3DInfo->VertexArray[inMap3DInfo->PolygonData[inIdx].vtx_idx2];
}


//---------------------------------------------------------------------------------------------------------
/**
 *	�|���S���C���f�b�N�X����|���S���̖@�����擾
 *	@param	inMAp3DInfo		�}�b�v�������		
 *	@param	inIdx			�|���S���f�[�^�C���f�b�N�X
 *	@param	outVertex		�@���f�[�^�i�[��|�C���^
 *	
 *	@retval	none
*/
//---------------------------------------------------------------------------------------------------------
static void GetPolygonNrm(MHI_CONST_PTR inMap3DInfo, u16 inIdx, VecFx32 *outVertex)
{
	*outVertex = inMap3DInfo->NormalArray[inMap3DInfo->PolygonData[inIdx].nrm_idx];
}


//---------------------------------------------------------------------------------------------------------
/**
 *�@Z�\�[�g����Ă���f�[�^��2���T��
 *	@param	inDataList		�f�[�^���X�g		
 *	@param	inDataSize		�f�[�^�T�C�Y
 *	@param	inZ_Val			Z�l
 *	@param	outIndex		�f�[�^�C���f�b�N�X
 *	
 *	@retval	BOOL	TRUE:�f�[�^����@FALSE:�f�[�^����
*/
//---------------------------------------------------------------------------------------------------------
static BOOL	BinSearch(const u16 *inDataList,const u16 inDataSize,const fx32 inZ_Val,u16 *outIndex)
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
		z_val = LINE_Z_VAL(inDataList,data_idx);
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
	//�f�o�b�O�F�~�����菈��
	
	;
}

//-------------------------------------------------------------------------------------------------
/**
 * @brief	�|�P����DP�`���̍����f�[�^�擾
 * @param	inX				XZ���ʏ��X�ʒu
 * @param	inZ				XZ���ʏ��Y�ʒu
 * @param	inMap3DInfo		DP�`���̍����f�[�^�ւ̃|�C���^
 * @param	attrInfo		�擾������������Ԃ����߂̃��[�N�ւ̃|�C���^
 * @return	int				�擾�ł��������f�[�^�̌���Ԃ�
 */
//-------------------------------------------------------------------------------------------------
int GetDPFormatHeight(const fx32 inX, const fx32 inZ, MHI_CONST_PTR inMap3DInfo, GFL_G3D_MAP_ATTRINFO* attrInfo)
{
	VecFx32 target;
	u16 i;
	int pol_count;
	u32 line_num,tbl_idx;

	u32 offset_idx;

	u16 line_data_idx;
	u16 line_index;
	u16 polygon_num;

	const u16 *line_list;

	if (inMap3DInfo->DataValid != TRUE){
		OS_Printf("�����f�[�^�\���̂Ȃ�\n");
		return 0;
	}
	//ZX���ʂɎˉe
	target.x = inX;
	target.y = 0;
	target.z = inZ;

	//�{���e�[�u�������o
	{
		u32 tbl_x_idx,tbl_z_idx;
		fx32 tbl_x,tbl_z;

		tbl_x = inX - inMap3DInfo->SplitGridData->StartX;
		if (tbl_x < 0){
			tbl_x = 0;
		}
		tbl_x_idx = FX_Whole(FX_Div(tbl_x, inMap3DInfo->SplitGridData->SizeX));
		if (tbl_x_idx >= inMap3DInfo->SplitGridData->NumX){
			tbl_x_idx = inMap3DInfo->SplitGridData->NumX-1;
		}

		tbl_z = inZ - inMap3DInfo->SplitGridData->StartZ;
		if (tbl_z < 0){
			tbl_z = 0;
		}
		tbl_z_idx = FX_Whole(FX_Div(tbl_z, inMap3DInfo->SplitGridData->SizeZ));
		if (tbl_z_idx >= inMap3DInfo->SplitGridData->NumZ){
			tbl_z_idx = inMap3DInfo->SplitGridData->NumZ-1;
		}

		tbl_idx = tbl_x_idx + (tbl_z_idx * inMap3DInfo->SplitGridData->NumZ);
	}

	line_num =		inMap3DInfo->GridDataTbl[tbl_idx*GRID_ENTRY_ONE_DATA+0];
	line_data_idx = inMap3DInfo->GridDataTbl[tbl_idx*GRID_ENTRY_ONE_DATA+1];
	line_list =		&inMap3DInfo->LineDataTbl[line_data_idx*LINE_ONE_DATA];
	//�����擾�p�O���b�h�f�[�^�ɓo�^����Ă��郉�C���f�[�^��2���T�����āA�����������Ă���|���S�������݂��郉�C�������o
	if ( BinSearch(line_list,line_num,target.z,&line_index)==FALSE){
		return 0;
	}

	polygon_num = line_list[line_index*LINE_ONE_DATA];
	offset_idx = (line_list[(line_index*LINE_ONE_DATA)+4]<<16) | line_list[(line_index*LINE_ONE_DATA)+3];


	//�����擾�p�O���b�h�����C���f�[�^�ɓo�^����Ă���f�[�^�����[�v���āA�����������Ă���|���S�������o
	pol_count = 0;
	for (i=0; i<polygon_num; i++) {
		VecFx32 vertex[3];
		fx32 d,y;
		//���_�f�[�^�擾
		u16 pol_index = inMap3DInfo->PolyIDList[offset_idx + i];
		GetPolygonVertex(inMap3DInfo, pol_index, vertex);
		//ZX���ʂɎˉe
		vertex[0].y = 0;
		vertex[1].y = 0;
		vertex[2].y = 0;
		//�O�p�`�̓��O����
		if (BG3D_CheckTriangleIObyZX(target,vertex[0],vertex[1],vertex[2]) == TRUE) {
			VecFx32 nrm;
			//�@���擾
			GetPolygonNrm(inMap3DInfo,pol_index, &nrm);
			//���ʂ̕������ɕK�v�ȃf�[�^�i���_��ʂ镽�ʂ���̃I�t�Z�b�g�j���擾
			d = inMap3DInfo->PolygonData[pol_index].paramD;
			//���ʂ̕�����(nrm.x * x + nrm.y * y + nrm.z * z + d = 0)��荂���擾
			//y = - (nrm.x * target.x + nrm.z * target.z + d) / nrm.y;
			y = -FX_Div((FX_Mul(nrm.x, target.x)+FX_Mul(nrm.z, target.z)+d), nrm.y);

			VEC_Fx16Set(&attrInfo->mapAttr[pol_count].vecN, nrm.x, nrm.y, nrm.z);
			attrInfo->mapAttr[pol_count].height = y;
			attrInfo->mapAttr[pol_count].attr = 0;
			pol_count++;
			if (pol_count >= MAX_GET_POL){
				OS_Printf("��v�f�[�^����������");
				break;
			}
		}
	}//end for
	

	return pol_count;
}

#if 0
/**
�\�[�g�p�z��̏�����
*/
static void InitArray(HEIGHT_ARRAY *outArray)
{
	int i;
	for(i=0;i<MAX_GET_POL;i++){
		outArray[i].Height = 0;
		outArray[i].Prev = -1;
		outArray[i].Next = -1;
	}
}

//---------------------------------------------------------------------------------------------------------
/**
 *	�����擾
 *
 *	@param	inNowY			����Y���W	
 *	@param	inX				���߂���������X���W
 *	@param	inZ				���߂���������Z���W
 *	@param	inMap3DInfo		�������
 *	@param	outY			�����i�[�o�b�t�@
 *	
 *	@retval	BOOL			TRUE:�������擾�ł���	FALSE:�������擾�ł��Ȃ�����
*/
//---------------------------------------------------------------------------------------------------------
BOOL GetHeightForBlock(const fx32 inNowY, const fx32 inX, const fx32 inZ, MHI_CONST_PTR inMap3DInfo,fx32 *outY)
{
	VecFx32 vertex[3];
	VecFx32 target,nrm;
	fx32 temp1,temp2;
	BOOL result;
	u16 i,pol_index;
	fx32 d,y;
	int pol_count;
	u32 line_num,tbl_idx,tbl_x_idx,tbl_z_idx;
	HEIGHT_ARRAY height_array[MAX_GET_POL];
	fx32 tbl_x,tbl_z;

	u32 offset_idx;

	u16 line_data_idx;
	u16 line_index;
	u16 polygon_num;

	const u16 *line_list;

	if (inMap3DInfo->DataValid != TRUE){
		OS_Printf("�����f�[�^�\���̂Ȃ�\n");
		return FALSE;
	}
	result = FALSE;
	//ZX���ʂɎˉe
	target.x = inX;
	target.y = 0;
	target.z = inZ;
	pol_count = 0;
	InitArray(height_array);
	//�{���e�[�u�������o
	tbl_x = inX - inMap3DInfo->SplitGridData->StartX;
	if (tbl_x < 0){
		tbl_x = 0;
	}
	tbl_z = inZ - inMap3DInfo->SplitGridData->StartZ;
	if (tbl_z < 0){
		tbl_z = 0;
	}

	tbl_x = FX_Div(tbl_x, inMap3DInfo->SplitGridData->SizeX);
	tbl_x_idx = (int)(tbl_x>>12);
	if (tbl_x_idx >= inMap3DInfo->SplitGridData->NumX){
		tbl_x_idx = inMap3DInfo->SplitGridData->NumX-1;
	}
	tbl_z = FX_Div(tbl_z, inMap3DInfo->SplitGridData->SizeZ);
	tbl_z_idx = (int)(tbl_z>>12);
	if (tbl_z_idx >= inMap3DInfo->SplitGridData->NumZ){
		tbl_z_idx = inMap3DInfo->SplitGridData->NumZ-1;
	}

	tbl_idx = tbl_x_idx + (tbl_z_idx * inMap3DInfo->SplitGridData->NumZ);
	line_num = inMap3DInfo->GridDataTbl[tbl_idx*GRID_ENTRY_ONE_DATA];
	line_data_idx = inMap3DInfo->GridDataTbl[tbl_idx*GRID_ENTRY_ONE_DATA+1];
#if 1	
	OS_Printf("grid_index:%d\n",tbl_idx);
	OS_Printf("line_data_idx = %d\n",line_data_idx);
	OS_Printf("line_num = %d\n",line_num);
#endif
	line_list = &inMap3DInfo->LineDataTbl[line_data_idx*LINE_ONE_DATA];
	//�����擾�p�O���b�h�f�[�^�ɓo�^����Ă��郉�C���f�[�^��2���T�����āA�����������Ă���|���S�������݂��郉�C�������o
	if ( BinSearch(line_list,line_num,target.z,&line_index)==FALSE){
	OS_Printf("NOTHING\n");		
		return FALSE;
	}
	OS_Printf("line_index = %d\n",line_index);

	polygon_num = line_list[line_index*LINE_ONE_DATA];
	OS_Printf("polygon_num = %d\n",polygon_num);
	offset_idx = (line_list[(line_index*LINE_ONE_DATA)+4]<<16) | line_list[(line_index*LINE_ONE_DATA)+3];

	OS_Printf("offset_idx:%d\n",offset_idx);

	//�����擾�p�O���b�h�����C���f�[�^�ɓo�^����Ă���f�[�^�����[�v���āA�����������Ă���|���S�������o
	OS_PutString("target:"); DEBUG_PutVector(&target); OS_PutString("\n");
	for(i=0;i<polygon_num;i++){
				pol_index = inMap3DInfo->PolyIDList[offset_idx+i];
		//���_�f�[�^�擾
		GetPolygonVertex(inMap3DInfo,pol_index, vertex);
		OS_Printf("pol_index:%d",pol_index);
		DEBUG_PutVector(&vertex[0]);
		DEBUG_PutVector(&vertex[1]);
		DEBUG_PutVector(&vertex[2]);
		OS_PutString("\n");
		//ZX���ʂɎˉe
		vertex[0].y = 0;
		vertex[1].y = 0;
		vertex[2].y = 0;
		//�O�p�`�̓��O����
		///result = BG3D_CheckTriangleIO(&target,vertex[0],vertex[1],vertex[2]);
		result = BG3D_CheckTriangleIObyZX(target,vertex[0],vertex[1],vertex[2]);
		if (result == TRUE){//�O�p�`��
			//�@���擾
			GetPolygonNrm(inMap3DInfo,pol_index, &nrm);
			//���ʂ̕������ɕK�v�ȃf�[�^�i���_��ʂ镽�ʂ���̃I�t�Z�b�g�j���擾
			d = inMap3DInfo->PolygonData[pol_index].paramD;
			//���ʂ̕�������荂���擾
			y = -(FX_Mul(nrm.x, target.x)+FX_Mul(nrm.z, target.z)+d);
			y = FX_Div(y, nrm.y);
			OS_Printf("height:%08x",y);
			height_array[pol_count].Height = y;
			//�z��̃\�[�g�i���s�ł͋@�\���Ă܂���B�Ή��\��j<<���g�p�ɂ��܂����B20060801
			///SortArray(pol_count,height_array);
			pol_count++;
	OS_Printf("%d\n",i);
	OS_Printf("D=%d\n",d);
			if (pol_count >= MAX_GET_POL){
				break;
			}
			///break;
		}
	}//end for
	

	if (pol_count > 1){//�|���S���f�[�^��������ꂽ�Ƃ�
	//OS_Printf("!!\n");		
		pol_index = 0;
		temp1 = Max(inNowY,height_array[0].Height) - Min(inNowY,height_array[0].Height);
		//���ׂẴ|���S���f�[�^�̍��������߁A���݂̍����Ɉ�ԋ߂�����I�o����
		for(i=1;i<pol_count;i++){
			temp2 = Max(inNowY,height_array[i].Height) - Min(inNowY,height_array[i].Height);
			if (temp1>temp2){
				temp1 = temp2;
				pol_index = i;
			}
		}
		*outY = height_array[pol_index].Height;
		OS_Printf("select[%d]...%08x\n",pol_index, *outY);
		return TRUE;
	}else if(pol_count == 1){
		*outY = height_array[0].Height;
		return TRUE;
	}
	OS_Printf("���������Ȃ�����\n");
#if 1	
	if (pol_count != 0){
		*outY = height_array[0].Height;
		return TRUE;
	}
#endif	
	return FALSE;
}
#endif

