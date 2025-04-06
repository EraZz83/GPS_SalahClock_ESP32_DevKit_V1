#include <cmath>
#include "DayOfYear.h"
#include "Converters.h"
#include <iostream>
#include <time.h>
#include <math.h>

struct dateTime
{
    bool summerTimeCorrection;
    int day, month ,year, hour, minute, second, timeShift;
    double milis;
};

dateTime actDateTime;

double Breitengrad = 47.52777, Laengengrad = 9.37074;

double getDeclinationAngle (int day, int month, int year){
    double r = (23.45 * sin(DEG_TO_RAD((360.0/365.0) * (getDayOfYear(day, month, year) - 81.0))));

    //std::cout << "Declinationangle: " << r << std::endl;
    return r;
}

double getLSTM(int TimeShiftInMin,bool SummertimeCorrection){
    //Meridian der lokalen Standardzeit - LocalStandardTimeMeridian [Grad]
    
        double r = TimeShiftInMin; //Zeitverschiebung (Zeitzone)
    r += (60 * float(SummertimeCorrection)); //Sommer- /Winterzeit - 0 = Winterzeit, 1 = Sommerzeit
    r = (15 * (r/60.0));

    //std::cout << "LSTM: " << r << std::endl;
    return r;
}

double getEoT(int day, int month, int year){
    double B = (DEG_TO_RAD(360.0 / 365.0) * (double(getDayOfYear(day,month,year)) - 81.0));
    double r = (9.87 * sin(2*B) - 7.53 * cos(B) - 1.5 * sin(B));

    //std::cout << "EoT: " << r << std::endl;
    return r;
}

double getTimeCorrectionInMs(dateTime actDT, double Longitude){
    double LSTM = getLSTM(actDT.timeShift, actDT.summerTimeCorrection);
    double EoT = getEoT(actDT.day, actDT.month, actDT.year);

    double r = (4.0*(Longitude - LSTM) + EoT);
    r *= 60000;
    
    //std::cout << "TC: " << r << std::endl;
    return r;
}

double getSolarTimeInMs(dateTime actDT, double Longitude){
    double TimeCorrection = getTimeCorrectionInMs(actDT, Longitude);

    double r = actDT.hour * 3600 * 1000;
    r += actDT.minute * 60 * 1000;
    r += actDT.second * 1000;

    r += TimeCorrection;

    //std::cout << "SolarTimeMs: " << r << std::endl;
    return r;
}

double getSolarTimeInHours(dateTime actDT, double Longitude){
    double solarTimeInMs = getSolarTimeInMs(actDT, Longitude);
    double r = solarTimeInMs / 3600000.0;

    //std::cout << "SolarTimeHours: " << r << std::endl;
    return r;
}

double getHRA(dateTime actDT, double Longitude){
    double solarTimeInHours = getSolarTimeInHours(actDT, Longitude);

    double r = 15.0*(solarTimeInHours - 12.0);

    //std::cout << "HourAngle: " << r << std::endl;
    return r;
}

double getElevation(dateTime actDT, double Latitute, double Longitude){
    double Declination = getDeclinationAngle(actDT.day, actDT.month, actDT.year);
    double HRA = getHRA(actDT,Longitude);
    double correction = 0.6;

    double r = (RAD_TO_DEG(asin((sin(DEG_TO_RAD(Latitute)) * sin(DEG_TO_RAD(Declination))) + (cos(DEG_TO_RAD(Latitute)) * cos(DEG_TO_RAD(Declination)) * cos(DEG_TO_RAD(HRA))))));

    r += correction;
    
    //std::cout << "Elevation: " << r << std::endl;
    return r;
}

double getAzimuth(dateTime actDT, double Latitute, double Longitude){
    double Declination = getDeclinationAngle(actDT.day, actDT.month, actDT.year);
    double Elevation = getElevation(actDT, Latitute, Longitude);
    double correction = -1.4;

    double r = (sin(DEG_TO_RAD(Elevation)) * sin(DEG_TO_RAD(Latitute)) - sin(DEG_TO_RAD(Declination)));
    r = (r / (cos(DEG_TO_RAD(Elevation)) * cos(DEG_TO_RAD(Latitute))));
    
    if ((abs(r) >= 1.0))
    {
        r = 2 - abs(r);
    }      
            
    r = acos(r);
    
    if (getSolarTimeInHours(actDT,Longitude) <= 12.0)
    {
        r = 180.0 - RAD_TO_DEG(r);
    }
    else
    {
        r = 180.0 + RAD_TO_DEG(r);
    }
    

    r += correction;
    //std::cout << "Azimuth: " << r << std::endl;
    return r;
}

