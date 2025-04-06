bool isLeapYear(int year) {
    // A leap year is divisible by 4, but not divisible by 100, unless it's divisible by 400
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        return true;
    return false;
}

int getDayOfYear(int day, int month, int year) {
    // Array of days in each month for a non-leap year
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // If it's a leap year, February has 29 days
    if (isLeapYear(year)) {
        daysInMonth[1] = 29;  // February
    }

    // Calculate the day of the year
    int dayOfYear = 0;

    // Add the days from previous months
    for (int i = 0; i < month - 1; i++) {
        dayOfYear += daysInMonth[i];
    }

    // Add the days of the current month
    dayOfYear += day;

    return dayOfYear;
}