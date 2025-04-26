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

AUTO_LOAD = ["sensor", "binary_sensor", "number"]

# makes it required in config
DEPENDENCIES = ["i2c"]

# @see https://github.com/elupus/home-assistant/blob/ffc5f436eedbbc4920fe16b809681d83cfddb3af/homeassistant/const.py#L1045
GALLONS_PER_MINUTE = "gal/min"
UNIT_PSI = "psi"

# configuration keys
CONF_PULSE = "pulse"


# CONF_MY_REQUIRED_KEY = "my_required_key"
CONF_GPIO_PIN_KEY = "gpio_pin"

fswm100_ns = cg.esphome_ns.namespace("fswm100")
FSWM100Component = fswm100_ns.class_("FSWM100", cg.Component)
PulseSensor = fswm100_ns.class_("PulseSensor", binary_sensor.BinarySensor)
# I2CDevice = fswm100_ns.class_("I2C", i2c.I2CDevice)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(FSWM100Component),
        # Flow Sensor
        cv.Required(CONF_FLOW): sensor.sensor_schema(
            icon=ICON_WATER,
            unit_of_measurement=GALLONS_PER_MINUTE,
            device_class=DEVICE_CLASS_VOLUME_FLOW_RATE,
        ),
        # Pulse Sensor
        cv.Required(CONF_PULSE): binary_sensor.binary_sensor_schema(
            PulseSensor, icon=ICON_PULSE, device_class=DEVICE_CLASS_EMPTY
        ).extend(
            {
                cv.GenerateID(): cv.declare_id(PulseSensor),
                cv.Required(CONF_GPIO_PIN_KEY): pins.gpio_input_pin_schema,
            }
        ),
        # Pressure Sensor
        cv.Required(CONF_PRESSURE): sensor.sensor_schema(
            icon=ICON_GAUGE,
            unit_of_measurement=UNIT_PSI,
            device_class=DEVICE_CLASS_PRESSURE,
        ),
        # Water Meter (counter)
        # I2C Device at 0x48 address (default)
        # cv.Required(CONF_I2C): i2c.i2c_device_schema(0x48).extend(
        #     {
        #         cv.GenerateID(): cv.declare_id(I2CDevice),
        #         cv.Required(CONF_SDA): pin_with_input_and_output_support,
        #         cv.Required(CONF_SCL): pin_with_input_and_output_support,
        #         cv.Optional(CONF_SCAN, default=True): cv.boolean,
        #         cv.Optional(CONF_FREQUENCY, default="100kHz"): cv.frequency,
        #     }
        # ),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    # Pulse
    if pulse_config := config.get(CONF_PULSE):
        # create an instance of the custom binary sensor class
        sens = cg.new_Pvariable(pulse_config[CONF_ID])
        await binary_sensor.register_binary_sensor(sens, pulse_config)
        # set it to the main class
        cg.add(var.set_pulse_sensor(sens))
        # get the configuration and apply it
        pulse_sensor_pin = await cg.gpio_pin_expression(pulse_config[CONF_GPIO_PIN_KEY])
        cg.add(sens.setup(pulse_sensor_pin))

    # Flow
    if flow_config := config.get(CONF_FLOW):
        sens = await sensor.new_sensor(flow_config)
        cg.add(var.set_flow_sensor(sens))

    # Pressure
    if pressure_config := config.get(CONF_PRESSURE):
        sens = await sensor.new_sensor(pressure_config)
        cg.add(var.set_pressure_sensor(sens))

    # I2C Device
    # if i2c_config := config.get(CONF_I2C):
    #     dev = cg.new_Pvariable(i2c_config[CONF_ID])
    #     await i2c.register_i2c_device(dev, i2c_config)
    #     # TODO:
    #     # cg.add(var.set_pressure_sensor(dev))
