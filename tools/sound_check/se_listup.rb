# SE �̑S���X�g���쐬
all_se_list = Array.new
file = File::open( "../../prog/include/sound/wb_sound_data.sadl", "r" )
lines = file.readlines
file.close

if lines then 
  lines.each do |line|
    if line =~ /(SEQ_SE_\w*)/ then 
      all_se_list << $1
    end
  end
end

# �S�Ă� SE �ɑΉ�����n�b�V�����쐬
se_hash = Hash.new 
all_se_list.each do |label|
  se_hash[ label ] = Array.new
end

# ����
file_list = Dir.glob( "../../prog/src/**/*" ) # �v���O�����\�[�X
file_list += Dir.glob( "../../resource/fldmapdata/script/**/*.ev" ) # �X�N���v�g
file_list.each do |filename|
  file = File::open( filename, "r" )
  lines = nil
  if File.file?( file ) then
    lines = file.readlines
  end
  file.close

  if lines then 
    lines.each do |line|
      if line =~ /(SEQ_SE_\w*)/ then 
        label = $1
        if all_se_list.include?( label ) then
          se_hash[ label ] << filename
        end
      end
    end
  end
end

# �o��
out_file = File.open( "result.txt", "w" )
all_se_list.each do |label|
  out_file.puts( label )
  se_hash[ label ].each do |filename|
    out_file.puts( "\t#{filename}" )
  end
end
out_file.close
