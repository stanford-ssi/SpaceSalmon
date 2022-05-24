
# run to configure
# echo "21" >  /sys/class/gpio/export
# echo "out" > /sys/class/gpio/gpio21/directio

echo "0" > /sys/class/gpio/gpio21/value
sleep 0.1
echo "1" > /sys/class/gpio/gpio21/value

sleep 0.1

echo "0" > /sys/class/gpio/gpio21/value
sleep 0.1
echo "1" > /sys/class/gpio/gpio21/value
