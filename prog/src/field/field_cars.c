#include <gflib.h>
#include "field_cars.h"
#include "arc/arc_def.h"
#include "arc/fieldmap/buildmodel_outdoor.naix"
#include "arc/fieldmap/zone_id.h"


//==============================================================================
/**
 * @brief �萔
 */
//==============================================================================

// �g���[���[�̐�
#define TRAILER_NUM (2)

// �g���[���̈ړ��͈�
#define RAIL_MIN_Z (600)
#define RAIL_MAX_Z (2115)

// �g���[���[�̈ړ����x
#define TRAILER_SPEED_MAX (16)
#define TRAILER_SPEED_MIN ( 8)

// ���Ԕ͈�(�v���C���[�ʒu)
#define START_MIN_Z   (750)
#define START_MAX_Z  (2000)

// ���ԊԊu(�P��:�t���[��)
#define START_MIN_INTERVAL   (60)
#define START_MAX_INTERVAL  (180)


//============================================================================== 
/**
 * @brief �C���f�b�N�X
 */
//==============================================================================

// ���\�[�X�E�C���f�b�N�X
enum RES_INDEX
{
	RES_INDEX_TRAILER_BMD,	// �g���[���[�̃��f��
	RES_INDEX_MAX
};

// �����_�[�E�C���f�b�N�X
enum RND_INDEX
{
	RND_INDEX_TRAILER,		// �g���[���[
	RND_INDEX_MAX
};

// 3D�I�u�W�F�N�g�E�C���f�b�N�X
enum OBJ_INDEX
{
	OBJ_INDEX_TRAILER,		// �g���[���[
	OBJ_INDEX_MAX
};


//============================================================================== 
/**
 * @brief �\����
 */
//============================================================================== 

//------------
// �g���[���[
//------------
typedef struct 
{
	BOOL              active;	// �����Ă��邩�ǂ���
	GFL_G3D_OBJSTATUS status;	// �X�e�[�^�X
	fx32              speed;	// �ړ����x
}
TRAILER; 

//------------
// �^�C�}�[
//------------
typedef struct
{
	int count;
}
TIMER;

//------------
// �V�X�e��
//------------
struct _FIELD_CARS
{
	HEAPID heapID;	// �q�[�vID

	FIELD_PLAYER* pFieldPlayer;	// �Ď��Ώۃv���C���[

	GFL_G3D_RES* pRes[ RES_INDEX_MAX ];	// ���\�[�X
	GFL_G3D_RND* pRnd[ RND_INDEX_MAX ];	// 3D�����_�[
	GFL_G3D_OBJ* pObj[ OBJ_INDEX_MAX ]; // 3D�I�u�W�F�N�g

	TRAILER trailer[ TRAILER_NUM ];	// �g���[���[
	TIMER   timer[ TRAILER_NUM ];	// �^�C�}�[

	BOOL active;	// ����t���O
};



//============================================================================== 
/**
 * @brief ����J�֐��̃v���g�^�C�v�錾
 */
//============================================================================== 

// �g���[���[����
static void MoveTrailer( TRAILER* p_trailer );

// �V�X�e��������
static void LoadResource( FIELD_CARS* p_sys );
static void CreateRender( FIELD_CARS* p_sys );
static void CreateObject( FIELD_CARS* p_sys );
static void InitTrailers( FIELD_CARS* p_sys );

// �V�X�e���i�s
static void ActTrailers( FIELD_CARS* p_sys );


//============================================================================== 
/**
 * @brief ���J�֐��̎���
 */
//============================================================================== 

//-------------------------------------------------------------------------------
/**
 * @brief �������֐�
 *
 * @param p_player �Ď��Ώۂ̃v���C���[�I�u�W�F�N�g
 * @param zone_id  �]�[��ID
 * @param heap_id  �g�p����q�[�vID
 *
 * @return �쐬�����V�X�e��
 */
//-------------------------------------------------------------------------------
FIELD_CARS* FIELD_CARS_Create( FIELD_PLAYER* p_player, u16 zone_id, HEAPID heap_id )
{
	FIELD_CARS* p_sys;

	// �V�X�e���{�̂��쐬
	p_sys = GFL_HEAP_AllocMemory( heap_id, sizeof( FIELD_CARS ) );

	// �V�X�e��������
	p_sys->heapID       = heap_id;
	p_sys->pFieldPlayer = p_player;
	p_sys->active       = ( zone_id == ZONE_ID_H01 );

	// �g���[���[������
	InitTrailers( p_sys );

	// ���\�[�X�ǂݍ���
	LoadResource( p_sys ); 

	// �����_�[�쐬
	CreateRender( p_sys );

	// 3D�I�u�W�F�N�g�̍쐬
	CreateObject( p_sys );

	return p_sys;
}



