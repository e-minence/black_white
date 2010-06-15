#! ruby -Ks

	$tokusei_hash = {
		"あくしゅう"=>1,
		"あめふらし"=>2,
		"かそく"=>3,
		"カブトアーマー"=>4,
		"がんじょう"=>5,
		"しめりけ"=>6,
		"じゅうなん"=>7,
		"すながくれ"=>8,
		"せいでんき"=>9,
		"ちくでん"=>10,
		"ちょすい"=>11,
		"どんかん"=>12,
		"ノーてんき"=>13,
		"ふくがん"=>14,
		"ふみん"=>15,
		"へんしょく"=>16,
		"めんえき"=>17,
		"もらいび"=>18,
		"りんぷん"=>19,
		"マイペース"=>20,
		"きゅうばん"=>21,
		"いかく"=>22,
		"かげふみ"=>23,
		"さめはだ"=>24,
		"ふしぎなまもり"=>25,
		"ふゆう"=>26,
		"ほうし"=>27,
		"シンクロ"=>28,
		"クリアボディ"=>29,
		"しぜんかいふく"=>30,
		"ひらいしん"=>31,
		"てんのめぐみ"=>32,
		"すいすい"=>33,
		"ようりょくそ"=>34,
		"はっこう"=>35,
		"トレース"=>36,
		"ちからもち"=>37,
		"どくのトゲ"=>38,
		"せいしんりょく"=>39,
		"マグマのよろい"=>40,
		"みずのベール"=>41,
		"じりょく"=>42,
		"ぼうおん"=>43,
		"あめうけざら"=>44,
		"すなおこし"=>45,
		"プレッシャー"=>46,
		"あついしぼう"=>47,
		"はやおき"=>48,
		"ほのおのからだ"=>49,
		"にげあし"=>50,
		"するどいめ"=>51,
		"かいりきバサミ"=>52,
		"ものひろい"=>53,
		"なまけ"=>54,
		"はりきり"=>55,
		"メロメロボディ"=>56,
		"プラス"=>57,
		"マイナス"=>58,
		"てんきや"=>59,
		"ねんちゃく"=>60,
		"だっぴ"=>61,
		"こんじょう"=>62,
		"ふしぎなうろこ"=>63,
		"ヘドロえき"=>64,
		"しんりょく"=>65,
		"もうか"=>66,
		"げきりゅう"=>67,
		"むしのしらせ"=>68,
		"いしあたま"=>69,
		"ひでり"=>70,
		"ありじごく"=>71,
		"やるき"=>72,
		"しろいけむり"=>73,
		"ヨガパワー"=>74,
		"シェルアーマー"=>75,
		"エアロック"=>76,
		"ちどりあし"=>77,
		"でんきエンジン"=>78,
		"とうそうしん"=>79,
		"ふくつのこころ"=>80,
		"ゆきがくれ"=>81,
		"くいしんぼう"=>82,
		"いかりのつぼ"=>83,
		"かるわざ"=>84,
		"たいねつ"=>85,
		"たんじゅん"=>86,
		"かんそうはだ"=>87,
		"ダウンロード"=>88,
		"てつのこぶし"=>89,
		"ポイズンヒール"=>90,
		"てきおうりょく"=>91,
		"スキルリンク"=>92,
		"うるおいボディ"=>93,
		"サンパワー"=>94,
		"はやあし"=>95,
		"ノーマルスキン"=>96,
		"スナイパー"=>97,
		"マジックガード"=>98,
		"ノーガード"=>99,
		"あとだし"=>100,
		"テクニシャン"=>101,
		"リーフガード"=>102,
		"ぶきよう"=>103,
		"かたやぶり"=>104,
		"きょううん"=>105,
		"ゆうばく"=>106,
		"きけんよち"=>107,
		"よちむ"=>108,
		"てんねん"=>109,
		"いろめがね"=>110,
		"フィルター"=>111,
		"スロースタート"=>112,
		"きもったま"=>113,
		"よびみず"=>114,
		"アイスボディ"=>115,
		"ハードロック"=>116,
		"ゆきふらし"=>117,
		"みつあつめ"=>118,
		"おみとおし"=>119,
		"すてみ"=>120,
		"マルチタイプ"=>121,
		"フラワーギフト"=>122,
		"ナイトメア"=>123,
		"わるいてぐせ"=>124,
		"ちからずく"=>125,
		"あまのじゃく"=>126,
		"きんちょうかん"=>127,
		"まけんき"=>128,
		"よわき"=>129,
		"のろわれボディ"=>130,
		"いやしのこころ"=>131,
		"フレンドガード"=>132,
		"くだけるよろい"=>133,
		"ヘヴィメタル"=>134,
		"ライトメタル"=>135,
		"マルチスケイル"=>136,
		"どくぼうそう"=>137,
		"ねつぼうそう"=>138,
		"しゅうかく"=>139,
		"テレパシー"=>140,
		"ムラっけ"=>141,
		"ぼうじん"=>142,
		"どくしゅ"=>143,
		"さいせいりょく"=>144,
		"はとむね"=>145,
		"すなかき"=>146,
		"ミラクルスキン"=>147,
		"アナライズ"=>148,
		"イリュージョン"=>149,
		"かわりもの"=>150,
		"すりぬけ"=>151,
		"ミイラ"=>152,
		"じしんかじょう"=>153,
		"せいぎのこころ"=>154,
		"びびり"=>155,
		"マジックミラー"=>156,
		"そうしょく"=>157,
		"いたずらごころ"=>158,
		"すなのちから"=>159,
		"てつのトゲ"=>160,
		"ダルマモード"=>161,
		"しょうりのほし"=>162,
		"ターボブレイズ"=>163,
		"テラボルテージ"=>164,
	}
