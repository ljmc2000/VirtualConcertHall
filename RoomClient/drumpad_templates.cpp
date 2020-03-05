#include "druminstrumentview.h"

static QList<PadTemplate> AmmoonDrumPad
({
     PadTemplate(36,
         3.0f/8.0f,
         3.0f/8.0f,
         1.0f/4.0f,
         1.0f/4.0f
     ),

     PadTemplate(38,
         0,
         4.0f/5.0f,
         1.0f/4.0f,
         1.0f/4.0f
     ),

     PadTemplate(42,
         1.0f/8.0f,
         1.0f/2.0f,
         3.0f/16.0f,
         3.0f/16.0f
     ),

     PadTemplate(44,
         7.0f/16.0f,
         11.0f/16.0f,
         1.0f/8.0f,
         1.0f/8.0f
     ),

     PadTemplate(45,
         1-1/4.0f,
         4.0f/5.0f,
         1.0f/4.0f,
         1.0f/4.0f
     ),

     PadTemplate(48,
         9.0f/16.0f,
         4.0f/5.0f,
         3.0f/16.0f,
         3.0f/16.0f
     ),

     PadTemplate(49,
         1.0f/8.0f,
         1.0f/4.0f,
         3.0f/16.0f,
         3.0f/16.0f
     ),

     PadTemplate(50,
         1.0f/4.0f,
         4.0f/5.0f,
         3.0f/16.0f,
         3.0f/16.0f
     ),

     PadTemplate(51,
         11.0f/16.0f,
         1.0f/2.0f,
         3.0f/16.0f,
         3.0f/16.0f
     ),

     PadTemplate(57,
         11.0f/16.0f,
         1.0f/4.0f,
         3.0f/16.0f,
         3.0f/16.0f
     ),
});

QHash<DrumLayout,QList<PadTemplate>> DrumInstrumentView::presets({
    {AMMOON,AmmoonDrumPad},
});
