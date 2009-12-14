#!ruby -Ks
#!/usr/bin/ruby
#
# ���M�����[�V�������o�C�i���ɂ���v���O����
# 2009.06.04 k.ohno
#
#

require "jcode"
require 'nkf'

COL_BASE_EVENTID = 0  #�C�x���g�h�c(�ő�2048���܂�)
COL_BASE_ROMVERSION = 1 #	�Ώۃo�[�W����
COL_BASE_MESSAGE = 2 #	������
COL_BASE_EVENTTITLE = 3 #�C�x���g�^�C�g��
COL_BASE_TIME = 4	#��M��������
COL_BASE_TYPE = 5   #	�z�M�f�[�^���
COL_BASE_NO = 6	#�|�P�������A�C�e���ԍ�	
COL_BASE_FORM = 7 #�t�H����
COL_BASE_ONCE = 8  #	��x�����̔z�M
COL_BASE_HAVE = 9  #	�󂯎�����t���O
COL_BASE_RARE = 10 #	���A

COL_POKE_EVENTID = 0 #�C�x���g�h�c(�ő�2048���܂�)
COL_POKE_PLAYERID  = 1 #	�e(�v���[���[)��ID
COL_POKE_ROMVERSION  = 2 #	���ꂽROM�o�[�W����
COL_POKE_RIBBON  = 3 #	���{���r�b�g 16�{
COL_POKE_GETBALL  = 4 #	�߂܂����{�[��
COL_POKE_HAVEITEM  = 5 #	��������
COL_POKE_TECHNIQUE1  = 6 #	�Z1
COL_POKE_TECHNIQUE2  = 7 #	�Z2
COL_POKE_TECHNIQUE3  = 8 #	�Z3
COL_POKE_TECHNIQUE4  = 9 #	�Z4
COL_POKE_MONSTER  = 10 #	�����X�^�[NO
COL_POKE_FORM  = 11 #	�t�H����NO
COL_POKE_LANGCODE  = 12 #	���R�[�h
COL_POKE_NICKNAME  = 13 #	�j�b�N�l�[��
COL_POKE_PERSONALITY = 14 #	���i
COL_POKE_SEX  = 15 #	����
COL_POKE_ATTRIBUTE  = 16 #	�Ƃ�����
COL_POKE_RARE  = 17 #	���A�ɂ��邩�ǂ��� 0=�����_�����A�Ȃ�  1=�����_�� 2=���A
COL_POKE_GETPLACE  = 18 #	�߂܂����ꏊ
COL_POKE_BARTHPLACE  = 19 #	���܂ꂽ�ꏊ
COL_POKE_GETLEVEL  = 20 #	�߂܂���LEVEL
COL_POKE_COOL  = 21 #	�������悳
COL_POKE_BEAUTIFUL  = 22 #	��������
COL_POKE_PRETTY  = 23 #	���킢��
COL_POKE_SAGE  = 24 #	��������
COL_POKE_STRONG  = 25 #	�����܂���
COL_POKE_GLOSS  = 26 #	�щ�
COL_POKE_RANDHP  = 27 #	HP����
COL_POKE_RANDSTR  = 28 #	�U���͗���
COL_POKE_RANDDEF  = 29 #	�h��͗���
COL_POKE_RANDSPEED  = 30 #	�f��������
COL_POKE_RANDMAGIC  = 31 #	���U����
COL_POKE_RANDSPIRIT  = 32 #	���h����
COL_POKE_PLAYERNAMESTR  = 33 #	�e�̖��O
COL_POKE_PLAYERSEX  = 34 #	�e�̐��� 0 1 2
COL_POKE_LEVEL  =  35 #	�|�P�������x��
COL_POKE_EGG  =  36 #	�^�}�S���ǂ���

COL_ITEM_EVENTID = 0    #�C�x���g�h�c(�ő�2048���܂�)
COL_ITEM_NO = 1    #	����ԍ�
COL_ITEM_MOVIE = 2    #	�f��z�M���ǂ���






COL_NO = 0    #No
COL_CUPNAME = 1    #�J�b�v��
COL_RULENAME = 2    #�ΐ탋�[��
COL_PROGRAM_LAVEL = 3    #�v���O�����Ŏg�����x��
COL_NUM_LO = 4    #�Q��������
COL_NUM_HI = 5    #�Q�������
COL_LEVEL = 6    #�Q�����x��
COL_LEVEL_RANGE = 7    #���x���͈�
COL_LEVEL_TOTAL = 8    #���x�����v
COL_BOTH_POKE = 9    #�����|�P����
COL_BOTH_ITEM = 10    #�����ǂ���
COL_VETO_POKE = 11    #�Q���֎~�|�P����
COL_VETO_ITEM = 12    #�������݋֎~����
COL_MUST_POKE = 13    #�K�{�|�P����
COL_MUST_POKE_FORM = 14    #�K�{�|�P�����t�H����
COL_SHOOTER = 15    #�V���[�^�[
COL_TIME_VS = 16    #�ΐ펞��
COL_TIME_COMMAND = 17    #���͎���
COL_NICKNAME = 18    #�j�b�N�l�[���\��
COL_AGE_LO = 19    #�N����ȏ�
COL_AGE_HI = 20    #�N����ȉ�
COL_SHOW_POKE = 21    #�|�P������������
COL_TIME_SHOW_POKE = 22 #�|�P����������������
COL_BTL_TYPE = 23 #�o�g���^�C�v

POKENUM_MAX_BYTE = (656/8)  ##���̂��炢�ɑ����邩�� �W�Q�o�C�g
ITEMNUM_MAX_BYTE = (608/8)  ##���̂��炢�ɂӂ��邩��




