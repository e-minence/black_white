del *.NCLR
del *.NCGR
del *.NSCR
del *.NCER
del *.NANR
del *.naix
del *.narc


echo g2d�R���o�[�g
g2dcvtr cc_circle.nce -br -ncn
g2dcvtr cc_back_s.nsc -bg
g2dcvtr cc_back_m.nsc -bg
g2dcvtr cc_count_m.nsc -bg
g2dcvtr cc_circle_light.ncl


echo narc�쐬
nnsarc -c -l -n -i compati_check -S arc_list.txt


echo g2d�t�@�C���폜
del *.NCLR
del *.NCGR
del *.NCER
del *.NANR
