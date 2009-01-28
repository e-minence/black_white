#=========================================================================================
=begin

	08.11.25 Satoshi Nohara

	16����3���̃V�[�P���X�i���o�[�ϊ��e�[�u���쐬

=end
#=========================================================================================

#-----------------------------------------------------------------------------------------
=begin

	gb_table.dat�ɏ�������

=end
#-----------------------------------------------------------------------------------------
def gb_table_write( file, io )

	io.print "//GB�V�[�P���X�i���o�[�ւ̕ϊ��e�[�u��\n"
	io.print "//�R���o�[�g���ăt�@�C���������Ă��܂�\n"
	io.print "//08.11.25 Satoshi Nohara\n\n"
	io.print "static const u16 gb_table[][2] = {\n"

	while text = file.gets do						#�t�@�C�����P�s���ǂݍ���
		text.chomp!									#���s������ꍇ�̂ݍ��(�j��I)
		ary = text.split(/\s+/)						#�������g���󔒕����ŕ�������

		if( ary[1] == nil )
			next
		end

		#for( i=1; i < 8 ;i++ )
		i = 1

		#print ary[i]
		#io.print ary[i]

		if( ary[2] == nil )
			next
		end

		if( ary[4] =~ /��/ )
			io.print "\t{", ary[2], ","
			#io.print ary[4]
			io.print "\t", ary[3], "},\n"
		end

	end

	io.print "};\n"
	io.print "#define GB_TABLE_MAX\t\t( NELEMS(gb_table) )\n\n"
end


#-----------------------------------------------------------------------------------------
=begin

	���C���̗���

=end
#-----------------------------------------------------------------------------------------
io = open( "gb_table.dat", "w" )					#�o�̓t�@�C��
file = open( ARGV[0] )								#gb_table.txt
gb_table_write( file, io )							#gb_table.dat�ɏ�������
file.close
io.close											#�o�̓t�@�C������


