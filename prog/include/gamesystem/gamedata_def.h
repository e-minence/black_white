//============================================================================================
/**
 * @file	gamedata_def.h
 * @brief	ゲーム進行データ管理
 * @date	2008.11.04
 *
 * @note
 * シャチプロジェクトにおいては、ゲーム進行データへのアクセスは
 * ゲームデータ型GAMEDATA経由とする。
 * 実際にセーブされるデータであるかどうかはGAMEDATAの内部実装による。
 *
 * 基本的にゲーム進行データはセーブデータとして存在するが、
 * 通信時の複数プレイヤー情報やフィールドマップ情報などセーブデータに
 * のるとは限らない情報もある。
 * そのようなセーブデータ・非セーブデータへ並列にアクセスするインターフェイスを
 * 各パートごとに構成するとパートごとにアクセス手法が違ったり、また同じ機能の
 * ものが複数存在するなどプログラム全体の複雑さが増してしまう可能性が高い。
 * それを避けるため、共通インターフェイスを作ることによって簡略化する試み
 */
//============================================================================================
#pragma once

//------------------------------------------------------------------
/**
 * @brief	GAMEDATA型定義
 *
 * _GAMEDATAの実体はgame_data.c内に定義されている
 */
//------------------------------------------------------------------
typedef struct _GAMEDATA GAMEDATA;

