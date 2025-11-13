# Guía para Tarea 4: Expansión de la Red Neuronal

## Objetivo
Modificar la arquitectura de la red neuronal agregando **2 nuevas entradas** (sensores) y **1 nueva salida** (actuador). Cada miembro del equipo debe usar sensores y actuadores DIFERENTES.

**Arquitectura Original**: [2, 3, 4]
**Arquitectura Modificada**: [4, 6, 5] (o ajustar la capa oculta según complejidad)

---

## Requisitos Importantes

### Diferenciación Entre Miembros
Cada miembro del equipo DEBE elegir sensores y actuadores diferentes:

**Gabriel propone**: Velocidad + Sensor IR de borde → Buzzer
**Mariano propone**: Luz LDR + Segundo ultrasónico → LED estado

⚠️ **NO duplicar los mismos sensores entre miembros**

---

## Paso 1: Elegir Nuevas Entradas y Salidas

### Opciones de Nuevas Entradas (Sensores)

#### Opción A: Velocidad Actual
- **Rango**: -1 (retroceso máximo) a +1 (avance máximo), 0 (detenido)
- **Implementación Arduino**: Usar encoders o estimar por tiempo
- **Ventaja**: Permite decisiones basadas en el estado de movimiento

#### Opción B: Sensor Infrarrojo de Borde
- **Rango**: -1 (borde/vacío detectado) a +1 (superficie segura)
- **Implementación Arduino**: Sensor IR digital o analógico
- **Ventaja**: Evita caídas en mesas o escalones

#### Opción C: Sensor de Luz (LDR)
- **Rango**: -1 (oscuridad total) a +1 (luz intensa)
- **Implementación Arduino**: LDR con analogRead
- **Ventaja**: Adaptar comportamiento según iluminación

#### Opción D: Segundo Sensor Ultrasónico Lateral
- **Rango**: -1 (muy cerca lateral) a +1 (sin obstáculo lateral)
- **Implementación Arduino**: Segundo HC-SR04 en costado
- **Ventaja**: Detección lateral para maniobras más precisas

#### Opción E: Acelerómetro (Inclinación)
- **Rango**: -1 (inclinado atrás) a +1 (inclinado adelante)
- **Implementación Arduino**: MPU6050 o similar
- **Ventaja**: Detectar rampas o superficies inclinadas

#### Opción F: Sensor de Temperatura/Humedad (DHT22)
- **Rango**: Normalizado según contexto
- **Implementación Arduino**: Librería DHT
- **Ventaja**: Condiciones ambientales (uso educativo)

### Opciones de Nueva Salida (Actuador)

#### Opción 1: Buzzer/Bocina
- **Valores**: 0 (apagado), 1 (encendido)
- **Implementación Arduino**: digitalWrite al pin del buzzer
- **Uso**: Alerta sonora en situaciones de peligro

#### Opción 2: LED de Estado
- **Valores**: 0 (apagado), 1 (encendido)
- **Implementación Arduino**: digitalWrite o PWM
- **Uso**: Indicador visual del modo de operación

#### Opción 3: Servo Adicional (Brazo/Cámara)
- **Valores**: 0 (posición inicial), 1 (posición extendida)
- **Implementación Arduino**: Servo.write()
- **Uso**: Brazo robótico simple o ajuste de cámara

#### Opción 4: Control de Velocidad Variable
- **Valores**: 0-1 (porcentaje de velocidad)
- **Implementación Arduino**: PWM en pines Enable
- **Uso**: Ajuste fino de velocidad según contexto

---

## Paso 2: Diseñar Tabla de Verdad Expandida

### Requisitos
- **Mínimo 15-20 escenarios** (más que los 9 originales)
- Cubrir combinaciones importantes de las 4 entradas
- Incluir casos extremos y situaciones de peligro

### Template de Tabla de Verdad

```
| E1 (Dist) | E2 (Pos) | E3 (Nueva1) | E4 (Nueva2) | Interpretación | S1 | S2 | S3 | S4 | S5 (Nueva) | Acción |
|-----------|----------|-------------|-------------|----------------|----|----|----|----|------------|--------|
| -1        | 0        | valor       | valor       | Descripción    | 1  | 0  | 0  | 1  | 0          | Acción |
| ...       | ...      | ...         | ...         | ...            | .. | .. | .. | .. | ..         | ...    |
```

### Ejemplo Completo: Gabriel (Velocidad + IR Borde → Buzzer)

