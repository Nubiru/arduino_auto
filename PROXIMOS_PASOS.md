# Próximos Pasos - Guía Rápida

## Resumen del Estado Actual

### ✅ Completado
- [x] Estructura de carpetas creada
- [x] README.md principal con documentación completa
- [x] Wiki creada (Home, Sidebar, Footer)
- [x] Análisis de arquitecturas (Tarea 1) en docs/
- [x] Análisis de enfoques (Tarea 2) en docs/
- [x] Guías detalladas para Tareas 3 y 4
- [x] Templates de código Arduino (original y expandido)
- [x] Repositorio Git inicializado y con remote configurado

### ⏳ Pendiente (Tu Trabajo)
- [ ] **Tarea 3**: Entrenar red neuronal original [2,3,4]
- [ ] **Tarea 4**: Expandir red neuronal a [4,6,5]
- [ ] **Tarea 5**: Completar documentación y hacer push a GitHub

---

## Checklist de Tareas por Completar

### Tarea 3: Entrenar Red Neuronal Original

**Tiempo estimado**: 2-3 horas

**Pasos**:
1. [ ] Abrir/crear notebook en Google Colab
2. [ ] Copiar código de entrenamiento desde `notebooks/GUIA_TAREA_3_Entrenamiento.md`
3. [ ] Ejecutar todas las celdas
4. [ ] Verificar que el error converja (< 0.01)
5. [ ] Generar y guardar gráfica de entrenamiento
6. [ ] Exportar matrices de pesos
7. [ ] Copiar pesos a archivo .txt
8. [ ] Guardar notebook con resultados
9. [ ] Descargar captura de la gráfica

**Archivos a generar**:
- `notebooks/[nombre]_tarea3_entrenamiento.ipynb` (o link a Colab)
- `docs/imagenes/graficas_entrenamiento/[nombre]_grafica_tarea3.png`
- `arduino/codigo_original/pesos_[nombre]_original.txt`

**Entregables para README**:
- Error final alcanzado
- Número de épocas
- Precisión de predicciones
- Link al notebook

---

### Tarea 4: Expandir Red Neuronal

**Tiempo estimado**: 4-6 horas

#### Subtarea 4.1: Diseñar Expansión
1. [ ] Decidir qué 2 sensores agregarás (diferentes a tu compañero)
2. [ ] Decidir qué 1 actuador agregarás
3. [ ] Diseñar tabla de verdad expandida con 15-20 escenarios
4. [ ] Documentar tabla en formato markdown o spreadsheet

**Sugerencias de diferenciación**:
- **Gabriel**: Velocidad + IR Borde → Buzzer
- **Mariano**: Luz LDR + Segundo Ultrasónico → LED Estado

#### Subtarea 4.2: Entrenar Modelo Expandido
1. [ ] Modificar código de Tarea 3 para arquitectura [4,6,5]
2. [ ] Implementar nuevo dataset (15-20 escenarios)
3. [ ] Entrenar la red (puede requerir 50000-80000 épocas)
4. [ ] Verificar convergencia y precisión
5. [ ] Exportar nuevas matrices de pesos
6. [ ] Generar y guardar gráfica
7. [ ] Guardar notebook con resultados

**Archivos a generar**:
- `notebooks/[nombre]_tarea4_expansion.ipynb` (o link a Colab actualizado)
- `docs/imagenes/graficas_entrenamiento/[nombre]_grafica_tarea4.png`
- `arduino/codigo_modificado/pesos_[nombre]_expandido.txt`

#### Subtarea 4.3: Implementar en Arduino
1. [ ] Copiar template desde `arduino/codigo_modificado/TEMPLATE_coche_expandido.ino`
2. [ ] Pegar pesos entrenados en arrays
3. [ ] Habilitar pines de tus sensores/actuadores
4. [ ] Implementar funciones de lectura de sensores
5. [ ] Implementar función de control de actuador
6. [ ] Compilar código (verificar que no haya errores)
7. [ ] Guardar como `arduino/codigo_modificado/[nombre]_coche_expandido.ino`

#### Subtarea 4.4: Simular en Wokwi
1. [ ] Crear proyecto nuevo en Wokwi
2. [ ] Agregar todos los componentes necesarios
3. [ ] Conectar según tu diseño
4. [ ] Cargar código Arduino
5. [ ] Ejecutar y probar varios escenarios
6. [ ] Tomar capturas de pantalla
7. [ ] Guardar y compartir link
8. [ ] Agregar link a `wokwi/links_simulaciones.txt`

