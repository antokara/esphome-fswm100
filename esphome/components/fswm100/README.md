# FSWM100

## setup

1. https://esphome.io/guides/contributing#contributing-to-esphome
1. https://esphome.io/components/external_components
1. https://esphome.io/guides/getting_started_command_line.html

## quick start

1. `docker pull ghcr.io/esphome/esphome`
1. `esphome wizard my-test-device.yaml`
1. `esphome config my-test-device.yaml`
1. `esphome compile my-test-device.yaml`
1. `esphome upload my-test-device.yaml`

## test config and quick compile

1. edit `tests/components/fswm100/common.yaml`, code, etc.
1. run `script/test_build_components -e compile -c fswm100`

## dev and flash

1. create `my-test-device.yaml` file at the root directory of the repo
1. enter python virtual `source venv/bin/activate`
1. ensure esphome is available `esphome version`
1. make changes to code/yaml/etc.
1. upload with `esphome run my-test-device.yaml`
    1. this will prompt after successful compile, how to flash (USB or OTA)
    1. to skip the prompt, use `esphome run my-test-device.yaml --device=/dev/ttyUSB0`