dateTime getTimeFromAngle(dateTime actDT ,double Latitute, double Longitude, double Elevation, bool beforeDhuhr){
    
double Declination = getDeclinationAngle(actDT.day, actDT.month, actDT.year);
dateTime dt;
dt = actDT;

    if (beforeDhuhr){
        dt.hour = 0;
        dt.minute = 0;
        dt.second = 0;
    } else {
        dt.hour = 23;
        dt.minute = 59;
        dt.second = 59;
    }
        
    while (true)
    {        
        if (dt.second >= 60){
            dt.second = 0;
            dt.minute += 1;
        }
        else if (dt.second < 0){
            dt.second += 60;
            dt.minute -= 1;
        }          
        if (dt.minute >= 60){
            dt.minute = 0;
            dt.hour += 1;
        }
        else if (dt.minute < 0){
            dt.minute += 60;
            dt.hour -= 1;
        }
        
        if (dt.hour >= 24){
            dt.hour = dt.hour % 24;        
        }
        else if (dt.hour < 0)
        {
            dt.hour = 0;
        }

        if (getElevation(dt, Latitute, Longitude) >= Elevation){
            return dt;    
        }
        
        if (beforeDhuhr){
            dt.minute += 1;
        } else {
            dt.minute -= 1;
        }
            
    };
}

dateTime getTimeFromHRA(dateTime actDT, double HRA, double Longitude){

    dateTime dt;
    dt = actDT;
    dt.hour = 0;
    dt.minute = 0;
    dt.second = 0;

    while (true)
    {   
        if (dt.second >= 60){
            dt.second = 0;
            dt.minute += 1;
        }
        else if (dt.second < 0){
            dt.second += 60;
            dt.minute -= 1;
            dt.hour -= 1;
        }  
        if (dt.minute >= 60){
            dt.minute = 0;
            dt.hour += 1;
        }
        else if (dt.minute < 0){
            dt.minute += 60;
            dt.hour -= 1;
        }
        
        if (dt.hour >= 24){
            dt.hour = dt.hour % 24;        
        }
        else if (dt.hour < 0)
        {
            dt.hour = 0;
        }

        if (getHRA(dt, Longitude) >= HRA){
            return dt;    
        }
        
        dt.minute += 1;    
    };
}

dateTime calculateTimeOffset(dateTime dt, dateTime offset){
    dateTime r = dt;
    dt.second += offset.second;
    dt.minute += offset.minute;
    dt.hour += offset.hour;
    

    if (dt.second >= 60){
        dt.second = 0;
        dt.minute += 1;
    }
    else if (dt.second < 0){
        dt.second += 60;
        dt.minute -= 1;
        dt.hour -= 1;
    }  
    if (dt.minute >= 60){
        dt.minute = 0;
        dt.hour += 1;
    }
    else if (dt.minute < 0){
        dt.minute += 60;
        dt.hour -= 1;
    }
    
    if (dt.hour >= 24){
        dt.hour = dt.hour % 24;        
    }
    else if (dt.hour < 0)
    {
        dt.hour = 0;
    }

    return dt;
}

dateTime countdown(bool positive){
    dateTime dt;

    if (positive){
        dt.hour = 0;
        dt.minute = 0;
        dt.second = 0;
    } else {
        dt.hour = 23;
        dt.minute = 59;
        dt.second = 59;
    }
        
    while (true)
    {        
        if (dt.second >= 60){
            dt.second = 0;
            dt.minute += 1;
        }
        else if (dt.second < 0){
            dt.second += 60;
            dt.minute -= 1;
        }          
        if (dt.minute >= 60){
            dt.minute = 0;
            dt.hour += 1;
        }
        else if (dt.minute < 0){
            dt.minute += 60;
            dt.hour -= 1;
        }
        
        if (dt.hour >= 24){
            dt.hour = dt.hour % 24;        
        }
        else if (dt.hour < 0)
        {
            dt.hour = 0;
        }

        std::cout << dt.hour << ":" << dt.minute << std::endl;

        if (positive){
            dt.minute += 1;
        } else {
            dt.minute -= 1;
        }
            
    };
}