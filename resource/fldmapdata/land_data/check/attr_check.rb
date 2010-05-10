#=================================================
#�O������w�肳�ꂽ�A�g���r���[�g���g�p����Ă��邩�𒲂ׂ܂�
#�w��A�g���r���[�g�́Afbd.txt�ɂP�U�i���Ń��X�g�����ċL�q���Ă�������
#�������ʂ�result.txt�ɏo�͂���܂��B
#
#�i���\�Ȑ��̃A�g���r���[�g��water.txt�ɗ񋓂��Ă��������B
#���͕K���i���s�̃r�b�g�ň͂܂�Ă��܂������̘R����`�F�b�N�ł��܂�
#=================================================

#=================================================
#�}�b�v�G�f�B�^�[����o�͂��ꂽbinary�ǂݍ��݃N���X
#=================================================
class MAPEDITOR_BINFILE_LOADER

  attr:header
  attr:dats
  attr:attr_tbl

  #�ǂݍ���
  def initialize file, water_tbl
    @dats = Array.new
    @header = file.read(4).unpack("SS")
    @attr_tbl = Array.new
    @WaterTbl = water_tbl

    #�ǂݍ��݃��[�v
    loop_num = @header[0] * @header[1]
    for i in  (0 ... loop_num) do
      @dats << MAPEDITOR_BINFILE.new( file )
    end
  end

  def MakeAttrTbl
    @dats.each{ |grid|
      @attr_tbl << grid.GetAttr
    }
  end

  def DumpAttr
    @attr_tbl.each { |i|
      int = i.to_i
      p "%#x" % int
    }
  end

  def GetAttrTbl
    return @attr_tbl
  end

  def Check( result_file, fbd )
    @attr_tbl.each_with_index{ |t, idx|
      low = t & 0xFFFF
      rc = fbd.include?(low)
      if rc == true then
        printf("finded 0x%x\n",low)
        result_file.printf("X=%d Z=%d finded 0x%x\n",idx % @header[0] , idx / @header[0], low)
      end
    }
  end

  def CheckWater( result_file )
    @attr_tbl.each_with_index{ |t, idx|
      #���`�F�b�N
      if IsWater( idx ) == true then
        u = idx - @header[0]
        l = idx - 1
        r = idx + 1
        d = idx + @header[0]

        pass = 0;
      
        if u >= 0 then
          if IsWater( u ) == true || IsHit(u)==1 then
            pass+=1
          end
        else
          pass+=1
        end

        if (l>=0) && ( (l/@header[1]) == (idx/@header[1]) ) then
          if IsWater( l ) == true || IsHit(l)==1 then
            pass+=1
          end
        else
          pass+=1
        end

        if d < @header[0]*@header[1] then
          if IsWater( d ) == true || IsHit(d)==1 then
            pass+=1
          end
        else
          pass+=1
        end

        if (r < @header[0]*@header[1]) && ( (r/@header[1]) == (idx/@header[1]) ) then
          if IsWater( r ) == true || IsHit(r)==1 then
            pass+=1
          end
        else
          pass+=1
        end

        if pass != 4 then
          result_file.printf("X=%d Z=%d WaterError\n",idx % @header[0] , idx / @header[0])
        end
      end
    }
  end

  def IsWater( idx )
    atr = @attr_tbl[idx]
    low = atr & 0xFFFF
    rc = @WaterTbl.include?(low)
    return rc 
  end

  def IsHit( idx )
    atr = @attr_tbl[idx]
    hit = (atr >> 16) & 0x1
    return hit
  end
end


#=================================================
#�}�b�v�G�f�B�^�[����o�͂��ꂽbinary�t�@�C��
#=================================================
class MAPEDITOR_BINFILE

	attr:vecN1       ##�P�ڂ̎O�p�`�̖@���x�N�g��

	attr:vecN2       ##�Q�ڂ̎O�p�`�̖@���x�N�g��

	attr:vecN1_D       ## ax+by+cz+d =0 ��D�l
	attr:vecN2_D

  attr:attr_trtype
	attr:attr          ##�A�g���r���[�g�r�b�g    0-15��value 16-30��flg
	attr:tryangleType  ##�O�p�`�̌`�̃^�C�v  �^ = 0 ��:vecN1,�E:vecN2  �_ = 1 �E:vecN1,��:vecN2

  def initialize file

    @vecN1 = file.read(6).unpack("SSS");

    @vecN2 = file.read(6).unpack("SSS");

    @vecN1_D = file.read(4).unpack("I")[0];
    @vecN2_D = file.read(4).unpack("I")[0];

    @attr_trtype = file.read(4).unpack("I")[0];
    @attr = @attr_trtype & 0x7fffffff
    @tryangleType = (@attr_trtype >> 31) & 0x00000001

  end

  def GetAttr
    #puts "attr #{@attr}"
    return @attr
  end
end

#�g�p�֎~�A�g���r���[�g�ꗗ�쐬
fbd = Array.new
fbd_file = File.open( "fbd.txt", "r" );
while line = fbd_file.gets
	num = line.chomp("\n").chomp("\r")
  fbd << Integer(num)
end

#���A�g���r���[�g�ꗗ�쐬
water = Array.new
water_file = File.open( "water.txt", "r" );
while line = water_file.gets
	num = line.chomp("\n").chomp("\r")
  water << Integer(num)
end

result_file = File.open( ARGV[1], "w" );

#�f�B���N�g����bin�t�@�C�����X�g�A�b�v
bin_ary = Dir.glob("../land_res/*.bin")
bin_ary.each{ | i |
  printf("%s check\n",i)
  result_file.printf( "%s check\n",i )
  file = File.open( i, "rb" );
  bin_loader = MAPEDITOR_BINFILE_LOADER.new( file, water )
  bin_loader.MakeAttrTbl

  if ARGV[0] == 0 then
    bin_loader.Check(result_file, fbd)
  else
    bin_loader.CheckWater(result_file)
  end
  p "clear"
}

#file = File.open( "../land_res/map24_23.bin", "rb" );
#bin_loader = MAPEDITOR_BINFILE_LOADER.new( file )
#bin_loader.MakeAttrTbl

#tbl = bin_loader.GetAttrTbl
#tbl.each{ |t|
#  low = t & 0xFFFF
#  rc = fbd.include?(low)
#  if rc == true then
#    printf("finded %x\n",low)
#    exit
#  end
#}