```python
# Dataset expandido para Gabriel [4 entradas, 5 salidas]
# E1: Distancia (-1=lejos, 0=medio, 1=cerca)
# E2: Posición (-1=derecha, 0=centro, 1=izquierda)
# E3: Velocidad (-1=retrocediendo, 0=detenido, 1=avanzando)
# E4: Sensor IR Borde (-1=borde, 1=seguro)
# S5: Buzzer (0=apagado, 1=encendido)

X_gabriel = np.array([
    # Situaciones normales (sin peligro)
    [-1,  0,  0,  1],  # Sin obstáculo, detenido, suelo seguro → Avanzar
    [-1,  0,  1,  1],  # Sin obstáculo, avanzando, seguro → Continuar
    [-1,  1,  0,  1],  # Sin obstáculo izq, detenido, seguro → Avanzar
    [-1, -1,  0,  1],  # Sin obstáculo der, detenido, seguro → Avanzar
    [-1,  0, -1,  1],  # Sin obstáculo, retrocediendo, seguro → Continuar retro

    # Obstáculos frontales (sin borde)
    [ 0, -1,  1,  1],  # Obstáculo derecha, avanzando, seguro → Girar derecha
    [ 0,  1,  1,  1],  # Obstáculo izquierda, avanzando, seguro → Girar izquierda
    [ 0,  0,  1,  1],  # Obstáculo centro, avanzando, seguro → Girar derecha
    [ 0,  0,  0,  1],  # Obstáculo centro, detenido, seguro → Girar derecha

    # Obstáculos muy cerca (sin borde)
    [ 1,  0,  1,  1],  # Muy cerca centro, avanzando, seguro → Retroceder
    [ 1,  1,  1,  1],  # Muy cerca izq, avanzando, seguro → Retroceder
    [ 1, -1,  1,  1],  # Muy cerca der, avanzando, seguro → Retroceder
    [ 1,  0,  0,  1],  # Muy cerca, detenido, seguro → Retroceder

    # SITUACIONES DE PELIGRO: Borde detectado
    [-1,  0,  1, -1],  # Avanzando pero BORDE → RETROCEDER + BUZZER
    [-1,  0,  0, -1],  # Detenido en borde → RETROCEDER + BUZZER
    [ 0,  0,  1, -1],  # Obstáculo Y borde → RETROCEDER + BUZZER
    [ 1,  0,  1, -1],  # Muy cerca Y borde → RETROCEDER + BUZZER
    [ 0, -1,  0, -1],  # Obstáculo der Y borde → Girar izq + BUZZER
    [ 0,  1,  0, -1],  # Obstáculo izq Y borde → Girar der + BUZZER
    [ 1,  0, -1, -1],  # Muy cerca, retrocediendo, borde → RETROCEDER + BUZZER
])

y_gabriel = np.array([
    # Situaciones normales
    [1, 0, 0, 1, 0],  # Avanzar sin alerta
    [1, 0, 0, 1, 0],  # Continuar avanzando
    [1, 0, 0, 1, 0],  # Avanzar
    [1, 0, 0, 1, 0],  # Avanzar
    [0, 1, 1, 0, 0],  # Continuar retrocediendo

    # Obstáculos (sin borde)
    [0, 1, 0, 1, 0],  # Girar derecha
    [1, 0, 1, 0, 0],  # Girar izquierda
    [0, 1, 0, 1, 0],  # Girar derecha
    [0, 1, 0, 1, 0],  # Girar derecha

    # Muy cerca (sin borde)
    [0, 1, 1, 0, 0],  # Retroceder
    [0, 1, 1, 0, 0],  # Retroceder
    [0, 1, 1, 0, 0],  # Retroceder
    [0, 1, 1, 0, 0],  # Retroceder

    # PELIGRO: Borde detectado
    [0, 1, 1, 0, 1],  # RETROCEDER + BUZZER
    [0, 1, 1, 0, 1],  # RETROCEDER + BUZZER
    [0, 1, 1, 0, 1],  # RETROCEDER + BUZZER
    [0, 1, 1, 0, 1],  # RETROCEDER + BUZZER
    [1, 0, 1, 0, 1],  # Girar izquierda + BUZZER
    [0, 1, 0, 1, 1],  # Girar derecha + BUZZER
    [0, 1, 1, 0, 1],  # RETROCEDER + BUZZER
])

print(f"Dataset Gabriel: {X_gabriel.shape} entradas, {y_gabriel.shape} salidas")
print(f"Total escenarios: {len(X_gabriel)}")
```

