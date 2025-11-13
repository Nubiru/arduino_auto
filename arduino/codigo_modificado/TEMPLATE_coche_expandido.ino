/*
 * COCHE AUTÓNOMO CON RED NEURONAL - VERSIÓN EXPANDIDA [4,6,5]
 *
 * Proyecto: Arduino + Machine Learning para navegación autónoma
 * Arquitectura: [4 entradas, 6 neuronas ocultas, 5 salidas]
 *
 * MODIFICACIONES:
 * + 2 nuevas entradas (sensores adicionales)
 * + 1 nueva salida (actuador adicional)
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
// NUEVOS PINES PARA SENSORES/ACTUADORES ADICIONALES
// ============================================================================

// OPCIÓN A: Velocidad + Sensor IR Borde + Buzzer (GABRIEL)
#define PIN_IR_BORDE A2      // Sensor infrarrojo de borde
#define PIN_BUZZER 10        // Buzzer de alerta

// OPCIÓN B: Luz LDR + Segundo Ultrasónico + LED Estado (MARIANO)
// #define PIN_LDR A3           // Sensor de luz ambiental
// #define PIN_ECHO2 A4         // Segundo sensor ultrasónico (echo)
// #define PIN_TRIG2 A5         // Segundo sensor ultrasónico (trigger)
// #define PIN_LED_ESTADO 10    // LED indicador de estado

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
const long interval = 100;

// Variables para tiempo de reacción adaptativo
int accionEnCurso = 0;
const int MULTIPLICADOR_REACCION = 50;

// Variables para nuevas entradas
float velocidadActual = 0.0;  // Para tracking de velocidad (-1 a 1)
unsigned long ultimoTiempoMovimiento = 0;
enum Accion { DETENIDO, AVANZANDO, RETROCEDIENDO, GIRANDO };
Accion ultimaAccion = DETENIDO;

// ============================================================================
// ARQUITECTURA DE LA RED NEURONAL [4,6,5]
// ============================================================================

const int InputNodes = 5;   // 4 entradas + 1 BIAS
const int HiddenNodes = 7;  // 6 neuronas + 1 BIAS
const int OutputNodes = 5;  // 5 salidas (4 motores + 1 actuador nuevo)

// Arrays para las capas
float InputLayer[InputNodes];
float HiddenLayer[HiddenNodes];
float OutputLayer[OutputNodes];

// ============================================================================
// MATRICES DE PESOS PRE-ENTRENADAS [4,6,5]
// ============================================================================
// NOTA: Estos valores DEBEN ser reemplazados por los pesos generados
// en el entrenamiento de Python/Colab con la arquitectura [4,6,5]

float HiddenWeights[InputNodes][HiddenNodes] = {
    // REEMPLAZAR CON PESOS REALES DESDE PYTHON
    // Dimensión: 5 filas (4 entradas + 1 BIAS) x 7 columnas (6 neuronas + 1 BIAS)
    {0.5, 0.3, -0.2, 0.7, -0.4, 0.6, 1.0},
    {-0.4, 0.6, 0.8, -0.3, 0.5, -0.7, 1.0},
    {0.3, -0.5, 0.4, 0.6, -0.2, 0.8, 1.0},
    {-0.6, 0.7, -0.3, 0.4, 0.5, -0.4, 1.0},
    {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}  // Fila del BIAS
};

float OutputWeights[HiddenNodes][OutputNodes] = {
    // REEMPLAZAR CON PESOS REALES DESDE PYTHON
    // Dimensión: 7 filas (6 neuronas + 1 BIAS) x 5 columnas (5 salidas)
    {0.7, -0.3, 0.1, 0.9, -0.5},
    {-0.5, 0.8, -0.6, 0.2, 0.4},
    {0.3, -0.7, 0.4, -0.1, 0.6},
    {0.6, 0.2, -0.8, 0.5, -0.3},
    {-0.4, 0.5, 0.3, -0.7, 0.8},
    {0.8, -0.2, 0.6, 0.4, -0.6},
    {1.0, 1.0, 1.0, 1.0, 1.0}  // Fila del BIAS
};

// ============================================================================
// SETUP - Inicialización
// ============================================================================

void setup() {
    // Inicializar comunicación serial
    Serial.begin(9600);
    Serial.println("Inicializando coche autonomo EXPANDIDO [4,6,5]...");

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

    // ========== CONFIGURAR NUEVOS PINES ==========

    // OPCIÓN A: Gabriel (IR Borde + Buzzer)
    pinMode(PIN_IR_BORDE, INPUT);
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, LOW);

    // OPCIÓN B: Mariano (LDR + LED Estado)
    // pinMode(PIN_LDR, INPUT);
    // pinMode(PIN_LED_ESTADO, OUTPUT);
    // digitalWrite(PIN_LED_ESTADO, LOW);

    // Velocidad inicial
    analogWrite(ENA, SPEED);
    analogWrite(ENB, SPEED);

    Serial.println("Sistema expandido listo!");
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

        // Actualizar estimación de velocidad
        actualizarVelocidad();

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
        return;
    }

    // ========== CAPTURA DE DATOS DE SENSORES ==========

    // Sensores originales
    int distance = Distance_test();
    int angulo = grados_servo;

    // ========== NORMALIZACIÓN DE ENTRADAS ORIGINALES ==========

    // Entrada 1: Distancia normalizada a rango [-1, 1]
    float entrada1 = ((-2.0 / DISTANCIA_MAXIMA) * float(distance)) + 1.0;
    entrada1 = constrain(entrada1, -1.0, 1.0);

    // Entrada 2: Posición angular normalizada a rango [-1, 1]
    float entrada2 = (2.0 / ANGULO_MAX) * float(angulo);
    entrada2 = constrain(entrada2, -1.0, 1.0);

    // ========== NUEVAS ENTRADAS ==========

    // Entrada 3: Velocidad estimada (ya normalizada -1 a 1)
    float entrada3 = velocidadActual;

    // Entrada 4: Sensor adicional (elegir según implementación)

    // OPCIÓN A: Sensor IR de borde
    float entrada4 = leerSensorBorde();

    // OPCIÓN B: Sensor de luz LDR
    // float entrada4 = leerLuzAmbiental();

    // OPCIÓN C: Segundo sensor ultrasónico
    // float entrada4 = leerSegundoUltrasonico();

    // ========== TIEMPO DE REACCIÓN ADAPTATIVO ==========
    accionEnCurso = int((entrada1 + 1.0) * MULTIPLICADOR_REACCION);

    // ========== PREPARAR ENTRADAS PARA LA RED ==========
    InputLayer[0] = entrada1;
    InputLayer[1] = entrada2;
    InputLayer[2] = entrada3;  // Nueva entrada 1
    InputLayer[3] = entrada4;  // Nueva entrada 2
    InputLayer[4] = 1.0;       // BIAS

    // ========== INFERENCIA DE LA RED NEURONAL ==========
    InputToOutput();

    // ========== PROCESAR SALIDAS ==========
    // Salidas para motores (0-3)
    int out1 = round(abs(OutputLayer[0]));
    int out2 = round(abs(OutputLayer[1]));
    int out3 = round(abs(OutputLayer[2]));
    int out4 = round(abs(OutputLayer[3]));

    // Nueva salida (4)
    int out5 = round(abs(OutputLayer[4]));

    // ========== AJUSTE DE VELOCIDAD PARA GIROS ==========
    int carSpeed = SPEED;
    if ((out1 + out3) == 2 || (out2 + out4) == 2) {
        carSpeed = SPEED * 2;
        ultimaAccion = GIRANDO;
    } else if (out1 == 1 && out4 == 1) {
        ultimaAccion = AVANZANDO;
    } else if (out2 == 1 && out3 == 1) {
        ultimaAccion = RETROCEDIENDO;
    } else {
        ultimaAccion = DETENIDO;
    }

    // Aplicar velocidad
    analogWrite(ENA, carSpeed);
    analogWrite(ENB, carSpeed);

    // ========== CONTROL DE MOTORES ==========
    digitalWrite(IN1, out1);
    digitalWrite(IN2, out2);
    digitalWrite(IN3, out3);
    digitalWrite(IN4, out4);

    // ========== CONTROL DE NUEVO ACTUADOR ==========

    // OPCIÓN A: Buzzer
    controlarBuzzer(out5);

    // OPCIÓN B: LED de estado
    // controlarLED(out5);

    // ========== DEBUG (OPCIONAL) ==========
    #ifdef DEBUG
    Serial.print("E1:");
    Serial.print(entrada1, 2);
    Serial.print(" E2:");
    Serial.print(entrada2, 2);
    Serial.print(" E3:");
    Serial.print(entrada3, 2);
    Serial.print(" E4:");
    Serial.print(entrada4, 2);
    Serial.print(" | Out:[");
    Serial.print(out1);
    Serial.print(",");
    Serial.print(out2);
    Serial.print(",");
    Serial.print(out3);
    Serial.print(",");
    Serial.print(out4);
    Serial.print(",");
    Serial.print(out5);
    Serial.println("]");
    #endif
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
// FUNCIONES PARA LEER NUEVOS SENSORES
// ============================================================================

// Actualizar estimación de velocidad basada en última acción
void actualizarVelocidad() {
    switch (ultimaAccion) {
        case AVANZANDO:
            velocidadActual = min(velocidadActual + 0.2, 1.0);
            break;
        case RETROCEDIENDO:
            velocidadActual = max(velocidadActual - 0.2, -1.0);
            break;
        case GIRANDO:
            // Mantener velocidad actual
            break;
        case DETENIDO:
            // Decelerar gradualmente
            velocidadActual *= 0.8;
            if (abs(velocidadActual) < 0.1) velocidadActual = 0.0;
            break;
    }
}

// Leer sensor IR de borde
float leerSensorBorde() {
    int valorIR = analogRead(PIN_IR_BORDE);

    // Mapear valores:
    // IR alto (>700) = superficie oscura/vacío/borde = -1 (PELIGRO)
    // IR bajo (<300) = superficie clara/sólida = 1 (SEGURO)

    if (valorIR > 700) {
        return -1.0;  // PELIGRO: Borde detectado
    } else if (valorIR < 300) {
        return 1.0;   // SEGURO: Superficie sólida
    } else {
        // Mapear rango intermedio
        return map(valorIR, 300, 700, 100, -100) / 100.0;
    }
}

// Leer sensor de luz LDR (OPCIONAL - para Mariano)
float leerLuzAmbiental() {
    int valorLDR = analogRead(PIN_LDR);

    // Mapear a rango [-1, 1]
    // -1 = oscuro (valor bajo)
    // +1 = brillante (valor alto)
    return map(valorLDR, 0, 1023, -100, 100) / 100.0;
}

// Leer segundo sensor ultrasónico lateral (OPCIONAL - para Mariano)
float leerSegundoUltrasonico() {
    // Implementación similar a Distance_test()
    // pero usando PIN_TRIG2 y PIN_ECHO2

    // digitalWrite(PIN_TRIG2, LOW);
    // delayMicroseconds(2);
    // digitalWrite(PIN_TRIG2, HIGH);
    // delayMicroseconds(10);
    // digitalWrite(PIN_TRIG2, LOW);

    // long duration = pulseIn(PIN_ECHO2, HIGH, 30000);
    // int distance = duration * 0.034 / 2;

    // if (distance == 0 || distance > DISTANCIA_MAXIMA) {
    //     distance = DISTANCIA_MAXIMA;
    // }

    // // Normalizar a [-1, 1]
    // float normalizado = ((-2.0 / DISTANCIA_MAXIMA) * float(distance)) + 1.0;
    // return constrain(normalizado, -1.0, 1.0);

    return 0.0;  // Placeholder
}

// ============================================================================
// FUNCIONES PARA CONTROLAR NUEVOS ACTUADORES
// ============================================================================

// Controlar buzzer
void controlarBuzzer(int estado) {
    if (estado == 1) {
        tone(PIN_BUZZER, 1000);  // Tono de 1000 Hz (alerta)
    } else {
        noTone(PIN_BUZZER);
    }
}

// Controlar LED de estado (OPCIONAL - para Mariano)
void controlarLED(int estado) {
    // digitalWrite(PIN_LED_ESTADO, estado);

    // O con PWM para intensidad variable:
    // analogWrite(PIN_LED_ESTADO, estado * 255);
}

// ============================================================================
// MEDICIÓN DE DISTANCIA CON SENSOR ULTRASÓNICO (ORIGINAL)
// ============================================================================

int Distance_test() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    int distance = duration * 0.034 / 2;

    if (distance == 0 || distance > DISTANCIA_MAXIMA) {
        distance = DISTANCIA_MAXIMA;
    }

    return distance;
}

// ============================================================================
// NOTAS IMPORTANTES PARA VERSIÓN EXPANDIDA
// ============================================================================

/*
 * PASOS PARA USAR ESTE CÓDIGO:
 *
 * 1. COMPLETAR TAREA 3: Entrenar red original [2,3,4]
 * 2. COMPLETAR TAREA 4:
 *    a. Elegir tus 2 nuevos sensores y 1 nuevo actuador
 *    b. Diseñar tabla de verdad expandida (15-20 escenarios)
 *    c. Entrenar red [4,6,5] en Colab siguiendo GUIA_TAREA_4_Expansion.md
 *    d. Copiar matrices de pesos generadas
 * 3. MODIFICAR ESTE CÓDIGO:
 *    a. Descomentar/habilitar pines de tus sensores/actuadores elegidos
 *    b. Pegar pesos reales en HiddenWeights y OutputWeights
 *    c. Activar la función correcta para leer entrada4
 *    d. Activar la función correcta para controlar salida5
 * 4. VERIFICAR:
 *    a. Todas las conexiones de hardware
 *    b. Que las dimensiones de los arrays sean correctas
 *    c. Que las funciones de normalización devuelvan valores en [-1, 1]
 * 5. PROBAR:
 *    a. Primero en simulación Wokwi
 *    b. Luego en hardware físico (si disponible)
 *
 * DIFERENCIAS CON VERSIÓN ORIGINAL:
 * - InputNodes: 3 → 5
 * - HiddenNodes: 4 → 7
 * - OutputNodes: 4 → 5
 * - Matrices de pesos más grandes
 * - 2 nuevas entradas normalizadas
 * - 1 nueva salida controlada
 *
 * COMPORTAMIENTO ESPERADO:
 * - Igual que la versión original, PERO:
 * - Toma en cuenta nuevos sensores para decisiones
 * - Activa nuevo actuador en situaciones específicas
 * - Ejemplo: Buzzer suena cuando detecta borde O obstáculo muy cerca
 * - Ejemplo: LED se enciende en modo de precaución (oscuridad, obstáculo lateral)
 *
 * PROBLEMAS COMUNES:
 * - "Not enough memory": Usar Arduino Mega o optimizar código
 * - Comportamiento inesperado: Verificar pesos y normalización de entradas
 * - Nuevo actuador no responde: Verificar pin y función de control
 * - Red no decide correctamente: Reentrenar con más escenarios
 *
 * PARA DOCUMENTAR:
 * - Qué sensores elegiste y por qué
 * - Qué actuador agregaste y su función
 * - Cómo afecta al comportamiento del coche
 * - Capturas de simulación funcionando
 * - Problemas encontrados y soluciones
 */
