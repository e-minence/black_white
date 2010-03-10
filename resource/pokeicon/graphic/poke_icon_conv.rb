#! ruby -Ks

def get_palette( read_file )
  #�L�����t�@�C������p���b�g�������o��
  fp_r = open( read_file, "rb" )

  #�w�b�_��ǂݔ�΂�
  data = fp_r.read( 16 )
  head = data.unpack( "a4" )

  if head[ 0 ] != "NCCG"
    print "NCG�t�@�C���ł͂���܂���\n"
    exit( 1 )
  end

  #�w�b�_��ǂݔ�΂�
  data = fp_r.read( 8 )
  head, size = data.unpack( "a4l" )

  if head != "CHAR"
    print "CHAR:NCG�t�@�C���ł͂���܂���\n"
    exit( 1 )
  end
    
  data = fp_r.read( size - 8 )

  #�w�b�_��ǂݔ�΂�
  data = fp_r.read( 16 )
  head, size, width, height = data.unpack( "a4l3" )

  if head != "ATTR"
    print "ATTR:NCG�t�@�C���ł͂���܂���\n"
    exit( 1 )
  end

  size -= 16
    
  pltt = 0
  size.times {|j|
    data = fp_r.read( 1 )
    pal = data.unpack( "C" )
    if pal[ 0 ] != 0
      pltt = pal[ 0 ]
      break
    end
  }
  pltt
end

	if ARGV.size < 1
		print "error: ruby poke_icon_conv.rb file_list\n"
		print "file_list:�ǂݍ��ރt�@�C�����X�g\n"
		exit( 1 )
	end

  ARGV_READ_FILE = 0

  file_list = []
  cnt = 0
  open( ARGV[ ARGV_READ_FILE ] ) {|fp_r|
    while str = fp_r.gets
      str = str.tr( "\"\r\n", "" )
      file_list[ cnt ] = str
      cnt += 1
    end
  }

  fp_pal = open( "pokeicon.dat", "w" )
  fp_pal.print "//============================================================================================\n"
  fp_pal.print "/**\n"
  fp_pal.print "* Pokemon Icon Palette Attribute\n"
  fp_pal.print "*	2005/12/27 by nakahiro\n"
  fp_pal.print "*	�V���`�o�[�W���� by soga\n"
  fp_pal.print "* �V���`�ŃI�X���X���������ɑΉ����邽�߁A����4bit�����A���4bit�����ɂ��Ă܂�\n"
  fp_pal.print "*/\n"
  fp_pal.print "//============================================================================================\n"
  fp_pal.print "const u8 IconPalAtr[] = {\n"

  dir = ""

  file_list.size.times {|i|
    if i == 0
      dir = file_list[ i ]
    end
    next if i == 0
    file_name = file_list[ i ].sub( ".NCGR", ".ncg" )

    #�I�X�t�@�C���R���o�[�g
    cmd = "./nce_lnk poke_icon_128k.nce " + file_name
    system( cmd )
    p cmd

    cmd = "g2dcvtr " + file_name.sub( ".ncg", ".nce" ) + " -br"
    system( cmd )
    p cmd

    osu_pltt = get_palette( file_name )

    #���X�t�@�C�������݂���Ȃ�R���o�[�g
    file_name = file_list[ i ].sub( "_m.NCGR", "_f.ncg" )
    if File.exist?( file_name ) == true 
      cmd = "./nce_lnk poke_icon_128k.nce " + file_name
      system( cmd )
      p cmd

      cmd = "g2dcvtr " + file_name.sub( ".ncg", ".nce" ) + " -br"
      system( cmd )
      p cmd

      mesu_pltt = get_palette( file_name )
    else
      fp_w = open( file_name.sub( ".ncg", ".NCGR" ), "w" )
      fp_w.close
      mesu_pltt = 0
    end
    fp_pal.printf("\t0x%d%d,\t\t// %d : %s\n", mesu_pltt, osu_pltt, i - 1, file_list[ i ].sub( ".NCGR", "" ) )
  }
  
  fp_pal.print "};\n"
  fp_pal.close
