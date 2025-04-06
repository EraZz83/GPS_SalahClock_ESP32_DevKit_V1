double pi = 3.141592653564; //4622948954930381964428810975665933446128475648233786783165;

double DEG_TO_RAD(double Degree){
    return ((Degree * pi)/180);
}

double RAD_TO_DEG(double Radiant){
    return  (Radiant * 180.0) / pi;
}

long long TimeOfDay_To_Milliseconds(int time) {
    // Extract hours, minutes, and seconds from the time parameter
    int hours = time / 10000;           // First 2 digits (17)
    int minutes = (time / 100) % 100;   // Middle 2 digits (21)
    int seconds = time % 100;           // Last 2 digits (58)

    // Convert to total seconds
    long long total_seconds = (hours * 3600) + (minutes * 60) + seconds;

    // Convert total seconds to milliseconds
    return total_seconds * 1000;
}