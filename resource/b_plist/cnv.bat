del *.NCLR
del *.NCGR
del *.NSCR
del *.NCER
del *.NANR
del *.naix
del *.narc


echo g2d�R���o�[�g
g2dcvtr poke_sel_u.nsc
g2dcvtr poke_sel_t.nsc
g2dcvtr st_waza_sel_u.nsc
g2dcvtr st_waza_sel_t.nsc
g2dcvtr st_main_u.nsc
g2dcvtr st_main_t.nsc
g2dcvtr st_waza_main_u.nsc
g2dcvtr st_waza_main_t.nsc
g2dcvtr item_waza_u.nsc
g2dcvtr item_waza_t.nsc
g2dcvtr waza_delb_sel_u.nsc
g2dcvtr waza_delb_sel_t.nsc
g2dcvtr waza_delc_sel_u.nsc
g2dcvtr waza_delc_sel_t.nsc
g2dcvtr waza_delb_main_u.nsc
g2dcvtr waza_delb_main_t.nsc
g2dcvtr waza_delc_main_u.nsc
g2dcvtr waza_delc_main_t.nsc
g2dcvtr poke_chg_u.nsc
g2dcvtr poke_chg_t.nsc
g2dcvtr b_plist_anm.nsc
g2dcvtr b_plist_anm2.nsc
g2dcvtr b_plist.ncg -bg
g2dcvtr b_plist.ncl
g2dcvtr b_plist_obj.nce -br
g2dcvtr b_plist_obj.ncl


echo narc�쐬
nnsarc -c -l -n -i b_plist_gra -S b_plist.txt

echo �f�[�^�R�s�[
copy b_plist_gra.naix ..\..\pokemon_gs\src\battle
copy b_plist_gra.narc ..\..\pokemon_gs\src\battle\graphic\b_plist_gra_gs.narc

echo g2d�t�@�C���폜
del *.NCLR
del *.NCGR
del *.NSCR
del *.NCER
del *.NANR
