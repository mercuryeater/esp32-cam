// turn on debug messages
#define VERBOSE
#include "EloquentSurveillance.h"

// Para la conexion y la soilicitud POST
#include <HTTPClient.h>
#include <WiFi.h>


const char* ssid = "QUEST";
const char* password =  "BobCorinne21";

String user = "TEST";
String pass = "12345";

/**
 * Instantiate motion detector
 */
EloquentSurveillance::Motion motion;


/**
 *
 */
void setup() {
    Serial.begin(115200);
    delay(3000);
    debug("INFO", "Init");

    /**
     * See CameraCaptureExample for more details
     */
    camera.aithinker();
    camera._240x240();
    camera.highQuality();

    /**
     * Configure motion detection
     *
     * > setMinChanges() accepts a number from 0 to 1 (percent) or an integer
     *   At least the given number of pixels must change from one frame to the next
     *   to trigger the motion.
     *   The following line translates to "Trigger motion if at least 10% of the pixels
     *   in the image changed value"
     */
    motion.setMinChanges(0.2);

    /**
     * > setMinPixelDiff() accepts an integer
     *   Each pixel value must differ at least of the given amount from one frame to the next
     *   to be considered as different.
     *   The following line translates to "Consider a pixel as changed if its value increased
     *   or decreased by 10 (out of 255)"
     */
    motion.setMinPixelDiff(10);

    /**
     * > setMinSizeDiff() accepts a number from 0 to 1 (percent) or an integer
     *   To speed up the detection, you can exit early if the image size is almost the same.
     *   This is an heuristic that says: "If two consecutive frames have a similar size, they
     *   probably have the same contents". This is by no means guaranteed, but can dramatically
     *   reduce the computation cost.
     *   The following line translates to "Check for motion if the filesize of the current image
     *   differs by more than 5% from the previous".
     *
     *   If you don't feel like this heuristic works for you, delete this line.
     */
    //motion.setMinSizeDiff(0.05);

    /**
     * Initialize the camera
     * If something goes wrong, print the error message
     */
    while (!camera.begin())
        debug("ERROR", camera.getErrorMessage());

    debug("SUCCESS", "Camera OK");

    WiFi.begin(ssid, password);

    Serial.print("Conectando...");
    while (WiFi.status() != WL_CONNECTED) { //Check for the connection
      delay(500);
      Serial.print(".");
    }

    Serial.print("Conectado con éxito, mi IP es: ");
    Serial.println(WiFi.localIP());     
}

/**
 *
 */
void loop() {
    /**
     * Try to capture a frame
     * If something goes wrong, print the error message
     */
    if (!camera.capture()) {
        debug("ERROR", camera.getErrorMessage());
        return;
    }

    /**
     * Look for motion.
     * In the `true` branch, you can handle a motion event.
     * For the moment, just print the processing time for motion detection.
     */
    if (!motion.update())
        return;

    if (motion.detect()) {
        debug("INFO", String("Motion detected in ") + motion.getExecutionTimeInMicros() + " us");

        if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status

        
          HTTPClient http;
          String datos_a_enviar = "user=" + user + "&pass=" + pass;

          http.begin("http://192.168.1.168:80/test");        //Indicamos el destino
          http.addHeader("Content-Type", "text/plain"); //Preparamos el header text/plain si solo vamos a enviar texto plano sin un paradigma llave:valor.

          //int codigo_respuesta = http.POST(datos_a_enviar);   //Enviamos el post pasándole, los datos que queremos enviar. (esta función nos devuelve un código que guardamos en un int)
          int codigo_respuesta = http.POST("JUST ANOTHER TEST FROM ESP32");

          if(codigo_respuesta>0){
          Serial.println("Código HTTP ► " + String(codigo_respuesta));   //Print return code
            
            if(codigo_respuesta == 200){
              String cuerpo_respuesta = http.getString();
              Serial.println("El servidor respondió ▼ ");
              Serial.println(cuerpo_respuesta);

            }

          }else{

          Serial.print("Error enviando POST, código: ");
          Serial.println(codigo_respuesta);

          }

          http.end();  //libero recursos

      }else{

          Serial.println("Error en la conexión WIFI");

      }
        delay(5000);
    }
    else if (!motion.isOk()) {
        /**
         * Something went wrong.
         * This part requires proper handling if you want to integrate it in your project
         * because you can reach this point for a number of reason.
         * For the moment, just print the error message
         */
        debug("ERROR", motion.getErrorMessage());
    }
}
