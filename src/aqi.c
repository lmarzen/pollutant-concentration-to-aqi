#include <math.h>
#include "aqi.h"

int max(int a, int b) {
  return a >= b ? a : b;
}

int compute_nepm_aqi(float std, float c) {
  return (int) round(c / std * 100);
}

int compute_piecewise_aqi(int i_low, int i_high,
                int c_low, int c_high, float c) {
  return (int) round(
    (((i_high - i_low)) / ((float)(c_high - c_low))) * (c - c_low) + i_low
    );  
}

/* Australia (AQI)
 *
 * References:
 *   https://www.environment.nsw.gov.au/topics/air/understanding-air-quality-data/air-quality-categories/history-of-air-quality-reporting/about-the-air-quality-index
 */
int australia_aqi(float co,  float no2,  float o3_1hr, float o3_4hr, 
                  float so2, float pm10, float pm2_5) {
  int aqi = 0;

  // co    μg/m^3, Carbon Monoxide (CO)
  // standard = 9.0ppm * 1000ppb * 1.1456 μg/m^3 = 10310.4
  aqi = max(aqi, compute_nepm_aqi(10310.4, co));
  // no2   μg/m^3, Nitrogen Dioxide (NO2)
  // standard = 0.12ppm * 1000ppb * 1.8816 μg/m^3 = 10310.4
  aqi = max(aqi, compute_nepm_aqi(225.792, no2));
  // o3    μg/m^3, Ground-Level Ozone (O3)
  // standard = 0.10ppm * 1000ppb * 1.9632 μg/m^3 = 196.32
  aqi = max(aqi, compute_nepm_aqi(196.32, o3_1hr));
  // standard = 0.08ppm * 1000ppb * 1.9632 μg/m^3 = 157.056
  aqi = max(aqi, compute_nepm_aqi(157.056, o3_4hr));
  // so2   μg/m^3, Sulfur Dioxide (SO2)
  // standard = 0.20ppm * 1000ppb * 8.4744 μg/m^3 = 1694.88
  aqi = max(aqi, compute_nepm_aqi(1694.88, so2));
  // pm10  μg/m^3, Coarse Particulate Matter (<10μm)
  aqi = max(aqi, compute_nepm_aqi(50, pm10));
  // pm2_5 μg/m^3, Fine Particulate Matter (<2.5μm)
  aqi = max(aqi, compute_nepm_aqi(25, pm2_5));

  return aqi;
}

/* Canada (AQHI)
 *
 * References:
 *   https://en.wikipedia.org/wiki/Air_Quality_Health_Index_(Canada)
 */
int canada_aqhi(float no2, float o3, float pm2_5) {
  return max(1, (int) round(
    (1000 / 10.4)
    * (
        (exp(0.000273533 * o3) - 1)  // 0.000537 * 1ppb/1.9632 μg/m^3 = 0.000273533
      + (exp(0.000462904 * no2) - 1) // 0.000871 * 1ppb/1.8816 μg/m^3 = 0.000462904
      + (exp(0.000487 * pm2_5) - 1)
      )
    ));

}

/* Europe (CAQI)
 *
 * References:
 *   http://airqualitynow.eu/about_indices_definition.php
 *   https://en.wikipedia.org/wiki/Air_quality_index
 */
int europe_caqi(float no2, float o3, float pm10, float pm2_5) {
  int aqi = 0;
  int i_low, i_high;
  int c_low, c_high;

  // no2   μg/m^3, Nitrogen Dioxide (NO2)
  if (no2 >= 0 && no2 <= 50) {
    i_low = 0;
    i_high = 25;
    c_low = 0;
    c_high = 50;
  } else if (no2 <= 100) {
    i_low = 25;
    i_high = 50;
    c_low = 50;
    c_high = 100;
  } else if (no2 <= 200) {
    i_low = 50;
    i_high = 75;
    c_low = 100;
    c_high = 200;
  } else if (no2 <= 400) {
    i_low = 75;
    i_high = 100;
    c_low = 200;
    c_high = 400;
  } else if (no2 > 400) {
    // > 100
    return 101;
  } else {
    return -1;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_low, i_high, c_low, c_high, no2));

  // o3    μg/m^3, Ground-Level Ozone (O3)
  if (o3 >= 0 && o3 <= 60) {
    i_low = 0;
    i_high = 25;
    c_low = 0;
    c_high = 60;
  } else if (o3 <= 120) {
    i_low = 25;
    i_high = 50;
    c_low = 60;
    c_high = 120;
  } else if (o3 <= 180) {
    i_low = 50;
    i_high = 75;
    c_low = 120;
    c_high = 180;
  } else if (o3 <= 240) {
    i_low = 75;
    i_high = 100;
    c_low = 180;
    c_high = 240;
  } else if (o3 > 240) {
    // > 100
    return 101;
  } else {
    return -1;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_low, i_high, c_low, c_high, o3));
  
  // pm10  μg/m^3, Coarse Particulate Matter (<10μm)
  if (pm10 >= 0 && pm10 <= 25) {
    i_low = 0;
    i_high = 25;
    c_low = 0;
    c_high = 25;
  } else if (pm10 <= 50) {
    i_low = 25;
    i_high = 50;
    c_low = 25;
    c_high = 50;
  } else if (pm10 <= 90) {
    i_low = 50;
    i_high = 75;
    c_low = 50;
    c_high = 90;
  } else if (pm10 <= 180) {
    i_low = 75;
    i_high = 100;
    c_low = 90;
    c_high = 180;
  } else if (pm10 > 180) {
    // > 100
    return 101;
  } else {
    return -1;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_low, i_high, c_low, c_high, pm10));

  // pm2_5 μg/m^3, Fine Particulate Matter (<2.5μm)
  if (pm2_5 >= 0 && pm2_5 <= 15) {
    i_low = 0;
    i_high = 25;
    c_low = 0;
    c_high = 15;
  } else if (pm2_5 <= 30) {
    i_low = 25;
    i_high = 50;
    c_low = 15;
    c_high = 30;
  } else if (pm2_5 <= 55) {
    i_low = 50;
    i_high = 75;
    c_low = 30;
    c_high = 55;
  } else if (pm2_5 <= 110) {
    i_low = 75;
    i_high = 100;
    c_low = 55;
    c_high = 110;
  } else if (pm2_5 > 110) {
    // > 100
    return 101;
  } else {
    return -1;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_low, i_high, c_low, c_high, pm2_5));

  return aqi;
}

