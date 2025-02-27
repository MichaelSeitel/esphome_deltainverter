#include "esphome.h"

#define STX 0x02
#define ETX 0x03
#define ENQ 0x05
#define ACK 0x06
#define NAK 0x15
// 9 Bytes are just for STX ENQ/ACK/NAK ADDR #bytes CMD SUB_CMD .. CRC_L CRC_H ETX
#define ProtocolOverhead 9

class deltainverter : public PollingComponent, public Sensor, public UARTDevice {
public:
    deltainverter(UARTComponent *parent) : PollingComponent(400), UARTDevice(parent) {}

    Sensor *ac_v1 = new Sensor();
    Sensor *ac_c1 = new Sensor();
    Sensor *ac_p1 = new Sensor();
    Sensor *ac_f1 = new Sensor();
    Sensor *ac_v2 = new Sensor();
    Sensor *ac_c2 = new Sensor();
    Sensor *ac_p2 = new Sensor();
    Sensor *ac_f2 = new Sensor();
    Sensor *ac_v3 = new Sensor();
    Sensor *ac_c3 = new Sensor();
    Sensor *ac_p3 = new Sensor();
    Sensor *ac_f3 = new Sensor();
    Sensor *solar_v1 = new Sensor();
    Sensor *solar_c1 = new Sensor();
    Sensor *solar_p1 = new Sensor();
    Sensor *solar_v2 = new Sensor();
    Sensor *solar_c2 = new Sensor();
    Sensor *solar_p2 = new Sensor();
    Sensor *ac_power = new Sensor();
    Sensor *bus_vplus = new Sensor();
    Sensor *bus_vminus = new Sensor();
    Sensor *ac_energy_today = new Sensor();
    Sensor *inv_runtime_today = new Sensor();
    Sensor *ac_energy_total = new Sensor();
    Sensor *inv_runtime_total = new Sensor();
    Sensor *temp_rack = new Sensor();

    void setup() override {}

    std::vector<int> bytes;

