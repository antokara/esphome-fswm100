from esphome import pins
import esphome.codegen as cg
from esphome.components import ads1115, binary_sensor, number, sensor, switch
from esphome.components.ads1115.sensor import GAIN, MUX, RESOLUTION, SAMPLERATE
import esphome.config_validation as cv
from esphome.const import (
    CONF_FILTERS,
    CONF_FLOW,
    CONF_GAIN,
    CONF_ID,
    CONF_MODE,
    CONF_MULTIPLEXER,
    CONF_NAME,
    CONF_PRESSURE,
    CONF_RESOLUTION,
    CONF_SAMPLE_RATE,
    DEVICE_CLASS_DURATION,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_PRESSURE,
    DEVICE_CLASS_VOLUME_FLOW_RATE,
    ENTITY_CATEGORY_CONFIG,
    ICON_GAUGE,
    ICON_PULSE,
    ICON_TIMELAPSE,
    ICON_WATER,
    UNIT_EMPTY,
    UNIT_SECOND,
)
from esphome.cpp_helpers import extract_registry_entry_config

#
# Make sure to autload the required components.
# Otherwise, the code will not compile and
# it will complain. For example:
#  fatal error: esphome/components/switch/switch.h: No such file or directory
#
AUTO_LOAD = ["sensor", "binary_sensor", "number", "switch"]

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
CONF_TEST = "test"
CONF_EFFECTIVE_NOISE_FLOOR = "effective_noise_floor"
CONF_MIN_DURATION = "min_duration"
CONF_MIN_VOLUME = "min_volume"
CONF_RATE_VOLUME = "rate_volume"
CONF_RATE_TIME = "rate_time"
CONF_PRESSURE_TEST = "pressure_test"
CONF_STATUS_LED = "status_led"
CONF_RED = "red"
CONF_GREEN = "green"
CONF_BLUE = "blue"

# icons
ICON_TIMER_PLAY_OUTLINE = "mdi:timer-play-outline"

# the nameppace for our component
fswm100_ns = cg.esphome_ns.namespace("fswm100")

