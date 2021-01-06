/************************************************************
■BME280 の使い方
	https://spiceman.jp/arduino-bme280-program/

■Arduino NanoとBME280を使って温度・湿度・気圧を計測する
	https://qiita.com/nanase/items/f34e03c29410add9c4d0
	
		■BME280搭載　温湿度・気圧センサモジュールの使い方
			https://trac.switch-science.com/wiki/BME280
			
■気圧から標高を計算
	https://keisan.casio.jp/exec/system/1257609530
	
■８．気象・天気のはなし⑧　気圧
	http://web.cc.yamaguchi-u.ac.jp/~yamaharu/tenki8.htm#:~:text=%E5%9C%B0%E4%B8%8A%E3%81%8B%E3%82%89%E9%AB%98%E3%81%84%E5%B1%B1%E3%81%AB,%E3%81%95%E3%81%A8%E3%81%A8%E3%82%82%E3%81%AB%E4%BD%8E%E3%81%8F%E3%81%AA%E3%82%8A%E3%81%BE%E3%81%99%E3%80%82&text=%E6%A8%99%E9%AB%98%E3%81%8C1000%EF%BD%8D%E9%AB%98%E3%81%8F%E3%81%AA%E3%82%8B,%E2%84%83%E3%81%BE%E3%81%A7%E4%BD%8E%E3%81%8F%E3%81%AA%E3%82%8A%E3%81%BE%E3%81%99%E3%80%82
	
■不快指数計算
	https://calculator.jp/health/discomfort/
************************************************************/
#define MODE_PLOTTER	0
#define MODE_PRINT		1
#define MODE_MEAS_TIME	2
	#define MODE MODE_PLOTTER

#include <Wire.h>
#include "SparkFunBME280.h"

/************************************************************
************************************************************/
void setupBME280();

/************************************************************
************************************************************/
BME280 sensor;

/************************************************************
************************************************************/

/******************************
******************************/
void setup() {
	/********************
	********************/
	Serial.begin(9600);
	
	/********************
	********************/
	Wire.begin();
	
	sensor.setI2CAddress(0x77);
	sensor.beginI2C();  // Wire を用いて I2C 接続開始
	
	/* */
	sensor.setFilter(0);							// 0->filter off, 1->coef = 2, 2->4, 3->8, 4->16
	sensor.setStandbyTime(0);						// no need when MODE_FORCED
	
	/* */
	int id_overSampling = 4;
	sensor.setTempOverSample(id_overSampling);		// valid number = 0, 1, 2, 4, 8, 16 : 0 means skip meas
	sensor.setPressureOverSample(id_overSampling);
	sensor.setHumidityOverSample(id_overSampling);
	
	/* */
	sensor.setMode(MODE_SLEEP); // default = NORMAL mode.
}

/******************************
******************************/
void setupBME280() {
	/********************
	********************/
	Wire.begin();
	
	/********************
	********************/
	sensor.setI2CAddress(0x76);
	if (sensor.beginI2C()) {
		Serial.println("I2C address: 0x76");
		return;
	}
	
	/********************
	********************/
	sensor.setI2CAddress(0x77);
	if (sensor.beginI2C()) {
		Serial.println("I2C address: 0x77");
		return;
	}
	
	/********************
	********************/
	Serial.println("Sensor connect failed");
	while(1) { }
}

/******************************
******************************/
void loop() {
	/********************
	********************/
	sensor.setMode(MODE_FORCED);
	
	unsigned long t_from = micros();
	
	while (!sensor.isMeasuring()) delay(1);
	while ( sensor.isMeasuring()) delay(1);
	
	unsigned long t_to = micros();
	
	/********************
	********************/
#if (MODE == MODE_MEAS_TIME)
    Serial.println(float(t_to - t_from)/1000, 2);
	delay(1000);
	
#elif (MODE == MODE_PRINT)

	Serial.print("Temp: ");
	Serial.print(sensor.readTempC(), 2);
	
	Serial.print(" °C, Humidity: ");
	Serial.print(sensor.readFloatHumidity(), 2);
	
	Serial.print(" %, Pressure: ");
	Serial.print(sensor.readFloatPressure() / 100.0, 1);
	Serial.print(" hPa, Altitude: ");
	
	Serial.print(sensor.readFloatAltitudeMeters(), 1);
	Serial.print(" m, Discomfort index: ");
	
	/* 不快指数 */
	double temp = sensor.readTempC();
	double humi = sensor.readFloatHumidity();
	double di = 0.81 * temp + 0.01 * humi * (0.99 * temp - 14.3) + 46.3;
	Serial.println(di, 1);
	
	/* */
	delay(3000);
	
#elif (MODE == MODE_PLOTTER)

	Serial.print(millis()/1000 % 2 * 100);
    Serial.print(",");
	
	Serial.println(sensor.readFloatHumidity(), 2);
	
	delay(1);
	
#else

	// nothing.
	
#endif
}