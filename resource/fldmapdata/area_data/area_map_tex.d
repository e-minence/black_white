# area_map_tex.rb�ɂ�萶������܂���
nsbtx_files/tex_set_map.nsbtx: src_imd_files/area00_tex_set.imd
	@echo src_imd_files/area00_tex_set.imd �� nsbtx_files/tex_set_map.nsbtx
	@$(COPY) src_imd_files/area00_tex_set.imd marged_imd_files/tex_set_map.imd
	@$(G3DCVTR) marged_imd_files/tex_set_map.imd -o nsbtx_files/tex_set_map.nsbtx -etex

nsbtx_files/tex_set_hos1.nsbtx: src_imd_files/testpc00_00c.imd src_imd_files/testroom00_00c.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/testpc00_00c.imd + src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_hos1.nsbtx
	@-rm marged_imd_files/tex_set_hos1.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_hos1.imd src_imd_files/testpc00_00c.imd src_imd_files/testroom00_00c.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_hos1.imd -o nsbtx_files/tex_set_hos1.nsbtx -etex

nsbtx_files/tex_set_out01_spring.nsbtx: src_imd_files/out01_texset_spring.imd src_imd_files/out01_local_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_spring.imd + src_imd_files/out01_local_texset.imd �� nsbtx_files/tex_set_out01_spring.nsbtx
	@-rm marged_imd_files/tex_set_out01_spring.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out01_spring.imd src_imd_files/out01_texset_spring.imd src_imd_files/out01_local_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out01_spring.imd -o nsbtx_files/tex_set_out01_spring.nsbtx -etex

nsbtx_files/tex_set_out01_summer.nsbtx: src_imd_files/out01_texset_summer.imd src_imd_files/out01_local_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_summer.imd + src_imd_files/out01_local_texset.imd �� nsbtx_files/tex_set_out01_summer.nsbtx
	@-rm marged_imd_files/tex_set_out01_summer.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out01_summer.imd src_imd_files/out01_texset_summer.imd src_imd_files/out01_local_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out01_summer.imd -o nsbtx_files/tex_set_out01_summer.nsbtx -etex

nsbtx_files/tex_set_out01_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd src_imd_files/out01_local_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_autumn.imd + src_imd_files/out01_local_texset.imd �� nsbtx_files/tex_set_out01_autumn.nsbtx
	@-rm marged_imd_files/tex_set_out01_autumn.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out01_autumn.imd src_imd_files/out01_texset_autumn.imd src_imd_files/out01_local_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out01_autumn.imd -o nsbtx_files/tex_set_out01_autumn.nsbtx -etex

nsbtx_files/tex_set_out01_winter.nsbtx: src_imd_files/out01_texset_winter.imd src_imd_files/out01_local_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_winter.imd + src_imd_files/out01_local_texset.imd �� nsbtx_files/tex_set_out01_winter.nsbtx
	@-rm marged_imd_files/tex_set_out01_winter.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out01_winter.imd src_imd_files/out01_texset_winter.imd src_imd_files/out01_local_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out01_winter.imd -o nsbtx_files/tex_set_out01_winter.nsbtx -etex

nsbtx_files/tex_set_out02_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out02_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out02_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out02_spring.imd -o nsbtx_files/tex_set_out02_spring.nsbtx -etex

nsbtx_files/tex_set_out02_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out02_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out02_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out02_summer.imd -o nsbtx_files/tex_set_out02_summer.nsbtx -etex

nsbtx_files/tex_set_out02_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out02_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out02_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out02_autumn.imd -o nsbtx_files/tex_set_out02_autumn.nsbtx -etex

nsbtx_files/tex_set_out02_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out02_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out02_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out02_winter.imd -o nsbtx_files/tex_set_out02_winter.nsbtx -etex

nsbtx_files/tex_set_out03_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out03_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out03_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out03_spring.imd -o nsbtx_files/tex_set_out03_spring.nsbtx -etex

nsbtx_files/tex_set_out03_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out03_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out03_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out03_summer.imd -o nsbtx_files/tex_set_out03_summer.nsbtx -etex

nsbtx_files/tex_set_out03_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out03_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out03_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out03_autumn.imd -o nsbtx_files/tex_set_out03_autumn.nsbtx -etex

nsbtx_files/tex_set_out03_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out03_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out03_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out03_winter.imd -o nsbtx_files/tex_set_out03_winter.nsbtx -etex

nsbtx_files/tex_set_out04_spring.nsbtx: src_imd_files/out01_texset_spring.imd src_imd_files/h01_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_spring.imd + src_imd_files/h01_texset.imd �� nsbtx_files/tex_set_out04_spring.nsbtx
	@-rm marged_imd_files/tex_set_out04_spring.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out04_spring.imd src_imd_files/out01_texset_spring.imd src_imd_files/h01_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out04_spring.imd -o nsbtx_files/tex_set_out04_spring.nsbtx -etex

nsbtx_files/tex_set_out04_summer.nsbtx: src_imd_files/out01_texset_summer.imd src_imd_files/h01_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_summer.imd + src_imd_files/h01_texset.imd �� nsbtx_files/tex_set_out04_summer.nsbtx
	@-rm marged_imd_files/tex_set_out04_summer.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out04_summer.imd src_imd_files/out01_texset_summer.imd src_imd_files/h01_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out04_summer.imd -o nsbtx_files/tex_set_out04_summer.nsbtx -etex

nsbtx_files/tex_set_out04_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd src_imd_files/h01_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_autumn.imd + src_imd_files/h01_texset.imd �� nsbtx_files/tex_set_out04_autumn.nsbtx
	@-rm marged_imd_files/tex_set_out04_autumn.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out04_autumn.imd src_imd_files/out01_texset_autumn.imd src_imd_files/h01_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out04_autumn.imd -o nsbtx_files/tex_set_out04_autumn.nsbtx -etex

nsbtx_files/tex_set_out04_winter.nsbtx: src_imd_files/out01_texset_winter.imd src_imd_files/h01_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_winter.imd + src_imd_files/h01_texset.imd �� nsbtx_files/tex_set_out04_winter.nsbtx
	@-rm marged_imd_files/tex_set_out04_winter.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out04_winter.imd src_imd_files/out01_texset_winter.imd src_imd_files/h01_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out04_winter.imd -o nsbtx_files/tex_set_out04_winter.nsbtx -etex

nsbtx_files/tex_set_out05_spring.nsbtx: src_imd_files/tex_c3.imd
	@echo src_imd_files/tex_c3.imd �� nsbtx_files/tex_set_out05_spring.nsbtx
	@$(COPY) src_imd_files/tex_c3.imd marged_imd_files/tex_set_out05_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out05_spring.imd -o nsbtx_files/tex_set_out05_spring.nsbtx -etex

nsbtx_files/tex_set_out05_summer.nsbtx: src_imd_files/tex_c3.imd
	@echo src_imd_files/tex_c3.imd �� nsbtx_files/tex_set_out05_summer.nsbtx
	@$(COPY) src_imd_files/tex_c3.imd marged_imd_files/tex_set_out05_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out05_summer.imd -o nsbtx_files/tex_set_out05_summer.nsbtx -etex

nsbtx_files/tex_set_out05_autumn.nsbtx: src_imd_files/tex_c3.imd
	@echo src_imd_files/tex_c3.imd �� nsbtx_files/tex_set_out05_autumn.nsbtx
	@$(COPY) src_imd_files/tex_c3.imd marged_imd_files/tex_set_out05_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out05_autumn.imd -o nsbtx_files/tex_set_out05_autumn.nsbtx -etex

nsbtx_files/tex_set_out05_winter.nsbtx: src_imd_files/tex_c3.imd
	@echo src_imd_files/tex_c3.imd �� nsbtx_files/tex_set_out05_winter.nsbtx
	@$(COPY) src_imd_files/tex_c3.imd marged_imd_files/tex_set_out05_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out05_winter.imd -o nsbtx_files/tex_set_out05_winter.nsbtx -etex

nsbtx_files/tex_set_out06_spring.nsbtx: src_imd_files/out01_texset_spring.imd src_imd_files/out06_local_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_spring.imd + src_imd_files/out06_local_texset.imd �� nsbtx_files/tex_set_out06_spring.nsbtx
	@-rm marged_imd_files/tex_set_out06_spring.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out06_spring.imd src_imd_files/out01_texset_spring.imd src_imd_files/out06_local_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out06_spring.imd -o nsbtx_files/tex_set_out06_spring.nsbtx -etex

nsbtx_files/tex_set_out06_summer.nsbtx: src_imd_files/out01_texset_summer.imd src_imd_files/out06_local_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_summer.imd + src_imd_files/out06_local_texset.imd �� nsbtx_files/tex_set_out06_summer.nsbtx
	@-rm marged_imd_files/tex_set_out06_summer.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out06_summer.imd src_imd_files/out01_texset_summer.imd src_imd_files/out06_local_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out06_summer.imd -o nsbtx_files/tex_set_out06_summer.nsbtx -etex

nsbtx_files/tex_set_out06_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd src_imd_files/out06_local_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_autumn.imd + src_imd_files/out06_local_texset.imd �� nsbtx_files/tex_set_out06_autumn.nsbtx
	@-rm marged_imd_files/tex_set_out06_autumn.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out06_autumn.imd src_imd_files/out01_texset_autumn.imd src_imd_files/out06_local_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out06_autumn.imd -o nsbtx_files/tex_set_out06_autumn.nsbtx -etex

nsbtx_files/tex_set_out06_winter.nsbtx: src_imd_files/out01_texset_winter.imd src_imd_files/out06_local_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_winter.imd + src_imd_files/out06_local_texset.imd �� nsbtx_files/tex_set_out06_winter.nsbtx
	@-rm marged_imd_files/tex_set_out06_winter.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out06_winter.imd src_imd_files/out01_texset_winter.imd src_imd_files/out06_local_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out06_winter.imd -o nsbtx_files/tex_set_out06_winter.nsbtx -etex

nsbtx_files/tex_set_out07_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out07_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out07_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out07_spring.imd -o nsbtx_files/tex_set_out07_spring.nsbtx -etex

nsbtx_files/tex_set_out07_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out07_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out07_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out07_summer.imd -o nsbtx_files/tex_set_out07_summer.nsbtx -etex

nsbtx_files/tex_set_out07_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out07_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out07_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out07_autumn.imd -o nsbtx_files/tex_set_out07_autumn.nsbtx -etex

