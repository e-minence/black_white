rem �R���o�[�g�ɕK�v�ȏ����Ă͂����Ȃ��t�@�C�����R���o�[�g�p�t�H���_�ɃR�s�[����
copy poke_icon_32k.nce graphic\poke_icon_32k.nce
copy poke_icon_64k.nce graphic\poke_icon_64k.nce
copy poke_icon_128k.nce graphic\poke_icon_128k.nce

cd graphic

rem poke_icon.ncl���e�A�C�R���p�ɃR�s�[���č쐬����
call ncl_copy.bat

rem ���ʂ�nce���R�s�[���đS�Ẵ|�P��������nce�t�@�C�����쐬
call link.bat

rem �R���o�[�g���s
call cnv.bat

rem �A�C�R���̃p���b�g�ԍ��e�[�u������
picon_attr_cnv attr_list.txt

echo g2d�t�@�C���폜
del *_????.NCGR
del *_????.NCER
del *_????.NANR
del poke_icon.NCLR
del icon_???.NCLR
del icon_*.ncl
del *.nce

move *.narc ../
move *.naix ../

cd..