    void update() {
        while (available() > 0) {
            bytes.push_back(read());
            //make sure at least 8 header bytes are available for check
            if (bytes.size() < ProtocolOverhead) {
                continue;
            }
            //ESP_LOGD("deltainverter", "Checking for inverter package");
            // Check for Delta Solivia Gateway package response.
            if (bytes[0] != STX || bytes[1] != ACK || bytes[3] != 0xA0) {
                bytes.erase(bytes.begin()); //remove first byte from buffer
                //buffer will never get above 8 until the response is a match
                continue;
            }
            //ESP_LOGD("deltainverter", "Got inverter package: %i", bytes.size());

            // Measurement & statsistic data (CMD = 0x60 SUB_CMD = 0x01)
            if (bytes[4] == 0x60 && bytes[5] == 0x01 && bytes.size() == (ProtocolOverhead + 158)) {
                // skip header
                int currentByteNo = 6;
                // SAP part number, serial number, datecode, revision
                currentByteNo += 11 + 13 + 4 + 2;
                // DSP FW Rev, FW Date
                currentByteNo += 2 + 2;
                // Redundant MCU FW Rev, Date
                currentByteNo += 2 + 2;
                // Display MCU FW Rev, Date
                currentByteNo += 2 + 2;
                // Display WebPage FW Rev, Date
                currentByteNo += 2 + 2;
                // Display WiFi FW Rev Date
                currentByteNo += 2 + 2;
                // AC Voltage P1
                TwoByte ac_v1_data;
                ac_v1_data.Byte[0] = bytes[currentByteNo + 1]; // AC Voltage P1 lsb
                ac_v1_data.Byte[1] = bytes[currentByteNo]; // AC Voltage P1 msb
                currentByteNo += 2;
                // AC Current P1
                TwoByte ac_c1_data;
                ac_c1_data.Byte[0] = bytes[currentByteNo + 1]; // AC Curent P1 lsb
                ac_c1_data.Byte[1] = bytes[currentByteNo]; // AC Current P1 msb
                currentByteNo += 2;
                // AC Power P1
                TwoByte ac_p1_data;
                ac_p1_data.Byte[0] = bytes[currentByteNo + 1]; // AC Power P1 lsb
                ac_p1_data.Byte[1] = bytes[currentByteNo]; // AC Power P1 msb
                currentByteNo += 2;
                // AC Phase P1
                TwoByte ac_f1_data;
                ac_f1_data.Byte[0] = bytes[currentByteNo + 1]; // AC Phase P1 lsb
                ac_f1_data.Byte[1] = bytes[currentByteNo]; // AC Phase P1 msb
                currentByteNo += 2;
                // AC Voltage Phase Redundant P1
                currentByteNo += 2 + 2;

                // AC Voltage P2
                TwoByte ac_v2_data;
                ac_v2_data.Byte[0] = bytes[currentByteNo + 1]; // AC Voltage P2 lsb
                ac_v2_data.Byte[1] = bytes[currentByteNo]; // AC Voltage P2 msb
                currentByteNo += 2;
                // AC Current P2
                TwoByte ac_c2_data;
                ac_c2_data.Byte[0] = bytes[currentByteNo + 1]; // AC Curent P2 lsb
                ac_c2_data.Byte[1] = bytes[currentByteNo]; // AC Current P2 msb
                currentByteNo += 2;
                // AC Power P2
                TwoByte ac_p2_data;
                ac_p2_data.Byte[0] = bytes[currentByteNo + 1]; // AC Power P2 lsb
                ac_p2_data.Byte[1] = bytes[currentByteNo]; // AC Power P2 msb
                currentByteNo += 2;
                // AC Phase P2
                TwoByte ac_f2_data;
                ac_f2_data.Byte[0] = bytes[currentByteNo + 1]; // AC Phase P2 lsb
                ac_f2_data.Byte[1] = bytes[currentByteNo]; // AC Phase P2 msb
                currentByteNo += 2;
                // AC Voltage Phase Redundant P2
                currentByteNo += 2 + 2;

                // AC Voltage P3
                TwoByte ac_v3_data;
                ac_v3_data.Byte[0] = bytes[currentByteNo + 1]; // AC Voltage P3 lsb
                ac_v3_data.Byte[1] = bytes[currentByteNo]; // AC Voltage P3 msb
                currentByteNo += 2;
                // AC Current P3
                TwoByte ac_c3_data;
                ac_c3_data.Byte[0] = bytes[currentByteNo + 1]; // AC Curent P3 lsb
                ac_c3_data.Byte[1] = bytes[currentByteNo]; // AC Current P3 msb
                currentByteNo += 2;
                // AC Power P3
                TwoByte ac_p3_data;
                ac_p3_data.Byte[0] = bytes[currentByteNo + 1]; // AC Power P3 lsb
                ac_p3_data.Byte[1] = bytes[currentByteNo]; // AC Power P3 msb
                currentByteNo += 2;
                // AC Phase P3
                TwoByte ac_f3_data;
                ac_f3_data.Byte[0] = bytes[currentByteNo + 1]; // AC Phase P3 lsb
                ac_f3_data.Byte[1] = bytes[currentByteNo]; // AC Phase P3 msb
                currentByteNo += 2;
                // AC Voltage Phase Redundant P3
                currentByteNo += 2 + 2;

                // Solar Voltage I1
                TwoByte solar_v1_data;
                solar_v1_data.Byte[0] = bytes[currentByteNo + 1]; // Solar Voltage I1 lsb
                solar_v1_data.Byte[1] = bytes[currentByteNo]; // Solar Voltage I1 msb
                currentByteNo += 2;
                // Solar Current I1
                TwoByte solar_c1_data;
                solar_c1_data.Byte[0] = bytes[currentByteNo + 1]; // Solar Current I1 lsb
                solar_c1_data.Byte[1] = bytes[currentByteNo]; // Solar Current I1 msb
                currentByteNo += 2;
                // Solar Power I1
                TwoByte solar_p1_data;
                solar_p1_data.Byte[0] = bytes[currentByteNo + 1]; // Solar Power I1 lsb
                solar_p1_data.Byte[1] = bytes[currentByteNo]; // Solar Power I1 msb
                currentByteNo += 2;

                // Solar Voltage I2
                TwoByte solar_v2_data;
                solar_v2_data.Byte[0] = bytes[currentByteNo + 1]; // Solar Voltage I2 lsb
                solar_v2_data.Byte[1] = bytes[currentByteNo]; // Solar Voltage I2 msb
                currentByteNo += 2;
                // Solar Current I2
                TwoByte solar_c2_data;
                solar_c2_data.Byte[0] = bytes[currentByteNo + 1]; // Solar Current I2 lsb
                solar_c2_data.Byte[1] = bytes[currentByteNo]; // Solar Current I2 msb
                currentByteNo += 2;
                // Solar Power I2
                TwoByte solar_p2_data;
                solar_p2_data.Byte[0] = bytes[currentByteNo + 1]; // Solar Power I2 lsb
                solar_p2_data.Byte[1] = bytes[currentByteNo]; // Solar Power I2 msb
                currentByteNo += 2;

                // AC Power
                TwoByte ac_power_data;
                ac_power_data.Byte[0] = bytes[currentByteNo + 1]; // AC Power lsb
                ac_power_data.Byte[1] = bytes[currentByteNo]; // AC Power msb
                currentByteNo += 2;

                // Bus Voltage +
                TwoByte bus_vplus_data;
                bus_vplus_data.Byte[0] = bytes[currentByteNo + 1]; // Bus Voltage + lsb
                bus_vplus_data.Byte[1] = bytes[currentByteNo]; // Bus Voltage + msb
                currentByteNo += 2;
                // Bus Voltage -
                TwoByte bus_vminus_data;
                bus_vminus_data.Byte[0] = bytes[currentByteNo + 1]; // Bus Voltage - lsb
                bus_vminus_data.Byte[1] = bytes[currentByteNo]; // Bus Voltage - msb
                currentByteNo += 2;

                // Supplied AC Energy today
                uint32_t ac_energy_today_data = int(
                        (unsigned char) (bytes[currentByteNo]) << 24 |
                        (unsigned char) (bytes[currentByteNo + 1]) << 16 |
                        (unsigned char) (bytes[currentByteNo + 2]) << 8 |
                        (unsigned char) (bytes[currentByteNo + 3]));  // Supplied AC Energy today
                currentByteNo += 4;

                // Inverter runtime today
                uint32_t inv_runtime_today_data = int(
                        (unsigned char) (bytes[currentByteNo]) << 24 |
                        (unsigned char) (bytes[currentByteNo + 1]) << 16 |
                        (unsigned char) (bytes[currentByteNo + 2]) << 8 |
                        (unsigned char) (bytes[currentByteNo + 3]));  // Inverter runtime today
                currentByteNo += 4;

                // Supplied AC Energy total
                uint32_t ac_energy_total_data = int(
                        (unsigned char) (bytes[currentByteNo]) << 24 |
                        (unsigned char) (bytes[currentByteNo + 1]) << 16 |
                        (unsigned char) (bytes[currentByteNo + 2]) << 8 |
                        (unsigned char) (bytes[currentByteNo + 3]));  // Supplied AC Energy total
                currentByteNo += 4;

                // Inverter runtime total
                uint32_t inv_runtime_total_data = int(
                        (unsigned char) (bytes[currentByteNo]) << 24 |
                        (unsigned char) (bytes[currentByteNo + 1]) << 16 |
                        (unsigned char) (bytes[currentByteNo + 2]) << 8 |
                        (unsigned char) (bytes[currentByteNo + 3]));  // Inverter runtime total
                currentByteNo += 4;

                // Temperature inside rack
                TwoByte temp_rack_data;
                temp_rack_data.Byte[0] = bytes[currentByteNo + 1]; // Temperature inside rack lsb
                temp_rack_data.Byte[1] = bytes[currentByteNo]; // Temperature inside rack msb
                currentByteNo += 2;

                // Quick and dirty check for package integrity is done, in order to avoid irratic sensor value updates
                // This effectively blocks out any erroneous sensor updates due to rx package corruption
                // Check if ETX = 3. If not (invalid package), ditch whole package, clear buffer and continue
                if (bytes[bytes.size() - 1] != 0x03) {
                    ESP_LOGI("deltainverter", "ETX check failure - NO sensor update! ETX: %i", bytes[bytes.size() - 1]);
                    bytes.clear();
                    continue;
                }
                // crc check
                unsigned char arr[bytes.size()];
                for (int i = 0; i < bytes.size(); i++) {
                    arr[i] = bytes[i];
                }
                if(calc_crc(arr + 1, arr + bytes.size() - 3) != bytes[bytes.size() - 2]) {
                    ESP_LOGE("deltainverter", "CRC check failure: calc_crc: %i - bytes_crc: %i", calc_crc(arr + 1, arr + bytes.size() - 3), bytes[bytes.size() - 2]);
                    bytes.clear();
                    continue;
                }
                //ESP_LOGI("deltainverter", "CRC check ok");


                // Publish Data
                ac_v1->publish_state(ac_v1_data.UInt16);
                ac_c1->publish_state(ac_c1_data.UInt16);
                ac_p1->publish_state(ac_p1_data.UInt16);
                ac_f1->publish_state(ac_f1_data.UInt16);
                ac_v2->publish_state(ac_v2_data.UInt16);
                ac_c2->publish_state(ac_c2_data.UInt16);
                ac_p2->publish_state(ac_p2_data.UInt16);
                ac_f2->publish_state(ac_f2_data.UInt16);
                ac_v3->publish_state(ac_v3_data.UInt16);
                ac_c3->publish_state(ac_c3_data.UInt16);
                ac_p3->publish_state(ac_p3_data.UInt16);
                ac_f3->publish_state(ac_f3_data.UInt16);
                solar_v1->publish_state(solar_v1_data.UInt16);
                solar_c1->publish_state(solar_c1_data.UInt16);
                solar_p1->publish_state(solar_p1_data.UInt16);
                solar_v2->publish_state(solar_v2_data.UInt16);
                solar_c2->publish_state(solar_c2_data.UInt16);
                solar_p2->publish_state(solar_p2_data.UInt16);
                ac_power->publish_state(ac_power_data.UInt16);
                bus_vplus->publish_state(bus_vplus_data.UInt16);
                bus_vminus->publish_state(bus_vminus_data.UInt16);
                ac_energy_today->publish_state(ac_energy_today_data);
                inv_runtime_today->publish_state(inv_runtime_today_data);
                ac_energy_total->publish_state(ac_energy_total_data);
                inv_runtime_total->publish_state(inv_runtime_total_data);
                temp_rack->publish_state(temp_rack_data.Int16);

                // Log some data
                ESP_LOGI("deltainverter", "Current AC Power: %i W, Today: %i Wh, Total: %i kWh, Temp: %i°C",
                         ac_power_data.UInt16, ac_energy_today_data, ac_energy_total_data, temp_rack_data.UInt16);
                // clear buffer
                bytes.clear();


            }
                // Setup data (CMD = 0x60 SUB_CMD = 0x02)
            else if (bytes[4] == 0x60 && bytes[5] == 0x02 && bytes.size() == (ProtocolOverhead + 59))
            {
                // Example: 02:06:01:3C:60:02: 0B:3B: 00:14: 09:E2: 04:0B: 00:1E: 07:A3: 09:E2: EA:60: 09:B0: 07:30: 01:2C: 07:A3: 14:1E: 00:14: 13:92: 12:8E: 00:14: 12:93: 00:64: 00:02: 00:00: 00:32: 04:4C: 02:58: 01:2C: 00:03: 00:03: 29:04: 00:01: EC:CF: 03

                // skip header
                int currentByteNo = 6;

                // Grid overvoltage setpoint
                TwoByte grid_overvoltage_setpoint;
                grid_overvoltage_setpoint.Byte[0] = bytes[currentByteNo + 1]; // Grid overvoltage setpoint lsb
                grid_overvoltage_setpoint.Byte[1] = bytes[currentByteNo]; // Grid overvoltage setpoint msb

                // Quick and dirty check for package integrity is done, in order to avoid irratic sensor value updates
                // This effectively blocks out any erroneous sensor updates due to rx package corruption
                // Check if ETX = 3. If not (invalid package), ditch whole package, clear buffer and continue
                if (bytes[bytes.size() - 1] != 0x03) {
                    ESP_LOGI("deltainverter", "ETX check failure - NO sensor update! ETX: %i", bytes[bytes.size() - 1]);
                    bytes.clear();
                    continue;
                }
                // crc check
                unsigned char arr[bytes.size()];
                for (int i = 0; i < bytes.size(); i++) {
                    arr[i] = bytes[i];
                }
                if(calc_crc(arr + 1, arr + bytes.size() - 3) != bytes[bytes.size() - 2]) {
                    ESP_LOGE("deltainverter", "CRC check failure: calc_crc: %i - bytes_crc: %i", calc_crc(arr + 1, arr + bytes.size() - 3), bytes[bytes.size() - 2]);
                    bytes.clear();
                    continue;
                }
                //ESP_LOGI("deltainverter", "CRC check ok");

                // Log some data
                ESP_LOGI("deltainverter", "Grid Overvoltage Setpoint: %i V",
                         grid_overvoltage_setpoint.UInt16);
                // clear buffer
                bytes.clear();
            }

        }
    }

    typedef union {
        unsigned char Byte[2];
        int16_t Int16;
        uint16_t UInt16;
        unsigned char UChar;
        char Char;
    } TwoByte;

    unsigned int calc_crc(unsigned char *sop, unsigned char *eop) {
        unsigned int crc;
        unsigned char bit_count;
        unsigned char *char_ptr;
        char_ptr = sop;
        crc = 0x0000;                       //initialize to zero, not all 1's
        do {
            crc ^= ((*char_ptr) & 0x00ff);      //make sure only 8-bits get modified
            bit_count = 0;
            do {
                if (crc & 0x0001) {             //test before shifting
                    crc >>= 1;
                    crc ^= 0xA001;            //reflected version of poly:0x8005
                } else {
                    crc >>= 1;
                }
            } while (bit_count++ < 7);        //for every bit
        } while (char_ptr++ < eop);           //for every byte
        return crc;                         //return 16 bits of crc
    }
};