nsbtx_files/tex_set_out07_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out07_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out07_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out07_winter.imd -o nsbtx_files/tex_set_out07_winter.nsbtx -etex

nsbtx_files/tex_set_out08_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out08_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out08_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out08_spring.imd -o nsbtx_files/tex_set_out08_spring.nsbtx -etex

nsbtx_files/tex_set_out08_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out08_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out08_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out08_summer.imd -o nsbtx_files/tex_set_out08_summer.nsbtx -etex

nsbtx_files/tex_set_out08_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out08_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out08_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out08_autumn.imd -o nsbtx_files/tex_set_out08_autumn.nsbtx -etex

nsbtx_files/tex_set_out08_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out08_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out08_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out08_winter.imd -o nsbtx_files/tex_set_out08_winter.nsbtx -etex

nsbtx_files/tex_set_out09_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out09_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out09_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out09_spring.imd -o nsbtx_files/tex_set_out09_spring.nsbtx -etex

nsbtx_files/tex_set_out09_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out09_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out09_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out09_summer.imd -o nsbtx_files/tex_set_out09_summer.nsbtx -etex

nsbtx_files/tex_set_out09_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out09_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out09_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out09_autumn.imd -o nsbtx_files/tex_set_out09_autumn.nsbtx -etex

nsbtx_files/tex_set_out09_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out09_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out09_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out09_winter.imd -o nsbtx_files/tex_set_out09_winter.nsbtx -etex

nsbtx_files/tex_set_out10_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out10_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out10_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out10_spring.imd -o nsbtx_files/tex_set_out10_spring.nsbtx -etex

nsbtx_files/tex_set_out10_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out10_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out10_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out10_summer.imd -o nsbtx_files/tex_set_out10_summer.nsbtx -etex

nsbtx_files/tex_set_out10_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out10_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out10_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out10_autumn.imd -o nsbtx_files/tex_set_out10_autumn.nsbtx -etex

nsbtx_files/tex_set_out10_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out10_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out10_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out10_winter.imd -o nsbtx_files/tex_set_out10_winter.nsbtx -etex

nsbtx_files/tex_set_out11_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out11_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out11_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out11_spring.imd -o nsbtx_files/tex_set_out11_spring.nsbtx -etex

nsbtx_files/tex_set_out11_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out11_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out11_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out11_summer.imd -o nsbtx_files/tex_set_out11_summer.nsbtx -etex

nsbtx_files/tex_set_out11_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out11_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out11_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out11_autumn.imd -o nsbtx_files/tex_set_out11_autumn.nsbtx -etex

nsbtx_files/tex_set_out11_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out11_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out11_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out11_winter.imd -o nsbtx_files/tex_set_out11_winter.nsbtx -etex

nsbtx_files/tex_set_out12_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out12_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out12_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out12_spring.imd -o nsbtx_files/tex_set_out12_spring.nsbtx -etex

nsbtx_files/tex_set_out12_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out12_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out12_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out12_summer.imd -o nsbtx_files/tex_set_out12_summer.nsbtx -etex

nsbtx_files/tex_set_out12_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out12_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out12_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out12_autumn.imd -o nsbtx_files/tex_set_out12_autumn.nsbtx -etex

nsbtx_files/tex_set_out12_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out12_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out12_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out12_winter.imd -o nsbtx_files/tex_set_out12_winter.nsbtx -etex

nsbtx_files/tex_set_out13_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out13_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out13_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out13_spring.imd -o nsbtx_files/tex_set_out13_spring.nsbtx -etex

nsbtx_files/tex_set_out13_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out13_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out13_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out13_summer.imd -o nsbtx_files/tex_set_out13_summer.nsbtx -etex

nsbtx_files/tex_set_out13_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out13_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out13_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out13_autumn.imd -o nsbtx_files/tex_set_out13_autumn.nsbtx -etex

nsbtx_files/tex_set_out13_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out13_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out13_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out13_winter.imd -o nsbtx_files/tex_set_out13_winter.nsbtx -etex

nsbtx_files/tex_set_out14_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out14_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out14_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out14_spring.imd -o nsbtx_files/tex_set_out14_spring.nsbtx -etex

nsbtx_files/tex_set_out14_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out14_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out14_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out14_summer.imd -o nsbtx_files/tex_set_out14_summer.nsbtx -etex

nsbtx_files/tex_set_out14_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out14_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out14_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out14_autumn.imd -o nsbtx_files/tex_set_out14_autumn.nsbtx -etex

nsbtx_files/tex_set_out14_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out14_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out14_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out14_winter.imd -o nsbtx_files/tex_set_out14_winter.nsbtx -etex

nsbtx_files/tex_set_out15_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out15_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out15_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out15_spring.imd -o nsbtx_files/tex_set_out15_spring.nsbtx -etex

nsbtx_files/tex_set_out15_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out15_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out15_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out15_summer.imd -o nsbtx_files/tex_set_out15_summer.nsbtx -etex

nsbtx_files/tex_set_out15_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out15_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out15_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out15_autumn.imd -o nsbtx_files/tex_set_out15_autumn.nsbtx -etex

nsbtx_files/tex_set_out15_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out15_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out15_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out15_winter.imd -o nsbtx_files/tex_set_out15_winter.nsbtx -etex

nsbtx_files/tex_set_out16_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out16_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out16_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out16_spring.imd -o nsbtx_files/tex_set_out16_spring.nsbtx -etex

