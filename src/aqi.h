#ifndef __AQI_H__
#define __AQI_H__

#ifdef __cplusplus
extern "C" {
#endif

// Useful sites with more information about various aqi scales:
//  https://en.wikipedia.org/wiki/Air_quality_index
//  https://atmotube.com/blog/standards-for-air-quality-indices-in-different-countries-aqi
typedef enum aqi_scale {
  australia_aqi,       // Australia (AQI)
  canada_aqhi,         // Canada (AQHI)
  europe_caqi,         // Europe (CAQI)
  hong_kong_aqhi,      // Hong Kong (AQHI)
  india_aqi,           // India (AQI)
  mainland_china_aqi,  // Mainland China (AQI)
  singapore_psi,       // Singapore (PSI)
  south_korea_cai,     // South Korea (CAI)
  united_kingdom_daqi, // United Kingdom (DAQI)
  united_states_aqi,   // United States (AQI)
} aqi_scale_t;

typedef enum pollutant {
  co,    // μg/m^3, Carbon Monoxide (CO)               1 ppb = 1.1456 μg/m^3
  nh3,   // μg/m^3, Ammonia (NH3)                      1 ppb = 0.6966 μg/m^3
  no,    // μg/m^3, Nitric Oxide (NO)                  1 ppb = 1.2274 μg/m^3
  no2,   // μg/m^3, Nitrogen Dioxide (NO2)             1 ppb = 1.8816 μg/m^3
  o3,    // μg/m^3, Ground-Level Ozone (O3)            1 ppb = 1.9632 μg/m^3
  so2,   // μg/m^3, Sulfur Dioxide (SO2)               1 ppb = 2.6203 μg/m^3
  pm2_5, // μg/m^3, Fine Particulate Matter (<2.5μm)
  pm10,  // μg/m^3, Coarse Particulate Matter (<10μm)
} pollutant_t;
// Note: Concentration (µg/m3) = molecular weight * concentration (ppb) / 24.45

/*
 * Compute Air Quality Index Value
 *
 * Parameters:
 *  The scale determines the standard that will be used to calculate AQI.
 *  Depending on the selected scale different pollutants are required.
 *  This library supports scales that use a subset of the 8 pollutant 
 *  parameters. If a scale does not require a specific pollutant it will be 
 *  ignored. Some scales have pollutants that can be optionally considered,
 *  pass a negative value (ie. -1) for these parameters if you do not want an
 *  optional pollutant to be considered.
 *  All pollutants should be provided as a concentration, in with units μg/m^3.
 *  (see above for conversion factors)
 * 
 * Returns the Air Quality Index value, rounded to the nearest integer
 *   -1 is returned if a parameter is invalid.
 * 
 * Usage Example:
 *   Calculating the Europe Common Air Quality Index (europe_caqi)
 *   CAQI requires no2, o3, pm10, and optionally pm2_5
 *   let no2 = 0.7711, o3 = 68.66, pm10 = 0.5404
 *   In this example, we will elect to not consider the optional pm2_5, 
 *   signified with -1.
 *                     scale co nh3 no     no2     o3 so2 pm2_5    pm10
 *   compute_aqi(europe_caqi, 0,  0, 0, 0.7711, 68.66,  0    -1, 0.5404)
 */
int compute_aqi(aqi_scale_t scale, 
                float co, float nh3, float no, float no2, 
                float o3, float so2, float pm2_5, float pm10);

/*
 * Returns the descriptor/category of an aqi value.
 * 
 * Usage Example:
 *   aqi_descriptor(united_states_aqi, 52);
 *   returns "Moderate"
 */
char* aqi_descriptor(aqi_scale_t scale, int aqi);

#ifdef __cplusplus
}
#endif

#endif