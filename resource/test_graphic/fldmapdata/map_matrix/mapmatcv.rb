###############################################################
#	mapmatcv.rb
#	�}�b�v�}�g���N�X�R���o�[�g
#
#	�Ăяo�� mapmapcv.rb BlockPath FilePath LandPath BinPath
#	FilePath �}�b�v�}�g���N�X�t�@�C���p�X
#	BlockPath �n�`�u���b�N�ԍ��t�@�C���p�X
#	LandPath �n�`���f���A�[�J�C�u�C���f�b�N�X�t�@�C���p�X
#	BinPath  �o�C�i���t�@�C���쐬��p�X
#
###############################################################
$KCODE = "SJIS"

#==============================================================
#	���}�g���N�X�t�H�[�}�b�g �o�C�g�P��
#	0-1 �}�b�v����
#	2-3 �}�b�v���
#	4-5	H�u���b�N�T�C�Y
#	6-7 V�u���b�N�T�C�Y
#	8-�u���b�N�T�C�Y(HxV)x4byte���̔z�u�z��
#
#	���n�`�f�[�^�e�L�X�g�t�H�[�}�b�g
#	format: tabspace
#	title
#    0 1 2 3	#dial X
#    1 9 0 0
#    2 8 0 2
#    3 1 0 6
#==============================================================

#==============================================================
#	�萔
#==============================================================
#�t�@�C���������݈ʒu
WPOS_FLAG	= (0)
WPOS_KIND	= (2)
WPOS_SIZE	= (4)
WPOS_TABLE	= (8)

#�z��Ȃ�
IDX_NON		= (0xffffffff)

#�֐��G���[
RET_ERROR	= (-1)
RET_TRUE	= (1)
RET_FALSE	= (0)

#�Œ蕶����
STR_NULL	= ""
STR_END		= "#END"

#==============================================================
#	�\�f�[�^
#==============================================================
#--------------------------------------------------------------
#	�\�f�[�^�̎w��ʒu�̐��l���擾
#	listfile	���X�g�t�@�C��
#	x	X�ʒu
#	y	Y�ʒu
#--------------------------------------------------------------
def table_posdata_get( listfile, x, y )
	listfile.pos = 0			#�擪��
	
	listfile.gets				#�^�C�g����΂�
	listfile.gets				#X�ڐ����΂�
	
	while y > 0					#Y���킹
		line = listfile.gets
		
		if( line == @nil || line.include?(STR_END) )
			return RET_ERROR
		end
		
		y -= 1
	end
	
	line = listfile.gets		#�ړ��Ă̍s

	if( line == @nil || line.include?(STR_END) )
		return RET_ERROR
	end
	
	str = split( "\t" )			#�^�u�P��
	x += 1						#Y�ڐ����΂�

	if( str[x] == @nil || str[x] == STR_END )
		return RET_ERROR
	end
	
	num = str[x].to_i
	return num
end

#==============================================================
#	�n�`�u���b�Nnaix
#==============================================================
#--------------------------------------------------------------
#	�n�`�u���b�Nnaix�t�@�C������w��C���f�b�N�X�̐��l���擾
#--------------------------------------------------------------
def landnaix_number_get( naix_file, str )
	naix_file.pos = 0			#�擪��
	
	loop{
		line = naix_file.gets
		
		if( line == @nil || line.include?("}") )
			return RET_ERROR
		end

		if( line.include?("enum") )
			break
		end
	}
	
	num = 0

	loop{
		line = naix_file.gets
		
		if( line == @nil || line.include?("}") )
			return RET_ERROR
		end
		
		if( line.include?(str) )
			return num
		end
		
		num += 1
	}
	
	return RET_ERROR
end

