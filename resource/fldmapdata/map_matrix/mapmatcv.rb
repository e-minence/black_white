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
#	0-1 �}�b�v�t���O�@0=�]�[��ID�z�񖳂� 1=�]�[��ID�z��L��
#	2-3 �}�b�v���
#	4-5	H�u���b�N�T�C�Y
#	6-7 V�u���b�N�T�C�Y
#	8- �u���b�N�T�C�Y(HxV)x4byte���̔z�u�z��(4byte�P��
#	8+�z�u�z��- �u���b�N�T�C�Y���̃]�[��ID�z��(4byte�P��
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
RET_ERROR_STR = nil

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
		
		if( line == nil || line.include?(STR_END) )
			return RET_ERROR
		end
		
		y -= 1
	end
	
	line = listfile.gets		#�ړ��Ă̍s

	if( line == nil || line.include?(STR_END) )
		return RET_ERROR
	end
	
	str = split( "\t" )			#�^�u�P��
	x += 1						#Y�ڐ����΂�
	
	if( str[x] == nil || str[x] == STR_END )
		return RET_ERROR
	end
	
	num = str[x].to_i
	return num
end

#--------------------------------------------------------------
#	�\�f�[�^�̎w��ʒu�̕�������擾
#	listfile	���X�g�t�@�C��
#	x	X�ʒu
#	y	Y�ʒu
#--------------------------------------------------------------
def table_posstr_get( listfile, x, y )
	listfile.pos = 0			#�擪��
	
	listfile.gets				#�^�C�g����΂�
	listfile.gets				#X�ڐ����΂�
	
	while y > 0					#Y���킹
		line = listfile.gets
		
		if( line == nil || line.include?(STR_END) )
			return RET_ERROR_STR
		end
		
		y -= 1
	end
	
	line = listfile.gets		#�ړ��Ă̍s

	if( line == nil || line.include?(STR_END) )
		return RET_ERROR_STR
	end
	
	str = split( "\t" )			#�^�u�P��
	x += 1						#Y�ڐ����΂�
	
	if( str[x] == nil || str[x] == STR_END )
		return RET_ERROR_STR
	end
	
	/(\w*)/ =~ str[x]			#�p���������o��
	ret = $1
	
	if( ret == nil || ret == "" )
		return RET_ERROR_STR
	end
	
	return ret
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
		
		if( line == nil || line.include?("}") )
			return RET_ERROR
		end

		if( line.include?("enum") )
			break
		end
	}
	
	num = 0

	loop{
		line = naix_file.gets
		
		if( line == nil || line.include?("}") )
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
		
		if( line == nil || line.include?(STR_END) )
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
#	define�t�@�C��
#==============================================================
#--------------------------------------------------------------
#	define���l�ǂݍ���
#	hfile  define��`�t�@�C��
#	search ����������
#	�߂�l �w�蕶����̐��l RET_ERROR=�q�b�g����
#--------------------------------------------------------------
def hfile_search( hfile, search )
	pos = hfile.pos
	hfile.pos = 0
	num = RET_ERROR
	
	search = search.strip #�擪�����̋󔒁A���s�폜
	
	while line = hfile.gets
		if( line =~ /\A#define/ )
			len = line.length
			str = line.split() #�󔒕����ȊO����
			
			if( str[1] == search ) #�q�b�g
				i = 2
				loop{
					str_num = str[i]
					
					if( str_num == nil )
						break
					end
					
					if( str_num =~ /0x[0-9a-zA-Z.*]/ ) #16�i�\�L
						/([\A0x0-9a-fA-F]+)/ =~ str_num
						str_num = $1
						num = str_num.hex
						hit = 1
						break
					end
				
					if( str_num =~ /[0-9.*]/ ) #10�i�\�L
						/([\A0-9]+)/ =~ str_num
						str_num = $1
						num = str_num.to_i
						hit = 1
						break
					end
					
					i = i + 1
				}
				break
			end
		end
	end
	
	hfile.pos = pos
	return num
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
#	ZONE ID����"ZONE_ID_"�𑫂����������Ԃ�
#--------------------------------------------------------------
def zone_id_str_get( id )
	name = sprintf( "ZONE_ID_%s", id )
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
def mtxline_convert(
	line, cr_dir_path, naix_file, blocknum_file, zone_id_header_file )
	if( line == nil || line.include?(STR_END) )
		return RET_TRUE
	end
	
	landfile = nil
	zonefile = nil
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
	
	if( wfile == nil )
		printf( "mapmatcv ERROR �o�C�i���t�@�C���쐬���s\n" )
		exit 1
	end
	
	filewrite_flag( wfile, 0 )					#�t�@�C���t���O 0
	
	if( landname != "X" && landname != "x" )	#�n�`��ގw��L��
		landname = sprintf( "%s\.txt", landname )
		landfile = File.open( landname )
		if( landfile == nil )
			printf( "%s open error!!\n", landname )
			exit 1
		end
	end
	
	if( zonename != "X" && zonename != "x" )	#�]�[��ID�w��L��
		filewrite_flag( wfile, 1 )				#�t�@�C���t���O 1��
		zonename = sprintf( "%s\.txt", zonename )
		zonefile = File.open( zonename )
		if( zonefile == nil )
			printf( "%s open error!!\n", landname )
			exit 1
		end
	end

	filewrite_kind( wfile, 0 )
	filewrite_xy_size( wfile, x_size, y_size )
	
	wfile.pos = WPOS_TABLE		#�������݈ʒu���e�[�u����
	y = 0
	
	while y < y_size			#���\�[�XID�e�[�u���쐬
		x = 0
		while x < x_size
			idx = 1
			
			if( landfile != nil )	#�e�[�u���w�肠��
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
	
	if( landfile != nil )
		landfile.close
	end

	if( zonefile != nil )	#zone id�e�[�u���w��L��
		y = 0
		while y < y_size			#���\�[�XID�e�[�u���쐬
			x = 0
			while x < x_size
				str_zone = table_posstr_get( zonefile, x, y )
				if( str_zone == RET_ERROR_STR )
					printf( "map_matconv ERROR ZONE_ID %d,%d\n", x, y )
					exit 1
				end
				
				idx = IDX_NON
				
				if( str_zone != "0" )	#ID�w��L��
					str_zone = zone_id_str_get( str_zone )
					idx = hfile_search( zone_id_header_file, str_zone )
					
					if( idx == RET_ERROR )
						printf( "map_matconv ZONE_ID ERROR %s %d,%d\n",
							str_zone, x, y )
						exit 1
					end
				end
				
				ary = Array( idx )
				wfile.write( ary.pack("I*") )
				x += 1
			end
			y += 1
		end
		
		zonefile.close
	end
	
	wfile.close
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
zone_id_header_path = ARGV[4] #�]�[��ID�w�b�_�[�t�@�C���p�X

if( mtx_path == nil ||
	landnaix_path == nil ||
	dirbin_path == nil ||
	zone_id_header_path == nil )
	printf( "mat_matconv ERROR �����ُ�\n" )
	exit 1
end

printf( "mat_matconv %s %s %s\n", mtx_path, landnaix_path, dirbin_path )

mtx_file = File.open( mtx_path, "r" )
if( mtx_file == nil )
	printf( "mat_matconv ERROR %s���J���܂���\n", mtx_path )
	exit 1
end

blocknum_file = File.open( blocknum_path, "r" )
if( blocknum_file == nil )
	printf( "mat_matconv ERROR %s���J���܂���\n", blocknum_path )
	exit 1
end

naix_file = File.open( landnaix_path, "r" )
if( naix_file == nil )
	printf( "mat_matconv ERROR %s���J���܂���\n", landnaix_path )
	exit 1
end

zone_id_header_file = File.open( zone_id_header_path, "r" )
if( zone_id_header_file == nil )
	printf( "mat_matconv ERROR %s���J���܂���\n", zone_id_header_path )
	exit 1
end

mtx_file.gets				#��s��΂�

loop{
	line = mtx_file.gets
	ret = mtxline_convert( line,
		dirbin_path, naix_file, blocknum_file, zone_id_header_file )
	if( ret == RET_TRUE )
		break
	end
}

mtx_file.close
blocknum_file.close
naix_file.close
zone_id_header_file.close
