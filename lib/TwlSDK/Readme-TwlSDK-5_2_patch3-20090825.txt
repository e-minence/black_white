������������������������������������������������������������������������������
��                                                                          ��
��  Readme-TwlSDK-5_2_patch3-20090825.txt                                   ��
��                                                                          ��
��  Plus Patch for TWL-SDK 5.2                                              ��
��                                                                          ��
��  Aug. 25, 2009                                                           ��
��                                                                          ��
������������������������������������������������������������������������������


�y �͂��߂� �z

    ���̃p�b�`�́ATWL-SDK 5.2 ������ �̃����[�X�ȍ~�ɔ������ꂽ�s����C����
    �邽�߂̂��̂ł��B�C���X�g�[�����邽�߂ɂ́A��������Ă���t�@�C����
    TWL-SDK 5.2 ������ (090615)���C���X�g�[������Ă���f�B���N�g���ɏ㏑���R
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
	/TwlSDK/build/libraries/snd/ARM9.TWL/src/sndex.c

���w�b�_�t�@�C��
	/TwlSDK/include/nitro/version.h
	/TwlSDK/include/twl/version.h

�����C�u�����t�@�C��
	/TwlSDK/lib/ARM9-TS/Debug/crt0.HYB.TWL.o
	/TwlSDK/lib/ARM9-TS/Debug/crt0.LTD.TWL.o
	/TwlSDK/lib/ARM9-TS/Debug/crt0.o
	/TwlSDK/lib/ARM9-TS/Debug/libna.TWL.HYB.a
	/TwlSDK/lib/ARM9-TS/Debug/libna.TWL.HYB.thumb.a
	/TwlSDK/lib/ARM9-TS/Debug/libna.TWL.LTD.a
	/TwlSDK/lib/ARM9-TS/Debug/libna.TWL.LTD.thumb.a
	/TwlSDK/lib/ARM9-TS/Debug/libos.TWL.LTD.a
	/TwlSDK/lib/ARM9-TS/Debug/libos.TWL.LTD.thumb.a
	/TwlSDK/lib/ARM9-TS/Debug/libsndex.TWL.HYB.a
	/TwlSDK/lib/ARM9-TS/Debug/libsndex.TWL.HYB.thumb.a
	/TwlSDK/lib/ARM9-TS/Debug/libsndex.TWL.LTD.a
	/TwlSDK/lib/ARM9-TS/Debug/libsndex.TWL.LTD.thumb.a
	/TwlSDK/lib/ARM9-TS/Release/crt0.HYB.TWL.o
	/TwlSDK/lib/ARM9-TS/Release/crt0.LTD.TWL.o
	/TwlSDK/lib/ARM9-TS/Release/crt0.o
	/TwlSDK/lib/ARM9-TS/Release/libna.TWL.HYB.a
	/TwlSDK/lib/ARM9-TS/Release/libna.TWL.HYB.thumb.a
	/TwlSDK/lib/ARM9-TS/Release/libna.TWL.LTD.a
	/TwlSDK/lib/ARM9-TS/Release/libna.TWL.LTD.thumb.a
	/TwlSDK/lib/ARM9-TS/Release/libos.TWL.LTD.a
	/TwlSDK/lib/ARM9-TS/Release/libos.TWL.LTD.thumb.a
	/TwlSDK/lib/ARM9-TS/Release/libsndex.TWL.HYB.a
	/TwlSDK/lib/ARM9-TS/Release/libsndex.TWL.HYB.thumb.a
	/TwlSDK/lib/ARM9-TS/Release/libsndex.TWL.LTD.a
	/TwlSDK/lib/ARM9-TS/Release/libsndex.TWL.LTD.thumb.a
	/TwlSDK/lib/ARM9-TS/Rom/crt0.HYB.TWL.o
	/TwlSDK/lib/ARM9-TS/Rom/crt0.LTD.TWL.o
	/TwlSDK/lib/ARM9-TS/Rom/crt0.o
	/TwlSDK/lib/ARM9-TS/Rom/libna.TWL.HYB.a
	/TwlSDK/lib/ARM9-TS/Rom/libna.TWL.HYB.thumb.a
	/TwlSDK/lib/ARM9-TS/Rom/libna.TWL.LTD.a
	/TwlSDK/lib/ARM9-TS/Rom/libna.TWL.LTD.thumb.a
	/TwlSDK/lib/ARM9-TS/Rom/libos.TWL.LTD.a
	/TwlSDK/lib/ARM9-TS/Rom/libos.TWL.LTD.thumb.a
	/TwlSDK/lib/ARM9-TS/Rom/libsndex.TWL.HYB.a
	/TwlSDK/lib/ARM9-TS/Rom/libsndex.TWL.HYB.thumb.a
	/TwlSDK/lib/ARM9-TS/Rom/libsndex.TWL.LTD.a
	/TwlSDK/lib/ARM9-TS/Rom/libsndex.TWL.LTD.thumb.a

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
	/TwlSDK/components/ferret/ARM7-TS.LTD/Debug/ferret.TWL.LTD.sbin
	/TwlSDK/components/ferret/ARM7-TS.LTD/Debug/ferret_defs.TWL.LTD.sbin
	/TwlSDK/components/ferret/ARM7-TS.LTD/Release/ferret.TWL.FLX.sbin
	/TwlSDK/components/ferret/ARM7-TS.LTD/Release/ferret.TWL.LTD.sbin
	/TwlSDK/components/ferret/ARM7-TS.LTD/Release/ferret_defs.TWL.LTD.sbin
	/TwlSDK/components/ferret/ARM7-TS.LTD/Rom/ferret.TWL.FLX.sbin
	/TwlSDK/components/ferret/ARM7-TS.LTD.thumb/Debug/ferret.TWL.FLX.sbin
	/TwlSDK/components/ferret/ARM7-TS.LTD.thumb/Debug/ferret.TWL.LTD.sbin
	/TwlSDK/components/ferret/ARM7-TS.LTD.thumb/Debug/ferret_defs.TWL.LTD.sbin
	/TwlSDK/components/ferret/ARM7-TS.LTD.thumb/Release/ferret.TWL.FLX.sbin
	/TwlSDK/components/ferret/ARM7-TS.LTD.thumb/Release/ferret.TWL.LTD.sbin
	/TwlSDK/components/ferret/ARM7-TS.LTD.thumb/Release/ferret_defs.TWL.LTD.sbin
	/TwlSDK/components/ferret/ARM7-TS.LTD.thumb/Rom/ferret.TWL.FLX.sbin
	/TwlSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub.sbin
	/TwlSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_defs.sbin
	/TwlSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub.sbin
	/TwlSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_defs.sbin
	/TwlSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub.sbin
	/TwlSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_defs.sbin
	/TwlSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub.sbin
	/TwlSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_defs.sbin
	/TwlSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub.sbin
	/TwlSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_defs.sbin
	/TwlSDK/components/mongoose/ARM7-TS/Release/mongoose_sub.sbin
	/TwlSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_defs.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB/Debug/mongoose.TWL.FLX.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB/Debug/mongoose.TWL.LTD.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB/Debug/mongoose_defs.TWL.FLX.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB/Release/mongoose.TWL.FLX.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB/Release/mongoose.TWL.LTD.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB/Release/mongoose_defs.TWL.FLX.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB/Rom/mongoose.TWL.FLX.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB/Rom/mongoose.TWL.LTD.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB.thumb/Debug/mongoose.TWL.FLX.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB.thumb/Debug/mongoose.TWL.LTD.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB.thumb/Debug/mongoose_defs.TWL.FLX.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB.thumb/Release/mongoose.TWL.FLX.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB.thumb/Release/mongoose.TWL.LTD.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB.thumb/Release/mongoose_defs.TWL.FLX.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB.thumb/Rom/mongoose.TWL.FLX.sbin
	/TwlSDK/components/mongoose/ARM7-TS.HYB.thumb/Rom/mongoose.TWL.LTD.sbin
	/TwlSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub.sbin
	/TwlSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_defs.sbin
	/TwlSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub.sbin
	/TwlSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_defs.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD/Debug/racoon.TWL.FLX.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD/Debug/racoon.TWL.LTD.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD/Debug/racoon_defs.TWL.LTD.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD/Release/racoon.TWL.FLX.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD/Release/racoon.TWL.LTD.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD/Release/racoon_defs.TWL.LTD.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD/Rom/racoon.TWL.FLX.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD.thumb/Debug/racoon.TWL.FLX.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD.thumb/Debug/racoon.TWL.LTD.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD.thumb/Debug/racoon_defs.TWL.LTD.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD.thumb/Release/racoon.TWL.FLX.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD.thumb/Release/racoon.TWL.LTD.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD.thumb/Release/racoon_defs.TWL.LTD.sbin
	/TwlSDK/components/racoon/ARM7-TS.LTD.thumb/Rom/racoon.TWL.FLX.sbin

���h�L�������g�E���̑�
	/TwlSDK/bin/ARM9-TS/Rom/NandFiler.srl
	/TwlSDK/bin/ARM9-TS/Rom/NandFiler.tad
	/TwlSDK/bin/ARM9-TS/Rom/TwlNmenu.srl
	/TwlSDK/bin/ARM9-TS/Rom/TwlNmenu.tad
	/TwlSDK/man/ja_JP/os/argument/about_argument.html
	/TwlSDK/man/ja_JP/os/argument/OS_GetArgc.html
	/TwlSDK/man/ja_JP/os/argument/OS_GetArgv.html


�ȏ�
