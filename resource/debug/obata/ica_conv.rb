################################################################################# 
#
# @brief .icaファイル コンバータ
# @file  ica_conv.rb
# @autor obata
# @date  2009.08.31
#
# [使用法]
#  ruby ica_conv.rb コンバート対象icaファイル名 出力バイナリファイル名
#
################################################################################# 

#================================================================================
# @brief コンバート後のデータを扱うクラス
#================================================================================
class ConvData
  # コンストラクタ
  def initialize
    @frame_size = 0
    @node_anm
    @scale_x = Array.new
    @scale_y = Array.new
    @scale_z = Array.new
    @rotate_x = Array.new
    @rotate_y = Array.new
    @rotate_z = Array.new
    @translate_x = Array.new
    @translate_y = Array.new
    @translate_z = Array.new
  end

  # セッターメソッド
  def frame_size=(value)
    @frame_size = value
  end
  def node_anm=(value)
    @node_anm = value
  end

  # データ追加メソッド
  def AddScaleData( x, y, z )
    @scale_x << x
    @scale_y << y
    @scale_z << z
  end
  def AddRotateData( x, y, z )
    @rotate_x << x
    @rotate_y << y
    @rotate_z << z
  end
  def AddTranslateData( x, y, z )
    @translate_x << x
    @translate_y << y
    @translate_z << z
  end

  # 文字列変換メソッド
  def to_s
    str = ""
    str += "frame_size = #{@frame_size}\n"
    0.upto( @frame_size - 1 ) do |i|
      str += "scale[#{i}]     = #{@scale_x[i]}, #{@scale_y[i]}, #{@scale_z[i]}\n"
      str += "rotate[#{i}]    = #{@rotate_x[i]}, #{@rotate_y[i]}, #{@rotate_z[i]}\n"
      str += "translate[#{i}] = #{@translate_x[i]}, #{@translate_y[i]}, #{@translate_z[i]}\n"
    end
    return str
  end

  # バイナリデータを出力する
  def OutputBinaryData( filename )
    file = File.open( filename, "wb" )
    file.print( [@frame_size].pack( "I" ) )
    @node_anm.OutputBinaryData( file )
    0.upto( @frame_size - 1 ) do |i|
      file.print( [ @rotate_x[i] ].pack( "f" ) )
      file.print( [ @rotate_y[i] ].pack( "f" ) )
      file.print( [ @rotate_z[i] ].pack( "f" ) )
      file.print( [ @translate_x[i] ].pack( "f" ) )
      file.print( [ @translate_y[i] ].pack( "f" ) )
      file.print( [ @translate_z[i] ].pack( "f" ) )
      puts @translate_x[i].to_s + ", " + @translate_y[i].to_s + ", " + @translate_z[i].to_s
    end
    file.close
  end
end


#================================================================================
# @brief <node_anm>要素を扱うクラス
#================================================================================
class NodeAnm
  # コンストラクタ
  def initialize
    @scale_x = Array.new
    @scale_y = Array.new
    @scale_z = Array.new
    @rotate_x = Array.new
    @rotate_y = Array.new
    @rotate_z = Array.new
    @translate_x = Array.new
    @translate_y = Array.new
    @translate_z = Array.new
  end

  # アクセッサ
  def frameStep_of_scaleX; @scale_x[0]; end
  def dataSize_of_scaleX;  @scale_x[1]; end
  def dataHead_of_scaleX;  @scale_x[2]; end
  def frameStep_of_scaleY; @scale_y[0]; end
  def dataSize_of_scaleY;  @scale_y[1]; end
  def dataHead_of_scaleY;  @scale_y[2]; end
  def frameStep_of_scaleZ; @scale_z[0]; end
  def dataSize_of_scaleZ;  @scale_z[1]; end
  def dataHead_of_scaleZ;  @scale_z[2]; end
  def frameStep_of_rotateX; @rotate_x[0]; end
  def dataSize_of_rotateX;  @rotate_x[1]; end
  def dataHead_of_rotateX;  @rotate_x[2]; end
  def frameStep_of_rotateY; @rotate_y[0]; end
  def dataSize_of_rotateY;  @rotate_y[1]; end
  def dataHead_of_rotateY;  @rotate_y[2]; end
  def frameStep_of_rotateZ; @rotate_z[0]; end
  def dataSize_of_rotateZ;  @rotate_z[1]; end
  def dataHead_of_rotateZ;  @rotate_z[2]; end
  def frameStep_of_translateX; @translate_x[0]; end
  def dataSize_of_translateX;  @translate_x[1]; end
  def dataHead_of_translateX;  @translate_x[2]; end
  def frameStep_of_translateY; @translate_y[0]; end
  def dataSize_of_translateY;  @translate_y[1]; end
  def dataHead_of_translateY;  @translate_y[2]; end
  def frameStep_of_translateZ; @translate_z[0]; end
  def dataSize_of_translateZ;  @translate_z[1]; end
  def dataHead_of_translateZ;  @translate_z[2]; end

  # 指定したインデックスの<node_anm>要素を読み込む
  def read( filename, index )
    file = File.open( filename, "r" )
    extract_flag = false
    file.each do |line|
      if line.index( "<node_anm" )!=nil && line.index( "index" )!=nil then
        i0 = line.index("\"") + 1
        i1 = line.rindex("\"") - 1
        val = line[i0..i1]
        idx = val.to_i
        if index == idx then
          extract_flag = true
        end
      elsif line.index( "</node_anm" )!=nil then
        extract_flag = false
      elsif extract_flag==true then
        i0 = line.index( "frame_step" )
        i1 = line.index( "\"", i0 ) + 1
        i2 = line.index( "\"", i1 ) - 1
        frame_step = line[i1..i2].to_i
        i0 = line.index( "data_size" )
        i1 = line.index( "\"", i0 ) + 1
        i2 = line.index( "\"", i1 ) - 1
        data_size = line[i1..i2].to_i
        i0 = line.index( "data_head" )
        i1 = line.index( "\"", i0 ) + 1
        i2 = line.index( "\"", i1 ) - 1
        data_head = line[i1..i2].to_i 
        if line.index( "scale_x" )!=nil then @scale_x << frame_step << data_size << data_head 
        elsif line.index( "scale_y" )!=nil then @scale_y << frame_step << data_size << data_head
        elsif line.index( "scale_z" )!=nil then @scale_z << frame_step << data_size << data_head 
        elsif line.index( "rotate_x" )!=nil then @rotate_x << frame_step << data_size << data_head 
        elsif line.index( "rotate_y" )!=nil then @rotate_y << frame_step << data_size << data_head
        elsif line.index( "rotate_z" )!=nil then @rotate_z << frame_step << data_size << data_head 
        elsif line.index( "translate_x" )!=nil then @translate_x << frame_step << data_size << data_head 
        elsif line.index( "translate_y" )!=nil then @translate_y << frame_step << data_size << data_head
        elsif line.index( "translate_z" )!=nil then @translate_z << frame_step << data_size << data_head 
        end
      end
    end
    file.close
  end

  # 文字列に変換する
  def to_s
    str = ""
    str += "scale_x : frame_step=" + @scale_x[0].to_s + 
                     " data_size=" + @scale_x[1].to_s + 
                     " data_head=" + @scale_x[2].to_s + "\n"
    str += "scale_y : frame_step=" + @scale_y[0].to_s + 
                     " data_size=" + @scale_y[1].to_s + 
                     " data_head=" + @scale_y[2].to_s + "\n"
    str += "scale_z : frame_step=" + @scale_z[0].to_s + 
                     " data_size=" + @scale_z[1].to_s + 
                     " data_head=" + @scale_z[2].to_s + "\n"
    str += "rotate_x : frame_step=" + @rotate_x[0].to_s + 
                     " data_size=" + @rotate_x[1].to_s + 
                     " data_head=" + @rotate_x[2].to_s + "\n"
    str += "rotate_y : frame_step=" + @rotate_y[0].to_s + 
                     " data_size=" + @rotate_y[1].to_s + 
                     " data_head=" + @rotate_y[2].to_s + "\n"
    str += "rotate_z : frame_step=" + @rotate_z[0].to_s + 
                     " data_size=" + @rotate_z[1].to_s + 
                     " data_head=" + @rotate_z[2].to_s + "\n"
    str += "translate_x : frame_step=" + @translate_x[0].to_s + 
                     " data_size=" + @translate_x[1].to_s + 
                     " data_head=" + @translate_x[2].to_s + "\n"
    str += "translate_y : frame_step=" + @translate_y[0].to_s + 
                     " data_size=" + @translate_y[1].to_s + 
                     " data_head=" + @translate_y[2].to_s + "\n"
    str += "translate_z : frame_step=" + @translate_z[0].to_s + 
                     " data_size=" + @translate_z[1].to_s + 
                     " data_head=" + @translate_z[2].to_s + "\n"
    return str
  end

  # バイナリデータを出力する
  def OutputBinaryData( file )
    file.print( @scale_x.pack( "III" ) )
    file.print( @scale_y.pack( "III" ) )
    file.print( @scale_z.pack( "III" ) )
    file.print( @rotate_x.pack( "III" ) )
    file.print( @rotate_y.pack( "III" ) )
    file.print( @rotate_z.pack( "III" ) )
    file.print( @translate_x.pack( "III" ) )
    file.print( @translate_y.pack( "III" ) )
    file.print( @translate_z.pack( "III" ) )
  end
