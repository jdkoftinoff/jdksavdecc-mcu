/*
  Copyright (c) 2015, J.D. Koftinoff Software, Ltd.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   3. Neither the name of J.D. Koftinoff Software, Ltd. nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#include "JDKSAvdeccMCU/World.hpp"
#include "JDKSAvdeccMCU/RangedValue.hpp"

namespace JDKSAvdeccMCU
{
float powers_of_ten[25] = {1e-12, 1e-11, 1e-10, 1e-9, 1e-8, 1e-7, 1e-6, 1e-5, 1e-4, 1e-3, 1e-2, 1e-1, 1e0,
                           1e1,   1e2,   1e3,   1e4,  1e5,  1e6,  1e7,  1e8,  1e9,  1e10, 1e11, 1e12};


const char *getAvdeccUnitsSuffix(uint8_t units_code)
{
    const char *s="";
    switch ( units_code )
    {
    case JDKSAVDECC_AEM_UNIT_UNITLESS:
        s = "";
        break;
    case JDKSAVDECC_AEM_UNIT_COUNT:
        s = "";
        break;
    case JDKSAVDECC_AEM_UNIT_PERCENT:
        s = "%";
        break;
    case JDKSAVDECC_AEM_UNIT_FSTOP:
        s = "Fstop";
        break;
    case JDKSAVDECC_AEM_UNIT_TIME_SECONDS:
        s = "sec";
        break;
    case JDKSAVDECC_AEM_UNIT_TIME_MINUTES:
        s = "min";
        break;
    case JDKSAVDECC_AEM_UNIT_TIME_HOURS:
        s = "hours";
        break;
    case JDKSAVDECC_AEM_UNIT_TIME_DAYS:
        s = "days";
        break;
    case JDKSAVDECC_AEM_UNIT_TIME_MONTHS:
        s = "months";
        break;
    case JDKSAVDECC_AEM_UNIT_TIME_YEARS:
        s = "years";
        break;
    case JDKSAVDECC_AEM_UNIT_TIME_SAMPLES:
        s = "samples";
        break;
    case JDKSAVDECC_AEM_UNIT_TIME_FRAMES:
        s = "frames";
        break;
    case JDKSAVDECC_AEM_UNIT_FREQUENCY_HERTZ:
        s = "Hz";
        break;
    case JDKSAVDECC_AEM_UNIT_FREQUENCY_SEMITONES:
        s = "Semitones";
        break;
    case JDKSAVDECC_AEM_UNIT_FREQUENCY_CENTS:
        s = "Cents";
        break;
    case JDKSAVDECC_AEM_UNIT_FREQUENCY_OCTAVES:
        s = "Octaves";
        break;
    case JDKSAVDECC_AEM_UNIT_FREQUENCY_FPS:
        s = "frames/sec";
        break;
    case JDKSAVDECC_AEM_UNIT_DISTANCE_METRES:
        s = "metres";
        break;
    case JDKSAVDECC_AEM_UNIT_TEMPERATURE_KELVIN:
        s = "ºK";
        break;
    case JDKSAVDECC_AEM_UNIT_MASS_GRAMS:
        s = "g";
        break;
    case JDKSAVDECC_AEM_UNIT_VOLTAGE_VOLTS:
        s = "V";
        break;
    case JDKSAVDECC_AEM_UNIT_VOLTAGE_DBV:
        s = "dBV";
        break;
    case JDKSAVDECC_AEM_UNIT_VOLTAGE_DBU:
        s = "dbU";
        break;
    case JDKSAVDECC_AEM_UNIT_CURRENT_AMPS:
        s = "A";
        break;
    case JDKSAVDECC_AEM_UNIT_POWER_WATTS:
        s = "W";
        break;
    case JDKSAVDECC_AEM_UNIT_POWER_DBM:
        s = "dBM";
        break;
    case JDKSAVDECC_AEM_UNIT_POWER_DBW:
        s = "dBW";
        break;
    case JDKSAVDECC_AEM_UNIT_PRESSURE_PASCALS:
        s = "Pa";
        break;
    case JDKSAVDECC_AEM_UNIT_MEMORY_BITS:
        s = "bits";
        break;
    case JDKSAVDECC_AEM_UNIT_MEMORY_BYTES:
        s = "Bytes";
        break;
    case JDKSAVDECC_AEM_UNIT_MEMORY_KIBIBYTES:
        s = "KiB";
        break;
    case JDKSAVDECC_AEM_UNIT_MEMORY_MEBIBYTES:
        s = "MiB";
        break;
    case JDKSAVDECC_AEM_UNIT_MEMORY_GIBIBYTES:
        s = "GiB";
        break;
    case JDKSAVDECC_AEM_UNIT_MEMORY_TEBIBYTES:
        s = "TiB";
        break;
    case JDKSAVDECC_AEM_UNIT_MEMORY_BANDWIDTH_BITS_PER_SEC:
        s = "bps";
        break;
    case JDKSAVDECC_AEM_UNIT_MEMORY_BANDWIDTH_BYTES_PER_SEC:
        s = "Bps";
        break;
    case JDKSAVDECC_AEM_UNIT_MEMORY_BANDWIDTH_KIBIBYTES_PER_SEC:
        s = "KiB/s";
        break;
    case JDKSAVDECC_AEM_UNIT_MEMORY_BANDWIDTH_MEBIBYTES_PER_SEC:
        s = "MiB/s";
        break;
    case JDKSAVDECC_AEM_UNIT_MEMORY_BANDWIDTH_GIGIBYTES_PER_SEC:
        s = "GiB/s";
        break;
    case JDKSAVDECC_AEM_UNIT_MEMORY_BANDWIDTH_TEBIBYTES_PER_SEC:
        s = "TiB/s";
        break;
    case JDKSAVDECC_AEM_UNIT_LUMINOSITY_CANDELAS:
        s = "cd";
        break;
    case JDKSAVDECC_AEM_UNIT_ENERGY_JOULES:
        s = "J";
        break;
    case JDKSAVDECC_AEM_UNIT_ANGLE_RADIANS:
        s = "Rads";
        break;
    case JDKSAVDECC_AEM_UNIT_FORCE_NEWTONS:
        s = "Newtons";
        break;
    case JDKSAVDECC_AEM_UNIT_RESISTANCE_OHMS:
        s = "Ω";
        break;
    case JDKSAVDECC_AEM_UNIT_VELOCITY_METRES_PER_SEC:
        s = "m/s";
        break;
    case JDKSAVDECC_AEM_UNIT_VELOCITY_RADIANS_PER_SEC:
        s = "rad/s";
        break;
    case JDKSAVDECC_AEM_UNIT_ACCELERATION_METRES_PER_SEC_SQUARED:
        s = "m/s²";
        break;
    case JDKSAVDECC_AEM_UNIT_ACCELERATION_RADIANS_PER_SEC_SQUARED:
        s = "rad/s²";
        break;
    case JDKSAVDECC_AEM_UNIT_MAGNETIC_FLUX_TESLAS:
        s = "T";
        break;
    case JDKSAVDECC_AEM_UNIT_AREA_METERS_SQUARED:
        s = "m²";
        break;
    case JDKSAVDECC_AEM_UNIT_VOLUME_METERS_CUBED:
        s = "m³";
        break;
    case JDKSAVDECC_AEM_UNIT_VOLUME_LITRES:
        s = "L";
        break;
    case JDKSAVDECC_AEM_UNIT_LEVEL_DB:
        s = "dB";
        break;
    case JDKSAVDECC_AEM_UNIT_LEVEL_DB_PEAK:
        s = "dB (Peak)";
        break;
    case JDKSAVDECC_AEM_UNIT_LEVEL_DB_RMS:
        s = "dB (RMS)";
        break;
    case JDKSAVDECC_AEM_UNIT_LEVEL_DBFS:
        s = "dBFS";
        break;
    case JDKSAVDECC_AEM_UNIT_LEVEL_DBFS_PEAK:
        s = "dBFS (Peak)";
        break;
    case JDKSAVDECC_AEM_UNIT_LEVEL_DBFS_RMS:
        s = "dBFS (RMS)";
        break;
    case JDKSAVDECC_AEM_UNIT_LEVEL_DBTP:
        s = "dBTP";
        break;
    case JDKSAVDECC_AEM_UNIT_LEVEL_DB_A:
        s = "dB (A)";
        break;
    case JDKSAVDECC_AEM_UNIT_LEVEL_DB_B:
        s = "dB (B)";
        break;
    case JDKSAVDECC_AEM_UNIT_LEVEL_DB_C:
        s = "dB (C)";
        break;
    case JDKSAVDECC_AEM_UNIT_LEVEL_DB_SPL:
        s = "dB (SPL)";
        break;
    case JDKSAVDECC_AEM_UNIT_LEVEL_LU:
        s = "LU";
        break;
    case JDKSAVDECC_AEM_UNIT_LEVEL_LUFS:
        s = "LUFS";
        break;
    default:
        s = "unknown";
    }
    return s;
}


}
