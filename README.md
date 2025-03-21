# ESPHome Delta Inverter Component

This repository adds support for Delta(R) inverters via RS485 to [ESPHome](https://esphome.io).

Thanks to [@mampfes](https://github.com/mampfes) for the esphome_obis_d0 repo which was a very good starting point.

## Example deltainverter.yaml
```yaml
esphome:
  name: "delta-inverter"
  
esp8266:
  board: nodemcu
  framework: 
    version: recommended

external_components:
  - source: github://MichaelSeitel/esphome_deltainverter

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password

  manual_ip:
    static_ip: 192.168.39.111
    gateway: 192.168.39.1
    subnet: 255.255.255.0
    dns1: 192.168.39.1

  # Enable fallback hotspot (captive portal) in case wifi connection fails
  ap:
    ssid: "Delta-Inverter Fallback Hotspot"
    password: "nqgK2EEFAcRI"

# Enable logging
logger:
#  level: VERBOSE
#  baud_rate: 0

# Enable Home Assistant API
api:
  encryption:
    key: ""

ota:
  - platform: esphome
    password: ""

captive_portal:


uart:
  id: rs485
  # tx_pin: 17 # Typical tx pin for hardware UART#2 on ESP32 modules
  # rx_pin: 16 # Typical rx pin for hardware UART#2 on ESP32 modules
  tx_pin: 4 # D2 ESP8266 tx pin for soft serial uart
  rx_pin: 5 # D1 ESP8266 rx pin for soft serial uart
#  tx_pin: "1" # GPIO1 = D10 - TX pin for hardware UART on ESP8266
#  rx_pin: "3" # GPIO3 = D9  - RX pin for hardware UART on ESP8266
  # Increase buffer size as total package we're sniffing is 262 bytes in total
  # Inverter reply: 6 bytes + Data: 167 bytes + ETX: 1 byte = 174 bytes
  rx_buffer_size: 256
  baud_rate: 19200
  parity: NONE
  stop_bits: 1
  debug: # Activate if you need to investigate package details in the log
    after:
      delimiter: [ 0x03 ]

deltainverter:
  id: my_delta_inverter
  uart_id: rs485
  on_telegram:
    then:
      - logger.log: "telegram received"

# With settings below, the requests are send every 1 seconds.
time:
  - platform: homeassistant
    id: esptime
  - platform: sntp
    on_time:
       # Request package every second from inverter at slave address: 0x01
      - seconds: /1
        hours: 5-22
        then:
          - uart.write: [0x02, 0x05, 0x01, 0x02, 0x60, 0x01, 0x85, 0xFC, 0x03]
#      - seconds: 0
#        minutes: 25
#        hours: 22
#        # Get Setup Data on boot
#        then:
#          - uart.write: [0x02, 0x05, 0x01, 0x02, 0x60, 0x02, 0xC5, 0xFD, 0x03]

sensor:
  - platform: uptime
    name: "Uptime sensor"   

  - platform: deltainverter
    name: "AC Voltage (Phase 1)"
    delta_name: "ac_v1"
    position: 54
    length: 2
    icon: mdi:current-ac
    unit_of_measurement: V
    accuracy_decimals: 1
    filters:
    - multiply:  0.1

  - platform: deltainverter
    name: "AC Current (Phase 1)"
    delta_name: "ac_c1"
    position: 56
    length: 2
    icon: mdi:current-ac
    unit_of_measurement: A
    accuracy_decimals: 2
    filters:
    - multiply: 0.01

  - platform: deltainverter
    name: "AC Power (Phase 1)"
    delta_name: "ac_p1"
    position: 58
    length: 2
    icon: mdi:current-ac
    unit_of_measurement: W
    accuracy_decimals: 0

  - platform: deltainverter
    name: "AC Frequency (Phase 1)"
    delta_name: "ac_f1"
    position: 60
    length: 2
    icon: mdi:sine-wave
    unit_of_measurement: Hz
    accuracy_decimals: 2
    filters:
    - multiply: 0.01

  - platform: deltainverter
    name: "AC Voltage (Phase 2)"
    delta_name: "ac_v2"
    position: 66
    length: 2
    icon: mdi:current-ac
    unit_of_measurement: V
    accuracy_decimals: 1
    filters:
    - multiply:  0.1

  - platform: deltainverter
    name: "AC Current (Phase 2)"
    delta_name: "ac_c2"
    position: 68
    length: 2
    icon: mdi:current-ac
    unit_of_measurement: A
    accuracy_decimals: 2
    filters:
    - multiply: 0.01

  - platform: deltainverter
    name: "AC Power (Phase 2)"
    delta_name: "ac_p2"
    position: 70
    length: 2
    icon: mdi:current-ac
    unit_of_measurement: W
    accuracy_decimals: 0

  - platform: deltainverter
    name: "AC Frequency (Phase 2)"
    delta_name: "ac_f2"
    position: 72
    length: 2
    icon: mdi:sine-wave
    unit_of_measurement: Hz
    accuracy_decimals: 2
    filters:
    - multiply: 0.01

  - platform: deltainverter
    name: "AC Voltage (Phase 3)"
    delta_name: "ac_v3"
    position: 78
    length: 2
    icon: mdi:current-ac
    unit_of_measurement: V
    accuracy_decimals: 1
    filters:
    - multiply:  0.1

  - platform: deltainverter
    name: "AC Current (Phase 3)"
    delta_name: "ac_c3"
    position: 80
    length: 2
    icon: mdi:current-ac
    unit_of_measurement: A
    accuracy_decimals: 2
    filters:
    - multiply: 0.01

  - platform: deltainverter
    name: "AC Power (Phase 3)"
    delta_name: "ac_p3"
    position: 82
    length: 2
    icon: mdi:current-ac
    unit_of_measurement: W
    accuracy_decimals: 0

  - platform: deltainverter
    name: "AC Frequency (Phase 3)"
    delta_name: "ac_f3"
    position: 84
    length: 2
    icon: mdi:sine-wave
    unit_of_measurement: Hz
    accuracy_decimals: 2
    filters:
    - multiply: 0.01

  - platform: deltainverter
    name: "Solar Voltage (Input 1)"
    delta_name: "solar_v1"
    position: 92
    length: 2
    icon: mdi:current-dc
    unit_of_measurement: V
    accuracy_decimals: 1
    filters:
    - multiply: 0.1

  - platform: deltainverter
    name: "Solar Current (Input 1)"
    delta_name: "solar_c1"
    position: 94
    length: 2
    icon: mdi:current-dc
    unit_of_measurement: A
    accuracy_decimals: 2
    filters:
    - multiply: 0.01

  - platform: deltainverter
    name: "Solar Power (Input 1)"
    delta_name: "solar_p1"
    position: 96
    length: 2
    icon: mdi:current-dc
    unit_of_measurement: W
    accuracy_decimals: 0

  - platform: deltainverter
    name: "Solar Voltage (Input 2)"
    delta_name: "solar_v2"
    position: 98
    length: 2
    icon: mdi:current-dc
    unit_of_measurement: V
    accuracy_decimals: 1
    filters:
    - multiply: 0.1

  - platform: deltainverter
    name: "Solar Current (Input 2)"
    delta_name: "solar_c2"
    position: 100
    length: 2
    icon: mdi:current-dc
    unit_of_measurement: A
    accuracy_decimals: 2
    filters:
    - multiply: 0.01

  - platform: deltainverter
    name: "Solar Power (Input 2)"
    delta_name: "solar_p2"
    position: 102
    length: 2
    icon: mdi:current-dc
    unit_of_measurement: W
    accuracy_decimals: 0

  - platform: deltainverter
    name: "Solar AC Power"
    delta_name: "ac_power"
    position: 104
    length: 2
    icon: mdi:solar-power
    device_class: power
    unit_of_measurement: W
    accuracy_decimals: 0

  - platform: deltainverter
    name: "Bus Voltage (+)"
    delta_name: "bus_vplus"
    position: 106
    length: 2
    icon: mdi:current-dc
    unit_of_measurement: V
    accuracy_decimals: 1
    filters:
    - multiply: 0.1

  - platform: deltainverter
    name: "Bus Voltage (-)"
    delta_name: "bus_vminus"
    position: 108
    length: 2
    icon: mdi:current-dc
    unit_of_measurement: V
    accuracy_decimals: 1
    filters:
    - multiply: 0.1

  - platform: deltainverter
    name: "Solar AC Energy Today"
    delta_name: "ac_energy_today"
    position: 110
    length: 4
    icon: mdi:lightning-bolt-circle
    device_class: energy
    unit_of_measurement: Wh
    accuracy_decimals: 0
    state_class: total_increasing

  - platform: deltainverter
    name: "Inverter Runtime Today"
    delta_name: "inv_runtime_today"
    position: 114
    length: 4
    icon: mdi:timer
    unit_of_measurement: s
    accuracy_decimals: 0
  
  - platform: deltainverter
    name: "Solar AC Energy Total"
    delta_name: "ac_energy_total"
    position: 118
    length: 4
    icon: mdi:lightning-bolt-circle
    device_class: energy
    unit_of_measurement: kWh
    accuracy_decimals: 0
    state_class: total_increasing

  - platform: deltainverter
    name: "Inverter Runtime Total"
    delta_name: "inv_runtime_total"
    position: 122
    length: 4
    icon: mdi:timer
    unit_of_measurement: s
    accuracy_decimals: 0
  
  - platform: deltainverter
    name: "Inverter Temperature"
    delta_name: "temp_rack"
    position: 126
    length: 2
    device_class: temperature
    unit_of_measurement: °C
    accuracy_decimals: 0

```