class RegulationBin
  
  def initialize
    @HashLevelRange = {"�Ȃ�"=>0 ,"�ȏ�"=>1 ,"�ȉ�"=>2, "�ȏ�␳"=>3, "�S�␳"=>4, "�ȉ��␳"=>5}
    @HashOKNG = {"NG"=>0,"OK"=>1, "MANUAL"=>2}
    @HashONOFF = {"OFF"=>0,"ON"=>1, "MANUAL"=>2}
    @HashBATTLE = {"SINGLE"=>0,"DOUBLE"=>1, "TRIPLE"=>2, "ROTATION"=>3, "MULTI"=>4}
    @HashROM = {"WHITE"=>0,"BLACK"=>1, "GOLD"=>2, "SILVER"=>3, "PLATINUM"=>4, "DIAMOND"=>5, "PEARL"=>6, "EMERALD"=>7 , "RED"=>8 , "GREEN"=>9 , "RUBY"=>10 , "SAPPHIRE"=>11 }
    @TempNum = "0"
    @HashPokeGmm = Hash.new
    @HashItemGmm = Hash.new
    @HashWazaGmm = Hash.new
  end
  
  
  def bitingmm(names, outFH , max, gmmhash)
    bitarray = Array.new
    for i in 0..max
      bitarray.push(0)
    end
    
    byteorder = Array.new([1,2,4,8,16,32,64,128])
    
    
    arrayname = names.split(',')
    
    arrayname.each{|monsname|
      
      keyno = gmmhash.index(monsname).to_i
      if keyno == nil
        p "�s���ȃ|�P�������A�C�e���������̂Œ�~���܂� " + monsname
        exit(-1)
      end
      bitarray[keyno / 8]  = bitarray[keyno / 8] + byteorder[keyno % 8]
    }
    for i in 0..max
      num = bitarray[i]
      outFH.write([num].pack("c"))
    end
    
    
    
  end
  
  def mojiout(value ,outFH , max)
      msg = NKF.nkf("-m0 -S --utf16",value)
      i = 0
      mojibyte1 = 0
      msg.each_byte{ | mojibyte |
        if i % 2 == 0
          mojibyte1 = mojibyte
        else
          outFH.putc( mojibyte )
          outFH.putc( mojibyte1 )
        end
        i=i+1
      }
      outFH.putc(0xff)  ##�I�[
      outFH.putc(0xff)
      num =max - msg.length
      k=0
      while k < num
        outFH.putc(0)
        k=k+1
      end
   end



  
  def valuefunc(index , value, outFH)
    case index
    when COL_NO    #No
    when COL_CUPNAME    #�J�b�v��
      msg = NKF.nkf('-m0 -S --utf16',value)
      i = 0
      mojibyte1 = 0
      msg.each_byte{ | mojibyte |
        if i % 2 == 0
          mojibyte1 = mojibyte
        else
          outFH.putc( mojibyte )
          outFH.putc( mojibyte1 )
        end
        i=i+1
      }
      outFH.putc(0xff)  ##�I�[
      outFH.putc(0xff)
      num = 24 - msg.length
      k=0
      while k < num
        outFH.putc(0)
        k=k+1
      end
    when COL_RULENAME    #�ΐ탋�[��
      msg = NKF.nkf("-m0 -S --utf16",value)
      i = 0
      mojibyte1 = 0
      msg.each_byte{ | mojibyte |
        if i % 2 == 0
          mojibyte1 = mojibyte
        else
          outFH.putc( mojibyte )
          outFH.putc( mojibyte1 )
        end
        i=i+1
      }
      outFH.putc(0xff)  ##�I�[
      outFH.putc(0xff)
      num = 24 - msg.length
      k=0
      while k < num
        outFH.putc(0)
        k=k+1
      end
    when COL_NUM_LO    #�Q��������
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_NUM_HI    #�Q�������
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_LEVEL    #�Q�����x��
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_LEVEL_RANGE    #���x���͈�
      num = @HashLevelRange[value]
      outFH.write([num].pack("c"))
    when COL_LEVEL_TOTAL    #���x�����v
      num = value.to_i
      outFH.write([num].pack("s"))
    when COL_BOTH_POKE    #�����|�P����
      num = @HashOKNG[value]
      outFH.write([num].pack("c"))
    when COL_BOTH_ITEM    #�����ǂ���
      num = @HashOKNG[value]
      outFH.write([num].pack("c"))
    when COL_VETO_POKE    #�Q���֎~�|�P����
      if value =~ /�Ȃ�/
        num = 0
        for i in 1..POKENUM_MAX_BYTE
          outFH.write([num].pack("c"))
        end
      else
        bitingmm(value,outFH, POKENUM_MAX_BYTE ,@HashPokeGmm)
      end        
    when COL_VETO_ITEM    #�������݋֎~����
      if value =~ /�Ȃ�/
        num = 0
        for i in 1..ITEMNUM_MAX_BYTE 
          outFH.write([num].pack("c"))
        end
      else
        bitingmm(value,outFH, ITEMNUM_MAX_BYTE ,@HashItemGmm)
      end        
    when COL_MUST_POKE    #�K�{�|�P����
      if value =~ /�Ȃ�/
        num = 0
        outFH.write([num].pack("s"))
      else
        keyno = @HashPokeGmm.index(value).to_i
        p keyno
        outFH.write([keyno].pack("s"))
      end
    when COL_MUST_POKE_FORM    #�K�{�|�P�����t�H����
      num = value.to_i
      outFH.write([num].pack("c")) 
    when COL_SHOOTER    #�V���[�^�[
      num = @HashOKNG[value]
      outFH.write([num].pack("c"))
    when COL_TIME_VS    #�ΐ펞��
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_TIME_COMMAND    #���͎���
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_NICKNAME    #�j�b�N�l�[���\��
      num = @HashONOFF[value]
      outFH.write([num].pack("c"))
    when COL_AGE_LO    #�N����ȏ�
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_AGE_HI    #�N����ȉ�
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_SHOW_POKE    #�|�P������������
      num = @HashOKNG[value]
      outFH.write([num].pack("c"))
    when COL_TIME_SHOW_POKE    #�|�P����������������
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_BTL_TYPE  #�o�g���^�C�v
      p value
      num = @HashBATTLE[value]
      outFH.write([num].pack("c"))
    else
    end
    
    
  end

  def valuefunc_poke(index , value, outFH)
    case index
    when COL_POKE_EVENTID  #�C�x���g�h�c(�ő�2048���܂�)
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_PLAYERID   #	�e(�v���[���[)��ID
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_ROMVERSION   #	���ꂽROM�o�[�W����



    when COL_POKE_RIBBON   #	���{���r�b�g 16�{
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_GETBALL   #	�߂܂����{�[��
      if value =~ /�Ȃ�/
        num = 0
        outFH.write([num].pack("s"))
      else
        keyno = @HashItemGmm.index(value).to_i
        outFH.write([keyno].pack("s"))
      end
    when COL_POKE_HAVEITEM   #	��������
      if value =~ /�Ȃ�/
        num = 0
        outFH.write([num].pack("s"))
      else
        keyno = @HashItemGmm.index(value).to_i
        outFH.write([keyno].pack("s"))
      end
    when COL_POKE_TECHNIQUE1   #	�Z1
      if value =~ /�Ȃ�/
        num = 0
        outFH.write([num].pack("s"))
      else
        keyno = @HashWazaGmm.index(value).to_i
        outFH.write([keyno].pack("s"))
      end
    when COL_POKE_TECHNIQUE2   #	�Z2
      if value =~ /�Ȃ�/
        num = 0
        outFH.write([num].pack("s"))
      else
        keyno = @HashWazaGmm.index(value).to_i
        outFH.write([keyno].pack("s"))
      end
    when COL_POKE_TECHNIQUE3   #	�Z3
      if value =~ /�Ȃ�/
        num = 0
        outFH.write([num].pack("s"))
      else
        keyno = @HashWazaGmm.index(value).to_i
        outFH.write([keyno].pack("s"))
      end
    when COL_POKE_TECHNIQUE4   #	�Z4
      if value =~ /�Ȃ�/
        num = 0
        outFH.write([num].pack("s"))
      else
        keyno = @HashWazaGmm.index(value).to_i
        outFH.write([keyno].pack("s"))
      end
    when COL_POKE_MONSTER   #	�����X�^�[NO
      if value =~ /�Ȃ�/
        num = 0
        outFH.write([num].pack("s"))
      else
        keyno = @HashPokeGmm.index(value).to_i
        outFH.write([keyno].pack("s"))
      end
    when COL_POKE_FORM   #	�t�H����NO
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_LANGCODE   #	���R�[�h
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_NICKNAME   #	�j�b�N�l�[��
	mojiout(value, outFH, )

    when COL_POKE_PERSONALITY  #	���i


    when COL_POKE_SEX   #	����
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_ATTRIBUTE  #	�Ƃ�����


    when COL_POKE_RARE   #	���A�ɂ��邩�ǂ��� 0=�����_�����A�Ȃ�  1=�����_�� 2=���A
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_GETPLACE   #	�߂܂����ꏊ

    when COL_POKE_BARTHPLACE   #	���܂ꂽ�ꏊ

    when COL_POKE_GETLEVEL  #	�߂܂���LEVEL
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_COOL   #	�������悳
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_BEAUTIFUL   #	��������
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_PRETTY   #	���킢��
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_SAGE   #	��������
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_STRONG   #	�����܂���
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_GLOSS   #	�щ�
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_RANDHP   #	HP����
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_RANDSTR   #	�U���͗���
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_RANDDEF   #	�h��͗���
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_RANDSPEED  #	�f��������
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_RANDMAGIC   #	���U����
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_RANDSPIRIT   #	���h����
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_PLAYERNAMESTR   #	�e�̖��O
    when COL_POKE_PLAYERSEX   #	�e�̐��� 0 1 2
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_LEVEL   #	�|�P�������x��
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_POKE_EGG   #	�^�}�S���ǂ���
    else
    end

