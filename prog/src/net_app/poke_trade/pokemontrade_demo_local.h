//=============================================================================
/**
 * @file	  pokemontrade_demo_local.h
 * @brief	  �|�P���������f����� ���[�J�����L��`
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/10/31
 */
//=============================================================================

#pragma once

typedef enum
{
  PTC_KIND_DEMO1,
  PTC_KIND_DEMO2,
  PTC_KIND_DEMO3,
  PTC_KIND_DEMO4,
  PTC_KIND_DEMO5,
  PTC_KIND_DEMO6,
  PTC_KIND_DEMO7,
  PTC_KIND_DEMO8,
  PTC_KIND_ORG,
  PTC_KIND_NUM_MAX,
} _TYPE_PTC_KIND;


typedef enum
{
  IRPTC_KIND_DEMO1,
  IRPTC_KIND_DEMO1_2,
  IRPTC_KIND_DEMO2,
  IRPTC_KIND_DEMO3,
  IRPTC_KIND_DEMO3_2,
  IRPTC_KIND_DEMO4,
  IRPTC_KIND_NUM_MAX,
} _TYPE_IRPTC_KIND;



struct _POKEMONTRADE_DEMO_WORK{
  GFL_PTC_PTR ptc[PTC_KIND_NUM_MAX];
  void* ptcheap[PTC_KIND_NUM_MAX];
  VecFx32 PushPos;
  VecFx32 ReturnPos;
  VecFx32 BackupScale[4];

  ICA_ANIME* icaCamera;
  ICA_ANIME* icaTarget;
  ICA_ANIME* icaBallin;
  ICA_ANIME* icaBallout;

  GFL_EMIT_PTR pBallInPer;
  GFL_EMIT_PTR pBallOutPer;

  BOOL bCamera;
  BOOL bTarget;
  
  GFL_CLWK* pPokeCreateCLWK;   //�|�P�����o��G�t�F�N�gCLACT
  void* effectRes;
  
  HEAPID heapID;
};


extern void POKEMONTRADE_DEMO_PTC_Init( POKEMONTRADE_DEMO_WORK* pWork );
extern void POKEMONTRADE_DEMO_PTC_Load1( POKEMONTRADE_DEMO_WORK* pWork );
extern void POKEMONTRADE_DEMO_PTC_Load2( POKEMONTRADE_DEMO_WORK* pWork );
extern void POKEMONTRADE_DEMO_PTC_End( POKEMONTRADE_DEMO_WORK* pWork,int num );
extern void POKEMONTRADE_DEMO_ICA_Init(POKEMONTRADE_DEMO_WORK* pWork,int type);
extern void POKEMONTRADE_DEMO_ICA_Delete(POKEMONTRADE_DEMO_WORK* pWork);

extern void POKEMONTRADE_DEMO_IRICA_Init(POKEMONTRADE_DEMO_WORK* pWork);
extern void POKEMONTRADE_DEMO_IRICA_Delete(POKEMONTRADE_DEMO_WORK* pWork);
extern void POKEMONTRADE_DEMO_IRPTC_Init( POKEMONTRADE_DEMO_WORK* pWork );

