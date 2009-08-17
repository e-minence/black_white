# �f�[�^�E�C���f�b�N�X
INDEX_ZONE   = 0
INDEX_KEY    = 1
INDEX_TEMPO  = 2
INDEX_REVERB = 3

# �e�[�u��1�v�f���̃f�[�^�T�C�Y
ONE_TABLE_DATA_SIZE = 4

# 1�f�[�^�T�C�Y
ONE_DATA_SIZE = 8


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
			offset += ONE_DATA_SIZE * data_index	   #              �f�[�^�擪����̃I�t�Z�b�g

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

src_filename = "dungeon.txt"
dst_filename = "dungeon.bin"


# �w�b�_(�f�[�^�E�e�[�u��)���쐬
WriteDataTable( src_filename, dst_filename, "wb" ) # �V�K�������݂���

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

    # �L�[�̒l��683�{����
    array[ INDEX_KEY ] = array[ INDEX_KEY ] * 683

    # �e���|�̒l��[-512, 512] �� [0, 512] �ɕϊ�����
    array[ INDEX_TEMPO ] = ( array[ INDEX_TEMPO ] + 512 ) / 2 

		# ���o�[�u�̒l��64�{����
		array[ INDEX_REVERB ] = array[ INDEX_REVERB ] * 0x40

		# �t�@�C���ɏo��
		line_pack  = array.pack( "SsSS" );
		wfile.write( line_pack )

		# DEBUG:
		puts( "[0]:zone_id = " + array[0].to_s );
		puts( "[0]:key     = " + array[1].to_s );
		puts( "[0]:tempo   = " + array[2].to_s );
		puts( "[0]:reverb  = " + array[3].to_s );

	end

	# �s�����J�E���g
	line_index += 1
end

# �t�@�C�������
wfile.close
file.close