# the C++ class names
FSWM100Component = fswm100_ns.class_("FSWM100", cg.Component)
PulseSensor = fswm100_ns.class_("PulseSensor", binary_sensor.BinarySensor)
PressureSensor = fswm100_ns.class_("PressureSensor", sensor.Sensor)
PressureTestSensor = fswm100_ns.class_("PressureTestSensor", sensor.Sensor)
FlowSensor = fswm100_ns.class_("FlowSensor", sensor.Sensor)
PressureSensorCalibration = fswm100_ns.class_(
    "PressureSensorCalibration", number.Number, cg.Component
)
PressureSensorTest = fswm100_ns.class_(
    "PressureSensorTest", switch.Switch, cg.Component
)
FlowSensorMinDuration = fswm100_ns.class_(
    "FlowSensorMinDuration", number.Number, cg.Component
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
                cv.Optional(CONF_EFFECTIVE_NOISE_FLOOR, default=0.05): cv.float_,
                cv.Optional(CONF_MIN_VOLUME, default=0.1): cv.float_,
                cv.Optional(CONF_RATE_TIME, default=60.0): cv.float_,
                cv.Optional(
                    CONF_MIN_DURATION,
                    default={
                        CONF_NAME: "Min. Flow Duration",
                    },
                ): number.number_schema(
                    FlowSensorMinDuration,
                    icon=ICON_TIMER_PLAY_OUTLINE,
                    unit_of_measurement=UNIT_SECOND,
                    device_class=DEVICE_CLASS_DURATION,
                    entity_category=ENTITY_CATEGORY_CONFIG,
                ).extend(
                    {
                        cv.Optional(CONF_MODE, default="BOX"): cv.enum(
                            number.NUMBER_MODES
                        ),
                    }
                ),
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
                cv.Optional(CONF_RATE_VOLUME, default=1): cv.float_,
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
                cv.Optional(CONF_EFFECTIVE_NOISE_FLOOR, default=0.05): cv.float_,
                cv.Optional(CONF_MIN_VOLTAGE, default=0.5): cv.float_,
                cv.Optional(CONF_MAX_VOLTAGE, default=4.5): cv.float_,
                cv.Optional(CONF_MIN_PRESSURE, default=0): cv.float_,
                cv.Optional(CONF_MAX_PRESSURE, default=100): cv.float_,
                cv.Optional(
                    CONF_CALIBRATION,
                    default={
                        CONF_NAME: "Pressure Calibration",
                    },
                ): number.number_schema(
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
                    }
                ),
                cv.Optional(
                    CONF_TEST,
                    default={
                        CONF_NAME: "Pressure Test",
                    },
                ): switch.switch_schema(
                    PressureSensorTest,
                    icon=ICON_TIMELAPSE,
                    entity_category=ENTITY_CATEGORY_CONFIG,
                ),
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
        # Pressure Test Sensor
        cv.Required(CONF_PRESSURE_TEST): sensor.sensor_schema(
            PressureTestSensor,
            icon=ICON_GAUGE,
            unit_of_measurement=UNIT_PSI,
            device_class=DEVICE_CLASS_PRESSURE,
        ).extend(
            {
                cv.GenerateID(): cv.declare_id(PressureTestSensor),
            }
        ),
        # Status LED
        cv.Required(CONF_STATUS_LED): cv.Schema(
            {
                cv.Required(CONF_RED): pins.gpio_output_pin_schema,
                cv.Required(CONF_GREEN): pins.gpio_output_pin_schema,
                cv.Required(CONF_BLUE): pins.gpio_output_pin_schema,
            }
        ),
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

    # status led configuration
    if status_led_config := config.get(CONF_STATUS_LED):
        cg.add(
            fswm100.set_status_led(
                await cg.gpio_pin_expression(status_led_config[CONF_RED]),
                await cg.gpio_pin_expression(status_led_config[CONF_GREEN]),
                await cg.gpio_pin_expression(status_led_config[CONF_BLUE]),
            )
        )

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
        cg.add(pulseSensor.setup(pulse_sensor_pin, pulse_config[CONF_RATE_VOLUME]))

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
                flow_config[CONF_EFFECTIVE_NOISE_FLOOR],
                flow_config[CONF_MIN_VOLUME],
                flow_config[CONF_RATE_TIME],
                flow_config[CONF_MULTIPLEXER],
                flow_config[CONF_GAIN],
                flow_config[CONF_SAMPLE_RATE],
                flow_config[CONF_RESOLUTION],
            )
        )
        # flow active duration configuration
        if flow_min_duration_config := flow_config.get(CONF_MIN_DURATION):
            flowSensorMinDuration = cg.new_Pvariable(
                flow_min_duration_config[CONF_ID],
                fswm100,
            )
            # register the sensor class instance
            await number.register_number(
                flowSensorMinDuration,
                flow_min_duration_config,
                min_value=1,
                max_value=120,
                step=1,
            )
            cg.add(flowSensorMinDuration.setup())
            # set the FlowSensorMinDuration class instance reference
            # to the FSWM100 class instance
            cg.add(fswm100.set_flow_sensor_min_duration(flowSensorMinDuration))

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
                pressure_config[CONF_EFFECTIVE_NOISE_FLOOR],
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
                pressure_calibration_config[CONF_ID],
                fswm100,
            )
            # register the sensor class instance
            await number.register_number(
                pressureSensorCalibration,
                pressure_calibration_config,
                min_value=0.1,
                max_value=2.0,
                step=0.01,
            )
            cg.add(pressureSensorCalibration.setup())
            # set the PressureSensorCalibration class instance reference
            # to the FSWM100 class instance
            cg.add(fswm100.set_pressure_sensor_calibration(pressureSensorCalibration))
        # pressure test configuration
        if pressure_test_config := pressure_config.get(CONF_TEST):
            pressureSensorTest = cg.new_Pvariable(
                pressure_test_config[CONF_ID],
                fswm100,
            )
            # register the sensor class instance
            await switch.register_switch(
                pressureSensorTest,
                pressure_test_config,
            )
            cg.add(pressureSensorTest.setup())
            # set the PressureSensorTest class instance reference
            # to the FSWM100 class instance
            cg.add(fswm100.set_pressure_sensor_test(pressureSensorTest))
    # Pressure Test Sensor
    if pressure_test_config := config.get(CONF_PRESSURE_TEST):
        # create an instance of our custom Sensor "pressureTestSensor" class
        # passing the FSWM100 class instance to its constructor
        pressureTestSensor = cg.new_Pvariable(pressure_test_config[CONF_ID], fswm100)
        # register the sensor class instance
        await sensor.register_sensor(pressureTestSensor, pressure_test_config)
        # set the PressureSensor class instance reference
        # to the FSWM100 class instance
        cg.add(fswm100.set_pressure_test_sensor(pressureTestSensor))
        # Generate the C++ lambda that will act as our filter factory
        # filters = await sensor.build_filters(pressure_test_config[CONF_FILTERS])
        filters = await build_filters(pressure_test_config[CONF_FILTERS])
        for conf in filters:
            print("filters:", conf)
        # This lambda function is the "factory". When called, it will execute
        # the code inside and return a new vector of filter objects.
        # e.g. []() -> std::vector<esphome::sensor::Filter *> { return { new esphome::sensor::SlidingWindowMovingAverageFilter(15, 5, 1), new esphome::sensor::OffsetFilter(10.0) }; }
        factory_lambda = cg.RawExpression(
            f"[]() -> std::vector<esphome::sensor::Filter *> {{ return {{ {', '.join(str(f) for f in filters)} }}; }}"
        )
        # setup the "pressureSensor" class instance, passing it the config
        cg.add(pressureTestSensor.setup(factory_lambda))


