#=======================================================================
# fmmdl_restbl.rb
# �t�B�[���h���샂�f�� ���\�[�X�e�[�u���쐬
# 
# ���� fmmdl_restbl.rb xlstxt_filename restbl_filename res_dir dmyfile
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
# dmyfile�@���C��
# �_�~�[�t�@�C���@dmyfile���w�肳��Ă���
# ���\�[�X�t�@�C���������ꍇ�Admyfile�����\�[�X�t�@�C���ɒu�������܂��B
#=======================================================================
$KCODE = "SJIS"

#=======================================================================
#	�萔
#=======================================================================
RESFILE_NAME_NO = (8) #���X�g�t�@�C���@���\�[�X�t�@�C�����L�q�ӏ�

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

dmyfile = nil

if( ARGV[3] != nil )
	dmyfile = sprintf( "%s\/%s", resdir_path, ARGV[3] )
	if( FileTest.exist?(dmyfile) != true )
		printf( "ERROR!! fmmdl_restbl.rb dmyfile\n" )
		exit 1
	end
end

xlstxt_file = File.open( xlstxt_filename, "r" )
restbl_file = File.open( restbl_filename, "w" )

line = xlstxt_file.gets #��s�ڔ�΂�

if( line == nil )
	printf( "ERROR!! fmmdl_restbl.rb %s���ُ�ł�\n", xlstxt_filename )
	error_end( restbl_filename, xlstxt_file, restbl_file )
	exit 1
end

flag = 0
restbl_file.printf( "FMMDL_RESLIST =" )

loop{
	line = xlstxt_file.gets
	str = line.split( "," )
	
	if( str[0] == "#END" )
		break
	end
	
	restbl_file.printf( " \\\n" )
	
	if( str[RESFILE_NAME_NO] != "" && str[RESFILE_NAME_NO] != nil )
		path = sprintf( "%s\/%s", resdir_path, str[RESFILE_NAME_NO] )
		
		if( FileTest.exist?(path) != true )
			if( dmyfile != nil )
				file_copy( dmyfile, path )
				printf( "%s���_�~�[�t�@�C������쐬���܂���\n", path )
			else
				printf( "ERROR!! %s ������܂���\n", str[RESFILE_NAME_NO] )
				error_end( restbl_filename, xlstxt_file, restbl_file )
				exit 1
			end
		end
		
		restbl_file.printf( "\t%s", str[RESFILE_NAME_NO] )
		flag = 1
	end
}

if( flag == 0 )
	printf( "fmmdl_restbl.rb ���\�[�X�Ώۂ�����܂���\n" )
	error_end( restbl_filename, xlstxt_file, restbl_file )
	exit 1
end

xlstxt_file.close
restbl_file.close
