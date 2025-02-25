import esphome.config_validation as cv

# import esphome.codegen as cg

CONF_MY_REQUIRED_KEY = "my_required_key"
CONF_MY_OPTIONAL_KEY = "my_optional_key"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_MY_REQUIRED_KEY): cv.string,
        cv.Optional(CONF_MY_OPTIONAL_KEY, default=10): cv.int_,
    }
).extend(cv.COMPONENT_SCHEMA)


# async def to_code(config):
#     var = cg.new_Pvariable(config[CONF_ID])
#     await cg.register_component(var, config)

#     cg.add(var.set_my_required_key(config[CONF_MY_REQUIRED_KEY]))
