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
    DEVICE_CLASS_PROBLEM,
    DEVICE_CLASS_VOLUME_FLOW_RATE,
    ENTITY_CATEGORY_CONFIG,
    ENTITY_CATEGORY_DIAGNOSTIC,
    ICON_GAUGE,
    ICON_PULSE,
    ICON_TIMELAPSE,
    ICON_WATER,
    UNIT_EMPTY,
    UNIT_SECOND,
)

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
CONF_MAX_VOLUME = "max_volume"
CONF_RATE_VOLUME = "rate_volume"
CONF_RATE_TIME = "rate_time"
CONF_PRESSURE_TEST = "pressure_test"
CONF_STATUS_LED = "status_led"
CONF_RED = "red"
CONF_GREEN = "green"
CONF_BLUE = "blue"

# diagnostic keys
DIAG_FLOW_PROBLEM = "flow_problem"
DIAG_PULSE_PROBLEM = "pulse_problem"
DIAG_PRESSURE_PROBLEM = "pressure_problem"

# icons
ICON_TIMER_PLAY_OUTLINE = "mdi:timer-play-outline"
ICON_ALERT_CIRCLE = "mdi:alert-circle"

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
FlowSensorProblemSensor = fswm100_ns.class_(
    "FlowSensorProblemSensor", binary_sensor.BinarySensor
)
PulseSensorProblemSensor = fswm100_ns.class_(
    "PulseSensorProblemSensor", binary_sensor.BinarySensor
)
PressureSensorProblemSensor = fswm100_ns.class_(
    "PressureSensorProblemSensor", binary_sensor.BinarySensor
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
                cv.Optional(CONF_MIN_VOLUME, default=0.07): cv.float_,
                cv.Optional(CONF_MAX_VOLUME, default=15.0): cv.float_,
                cv.Optional(CONF_MIN_VOLTAGE, default=2.0): cv.float_,
                cv.Optional(CONF_MAX_VOLTAGE, default=3.0): cv.float_,
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
        # Diagnostic
        cv.Optional(
            DIAG_FLOW_PROBLEM,
            default={
                CONF_NAME: "Flow Sensor Problem",
            },
        ): binary_sensor.binary_sensor_schema(
            FlowSensorProblemSensor,
            icon=ICON_ALERT_CIRCLE,
            device_class=DEVICE_CLASS_PROBLEM,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(
            DIAG_PULSE_PROBLEM,
            default={
                CONF_NAME: "Pulse Sensor Problem",
            },
        ): binary_sensor.binary_sensor_schema(
            PulseSensorProblemSensor,
            icon=ICON_ALERT_CIRCLE,
            device_class=DEVICE_CLASS_PROBLEM,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(
            DIAG_PRESSURE_PROBLEM,
            default={
                CONF_NAME: "Pressure Sensor Problem",
            },
        ): binary_sensor.binary_sensor_schema(
            PressureSensorProblemSensor,
            icon=ICON_ALERT_CIRCLE,
            device_class=DEVICE_CLASS_PROBLEM,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
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
        # Generate the C++ lambda that will act as our filter factory
        filters = await build_filters(flow_config[CONF_FILTERS])
        # This lambda function is the "factory". When called, it will execute
        # the code inside and return a new vector of filter objects.
        # e.g. []() -> std::vector<esphome::sensor::Filter *> { return { new esphome::sensor::SlidingWindowMovingAverageFilter(15, 5, 1), new esphome::sensor::OffsetFilter(10.0) }; }
        filters_factory = cg.RawExpression(
            f"[]() -> std::vector<esphome::sensor::Filter *> {{ return {{ {', '.join(str(f) for f in filters)} }}; }}"
        )
        print(f"  - Flow Sensor Filters Factory: {filters_factory}")
        # setup the "flowSensor" class instance, passing it the config
        cg.add(
            flowSensor.setup(
                filters_factory,
                flow_config[CONF_EFFECTIVE_NOISE_FLOOR],
                flow_config[CONF_MIN_VOLTAGE],
                flow_config[CONF_MAX_VOLTAGE],
                flow_config[CONF_MIN_VOLUME],
                flow_config[CONF_MAX_VOLUME],
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
        filters = await build_filters(pressure_test_config[CONF_FILTERS])
        # This lambda function is the "factory". When called, it will execute
        # the code inside and return a new vector of filter objects.
        # e.g. []() -> std::vector<esphome::sensor::Filter *> { return { new esphome::sensor::SlidingWindowMovingAverageFilter(15, 5, 1), new esphome::sensor::OffsetFilter(10.0) }; }
        filters_factory = cg.RawExpression(
            f"[]() -> std::vector<esphome::sensor::Filter *> {{ return {{ {', '.join(str(f) for f in filters)} }}; }}"
        )
        print(f"  - PressureTestSensor Filters Factory: {filters_factory}")
        # setup the "pressureSensor" class instance, passing it the config
        cg.add(pressureTestSensor.setup(filters_factory))
    # Diagnostic
    if diag_flow_problem_config := config.get(DIAG_FLOW_PROBLEM):
        # create an instance of our custom BinarySensor "FlowSensorProblemSensor" class
        # passing the FSWM100 class instance to its constructor
        flowProblemSensor = cg.new_Pvariable(diag_flow_problem_config[CONF_ID], fswm100)
        # register the sensor class instance
        await binary_sensor.register_binary_sensor(
            flowProblemSensor, diag_flow_problem_config
        )
        # set the FlowSensorProblemSensor class instance reference
        # to the FSWM100 class instance
        cg.add(fswm100.set_flow_problem_sensor(flowProblemSensor))
        # setup the "FlowSensorProblemSensor" class instance, passing it the config
        cg.add(flowProblemSensor.setup())
    if diag_pulse_problem_config := config.get(DIAG_PULSE_PROBLEM):
        # create an instance of our custom BinarySensor "PulseSensorProblemSensor" class
        # passing the FSWM100 class instance to its constructor
        pulseProblemSensor = cg.new_Pvariable(
            diag_pulse_problem_config[CONF_ID], fswm100
        )
        # register the sensor class instance
        await binary_sensor.register_binary_sensor(
            pulseProblemSensor, diag_pulse_problem_config
        )
        # set the PulseSensorProblemSensor class instance reference
        # to the FSWM100 class instance
        cg.add(fswm100.set_pulse_problem_sensor(pulseProblemSensor))
        # setup the "PulseSensorProblemSensor" class instance, passing it the config
        cg.add(pulseProblemSensor.setup())
    if diag_pressure_problem_config := config.get(DIAG_PRESSURE_PROBLEM):
        # create an instance of our custom BinarySensor "PressureSensorProblemSensor" class
        # passing the FSWM100 class instance to its constructor
        pressureProblemSensor = cg.new_Pvariable(
            diag_pressure_problem_config[CONF_ID], fswm100
        )
        # register the sensor class instance
        await binary_sensor.register_binary_sensor(
            pressureProblemSensor, diag_pressure_problem_config
        )
        # set the PressureSensorProblemSensor class instance reference
        # to the FSWM100 class instance
        cg.add(fswm100.set_pressure_problem_sensor(pressureProblemSensor))
        # setup the "PressureSensorProblemSensor" class instance, passing it the config
        cg.add(pressureProblemSensor.setup())


def filter_key_to_class_name(filter_key):
    """
    Converts a snake_case filter key into its PascalCase C++ class name.

    For example: 'sliding_window_moving_average' becomes 'SlidingWindowMovingAverageFilter'.
    """
    # Split the key by underscores, capitalize each part, and join them.
    pascal_case_name = "".join(part.capitalize() for part in filter_key.split("_"))
    return f"esphome::sensor::{pascal_case_name}Filter"


async def build_filters(config):
    """
    This function attempts to mimic the behavior of the internal esphome.sensor.build_filters.

    It takes a list of filter configurations (like from YAML), validates them
    against the live ESPHome filter registry, and returns a list of C++
    codegen objects ready to be used.
    """
    # This list will hold the generated C++ filter objects
    filters_cpp = []

    # Iterate over each filter dictionary in the configuration list
    for conf in config:
        # A filter config must be a dictionary with:
        #   - one key (the filter name) and
        #   - the "type_id" key
        if not isinstance(conf, dict) or len(conf) != 2:
            raise cv.Invalid(
                f"Filter configuration must be a dictionary with one key. Invalid value: {conf}"
            )

        # The key is the name of the filter, e.g., "offset"
        filter_key = next(iter(conf))
        # Look up the filter's information in the real registry
        if filter_key not in sensor.FILTER_REGISTRY:
            raise cv.Invalid(
                f"Filter with key '{filter_key}' not found in ESPHome's sensor.FILTER_REGISTRY."
            )
        filter = sensor.FILTER_REGISTRY[filter_key]
        schema = filter.schema
        filter_config = conf[filter_key]

        try:
            validated_conf = schema(filter_config)
        except cv.Invalid as e:
            print(f"  - Validation Error: {e}")
            raise

        # --- Manually create the C++ 'new' expression ---
        # Get the arguments for the C++ constructor
        if isinstance(validated_conf, dict):
            args = list(validated_conf.values())
        else:
            args = [validated_conf]

        # Format arguments for C++ (e.g., add quotes to strings)
        formatted_args = []
        for arg in args:
            # @see components/sensor/__init__.py:delta_filter_to_code
            if filter_key == "delta" and isinstance(arg, str):
                # convert the "absolute" or "percentage" string to a percentage boolean
                # meaning, pass true for "percentage" and false for "absolute"
                formatted_args.append("true" if arg == "percentage" else "false")
            elif filter_key in {"throttle_average", "heartbeat"}:
                # convert string to milliseconds
                formatted_args.append(
                    str(cv.time_period_str_unit(arg).total_milliseconds)
                )
            elif isinstance(arg, str) and not isinstance(
                arg, (cv.Lambda, cg.RawExpression)
            ):
                # For lambdas, we don't add quotes. For other strings, we do.
                if "return" in arg or arg.strip().startswith("return"):
                    formatted_args.append(str(arg))
                else:
                    formatted_args.append(f'"{arg}"')
            else:
                formatted_args.append(str(arg))

        # Construct the C++ code for creating a new instance
        cpp_code = (
            f"new {filter_key_to_class_name(filter_key)}({', '.join(formatted_args)})"
        )
        template_ = cg.RawExpression(cpp_code)
        filters_cpp.append(template_)
    return filters_cpp
