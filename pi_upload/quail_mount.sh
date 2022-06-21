

# configures if not configured
if [ ! -d "/sys/class/gpio/gpio21" ]; then
	echo "21" > /sys/class/gpio/export
fi

echo "out" > /sys/class/gpio/gpio21/direction


echo "0" > /sys/class/gpio/gpio21/value
sleep 0.1
echo "1" > /sys/class/gpio/gpio21/value

sleep 0.1

echo "0" > /sys/class/gpio/gpio21/value
sleep 0.1
echo "1" > /sys/class/gpio/gpio21/value