nsbtx_files/tex_set_out16_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out16_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out16_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out16_summer.imd -o nsbtx_files/tex_set_out16_summer.nsbtx -etex

nsbtx_files/tex_set_out16_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out16_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out16_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out16_autumn.imd -o nsbtx_files/tex_set_out16_autumn.nsbtx -etex

nsbtx_files/tex_set_out16_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out16_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out16_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out16_winter.imd -o nsbtx_files/tex_set_out16_winter.nsbtx -etex

nsbtx_files/tex_set_out17_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out17_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out17_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out17_spring.imd -o nsbtx_files/tex_set_out17_spring.nsbtx -etex

nsbtx_files/tex_set_out17_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out17_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out17_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out17_summer.imd -o nsbtx_files/tex_set_out17_summer.nsbtx -etex

nsbtx_files/tex_set_out17_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out17_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out17_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out17_autumn.imd -o nsbtx_files/tex_set_out17_autumn.nsbtx -etex

nsbtx_files/tex_set_out17_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out17_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out17_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out17_winter.imd -o nsbtx_files/tex_set_out17_winter.nsbtx -etex

nsbtx_files/tex_set_out18_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out18_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out18_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out18_spring.imd -o nsbtx_files/tex_set_out18_spring.nsbtx -etex

nsbtx_files/tex_set_out18_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out18_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out18_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out18_summer.imd -o nsbtx_files/tex_set_out18_summer.nsbtx -etex

nsbtx_files/tex_set_out18_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out18_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out18_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out18_autumn.imd -o nsbtx_files/tex_set_out18_autumn.nsbtx -etex

nsbtx_files/tex_set_out18_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out18_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out18_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out18_winter.imd -o nsbtx_files/tex_set_out18_winter.nsbtx -etex

nsbtx_files/tex_set_out19_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out19_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out19_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out19_spring.imd -o nsbtx_files/tex_set_out19_spring.nsbtx -etex

nsbtx_files/tex_set_out19_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out19_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out19_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out19_summer.imd -o nsbtx_files/tex_set_out19_summer.nsbtx -etex

nsbtx_files/tex_set_out19_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out19_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out19_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out19_autumn.imd -o nsbtx_files/tex_set_out19_autumn.nsbtx -etex

nsbtx_files/tex_set_out19_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out19_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out19_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out19_winter.imd -o nsbtx_files/tex_set_out19_winter.nsbtx -etex

nsbtx_files/tex_set_out20_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out20_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out20_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out20_spring.imd -o nsbtx_files/tex_set_out20_spring.nsbtx -etex

nsbtx_files/tex_set_out20_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out20_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out20_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out20_summer.imd -o nsbtx_files/tex_set_out20_summer.nsbtx -etex

nsbtx_files/tex_set_out20_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out20_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out20_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out20_autumn.imd -o nsbtx_files/tex_set_out20_autumn.nsbtx -etex

nsbtx_files/tex_set_out20_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out20_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out20_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out20_winter.imd -o nsbtx_files/tex_set_out20_winter.nsbtx -etex

nsbtx_files/tex_set_out21_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out21_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out21_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out21_spring.imd -o nsbtx_files/tex_set_out21_spring.nsbtx -etex

nsbtx_files/tex_set_out21_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out21_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out21_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out21_summer.imd -o nsbtx_files/tex_set_out21_summer.nsbtx -etex

nsbtx_files/tex_set_out21_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out21_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out21_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out21_autumn.imd -o nsbtx_files/tex_set_out21_autumn.nsbtx -etex

nsbtx_files/tex_set_out21_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out21_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out21_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out21_winter.imd -o nsbtx_files/tex_set_out21_winter.nsbtx -etex

nsbtx_files/tex_set_out22_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out22_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out22_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out22_spring.imd -o nsbtx_files/tex_set_out22_spring.nsbtx -etex

nsbtx_files/tex_set_out22_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out22_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out22_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out22_summer.imd -o nsbtx_files/tex_set_out22_summer.nsbtx -etex

nsbtx_files/tex_set_out22_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out22_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out22_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out22_autumn.imd -o nsbtx_files/tex_set_out22_autumn.nsbtx -etex

nsbtx_files/tex_set_out22_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out22_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out22_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out22_winter.imd -o nsbtx_files/tex_set_out22_winter.nsbtx -etex

nsbtx_files/tex_set_out23_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out23_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out23_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out23_spring.imd -o nsbtx_files/tex_set_out23_spring.nsbtx -etex

nsbtx_files/tex_set_out23_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out23_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out23_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out23_summer.imd -o nsbtx_files/tex_set_out23_summer.nsbtx -etex

nsbtx_files/tex_set_out23_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out23_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out23_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out23_autumn.imd -o nsbtx_files/tex_set_out23_autumn.nsbtx -etex

nsbtx_files/tex_set_out23_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out23_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out23_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out23_winter.imd -o nsbtx_files/tex_set_out23_winter.nsbtx -etex

nsbtx_files/tex_set_out24_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out24_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out24_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out24_spring.imd -o nsbtx_files/tex_set_out24_spring.nsbtx -etex

nsbtx_files/tex_set_out24_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out24_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out24_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out24_summer.imd -o nsbtx_files/tex_set_out24_summer.nsbtx -etex

nsbtx_files/tex_set_out24_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out24_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out24_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out24_autumn.imd -o nsbtx_files/tex_set_out24_autumn.nsbtx -etex

nsbtx_files/tex_set_out24_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out24_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out24_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out24_winter.imd -o nsbtx_files/tex_set_out24_winter.nsbtx -etex

nsbtx_files/tex_set_out25_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out25_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out25_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out25_spring.imd -o nsbtx_files/tex_set_out25_spring.nsbtx -etex

nsbtx_files/tex_set_out25_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out25_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out25_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out25_summer.imd -o nsbtx_files/tex_set_out25_summer.nsbtx -etex

nsbtx_files/tex_set_out25_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out25_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out25_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out25_autumn.imd -o nsbtx_files/tex_set_out25_autumn.nsbtx -etex

nsbtx_files/tex_set_out25_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out25_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out25_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out25_winter.imd -o nsbtx_files/tex_set_out25_winter.nsbtx -etex

nsbtx_files/tex_set_out26_spring.nsbtx: src_imd_files/out01_texset_spring.imd src_imd_files/out26_local_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_spring.imd + src_imd_files/out26_local_texset.imd �� nsbtx_files/tex_set_out26_spring.nsbtx
	@-rm marged_imd_files/tex_set_out26_spring.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out26_spring.imd src_imd_files/out01_texset_spring.imd src_imd_files/out26_local_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out26_spring.imd -o nsbtx_files/tex_set_out26_spring.nsbtx -etex

nsbtx_files/tex_set_out26_summer.nsbtx: src_imd_files/out01_texset_summer.imd src_imd_files/out26_local_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_summer.imd + src_imd_files/out26_local_texset.imd �� nsbtx_files/tex_set_out26_summer.nsbtx
	@-rm marged_imd_files/tex_set_out26_summer.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out26_summer.imd src_imd_files/out01_texset_summer.imd src_imd_files/out26_local_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out26_summer.imd -o nsbtx_files/tex_set_out26_summer.nsbtx -etex

nsbtx_files/tex_set_out26_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd src_imd_files/out26_local_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_autumn.imd + src_imd_files/out26_local_texset.imd �� nsbtx_files/tex_set_out26_autumn.nsbtx
	@-rm marged_imd_files/tex_set_out26_autumn.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out26_autumn.imd src_imd_files/out01_texset_autumn.imd src_imd_files/out26_local_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out26_autumn.imd -o nsbtx_files/tex_set_out26_autumn.nsbtx -etex

nsbtx_files/tex_set_out26_winter.nsbtx: src_imd_files/out01_texset_winter.imd src_imd_files/out26_local_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_winter.imd + src_imd_files/out26_local_texset.imd �� nsbtx_files/tex_set_out26_winter.nsbtx
	@-rm marged_imd_files/tex_set_out26_winter.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out26_winter.imd src_imd_files/out01_texset_winter.imd src_imd_files/out26_local_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out26_winter.imd -o nsbtx_files/tex_set_out26_winter.nsbtx -etex

nsbtx_files/tex_set_out27_spring.nsbtx: src_imd_files/out01_texset_spring.imd src_imd_files/out27_local_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_spring.imd + src_imd_files/out27_local_texset.imd �� nsbtx_files/tex_set_out27_spring.nsbtx
	@-rm marged_imd_files/tex_set_out27_spring.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out27_spring.imd src_imd_files/out01_texset_spring.imd src_imd_files/out27_local_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out27_spring.imd -o nsbtx_files/tex_set_out27_spring.nsbtx -etex

nsbtx_files/tex_set_out27_summer.nsbtx: src_imd_files/out01_texset_summer.imd src_imd_files/out27_local_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_summer.imd + src_imd_files/out27_local_texset.imd �� nsbtx_files/tex_set_out27_summer.nsbtx
	@-rm marged_imd_files/tex_set_out27_summer.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out27_summer.imd src_imd_files/out01_texset_summer.imd src_imd_files/out27_local_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out27_summer.imd -o nsbtx_files/tex_set_out27_summer.nsbtx -etex

nsbtx_files/tex_set_out27_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd src_imd_files/out27_local_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_autumn.imd + src_imd_files/out27_local_texset.imd �� nsbtx_files/tex_set_out27_autumn.nsbtx
	@-rm marged_imd_files/tex_set_out27_autumn.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out27_autumn.imd src_imd_files/out01_texset_autumn.imd src_imd_files/out27_local_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out27_autumn.imd -o nsbtx_files/tex_set_out27_autumn.nsbtx -etex

nsbtx_files/tex_set_out27_winter.nsbtx: src_imd_files/out01_texset_winter.imd src_imd_files/out27_local_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/out01_texset_winter.imd + src_imd_files/out27_local_texset.imd �� nsbtx_files/tex_set_out27_winter.nsbtx
	@-rm marged_imd_files/tex_set_out27_winter.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_out27_winter.imd src_imd_files/out01_texset_winter.imd src_imd_files/out27_local_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out27_winter.imd -o nsbtx_files/tex_set_out27_winter.nsbtx -etex

nsbtx_files/tex_set_out28_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out28_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out28_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out28_spring.imd -o nsbtx_files/tex_set_out28_spring.nsbtx -etex

nsbtx_files/tex_set_out28_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out28_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out28_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out28_summer.imd -o nsbtx_files/tex_set_out28_summer.nsbtx -etex

nsbtx_files/tex_set_out28_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out28_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out28_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out28_autumn.imd -o nsbtx_files/tex_set_out28_autumn.nsbtx -etex

nsbtx_files/tex_set_out28_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out28_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out28_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out28_winter.imd -o nsbtx_files/tex_set_out28_winter.nsbtx -etex

nsbtx_files/tex_set_out29_spring.nsbtx: src_imd_files/out01_texset_spring.imd
	@echo src_imd_files/out01_texset_spring.imd �� nsbtx_files/tex_set_out29_spring.nsbtx
	@$(COPY) src_imd_files/out01_texset_spring.imd marged_imd_files/tex_set_out29_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out29_spring.imd -o nsbtx_files/tex_set_out29_spring.nsbtx -etex

nsbtx_files/tex_set_out29_summer.nsbtx: src_imd_files/out01_texset_summer.imd
	@echo src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_out29_summer.nsbtx
	@$(COPY) src_imd_files/out01_texset_summer.imd marged_imd_files/tex_set_out29_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out29_summer.imd -o nsbtx_files/tex_set_out29_summer.nsbtx -etex

nsbtx_files/tex_set_out29_autumn.nsbtx: src_imd_files/out01_texset_autumn.imd
	@echo src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_out29_autumn.nsbtx
	@$(COPY) src_imd_files/out01_texset_autumn.imd marged_imd_files/tex_set_out29_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out29_autumn.imd -o nsbtx_files/tex_set_out29_autumn.nsbtx -etex

nsbtx_files/tex_set_out29_winter.nsbtx: src_imd_files/out01_texset_winter.imd
	@echo src_imd_files/out01_texset_winter.imd �� nsbtx_files/tex_set_out29_winter.nsbtx
	@$(COPY) src_imd_files/out01_texset_winter.imd marged_imd_files/tex_set_out29_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out29_winter.imd -o nsbtx_files/tex_set_out29_winter.nsbtx -etex

nsbtx_files/tex_set_out30_spring.nsbtx: src_imd_files/out30_local_texset.imd
	@echo src_imd_files/out30_local_texset.imd �� nsbtx_files/tex_set_out30_spring.nsbtx
	@$(COPY) src_imd_files/out30_local_texset.imd marged_imd_files/tex_set_out30_spring.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out30_spring.imd -o nsbtx_files/tex_set_out30_spring.nsbtx -etex

nsbtx_files/tex_set_out30_summer.nsbtx: src_imd_files/out30_local_texset.imd
	@echo src_imd_files/out30_local_texset.imd �� nsbtx_files/tex_set_out30_summer.nsbtx
	@$(COPY) src_imd_files/out30_local_texset.imd marged_imd_files/tex_set_out30_summer.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out30_summer.imd -o nsbtx_files/tex_set_out30_summer.nsbtx -etex

nsbtx_files/tex_set_out30_autumn.nsbtx: src_imd_files/out30_local_texset.imd
	@echo src_imd_files/out30_local_texset.imd �� nsbtx_files/tex_set_out30_autumn.nsbtx
	@$(COPY) src_imd_files/out30_local_texset.imd marged_imd_files/tex_set_out30_autumn.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out30_autumn.imd -o nsbtx_files/tex_set_out30_autumn.nsbtx -etex

nsbtx_files/tex_set_out30_winter.nsbtx: src_imd_files/out30_local_texset.imd
	@echo src_imd_files/out30_local_texset.imd �� nsbtx_files/tex_set_out30_winter.nsbtx
	@$(COPY) src_imd_files/out30_local_texset.imd marged_imd_files/tex_set_out30_winter.imd
	@$(G3DCVTR) marged_imd_files/tex_set_out30_winter.imd -o nsbtx_files/tex_set_out30_winter.nsbtx -etex

nsbtx_files/tex_set_in01.nsbtx: src_imd_files/testroom00_00c.imd src_imd_files/in00_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/testroom00_00c.imd + src_imd_files/in00_texset.imd �� nsbtx_files/tex_set_in01.nsbtx
	@-rm marged_imd_files/tex_set_in01.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_in01.imd src_imd_files/testroom00_00c.imd src_imd_files/in00_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in01.imd -o nsbtx_files/tex_set_in01.nsbtx -etex

nsbtx_files/tex_set_in02.nsbtx: src_imd_files/in02_texset.imd
	@echo src_imd_files/in02_texset.imd �� nsbtx_files/tex_set_in02.nsbtx
	@$(COPY) src_imd_files/in02_texset.imd marged_imd_files/tex_set_in02.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in02.imd -o nsbtx_files/tex_set_in02.nsbtx -etex

nsbtx_files/tex_set_in03.nsbtx: src_imd_files/testpc00_00c.imd src_imd_files/in00_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/testpc00_00c.imd + src_imd_files/in00_texset.imd �� nsbtx_files/tex_set_in03.nsbtx
	@-rm marged_imd_files/tex_set_in03.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_in03.imd src_imd_files/testpc00_00c.imd src_imd_files/in00_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in03.imd -o nsbtx_files/tex_set_in03.nsbtx -etex