#==============================================================
#	�n�`�u���b�N�ԍ��t�@�C��
#==============================================================
#--------------------------------------------------------------
#	�w��u���b�N�ԍ�����n�`�f�[�^�C���f�b�N�X�ԍ����擾
#--------------------------------------------------------------
def blocknum_landidx_get( idx, blocknum_file, naix_file )
	naix_file.pos = 0			#�擪��
	blocknum_file.pos = 0
	line = blocknum_file.gets	#��s��΂�
	
	loop{
		line = blocknum_file.gets
		
		if( line == @nil || line.include?(STR_END) )
			return RET_ERROR
		end
		
		str = split( "\t" )		#�^�u�P��
		num = str[0].to_i		#�ԍ�
		
		if( idx == num )		#��v
			ret = landnaix_number_get( naix_file, str[1] )
			
			if( ret == RET_ERROR )
				printf( "mapmatcv ERROR " )
				printf( "%s�̃u���b�N��������܂���\n", str[1] )
			end
			
			return ret
		end
	}
end

#==============================================================
#	�}�g���N�X�t�@�C��
#==============================================================
#--------------------------------------------------------------
#	�}�g���N�X�����񂩂�}�g���b�N�XID�擾
#--------------------------------------------------------------
def matstr_matid_get( str )
	return str[0]
end

#--------------------------------------------------------------
#	�}�g���N�X�����񂩂�X�T�C�Y���擾
#--------------------------------------------------------------
def matstr_x_size_get( str )
	num = str[1].to_i
	return num
end

#--------------------------------------------------------------
#	�}�g���N�X�����񂩂�Y�T�C�Y���擾
#--------------------------------------------------------------
def matstr_y_size_get( str )
	num = str[2].to_i
	return num
end

#--------------------------------------------------------------
#	�}�g���N�X�����񂩂�v���t�B�b�N�X���擾
#--------------------------------------------------------------
def matstr_prefix_get( str )
	return str[3]
end

#--------------------------------------------------------------
#	�}�g���N�X�����񂩂�n�`�t�@�C�������擾
#--------------------------------------------------------------
def matstr_landfile_name_get( str )
	return str[4]
end

#--------------------------------------------------------------
#	�}�g���N�X�����񂩂�ZONE_ID�Ǘ��\���擾
#--------------------------------------------------------------
def matstr_zonefile_name_get( str )
	return str[5]
end

#--------------------------------------------------------------
#	�v���t�B�b�N�X����x,y�̒l�𑫂����������Ԃ�
#--------------------------------------------------------------
def prefixname_get( prefix, x, y )
	name = sprintf( "%s%02d_%02d", prefix, x, y )
	return name
end

#--------------------------------------------------------------
#	�}�g���N�XID���o�C�i���t�@�C�����ɕϊ������������Ԃ�
#--------------------------------------------------------------
def	matid_binname_get( id )
	name = sprintf( "%s_mat.bin", id )
	return name
end

#--------------------------------------------------------------
#	���݃t���O��������
#--------------------------------------------------------------
def filewrite_flag( wfile, flag )
	pos = wfile.pos
	wfile.pos = WPOS_FLAG
	ary = Array( flag )
	wfile.write( ary.pack("S*") )
	wfile.pos = pos
end

#--------------------------------------------------------------
#	�}�b�v��ޏ�������
#--------------------------------------------------------------
def filewrite_kind( wfile, kind )
	pos = wfile.pos
	wfile.pos = WPOS_KIND
	ary = Array( kind )
	wfile.write( ary.pack("S*") )
	wfile.pos = pos
end

#--------------------------------------------------------------
#	X,Y��������
#--------------------------------------------------------------
def filewrite_xy_size( wfile, x, y )
	pos = wfile.pos
	wfile.pos = WPOS_SIZE
	ary = Array( x )
	wfile.write( ary.pack("S*") )
	ary = Array( y )
	wfile.write( ary.pack("S*") )
	wfile.pos = pos
end

