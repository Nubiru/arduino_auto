/*
 * COCHE AUTÓNOMO CON RED NEURONAL - GABRIEL OSEMBERG
 * VERSIÓN PARA WOKWI CON LEDS (Simulación)
 * RED ORIGINAL [2,3,4] - PESOS ENTRENADOS
 *
 * NOTA: Se usan LEDs en lugar de motores DC porque Wokwi no tiene motores DC.
 * En hardware real, estos pines controlarían el driver L298N.
 *
 * Resultados del Entrenamiento:
 * - Error final: 0.022826
 * - Precisión: 100%
 * - Épocas: 40,000
 */

#include <Servo.h>

// ============================================================================
// CONFIGURACIÓN DE PINES
// ============================================================================

// LEDs que representan los 4 motores
#define LED_MOTOR1 2  // Motor delantero izquierdo
#define LED_MOTOR2 3  // Motor trasero izquierdo
#define LED_MOTOR3 4  // Motor trasero derecho
#define LED_MOTOR4 5  // Motor delantero derecho

// Sensor Ultrasónico
#define ECHO_PIN 11
#define TRIG_PIN 12

// Servo Motor
#define SERVO_PIN 9
Servo servoMotor;

// ============================================================================
// PARÁMETROS DE CONTROL
// ============================================================================

#define ANGULO_MIN 30        // Ángulo mínimo del servo
#define ANGULO_MAX 150       // Ángulo máximo del servo
#define DISTANCIA_MAXIMA 200 // Distancia máxima de detección (cm)

// Variables de control del servo
int grados_servo = ANGULO_MIN;
int incremento = 5;
unsigned long previousMillis = 0;
const long interval = 100;  // Intervalo de actualización servo (ms)

// Variables para tiempo de reacción adaptativo
int accionEnCurso = 0;
const int MULTIPLICADOR_REACCION = 50;

// ============================================================================
// ARQUITECTURA DE LA RED NEURONAL [2,3,4]
// ============================================================================

const int InputNodes = 3;   // 2 entradas + 1 BIAS
const int HiddenNodes = 4;  // 3 neuronas + 1 BIAS
const int OutputNodes = 4;  // 4 salidas (motores)

// Arrays para las capas
float InputLayer[InputNodes];
float HiddenLayer[HiddenNodes];
float OutputLayer[OutputNodes];

// ============================================================================
// MATRICES DE PESOS ENTRENADAS - GABRIEL OSEMBERG
// ============================================================================

float HiddenWeights[3][4] = {
    {0.09114312, -2.45302790, -45.66549094, -1.84135830},
    {24.32126115, -0.20990362, 0.16886829, -0.23475454},
    {26.36326205, 0.44933282, 0.16455262, -0.36798711}
};

float OutputWeights[4][4] = {
    {1.48225984, 1.49254769, 1.57294093, 3.19259587},
    {-3.10290811, -0.38778420, -3.00188098, 2.00064132},
    {5.43171528, -2.51351761, 1.04428182, -3.62118373},
    {-1.00216187, 1.58838169, 0.42982645, 4.99342611}
};

// ============================================================================
// SETUP - Inicialización
// ============================================================================

void setup() {
    // Inicializar comunicación serial
    Serial.begin(9600);
    Serial.println("========================================");
    Serial.println("COCHE AUTONOMO - GABRIEL OSEMBERG");
    Serial.println("Red Neuronal [2,3,4]");
    Serial.println("Precision: 100% | Error: 0.022826");
    Serial.println("========================================");
    Serial.println("SIMULACION WOKWI - LEDs representan motores");
    Serial.println("========================================");

    // Configurar pines de LEDs (motores)
    pinMode(LED_MOTOR1, OUTPUT);
    pinMode(LED_MOTOR2, OUTPUT);
    pinMode(LED_MOTOR3, OUTPUT);
    pinMode(LED_MOTOR4, OUTPUT);

    // Apagar todos los LEDs inicialmente
    digitalWrite(LED_MOTOR1, LOW);
    digitalWrite(LED_MOTOR2, LOW);
    digitalWrite(LED_MOTOR3, LOW);
    digitalWrite(LED_MOTOR4, LOW);

    // Configurar sensor ultrasónico
    pinMode(ECHO_PIN, INPUT);
    pinMode(TRIG_PIN, OUTPUT);

    // Configurar servo
    servoMotor.attach(SERVO_PIN);
    servoMotor.write(grados_servo);

    Serial.println("Sistema listo!");
    Serial.println();
    delay(2000);
}

// ============================================================================
// LOOP PRINCIPAL
// ============================================================================

void loop() {
    unsigned long currentMillis = millis();

    // Controlar movimiento del servo (barrido)
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        // Mover servo
        servoMotor.write(grados_servo);
        grados_servo += incremento;

        // Invertir dirección al llegar a los límites
        if (grados_servo >= ANGULO_MAX || grados_servo <= ANGULO_MIN) {
            incremento = -incremento;
        }

        // Ejecutar lógica de conducción
        conducir();
    }
}

// ============================================================================
// FUNCIÓN PRINCIPAL DE CONDUCCIÓN
// ============================================================================

