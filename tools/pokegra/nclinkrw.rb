
#=======================================================================================
#
#	NitroCharacter�t�@�C����LINK�����J�����g�f�B���N�g�ɏ���������
#
#=======================================================================================

	$KCODE = "Shift-JIS"

#�N���X��`

#�T�u���[�`��

#���C�����[�`��
	if ARGV.size < 2
		print "error: ruby nclinkrw.rb read_file output_dir\n"
		print "read_file:�ǂݍ��ރt�@�C��\n"
		print "output_dir:�R���o�[�g�����t�@�C�����i�[����f�B���N�g��\n"
		error_action()
	end

	ARGV_READ_FILE = 0
	ARGV_OUT_DIR = 1

	TYPE_NMC = 0
	TYPE_NCE = 1
	TYPE_NCG = 2

	head = [ "NCMC", "NCOB", "NCOB", "NCCG" ]

	signature = [
		#NMC
		[ "MCEL", "CCTL", "GRP ", "ANIM", "ACTL", "LABL", "CMNT", "CCMT", "ECMT", "FCMT", "CLBL" ],
		#NCE 2D
		[ "CELL", "CNUM", "GRP ", "ANIM", "ACTL", "MODE", "LABL", "CMNT", "CCMT", "ECMT", "FCMT", "CLBL", "EXTR" ],
		#NCE 1D
		[ "CELL", "CNUM", "CHAR", "GRP ", "ANIM", "ACTL", "MODE", "LABL", "CMNT", "CCMT", "ECMT", "FCMT", "CLBL", "EXTR" ],
		#NCG
		[ "CHAR", "ATTR" ]
	]

	read_data = []
	data = []
	sig = []

	read_file = ARGV[ ARGV_READ_FILE ]

	begin
		fp_r = open( read_file )
	rescue
		print "FileOpenError:" + read_file + "\n"
		exit( 1 )
	end

	fp_r.binmode

	w_file = ARGV[ ARGV_OUT_DIR ] +	File::basename( read_file )
	fp_w = open( w_file, "wb" )

	read_data = fp_r.read( 16 )
	data = read_data.unpack("c4")

	for type in TYPE_NMC..TYPE_NCG
		cnt = 0
		ng_flag = 0
		head[ type ].each_byte  { |c|
			if data[ cnt ] != c
				ng_flag = 1
				break
			end
			cnt += 1
		}
		if ng_flag == 0
			break
		end
	end

	if ng_flag == 1
		fp_r.close
		fp_w.close
		com = "cp " + ARGV[ ARGV_READ_FILE ] + " " + ARGV[ ARGV_OUT_DIR ]
		p com
		system( com )
		exit( 0 )
	end

	#
	read_data.size.times{ |c|
		fp_w.printf("%c",read_data[ c ])
	}

	signature[ type ].size.times{ |sig_cnt|
		read_data = fp_r.read( 8 )
		sig, size = read_data.unpack("a4l")

		cnt = 0
		signature[ type ][ sig_cnt ].each_byte  { |c|
			if sig[ cnt ] != c
				print signature[ type ][ sig_cnt ] + "NG!\n"
				exit( 1 )
			end
			cnt += 1
		}

		read_data.size.times{ |c|
			fp_w.printf("%c",read_data[ c ])
		}

		size -= 8

		read_data = fp_r.read( size )

		read_data.size.times{ |c|
			fp_w.printf("%c",read_data[ c ])
		}
	}

	read_data = fp_r.read( 8 )
	data, size = read_data.unpack("a4l")

	cnt = 0
	"LINK".each_byte  { |c|
		if data[ cnt ] != c
			print "LINKNG!\n"
			exit( 1 )
		end
		cnt += 1
	}

	file_name = File::basename( fp_r.read( size ) )

	size = 8 + file_name.size 
	padding = 4 - ( size % 4 )
	size += padding

	current_file = File::basename( read_file )

	#�R�s�[���ă_�~�[�t�@�C��������Ă���ꍇ������̂ŁA�i���o�[�����킹�Ă���
	if( file_name[ 5..7 ] != current_file[ 5..7 ] )
		file_name[ 5..7 ] = current_file[ 5..7 ]
	end

	data = [ "LINK", size, file_name ].pack("a* l a*") 

	padding.times{
		data << 0
	}

	data.size.times{ |c|
		fp_w.printf("%c",data[ c ])
	}

	fp_r.close
	fp_w.close
