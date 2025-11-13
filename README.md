# Coche Autónomo Arduino con Red Neuronal

## Integrantes del Equipo
- Gabriel Osemberg - [Colab](https://colab.research.google.com/drive/1bfZeigmpn53fXfVV7x_setYsHZ7hpEtG#scrollTo=9BaLTKenQrQZ) [Wokwi](https://wokwi.com/projects/447443477537977345)
- Mariano Capella - [Colab](https://colab.research.google.com/drive/1eboG6E3a6TOkrEtj2H-AINEdRsHhfTAo#scrollTo=GffuaCAYU5v8) [Wokwi](https://wokwi.com/projects/446822632128479233)

## Índice
1. [Resumen del Proyecto](#resumen-del-proyecto)
2. [Arquitecturas Observadas](#arquitecturas-observadas)
3. [Enfoques de Resolución de Problemas](#enfoques-de-resolución-de-problemas)
4. [Tabla de Verdad Original](#tabla-de-verdad-original)
5. [Modificaciones Realizadas (Tarea 4)](#modificaciones-realizadas-tarea-4)
6. [Implementación en Arduino](#implementación-en-arduino)
7. [Resultados y Conclusiones](#resultados-y-conclusiones)
8. [Referencias](#referencias)
9. [Instrucciones de Uso](#instrucciones-de-uso)
10. [Estructura del Repositorio](#estructura-del-repositorio)

---

## Resumen del Proyecto

Este proyecto implementa un sistema de conducción autónoma en un vehículo robótico basado en Arduino, utilizando una **Red Neuronal Artificial** entrenada mediante técnicas de Machine Learning. El objetivo principal es demostrar la viabilidad de implementar inteligencia artificial en dispositivos con recursos computacionales limitados, permitiendo que el coche navegue de forma autónoma evitando obstáculos.

### Características Principales:
- **Entrenamiento Offline**: La red neuronal se entrena en Python utilizando backpropagation
- **Inferencia Online**: El modelo entrenado se ejecuta en Arduino para toma de decisiones en tiempo real
- **Arquitectura Escalable**: Fácil incorporación de nuevos sensores sin reescribir lógica de control
- **Simulación en Wokwi**: Prototipado y pruebas sin hardware físico

### Tecnologías Utilizadas:
- Python (NumPy) para entrenamiento de red neuronal
- Arduino (C++) para implementación embebida
- Wokwi para simulación de hardware
- Google Colab para experimentación y documentación

---

## Arquitecturas Observadas

El proyecto se fundamenta en una **arquitectura dual** que separa el entrenamiento computacionalmente intensivo de la ejecución ligera en dispositivo embebido.

### Arquitectura de Software: Red Neuronal

#### Configuración Original
- **Capa de Entrada**: 2 neuronas + 1 BIAS (distancia y posición angular normalizadas)
- **Capa Oculta**: 3 neuronas + 1 BIAS con función de activación tanh
- **Capa de Salida**: 4 neuronas (control de 4 motores DC)

```python
# Arquitectura en Python
NeuralNetwork([2, 3, 4], activation='tanh')
```

```cpp
// Implementación en Arduino
InputNodes = 3;    // 2 sensores + 1 BIAS
HiddenNodes = 4;   // 3 neuronas + 1 BIAS
OutputNodes = 4;   // 4 motores
```

#### Proceso de Entrenamiento (Offline)
1. **Dataset**: 9 escenarios de entrada-salida cuidadosamente diseñados
2. **Algoritmo**: Descenso de gradiente estocástico con retropropagación
3. **Parámetros**:
   - Tasa de aprendizaje: 0.03
   - Épocas: 40,001
   - Inicialización: Pesos aleatorios entre -1 y 1
4. **Resultado**: Matrices de pesos (HiddenWeights y OutputWeights)

#### Proceso de Inferencia (Online)
1. Captura de datos de sensores (distancia y ángulo)
2. Normalización a rango [-1, 1]
3. Propagación hacia adelante (forward propagation)
4. Conversión de salidas continuas a binarias (0 o 1)
5. Activación directa de motores según patrón de salida

### Arquitectura de Hardware

#### Componentes Esenciales:
- **Arduino Uno/Mega**: Microcontrolador central
- **L298N**: Controlador de motor (puente H dual)
- **4x Motores DC**: Propulsión y dirección
- **Servo SG90**: Orientación dinámica del sensor
- **Sensor Ultrasónico HC-SR04**: Detección de distancia (2-400 cm)
- **Fuente de alimentación**: Baterías para motores y lógica

#### Ensamblaje Crítico:
El sensor ultrasónico montado sobre el servo motor crea un sistema de **"radar" dinámico**:
- Barrido angular entre `ANGULO_MIN` y `ANGULO_MAX`
- Medición de distancia en cada posición angular
- Proporciona dos entradas esenciales: **distancia** y **posición** del obstáculo

### Arquitectura de Integración

**Ciclo de operación en tiempo real**:
```
1. Escaneo del entorno (servo motor barre área frontal)
2. Captura de datos (sensor ultrasónico mide distancia)
3. Normalización de entradas (escalar a [-1, 1])
4. Cálculo de tiempo de reacción adaptativo
5. Inferencia de red neuronal (forward propagation)
6. Decisión binaria (redondeo de salidas)
7. Control de motores (activación de pines digitales)
```

**Ver análisis completo**: [Arquitecturas Observadas](docs/Arquitecturas_Observadas.md)

---

## Enfoques de Resolución de Problemas

### Decisión Estratégica: Machine Learning vs. Programación Basada en Reglas

El proyecto adopta un **enfoque de Machine Learning** en lugar de lógica condicional tradicional (if-then-else).

#### Ventajas del Enfoque ML:
- **Escalabilidad**: Agregar nuevos sensores solo requiere actualizar dataset y reentrenar
- **Robustez**: Capacidad de generalización ante situaciones no vistas
- **Mantenibilidad**: Evita código complejo con sentencias condicionales anidadas

#### Comparativa:

| Aspecto | ML (Implementado) | Reglas (Alternativa) |
|---------|-------------------|----------------------|
| Complejidad inicial | Media-Alta | Baja |
| Escalabilidad | Excelente | Mala |
| Mantenimiento | Simple (reentrenar) | Complejo (reescribir código) |
| Adaptabilidad | Alta (generaliza) | Baja (solo casos programados) |
| Requisito computacional | Alto (entrenamiento) | Bajo |

### Arquitectura Híbrida de Dos Fases

**Fase 1: Entrenamiento en Python**
- Entorno de alto rendimiento (Colab/PC)
- Frameworks especializados (NumPy, posibilidad de TensorFlow/PyTorch)
- Iteraciones intensivas hasta convergencia
- Genera matrices de pesos optimizadas

**Fase 2: Ejecución en Arduino**
- Solo implementa forward propagation
- Operaciones simples: multiplicación de matrices y tanh
- Requisitos mínimos de memoria y procesamiento
- Toma de decisiones en tiempo real (<100ms)

### Soluciones Tácticas Implementadas

#### 1. Normalización de Entradas Sensoriales
```cpp
// Mapeo de distancia a [-1, 1]
entrada1 = ((-2.0/distanciaMaxima) * double(distance)) + 1.0;
// -1: sin peligro (distancia máxima)
// +1: peligro inminente (distancia cero)
```

#### 2. Sistema de Radar con Servo
- Barrido continuo del entorno
- Interpretación espacial (distancia + dirección)
- Actualización dinámica de entradas a la red

#### 3. Ajuste de Velocidad en Giros
```cpp
// Duplicar velocidad durante giros para vencer inercia
if ((out1+out3)==2 || (out2+out4)==2) {
    carSpeed = SPEED * 2;
}
```

#### 4. Tiempo de Reacción Adaptativo
```cpp
// Más tiempo de maniobra cuando obstáculo está cerca
accionEnCurso = (entrada1 + 1) * MULTIPLICADOR_REACCION;
```

**Ver análisis completo**: [Enfoques de Resolución de Problemas](docs/Enfoques_Resolucion_Problemas.md)

---

## Tabla de Verdad Original

Dataset de entrenamiento con 9 escenarios que mapean percepciones a acciones:

| Entrada 1<br>(Distancia) | Entrada 2<br>(Posición) | Interpretación | Motor 1 | Motor 2 | Motor 3 | Motor 4 | Acción |
|:------------------------:|:-----------------------:|----------------|:-------:|:-------:|:-------:|:-------:|--------|
| -1 | 0 | Sin obstáculos, centro | 1 | 0 | 0 | 1 | **Avanzar** |
| -1 | 1 | Sin obstáculos, izquierda | 1 | 0 | 0 | 1 | **Avanzar** |
| -1 | -1 | Sin obstáculos, derecha | 1 | 0 | 0 | 1 | **Avanzar** |
| 0 | -1 | Obstáculo detectado, derecha | 0 | 1 | 0 | 1 | **Girar derecha** |
| 0 | 1 | Obstáculo detectado, izquierda | 1 | 0 | 1 | 0 | **Girar izquierda** |
| 0 | 0 | Obstáculo detectado, centro | 1 | 0 | 0 | 1 | **Avanzar** ⚠️ |
| 1 | 1 | Demasiado cerca, izquierda | 0 | 1 | 1 | 0 | **Retroceder** |
| 1 | -1 | Demasiado cerca, derecha | 0 | 1 | 1 | 0 | **Retroceder** |
| 1 | 0 | Demasiado cerca, centro | 0 | 1 | 1 | 0 | **Retroceder** |

### Codificación de Acciones:

| Acción | M1 | M2 | M3 | M4 | Descripción |
|--------|:--:|:--:|:--:|:--:|-------------|
| **Avanzar** | 1 | 0 | 0 | 1 | Motores delanteros activos |
| **Retroceder** | 0 | 1 | 1 | 0 | Motores traseros activos |
| **Girar Derecha** | 0 | 1 | 0 | 1 | Motor izq. adelante, der. atrás |
| **Girar Izquierda** | 1 | 0 | 1 | 0 | Motor der. adelante, izq. atrás |

⚠️ **Nota**: El caso [0,0] → Avanzar puede resultar problemático en implementación real (riesgo de colisión).

---

## Modificaciones Realizadas (Tarea 4)

> **Requisito**: Simular 2 entradas nuevas y 1 salida más. Cada miembro del equipo debe generar una nueva tabla de verdad y desplegarla.

### 5.1 Miembro 1: Gabriel Osemberg

**Arquitectura Modificada**: `[4, 6, 5]`

**Nuevas entradas agregadas**:
- **Entrada 3**: Velocidad actual normalizada
  - -1: Máxima velocidad hacia atrás
  - 0: Detenido
  - 1: Máxima velocidad hacia adelante

- **Entrada 4**: Sensor infrarrojo de borde
  - -1: Borde/vacío detectado
  - 1: Superficie segura

**Nueva salida agregada**:
- **Salida 5**: Buzzer de alerta (0=apagado, 1=encendido)

#### Tabla de Verdad Expandida (Gabriel)

| E1 | E2 | E3 | E4 | Interpretación | S1 | S2 | S3 | S4 | S5 | Acción |
|:--:|:--:|:--:|:--:|----------------|:--:|:--:|:--:|:--:|:--:|--------|
| -1 | 0 | 0 | 1 | Sin obstáculo, detenido, suelo seguro | 1 | 0 | 0 | 1 | 0 | Avanzar normal |
| -1 | 0 | 1 | 1 | Sin obstáculo, avanzando, seguro | 1 | 0 | 0 | 1 | 0 | Continuar avanzando |
| -1 | 0 | 1 | -1 | Avanzando pero BORDE detectado | 0 | 1 | 1 | 0 | 1 | RETROCEDER + Alerta |
| 0 | -1 | 1 | 1 | Obstáculo derecha, avanzando | 0 | 1 | 0 | 1 | 0 | Girar derecha |
| 0 | 1 | 1 | 1 | Obstáculo izquierda, avanzando | 1 | 0 | 1 | 0 | 0 | Girar izquierda |
| 1 | 0 | 1 | 1 | Obstáculo muy cerca, avanzando | 0 | 1 | 1 | 0 | 1 | RETROCEDER + Alerta |
| 1 | 0 | 0 | -1 | Obstáculo cerca Y borde | 0 | 1 | 1 | 0 | 1 | RETROCEDER + Alerta |
| 0 | 0 | -1 | 1 | Obstáculo centro, retrocediendo | 1 | 0 | 1 | 0 | 0 | Girar (cambiar dirección) |
| -1 | 1 | 0 | 1 | Sin obstáculo izq, detenido | 1 | 0 | 0 | 1 | 0 | Avanzar |
| -1 | -1 | 0 | 1 | Sin obstáculo der, detenido | 1 | 0 | 0 | 1 | 0 | Avanzar |
| 1 | 1 | -1 | -1 | Muy cerca izq, retrocediendo, borde | 0 | 1 | 1 | 0 | 1 | RETROCEDER + Alerta |
| 1 | -1 | -1 | -1 | Muy cerca der, retrocediendo, borde | 0 | 1 | 1 | 0 | 1 | RETROCEDER + Alerta |
| 0 | 0 | 0 | -1 | Obstáculo centro, detenido, borde | 0 | 1 | 1 | 0 | 1 | RETROCEDER + Alerta |
| -1 | 0 | -1 | 1 | Sin obstáculo, retrocediendo, seguro | 0 | 1 | 1 | 0 | 0 | Continuar retrocediendo |
| 0 | -1 | 0 | -1 | Obstáculo derecha, detenido, borde | 1 | 0 | 1 | 0 | 1 | Girar izquierda + Alerta |

**Resultados del Entrenamiento**:
- Épocas: [COMPLETAR DESPUÉS DE EJECUTAR COLAB]
- Error final: [COMPLETAR]
- Gráfica: [INCLUIR CAPTURA]

**Simulación Wokwi**: [LINK PENDIENTE]

**Notebook Colab**: [Link de Gabriel](https://colab.research.google.com/drive/1bfZeigmpn53fXfVV7x_setYsHZ7hpEtG#scrollTo=9BaLTKenQrQZ)

---

### 5.2 Miembro 2: Mariano Capella

**Arquitectura Modificada**: `[4, 6, 5]`

**Nuevas entradas agregadas**:
- **Entrada 3**: Sensor de luz LDR (luminosidad ambiental)
  - -1: Oscuridad total
  - 0: Iluminación media
  - 1: Luz intensa

- **Entrada 4**: Segundo sensor ultrasónico lateral
  - -1: Obstáculo muy cerca lateral
  - 0: Obstáculo detectado lateral
  - 1: Sin obstáculo lateral

**Nueva salida agregada**:
- **Salida 5**: LED de estado (0=apagado, 1=encendido - indica modo precaución)

#### Tabla de Verdad Expandida (Mariano)

| E1 | E2 | E3 | E4 | Interpretación | S1 | S2 | S3 | S4 | S5 | Acción |
|:--:|:--:|:--:|:--:|----------------|:--:|:--:|:--:|:--:|:--:|--------|
| -1 | 0 | 1 | 1 | Sin obstáculo, luz buena, lateral libre | 1 | 0 | 0 | 1 | 0 | Avanzar normal |
| -1 | 0 | -1 | 1 | Sin obstáculo frontal, oscuro, lateral libre | 1 | 0 | 0 | 1 | 1 | Avanzar + LED (precaución) |
| -1 | 0 | 0 | 0 | Sin obst. frontal, luz media, obst. lateral | 1 | 0 | 0 | 1 | 1 | Avanzar + LED (precaución) |
| 0 | -1 | 1 | 1 | Obst. derecha frontal, luz buena | 0 | 1 | 0 | 1 | 0 | Girar derecha |
| 0 | -1 | 1 | -1 | Obst. derecha frontal, obst. lateral cerca | 0 | 1 | 1 | 0 | 1 | Retroceder + LED |
| 0 | 1 | 1 | 1 | Obst. izquierda frontal, luz buena | 1 | 0 | 1 | 0 | 0 | Girar izquierda |
| 0 | 1 | 1 | -1 | Obst. izquierda frontal, lateral cerca | 0 | 1 | 1 | 0 | 1 | Retroceder + LED |
| 1 | 0 | 1 | 1 | Muy cerca frontal, luz buena, lateral libre | 0 | 1 | 1 | 0 | 0 | Retroceder |
| 1 | 0 | -1 | 1 | Muy cerca frontal, oscuro | 0 | 1 | 1 | 0 | 1 | Retroceder + LED |
| 1 | 0 | 1 | -1 | Muy cerca frontal Y lateral | 0 | 1 | 1 | 0 | 1 | Retroceder + LED |
| 1 | 1 | -1 | -1 | Muy cerca izq, oscuro, lateral cerca | 0 | 1 | 1 | 0 | 1 | Retroceder + LED |
| 1 | -1 | -1 | -1 | Muy cerca der, oscuro, lateral cerca | 0 | 1 | 1 | 0 | 1 | Retroceder + LED |
| 0 | 0 | -1 | 0 | Obst. centro, oscuro, lateral medio | 0 | 1 | 0 | 1 | 1 | Girar derecha + LED |
| -1 | 1 | 0 | 1 | Sin obst. frontal izq, luz media | 1 | 0 | 0 | 1 | 0 | Avanzar |
| -1 | -1 | 0 | 1 | Sin obst. frontal der, luz media | 1 | 0 | 0 | 1 | 0 | Avanzar |

**Resultados del Entrenamiento**:
- Épocas: [COMPLETAR DESPUÉS DE EJECUTAR COLAB]
- Error final: [COMPLETAR]
- Gráfica: [INCLUIR CAPTURA]

**Simulación Wokwi**: [LINK PENDIENTE]

**Notebook Colab**: [Link de Mariano](https://colab.research.google.com/drive/1eboG6E3a6TOkrEtj2H-AINEdRsHhfTAo#scrollTo=GffuaCAYU5v8)

---

## Implementación en Arduino

### Código Original

El código base implementa la red neuronal [2,3,4] en Arduino. Archivos disponibles en:
- `arduino/codigo_original/` - [PENDIENTE: Agregar código del proyecto Wokwi original]

**Componentes clave del código**:
```cpp
// Definición de arquitectura
const int InputNodes = 3;
const int HiddenNodes = 4;
const int OutputNodes = 4;

// Matrices de pesos (pre-entrenadas en Python)
float HiddenWeights[InputNodes][HiddenNodes];
float OutputWeights[HiddenNodes][OutputNodes];

// Función de activación tanh
float activation(float x) {
    return tanh(x);
}

// Forward propagation
void InputToOutput(float* entrada, int* salida) {
    // Capa oculta
    for (int i = 0; i < HiddenNodes; i++) {
        HiddenLayer[i] = 0;
        for (int j = 0; j < InputNodes; j++) {
            HiddenLayer[i] += entrada[j] * HiddenWeights[j][i];
        }
        HiddenLayer[i] = activation(HiddenLayer[i]);
    }

    // Capa de salida
    for (int i = 0; i < OutputNodes; i++) {
        OutputLayer[i] = 0;
        for (int j = 0; j < HiddenNodes; j++) {
            OutputLayer[i] += HiddenLayer[j] * OutputWeights[j][i];
        }
        OutputLayer[i] = activation(OutputLayer[i]);
        salida[i] = round(abs(OutputLayer[i]));
    }
}
```

### Código Modificado

Los códigos adaptados para las arquitecturas [4,6,5] de cada miembro están en:
- `arduino/codigo_modificado/gabriel_coche.ino` - [PENDIENTE]
- `arduino/codigo_modificado/mariano_coche.ino` - [PENDIENTE]

**Cambios principales**:
1. Actualizar dimensiones de arrays
2. Agregar lectura de nuevos sensores
3. Normalizar nuevas entradas
4. Copiar nuevas matrices de pesos
5. Implementar control de nuevos actuadores

---

## Resultados y Conclusiones

### Comportamiento Observado

**Modelo Original**:
- [COMPLETAR después de pruebas en Wokwi]
- Tasa de éxito en evasión: [%]
- Casos problemáticos identificados: [...]

**Modelos Modificados**:
- Gabriel (con velocidad + IR borde): [COMPLETAR]
- Mariano (con luz + ultrasónico lateral): [COMPLETAR]

### Métricas de Precisión

| Modelo | Épocas | Error Final | Convergencia | Tiempo Entrenamiento |
|--------|:------:|:-----------:|:------------:|:--------------------:|
| Original [2,3,4] | 40,001 | - | Sí | ~30s |
| Gabriel [4,6,5] | - | - | - | - |
| Mariano [4,6,5] | - | - | - | - |

### Problemas Encontrados y Soluciones

1. **Problema**: Coche no avanza inicialmente en simulación
   - **Causa**: Valor SPEED insuficiente para motores
   - **Solución**: Incrementar de 100 a 200

2. **Problema**: Giros sin suficiente fuerza
   - **Causa**: Inercia del chasis
   - **Solución**: Duplicar velocidad durante giros `(carSpeed = SPEED * 2)`

3. **Problema**: Dataset pequeño (9 escenarios)
   - **Riesgo**: Overfitting, falta de robustez
   - **Solución**: Expandir a 15-20 escenarios en versiones modificadas

4. **Problema**: [AGREGAR MÁS DESPUÉS DE IMPLEMENTACIÓN]

### Mejoras Futuras

1. **Aumento de Dataset**: Expandir a 30-50 escenarios con más variabilidad
2. **Aprendizaje por Refuerzo**: Permitir que el coche aprenda de errores en tiempo real
3. **Sensores Adicionales**:
   - Cámara (visión por computadora)
   - IMU (detección de colisiones e inclinación)
   - GPS (navegación a objetivo)
4. **Optimización de Red**: Probar arquitecturas más profundas [2,6,6,4]
5. **Hardware Real**: Implementar en Arduino físico con componentes reales

### Conclusión General

Este proyecto demuestra exitosamente que:
- Es viable implementar ML en dispositivos con recursos limitados (Arduino)
- La arquitectura híbrida (entrenamiento offline + inferencia online) es eficiente
- Las redes neuronales ofrecen escalabilidad superior vs. programación basada en reglas
- La separación de entrenamiento y ejecución permite usar hardware especializado para cada fase

El enfoque de Machine Learning proporciona una base sólida y extensible para sistemas autónomos más complejos, facilitando la incorporación de nuevos sensores mediante simple actualización del dataset y reentrenamiento del modelo.

---

## Referencias

### Proyecto Base
- **Artículo**: [Programa un Coche Arduino con Inteligencia Artificial](https://www.aprendemachinelearning.com/programa-un-coche-arduino-con-inteligencia-artificial/)
- **Notebook Original**: [Red_Neuronal_coche.ipynb](https://github.com/jbagnato/machine-learning/blob/master/Red_Neuronal_coche.ipynb)
- **Simulación Wokwi Base**: [Proyecto 446073684209624065](https://wokwi.com/projects/446073684209624065)

### Notebooks del Equipo
- **Gabriel Osemberg**: [Colab Notebook](https://colab.research.google.com/drive/1bfZeigmpn53fXfVV7x_setYsHZ7hpEtG#scrollTo=9BaLTKenQrQZ)
- **Mariano Capella**: [Colab Notebook](https://colab.research.google.com/drive/1eboG6E3a6TOkrEtj2H-AINEdRsHhfTAo#scrollTo=GffuaCAYU5v8)

### Documentación Complementaria
- [Análisis de Arquitecturas Observadas](docs/Arquitecturas_Observadas.md)
- [Enfoques de Resolución de Problemas](docs/Enfoques_Resolucion_Problemas.md)

### Repositorio GitHub
- **URL**: [https://github.com/Nubiru/arduino_auto](https://github.com/Nubiru/arduino_auto)

---

## Instrucciones de Uso

### 1. Entrenar el Modelo (Google Colab)

#### Paso 1: Abrir Notebook
Acceder a uno de los notebooks del equipo o crear uno nuevo basado en el template.

#### Paso 2: Instalar Dependencias
```python
import numpy as np
```

#### Paso 3: Definir Arquitectura
```python
# Para modelo original
nn = NeuralNetwork([2, 3, 4], activation='tanh', learning_rate=0.03)

# Para modelo modificado
nn = NeuralNetwork([4, 6, 5], activation='tanh', learning_rate=0.03)
```

#### Paso 4: Preparar Dataset
```python
# Ejemplo para modelo modificado [4 entradas]
X = np.array([
    [-1, 0, 0, 1],   # Sin obstáculo, detenido, suelo seguro
    [-1, 0, 1, 1],   # Sin obstáculo, avanzando, suelo seguro
    # ... más escenarios
])

y = np.array([
    [1, 0, 0, 1, 0],  # Avanzar sin alerta
    [1, 0, 0, 1, 0],  # Continuar avanzando
    # ... más salidas
])
```

#### Paso 5: Entrenar
```python
nn.fit(X, y, epochs=40001)
```

#### Paso 6: Exportar Pesos
```python
print("HiddenWeights:")
print(nn.HiddenWeights)
print("\nOutputWeights:")
print(nn.OutputWeights)
```

### 2. Implementar en Arduino

#### Paso 1: Copiar Template
Copiar código base desde `arduino/codigo_original/` o `arduino/codigo_modificado/`

#### Paso 2: Actualizar Arquitectura
```cpp
const int InputNodes = 5;    // 4 entradas + 1 BIAS
const int HiddenNodes = 7;   // 6 neuronas + 1 BIAS
const int OutputNodes = 5;   // 5 salidas
```

#### Paso 3: Pegar Pesos Entrenados
```cpp
float HiddenWeights[5][7] = {
    // Copiar valores desde salida de Python
    {...},
    {...},
    // ...
};

float OutputWeights[7][5] = {
    // Copiar valores desde salida de Python
    {...},
    // ...
};
```

#### Paso 4: Agregar Lectura de Nuevos Sensores
```cpp
// Ejemplo: Sensor IR de borde
int valorIR = analogRead(PIN_IR);
entrada[3] = map(valorIR, 0, 1023, -100, 100) / 100.0;
```

#### Paso 5: Agregar Control de Nuevos Actuadores
```cpp
// Ejemplo: Buzzer
if (salida[4] == 1) {
    digitalWrite(PIN_BUZZER, HIGH);
} else {
    digitalWrite(PIN_BUZZER, LOW);
}
```

### 3. Simular en Wokwi

#### Paso 1: Crear Proyecto
1. Ir a [Wokwi.com](https://wokwi.com)
2. Seleccionar "New Project" → "Arduino Uno"

#### Paso 2: Agregar Componentes
- Arduino Uno
- L298N Motor Driver
- 4x DC Motors
- Servo Motor (SG90)
- Ultrasonic Sensor (HC-SR04)
- [Componentes adicionales según modificación]

#### Paso 3: Conectar Circuito
Seguir diagrama de conexiones del artículo base.

#### Paso 4: Cargar Código
Pegar código Arduino en el editor de Wokwi.

#### Paso 5: Ejecutar Simulación
Presionar "Start Simulation" y observar comportamiento.

---

## Estructura del Repositorio

```
arduino_auto/
│
├── README.md                          # Este archivo (documentación principal)
├── tarea.txt                          # Enunciado original del proyecto
│
├── wiki/                              # Documentación para GitHub Wiki
│   ├── Home.md                        # Página principal de la wiki
│   ├── _Sidebar.md                    # Barra lateral de navegación
│   └── _Footer.md                     # Pie de página con referencias
│
├── notebooks/                         # Notebooks de entrenamiento
│   ├── template_entrenamiento.ipynb   # Template base para entrenar
│   ├── gabriel_entrenamiento.ipynb    # [PENDIENTE] Notebook de Gabriel
│   └── mariano_entrenamiento.ipynb    # [PENDIENTE] Notebook de Mariano
│
├── arduino/                           # Código Arduino
│   ├── codigo_original/               # Implementación del proyecto base
│   │   └── coche_original.ino         # [PENDIENTE]
│   │
│   └── codigo_modificado/             # Versiones modificadas (Tarea 4)
│       ├── gabriel_coche.ino          # [PENDIENTE] Versión de Gabriel
│       ├── mariano_coche.ino          # [PENDIENTE] Versión de Mariano
│       └── pesos/                     # Matrices de pesos generadas
│           ├── gabriel_pesos.txt
│           └── mariano_pesos.txt
│
├── docs/                              # Análisis y documentación complementaria
│   ├── Arquitecturas_Observadas.md    # Análisis de arquitecturas (Tarea 1)
│   ├── Enfoques_Resolucion_Problemas.md  # Análisis de enfoques (Tarea 2)
│   └── imagenes/                      # Diagramas y capturas
│       ├── arquitectura_dual.png
│       ├── tabla_verdad.png
│       └── graficas_entrenamiento/
│
└── wokwi/                             # Recursos de simulación
    ├── links_simulaciones.txt         # URLs de proyectos Wokwi
    └── capturas/                      # Screenshots de simulaciones
        ├── gabriel_simulacion.png
        └── mariano_simulacion.png
```

---

## Estado del Proyecto

### Completado
- ✅ Análisis de arquitecturas observadas
- ✅ Análisis de enfoques de resolución de problemas
- ✅ Estructura de repositorio creada
- ✅ Documentación base en README

### En Progreso
- ⏳ Ejecución de notebooks para entrenamiento (Tarea 3)
- ⏳ Modificación de arquitectura y nuevas tablas de verdad (Tarea 4)
- ⏳ Implementación en Arduino de versiones modificadas
- ⏳ Creación de simulaciones Wokwi

### Pendiente
- ❌ Capturas de gráficas de entrenamiento
- ❌ Matrices de pesos de modelos modificados
- ❌ Pruebas y validación en simulación
- ❌ Documentación de resultados finales
- ❌ Crear páginas wiki con cross-references

---

## Licencia y Créditos

**Curso**: Metodologías de Resolución de Problemas y Arquitectura de Computing
**Institución**: [Nombre de la Universidad]
**Año**: 2025

**Basado en el trabajo de**:
- Javier Bagnato - [Proyecto Original](https://github.com/jbagnato/machine-learning)
- AprendeMachineLearning.com - [Artículo Tutorial](https://www.aprendemachinelearning.com/)

**Desarrollado por**:
- Gabriel Osemberg
- Mariano Capella

---

**Última actualización**: [Fecha Automática]

Para más información, consultar la [Wiki del Proyecto](../../wiki).

TABLA DE VERDAD:
MARIANO CAPELLA


Modelo modificado — Red neuronal con 4 entradas y 5 salidas

| Nº | Entrada 1 | Entrada 2 | Avanzar | Girar der. | Girar izq. | Retroceder |
| -- | --------- | --------- | ------- | ---------- | ---------- | ---------- |
| 1  | -1        | 0         | 1       | 0          | 0          | 1          |
| 2  | -1        | 1         | 1       | 0          | 0          | 1          |
| 3  | -1        | -1        | 1       | 0          | 0          | 1          |
| 4  | 0         | -1        | 0       | 1          | 0          | 1          |
| 5  | 0         | 1         | 1       | 0          | 1          | 0          |
| 6  | 0         | 0         | 1       | 0          | 0          | 1          |
| 7  | 1         | 1         | 0       | 1          | 1          | 0          |
| 8  | 1         | -1        | 0       | 1          | 1          | 0          |
| 9  | 1         | 0         | 0       | 1          | 1          | 0          |

Entradas:
Sensor izquierdo
Sensor derecho
Sensor trasero (nueva)
Sensor superior (nueva)

Salidas (acciones):

Avanzar
Girar derecha
Girar izquierda
Retroceder
Detenerse (nueva)

Enlace de wokwoi para modelo original de Arduino:
[text](https://wokwi.com/projects/446822632128479233)