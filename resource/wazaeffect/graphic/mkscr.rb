
	if ARGV.size < 1
		print "error: ruby mkscr.rb write_file\n";
		print "write_file: �����o���t�@�C����\n";
		exit( 1 )
	end

	ARGV_WRITE_FILE = 0

	fp_w = open( ARGV[ARGV_WRITE_FILE], "w" )

  #BG�t�@�C�����X�g����
  Dir::glob("./*.nsc").each {|f|
    name = File::basename( f, '.*' )
    fp_w.print( "\"" + name + ".NSCR\"\n" )
    fp_w.print( "\"" + name + ".NCGR\"\n" )
    fp_w.print( "\"" + name + ".NCLR\"\n" )
  }

  #OBJ�t�@�C�����X�g����
  Dir::glob("./*.nce").each {|f|
    name = File::basename( f, '.*' )
    fp_w.print( "\"" + name + ".NCGR\"\n" )
    fp_w.print( "\"" + name + ".NCLR\"\n" )
    fp_w.print( "\"" + name + ".NCER\"\n" )
    fp_w.print( "\"" + name + ".NANR\"\n" )
  }

  fp_w.close
