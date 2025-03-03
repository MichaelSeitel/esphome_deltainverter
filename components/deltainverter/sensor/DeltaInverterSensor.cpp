#include <cmath>
#include <cstdlib>
#include <string>

#include "esphome/core/log.h"

#include "../DeltaInverter.h"
#include "DeltaInverterSensor.h"

namespace esphome
{
    namespace deltainverter
    {
        static const char* const TAG = "deltainverter_sensor";

        DeltaInverterSensor::DeltaInverterSensor(std::string delta_name, int position, int length, int timeoutMS) :
            DeltaInverterSensorBase{delta_name, position, length, timeoutMS}
        {}

        void DeltaInverterSensor::publish_val(const uint32_t& value)
        {
            if (check_value(value))
            {
                publish_state(value);
                reset_timeout_counter();
            }
            else
            {
                publish_invalid();
            }
        }

        void DeltaInverterSensor::publish_invalid()
        {
            publish_state(NAN);
        }
    } // namespace deltainverter
} // namespace esphome