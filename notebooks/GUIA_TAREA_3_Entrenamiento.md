# Guía para Tarea 3: Entrenar la Red Neuronal

## Objetivo

Ejecutar el notebook de Colab para entrenar la red neuronal original [2,3,4] del proyecto base, comprender el proceso y documentar los resultados.

---

## Pasos a Seguir

### 1. Preparación del Notebook

#### Opción A: Usar Notebook Base

1. Ir al [notebook original](https://github.com/jbagnato/machine-learning/blob/master/Red_Neuronal_coche.ipynb)
2. Hacer clic en "Open in Colab"
3. Hacer una copia: `Archivo > Guardar una copia en Drive`

#### Opción B: Crear Notebook Nuevo

1. Ir a [Google Colab](https://colab.research.google.com/)
2. Crear nuevo notebook: `Archivo > Nuevo cuaderno`
3. Copiar el código de referencia

---

### 2. Implementar la Clase NeuralNetwork

Copiar y ejecutar el siguiente código en tu Colab:

```python
import numpy as np

class NeuralNetwork:
    def __init__(self, layers, activation='tanh', learning_rate=0.03):
        """
        Inicializa la red neuronal

        Args:
            layers: lista con el número de neuronas por capa [input, hidden, output]
                   Ejemplo: [2, 3, 4] significa 2 entradas, 3 neuronas ocultas, 4 salidas
            activation: función de activación ('tanh' recomendado)
            learning_rate: tasa de aprendizaje (0.03 por defecto)
        """
        if activation == 'tanh':
            self.activation = np.tanh
            self.activation_prime = lambda x: 1 - np.tanh(x) ** 2
        else:
            raise ValueError("Solo 'tanh' está implementado")

        self.learning_rate = learning_rate

        # Inicializar pesos con valores aleatorios entre -1 y 1
        # +1 para el BIAS en cada capa
        self.HiddenWeights = np.random.uniform(-1.0, 1.0,
                                               size=(layers[0]+1, layers[1]+1))
        self.OutputWeights = np.random.uniform(-1.0, 1.0,
                                               size=(layers[1]+1, layers[2]))

    def forward_propagation(self, inputs):
        """
        Propaga las entradas hacia adelante a través de la red

        Args:
            inputs: array de valores de entrada

        Returns:
            output: predicción de la red
        """
        # Agregar BIAS a las entradas
        inputs = np.append(inputs, 1)

        # Capa oculta
        self.HiddenLayer = self.activation(np.dot(inputs, self.HiddenWeights))

        # Capa de salida
        self.OutputLayer = self.activation(np.dot(self.HiddenLayer, self.OutputWeights))

        return self.OutputLayer

    def backward_propagation(self, inputs, target, output):
        """
        Retropropagación: ajusta los pesos según el error

        Args:
            inputs: valores de entrada originales
            target: salida esperada
            output: salida actual de la red
        """
        # Agregar BIAS
        inputs = np.append(inputs, 1)

        # Calcular error de la capa de salida
        output_error = target - output
        output_delta = output_error * self.activation_prime(output)

        # Calcular error de la capa oculta
        hidden_error = output_delta.dot(self.OutputWeights.T)
        hidden_delta = hidden_error * self.activation_prime(self.HiddenLayer)

        # Actualizar pesos
        self.OutputWeights += self.HiddenLayer.reshape(-1, 1).dot(
            output_delta.reshape(1, -1)) * self.learning_rate
        self.HiddenWeights += inputs.reshape(-1, 1).dot(
            hidden_delta.reshape(1, -1)) * self.learning_rate

    def fit(self, X, y, epochs=40001, verbose=True):
        """
        Entrena la red neuronal

        Args:
            X: matriz de entradas (n_samples, n_features)
            y: matriz de salidas esperadas (n_samples, n_outputs)
            epochs: número de iteraciones de entrenamiento
            verbose: si True, muestra progreso cada 5000 épocas
        """
        errors = []

        for epoch in range(epochs):
            total_error = 0

            # Entrenar con cada ejemplo
            for inputs, target in zip(X, y):
                output = self.forward_propagation(inputs)
                self.backward_propagation(inputs, target, output)
                total_error += np.mean((target - output) ** 2)

            # Guardar error promedio
            avg_error = total_error / len(X)
            errors.append(avg_error)

            # Mostrar progreso
            if verbose and epoch % 5000 == 0:
                print(f'Época {epoch}: Error = {avg_error:.6f}')

        if verbose:
            print(f'\\n✓ Entrenamiento completado!')
            print(f'Error final: {errors[-1]:.6f}')

        return errors

    def predict(self, X):
        """
        Hace predicciones para un conjunto de entradas

        Args:
            X: matriz de entradas

        Returns:
            predictions: predicciones de la red
        """
        predictions = []
        for inputs in X:
            output = self.forward_propagation(inputs)
            predictions.append(output)
        return np.array(predictions)
```

---

### 3. Definir el Dataset (Tabla de Verdad Original)

```python
# Dataset de entrenamiento
# 9 escenarios que mapean [distancia, posición] a [motor1, motor2, motor3, motor4]

X = np.array([
    [-1,  0],  # Sin obstáculos, centro
    [-1,  1],  # Sin obstáculos, izquierda
    [-1, -1],  # Sin obstáculos, derecha
    [ 0, -1],  # Obstáculo detectado, derecha
    [ 0,  1],  # Obstáculo detectado, izquierda
    [ 0,  0],  # Obstáculo detectado, centro
    [ 1,  1],  # Demasiado cerca, izquierda
    [ 1, -1],  # Demasiado cerca, derecha
    [ 1,  0],  # Demasiado cerca, centro
])

y = np.array([
    [1, 0, 0, 1],  # Avanzar
    [1, 0, 0, 1],  # Avanzar
    [1, 0, 0, 1],  # Avanzar
    [0, 1, 0, 1],  # Girar derecha
    [1, 0, 1, 0],  # Girar izquierda
    [1, 0, 0, 1],  # Avanzar (problemático)
    [0, 1, 1, 0],  # Retroceder
    [0, 1, 1, 0],  # Retroceder
    [0, 1, 1, 0],  # Retroceder
])

# Verificar dimensiones
print(f"Shape de X (entradas): {X.shape}")  # Debe ser (9, 2)
print(f"Shape de y (salidas): {y.shape}")   # Debe ser (9, 4)
print(f"\\nDataset cargado correctamente con {len(X)} escenarios")
```

---

### 4. Crear y Entrenar la Red

```python
# Crear red neuronal [2,3,4]
# 2 entradas (distancia, posición)
# 3 neuronas en capa oculta
# 4 salidas (4 motores)
nn = NeuralNetwork([2, 3, 4], activation='tanh', learning_rate=0.03)

print("Red neuronal creada:")
print(f"- Arquitectura: [2, 3, 4]")
print(f"- HiddenWeights shape: {nn.HiddenWeights.shape}")  # (3, 4) con BIAS
print(f"- OutputWeights shape: {nn.OutputWeights.shape}")  # (4, 4) con BIAS
print(f"- Learning rate: {nn.learning_rate}")
print(f"- Activation: tanh\\n")

# Entrenar
print("Iniciando entrenamiento...\\n")
errors = nn.fit(X, y, epochs=40001, verbose=True)
```

---

### 5. Visualizar el Aprendizaje

```python
import matplotlib.pyplot as plt

# Graficar error vs épocas
plt.figure(figsize=(12, 6))

# Gráfica completa
plt.subplot(1, 2, 1)
plt.plot(errors, linewidth=2)
plt.title('Error vs Épocas (Completo)', fontsize=14, fontweight='bold')
plt.xlabel('Época', fontsize=12)
plt.ylabel('Error Cuadrático Medio', fontsize=12)
plt.grid(True, alpha=0.3)

# Gráfica desde época 5000 (zoom para ver convergencia)
plt.subplot(1, 2, 2)
plt.plot(errors[5000:], linewidth=2, color='orange')
plt.title('Error vs Épocas (desde época 5000)', fontsize=14, fontweight='bold')
plt.xlabel('Época', fontsize=12)
plt.ylabel('Error Cuadrático Medio', fontsize=12)
plt.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('grafica_entrenamiento.png', dpi=300, bbox_inches='tight')
plt.show()

print(f"\\n📊 Gráfica guardada como 'grafica_entrenamiento.png'")
print(f"📈 Error inicial: {errors[0]:.6f}")
print(f"📉 Error final: {errors[-1]:.6f}")
print(f"📊 Mejora: {((errors[0] - errors[-1]) / errors[0] * 100):.2f}%")
```

---

### 6. Probar Predicciones

```python
# Probar la red entrenada
print("\\n" + "="*60)
print("PRUEBAS DE PREDICCIÓN")
print("="*60)

predictions = nn.predict(X)

# Redondear predicciones a valores binarios
predictions_binary = np.round(np.abs(predictions))

print("\\n{:<25} {:<25} {:<10}".format("Entrada [Dist, Pos]",
                                       "Predicción",
                                       "Esperado"))
print("-" * 70)

acciones = ["Avanzar", "Avanzar", "Avanzar", "Girar Der", "Girar Izq",
            "Avanzar", "Retroceder", "Retroceder", "Retroceder"]

for i, (entrada, pred, target, accion) in enumerate(zip(X, predictions_binary, y, acciones)):
    coincide = "✓" if np.array_equal(pred, target) else "✗"
    print(f"{str(entrada):<25} {str(pred.astype(int)):<25} "
          f"{str(target):<10} {coincide} {accion}")

# Calcular precisión
accuracy = np.mean([np.array_equal(p, t) for p, t in zip(predictions_binary, y)]) * 100
print(f"\\n🎯 Precisión: {accuracy:.2f}%")
```

---

### 7. Exportar Matrices de Pesos para Arduino

```python
print("\\n" + "="*60)
print("MATRICES DE PESOS PARA ARDUINO")
print("="*60)

print("\\n// HiddenWeights")
print("float HiddenWeights[3][4] = {")
for i, row in enumerate(nn.HiddenWeights):
    valores = ", ".join([f"{val:.8f}" for val in row])
    coma = "," if i < len(nn.HiddenWeights) - 1 else ""
    print(f"    {{{valores}}}{coma}")
print("};")

print("\\n// OutputWeights")
print("float OutputWeights[4][4] = {")
for i, row in enumerate(nn.OutputWeights):
    valores = ", ".join([f"{val:.8f}" for val in row])
    coma = "," if i < len(nn.OutputWeights) - 1 else ""
    print(f"    {{{valores}}}{coma}")
print("};")

print("\\n📋 Copia y pega estos arrays en tu código Arduino")
```

---

### 8. Guardar Pesos en Archivo de Texto

```python
# Guardar en formato legible
with open('pesos_entrenados_original.txt', 'w') as f:
    f.write("="*60 + "\\n")
    f.write("PESOS ENTRENADOS - RED NEURONAL [2,3,4]\\n")
    f.write("="*60 + "\\n\\n")

    f.write("HiddenWeights (3x4):\\n")
    f.write(str(nn.HiddenWeights) + "\\n\\n")

    f.write("OutputWeights (4x4):\\n")
    f.write(str(nn.OutputWeights) + "\\n\\n")

    f.write("="*60 + "\\n")
    f.write(f"Error final: {errors[-1]:.8f}\\n")
    f.write(f"Precisión: {accuracy:.2f}%\\n")
    f.write("="*60 + "\\n")

print("💾 Pesos guardados en 'pesos_entrenados_original.txt'")
```

---

## Checklist de Completitud

Al finalizar, debes tener:

- [x] Notebook ejecutado completamente sin errores
- [x] Gráfica de error vs épocas generada y guardada
- [ ] Error final < 0.01 (idealmente < 0.001)
- [ ] Precisión en predicciones = 100%
- [ ] Matrices de pesos copiadas y guardadas
- [ ] Captura de pantalla de la gráfica
- [ ] Archivo de texto con los pesos guardado

---

## Qué Documentar

Para el README y la wiki, incluir:

1. **Captura de la gráfica** de entrenamiento (ambos paneles)
2. **Valor del error final** alcanzado
3. **Número de épocas** utilizadas
4. **Precisión** en las predicciones
5. **Observaciones** sobre el comportamiento de convergencia
6. **Problemas encontrados** (si hubo alguno)

---

## Problemas Comunes y Soluciones

### Problema 1: Error no converge (se queda alto)

**Solución**:

- Aumentar épocas a 60000-80000
- Ajustar learning_rate a 0.05

### Problema 2: Error oscila mucho

**Solución**:

- Reducir learning_rate a 0.01-0.02
- Verificar que el dataset esté correctamente normalizado

### Problema 3: Predicciones incorrectas

**Solución**:

- Verificar que la función de redondeo use `abs()` antes del `round()`
- Asegurar que el BIAS se esté agregando correctamente

### Problema 4: Shapes incompatibles

**Solución**:

- Verificar que X tenga shape (9, 2)
- Verificar que y tenga shape (9, 4)
- Revisar que los índices de las matrices sean correctos

---

## Siguiente Paso

Una vez completada la Tarea 3, estarás listo para la **Tarea 4**: expandir la red a [4,6,5] con 2 nuevas entradas y 1 nueva salida.

Ver: `GUIA_TAREA_4_Expansion.md`

---

**Éxito en tu entrenamiento!** 🚀
