//=============================================================================
/**
 * @file	  pokemontrade_demo_gts.c
 * @bfief	  �|�P���������A�j���[�V����  ��ɍs������
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  09/09/27
 */
//=============================================================================

#include <gflib.h>


#include "arc_def.h"
#include "net/network_define.h"
#include "system\wipe.h"

#include "net_app/pokemontrade.h"
#include "system/main.h"

#include "poke_icon.naix"
#include "pokeicon/pokeicon.h"

#include "ircbattle.naix"
#include "trade.naix"



#include "poke_tool/poke_tool_def.h"
#include "box_m_obj_NANR_LBLDEFS.h"
#include "p_st_obj_d_NANR_LBLDEFS.h"
#include "poke_tool/status_rcv.h"
#include "tradedemo.naix"

#include "pokemontrade_local.h"
//#include "app/mailtool.h"

#include "spahead.h"

#include "sound/pm_voice.h"

#include "pokemontrade_demo.cdat"



//-------------------------------------------------
/**
 *	@brief      MCSS�ړ�����
 *	@param[in,out]	_POKEMCSS_MOVE_WORK   �ړ��|�C���^
 *	@param[in]	time   �ړ�����
 *	@param[in]	pPos    �ŏI�ړ���
 */
//-------------------------------------------------

void POKEMONTRADE_pokeMoveRenew(_POKEMCSS_MOVE_WORK* pPoke,int time, const VecFx32* pPos)
{
  {
    VecFx32 apos;
    MCSS_GetPosition(pPoke->pMcss, &apos);
    pPoke->time = time;
    pPoke->nowcount=0;
    GFL_STD_MemCopy(pPos, &pPoke->end, sizeof(VecFx32));
    GFL_STD_MemCopy(&apos, &pPoke->start, sizeof(VecFx32));
  }
}

//-------------------------------------------------
/**
 *	@brief      MCSS�ړ����ߍ쐬
 *	@param[in]	pokeMcss   �Ώ�MCSS
 *	@param[in]	time   �ړ�����
 *	@param[in]	pPos    �ŏI�ړ���
 *	@param[in]	HEAPID  HEAPID
 */
//-------------------------------------------------

_POKEMCSS_MOVE_WORK* POKEMONTRADE_pokeMoveCreate(MCSS_WORK* pokeMcss, int time, const VecFx32* pPos, HEAPID heapID)
{
  _POKEMCSS_MOVE_WORK* pPoke = GFL_HEAP_AllocClearMemory(heapID, sizeof(_POKEMCSS_MOVE_WORK));

  pPoke->pMcss = pokeMcss;
  POKEMONTRADE_pokeMoveRenew(pPoke,time,pPos);
  pPoke->percent=0.0f;
  return pPoke;

}

//-------------------------------------------------
/**
 *	@brief      MCSS�e�[�u���ړ����ߍ쐬
 *	@param[in]	pokeMcss   �Ώ�MCSS
 *	@param[in]	time   �ړ�����
 *	@param[in]	pPos    �ŏI�ړ���
 *	@param[in]	HEAPID  HEAPID
 */
//-------------------------------------------------

_POKEMCSS_MOVE_WORK* POKEMONTRADE_pokeTblMoveCreate(MCSS_WORK* pokeMcss, int time, const VecFx32* pPos, VecFx32* pTbl, HEAPID heapID)
{
  _POKEMCSS_MOVE_WORK* pPoke = GFL_HEAP_AllocClearMemory(heapID, sizeof(_POKEMCSS_MOVE_WORK));

  pPoke->pMcss = pokeMcss;
  POKEMONTRADE_pokeMoveRenew(pPoke,time,pPos);
  pPoke->percent=0.0f;
  pPoke->MoveTbl = pTbl;
  return pPoke;

}


//-------------------------------------------------
/**
 *	@brief      MCSS�ړ����ߎ��s
 *	@param[in]	pokeMcss   �Ώ�MCSS
 *	@param[in]	time   �ړ�����
 *	@param[in]	xend    X�̈ړ���
 *	@param[in]	zend    Z�̈ړ���
 *	@param[in]	HEAPID  HEAPID
 */
//-------------------------------------------------

static fx32 _movemath(fx32 st,fx32 en,_POKEMCSS_MOVE_WORK* pMove,BOOL bWave)
{
  fx32 re;
  re = en - st;
  re = re / pMove->time;
  

  re = st + re * pMove->nowcount;
  if(pMove->wave && bWave){
    re = en + FX_SinIdx(pMove->sins) * _WAVE_NUM;
  }
  return re;
}

//-------------------------------------------------
/**
 *	@brief      �e�[�u���ɂ��ړ��֐�
 *	@param[in]	pMove
 */
//-------------------------------------------------

static void _pokeMoveTblFunc(_POKEMCSS_MOVE_WORK* pMove)
{
  VecFx32 apos,aposold;
  VecFx32* pPMT = &pMove->MoveTbl[pMove->nowcount];
  
  apos.x = pPMT->x + pMove->start.x;
  apos.y = pPMT->y + pMove->start.y;
  apos.z = pPMT->z + pMove->start.z;

  MCSS_SetPosition( pMove->pMcss ,&apos );


}

//-------------------------------------------------
/**
 *	@brief      MCSS�ړ��֐�
 *	@param[in]	pMove   �ړ����[�N
 */
//-------------------------------------------------

void POKEMONTRADE_pokeMoveFunc(_POKEMCSS_MOVE_WORK* pMove)
{
  if(!pMove){
    return;
  }
  pMove->nowcount++;
  if(pMove->time < pMove->nowcount){
    return;
  }
  if(pMove->time != pMove->nowcount)
  {
    if(pMove->MoveTbl){
      _pokeMoveTblFunc(pMove);
      return;
    }
    else{
      VecFx32 apos,aposold;
      MCSS_GetPosition( pMove->pMcss ,&aposold );
      apos.x = _movemath(pMove->start.x, pMove->end.x ,pMove, FALSE);
      if(pMove->wave){
        float an = FX_FX32_TO_F32(aposold.x - apos.x);
        an = an * 374;
        pMove->sins -= (s32)an;
      }
      apos.y = _movemath(pMove->start.y, pMove->end.y ,pMove, TRUE);
      apos.z = _movemath(pMove->start.z, pMove->end.z ,pMove, FALSE);
      MCSS_SetPosition( pMove->pMcss ,&apos );
    }
  }
  else{
    MCSS_SetPosition( pMove->pMcss ,&pMove->end );
  }
}

//MCSS�I���ׂ̈̃R�[���o�b�N
void POKEMONTRADE_McssAnmStop( u32 data, fx32 currentFrame )
{
  POKEMON_TRADE_WORK* pWork = (POKEMON_TRADE_WORK*)data;

  MCSS_SetAnmStopFlag( pWork->pokeMcss[0]);
  pWork->mcssStop[0] = TRUE;
}
