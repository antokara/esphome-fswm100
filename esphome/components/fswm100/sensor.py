import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_FLOW,
    CONF_ID,
    CONF_PRESSURE,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_PRESSURE,
    DEVICE_CLASS_VOLUME_FLOW_RATE,
    ICON_GAUGE,
    ICON_PULSE,
    ICON_WATER,
    UNIT_EMPTY,
)

# @see https://github.com/elupus/home-assistant/blob/ffc5f436eedbbc4920fe16b809681d83cfddb3af/homeassistant/const.py#L1045
GALLONS_PER_MINUTE = "gal/min"
UNIT_PSI = "psi"
CONF_PULSE = "pulse"

# CONF_MY_REQUIRED_KEY = "my_required_key"
# CONF_MY_OPTIONAL_KEY = "my_optional_key"

# CONFIG_SCHEMA = cv.Schema(
#     {
#         cv.Required(CONF_MY_REQUIRED_KEY): cv.string,
#         cv.Optional(CONF_MY_OPTIONAL_KEY, default=10): cv.int_,
#     }
# ).extend(cv.COMPONENT_SCHEMA)

fswm100_ns = cg.esphome_ns.namespace("fswm100")
FSWM100Component = fswm100_ns.class_("FSWM100", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(FSWM100Component),
        cv.Optional(CONF_FLOW): sensor.sensor_schema(
            unit_of_measurement=GALLONS_PER_MINUTE,
            icon=ICON_WATER,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLUME_FLOW_RATE,
        ),
        cv.Optional(CONF_PULSE): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_PULSE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
        ),
        cv.Optional(CONF_PRESSURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_PSI,
            icon=ICON_GAUGE,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_PRESSURE,
        ),
    }
).extend(cv.COMPONENT_SCHEMA)

# async def to_code(config):
#     var = cg.new_Pvariable(config[CONF_ID])
#     await cg.register_component(var, config)
#     cg.add(var.set_my_required_key(config[CONF_MY_REQUIRED_KEY]))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if flow_config := config.get(CONF_FLOW):
        sens = await sensor.new_sensor(flow_config)
        cg.add(var.set_flow_sensor(sens))
    if pulse_config := config.get(CONF_PULSE):
        sens = await sensor.new_sensor(pulse_config)
        cg.add(var.set_pulse_sensor(sens))
    if pressure_config := config.get(CONF_PRESSURE):
        sens = await sensor.new_sensor(pressure_config)
        cg.add(var.set_pressure_sensor(sens))
