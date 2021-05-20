#ifdef EMBEDDED

#include "ble.h"
#include <bluefruit.h>
#include <ble_gap.h>
#include "system/system.h"


BLE::BLE() {
}

void BLE::startAdvertising() {
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_GENERIC_WATCH);

  // Include BLE HID service
  Bluefruit.Advertising.addService(_hid);

  // There is enough room for the dev name in the advertising packet
  Bluefruit.Advertising.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(false);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(5);      // number of seconds in fast mode
  Bluefruit.Advertising.start(30);                // 0 = Don't stop advertising after n seconds  
}

void BLE::run() {
  Bluefruit.begin();
  Bluefruit.setTxPower(4);
  Bluefruit.autoConnLed(false);
  Bluefruit.setName("BLETest");

  Bluefruit.Periph.setConnInterval(16, 300);
  _hid.begin();
  // can't do this until the next release of framework-arduinoadafruitnrf52,
  // with the 0.22 release of the adafruit BSP
  //Bluefruit.Periph.setConnSlaveLatency(100);

  startAdvertising();
}

void BLE::test() {
	for (uint16_t conn_hdl=0; conn_hdl < BLE_MAX_CONNECTION; conn_hdl++) {
		BLEConnection* connection = Bluefruit.Connection(conn_hdl);
		if ( connection ) {
      _hid.consumerKeyPress(conn_hdl, HID_USAGE_CONSUMER_VOLUME_DECREMENT);
			delay(10);
      _hid.consumerKeyRelease(conn_hdl);

      Serial.print("interval: ");
      Serial.println(connection->getConnectionInterval());
      
      ble_gap_conn_params_t pp;
      uint32_t status = sd_ble_gap_ppcp_get(&pp);
      Serial.printf("Status = %d\n\r", status);
      Serial.printf("\t%d\r\n\t%d\r\n\t%d\r\n\t%d\r\n", pp.min_conn_interval, pp.max_conn_interval, pp.slave_latency, pp.conn_sup_timeout);
		}
	}
}



#endif
