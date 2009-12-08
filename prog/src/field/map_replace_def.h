#pragma once

typedef enum {
  MAPREPLACE_ID_SEASON = 0,
  MAPREPLACE_ID_VERSION,
  MAPREPLACE_ID_SEASON_VERSION,
  MAPREPLACE_ID_EVENT01,
  MAPREPLACE_ID_EVENT02,
  MAPREPLACE_ID_EVENT03,
  MAPREPLACE_ID_EVENT04,
  MAPREPLACE_ID_EVENT05,
  MAPREPLACE_ID_EVENT06,
  MAPREPLACE_ID_EVENT07,
  MAPREPLACE_ID_EVENT08,
  MAPREPLACE_ID_EVENT09,

  MAPREPLACE_ID_MAX,
}MAPREPLACE_ID;

enum {
  MAPREPLACE_EVENT_TYPE_NUM = 9,
};

typedef enum {
  MAPREPLACE_LAYER_MAPBLOCK = 0,
  MAPREPLACE_LAYER_MATRIX,
  MAPREPLACE_LAYER_MAX,
}MAPREPLACE_LAYER;

