del *.NCLR
del *.NCGR
del *.NSCR
del *.NCER
del *.NANR

echo g2d�R���o�[�g
g2dcvtr wb_logo_bk.nsc -bg
g2dcvtr wb_logo_bk.ncl -pcm
g2dcvtr wb_logo_bk2.nsc -bg
g2dcvtr wb_logo_bk2.ncl -pcm
g2dcvtr wht_logo.nsc -bg
g2dcvtr wht_base.nsc -bg
g2dcvtr wht_logo.ncl
g2dcvtr blk_logo.nsc -bg
g2dcvtr blk_base.nsc -bg
g2dcvtr blk_logo.ncl
g2dcvtr mist.nsc -bg
g2dcvtr mist.ncl -pcm
g2dcvtr wht_obj.nce
g2dcvtr blk_obj.nce


echo narc�쐬
nnsarc -c -l -n -i title -S title_arc.txt

del *.NCLR
del *.NCGR
del *.NSCR
del *.NCER
del *.NANR
