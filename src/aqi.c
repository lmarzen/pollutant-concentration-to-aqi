#include <math.h>
#include "aqi.h"

int max(int a, int b) {
  return a >= b ? a : b;
}

int compute_nepm_aqi(float std, float c) {
  return (int) round(c / std * 100);
}

int compute_piecewise_aqi(float i_lo, float i_hi,
                          float c_lo, float c_hi, float c) {
  return (int) max(i_lo, round(
    (((i_hi - i_lo)) / ((c_hi - c_lo))) * (c - c_lo) + i_lo
    ));
}

/* Australia (AQI)
 *
 * References:
 *   https://www.environment.nsw.gov.au/topics/air/understanding-air-quality-data/air-quality-categories/history-of-air-quality-reporting/about-the-air-quality-index
 */
int australia_aqi(float co_8h,  float no2_1h,   float o3_1h, float o3_4h, 
                  float so2_1h, float pm10_24h, float pm2_5_24h) {
  int aqi = 0;

  // co    μg/m^3, Carbon Monoxide (CO)
  // standard = 9.0ppm * 1000ppb * 1.1456 μg/m^3 = 10310.4
  aqi = max(aqi, compute_nepm_aqi(10310.4, co_8h));
  // no2   μg/m^3, Nitrogen Dioxide (NO2)
  // standard = 0.12ppm * 1000ppb * 1.8816 μg/m^3 = 10310.4
  aqi = max(aqi, compute_nepm_aqi(225.792, no2_1h));
  // o3    μg/m^3, Ground-Level Ozone (O3)
  // standard = 0.10ppm * 1000ppb * 1.9632 μg/m^3 = 196.32
  aqi = max(aqi, compute_nepm_aqi(196.32, o3_1h));
  // standard = 0.08ppm * 1000ppb * 1.9632 μg/m^3 = 157.056
  aqi = max(aqi, compute_nepm_aqi(157.056, o3_4h));
  // so2   μg/m^3, Sulfur Dioxide (SO2)
  // standard = 0.20ppm * 1000ppb * 8.4744 μg/m^3 = 1694.88
  aqi = max(aqi, compute_nepm_aqi(1694.88, so2_1h));
  // pm10  μg/m^3, Coarse Particulate Matter (<10μm)
  aqi = max(aqi, compute_nepm_aqi(50, pm10_24h));
  // pm2_5 μg/m^3, Fine Particulate Matter (<2.5μm)
  aqi = max(aqi, compute_nepm_aqi(25, pm2_5_24h));

  return aqi;
}

/* Canada (AQHI)
 *
 * References:
 *   https://en.wikipedia.org/wiki/Air_Quality_Health_Index_(Canada)
 */
int canada_aqhi(float no2_3h, float o3_3h, float pm2_5_3h) {
  return max(1, (int) round(
    (1000 / 10.4)
    * (
        (exp(0.000273533 * o3_3h) - 1)  // 0.000537 * 1ppb/1.9632 μg/m^3 = 0.000273533
      + (exp(0.000462904 * no2_3h) - 1) // 0.000871 * 1ppb/1.8816 μg/m^3 = 0.000462904
      + (exp(0.000487 * pm2_5_3h) - 1)
      )
    ));

}

/* Europe (CAQI)
 *
 * References:
 *   http://airqualitynow.eu/about_indices_definition.php
 *   https://en.wikipedia.org/wiki/Air_quality_index#CAQI
 */
