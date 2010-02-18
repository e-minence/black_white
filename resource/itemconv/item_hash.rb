#! ruby -Ks

	$item_hash = {
		"−−−−−−−−"=>0,
		"マスターボール"=>1,
		"ハイパーボール"=>2,
		"スーパーボール"=>3,
		"モンスターボール"=>4,
		"サファリボール"=>5,
		"ネットボール"=>6,
		"ダイブボール"=>7,
		"ネストボール"=>8,
		"リピートボール"=>9,
		"タイマーボール"=>10,
		"ゴージャスボール"=>11,
		"プレミアボール"=>12,
		"ダークボール"=>13,
		"ヒールボール"=>14,
		"クイックボール"=>15,
		"プレシャスボール"=>16,
		"キズぐすり"=>17,
		"どくけし"=>18,
		"やけどなおし"=>19,
		"こおりなおし"=>20,
		"ねむけざまし"=>21,
		"まひなおし"=>22,
		"かいふくのくすり"=>23,
		"まんたんのくすり"=>24,
		"すごいキズぐすり"=>25,
		"いいキズぐすり"=>26,
		"なんでもなおし"=>27,
		"げんきのかけら"=>28,
		"げんきのかたまり"=>29,
		"おいしいみず"=>30,
		"サイコソーダ"=>31,
		"ミックスオレ"=>32,
		"モーモーミルク"=>33,
		"ちからのこな"=>34,
		"ちからのねっこ"=>35,
		"ばんのうごな"=>36,
		"ふっかつそう"=>37,
		"ピーピーエイド"=>38,
		"ピーピーリカバー"=>39,
		"ピーピーエイダー"=>40,
		"ピーピーマックス"=>41,
		"フエンせんべい"=>42,
		"きのみジュース"=>43,
		"せいなるはい"=>44,
		"マックスアップ"=>45,
		"タウリン"=>46,
		"ブロムヘキシン"=>47,
		"インドメタシン"=>48,
		"リゾチウム"=>49,
		"ふしぎなアメ"=>50,
		"ポイントアップ"=>51,
		"キトサン"=>52,
		"ポイントマックス"=>53,
		"もりのヨウカン"=>54,
		"エフェクトガード"=>55,
		"クリティカッター"=>56,
		"プラスパワー"=>57,
		"ディフェンダー"=>58,
		"スピーダー"=>59,
		"ヨクアタール"=>60,
		"スペシャルアップ"=>61,
		"スペシャルガード"=>62,
		"ピッピにんぎょう"=>63,
		"エネコのシッポ"=>64,
		"あおいビードロ"=>65,
		"きいろビードロ"=>66,
		"あかいビードロ"=>67,
		"くろいビードロ"=>68,
		"しろいビードロ"=>69,
		"あさせのしお"=>70,
		"あさせのかいがら"=>71,
		"あかいかけら"=>72,
		"あおいかけら"=>73,
		"きいろいかけら"=>74,
		"みどりのかけら"=>75,
		"シルバースプレー"=>76,
		"ゴールドスプレー"=>77,
		"あなぬけのヒモ"=>78,
		"むしよけスプレー"=>79,
		"たいようのいし"=>80,
		"つきのいし"=>81,
		"ほのおのいし"=>82,
		"かみなりのいし"=>83,
		"みずのいし"=>84,
		"リーフのいし"=>85,
		"ちいさなキノコ"=>86,
		"おおきなキノコ"=>87,
		"しんじゅ"=>88,
		"おおきなしんじゅ"=>89,
		"ほしのすな"=>90,
		"ほしのかけら"=>91,
		"きんのたま"=>92,
		"ハートのウロコ"=>93,
		"あまいミツ"=>94,
		"すくすくこやし"=>95,
		"じめじめこやし"=>96,
		"ながながこやし"=>97,
		"ねばねばこやし"=>98,
		"ねっこのカセキ"=>99,
		"ツメのカセキ"=>100,
		"かいのカセキ"=>101,
		"こうらのカセキ"=>102,
		"ひみつのコハク"=>103,
		"たてのカセキ"=>104,
		"ずがいのカセキ"=>105,
		"きちょうなホネ"=>106,
		"ひかりのいし"=>107,
		"やみのいし"=>108,
		"めざめいし"=>109,
		"まんまるいし"=>110,
		"かなめいし"=>111,
		"はっきんだま"=>112,
		"？？？"=>113,
		"？？？"=>114,
		"？？？"=>115,
		"？？？"=>116,
		"？？？"=>117,
		"？？？"=>118,
		"？？？"=>119,
		"？？？"=>120,
		"？？？"=>121,
		"？？？"=>122,
		"？？？"=>123,
		"？？？"=>124,
		"？？？"=>125,
		"？？？"=>126,
		"？？？"=>127,
		"？？？"=>128,
		"？？？"=>129,
		"？？？"=>130,
		"？？？"=>131,
		"？？？"=>132,
		"？？？"=>133,
		"？？？"=>134,
		"こんごうだま"=>135,
		"しらたま"=>136,
		"グラスメール"=>137,
		"フレイムメール"=>138,
		"ブルーメール"=>139,
		"ブルームメール"=>140,
		"トンネルメール"=>141,
		"スチールメール"=>142,
		"ラブラブメール"=>143,
		"ブリザードメール"=>144,
		"スペースメール"=>145,
		"エアメール"=>146,
		"モザイクメール"=>147,
		"ブリックメール"=>148,
		"クラボのみ"=>149,
		"カゴのみ"=>150,
		"モモンのみ"=>151,
		"チーゴのみ"=>152,
		"ナナシのみ"=>153,
		"ヒメリのみ"=>154,
		"オレンのみ"=>155,
		"キーのみ"=>156,
		"ラムのみ"=>157,
		"オボンのみ"=>158,
		"フィラのみ"=>159,
		"ウイのみ"=>160,
		"マゴのみ"=>161,
		"バンジのみ"=>162,
		"イアのみ"=>163,
		"ズリのみ"=>164,
		"ブリーのみ"=>165,
		"ナナのみ"=>166,
		"セシナのみ"=>167,
		"パイルのみ"=>168,
		"ザロクのみ"=>169,
		"ネコブのみ"=>170,
		"タポルのみ"=>171,
		"ロメのみ"=>172,
		"ウブのみ"=>173,
		"マトマのみ"=>174,
		"モコシのみ"=>175,
		"ゴスのみ"=>176,
		"ラブタのみ"=>177,
		"ノメルのみ"=>178,
		"ノワキのみ"=>179,
		"シーヤのみ"=>180,
		"カイスのみ"=>181,
		"ドリのみ"=>182,
		"ベリブのみ"=>183,
		"オッカのみ"=>184,
		"イトケのみ"=>185,
		"ソクノのみ"=>186,
		"リンドのみ"=>187,
		"ヤチェのみ"=>188,
		"ヨプのみ"=>189,
		"ビアーのみ"=>190,
		"シュカのみ"=>191,
		"バコウのみ"=>192,
		"ウタンのみ"=>193,
		"タンガのみ"=>194,
		"ヨロギのみ"=>195,
		"カシブのみ"=>196,
		"ハバンのみ"=>197,
		"ナモのみ"=>198,
		"リリバのみ"=>199,
		"ホズのみ"=>200,
		"チイラのみ"=>201,
		"リュガのみ"=>202,
		"カムラのみ"=>203,
		"ヤタピのみ"=>204,
		"ズアのみ"=>205,
		"サンのみ"=>206,
		"スターのみ"=>207,
		"ナゾのみ"=>208,
		"ミクルのみ"=>209,
		"イバンのみ"=>210,
		"ジャポのみ"=>211,
		"レンブのみ"=>212,
		"ひかりのこな"=>213,
		"しろいハーブ"=>214,
		"きょうせいギプス"=>215,
		"がくしゅうそうち"=>216,
		"せんせいのツメ"=>217,
		"やすらぎのすず"=>218,
		"メンタルハーブ"=>219,
		"こだわりハチマキ"=>220,
		"おうじゃのしるし"=>221,
		"ぎんのこな"=>222,
		"おまもりこばん"=>223,
		"きよめのおふだ"=>224,
		"こころのしずく"=>225,
		"しんかいのキバ"=>226,
		"しんかいのウロコ"=>227,
		"けむりだま"=>228,
		"かわらずのいし"=>229,
		"きあいのハチマキ"=>230,
		"しあわせタマゴ"=>231,
		"ピントレンズ"=>232,
		"メタルコート"=>233,
		"たべのこし"=>234,
		"りゅうのウロコ"=>235,
		"でんきだま"=>236,
		"やわらかいすな"=>237,
		"かたいいし"=>238,
		"きせきのタネ"=>239,
		"くろいメガネ"=>240,
		"くろおび"=>241,
		"じしゃく"=>242,
		"しんぴのしずく"=>243,
		"するどいくちばし"=>244,
		"どくバリ"=>245,
		"とけないこおり"=>246,
		"のろいのおふだ"=>247,
		"まがったスプーン"=>248,
		"もくたん"=>249,
		"りゅうのキバ"=>250,
		"シルクのスカーフ"=>251,
		"アップグレード"=>252,
		"かいがらのすず"=>253,
		"うしおのおこう"=>254,
		"のんきのおこう"=>255,
		"ラッキーパンチ"=>256,
		"メタルパウダー"=>257,
		"ふといホネ"=>258,
		"ながねぎ"=>259,
		"あかいバンダナ"=>260,
		"あおいバンダナ"=>261,
		"ピンクのバンダナ"=>262,
		"みどりのバンダナ"=>263,
		"きいろのバンダナ"=>264,
		"こうかくレンズ"=>265,
		"ちからのハチマキ"=>266,
		"ものしりメガネ"=>267,
		"たつじんのおび"=>268,
		"ひかりのねんど"=>269,
		"いのちのたま"=>270,
		"パワフルハーブ"=>271,
		"どくどくだま"=>272,
		"かえんだま"=>273,
		"スピードパウダー"=>274,
		"きあいのタスキ"=>275,
		"フォーカスレンズ"=>276,
		"メトロノーム"=>277,
		"くろいてっきゅう"=>278,
		"こうこうのしっぽ"=>279,
		"あかいいと"=>280,
		"くろいヘドロ"=>281,
		"つめたいいわ"=>282,
		"さらさらいわ"=>283,
		"あついいわ"=>284,
		"しめったいわ"=>285,
		"ねばりのかぎづめ"=>286,
		"こだわりスカーフ"=>287,
		"くっつきバリ"=>288,
		"パワーリスト"=>289,
		"パワーベルト"=>290,
		"パワーレンズ"=>291,
		"パワーバンド"=>292,
		"パワーアンクル"=>293,
		"パワーウエイト"=>294,
		"きれいなぬけがら"=>295,
		"おおきなねっこ"=>296,
		"こだわりメガネ"=>297,
		"ひのたまプレート"=>298,
		"しずくプレート"=>299,
		"いかずちプレート"=>300,
		"みどりのプレート"=>301,
		"つららのプレート"=>302,
		"こぶしのプレート"=>303,
		"もうどくプレート"=>304,
		"だいちのプレート"=>305,
		"あおぞらプレート"=>306,
		"ふしぎのプレート"=>307,
		"たまむしプレート"=>308,
		"がんせきプレート"=>309,
		"もののけプレート"=>310,
		"りゅうのプレート"=>311,
		"こわもてプレート"=>312,
		"こうてつプレート"=>313,
		"あやしいおこう"=>314,
		"がんせきおこう"=>315,
		"まんぷくおこう"=>316,
		"さざなみのおこう"=>317,
		"おはなのおこう"=>318,
		"こううんのおこう"=>319,
		"きよめのおこう"=>320,
		"プロテクター"=>321,
		"エレキブースター"=>322,
		"マグマブースター"=>323,
		"あやしいパッチ"=>324,
		"れいかいのぬの"=>325,
		"するどいツメ"=>326,
		"するどいキバ"=>327,
		"わざマシン０１"=>328,
		"わざマシン０２"=>329,
		"わざマシン０３"=>330,
		"わざマシン０４"=>331,
		"わざマシン０５"=>332,
		"わざマシン０６"=>333,
		"わざマシン０７"=>334,
		"わざマシン０８"=>335,
		"わざマシン０９"=>336,
		"わざマシン１０"=>337,
		"わざマシン１１"=>338,
		"わざマシン１２"=>339,
		"わざマシン１３"=>340,
		"わざマシン１４"=>341,
		"わざマシン１５"=>342,
		"わざマシン１６"=>343,
		"わざマシン１７"=>344,
		"わざマシン１８"=>345,
		"わざマシン１９"=>346,
		"わざマシン２０"=>347,
		"わざマシン２１"=>348,
		"わざマシン２２"=>349,
		"わざマシン２３"=>350,
		"わざマシン２４"=>351,
		"わざマシン２５"=>352,
		"わざマシン２６"=>353,
		"わざマシン２７"=>354,
		"わざマシン２８"=>355,
		"わざマシン２９"=>356,
		"わざマシン３０"=>357,
		"わざマシン３１"=>358,
		"わざマシン３２"=>359,
		"わざマシン３３"=>360,
		"わざマシン３４"=>361,
		"わざマシン３５"=>362,
		"わざマシン３６"=>363,
		"わざマシン３７"=>364,
		"わざマシン３８"=>365,
		"わざマシン３９"=>366,
		"わざマシン４０"=>367,
		"わざマシン４１"=>368,
		"わざマシン４２"=>369,
		"わざマシン４３"=>370,
		"わざマシン４４"=>371,
		"わざマシン４５"=>372,
		"わざマシン４６"=>373,
		"わざマシン４７"=>374,
		"わざマシン４８"=>375,
		"わざマシン４９"=>376,
		"わざマシン５０"=>377,
		"わざマシン５１"=>378,
		"わざマシン５２"=>379,
		"わざマシン５３"=>380,
		"わざマシン５４"=>381,
		"わざマシン５５"=>382,
		"わざマシン５６"=>383,
		"わざマシン５７"=>384,
		"わざマシン５８"=>385,
		"わざマシン５９"=>386,
		"わざマシン６０"=>387,
		"わざマシン６１"=>388,
		"わざマシン６２"=>389,
		"わざマシン６３"=>390,
		"わざマシン６４"=>391,
		"わざマシン６５"=>392,
		"わざマシン６６"=>393,
		"わざマシン６７"=>394,
		"わざマシン６８"=>395,
		"わざマシン６９"=>396,
		"わざマシン７０"=>397,
		"わざマシン７１"=>398,
		"わざマシン７２"=>399,
		"わざマシン７３"=>400,
		"わざマシン７４"=>401,
		"わざマシン７５"=>402,
		"わざマシン７６"=>403,
		"わざマシン７７"=>404,
		"わざマシン７８"=>405,
		"わざマシン７９"=>406,
		"わざマシン８０"=>407,
		"わざマシン８１"=>408,
		"わざマシン８２"=>409,
		"わざマシン８３"=>410,
		"わざマシン８４"=>411,
		"わざマシン８５"=>412,
		"わざマシン８６"=>413,
		"わざマシン８７"=>414,
		"わざマシン８８"=>415,
		"わざマシン８９"=>416,
		"わざマシン９０"=>417,
		"わざマシン９１"=>418,
		"わざマシン９２"=>419,
		"ひでんマシン０１"=>420,
		"ひでんマシン０２"=>421,
		"ひでんマシン０３"=>422,
		"ひでんマシン０４"=>423,
		"ひでんマシン０５"=>424,
		"ひでんマシン０６"=>425,
		"ひでんマシン０７"=>426,
		"ひでんマシン０８"=>427,
		"たんけんセット"=>428,
		"たからぶくろ"=>429,
		"ルールブック"=>430,
		"ポケトレ"=>431,
		"ポイントカード"=>432,
		"ぼうけんノート"=>433,
		"シールいれ"=>434,
		"アクセサリーいれ"=>435,
		"シールぶくろ"=>436,
		"ともだちてちょう"=>437,
		"はつでんしょキー"=>438,
		"こだいのおまもり"=>439,
		"ギンガだんのカギ"=>440,
		"あかいくさり"=>441,
		"タウンマップ"=>442,
		"バトルサーチャー"=>443,
		"コインケース"=>444,
		"ボロのつりざお"=>445,
		"いいつりざお"=>446,
		"すごいつりざお"=>447,
		"コダックじょうろ"=>448,
		"ポフィンケース"=>449,
		"じてんしゃ"=>450,
		"ルームキー"=>451,
		"オーキドのてがみ"=>452,
		"みかづきのはね"=>453,
		"メンバーズカード"=>454,
		"てんかいのふえ"=>455,
		"ふねのチケット"=>456,
		"コンテストパス"=>457,
		"かざんのおきいし"=>458,
		"おとどけもの"=>459,
		"ひきかえけん１"=>460,
		"ひきかえけん２"=>461,
		"ひきかえけん３"=>462,
		"そうこのカギ"=>463,
		"ひでんのくすり"=>464,
		"バトルレコーダー"=>465,
		"グラシデアのはな"=>466,
		"ひみつのカギ"=>467,
		"ぼんぐりケース"=>468,
		"アンノーンノート"=>469,
		"きのみプランター"=>470,
		"ダウジングマシン"=>471,
		"ブルーカード"=>472,
		"おいしいシッポ"=>473,
		"とうめいなスズ"=>474,
		"カードキー"=>475,
		"ちかのかぎ"=>476,
		"ゼニガメじょうろ"=>477,
		"あかいウロコ"=>478,
		"おとしもの"=>479,
		"リニアパス"=>480,
		"きかいのぶひん"=>481,
		"ぎんいろのはね"=>482,
		"にじいろのはね"=>483,
		"ふしぎなタマゴ"=>484,
		"あかぼんぐり"=>485,
		"あおぼんぐり"=>486,
		"きぼんぐり"=>487,
		"みどぼんぐり"=>488,
		"ももぼんぐり"=>489,
		"しろぼんぐり"=>490,
		"くろぼんぐり"=>491,
		"スピードボール"=>492,
		"レベルボール"=>493,
		"ルアーボール"=>494,
		"ヘビーボール"=>495,
		"ラブラブボール"=>496,
		"フレンドボール"=>497,
		"ムーンボール"=>498,
		"コンペボール"=>499,
		"パークボール"=>500,
		"フォトアルバム"=>501,
		"ＧＢプレイヤー"=>502,
		"うみなりのスズ"=>503,
		"いかりまんじゅう"=>504,
		"データカード０１"=>505,
		"データカード０２"=>506,
		"データカード０３"=>507,
		"データカード０４"=>508,
		"データカード０５"=>509,
		"データカード０６"=>510,
		"データカード０７"=>511,
		"データカード０８"=>512,
		"データカード０９"=>513,
		"データカード１０"=>514,
		"データカード１１"=>515,
		"データカード１２"=>516,
		"データカード１３"=>517,
		"データカード１４"=>518,
		"データカード１５"=>519,
		"データカード１６"=>520,
		"データカード１７"=>521,
		"データカード１８"=>522,
		"データカード１９"=>523,
		"データカード２０"=>524,
		"データカード２１"=>525,
		"データカード２２"=>526,
		"データカード２３"=>527,
		"データカード２４"=>528,
		"データカード２５"=>529,
		"データカード２６"=>530,
		"データカード２７"=>531,
		"もえぎいろのたま"=>532,
		"ロックカプセル"=>533,
		"べにいろのたま"=>534,
		"あいいろのたま"=>535,
		"なぞのすいしょう"=>536,
		"パレスへゴー"=>537,
		"クリティカット２"=>538,
		"スピーダー２"=>539,
		"ＳＰアップ２"=>540,
		"ＳＰガード２"=>541,
		"ディフェンダー２"=>542,
		"プラスパワー２"=>543,
		"ヨクアタール２"=>544,
		"スピーダー３"=>545,
		"ＳＰアップ３"=>546,
		"ＳＰガード３"=>547,
		"ディフェンダー３"=>548,
		"プラスパワー３"=>549,
		"ヨクアタール３"=>550,
		"スピーダー６"=>551,
		"ＳＰアップ６"=>552,
		"ＳＰガード６"=>553,
		"ディフェンダー６"=>554,
		"プラスパワー６"=>555,
		"ヨクアタール６"=>556,
		"スキルコール"=>557,
		"アイテムドロップ"=>558,
		"アイテムコール"=>559,
		"フラットコール"=>560,
		"クリティカット３"=>561,
		"かるいし"=>562,
		"しんかのきせき"=>562,
		"ゴツゴツメット"=>563,
		"ふうせん"=>564,
		"レッドカード"=>565,
		"ねらいのまと"=>566,
		"しめつけバンド"=>567,
		"きゅうこん"=>568,
		"じゅうでんち"=>569,
		"だっしゅつポッド"=>570,
		"ほのおのジュエル"=>571,
		"みずのジュエル"=>572,
		"でんきのジュエル"=>573,
		"くさのジュエル"=>574,
		"こおりのジュエル"=>575,
		"かくとうジュエル"=>576,
		"どくのジュエル"=>577,
		"じめんのジュエル"=>578,
		"ひこうのジュエル"=>579,
		"エスパージュエル"=>580,
		"むしのジュエル"=>581,
		"いわのジュエル"=>582,
		"ゴーストジュエル"=>583,
		"ドラゴンジュエル"=>584,
		"あくのジュエル"=>585,
		"はがねのジュエル"=>586,
		"ノーマルジュエル"=>587,
		"たいりょくのハネ"=>588,
		"きんりょくのハネ"=>589,
		"ていこうのハネ"=>590,
		"ちりょくのハネ"=>591,
		"せいしんのハネ"=>592,
		"しゅんぱつのハネ"=>593,
		"ハートのハネ"=>594,
		"かめのカセキ"=>595,
		"とりのカセキ"=>596,
		"ライトストーン"=>597,
		"ダークストーン"=>598,
		"わざマシン９３"=>599,
		"わざマシン９４"=>600,
		"わざマシン９５"=>601,
		"わざマシン９６"=>602,
		"わざマシン９７"=>603,
		"わざマシン９８"=>604,
		"わざマシン９９"=>605,
		"わざマシン１００"=>606,
		"わざマシン１０１"=>607,
		"トランシーバー"=>608,
		"ゴッドストーン"=>609,
		"リバティチケット"=>610,
		"アクアカセット"=>611,
		"イナズマカセット"=>612,
		"ブレイズカセット"=>613,
		"フリーズカセット"=>614,
		"パレスボール"=>615,
		"きれいないし"=>616,
		"ポケじゃらし"=>617,
		"グッズケース"=>618,
	}
