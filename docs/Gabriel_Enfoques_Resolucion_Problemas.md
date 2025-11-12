Introducción al Desafío del Proyecto
El presente análisis aborda un proyecto de robótica cuyo objetivo central es lograr que un vehículo a escala, basado en una placa Arduino, conduzca de forma autónoma evitando obstáculos. Este desafío representa una aplicación estratégica y práctica de los principios de Machine Learning en un entorno de hardware con recursos computacionales limitados. La finalidad no es solo construir un coche funcional, sino también demostrar la viabilidad de implementar una inteligencia artificial capaz de tomar decisiones en tiempo real basándose en la información de sus sensores.

La solución propuesta se fundamenta en un enfoque híbrido que aprovecha las fortalezas de diferentes entornos de desarrollo. Se utiliza una Red Neuronal, cuyo entrenamiento intensivo en cómputo se realiza en un entorno más potente como Python. Posteriormente, el "conocimiento" adquirido por la red, encapsulado en una matriz de pesos, se transfiere e implementa en el microcontrolador Arduino, que se encarga exclusivamente de la ejecución del modelo para controlar el vehículo.

Este documento ofrece un análisis detallado de las decisiones clave y las estrategias de resolución de problemas empleadas a lo largo del proyecto, desde la elección del paradigma de programación hasta los ajustes tácticos realizados durante la implementación física.

Decisión Estratégica Clave: Machine Learning Frente a la Programación Basada en Reglas
La elección del paradigma de programación inicial constituye una de las decisiones más críticas en un proyecto de esta naturaleza, ya que define fundamentalmente la arquitectura, la flexibilidad y la capacidad de evolución futura del sistema. En este caso, se optó por un enfoque de Machine Learning en detrimento de un sistema tradicional basado en reglas explícitas.

La evaluación de este enfoque se justifica al contrastarlo con un sistema convencional basado en una lógica de if-then. Como se discute en el material de origen, un algoritmo basado en reglas se volvería progresivamente más complejo y difícil de mantener a medida que se añaden nuevos sensores (por ejemplo, de luz, sonido o velocidad). Cada nuevo sensor requeriría la programación manual de un conjunto creciente de condiciones y excepciones, aumentando la propensión a errores y la rigidez del sistema. En cambio, la Red Neuronal ofrece una solución más robusta y escalable que "aprende" a gestionar esta complejidad de forma integrada, adaptando su comportamiento en función de las nuevas entradas de datos tras un reentrenamiento.

La siguiente comparativa resume las ventajas y limitaciones de cada enfoque en el contexto de este proyecto:

Ventajas del Enfoque de Machine Learning
Escalabilidad: Permite agregar nuevos sensores (más "features" o características) de manera sencilla. Solo es necesario actualizar el conjunto de datos y reentrenar el modelo, sin reescribir la lógica de control fundamental.
Robustez: El modelo puede generalizar el conocimiento y tomar decisiones razonables ante situaciones no vistas explícitamente durante el entrenamiento, ofreciendo una mayor flexibilidad en entornos no controlados.
Mantenibilidad: Evita la creación de un código extenso y complejo basado en sentencias condicionales anidadas (if-then), lo que simplifica su gestión a largo plazo.
Limitaciones de un Sistema Basado en Reglas
Complejidad Creciente: Cada sensor o condición adicional incrementa exponencialmente la complejidad del código, haciéndolo difícil de programar y depurar.
Rigidez: El sistema solo puede responder a las condiciones exactas que han sido programadas explícitamente, careciendo de capacidad para manejar escenarios imprevistos.
Bajo Mantenimiento: Cualquier modificación o ajuste en el comportamiento del vehículo requiere una intervención manual directa en el código, lo que consume tiempo y es propenso a errores.
La adopción del Machine Learning no solo resuelve el problema inmediato de la evasión de obstáculos, sino que sienta las bases para un sistema más inteligente y adaptable, lo que conduce a la arquitectura específica de la solución implementada.

Arquitectura de la Solución: Un Enfoque Híbrido de Dos Fases
Uno de los principales desafíos técnicos del proyecto es la implementación de un modelo de Inteligencia Artificial en un microcontrolador como Arduino, cuyo poder de procesamiento y memoria son limitados. Para superar esta restricción, se diseñó una arquitectura de dos fases que separa el proceso de entrenamiento (computacionalmente intensivo) de la ejecución del modelo (relativamente ligero). Este enfoque híbrido es una solución elegante y eficiente para desplegar IA en dispositivos de borde (edge devices).

Fase 1: Entrenamiento del Modelo en Python

El problema del "aprendizaje" del coche se resolvió externalizando el entrenamiento de la Red Neuronal a un entorno de software más potente y flexible: Python. Esta fase se centra en crear y entrenar un modelo capaz de mapear las entradas sensoriales a las acciones de conducción correctas. Una discrepancia clave entre el modelo conceptual en Python y la implementación física es la inclusión de unidades de sesgo (bias). Mientras que el modelo conceptual en Python se define con NeuralNetwork([2,3,4]), la implementación real en Arduino utiliza una arquitectura más robusta:

3 nodos de entrada: 2 para los datos de los sensores (distancia y posición) más 1 nodo de sesgo.
4 nodos en la capa oculta: 3 neuronas de procesamiento más 1 nodo de sesgo.
4 neuronas en la capa de salida: Cada una corresponde a uno de los cuatro motores del vehículo.
Los datos de entrenamiento utilizados para enseñar a la red se resumen en la siguiente tabla. Es notable que el set de entrenamiento contenga un caso límite contraintuitivo: enseñar al coche a avanzar cuando un obstáculo está detectado (0) y justo en el centro (0). En un escenario real, esto podría provocar colisiones o un comportamiento errático. Esta decisión podría ser una simplificación deliberada para un dataset pequeño, un error en la entrada de datos, o una estrategia para que el coche se acerque a un objeto antes de tomar una acción evasiva más drástica.

Entrada 1: Distancia Normalizada Entrada 2: Posición Angular Normalizada Salida Esperada (Motores) Acción Correspondiente -1 (Sin Obstáculos) 0 (Centro) [1,0,0,1] Avanzar -1 (Sin Obstáculos) 1 (Izquierda) [1,0,0,1] Avanzar -1 (Sin Obstáculos) -1 (Derecha) [1,0,0,1] Avanzar 0 (Obstáculo Detectado) -1 (Derecha) [0,1,0,1] Giro Derecha 0 (Obstáculo Detectado) 1 (Izquierda) [1,0,1,0] Giro Izquierda 0 (Obstáculo Detectado) 0 (Centro) [1,0,0,1] Avanzar 1 (Demasiado Cerca) 1 (Izquierda) [0,1,1,0] Retroceder 1 (Demasiado Cerca) -1 (Derecha) [0,1,1,0] Retroceder 1 (Demasiado Cerca) 0 (Centro) [0,1,1,0] Retroceder

Entrada 1: Distancia Normalizada

Entrada 2: Posición Angular Normalizada

Salida Esperada (Motores)

Acción Correspondiente

-1 (Sin Obstáculos) 0 (Centro) [1,0,0,1] Avanzar -1 (Sin Obstáculos) 1 (Izquierda) [1,0,0,1] Avanzar -1 (Sin Obstáculos) -1 (Derecha) [1,0,0,1] Avanzar 0 (Obstáculo Detectado) -1 (Derecha) [0,1,0,1] Giro Derecha 0 (Obstáculo Detectado) 1 (Izquierda) [1,0,1,0] Giro Izquierda 0 (Obstáculo Detectado) 0 (Centro) [1,0,0,1] Avanzar 1 (Demasiado Cerca) 1 (Izquierda) [0,1,1,0] Retroceder 1 (Demasiado Cerca) -1 (Derecha) [0,1,1,0] Retroceder 1 (Demasiado Cerca) 0 (Centro) [0,1,1,0] Retroceder

Fase 2: Implementación y Ejecución en Arduino

Una vez entrenado el modelo, el problema de la ejecución autónoma se resuelve implementando la red neuronal en el hardware del coche. Es crucial entender que el Arduino no realiza ningún tipo de entrenamiento; su única función es ejecutar el modelo ya entrenado para tomar decisiones en tiempo real.

El proceso de transferencia del conocimiento es notablemente directo: consiste en copiar y pegar la matriz de pesos obtenida en Python directamente en las variables correspondientes (HiddenWeights y OutputWeights) dentro del código fuente de Arduino. La inclusión de las unidades de sesgo (bias), una técnica estándar para aumentar la flexibilidad del modelo al permitir que la función de activación se desplace, se refleja en las dimensiones de estas matrices de pesos.

La tarea del microcontrolador se simplifica a ejecutar únicamente la propagación hacia adelante (forward propagation). Este proceso consiste en tomar las lecturas de los sensores, pasarlas a través de la red y calcular las salidas mediante operaciones matemáticas relativamente simples, como sumas y multiplicaciones de matrices, y la aplicación de la función de activación tanh. Esta estrategia es altamente eficiente y perfectamente adecuada para el hardware de baja potencia de Arduino, permitiendo una toma de decisiones rápida y autónoma. Esta arquitectura de dos fases conecta de manera efectiva el mundo del desarrollo de IA de alto nivel con la implementación práctica en robótica, un puente que ahora exploraremos a nivel de los detalles tácticos.

Resolución de Problemas Tácticos en la Implementación
La arquitectura conceptual descrita se traduce en una serie de soluciones prácticas para resolver los problemas concretos que surgen en la implementación física y de software del coche. Estos desafíos abarcan desde la captura e interpretación de datos del entorno hasta la activación precisa de los componentes mecánicos.

Captura y Normalización de Entradas Sensoriales

