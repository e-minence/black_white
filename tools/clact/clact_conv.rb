
#=======================================================================================
#
#	CLACT�Ŏg�p����L�����f�[�^���R���o�[�g
#
#=======================================================================================

	$KCODE = "Shift-JIS"

#�N���X��`

#�T�u���[�`��
# �t�@�C�����̓f�B���N�g�����폜���郁�\�b�h���쐬
def deleteall(delthem)
  if FileTest.directory?(delthem) then  # �f�B���N�g�����ǂ����𔻕�
    Dir.foreach( delthem ) do |file|    # ���g���ꗗ
      next if /^\.+$/ =~ file           # ��ʃf�B���N�g���Ǝ��g��Ώۂ���O��
      deleteall( delthem.sub(/\/+$/,"") + "/" + file )
    end
    Dir.rmdir(delthem) rescue ""        # ���g����ɂȂ����f�B���N�g�����폜
  else
    File.delete(delthem)                # �f�B���N�g���łȂ���΍폜
  end
end

def error_action()
	deleteall("./clact_conv_temp")
	exit( 1 )
end

#���C�����[�`��
	if ARGV.size < 5
		print "error: ruby clact_conv.rb nce_file ncg_file ncl_file g2dcvtr_dir output_dir\n"
		print "nce_file:�ǂݍ���nce�t�@�C��\n"
		print "ncg_file:�ǂݍ���ncg�t�@�C��\n"
		print "ncl_file:�ǂݍ���ncl�t�@�C��\n"
		print "g2dcvtr_dir:g2dcvtr.exe�����݂���f�B���N�g��\n"
		print "output_dir:g2dcvtr.exe�ŃR���o�[�g�����t�@�C�����i�[����f�B���N�g��\n"
		error_action()
	end

	ARGV_NCE_FILE = 0
	ARGV_NCG_FILE = 1
	ARGV_NCL_FILE = 2
	ARGV_G2D_DIR = 3
	ARGV_OUT_DIR = 4

	read_data = []
	data = []
	sig = []

	Dir::mkdir("./clact_conv_temp")

	nce_file = File::basename( ARGV[ ARGV_NCE_FILE ] )
	ncg_file = File::basename( ARGV[ ARGV_NCG_FILE ] )
	ncl_file = File::basename( ARGV[ ARGV_NCL_FILE ] )

#NCE Check
	begin
		fp_r = open( ARGV[ ARGV_NCE_FILE ] )
	rescue
		print "FileOpenError:" + ARGV[ ARGV_NCE_FILE ]
		error_action()
	end

	fp_r.binmode

	w_file = "./clact_conv_temp/" + nce_file

	fp_w = open( w_file, "wb" )

	read_data = fp_r.read( 16 )
	data = read_data.unpack("c4")

	cnt = 0
	"NCOB".each_byte  { |c|
		if data[ cnt ] != c
			print "NCE NG!\n"
			error_action()
		end
		cnt += 1
	}

	read_data.size.times{ |c|
		fp_w.printf("%c",read_data[ c ])
	}

	nce_sig = [ "CELL", "CNUM", "CHAR", "GRP ", "ANIM", "ACTL", "MODE", "LABL",
							"CMNT", "CCMT", "ECMT", "FCMT", "CLBL", "EXTR" ]

	nce_sig.size.times{ |sig_cnt|
		read_data = fp_r.read( 8 )
		sig, size = read_data.unpack("a4l")

		cnt = 0
		nce_sig[ sig_cnt ].each_byte  { |c|
			if sig[ cnt ] != c
				print nce_sig[ sig_cnt ] + "NG!\n"
				error_action()
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

	read_data = fp_r.read( 4 )
	data = read_data.unpack("c4")

	cnt = 0
	"LINK".each_byte  { |c|
		if data[ cnt ] != c
			print "LINKNG!\n"
			error_action()
		end
		cnt += 1
	}

	size = 8 + ncg_file.size 
	padding = 4 - ( size % 4 )
	size += padding
	
	data = ["LINK", size, ncg_file ].pack("a* l a*") 

	padding.times{
		data << 0
	}

	data.size.times{ |c|
		fp_w.printf("%c",data[ c ])
	}

	fp_r.close
	fp_w.close

#NCG Check
	begin
		fp_r = open( ARGV[ ARGV_NCG_FILE ] )
	rescue
		print "FileOpenError:" + ARGV[ ARGV_NCG_FILE ]
		error_action()
	end

	fp_r.binmode

	w_file = "./clact_conv_temp/" + ncg_file

	fp_w = open( w_file, "wb" )

	read_data = fp_r.read( 16 )
	data = read_data.unpack("c4")

	cnt = 0
	"NCCG".each_byte  { |c|
		if data[ cnt ] != c
			print "NCG NG!\n"
			error_action()
		end
		cnt += 1
	}

	read_data.size.times{ |c|
		fp_w.printf("%c",read_data[ c ])
	}

	nce_sig = [ "CHAR", "ATTR" ]

	nce_sig.size.times{ |sig_cnt|
		read_data = fp_r.read( 8 )
		sig, size = read_data.unpack("a4l")

		cnt = 0
		nce_sig[ sig_cnt ].each_byte  { |c|
			if sig[ cnt ] != c
				print nce_sig[ sig_cnt ] + "NG!\n"
				error_action()
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

	read_data = fp_r.read( 4 )
	data = read_data.unpack("c4")

	cnt = 0
	"LINK".each_byte  { |c|
		if data[ cnt ] != c
			print "LINKNG!\n"
			error_action()
		end
		cnt += 1
	}

	size = 8 + ncl_file.size 
	padding = 4 - ( size % 4 )
	size += padding
	
	data = [ "LINK", size, ncl_file ].pack("a* l a*") 

	padding.times{
		data << 0
	}

	data.size.times{ |c|
		fp_w.printf("%c",data[ c ])
	}

	fp_r.close
	fp_w.close

#NCL Check
#NCL�͑f���ɃR�s�[���邾���ł������ǈꉞ
	begin
		fp_r = open( ARGV[ ARGV_NCL_FILE ] )
	rescue
		print "FileOpenError:" + ARGV[ ARGV_NCL_FILE ]
		error_action()
	end

	fp_r.binmode

	w_file = "./clact_conv_temp/" + ncl_file

	fp_w = open( w_file, "wb" )

	fp_w.write( fp_r.read )

	fp_r.close
	fp_w.close

	com = ARGV[ ARGV_G2D_DIR ] + "g2dcvtr ./clact_conv_temp/" + nce_file + " -pcm -o/" + ARGV[ ARGV_OUT_DIR ]

	p com

	system( com )

	deleteall("./clact_conv_temp")
