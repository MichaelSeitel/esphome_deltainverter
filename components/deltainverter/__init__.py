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