require "gmm_generator.rb"


directory = "."
fileName = "test.gmm"
stringLavel = Array.new
stringJPN = Array.new
stringJPN_KANJI = Array.new


stringLavel << "test1"
stringJPN << "�Ă���"
stringJPN_KANJI << "�e�X�g"

stringLavel << "test2"
stringJPN << "test"
stringJPN_KANJI << "TEST"


GenerateGMM( directory, fileName, stringLavel, stringJPN, stringJPN_KANJI )