/* Hong Kong (AQHI)
 *
 * References:
 *   https://aqicn.org/faq/2015-06-03/overview-of-hong-kongs-air-quality-health-index/
 *   https://www.aqhi.gov.hk/en/what-is-aqhi/faqs.html
 */
int hong_kong_aqhi(float no2, float o3, float so2, float pm10, float pm2_5) {
  float ar =   ((exp(0.0004462559 * no2) - 1) * 100)
             + ((exp(0.0001393235 * so2) - 1) * 100)
             + ((exp(0.0005116328 * o3) - 1) * 100)
             + fmax(((exp(0.0002821751 * pm10) - 1) * 100), 
                    ((exp(0.0002180567 * pm2_5) - 1) * 100));
  if (ar >= 0 && ar <= 1.88) {
    return 1;
  } else if (ar <= 3.76) {
    return 2;
  } else if (ar <= 5.64) {
    return 3;
  } else if (ar <= 7.52) {
    return 4;
  } else if (ar <= 9.41) {
    return 5;
  } else if (ar <= 11.29) {
    return 6;
  } else if (ar <= 12.91) {
    return 7;
  } else if (ar <= 15.07) {
    return 8;
  } else if (ar <= 17.22) {
    return 9;
  } else if (ar <= 19.37) {
    return 10;
  } else if (ar > 19.37) {
    return 11;
  }
  return -1;
}

/* India (AQI)
 *
 * References:
 *   https://www.aqi.in/blog/aqi/
 */
int india_aqi();

/* Mainland China (AQI)
 *
 * References:
 *   
 *   
 */
int mainland_china_aqi();

/* Singapore (PSI)
 *
 * References:
 *   
 *   
 */
int singapore_psi();

/* South Korea (CAI)
 *
 * References:
 *   
 *   
 */
int south_korea_cai();

/* United Kingdom (DAQI)
 *
 * References:
 *   
 *   
 */
int united_kingdom_daqi();

/* United States (AQI)
 *
 * References:
 *   
 *   
 */
int united_states_aqi();

/*   co    μg/m^3, Carbon Monoxide (CO)               1 ppb = 1.1456 μg/m^3
 *   nh3   μg/m^3, Ammonia (NH3)                      1 ppb = 0.6966 μg/m^3
 *   no    μg/m^3, Nitric Oxide (NO)                  1 ppb = 1.2274 μg/m^3
 *   no2   μg/m^3, Nitrogen Dioxide (NO2)             1 ppb = 1.8816 μg/m^3
 *   o3    μg/m^3, Ground-Level Ozone (O3)            1 ppb = 1.9632 μg/m^3
 *   pb    μg/m^3, Lead (Pb)                          1 ppb = 1.9632 μg/m^3
 *   so2   μg/m^3, Sulfur Dioxide (SO2)               1 ppb = 8.4744 μg/m^3
 *   pm2_5 μg/m^3, Fine Particulate Matter (<2.5μm)
 *   pm10  μg/m^3, Coarse Particulate Matter (<10μm)
 */

char* australia_aqi_desc();
char* canada_aqhi_desc();
char* europe_caqi_desc();
char* hong_kong_aqhi_desc();
char* india_aqi_desc();
char* mainland_china_aqi_desc();
char* singapore_psi_desc();
char* south_korea_cai_desc();
char* united_kingdom_daqi_desc();
char* united_states_aqi_desc();