async def build_filters(config):
    """
    This function mimics the behavior of the internal esphome.sensor.build_filters.

    It takes a list of filter configurations (like from YAML), validates them
    against the live ESPHome filter registry, and returns a list of C++
    codegen objects ready to be used.
    """
    # This list will hold the generated C++ filter objects
    filters_cpp = []

    print("--- Starting Filter Build Process (using live registry) ---")

    # Iterate over each filter dictionary in the configuration list
    for i, conf in enumerate(config):
        print(f"\nProcessing filter #{i + 1}: {conf}")

        # A filter config must be a dictionary with:
        #   - one key (the filter name) and
        #   - the "type_id" key
        if not isinstance(conf, dict) or len(conf) != 2:
            raise cv.Invalid(
                f"Filter configuration must be a dictionary with one key. Invalid value: {conf}"
            )

        # The key is the name of the filter, e.g., "offset"
        filter_key = next(iter(conf))

        print(f"  - Will lookup the Filter type '{filter_key}'")

        # Look up the filter's information in the real registry
        if filter_key not in sensor.FILTER_REGISTRY:
            raise cv.Invalid(
                f"Filter with key '{filter_key}' not found in ESPHome's sensor.FILTER_REGISTRY."
            )

        print(f"  - Filter type looked-up: '{filter_key}'")

        # <esphome.util.RegistryEntry object at 0x7f680ef1ab40>
        filter = sensor.FILTER_REGISTRY[filter_key]
        coroutine_fun = filter.coroutine_fun
        schema = filter.schema
        name = filter.name
        builderConfig = conf[filter_key]
        print(f"  - builderConfig: '{builderConfig}'")
        type_id = conf["type_id"]
        type_id = "sensor_deltafilter_id_3"

        # builderConfig = extract_registry_entry_config(sensor.FILTER_REGISTRY, conf)

        print(f"  - filter_key: '{filter_key}'")
        print(f"  - schema: '{schema}'")
        print(f"  - name: '{name}'")
        print(f"  - type_id: '{type_id}'")
        print(f"  - coroutine_fun: '{coroutine_fun}'")
        print(
            f"  - coroutine_fun call: '{await coroutine_fun(builderConfig, type_id)}'"
        )

        # # Validate the configuration for this filter using its schema
        # # The schema handles both shorthand (e.g., `offset: 10.0`) and
        # # full dictionary notation.
        # try:
        #     validated_conf = schema(conf)
        #     # The schema returns the full config, so we need the value part
        #     validated_conf = validated_conf[filter_key]
        # except cv.Invalid as e:
        #     print(f"  - Validation Error: {e}")
        #     raise

        # print(f"  - Configuration validated: {validated_conf}")

        # # Generate the C++ code object for the filter
        # # cg.new_Pvariable creates the 'new ClassName(...)' C++ code.
        # # We need to pass the validated parameters to it.
        # template_ = await cg.templatable(validated_conf, cv.Schema({}), filter_class)

        # print(f"  - Generated C++ Code: {template_}")
        # filters_cpp.append(template_)

    print("\n--- Filter Build Process Finished ---")
    return filters_cpp