//------------------------------------------------------------------------------- 
/**
 * @brief �j���֐�
 *
 * @param p_sys �j������V�X�e��
 */
//------------------------------------------------------------------------------- 
void FIELD_CARS_Delete( FIELD_CARS* p_sys )
{
	int i;

	// 3D�I�u�W�F�N�g
	for( i=0; i<OBJ_INDEX_MAX; i++ )
	{
		GFL_G3D_OBJECT_Delete( p_sys->pObj[i] );
	}

	// 3D�����_�[
	for( i=0; i<RND_INDEX_MAX; i++ )
	{
		GFL_G3D_RENDER_Delete( p_sys->pRnd[i] );
	}
	
	// ���\�[�X
	for( i=0; i<RES_INDEX_MAX; i++ )
	{
		GFL_G3D_DeleteResource( p_sys->pRes[i] );
	}
	
	// �V�X�e���{��
	GFL_HEAP_FreeMemory( p_sys );
}



//------------------------------------------------------------------------------- 
/**
 * @brief �i�s�֐�
 *
 * @param �i�s����V�X�e��
 */
//------------------------------------------------------------------------------- 
void FIELD_CARS_Process( FIELD_CARS* p_sys )
{
	// ����t���O�������Ă��Ȃ����, �������Ȃ�
	if( p_sys->active != TRUE ) return;

	// �g���[���[�̓���
	ActTrailers( p_sys ); 
}


//------------------------------------------------------------------------------- 
/**
 * @brief �`��֐�
 *
 * @param p_sys �`�悷��V�X�e��
 */
//------------------------------------------------------------------------------- 
void FIELD_CARS_Draw( FIELD_CARS* p_sys )
{ 
	int i;

	// ����t���O�������Ă��Ȃ����, �������Ȃ�
	if( p_sys->active != TRUE ) return;

	// ���쒆�̃g���[���[�݂̂�`�悷��
	for( i=0; i<TRAILER_NUM; i++ )
	{ 
		if( p_sys->trailer[i].active == TRUE )
		{ 
			GFL_G3D_DRAW_DrawObject( p_sys->pObj[ OBJ_INDEX_TRAILER ], &p_sys->trailer[i].status );
		}
	}
}


//============================================================================== 
/**
 * @brief ����J�֐��̎���
 */
//============================================================================== 

//------------------------------------------------------------------------------- 
/**
 * @brief �g���[���[�𓮂���
 *
 * @param p_trailer �������g���[���[
 */
//------------------------------------------------------------------------------- 
static void MoveTrailer( TRAILER* p_trailer )
{
	// �A�N�e�B�u�łȂ����, �����Ȃ�
	if( p_trailer->active != TRUE ) return;

	// �ړ�
	p_trailer->status.trans.z += p_trailer->speed;

	// �ړ��͈͂𒴂�����, ����I��
	if( ( FX_Whole( p_trailer->status.trans.z ) < RAIL_MIN_Z ) || ( RAIL_MAX_Z < FX_Whole( p_trailer->status.trans.z ) ) )
	{
		p_trailer->active = FALSE;
	}
}


//------------------------------------------------------------------------------- 
/**
 * @brief ���\�[�X��ǂݍ���
 *
 * @param p_sys �ǂݍ��ރV�X�e��
 */
//------------------------------------------------------------------------------- 
static void LoadResource( FIELD_CARS* p_sys )
{
	// �g���[���[�̃��f��
	p_sys->pRes[ RES_INDEX_TRAILER_BMD ] = 
		GFL_G3D_CreateResourceArc( 
				ARCID_BMODEL_OUTDOOR, NARC_output_buildmodel_outdoor_trailer_01_nsbmd );

	// �e�N�X�`���]��
	GFL_G3D_TransVramTexture( p_sys->pRes[ RES_INDEX_TRAILER_BMD ] );

	// DEBUG:
	{
		int i; 
		for( i=0; i<RES_INDEX_MAX; i++ )
		{
			GF_ASSERT_MSG( p_sys->pRes[i] != NULL, "���\�[�X�ǂݍ��ݎ��s" );
		}
	}
}

//------------------------------------------------------------------------------- 
/**
 * @brief 3D�����_�[���쐬����
 *
 * @param p_sys �쐬�Ώۂ̃V�X�e��
 */ 
//------------------------------------------------------------------------------- 
static void CreateRender( FIELD_CARS* p_sys )
{
	// �g���[���[
	p_sys->pRnd[ RND_INDEX_TRAILER ] = 
		GFL_G3D_RENDER_Create(
				p_sys->pRes[ RES_INDEX_TRAILER_BMD ],
				0,
				p_sys->pRes[ RES_INDEX_TRAILER_BMD ] ); 

	// DEBUG:
	{
		int i; 
		for( i=0; i<RND_INDEX_MAX; i++ )
		{
			GF_ASSERT_MSG( p_sys->pRnd[i] != NULL, "�����_�[�쐬���s" );
		}
	}
}

