/*
 * COCHE AUTÓNOMO CON RED NEURONAL - VERSIÓN ORIGINAL [2,3,4]
 *
 * Proyecto: Arduino + Machine Learning para navegación autónoma
 * Arquitectura: [2 entradas, 3 neuronas ocultas, 4 salidas]
 *
 * Curso: Metodologías de Resolución de Problemas
 * Equipo: Gabriel Osemberg, Mariano Capella
 *
 * Basado en: https://www.aprendemachinelearning.com/programa-un-coche-arduino-con-inteligencia-artificial/
 */

#include <Servo.h>

// ============================================================================
// CONFIGURACIÓN DE PINES
// ============================================================================

// Motor Driver L298N
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5
#define ENA 6  // PWM para velocidad motor izquierdo
#define ENB 7  // PWM para velocidad motor derecho

// Sensor Ultrasónico
#define ECHO_PIN 11
#define TRIG_PIN 12

// Servo Motor
#define SERVO_PIN 9
Servo servoMotor;

// ============================================================================
// PARÁMETROS DE CONTROL
// ============================================================================

#define SPEED 150            // Velocidad base (0-255)
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
// MATRICES DE PESOS PRE-ENTRENADAS
// ============================================================================
// NOTA: Estos valores deben ser reemplazados por los pesos generados
// en el entrenamiento de Python/Colab

float HiddenWeights[InputNodes][HiddenNodes] = {
    // REEMPLAZAR CON PESOS REALES DESDE PYTHON
    // Valores temporales de ejemplo (no funcionales)
    {0.5, 0.3, -0.2, 1.0},
    {-0.4, 0.6, 0.8, 1.0},
    {1.0, 1.0, 1.0, 1.0}  // Fila del BIAS
};

float OutputWeights[HiddenNodes][OutputNodes] = {
    // REEMPLAZAR CON PESOS REALES DESDE PYTHON
    {0.7, -0.3, 0.1, 0.9},
    {-0.5, 0.8, -0.6, 0.2},
    {0.3, -0.7, 0.4, -0.1},
    {1.0, 1.0, 1.0, 1.0}  // Fila del BIAS
};

// ============================================================================
// SETUP - Inicialización
// ============================================================================

void setup() {
    // Inicializar comunicación serial
    Serial.begin(9600);
    Serial.println("Inicializando coche autonomo...");

    // Configurar pines de motores
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);

    // Configurar sensor ultrasónico
    pinMode(ECHO_PIN, INPUT);
    pinMode(TRIG_PIN, OUTPUT);

    // Configurar servo
    servoMotor.attach(SERVO_PIN);
    servoMotor.write(grados_servo);

    // Velocidad inicial máxima
    analogWrite(ENA, SPEED);
    analogWrite(ENB, SPEED);

    Serial.println("Sistema listo!");
    delay(1000);
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
    // -1: distancia máxima (sin peligro)
    //  0: obstáculo detectado
    // +1: obstáculo muy cerca (peligro)
    float entrada1 = ((-2.0 / DISTANCIA_MAXIMA) * float(distance)) + 1.0;
    entrada1 = constrain(entrada1, -1.0, 1.0);

    // Entrada 2: Posición angular normalizada a rango [-1, 1]
    // -1: derecha
    //  0: centro
    // +1: izquierda
    float entrada2 = (2.0 / ANGULO_MAX) * float(angulo);
    entrada2 = constrain(entrada2, -1.0, 1.0);

    // ========== TIEMPO DE REACCIÓN ADAPTATIVO ==========
    // Más tiempo de maniobra cuando el obstáculo está más cerca
    accionEnCurso = int((entrada1 + 1.0) * MULTIPLICADOR_REACCION);

    // ========== PREPARAR ENTRADAS PARA LA RED ==========
    InputLayer[0] = entrada1;
    InputLayer[1] = entrada2;
    InputLayer[2] = 1.0;  // BIAS

    // ========== INFERENCIA DE LA RED NEURONAL ==========
    InputToOutput();

    // ========== PROCESAR SALIDAS ==========
    // Convertir salidas continuas a binarias (0 o 1)
    int out1 = round(abs(OutputLayer[0]));
    int out2 = round(abs(OutputLayer[1]));
    int out3 = round(abs(OutputLayer[2]));
    int out4 = round(abs(OutputLayer[3]));

    // ========== AJUSTE DE VELOCIDAD PARA GIROS ==========
    int carSpeed = SPEED;

    // Si está girando, aumentar velocidad para vencer inercia
    if ((out1 + out3) == 2 || (out2 + out4) == 2) {
        carSpeed = SPEED * 2;
    }

    // Aplicar velocidad
    analogWrite(ENA, carSpeed);
    analogWrite(ENB, carSpeed);

    // ========== CONTROL DE MOTORES ==========
    digitalWrite(IN1, out1);
    digitalWrite(IN2, out2);
    digitalWrite(IN3, out3);
    digitalWrite(IN4, out4);

    // ========== DEBUG (OPCIONAL) ==========
    #ifdef DEBUG
    Serial.print("Dist: ");
    Serial.print(distance);
    Serial.print(" | Ang: ");
    Serial.print(angulo);
    Serial.print(" | E1: ");
    Serial.print(entrada1);
    Serial.print(" | E2: ");
    Serial.print(entrada2);
    Serial.print(" | Out: [");
    Serial.print(out1);
    Serial.print(",");
    Serial.print(out2);
    Serial.print(",");
    Serial.print(out3);
    Serial.print(",");
    Serial.print(out4);
    Serial.println("]");
    #endif
}