end













  
  def programfunc(index , value, outFH)
    case index
    when COL_NO               # No
      @TempNum = value
      
    when COL_PROGRAM_LAVEL    # Program
      num = value.to_i
      num = @TempNum.to_i
      outFH.puts( "#define REG_" + value + " " +  num.to_s)
    else
    end
  end

  def headerplus(outHeader)
    outHeader.puts("//���M�����[�V�����̃f�[�^�����o�����x���ł�")
    outHeader.puts("//reguration.rb reguration.xls ����̎��������ł�")
    outHeader.puts("#pragma once")
  end
  
  
  def convline( tabfile, outfile,outheader )
    
    outHeader = File.new(outheader,"w")
    
    headerplus(outHeader)
    
    fpr = File.new(tabfile,"r")
    lineindex=0
    fpr.each{ |line|
      index = 0
      if line =~ /^\d+/
      elsif line =~ /^No/
        next
      else
        break
      end
      outFH = File.new("reguration" + sprintf("%03d",lineindex) + ".bin","wb")
      tabarray = line.split(/\t+/)
      tabarray.each{ |value|
        valuefunc(index, value, outFH)
        programfunc(index, value, outHeader)
        index = index + 1
      }
      outFH.close
      lineindex = lineindex + 1
    }
    outHeader.close
    fpr.close
    
  end
  
  #GMM�t�@�C������K�v�ȕ�����e�[�u�����̂�ǂݍ���
  def gmmread( gmmfile , hashgmm)
    fpr = File.new(gmmfile,"r")
    pokeno = 0
    fpr.each{ |line|
      if line =~ /<row\s+id="MONSNAME_(\d+)">/
        pokeno = $1
      elsif line =~ /<row\s+id="ITEMNAME_(\d+)">/
        pokeno = $1
      elsif line =~ /<row\s+id="WAZANAME_(\d+)">/
        pokeno = $1
      elsif pokeno != 0
        line = NKF.nkf('-m0 -Ws',line)
        if line =~ /<language.name=.JPN.*>(.*)<\/language>/
          monsname = $1
          hashgmm.store(pokeno, monsname)
        end
        pokeno = 0
      end
    }
    fpr.close
  end
  
  
  def allgmmread(pokegmm,itemgmm, wazagmm)
  
    gmmread(pokegmm, @HashPokeGmm)
    gmmread(itemgmm, @HashItemGmm)
    gmmread(wazagmm, @HashWazaGmm)

  end
  
end

begin
  
  
  
  
  dep = RegulationBin.new
  dep.allgmmread(ARGV[3],ARGV[4],ARGV[5])
  dep.convline( ARGV[0],ARGV[1],ARGV[2] )
  
  
  exit(0)
end
