# Bienvenido al Proyecto: Coche Autónomo Arduino con Red Neuronal

![Estado: En Desarrollo](https://img.shields.io/badge/Estado-En%20Desarrollo-yellow)
![Licencia: Educativo](https://img.shields.io/badge/Licencia-Educativo-blue)

## Descripción General

Este proyecto implementa un **sistema de conducción autónoma** en un vehículo robótico basado en Arduino, utilizando una **Red Neuronal Artificial** entrenada mediante técnicas de Machine Learning. Es parte del curso de **Metodologías de Resolución de Problemas y Arquitectura de Computing**.

### Objetivo Principal

Demostrar la **viabilidad de implementar inteligencia artificial en dispositivos con recursos computacionales limitados**, permitiendo que un coche navegue autónomamente evitando obstáculos sin programación basada en reglas explícitas.

---

## Equipo de Desarrollo

| Integrante           | Rol                        | Notebook Colab                                                                                                  |
| -------------------- | -------------------------- | --------------------------------------------------------------------------------------------------------------- |
| **Gabriel Osemberg** | Desarrollador/Investigador | [Ver Notebook](https://colab.research.google.com/drive/1bfZeigmpn53fXfVV7x_setYsHZ7hpEtG#scrollTo=9BaLTKenQrQZ) |
| **Mariano Capella**  | Desarrollador/Investigador | [Ver Notebook](https://colab.research.google.com/drive/1eboG6E3a6TOkrEtj2H-AINEdRsHhfTAo#scrollTo=GffuaCAYU5v8) |

---

## Características del Proyecto

### Innovación Técnica

- **Arquitectura Híbrida**: Entrenamiento offline en Python + Inferencia online en Arduino
- **Machine Learning en Hardware Limitado**: Red neuronal ejecutándose en microcontrolador de 8 bits
- **Sistema de Radar Dinámico**: Sensor ultrasónico montado en servo para percepción 2D del entorno

### Tecnologías Utilizadas

- 🐍 **Python** con NumPy para entrenamiento de la red neuronal
- 🤖 **Arduino (C++)** para implementación embebida
- 🌐 **Wokwi** para simulación de hardware
- 📊 **Google Colab** para experimentación y documentación
- 📡 **Sensores**: Ultrasónico HC-SR04, IR, LDR
- ⚙️ **Actuadores**: Motores DC, Servo SG90, Buzzer, LEDs

---

## Arquitectura del Sistema

### Flujo de Trabajo General

```
┌─────────────────────────────────────────────────────────────┐
│                    FASE 1: ENTRENAMIENTO                    │
│                      (Entorno Python)                       │
├─────────────────────────────────────────────────────────────┤
│  1. Diseño de tabla de verdad (escenarios de entrada-salida)│
│  2. Definición de arquitectura de red neuronal              │
│  3. Entrenamiento con backpropagation (40,000+ épocas)      │
│  4. Generación de matrices de pesos optimizadas             │
└────────────────────┬────────────────────────────────────────┘
                     │
                     │ Transferencia de pesos
                     │
┌────────────────────▼────────────────────────────────────────┐
│                     FASE 2: INFERENCIA                      │
│                     (Entorno Arduino)                       │
├─────────────────────────────────────────────────────────────┤
│  1. Captura de datos de sensores en tiempo real             │
│  2. Normalización de entradas [-1, 1]                       │
│  3. Forward propagation con pesos pre-entrenados            │
│  4. Decisión binaria (activación de motores)                │
│  5. Ejecución de acción física (avanzar/girar/retroceder)   │
└─────────────────────────────────────────────────────────────┘
```

---

## Contenido de la Wiki

### Documentación Fundamental

#### 📋 [Análisis de Arquitecturas](Arquitecturas)

Análisis completo de las tres arquitecturas observadas:

- **Arquitectura de Software**: Estructura de la red neuronal [2,3,4] → [4,6,5]
- **Arquitectura de Hardware**: Componentes físicos y ensamblaje
- **Arquitectura de Integración**: Flujo de control y datos en tiempo real

#### 🧠 [Enfoques de Resolución](Enfoques)

Decisiones estratégicas y tácticas del proyecto:

- Justificación de Machine Learning vs. Programación Basada en Reglas
- Arquitectura híbrida de dos fases
- Soluciones tácticas de implementación

### Implementación

#### 🔢 [Tabla de Verdad Original](Tabla-Verdad-Original)

Dataset base con 9 escenarios que mapean percepciones a acciones del vehículo.

#### ⚡ [Modificaciones de Red Neuronal](Modificaciones)

Expansión de la arquitectura con nuevas entradas y salidas:

- **Gabriel**: Velocidad + IR Borde → Buzzer
- **Mariano**: Luz LDR + Ultrasónico Lateral → LED Estado

#### 💻 [Código Arduino](Codigo-Arduino)

Implementación del modelo en microcontrolador:

- Código original [2,3,4]
- Códigos modificados [4,6,5] por cada miembro

#### 🌐 [Simulación Wokwi](Simulacion-Wokwi)

Prototipado virtual del hardware y pruebas de comportamiento.

### Recursos

#### 📚 [Instrucciones de Uso](Instrucciones)

Guía paso a paso:

- Cómo entrenar el modelo en Colab
- Cómo implementar en Arduino
- Cómo simular en Wokwi

#### 🔗 [Referencias Externas](Referencias)

Links al proyecto base, artículos, notebooks y simulaciones.

---

## Progreso del Proyecto

### ✅ Completado

- [x] Análisis de arquitecturas observadas (Tarea 1)
- [x] Análisis de enfoques de resolución de problemas (Tarea 2)
- [x] Estructura de repositorio y documentación base

### ⏳ En Progreso

- [ ] Ejecución de notebooks para entrenamiento (Tarea 3)
- [ ] Modificación de arquitectura con nuevas entradas/salidas (Tarea 4)
- [ ] Implementación en Arduino de versiones modificadas
- [ ] Creación de simulaciones Wokwi individuales

### 📅 Pendiente

- [ ] Capturas de gráficas de entrenamiento
- [ ] Documentación de resultados finales
- [ ] Validación completa en simulación

---

## Enlaces Rápidos

### Repositorio

- 🏠 [README Principal](../README.md)
- 📂 [Ver Código en GitHub](https://github.com/Nubiru/arduino_auto)

### Documentación Complementaria

- 📄 [Arquitecturas Observadas (Markdown)](../docs/Arquitecturas_Observadas.md)
- 📄 [Enfoques de Resolución (Markdown)](../docs/Enfoques_Resolucion_Problemas.md)

### Notebooks de Entrenamiento

- 🔬 [Gabriel Osemberg - Colab](https://colab.research.google.com/drive/1bfZeigmpn53fXfVV7x_setYsHZ7hpEtG#scrollTo=9BaLTKenQrQZ)
- 🔬 [Mariano Capella - Colab](https://colab.research.google.com/drive/1eboG6E3a6TOkrEtj2H-AINEdRsHhfTAo#scrollTo=GffuaCAYU5v8)

### Proyecto Base

- 📖 [Artículo Original](https://www.aprendemachinelearning.com/programa-un-coche-arduino-con-inteligencia-artificial/)
- 💾 [Notebook Base](https://github.com/jbagnato/machine-learning/blob/master/Red_Neuronal_coche.ipynb)
- 🎮 [Simulación Wokwi Base](https://wokwi.com/projects/446073684209624065)

---

## Contacto y Contribuciones

Este es un proyecto educativo desarrollado como parte de un curso universitario. Para consultas sobre el proyecto, contactar a los miembros del equipo a través de sus notebooks de Colab.

---

**Última actualización**: Fecha automática al hacer push

---

> **Nota**: Esta wiki está en constante desarrollo. Las secciones marcadas como "pendiente" se irán completando conforme avance la implementación del proyecto.
