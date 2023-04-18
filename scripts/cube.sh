# run fromm project root `bash scripts/cube.sh`
Xvfb :10 -ac > /dev/null &
export DISPLAY=:10
java -jar /opt/stm32cubemx/STM32CubeMX -q scripts/.cube
killall Xvfb
