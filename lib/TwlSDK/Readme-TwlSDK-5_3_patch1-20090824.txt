������������������������������������������������������������������������������
��                                                                          ��
��  Readme-TwlSDK-5_3_patch1-20090824.txt                                   ��
��                                                                          ��
��  Plus Patch for TWL-SDK 5.3                                              ��
��                                                                          ��
��  Aug. 24, 2009                                                           ��
��                                                                          ��
������������������������������������������������������������������������������


�y �͂��߂� �z

    ���̃p�b�`�́ATWL-SDK 5.3 ������ �̃����[�X�ȍ~�ɔ������ꂽ�s����C����
    �邽�߂̂��̂ł��B�C���X�g�[�����邽�߂ɂ́A��������Ă���t�@�C����
    TWL-SDK 5.3 ������ (090807)���C���X�g�[������Ă���f�B���N�g���ɏ㏑���R
    �s�[���Ă��������B


�y ���� �z

    TwlSDK �̃��C�u�������r���h������������ꍇ�A�{�p�b�`�𓖂Ă��������ƃL��
    �b�V���̉e���Ńv���O�������r���h����Ƃ��ɃG���[����������\��������܂��B
    �����r���h���ɃG���[�����������ꍇ�́ATwlSDK �̃��[�g�f�B���N�g����
    �umake clobber�v�����s���邱�ƂŃL���b�V�����폜����܂��B
    ���̌�A���� TwlSDK �̃��[�g�f�B���N�g���Łumake�v���邱�ƂŁA���̏Ǐ�͉�
    �P����܂��B


�y �t�@�C�����X�g �z
���\�[�X�t�@�C��
	/TwlSDK/build/libraries/init/ARM9.TWL/src/crt0.FLX.c
	/TwlSDK/build/libraries/os/ARM9/src/os_china.c
	/TwlSDK/build/libraries/os/common/src/os_init.c

���w�b�_�t�@�C��
	/TwlSDK/include/nitro/version.h
	/TwlSDK/include/twl/version.h

�����C�u�����t�@�C��
	/TwlSDK/lib/ARM9-TS/Debug/crt0.HYB.TWL.o
	/TwlSDK/lib/ARM9-TS/Debug/crt0.LTD.TWL.o
	/TwlSDK/lib/ARM9-TS/Debug/crt0.o
	/TwlSDK/lib/ARM9-TS/Debug/libos.TWL.LTD.a
	/TwlSDK/lib/ARM9-TS/Debug/libos.TWL.LTD.thumb.a
	/TwlSDK/lib/ARM9-TS/Release/crt0.HYB.TWL.o
	/TwlSDK/lib/ARM9-TS/Release/crt0.LTD.TWL.o
	/TwlSDK/lib/ARM9-TS/Release/crt0.o
	/TwlSDK/lib/ARM9-TS/Release/libos.TWL.LTD.a
	/TwlSDK/lib/ARM9-TS/Release/libos.TWL.LTD.thumb.a
	/TwlSDK/lib/ARM9-TS/Rom/crt0.HYB.TWL.o
	/TwlSDK/lib/ARM9-TS/Rom/crt0.LTD.TWL.o
	/TwlSDK/lib/ARM9-TS/Rom/crt0.o
	/TwlSDK/lib/ARM9-TS/Rom/libos.TWL.LTD.a
	/TwlSDK/lib/ARM9-TS/Rom/libos.TWL.LTD.thumb.a

���R���|�[�l���g�t�@�C��
	/TwlSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub.nef
	/TwlSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub.nef
	/TwlSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub.nef
	/TwlSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub.nef
	/TwlSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub.nef
	/TwlSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub.nef
	/TwlSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub.nef
	/TwlSDK/components/mongoose/ARM7-TS/Release/mongoose_sub.nef
	/TwlSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub.nef
	/TwlSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub.nef
	/TwlSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub.nef
	/TwlSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub.nef
	/TwlSDK/components/ferret/ARM7-TS.LTD/Debug/ferret.tef
	/TwlSDK/components/ferret/ARM7-TS.LTD/Release/ferret.tef
	/TwlSDK/components/ferret/ARM7-TS.LTD/Rom/ferret.tef
	/TwlSDK/components/ferret/ARM7-TS.LTD.thumb/Debug/ferret.tef
	/TwlSDK/components/ferret/ARM7-TS.LTD.thumb/Release/ferret.tef
	/TwlSDK/components/ferret/ARM7-TS.LTD.thumb/Rom/ferret.tef
	/TwlSDK/components/ichneumon/ARM7-TS.HYB/Debug/ichneumon.tef
	/TwlSDK/components/ichneumon/ARM7-TS.HYB/Release/ichneumon.tef
	/TwlSDK/components/ichneumon/ARM7-TS.HYB/Rom/ichneumon.tef
	/TwlSDK/components/ichneumon/ARM7-TS.HYB.thumb/Debug/ichneumon.tef
	/TwlSDK/components/ichneumon/ARM7-TS.HYB.thumb/Release/ichneumon.tef
	/TwlSDK/components/ichneumon/ARM7-TS.HYB.thumb/Rom/ichneumon.tef
	/TwlSDK/components/mongoose/ARM7-TS.HYB/Debug/mongoose.tef
	/TwlSDK/components/mongoose/ARM7-TS.HYB/Release/mongoose.tef
	/TwlSDK/components/mongoose/ARM7-TS.HYB/Rom/mongoose.tef
	/TwlSDK/components/mongoose/ARM7-TS.HYB.thumb/Debug/mongoose.tef
	/TwlSDK/components/mongoose/ARM7-TS.HYB.thumb/Release/mongoose.tef
	/TwlSDK/components/mongoose/ARM7-TS.HYB.thumb/Rom/mongoose.tef
	/TwlSDK/components/racoon/ARM7-TS.LTD/Debug/racoon.tef
	/TwlSDK/components/racoon/ARM7-TS.LTD/Release/racoon.tef
	/TwlSDK/components/racoon/ARM7-TS.LTD/Rom/racoon.tef
	/TwlSDK/components/racoon/ARM7-TS.LTD.thumb/Debug/racoon.tef
	/TwlSDK/components/racoon/ARM7-TS.LTD.thumb/Release/racoon.tef
	/TwlSDK/components/racoon/ARM7-TS.LTD.thumb/Rom/racoon.tef
	/TwlSDK/components/ferret/ARM7-TS.LTD/Debug/ferret.TWL.FLX.sbin
	/TwlSDK/components/ferret/ARM7-TS.LTD/Release/ferret.TWL.FLX.sbin
	/TwlSDK/components/ferret/ARM7-TS.LTD/Rom/ferret.TWL.FLX.sbin
	/TwlSDK/components/ferret/ARM7-TS.LTD.thumb/Debug/ferret.TWL.FLX.sbin
	/TwlSDK/components/ferret/ARM7-TS.LTD.thumb/Release/ferret.TWL.FLX.sbin
	/TwlSDK/components/ferret/ARM7-TS.LTD.thumb/Rom/ferret.TWL.FLX.sbin
	/TwlSDK/components/ichneumon/ARM7-TS.HYB/Debug/ichneumon.TWL.FLX.sbin
	/TwlSDK/components/ichneumon/ARM7-TS.HYB/Release/ichneumon.TWL.FLX.sbin
	/TwlSDK/components/ichneumon/ARM7-TS.HYB/Rom/ichneumon.TWL.FLX.sbin
	/TwlSDK/components/ichneumon/ARM7-TS.HYB.thumb/Debug/ichneumon.TWL.FLX.sbin
	/TwlSDK/components/ichneumon/ARM7-TS.HYB.thumb/Release/ichneumon.TWL.FLX.sbin
	/TwlSDK/components/ichneumon/ARM7-TS.HYB.thumb/Rom/ichneumon.TWL.FLX.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB/Debug/mongoose.TWL.FLX.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB/Release/mongoose.TWL.FLX.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB/Rom/mongoose.TWL.FLX.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB.thumb/Debug/mongoose.TWL.FLX.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB.thumb/Release/mongoose.TWL.FLX.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB.thumb/Rom/mongoose.TWL.FLX.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD/Debug/racoon.TWL.FLX.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD/Release/racoon.TWL.FLX.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD/Rom/racoon.TWL.FLX.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD.thumb/Debug/racoon.TWL.FLX.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD.thumb/Release/racoon.TWL.FLX.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD.thumb/Rom/racoon.TWL.FLX.sbin

���h�L�������g�E���̑�
	/TwlSDK/bin/ARM9-TS/Rom/NandFiler.srl
	/TwlSDK/bin/ARM9-TS/Rom/NandFiler.tad
	/TwlSDK/bin/ARM9-TS/Rom/TwlNmenu.srl
	/TwlSDK/bin/ARM9-TS/Rom/TwlNmenu.tad


�ȏ�
