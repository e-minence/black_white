#=======================================================================
# land_list.rb
# �n�`�f�[�^�ꗗ���쐬����B
# land_list.rb land.csv..
# land.csv �e�L�X�g�ϊ��ς݂̒n�`�f�[�^�Ǘ��\ �����w��\
#=======================================================================

#=======================================================================
# �萔
#=======================================================================
#�쐬����t�@�C����
FNAME_OUTPUT_LIST = "land_output_list"
FNAME_MODEL_LIST = "land_model_list"
FNAME_DEPEND_LIST = "land_depend_list"
FNAME_MAKE_DEPEND = "land_make_depend"

#land_output�f�B���N�g����
DIR_OUTPUT = "land_output"
DIR_RES = "land_res"

#makefile�Œ�`����Ă���e�f�B���N�g����
DIRSTR_OUTPUT = "$(DIR_OUTPUT)"
DIRSTR_TEMP = "$(DIR_TEMP)"
DIRSTR_RES = "$(DIR_RES)"

#makefile�Ŏg�p���郉�x����
LNAME_MODEL_LIST = "LAND_MODEL_LIST"
LNAME_DEPEND_LIST = "LAND_DEPEND_LIST"

#makefile�Œ�`����Ă��镶����
STR_BINLINKER = "$(BINLINKER)"
STR_G3DCVTR = "$(G3DCVTR)"

#�_�~�[�t�@�C��
FNAME_DMYFILE_GRID_3DMD = "dmygmap.3dmd"
FNAME_DMYFILE_GRID_BIN = "dmygmap.bin"
FNAME_DMYFILE_GRID_IMD = "dmygmap.imd"

#=======================================================================
#	�t�@�C���R�s�[
#=======================================================================
def file_copy( srcpath, copypath )
	open( srcpath, "rb" ){ |input|
		open( copypath, "wb" ){ |output|
			output.write( input.read )
		}
	}
end

#=======================================================================
# �ꗗ�쐬
#=======================================================================
file_output_list = File.open( FNAME_OUTPUT_LIST, "w" )
file_model_list = File.open( FNAME_MODEL_LIST, "w" )
file_depend_list = File.open( FNAME_DEPEND_LIST, "w" )
file_make_depend = File.open( FNAME_MAKE_DEPEND, "w" )

file_model_list.printf( "%s = \\\n", LNAME_MODEL_LIST )
file_depend_list.printf( "%s = \\\n", LNAME_DEPEND_LIST )

yen_flg = 0

while file_name = ARGV.shift
	file_csv = File.open( file_name, "r" )
  
	line = file_csv.gets #�f�[�^�J�n�ʒu�܂Ői�߂�
	line = file_csv.gets
	line = file_csv.gets
  
	while line = file_csv.gets
		if( line =~ /^#END/ )
			break
		end
    
		if( yen_flg != 0 )
			file_model_list.printf( " \\\n" )
			file_depend_list.printf( " \\\n" )
		end
    
		yen_flg = 1
		column = line.split ","
		name = column[1]
    type = column[2]

    #land_output_list
		file_output_list.printf( "\"%s/%s.3dppack\"\n", DIR_OUTPUT, name )
    
    #land_model_list
		file_model_list.printf( "\t%s/%s.imd", DIRSTR_RES, name )
    
    #land_depend_list
		file_depend_list.printf( "\t%s/%s.3dppack", DIRSTR_OUTPUT, name )
    
    #land_make_depend
    file_make_depend.printf( "#%s\n", name )
    
    file_make_depend.printf(
      "%s/%s.3dppack: %s/%s.nsbmd %s/%s.bin %s/%s.3dmd\n",
      DIRSTR_OUTPUT, name,
      DIRSTR_TEMP, name,
      DIRSTR_RES, name,
      DIRSTR_RES, name )
    
    file_make_depend.printf( "\t@echo create 3dppack %s\n", name )
    
    file_make_depend.printf(
      "\t@%s %s/%s.nsbmd %s/%s.bin %s/%s.3dmd %s/%s.3dppack WB\n\n",
      STR_BINLINKER,
      DIRSTR_TEMP, name,
      DIRSTR_RES, name,
      DIRSTR_RES, name,
      DIRSTR_OUTPUT, name )
    
    file_make_depend.printf( "%s/%s.nsbmd: %s/%s.imd\n",
      DIRSTR_TEMP, name,
      DIRSTR_RES, name )
    
    file_make_depend.printf( "\t@echo create_nsbmd %s\n", name )
    
    file_make_depend.printf( "\t@%s %s/%s.imd -o %s/%s.nsbmd\n\n",
      STR_G3DCVTR, DIRSTR_RES, name, DIRSTR_TEMP, name )
    
    check = DIR_RES + "/" + name + ".imd"
    if( FileTest.exist?(check) != true )
      printf( "%s.imd���_�~�[�t�@�C�����琶�����܂�\n", name )
      file_copy( DIR_RES+"/"+FNAME_DMYFILE_GRID_IMD, check )
    end
   
    check = DIR_RES + "/" + name + ".bin"
    if( FileTest.exist?(check) != true )
      printf( "%s.bin���_�~�[�t�@�C�����琶�����܂�\n", name )
      file_copy( DIR_RES+"/"+FNAME_DMYFILE_GRID_BIN, check )
    end
    
    check = DIR_RES + "/" + name + ".3dmd"
    if( FileTest.exist?(check) != true )
      printf( "%s.3dmd���_�~�[�t�@�C�����琶�����܂�\n", name )
      file_copy( DIR_RES+"/"+FNAME_DMYFILE_GRID_3DMD, check )
    end
	end
	file_csv.close
end

file_make_depend.close
file_depend_list.close
file_model_list.close
file_output_list.close
