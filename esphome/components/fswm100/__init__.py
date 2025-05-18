from esphome import pins
import esphome.codegen as cg
from esphome.components import ads1115, binary_sensor, number, sensor
from esphome.components.ads1115.sensor import GAIN, MUX, RESOLUTION, SAMPLERATE
import esphome.config_validation as cv
from esphome.const import (
    CONF_FLOW,
    CONF_GAIN,
    CONF_ICON,
    CONF_ID,
    CONF_INITIAL_VALUE,
    CONF_MAX_VALUE,
    CONF_MIN_VALUE,
    CONF_MODE,
    CONF_MULTIPLEXER,
    CONF_NAME,
    CONF_PRESSURE,
    CONF_RESOLUTION,
    CONF_SAMPLE_RATE,
    CONF_STEP,
    CONF_VALUE,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_PRESSURE,
    DEVICE_CLASS_VOLUME_FLOW_RATE,
    ENTITY_CATEGORY_CONFIG,
    ICON_GAUGE,
    ICON_PULSE,
    ICON_WATER,
    UNIT_EMPTY,
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
CONF_GPIO_PIN_KEY = "gpio_pin"
CONF_MIN_VOLTAGE = "min_voltage"
CONF_MAX_VOLTAGE = "max_voltage"
CONF_MIN_PRESSURE = "min_pressure"
CONF_MAX_PRESSURE = "max_pressure"
CONF_CALIBRATION = "calibration"

# the nameppace for our component
fswm100_ns = cg.esphome_ns.namespace("fswm100")

# the C++ class names
FSWM100Component = fswm100_ns.class_("FSWM100", cg.Component)
PulseSensor = fswm100_ns.class_("PulseSensor", binary_sensor.BinarySensor)
PressureSensor = fswm100_ns.class_("PressureSensor", sensor.Sensor)
FlowSensor = fswm100_ns.class_("FlowSensor", sensor.Sensor)
PressureSensorCalibration = fswm100_ns.class_(
    "PressureSensorCalibration", number.Number, cg.Component
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(FSWM100Component),
        # Require the ID of an existing ads1115 component
        cv.GenerateID(ads1115.CONF_ADS1115_ID): cv.use_id(ads1115.ADS1115Component),
        # Flow Sensor
        cv.Required(CONF_FLOW): sensor.sensor_schema(
            FlowSensor,
            icon=ICON_WATER,
            unit_of_measurement=GALLONS_PER_MINUTE,
            device_class=DEVICE_CLASS_VOLUME_FLOW_RATE,
        ).extend(
            {
                cv.GenerateID(): cv.declare_id(FlowSensor),
                # ads1115 properties
                cv.Required(CONF_MULTIPLEXER): cv.enum(MUX, upper=True, space="_"),
                cv.Optional(CONF_GAIN): cv.enum(GAIN, string=True),
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
            PressureSensor,
            icon=ICON_GAUGE,
            unit_of_measurement=UNIT_PSI,
            device_class=DEVICE_CLASS_PRESSURE,
        ).extend(
            {
                cv.GenerateID(): cv.declare_id(PressureSensor),
                cv.Optional(CONF_MIN_VOLTAGE, default=0.5): cv.float_,
                cv.Optional(CONF_MAX_VOLTAGE, default=4.5): cv.float_,
                cv.Optional(CONF_MIN_PRESSURE, default=0): cv.float_,
                cv.Optional(CONF_MAX_PRESSURE, default=100): cv.float_,
                cv.Optional(CONF_CALIBRATION, default=1.0): number.number_schema(
                    PressureSensorCalibration,
                    icon=ICON_GAUGE,
                    unit_of_measurement=UNIT_EMPTY,
                    device_class=DEVICE_CLASS_EMPTY,
                    entity_category=ENTITY_CATEGORY_CONFIG,
                ).extend(
                    {
                        cv.Optional(CONF_MODE, default="BOX"): cv.enum(
                            number.NUMBER_MODES
                        ),
                        cv.Optional(CONF_VALUE, default=1.0): cv.float_,
                    }
                ),
                # cv.Optional(CONF_CALIBRATION, default=1): number.NUMBER_SCHEMA.extend(
                # {
                #     cv.GenerateID(): cv.declare_id(PressureSensorCalibration), # ID for the number entity itself
                #     # cv.Optional(CONF_NAME): cv.string_strict,
                #     cv.Optional(CONF_MIN_VALUE): cv.float_,
                #     cv.Optional(CONF_MAX_VALUE): cv.float_,
                #     cv.Optional(CONF_STEP): cv.positive_float,
                #     cv.Optional(CONF_INITIAL_VALUE): cv.float_,
                #     cv.Optional(CONF_MODE, default="auto"): cv.enum(number.NUMBER_MODES, lower=True),
                #     cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=""): cv.string_strict,
                #     cv.Optional(CONF_ICON, default="mdi:ray-vertex"): cv.icon,
                #     cv.Optional(CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_CONFIG): cv.entity_category,
                # }
                # ADS1115 properties
                cv.Required(CONF_MULTIPLEXER): cv.enum(MUX, upper=True, space="_"),
                cv.Optional(CONF_GAIN): cv.enum(GAIN, string=True),
                cv.Optional(CONF_RESOLUTION, default="16_BITS"): cv.enum(
                    RESOLUTION, upper=True, space="_"
                ),
                cv.Optional(CONF_SAMPLE_RATE, default="860"): cv.enum(
                    SAMPLERATE, string=True
                ),
                cv.Optional(CONF_RESOLUTION, default="16_BITS"): cv.enum(
                    RESOLUTION, upper=True, space="_"
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
    # our Main (external component) "fswm100" configuration...
    # instantiate the FSWM100 class
    fswm100 = cg.new_Pvariable(config[CONF_ID])
    # register it as a component with ESPHome
    await cg.register_component(fswm100, config)

    # Get a C++ variable representing the shared ADS1115 component
    shared_ads1115 = await cg.get_variable(config[ads1115.CONF_ADS1115_ID])
    # Call the C++ method to set the ADS1115 parent
    cg.add(fswm100.set_ads1115(shared_ads1115))

    # pulse configuration
    if pulse_config := config.get(CONF_PULSE):
        # create an instance of our custom BinarySensor "PulseSensor" class
        # passing the FSWM100 class instance to its constructor
        pulseSensor = cg.new_Pvariable(pulse_config[CONF_ID], fswm100)
        # register the sensor class instance
        await binary_sensor.register_binary_sensor(pulseSensor, pulse_config)
        # set the PulseSensor class instance reference
        # to the FSWM100 class instance
        cg.add(fswm100.set_pulse_sensor(pulseSensor))
        # create a configuration object instance from the "pulse.gpio_pin" config
        pulse_sensor_pin = await cg.gpio_pin_expression(pulse_config[CONF_GPIO_PIN_KEY])
        # setup the "pulseSensor" class instance, passing it the config
        cg.add(pulseSensor.setup(pulse_sensor_pin))
        # TODO:
        # use:
        #   - count_volume (e.g. 1 pulse per unit)
        #   - count_frequency (e.g. 1 pulse every 60 seconds)
        #   - debounce   (e.g. 250 millis)

    # Flow
    if flow_config := config.get(CONF_FLOW):
        # create an instance of our custom Sensor "FlowSensor" class
        # passing the FSWM100 class instance to its constructor
        flowSensor = cg.new_Pvariable(flow_config[CONF_ID], fswm100)
        # register the sensor class instance
        await sensor.register_sensor(flowSensor, flow_config)
        # set the FlowSensor class instance reference
        # to the FSWM100 class instance
        cg.add(fswm100.set_flow_sensor(flowSensor))
        # setup the "flowSensor" class instance, passing it the config
        cg.add(
            flowSensor.setup(
                flow_config[CONF_MULTIPLEXER],
                flow_config[CONF_GAIN],
                flow_config[CONF_SAMPLE_RATE],
                flow_config[CONF_RESOLUTION],
            )
        )
        # the flow is to use the pulse+IR, to calculate itself
        # use:
        #   - min_flow_volume (e.g. 0.1 GPM)
        #   - TODO: maybe, make these dynamic OR self-calibrate
        #   - ir_count_delta_threshold (how much delta between readings needed to increase counts)
        #   - ir_delta_counts (how many counts are needed within the window, to consider active flow)
        #   - ir_window_duration (duration of active flow window)

    # Pressure
    if pressure_config := config.get(CONF_PRESSURE):
        # create an instance of our custom Sensor "PressureSensor" class
        # passing the FSWM100 class instance to its constructor
        pressureSensor = cg.new_Pvariable(pressure_config[CONF_ID], fswm100)
        # register the sensor class instance
        await sensor.register_sensor(pressureSensor, pressure_config)
        # set the PressureSensor class instance reference
        # to the FSWM100 class instance
        cg.add(fswm100.set_pressure_sensor(pressureSensor))
        # setup the "pressureSensor" class instance, passing it the config
        cg.add(
            pressureSensor.setup(
                pressure_config[CONF_MIN_VOLTAGE],
                pressure_config[CONF_MAX_VOLTAGE],
                pressure_config[CONF_MIN_PRESSURE],
                pressure_config[CONF_MAX_PRESSURE],
                # ADS1115 properties
                pressure_config[CONF_MULTIPLEXER],
                pressure_config[CONF_GAIN],
                pressure_config[CONF_SAMPLE_RATE],
                pressure_config[CONF_RESOLUTION],
            )
        )
        # pressure calibration configuration
        if pressure_calibration_config := pressure_config.get(CONF_CALIBRATION):
            pressureSensorCalibration = cg.new_Pvariable(
                pressure_calibration_config[CONF_ID]
            )
            # register the sensor class instance
            await number.register_number(
                pressureSensorCalibration,
                pressure_calibration_config,
                min_value=0.1,
                max_value=2.0,
                step=0.1,
            )
        # set the PressureSensor class instance reference
        # to the FSWM100 class instance
        # cg.add(fswm100.set_pressure_sensor(pressureSensor))

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
