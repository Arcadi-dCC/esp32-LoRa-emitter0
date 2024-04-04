#ifndef collectionCfg_H
#define collectionCfg_H

#include <platformTypes.h>

#define COLLECTION_START    "20:30:00"  //Start time of garbage collection (hh:mm:ss)
#define COLLECTION_END      "23:15:00"  //End time of garbage collection (hh:mm:ss)

#define COLLECTION_DAYS     "MTWRF"     //Place the collection days: (M)onday, (T)uesday, (W)ednesday, thu(R)sday, (F)riday, (S)aturday, sun(D)ay

#define TOTAL_BINS 7U

extern uint16 bins[TOTAL_BINS][2U];

#endif //collectionCfg_H