nsbtx_files/tex_set_in04.nsbtx: src_imd_files/testroom00_00c.imd src_imd_files/in00_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/testroom00_00c.imd + src_imd_files/in00_texset.imd �� nsbtx_files/tex_set_in04.nsbtx
	@-rm marged_imd_files/tex_set_in04.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_in04.imd src_imd_files/testroom00_00c.imd src_imd_files/in00_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in04.imd -o nsbtx_files/tex_set_in04.nsbtx -etex

nsbtx_files/tex_set_in05.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in05.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in05.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in05.imd -o nsbtx_files/tex_set_in05.nsbtx -etex

nsbtx_files/tex_set_in06.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in06.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in06.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in06.imd -o nsbtx_files/tex_set_in06.nsbtx -etex

nsbtx_files/tex_set_in07.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in07.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in07.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in07.imd -o nsbtx_files/tex_set_in07.nsbtx -etex

nsbtx_files/tex_set_in08.nsbtx: src_imd_files/testroom00_00c.imd src_imd_files/in00_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/testroom00_00c.imd + src_imd_files/in00_texset.imd �� nsbtx_files/tex_set_in08.nsbtx
	@-rm marged_imd_files/tex_set_in08.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_in08.imd src_imd_files/testroom00_00c.imd src_imd_files/in00_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in08.imd -o nsbtx_files/tex_set_in08.nsbtx -etex

nsbtx_files/tex_set_in09.nsbtx: src_imd_files/in09_local_texset.imd src_imd_files/in00_texset.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/in09_local_texset.imd + src_imd_files/in00_texset.imd �� nsbtx_files/tex_set_in09.nsbtx
	@-rm marged_imd_files/tex_set_in09.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_in09.imd src_imd_files/in09_local_texset.imd src_imd_files/in00_texset.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in09.imd -o nsbtx_files/tex_set_in09.nsbtx -etex

nsbtx_files/tex_set_in10.nsbtx: src_imd_files/in10_local_texset.imd
	@echo src_imd_files/in10_local_texset.imd �� nsbtx_files/tex_set_in10.nsbtx
	@$(COPY) src_imd_files/in10_local_texset.imd marged_imd_files/tex_set_in10.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in10.imd -o nsbtx_files/tex_set_in10.nsbtx -etex

nsbtx_files/tex_set_in11.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in11.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in11.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in11.imd -o nsbtx_files/tex_set_in11.nsbtx -etex

nsbtx_files/tex_set_in12.nsbtx: src_imd_files/in12_local_texset.imd
	@echo src_imd_files/in12_local_texset.imd �� nsbtx_files/tex_set_in12.nsbtx
	@$(COPY) src_imd_files/in12_local_texset.imd marged_imd_files/tex_set_in12.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in12.imd -o nsbtx_files/tex_set_in12.nsbtx -etex

nsbtx_files/tex_set_in13.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in13.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in13.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in13.imd -o nsbtx_files/tex_set_in13.nsbtx -etex

nsbtx_files/tex_set_in14.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in14.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in14.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in14.imd -o nsbtx_files/tex_set_in14.nsbtx -etex

nsbtx_files/tex_set_in15.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in15.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in15.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in15.imd -o nsbtx_files/tex_set_in15.nsbtx -etex

nsbtx_files/tex_set_in16.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in16.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in16.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in16.imd -o nsbtx_files/tex_set_in16.nsbtx -etex

nsbtx_files/tex_set_in17.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in17.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in17.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in17.imd -o nsbtx_files/tex_set_in17.nsbtx -etex

nsbtx_files/tex_set_in18.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in18.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in18.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in18.imd -o nsbtx_files/tex_set_in18.nsbtx -etex

nsbtx_files/tex_set_in19.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in19.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in19.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in19.imd -o nsbtx_files/tex_set_in19.nsbtx -etex

nsbtx_files/tex_set_in20.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in20.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in20.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in20.imd -o nsbtx_files/tex_set_in20.nsbtx -etex

nsbtx_files/tex_set_in21.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in21.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in21.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in21.imd -o nsbtx_files/tex_set_in21.nsbtx -etex

nsbtx_files/tex_set_in22.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in22.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in22.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in22.imd -o nsbtx_files/tex_set_in22.nsbtx -etex

nsbtx_files/tex_set_in23.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in23.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in23.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in23.imd -o nsbtx_files/tex_set_in23.nsbtx -etex

nsbtx_files/tex_set_in24.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in24.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in24.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in24.imd -o nsbtx_files/tex_set_in24.nsbtx -etex

nsbtx_files/tex_set_in25.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in25.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in25.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in25.imd -o nsbtx_files/tex_set_in25.nsbtx -etex

nsbtx_files/tex_set_in26.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in26.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in26.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in26.imd -o nsbtx_files/tex_set_in26.nsbtx -etex

nsbtx_files/tex_set_in27.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in27.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in27.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in27.imd -o nsbtx_files/tex_set_in27.nsbtx -etex

nsbtx_files/tex_set_in28.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in28.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in28.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in28.imd -o nsbtx_files/tex_set_in28.nsbtx -etex

nsbtx_files/tex_set_in29.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in29.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in29.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in29.imd -o nsbtx_files/tex_set_in29.nsbtx -etex

nsbtx_files/tex_set_in30.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in30.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in30.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in30.imd -o nsbtx_files/tex_set_in30.nsbtx -etex

nsbtx_files/tex_set_in31.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in31.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in31.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in31.imd -o nsbtx_files/tex_set_in31.nsbtx -etex