int europe_caqi(float no2_1h, float o3_1h, float pm10_1h, float pm2_5_1h) {
  int aqi = 0;
  float i_lo, i_hi;
  float c_lo, c_hi;

  // no2   μg/m^3, Nitrogen Dioxide (NO2)
  if (no2_1h <= 50) {
    i_lo = 0;
    i_hi = 25;
    c_lo = 0;
    c_hi = 50;
  } else if (no2_1h <= 100) {
    i_lo = 26;
    i_hi = 50;
    c_lo = 50;
    c_hi = 100;
  } else if (no2_1h <= 200) {
    i_lo = 51;
    i_hi = 75;
    c_lo = 100;
    c_hi = 200;
  } else if (no2_1h <= 400) {
    i_lo = 76;
    i_hi = 100;
    c_lo = 200;
    c_hi = 400;
  } else {
    // index > 100
    return 101;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, no2_1h));

  // o3    μg/m^3, Ground-Level Ozone (O3)
  if (o3_1h <= 60) {
    i_lo = 0;
    i_hi = 25;
    c_lo = 0;
    c_hi = 60;
  } else if (o3_1h <= 120) {
    i_lo = 25;
    i_hi = 50;
    c_lo = 60;
    c_hi = 120;
  } else if (o3_1h <= 180) {
    i_lo = 51;
    i_hi = 75;
    c_lo = 120;
    c_hi = 180;
  } else if (o3_1h <= 240) {
    i_lo = 76;
    i_hi = 100;
    c_lo = 180;
    c_hi = 240;
  } else {
    // index > 100
    return 101;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, o3_1h));
  
  // pm10  μg/m^3, Coarse Particulate Matter (<10μm)
  if (pm10_1h <= 25) {
    i_lo = 0;
    i_hi = 25;
    c_lo = 0;
    c_hi = 25;
  } else if (pm10_1h <= 50) {
    i_lo = 26;
    i_hi = 50;
    c_lo = 25;
    c_hi = 50;
  } else if (pm10_1h <= 90) {
    i_lo = 51;
    i_hi = 75;
    c_lo = 50;
    c_hi = 90;
  } else if (pm10_1h <= 180) {
    i_lo = 76;
    i_hi = 100;
    c_lo = 90;
    c_hi = 180;
  } else {
    // index > 100
    return 101;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, pm10_1h));

  // pm2_5 μg/m^3, Fine Particulate Matter (<2.5μm)
  if (pm2_5_1h <= 15) {
    i_lo = 0;
    i_hi = 25;
    c_lo = 0;
    c_hi = 15;
  } else if (pm2_5_1h <= 30) {
    i_lo = 26;
    i_hi = 50;
    c_lo = 15;
    c_hi = 30;
  } else if (pm2_5_1h <= 55) {
    i_lo = 51;
    i_hi = 75;
    c_lo = 30;
    c_hi = 55;
  } else if (pm2_5_1h <= 110) {
    i_lo = 76;
    i_hi = 100;
    c_lo = 55;
    c_hi = 110;
  } else {
    // index > 100
    return 101;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, pm2_5_1h));

  return aqi;
}

/* Hong Kong (AQHI)
 *
 * References:
 *   https://www.aqhi.gov.hk/en/what-is-aqhi/faqs.html
 *   https://aqicn.org/faq/2015-06-03/overview-of-hong-kongs-air-quality-health-index/
 */
int hong_kong_aqhi(float no2_3h,  float o3_3h, float so2_3h, 
                   float pm10_3h, float pm2_5_3h) {
  float ar =   ((exp(0.0004462559 * no2_3h) - 1) * 100)
             + ((exp(0.0001393235 * so2_3h) - 1) * 100)
             + ((exp(0.0005116328 * o3_3h) - 1) * 100)
             + fmax(((exp(0.0002821751 * pm10_3h) - 1) * 100), 
                    ((exp(0.0002180567 * pm2_5_3h) - 1) * 100));
  if (ar <= 1.88) {
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
  } else {
    // index > 10
    return 11;
  }
}

/* India (AQI)
 *
 * References:
 *   https://www.aqi.in/blog/aqi/
 */