// ============================================================================
// FORWARD PROPAGATION - Inferencia de la Red Neuronal
// ============================================================================

void InputToOutput() {
    // ========== CAPA OCULTA ==========
    // Calcular activación de cada neurona oculta
    for (int i = 0; i < HiddenNodes; i++) {
        float sum = 0.0;

        // Sumar producto de entradas * pesos
        for (int j = 0; j < InputNodes; j++) {
            sum += InputLayer[j] * HiddenWeights[j][i];
        }

        // Aplicar función de activación tanh
        HiddenLayer[i] = tanh(sum);
    }

    // ========== CAPA DE SALIDA ==========
    // Calcular activación de cada neurona de salida
    for (int i = 0; i < OutputNodes; i++) {
        float sum = 0.0;

        // Sumar producto de capa oculta * pesos
        for (int j = 0; j < HiddenNodes; j++) {
            sum += HiddenLayer[j] * OutputWeights[j][i];
        }

        // Aplicar función de activación tanh
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
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // Timeout 30ms

    // Calcular distancia en centímetros
    // Velocidad del sonido: 340 m/s = 0.034 cm/μs
    // Dividir por 2 porque el sonido va y vuelve
    int distance = duration * 0.034 / 2;

    // Limitar a rango válido
    if (distance == 0 || distance > DISTANCIA_MAXIMA) {
        distance = DISTANCIA_MAXIMA;
    }

    return distance;
}

// ============================================================================
// FUNCIONES AUXILIARES DE CONTROL (OPCIONALES)
// ============================================================================

// Detener todos los motores
void stop() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

// Avanzar
void forward() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

// Retroceder
void backward() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

// Girar derecha
void turnRight() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

// Girar izquierda
void turnLeft() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

// ============================================================================
// NOTAS IMPORTANTES
// ============================================================================

/*
 * ANTES DE USAR ESTE CÓDIGO:
 *
 * 1. Entrenar la red neuronal en Python/Colab usando GUIA_TAREA_3_Entrenamiento.md
 * 2. Copiar las matrices HiddenWeights y OutputWeights generadas
 * 3. Pegar los valores reales en las secciones correspondientes arriba
 * 4. Verificar las conexiones de hardware según el diagrama
 * 5. Ajustar SPEED si los motores no tienen suficiente fuerza
 * 6. Probar en simulación Wokwi antes de cargar en Arduino físico
 *
 * COMPORTAMIENTO ESPERADO:
 * - El servo barre de izquierda a derecha continuamente
 * - El sensor ultrasónico mide distancias en cada posición
 * - La red neuronal decide la acción basándose en distancia y posición
 * - El coche avanza, retrocede o gira según las predicciones
 * - Aumenta velocidad durante giros para mejor maniobrabilidad
 *
 * PROBLEMAS COMUNES:
 * - Si no avanza: Aumentar SPEED
 * - Si giros débiles: El código ya duplica velocidad, verificar conexiones
 * - Si comportamiento errático: Verificar que los pesos sean correctos
 * - Si no detecta obstáculos: Verificar sensor ultrasónico y sus pines
 *
 * Para la versión expandida [4,6,5], ver:
 * arduino/codigo_modificado/TEMPLATE_coche_expandido.ino
 */
