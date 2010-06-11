#=================================================
#���̌����}�b�v�̃A�g���r���[�g�l�G���[�����o���܂�
#
#���̌����}�b�v�̃x�[�X���G�N�X�g���K�w�̍��������ƃA�g���r���[�g���r��
#��肪�N�肦������𖞂������O���b�h����W���o�͂ɏo�͂��܂�
#
#����0:�G���[�`�F�b�N���x��(level2�ȏ�`�F�b�N�����Alevel3�ȏ�̓`�F�b�N�K�{!)
#
# �`�F�b�N���x����
#�@level0 0<��������<32.0 || (����0&& n_attr!= ex_attr)��􂢏o��
#�@level1 0<��������<32.0 ��􂢏o��
#  level2 16.0f < ��������< 32.0f �ŐN����􂢏o��
#�@level3 0<��������<16.0 ��􂢏o��(�K�{!)
# �`�F�b�N���x����
#=================================================

$error_count = 0
$error_map_count = 0

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
    @height_tbl = Array.new
    @WaterTbl = water_tbl
    @length = @header[0]*@header[1]
    #�ǂݍ��݃��[�v
    loop_num = @header[0] * @header[1]
    for i in  (0 ... loop_num) do
      @dats << MAPEDITOR_BINFILE.new( file )
    end
  end

  #�A�g���r���[�g�z��擾
  def MakeAttrTbl
    @dats.each{ |grid|
      @attr_tbl << grid.GetAttr
    }
  end

  #�����z��擾
  def MakeHeightTbl
    idx = 0
    @dats.each{ |grid|
      y = grid.GetHeight( idx%32, idx/32)
      @height_tbl << y 
      idx += 1
    }
  end
  
  def f32_to_fx32 f
    if f > 0 then
      return f.prec_f*4096.0+0.5
    else
      return f.prec_f*4096.0-0.5
    end
  end

  #�@0<��������<32.0 || (����0&& n_attr!= ex_attr)��􂢏o��
  def CheckAttrError_Level0 h_diff, n_attr, ex_attr
    if h_diff == 0 then
      if n_attr != ex_attr then
        return true
      end
    elsif h_diff < 32.0 then
      return true
    end
    return false
  end
  
  #�@0<��������<32.1 ��􂢏o��
  def CheckAttrError_Level2 h_diff, n_attr, ex_attr
    if h_diff == 0 then
      return false
    end
    if h_diff < 32.0 then
      return true
    end
    return false
  end
  
  
  # 16.0f < ��������< 32.0f �ŐN����􂢏o��
  def CheckAttrError_Leve2 h_diff, n_attr, ex_attr
    if h_diff == 0 then
      return false
    end
    if h_diff <= 16.0 || h_diff >= 32.0 then
      return false
    end
   
    hitch = 0
    hitch += (n_attr&0x00010000!=0) ? 1:0
    hitch += (ex_attr&0x00010000!=0) ? 1:0
    if hitch == 2 then
      return false
    end

    return true 
  end
  
  
  #�@0<��������<16.0 ��􂢏o��
  def CheckAttrError_Level3 h_diff, n_attr, ex_attr
    if h_diff == 0 then
      return false
    end
    if h_diff < 16.0 then
      return true
    end
    return false
  end
 

  def CheckHeightAttr height,attr,level
    for i in 0...@length do
      h_diff = (@height_tbl[i]-height[i]).abs

      case level
      when 0
        ret = CheckAttrError_Level0(h_diff,attr[i],@attr_tbl[i])
      when 1
        ret = CheckAttrError_Level1(h_diff,attr[i],@attr_tbl[i])
      when 2
        ret = CheckAttrError_Level2(h_diff,attr[i],@attr_tbl[i])
      when 3
        ret = CheckAttrError_Level3(h_diff,attr[i],@attr_tbl[i])
      else
        ret = CheckAttrError_Level0(h_diff,attr[i],@attr_tbl[i])
      end

      if ret then
        n_hitch = (attr[i]&0x00010000!=0) ? '�~':'��'
        ex_hitch = (@attr_tbl[i]&0x00010000!=0) ? '�~':'��'
        printf(" pos( %d, %d ) h_diff = %f n_attr=%08x %s, ex_attr=%08x %s, n_h=%f ex_h=%f\n",
          i%32,i/32,
          h_diff,attr[i],n_hitch,@attr_tbl[i],ex_hitch,height[i], @height_tbl[i])
        $error_count+=1
      end
    end
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
  
  def GetHeightTbl
    return @height_tbl
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

    @vecN1 = file.read(6).unpack("sss");

    @vecN2 = file.read(6).unpack("sss");

    @vecN1_D = file.read(4).unpack("i")[0];
    @vecN2_D = file.read(4).unpack("i")[0];

    @attr_trtype = file.read(4).unpack("I")[0];
    @attr = @attr_trtype & 0x7fffffff
    @tryangleType = (@attr_trtype >> 31) & 0x00000001
  end

  def GetAttr
    #puts "attr #{@attr}"
    return @attr
  end

  def FxMul v1,v2
    val = (v1*v2)>>12
    return val
  end

  def FxDiv v1,v2
    val = ((v1<<12)/v2)
    return val
  end

  def f32_to_fx32 f
    if f > 0 then
      return f.prec_f*4096.0+0.5
    else
      return f.prec_f*4096.0-0.5
    end
  end

  def fx32_to_f32 fx
    return fx.prec_f/4096.0
  end

