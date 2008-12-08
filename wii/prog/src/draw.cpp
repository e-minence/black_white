#include <revolution.h>
#include <revolution/mem.h>
#include <demo.h>

#include <nw4r/lyt.h>
#include "lyt_demolib.h"

#include "draw.h"
#include "common_def.h"
#include "ds_net.h"

nw4r::lyt::ArcResourceAccessor* spResAccessor;
MEMHeapHandle sMemHandle;
MEMAllocator sAllocator;
void* archiveBuf;
nw4r::lyt::ArcResourceAccessor* spResAccessorBg;
void* archiveBufBg;
nw4r::lyt::Layout* pLayout[MY_MAX_NODES];
nw4r::lyt::Layout* pLayoutBg;
nw4r::lyt::AnimTransform* pAnimTrans[MY_MAX_NODES][ANIM_TYPE_MAX];
f32 animFrame[ANIM_TYPE_MAX];
nw4r::lyt::DrawInfo drawInfo;
	

/*---------------------------------------------------------------------------*
  Name:		 ReadDvdFile

  Description:  DVD��̎w�肵���t�@�C���̓��e��S�ēǂݍ��݂܂��B
				�t�@�C���̓��e��ǂݍ��ރo�b�t�@�̓t�@�C���̃T�C�Y�ɉ����Ċm�ۂ��܂��B

  Arguments:	fileName:   �ǂݍ��ރt�@�C����
				memHandle:  �q�[�v�̃n���h��

  Returns:	  �t�@�C���̓��e��ǂݍ��񂾃o�b�t�@�̃|�C���^�B
 *---------------------------------------------------------------------------*/
void* ReadDvdFile( const char* fileName, MEMHeapHandle memHandle );
void*
ReadDvdFile(
	const char*	 fileName,
	MEMHeapHandle   memHandle
)
{
	DVDFileInfo fileInfo;
	if (! DVDOpen(fileName, &fileInfo))
	{
		return NULL;
	}

	u32 fileLenUp32 = nw4r::ut::RoundUp(DVDGetLength(&fileInfo), 32);
	void* readBuf = MEMAllocFromExpHeapEx(memHandle, fileLenUp32, 32);
	s32 readBytes = DVDRead(&fileInfo, readBuf, s32(fileLenUp32), 0);
	NW4R_ASSERT(readBytes > 0);
	DVDClose(&fileInfo);

	return readBuf;
}