void conducir() {
    // Decrementar contador de acción en curso
    if (accionEnCurso > 0) {
        accionEnCurso--;
        return;  // Continuar con la acción actual
    }

    // ========== CAPTURA DE DATOS DE SENSORES ==========

    // Leer distancia del sensor ultrasónico
    int distance = Distance_test();

    // Obtener posición actual del servo
    int angulo = grados_servo;

    // ========== NORMALIZACIÓN DE ENTRADAS ==========

    // Entrada 1: Distancia normalizada a rango [-1, 1]
    float entrada1 = ((-2.0 / DISTANCIA_MAXIMA) * float(distance)) + 1.0;
    entrada1 = constrain(entrada1, -1.0, 1.0);

    // Entrada 2: Posición angular normalizada a rango [-1, 1]
    float entrada2 = (2.0 / ANGULO_MAX) * float(angulo);
    entrada2 = constrain(entrada2, -1.0, 1.0);

    // ========== TIEMPO DE REACCIÓN ADAPTATIVO ==========
    accionEnCurso = int((entrada1 + 1.0) * MULTIPLICADOR_REACCION);

    // ========== PREPARAR ENTRADAS PARA LA RED ==========
    InputLayer[0] = entrada1;
    InputLayer[1] = entrada2;
    InputLayer[2] = 1.0;  // BIAS

    // ========== INFERENCIA DE LA RED NEURONAL ==========
    InputToOutput();

    // ========== PROCESAR SALIDAS ==========
    int out1 = round(abs(OutputLayer[0]));
    int out2 = round(abs(OutputLayer[1]));
    int out3 = round(abs(OutputLayer[2]));
    int out4 = round(abs(OutputLayer[3]));

    // ========== CONTROL DE LEDs (MOTORES) ==========
    digitalWrite(LED_MOTOR1, out1);
    digitalWrite(LED_MOTOR2, out2);
    digitalWrite(LED_MOTOR3, out3);
    digitalWrite(LED_MOTOR4, out4);

    // ========== DEBUG - Serial Monitor ==========
    Serial.print("Dist: ");
    Serial.print(distance);
    Serial.print(" cm | Ang: ");
    Serial.print(angulo);
    Serial.print("° | E1: ");
    Serial.print(entrada1, 2);
    Serial.print(" E2: ");
    Serial.print(entrada2, 2);
    Serial.print(" | Salidas: [");
    Serial.print(out1);
    Serial.print(",");
    Serial.print(out2);
    Serial.print(",");
    Serial.print(out3);
    Serial.print(",");
    Serial.print(out4);
    Serial.print("] | ");

    // Mostrar acción y patrón de LEDs
    if (out1 == 1 && out4 == 1 && out2 == 0 && out3 == 0) {
        Serial.println("▶ AVANZAR (LED1+LED4)");
    } else if (out2 == 1 && out3 == 1 && out1 == 0 && out4 == 0) {
        Serial.println("◀ RETROCEDER (LED2+LED3)");
    } else if (out2 == 1 && out4 == 1) {
        Serial.println("↻ GIRAR DERECHA (LED2+LED4)");
    } else if (out1 == 1 && out3 == 1) {
        Serial.println("↺ GIRAR IZQUIERDA (LED1+LED3)");
    } else {
        Serial.println("■ DETENIDO");
    }
}

// ============================================================================
// FORWARD PROPAGATION - Inferencia de la Red Neuronal
// ============================================================================

void InputToOutput() {
    // ========== CAPA OCULTA ==========
    for (int i = 0; i < HiddenNodes; i++) {
        float sum = 0.0;
        for (int j = 0; j < InputNodes; j++) {
            sum += InputLayer[j] * HiddenWeights[j][i];
        }
        HiddenLayer[i] = tanh(sum);
    }

    // ========== CAPA DE SALIDA ==========
    for (int i = 0; i < OutputNodes; i++) {
        float sum = 0.0;
        for (int j = 0; j < HiddenNodes; j++) {
            sum += HiddenLayer[j] * OutputWeights[j][i];
        }
        OutputLayer[i] = tanh(sum);
    }
}

// ============================================================================
// MEDICIÓN DE DISTANCIA CON SENSOR ULTRASÓNICO
// ============================================================================

int Distance_test() {
    // Generar pulso ultrasónico
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Medir tiempo de respuesta del eco
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);

    // Calcular distancia en centímetros
    int distance = duration * 0.034 / 2;

    // Limitar a rango válido
    if (distance == 0 || distance > DISTANCIA_MAXIMA) {
        distance = DISTANCIA_MAXIMA;
    }

    return distance;
}

/*
 * LEYENDA DE PATRONES DE LEDs:
 *
 * AVANZAR:         LED1=ON,  LED2=OFF, LED3=OFF, LED4=ON
 * RETROCEDER:      LED1=OFF, LED2=ON,  LED3=ON,  LED4=OFF
 * GIRAR DERECHA:   LED1=OFF, LED2=ON,  LED3=OFF, LED4=ON
 * GIRAR IZQUIERDA: LED1=ON,  LED2=OFF, LED3=ON,  LED4=OFF
 *
 * En hardware real con motores DC:
 * - LED1 = Motor delantero izquierdo adelante
 * - LED2 = Motor trasero izquierdo atrás
 * - LED3 = Motor trasero derecho atrás
 * - LED4 = Motor delantero derecho adelante
 */
