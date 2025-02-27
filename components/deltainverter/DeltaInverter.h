#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome
{
    namespace deltainverter
    {
        class IDeltaInverterSensor
        {
            public:
                virtual ~IDeltaInverterSensor() = default;

                virtual void publish_val(const std::string& value) = 0;
                virtual void publish_invalid() = 0;
                virtual bool has_timed_out() = 0;
                virtual const uint8_t get_position() const = 0;
                virtual const uint8_t get_length() const = 0;
        };

        class DeltaInverterSensorBase : public IDeltaInverterSensor
        {
        private:
            uint32_t lastUpdate_{0};    // in milliseconds
            const uint32_t timeout_;    // in milliseconds
            uint8_t position_;
            uint8_t length_;
            std::string delta_name_;
        protected:
            bool check_value(const std::string& value);

            void reset_timeout_counter();
            bool has_timed_out() override;
        public:
            DeltaInverterSensorBase(std::string delta_name, int position, int length, int timeout_ms) :
                    delta_name_{std::move(delta_name)},
                    position_{static_cast<uint8_t>(position)},
                    length_{static_cast<uint8_t>(length)},
                    timeout_{static_cast<uint32_t>(timeout_ms)}
            {}
            
            const uint8_t get_position() const override { return position_; }
            const uint8_t get_length() const override { return length_; }
        };        

        class TelegramTrigger : public Trigger<>{};

        class DeltaInverter : public Component,
                              public uart::UARTDevice
        {
        private:
            /* data */
            std::array<uint8_t, 150> buffer_;
            uint16_t length_{0};    // used bytes in buffer_

            using SearchFcn = void(DeltaInverter::*)();
            SearchFcn search_{nullptr};

            std::map<std::string, IDeltaInverterSensor*> sensors_;
            std::vector<TelegramTrigger*> telegramTriggers_;
        protected:
            void reset();

            void search_start_of_telegram();
            void search_end_of_telegram();

            void skip_header();
            void parse_record();
            void end_of_telegram();

            uint16_t calc_crc();
        public:
            DeltaInverter();

            void setup() override
            {
                // nothing to do here
            }

            void loop() override;

            void register_sensor(IDeltaInverterSensor* sensor);
            
            void register_telegram_trigger(TelegramTrigger* trigger)
            {
                telegramTriggers_.push_back(trigger);
            }
        };

    } // namespace deltainverter
} // namespace esphome