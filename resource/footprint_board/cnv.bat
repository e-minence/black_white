del *.NCLR
del *.NCGR
del *.NSCR
del *.NCER
del *.NANR
del *.naix
del *.narc


echo g2d�R���o�[�g
g2dcvtr a_board_eff.nce -br
g2dcvtr a_board_sita.nsc -bg
g2dcvtr a_board_sita_bg.nsc -bg
g2dcvtr ashiato_board.nsc -bg
g2dcvtr ashiato_board_bg.nsc -bg
g2dcvtr ashiato_frame.nce -br -ncn
g2dcvtr ashiato_gage.nce -br -ncn
g2dcvtr foot_dummy.nce -br -ncn
g2dcvtr wifi_mark.nce -br -ncn
g2dcvtr a_board_font_s.ncl
g2dcvtr a_board_font_b.ncl


echo g3d�R���o�[�g
g3dcvtr a_board1.imd
g3dcvtr a_board2.imd
g3dcvtr ashiato.imd


echo narc�쐬
nnsarc -c -l -n -i footprint_board -S footprint_board.txt


echo g2d�t�@�C���폜
del *.NCLR
del *.NCGR
del *.NSCR
del *.NCER
del *.NANR
del *.nsbmd
