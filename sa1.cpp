from machine import Pin, I2C
import ssd1306
import dht
import time

# ESP32 Pin assignment 
i2c = I2C(0, scl=Pin(22), sda=Pin(21))

oled_width = 128
oled_height = 64
oled = ssd1306.SSD1306_I2C(oled_width, oled_height, i2c)

dht_pin = Pin(2)  # DHT22 sensor connected to GPIO 2
d = dht.DHT22(dht_pin)

def read_dht_sensor():
    d.measure()
    temperature = d.temperature()
    humidity = d.humidity()
    return temperature, humidity

def display_temperature_and_humidity(temperature, humidity):
    oled.fill(0)
    oled.text("Temp: {} C".format(temperature), 10, 10)
    oled.text("Humidity: {} %".format(humidity), 10, 30)
    oled.show()

def check_temperature_threshold(temperature):
    TEMPERATURE_THRESHOLD = 25
    if temperature >= TEMPERATURE_THRESHOLD:
        funcao_temp_atingida()

def funcao_temp_atingida():
    print("Temperatura atingiu o limite!")
    # Coloque aqui o código que deve ser executado quando a temperatura atingir o limite
    # Por exemplo, você pode enviar uma notificação, acionar um alarme, etc.

def main_loop():
    while True:
        temperature, humidity = read_dht_sensor()
        display_temperature_and_humidity(temperature, humidity)
        check_temperature_threshold(temperature)
        time.sleep(2)

main_loop()