#--------------------------------------------------------------
#	��s�R���o�[�g
#	return RET_TRUE == �I�[
#--------------------------------------------------------------
def mtxline_convert( line, cr_dir_path, naix_file, blocknum_file )
	if( line == @nil || line.include?(STR_END) )
		return RET_TRUE
	end
	
	landfile = @nil
	str = line.split( "\t" )
	str_id = matstr_matid_get( str )
	x_size = matstr_x_size_get( str )
	y_size = matstr_y_size_get( str )
	prefix = matstr_prefix_get( str )
	landname = matstr_landfile_name_get( str )
	zonename = matstr_zonefile_name_get( str )
	
	printf( "mapmatcv convert %s\n", str_id )
	
	str_wfile = matid_binname_get( str_id )
	str_wfile = sprintf( "%s/%s", cr_dir_path, str_wfile )
	wfile = File.open( str_wfile, "wb" )
	
	if( wfile == @nil )
		printf( "mapmatcv ERROR �o�C�i���t�@�C���쐬���s\n" )
		exit 1
	end
	
	if( landname != "X" )		#�]�[���t�@�C���w�薳��
		landname = sprintf( "%s\.txt", landname )
		landfile = File.open( landname )
	end
	
	filewrite_flag( wfile, 1 )
	filewrite_kind( wfile, 1 )
	filewrite_xy_size( wfile, x_size, y_size )
	
	wfile.pos = WPOS_TABLE		#�������݈ʒu���e�[�u����
	y = 0
	
	while y < y_size
		x = 0
		while x < x_size
			idx = 1
			
			if( landname != "X" )	#�e�[�u���w�肠��
				idx = table_posdata_get( landfile, x, y )
				
				if( idx == RET_ERROR )
					printf( "map_matconv ERROR �n�`�e�[�u���ُ�\n" )
					exit 1
				end
			end
			
			case idx
			when 0					#�w��Ȃ�
				idx = IDX_NON
			when 1					#�}�b�v���ݗL��
				binname = prefixname_get( prefix, x, y )
				idx = landnaix_number_get( naix_file, binname )
				
				if( idx == RET_ERROR )
					printf( "map_matconv ERROR �n�`%s������\n", binname )
					exit 1
				end
			else					#�u���b�N�ԍ��w��
				idx = blocknum_landidx_get( idx, blocknum_file, naix_file )
				
				if( idx == RET_ERROR )
					printf( "map_matconv ERROR �u���b�N�ԍ��ُ� X=%d,Y=%d\n",
						  x, y )
					exit 1
				end
			end

			ary = Array( idx )
			wfile.write( ary.pack("I*") )
			x += 1
		end
		y += 1
	end
	
	wfile.close
	
	if( landfile != @nil )
		landfile.close
	end

	return RET_FALSE
end

#==============================================================
#	�R���o�[�g���C��
#==============================================================
#--------------------------------------------------------------
#	�}�b�v�}�g���N�X�R���o�[�g
#--------------------------------------------------------------
mtx_path = ARGV[0]		#�}�b�v�}�g���N�X�t�@�C���p�X
blocknum_path = ARGV[1]		#�u���b�N�ԍ��t�@�C���p�X
landnaix_path = ARGV[2]		#�n�`���f���A�[�J�C�u�C���f�b�N�X�t�@�C���p�X
dirbin_path = ARGV[3]		#�o�C�i���t�@�C���쐬��p�X

if( mtx_path == @nil || landnaix_path == @nil || dirbin_path == @nil )
	printf( "mat_matconv ERROR �����ُ�\n" )
	exit 1
end

printf( "mat_matconv %s %s %s\n", mtx_path, landnaix_path, dirbin_path )

mtx_file = File.open( mtx_path, "r" )
if( mtx_file == @nil )
	printf( "mat_matconv ERROR %s���J���܂���\n", mtx_path )
	exit 1
end

blocknum_file = File.open( blocknum_path, "r" )
if( blocknum_file == @nil )
	printf( "mat_matconv ERROR %s���J���܂���\n", blocknum_path )
	exit 1
end

naix_file = File.open( landnaix_path, "r" )
if( naix_file == @nil )
	printf( "mat_matconv ERROR %s���J���܂���\n", landnaix_path )
	exit 1
end

mtx_file.gets				#��s��΂�

loop{
	line = mtx_file.gets
	ret = mtxline_convert( line, dirbin_path, naix_file, blocknum_file )
	if( ret == RET_TRUE )
		break
	end
}

mtx_file.close
blocknum_file.close
naix_file.close
