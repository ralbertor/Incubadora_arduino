# Proyecto Incubadora WiFi con Arduino
Este proyecto consiste en una incubadora controlada por WiFi utilizando un microcontrolador Arduino, sensores de temperatura y humedad, y una pantalla LCD. El sistema está conectado a un servidor MQTT para enviar los datos recolectados y recibir alarmas en caso de condiciones anormales. Además, la incubadora cuenta con un botón para silenciar las alarmas.

## Funcionalidades
- Medición de temperatura y humedad usando sensores DHT22.
- Visualización de datos en una pantalla LCD I2C.
- Envío de datos de temperatura y humedad a un servidor MQTT.
- Gestión de alarmas para temperatura y humedad, con LEDs y un buzzer.
- Botón para silenciar alarmas.
- Hardware Utilizado
- Arduino (compatible con ESP32/ESP8266 para conexión WiFi).
- Sensor de temperatura y humedad DHT22.
- Pantalla LCD I2C.
- Buzzer para alarmas auditivas.
- LEDs (rojo para temperatura, ámbar para humedad) como indicadores visuales de alarmas.
- Botón para silenciar las alarmas.
## Bibliotecas Utilizadas
Para este proyecto se utilizaron las siguientes bibliotecas de Arduino:

- `WiFi.h`: para la conexión WiFi.
- `PubSubClient.h`: para la comunicación MQTT.
- `DHT.h` y `DHT_U.h`: para la gestión del sensor DHT22.
- `Wire.h` y `LiquidCrystal_I2C.h`: para la pantalla LCD I2C.
- `pitches.h`: para el control de tonos del buzzer.
## Conexiones de Hardware
| Componente          | Pin Arduino |
|---------------------|-------------|
| Sensor DHT22 (Temp) | 16          |
| Sensor DHT22 (Hum)  | 17          |
| Buzzer              | 0           |
| LED Rojo (Temp)     | 2           |
| LED Ámbar (Hum)     | 4           |
| Botón Silenciador   | 12          |
| Pantalla LCD I2C    | SDA/SCL     |

## Configuración del Servidor MQTT
El proyecto se conecta al servidor MQTT `mqtt-dashboard.com` en el puerto 1883, con los siguientes tópicos:

- Publicación de datos: `testtopic/sensores1`
- Recepción de alarmas: `testtopic/alarmas`
## Código Principal
El código fuente completo del proyecto se encuentra en el archivo `incubadora.ino`.

## Estructura del Programa
1. Conexión a WiFi: El dispositivo se conecta a la red WiFi al iniciar.
2. Recolección de datos: Se leen los valores de los sensores de temperatura y humedad cada 3 segundos.
3. Publicación de datos: Los valores son enviados al servidor MQTT para su monitorización.
4. Gestión de alarmas: En caso de condiciones críticas (alta temperatura o humedad), se activan las alarmas (buzzer y LEDs).
5. Silenciador de alarmas: El botón permite activar o desactivar el sonido de la alarma mientras las luces indicadoras continúan encendidas.
## Instalación
1. Instala las bibliotecas necesarias en el IDE de Arduino.
2. Conecta los componentes de acuerdo con las conexiones descritas.
3. Sube el código al microcontrolador.
4. Asegúrate de configurar correctamente el nombre y la clave de la red WiFi en el código.
## Posibles Mejoras
- Implementación de una interfaz web para monitorizar los datos de la incubadora.
- Integración con servicios de notificación por email o SMS ante condiciones críticas.
- Optimización del código para reducir el consumo de energía.
