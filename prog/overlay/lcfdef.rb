#!/usr/bin/ruby
#
#  make_prog_files���� .o�𔲂��o���ăt�@�C���ɂ��܂�
#
# OBJS_OVERLAY=./obj/ARM9-TS.HYB/Release/net_devwifi.o ./obj/ARM9-TS.HYB/Release/dwc_rap.o 
# #
#



class MPFRead

  
  
  
  def FileRead( mpfname, convFileLine )
    ignoreflg = 0
    
    fpr = File.new(mpfname)
    
    fpr.each{ |line|
      if /^SRCS_OVERLAY.*/ =~ line    ### �I�[�o�[���C��`���o��܂Ŗ���
        ignoreflg=1
      end
      if /^#/ =~ line    ### �R�����g�͖���
      elsif ignoreflg==0
      elsif /(\w+\.c)/ =~ line
        convFileLine.push($1)
      end
    }
    fpr.close
  end
  
  
  def FileWrite( outname, convFileLine, dirname)
    
    fpw = File.new(outname,"w")
    fpw.write("OBJS_OVERLAY=")
    
    convFileLine.each{ |line|
      sourcename = File::basename(line,'.*') 
      fpw.write("./obj/ARM9-TS/Release/" + sourcename + ".o ")
    }
    fpw.puts("")
    fpw.close
  end
  
  
  
  
end







### ���C��
begin
  
  ConvFileLine = Array.new
  
  mpfhandle = MPFRead.new
  mpfhandle.FileRead( ARGV[0], ConvFileLine )  ##�R���o�[�g�Ώۃt�@�C����ǂݍ���
  mpfhandle.FileWrite( ARGV[1], ConvFileLine,"./obj/ARM9-TS/Release/")
  mpfhandle.FileWrite( ARGV[2], ConvFileLine,"./obj/ARM9-TS.HYB/Release/")
  
  
  #  MPFRead.TagConv()   ## �^�O��u��������
#  MPFRead.FileWrite( ARGV[1])  ##�R���o�[�g���ʂ��o�͂���
  
  exit(0)
end