nsbtx_files/tex_set_in32.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in32.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in32.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in32.imd -o nsbtx_files/tex_set_in32.nsbtx -etex

nsbtx_files/tex_set_in33.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in33.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in33.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in33.imd -o nsbtx_files/tex_set_in33.nsbtx -etex

nsbtx_files/tex_set_in34.nsbtx: src_imd_files/testroom00_00c.imd
	@echo src_imd_files/testroom00_00c.imd �� nsbtx_files/tex_set_in34.nsbtx
	@$(COPY) src_imd_files/testroom00_00c.imd marged_imd_files/tex_set_in34.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in34.imd -o nsbtx_files/tex_set_in34.nsbtx -etex

nsbtx_files/tex_set_in35.nsbtx: src_imd_files/in35_texset.imd
	@echo src_imd_files/in35_texset.imd �� nsbtx_files/tex_set_in35.nsbtx
	@$(COPY) src_imd_files/in35_texset.imd marged_imd_files/tex_set_in35.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in35.imd -o nsbtx_files/tex_set_in35.nsbtx -etex

nsbtx_files/tex_set_in_pc.nsbtx: src_imd_files/testpc00_00c.imd
	@echo src_imd_files/testpc00_00c.imd �� nsbtx_files/tex_set_in_pc.nsbtx
	@$(COPY) src_imd_files/testpc00_00c.imd marged_imd_files/tex_set_in_pc.imd
	@$(G3DCVTR) marged_imd_files/tex_set_in_pc.imd -o nsbtx_files/tex_set_in_pc.nsbtx -etex

nsbtx_files/tex_set_dun01.nsbtx: src_imd_files/dun01_tex_set.imd src_imd_files/out01_texset_summer.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/dun01_tex_set.imd + src_imd_files/out01_texset_summer.imd �� nsbtx_files/tex_set_dun01.nsbtx
	@-rm marged_imd_files/tex_set_dun01.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_dun01.imd src_imd_files/dun01_tex_set.imd src_imd_files/out01_texset_summer.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_dun01.imd -o nsbtx_files/tex_set_dun01.nsbtx -etex

nsbtx_files/tex_set_dun02.nsbtx: src_imd_files/area00_tex_set.imd src_imd_files/out01_texset_autumn.imd src_imd_files/tex_griddmy.imd
	@echo src_imd_files/area00_tex_set.imd + src_imd_files/out01_texset_autumn.imd �� nsbtx_files/tex_set_dun02.nsbtx
	@-rm marged_imd_files/tex_set_dun02.imd
	@-$(TEX_MAG) -q -o marged_imd_files/tex_set_dun02.imd src_imd_files/area00_tex_set.imd src_imd_files/out01_texset_autumn.imd src_imd_files/tex_griddmy.imd
	@$(G3DCVTR) marged_imd_files/tex_set_dun02.imd -o nsbtx_files/tex_set_dun02.nsbtx -etex

nsbtx_files/tex_set_dun03.nsbtx: src_imd_files/dun03_tex_set.imd
	@echo src_imd_files/dun03_tex_set.imd �� nsbtx_files/tex_set_dun03.nsbtx
	@$(COPY) src_imd_files/dun03_tex_set.imd marged_imd_files/tex_set_dun03.imd
	@$(G3DCVTR) marged_imd_files/tex_set_dun03.imd -o nsbtx_files/tex_set_dun03.nsbtx -etex

nsbtx_files/tex_set_dun04.nsbtx: src_imd_files/dun04_tex_set.imd
	@echo src_imd_files/dun04_tex_set.imd �� nsbtx_files/tex_set_dun04.nsbtx
	@$(COPY) src_imd_files/dun04_tex_set.imd marged_imd_files/tex_set_dun04.imd
	@$(G3DCVTR) marged_imd_files/tex_set_dun04.imd -o nsbtx_files/tex_set_dun04.nsbtx -etex

nsbtx_files/tex_set_test_h01.nsbtx: src_imd_files/test_h01_texset.imd
	@echo src_imd_files/test_h01_texset.imd �� nsbtx_files/tex_set_test_h01.nsbtx
	@$(COPY) src_imd_files/test_h01_texset.imd marged_imd_files/tex_set_test_h01.imd
	@$(G3DCVTR) marged_imd_files/tex_set_test_h01.imd -o nsbtx_files/tex_set_test_h01.nsbtx -etex

nsbtx_files/tex_set_test_h02.nsbtx: src_imd_files/test_h02_texset.imd
	@echo src_imd_files/test_h02_texset.imd �� nsbtx_files/tex_set_test_h02.nsbtx
	@$(COPY) src_imd_files/test_h02_texset.imd marged_imd_files/tex_set_test_h02.imd
	@$(G3DCVTR) marged_imd_files/tex_set_test_h02.imd -o nsbtx_files/tex_set_test_h02.nsbtx -etex

nsbtx_files/tex_set_loopbridge.nsbtx: src_imd_files/loopbridge.imd
	@echo src_imd_files/loopbridge.imd �� nsbtx_files/tex_set_loopbridge.nsbtx
	@$(COPY) src_imd_files/loopbridge.imd marged_imd_files/tex_set_loopbridge.imd
	@$(G3DCVTR) marged_imd_files/tex_set_loopbridge.imd -o nsbtx_files/tex_set_loopbridge.nsbtx -etex

