
#include "JDKSAvdeccMCU/World.hpp"
#include "JDKSAvdeccMCU/RangedValue.hpp"

namespace JDKSAvdeccMCU
{
float powers_of_ten[25] = {1e-12, 1e-11, 1e-10, 1e-9, 1e-8, 1e-7, 1e-6, 1e-5, 1e-4, 1e-3, 1e-2, 1e-1, 1e0,
                           1e1,   1e2,   1e3,   1e4,  1e5,  1e6,  1e7,  1e8,  1e9,  1e10, 1e11, 1e12};


const char *getAvdeccUnitsSuffix(UnitsCode units_code)
{
    const char *s="";
    switch ( units_code)
    {
    case UnitsCode::UNITLESS:
        s = "";
        break;
    case UnitsCode::COUNT:
        s = "";
        break;
    case UnitsCode::PERCENT:
        s = "%";
        break;
    case UnitsCode::FSTOP:
        s = "Fstop";
        break;
    case UnitsCode::TIME_SECONDS:
        s = "sec";
        break;
    case UnitsCode::TIME_MINUTES:
        s = "min";
        break;
    case UnitsCode::TIME_HOURS:
        s = "hours";
        break;
    case UnitsCode::TIME_DAYS:
        s = "days";
        break;
    case UnitsCode::TIME_MONTHS:
        s = "months";
        break;
    case UnitsCode::TIME_YEARS:
        s = "years";
        break;
    case UnitsCode::TIME_SAMPLES:
        s = "samples";
        break;
    case UnitsCode::TIME_FRAMES:
        s = "frames";
        break;
    case UnitsCode::FREQUENCY_HERTZ:
        s = "Hz";
        break;
    case UnitsCode::FREQUENCY_SEMITONES:
        s = "Semitones";
        break;
    case UnitsCode::FREQUENCY_CENTS:
        s = "Cents";
        break;
    case UnitsCode::FREQUENCY_OCTAVES:
        s = "Octaves";
        break;
    case UnitsCode::FREQUENCY_FPS:
        s = "frames/sec";
        break;
    case UnitsCode::DISTANCE_METRES:
        s = "metres";
        break;
    case UnitsCode::TEMPERATURE_KELVIN:
        s = "ºK";
        break;
    case UnitsCode::MASS_GRAMS:
        s = "g";
        break;
    case UnitsCode::VOLTAGE_VOLTS:
        s = "V";
        break;
    case UnitsCode::VOLTAGE_DBV:
        s = "dBV";
        break;
    case UnitsCode::VOLTAGE_DBU:
        s = "dbU";
        break;
    case UnitsCode::CURRENT_AMPS:
        s = "A";
        break;
    case UnitsCode::POWER_WATTS:
        s = "W";
        break;
    case UnitsCode::POWER_DBM:
        s = "dBM";
        break;
    case UnitsCode::POWER_DBW:
        s = "dBW";
        break;
    case UnitsCode::PRESSURE_PASCALS:
        s = "Pa";
        break;
    case UnitsCode::MEMORY_BITS:
        s = "bits";
        break;
    case UnitsCode::MEMORY_BYTES:
        s = "Bytes";
        break;
    case UnitsCode::MEMORY_KIBIBYTES:
        s = "KiB";
        break;
    case UnitsCode::MEMORY_MEBIBYTES:
        s = "MiB";
        break;
    case UnitsCode::MEMORY_GIBIBYTES:
        s = "GiB";
        break;
    case UnitsCode::MEMORY_TEBIBYTES:
        s = "TiB";
        break;
    case UnitsCode::MEMORY_BANDWIDTH_BITS_PER_SEC:
        s = "bps";
        break;
    case UnitsCode::MEMORY_BANDWIDTH_BYTES_PER_SEC:
        s = "Bps";
        break;
    case UnitsCode::MEMORY_BANDWIDTH_KIBIBYTES_PER_SEC:
        s = "KiB/s";
        break;
    case UnitsCode::MEMORY_BANDWIDTH_MEBIBYTES_PER_SEC:
        s = "MiB/s";
        break;
    case UnitsCode::MEMORY_BANDWIDTH_GIGIBYTES_PER_SEC:
        s = "GiB/s";
        break;
    case UnitsCode::MEMORY_BANDWIDTH_TEBIBYTES_PER_SEC:
        s = "TiB/s";
        break;
    case UnitsCode::LUMINOSITY_CANDELAS:
        s = "cd";
        break;
    case UnitsCode::ENERGY_JOULES:
        s = "J";
        break;
    case UnitsCode::ANGLE_RADIANS:
        s = "Rads";
        break;
    case UnitsCode::FORCE_NEWTONS:
        s = "Newtons";
        break;
    case UnitsCode::RESISTANCE_OHMS:
        s = "Ω";
        break;
    case UnitsCode::VELOCITY_METRES_PER_SEC:
        s = "m/s";
        break;
    case UnitsCode::VELOCITY_RADIANS_PER_SEC:
        s = "rad/s";
        break;
    case UnitsCode::ACCELERATION_METRES_PER_SEC_SQUARED:
        s = "m/s²";
        break;
    case UnitsCode::ACCELERATION_RADIANS_PER_SEC_SQUARED:
        s = "rad/s²";
        break;
    case UnitsCode::MAGNETIC_FLUX_TESLAS:
        s = "T";
        break;
    case UnitsCode::AREA_METERS_SQUARED:
        s = "m²";
        break;
    case UnitsCode::VOLUME_METERS_CUBED:
        s = "m³";
        break;
    case UnitsCode::VOLUME_LITRES:
        s = "L";
        break;
    case UnitsCode::LEVEL_DB:
        s = "dB";
        break;
    case UnitsCode::LEVEL_DB_PEAK:
        s = "dB (Peak)";
        break;
    case UnitsCode::LEVEL_DB_RMS:
        s = "dB (RMS)";
        break;
    case UnitsCode::LEVEL_DBFS:
        s = "dBFS";
        break;
    case UnitsCode::LEVEL_DBFS_PEAK:
        s = "dBFS (Peak)";
        break;
    case UnitsCode::LEVEL_DBFS_RMS:
        s = "dBFS (RMS)";
        break;
    case UnitsCode::LEVEL_DBTP:
        s = "dBTP";
        break;
    case UnitsCode::LEVEL_DB_A:
        s = "dB (A)";
        break;
    case UnitsCode::LEVEL_DB_B:
        s = "dB (B)";
        break;
    case UnitsCode::LEVEL_DB_C:
        s = "dB (C)";
        break;
    case UnitsCode::LEVEL_DB_SPL:
        s = "dB (SPL)";
        break;
    case UnitsCode::LEVEL_LU:
        s = "LU";
        break;
    case UnitsCode::LEVEL_LUFS:
        s = "LUFS";
        break;
    default:
        s = "unknown";
    }
    return s;
}

}
