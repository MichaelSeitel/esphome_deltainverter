import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.cpp_helpers import setup_entity
from esphome.components import uart
from esphome.const import CONF_ID, CONF_TRIGGER_ID
from esphome import automation

CODEOWNERS = ["@MichaelSeitel"]

DEPENDENCIES = ["uart"]

deltainverter_ns = cg.esphome_ns.namespace("deltainverter")
DeltaInverter = deltainverter_ns.class_("DeltaInverter", cg.Component, uart.UARTDevice)
MULTI_CONF = True


CONF_DELTA_NAME = "delta_name"
CONF_POSITION = "position"
CONF_LENGTH = "length"
CONF_ON_TELEGRAM = "on_telegram"

#Triggers
TelegramTrigger = deltainverter_ns.class_("TelegramTrigger", automation.Trigger.template())

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(DeltaInverter),
        cv.Optional(CONF_ON_TELEGRAM): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(TelegramTrigger),
            }
        ),
    }
).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    for conf in config.get(CONF_ON_TELEGRAM, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID])
        cg.add(var.register_telegram_trigger(trigger))
        await automation.build_automation(trigger, [], conf)