=begin
  def GetHeight x,z
    printf("x=%d, z=%d, ",x,z)
    x = (((x-16)*16)+8)<<12
    z = (((z-16)*16)+8)<<12
    ax = FxMul(@vecN1[0],x)
    cz = FxMul(@vecN1[2],z)
    by = -ax+cz+@vecN1_D
    y = FxDiv(by,@vecN1[1])
    printf("xd=%08x, zd=%08x, yd=%08x\n",x,z,y)
    return y 
  end
=end
  def GetHeight x,z
#    printf("x=%d, z=%d, \n",x,z)
    
    fx = (((x-16)*16)+8).prec_f
    fz = ((256-(z*16))-8).prec_f
    vx = fx32_to_f32(@vecN1[0].to_i) 
    vy = fx32_to_f32(@vecN1[1].to_i) 
    vz = fx32_to_f32(@vecN1[2].to_i)
    vd = fx32_to_f32(@vecN1_D.to_i)
#    printf(" hx=%08x, hy=%08x, hz=%08x hd=%08x\n",@vecN1[0],@vecN1[1],@vecN1[2], @vecN1_D)
#    printf(" vx=%f,vy=%f, vz=%f, vd=%f\n",vx,vy,vz,vd)

    ax = fx*vx
    cz = fz*vz
    by = -ax-cz-vd
    y = by/vy
#    printf(" xf=%f, zf=%f, yd=%08x\n",fx,fz,f32_to_fx32(y))
    return y
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

#�f�B���N�g����bin�t�@�C�����X�g�A�b�v
#bin_ary = Dir.glob("../land_res/*.bin")
#bin_ary = Dir.glob("../land_res/m_dun0201_02_01_ex.bin")
bin_ary = Dir.glob("../land_res/*_ex.bin")
bin_ary.each{ | i |
  old_error = $error_count
  
  printf("\n%s check\n",i)
  file = File.open( i, "rb" );
  bin_loader_ex = MAPEDITOR_BINFILE_LOADER.new( file, nil )
  bin_loader_ex.MakeAttrTbl
  bin_loader_ex.MakeHeightTbl
  file.close()

  i.sub!(/_ex\.bin/,".bin") 
  printf("%s check\n",i)
  file = File.open( i, "rb" );
  bin_loader = MAPEDITOR_BINFILE_LOADER.new( file, nil )
  bin_loader.MakeAttrTbl
  bin_loader.MakeHeightTbl
  file.close()
  
  bin_loader_ex.CheckHeightAttr( bin_loader.GetHeightTbl(), bin_loader.GetAttrTbl(), ARGV[0].to_i )

  if old_error < $error_count then
    $error_map_count += 1
  end
  p "clear"
}

printf("\n////////////////////////////////////\nTotal error grid = %d map = %d\n",$error_count,$error_map_count)
