/*---------------------------------------------------------------------------*
  Project:  simple particle library
  File:     spl_random.h

  

  $Id: spl_random.h,v 1.1 2004/04/08 00:23:26 konoh Exp $
  $Log: spl_random.h,v $
  Revision 1.1  2004/04/08 00:23:26  konoh
  (none)

 *---------------------------------------------------------------------------*/

#ifndef __SPL_RANDOM_H__
#define __SPL_RANDOM_H__

/* Ά¬ ---------------------------------------------------------------- */

extern u32 spl_rndm_seed;
#define SPL_SRND(x) ((spl_rndm_seed)=(x))
#define SPL_RNDI()  ((spl_rndm_seed)=(spl_rndm_seed*1592653589UL)+453816691UL)
#define SPL_RNDF()  (SPL_RNDI() >> 20) // ¬PQrbgͺΕOΘγP’H

/* ------------------------------------------------------------------------- */


/* CΣxNgΜζΎ ------------------------------------------------------ */

extern void spl_rndm_get_arb_vec_xyz(VecFx32* p_vec);
extern void spl_rndm_get_arb_vec_xy(VecFx32* p_vec);

/* ------------------------------------------------------------------------- */


/* eνΜΝΝirangeΝWrbgΕO`PΜΝΝΜΖ·ιj---------------- */

// [num * (1 - range), num)
#define SPL_RNDM_GET_01(num, range)\
        (((num) * (0xff - (((range) * (int) (SPL_RNDI() >> 24)) >> 8))) >> 8)

// [num * (1 - range), num * 2 * (1 - range))
#define SPL_RNDM_GET_02(num, range)\
        ((num) * (0xff + (range) - (((range) * (int) (SPL_RNDI() >> 24)) >> 7)) >> 8)

// [-num, num)
#define SPL_RNDM_GET_M11(num)\
        (((num) * ((int) (SPL_RNDI() >> 23)) - ((num) << 8)) >> 8)

/* ------------------------------------------------------------------------- */

#endif
