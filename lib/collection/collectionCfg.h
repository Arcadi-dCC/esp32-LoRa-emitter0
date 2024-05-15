#ifndef collectionCfg_H
#define collectionCfg_H

#include <platformTypes.h>

#define COLLECTION_DAYS     "MTWRF"     //Place the collection days: sun(D)ay, (M)onday, (T)uesday, (W)ednesday, thu(R)sday, (F)riday, (S)aturday

#define TOTAL_BINS      7U
extern uint16 bins[TOTAL_BINS][6U];

#define BIN_HEIGHT    150U        //Height of garbage container [cm]

#endif //collectionCfg_H
