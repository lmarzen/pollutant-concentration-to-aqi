#ifndef __AQI_H__
#define __AQI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Each AQI scale has a maximum value, above which there is no defined piecewise
 * function. The way these indexes are commonly denoted is by ">{AQI_MAX}" or 
 * "{AQI_MAX}+". Note: Minimum AQI for all scales is 0.
 * 
 * Uncomment if you want to make use of these values
 */
// #define AUSTRALIA_AQI_MAX       200
// #define CANADA_AQHI_MAX         10
// #define EUROPE_CAQI_MAX         100
// #define HONG_KONG_AQHI_MAX      10
// #define INDIA_AQI_MAX           400
// #define MAINLAND_CHINA_AQI_MAX  300
// #define SINGAPORE_PSI_MAX       400
// #define SOUTH_KOREA_CAI_MAX     500
// #define UNITED_KINGDOM_DAQI_MAX 10
// #define UNITED_STATES_AQI_MAX   500

/* Returns the Air Quality Index, rounded to the nearest integer
 *   -1 is returned if a parameter is invalid.
 * 
 * All pollutants will be interpreted as a concentration, with units μg/m^3.
 * Pollutants:
 *   co    μg/m^3, Carbon Monoxide (CO)               1 ppb = 1.1456 μg/m^3
 *   nh3   μg/m^3, Ammonia (NH3)                      1 ppb = 0.6966 μg/m^3
 *   no    μg/m^3, Nitric Oxide (NO)                  1 ppb = 1.2274 μg/m^3
 *   no2   μg/m^3, Nitrogen Dioxide (NO2)             1 ppb = 1.8816 μg/m^3
 *   o3    μg/m^3, Ground-Level Ozone (O3)            1 ppb = 1.9632 μg/m^3
 *   pb    μg/m^3, Lead (Pb)                          1 ppb = 1.9632 μg/m^3
 *   so2   μg/m^3, Sulfur Dioxide (SO2)               1 ppb = 8.4744 μg/m^3
 *   pm2_5 μg/m^3, Fine Particulate Matter (<2.5μm)
 *   pm10  μg/m^3, Coarse Particulate Matter (<10μm)
 * Note: Concentration (µg/m^3) = molecular weight * concentration (ppb) / 24.45
 * 
 * Useful websites with more information about various aqi scales:
 * https://en.wikipedia.org/wiki/Air_quality_index
 * https://atmotube.com/blog/standards-for-air-quality-indices-in-different-countries-aqi
 */
int australia_aqi();       // Australia (AQI)
int canada_aqhi();         // Canada (AQHI)
int europe_caqi(float no2, float o3, float pm2_5, float pm10);         // Europe (CAQI)
int hong_kong_aqhi();      // Hong Kong (AQHI)
int india_aqi();           // India (AQI)
int mainland_china_aqi();  // Mainland China (AQI)
int singapore_psi();       // Singapore (PSI)
int south_korea_cai();     // South Korea (CAI)
int united_kingdom_daqi(); // United Kingdom (DAQI)
int united_states_aqi();   // United States (AQI)

/* Returns the descriptor/category of an aqi value.
 * 
 * Usage Example:
 *   united_states_aqi_desc(52);
 *   returns "Moderate"
 */
char* australia_aqi_desc(int api);
char* canada_aqhi_desc(int api);
char* europe_caqi_desc(int api);
char* hong_kong_aqhi_desc(int api);
char* india_aqi_desc(int api);
char* mainland_china_aqi_desc(int api);
char* singapore_psi_desc(int api);
char* south_korea_cai_desc(int api);
char* united_kingdom_daqi_desc(int api);
char* united_states_aqi_desc(int api);

#ifdef __cplusplus
}
#endif

#endif