int india_aqi(float co_8h,  float nh3_24h, float no2_24h,  float o3_8h, 
              float pb_24h, float so2_24h, float pm10_24h, float pm2_5_24h) {
  int aqi = 0;
  float i_lo, i_hi;
  float c_lo, c_hi;

  // co    μg/m^3, Carbon Monoxide (CO)
  if (co_8h <= 1) {
    i_lo = 0;
    i_hi = 50;
    c_lo = 0;
    c_hi = 1;
  } else if (co_8h <= 2) {
    i_lo = 51;
    i_hi = 100;
    c_lo = 1;
    c_hi = 2;
  } else if (co_8h <= 10) {
    i_lo = 101;
    i_hi = 200;
    c_lo = 2;
    c_hi = 10;
  } else if (co_8h <= 17) {
    i_lo = 201;
    i_hi = 300;
    c_lo = 10;
    c_hi = 17;
  } else if (co_8h <= 34) {
    i_lo = 301;
    i_hi = 400;
    c_lo = 17;
    c_hi = 34;
  } else {
    // index > 400
    return 401;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, co_8h));

  // nh3   μg/m^3, Ammonia (NH3)
  if (nh3_24h <= 200) {
    i_lo = 0;
    i_hi = 50;
    c_lo = 0;
    c_hi = 200;
  } else if (nh3_24h <= 400) {
    i_lo = 51;
    i_hi = 100;
    c_lo = 200;
    c_hi = 400;
  } else if (nh3_24h <= 800) {
    i_lo = 101;
    i_hi = 200;
    c_lo = 400;
    c_hi = 800;
  } else if (nh3_24h <= 1200) {
    i_lo = 201;
    i_hi = 300;
    c_lo = 800;
    c_hi = 1200;
  } else if (nh3_24h <= 1800) {
    i_lo = 301;
    i_hi = 400;
    c_lo = 1200;
    c_hi = 1800;
  } else {
    // index > 400
    return 401;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, nh3_24h));

  // no2   μg/m^3, Nitrogen Dioxide (NO2)
  if (no2_24h <= 40) {
    i_lo = 0;
    i_hi = 50;
    c_lo = 0;
    c_hi = 40;
  } else if (no2_24h <= 80) {
    i_lo = 51;
    i_hi = 100;
    c_lo = 40;
    c_hi = 80;
  } else if (no2_24h <= 180) {
    i_lo = 101;
    i_hi = 200;
    c_lo = 80;
    c_hi = 180;
  } else if (no2_24h <= 280) {
    i_lo = 201;
    i_hi = 300;
    c_lo = 180;
    c_hi = 280;
  } else if (no2_24h <= 400) {
    i_lo = 301;
    i_hi = 400;
    c_lo = 280;
    c_hi = 400;
  } else {
    // index > 400
    return 401;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, no2_24h));

  // o3    μg/m^3, Ozone (O3)
  if (o3_8h <= 50) {
    i_lo = 0;
    i_hi = 50;
    c_lo = 0;
    c_hi = 50;
  } else if (o3_8h <= 100) {
    i_lo = 51;
    i_hi = 100;
    c_lo = 50;
    c_hi = 100;
  } else if (o3_8h <= 168) {
    i_lo = 101;
    i_hi = 200;
    c_lo = 100;
    c_hi = 168;
  } else if (o3_8h <= 208) {
    i_lo = 201;
    i_hi = 300;
    c_lo = 168;
    c_hi = 208;
  } else if (o3_8h <= 748) {
    i_lo = 301;
    i_hi = 400;
    c_lo = 208;
    c_hi = 748;
  } else {
    // index > 400
    return 401;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, o3_8h));

  // pb    μg/m^3, Lead (Pb)
  if (pb_24h pb_24h <= 0.5) {
    i_lo = 0;
    i_hi = 50;
    c_lo = 0;
    c_hi = 0.5;
  } else if (pb_24h <= 1.0) {
    i_lo = 51;
    i_hi = 100;
    c_lo = 0.5;
    c_hi = 1.0;
  } else if (pb_24h <= 2.0) {
    i_lo = 101;
    i_hi = 200;
    c_lo = 1.0;
    c_hi = 2.0;
  } else if (pb_24h <= 3.0) {
    i_lo = 201;
    i_hi = 300;
    c_lo = 2.0;
    c_hi = 3.0;
  } else if (pb_24h <= 3.5) {
    i_lo = 301;
    i_hi = 400;
    c_lo = 3.0;
    c_hi = 3.5;
  } else {
    // index > 400
    return 401;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, pb_24h));

  // so2   μg/m^3, Sulfur Dioxide (SO2)
  if (so2_24h <= 40) {
    i_lo = 0;
    i_hi = 50;
    c_lo = 0;
    c_hi = 40;
  } else if (so2_24h <= 80) {
    i_lo = 51;
    i_hi = 100;
    c_lo = 40;
    c_hi = 80;
  } else if (so2_24h <= 380) {
    i_lo = 101;
    i_hi = 200;
    c_lo = 80;
    c_hi = 380;
  } else if (so2_24h <= 800) {
    i_lo = 201;
    i_hi = 300;
    c_lo = 380;
    c_hi = 800;
  } else if (so2_24h <= 1600) {
    i_lo = 301;
    i_hi = 400;
    c_lo = 800;
    c_hi = 1600;
  } else {
    // index > 400
    return 401;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, so2_24h));

  // pm10  μg/m^3, Coarse Particulate Matter (<10μm)
  if (pm10_24h <= 50) {
    i_lo = 0;
    i_hi = 50;
    c_lo = 0;
    c_hi = 50;
  } else if (pm10_24h <= 100) {
    i_lo = 51;
    i_hi = 100;
    c_lo = 50;
    c_hi = 100;
  } else if (pm10_24h <= 250) {
    i_lo = 101;
    i_hi = 200;
    c_lo = 100;
    c_hi = 250;
  } else if (pm10_24h <= 350) {
    i_lo = 201;
    i_hi = 300;
    c_lo = 250;
    c_hi = 350;
  } else if (pm10_24h <= 430) {
    i_lo = 301;
    i_hi = 400;
    c_lo = 350;
    c_hi = 430;
  } else {
    // index > 400
    return 401;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, pm10_24h));

  // pm2_5 μg/m^3, Fine Particulate Matter (<2.5μm)
  if (pm2_5_24h <= 30) {
    i_lo = 0;
    i_hi = 50;
    c_lo = 0;
    c_hi = 30;
  } else if (pm2_5_24h <= 60) {
    i_lo = 51;
    i_hi = 100;
    c_lo = 30;
    c_hi = 60;
  } else if (pm2_5_24h <= 90) {
    i_lo = 101;
    i_hi = 200;
    c_lo = 60;
    c_hi = 90;
  } else if (pm2_5_24h <= 120) {
    i_lo = 201;
    i_hi = 300;
    c_lo = 90;
    c_hi = 120;
  } else if (pm2_5_24h <= 250) {
    i_lo = 301;
    i_hi = 400;
    c_lo = 120;
    c_hi = 250;
  } else {
    // index > 400
    return 401;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, pm2_5_24h));

  return aqi;
}

