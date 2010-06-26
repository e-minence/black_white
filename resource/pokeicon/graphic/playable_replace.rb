# @brief  �̌��łŏo�����Ȃ��|�P�����̃A�C�R�����u�H�v�ɒu��������
# @file   playable_replace.rb
# @author obata
# @date   2010.06.26

require "fileutils.rb"
require "../../../tools/hash/monsno_hash.rb"
require "../../playable_keepmons.rb"


use_monsno = Array.new

# �u���ΏۊO�̃����X�^�[No.�ꗗ���쐬
$keep_monsname.each do |mons_name|
  use_monsno << $monsno_hash[ mons_name ]
end

# �u��
Dir[ "*_f.NCGR" ].each do |file_name|
  if file_name =~ /poke_icon_(\d+).*_f.NCGR/ then
    grano = $1
    monsno = $gra2zukan_hash[ grano ]
    if monsno != 0 && use_monsno.include?( monsno ) == false then
      FileUtils.copy( "poke_icon_000_f.NCGR", file_name )
      puts "replace #{file_name}"
    end
  end
end

Dir[ "*_m.NCGR" ].each do |file_name|
  if file_name =~ /poke_icon_(\d+).*_m.NCGR/ then
    grano = $1
    monsno = $gra2zukan_hash[ grano ]
    if monsno != 0 && use_monsno.include?( monsno ) == false then
      FileUtils.copy( "poke_icon_000_m.NCGR", file_name )
      puts "replace #{file_name}"
    end
  end
end

Dir[ "*_m.NANR" ].each do |file_name|
  if file_name =~ /poke_icon_(\d+).*_m.NANR/ then
    grano = $1
    monsno = $gra2zukan_hash[ grano ]
    if monsno != 0 && use_monsno.include?( monsno ) == false then
      FileUtils.copy( "poke_icon_000_m.NANR", file_name )
      puts "replace #{file_name}"
    end
  end
end