---

## Paso 3: Modificar el Código de Entrenamiento

### Actualizar la Arquitectura

```python
# ANTES: Red original
# nn = NeuralNetwork([2, 3, 4])

# DESPUÉS: Red expandida
nn = NeuralNetwork([4, 6, 5], activation='tanh', learning_rate=0.03)

print(f"Nueva arquitectura: [4, 6, 5]")
print(f"- 4 entradas: Distancia, Posición, [Nueva1], [Nueva2]")
print(f"- 6 neuronas en capa oculta (+ 1 BIAS = 7 nodos)")
print(f"- 5 salidas: Motor1, Motor2, Motor3, Motor4, [Nuevo Actuador]")
print(f"\\nHiddenWeights shape: {nn.HiddenWeights.shape}")  # (5, 7)
print(f"OutputWeights shape: {nn.OutputWeights.shape}")      # (7, 5)
```

### Entrenar con Nuevo Dataset

```python
# Usar el dataset expandido (ej: X_gabriel, y_gabriel)
errors = nn.fit(X_gabriel, y_gabriel, epochs=50001, verbose=True)

# Nota: Puede requerir más épocas por mayor complejidad
# Si el error no converge, probar con 60000-80000 épocas
```

### Probar Predicciones

```python
predictions = nn.predict(X_gabriel)
predictions_binary = np.round(np.abs(predictions))

# Verificar precisión
correctos = np.sum([np.array_equal(pred, target)
                    for pred, target in zip(predictions_binary, y_gabriel)])
accuracy = (correctos / len(y_gabriel)) * 100

print(f"\\n🎯 Predicciones correctas: {correctos}/{len(y_gabriel)}")
print(f"🎯 Precisión: {accuracy:.2f}%")

# Si la precisión no es 100%, entrenar más épocas
```

---

## Paso 4: Exportar Pesos para Arduino Modificado

```python
print("\\n" + "="*70)
print("MATRICES DE PESOS PARA ARDUINO [4,6,5]")
print("="*70)

print("\\n// HiddenWeights (5 x 7) - incluye BIAS")
print("float HiddenWeights[5][7] = {")
for i, row in enumerate(nn.HiddenWeights):
    valores = ", ".join([f"{val:.8f}f" for val in row])
    coma = "," if i < len(nn.HiddenWeights) - 1 else ""
    print(f"    {{{valores}}}{coma}")
print("};")

print("\\n// OutputWeights (7 x 5) - incluye BIAS")
print("float OutputWeights[7][5] = {")
for i, row in enumerate(nn.OutputWeights):
    valores = ", ".join([f"{val:.8f}f" for val in row])
    coma = "," if i < len(nn.OutputWeights) - 1 else ""
    print(f"    {{{valores}}}{coma}")
print("};")

# Guardar en archivo
with open(f'pesos_expandidos_{nombre_miembro}.txt', 'w') as f:
    f.write("HiddenWeights:\\n")
    f.write(str(nn.HiddenWeights) + "\\n\\n")
    f.write("OutputWeights:\\n")
    f.write(str(nn.OutputWeights) + "\\n")

print(f"\\n💾 Pesos guardados en 'pesos_expandidos_{nombre_miembro}.txt'")
```

---

## Paso 5: Adaptar Código Arduino

### Actualizar Dimensiones

```cpp
// ANTES
const int InputNodes = 3;   // 2 + 1 BIAS
const int HiddenNodes = 4;  // 3 + 1 BIAS
const int OutputNodes = 4;

// DESPUÉS
const int InputNodes = 5;   // 4 + 1 BIAS
const int HiddenNodes = 7;  // 6 + 1 BIAS
const int OutputNodes = 5;
```

### Declarar Arrays Más Grandes

```cpp
// Arrays para las capas
float InputLayer[InputNodes];
float HiddenLayer[HiddenNodes];
float OutputLayer[OutputNodes];

// Arrays para pesos (copiar desde Python)
float HiddenWeights[InputNodes][HiddenNodes] = {
    // Pegar valores desde salida de Python
    {...},
    {...},
    // ... 5 filas x 7 columnas
};

float OutputWeights[HiddenNodes][OutputNodes] = {
    // Pegar valores desde salida de Python
    {...},
    // ... 7 filas x 5 columnas
};
```

### Leer Nuevos Sensores

