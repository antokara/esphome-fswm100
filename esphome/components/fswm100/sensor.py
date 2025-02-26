import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import ICON_EMPTY, UNIT_EMPTY

# CONF_MY_REQUIRED_KEY = "my_required_key"
# CONF_MY_OPTIONAL_KEY = "my_optional_key"

# CONFIG_SCHEMA = cv.Schema(
#     {
#         cv.Required(CONF_MY_REQUIRED_KEY): cv.string,
#         cv.Optional(CONF_MY_OPTIONAL_KEY, default=10): cv.int_,
#     }
# ).extend(cv.COMPONENT_SCHEMA)

water_flow_sensor_ns = cg.esphome_ns.namespace("water_flow_sensor")
WaterFlowSensor = water_flow_sensor_ns.class_(
    "WaterFlowSensor", sensor.Sensor, cg.PollingComponent
)

CONFIG_SCHEMA = sensor.sensor_schema(
    WaterFlowSensor,
    unit_of_measurement=UNIT_EMPTY,
    icon=ICON_EMPTY,
    accuracy_decimals=2,
).extend(cv.polling_component_schema("60000ms"))

# async def to_code(config):
#     var = cg.new_Pvariable(config[CONF_ID])
#     await cg.register_component(var, config)
#     cg.add(var.set_my_required_key(config[CONF_MY_REQUIRED_KEY]))


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
