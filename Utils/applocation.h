#ifndef APPLOCATION_H
#define APPLOCATION_H

#include "Helpers/constants.h"

namespace Utils {

    class AppLocation
    {
    public:
        AppLocation() = delete;

        static void calculateAppGeometry();


        static int appMinWidth;
        static int appMinHeight;

        static int appMoveOnX;
        static int appMoveOnY;


    private:
        const static int appWidthDefault = Helpers::appWidthDefault;
        const static int appHeightDefault = Helpers::appHeightDefault;
    };

};

#endif // APPLOCATION_H
