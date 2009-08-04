# �f�[�^�E�C���f�b�N�X
INDEX_ZONE = 0
INDEX_POS_X = 1
INDEX_POS_Y = 2
INDEX_POS_Z = 3
INDEX_VOLUME_1 = 4
INDEX_VOLUME_2 = 5
INDEX_VOLUME_3 = 6
INDEX_VOLUME_4 = 7
INDEX_RANGE_X_1 = 8
INDEX_RANGE_X_2 = 9
INDEX_RANGE_X_3 = 10
INDEX_RANGE_X_4 = 11
INDEX_RANGE_Y_1 = 12
INDEX_RANGE_Y_2 = 13
INDEX_RANGE_Y_3 = 14
INDEX_RANGE_Y_4 = 15
INDEX_RANGE_Z_1 = 16
INDEX_RANGE_Z_2 = 17
INDEX_RANGE_Z_3 = 18
INDEX_RANGE_Z_4 = 19

# �e�[�u��1�v�f���̃f�[�^�T�C�Y
ONE_TABLE_DATA_SIZE = 4

# 1���j�b�g���̃f�[�^�T�C�Y
ONE_UNIT_DATA_SIZE = 24


#---------------------------------------------------------------------------------- 
# �]�[��������, �]�[��ID���擾����
def GetZoneID( name )

	zone_id = 0;

	# ��������萔�����쐬
	def_name = "ZONE_ID_" + name

	# �t�@�C�����J��
	file_def = File.open( "../../prog/arc/fieldmap/zone_id.h" )

	# 1�s�����Ă���
	file_def.each do |line|

		# �����萔�������������ꍇ
		if( line.index( def_name ) != nil ) then

			# ()���̕�����𐔒l�ɕϊ�����
			i0 = line.index( "(" ) + 1
			i1 = line.index( ")" ) - 1
			str_num = line[ i0..i1 ]
			zone_id = str_num.to_i 
			break
			
		end
		
	end
	
	# �t�@�C�������
	file_def.close

	return zone_id
end

#----------------------------------------------------------------------------------
# �]�[��ID�̍ő�l���擾����
def GetMaxZoneID()

	max_id = 0;

	# �t�@�C�����J��
	file_def = File.open( "../../prog/arc/fieldmap/zone_id.h" )

	# 1�s�����Ă���
	file_def.each do |line|

		# �����萔�������������ꍇ
		if( line.index( "ZONE_ID_MAX" ) != nil ) then

			# ()���̕�����𐔒l�ɕϊ�����
			i0 = line.index( "(" ) + 1
			i1 = line.index( ")" ) - 1
			str_num = line[ i0..i1 ]
			max_id = str_num.to_i 
			break
			
		end
		
	end
	
	# �t�@�C�������
	file_def.close

	return max_id
end

#----------------------------------------------------------------------------------
# �f�[�^�E�e�[�u�����o�͂���
def WriteDataTable( src_filename, dst_filename, open_mode )

	# �t�@�C�����J��
	file  = File.open( src_filename )
	wfile = File.open( dst_filename, open_mode )

	# �f�[�^��(�e�[�u���T�C�Y)���J�E���g
	data_num = 0;
	file.each do |line|
		line = line.gsub( /\s/, "" )
		if( line != "" ) then
			data_num += 1;
		end
	end
	data_num -= 1;	# 1�s�ڂ͖�������̂�-1
	array = [ data_num ]
	wfile.write( array.pack( "C" ) )


	# �f�[�^�e�[�u���쐬
	line_index = 0
	data_index = 0			# �o�͂����f�[�^�̐�
	file.pos   = 0
	file.each do |line|

		# ��f�[�^��������, �I��
		empty_test = line.gsub( /\s/, "" )
		if( empty_test == "" ) then
			break;
		end

		# 1�s�ڂ͖�������
		if( line_index != 0 ) then

			# �^�u�ŋ�؂�
			line_split = line.split( /\t/ ) 

			# �]�[��ID���擾
			zone_id = GetZoneID( line_split[ INDEX_ZONE ] )

			# �f�[�^�ւ̃I�t�Z�b�g���v�Z
			offset  = 1 + ONE_TABLE_DATA_SIZE * data_num   # �I�t�Z�b�g = �f�[�^�擪�܂ł̃I�t�Z�b�g + 
			offset += ONE_UNIT_DATA_SIZE * data_index	   #              �f�[�^�擪����̃I�t�Z�b�g

			# �o�͂����f�[�^�̐����J�E���g
			data_index += 1 

			# �e�[�u���E�f�[�^���o��
			array = [ zone_id, offset ]
			wfile.write( array.pack( "SS" ) )

		end

		line_index += 1

	end
	
	# �t�@�C�������
	file.close
	wfile.close
end

#----------------------------------------------------------------------------------

src_filename = "city.txt"
dst_filename = "city.bin"


# �w�b�_(�f�[�^�E�e�[�u��)���쐬
WriteDataTable( src_filename, dst_filename, "wb" )

# �t�@�C�����J��
file  = File.open( src_filename )
wfile = File.open( dst_filename, "ab" );	# �ǋL����

# �s�C���f�b�N�X
line_index = 0;

# ���ׂĂ̍s�ɂ��ď�������
file.each do |line|

	# ��f�[�^��������, �I��
	empty_test = line.gsub( /\s/, "" )
	if( empty_test == "" ) then
		break;
	end

	# 1�s�ڂ͖�������
	if( line_index != 0 ) then

		# �^�u�ŋ�؂�
		line_split = line.split( /\t/ )
		
		# ��؂����e�f�[�^�𐔒l�z��ɕϊ�
		array      = Array.new 
		data_index = 0
		line_split.each do |s|

			# �X�w��̓]�[��ID�ɕϊ�����
			if( data_index == INDEX_ZONE ) then
				array << GetZoneID( s )

			# ����ȊO�� ���̂܂ܐ��l�ɕϊ�
			else 
				array << s.to_i
			end

			# �f�[�^�ԍ����J�E���g
			data_index += 1

		end

		# �t�@�C���ɏo��
		line_pack  = array.pack( "SSSSCCCCCCCCCCCCCCCC" );
		wfile.write( line_pack )

	end

	# �s�����J�E���g
	line_index += 1
end

# �t�@�C�������
wfile.close
file.close