**Archivos a generar**:
- `wokwi/capturas/[nombre]_circuito.png`
- `wokwi/capturas/[nombre]_funcionando.png`
- Link de simulación en `wokwi/links_simulaciones.txt`

---

### Tarea 5: Completar Documentación y GitHub

**Tiempo estimado**: 2-3 horas

#### Subtarea 5.1: Actualizar README.md
1. [ ] Completar sección 5.1 o 5.2 (según miembro) con tus resultados:
   - Tabla de verdad expandida
   - Descripción de sensores/actuadores elegidos
   - Resultados de entrenamiento (épocas, error)
   - Link al notebook
   - Link a simulación Wokwi
2. [ ] Agregar captura de gráfica en sección correspondiente
3. [ ] Actualizar sección "Resultados y Conclusiones" con tus observaciones
4. [ ] Completar tabla de métricas con tus datos

#### Subtarea 5.2: Organizar Archivos
1. [ ] Verificar que todos los archivos estén en las carpetas correctas
2. [ ] Asegurar que los nombres sean descriptivos
3. [ ] Agregar cualquier archivo faltante

Estructura esperada:
```
arduino_auto/
├── README.md (actualizado con tus resultados)
├── docs/
│   ├── Arquitecturas_Observadas.md
│   ├── Enfoques_Resolucion_Problemas.md
│   └── imagenes/
│       └── graficas_entrenamiento/
│           ├── [nombre]_grafica_tarea3.png
│           └── [nombre]_grafica_tarea4.png
├── notebooks/
│   ├── GUIA_TAREA_3_Entrenamiento.md
│   ├── GUIA_TAREA_4_Expansion.md
│   └── (Links a tus Colabs en README)
├── arduino/
│   ├── codigo_original/
│   │   ├── TEMPLATE_coche_original.ino
│   │   └── pesos_[nombre]_original.txt
│   └── codigo_modificado/
│       ├── TEMPLATE_coche_expandido.ino
│       ├── [nombre]_coche_expandido.ino
│       └── pesos_[nombre]_expandido.txt
├── wokwi/
│   ├── links_simulaciones.txt (actualizado)
│   └── capturas/
│       ├── [nombre]_circuito.png
│       └── [nombre]_funcionando.png
└── wiki/
    ├── Home.md
    ├── _Sidebar.md
    └── _Footer.md
```

#### Subtarea 5.3: Git Commit y Push
⚠️ **IMPORTANTE**: TÚ manejas los comandos git, NO el asistente.

Cuando estés listo:
```bash
# Ver estado
git status

# Agregar todos los archivos
git add .

# Hacer commit
git commit -m "Completar Tareas 3, 4 y 5: Implementación y documentación completa

- Entrenar red neuronal original [2,3,4]
- Expandir a arquitectura [4,6,5] con nuevos sensores/actuadores
- Gabriel: Velocidad + IR Borde -> Buzzer
- Mariano: Luz LDR + Ultrasónico Lateral -> LED Estado
- Implementar código Arduino para ambas versiones
- Crear simulaciones Wokwi
- Documentar resultados completos en README y wiki

🤖 Generated with Claude Code
Co-Authored-By: Claude <noreply@anthropic.com>"

# Push a GitHub
git push -u origin main
```

#### Subtarea 5.4: Verificar GitHub
1. [ ] Ir a https://github.com/Nubiru/arduino_auto
2. [ ] Verificar que todos los archivos estén visibles
3. [ ] Verificar que el README se vea correctamente
4. [ ] Verificar links a Colabs y Wokwi
5. [ ] (Opcional) Habilitar GitHub Wiki y copiar archivos de wiki/

---

## Coordinación del Equipo

### División de Trabajo Sugerida

**Gabriel Osemberg**:
- Nuevas entradas: Velocidad + Sensor IR de Borde
- Nueva salida: Buzzer (alerta sonora)
- Lógica: Buzzer suena cuando detecta borde O obstáculo muy cerca
- Casos de prueba: Enfocarse en detección de bordes y situaciones de peligro

