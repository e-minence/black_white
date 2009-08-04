#=======================================================================
# fmmdl_restbl.rb
# �t�B�[���h���샂�f�� ���\�[�X�e�[�u���쐬
# 
# ���� fmmdl_restbl.rb
# xlstxt_filename restbl_filename res_dir select_res dmyfile
# 
# xlstxt_filename
# ���샂�f�����X�g�\ �e�L�X�g�ϊ��ς݃t�@�C����
# 
# restbl_filename
# ���샂�f�����\�[�X�t�@�C�����L�q��t�@�C����
# 
# res_dir
# ���\�[�X�t�@�C�����u����Ă���f�B���N�g��
# 
# select_res
# xlstxt_filename�Ŏw�肳��Ă��镡���̃��\�[�X�t�@�C���̓��A�ǂ�����g����
# 0=���i���f���f���t�@�C�����g�p 1=�e�X�g�p���g�p
#
# dmyfile�@���C��
# �_�~�[�t�@�C���@dmyfile���w�肳��Ă���
# ���\�[�X�t�@�C���������ꍇ�Admyfile�����\�[�X�t�@�C���ɒu�������܂��B
#=======================================================================
$KCODE = "SJIS"
load "rbdefine"

#=======================================================================
#	�萔
#=======================================================================
STR_ERROR = "ERROR"
STR_END = "#END"

RET_FALSE = (0)
RET_TRUE = (1)

#=======================================================================
#	�ُ�I��
#=======================================================================
def error_end( path_delfile, file0, file1 )
	file0.close
	file1.close
	File.delete( path_delfile )
end

#=======================================================================
#	�t�@�C���R�s�[
#=======================================================================
def file_copy( srcpath, copypath )
	open( srcpath ){ |input|
		open( copypath, "w" ){ |output|
			output.write( input.read )
		}
	}
end

#=======================================================================
#	�w��ԍ��̃��\�[�X�t�@�C�������擾
#	�߂�l nil=����,STR_ERROR=�G���[,STR_END=�I��
#=======================================================================
def xlstxt_get_resfile_name( xlstxt_file, no, sel_res )
	xlstxt_file.pos = 0
  
  xlsline = RBDEF_NUM_RESFILE_NAME_0

  if( sel_res != "0" )
    xlsline = RBDEF_NUM_RESFILE_NAME_1
  end
  
	line = xlstxt_file.gets #��s�ڔ�΂�
	
	if( line == nil )
		return STR_ERROR
	end
	
	while line = xlstxt_file.gets
		str = line.split( "," )
		
		if( str[0] == STR_END )
			return str[0]
		end
		
		if( no <= 0 )	#�w��ʒu
			if( str[xlsline] != "" && str[xlsline] != nil )
				return str[xlsline]
			else
				break
			end
		end
		
		no = no - 1
	end
	
	return nil
end

#=======================================================================
#	�w��ԍ����O���\�[�X�t�@�C�������������d���`�F�b�N
#	�߂�l RET_TRUE=�w��t�@�C���������݂��Ă��� RET_FALSE=����
#	�߂�l nil=����,STR_ERROR=�G���[,STR_END=�I��
#=======================================================================
def xlstxt_check_resfile_name( xlstxt_file, check_no, check_str, sel_res )
	no = 0
	xlstxt_file.pos = 0
	
  xlsline = RBDEF_NUM_RESFILE_NAME_0
  
  if( sel_res != "0" )
    xlsline = RBDEF_NUM_RESFILE_NAME_1
  end
  
	line = xlstxt_file.gets #��s�ڔ�΂�

	if( line == nil )
		return RET_FALSE
	end
	
	while line = xlstxt_file.gets
		str = line.split( "," )
		
		if( str[0] == STR_END )
			break
		end
		
		if( no < check_no )	#�w��ʒu���O
			if( str[xlsline] != "" && str[xlsline] != nil )
				if( str[xlsline] == check_str )
					return RET_TRUE
				end
			end
		end
		
		no = no + 1
	end
	
	return RET_FALSE
end

#=======================================================================
#	���\�[�X�e�[�u���쐬
#=======================================================================
xlstxt_filename = ARGV[0]

if( xlstxt_filename == nil )
	printf( "ERROR!! fmmdl_restbl.rb restbl_filename\n" )
	exit 1
end

restbl_filename = ARGV[1]

if( restbl_filename == nil )
	printf( "ERROR!! fmmdl_restbl.rb restbl_filename\n" )
	exit 1
end

resdir_path = ARGV[2] 

if( resdir_path == nil )
	printf( "ERROR!! fmmdl_restbl.rb resdir\n" )
	exit 1
end

if( FileTest.directory?(resdir_path) != true )
	printf( "ERROR!! fmmdl_restbl.rb resdir\n" )
	exit 1
end

sel_res = ARGV[3]

if( sel_res != "0" && sel_res != "1" )
  printf( "ERROR!! fmmdl_restbl.rb sel_res\n" )
  exit 1
end

dmyfile = nil

if( ARGV[4] != nil )
	dmyfile = sprintf( "%s\/%s", resdir_path, ARGV[4] )
	if( FileTest.exist?(dmyfile) != true )
		printf( "ERROR!! fmmdl_restbl.rb dmyfile\n" )
		exit 1
	end
end

xlstxt_file = File.open( xlstxt_filename, "r" )
restbl_file = File.open( restbl_filename, "w" )

no = 0
flag = 0
restbl_file.printf( "FMMDL_RESLIST =" )

loop{
	str = xlstxt_get_resfile_name( xlstxt_file, no, sel_res )
	
	if( str == STR_ERROR )
		printf( "ERROR!! fmmdl_restbl.rb %s���ُ�ł�\n", xlstxt_filename )
		error_end( restbl_filename, xlstxt_file, restbl_file )
		exit 1
	end
	
	if( str == STR_END )
		break
	end
	
	if( str != nil )
		if( xlstxt_check_resfile_name(xlstxt_file,no,str,sel_res) == RET_FALSE )
			restbl_file.printf( " \\\n" )
			path = sprintf( "%s\/%s", resdir_path, str )
		
			if( FileTest.exist?(path) != true )
				if( dmyfile != nil )
					file_copy( dmyfile, path )
					printf( "%s���_�~�[�t�@�C������쐬���܂���\n", path )
				else
					printf( "ERROR!! %s ������܂���\n", str )
					error_end( restbl_filename, xlstxt_file, restbl_file )
					exit 1
				end
			end
			
			restbl_file.printf( "\t%s", str )
			flag = 1
		end
	end
	
	no = no + 1
}

if( flag == 0 )
	printf( "fmmdl_restbl.rb ���\�[�X�Ώۂ�����܂���\n" )
	error_end( restbl_filename, xlstxt_file, restbl_file )
	exit 1
end

xlstxt_file.close
restbl_file.close
