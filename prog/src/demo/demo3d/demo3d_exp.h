/*
 *  @file   demo3d_exp.h
 *  @brief  demo3d�@��O������`�p�܂Ƃ߃w�b�_
 *  @author hosaka,iwasawa
 *  @date   10.02.25
 */

// �s���S�^
typedef struct _APP_EXCEPTION_WORK APP_EXCEPTION_WORK;

//--------------------------------------------------------------
/// ��O���� �֐���`
//==============================================================
typedef void (*APP_EXCEPTION_FUNC)( APP_EXCEPTION_WORK* wk );

//--------------------------------------------------------------
/// ��O���� �֐��e�[�u��
//==============================================================
typedef struct {
  APP_EXCEPTION_FUNC Init;
  APP_EXCEPTION_FUNC Main;
  APP_EXCEPTION_FUNC End;
} APP_EXCEPTION_FUNC_SET;

//--------------------------------------------------------------
///	�A�v����O����
//==============================================================
struct _APP_EXCEPTION_WORK {
  //[IN]
  HEAPID heapID;
  const DEMO3D_GRAPHIC_WORK* graphic;
  const DEMO3D_ENGINE_WORK* engine;
  //[PRIVATE]
  const APP_EXCEPTION_FUNC_SET* p_funcset;
  void *userwork;
  int  key_skip;  // �L�[�X�L�b�v
};

//////////////////////////////////////////////////////////////
//�f���ʒ�`


//�V���D�f����O�����e�[�u��
extern const APP_EXCEPTION_FUNC_SET DATA_c_exp_funcset_c_cruiser;
