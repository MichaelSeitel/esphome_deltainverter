#pragma once

#include <string>

#include "esphome/components/sensor/sensor.h"

#include "../DeltaInverter.h"

namespace esphome
{
    namespace deltainverter
    {
        class DeltaInverterSensor : public DeltaInverterSensorBase, public sensor::Sensor, public Component
        {
        public:
            DeltaInverterSensor(std::string delta_name, int position, int length, int timeoutMS);
            void publish_val(const uint32_t& value) override;
            void publish_invalid() override;

        protected:

        private:
        };

    } // namespace deltainverter
} // namespace esphome