//------------------------------------------------------------------------------- 
/**
 * @brief 3D�I�u�W�F�N�g���쐬����
 *
 * @param p_sys �쐬�Ώۂ̃V�X�e��
 */
//------------------------------------------------------------------------------- 
static void CreateObject( FIELD_CARS* p_sys )
{
	// �g���[���[
	p_sys->pObj[ OBJ_INDEX_TRAILER ]
		= GFL_G3D_OBJECT_Create( p_sys->pRnd[ RND_INDEX_TRAILER ], NULL, 0 );

	// DEBUG:
	{
		int i; 
		for( i=0; i<OBJ_INDEX_MAX; i++ )
		{
			GF_ASSERT_MSG( p_sys->pObj[i] != NULL, "�I�u�W�F�N�g�쐬���s" );
		}
	}
}

//------------------------------------------------------------------------------- 
/**
 * @brief �g���[���[������������
 *
 * @param p_sys �������Ώۂ̃V�X�e��
 */
//------------------------------------------------------------------------------- 
static void InitTrailers( FIELD_CARS* p_sys )
{
	p_sys->trailer[0].active = FALSE;
	VEC_Set( &p_sys->trailer[0].status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
	MTX_Identity33( &p_sys->trailer[0].status.rotate );

	p_sys->trailer[1].active = FALSE;
	VEC_Set( &p_sys->trailer[1].status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
	MTX_RotY33( &p_sys->trailer[1].status.rotate, FX_SinIdx( 32768 ), FX_CosIdx( 32768 ) );	// y��180�x��]
}

//------------------------------------------------------------------------------- 
/**
 * @brief �g���[���[�𓮂���
 *
 * @param p_sys �����Ώۂ̃V�X�e��
 */
//------------------------------------------------------------------------------- 
static void ActTrailers( FIELD_CARS* p_sys )
{
	int i;
	VecFx32 pos;
	BOOL player_is_in_area = FALSE;	// �v���C���[�����ԗ̈���ɂ��邩�ǂ���

	// �v���C���[�̈ʒu����
	FIELD_PLAYER_GetPos( p_sys->pFieldPlayer, &pos );
	if( ( START_MIN_Z < FX_Whole( pos.z ) ) && ( FX_Whole( pos.z ) < START_MAX_Z ) )
	{
		player_is_in_area = TRUE;
	}

	// �S�Ẵg���[�����`�F�b�N����
	for( i=0; i<TRAILER_NUM; i++ )
	{
		// �����Ă���ꍇ
		if( p_sys->trailer[i].active == TRUE )
		{
			// �ړ�������
			MoveTrailer( &p_sys->trailer[i] );

			// �ړ����I�������, �^�C�}�[�Z�b�g
			if( p_sys->trailer[i].active == FALSE )
			{
				p_sys->timer[i].count
					= START_MIN_INTERVAL + GFL_STD_MtRand( START_MAX_INTERVAL - START_MIN_INTERVAL ); 

				// DEBUG:
				OBATA_Printf("SetTimer : count = %d\n", p_sys->timer[i].count );
			}
		}
		// �����Ă��Ȃ��ꍇ
		else 
		{ 
			// ���ԗ̈���ɂ����, ���Ԕ�����s��
			if( player_is_in_area )
			{
				// ���Ԍo�߂Ŕ���
				if( --p_sys->timer[i].count < 0 )
				{
					fx32 x = 0;
					fx32 z = 0;
					fx32 s = 0;

					switch( i )
					{
						case 0:	
							x = (997) << FX32_SHIFT;
							z = (RAIL_MAX_Z) << FX32_SHIFT; 
							s = -( GFL_STD_MtRand( TRAILER_SPEED_MAX - TRAILER_SPEED_MIN ) + TRAILER_SPEED_MIN ) << FX32_SHIFT;	
							break;
						case 1:	
							x = (1053) << FX32_SHIFT;
							z = (RAIL_MIN_Z) << FX32_SHIFT; 
							s = ( GFL_STD_MtRand( TRAILER_SPEED_MAX - TRAILER_SPEED_MIN ) + TRAILER_SPEED_MIN ) << FX32_SHIFT;	
							break;
					}

					p_sys->trailer[i].active = TRUE;
					p_sys->trailer[i].status.trans.x  = x;
					p_sys->trailer[i].status.trans.y  = (245) << FX32_SHIFT;
					p_sys->trailer[i].status.trans.z  = z;
					p_sys->trailer[i].speed  = s;

					// DEBUG:
					OBATA_Printf( "TrailerStart : speed = %d\n", FX_Whole( p_sys->trailer[i].speed ) );
				}
			} 
		}
	}
}

