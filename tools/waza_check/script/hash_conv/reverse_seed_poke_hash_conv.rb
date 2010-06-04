# @brief  ��|�P�����n�b�V���𐶐�����
# @file   reverse_seed_poke_hash_conv.rb
# @author obata
# @date   2010.06.01

require "monsno_hash.rb"
require "evolve_hash_conv.rb"
require "personal_parser.rb"
require "seed_poke_hash_conv.rb"

def OutputReverseSeedPokeHash_from_PersonalData( personal_filename, output_path )
  personal_parser = PersonalDataParser.new( personal_filename ) 
  reverse_seed_poke_hash = CreateReverseSeedPokeHash_from_PersonalData( personal_filename )
  reverse_seed_poke_hash = RegisterExceptionalBabyPokemon( reverse_seed_poke_hash )
  out_data = CreateOutputData_of_ReverseSeedPokeHash( personal_parser, reverse_seed_poke_hash )
  OutputReverseSeedPokeHashData_to_File( out_data, output_path, "reverse_seed_poke_hash.rb" )
end

def CreateReverseSeedPokeHash_from_PersonalData( personal_filename )
  personal_parser = PersonalDataParser.new( personal_filename ) 
  mons_fullname_list = personal_parser.get_mons_fullname_list
  reverse_seed_poke_hash = CreateNullHash_for_ReverseSeedPokeHash( personal_parser )
  seed_poke_hash = CreateSeedPokeHash_from_PersonalData( personal_parser )
  mons_fullname_list.each do |mons_fullname|
    seed_poke_list = seed_poke_hash[ mons_fullname ]
    seed_poke_list.each do |seed_mons_name|
      reverse_seed_poke_hash[ seed_mons_name ] << mons_fullname
    end
  end
  return reverse_seed_poke_hash
end

def GetSeedPokemon( reverse_evolve_hash, mons_fullname )
  seed_mons_name = GetPrevEvolvePokemon( reverse_evolve_hash, mons_fullname )
  if seed_mons_name == nil || seed_mons_name == mons_fullname then
    return mons_fullname
  else
    return GetSeedPokemon( reverse_evolve_hash, seed_mons_name )
  end
end

def GetSeedPokemonList( reverse_evolve_hash, mons_fullname )
  seed_poke_list = Array.new
end

def GetPrevEvolvePokemon( reverse_evolve_hash, mons_fullname ) 
  prev_mons_list = reverse_evolve_hash[ mons_fullname ]
  if prev_mons_list == nil then
    return mons_fullname
  else 
    return prev_mons_list[0]
  end
end

def CreateNullHash_for_ReverseSeedPokeHash( personal_parser )
  hash = Hash.new
  mons_fullname_list = personal_parser.get_mons_fullname_list
  mons_fullname_list.each do |mons_fullname|
    hash[ mons_fullname ] = Array.new
  end
  return hash
end

# ��O�P�[�X��o�^���� ( �׃r�B�|�P���� )
def RegisterExceptionalBabyPokemon( reverse_seed_poke_hash )
  reverse_seed_poke_hash[ "�}����" ] << "�}����"
  reverse_seed_poke_hash[ "�}����" ] << "�}������"
  reverse_seed_poke_hash[ "�\�[�i���X" ] << "�\�[�i���X"
  reverse_seed_poke_hash[ "�o�����[�h" ] << "�o�����[�h"
  reverse_seed_poke_hash[ "�E�\�b�L�[" ] << "�E�\�b�L�["
  reverse_seed_poke_hash[ "�}���^�C��" ] << "�}���^�C��"
  reverse_seed_poke_hash[ "�J�r�S��" ] << "�J�r�S��"
  reverse_seed_poke_hash[ "���[���A" ] << "���[���A"
  reverse_seed_poke_hash[ "���[���A" ] << "���Y���C�h"
  reverse_seed_poke_hash[ "���b�L�[" ] << "���b�L�["
  reverse_seed_poke_hash[ "���b�L�[" ] << "�n�s�i�X"
  reverse_seed_poke_hash[ "�`���[��" ] << "�`���[��"
  return reverse_seed_poke_hash
end

def CreateOutputData_of_ReverseSeedPokeHash( personal_parser, reverse_seed_poke_hash )
  out_data = Array.new
  out_data << "$reverse_seed_poke_hash = {"
  mons_fullname_list = personal_parser.get_mons_fullname_list
  mons_fullname_list.each do |mons_fullname|
    out_data << "\t\"#{mons_fullname}\" => ["
    ev_poke_list = reverse_seed_poke_hash[ mons_fullname ]
    ev_poke_list.each do |evolve_poke_name|
      out_data << "\t\t\"#{evolve_poke_name}\","
    end
    out_data << "\t],"
  end
  out_data << "}"
  return out_data
end

def OutputReverseSeedPokeHashData_to_File( out_data, output_path, filename )
  output_filename = output_path + filename
  output_file = File.open( output_filename, "w" )
  out_data.each do |data|
    output_file.puts( data )
  end
  output_file.close
end