#### Ejemplo: Velocidad Estimada

```cpp
// Variables globales
float velocidadActual = 0.0;  // -1 a 1
unsigned long ultimoTiempoMovimiento = 0;

void actualizarVelocidad() {
    // Estimar velocidad basada en últimos comandos
    // Simplificación: usar estado de motores previo

    if (ultimaAccion == AVANZAR) {
        velocidadActual = 1.0;
    } else if (ultimaAccion == RETROCEDER) {
        velocidadActual = -1.0;
    } else {
        // Decelerar gradualmente
        velocidadActual *= 0.9;
        if (abs(velocidadActual) < 0.1) velocidadActual = 0.0;
    }
}
```

#### Ejemplo: Sensor IR de Borde

```cpp
// Pines
const int PIN_IR_BORDE = A2;

float leerSensorBorde() {
    int valorIR = analogRead(PIN_IR_BORDE);

    // Convertir a rango [-1, 1]
    // IR alto (>700) = superficie oscura/borde = -1
    // IR bajo (<300) = superficie clara/segura = 1

    if (valorIR > 700) {
        return -1.0;  // PELIGRO: Borde detectado
    } else if (valorIR < 300) {
        return 1.0;   // SEGURO: Superficie sólida
    } else {
        // Mapear rango intermedio
        return map(valorIR, 300, 700, 100, -100) / 100.0;
    }
}
```

#### Ejemplo: Sensor de Luz (LDR)

```cpp
const int PIN_LDR = A3;

float leerLuzAmbiental() {
    int valorLDR = analogRead(PIN_LDR);

    // Convertir a rango [-1, 1]
    // -1 = oscuro (valor bajo)
    // +1 = brillante (valor alto)

    return map(valorLDR, 0, 1023, -100, 100) / 100.0;
}
```

### Preparar Entradas para la Red

```cpp
void conducir() {
    // Leer sensores
    int distance = Distance_test();
    int angulo = grados_servo;

    // Normalizar entradas originales
    float entrada1 = ((-2.0/ditanciaMaxima) * float(distance)) + 1.0;
    float entrada2 = ((2.0/ANGULO_MAX) * float(angulo));

    // Leer y normalizar NUEVAS entradas
    float entrada3 = velocidadActual;  // Ya está normalizada
    float entrada4 = leerSensorBorde(); // Ya está normalizada

    // Preparar array de entrada CON BIAS
    InputLayer[0] = entrada1;
    InputLayer[1] = entrada2;
    InputLayer[2] = entrada3;  // Nueva entrada 1
    InputLayer[3] = entrada4;  // Nueva entrada 2
    InputLayer[4] = 1.0;       // BIAS

    // Inferencia de la red (forward propagation)
    InputToOutput();

    // Procesar salidas...
}
```

### Controlar Nuevo Actuador

#### Ejemplo: Buzzer

```cpp
const int PIN_BUZZER = 9;

void setup() {
    // ...
    pinMode(PIN_BUZZER, OUTPUT);
}

void aplicarAccion() {
    // Procesar salidas de motores (OutputLayer[0-3])
    int out1 = round(abs(OutputLayer[0]));
    int out2 = round(abs(OutputLayer[1]));
    int out3 = round(abs(OutputLayer[2]));
    int out4 = round(abs(OutputLayer[3]));

    // Procesar NUEVA salida (buzzer)
    int buzzer = round(abs(OutputLayer[4]));

    // Control de motores (código original)
    digitalWrite(IN1, out1);
    digitalWrite(IN2, out2);
    digitalWrite(IN3, out3);
    digitalWrite(IN4, out4);

    // Control de buzzer
    if (buzzer == 1) {
        tone(PIN_BUZZER, 1000);  // Tono de 1000 Hz
    } else {
        noTone(PIN_BUZZER);
    }
}
```

#### Ejemplo: LED de Estado

```cpp
const int PIN_LED_ESTADO = 10;

void aplicarAccion() {
    // ... (motores)

    // Control de LED
    int ledEstado = round(abs(OutputLayer[4]));
    digitalWrite(PIN_LED_ESTADO, ledEstado);

    // Opcional: PWM para brillo variable
    // analogWrite(PIN_LED_ESTADO, ledEstado * 255);
}
```

---

## Paso 6: Simular en Wokwi

### Crear Proyecto Modificado

