from esphome import pins
import esphome.codegen as cg
from esphome.components import binary_sensor, sensor
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
)

AUTO_LOAD = ["sensor", "binary_sensor"]

# makes it required in config
# DEPENDENCIES = ["binary_sensor"]

# @see https://github.com/elupus/home-assistant/blob/ffc5f436eedbbc4920fe16b809681d83cfddb3af/homeassistant/const.py#L1045
GALLONS_PER_MINUTE = "gal/min"
UNIT_PSI = "psi"
CONF_PULSE = "pulse"

# CONF_MY_REQUIRED_KEY = "my_required_key"
CONF_GPIO_PIN_KEY = "gpio_pin"

fswm100_ns = cg.esphome_ns.namespace("fswm100")
FSWM100Component = fswm100_ns.class_("FSWM100", cg.Component)
PulseSensor = fswm100_ns.class_("PulseSensor", binary_sensor.BinarySensor)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(FSWM100Component),
        # Flow Sensor
        cv.Optional(CONF_FLOW): sensor.sensor_schema(
            unit_of_measurement=GALLONS_PER_MINUTE,
            icon=ICON_WATER,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLUME_FLOW_RATE,
        ),
        # Pulse Sensor
        cv.Optional(CONF_PULSE): binary_sensor.binary_sensor_schema(
            PulseSensor, icon=ICON_PULSE, device_class=DEVICE_CLASS_EMPTY
        ).extend(
            {
                cv.GenerateID(): cv.declare_id(PulseSensor),
                cv.Optional(
                    CONF_GPIO_PIN_KEY, default=5
                ): pins.gpio_input_pullup_pin_schema,
            }
        ),
        # Pressure Sensor
        cv.Optional(CONF_PRESSURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_PSI,
            icon=ICON_GAUGE,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_PRESSURE,
        ),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    # Pulse
    if pulse_config := config.get(CONF_PULSE):
        # Create an instance of the custom binary sensor class
        sens = cg.new_Pvariable(config[CONF_PULSE][CONF_ID])
        await binary_sensor.register_binary_sensor(sens, pulse_config)
        cg.add(var.set_pulse_sensor(sens))
        pulse_sensor_pin = await cg.gpio_pin_expression(
            config[CONF_PULSE][CONF_GPIO_PIN_KEY]
        )
        cg.add(var.set_pulse_sensor_gpio_pin(pulse_sensor_pin))

    # Flow
    if flow_config := config.get(CONF_FLOW):
        sens = await sensor.new_sensor(flow_config)
        cg.add(var.set_flow_sensor(sens))

    # Pressure
    if pressure_config := config.get(CONF_PRESSURE):
        sens = await sensor.new_sensor(pressure_config)
        cg.add(var.set_pressure_sensor(sens))
