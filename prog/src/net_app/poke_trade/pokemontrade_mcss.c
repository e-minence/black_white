

//-------------------------------------------------
/**
 *	@brief      MCSS�ړ�����
 *	@param[in,out]	_POKEMCSS_MOVE_WORK   �ړ��|�C���^
 *	@param[in]	time   �ړ�����
 *	@param[in]	pPos    �ŏI�ړ���
 */
//-------------------------------------------------

static void _pokeMoveRenew(_POKEMCSS_MOVE_WORK* pPoke,int time, const VecFx32* pPos)
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

static _POKEMCSS_MOVE_WORK* _pokeMoveCreate(MCSS_WORK* pokeMcss, int time, const VecFx32* pPos, HEAPID heapID)
{
  _POKEMCSS_MOVE_WORK* pPoke = GFL_HEAP_AllocClearMemory(heapID, sizeof(_POKEMCSS_MOVE_WORK));

  pPoke->pMcss = pokeMcss;
  _pokeMoveRenew(pPoke,time,pPos);
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

static _POKEMCSS_MOVE_WORK* _pokeTblMoveCreate(MCSS_WORK* pokeMcss, int time, const VecFx32* pPos, VecFx32* pTbl, HEAPID heapID)
{
  _POKEMCSS_MOVE_WORK* pPoke = GFL_HEAP_AllocClearMemory(heapID, sizeof(_POKEMCSS_MOVE_WORK));

  pPoke->pMcss = pokeMcss;
  _pokeMoveRenew(pPoke,time,pPos);
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

static void _pokeMoveFunc(_POKEMCSS_MOVE_WORK* pMove)
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
static void _McssAnmStop( u32 data, fx32 currentFrame )
{
  POKEMON_TRADE_WORK* pWork = (POKEMON_TRADE_WORK*)data;

  MCSS_SetAnmStopFlag( pWork->pokeMcss[0]);
  pWork->mcssStop[0] = TRUE;
}