/* Mainland China (AQI)
 *
 * References:
 *   https://web.archive.org/web/20180830110324/http://kjs.mep.gov.cn/hjbhbz/bzwb/jcffbz/201203/W020120410332725219541.pdf
 *   https://en.wikipedia.org/wiki/Air_quality_index#Mainland_China
 *   https://datadrivenlab.org/air-quality-2/chinas-new-air-quality-index-how-does-it-measure-up/
 */
int mainland_china_aqi(float co_1h, float co_24h, float no2_1h, float no2_24h,  
                       float o3_1h, float o3_8h,  float so2_1h, float so2_24h, 
                       float pm10_24h, float pm2_5_24h) {
  int aqi = 0;
  float i_lo, i_hi;
  float c_lo, c_hi;

  // co    μg/m^3, Carbon Monoxide (CO)
  if (co_1h <= 5) {
    i_lo = 0;
    i_hi = 50;
    c_lo = 0;
    c_hi = 5;
  } else if (co_1h <= 10) {
    i_lo = 51;
    i_hi = 100;
    c_lo = 5;
    c_hi = 10;
  } else if (co_1h <= 35) {
    i_lo = 101;
    i_hi = 150;
    c_lo = 10;
    c_hi = 35;
  } else if (co_1h <= 60) {
    i_lo = 151;
    i_hi = 200;
    c_lo = 35;
    c_hi = 60;
  } else if (co_1h <= 90) {
    i_lo = 201;
    i_hi = 300;
    c_lo = 60;
    c_hi = 90;
  } else if (co_1h <= 120) {
    i_lo = 301;
    i_hi = 400;
    c_lo = 90;
    c_hi = 120;
  } else if (co_1h <= 150) {
    i_lo = 401;
    i_hi = 500;
    c_lo = 120;
    c_hi = 150;
  } else {
    // index > 500
    return 501;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, co_1h));

  if (co_24h <= 2) {
    i_lo = 0;
    i_hi = 50;
    c_lo = 0;
    c_hi = 2;
  } else if (co_24h <= 4) {
    i_lo = 51;
    i_hi = 100;
    c_lo = 2;
    c_hi = 4;
  } else if (co_24h <= 14) {
    i_lo = 101;
    i_hi = 150;
    c_lo = 4;
    c_hi = 14;
  } else if (co_24h <= 24) {
    i_lo = 151;
    i_hi = 200;
    c_lo = 14;
    c_hi = 24;
  } else if (co_24h <= 36) {
    i_lo = 201;
    i_hi = 300;
    c_lo = 24;
    c_hi = 36;
  } else if (co_24h <= 48) {
    i_lo = 301;
    i_hi = 400;
    c_lo = 36;
    c_hi = 48;
  } else if (co_24h <= 60) {
    i_lo = 401;
    i_hi = 500;
    c_lo = 48;
    c_hi = 60;
  } else {
    // index > 500
    return 501;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, co_24h));

  // no2   μg/m^3, Nitrogen Dioxide (NO2)
  if (no2_1h <= 100) {
    i_lo = 0;
    i_hi = 50;
    c_lo = 0;
    c_hi = 100;
  } else if (no2_1h <= 200) {
    i_lo = 51;
    i_hi = 100;
    c_lo = 100;
    c_hi = 200;
  } else if (no2_1h <= 700) {
    i_lo = 101;
    i_hi = 150;
    c_lo = 200;
    c_hi = 700;
  } else if (no2_1h <= 1200) {
    i_lo = 151;
    i_hi = 200;
    c_lo = 700;
    c_hi = 1200;
  } else if (no2_1h <= 2340) {
    i_lo = 201;
    i_hi = 300;
    c_lo = 1200;
    c_hi = 2340;
  } else if (no2_1h <= 3090) {
    i_lo = 301;
    i_hi = 400;
    c_lo = 2340;
    c_hi = 3090;
  } else if (no2_1h <= 3840) {
    i_lo = 401;
    i_hi = 500;
    c_lo = 3090;
    c_hi = 3840;
  } else {
    // index > 500
    return 501;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, no2_1h));

  if (no2_24h <= 40) {
    i_lo = 0;
    i_hi = 50;
    c_lo = 0;
    c_hi = 40;
  } else if (no2_24h <= 80) {
    i_lo = 51;
    i_hi = 100;
    c_lo = 40;
    c_hi = 80;
  } else if (no2_24h <= 180) {
    i_lo = 101;
    i_hi = 150;
    c_lo = 80;
    c_hi = 180;
  } else if (no2_24h <= 280) {
    i_lo = 151;
    i_hi = 200;
    c_lo = 180;
    c_hi = 280;
  } else if (no2_24h <= 565) {
    i_lo = 201;
    i_hi = 300;
    c_lo = 280;
    c_hi = 565;
  } else if (no2_24h <= 750) {
    i_lo = 301;
    i_hi = 400;
    c_lo = 565;
    c_hi = 750;
  } else if (no2_24h <= 940) {
    i_lo = 401;
    i_hi = 500;
    c_lo = 750;
    c_hi = 940;
  } else {
    // index > 500
    return 501;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, no2_24h));

  // o3    μg/m^3, Ozone (O3)
  if (o3_1h <= 160) {
    i_lo = 0;
    i_hi = 50;
    c_lo = 0;
    c_hi = 160;
  } else if (o3_1h <= 200) {
    i_lo = 51;
    i_hi = 100;
    c_lo = 160;
    c_hi = 200;
  } else if (o3_1h <= 300) {
    i_lo = 101;
    i_hi = 150;
    c_lo = 200;
    c_hi = 300;
  } else if (o3_1h <= 400) {
    i_lo = 151;
    i_hi = 200;
    c_lo = 300;
    c_hi = 400;
  } else if (o3_1h <= 800) {
    i_lo = 201;
    i_hi = 300;
    c_lo = 400;
    c_hi = 800;
  } else if (o3_1h <= 1000) {
    i_lo = 301;
    i_hi = 400;
    c_lo = 800;
    c_hi = 1000;
  } else if (o3_1h <= 1200) {
    i_lo = 401;
    i_hi = 500;
    c_lo = 1000;
    c_hi = 1200;
  } else {
    // index > 500
    return 501;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, o3_1h));

  // If 8 hour average of o3 is > 800 μg/m^3 don't calculate it.
  if (o3_8h <= 800) {
    if (o3_8h <= 100) {
      i_lo = 0;
      i_hi = 50;
      c_lo = 0;
      c_hi = 100;
    } else if (o3_8h <= 160) {
      i_lo = 51;
      i_hi = 100;
      c_lo = 100;
      c_hi = 160;
    } else if (o3_8h <= 215) {
      i_lo = 101;
      i_hi = 150;
      c_lo = 160;
      c_hi = 215;
    } else if (o3_8h <= 265) {
      i_lo = 151;
      i_hi = 200;
      c_lo = 215;
      c_hi = 265;
    } else {
      // 265 < o3_8h <= 800
      i_lo = 201;
      i_hi = 300;
      c_lo = 265;
      c_hi = 800;
    }
    aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, o3_8h));
  }

  // so2   μg/m^3, Sulfur Dioxide (SO2)
  // If 1 hour average of so2 is > 800 μg/m^3 don't calculate it.
  if (so2_1h <= 800) {
    if (so2_1h <= 150) {
      i_lo = 0;
      i_hi = 50;
      c_lo = 0;
      c_hi = 150;
    } else if (so2_1h <= 500) {
      i_lo = 51;
      i_hi = 100;
      c_lo = 150;
      c_hi = 500;
    } else if (so2_1h <= 650) {
      i_lo = 101;
      i_hi = 150;
      c_lo = 500;
      c_hi = 650;
    } else {
      // 650 < so2_1h <= 800
      i_lo = 151;
      i_hi = 200;
      c_lo = 650;
      c_hi = 800;
    }
    aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, so2_1h));
  }

  if (so2_24h <= 50) {
    i_lo = 0;
    i_hi = 50;
    c_lo = 0;
    c_hi = 50;
  } else if (so2_24h <= 150) {
    i_lo = 51;
    i_hi = 100;
    c_lo = 50;
    c_hi = 150;
  } else if (so2_24h <= 475) {
    i_lo = 101;
    i_hi = 150;
    c_lo = 150;
    c_hi = 475;
  } else if (so2_24h <= 800) {
    i_lo = 151;
    i_hi = 200;
    c_lo = 475;
    c_hi = 800;
  } else if (so2_24h <= 1600) {
    i_lo = 201;
    i_hi = 300;
    c_lo = 800;
    c_hi = 1600;
  } else if (so2_24h <= 2100) {
    i_lo = 301;
    i_hi = 400;
    c_lo = 1600;
    c_hi = 2100;
  } else if (so2_24h <= 2620) {
    i_lo = 401;
    i_hi = 500;
    c_lo = 2100;
    c_hi = 2620;
  } else {
    // index > 500
    return 501;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, so2_24h));

  // pm10  μg/m^3, Coarse Particulate Matter (<10μm)
  if (pm10_24h <= 50) {
    i_lo = 0;
    i_hi = 50;
    c_lo = 0;
    c_hi = 50;
  } else if (pm10_24h <= 150) {
    i_lo = 51;
    i_hi = 100;
    c_lo = 50;
    c_hi = 150;
  } else if (pm10_24h <= 250) {
    i_lo = 101;
    i_hi = 150;
    c_lo = 150;
    c_hi = 250;
  } else if (pm10_24h <= 350) {
    i_lo = 151;
    i_hi = 200;
    c_lo = 250;
    c_hi = 350;
  } else if (pm10_24h <= 420) {
    i_lo = 201;
    i_hi = 300;
    c_lo = 350;
    c_hi = 420;
  } else if (pm10_24h <= 500) {
    i_lo = 301;
    i_hi = 400;
    c_lo = 420;
    c_hi = 500;
  } else if (pm10_24h <= 600) {
    i_lo = 401;
    i_hi = 500;
    c_lo = 500;
    c_hi = 600;
  } else {
    // index > 500
    return 501;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, pm10_24h));

  // pm2_5 μg/m^3, Fine Particulate Matter (<2.5μm)
  if (pm2_5_24h <= 35) {
    i_lo = 0;
    i_hi = 50;
    c_lo = 0;
    c_hi = 35;
  } else if (pm2_5_24h <= 75) {
    i_lo = 51;
    i_hi = 100;
    c_lo = 35;
    c_hi = 75;
  } else if (pm2_5_24h <= 115) {
    i_lo = 101;
    i_hi = 150;
    c_lo = 75;
    c_hi = 115;
  } else if (pm2_5_24h <= 150) {
    i_lo = 151;
    i_hi = 200;
    c_lo = 115;
    c_hi = 150;
  } else if (pm2_5_24h <= 250) {
    i_lo = 201;
    i_hi = 300;
    c_lo = 150;
    c_hi = 250;
  } else if (pm2_5_24h <= 350) {
    i_lo = 301;
    i_hi = 400;
    c_lo = 250;
    c_hi = 350;
  } else if (pm2_5_24h <= 500) {
    i_lo = 401;
    i_hi = 500;
    c_lo = 350;
    c_hi = 500;
  } else {
    // index > 500
    return 501;
  }
  aqi = max(aqi, compute_piecewise_aqi(i_lo, i_hi, c_lo, c_hi, pm2_5_24h));

  return aqi;
}

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
