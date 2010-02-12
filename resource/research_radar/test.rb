require "gmm_generator.rb"


directory = "."
fileName = "test.gmm"
stringLavel = Array.new
stringJPN = Array.new
stringJPN_KANJI = Array.new


stringLavel << "test1"
stringJPN << "てすと"
stringJPN_KANJI << "テスト"

stringLavel << "test2"
stringJPN << "test"
stringJPN_KANJI << "TEST"


GenerateGMM( directory, fileName, stringLavel, stringJPN, stringJPN_KANJI )
