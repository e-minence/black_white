rem �R���o�[�g�ɕK�v�ȏ����Ă͂����Ȃ��t�@�C�����R���o�[�g�p�t�H���_�ɃR�s�[����
copy poke_icon_32k.nce graphic\poke_icon_32k.nce
copy poke_icon_64k.nce graphic\poke_icon_64k.nce
copy poke_icon_128k.nce graphic\poke_icon_128k.nce

cd graphic

rem poke_icon.ncl���e�A�C�R���p�ɃR�s�[���č쐬����
rem call ncl_copy.bat�����Ԃ����ł������͂��@poke_icon.ncl�ƃ����N���Ƃ��΂���ŃR���o�[�g����͂�

rem ���ʂ�nce���R�s�[���đS�Ẵ|�P��������nce�t�@�C�����쐬
rem call link.bat

rem �R���o�[�g���s
rem call cnv.bat

rem �A�C�R���̃p���b�g�ԍ��e�[�u������
rem picon_attr_cnv attr_list.txt

rem ��O�̃o�b�`�t�@�C�����P��Ruby�X�N���v�g�Ŏ��s����
ruby poke_icon_conv.rb attr_list.txt ..\poke_icon.narc

rem �̌��łŎg�p���Ȃ��A�C�R�����u�H�v�ɒu������
ruby -Ks playable_replace.rb

rem del *.naix
rem del *.narc

echo g2d�R���o�[�g
g2dcvtr poke_icon_32k.nce -br
g2dcvtr poke_icon_64k.nce -br
g2dcvtr poke_icon_128k.nce -br

g2dcvtr poke_icon.ncl -pcm

echo narc�쐬
nnsarc -c -l -n -i poke_icon -S pokeicon_all.scr

echo g2d�t�@�C���폜
rem del *_????.NCGR
rem del *_????.NCER
rem del *_????.NANR
rem del poke_icon.NCLR
rem del icon_???.NCLR
rem del icon_*.ncl
del *.nce

move *.narc ../
move *.naix ../

cd..

