import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID, CONF_TIMEOUT

from .. import CONF_DELTAINVERTER_ID, CONF_DELTA_NAME, CONF_POSITION, CONF_LENGTH, DeltaInverter, deltainverter_ns

AUTO_LOAD = ["deltainverter"]

DeltaInverterSensor = deltainverter_ns.class_(
    "DeltaInverterSensor", sensor.Sensor, cg.Component)


CONFIG_SCHEMA = sensor.sensor_schema().extend(
    {
        cv.GenerateID(): cv.declare_id(DeltaInverterSensor),
        cv.GenerateID(CONF_DELTAINVERTER_ID): cv.use_id(DeltaInverter),
        cv.Required(CONF_DELTA_NAME): cv.string,
        cv.Required(CONF_POSITION): cv.int_,
        cv.Required(CONF_LENGTH): cv.int_,
        cv.Optional(CONF_TIMEOUT, default="5s"): cv.time_period,
    }
)


async def to_code(config):
    var = cg.new_Pvariable(
        config[CONF_ID],
        config[CONF_DELTA_NAME],
        config[CONF_POSITION],
        config[CONF_LENGTH],
        config[CONF_TIMEOUT].total_milliseconds,
    )
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)
    deltainverter = await cg.get_variable(config[CONF_DELTAINVERTER_ID])
    cg.add(deltainverter.register_sensor(var))