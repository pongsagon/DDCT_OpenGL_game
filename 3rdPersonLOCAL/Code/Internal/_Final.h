#pragma once

#include "_Def5.h"
#include "_Final/Raycast.h"


class Destroyer : public BanKBehavior {
    public:
        float Lifespan = 5;
        void Update() {
            if (Lifespan <= 0) {
                GameObject->Destroy= true;
            }
            else
            {
                Lifespan -= Time.Deltatime;
            }
        }
}; 
Destroyer* Edit_Destroyer;