El primer problema táctico es cómo el coche puede "ver" y cuantificar su entorno. La solución implementada es un sistema análogo a un radar, que combina un sensor ultrasónico montado sobre un servomotor. El servo realiza un barrido de izquierda a derecha, permitiendo que el sensor mida no solo la distancia a un obstáculo, sino también su posición angular relativa.

Un paso crítico en este proceso es el preprocesamiento de los datos sensoriales. Las lecturas del sensor ultrasónico (distance) y el ángulo del servo (grados_servo) se deben transformar para que sean compatibles con la red neuronal. Concretamente, se normalizan para que sus valores se encuentren siempre en el rango de -1 a 1, un requisito para la función de activación de tangente hiperbólica (tanh) utilizada por la red, cuyo rango de salida es precisamente de -1 a 1. Un ejemplo de esta normalización se encuentra en el código de Arduino para la distancia:

[ ]
entrada1 = ((-2.0/ditanciaMaxima) \* double(distance)) + 1.0;
Esta función lineal mapea la distancia máxima (ditanciaMaxima) a -1 (sin peligro) y una distancia de cero a +1 (peligro inminente), proporcionando una entrada estandarizada y significativa para la red.

Traducción de Salidas Neuronales a Acciones Físicas

El segundo desafío consiste en convertir las salidas abstractas de la red (un conjunto de cuatro valores numéricos) en acciones físicas y coordinadas del vehículo. La solución es un mapeo directo: cada una de las cuatro salidas de la red neuronal controla el estado (encendido o apagado) de uno de los cuatro motores del coche. Esta correspondencia permite ejecutar las acciones de avanzar, retroceder y girar.

Además, el código de Arduino incluye un ajuste práctico para resolver un problema del mundo real: la falta de fuerza durante las maniobras de giro. Se implementó una lógica que duplica la velocidad de los motores (carSpeed = SPEED \* 2;) específicamente cuando se ejecuta una acción de giro. Esta condición se cumple cuando las salidas de la red corresponden a un patrón de giro (p. ej., [1,0,1,0] para giro a la izquierda), activando un motor de cada lado del coche en direcciones opuestas. La lógica (out1+out3)==2 || (out2+out4)==2 detecta eficazmente estos patrones, asegurando que el coche tenga el par motor necesario para superar la inercia y la fricción.

Depuración y Ajustes del Mundo Real Tras la implementación inicial, surgieron problemas prácticos reportados por otros usuarios, como el hecho de que el coche no avanzaba por sí solo y solo reaccionaba al acercarle un obstáculo. Este tipo de comportamiento indica una discrepancia entre el modelo teórico y la realidad física del hardware. La solución propuesta para este problema fue ajustar un parámetro clave en el código: la variable SPEED. Se recomendó aumentar su valor (por ejemplo, de 100 a 200) para proporcionar más potencia a los motores. Este ajuste demuestra un ciclo de depuración iterativo, donde la observación del comportamiento del sistema en el mundo real conduce a la modificación de parámetros para alinear el rendimiento con las expectativas. Estos ejemplos ilustran cómo la implementación exitosa de un sistema de IA en robótica depende tanto de una arquitectura sólida como de la capacidad para resolver problemas tácticos y realizar ajustes empíricos.

Conclusión: Un Enfoque Escalable y Robusto
El análisis de este proyecto de coche autónomo con Arduino revela un enfoque de resolución de problemas bien estructurado y eficaz. La combinación de un modelo de Machine Learning con una arquitectura híbrida de entrenamiento offline y ejecución en el dispositivo no solo ha resuelto con éxito el desafío inmediato de la conducción autónoma para evitar obstáculos, sino que también ha establecido una base sólida y flexible para futuras mejoras.

La decisión estratégica de abandonar un sistema basado en reglas en favor de una Red Neuronal se valida como el pilar de la robustez y escalabilidad del proyecto. Como se señala en la documentación, la adición de nuevos sensores (infrarrojos, de velocidad, de luz, etc.) se simplificaría enormemente: bastaría con añadir nuevas columnas ("features") al conjunto de datos y reentrenar el modelo. Este proceso contrasta drásticamente con la complejidad que implicaría reescribir y depurar un algoritmo basado en reglas para acomodar cada nueva fuente de datos.

Si bien la simplicidad del conjunto de datos de entrenamiento —apenas nueve ejemplos— es instructiva y demuestra la capacidad de las redes neuronales para generalizar, en un entorno de producción plantearía riesgos significativos. Un modelo entrenado en tan pocos escenarios es propenso al sobreajuste (overfitting) y carecería de la robustez necesaria para manejar la gran variedad de situaciones del mundo real.No obstante, como prueba de concepto, ilustra el potencial del enfoque.

En definitiva, este proyecto sirve como un ejemplo elocuente de cómo los principios de la Inteligencia Artificial pueden aplicarse de manera efectiva para resolver problemas complejos en el mundo físico. Demuestra que, con una arquitectura inteligente, es posible superar las limitaciones del hardware y desplegar sistemas autónomos funcionales, incluso con recursos computacionales modestos como los que ofrece una placa Arduino.
