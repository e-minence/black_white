del *.NCLR
del *.NCGR
del *.NSCR
del *.NCER
del *.NANR

echo g2d�R���o�[�g
g2dcvtr wb_logo_bk.nsc -bg
g2dcvtr wb_logo_bk.ncl -pcm
g2dcvtr mist.nsc -bg
g2dcvtr mist.ncl -pcm

echo narc�쐬
nnsarc -c -l -n -i title -S title_arc.txt

del *.NCLR
del *.NCGR
del *.NSCR
del *.NCER
del *.NANR