end


#================================================================================
# @brief icaファイルのデータを扱うクラス
#================================================================================
class Ica

  # コンストラクタ
  def initialize
    @frame_size = 0
    @scale      = Array.new
    @rotate     = Array.new
    @translate  = Array.new
    @node_anm   = Array.new
  end

  # icaファイルを読み込む
  def read( filename )
    self.read_anm_info( filename )
    self.read_scale_data( filename )
    self.read_rotate_data( filename )
    self.read_translate_data( filename )
    self.read_anm_array( filename )
  end

  # <node_anm_info>要素を読み込む
  def read_anm_info( filename )
    file = File.open( filename, "r" )
    extract_flag = false
    file.each do |line|
      if extract_flag==true && line.index( "/>" )!=nil then
        extract_flag = false
      elsif line.index( "<node_anm_info" )!=nil then
        extract_flag = true
      elsif extract_flag==true then
        if line.index( "frame_size" )!=nil then
          i0 = line.index( "\"" ) + 1
          i1 = line.rindex( "\"" ) - 1
          val = line[i0..i1]
          @frame_size = val.to_i
        end
      end
    end
    file.close
  end

  # <node_scale_data>要素を読み込む
  def read_scale_data( filename )
    file = File.open( filename, "r" )
    extract_flag = false
    file.each do |line|
      if line.index( "</node_scale_data" )!=nil then
        extract_flag = false
      elsif line.index( "<node_scale_data" )!=nil then
        extract_flag = true
      elsif extract_flag==true then
        line.strip!
        item = line.split( /\s/ )
        item.each do |val|
          @scale << val.to_f
        end
      end
    end
    file.close
  end

  # <node_rotate_data>を読み込む
  def read_rotate_data( filename )
    file = File.open( filename, "r" )
    extract_flag = false
    file.each do |line|
      if line.index( "</node_rotate_data" )!=nil then
        extract_flag = false
      end
      if extract_flag==true then
        line.strip!
        item = line.split( /\s/ )
        item.each do |val|
          @rotate << val.to_f
        end
      end
      if line.index( "<node_rotate_data" )!=nil then
        extract_flag = true
      end
    end
    file.close
  end

  # <node_translate_data>を読み込む
  def read_translate_data( filename )
    file = File.open( filename, "r" )
    extract_flag = false
    file.each do |line|
      if line.index( "</node_translate_data" )!=nil then
        extract_flag = false
      elsif line.index( "<node_translate_data" )!=nil then
        extract_flag = true
      elsif extract_flag==true then
        line.strip!
        item = line.split( /\s/ )
        item.each do |val|
          @translate << val.to_f
        end
      end
    end
    file.close
  end

  # <node_anm_array>要素を読み込む
  def read_anm_array( filename )
    file = File.open( filename, "r" )
    file.each do |line|
      if line.index( "<node_anm_array" )!=nil then
        i0 = line.index( "size" )
        i1 = line.index( "\"", i0 ) + 1
        i2 = line.index( "\"", i1 ) - 1
        size = line[i1..i2].to_i
        0.upto( size-1 ) do |i|
          node_anm = NodeAnm.new
          node_anm.read( filename, i )
          @node_anm << node_anm
        end
      end
    end
    file.close
  end

  # 文字列に変換する
  def to_s
    str = ""
    str += "frame_size = " + @frame_size.to_s + "\n"
    count = 0
    @scale.each do |val|
      str += "scale[" + count.to_s + "] = " + val.to_s + "\n" 
      count += 1
    end
    count = 0
    @rotate.each do |val|
      str += "rotate[" + count.to_s + "] = " + val.to_s + "\n" 
      count += 1
    end
    count = 0
    @translate.each do |val|
      str += "translate[" + count.to_s + "] = " + val.to_s + "\n" 
      count += 1
    end
    @node_anm.each do |anm|
      str += anm.to_s
    end
    return str
  end

  # コンバートデータを作成する
  def GetConvData
    conv_data = ConvData.new

    # フレーム数
    conv_data.frame_size = @frame_size
    
    # アニメーション属性
    conv_data.node_anm = @node_anm[0]

    # 各フレームの値
    0.upto( @frame_size - 1 ) do |i|
      sx = CalcLinearValue( @node_anm[0].frameStep_of_scaleX, 
                           @node_anm[0].dataSize_of_scaleX, 
                           @node_anm[0].dataHead_of_scaleX, 
                           @scale, i )
      sy = CalcLinearValue( @node_anm[0].frameStep_of_scaleY, 
                           @node_anm[0].dataSize_of_scaleY, 
                           @node_anm[0].dataHead_of_scaleY, 
                           @scale, i )
      sz = CalcLinearValue( @node_anm[0].frameStep_of_scaleZ, 
                           @node_anm[0].dataSize_of_scaleZ, 
                           @node_anm[0].dataHead_of_scaleZ, 
                           @scale, i )
      rx = CalcLinearValue( @node_anm[0].frameStep_of_rotateX, 
                           @node_anm[0].dataSize_of_rotateX, 
                           @node_anm[0].dataHead_of_rotateX, 
                           @rotate, i )
      ry = CalcLinearValue( @node_anm[0].frameStep_of_rotateY, 
                           @node_anm[0].dataSize_of_rotateY, 
                           @node_anm[0].dataHead_of_rotateY, 
                           @rotate, i )
      rz = CalcLinearValue( @node_anm[0].frameStep_of_rotateZ, 
                           @node_anm[0].dataSize_of_rotateZ, 
                           @node_anm[0].dataHead_of_rotateZ, 
                           @rotate, i )
      tx = CalcLinearValue( @node_anm[0].frameStep_of_translateX, 
                           @node_anm[0].dataSize_of_translateX, 
                           @node_anm[0].dataHead_of_translateX, 
                           @translate, i )
      ty = CalcLinearValue( @node_anm[0].frameStep_of_translateY, 
                           @node_anm[0].dataSize_of_translateY, 
                           @node_anm[0].dataHead_of_translateY, 
                           @translate, i )
      tz = CalcLinearValue( @node_anm[0].frameStep_of_translateZ, 
                           @node_anm[0].dataSize_of_translateZ, 
                           @node_anm[0].dataHead_of_translateZ, 
                           @translate, i )
      conv_data.AddScaleData( sx, sy, sz )
      conv_data.AddRotateData( rx, ry, rz )
      conv_data.AddTranslateData( tx, ty, tz )
    end

    return conv_data
  end

  # 指定フレームの値を計算する
  def CalcLinearValue( frame_step, data_size, data_head, data_array, frame ) 
    # ループさせる
    frame = frame % @frame_size

    if data_size==1 then  # データが1つの場合
      return data_array[ data_head ]
    end

    if frame_step==1 then   # frame_stepが1の場合
      return data_array[ data_head + frame ]
    end

    shift = ( frame_step==2 ) ? 1 : 2
    ival  = frame >> shift
    ival += data_head

    # 間聞き部分と全フレーム部分のしきいとなるフレーム
    iframe_last_interp = ((@frame_size - 1) >> shift) << shift

    # しきいとなるフレーム以降の場合, 実データ配列から直接返す
    if iframe_last_interp <= frame then
      return data_array[ ival + frame - iframe_last_interp ]
    end

    # しきいとなるフレームより前の場合は補間計算で求める
    val0 = data_array[ ival ]
    val1 = data_array[ ival + 1 ]
    w1   = frame - (ival << shift ) 
    w0   = frame_step - w1

    # ちょうどフレーム上にある場合
    if w1==0 then
      return val0
    # 通常の補間処理
    else
      return ( (val0 * w0 + val1 * w1) / (w0 + w1) )
    end 
  end
end



#================================================================================
#
# @brief メイン
#
# @param ARGV[0] コンバート対象のicaファイル名
# @param ARGV[1] 出力するバイナリデータのファイル名
#
#================================================================================
ica = Ica.new
ica.read( ARGV[0] )
conv_data = ica.GetConvData
conv_data.OutputBinaryData( ARGV[1] )