**Mariano Capella**:
- Nuevas entradas: Luz LDR + Segundo Sensor Ultrasónico Lateral
- Nueva salida: LED de Estado
- Lógica: LED enciende en condiciones de precaución (oscuridad, obstáculo lateral)
- Casos de prueba: Enfocarse en navegación con poca luz y detección lateral

### Puntos de Sincronización

1. **Antes de Tarea 4**: Confirmar que ambos usan sensores/actuadores DIFERENTES
2. **Después de entrenar**: Comparar errores y precisiones
3. **Después de simular**: Compartir observaciones y problemas encontrados
4. **Antes del push final**: Revisar que el README incluya ambas implementaciones

---

## Recursos Útiles

### Documentación de Referencia
- [GUIA_TAREA_3_Entrenamiento.md](notebooks/GUIA_TAREA_3_Entrenamiento.md)
- [GUIA_TAREA_4_Expansion.md](notebooks/GUIA_TAREA_4_Expansion.md)
- [README.md](README.md) (sección de instrucciones)

### Links Externos
- Artículo base: https://www.aprendemachinelearning.com/programa-un-coche-arduino-con-inteligencia-artificial/
- Notebook original: https://github.com/jbagnato/machine-learning/blob/master/Red_Neuronal_coche.ipynb
- Wokwi simulator: https://wokwi.com

### Notebooks de Colab del Equipo
- Gabriel: https://colab.research.google.com/drive/1bfZeigmpn53fXfVV7x_setYsHZ7hpEtG
- Mariano: https://colab.research.google.com/drive/1eboG6E3a6TOkrEtj2H-AINEdRsHhfTAo

---

## Criterios de Éxito del Proyecto

Tu proyecto estará completo y exitoso cuando:

### Técnicos
✅ Red original entrenada con error < 0.01 y precisión 100%
✅ Red expandida entrenada con error < 0.01 y precisión ≥ 95%
✅ Código Arduino compila sin errores
✅ Simulación Wokwi funciona correctamente
✅ Nuevo actuador responde apropiadamente

### Documentación
✅ README.md completo con todas las secciones
✅ Tablas de verdad documentadas (original y expandida)
✅ Gráficas de entrenamiento incluidas
✅ Links a notebooks y simulaciones funcionando
✅ Observaciones y conclusiones escritas

### GitHub
✅ Todos los archivos subidos y organizados
✅ Estructura de carpetas correcta
✅ README se visualiza correctamente
✅ Links externos funcionan

---

## Preguntas Frecuentes

### ¿Debo usar exactamente los sensores sugeridos?
No, son sugerencias. Puedes elegir otros sensores disponibles en Wokwi, pero deben ser DIFERENTES a los de tu compañero.

### ¿Qué hago si mi red no converge?
1. Aumentar épocas (60000-80000)
2. Ajustar learning_rate (probar 0.02 o 0.05)
3. Verificar dataset (sin errores en salidas esperadas)
4. Aumentar neuronas en capa oculta (7-8)

### ¿Mi simulación Wokwi debe ser idéntica al hardware real?
No. Algunos sensores deben simularse con potenciómetros. Documenta qué es simulación y qué requeriría hardware real.

### ¿Puedo modificar los templates de código Arduino?
Sí, son plantillas. Personaliza según tus sensores/actuadores, pero mantén la estructura de la red neuronal.

### ¿Ambos miembros deben documentar en el mismo README?
Sí. El README tiene secciones 5.1 y 5.2 para cada miembro. Cada uno completa su sección.

---

## Contacto y Soporte

- **Dudas técnicas de ML/Python**: Revisar guías en notebooks/
- **Dudas de Arduino**: Ver comentarios en templates de arduino/
- **Dudas de organización**: Ver este documento y README.md
- **Coordinación con compañero**: Usar sus notebooks de Colab para comunicación

---

## Motivación Final

Este proyecto demuestra conceptos avanzados:
- Machine Learning en dispositivos embebidos
- Separación entrenamiento/inferencia
- Arquitecturas escalables
- Integración hardware-software

Es una base sólida para tu portafolio y CV. ¡Toma buenos screenshots, documenta bien y preséntalo con orgullo!

**¡Éxito con tu proyecto!** 🚀🤖🎓

---

**Última actualización**: Generado inicialmente
**Próxima actualización**: Cuando completes las tareas pendientes
