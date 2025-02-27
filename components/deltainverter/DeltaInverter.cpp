#include "esphome/core/log.h"

#include "DeltaInverter.h"

namespace esphome
{
    namespace deltainverter
    {
        static const char* const TAG = "deltainverter";

        static constexpr uint8_t STX = 0x02;
        static constexpr uint8_t ETX = 0x03;
        static constexpr uint8_t ENQ = 0x05;
        static constexpr uint8_t ACK = 0x06;
        static constexpr uint8_t NAK = 0x15;

        static constexpr uint8_t LENGTHOFMESSAGE = 167;

        bool DeltaInverterSensorBase::check_value(const std::string& value)
        {
            return true;
        }

        void DeltaInverterSensorBase::reset_timeout_counter()
        {
            lastUpdate_ = millis();
        }

        void DeltaInverterSensorBase::has_timed_out()
        {
            auto now = millis();
            if(lastUpdate_ != 0 && now - lastUpdate_ > timeout_)
            {
                lastUpdate_ = 0;    // invalidate last update in case of a timeout
                return true;
            }
            return false;
        }

        DeltaInverter::DeltaInverter()
        {
            reset();
        }

        void DeltaInverter::register_sensor(IDeltaInverterSensor* sensor)
        {
            sensors_.emplace(sensor->get_position(), sensor);
        }

        void DeltaInverter::reset()
        {
            search_ = &DeltaInverter::search_start_of_telegram;
        }

        void DeltaInverter::loop()
        {
            while(available() > 0)
            {
                /* code */
                (this->search_)();
            }

            for(auto& it: sensors_)
            {
                if(it.second->has_timed_out())
                {
                    it.second->publish_invalid();
                }
            }
        }

        void DeltaInverter::search_start_of_telegram()
        {
            uint8_t* dest = &buffer_[length_++];
            (void)read_byte(dest);

            //check if this is the start of a telegram (STX)
            if(*dest == STX)
            {
                // start of telegram
                search_ = &DeltaInverter::search_end_of_telegram;
                length_ = 1;
            }
        }

        void DeltaInverter::search_end_of_telegram()
        {
            uint8_t* dest = &buffer_[length_++];
            (void)read_byte(dest);

            if(*dest == ETX)
            {
                parse_record();
                length_ = 0;
            }

            if(length_ == buffer_.size())
            {
                // abort before overflow
                reset();
                ESP_LOGD(TAG, "abort");
            }
        }

        void DeltaInverter::parse_record()
        {
            if(length_ != LENGTHOFMESSAGE)
            {
                ESP_LOGD(TAG, "invalid telegram length");
                return;
            }
            if(buffer_[0] != STX || buffer_[1] != ACK || buffer_[3] != 0xA0 || buffer_[length_] != ETX) {
                ESP_LOGD(TAG, "invalid telegram");
                return;
            }
            if(buffer_[4] == 0x60 && buffer_[5] == 0x01)
            {
                if(calc_crc() != buffer_[length_ - 2] << 8 + buffer_[length_ - 1])
                {
                    ESP_LOGE(TAG, "CRC check failure: calc_crc: %i - bytes_crc: %i", calc_crc(), buffer_[length_ - 2] << 8 + buffer_[length_ - 1]);
                    return;
                }
                // parse data
                for(auto& it: sensors_)
                {
                    switch (it.second->get_length())
                    {
                    case 1:
                        it.second->publish_val(buffer_[it.first]);
                        break;
                    case 2:
                        it.second->publish_val(uint16_t(buffer_[it.first] 
                            | buffer_[it.first+1] << 8));
                        break;
                    case 4:
                        it.second->publish_val(uint32_t(buffer_[it.first] << 24 
                            | buffer_[it.first+1] << 16
                            | buffer_[it.first+2] << 8
                            | buffer_[it.first+3]));
                        break;
                    default:
                        break;
                    }
                }
            }
            
        }

        void DeltaInverter::end_of_telegram()
        {
            for(auto* trigger : telegramTriggers_)
            {
                trigger->trigger();
            }
            reset();
        }

        uint16_t DeltaInverter::calc_crc() 
        {
            uint8_t *sop = buffer_[0];
            uint8_t *eop = buffer_[length_ - 3];
            uint16_t crc;
            uint8_t bit_count;
            uint8_t *char_ptr;
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
    } // namespace deltainverter
} // namespace esphome