1. Ir a [Wokwi.com](https://wokwi.com)
2. Crear nuevo proyecto Arduino Uno
3. Agregar componentes adicionales:
   - Para buzzer: Buzzer piezo
   - Para LED: LED + Resistencia 220Ω
   - Para LDR: Photoresistor + Resistencia 10kΩ
   - Para sensor IR: Simular con potenciómetro

### Conexiones Sugeridas

```
Nuevos Componentes:
- Buzzer: Pin 9
- LED Estado: Pin 10
- LDR: A3 (con divisor de voltaje)
- Sensor IR Borde: A2 (simular con potenciómetro)
```

### Probar Escenarios

En la simulación, probar:
1. Comportamiento normal sin obstáculos
2. Detección de obstáculos con sensor frontal
3. Activación de nueva salida en situaciones críticas
4. Comportamiento con diferentes valores de nuevas entradas

---

## Checklist de Completitud Tarea 4

### Notebook de Entrenamiento
- [ ] Arquitectura modificada a [4, 6, 5]
- [ ] Dataset expandido con 15-20+ escenarios
- [ ] Entrenamiento completado con error < 0.01
- [ ] Precisión en predicciones = 100%
- [ ] Gráfica de entrenamiento generada
- [ ] Matrices de pesos exportadas

### Código Arduino
- [ ] Dimensiones actualizadas (InputNodes=5, etc.)
- [ ] Pesos copiados desde Python
- [ ] Código para leer nuevos sensores
- [ ] Normalización de nuevas entradas
- [ ] Control de nuevo actuador
- [ ] Código compila sin errores

### Simulación Wokwi
- [ ] Proyecto creado con todos los componentes
- [ ] Conexiones correctas
- [ ] Código cargado y funcionando
- [ ] Comportamiento verificado en varios escenarios
- [ ] Capturas de pantalla guardadas

### Documentación
- [ ] Tabla de verdad expandida documentada
- [ ] Descripción de nuevos sensores/actuadores
- [ ] Resultados de entrenamiento documentados
- [ ] Link a simulación Wokwi compartido
- [ ] Problemas encontrados y soluciones documentados

---

## Problemas Comunes y Soluciones

### Problema 1: Red no converge con nueva arquitectura
**Síntomas**: Error se mantiene alto (>0.1) después de 50000 épocas

**Soluciones**:
1. Aumentar épocas a 80000-100000
2. Ajustar learning_rate (probar 0.05 o 0.02)
3. Revisar que el dataset tenga buena variabilidad
4. Verificar que no haya errores en las salidas esperadas

### Problema 2: Precisión <100% en predicciones
**Solución**: Normal con datasets pequeños. Si es <90%:
- Aumentar neuronas en capa oculta a 8-10
- Agregar más escenarios al dataset
- Verificar que las normalizaciones sean correctas

### Problema 3: Arduino se queda sin memoria
**Síntomas**: Error "not enough memory" al compilar

**Soluciones**:
1. Usar Arduino Mega en lugar de Uno
2. Usar tipo `float` en lugar de `double`
3. Optimizar arrays temporales
4. Remover prints de debugging

### Problema 4: Wokwi no tiene el sensor necesario
**Soluciones**:
- Simular con potenciómetro (para sensores analógicos)
- Usar botón (para sensores digitales)
- Documentar que es simulación, indicar hardware real requerido

### Problema 5: Nuevas entradas siempre en extremos
**Causa**: Normalización incorrecta

**Solución**: Revisar funciones de mapeo y normalización
```cpp
// Verificar que los rangos sean correctos
float valor = map(lectura, min_esperado, max_esperado, -100, 100) / 100.0;
```

---

## Criterios de Éxito

Tu implementación de Tarea 4 es exitosa si:

✅ La red entrena y converge (error final < 0.01)
✅ Las predicciones tienen precisión ≥ 95%
✅ El código Arduino compila sin errores
✅ La simulación Wokwi muestra comportamiento coherente
✅ El nuevo actuador se activa en situaciones apropiadas
✅ La documentación está completa y clara

---

## Siguiente Paso

Una vez completada la Tarea 4:
1. Actualizar README.md con tus resultados específicos
2. Agregar capturas y gráficas a la carpeta docs/imagenes/
3. Subir tu notebook modificado a notebooks/
4. Compartir link de simulación Wokwi
5. Documentar en la wiki (si aplicable)

---

**Éxito con tu expansión de la red neuronal!** 🚀🤖

¿Preguntas? Revisa la wiki o consulta con tu compañero de equipo.
