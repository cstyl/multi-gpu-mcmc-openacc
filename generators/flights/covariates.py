def calculate_late(ArrDelay):
    return 1.0 if ArrDelay>15 else -1.0

def calculate_night(CRSDepTime):
    return 1.0 if (CRSDepTime >= 2000 or CRSDepTime <= 500) else 0.0

def calculate_weekend(DayOfWeek):
    return 1.0 if DayOfWeek > 5 else 0.0

# Time given in HHMM format. Converted in minutes and then to hours (0 to 24)
def calculate_dephour(DepTime):
    return (int(DepTime / 100) * 60 + DepTime % 100) / 60

# Convert in 1000miles
def calculate_distance(Distance):
    return Distance / 1000
