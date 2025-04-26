from esphome import pins
import esphome.codegen as cg
from esphome.components import binary_sensor, sensor
from esphome.components.ads1115.sensor import GAIN, MUX, RESOLUTION, SAMPLERATE
import esphome.config_validation as cv
from esphome.const import (
    CONF_FLOW,
    CONF_GAIN,
    CONF_ID,
    CONF_MULTIPLEXER,
    CONF_PRESSURE,
    CONF_RESOLUTION,
    CONF_SAMPLE_RATE,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_PRESSURE,
    DEVICE_CLASS_VOLUME_FLOW_RATE,
    ICON_GAUGE,
    ICON_PULSE,
    ICON_WATER,
)

AUTO_LOAD = ["sensor", "binary_sensor", "number"]

# makes them required in the config
# since those are shared components...
DEPENDENCIES = ["i2c", "ads1115"]

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

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(FSWM100Component),
        # Flow Sensor
        cv.Required(CONF_FLOW): sensor.sensor_schema(
            icon=ICON_WATER,
            unit_of_measurement=GALLONS_PER_MINUTE,
            device_class=DEVICE_CLASS_VOLUME_FLOW_RATE,
        ).extend(
            {
                cv.GenerateID(): cv.declare_id(sensor.Sensor),
                # ads1115 properties
                cv.Optional(CONF_MULTIPLEXER, default="A0_A1"): cv.enum(
                    MUX, upper=True
                ),
                cv.GenerateID(): cv.declare_id(sensor.Sensor),
                cv.Required(CONF_MULTIPLEXER): cv.enum(MUX, upper=True, space="_"),
                cv.Required(CONF_GAIN): cv.enum(GAIN, string=True),
                cv.Optional(CONF_RESOLUTION, default="16_BITS"): cv.enum(
                    RESOLUTION, upper=True, space="_"
                ),
                cv.Optional(CONF_SAMPLE_RATE, default="860"): cv.enum(
                    SAMPLERATE, string=True
                ),
            }
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
        ).extend(
            {
                cv.GenerateID(): cv.declare_id(sensor.Sensor),
                # ads1115 properties
                cv.Optional(CONF_MULTIPLEXER, default="A0_A1"): cv.enum(
                    MUX, upper=True
                ),
                cv.GenerateID(): cv.declare_id(sensor.Sensor),
                cv.Required(CONF_MULTIPLEXER): cv.enum(MUX, upper=True, space="_"),
                cv.Required(CONF_GAIN): cv.enum(GAIN, string=True),
                cv.Optional(CONF_RESOLUTION, default="16_BITS"): cv.enum(
                    RESOLUTION, upper=True, space="_"
                ),
                cv.Optional(CONF_SAMPLE_RATE, default="860"): cv.enum(
                    SAMPLERATE, string=True
                ),
            }
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
        # TODO:
        # use:
        #   - count_volume (e.g. 1 pulse per unit)
        #   - count_frequency (e.g. 1 pulse every 60 seconds)
        #   - debounce   (e.g. 250 millis)

    # Flow
    if flow_config := config.get(CONF_FLOW):
        sens = await sensor.new_sensor(flow_config)
        cg.add(var.set_flow_sensor(sens))
        # TODO: pass the ads1115 properties
        # the flow is to use the pulse+IR, to calculate itself
        # use:
        #   - min_flow_volume (e.g. 0.1 GPM)
        #   - TODO: maybe, make these dynamic OR self-calibrate
        #   - ir_count_delta_threshold (how much delta between readings needed to increase counts)
        #   - ir_delta_counts (how many counts are needed within the window, to consider active flow)
        #   - ir_window_duration (duration of active flow window)

    # Pressure
    if pressure_config := config.get(CONF_PRESSURE):
        sens = await sensor.new_sensor(pressure_config)
        cg.add(var.set_pressure_sensor(sens))
        # TODO: pass the ads1115 properties
        # use:
        #  - calibration multiplier
        #  - min pressure (e.g. 0 PSI)
        #  - max pressure (e.g. 100 PSI)
        #  - min voltage (e.g. 0.45)
        #  - max voltage (e.g. 5.00)
        #  - send_delta (use built-in sensor filter?)
        #  - send_frequency (use built-in sensor frequency?)
        #  - test_delta (for water leak test)
        #  - test_frequency (for water leak test)