/*---------------------------------------------------------------------------*
  Name:         SetupCamera

  Description:  ���f���r���[�s��Ǝˉe�s���ݒ肵�܂��B

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
void SetupCamera(nw4r::lyt::DrawInfo* pDrawInfo, const nw4r::lyt::Layout& layout);
void
SetupCamera(nw4r::lyt::DrawInfo* pDrawInfo, const nw4r::lyt::Layout& layout)
{
	const nw4r::ut::Rect layoutRect = layout.GetLayoutRect();

	f32 znear   =  -1000.f;
	f32 zfar    =   1000.f;

	// �ˉe�s��𐳎ˉe�ɐݒ�
	Mtx44 projMtx;
	MTXOrtho(projMtx, layoutRect.top, layoutRect.bottom, layoutRect.left, layoutRect.right, znear, zfar);
//	MTXOrtho(projMtx, 0, 480, 0, 640, znear, zfar);
	GXSetProjection(projMtx, GX_ORTHOGRAPHIC);

	// ���f���r���[�s���P�ʍs��ɐݒ�
	nw4r::math::MTX34 viewMtx;
	MTX34Identity(&viewMtx);
	pDrawInfo->SetViewMtx(viewMtx);

	// ���C�A�E�g��Rect���Z�b�g
	pDrawInfo->SetViewRect(layoutRect);
}


void	InitDrawSystem(void)
{
	//Init(){
	sMemHandle = demo::InitHeap();
	MEMInitAllocatorForExpHeap(&sAllocator, sMemHandle, 4);
	DEMOInit(NULL);	// Init os, pad, gx, vi
	demo::InitPad();
	DVDInit();
	nw4r::lyt::LytInit();

	//InitGX(){
	GXSetClipMode(GX_CLIP_ENABLE);			  // �N���b�s���O��L��
	GXSetCullMode(GX_CULL_NONE);				// �J�����O���܂���
	GXSetZTexture(GX_ZT_DISABLE, GX_TF_Z8, 0);  // Z�e�N�X�`���𖳌�
	GXSetZMode(GX_FALSE, GX_NEVER, GX_FALSE);   // Z��r���s���܂���
	// Z�o�b�t�@�ɂ���r���L���ȂƂ��ɁA�e�N�X�`�������̑O��Z�o�b�t�@�������g�p�\�ɂ��܂��B
	// (���̃��C�A�E�g�ł̓A���t�@��r���s���Ă��܂���B)
	GXSetZCompLoc(GX_FALSE);
	// �t�H�O�Ȃ�
	const nw4r::ut::Color fogColor = 0;
	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, fogColor);
	//}InitGX()
	//}Init()
	
	NW4R_LOG("heap free size %d\n", MEMGetTotalFreeSizeForExpHeap(sMemHandle));

	archiveBuf = ReadDvdFile("hero.arc", sMemHandle);
	NW4R_NULL_ASSERT(archiveBuf);
	spResAccessor = new nw4r::lyt::ArcResourceAccessor();
	// �A�[�J�C�u�f�[�^�ƃ��\�[�X�̃��[�g�ƂȂ�f�B���N�g�����w��
	const bool ret = spResAccessor->Attach(archiveBuf, "hero");
	NW4R_ASSERT( ret == true );

	nw4r::lyt::Layout::SetAllocator(&sAllocator);

	for( u8 i=0;i<MY_MAX_NODES;i++)
	{
		pLayout[i] = new nw4r::lyt::Layout();
		// ���C�A�E�g���\�[�X�̓ǂݍ���
		{	
			const void* lytRes = spResAccessor->GetResource(0, "hero.brlyt");
			NW4R_NULL_ASSERT(lytRes);
			pLayout[i]->Build(lytRes, spResAccessor);
		}
	}
	const int BoundPaneNum = 6;

	static const char *const animNames[ANIM_TYPE_MAX] = { 
		"hero_stop_d.brlan",	"hero_walk_d.brlan",
		"hero_stop_l.brlan",	"hero_walk_l.brlan",
		"hero_stop_r.brlan",	"hero_walk_r.brlan",
		"hero_stop_u.brlan",	"hero_walk_u.brlan",	};
		
	for( u8 j=0;j<MY_MAX_NODES;j++)
	{
		for( u8 i=0;i<ANIM_TYPE_MAX;i++ )
		{
			const void* rlanRes = spResAccessor->GetResource(0, animNames[i] );
			NW4R_NULL_ASSERT(rlanRes);
			pAnimTrans[j][i] = pLayout[j]->CreateAnimTransform(rlanRes, spResAccessor);
		}
		// ����̃y�C��(�Ƃ��̎q��)�ɑ΂��ăA�j���[�V������ݒ肷��
		nw4r::lyt::Pane* pPane = pLayout[j]->GetRootPane();
		if( pPane != NULL )
		{
			for( u8 i=0;i<ANIM_TYPE_MAX;i++ )
			{
				pPane->BindAnimation(pAnimTrans[j][i],true,false);
				pLayout[j]->SetAnimationEnable( pAnimTrans[j][i] , false );
			}
		}
	}

	archiveBufBg = ReadDvdFile("map.arc", sMemHandle);
	NW4R_NULL_ASSERT(archiveBufBg);
	spResAccessorBg = new nw4r::lyt::ArcResourceAccessor();
	// �A�[�J�C�u�f�[�^�ƃ��\�[�X�̃��[�g�ƂȂ�f�B���N�g�����w��
	spResAccessorBg->Attach(archiveBufBg, "map");
	pLayoutBg = new nw4r::lyt::Layout();
	// ���C�A�E�g���\�[�X�̓ǂݍ���
	{	
		const void* lytRes = spResAccessorBg->GetResource(0, "map.brlyt");
		NW4R_NULL_ASSERT(lytRes);
		pLayoutBg->Build(lytRes, spResAccessorBg);
	}
	
	for (int i = 0; i < ANIM_TYPE_MAX; ++i)
	{
		animFrame[i] = 0.f;
	}

}

void	UpdateDrawSystem(void)
{
	demo::ReadPad();

	DEMOBeforeRender();
	{
		SetupCamera(&drawInfo, *pLayoutBg);
		pLayoutBg->Animate();
		pLayoutBg->CalculateMtx(drawInfo);
		pLayoutBg->Draw(drawInfo);
		for( u8 j=0;j<MY_MAX_NODES;j++)
		{
			SetupCamera(&drawInfo, *pLayout[j]);
			// �q���̃A�j���[�V����
			PLAYER_STATE	*plData = MPDS_GetPlayerState( j );
			if( plData->isValid_ == FALSE )
			{
				pLayout[j]->GetRootPane()->SetVisible(false);
			}
			else
			{
				pLayout[j]->GetRootPane()->SetVisible(true);
				if( plData->newAnim_ != plData->oldAnim_ )
				{
					if( plData->oldAnim_ < ANIM_TYPE_MAX )
					{
						pLayout[j]->SetAnimationEnable( pAnimTrans[j][plData->oldAnim_] , false );
					}
					if( plData->newAnim_ < ANIM_TYPE_MAX )
					{
						pLayout[j]->SetAnimationEnable( pAnimTrans[j][plData->newAnim_] , true );
					}
					plData->oldAnim_ = plData->newAnim_;
				}
				
				nw4r::math::VEC3 pos;
				static const f32 POS_TOP  = 1000.0f;
				static const f32 POS_LEFT = 1000.0f;
				static const f32 POS_WIDTH_HALF  = 320.0f;
				static const f32 POS_HEIGHT_HALF = 240.0f;
				
				pos.x = (f32)plData->posX_ - POS_TOP - POS_WIDTH_HALF;
				pos.y = -((f32)plData->posY_ - POS_LEFT- POS_HEIGHT_HALF);
				pos.z = (-pos.y+POS_HEIGHT_HALF) / 480.0f;
				
				pLayout[j]->GetRootPane()->SetTranslate(pos);
				
			}
		}
		// �t���[���̍X�V
		for (int i = 0; i < ANIM_TYPE_MAX; i++)
		{
			animFrame[i] += 1.0f;
			for( u8 j=0;j<MY_MAX_NODES;j++)
			{
				while (animFrame[i] >= pAnimTrans[j][i]->GetFrameSize())
				{
					animFrame[i] -= pAnimTrans[j][i]->GetFrameSize();
				}
				pAnimTrans[j][i]->SetFrame(animFrame[i]);
			}
		}

		for( u8 j=0;j<MY_MAX_NODES;j++)
		{
			pLayout[j]->Animate();
			pLayout[j]->CalculateMtx(drawInfo);
			pLayout[j]->Draw(drawInfo);
		}
		
		//DrawCrossLine(curPos, nw4r::ut::Color::YELLOW, pLayout->GetLayoutRect());
	}
	DEMODoneRender();
}

void	TermDrawSystem(void)
{
	for( u8 j=0;j<MY_MAX_NODES;j++)
	{
		delete pLayout[j];
	}

	delete spResAccessor;

	MEMFreeToExpHeap(sMemHandle, archiveBuf);

	delete pLayoutBg;
	delete spResAccessorBg;
	MEMFreeToExpHeap(sMemHandle, archiveBufBg);

	NW4R_LOG("heap free size %d\n", MEMGetTotalFreeSizeForExpHeap(sMemHandle));
	
	
}
